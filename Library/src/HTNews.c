/*			NEWS ACCESS				HTNews.c
**			===========
**
** History:
**	26 Sep 90	Written TBL
**	29 Nov 91	Downgraded to C, for portable implementation.
*/
/* Implements:
*/
#include "HTNews.h"

#define CR   FROMASCII('\015')	/* Must be converted to ^M for transmission */
#define LF   FROMASCII('\012')	/* Must be converted to ^J for transmission */

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

#include "HTML.h"
#include "HTParse.h"
#include "HTFormat.h"
#include "HTAlert.h"

#define BIG 1024 /* @@@ */

struct _HTStructured {
	CONST HTStructuredClass *	isa;
	/* ... */
};

#define NEWS_PROGRESS(foo) HTProgress(foo)


#define NEXT_CHAR HTGetChararcter()
#define LINE_LENGTH 512			/* Maximum length of line of ARTICLE etc */
#define GROUP_NAME_LENGTH	256	/* Maximum length of group name */


/*	Module-wide variables
*/
PUBLIC char * HTNewsHost;
PRIVATE struct sockaddr_in soc_address;		/* Binary network address */
PRIVATE int s;					/* Socket for NewsHost */
PRIVATE char response_text[LINE_LENGTH+1];	/* Last response */
/* PRIVATE HText *	HT;	*/		/* the new hypertext */
PRIVATE HTStructured * target;			/* The output sink */
PRIVATE HTStructuredClass targetClass;		/* Copy of fn addresses */
PRIVATE HTParentAnchor *node_anchor;		/* Its anchor */
PRIVATE int	diagnostic;			/* level: 0=none 2=source */


#define PUTC(c) (*targetClass.put_character)(target, c)
#define PUTS(s) (*targetClass.put_string)(target, s)
#define START(e) (*targetClass.start_element)(target, e, 0, 0)
#define END(e) (*targetClass.end_element)(target, e)

PUBLIC CONST char * HTGetNewsHost NOARGS
{
	return HTNewsHost;
}

PUBLIC void HTSetNewsHost ARGS1(CONST char *, value)
{
	StrAllocCopy(HTNewsHost, value);
}

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
    if ((HTNewsHost = NXGetDefaultValue("WorldWideWeb","NewsHost"))==0)
        if ((HTNewsHost = NXGetDefaultValue("News","NewsHost")) == 0)
	    HTNewsHost = DEFAULT_NEWS_HOST;
#else
    if (getenv("NNTPSERVER")) {
        StrAllocCopy(HTNewsHost, (char *)getenv("NNTPSERVER"));
	if (TRACE) fprintf(stderr, "HTNews: NNTPSERVER defined as `%s'\n",
		HTNewsHost);
    } else {
        char server_name[256];
        FILE* fp = fopen(SERVER_FILE, "r");
        if (fp) {
	    if (fscanf(fp, "%s", server_name)==1) {
	        StrAllocCopy(HTNewsHost, server_name);
		if (TRACE) fprintf(stderr,
		"HTNews: File %s defines news host as `%s'\n",
		        SERVER_FILE, HTNewsHost);
	    }
	    fclose(fp);
	}
    }
    if (!HTNewsHost) HTNewsHost = DEFAULT_NEWS_HOST;
