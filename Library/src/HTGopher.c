/*							       	     HTGopher.c
**	GOPHER ACCESS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
** History:
**	26 Sep 90	Adapted from other accesses (News, HTTP) TBL
**	29 Nov 91	Downgraded to C, for portable implementation.
**	28 Apr 94	target no more global and icons implemented
**			HF, frystyk@w3.org
**	 2 May 94	Fixed possible security hole when the URL contains
**			a newline, that could cause multiple commands to be
**			sent to a Gopher server. AL, luotonen@www.cern.ch
**	12 May 94	Checked and made ready for multi-threads, Frystyk
**	 8 Jul 94  FM	Insulate free() from _free structure element.
**	   Sep 95  HFN	Made non-blocking and state stream oriented
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "WWWHTML.h"
#include "WWWDir.h"
#include "WWWTrans.h"
#include "HTNetMan.h"
#include "HTGopher.h"					 /* Implemented here */

/* Macros and other defines */
#ifndef GOPHER_PORT
#define GOPHER_PORT 70					/* See protocol spec */
#endif

/* Hypertext object building machinery */
#define PUTC(c) (*target->isa->put_character)(target, c)
#define PUTS(s) (*target->isa->put_string)(target, s)
#define START(e) (*target->isa->start_element)(target, e, 0, 0)
#define END(e) (*target->isa->end_element)(target, e)

/* Type definitions and global variables etc. local to this module */
typedef enum _HTGopherType {
    GT_TEXT		= '0',
    GT_MENU		= '1',
    GT_CSO		= '2',
    GT_ERROR		= '3',
    GT_MACBINHEX	= '4',
    GT_PCBINHEX		= '5',
    GT_UUENCODED	= '6',
    GT_INDEX		= '7',
    GT_TELNET		= '8',
    GT_BINARY		= '9',
    GT_GIF		= 'g',
    GT_HTML		= 'h',        				     /* HTML */
    GT_INFO		= 'i',
    GT_SOUND		= 's',
    GT_WWW		= 'w',				       /* W3 address */
    GT_IMAGE		= 'I',
    GT_TN3270		= 'T',
    GT_DUPLICATE	= '+',
    GT_PLUS_IMAGE	= ':',			/* Addition from Gopher Plus */
    GT_PLUS_MOVIE	= ';',
    GT_PLUS_SOUND	= '<',
    GT_EOF		= '.'
} HTGopherType;

/* Final states have negative value */
typedef enum _GopherState {
    GOPHER_ERROR	= -3,
    GOPHER_NO_DATA	= -2,
    GOPHER_GOT_DATA	= -1,
    GOPHER_BEGIN	= 0,
    GOPHER_NEED_CONNECTION,
    GOPHER_NEED_REQUEST,
    GOPHER_NEED_RESPONSE
} GopherState;

/* This is the context structure for the this module */
typedef struct _gopher_info {
    HTGopherType	type; 		                 /* Gopher item type */
    GopherState		state;
    char *		cmd;
    HTNet *		net;
} gopher_info;

#define MAX_GOPHER_LINE		256

struct _HTStructured {
    const HTStructuredClass *	isa;
};

struct _HTStream {
    const HTStreamClass *	isa;
    HTStructured *	  	target;
    HTRequest *			request;
    HTEOLState			state;
    char *			url;
    BOOL			pre;		       /* Preformatted mode? */
    BOOL			junk;		       /* For too long lines */
    BOOL			CSO;
    char			cso_rec[10];		/* CSO record number */
    char 			buffer[MAX_GOPHER_LINE+1];
    int				buflen;
};

struct _HTInputStream {
    const HTInputStreamClass *	isa;
};

PRIVATE HTDirShow	dir_show = HT_DS_ICON;

/* ------------------------------------------------------------------------- */

