/*			GOPHER ACCESS				HTGopher.c
**			=============
**
** History:
**	26 Sep 90	Adapted from other accesses (News, HTTP) TBL
**	29 Nov 91	Downgraded to C, for portable implementation.
**	28 Apr 94	target no more global and icons implemented
**			HF, frystyk@dxcern.cern.ch
**	 2 May 94	Fixed possible security hole when the URL contains
**			a newline, that could cause multiple commands to be
**			sent to a Gopher server. AL, luotonen@www.cern.ch
**	12 May 94	Checked and made ready for multi-threads, Frystyk
**	 8 Jul 94  FM	Insulate free() from _free structure element.
**
** NOTE:
**	When parsing a gopher menu, we can't use the default HTParseSocket
**	but we will hav eto take care of the stram ourselves :-(
**
*/

/* Implementation dependent include files */
#include "tcp.h"

/* Library include files */
#include "HTParse.h"
#include "HTUtils.h"
#include "HTTCP.h"
#include "HTIcons.h"
#include "HTAccess.h"
#include "HTFormat.h"
#include "HTError.h"
#include "HTFile.h"
#include "HTML.h"
#include "HTMLPDTD.h"
#include "HTDirBrw.h"
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
#define FREE_TARGET (*target->isa->_free)(target)

/* Type definitions and global variables etc. local to this module */
typedef enum _HTGopherType {
    GOPHER_TEXT		= '0',
    GOPHER_MENU		= '1',
    GOPHER_CSO		= '2',
    GOPHER_ERROR	= '3',
    GOPHER_MACBINHEX	= '4',
    GOPHER_PCBINHEX	= '5',
    GOPHER_UUENCODED	= '6',
    GOPHER_INDEX	= '7',
    GOPHER_TELNET	= '8',
    GOPHER_BINARY       = '9',
    GOPHER_GIF          = 'g',
    GOPHER_HTML		= 'h',        				     /* HTML */
    GOPHER_INFO		= 'i',
    GOPHER_SOUND        = 's',
    GOPHER_WWW		= 'w',				       /* W3 address */
    GOPHER_IMAGE        = 'I',
    GOPHER_TN3270       = 'T',
    GOPHER_DUPLICATE	= '+',
    GOPHER_PLUS_IMAGE	= ':',			/* Addition from Gopher Plus */
    GOPHER_PLUS_MOVIE	= ';',
    GOPHER_PLUS_SOUND	= '<'
} HTGopherType;

struct _HTStructured {
	CONST HTStructuredClass *	isa;
	/* ... */
};

/* This is the local definition of HTRequest->net_info */
typedef struct _gopher_info {
    int                         sockfd;   /* Socket number for communication */
    HTInputSocket *		isoc;			     /* Input buffer */
    int 			addressCount;	  /* Attempts if multi-homed */
    BOOL			CRLFdotCRLF;   /* Transmission end like this */
    HTRequest *		   	request;		/* Request structure */

    HTGopherType	        type; 	                 /* Gopher item type */
} gopher_info;

/* ------------------------------------------------------------------------- */