#endif

    if (*HTNewsHost>='0' && *HTNewsHost<='9') {   /* Numeric node address: */
	sin->sin_addr.s_addr = inet_addr((char *)HTNewsHost); /* See arpa/inet.h */

    } else {		    /* Alphanumeric node name: */
	phost=gethostbyname((char*)HTNewsHost);	/* See netdb.h */
	if (!phost) {
	    char message[150];		/* @@@ */
	    sprintf(message, 
	    "HTNews: Can't find news host `%s'.\n%s",HTNewsHost,
	    "Please define your NNTP server");
	    HTAlert(message);
	    CTRACE(tfp,
	      "HTNews: Can't find news host `%s'.\n",HTNewsHost);
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
	if (((*p++=NEXT_CHAR) == LF)
	                || (p == &response_text[LINE_LENGTH])) {
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

/*	Start anchor element
**	--------------------
*/
PRIVATE void start_anchor ARGS1(CONST char *,  href)
{
    BOOL		present[HTML_A_ATTRIBUTES];
    CONST char*		value[HTML_A_ATTRIBUTES];
    
    {
    	int i;
    	for(i=0; i<HTML_A_ATTRIBUTES; i++)
	    present[i] = (i==HTML_A_HREF);
    }
    value[HTML_A_HREF] = href;
    (*targetClass.start_element)(target, HTML_A , present, value);

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
    
    start_anchor(href);
    PUTS(text);
    END(HTML_A);
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
    PUTS("Network Error: connection lost");
    PUTC('\n');
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
        (*targetClass.start_element)(target, HTML_ADDRESS, 0, 0);
	while(!done){
	    char ch = *p++ = NEXT_CHAR;
	    if (ch==(char)EOF) {
		abort_socket();	/* End of file, close socket */
	    	return;		/* End of file on response */
	    }
	    if ((ch == LF) || (p == &line[LINE_LENGTH])) {
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
		    END(HTML_ADDRESS);
		    START(HTML_TITLE);			/** Uuugh! @@@ */
		    PUTS(line+8);
		    END(HTML_TITLE);
		    START(HTML_ADDRESS);
		    (*targetClass.start_element)(target, HTML_H1 , 0, 0);
		    PUTS(line+8);
		    (*targetClass.end_element)(target, HTML_H1);
		    (*targetClass.start_element)(target, HTML_ADDRESS , 0, 0);
		} else if (match(line, "DATE:")
			|| match(line, "FROM:")
			|| match(line, "ORGANIZATION:")) {
		    strcat(line, "\n");
		    PUTS(strchr(line,':')+1);
		} else if (match(line, "NEWSGROUPS:")) {
		    StrAllocCopy(newsgroups, HTStrip(strchr(line,':')+1));
		    
		} else if (match(line, "REFERENCES:")) {
		    StrAllocCopy(references, HTStrip(strchr(line,':')+1));
		    
		} /* end if match */
		p = line;			/* Restart at beginning */
	    } /* if end of line */
	} /* Loop over characters */
	(*targetClass.end_element)(target, HTML_ADDRESS);
    
	if (newsgroups || references) {
	    (*targetClass.start_element)(target, HTML_DLC , 0, 0);
	    if (newsgroups) {
	        (*targetClass.start_element)(target, HTML_DT , 0, 0);
		PUTS("Newsgroups:");
	        (*targetClass.start_element)(target, HTML_DD , 0, 0);
		write_anchors(newsgroups);
		free(newsgroups);
	    }
	    
	    if (references) {
	        (*targetClass.start_element)(target, HTML_DT , 0, 0);
		PUTS("References:");
	        (*targetClass.start_element)(target, HTML_DD , 0, 0);
		write_anchors(references);
		free(references);
	    }
	    (*targetClass.end_element)(target, HTML_DLC);
	}
	PUTS("\n\n\n");
	
    }
    
/*	Read in the BODY of the Article:
*/
    (*targetClass.start_element)(target, HTML_PRE , 0, 0);

    p = line;
    while(!done){
	char ch = *p++ = NEXT_CHAR;
	if (ch==(char)EOF) {
	    abort_socket();	/* End of file, close socket */
	    return;		/* End of file on response */
	}
	if ((ch == LF) || (p == &line[LINE_LENGTH])) {
	    *p++=0;				/* Terminate the string */
	    if (TRACE) fprintf(stderr, "B %s", line);
	    if (line[0]=='.') {
		if (line[1]<' ') {		/* End of article? */
		    done = YES;
		    break;
		} else {			/* Line starts with dot */
		    PUTS(&line[1]);	/* Ignore first dot */
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
			PUTS(l);
			*p = '<'; 		/* again */
			*q = 0;
			start_anchor(p+1);
			*q = '>'; 		/* again */
			PUTS(p);
			(*targetClass.end_element)(target, HTML_A);
			q[1] = c;		/* again */
			l=q+1;
		    } else break;		/* line has unmatched <> */
		} 
		PUTS( l);	/* Last bit of the line */
	    } /* if not dot */
	    p = line;				/* Restart at beginning */
	} /* if end of line */
    } /* Loop over characters */
    
    (*targetClass.end_element)(target, HTML_PRE);
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
    (*targetClass.start_element)(target, HTML_H1 , 0, 0);
    PUTS( "Newsgroups");
    (*targetClass.end_element)(target, HTML_PRE);
    p = line;
    (*targetClass.start_element)(target, HTML_MENU , 0, 0);
    while(!done){
	char ch = *p++ = NEXT_CHAR;
	if (ch==(char)EOF) {
	    abort_socket();	/* End of file, close socket */
	    return;		/* End of file on response */
	}
	if ((ch == LF) || (p == &line[LINE_LENGTH])) {
	    *p++=0;				/* Terminate the string */
	    if (TRACE) fprintf(stderr, "B %s", line);
    	    (*targetClass.start_element)(target, HTML_LI , 0, 0);
	    if (line[0]=='.') {
		if (line[1]<' ') {		/* End of article? */
		    done = YES;
		    break;
		} else {			/* Line starts with dot */
		    PUTS( &line[1]);
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
		    PUTS(line);
	    } /* if not dot */
	    p = line;			/* Restart at beginning */
	} /* if end of line */
    } /* Loop over characters */
    (*targetClass.end_element)(target, HTML_MENU);
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
        PUTS( "\nNo articles in this group.\n");
	return;
    }
    
#define FAST_THRESHOLD 100	/* Above this, read IDs fast */
#define CHOP_THRESHOLD 50	/* Above this, chop off the rest */

    if (first_required<first) first_required = first;		/* clip */
    if ((last_required==0) || (last_required > last)) last_required = last;
    
    if (last_required<=first_required) {
        PUTS( "\nNo articles in this range.\n");
	return;
    }

    if (last_required-first_required+1 > MAX_CHUNK) {	/* Trim this block */
        first_required = last_required-CHUNK_SIZE+1;
    }
    if (TRACE) printf (
    "    Chunk will be (%d-%d)\n", first_required, last_required);

/*	Set window title
*/
    sprintf(buffer, "Newsgroup %s,  Articles %d-%d",
    		groupName, first_required, last_required);
    START(HTML_TITLE);
    PUTS(buffer);
    END(HTML_TITLE);

/*	Link to earlier articles
*/
    if (first_required>first) {
    	int before;			/* Start of one before */
	if (first_required-MAX_CHUNK <= first) before = first;
	else before = first_required-CHUNK_SIZE;
    	sprintf(buffer, "%s/%d-%d", groupName, before, first_required-1);
	if (TRACE) fprintf(stderr, "    Block before is %s\n", buffer);
	PUTS( " (");
	start_anchor(buffer);
	PUTS("Earlier articles");
	END(HTML_A);
	PUTS( "...)\n");
    }
    
    done = NO;

/*#define USE_XHDR*/
#ifdef USE_XHDR
    if (count>FAST_THRESHOLD)  {
        sprintf(buffer,
	"\nThere are about %d articles currently available in %s, IDs as follows:\n\n",
		count, groupName); 
        PUTS(buffer);
        sprintf(buffer, "XHDR Message-ID %d-%d%c%c", first, last, CR, LF);
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
		PUTS("\nAll available articles in ");
        else PUTS( "\nArticles in ");
	PUTS(groupName);
	START(HTML_MENU);
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
			sprintf(buffer, "HEAD %d%c%c", art, CR, LF);	/* Only one */
			status = response(buffer);
		    } else {					/* First of many */
			sprintf(buffer, "HEAD %d%c%cHEAD %d%c%c",
				art, CR, LF, art+1, CR, LF);
			status = response(buffer);
		    }
	    } else if (art==last_required) {			/* Last of many */
		    status = response(NULL);
	    } else {						/* Middle of many */
		    sprintf(buffer, "HEAD %d%c%c", art+1, CR, LF);
		    status = response(buffer);
	    }
	    
