/*
**	DEFAULT MIME HEADER PARSER 
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
*/

/* Library include files */
#include "sysdep.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTReqMan.h"	/* @@@ */
#include "HTHeader.h"
#include "HTMIMImp.h"					 /* Implemented here */

/* ------------------------------------------------------------------------- */

PUBLIC int HTMIME_accept (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_acceptCharset (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_acceptEncoding (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_acceptLanguage (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_acceptRanges (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_age (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_allow (HTRequest * request, char * token, char * value)
{
    char * field;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    while ((field = HTNextField(&value)) != NULL) {
        HTMethod new_method;
	/* We treat them as case-insensitive! */
	if ((new_method = HTMethod_enum(field)) != METHOD_INVALID)
	    HTAnchor_appendMethods(anchor, new_method);
    }
    if (STREAM_TRACE)
        HTTrace("MIMEParser.. Methods allowed: %d\n", HTAnchor_methods(anchor));
    return HT_OK;
}

PUBLIC int HTMIME_authenticate (HTRequest * request, char * token, char * value)
{    
    char * scheme = HTNextField(&value);
    if (scheme) {
	HTRequest_addChallenge(request, scheme, value);
	HTRequest_setScheme(request, scheme);
    }
    return HT_OK;
}

PUBLIC int HTMIME_authorization (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_cacheControl (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_connection (HTRequest * request, char * token, char * value)
{
    char * field;
    if ((field = HTNextField(&value)) != NULL) {
	if (!strcasecomp(field, "close")) {			 /* HTTP/1.1 */
	    HTNet * net = HTRequest_net(request);
	    HTNet_setPersistent(net, NO, HT_TP_INTERLEAVE);
	    if (STREAM_TRACE) HTTrace("MIMEParser.. Close negotiated\n");
	} else if (!strcasecomp(field, "keep-alive")) {	         /* HTTP/1.0 */
	    HTNet * net = HTRequest_net(request);
	    HTNet_setPersistent(net, YES, HT_TP_SINGLE);
	    if (STREAM_TRACE) HTTrace("MIMEParser.. HTTP/1.0 Keep Alive\n");
	}
    }
    return HT_OK;
}

PUBLIC int HTMIME_contentBase (HTRequest * request, char * token, char * value)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    HTAnchor_setBase(anchor, HTStrip(value));
    return HT_OK;
}

PUBLIC int HTMIME_contentEncoding (HTRequest * request, char * token, char * value)
{
    char * field;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    while ((field = HTNextField(&value)) != NULL) {
        char * lc = field;
	while ((*lc = TOLOWER(*lc))) lc++;
	HTAnchor_addEncoding(anchor, HTAtom_for(field));
    }
    return HT_OK;
}

PUBLIC int HTMIME_contentLanguage (HTRequest * request, char * token, char * value)
{
    char * field;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    while ((field = HTNextField(&value)) != NULL) {
        char * lc = field;
	while ((*lc = TOLOWER(*lc))) lc++;
	HTAnchor_addLanguage(anchor, HTAtom_for(field));
    }
    return HT_OK;
}

PUBLIC int HTMIME_contentLength (HTRequest * request, char * token, char * value)
{
    char * field;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    if ((field = HTNextField(&value)) != NULL)
        HTAnchor_setLength(anchor, atol(field));
    return HT_OK;
}

/*
**	For content location we create a new anchor for the specific URL
**	and then register this as a variant of that anchor
*/
PUBLIC int HTMIME_contentLocation (HTRequest * request, char * token, char * value)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    HTAnchor_setLocation(anchor, HTStrip(value));
    return HT_OK;
}

PUBLIC int HTMIME_contentMD5 (HTRequest * request, char * token, char * value)
{
    char * field;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    if ((field = HTNextField(&value)) != NULL)
        HTAnchor_setMd5(anchor, field);
    return HT_OK;
}

PUBLIC int HTMIME_contentRange (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_contentTransferEncoding (HTRequest * request, char * token, char * value)
{
    char * field;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    if ((field = HTNextField(&value)) != NULL) {
        char *lc = field;
	while ((*lc = TOLOWER(*lc))) lc++;
	HTAnchor_setTransfer(anchor, HTAtom_for(field));
    }
    return HT_OK;
}

PUBLIC int HTMIME_contentType (HTRequest * request, char * token, char * value)
{
    char * field;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    if ((field = HTNextField(&value)) != NULL) {

	/* Get the Content-Type */
        char *lc = field;
	while ((*lc = TOLOWER(*lc))) lc++; 
	HTAnchor_setFormat(anchor, HTAtom_for(field));

	/* Get all the parameters to the Content-Type */
	{
	    char * param;
	    while ((field = HTNextField(&value)) != NULL &&
		   (param = HTNextField(&value)) != NULL) {
		lc = field;
		while ((*lc = TOLOWER(*lc))) lc++;
		lc = param;
		while ((*lc = TOLOWER(*lc))) lc++;
		HTAnchor_addFormatParam(anchor, field, param);
	    }
	}
    }
    return HT_OK;
}

PUBLIC int HTMIME_date (HTRequest * request, char * token, char * value)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    HTAnchor_setDate(anchor, HTParseTime(value, 
					 HTRequest_userProfile(request)));
    return HT_OK;
}

PUBLIC int HTMIME_derivedFrom (HTRequest * request, char * token, char * value)
{
    char * field;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    if ((field = HTNextField(&value)) != NULL)
        HTAnchor_setDerived(anchor, field);
    return HT_OK;
}

PUBLIC int HTMIME_etag (HTRequest * request, char * token, char * value)
{
    char * field;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    if ((field = HTNextField(&value)) != NULL)
        HTAnchor_setEtag(anchor, field);
    return HT_OK;
}

PUBLIC int HTMIME_expires (HTRequest * request, char * token, char * value)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    HTAnchor_setExpires(anchor, HTParseTime(value, 
					    HTRequest_userProfile(request)));
    return HT_OK;
}

PUBLIC int HTMIME_extension (HTRequest * request, char * token, char * value)
{
    char * param = NULL;
    char * extension = HTNextSExp(&value, &param);
    if (extension) {
	if (PROT_TRACE)
	    HTTrace("Extension... name: `%s\', value: `%s\'\n",
		    extension, param);
	HTRequest_addExtension(request, extension, param);
    }
    return HT_OK;
}

/*
**	We add the from information to the User object
**	Create a new User Profile if we are using the global libwww one.
*/
PUBLIC int HTMIME_from (HTRequest * request, char * token, char * value)
{
    char * field;
    if ((field = HTNextField(&value)) != NULL) {
	HTUserProfile * up = HTRequest_userProfile(request);
	if (up == HTLib_userProfile())
	    up = HTUserProfile_new("server-profile", NULL);
	HTUserProfile_setEmail(up, field);
    }
    return HT_OK;
}

PUBLIC int HTMIME_host (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_ifModifiedSince (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_ifMatch (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_ifNoneMatch (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_ifRange (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_ifUnmodifiedSince (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_lastModified (HTRequest * request, char * token, char * value)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    HTAnchor_setLastModified(anchor, HTParseTime(value, 
					    HTRequest_userProfile(request)));
    return HT_OK;
}

PUBLIC int HTMIME_link (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_location (HTRequest * request, char * token, char * value)
{
    HTAnchor * redirection = HTAnchor_findAddress(HTStrip(value));
    HTRequest_setRedirection(request, redirection);

    /* 
    ** If moved permanent then make a typed link between the old and the new
    ** anchor. If the location header is part of a 201 response then make a new
    ** anchor and link this to the original anchor with "created".
    */
    return HT_OK;
}

PUBLIC int HTMIME_maxForwards (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_messageDigest (HTRequest * request, char * token, char * value)
{
    if (!request->challenge) request->challenge = HTAssocList_new();
    HTAssocList_addObject(request->challenge, "Digest-MessageDigest", value);
    return HT_OK;
}

PUBLIC int HTMIME_pragma (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_proxyAuthorization (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_public (HTRequest * request, char * token, char * value)
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
    if (STREAM_TRACE)
        HTTrace("MIMEParser.. Public methods: %d\n",
		HTHost_publicMethods(host));
    return HT_OK;
}

PUBLIC int HTMIME_range (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_referer (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_retryAfter (HTRequest * request, char * token, char * value)
{
    request->retry_after =
	HTParseTime(value, HTRequest_userProfile(request));
    return HT_OK;
}

PUBLIC int HTMIME_server (HTRequest * request, char * token, char * value)
{
    char * field;
    HTNet * net = HTRequest_net(request);
    HTHost * host = HTNet_host(net);
    if ((field = HTNextField(&value)) != NULL)
        HTHost_setServer(host, field);
    return HT_OK;
}

PUBLIC int HTMIME_title (HTRequest * request, char * token, char * value)
{
    char * field;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    if ((field = HTNextField(&value)) != NULL)
        HTAnchor_setTitle(anchor, field);
    return HT_OK;
}

PUBLIC int HTMIME_upgrade (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_uri (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_userAgent (HTRequest * request, char * token, char * value)
{
    char * field;
    HTNet * net = HTRequest_net(request);
    HTHost * host = HTNet_host(net);
    if ((field = HTNextField(&value)) != NULL)
        HTHost_setUserAgent(host, field);
    return HT_OK;
}

PUBLIC int HTMIME_vary (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_version (HTRequest * request, char * token, char * value)
{
    char * field;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    if ((field = HTNextField(&value)) != NULL)
        HTAnchor_setVersion(anchor, field);
    return HT_OK;
}

PUBLIC int HTMIME_via (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_warning (HTRequest * request, char * token, char * value)
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
	if (STREAM_TRACE) HTTrace("MIMEParser.. Invalid warning\n");
    }
    return HT_OK;
}
