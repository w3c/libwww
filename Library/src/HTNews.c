/*			NEWS ACCESS				HTNews.c
**			===========
**
** History:
**	26 Sep 90	Written TBL
**	29 Nov 91	Downgraded to C, for portable implementation.
*/

#define NEWS_PORT 119		/* See rfc977 */
#define APPEND			/* Use append methods */
#define MAX_CHUNK	40	/* Largest number of articles in one window */
#define CHUNK_SIZE	20	/* Number of articles for quick display */

#ifndef DEFAULT_NEWS_HOST
#define DEFAULT_NEWS_HOST "news"
#endif
#ifndef SERVER_FILE
#define SERVER_FILE "/usr/local/lib/rn/server"
#endif

#include <ctype.h>
#include "HTUtils.h"		/* Coding convention macros */
#include "tcp.h"

#include "HTNews.h"

#include "HText.h"
#include "HTParse.h"
#include "HTFormat.h"

#ifdef NeXTStep
#include <appkit/defaults.h>
#define NEWS_PROGRESS(foo)
#else
#define NEWS_PROGRESS(foo) fprintf(stderr, "%s\n", (foo))
#endif

extern HTStyleSheet * styleSheet;

#define NEXT_CHAR HTGetChararcter()
#define LINE_LENGTH 512			/* Maximum length of line of ARTICLE etc */
#define GROUP_NAME_LENGTH	256	/* Maximum length of group name */


/*	Module-wide variables
*/
PRIVATE char * NewsHost;
PRIVATE struct sockaddr_in soc_address;		/* Binary network address */
PRIVATE int s;					/* Socket for NewsHost */
PRIVATE char response_text[LINE_LENGTH+1];	/* Last response */
PRIVATE HText *	HT;				/* the new hypertext */
PRIVATE HTParentAnchor *node_anchor;		/* Its anchor */
PRIVATE int	diagnostic;			/* level: 0=none 2=source */

PRIVATE HTStyle *addressStyle;			/* For address etc */
PRIVATE HTStyle *heading1Style;			/* For heading level 1 */
PRIVATE HTStyle *textStyle;			/* Text style */


/*	Initialisation for this module
**	------------------------------
**
**	Except on the NeXT, we pick up the NewsHost name from
**
**	1.	Environment variable NNTPSERVER
**	2.	File SERVER_FILE
**	3.	Compilation time macro DEFAULT_NEWS_HOST
**	4.	Default to "news"
**
**	On the NeXT, we pick up the NewsHost name from, in order:
**
**	1.	WorldWideWeb default "NewsHost"
**	2.	Global default "NewsHost"
**	3.	News default "NewsHost"
**	4.	Compilation time macro DEFAULT_NEWS_HOST
**	5.	Default to "news"
*/
PRIVATE BOOL initialized = NO;
PRIVATE BOOL initialize NOARGS
{
    CONST struct hostent  *phost;	  /* Pointer to host - See netdb.h */
    struct sockaddr_in* sin = &soc_address;

        
/*  Set up defaults:
*/
    sin->sin_family = AF_INET;	    	/* Family = internet, host order  */
    sin->sin_port = htons(NEWS_PORT);   /* Default: new port,    */

/*   Get name of Host
*/
#ifdef NeXTStep
    if ((NewsHost = NXGetDefaultValue("WorldWideWeb","NewsHost"))==0)
        if ((NewsHost = NXGetDefaultValue("News","NewsHost")) == 0)
	    NewsHost = DEFAULT_NEWS_HOST;
#else
    if (getenv("NNTPSERVER")) {
        StrAllocCopy(NewsHost, (char *)getenv("NNTPSERVER"));
	if (TRACE) fprintf(stderr, "HTNews: NNTPSERVER defined as `%s'\n",
		NewsHost);
    } else {
        char server_name[256];
        FILE* fp = fopen(SERVER_FILE, "r");
        if (fp) {
	    if (fscanf(fp, "%s", server_name)==1) {
	        StrAllocCopy(NewsHost, server_name);
		if (TRACE) fprintf(stderr,
		"HTNews: File %s defines news host as `%s'\n",
		        SERVER_FILE, NewsHost);
	    }
	    fclose(fp);
	}
    }
    if (!NewsHost) NewsHost = DEFAULT_NEWS_HOST;
#endif

    if (*NewsHost>='0' && *NewsHost<='9') {   /* Numeric node address: */
	sin->sin_addr.s_addr = inet_addr((char *)NewsHost); /* See arpa/inet.h */

    } else {		    /* Alphanumeric node name: */
	phost=gethostbyname((char*)NewsHost);	/* See netdb.h */
	if (!phost) {
#ifdef NeXTStep
	    NXRunAlertPanel(NULL, "Can't find news host name `%s'.",
	    	NULL, NULL, NULL, NewsHost);
#else
	    fprintf(stderr,
	      "HTNews: Can't find news host `%s'.\n",NewsHost);
	    fprintf(stderr,
"  Please see online documentation for instructions to set the news host.\n");
#endif
	    CTRACE(tfp,
	      "HTNews: Can't find news host `%s'.\n",NewsHost);
	    return NO;  /* Fail */
	}
	memcpy(&sin->sin_addr, phost->h_addr, phost->h_length);
    }

    if (TRACE) fprintf(stderr,  
	"HTNews: Parsed address as port %4x, inet %d.%d.%d.%d\n",
		(unsigned int)ntohs(sin->sin_port),
		(int)*((unsigned char *)(&sin->sin_addr)+0),
		(int)*((unsigned char *)(&sin->sin_addr)+1),
		(int)*((unsigned char *)(&sin->sin_addr)+2),
		(int)*((unsigned char *)(&sin->sin_addr)+3));

    s = -1;		/* Disconnected */
    
    return YES;
}



