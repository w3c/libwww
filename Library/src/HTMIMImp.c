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
        if (!strcasecomp(field, "keep-alive")) {
	    HTNet * net = HTRequest_net(request);
	    HTNet_setPersistent(net, YES);
	    if (STREAM_TRACE) HTTrace("MIMEParser.. Persistent Connection\n");
	}
    }
    return HT_OK;
}

PUBLIC int HTMIME_contentBase (HTRequest * request, char * token, char * value)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
#if 0
    HTAnchor_setBase(anchor, HTStrip(value));
#endif
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

PUBLIC int HTMIME_contentLocation (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_contentMD5 (HTRequest * request, char * token, char * value)
{

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
        char *lc = field;
	while ((*lc = TOLOWER(*lc))) lc++; 
	HTAnchor_setFormat(anchor, HTAtom_for(field));
	while ((field = HTNextField(&value)) != NULL) {
	    if (!strcasecomp(field, "charset")) {
	        if ((field = HTNextField(&value)) != NULL) {
		    lc = field;
		    while ((*lc = TOLOWER(*lc))) lc++;
		    HTAnchor_setCharset(anchor, HTAtom_for(field));
		}
	    } else if (!strcasecomp(field, "level")) {
	        if ((field = HTNextField(&value)) != NULL) {
		    lc = field;
		    while ((*lc = TOLOWER(*lc))) lc++;
		    HTAnchor_setLevel(anchor, HTAtom_for(field));
		}
	    } else if (!strcasecomp(field, "boundary")) {
	        if ((field = HTNextField(&value)) != NULL) {
		    StrAllocCopy(request->boundary, field);
	        }
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

PUBLIC int HTMIME_messageDigest (HTRequest * request, char * token, char * value)
{
    if (!request->challenge) request->challenge = HTAssocList_new();
    HTAssocList_add(request->challenge, "Digest-MessageDigest", value);
    return HT_OK;
}

PUBLIC int HTMIME_etag (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_expires (HTRequest * request, char * token, char * value)
{
    HTParentAnchor * anchor = HTRequest_anchor(request);
    HTAnchor_setExpires(anchor, HTParseTime(value, 
					    HTRequest_userProfile(request)));
    return HT_OK;
}

PUBLIC int HTMIME_from (HTRequest * request, char * token, char * value)
{

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
    return HT_OK;
}

PUBLIC int HTMIME_maxForwards (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_newsGroups (HTRequest * request, char * token, char * value)
{
    /* HTRequest_net(request)->nntp = YES; */	       /* Due to news brain damage */
    return HT_OK;
}

PUBLIC int HTMIME_pragma (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_proxyAuthenticate (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_proxyAuthorization (HTRequest * request, char * token, char * value)
{

    return HT_OK;
}

PUBLIC int HTMIME_public (HTRequest * request, char * token, char * value)
{

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

    return HT_OK;
}

PUBLIC int HTMIME_authenticate (HTRequest * request, char * token, char * value)
{
    HTAssocList * challenge = HTRequest_challenge(request);
    if (!challenge) challenge = HTAssocList_new();
    HTAssocList_add(challenge, "WWW-authenticate", value);

    StrAllocCopy(request->scheme, "basic");	/* @@@@@@@@@ */

    return HT_OK;
}