/*							      get_gopher_icon
**
**	This function finds an appopriate icon for the item in the gopher
**	list. Actually it is only a shell build upon HTGetIcon().
**
*/
PRIVATE HTIconNode *get_gopher_icon ARGS2(CONST char *, filename,
					  int, gopher_type)
{
    HTFormat content_type = NULL;
    HTAtom *content_encoding = NULL;

    if (gopher_type == GOPHER_MENU)
	return icon_dir ? icon_dir : icon_unknown;

    switch(gopher_type) {
      case GOPHER_TEXT:
	content_type = HTAtom_for("text/void");
	break;
      case GOPHER_IMAGE:
      case GOPHER_PLUS_IMAGE:
      case GOPHER_GIF:
	content_type = HTAtom_for("image/void");
	break;
      case GOPHER_WWW:
      case GOPHER_HTML:
	content_type = HTAtom_for("text/void");
	break;
      case GOPHER_SOUND:
      case GOPHER_PLUS_SOUND:
	content_type = HTAtom_for("audio/void");
	break;
      case GOPHER_PLUS_MOVIE:
	content_type = HTAtom_for("video/void");
	break;
      case GOPHER_INDEX:
	content_type = HTAtom_for("application/x-gopher-index");
	break;
      case GOPHER_CSO:
	content_type = HTAtom_for("application/x-gopher-cso");
	break;
      case GOPHER_TELNET:
	content_type = HTAtom_for("application/x-gopher-telnet");
	break;
      case GOPHER_TN3270:
	content_type = HTAtom_for("application/x-gopher-tn3270");
	break;
      case GOPHER_DUPLICATE:
	content_type = HTAtom_for("application/x-gopher-duplicate");
	break;
      case GOPHER_ERROR:
	content_type = HTAtom_for("www/unknown");
	break;
      case GOPHER_MACBINHEX:
      case GOPHER_PCBINHEX:
      case GOPHER_UUENCODED:
      case GOPHER_BINARY:
	{       /* Do our own filetyping -- maybe we get lucky */
            HTAtom *language;
            content_type = HTFileFormat(filename, &content_encoding,
					&language);
	}
      default:
	content_type = HTAtom_for("www/unknown");
	break;
    }
    return HTGetIcon(S_IFMT & S_IFREG, content_type, content_encoding);
}


/*							      parse_menu
**
**	This function parses a gopher menu and puts it into a iconized
**	list.
**
**	Returns HT_LOADED on succed, HT_INTERRUPTED if interrupted and -1
**	if other error.
**
*/
PRIVATE int parse_menu ARGS3(HTRequest *,     	request,
			     gopher_info *,	gopher,
			     CONST char *,	url)
