/*	HyperText73 Tranfer Protocol	- Client implementation		HTTP.c
**	==========================
**
**	This module implments the HTTP protocol
**
** History:
**    < May 24 94 ??	Unknown - but obviously written
**	May 24 94 HF	Made reentrent and cleaned up a bit. Implemented
**			Forward, redirection, error handling and referer field
**	 8 Jul 94  FM	Insulate free() from _free structure element.
**
*/

#define HTTP_VERSION	"HTTP/1.0"
#define HTTP2				      /* Version is greater than 0.9 */
#define VERSION_LENGTH 		20    /* Number of chars in protocol version */

/* Uses: */
#include "HTParse.h"
#include "HTUtils.h"

/* System dependent stuff */
#include "tcp.h"

/* Library Includes */
#include "HTTCP.h"
#include "HTFormat.h"
#include "HTAlert.h"
#include "HTMIME.h"
#include "HTML.h"		/* SCW */
#include "HTInit.h"		/* SCW */
#include "HTAccess.h"		/* HTRequest */
#include "HTAABrow.h"		/* Access Authorization */
#include "HTTee.h"		/* Tee off a cache stream */
#include "HTFWriter.h"		/* Write to cache file */
#include "HTError.h"
#include "HTChunk.h"
#include "HTTP.h"					       /* Implements */

/* Macros and other defines */
#define PUTBLOCK(b, l)	(*target->isa->put_block)(target, b, l)
#define PUTS(s)		(*target->isa->put_string)(target, s)
#define FREE_TARGET	(*target->isa->_free)(target)

struct _HTStream {
	HTStreamClass * isa;		/* all we need to know */
};

/* Globals */
extern char * HTAppName;		  /* Application name: please supply */
extern char * HTAppVersion;	       /* Application version: please supply */
PUBLIC int  HTMaxRedirections = 10;	       /* Max number of redirections */
PUBLIC BOOL HTEnableFrom = NO;			      /* Enable From header? */

#ifdef OLD_CODE
PUBLIC long HTProxyBytes = 0;	/* Number of bytes transferred thru proxy */
#endif

extern BOOL using_proxy;	/* are we using a proxy gateway? */
PUBLIC char * HTProxyHeaders = NULL;	/* Headers to pass as-is */

/* Type definitions and global variables etc. local to this module */
/* This is the local definition of HTRequest->net_info */
typedef enum _HTTPState {
    HTTP_ERROR   = -2,
    HTTP_FAILURE = -1,
    HTTP_IDLE    = 0,
    HTTP_BEGIN,
    HTTP_INTERRUPTED,
    HTTP_CONNECTED,
    HTTP_SEND_REQUEST,
    HTTP_GOT_RESPONSE
} HTTPState;

typedef struct _http_info {
    int			sockfd;		  /* Socket number for communication */
    HTInputSocket *	isoc;				     /* Input buffer */
    int 		addressCount; /* No. of attempts if multi-homed host */
    BOOL		CRLFdotCRLF;	       /* Transmission end like this */
    HTRequest *		request;			/* Request structure */

    HTTPState		state;			  /* State of the connection */
} http_info;

/* ------------------------------------------------------------------------- */

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


/*                                                              HTTPCleanup
**
**      This function closes the connection and frees memory.
**
**      Returns 0 on OK, else -1
*/
PRIVATE int HTTPCleanup ARGS1(http_info *, http)
{
    int status = 0;
    if (!http) {
	if (TRACE) fprintf(stderr, "HTTPCleanup. Bad argument!\n");
	status = -1;
    } else {
	if (http->sockfd >= 0) {
	    if (TRACE) fprintf(stderr, "HTTP........ Closing socket %d\n",
			       http->sockfd);
	    if ((status = NETCLOSE(http->sockfd)) < 0)
		HTErrorSysAdd(http->request, ERR_FATAL, NO, "NETCLOSE");
	}
    }	
    free(http);
    return status;
}


