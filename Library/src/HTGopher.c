/*			GOPHER ACCESS				HTGopher.c
**			=============
**
** History:
**	26 Sep 90	Adapted from other accesses (News, HTTP) TBL
**	29 Nov 91	Downgraded to C, for portable implementation.
*/

/* Implements:
*/
#include "HTGopher.h"


#define GOPHER_PORT 70		/* See protocol spec */
#define BIG 1024		/* Bug */
#define LINE_LENGTH 256		/* Bug */

/*	Gopher entity types:
*/
#define GOPHER_TEXT		'0'
#define GOPHER_MENU		'1'
#define GOPHER_CSO		'2'
#define GOPHER_ERROR		'3'
#define GOPHER_MACBINHEX	'4'
#define GOPHER_PCBINHEX		'5'
#define GOPHER_UUENCODED	'6'
#define GOPHER_INDEX		'7'
#define GOPHER_TELNET		'8'
#define GOPHER_BINARY           '9'
#define GOPHER_GIF              'g'
#define GOPHER_HTML		'h'	        /* HTML */
#define GOPHER_SOUND            's'
#define GOPHER_WWW		'w'		/* W3 address */
#define GOPHER_IMAGE            'I'
#define GOPHER_TN3270           'T'
#define GOPHER_DUPLICATE	'+'

#include <ctype.h>
#include "HTUtils.h"		/* Coding convention macros */
#include "tcp.h"


#include "HTParse.h"
#include "HTFormat.h"
#include "HTTCP.h"

/*		Hypertext object building machinery
*/
#include "HTML.h"

#define PUTC(c) (*targetClass.put_character)(target, c)
#define PUTS(s) (*targetClass.put_string)(target, s)
#define START(e) (*targetClass.start_element)(target, e, 0, 0)
#define END(e) (*targetClass.end_element)(target, e)
#define FREE_TARGET (*targetClass.free)(target)
struct _HTStructured {
	CONST HTStructuredClass *	isa;
	/* ... */
};

PRIVATE HTStructured *target;			/* the new hypertext */
PRIVATE HTStructuredClass targetClass;		/* Its action routines */


#define GOPHER_PROGRESS(foo) HTAlert(foo)


#define NEXT_CHAR HTGetChararcter() 



/*	Module-wide variables
*/
PRIVATE int s;					/* Socket for GopherHost */



/*	Matrix of allowed characters in filenames
**	-----------------------------------------
*/

PRIVATE BOOL acceptable[256];
PRIVATE BOOL acceptable_inited = NO;

PRIVATE void init_acceptable NOARGS
{
    unsigned int i;
    char * good = 
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./-_$";
    for(i=0; i<256; i++) acceptable[i] = NO;
    for(;*good; good++) acceptable[(unsigned int)*good] = YES;
    acceptable_inited = YES;
}

PRIVATE CONST char hex[17] = "0123456789abcdef";

/*	Decdoe one hex character
*/

PRIVATE char from_hex ARGS1(char, c)
{
    return 		  (c>='0')&&(c<='9') ? c-'0'
			: (c>='A')&&(c<='F') ? c-'A'+10
			: (c>='a')&&(c<='f') ? c-'a'+10
			:		       0;
}



/*	Paste in an Anchor
**	------------------
**
**	The title of the destination is set, as there is no way
**	of knowing what the title is when we arrive.
**
** On entry,
**	HT 	is in append mode.
**	text 	points to the text to be put into the file, 0 terminated.
**	addr	points to the hypertext refernce address 0 terminated.
*/
PRIVATE void write_anchor ARGS2(CONST char *,text, CONST char *,addr)
{


    
    BOOL present[HTML_A_ATTRIBUTES];
    CONST char * value[HTML_A_ATTRIBUTES];
    
    int i;
    
    for (i=0; i<HTML_A_ATTRIBUTES; i++) present[i]=0;
    present[HTML_A_HREF] = YES;
    value[HTML_A_HREF] = addr;
    present[HTML_A_TITLE] = YES;
    value[HTML_A_TITLE] = text;
    
    (*targetClass.start_element)(target, HTML_A, present, value);
	    
    PUTS(text);
    END(HTML_A);
}