#define TAB 		'\t'
#define HEX_ESCAPE 	'%'
{
    int status = -1;
    unsigned int files = 0;
    int ch;
    HTChunk *chunk = HTChunkCreate(128);
    char *message = NULL;			     /* For a gopher message */
    char *info = NULL;		  /* For gopher information send as `i' type */

    HTStructured *target = NULL;

    gopher->isoc = HTInputSocket_new(gopher->sockfd);
    
    /* Output the list */
    while ((ch = HTInputSocket_getCharacter(gopher->isoc)) >= 0) {
        if (ch == CR || ch == LF) {
	    if (chunk->size) {				    /* Got some text */
		char *name = NULL;		       /* Gopher menu fields */
		char *selector = NULL;
		char *host = NULL;
		char *port = NULL;
		char *strptr;
		char *errptr;
		char gtype;
		HTChunkTerminate(chunk);
		strptr = chunk->data;		      /* Scan it to parse it */
		if (PROT_TRACE)
		    fprintf(stderr, "HTGopher.... Menu item: `%s\'\n",
			    chunk->data);
		gtype = *strptr++;

#if 0
		if (gtype == GOPHER_ERROR) {
		    StrAllocCat(message, chunk->data+1);
		    break;
		}
#endif
		/* If information then add it to the info string */
		if (gtype == GOPHER_INFO) {
		    if ((errptr = strchr(chunk->data, '\t')) != NULL)
			*errptr = '\0';
		    if (info) {
			StrAllocCat(info, "\n");
			StrAllocCat(info, chunk->data+1);
		    } else
			StrAllocCopy(info, chunk->data+1);
		    HTChunkClear(chunk);
		    continue;
		}

		/* If first item is an error, then don't put any header out
		   but wait and see if there is a next item in the list. If not
		   then make error message, else use as list message. */
		if (!files && (strstr(chunk->data, "error.host") ||
		    strstr(chunk->data, "errorhost"))) {

		    /* If message is already initialized, then add this one. */
		    /* We don't want the gopher type character */
		    if ((errptr = strchr(chunk->data, '\t')) != NULL)
			*errptr = '\0';
		    if (message) {
			StrAllocCat(message, "\n");
			StrAllocCat(message, chunk->data+1);
		    } else
			StrAllocCopy(message, chunk->data+1);
		    HTChunkClear(chunk);
		    continue;
		}

		/* Stop listing if line with a dot by itself */
		if (!files && message && gtype=='.' && !*strptr) {
		    status = -1;
		    break;
		}

		/* Output title, maybe top message and list top  */
		if (!files) {
		    CONST char *title = HTAnchor_title(request->anchor);
		    char *outstr = NULL;
		    target = HTML_new(request, NULL, WWW_HTML,
				      request->output_format,
				      request->output_stream);
		    if (title) {
			StrAllocCopy(outstr, title);
			HTUnEscape(outstr);
		    } else
			StrAllocCopy(outstr, "Gopher Menu");
		    START(HTML_HTML);
		    START(HTML_HEAD);
		    START(HTML_TITLE);
		    PUTS(outstr);
		    END(HTML_TITLE);
		    END(HTML_HEAD);

		    START(HTML_BODY);
		    START(HTML_H1);
		    PUTS(outstr);
		    END(HTML_H1);
		    FREE(outstr);
		
		    /* Output any message on top of list */
		    if ((message || info) && HTDirInfo == HT_DIR_INFO_TOP) {
			if (message) PUTS(message);
			if (info) PUTS(info);
			START(HTML_BR);
		    }

		    /* Make everything in list preformatted */
		    START(HTML_PRE);

#ifdef OLD_CODE
		    /* Output the header line of the list */
		    if (!icon_blank) icon_blank = icon_unknown;
		    if (HTDirShowMask & HT_DIR_SHOW_ICON && icon_blank) {
			HTMLPutImg(target, icon_blank->icon_url,
				   HTIcon_alt_string(icon_blank->icon_alt, NO),
				   NULL);
		    }
		    PUTC(' ');
		    PUTS("Name");
		    PUTC('\n');
#endif /* OLD_CODE */
		    START(HTML_HR);
		    PUTC('\n');
		}

		/* Stop listing if line with a dot by itself */
		if (gtype=='.' && !*strptr) {
		    status = (!files && message) ? -1 : HT_LOADED;
		    break;
		}

		/* Parse menu item */
		if (*strptr) {
		    name = strptr;
		    selector = strchr(name, TAB);
		    if (selector) {
			*selector++ = 0;		   /* Terminate name */
			host = strchr(selector, TAB);
			if (host) {
			    *host++ = 0;	       /* Terminate selector */
			    port = strchr(host, TAB);
			    if (port) {
				char *junk;
				*port = ':';	     /* delimit host a la W3 */
				if ((junk = strchr(port, TAB)) != NULL)
				    *junk = '\0';		/* Chop port */
				if (*(port+1) == '0' && !*(port+2))
				    *port = '\0';
			    } /* port */
			} /* host */
		    } /* selector */
		} /* gtype and name */
		
		/* Get Icon type and output the icon */
		if (HTDirShowMask & HT_DIR_SHOW_ICON) {
		    HTIconNode *icon = get_gopher_icon(url, gtype);
		    if (icon && icon->icon_url) {
			HTMLPutImg(target, icon->icon_url,
				   HTIcon_alt_string(icon->icon_alt, YES),
				   NULL);
			PUTC(' ');
		    }
		}

		if (gtype == GOPHER_WWW) {	     /* Gopher pointer to W3 */
		    char *escaped = NULL;
		    escaped = HTEscape(selector, URL_PATH);
		    HTStartAnchor(target, NULL, escaped);
		    PUTS(name);
		    END(HTML_A);
		    free(escaped);
		} else if (port) {     		    /* Other types need port */
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
		    if ((address = (char *) malloc(addr_len)) == NULL)
			outofmem(__FILE__, "Gopher ParseMenu");
		    *address = '\0';

		    if (gtype == GOPHER_TELNET) {
			if (escaped)
			    sprintf(address, "telnet://%s@%s/",
				    escaped, host);
			else
			    sprintf(address, "telnet://%s/", host);
		    }
		    else if (gtype == GOPHER_TN3270) {
			if (escaped)
			    sprintf(address, "tn3270://%s@%s/",
				    escaped, host);
			else 
			    sprintf(address, "tn3270://%s/", host);
		    } else {
			if (escaped)
			    sprintf(address, "//%s/%c%s", host, gtype,
				    escaped);
			else
			    sprintf(address, "//%s/%c", host, gtype);
		    }

		    /* Now output the anchor if not a Gopher error */
		    if (gtype != GOPHER_ERROR &&
			!strstr(address, "error.host") &&
			!strstr(address, "errorhost")) {
			HTStartAnchor(target, NULL, address);
			PUTS(name);
			END(HTML_A);
		    } else 
			PUTS(name);	   /* Just put it out, but skip type */
		    FREE(address);
		    FREE(escaped);
		} else {				   /* If parse error */
		    if (PROT_TRACE)
			fprintf(stderr, "HTGopher.... Bad menu item, `%s\'\n",
				chunk->data);
		    PUTS(chunk->data);
		}
		PUTC('\n');
		HTChunkClear(chunk);
		++files;			   /* Update number of files */
	    }
	} else
	    HTChunkPutc(chunk, ch);
    }
    if (ch < 0)
	status = ch;

    /* If no files and message is initialized then make error message,
       else output the bottom part of the list*/
    if (status != HT_INTERRUPTED) {
	if (!files && status < 0) {
	    if (message) {
		HTErrorAdd(request, ERR_FATAL, NO, HTERR_GOPHER_SERVER,
			   (void *) message, strlen(message), "parse_menu");
	    } else {
		HTErrorAdd(request, ERR_FATAL, NO, HTERR_GOPHER_SERVER,
			   chunk->data, chunk->size, "parse_menu");
	    }
	} else if (target) {
#ifdef OLD_CODE
	    char *outstr;
	    if ((outstr = (char *) malloc(100)) == NULL)
		outofmem(__FILE__, "parse_menu");
	    if (files == 0)
		sprintf(outstr, "Empty directory");
	    else if (files == 1)
		sprintf(outstr, "1 file");
	    else
		sprintf(outstr, "%u files", files);
	    START(HTML_HR);
	    PUTS(outstr);
	    free(outstr);
#endif /* OLD_CODE */
	    START(HTML_HR);
	    if (!files) PUTS("Empty Gopher Menu");
	    END(HTML_PRE);
	    
	    /* Put out any messages */
	    if ((message || info) && HTDirInfo == HT_DIR_INFO_BOTTOM) {
		if (message) PUTS(message);
		if (info) PUTS(info);
		START(HTML_BR);
	    }
	    END(HTML_BODY);
	    END(HTML_HTML);
	    FREE_TARGET;
	} else {
	    if (PROT_TRACE)
		fprintf(stderr, "HTGopher.... Interrupted before any stream was put up.\n");
	}
    }

    /* Cleanup */
    FREE(message);
    FREE(info);
    HTInputSocket_free(gopher->isoc);
    HTChunkFree(chunk);
    return status;
}