#else	/* NOT OVERLAP */
	    sprintf(buffer, "HEAD %d%c%c", art, CR, LF);
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
		    if ((ch == LF)
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
				if (*p==LF) *p = 0;	/* Chop off newline */
			    }
			    break;
				    
			} /* end switch on first character */
		    } /* if end of line */
		} /* Loop over characters */
    
		START(HTML_LI);
		sprintf(buffer, "\"%s\" - %s", subject, author);
		if (reference) {
		    write_anchor(buffer, reference);
		    free(reference);
		    reference=0;
		} else {
		    PUTS(buffer);
		}
		
    
/*	 indicate progress!   @@@@@@
*/
    
	    } /* If good response */
	} /* Loop over article */	    
    } /* If read headers */
    END(HTML_MENU);
    START(HTML_P);
    
/*	Link to later articles
*/
    if (last_required<last) {
    	int after;			/* End of article after */
	after = last_required+CHUNK_SIZE;
    	if (after==last) sprintf(buffer, "news:%s", groupName);	/* original group */
    	else sprintf(buffer, "news:%s/%d-%d", groupName, last_required+1, after);
	if (TRACE) fprintf(stderr, "    Block after is %s\n", buffer);
	PUTS( "(");
	start_anchor(buffer);
	PUTS( "Later articles");
	END(HTML_A);
	PUTS( "...)\n");
    }
    

}