/*	Parse a Gopher Menu document
**	============================
**
*/

PRIVATE void parse_menu ARGS2 (
	CONST char *,		arg,
	HTParentAnchor *,	anAnchor)
{
    char gtype;
    char ch;
    char line[BIG];
    char address[BIG];
    char *name, *selector;		/* Gopher menu fields */
    char *host;
    char *port;
    char *p = line;
    CONST char *title;

#define TAB 		'\t'
#define HEX_ESCAPE 	'%'

    
    title = HTAnchor_title(anAnchor);
    if (title) {
        START(HTML_H1);
	PUTS(title);
	END(HTML_H1);
    } else
        PUTS("Select one of:\n\n");
    
    START(HTML_MENU);
    while ((ch=NEXT_CHAR) != (char)EOF) {
        if (ch != LF) {
	    *p = ch;		/* Put character in line */
	    if (p< &line[BIG-1]) p++;
	    
	} else {
	    *p++ = 0;		/* Terminate line */
	    p = line;		/* Scan it to parse it */
	    port = 0;		/* Flag "not parsed" */
	    if (TRACE) fprintf(stderr, "HTGopher: Menu item: %s\n", line);
	    gtype = *p++;
	    
	    /* Break on line with a dot by itself */
	    if ((gtype=='.') && ((*p=='\r') || (*p==0))) break;

	    if (gtype && *p) {
	        name = p;
		selector = strchr(name, TAB);
		START(HTML_LI);
		if (selector) {
		    *selector++ = 0;	/* Terminate name */
		    host = strchr(selector, TAB);
		    if (host) {
			*host++ = 0;	/* Terminate selector */
			port = strchr(host, TAB);
			if (port) {
			    char *junk;
			    port[0] = ':';	/* delimit host a la W3 */
			    junk = strchr(port, TAB);
			    if (junk) *junk++ = 0;	/* Chop port */
			    if ((port[1]=='0') && (!port[2]))
			        port[0] = 0;	/* 0 means none */
			} /* no port */
		    } /* host ok */
		} /* selector ok */
	    } /* gtype and name ok */
	    
	    if (gtype == GOPHER_WWW) {	/* Gopher pointer to W3 */
		write_anchor(name, selector);
		
	    } else if (port) {		/* Other types need port */
		if (gtype == GOPHER_TELNET) {
		    if (*selector) sprintf(address, "telnet://%s@%s/",
					   selector, host);
		    else sprintf(address, "telnet://%s/", host);
		}
		else if (gtype == GOPHER_TN3270) 
		{
		    if (*selector) 
			sprintf(address, "tn3270://%s@%s/",
				selector, host);
		    else 
			sprintf(address, "tn3270://%s/", host);
		}
		else {			/* If parsed ok */
		    char *q;
		    char *p;
		    sprintf(address, "//%s/%c", host, gtype);
		    q = address+ strlen(address);
		    for(p=selector; *p; p++) {	/* Encode selector string */
			if (acceptable[*p]) *q++ = *p;
			else {
			    *q++ = HEX_ESCAPE;	/* Means hex coming */
			    *q++ = hex[(TOASCII(*p)) >> 4];
			    *q++ = hex[(TOASCII(*p)) & 15];
			}
		    }
		    *q++ = 0;			/* terminate address */
		}
		PUTS("        "); /* Prettier JW/TBL */
		/* Error response from Gopher doesn't deserve to
		   be a hyperlink. */
		if (strcmp (address, "gopher://error.host:1/0"))
		    write_anchor(name, address);
		else
		    PUTS(name);
		PUTS("\n");
	    } else { /* parse error */
	        if (TRACE) fprintf(stderr,
			"HTGopher: Bad menu item.\n");
		PUTS(line);

	    } /* parse error */
	    
	    p = line;	/* Start again at beginning of line */
	    
	} /* if end of line */
	
    } /* Loop over characters */
	
    END(HTML_MENU);
    FREE_TARGET;
    
    return;
}
/*	Parse a Gopher CSO document
 **	============================
 **
 **   Accepts an open socket to a CSO server waiting to send us
 **   data and puts it on the screen in a reasonable manner.
 **
 **   Perhaps this data can be automatically linked to some
 **   other source as well???
 **
 **  Taken from hacking by Lou Montulli@ukanaix.cc.ukans.edu
 **  on XMosaic-1.1, and put on libwww 2.11 by Arthur Secret, 
 **  secret@dxcern.cern.ch .
 */