/*	GopherIcon
**	----------
**	This function finds an appopriate icon for the item in the gopher
**	list. Actually it is only a shell build upon HTIcon_find().
*/
PRIVATE HTIconNode *GopherIcon (HTGopherType type)
{
    HTFormat   content_type = NULL;
    HTEncoding content_encoding = NULL;
    HTFileMode mode = HT_IS_FILE;
    switch(type) {
      case GT_MENU:
	mode = HT_IS_DIR;
      case GT_TEXT:
	content_type = HTAtom_for("text/void");
	break;
      case GT_IMAGE:
      case GT_PLUS_IMAGE:
      case GT_GIF:
	content_type = HTAtom_for("image/void");
	break;
      case GT_WWW:
      case GT_HTML:
	content_type = HTAtom_for("text/void");
	break;
      case GT_SOUND:
      case GT_PLUS_SOUND:
	content_type = HTAtom_for("audio/void");
	break;
      case GT_PLUS_MOVIE:
	content_type = HTAtom_for("video/void");
	break;
      case GT_INDEX:
	content_type = HTAtom_for("application/x-gopher-index");
	break;
      case GT_CSO:
	content_type = HTAtom_for("application/x-gopher-cso");
	break;
      case GT_TELNET:
	content_type = HTAtom_for("application/x-gopher-telnet");
	break;
      case GT_TN3270:
	content_type = HTAtom_for("application/x-gopher-tn3270");
	break;
      case GT_DUPLICATE:
	content_type = HTAtom_for("application/x-gopher-duplicate");
	break;
      case GT_ERROR:
	content_type = HTAtom_for("www/unknown");
	break;
      case GT_BINARY:
	content_type = WWW_BINARY;
	break;
      default:
	content_type = HTAtom_for("www/unknown");
	break;
    }
    return HTIcon_find(mode, content_type, content_encoding);
}

/* ------------------------------------------------------------------------- */
/*				    STREAMS				     */
/* ------------------------------------------------------------------------- */

/*	GopherTitle
**	-----------
**	Create the top part of the page
*/
PRIVATE BOOL GopherTitle (HTStream *me)
{
    HTStructured *target = me->target;
    char *str = NULL;
    StrAllocCopy(str, me->CSO ? "CSO Search " : "GopherMenu");

    START(HTML_HTML);
    START(HTML_HEAD);
    START(HTML_TITLE);
    if (me->CSO) {
	char *keyword = strchr(me->url, '?');
	if (keyword) {
	    StrAllocCat(str, "for ");
	    StrAllocCat(str, ++keyword);
	}
    }
    PUTS(str);
    END(HTML_TITLE);
    END(HTML_HEAD);

    START(HTML_BODY);
    START(HTML_H1);
    PUTS(str);
    END(HTML_H1);
    HT_FREE(str);
    return YES;
}

/*	GopherBottom
**	------------
**	Create the bottom part of the page
*/
PRIVATE BOOL GopherBottom (HTStream *me)
{
    HTStructured *target = me->target;
    if (me->pre)
	END(HTML_PRE);
    END(HTML_BODY);
    END(HTML_HTML);
    return YES;
}

