/*
**	DEFAULT MIME HEADER PARSERS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This module contains the default MIME header parsers for the MIME
**	parser in HTMIME.c. They are all initialized at run time and can hence
**	be replaced or extended with your own set.
**
** History:
**	   Jun 96  HFN	Written
**         Dec 98  JKO  Added support for message-digest authentication
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTHeader.h"
#include "HTTPUtil.h"
#include "HTMIMImp.h"					 /* Implemented here */

/* ------------------------------------------------------------------------- */

PUBLIC int HTMIME_accept (HTRequest * request, HTResponse * response,
			  char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_acceptCharset (HTRequest * request, HTResponse * response,
				 char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_acceptEncoding (HTRequest * request, HTResponse * response,
				  char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_acceptLanguage (HTRequest * request, HTResponse * response,
				  char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_acceptRanges (HTRequest * request, HTResponse * response,
				char * token, char * value)
{
    if (value) {
	HTNet * net = HTRequest_net(request);
	HTHost * host = HTNet_host(net);
	HTHost_setRangeUnits(host, value);
    }
    return HT_OK;
}

PUBLIC int HTMIME_authenticate (HTRequest * request, HTResponse * response,
				char * token, char * value)
{    
    char * scheme = HTNextField(&value);
    if (scheme) {
	HTResponse_addChallenge(response, scheme, value);
	HTResponse_setScheme(response, scheme);
    }
    return HT_OK;
}

PUBLIC int HTMIME_authenticationInfo (HTRequest * request, 
		 		      HTResponse * response,
                                      char * token, char * value)
{
  if (token && value) {
    HTResponse_addChallenge(response, token, value);
    HTResponse_setScheme(response, "digest");
  }
  return HT_OK;
}

PUBLIC int HTMIME_authorization (HTRequest * request, HTResponse * response,
				 char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_cacheControl (HTRequest * request, HTResponse * response,
				char * token, char * value)
{
    /*
    **  Walk through the set of cache-control directives and add them to the
    **  response association list for cache control directives
    */
    char * name_val;
    while ((name_val = HTNextPair(&value)) != NULL) {
	char * name = HTNextField(&name_val);
	char * val = HTNextField(&name_val);
	if (name) HTResponse_addCacheControl(response, name, val ? val : "");
    }
    return HT_OK;
}

PUBLIC int HTMIME_connection (HTRequest * request, HTResponse * response,
			      char * token, char * value)
{
    /*
    **  Walk through the set of connection directives and add them to the
    **  response association list for connection directives
    */
    char * name_val;
    while ((name_val = HTNextPair(&value)) != NULL) {
	char * name = HTNextField(&name_val);
	char * val = HTNextField(&name_val);

	/*
	**  If we have a name then look if it is concerning persistent
	**  connections. If so, then we handle it here, otherwise we leave it
	**  to somebody else by simply adding it to the list of connection
	**  tokens.
	*/
	if (name) {
	    HTNet * net = HTRequest_net(request);
	    HTHost * host = HTNet_host(net);
	    if (!strcasecomp(name, "close")) {			 /* HTTP/1.1 */
		HTTRACE(STREAM_TRACE, "MIMEParser.. Close received...\n");
		HTHost_setCloseNotification(host, YES);
	    } else if (!strcasecomp(name, "keep-alive")) {       /* HTTP/1.0 */

		/*
		**  In case this is an HTTP/1.1 server sending keep-alive then
		**  ignore it.
		*/
		if (HTHost_version(host) < HTTP_11) {
		    HTNet_setPersistent(net, YES, HT_TP_SINGLE);
		    HTTRACE(STREAM_TRACE, "MIMEParser.. HTTP/1.0 Keep Alive\n");
		} else 
		    HTTRACE(STREAM_TRACE, "MIMEParser.. HTTP/1.0 Keep Alive ignored\n");
	    } else
		HTResponse_addConnection(response, name, val ? val : "");
	}
    }
    return HT_OK;
}

PUBLIC int HTMIME_contentEncoding (HTRequest * request, HTResponse * response,
				   char * token, char * value)
{
    char * field;
    while ((field = HTNextField(&value)) != NULL) {
        char * lc = field;
	while ((*lc = TOLOWER(*lc))) lc++;
	HTResponse_addEncoding(response, HTAtom_for(field));
    }
    return HT_OK;
}

PUBLIC int HTMIME_contentLength (HTRequest * request, HTResponse * response,
				 char * token, char * value)
{
    char * field;
    if ((field = HTNextField(&value)) != NULL)
        HTResponse_setLength(response, atol(field));
    return HT_OK;
}

PUBLIC int HTMIME_contentRange (HTRequest * request, HTResponse * response,
				char * token, char * value)
{
    char * field;
    if ((field = HTNextField(&value)))
	HTResponse_addRange(response, field, value);
    return HT_OK;
}

PUBLIC int HTMIME_contentTransferEncoding (HTRequest * request, HTResponse * response,
					   char * token, char * value)
{
    char * field;
    if ((field = HTNextField(&value)) != NULL) {
        char *lc = field;
	while ((*lc = TOLOWER(*lc))) lc++;
	HTResponse_setContentTransferEncoding(response, HTAtom_for(field));
    }
    return HT_OK;
}

PUBLIC int HTMIME_contentType (HTRequest * request, HTResponse * response,
			       char * token, char * value)
{
    char * field;
    if ((field = HTNextField(&value)) != NULL) {

	/* Get the Content-Type */
        char *lc = field;
	while ((*lc = TOLOWER(*lc))) lc++; 
	HTResponse_setFormat(response, HTAtom_for(field));

	/* Get all the parameters to the Content-Type */
	{
	    char * param;
	    while ((field = HTNextField(&value)) != NULL &&
		   (param = HTNextField(&value)) != NULL) {
		lc = field;
		while ((*lc = TOLOWER(*lc))) lc++;
		lc = param;
		while ((*lc = TOLOWER(*lc))) lc++;
		HTResponse_addFormatParam(response, field, param);
	    }
	}
    }
    return HT_OK;
}

#if 0
PRIVATE int HTFindInt(char * haystack, char * needle, int deflt)
{
    char * start = strstr(haystack, needle);
    int value = deflt;
    if (start != NULL) {
	start += strlen(needle);
	while isspace(*start) start++;
	if (isdigit(*start)) {
	    char * end = start + 1;
	    char save;
	    while (isdigit(*end)) end++;
	    save = *end;
	    *end = 0;
	    value = atoi(start);
	    *end = save;
	}
    }
    return value;
}
#endif

PUBLIC int HTMIME_keepAlive (HTRequest * request, HTResponse * response,
			     char * token, char * value)
{
    char * name_val;
    HTNet * net = HTRequest_net(request);
    HTHost * host = HTNet_host(net);
    while ((name_val = HTNextPair(&value)) != NULL) {
	char * name = HTNextField(&name_val);
	char * val = HTNextField(&name_val);
	if (!strcasecomp(name, "max") && val) {
	    int max = atoi(val);
	    HTTRACE(STREAM_TRACE, "MIMEParser.. Max %d requests pr connection\n" _ max);
	    HTHost_setReqsPerConnection(host, max);
	} else if (!strcasecomp(name, "timeout") && val) {
	    int timeout = atoi(val);
	    HTTRACE(STREAM_TRACE, "MIMEParser.. Timeout after %d secs\n" _ timeout);
	}
    }
    return HT_OK;
}

PUBLIC int HTMIME_link (HTRequest * request, HTResponse * response,
			char * token, char * value)
{
    char * element;
    HTParentAnchor * me = HTRequest_anchor(request);
    while ((element = HTNextElement(&value))) {
	char * param_pair;
	char * uri = HTNextField(&element);
	HTChildAnchor * child_dest = HTAnchor_findChildAndLink(me, NULL, uri, NULL);
	HTParentAnchor * parent_dest =
	    HTAnchor_parent(HTAnchor_followMainLink((HTAnchor *) child_dest));
	if (parent_dest) {
	    while ((param_pair = HTNextPair(&element))) {
		char * name = HTNextField(&param_pair);
		char * val = HTNextField(&param_pair);
		if (name) {
		    if (!strcasecomp(name, "rel") && val && *val) {
			HTTRACE(STREAM_TRACE, "MIMEParser.. Link forward relationship `%s\'\n" _ 
				    val);
			HTLink_add((HTAnchor *) me, (HTAnchor *) parent_dest,
				   (HTLinkType) HTAtom_caseFor(val),
				   METHOD_INVALID);
		    } else if (!strcasecomp(name, "rev") && val && *val) {
			HTTRACE(STREAM_TRACE, "MIMEParser.. Link reverse relationship `%s\'\n" _ 
				    val);
			HTLink_add((HTAnchor *) parent_dest, (HTAnchor *) me,
				   (HTLinkType) HTAtom_caseFor(val),
				   METHOD_INVALID);
		    } else if (!strcasecomp(name, "type") && val && *val) {
			HTTRACE(STREAM_TRACE, "MIMEParser.. Link type `%s\'\n" _ val);
			if (HTAnchor_format(parent_dest) == WWW_UNKNOWN)
			    HTAnchor_setFormat(parent_dest, (HTFormat) HTAtom_caseFor(val));
		    } else
			HTTRACE(STREAM_TRACE, "MIMEParser.. Link unknown `%s\' with value `%s\'\n" _ 
				    name _ val ? val : "<null>");
		}
	    }
	}
    }
    return HT_OK;
}

PUBLIC int HTMIME_location (HTRequest * request, HTResponse * response,
			    char * token, char * value)
{
    HTAnchor * redirection = NULL;
    char * location = HTStrip(value);

    /*
    **  If not absolute URI (Error) then find the base
    */
    if (!HTURL_isAbsolute(location)) {
	char * base = HTAnchor_address((HTAnchor *) HTRequest_anchor(request));
	location = HTParse(location, base, PARSE_ALL);
	redirection = HTAnchor_findAddress(location);
	HT_FREE(base);
	HT_FREE(location);
    } else {
	redirection = HTAnchor_findAddress(location);
    }
    HTResponse_setRedirection(response, redirection);
    return HT_OK;
}

PUBLIC int HTMIME_maxForwards (HTRequest * request, HTResponse * response,
			       char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_messageDigest (HTRequest * request, HTResponse * response,
				 char * token, char * value)
{
    HTResponse_addChallenge(response, "Digest-MessageDigest", value);
    return HT_OK;
}

PUBLIC int HTMIME_pragma (HTRequest * request, HTResponse * response,
			  char * token, char * value)
{
    /*
    **  Walk through the set of pragma directives and search for one that may
    **  affect the cachability of this object
    */
    char * name_val;
    while ((name_val = HTNextPair(&value)) != NULL) {
	char * name = HTNextField(&name_val);
	if (name) {
	    if (!strcasecomp(name, "no-cache")) {
		HTResponse_setCachable(response, HT_NO_CACHE);
		HTTRACE(STREAM_TRACE, "MIMEParser.. No-Cache Pragma\n");
	    }
	}
    }
    return HT_OK;
}

PUBLIC int HTMIME_protocol (HTRequest * request, HTResponse * response,
			    char * token, char * value)
{
    char * param = NULL;
    char * protocol = HTNextSExp(&value, &param);
    if (protocol) {
	HTTRACE(PROT_TRACE, "Protocol.... Name: `%s\', value: `%s\'\n" _ 
		    protocol _ param);
	HTResponse_addProtocol(response, protocol, param);
    }
    return HT_OK;
}

PUBLIC int HTMIME_protocolInfo (HTRequest * request, HTResponse * response,
				char * token, char * value)
{
    char * param = NULL;
    char * info = HTNextSExp(&value, &param);
    if (info) {
	HTTRACE(PROT_TRACE, "Protocol.... Info: `%s\', value: `%s\'\n" _ 
		    info _ param);
	HTResponse_addProtocolInfo(response, info, param);
    }
    return HT_OK;
}

PUBLIC int HTMIME_protocolRequest (HTRequest * request, HTResponse * response,
				   char * token, char * value)
{
    char * param = NULL;
    char * preq = HTNextSExp(&value, &param);
    if (preq) {
	HTTRACE(PROT_TRACE, "Protocol.... Request: `%s\', value: `%s\'\n" _ 
		    preq _ param);
	HTResponse_addProtocolRequest(response, preq, param);
    }
    return HT_OK;
}

PUBLIC int HTMIME_proxyAuthorization (HTRequest * request, HTResponse * response,
				      char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_proxyAuthenticationInfo (HTRequest * request, 
					   HTResponse * response,
					   char * token, char * value)
{
  /* deal here with the next nonce, the qop, and the digest response (mutual
     authentication  */  
  if (token && value) {
    HTResponse_addChallenge(response, token, value);
    HTResponse_setScheme(response, "digest");
  }
  return HT_OK;
}

PUBLIC int HTMIME_public (HTRequest * request, HTResponse * response,
			  char * token, char * value)
{
    char * field;
    HTNet * net = HTRequest_net(request);
    HTHost * host = HTNet_host(net);
    while ((field = HTNextField(&value)) != NULL) {
        HTMethod new_method;
	/* We treat them as case-insensitive! */
	if ((new_method = HTMethod_enum(field)) != METHOD_INVALID)
	    HTHost_appendPublicMethods(host, new_method);
    }
    HTTRACE(STREAM_TRACE, "MIMEParser.. Public methods: %d\n" _ 
		HTHost_publicMethods(host));
    return HT_OK;
}

PUBLIC int HTMIME_range (HTRequest * request, HTResponse * response,
			 char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_referer (HTRequest * request, HTResponse * response,
			   char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_retryAfter (HTRequest * request, HTResponse * response,
			      char * token, char * value)
{
    HTUserProfile * up = HTRequest_userProfile(request);
    HTResponse_setRetryTime(response, HTParseTime(value, up, YES));
    return HT_OK;
}

PUBLIC int HTMIME_server (HTRequest * request, HTResponse * response,
			  char * token, char * value)
{
    char * field;
    HTNet * net = HTRequest_net(request);
    HTHost * host = HTNet_host(net);
    if ((field = HTNextField(&value)) != NULL)
        HTHost_setServer(host, field);
    return HT_OK;
}

PUBLIC int HTMIME_transferEncoding (HTRequest * request, HTResponse * response,
				    char * token, char * value)
{
    char * field;
    while ((field = HTNextField(&value)) != NULL) {
        char * lc = field;
	while ((*lc = TOLOWER(*lc))) lc++;
	HTResponse_addTransfer(response, HTAtom_for(field));
    }
    return HT_OK;
}


PUBLIC int HTMIME_trailer (HTRequest * request, HTResponse * response,
			   char * token, char * value)
{
    /*
    **  Walk through the set of trailer directives and add them to the
    **  response association list for trailer directives
    */
    char * name_val;
    while ((name_val = HTNextPair(&value)) != NULL) {
	char * name = HTNextField(&name_val);
	char * val = HTNextField(&name_val);
	if (name) HTResponse_addTrailer(response, name, val ? val : "");
    }
    return HT_OK;
}

PUBLIC int HTMIME_upgrade (HTRequest * request, HTResponse * response,
			   char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_userAgent (HTRequest * request, HTResponse * response,
			     char * token, char * value)
{
    char * field;
    HTNet * net = HTRequest_net(request);
    HTHost * host = HTNet_host(net);
    if ((field = HTNextField(&value)) != NULL)
        HTHost_setUserAgent(host, field);
    return HT_OK;
}

PUBLIC int HTMIME_vary (HTRequest * request, HTResponse * response,
			char * token, char * value)
{
    /*
    **  Walk through the set of vary directives and add them to the
    **  response association list for vary directives
    */
    char * name_val;
    while ((name_val = HTNextPair(&value)) != NULL) {
	char * name = HTNextField(&name_val);
	char * val = HTNextField(&name_val);
	if (name) HTResponse_addVariant(response, name, val ? val : "");
    }
    return HT_OK;
}

PUBLIC int HTMIME_via (HTRequest * request, HTResponse * response,
		       char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_warning (HTRequest * request, HTResponse * response,
			   char * token, char * value)
{
    char * codestr = HTNextField(&value);
    char * agent = HTNextField(&value);
    if (codestr && agent) {
	int code = atoi(codestr);
	int idx;
	char * ptr;
	if (code==10) idx=HTERR_STALE; else
	    if (code==11) idx=HTERR_REVALIDATION_FAILED; else
		if (code==12) idx=HTERR_DISCONNECTED_CACHE; else
		    if (code==13) idx=HTERR_HEURISTIC_EXPIRATION; else
			if (code==14) idx=HTERR_TRANSFORMED; else
			    idx=HTERR_CACHE;
	if ((ptr = strchr(agent, '\r')) != NULL)	  /* Strip \r and \n */
	    *ptr = '\0';
	else if ((ptr = strchr(agent, '\n')) != NULL)
	    *ptr = '\0';
	HTRequest_addError(request, ERR_WARN, NO, idx, agent,
			   (int) strlen(agent), "HTMIME_warning");
    } else {
	HTTRACE(STREAM_TRACE, "MIMEParser.. Invalid warning\n");
    }
    return HT_OK;
}