PRIVATE void parse_cso ARGS2 (
			      CONST char *,	arg,
			      HTParentAnchor *,anAnchor)
{
    char ch;
    char line[BIG];
    char *p = line;
    char *second_colon, last_char='\0';
    CONST char *title;
    
    title = HTAnchor_title(anAnchor);
    START(HTML_H1);
    PUTS("CSO Search Results");
    END(HTML_H1);
    START(HTML_PRE);

    /* start grabbing chars from the network */
    while ((ch=NEXT_CHAR) != (char)EOF) 
	{
	    if (ch != '\n') 
		{
		    *p = ch;		/* Put character in line */
		    if (p< &line[BIG-1]) p++;
		} 
	    else 
		{
		    *p++ = 0;		/* Terminate line */
		    p = line;		/* Scan it to parse it */
		    
		    /* OK we now have a line in 'p' lets parse it and 
		       print it */
		    
		    /* Break on line that begins with a 2. It's the end of
		     * data.
		     */
		    if (*p == '2')
			break;
		    
		    /*  lines beginning with 5 are errors, 
		     *  print them and quit
		     */
		    if (*p == '5') {
			START(HTML_H2);
			PUTS(p+4);
			END(HTML_H2);
			break;
		    }
		    
		    if(*p == '-') {
			/*  data lines look like  -200:#:
			 *  where # is the search result number and can be  
			 *  multiple digits (infinate?)
			 *  find the second colon and check the digit to the
			 *  left of it to see if they are diferent
			 *  if they are then a different person is starting. 
			 *  make this line an <h2>
			 */
			
			/* find the second_colon */
			second_colon = strchr( strchr(p,':')+1, ':');
			
			if(second_colon != NULL) {  /* error check */
			    
			    if (*(second_colon-1) != last_char)   
				/* print seperator */
			    {
				END(HTML_PRE);
				START(HTML_H2);
			    }
				
			    
			    /* right now the record appears with the alias 
			     * (first line)
			     * as the header and the rest as <pre> text
			     * It might look better with the name as the
			     * header and the rest as a <ul> with <li> tags
			     * I'm not sure whether the name field comes in any
			     * special order or if its even required in a 
			     * record,
			     * so for now the first line is the header no 
			     * matter
			     * what it is (it's almost always the alias)
			     * A <dl> with the first line as the <DT> and
			     * the rest as some form of <DD> might good also?
			     */
			    
			    /* print data */
			    PUTS(second_colon+1);
			    PUTS("\n");
			    
			    if (*(second_colon-1) != last_char)   
				/* end seperator */
			    {
				END(HTML_H2);
				START(HTML_PRE);
			    }
							    
			    /* save the char before the second colon
			     * for comparison on the next pass
			     */
			    last_char =  *(second_colon-1) ;
			    
			} /* end if second_colon */
		    } /* end if *p == '-' */
		} /* if end of line */
	    
	} /* Loop over characters */
    
    /* end the text block */
    PUTS("\n");
    END(HTML_PRE);
    PUTS("\n");
    FREE_TARGET;

    return;  /* all done */
} /* end of procedure */

/*	Display a Gopher Index document
 **	-------------------------------
 */

PRIVATE void display_index ARGS2 (
				  CONST char *,	arg,
				  HTParentAnchor *,anAnchor)
{
    
    START(HTML_H1);
    PUTS(arg);
    PUTS(" index");
    END(HTML_H1);
    START(HTML_ISINDEX);
    PUTS("\nThis is a searchable Gopher index.");
    PUTS(" Please enter keywords to search for.\n");
    
    if (!HTAnchor_title(anAnchor))
    	HTAnchor_setTitle(anAnchor, arg);
    
    FREE_TARGET;
    return;
}