/*	GopherMenuLine
**	--------------
**	Parses a Gopher Menu Line
**	Return YES if more data else NO
*/
PRIVATE BOOL GopherMenuLine (HTStream *me, char *line)
{
    HTStructured *target = me->target;
    HTGopherType gtype = (HTGopherType) *line++;
    HTTRACE(PROT_TRACE, "HTGopher.... Menu line: `%s\'\n" _ line);
    if (gtype == GT_INFO) {
	char *stop = strchr(line, '\t');
	if (stop) *stop = '\0';
	PUTS(line);
    } else if (gtype == GT_ERROR) {
	char *stop = strchr(line, '\t');
	if (stop) *stop = '\0';
	PUTS(line);
    } else if ((strstr(line, "error.host") || strstr(line, "errorhost"))) {
	char *stop = strchr(line, '\t');	      /* Chop off error.host */
	if (stop) *stop = '\0';
	PUTS(line);
    } else if (gtype == GT_EOF) {
	return NO;
    } else {				    /* Parse normal gopher menu line */
	char *name = line;			     /* Get link information */
	char *selector = strchr(name, '\t');
	char *host = NULL;
	char *port = NULL;
	if (selector) {
	    *selector++ = '\0';
	    if ((host = strchr(selector, '\t'))) {
		*host++ = '\0';
		if ((port = strchr(host, '\t'))) {
		    char *junk;
		    *port = ':';	 	     /* delimit host a la W3 */
		    if ((junk = strchr(port, '\t')) != NULL)
			*junk = '\0';				/* Chop port */
		    if (*(port+1) == '0' && !*(port+2))
			*port = '\0';
		}
	    }
	}
	if (!me->pre) {		      /* For now we use preformatted listing */
	    START(HTML_PRE);
	    me->pre = YES;
	}
	if (dir_show & HT_DS_ICON) {		 	 /* Put out the icon */
	    HTIconNode *icon = GopherIcon(gtype);
	    if (icon) {
		char * alt = HTIcon_alternative(icon, YES);
		HTMLPutImg(target, HTIcon_url(icon), alt, NULL);
		HT_FREE(alt);
		PUTC(' ');
	    }
	}
	if (gtype == GT_WWW) {			     /* Gopher pointer to W3 */
	    char *escaped = NULL;
	    escaped = HTEscape(selector, URL_PATH);
	    HTStartAnchor(target, NULL, escaped);
	    PUTS(name);
	    END(HTML_A);
	    HT_FREE(escaped);
	} else if (port) {			    /* Other types need port */
	    char *escaped = NULL;
	    char *address = NULL;
	    int addr_len;
	    
	    /* Calculate the length of the WWW-address */
	    if (selector && *selector) {
		escaped = HTEscape(selector, URL_PATH);
		addr_len = 15 + strlen(escaped) + strlen(host) + 1;
	    } else {
		addr_len = 15 + strlen(host) + 1;
	    }
	    if ((address = (char *) HT_MALLOC(addr_len)) == NULL)
	        HT_OUTOFMEM("GopherMenuLine");
	    *address = '\0';

	    if (gtype == GT_TELNET) {
		if (escaped)
		    sprintf(address, "telnet://%s@%s/", escaped, host);
		else
		    sprintf(address, "telnet://%s/", host);
	    } else if (gtype == GT_TN3270) {
		if (escaped)
		    sprintf(address, "tn3270://%s@%s/", escaped, host);
		else
		    sprintf(address, "tn3270://%s/", host);
	    } else {
		if (escaped)
		    sprintf(address, "//%s/%c%s", host, gtype, escaped);
		else
		    sprintf(address, "//%s/%c", host, gtype);
	    }
	    
	    HTStartAnchor(target, NULL, address);
	    PUTS(name);
	    END(HTML_A);
	    HT_FREE(address);
	    HT_FREE(escaped);
	    PUTC('\n');
	} else {					   /* If parse error */
	    HTTRACE(PROT_TRACE, "HTGopher.... Bad menu item, `%s\'\n" _ line);
	}
    }
    return YES;
}