/*	Get Styles from stylesheet
**	--------------------------
*/
PRIVATE void get_styles NOARGS
{
    if (!heading1Style) heading1Style = HTStyleNamed(styleSheet, "Heading1");
    if (!addressStyle) addressStyle = HTStyleNamed(styleSheet, "Address");
    if (!textStyle) textStyle = HTStyleNamed(styleSheet, "Example");
}


/*	Send NNTP Command line to remote host & Check Response
**	------------------------------------------------------
**
** On entry,
**	command	points to the command to be sent, including CRLF, or is null
**		pointer if no command to be sent.
** On exit,
**	Negative status indicates transmission error, socket closed.
**	Positive status is an NNTP status.
*/


PRIVATE int response ARGS1(CONST char *,command)
{
    int result;    
    char * p = response_text;
    if (command) {
        int status;
	int length = strlen(command);
	if (TRACE) fprintf(stderr, "NNTP command to be sent: %s", command);
#ifdef NOT_ASCII
	{
	    CONST char  * p;
	    char 	* q;
	    char ascii[LINE_LENGTH+1];
	    for(p = command, q=ascii; *p; p++, q++) {
		*q = TOASCII(*p);
	    }
            status = NETWRITE(s, ascii, length);
	}
#else
        status = NETWRITE(s, command, length);
#endif
	if (status<0){
	    if (TRACE) fprintf(stderr,
	        "HTNews: Unable to send command. Disconnecting.\n");
	    NETCLOSE(s);
	    s = -1;
	    return status;
	} /* if bad status */
    } /* if command to be sent */
    
    for(;;) {  
	if (((*p++=NEXT_CHAR) == '\n') || (p == &response_text[LINE_LENGTH])) {
	    *p++=0;				/* Terminate the string */
	    if (TRACE) fprintf(stderr, "NNTP Response: %s\n", response_text);
	    sscanf(response_text, "%d", &result);
	    return result;	    
	} /* if end of line */
	
	if (*(p-1) < 0) {
	    if (TRACE) fprintf(stderr,
	    	"HTNews: EOF on read, closing socket %d\n", s);
	    NETCLOSE(s);	/* End of file, close socket */
	    return s = -1;	/* End of file on response */
	}
    } /* Loop over characters */
}