/*      Display a CSO index document
**      -------------------------------
*/

PRIVATE void display_cso ARGS2 (
        CONST char *,   arg,
        HTParentAnchor *,anAnchor)
{
    START(HTML_H1);
    PUTS(arg);
    PUTS(" index");
    END(HTML_H1);
    START(HTML_ISINDEX);
    PUTS("\nThis is a searchable index of a CSO database.\n");
    PUTS(" Please enter keywords to search for. The keywords that you enter");
    PUTS(" will allow you to search on a person's name in the database.\n");

    if (!HTAnchor_title(anAnchor))
    	HTAnchor_setTitle(anAnchor, arg);
    
    FREE_TARGET;
    return;
}


/*		De-escape a selector into a command
**		-----------------------------------
**
**	The % hex escapes are converted. Otheriwse, the string is copied.
*/
PRIVATE void de_escape ARGS2(char *, command, CONST char *, selector)
{
    CONST char * p = selector;
    char * q = command;
	if (command == NULL) outofmem(__FILE__, "HTLoadGopher");
    while (*p) {		/* Decode hex */
	if (*p == HEX_ESCAPE) {
	    char c;
	    unsigned int b;
	    p++;
	    c = *p++;
	    b =   from_hex(c);
	    c = *p++;
	    if (!c) break;	/* Odd number of chars! */
	    *q++ = FROMASCII((b<<4) + from_hex(c));
	} else {
	    *q++ = *p++;	/* Record */
	}
    }
    *q++ = 0;	/* Terminate command */

}