/*	GopherCSOLine
**	--------------
**	Parses a Gopher Menu Line
**	Return YES if more data else NO
*/
PRIVATE BOOL GopherCSOLine (HTStream *me, char *line)
{
    HTStructured *target = me->target;
    if (*line == '1') {					 /* Information line */
	char *start = strchr(line, ':');
	if (start) start++; else start=line;
	PUTS(start);
    } else if (*line == '2') {				/* Transfer complete */
	return NO;
    } else if (*line == '5') {					    /* Error */
	char *start = strchr(line, ':');
	if (start) start++; else start=line;
	PUTS(start);
    } else if (*line == '-') {					     /* data */
	/*  data lines look like '-200:code:field:value'
	 *  where code is the search result number and can be  
	 *  multiple digits (infinte?)
	 *  find the second colon and check the digit to the
	 *  left of it to see if they are diferent
	 *  if they are then a different person is starting. 
	 */
	char *code;
	char *field;
	if ((code = strchr(line, ':')) && (field = strchr(++code, ':'))) {
	    BOOL newrec = YES;
	    *field++ = '\0';
	    if (!*me->cso_rec) {		   /* Header of first record */
		START(HTML_DL);
	    } else if (strcmp(me->cso_rec, code)) {    /* Another new record */
		START(HTML_B);
	    } else
		newrec = NO;
	    START(HTML_DT);
	    
	    /* I'm not sure whether the name field comes in any
	     *  special order or if its even required in a 
	     *  record, so for now the first line is the header
	     *  no matter what it is (it's almost always the
	     *  alias)
	     */
	    {
		char *value = strchr(field, ':');
		if (!value)
		    value = "Empty value";
		else
		    *value++ = '\0';
		{
		    char *strip = HTStrip(field);
		    PUTS(strip);
		    START(HTML_DD);
		    strip = HTStrip(value);
		    if (newrec) {
			PUTS(strip);
			END(HTML_B);
		    } else
			PUTS(strip);
	    }

	    /* Save the code for comparison on the next pass */
	    strcpy(me->cso_rec, code);
	    }
	}
    } else {						     /* Unknown line */
	char *start = strchr(line, ':');
	if (start) start++; else start=line;
	PUTS(start);
    }
    return YES;
}

/*
**  Searches for Gopher line until buffer fills up or a CRLF or LF is found
*/
PRIVATE int GopherMenu_put_block (HTStream * me, const char * b, int l)
{
    while (l-- > 0) {
	if (me->state == EOL_FCR) {
	    if (*b == LF && me->buflen) {
		if (!me->junk) {
		    BOOL cont;
		    *(me->buffer+me->buflen) = '\0';
		    cont = me->CSO ? GopherCSOLine(me, me->buffer) :
			GopherMenuLine(me, me->buffer);
		    if (cont == NO) return HT_LOADED;
		} else
		    me->junk = NO;			   /* back to normal */
	    }
	    me->buflen = 0;
	    me->state = EOL_BEGIN;
	} else if (*b == CR) {
	    me->state = EOL_FCR;
	} else if (*b == LF && me->buflen) {
	    if (!me->junk) {
		BOOL cont;
		*(me->buffer+me->buflen) = '\0';
		cont = me->CSO ? GopherCSOLine(me, me->buffer) :
		    GopherMenuLine(me, me->buffer);
		if (cont == NO) return HT_LOADED;
	    } else
		me->junk = NO;				   /* back to normal */
	    me->buflen = 0;
	    me->state = EOL_BEGIN;
	} else {
	    *(me->buffer+me->buflen++) = *b;
	    if (me->buflen >= MAX_GOPHER_LINE) {
		HTTRACE(PROT_TRACE, "Gopher...... Line too long - ignored\n");
		me->buflen = 0;
		me->junk = YES;
	    }
	}
	b++;
    }
    return HT_OK;
}

PRIVATE int GopherMenu_put_string (HTStream * me, const char* s)
{
    return GopherMenu_put_block(me, s, (int) strlen(s));
}

PRIVATE int GopherMenu_put_character (HTStream * me, char c)
{
    return GopherMenu_put_block(me, &c, 1);
}

PRIVATE int GopherMenu_flush (HTStream * me)
{    
    return (*me->target->isa->flush)(me->target);
}

PRIVATE int GopherMenu_free (HTStream * me)
{
    int status = HT_OK;
    GopherBottom(me);
    if ((status = (*me->target->isa->_free)(me->target)) == HT_WOULD_BLOCK)
	return HT_WOULD_BLOCK;
    HT_FREE(me);
    return HT_OK;
}

PRIVATE int GopherMenu_abort (HTStream * me, HTList * e)
{
    (*me->target->isa->abort)(me->target, e);
    HT_FREE(me);
    HTTRACE(PROT_TRACE, "Gopher...... ABORTING...\n");
    return HT_ERROR;
}