/*	Case insensitive string comparisons
**	-----------------------------------
**
** On entry,
**	template must be already un upper case.
**	unknown may be in upper or lower or mixed case to match.
*/
PRIVATE BOOL match ARGS2 (CONST char *,unknown, CONST char *,template)
{
    CONST char * u = unknown;
    CONST char * t = template;
    for (;*u && *t && (TOUPPER(*u)==*t); u++, t++) /* Find mismatch or end */ ;
    return (BOOL)(*t==0);		/* OK if end of template */
}

/*	Find Author's name in mail address
**	----------------------------------
**
** On exit,
**	THE EMAIL ADDRESS IS CORRUPTED
**
** For example, returns "Tim Berners-Lee" if given any of
**	" Tim Berners-Lee <tim@online.cern.ch> "
**  or	" tim@online.cern.ch ( Tim Berners-Lee ) "
*/
PRIVATE char * author_name ARGS1 (char *,email)
{
    char *s, *e;
    
    if ((s=strchr(email,'(')) && (e=strchr(email, ')')))
        if (e>s) {
	    *e=0;			/* Chop off everything after the ')'  */
	    return HTStrip(s+1);	/* Remove leading and trailing spaces */
	}
	
    if ((s=strchr(email,'<')) && (e=strchr(email, '>')))
        if (e>s) {
	    strcpy(s, e+1);		/* Remove <...> */
	    return HTStrip(email);	/* Remove leading and trailing spaces */
	}
	
    return HTStrip(email);		/* Default to the whole thing */

}


/*	Paste in an Anchor
**	------------------
**
**
** On entry,
**	HT 	has a selection of zero length at the end.
**	text 	points to the text to be put into the file, 0 terminated.
**	addr	points to the hypertext refernce address,
**		terminated by white space, comma, NULL or '>' 
*/
PRIVATE void write_anchor ARGS2(CONST char *,text, CONST char *,addr)
{
    char href[LINE_LENGTH+1];
		
    {
    	CONST char * p;
	strcpy(href,"news:");
	for(p=addr; *p && (*p!='>') && !WHITE(*p) && (*p!=','); p++);
        strncat(href, addr, p-addr);	/* Make complete hypertext reference */
    }
    
    HText_beginAnchor(HT,
    		HTAnchor_findChildAndLink(node_anchor, "",  href, 0));
    HText_appendText(HT, text);
    HText_endAnchor(HT);
}


/*	Write list of anchors
**	---------------------
**
**	We take a pointer to a list of objects, and write out each,
**	generating an anchor for each.
**
** On entry,
**	HT 	has a selection of zero length at the end.
**	text 	points to a comma or space separated list of addresses.
** On exit,
**	*text	is NOT any more chopped up into substrings.
*/
PRIVATE void write_anchors ARGS1 (char *,text)
{
    char * start = text;
    char * end;
    char c;
    for (;;) {
        for(;*start && (WHITE(*start)); start++);  /* Find start */
	if (!*start) return;			/* (Done) */
        for(end=start; *end && (*end!=' ') && (*end!=','); end++);/* Find end */
	if (*end) end++;	/* Include comma or space but not NULL */
	c = *end;
	*end = 0;
	write_anchor(start, start);
	*end = c;
	start = end;			/* Point to next one */
    }
}

/*	Abort the connection					abort_socket
**	--------------------
*/
PRIVATE void abort_socket NOARGS
{
    if (TRACE) fprintf(stderr,
	    "HTNews: EOF on read, closing socket %d\n", s);
    NETCLOSE(s);	/* End of file, close socket */
    HText_appendText(HT, "Network Error: connection lost");
    HText_appendParagraph(HT);
    s = -1;		/* End of file on response */
    return;
}