/*		Load by name					HTLoadGopher
**		============
**
**	 Bug:	No decoding of strange data types as yet.
**
*/
PUBLIC int HTLoadGopher ARGS4(
	CONST char *,		arg,
	HTParentAnchor *,	anAnchor,
	HTFormat,		format_out,
	HTStream*,		sink)
{
    char *command;			/* The whole command */
    int status;				/* tcp return */
    char gtype;				/* Gopher Node type */
    char * selector;			/* Selector string */
 
    struct sockaddr_in soc_address;	/* Binary network address */
    struct sockaddr_in* sin = &soc_address;
    
    if (!acceptable_inited) init_acceptable();
    
    if (!arg) return -3;		/* Bad if no name sepcified	*/
    if (!*arg) return -2;		/* Bad if name had zero length	*/
    
    if (TRACE) fprintf(stderr, "HTGopher: Looking for %s\n", arg);
    
    
/*  Set up defaults:
*/
    sin->sin_family = AF_INET;	    		/* Family, host order  */
    sin->sin_port = htons(GOPHER_PORT);	    	/* Default: new port,  */

/* Get node name and optional port number:
*/
    {
	char *p1 = HTParse(arg, "", PARSE_HOST);
	int status = HTParseInet(sin, p1);
        free(p1);
        if (status) return status;   /* Bad */
    }
    
/* Get entity type, and selector string.
*/        
    {
	char * p1 = HTParse(arg, "", PARSE_PATH|PARSE_PUNCTUATION);
        gtype = '1';		/* Default = menu */
	selector = p1;
	if ((*selector++=='/') && (*selector)) {	/* Skip first slash */
	    gtype = *selector++;			/* Pick up gtype */
	}
	if (gtype == GOPHER_INDEX) {
	    char * query;
            HTAnchor_setIndex(anAnchor);	/* Search is allowed */
	    query = strchr(selector, '?');	/* Look for search string */
	    if (!query || !query[1]) {		/* No search required */
		target = HTML_new(anAnchor, format_out, sink);
		targetClass = *target->isa;
		display_index(arg, anAnchor);	/* Display "cover page" */
		return HT_LOADED;		/* Local function only */
	    }
	    *query++ = 0;			/* Skip '?' 	*/
	    command = malloc(strlen(selector)+ 1 + strlen(query)+ 2 + 1);
              if (command == NULL) outofmem(__FILE__, "HTLoadGopher");
	      
	    de_escape(command, selector);	/* Bug fix TBL 921208 */

	    strcat(command, "\t");
	  
	    {					/* Remove plus signs 921006 */
	    	char *p;
		for (p=query; *p; p++) {
		    if (*p == '+') *p = ' ';
		}
	    }
	    strcat(command, query);
        } else if (gtype == GOPHER_CSO) {
            char * query;
            HTAnchor_setIndex(anAnchor);        /* Search is allowed */
            query = strchr(selector, '?');      /* Look for search string */
            if (!query || !query[1]) {          /* No search required */
		target = HTML_new(anAnchor, format_out, sink);
		targetClass = *target->isa;
                display_cso(arg, anAnchor);     /* Display "cover page" */
                return HT_LOADED;                 /* Local function only */
            }
            *query++ = 0;                       /* Skip '?'     */
            command = malloc(strlen("query")+ 1 + strlen(query)+ 2 + 1);
              if (command == NULL) outofmem(__FILE__, "HTLoadGopher");

            de_escape(command, selector);       /* Bug fix TBL 921208 */

            strcpy(command, "query ");

            {                                   /* Remove plus signs 921006 */
                char *p;
                for (p=query; *p; p++) {
                    if (*p == '+') *p = ' ';
                }
            }
            strcat(command, query);

	    
	} else {				/* Not index */
	    command = command = malloc(strlen(selector)+2+1);
	    de_escape(command, selector);
	}
	free(p1);
    }
    
    {
	char * p = command + strlen(command);
	*p++ = CR;		/* Macros to be correct on Mac */
	*p++ = LF;
	*p++ = 0;
	/* strcat(command, "\r\n");	*/	/* CR LF, as in rfc 977 */
    }

/*	Set up a socket to the server for the data:
*/      
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    status = connect(s, (struct sockaddr*)&soc_address, sizeof(soc_address));
    if (status<0){
	if (TRACE) fprintf(stderr, "HTTPAccess: Unable to connect to remote host for `%s'.\n",
	    arg);
	free(command);
	return HTInetStatus("connect");
    }
    
    HTInitInput(s);		/* Set up input buffering */
    
    if (TRACE) fprintf(stderr, "HTGopher: Connected, writing command `%s' to socket %d\n", command, s);
    
#ifdef NOT_ASCII
    {
    	char * p;
	for(p = command; *p; p++) {
	    *p = TOASCII(*p);
	}
    }
#endif

    status = NETWRITE(s, command, (int)strlen(command));
    free(command);
    if (status<0){
	if (TRACE) fprintf(stderr, "HTGopher: Unable to send command.\n");
	    return HTInetStatus("send");
    }

/*	Now read the data from the socket:
*/    
    switch (gtype) {
    
    case GOPHER_HTML :
    	HTParseSocket(WWW_HTML, format_out, anAnchor, s, sink);
	break;

    case GOPHER_GIF:
    case GOPHER_IMAGE:
    	HTParseSocket(HTAtom_for("image/gif"), 
			   format_out, anAnchor, s, sink);
  	break;
    case GOPHER_MENU :
    case GOPHER_INDEX :
	target = HTML_new(anAnchor, format_out, sink);
	targetClass = *target->isa;
        parse_menu(arg, anAnchor);
	break;
	 
    case GOPHER_CSO:
	target = HTML_new(anAnchor, format_out, sink);
	targetClass = *target->isa;
      	parse_cso(arg, anAnchor);
	break;
   	
      case GOPHER_MACBINHEX:
      case GOPHER_PCBINHEX:
      case GOPHER_UUENCODED:
      case GOPHER_BINARY:
        /* Specifying WWW_UNKNOWN forces dump to local disk. */
        HTParseSocket (WWW_UNKNOWN, format_out, anAnchor, s, sink);
	break;

    case GOPHER_TEXT :
    default:			/* @@ parse as plain text */
     	HTParseSocket(WWW_PLAINTEXT, format_out, anAnchor, s, sink);
	break;

      case GOPHER_SOUND :
    	HTParseSocket(WWW_AUDIO, format_out, anAnchor, s, sink);
	break;
	
    } /* switch(gtype) */

    NETCLOSE(s);
    return HT_LOADED;
}

PUBLIC HTProtocol HTGopher = { "gopher", HTLoadGopher, NULL };