/*	Parse a Gopher CSO document
**	============================
**
**	Accepts an open socket to a CSO server waiting to send us
**	data and puts it on the screen in a reasonable manner.
**
**	Perhaps this data can be automatically linked to some
**	other source as well???
**
**	Taken from hacking by Lou Montulli@ukanaix.cc.ukans.edu
**	on XMosaic-1.1, and put on libwww 2.11 by Arthur Secret, 
**	secret@dxcern.cern.ch.
**
**	Returns HT_LOADED on succed, HT_INTERRUPTED if interrupted and -1
**	if other error.
*/
PRIVATE int parse_cso ARGS3(HTRequest *, 	request,
			    gopher_info *, 	gopher,
			    CONST char *, 	url)
{
    int status = -1;
    unsigned int records = 0;
    int ch;
    char *cur_code = NULL;
    HTChunk *chunk = HTChunkCreate(128);
    HTStructured *target = NULL;

    gopher->isoc = HTInputSocket_new(gopher->sockfd);
    
    /* Start grabbing chars from the network */
    while ((ch = HTInputSocket_getCharacter(gopher->isoc)) >= 0) {
	if (ch == CR || ch == LF) {
	    if (chunk->size) {		
		/* OK we now have a line in 'p' lets parse it and print it */
		char *strptr;
		HTChunkTerminate(chunk);
		strptr = chunk->data;

		/* If line begins with a 1, then more data is coming, so we
		   put out the title */
		if (*strptr == '1' ||
		    !strncmp(strptr, "501", 3) || !strncmp(strptr, "502", 3)) {

		    /* Put up new stream */
		    target = HTML_new(request, NULL, WWW_HTML,
				      request->output_format,
				      request->output_stream);
		    START(HTML_H1);
		    PUTS("CSO Search Results");
		    END(HTML_H1);

                    /* Output the header line of the list */
                    START(HTML_PRE); /* To make it look as the other headers */
                    if (!icon_blank) icon_blank = icon_unknown;
                    if (HTDirShowMask & HT_DIR_SHOW_ICON && icon_blank) {
                        HTMLPutImg(target, icon_blank->icon_url,
                                   HTIcon_alt_string(icon_blank->icon_alt, NO),
                                   NULL);
                    }
                    PUTC(' ');
                    PUTS("Record");
                    PUTC('\n');
                    START(HTML_HR);
                    PUTC('\n');
		    END(HTML_PRE);
		}

		/* Break on line that begins with a 2. It's the end of data. */
		if (*strptr == '2') {
		    status = HT_LOADED;
		    break;
		}
		
		/* Lines beginning with 5 are errors, generate msg and quit */
		if (*strptr == '5') {
		    char *msgptr = strchr(chunk->data, ':');
		    if (!msgptr)
			msgptr = chunk->data;
		    else
			++msgptr;
		    if (!strncmp(strptr, "501", 3))	       /* No entries */
			status = HT_LOADED;
		    else if (!strncmp(strptr, "502", 3)) {	 /* Too many */
			status = HT_LOADED;
			PUTS(msgptr);
		    } else {
			HTErrorAdd(request, ERR_FATAL, NO, HTERR_CSO_SERVER,
				   (void *) msgptr,
				   strlen(msgptr), "parse_cso");
		    }
		    break;
		}
		
		if(*strptr == '-') {
		    /*  data lines look like  -200:#:
		     *  where # is the search result number and can be  
		     *  multiple digits (infinate?)
		     *  find the second colon and check the digit to the
		     *  left of it to see if they are diferent
		     *  if they are then a different person is starting. 
		     *  make this line an <h2>
		     */
		    char *code;	      	    /* format: -200:code:field:value */
		    char *field;
		    char *value;
		    if ((code = strchr(strptr, ':')) != NULL &&
			(field = strchr(++code, ':')) != NULL) {
			*field++ = '\0';
			
			/* Let's do a strcmp instead of numbers */
			if (!records) {		   /* Header of first record */
			    records++;
			    START(HTML_H2);
			    PUTS("Record 1");
			    END(HTML_H2);
			    START(HTML_DL);
			} else if (cur_code && strcmp(code, cur_code)) {
			    char recstr[20];
			    records++;
			    END(HTML_DL);
			    START(HTML_H3);
			    PUTS("Record ");
			    sprintf(recstr, "%u", records);
			    PUTS(recstr);
			    END(HTML_H3);
			    START(HTML_DL);
			} else
			    START(HTML_DT);
			
			/* I'm not sure whether the name field comes in any
			 *  special order or if its even required in a 
			 *  record, so for now the first line is the header
			 *  no matter what it is (it's almost always the
			 *  alias)
			 */
			if ((value = strchr(field, ':')) == NULL)
			    value = "Empty?";
			else
			    *value++ = '\0';
			{
			    char *strip = HTStrip(field);
			    PUTS(strip);
			    START(HTML_DD);
			    strip = HTStrip(value);
			    PUTS(strip);
			}
			
			/* save the code for comparison on the next pass */
			StrAllocCopy(cur_code, code);
		    }
		} /* end data line */
		HTChunkClear(chunk);
	    } /* end new line */
	} else
	    HTChunkPutc(chunk, ch);
    }
    if (ch < 0)
	status = ch;

    /* Put out the bottom line */
    if (status != HT_INTERRUPTED) {
	if (target) {
	    char *outstr;
	    if ((outstr = (char *) malloc(100)) == NULL)
		outofmem(__FILE__, "parse_menu");
	    if (!records)
		sprintf(outstr, "No records");
	    else if (records == 1)
		sprintf(outstr, "1 record");
	    else
		sprintf(outstr, "%u records", records);
	    START(HTML_PRE);
	    START(HTML_HR);
	    PUTS(outstr);
	    END(HTML_PRE);
	    free(outstr);
	    FREE_TARGET;
	} else {
	    if (PROT_TRACE)
		fprintf(stderr, "HTGopher.... Interrupted before any stream was put up.\n");
	}
    }

    /* Clean up */
    HTInputSocket_free(gopher->isoc);
    HTChunkFree(chunk);
    FREE(cur_code);
    return status;
}