/*	Read in an Article					read_article
**	------------------
**
**
**	Note the termination condition of a single dot on a line by itself.
**	RFC 977 specifies that the line "folding" of RFC850 is not used, so we
**	do not handle it here.
**
** On entry,
**	s	Global socket number is OK
**	HT	Global hypertext object is ready for appending text
*/       
PRIVATE void read_article NOARGS
{

    char line[LINE_LENGTH+1];
    char *references=NULL;			/* Hrefs for other articles */
    char *newsgroups=NULL;			/* Newsgroups list */
    char *p = line;
    BOOL done = NO;
    
/*	Read in the HEADer of the article:
**
**	The header fields are either ignored, or formatted and put into the
**	 Text.
*/
    if (!diagnostic) {
        HText_setStyle(HT, addressStyle);
	while(!done){
	    char ch = *p++ = NEXT_CHAR;
	    if (ch==(char)EOF) {
		abort_socket();	/* End of file, close socket */
	    	return;		/* End of file on response */
	    }
	    if ((ch == '\n') || (p == &line[LINE_LENGTH])) {
		*--p=0;				/* Terminate the string */
		if (TRACE) fprintf(stderr, "H %s\n", line);

		if (line[0]=='.') {	
		    if (line[1]<' ') {		/* End of article? */
			done = YES;
			break;
		    }
		
		} else if (line[0]<' ') {
		    break;		/* End of Header? */
		} else if (match(line, "SUBJECT:")) {
		    HTAnchor_setTitle(node_anchor, line+8);
		    HText_setStyle(HT, heading1Style);
		    HText_appendText(HT, line+8);
		    HText_setStyle(HT, addressStyle);
		} else if (match(line, "DATE:")
			|| match(line, "FROM:")
			|| match(line, "ORGANIZATION:")) {
		    strcat(line, "\n");
		    HText_appendText(HT, strchr(line,':')+1);
		} else if (match(line, "NEWSGROUPS:")) {
		    StrAllocCopy(newsgroups, HTStrip(strchr(line,':')+1));
		    
		} else if (match(line, "REFERENCES:")) {
		    StrAllocCopy(references, HTStrip(strchr(line,':')+1));
		    
		} /* end if match */
		p = line;			/* Restart at beginning */
	    } /* if end of line */
	} /* Loop over characters */
    
	HText_appendCharacter(HT, '\n');
	HText_setStyle(HT, textStyle);
	if (newsgroups) {
	    HText_appendText(HT, "\nNewsgroups: ");
	    write_anchors(newsgroups);
	    free(newsgroups);
	}
	
	if (references) {
	    HText_appendText(HT, "\nReferences: ");
	    write_anchors(references);
	    free(references);
	}
    
	HText_appendText(HT, "\n\n\n");
	
    } else { /* diagnostic */
        HText_setStyle(HT, textStyle);
    }
    
/*	Read in the BODY of the Article:
*/
    p = line;
    while(!done){
	char ch = *p++ = NEXT_CHAR;
	if (ch==(char)EOF) {
	    abort_socket();	/* End of file, close socket */
	    return;		/* End of file on response */
	}
	if ((ch == '\n') || (p == &line[LINE_LENGTH])) {
	    *p++=0;				/* Terminate the string */
	    if (TRACE) fprintf(stderr, "B %s", line);
	    if (line[0]=='.') {
		if (line[1]<' ') {		/* End of article? */
		    done = YES;
		    break;
		} else {			/* Line starts with dot */
		    HText_appendText(HT, &line[1]);	/* Ignore first dot */
		}
	    } else {

/*	Normal lines are scanned for buried references to other articles.
**	Unfortunately, it will pick up mail addresses as well!
*/
		char *l = line;
		char * p;
		while (p=strchr(l, '<')) {
		    char *q  = strchr(p,'>');
		    char *at = strchr(p, '@');
		    if (q && at && at<q) {
		        char c = q[1];
			q[1] = 0;		/* chop up */
			*p = 0;
			HText_appendText(HT, l);
			*p = '<'; 		/* again */
			*q = 0;
			HText_beginAnchor(HT,
			    HTAnchor_findChildAndLink(
			    	node_anchor, "", p+1, 0));
			*q = '>'; 		/* again */
			HText_appendText(HT, p);
			HText_endAnchor(HT);
			q[1] = c;		/* again */
			l=q+1;
		    } else break;		/* line has unmatched <> */
		} 
		HText_appendText(HT,  l);	/* Last bit of the line */
	    } /* if not dot */
	    p = line;				/* Restart at beginning */
	} /* if end of line */
    } /* Loop over characters */
}