/*	Goper Menu Stream
**	-----------------
*/
PRIVATE const HTStreamClass GopherMenuClass =
{		
    "GopherMenu",
    GopherMenu_flush,
    GopherMenu_free,
    GopherMenu_abort,
    GopherMenu_put_character,
    GopherMenu_put_string,
    GopherMenu_put_block
};

/*
**  Stream for creating a HTML object out of a Gopher Menu object
*/
PRIVATE HTStream * GopherMenu_new (HTRequest * request, char *url, BOOL CSO)
{
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("GopherMenu_new");
    me->isa = &GopherMenuClass;
    me->target = HTMLGenerator(request, NULL, WWW_HTML,
			       HTRequest_outputFormat(request),
			       HTRequest_outputStream(request));
    HTAnchor_setFormat(HTRequest_anchor(request), WWW_HTML);
    me->request = request;    
    me->state = EOL_BEGIN;
    me->url = url;
    me->CSO = CSO;
    GopherTitle(me);
    return me;
}

/* ------------------------------------------------------------------------- */
/*				GOPHER STATE MACHINE			     */
/* ------------------------------------------------------------------------- */

/*	GopherCleanup
**	-------------
**      This function closes the connection and frees memory.
**      Returns YES if OK, else NO
*/
PRIVATE BOOL GopherCleanup (HTRequest * request, int status)
{
    HTNet * net = HTRequest_net(request);
    gopher_info * gopher = (gopher_info *) HTNet_context(net);
    HTStream * input = HTRequest_inputStream(request);

    /* Free stream with data TO network */
    if (input) {
	if (status == HT_INTERRUPTED)
	    (*input->isa->abort)(input, NULL);
	else
	    (*input->isa->_free)(input);
    }

    /* Remove the request object and our own context structure for gopher */
    HTNet_delete(net, status);
    if (gopher) {
	HT_FREE(gopher->cmd);
	HT_FREE(gopher);
    }
    return YES;
}

/*	Display a Gopher Index document
**	-------------------------------
**	We create a small HTML object as we have no network activity
*/
PRIVATE void display_index (HTRequest * request)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    HTStructured *target = HTMLGenerator(request, NULL, WWW_HTML,
					 HTRequest_outputFormat(request),
					 HTRequest_outputStream(request));

    /* Update anchor information */
    HTAnchor_setFormat(anchor, WWW_HTML);
    HTAnchor_setTitle(anchor, "Searchable Gopher Index");
    /* @@@ We don't set Content-Length */

    START(HTML_HTML);
    START(HTML_HEAD);
    START(HTML_TITLE);
    PUTS("Searchable Gopher Index");
    END(HTML_TITLE);
    END(HTML_HEAD);
    START(HTML_BODY);

    START(HTML_H1);
    PUTS("Searchable Gopher Index");
    END(HTML_H1);
    START(HTML_ISINDEX);
    END(HTML_BODY);
    END(HTML_HTML);
    (*target->isa->_free)(target);
}


/*      Display a CSO index document
**      -------------------------------
**	We create a small HTML object as we have no network activity
*/
PRIVATE void display_cso (HTRequest * request)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    HTStructured *target = HTMLGenerator(request, NULL, WWW_HTML,
					 HTRequest_outputFormat(request),
					 HTRequest_outputStream(request));

    /* Update anchor information */
    HTAnchor_setFormat(anchor, WWW_HTML);
    HTAnchor_setTitle(anchor, "Searchable SCO Index");
    /* @@@ We don't set Content-Length */

    START(HTML_HTML);
    START(HTML_HEAD);
    START(HTML_TITLE);
    PUTS("Searchable Index of a CSO Name Server");
    END(HTML_TITLE);
    END(HTML_HEAD);
    START(HTML_BODY);

    START(HTML_H1);
    PUTS("Searchable Index of a CSO Name Server");
    END(HTML_H1);
    PUTS("A CSO Name Server usually provides directory information about people.");
    START(HTML_ISINDEX);
    END(HTML_BODY);
    END(HTML_HTML);
    (*target->isa->_free)(target);
}