/*	Display a Gopher Index document
**	-------------------------------
*/
PRIVATE void display_index ARGS2(HTRequest *, 		request,
				 CONST char *,		url)
{
    HTStructured *target = HTML_new(request, NULL, WWW_HTML,
				    request->output_format,
				    request->output_stream);

    START(HTML_H1);
    PUTS("Searchable Gopher Index");
    END(HTML_H1);
    START(HTML_ISINDEX);
    if (!HTAnchor_title(request->anchor))
    	HTAnchor_setTitle(request->anchor, url);    
    FREE_TARGET;
    return;
}


/*      Display a CSO index document
**      -------------------------------
*/
PRIVATE void display_cso ARGS2(HTRequest *,		request,
			       CONST char *,		url)
{
    HTStructured *target = HTML_new(request, NULL, WWW_HTML,
				    request->output_format,
				    request->output_stream);
    START(HTML_H1);
    PUTS("Searchable Index of a CSO Name Server");
    END(HTML_H1);
    START(HTML_ISINDEX);
    if (!HTAnchor_title(request->anchor))
    	HTAnchor_setTitle(request->anchor, url);
    FREE_TARGET;
    return;
}



/*                                                        HTGopher_send_cmd
**
**      This function creates a socket and writes the gopher command to it.
**	The command must be terminated with <CRLF>
**
**      Returns 0 on OK, else <0 but does NOT close the connection
*/
PRIVATE int HTGopher_send_cmd ARGS3(gopher_info *, 	gopher,
				    char *,		url,
				    char *,		command)
{
    int status = 0;
    if (!gopher || !command) {
	if (PROT_TRACE)
	    fprintf(stderr, "Gopher Tx... Bad argument!\n");
	return -1;
    }
    if ((status = HTDoConnect((HTNetInfo *) gopher, url, GOPHER_PORT,
			      NULL, NO)) < 0) {
	if (PROT_TRACE)
	    fprintf(stderr, "HTLoadGopher Connection not established!\n");
	return status;
    }	
    if (PROT_TRACE)
	fprintf(stderr, "Gopher...... Connected, socket %d\n", gopher->sockfd);
    
    /* Write the command to the socket */
#ifdef NOT_ASCII
    {
	char * p;
	for(p = command; *p; p++) {
	    *p = TOASCII(*p);
	}
    }
#endif
    if (PROT_TRACE)
	fprintf(stderr, "Gopher Tx... %s", command);
    if ((status = NETWRITE(gopher->sockfd, command,
			  (int) strlen(command))) < 0) {
	if (PROT_TRACE)
	    fprintf(stderr, "Gopher...... Error sending command: %s\n",
		    command);
	HTErrorSysAdd(gopher->request, ERR_FATAL, NO, "NETWRITE");
    } else
	status = 0;
    return status;
}


