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

#define CR   FROMASCII('\015')	/* Must be converted to ^M for transmission */
#define LF   FROMASCII('\012')	/* Must be converted to ^J for transmission */

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
    char * eol = 0;			/* End of line if found */
    int status;				/* tcp return */
    HTStream *	target = NULL;		/* Unconverted data */
    HTFormat format_in;			/* Format arriving in the message */
    
    CONST char* gate = 0;		/* disable this feature */
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
    {
	char * p = command + strlen(command);
	*p++ = CR;		/* Macros to be correct on Mac */
	*p++ = LF;
	*p++ = 0;
	/* strcat(command, "\r\n");	*/	/* CR LF, as in rfc 977 */
    }

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
                 sprintf(line, "Accept: %s q=%.3f%c%c",
			 HTAtom_name(pres->rep), pres->quality, CR, LF);
	      } else {
                 sprintf(line, "Accept: %s%c%c",
			 HTAtom_name(pres->rep), CR, LF);
	      }
	      StrAllocCat(command, line);

	    }
	}
    }
       
    StrAllocCat(command, "\015\012");	/* Blank line means "end" */
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

    format_in = WWW_HTML;	/* default */
    {
    
    /* Get numeric status etc */

	int status;
	int length = 0;
	BOOL end_of_file = NO;
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
	    eol = strchr(line_buffer + length, LF);
            if (eol && *(eol-1) == CR) *(eol-1) = ' '; 

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
	    
	    format_in = HTAtom_for("www/mime");

	    switch (server_status / 100) {
	    
	    default:		/* bad number */
		HTAlert("Unknown status reply from server!");
		break;
		
	    case 3:		/* Various forms of redirection */
	    	HTAlert(
	"Redirection response from server is not handled by this client");
		break;
		
	    case 4:		/* "I think I goofed" */
	    case 5:		/* I think you goofed */
		HTAlert("Error response from server");
		break;
		
	    case 2:		/* Good: Got MIME object */
		break;

	    }
	    
	    break;		/* Get out of for loop */
	    
	} /* Loop over lines */
    }		/* Scope of HTTP2 handling block */

/*	Set up the stream stack to handle the body of the message
*/

    target = HTStreamStack(format_in,
			format_out,
			sink , anAnchor);

    if (!target) {
	char buffer[1024];	/* @@@@@@@@ */
	if (line_buffer) free(line_buffer);
	sprintf(buffer, "Sorry, no known way of converting %s to %s.",
		HTAtom_name(format_in), HTAtom_name(format_out));
	fprintf(stderr, "HTTP: %s", buffer);
	return HTLoadError(sink, 501, buffer);
    }

    
/*	Push the data, maybe ignoring CR, down the stream
**	We have to remember the end of the first buffer we just read
*/
    if (format_in != WWW_HTML) {
        if (eol) (*target->isa->put_string)(target, eol+1);
	HTCopy(s, target);
	
    } else {   /* ascii text with CRLFs :-( */
        if (eol) {
	    char * p;
	    for (p = eol+1; *p; p++)
	        if (*p != '\r') (*target->isa->put_character)(target, *p);
	}
	HTCopyNoCR(s, target);
    }
    (*target->isa->end_document)(target);
    (*target->isa->free)(target);
	

/*	Clean up
*/
    
    if (line_buffer) free(line_buffer);

    if (TRACE) fprintf(stderr, "HTTP: close socket %d.\n", s);
    status = NETCLOSE(s);

    return HT_LOADED;			/* Good return */

}

/*	Protocol descriptor
*/

PUBLIC HTProtocol HTTP = { "http", HTLoadHTTP, 0 };