/*	Read in a List of Newsgroups
**	----------------------------
*/
/*
**	Note the termination condition of a single dot on a line by itself.
**	RFC 977 specifies that the line "folding" of RFC850 is not used, so we
**	do not handle it here.
*/        
PRIVATE void read_list NOARGS
{

    char line[LINE_LENGTH+1];
    char *p;
    BOOL done = NO;
    
/*	Read in the HEADer of the article:
**
**	The header fields are either ignored, or formatted and put into the
**	Text.
*/
    HText_setStyle(HT, heading1Style);
    HText_appendText(HT,  "Newsgroups");
    HText_setStyle(HT, textStyle);
    p = line;
    while(!done){
	char ch = *p++ = NEXT_CHAR;
	if (ch==(char)EOF) {
	    abort_socket();	/* End of file, close socket */
	    return;		/* End of file on response */
	}
	if ((ch == '\n') || (p == &line[LINE_LENGTH])) {
	    *p++=0;				/* Terminate the string */
	    if (TRACE) fprintf(stderr, "B %s", line);
	    if (line[0]=='.') {
		if (line[1]<' ') {		/* End of article? */
		    done = YES;
		    break;
		} else {			/* Line starts with dot */
		    HText_appendText(HT,  &line[1]);
		}
	    } else {

/*	Normal lines are scanned for references to newsgroups.
*/
		char group[LINE_LENGTH];
		int first, last;
		char postable;
		if (sscanf(line, "%s %d %d %c", group, &first, &last, &postable)==4)
		    write_anchor(line, group);
		else
		    HText_appendText(HT, line);
	    } /* if not dot */
	    p = line;			/* Restart at beginning */
	} /* if end of line */
    } /* Loop over characters */
}