/*		Load by name					HTLoadGopher
**		============
**
**	Given a hypertext address, this routine loads a gopher document
**
** On entry,
**      request         This is the request structure
** On exit,
**      returns         <0              Error has occured
**                      HT_LOADED       OK
**
*/
PUBLIC int HTLoadGopher ARGS1(HTRequest *, request)
{
    char *url;
    int status = -1;
    char *command = NULL;
    gopher_info *gopher;
    
    if (!request || !request->anchor) {
	if (PROT_TRACE) fprintf(stderr, "HTLoadGopher Bad argument\n");
	return -1;
    }
    url = HTAnchor_physical(request->anchor);
    if (PROT_TRACE) fprintf(stderr, "HTGopher.... Looking for `%s\'\n", url);

    /* Initiate a new gopher structure and bind to resuest structure */
    if ((gopher = (gopher_info *) calloc(1, sizeof(gopher_info))) == NULL)
	outofmem(__FILE__, "HTLoadGopher");
    gopher->sockfd = -1;
    gopher->request = request;
    request->net_info = (HTNetInfo *) gopher;
    gopher->type = GOPHER_MENU;
    
    /* Get entity type, and selector string and generate command  */
    {
	char *path = HTParse(url, "", PARSE_PATH);
	char *selector = path;
	char *query = NULL;
	char *separator = NULL;
	if (*selector)
	    gopher->type = *selector++;			    /* Pick up gtype */
	if (gopher->type == GOPHER_INDEX) {
            HTAnchor_setIndex(request->anchor);		/* Search is allowed */
	    query = strchr(selector, '?');	   /* Look for search string */

	    /* Display local "cover page" only if no search requested */
	    if (!query || !*(query+1)) {	       /* No search required */
		display_index(request, url);
		status = HT_LOADED;		      /* Local function only */
	    } else {
		*query++ = 0;				   	 /* Skip '?' */
		separator = "\t";
	    }
        } else if (gopher->type == GOPHER_CSO) {
            HTAnchor_setIndex(request->anchor);         /* Search is allowed */
            query = strchr(selector, '?');	   /* Look for search string */

	    /* Display local "cover page" only if no search requested */
            if (!query || !*(query+1)) {               /* No search required */
                display_cso(request, url);
                status = HT_LOADED;                   /* Local function only */
            } else {
		*query++ = 0;  		                     /* Skip '?'     */
		separator = "query ";
	    }
	}

	/* Now generate the final command */
	if (status != HT_LOADED) {
	    char crlf[3];
	    StrAllocCopy(command, selector);
	    if (query) {
		char *p;
		for (p=query; *p; p++)           /* Remove plus signs 921006 */
		    if (*p == '+') *p = ' ';
		StrAllocCat(command, separator);
		StrAllocCat(command, query);
	    }
	    HTUnEscape(command);
	    HTCleanTelnetString(command);	   /* Prevent security holes */
	    *crlf = CR;				       /* Telnet termination */
	    *(crlf+1) = LF;
	    *(crlf+2) = '\0';
	    StrAllocCat(command, crlf);
	} 
	free(path);
    }
    
    /* Now we must ask the server for real data :-( */
    if (status != HT_LOADED) {
	if ((status = HTGopher_send_cmd(gopher, url, command)) == 0) {
	    
	    /* Now read the data from the socket: */    
	    switch (gopher->type) {
	      case GOPHER_HTML:
		status = HTParseSocket(WWW_HTML,  gopher->sockfd, request);
		break;
		
	      case GOPHER_GIF:
	      case GOPHER_IMAGE:
	      case GOPHER_PLUS_IMAGE:
		status = HTParseSocket(HTAtom_for("image/gif"), gopher->sockfd,
				       request);
		break;
	      case GOPHER_MENU:
	      case GOPHER_INDEX:
		status = parse_menu(request, gopher, url);
		break;
		
	      case GOPHER_CSO:
		status = parse_cso(request, gopher, url);
		break;
		
	      case GOPHER_MACBINHEX:
	      case GOPHER_PCBINHEX:
	      case GOPHER_UUENCODED:
	      case GOPHER_BINARY:
 		{
		    /* Do our own filetyping -- maybe we get lucky */
		    HTFormat format;
		    format = HTFileFormat(url, &request->content_encoding,
					  &request->content_language);
		    if (format) {
			if (PROT_TRACE)
			    fprintf(stderr, "Gopher...... Figured out content-type myself: %s\n", HTAtom_name(format));
			status = HTParseSocket(format, gopher->sockfd,
					       request);
		    }
		    else {
			if (PROT_TRACE)
			    fprintf(stderr,"Gopher...... using www/unknown\n");
			/* Specifying WWW_UNKNOWN forces dump to local disk */
			HTParseSocket(WWW_UNKNOWN, gopher->sockfd, request);
		    }
		}
		break;
		
	      case GOPHER_SOUND:
	      case GOPHER_PLUS_SOUND:
		status = HTParseSocket(WWW_AUDIO,  gopher->sockfd, request);
		break;
		
	      case GOPHER_PLUS_MOVIE:
		status = HTParseSocket(WWW_VIDEO,  gopher->sockfd, request);
		break;

		/* Try and look at the suffix - maybe it is a PostScript file
		   so that we should start an externam viewer. */
	      case GOPHER_TEXT:
	      default:
 		{
		    HTFormat format;
		    format = HTFileFormat(url, &request->content_encoding,
					  &request->content_language);
		    if (format) {
			if (PROT_TRACE)
			    fprintf(stderr, "Gopher...... Figured out content-type myself: %s\n", HTAtom_name(format));
			status = HTParseSocket(format, gopher->sockfd,
					       request);
		    }
		    else {
			status = HTParseSocket(WWW_PLAINTEXT, gopher->sockfd,
					       request);
		    }
		}
		break;
	    }
	}

	/* Close the connection */
	if (gopher->sockfd >= 0) {
	    if (PROT_TRACE) fprintf(stderr, "Gopher...... Closing socket %d\n",
				    gopher->sockfd);
	    if (NETCLOSE(gopher->sockfd) < 0)
		status = HTErrorSysAdd(request, ERR_FATAL, NO, "NETCLOSE");
	}
    }
    if (status == HT_INTERRUPTED) {
        HTErrorAdd(request, ERR_WARNING, NO, HTERR_INTERRUPTED, NULL, 0,
		   "HTLoadGopher");
    }
    FREE(command);
    free(gopher);

    if (status < 0 && status != HT_INTERRUPTED) {
	char *unescaped = NULL;
	StrAllocCopy(unescaped, url);
	HTUnEscape(unescaped);
        HTErrorAdd(request, ERR_FATAL, NO, HTERR_INTERNAL, (void *) unescaped,
		   (int) strlen(unescaped), "HTLoadGopher");
	HTAnchor_clearIndex(request->anchor);
	free(unescaped);
    }
    return status;
}

GLOBALDEF PUBLIC HTProtocol HTGopher = { "gopher", HTLoadGopher, NULL, NULL };