/*                                                              HTTPSendRequest
**
**      This function composes and sends a request to the connected server
**      specified.
**
**      Returns 0 on OK, else -1 but does NOT close the connection
*/
PRIVATE int HTTPSendRequest ARGS3(HTRequest *, request,
				  http_info *, http, char *, url)
{
    int status = 0;
    BOOL extensions = YES;			  /* Assume good HTTP server */
    HTChunk *command = HTChunkCreate(2048);		/* The whole command */
    if (request->method != METHOD_INVALID) {
	HTChunkPuts(command, HTMethod_name(request->method));
	HTChunkPutc(command, ' ');
    }
    else
	HTChunkPuts(command, "GET ");

    /* if we are using a proxy gateway don't copy in the first slash
     ** of say: /gopher://a;lkdjfl;ajdf;lkj/;aldk/adflj
     ** so that just gohper://.... is sent. */
    {
	char *p1 = HTParse(url, "", PARSE_PATH|PARSE_PUNCTUATION);
	if (using_proxy)
	    HTChunkPuts(command, p1+1);
	else
	    HTChunkPuts(command, p1);
	free(p1);
    }

#ifdef HTTP2
    if (extensions) {
	HTChunkPutc(command, ' ');
	HTChunkPuts(command, HTTP_VERSION);
    }
#endif
    HTChunkPutc(command, CR);			     /* CR LF, as in rfc 977 */
    HTChunkPutc(command, LF);
    
    if (extensions && HTImProxy && HTProxyHeaders) {
	HTChunkPuts(command, HTProxyHeaders);
    } else if (extensions) {
	char line[256];    /*@@@@ */

	/* If no conversion list, then put it up, but leave initialization
	   to the client */
	if (!HTConversions)
	    HTConversions = HTList_new();

	/* Run through both lists and generate `accept' lines */
	{
	    int i;
	    HTList *conversions[2];
	    conversions[0] = HTConversions;
	    conversions[1] = request->conversions;
	    
	    for (i=0; i<2; i++) {
		HTList *cur = conversions[i];
		HTPresentation *pres;
		while ((pres = (HTPresentation *) HTList_nextObject(cur))) {
		    if (pres->rep_out == WWW_PRESENT) {
			if (pres->quality != 1.0) {
			    sprintf(line, "Accept: %s; q=%.3f%c%c",
				    HTAtom_name(pres->rep),
				    pres->quality, CR, LF);
			} else {
			    sprintf(line, "Accept: %s%c%c",
				    HTAtom_name(pres->rep), CR, LF);
			}
			HTChunkPuts(command, line);
		    }
		}
	    }
	}

	/* Put out referer field if any parent */
	if (request->parentAnchor) {
	    char *this = HTAnchor_address((HTAnchor *) request->anchor);
	    char *parent = HTAnchor_address((HTAnchor *)request->parentAnchor);
	    char *relative = HTParse(parent, this,
                PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
	    if (relative && *relative) {
		sprintf(line, "Referer: %s%c%c", parent, CR, LF);
		HTChunkPuts(command, line);
	    }
	    free(this);
	    free(parent);
	    free(relative);
	}

	/* Put out from field if enabled by client */
	if (HTEnableFrom) {
	    CONST char *mailaddress = HTGetMailAddress();
	    if (mailaddress != NULL) {
		sprintf(line, "From: %s%c%c", mailaddress, CR, LF);
		HTChunkPuts(command, line);
	    }
	}
    
	/* Put out user-agent */
	sprintf(line, "User-Agent: %s/%s  libwww/%s%c%c",
		HTAppName ? HTAppName : "unknown",
		HTAppVersion ? HTAppVersion : "0.0",
		HTLibraryVersion, CR, LF);
	HTChunkPuts(command, line);

	/* Put out authorization */
	if (request->authorization != NULL) {
	    HTChunkPuts(command, "Authorization: ");
	    HTChunkPuts(command, request->authorization);
	    HTChunkPutc(command, CR);
	    HTChunkPutc(command, LF);
	}
    }
    HTChunkPutc(command, CR);			   /* Blank line means "end" */
    HTChunkPutc(command, LF);
    HTChunkTerminate(command);
    if (TRACE) fprintf(stderr, "HTTP Tx..... %s", command->data);
    
    /* Translate into ASCII if necessary */
#ifdef NOT_ASCII
    {
	char * p;
	for(p = command->data; *p; p++) {
	    *p = TOASCII(*p);
	}
    }
#endif
    
    /* Now, we are ready for sending the request */
    if ((status = NETWRITE(http->sockfd, command->data, command->size-1))<0) {
	if (TRACE) fprintf(stderr, "HTTP Tx..... Error sending command\n");
	HTErrorSysAdd(request, ERR_FATAL, NO, "NETWRITE");
	if (status != HT_INTERRUPTED) {
	    char *unescaped = NULL;
	    StrAllocCopy(unescaped, url);
	    HTUnEscape(unescaped);
	    HTErrorAdd(request, ERR_FATAL, NO, HTERR_INTERNAL,
		       (void *) unescaped, (int) strlen(unescaped),
		       "HTTPSendRequest");
	    free(unescaped);
	}
    }
    HTChunkFree(command);
    return status;
}


/*                                                              HTTPGetBody
**
**      Put up a streamstack and read the body from the socket.
**	In the special case of user asking for source and the message
**	being in MIME, we force the MIME decoding to occur, as it is really
**	HTTP decoding.  If the user really wants the HTTP headers, he
**	can ask for them as www/mime.
**
**	Returns < 0 on error, else HT_LOADED
*/
PRIVATE int HTTPGetBody ARGS4(HTRequest *, request, http_info *, http,
			      HTFormat, format_in, BOOL, use_cache)
{
    int status = -1;
    HTStream  *target = NULL;				 /* Unconverted data */
    if (format_in == WWW_MIME && request->output_format == WWW_SOURCE) {
	target = HTMIMEConvert(request, NULL, format_in,
			       request->output_format,
			       request->output_stream);
    } else
	target = HTStreamStack(format_in, request, NO);
    if (target) {
	
	/* @@ Bug: The decision of whether or not to cache should also
	   be made contingent on a IP address match or non match. */
	
	if (HTCacheDir && use_cache) {
	    target = HTTee(target,
			   HTCacheWriter(request, NULL, format_in,
					 request->output_format,
					 request->output_stream));
	}
	
	/* Push the data down the stream remembering the end of the
	   first buffer we just read */
	if (format_in == WWW_HTML)
	    target = HTNetToText(target);/* Pipe through CR stripper */
	
	PUTBLOCK(http->isoc->input_pointer,
		 http->isoc->input_limit - http->isoc->input_pointer);
	HTCopy(http->sockfd, target);		     /* USE RETURN AS STATUS */
	FREE_TARGET;
	status = HT_LOADED;
    }
    return status;
}


/*                                                              HTTPRedirect
**
**      Reads the response from a 3xx server status code. Only the first line
**	is read. The format expected is
**
**	Location: <url> String CrLf	OR	URI: <url> String CrLf 
**
**	The comment string is ignored!
**
**	NOTE: THIS IS NOT IN CORRESPONDANCE WITH THE SPECS!!!
**
**	Returns new anchor on success else NULL
*/
PRIVATE HTAnchor *HTTPRedirect ARGS3(HTRequest *, request,
				     http_info *, http, int, code)
{
    BOOL found = NO;
    HTAnchor *anchor = NULL;				       /* New anchor */
    char *line;
    if (TRACE)
	fprintf(stderr, "Redirection. Looking for URL's\n");
    while ((line = HTInputSocket_getUnfoldedLine(http->isoc)) != NULL) {
	char *strptr = line;
	if (*strptr) {
	    while (*strptr && *strptr == ' ')	      /* Skip leading spaces */
		strptr++;
	    if (!strncasecomp(strptr, "location:", 9) ||
		!strncasecomp(strptr, "uri:", 4)) {
		char *url = strchr(strptr, ' ');
		char *comment;
		while (*url && *url == ' ')	      /* Skip leading spaces */
		    url++;
		if ((comment = strchr(url, ' ')) != NULL)
		    *comment = '\0';			/* Skip any comments */
		if (code == 301)
		    HTErrorAdd(request, ERR_INFO, NO, HTERR_MOVED,
			       (void *) url, (int) strlen(url),
			       "HTTPRedirect");
		else if (code == 302)
		    HTErrorAdd(request, ERR_INFO, NO, HTERR_FOUND,
			       (void *) url, (int) strlen(url),
			       "HTTPRedirect");
		else {
		    if (TRACE)
			fprintf(stderr,
				"Redirection. Weird, should never happen\n");
		}

		/* Now use the new anchor instead of the old one */
		anchor = HTAnchor_findAddress(url);
		found = YES;
		FREE(line);
		break;
	    }
	}
	free(line);
    }

    if (!found) {
	int length = (int) strlen(line);
	HTErrorAdd(request, ERR_FATAL, NO, HTERR_BAD_REPLY,
		   (void *) line, length < 50 ? length : 50, "HTTPRedirect");
    }
    return anchor;
}


/*		Load Document from HTTP Server			HTLoadHTTP()
**		==============================
**
**	Given a hypertext address, this routine loads a document.
**
** On entry,
**      request		This is the request structure
** On exit,
**	returns		<0		Error has occured
**			HT_LOADED	if return status 200 OK
**			HT_NO_DATA	if return status 204 No Response
*/
PUBLIC int HTLoadHTTP ARGS1 (HTRequest *, request)
{
    char *url;
    int	status = -1;				       /* The current status */
    http_info *http;			    /* Specific protocol information */

    if (!request || !request->anchor) {
        if (TRACE) fprintf(stderr, "HTLoadHTTP.. Bad argument\n");
        return -1;
    }
    url = HTAnchor_physical(request->anchor);
    if (TRACE) fprintf(stderr, "HTTP........ Looking for `%s\'\n", url);

    /* Initiate a new http structure and bind to request structure */
    /* this is actually state HTTP_BEGIN, but it can't be in the state  */
    /* machine as we need the structure first. */
    if ((http = (http_info *) calloc(1, sizeof(http_info))) == NULL)
	outofmem(__FILE__, "HTLoadHTTP");
    http->sockfd = -1;			  	    /* Invalid socket number */
    http->request = request;
    http->state = HTTP_IDLE;
    request->net_info = (HTNetInfo *) http;
    
    /*
     ** Compose authorization information (this was moved here
     ** from after the making of the connection so that the connection
     ** wouldn't have to wait while prompting username and password
     ** from the user).				-- AL 13.10.93
     */
    HTAA_composeAuth(request);
    if (TRACE) {
	if (request->authorization)
	    fprintf(stderr, "HTTP........ Sending Authorization: %s\n",
		    request->authorization);
	else
	    fprintf(stderr, "HTTP........ Not sending authorization (yet)\n");
    }

    /* Now let's set up a connection and input buffer */
    if ((status = HTDoConnect((HTNetInfo *) http, url, TCP_PORT,
			      NULL, NO)) < 0) {
	if (TRACE)
	    fprintf(stderr, "HTTP........ Connection not established\n");
	if (status != HT_INTERRUPTED) {
	    char *unescaped = NULL;
	    StrAllocCopy(unescaped, url);
	    HTUnEscape(unescaped);
	    HTErrorAdd(request, ERR_FATAL, NO, HTERR_INTERNAL,
		       (void *) unescaped, (int) strlen(unescaped),
		       "HTLoadHTTP");
	    free(unescaped);
	}
	HTTPCleanup(http);
	return status;
    }
    if (TRACE) fprintf(stderr, "HTTP........ Connected, socket %d\n",
		       http->sockfd);

    /* Compose the request and send it */
    if ((status = HTTPSendRequest(request, http, url)) < 0) {
	HTTPCleanup(http);
	return status;
    }    

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

    CTRACE(stderr, "Waiting..... for response\n");

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
		int status = NETWRITE(http->sockfd, buf, i);
		if (status < 0) {
		    CTRACE(stderr, "HTTPAccess.. Unable to forward body\n");
		    HTErrorSysAdd(request, ERR_FATAL, NO, "NETWRITE");
		    HTTPCleanup(http);
		    return status;
		}
		remain -= i;
		i = remain;
	    }
	}

	/*
	** Load results directly to client
	*/
	HTCopy(http->sockfd, request->output_stream);
	(*request->output_stream->isa->_free)(request->output_stream);
	HTTPCleanup(http);
	return HT_LOADED;
    } else {						    /* read response */

	HTFormat format_in;		/* Format arriving in the message */
	char *status_line = NULL;
	http->isoc = HTInputSocket_new(http->sockfd);
	status_line = HTInputSocket_getStatusLine(http->isoc);

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
	    if (PROT_TRACE)
		fprintf(stderr,	
			"HTTP........ This seems like a HTTP 0.9 server\n");
	    if (HTInputSocket_seemsBinary(http->isoc)) {
		format_in = HTAtom_for("www/unknown");
	    } else {
		format_in = WWW_HTML;
	    }
	    status = HTTPGetBody(request, http, format_in, NO);
	} else {
	    /*
	    ** We now have a terminated server status line, and we have
	    ** checked that it is most probably a legal one.  Parse it.
	    */
	    char server_version[VERSION_LENGTH+1];
	    int server_status;

	    if (TRACE)
		fprintf(stderr, "HTTP Rx..... `%.70s\'\n", status_line);
	    {
		char formatstr[20];
		sprintf(formatstr, "%%%ds%%d", VERSION_LENGTH);
		if (sscanf(status_line, formatstr, server_version,
			   &server_status) < 2) {
		    int length = (int) strlen(status_line);
		    HTErrorAdd(request, ERR_FATAL, NO, HTERR_BAD_REPLY,
			       (void *) status_line, length < 50 ? length : 50,
			       "HTLoadHTTP");
		    HTInputSocket_free(http->isoc);
		    free(http);
		    free(status_line);
		    return -1;				     /* Bad response */
		}
		*(server_version+VERSION_LENGTH) = '\0';
	    }
	    format_in = HTAtom_for("www/mime");
    
	    /* Big switch for all response codes */
	    switch (server_status/100) {

	      case 2:		/* Good: Got MIME object */
		switch (server_status) {
		  case 204:				      /* No response */
		    HTErrorAdd(request, ERR_INFO, NO, HTERR_NO_RESPONSE,
			       NULL, 0, "HTLoadHTTP");
		    status = HT_NO_DATA;
		    break;			       /* Don't get any body */
		  case 203:					  /* Partial */
		    HTErrorAdd(request, ERR_INFO, NO, HTERR_PARTIAL,
			       NULL, 0, "HTLoadHTTP");
		    /* Drop through to 200 as we still have to get the body */
		  case 200:
		    status = HTTPGetBody(request, http, format_in, YES);
		    break;
		  default:
		    {
			int length = (int) strlen(status_line);
			HTErrorAdd(request, ERR_FATAL, NO, HTERR_BAD_REPLY,
				   (void *) status_line, length < 50 ?
				   length : 50, "HTLoadHTTP");
		    }
		    status = -1;
		    break;
		}
		break;

	      case 3:		/* Various forms of redirection */
		switch (server_status) {
		  case 301:					    /* Moved */
		  case 302:					    /* Found */
		    {
			HTParentAnchor *anchor;
			if ((anchor = (HTParentAnchor *)
			     HTTPRedirect(request, http,
					  server_status)) != NULL) {
			    free(status_line);
			    HTInputSocket_free(http->isoc);
			    HTTPCleanup(http);
			    request->redirections++;

			    /* If we have not reached the roof for redirects
			       then call HTLoadAnchor recursively but keep
			       the error_stack so that the user knows what
			       is going on */
			    if (request->redirections < HTMaxRedirections) {
				if (HTLoadAnchorRecursive((HTAnchor *) anchor,
							  request) == YES)
				    return HT_LOADED;
				else
				    return -1;
			    } else {				
				HTErrorAdd(request, ERR_FATAL, NO, 
					   HTERR_MAX_REDIRECT, NULL, 0,
					   "HTTLoadHTTP");
				return -1;
			    }
			}
		    }
		    break;
		  case 303:					   /* Method */
		    HTAlert("This client doesn't support automatic redirection of type `Method'");
		    status = -1;
		    break;
		  case 304:			       /* Not modified Since */
		    {
			char *unescaped = NULL;
			StrAllocCopy(unescaped, url);
			HTUnEscape(unescaped);
			HTErrorAdd(request, ERR_INFO, NO,
				   HTERR_NOT_MODIFIED, (void *) unescaped,
				   (int) strlen(unescaped), "HTLoadHTTP");
			free(unescaped);
		    }
		    status = HT_LOADED;
		    break;

		  default:
		    {
			int length = (int) strlen(status_line);
			HTErrorAdd(request, ERR_FATAL, NO, HTERR_BAD_REPLY,
				   (void *) status_line, length < 50 ?
				   length : 50, "HTLoadHTTP");
		    }
		    status = -1;
		    break;
		}
		break;
		    
	      case 4:		/* Access Authorization problem */
		switch (server_status) {
		  case 400:
		    {
			char *unescaped = NULL;
			StrAllocCopy(unescaped, url);
			HTUnEscape(unescaped);
			HTErrorAdd(request, ERR_FATAL, NO,
				   HTERR_BAD_REQUEST, (void *) unescaped,
				   (int) strlen(unescaped), "HTLoadHTTP");
			free(unescaped);
		    }
		    status = -1;
		    break;
		  case 401:
		    parse_401_headers(request, http->isoc);
		    if (TRACE) fprintf(stderr, "HTTP........ Closing socket %d to retry with Access Autorization\n", http->sockfd);
		    if ((status = NETCLOSE(http->sockfd)) < 0) {
			HTErrorSysAdd(http->request, ERR_FATAL, NO,"NETCLOSE");
			break;
		    }
		    http->sockfd = -1;
		    if (HTAA_retryWithAuth(request, HTLoadHTTP)) {
			status = HT_LOADED;/* @@ THIS ONLY WORKS ON LINEMODE */
			break;
		    } else {
			char *unescaped = NULL;
			StrAllocCopy(unescaped, url);
			HTUnEscape(unescaped);
			HTErrorAdd(request, ERR_FATAL, NO, HTERR_UNAUTHORIZED,
				   (void *) unescaped,
				   (int) strlen(unescaped), "HTLoadHTTP");
			free(unescaped);
			status = -1;
		    }
		    break;
		  case 402:
		    {
			char *unescaped = NULL;
			StrAllocCopy(unescaped, url);
			HTUnEscape(unescaped);
			HTErrorAdd(request, ERR_FATAL, NO,
				   HTERR_PAYMENT_REQUIRED, (void *) unescaped,
				   (int) strlen(unescaped), "HTLoadHTTP");
			free(unescaped);
		    }
		    status = -1;
		    break;
		  case 403:
		    {
			char *unescaped = NULL;
			StrAllocCopy(unescaped, url);
			HTUnEscape(unescaped);
			HTErrorAdd(request, ERR_FATAL, NO,
				   HTERR_FORBIDDEN, (void *) unescaped,
				   (int) strlen(unescaped), "HTLoadHTTP");
			free(unescaped);
		    }
		    status = -1;
		    break;
		  case 404:
		    {
			char *unescaped = NULL;
			StrAllocCopy(unescaped, url);
			HTUnEscape(unescaped);
			HTErrorAdd(request, ERR_FATAL, NO,
				   HTERR_NOT_FOUND, (void *) unescaped,
				   (int) strlen(unescaped), "HTLoadHTTP");
			free(unescaped);
		    }
		    status = -1;
		    break;
		  default:
		    {
			char *unescaped = NULL;
			StrAllocCopy(unescaped, url);
			HTUnEscape(unescaped);
			HTErrorAdd(request, ERR_FATAL, NO, HTERR_BAD_REPLY,
				   (void *) unescaped,
				   (int) strlen(unescaped), "HTLoadHTTP");
			free(unescaped);
#ifdef OLD_CODE
			char *p1 = HTParse(url, "", PARSE_HOST);
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
#endif /* OLD_CODE */
		    }
		    status = -1;
		    break;
		}
		break;

	      case 5:		/* I think you goofed */
		{
		    char *unescaped = NULL;
		    StrAllocCopy(unescaped, url);
		    HTUnEscape(unescaped);
		    HTErrorAdd(request, ERR_FATAL, NO, HTERR_INTERNAL,
			       (void *) unescaped, (int) strlen(unescaped),
			       "HTLoadHTTP");
		    free(unescaped);
#ifdef OLD_CODE
		    char *p1 = HTParse(url, "", PARSE_HOST);
		    char * message = (char*)malloc(strlen(status_line) + 
						   strlen(p1) + 100);
		    if (!message) outofmem(__FILE__, "HTTP 5xx status");
		    sprintf(message,
			    "HTTP server at %s replies:\n%s", p1, status_line);
		    status = HTLoadError(request, server_status, message);
		    free(message);
		    free(p1);
#endif
		}
		status = -1;
		break;

	      default:					       /* bad number */
		{
		    int length = (int) strlen(status_line);
		    HTErrorAdd(request, ERR_FATAL, NO, HTERR_BAD_REPLY,
			       (void *) status_line, length < 50 ? length : 50,
			       "HTLoadHTTP");
		}
		status = -1;
		break;
	    }
	    FREE(status_line);			 /* Leak fix Henrik 18/02-94 */
	}

	/* Close the socket and free memory */
	HTInputSocket_free(http->isoc);
	HTTPCleanup(http);
	return status;			/* Good return  */    
    }
}

/*	Protocol descriptor
*/

GLOBALDEF PUBLIC HTProtocol HTTP = { "http", HTLoadHTTP, 0, 0 };