/*	Read in a Newsgroup
**	-------------------
**	Unfortunately, we have to ask for each article one by one if we
**	want more than one field.
**
*/
PRIVATE void read_group ARGS3(
  CONST char *,groupName,
  int,first_required,
  int,last_required
)
{
    char line[LINE_LENGTH+1];
    char author[LINE_LENGTH+1];
    char subject[LINE_LENGTH+1];
    char *p;
    BOOL done;

    char buffer[LINE_LENGTH];
    char *reference=0;			/* Href for article */
    int art;				/* Article number WITHIN GROUP */
    int status, count, first, last;	/* Response fields */
					/* count is only an upper limit */

    sscanf(response_text, " %d %d %d %d", &status, &count, &first, &last);
    if(TRACE) printf("Newsgroup status=%d, count=%d, (%d-%d) required:(%d-%d)\n",
    			status, count, first, last, first_required, last_required);
    if (last==0) {
        HText_appendText(HT,  "\nNo articles in this group.\n");
	return;
    }
    
#define FAST_THRESHOLD 100	/* Above this, read IDs fast */
#define CHOP_THRESHOLD 50	/* Above this, chop off the rest */

    if (first_required<first) first_required = first;		/* clip */
    if ((last_required==0) || (last_required > last)) last_required = last;
    
    if (last_required<=first_required) {
        HText_appendText(HT,  "\nNo articles in this range.\n");
	return;
    }

    if (last_required-first_required+1 > MAX_CHUNK) {	/* Trim this block */
        first_required = last_required-CHUNK_SIZE+1;
    }
    if (TRACE) printf (
    "    Chunk will be (%d-%d)\n", first_required, last_required);

/*	Link to earlier articles
*/
    if (first_required>first) {
    	int before;			/* Start of one before */
	if (first_required-MAX_CHUNK <= first) before = first;
	else before = first_required-CHUNK_SIZE;
    	sprintf(buffer, "%s/%d-%d", groupName, before, first_required-1);
	if (TRACE) fprintf(stderr, "    Block before is %s\n", buffer);
	HText_appendText(HT,  " (");
	HText_beginAnchor(HT,
	    HTAnchor_findChildAndLink(node_anchor, "", buffer, 0));
	HText_appendText(HT,  "Earlier articles");
	HText_endAnchor(HT);
	HText_appendText(HT,  "...)\n");
    }
    
    done = NO;

/*#define USE_XHDR*/
#ifdef USE_XHDR
    if (count>FAST_THRESHOLD)  {
        sprintf(buffer,
	"\nThere are about %d articles currently available in %s, IDs as follows:\n\n",
		count, groupName); 
        HText_appendText(HT, buffer);
        sprintf(buffer, "XHDR Message-ID %d-%d\n", first, last);
	status = response(buffer);
	if (status==221) {

	    p = line;
	    while(!done){
		char ch = *p++ = NEXT_CHAR;
		if (ch==(char)EOF) {
		    abort_socket();	/* End of file, close socket */
		    return;		/* End of file on response */
		}
		if ((ch == '\n') || (p == &line[LINE_LENGTH])) {
		    *p++=0;				/* Terminate the string */
		    if (TRACE) fprintf(stderr, "X %s", line);
		    if (line[0]=='.') {
			if (line[1]<' ') {		/* End of article? */
			    done = YES;
			    break;
			} else {			/* Line starts with dot */
			    	/* Ignore strange line */
			}
		    } else {
	
	/*	Normal lines are scanned for references to articles.
	*/
			char * space = strchr(line, ' ');
			if (space++)
			    write_anchor(space, space);
		    } /* if not dot */
		    p = line;			/* Restart at beginning */
		} /* if end of line */
	    } /* Loop over characters */

	    /* leaving loop with "done" set */
	} /* Good status */
    };
#endif

/*	Read newsgroup using individual fields:
*/
    if (!done) {
        if (first==first_required && last==last_required)
		HText_appendText(HT, "\nAll available articles in ");
        else HText_appendText(HT,  "\nArticles in ");
	HText_appendText(HT, groupName);
	HText_appendText(HT, "\n\n");
	for(art=first_required; art<=last_required; art++) {
    
/*#define OVERLAP*/
#ifdef OVERLAP
/* With this code we try to keep the server running flat out by queuing just
** one extra command ahead of time. We assume (1) that the server won't abort
** if it gets input during output, and (2) that TCP buffering is enough for the
** two commands. Both these assumptions seem very reasonable. However, we HAVE
** had a hangup with a loaded server.
*/
	    if (art==first_required) {
		if (art==last_required) {
			sprintf(buffer, "HEAD %d\n", art);	/* Only one */
			status = response(buffer);
		    } else {					/* First of many */
			sprintf(buffer, "HEAD %d\nHEAD %d\n", art, art+1);
			status = response(buffer);
		    }
	    } else if (art==last_required) {			/* Last of many */
		    status = response(NULL);
	    } else {						/* Middle of many */
		    sprintf(buffer, "HEAD %d\n", art+1);
		    status = response(buffer);
	    }
	    
#else	/* NOT OVERLAP */
	    sprintf(buffer, "HEAD %d\n", art);
	    status = response(buffer);
#endif	/* NOT OVERLAP */

	    if (status == 221) {	/* Head follows - parse it:*/
    
		p = line;				/* Write pointer */
		done = NO;
		while(!done){
		    char ch = *p++ = NEXT_CHAR;
		    if (ch==(char)EOF) {
			abort_socket();	/* End of file, close socket */
			return;		/* End of file on response */
		    }
		    if ((ch == '\n')
			|| (p == &line[LINE_LENGTH]) ) {
		    
			*--p=0;		/* Terminate  & chop LF*/
			p = line;		/* Restart at beginning */
			if (TRACE) fprintf(stderr, "G %s\n", line);
			switch(line[0]) {
    
			case '.':
			    done = (line[1]<' ');	/* End of article? */
			    break;
    
			case 'S':
			case 's':
			    if (match(line, "SUBJECT:"))
				strcpy(subject, line+9);/* Save subject */
			    break;
    
			case 'M':
			case 'm':
			    if (match(line, "MESSAGE-ID:")) {
				char * addr = HTStrip(line+11) +1; /* Chop < */
				addr[strlen(addr)-1]=0;		/* Chop > */
				StrAllocCopy(reference, addr);
			    }
			    break;
    
			case 'f':
			case 'F':
			    if (match(line, "FROM:")) {
				char * p;
				strcpy(author,
					author_name(strchr(line,':')+1));
				p = author + strlen(author) - 1;
				if (*p=='\n') *p = 0;	/* Chop off newline */
			    }
			    break;
				    
			} /* end switch on first character */
		    } /* if end of line */
		} /* Loop over characters */
    
		sprintf(buffer, "\"%s\" - %s", subject, author);
		if (reference) {
		    write_anchor(buffer, reference);
		    free(reference);
		    reference=0;
		} else {
		    HText_appendText(HT, buffer);
		}
	        HText_appendParagraph(HT);
		
    
/*	Change the title bar to indicate progress!
*/
		if (art%10 == 0) {
		    sprintf(buffer, "Reading newsgroup %s,  Article %d (of %d-%d) ...",
			    groupName, art, first, last);
		    HTAnchor_setTitle(node_anchor, buffer);
		}
    
	    } /* If good response */
	} /* Loop over article */	    
    } /* If read headers */
    
/*	Link to later articles
*/
    if (last_required<last) {
    	int after;			/* End of article after */
	after = last_required+CHUNK_SIZE;
    	if (after==last) sprintf(buffer, "news:%s", groupName);	/* original group */
    	else sprintf(buffer, "news:%s/%d-%d", groupName, last_required+1, after);
	if (TRACE) fprintf(stderr, "    Block after is %s\n", buffer);
	HText_appendText(HT,  "(");
	HText_beginAnchor(HT, HTAnchor_findChildAndLink(
		node_anchor, "", buffer, 0));
	HText_appendText(HT,  "Later articles");
	HText_endAnchor(HT);
	HText_appendText(HT,  "...)\n");
    }
    
/*	Set window title
*/
    sprintf(buffer, "Newsgroup %s,  Articles %d-%d",
    		groupName, first_required, last_required);
    HTAnchor_setTitle(node_anchor, buffer);

}