/*	HTLoadGopher
**	------------
**	Given a hypertext address, this routine loads a gopher document
**
** On entry,
**      request		This is the request structure
** On Exit
**	returns		HT_ERROR	Error has occured in call back
**			HT_OK		Call back was OK
*/
PRIVATE int GopherEvent (SOCKET soc, void * pVoid, HTEventType type);

PUBLIC int HTLoadGopher (SOCKET soc, HTRequest * request)
{
    gopher_info * gopher;
    HTNet * net = HTRequest_net(request);
    HTParentAnchor * anchor = HTRequest_anchor(request);
    char * url = HTAnchor_physical(anchor);
    
    /*
    ** Initiate a new gopher structure and bind to request structure
    ** This is actually state GOPHER_BEGIN, but it can't be in the state
    ** machine as we need the structure first.
    */
    HTTRACE(PROT_TRACE, "Gopher...... Looking for `%s\'\n" _ url);
    if ((gopher = (gopher_info *) HT_CALLOC(1, sizeof(gopher_info))) == NULL)
	HT_OUTOFMEM("HTLoadGopher");
    gopher->type = GT_MENU;
    gopher->state = GOPHER_BEGIN;
    gopher->net = net;
    HTNet_setContext(net, gopher);
    HTNet_setEventCallback(net, GopherEvent);
    HTNet_setEventParam(net, gopher);  /* callbacks get http* */

    return GopherEvent(soc, gopher, HTEvent_BEGIN);		/* get it started - ops is ignored */
}

