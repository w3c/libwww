/*	HyperText73 Tranfer Protocol	- Client implementation		HTTP.c
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
#include "HTML.h"		/* SCW */
#include "HTInit.h"		/* SCW */
#include "HTAccess.h"		/* HTRequest */
#include "HTAABrow.h"		/* Access Authorization */
#include "HTTee.h"		/* Tee off a cache stream */
#include "HTFWriter.h"		/* Write to cache file */

struct _HTStream {
	HTStreamClass * isa;		/* all we need to know */
};

extern char * HTAppName;	/* Application name: please supply */
extern char * HTAppVersion;	/* Application version: please supply */

PUBLIC long HTProxyBytes = 0;	/* Number of bytes transferred thru proxy */
extern BOOL using_proxy;	/* are we using a proxy gateway? */
PUBLIC char * HTProxyHeaders = NULL;	/* Headers to pass as-is */

PRIVATE void parse_401_headers ARGS2(HTRequest *,	req,
				     HTInputSocket *,	isoc)
{
    HTAAScheme scheme;
    char *line;
    int num_schemes = 0;
    HTList *valid_schemes = HTList_new();
    HTAssocList **scheme_specifics = NULL;
    char *template = NULL;

    /* Read server reply header lines */

    if (TRACE)
	fprintf(stderr, "Server 401 reply header lines:\n");

    while (NULL != (line = HTInputSocket_getUnfoldedLine(isoc)) &&
	   *line != 0) {

	if (TRACE) fprintf(stderr, "%s\n", line);

	if (strchr(line, ':')) {	/* Valid header line */

	    char *p = line;
	    char *fieldname = HTNextField(&p);
	    char *arg1 = HTNextField(&p);
	    char *args = p;
	    
	    if (0==strcasecomp(fieldname, "WWW-Authenticate:")) {
		if (HTAA_UNKNOWN != (scheme = HTAAScheme_enum(arg1))) {
		    HTList_addObject(valid_schemes, (void*)scheme);
		    if (!scheme_specifics) {
			int i;
			scheme_specifics = (HTAssocList**)
			    malloc(HTAA_MAX_SCHEMES * sizeof(HTAssocList*));
			if (!scheme_specifics)
			    outofmem(__FILE__, "parse_401_headers");
			for (i=0; i < HTAA_MAX_SCHEMES; i++)
			    scheme_specifics[i] = NULL;
		    }
		    scheme_specifics[scheme] = HTAA_parseArgList(args);
		    num_schemes++;
		}
		else if (TRACE) {
		    fprintf(stderr, "Unknown scheme `%s' %s\n",
			    (arg1 ? arg1 : "(null)"),
			    "in WWW-Authenticate: field");
		}
	    }

	    else if (0==strcasecomp(fieldname, "WWW-Protection-Template:")) {
		if (TRACE)
		    fprintf(stderr, "Protection template set to `%s'\n", arg1);
		StrAllocCopy(template, arg1);
	    }

	} /* if a valid header line */
	else if (TRACE) {
	    fprintf(stderr, "Invalid header line `%s' ignored\n", line);
	} /* else invalid header line */
	free(line);
    } /* while header lines remain */
    FREE(line);
    req->valid_schemes = valid_schemes;
    req->scheme_specifics = scheme_specifics;
    req->prot_template = template;
}