/*		Load by name					HTLoadNews
**		============
*/
PUBLIC int HTLoadNews ARGS3(
	CONST char *,arg,
	HTParentAnchor *,anAnchor,
	int,diag)
{
    char command[257];			/* The whole command */
    char groupName[GROUP_NAME_LENGTH];	/* Just the group name */
    int status;				/* tcp return */
    int retries;			/* A count of how hard we have tried */ 
    BOOL group_wanted;			/* Flag: group was asked for, not article */
    BOOL list_wanted;			/* Flag: group was asked for, not article */
    int first, last;			/* First and last articles asked for */

    diagnostic = diag;			/* set global flag */
    
    if (TRACE) fprintf(stderr, "HTNews: Looking for %s\n", arg);
    get_styles();
    
    if (!initialized) initialized = initialize();
    if (!initialized) return -1;	/* FAIL */
    
    {
        CONST char * p1=arg;

/*	We will ask for the document, omitting the host name & anchor.
**
**	Syntax of address is
**		xxx@yyy			Article
**		<xxx@yyy>		Same article
**		xxxxx			News group (no "@")
**		group/n1-n2		Articles n1 to n2 in group
*/        
	group_wanted = (strchr(arg, '@')==0) && (strchr(arg, '*')==0);
	list_wanted  = (strchr(arg, '@')==0) && (strchr(arg, '*')!=0);

	/* p1 = HTParse(arg, "", PARSE_PATH | PARSE_PUNCTUATION); */
	/* Don't use HTParse because news: access doesn't follow traditional
	   rules. For instance, if the article reference contains a '#',
	   the rest of it is lost -- JFG 10/7/92, from a bug report */
 	if (!strncasecomp (arg, "news:", 5))
	  p1 = arg + 5;  /* Skip "news:" prefix */
	if (list_wanted) {
	    strcpy(command, "LIST ");
	} else if (group_wanted) {
	    char * slash = strchr(p1, '/');
	    strcpy(command, "GROUP ");
	    first = 0;
	    last = 0;
	    if (slash) {
		*slash = 0;
		strcpy(groupName, p1);
		*slash = '/';
		(void) sscanf(slash+1, "%d-%d", &first, &last);
	    } else {
		strcpy(groupName, p1);
	    }
	    strcat(command, groupName);
	} else {
	    strcpy(command, "ARTICLE ");
	    if (strchr(p1, '<')==0) strcat(command,"<");
	    strcat(command, p1);
	    if (strchr(p1, '>')==0) strcat(command,">");
	}
/*   	free(p1); * bug fix TBL 5 Aug 92 */

        strcat(command, "\r\n");		/* CR LF, as in rfc 977 */
	
    } /* scope of p1 */
    
    if (!*arg) return NO;			/* Ignore if no name */

    
/*	Make a hypertext object with an anchor list.
*/       
    node_anchor = anAnchor;
    HT = HText_new(anAnchor);
    HText_beginAppend(HT);
    	
/*	Now, let's get a stream setup up from the NewsHost:
*/       
    for(retries=0;retries<2; retries++){
    
        if (s<0) {
    	    HTAnchor_setTitle(node_anchor, "Connecting to NewsHost ...");/* Tell user  */
            NEWS_PROGRESS("Connecting to NewsHost ...");
	    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	    status = connect(s, (struct sockaddr*)&soc_address, sizeof(soc_address));
	    if (status<0){
		char message[256];
	        NETCLOSE(s);
		s = -1;
		if (TRACE) fprintf(stderr, "HTNews: Unable to connect to news host.\n");
/*		if (retries<=1) continue;   WHY TRY AGAIN ? 	*/
#ifdef NeXTStep
		NXRunAlertPanel(NULL,
    		    "Could not access newshost %s.",
		    NULL,NULL,NULL,
		    NewsHost);
#else
		fprintf(stderr, "Could not access newshost %s\n",
		    NewsHost);
#endif
		sprintf(message,
"\nCould not access %s.\n\n (Check default WorldWideWeb NewsHost ?)\n",
		    NewsHost);
		HText_beginAppend(HT);
		HText_appendText(HT, message);
		HText_endAppend(HT);
		return YES;
	    } else {
		if (TRACE) fprintf(stderr, "HTNews: Connected to news host %s.\n",
				NewsHost);
		HTInitInput(s);		/* set up buffering */
		if ((response(NULL) / 100) !=2) {
			NETCLOSE(s);
			s = -1;
#ifdef NeXTStep
			NXRunAlertPanel("News access",
			    "Could not retrieve information:\n   %s.",
			    NULL,NULL,NULL,
			    response_text);
#endif
    			HTAnchor_setTitle(node_anchor, "News host response");
			HText_beginAppend(HT);
			HText_appendText(HT,
			     "Sorry, could not retrieve information: ");
			HText_appendText(HT, response_text);
			HText_endAppend(HT);
			return YES;
		}
	    }
	} /* If needed opening */
	
	HTAnchor_setTitle(node_anchor, arg);/* Tell user something's happening */
	status = response(command);
	if (status<0) break;
	if ((status/ 100) !=2) {
/*	    NXRunAlertPanel("News access", response_text,
	    	NULL,NULL,NULL);
*/
	    HText_beginAppend(HT);
	    HText_appendText(HT, response_text);
	    HText_endAppend(HT);
	    NETCLOSE(s);
	    s = -1;
/* return HT; -- no:the message might be "Timeout-disconnected" left over */
	    continue;	/*	Try again */
	}
  
/*	Load a group, article, etc
*/
        HText_beginAppend(HT);
	
	if (list_wanted) read_list();
	else if (group_wanted) read_group(groupName, first, last);
        else read_article();

	HText_endAppend(HT);
	return YES;
	
    } /* Retry loop */
    
    HText_beginAppend(HT);
    HText_appendText(HT, "Sorry, could not load requested news.\n");
    HText_endAppend(HT);
    
/*    NXRunAlertPanel(NULL, "Sorry, could not load `%s'.",
	    NULL,NULL,NULL, arg);No -- message earlier wil have covered it */

    return YES;
}