PRIVATE int GopherEvent (SOCKET soc, void * pVoid, HTEventType type)
{
    gopher_info * gopher = (gopher_info *)pVoid;
    int status = HT_ERROR;
    HTNet * net = gopher->net;
    HTRequest * request = HTNet_request(net);
    HTParentAnchor * anchor = HTRequest_anchor(request);
    HTHost * host = HTNet_host(net);
    char * url = HTAnchor_physical(anchor);
    
    if (type == HTEvent_CLOSE) {			      /* Interrupted */
	GopherCleanup(request, HT_INTERRUPTED);
	return HT_OK;
    } else
	gopher = (gopher_info *) HTNet_context(net);	/* Get existing copy */

    /* Now jump into the machine. We know the state from the previous run */
    while (1) {
	switch (gopher->state) {

	  case GOPHER_BEGIN:	     /* Get entity type, and selector string */
	    {
		char *path = HTParse(url, "", PARSE_PATH);
		char *selector = path;
		char *query = NULL;
		char *separator = NULL;
		if (*selector) gopher->type = (HTGopherType) *selector++;
		if (gopher->type == GT_INDEX) {
		    HTAnchor_setIndex(anchor);			 /* Is index */
		    query = strchr(selector, '?');

		    /* Display local cover page only if no search requested */
		    if (!query || !*(query+1)) {       /* No search required */
			display_index(request);
			gopher->state = GOPHER_GOT_DATA;
			HT_FREE(path);
			break;
		    } else {
			*query++ = 0; 				 /* Skip '?' */
			separator = "\t";
		    }
		} else if (gopher->type == GT_CSO) {
		    HTAnchor_setIndex(anchor);		/* Search is allowed */
		    query = strchr(selector, '?'); /* Look for search string */
		    
		    /* Display local cover page only if no search requested */
		    if (!query || !*(query+1)) {       /* No search required */
			display_cso(request);
			gopher->state = GOPHER_GOT_DATA;
			HT_FREE(path);
			break;
		    } else {
			*query++ = 0; 				 /* Skip '?' */
			*selector = 0;
			separator = "query ";
		    }
		}

		/* Now generate the final command */
		{
		    char crlf[3];
		    StrAllocCopy(gopher->cmd, selector);
		    if (query) {
			char *p;
			for (p=query; *p; p++)   /* Remove plus signs 921006 */
			    if (*p == '+') *p = ' ';
			StrAllocCat(gopher->cmd, separator);
			StrAllocCat(gopher->cmd, query);
		    }
		    HTUnEscape(gopher->cmd);
		    HTCleanTelnetString(gopher->cmd);   /* Prevent sec holes */
		    *crlf = CR;			       /* Telnet termination */
		    *(crlf+1) = LF;
		    *(crlf+2) = '\0';
		    StrAllocCat(gopher->cmd, crlf);
		}
		HT_FREE(path);
		gopher->state = GOPHER_NEED_CONNECTION;
	    }
	    break;

	  case GOPHER_NEED_CONNECTION:
	    status = HTHost_connect(host, net, url);
	    host = HTNet_host(net);
	    if (status == HT_OK) {
		/*
		** Check the protocol class to see if we have connected to a
		** the right class of server, in this case HTTP.
		*/
		{
		    HTHost * host = HTNet_host(net);
		    char * s_class = HTHost_class(host);
		    if (s_class && strcasecomp(s_class, "gopher")) {
			HTRequest_addError(request, ERR_FATAL, NO, HTERR_CLASS,
					   NULL, 0, "HTLoadGopher");
			gopher->state = GOPHER_ERROR;
			break;
		    }
		    HTHost_setClass(host, "gopher");
		}

		/* 
		** Create the stream pipe FROM the channel to the application.
		** The target for the input stream pipe is set up using the
		** stream stack.
		*/
		{
		    if (gopher->type == GT_MENU || gopher->type == GT_INDEX)
			net->readStream = GopherMenu_new(request, url, NO);
		    else if (gopher->type == GT_CSO)
			net->readStream = GopherMenu_new(request, url, YES);
		    else
			net->readStream = HTStreamStack(WWW_UNKNOWN,
					       HTRequest_outputFormat(request),
					       HTRequest_outputStream(request),
					       request, NO);
		    HTRequest_setOutputConnected(request, YES);
		}

		/*
		** Create the stream pipe TO the channel from the application
		** and hook it up to the request object
		*/
		{
		    HTOutputStream * output = HTNet_getOutput(net, NULL, 0);
		    HTRequest_setInputStream(request, (HTStream *) output);
		}
		gopher->state = GOPHER_NEED_REQUEST;

	    } else if (status == HT_WOULD_BLOCK)
		return HT_OK;
	    else
		gopher->state = GOPHER_ERROR;
	    break;

	  case GOPHER_NEED_REQUEST:
	    HTTRACE(PROT_TRACE, "Gopher Tx... `%s\'" _ gopher->cmd);
	    {	
		HTStream * input = HTRequest_inputStream(request);
		status = (*input->isa->put_block)
		    (input, gopher->cmd, strlen(gopher->cmd));
		if (status == HT_WOULD_BLOCK)
		    return HT_OK;
		else if (status == HT_ERROR)
		    gopher->state = GOPHER_ERROR;
		else
		    gopher->state = GOPHER_NEED_RESPONSE;
	    }
	    break;

	  case GOPHER_NEED_RESPONSE:
	    status = HTHost_read(net->host, net);
	    if (status == HT_WOULD_BLOCK)
		return HT_OK;
	    else if (status == HT_LOADED || status == HT_CLOSED)
		gopher->state = GOPHER_GOT_DATA;
	    else
		gopher->state = GOPHER_ERROR;
	    break;

	  case GOPHER_GOT_DATA:
	    GopherCleanup(request, HT_LOADED);
	    return HT_OK;
	    break;

	  case GOPHER_NO_DATA:
	    GopherCleanup(request, HT_NO_DATA);
	    return HT_OK;
	    break;

	  case GOPHER_ERROR:
	    GopherCleanup(request, HT_ERROR);
	    return HT_OK;
	    break;
	}
    }  /* while(1) */
}