/*		Load Document from HTTP Server			HTLoadHTTP()
**		==============================
**
**	Given a hypertext address, this routine loads a document.
**
**
** On entry,
**	arg	is the hypertext reference of the article to be loaded.
**
** On exit,
**	returns	>=0	If no error, a good socket number
**		<0	Error.
**
**	The socket must be closed by the caller after the document has been
**	read.
**
*/
PUBLIC int HTLoadHTTP ARGS1 (HTRequest *, request)
{
    CONST char * arg = NULL;
    int s;				/* Socket number for returned data */
    int status;				/* tcp return */
    char crlf[3];			/* A CR LF equivalent string */
    HTStream *	target = NULL;		/* Unconverted data */
    BOOL cache_http = YES;	   /* Enable caching of HTTP-retrieved files */

    CONST char* gate = 0;		/* disable this feature */
    SockA soc_address;			/* Binary network address */
    SockA * sin = &soc_address;
    BOOL extensions = YES;		/* Assume good HTTP server */

    if (HTImProxy) HTProxyBytes = 0;

    arg = HTAnchor_physical(request->anchor);

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

    sprintf(crlf, "%c%c", CR, LF);	/* To be corect on Mac, VM, etc */
    
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
    
/*
** Compose authorization information (this was moved here
** from after the making of the connection so that the connection
** wouldn't have to wait while prompting username and password
** from the user).				-- AL 13.10.93
*/
#ifdef ACCESS_AUTH
    HTAA_composeAuth(request);
    if (TRACE) {
	if (request->authorization)
	    fprintf(stderr, "HTTP: Sending Authorization: %s\n",
		    request->authorization);
	else
	    fprintf(stderr, "HTTP: Not sending authorization (yet)\n");
    }
#endif /* ACCESS_AUTH */
   
/*	Now, let's get a socket set up from the server for the data:
*/      
#ifdef DECNET
    s = socket(AF_DECnet, SOCK_STREAM, 0);
#else
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif
    status = connect(s, (struct sockaddr*)&soc_address, sizeof(soc_address));
    if (status < 0) {
	    if (TRACE) fprintf(stderr, 
	      "HTTP: Unable to connect to remote host for `%s' (errno = %d).\n",
	      arg, errno);

	    return HTInetStatus("connect");
      }
    
    if (TRACE) fprintf(stderr, "HTTP connected, socket %d\n", s);


/*	Compose and send command
**	------------------------
*/
    {
        char *command;			/* The whole command */
	
/*	Ask that node for the document,
**	omitting the host name & anchor if not gatewayed.
*/        
	if (gate) { /* This is no longer used, and could be thrown away */
	    command = malloc(4 + strlen(arg)+ 2 + 31);
	    if (command == NULL) outofmem(__FILE__, "HTLoadHTTP");
	    strcpy(command, "GET ");
	    strcat(command, arg);
	} else { /* not gatewayed */
	    char * p1 = HTParse(arg, "", PARSE_PATH|PARSE_PUNCTUATION);
	    command = malloc(4 + strlen(p1)+ 2 + 31);
	    if (command == NULL) outofmem(__FILE__, "HTLoadHTTP");
	    if (request->method != METHOD_INVALID) {
		strcpy(command, HTMethod_name(request->method));
		strcat(command, " ");
	    }
	    else {
		strcpy(command, "GET ");
	    }
	    /* if we are using a proxy gateway don't copy in the first slash
	    ** of say: /gopher://a;lkdjfl;ajdf;lkj/;aldk/adflj
	    ** so that just gohper://.... is sent.
	    */
	    if (using_proxy)
		strcat(command, p1+1);
	    else
		strcat(command, p1);
	    free(p1);
	}
#ifdef HTTP2
	if (extensions) {
	    strcat(command, " ");
	    strcat(command, HTTP_VERSION);
	}
#endif
    
	strcat(command, crlf);	/* CR LF, as in rfc 977 */
    
	if (extensions && HTImProxy && HTProxyHeaders) {
	    StrAllocCat(command, HTProxyHeaders);
	}
	else if (extensions) {

	    int i;
	    HTAtom * present = WWW_PRESENT;
	    char line[256];    /*@@@@ */
	    HTList *conversions[2];

	    if (!HTConversions) {
		HTConversions = HTList_new();
/*		HTFormatInit(HTConversions);	App may do this not us tbl940210 */
	    }

	    conversions[0] = HTConversions;
	    conversions[1] = request->conversions;

	    
	    for (i=0; i<2; i++) {
		HTList *cur = conversions[i];
		HTPresentation *pres;

		while ((pres = (HTPresentation*)HTList_nextObject(cur))) {
		    if (pres->rep_out == present) {
			if (pres->quality != 1.0) {
			    sprintf(line, "Accept: %s; q=%.3f%c%c",
				    HTAtom_name(pres->rep),
				    pres->quality, CR, LF);
			} else {
			    sprintf(line, "Accept: %s%c%c",
				    HTAtom_name(pres->rep), CR, LF);
			}
			StrAllocCat(command, line);
		    }
		}
	    }

	    sprintf(line, "User-Agent: %s/%s  libwww/%s%c%c",
		    HTAppName ? HTAppName : "unknown",
		    HTAppVersion ? HTAppVersion : "0.0",
		    HTLibraryVersion, CR, LF);
	    StrAllocCat(command, line);

#ifdef ACCESS_AUTH
	    if (request->authorization != NULL) {
		sprintf(line, "Authorization: %s%c%c",
			request->authorization, CR, LF);
		StrAllocCat(command, line);
	    }
#endif /* ACCESS_AUTH */

	}

	StrAllocCat(command, crlf);	/* Blank line means "end" */
    
	if (TRACE) fprintf(stderr, "HTTP Tx: %s\n", command);
    
    /*	Translate into ASCII if necessary
    */
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
	if (status<0) {
	    if (TRACE) fprintf(stderr,
	    	"HTTPAccess: Unable to send command.\n");
	    return HTInetStatus("send");
	}
    } /* compose and send command */
    

/*	Read the response
**	-----------------
**
**	HTTP0 servers must return ASCII style text, though it can in
**	principle be just text without any markup at all.
**	Full HTTP servers must return a response
**	line and RFC822 style header.  The response must therefore in
**	either case have a CRLF somewhere soon.
**
**	This is the theory.  In practice, there are (1993) unfortunately
**	many binary documents just served up with HTTP0.9.  This
**	means we have to preserve the binary buffer (on the assumption that
**	conversion from ASCII may lose information) in case it turns
**	out that we want the binary original.
*/
    if (HTImProxy) {

	/*
	** Server as a gateway -- send body of the message
	** received from client (if any).
	*/
	if (request->isoc && request->content_length > 0) {
	    int remain = request->content_length;
	    int i = remain;
	    char * buf;

	    while (remain > 0  &&
		   (buf = HTInputSocket_getBlock(request->isoc, &i))) {
		int status = NETWRITE(s, buf, i);
		if (status < 0) {
		    CTRACE(stderr, "HTTPAccess.. Unable to forward body\n");
		    return HTInetStatus("send");
		}
		remain -= i;
		i = remain;
	    }
	}

	/*
	** Load results directly to client
	*/
	HTProxyBytes = HTCopy(s, request->output_stream);
	(*request->output_stream->isa->free)(request->output_stream);

	return HT_LOADED;
    }
    else {	/* read response */

	HTFormat format_in;		/* Format arriving in the message */
	HTInputSocket *isoc = HTInputSocket_new(s);
	char * status_line = HTInputSocket_getStatusLine(isoc);

/* Kludge to trap binary responses from illegal HTTP0.9 servers.
** First time we have enough, look at the stub in ASCII
** and get out of here if it doesn't look right.
**
** We also check for characters above 128 in the first few bytes, and
** if we find them we forget the html default.
**
** Bugs: A HTTP0.9 server returning a document starting "HTTP/"
**	will be taken as a HTTP 1.0 server.  Failure.
**	An HTTP 0.9 server returning a binary document with
**	characters < 128 will be read as ASCII.
*/

	/* If HTTP 0 response, then DO NOT CACHE (Henrik 14/02-94) */
	if (!status_line) {	
	    if (HTInputSocket_seemsBinary(isoc)) {
		format_in = HTAtom_for("www/unknown");
	    }
	    else {
		format_in = WWW_HTML;
	    }
	    cache_http = NO;	/* Do not cache */
	    goto copy;
	} /* end kludge */

	if (status_line) {	/* Decode full HTTP response */
	    /*
	    ** We now have a terminated server status line, and we have
	    ** checked that it is most probably a legal one.  Parse it.
	    */
	    char server_version[VERSION_LENGTH+1];
	    int server_status;

	    if (TRACE)
		fprintf(stderr, "HTTP Status Line: Rx: %.70s\n", status_line);
    
	    sscanf(status_line, "%20s%d", server_version, &server_status);

	    format_in = HTAtom_for("www/mime");
    
	    switch (server_status / 100) {

	      default:		/* bad number */
		HTAlert("Unknown status reply from server!");
		break;
		    
	      case 3:		/* Various forms of redirection */
		HTAlert(
	    "Redirection response from server is not handled by this client");
		break;
		    
	      case 4:		/* Access Authorization problem */
#ifdef ACCESS_AUTH
		switch (server_status) {
		  case 401:
		    parse_401_headers(request, isoc);

		    if (TRACE) fprintf(stderr, "%s %d %s\n",
				       "HTTP: close socket", s,
				       "to retry with Access Authorization");
		    if (HTAA_retryWithAuth(request, HTLoadHTTP)) {
			status = HT_LOADED;/* @@ THIS ONLY WORKS ON LINEMODE */
			goto clean_up;
		    }
		    /* else falltrough */
		  default:
		    {
			char *p1 = HTParse(gate ? gate : arg, "",
					   PARSE_HOST);
			char * message;

			if (!(message = (char*)malloc(strlen(status_line) +
						      strlen(p1) + 100)))
			    outofmem(__FILE__, "HTTP 4xx status");
			sprintf(message,
				"HTTP server at %s replies:\n%s\n\n%s\n",
				p1, status_line,
				((server_status == 401) 
				 ? "Access Authorization package giving up.\n"
				 : ""));
			status = HTLoadError(request, server_status, message);
			free(message);
			free(p1);
			goto clean_up;
		    }
		} /* switch */
		goto clean_up;
		break;
#else
		/* case 4 without Access Authorization falls through */
		/* to case 5 (previously "I think I goofed").  -- AL */
#endif /* ACCESS_AUTH */

	      case 5:		/* I think you goofed */
		{
		    char *p1 = HTParse(gate ? gate : arg, "", PARSE_HOST);
		    char * message = (char*)malloc(strlen(status_line) + 
						   strlen(p1) + 100);
		    if (!message) outofmem(__FILE__, "HTTP 5xx status");
		    sprintf(message,
			    "HTTP server at %s replies:\n%s", p1, status_line);
		    status = HTLoadError(request, server_status, message);
		    free(message);
		    free(p1);
		    goto clean_up;
		}
		break;
		    
	      case 2:		/* Good: Got MIME object */
		break;
    
	    } /* switch on response code */
	    
	} /* Full HTTP reply */
	    
    
/*	Set up the stream stack to handle the body of the message
**
**	In the special case of user asking for source and the message
**	being in MIME, we force the MIME decoding to occur, as it is really
**	HTTP decoding.  If the user really wants the HTTP headers, he
**	can ask for them as www/mime.
*/

copy:

	if ((format_in == WWW_MIME)
	    && (request->output_format == WWW_SOURCE)) {
	    target = HTMIMEConvert(request, NULL, format_in,
	    request->output_format, request->output_stream);
	} else {
	    target = HTStreamStack(format_in, request);
	}
	
	if (!target) {
	    char buffer[1024];	/* @@@@@@@@ */
	    sprintf(buffer, "Sorry, no known way of converting %s to %s.",
		    HTAtom_name(format_in), HTAtom_name(request->output_format));
	    fprintf(stderr, "HTTP: %s", buffer);
	    status = HTLoadError(request, 501, buffer);
	    goto clean_up;
	}
    
        /* @@ Bug: The decision of whether or not to cache should also be
	** made contingent on a IP address match or non match.
	*/
        if (cache_http) {
	    target = HTTee(target, HTCacheWriter(request, NULL, format_in,
						 request->output_format,
						 request->output_stream));
	}
	
/*	Push the data down the stream
**	We have to remember the end of the first buffer we just read
*/
	if (format_in == WWW_HTML)  {  
	    target = HTNetToText(target);	/* Pipe through CR stripper */
	}

	(*target->isa->put_block)(target,
				  isoc->input_pointer,
				  isoc->input_limit - isoc->input_pointer);

	HTCopy(s, target);
	    
	(*target->isa->free)(target);
	status = HT_LOADED;
    
/*	Clean up
*/
	
clean_up: 
	if (TRACE) fprintf(stderr, "HTTP: close socket %d.\n", s);
	(void) NETCLOSE(s);
	if (isoc)
	    HTInputSocket_free(isoc);	    
	if (status_line)
	    free(status_line);		/* Leak fix Henrik 18/02-94 */
	return status;			/* Good return  */
    
    } /* read response */
} /* load HTTP */

/*	Protocol descriptor
*/

GLOBALDEF PUBLIC HTProtocol HTTP = { "http", HTLoadHTTP, 0, 0 };

