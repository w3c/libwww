/*	HyperText Tranfer Protocol	- Client implementation		HTTP.c
**	==========================
**
** Bugs:
**	Not implemented:
**		Forward
**		Redirection
**		Error handling
*/

/*	Module parameters:
**	-----------------
**
**  These may be undefined and redefined by syspec.h
*/

/* Implements:
*/
#include "HTTP.h"

#define HTTP_VERSION	"HTTP/1.0"
#define HTTP2				/* Version is greater than 0.9 */

#define INIT_LINE_SIZE		1024	/* Start with line buffer this big */
#define LINE_EXTEND_THRESH	256	/* Minimum read size */
#define VERSION_LENGTH 		20	/* for returned protocol version */

/* Uses:
*/
#include "HTParse.h"
#include "HTUtils.h"
#include "tcp.h"
#include "HTTCP.h"
#include "HTFormat.h"
#include <ctype.h>
#include "HTAlert.h"
#include "HTMIME.h"


struct _HTStream {
	HTStreamClass * isa;		/* all we need to know */
};


/*		Load Document from HTTP Server			HTLoadHTTP()
**		==============================
**
**	Given a hypertext address, this routine loads a document.
**
**
** On entry,
**	arg	is the hypertext reference of the article to be loaded.
**	gate	is nill if no gateway, else the gateway address.
**
** On exit,
**	returns	>=0	If no error, a good socket number
**		<0	Error.
**
**	The socket must be closed by the caller after the document has been
**	read.
**
*/
PUBLIC int HTLoadHTTP ARGS4 (
	CONST char *, 		arg,
/*	CONST char *,		gate, */
	HTParentAnchor *,	anAnchor,
	HTFormat,		format_out,
	HTStream*,		sink)
{
    int s;				/* Socket number for returned data */
    char *command;			/* The whole command */
    int status;				/* tcp return */
    CONST char* gate = 0;		/* disable this feature */
    HTFormat format = WWW_HTML;		/* default is HTTP2 */   
    SockA soc_address;			/* Binary network address */
    SockA * sin = &soc_address;
    BOOL had_header = NO;		/* Have we had at least one header? */
    char * line_buffer = NULL;
    BOOL extensions = YES;		/* Assume good HTTP server */
    if (!arg) return -3;		/* Bad if no name sepcified	*/
    if (!*arg) return -2;		/* Bad if name had zero length	*/

/*  Set up defaults:
*/
#ifdef DECNET
    sin->sdn_family = AF_DECnet;	    /* Family = DECnet, host order */
    sin->sdn_objnum = DNP_OBJ;          /* Default: http object number */
#else  /* Internet */
    sin->sin_family = AF_INET;	    /* Family = internet, host order */
    sin->sin_port = htons(TCP_PORT);    /* Default: http port    */
#endif

    if (TRACE) {
        if (gate) fprintf(stderr,
		"HTTPAccess: Using gateway %s for %s\n", gate, arg);
        else fprintf(stderr, "HTTPAccess: Direct access for %s\n", arg);
    }
    
/* Get node name and optional port number:
*/
    {
	char *p1 = HTParse(gate ? gate : arg, "", PARSE_HOST);
	int status = HTParseInet(sin, p1);  /* TBL 920622 */
        free(p1);
	if (status) return status;   /* No such host for example */
    }
    
retry:
   
/*	Now, let's get a socket set up from the server for the sgml data:
*/      
#ifdef DECNET
    s = socket(AF_DECnet, SOCK_STREAM, 0);
#else
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif
    status = connect(s, (struct sockaddr*)&soc_address, sizeof(soc_address));
    if (status < 0) {
#ifndef DECNET
	/* This code is temporary backward-compatibility. It should
	   go away when no server runs on port 2784 alone */
	if (sin->sin_port == htons(TCP_PORT)) {  /* Try the old one */
	  if (TRACE) printf (
	    "HTTP: Port %d doesn't answer (errno = %d). Trying good old port %d...\n",
	    TCP_PORT, errno, OLD_TCP_PORT);
	  sin->sin_port = htons(OLD_TCP_PORT);
	  /* First close current socket and open a clean one */
	  status = NETCLOSE (s);
	  s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	  status = connect(s, (struct sockaddr*)&soc_address,
			   sizeof(soc_address));
	}
	if (status < 0)
#endif
	  {
	    if (TRACE) fprintf(stderr, 
	      "HTTP: Unable to connect to remote host for `%s' (errno = %d).\n", arg, errno);
	    /* free(command);   BUG OUT TBL 921121 */
	    return HTInetStatus("connect");
	  }
      }
    
    if (TRACE) fprintf(stderr, "HTTP connected, socket %d\n", s);

/*	Ask that node for the document,
**	omitting the host name & anchor if not gatewayed.
*/        
    if (gate) {
        command = malloc(4 + strlen(arg)+ 2 + 31);
        if (command == NULL) outofmem(__FILE__, "HTLoadHTTP");
        strcpy(command, "GET ");
	strcat(command, arg);
    } else { /* not gatewayed */
	char * p1 = HTParse(arg, "", PARSE_PATH|PARSE_PUNCTUATION);
        command = malloc(4 + strlen(p1)+ 2 + 31);
        if (command == NULL) outofmem(__FILE__, "HTLoadHTTP");
        strcpy(command, "GET ");
	strcat(command, p1);
	free(p1);
    }
#ifdef HTTP2
    if (extensions) {
        strcat(command, " ");
        strcat(command, HTTP_VERSION);
    }
#endif
    strcat(command, "\r\n");	/* Include CR for telnet compat. */
	    

#ifdef HTTP2
    if (extensions) {

	int n;
	int i;
        HTAtom * present = WWW_PRESENT;
	char line[256];    /*@@@@ */

	if (!HTPresentations) HTFormatInit();
	n = HTList_count(HTPresentations);

	for(i=0; i<n; i++) {
	    HTPresentation * pres = HTList_objectAt(HTPresentations, i);
	    if (pres->rep_out == present) {
	      if (pres->quality != 1.0) {
                 sprintf(line, "Accept: %s q=%.3f\r\n",
			 HTAtom_name(pres->rep), pres->quality);
	      } else {
                 sprintf(line, "Accept: %s\r\n",
			 HTAtom_name(pres->rep));
	      }
	      StrAllocCat(command, line);

	    }
	}
    }
    
    StrAllocCat(command, "\r\n");	/* BLANK LINE means "end" */
   
#endif

#ifdef NOT_ASCII
    {
    	char * p;
	for(p = command; *p; p++) {
	    *p = TOASCII(*p);
	}
    }
#endif

    if (TRACE) fprintf(stderr, "HTTP Tx: %s\n", command);
    status = NETWRITE(s, command, (int)strlen(command));
    free(command);
    if (status<0) {
	if (TRACE) fprintf(stderr, "HTTPAccess: Unable to send command.\n");
	    return HTInetStatus("send");
    }


/*	Now load the data:	HTTP2 response parse
*/
#ifdef HTTP2
    {
    
    /* Get numeric status etc */

	int status;
	int length = 0;
	char * eol = 0;
	BOOL end_of_file = NO;
	HTFormat format = WWW_PLAINTEXT;	/* default */
	HTAtom * encoding = HTAtom_for("7bit");
	int buffer_length = INIT_LINE_SIZE;	/* Why not? */
	
	line_buffer = (char *) malloc(buffer_length * sizeof(char));
	if (!line_buffer) outofmem(__FILE__, "HTLoadHTTP");
	
	for(;;) {

	    int fields;
	    char server_version [VERSION_LENGTH+1];
	    int server_status;
	    
	   /* Extend line buffer if necessary for those crazy WAIS URLs ;-) */
	   
	    if (buffer_length - length < LINE_EXTEND_THRESH) {
	        buffer_length = buffer_length + buffer_length;
		line_buffer = (char *) realloc(
			line_buffer, buffer_length * sizeof(char));
		if (!line_buffer) outofmem(__FILE__, "HTLoadHTTP");
	    }
	    status = NETREAD(s, line_buffer + length,
	    			buffer_length - length -1);
	    if (status < 0) {
	        HTAlert("Unexpected network read error on response");
		return status;
	    }
	    if (status == 0) {
	        end_of_file = YES;
		break;
	    }
	    line_buffer[length+status] = 0;
#ifdef NOT_ASCII
	    {
		char * p;
		for(p = line_buffer+length; *p; p++) {
		    *p = FROMASCII(*p);
		}
	    }
#endif
	    eol = strchr(line_buffer + length, '\n');
            if (eol && *(eol-1) == '\r') *(eol-1) = ' '; 

	    length = length + status;
	    	    
	    if (!eol && !end_of_file) continue;		/* No LF */	    
	    
	    *eol = 0;		/* Terminate the line */


/*	 We now have a terminated unfolded line.
*/

	    if (TRACE)fprintf(stderr, "HTTP: Rx: %s\n", line_buffer);

/* Kludge to work with old buggy servers.  They can't handle the third word
** so we try again without it.
*/
	    if (extensions &&
	         0==strcmp(line_buffer,		/* Old buggy server? */
	    	   "Document address invalid or access not authorised")) {
		extensions = NO;
    		if (line_buffer) free(line_buffer);
    		if (TRACE) fprintf(stderr,
		    "HTTP: close socket %d to retry with HTTP0\n", s);
    		NETCLOSE(s);
		goto retry;		/* @@@@@@@@@@ */
	    }

	    fields = sscanf(line_buffer, "%20s%d",
	    	server_version,
	    	&server_status);

	    if (fields < 2) break;
	    
	    switch (server_status / 100) {
	    
	    case 3:		/* Various forms of redirection */
	    case 4:		/* "I think I goofed" */
	    case 5:		/* I think you goofed */
	    default:		/* bad number */
	    	
		HTAlert("Bad status reply from server");
		/* Fall through @@@@@@@@@@@@@@@@@@@@@ */
		
	    case 2:		/* Good: Got MIME object */
		{
		    HTStream * mime = HTStreamStack(HTAtom_for("www/mime"),
		    	format_out, sink, anAnchor);
			
		    if (!mime) {
		    	if (line_buffer) free(line_buffer);
			return HTLoadError(sink, 403,
				"MIME: Can't convert this format");
		    }
		    mime->isa->put_string(mime, eol+1); /* Rest of buffer */
		    HTCopyNoCR(s, mime);		/* Rest of doc */
		    mime->isa->end_document(mime);
		    mime->isa->free(mime);
		    goto done;
		}
		break;

	    }
	    
	    break;		/* Get out of for loop */
	    
	} /* Loop over lines */
    }		/* Scope of HTTP2 handling block */

/* Now, we can assume that we did NOT have a MIME header so behave as for HTTP0
*/
    {
	HTParseSocket(format, format_out,
                (HTParentAnchor *) anAnchor, s, sink);
    }
#else
    HTParseSocket(format, format_out,
                (HTParentAnchor *) anAnchor, s, sink);
#endif

/*	Clean up
*/
done:
    if (line_buffer) free(line_buffer);
    
    if (TRACE) fprintf(stderr, "HTTP: close socket %d.\n", s);
    status = NETCLOSE(s);

    return HT_LOADED;			/* Good return */
}

/*	Protocol descriptor
*/

PUBLIC HTProtocol HTTP = { "http", HTLoadHTTP, 0 };
