/*			GOPHER ACCESS				HTGopher.c
**			=============
**
** History:
**	26 Sep 90	Adapted from other accesses (News, HTTP) TBL
**	29 Nov 91	Downgraded to C, for portable implementation.
*/

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
#define GOPHER_HTML		'h'		/* HTML */
#define GOPHER_DUPLICATE	'+'
#define GOPHER_WWW		'w'		/* W3 address */

#include <ctype.h>
#include "HTUtils.h"		/* Coding convention macros */
#include "tcp.h"

#include "HTGopher.h"

#include "HText.h"
#include "HTParse.h"
#include "HTFormat.h"
#include "HTTCP.h"

#ifdef NeXTStep
#include <appkit/defaults.h>
#define GOPHER_PROGRESS(foo)
#else
#define GOPHER_PROGRESS(foo) fprintf(stderr, "%s\n", (foo))
#endif

extern HTStyleSheet * styleSheet;

#define NEXT_CHAR HTGetChararcter()



/*	Module-wide variables
*/
PRIVATE int s;					/* Socket for GopherHost */
PRIVATE HText *	HT;				/* the new hypertext */
PRIVATE HTParentAnchor *node_anchor;		/* Its anchor */
PRIVATE int	diagnostic;			/* level: 0=none 2=source */

PRIVATE HTStyle *addressStyle;			/* For address etc */
PRIVATE HTStyle *heading1Style;			/* For heading level 1 */
PRIVATE HTStyle *textStyle;			/* Text style */


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



/*	Get Styles from stylesheet
**	--------------------------
*/
PRIVATE void get_styles NOARGS
{
    if (!heading1Style) heading1Style = HTStyleNamed(styleSheet, "Heading1");
    if (!addressStyle) addressStyle = HTStyleNamed(styleSheet, "Address");
    if (!textStyle) textStyle = HTStyleNamed(styleSheet, "Example");
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
    HTChildAnchor 	*anchor;
    HTParentAnchor	*dest;
    
    HText_beginAnchor(HT,
    		anchor = HTAnchor_findChildAndLink(node_anchor, "",  addr, 0));
    dest = HTAnchor_parent(
	    HTAnchor_followMainLink((HTAnchor *)anchor));
	    
    if (!HTAnchor_title(dest)) HTAnchor_setTitle(dest, text);
	    
    HText_appendText(HT, text);
    HText_endAnchor(HT);
}


/*	Parse a Gopher Menu document
**	============================
**
*/

PRIVATE void parse_menu ARGS2 (
	CONST char *,	arg,
	HTParentAnchor *,anAnchor)
{
    char gtype;
    char ch;
    char line[BIG];
    char address[BIG];
    char *name, *selector;		/* Gopher menu fields */
    char *host;
    char *port;
    char *p = line;
    

#define TAB 		'\t'
#define HEX_ESCAPE 	'%'

    if (!HTAnchor_title(anAnchor))
    	HTAnchor_setTitle(anAnchor, arg);/* Tell user something's happening */
    
    node_anchor = anAnchor;
    HT = HText_new(anAnchor);
    
    HText_beginAppend(HT);
    HText_appendText(HT, "Select one of:\n\n");
    
    while ((ch=NEXT_CHAR) != (char)EOF) {
    
        if (ch != '\n') {
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
		HText_appendParagraph(HT);

	    } else if (port) {		/* Other types need port */
		if (gtype == GOPHER_TELNET) {
		    if (*selector) sprintf(address, "telnet://%s@%s/",
		    	selector, host);
		    else sprintf(address, "telnet://%s/", host);
		    
		} else {			/* If parsed ok */
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
		HText_appendText(HT, "        "); /* Prettier JW/TBL */
		write_anchor(name, address);
		HText_appendParagraph(HT);
	    } else { /* parse error */
	        if (TRACE) fprintf(stderr,
			"HTGopher: Bad menu item.\n");
		HText_appendText(HT, line);
		HText_appendParagraph(HT);
	    } /* parse error */
	    
	    p = line;	/* Start again at beginning of line */
	    
	} /* if end of line */
	
    } /* Loop over characters */
	
    HText_endAppend(HT);
    return;
}

/*	Display a Gopher Index document
**	-------------------------------
*/

PRIVATE void display_index ARGS2 (
	CONST char *,	arg,
	HTParentAnchor *,anAnchor)
{
    node_anchor = anAnchor;
    HT = HText_new(anAnchor);
    HText_beginAppend(HT);
    HText_setStyle(HT, heading1Style);
    HText_appendText(HT, arg);
    HText_setStyle(HT, textStyle);
    HText_appendText(HT, "\nThis is a searchable index.\n");
	
    if (!HTAnchor_title(anAnchor))
    	HTAnchor_setTitle(anAnchor, arg);/* Tell user something's happening */
    
    HText_endAppend(HT);
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
PUBLIC int HTLoadGopher ARGS3(
	CONST char *,arg,
	HTParentAnchor *,anAnchor,
	int,diag)
{
    char *command;			/* The whole command */
    int status;				/* tcp return */
    char gtype;				/* Gopher Node type */
    char * selector;			/* Selector string */
 
    struct sockaddr_in soc_address;	/* Binary network address */
    struct sockaddr_in* sin = &soc_address;

    diagnostic = diag;			/* set global flag */
    
    if (!acceptable_inited) init_acceptable();
    
    if (!arg) return -3;		/* Bad if no name sepcified	*/
    if (!*arg) return -2;		/* Bad if name had zero length	*/
    
    if (TRACE) fprintf(stderr, "HTGopher: Looking for %s\n", arg);
    get_styles();
    
    
/*  Set up defaults:
*/
    sin->sin_family = AF_INET;	    		/* Family, host order  */
    sin->sin_port = htons(GOPHER_PORT);	    	/* Default: new port,  */

    if (TRACE) fprintf(stderr, "HTTPAccess: Looking for %s\n", arg);

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
		display_index(arg, anAnchor);	/* Display "cover page" */
		return 1;			/* Local function only */
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
	    
	} else {				/* Not index */
	    command = command = malloc(strlen(selector)+2+1);
	    de_escape(command, selector);
	}
	free(p1);
    }
    
    strcat(command, "\r\n");		/* Include CR for telnet compat. */
    

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
    if (diagnostic==2) gtype = GOPHER_TEXT;	/* Read as plain text anyway */
    
    switch (gtype) {
    
    case GOPHER_HTML :
    	HTParseFormat(WWW_HTML, anAnchor, s);
	NETCLOSE(s);
	return 1;

    case GOPHER_MENU :
    case GOPHER_INDEX :
        parse_menu(arg, anAnchor);
	NETCLOSE(s);
	return 1;
    	
    case GOPHER_TEXT :
    default:			/* @@ parse as plain text */
     	HTParseFormat(WWW_PLAINTEXT, anAnchor, s);
	NETCLOSE(s);
	return 1;
    } /* switch(gtype) */
    /*NOTREACHED*/
}