/*		Load by name					HTLoadNews
**		============
*/
PUBLIC int HTLoadNews ARGS4(
	CONST char *,		arg,
	HTParentAnchor *,	anAnchor,
	HTFormat,		format_out,
	HTStream*,		stream)
{
    char command[257];			/* The whole command */
    char groupName[GROUP_NAME_LENGTH];	/* Just the group name */
    int status;				/* tcp return */
    int retries;			/* A count of how hard we have tried */ 
    BOOL group_wanted;			/* Flag: group was asked for, not article */
    BOOL list_wanted;			/* Flag: group was asked for, not article */
    int first, last;			/* First and last articles asked for */

    diagnostic = (format_out == WWW_SOURCE);	/* set global flag */
    
    if (TRACE) fprintf(stderr, "HTNews: Looking for %s\n", arg);
    
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

        {
	    char * p = command + strlen(command);
	    *p++ = CR;		/* Macros to be correct on Mac */
	    *p++ = LF;
	    *p++ = 0;
	    /* strcat(command, "\r\n");	*/	/* CR LF, as in rfc 977 */
	}
    } /* scope of p1 */
    
    if (!*arg) return NO;			/* Ignore if no name */

    
/*	Make a hypertext object with an anchor list.
*/       
    node_anchor = anAnchor;
    target = HTML_new(anAnchor, format_out, stream);
    targetClass = *target->isa;	/* Copy routine entry points */
    
    	
/*	Now, let's get a stream setup up from the NewsHost:
*/       
    for(retries=0;retries<2; retries++){
    
        if (s<0) {
            NEWS_PROGRESS("Connecting to NewsHost ...");
	    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	    status = connect(s, (struct sockaddr*)&soc_address, sizeof(soc_address));
	    if (status<0){
		char message[256];
	        NETCLOSE(s);
		s = -1;
		if (TRACE) fprintf(stderr, "HTNews: Unable to connect to news host.\n");
/*		if (retries<=1) continue;   WHY TRY AGAIN ? 	*/
		sprintf(message,
"\nCould not access %s.\n\n (Check default WorldWideWeb NewsHost ?)\n",
		    HTNewsHost);
		return HTLoadError(stream, 500, message);
	    } else {
		if (TRACE) fprintf(stderr, "HTNews: Connected to news host %s.\n",
				HTNewsHost);
		HTInitInput(s);		/* set up buffering */
		if ((response(NULL) / 100) !=2) {
			char message[BIG];
			NETCLOSE(s);
			s = -1;
			sprintf(message, 
		  "Can't read news info. News host %.20s responded: %.200s",
		  	    HTNewsHost, response_text);
		        return HTLoadError(stream, 500, message);
		}
	    }
	} /* If needed opening */
	
	/* @@@@@@@@@@@@@@Tell user something's happening */
	
	status = response(command);
	if (status<0) break;
	if ((status/ 100) !=2) {
	    HTProgress(response_text);
/*	    NXRunAlertPanel("News access", response_text,
	    	NULL,NULL,NULL);
*/
	    NETCLOSE(s);
	    s = -1;
/* return HT; -- no:the message might be "Timeout-disconnected" left over */
	    continue;	/*	Try again */
	}
  
/*	Load a group, article, etc
*/
        
	
	if (list_wanted) read_list();
	else if (group_wanted) read_group(groupName, first, last);
        else read_article();

	(*targetClass.free)(target);
	return HT_LOADED;
	
    } /* Retry loop */
    
    
    /* HTAlert("Sorry, could not load requested news.\n"); */
        
/*    NXRunAlertPanel(NULL, "Sorry, could not load `%s'.",
	    NULL,NULL,NULL, arg);No -- message earlier wil have covered it */

    return HT_LOADED;
}

PUBLIC HTProtocol HTNews = { "news", HTLoadNews, NULL };
