/* 								     HTAABrow.c
**	BROWSER SIDE ACCESS AUTHORIZATION MODULE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	Contains code for parsing challenges and creating credentials for 
**	basic authentication schemes. See also the HTAAUtil module
**	for how to handle other authentication schemes. You don't have to use
**	this code at all.
**
** AUTHORS:
**	AL	Ari Luotonen	luotonen@dxcern.cern.ch
**	HFN	Henrik Frystyk
**
** HISTORY:
**	Oct 17	AL	Made corrections suggested by marca:
**			Added  if (!realm->username) return NULL;
**			Changed some ""s to NULLs.
**			Now doing HT_CALLOC() to init uuencode source;
**			otherwise HTUU_encode() reads uninitialized memory
**			every now and then (not a real bug but not pretty).
**			Corrected the formula for uuencode destination size.
**	Feb 96 HFN	Rewritten to make it scheme independent and based on
**			callback functions and an info structure
*/

/* Library include files */
#include "WWWLib.h"
#include "HTAAUtil.h"
#include "HTAABrow.h"					 /* Implemented here */

#define BASIC_AUTH	"basic"

typedef struct _HTBasic {		  /* Basic challenge and credentials */
    char *	uid;
    char *	pw;
    BOOL	retry;			    /* Should we ask the user again? */
    BOOL	proxy;				     /* Proxy authentication */
} HTBasic;

/* ------------------------------------------------------------------------- */

PRIVATE HTBasic * HTBasic_new()
{
    HTBasic * me = NULL;
    if ((me = (HTBasic *) HT_CALLOC(1, sizeof(HTBasic))) == NULL)
	HT_OUTOFMEM("HTBasic_new");
    me->retry = YES;			       /* Ask the first time through */
    return me;
}

/*	HTBasic_delete
**	--------------
**	Deletes a "basic" information object
*/
PUBLIC int HTBasic_delete (void * context)
{
    HTBasic * basic = (HTBasic *) context;
    if (basic) {
	HT_FREE(basic->uid);
	HT_FREE(basic->pw);
	HT_FREE(basic);
	return YES;
    }
    return NO;
}

/*
**	Create a protection template for the files
**	in the same directory as the given file
**	Returns	a template matching docname, and other files in that directory.
**
**		E.g.  /foo/bar/x.html  =>  /foo/bar/ *
**						    ^
**				Space only to prevent it from
**				being a comment marker here,
**				there really isn't any space.
*/
PRIVATE char * make_template (const char * docname)
{
    char * tmplate = NULL;
    if (docname) {
	char * host = HTParse(docname, "", PARSE_ACCESS|PARSE_HOST|PARSE_PUNCTUATION);
	char * path = HTParse(docname, "", PARSE_PATH|PARSE_PUNCTUATION);
	char * slash = strrchr(path, '/');
	if (slash) {
	    if (*(slash+1)) {		
		strcpy(slash, "*");
		StrAllocCat(host, path);
	    } else
		StrAllocCat(host, "*");
	}
	HT_FREE(path);
	tmplate = host;
    } else
	StrAllocCopy(tmplate, "*");
    if (AUTH_TRACE)
	HTTrace("Template.... Made template `%s' for file `%s'\n",
		tmplate, docname ? docname : "<null>");
    return tmplate;
}

/*
**	Make basic authentication scheme credentials and register this
**	information in the request object as credentials. They will then
**	be included in the request header. An example is 
**
**		"Basic AkRDIhEF8sdEgs72F73bfaS=="
**
**	The function can both create normal and proxy credentials
**	Returns	HT_OK or HT_ERROR
*/
PRIVATE BOOL basic_credentials (HTRequest * request, HTBasic * basic)
{
    if (request && basic) {
	char * cleartext = NULL;
	char * cipher = NULL;
	int cl_len = strlen(basic->uid ? basic->uid : "") +
	    strlen(basic->pw ? basic->pw : "") + 2;
	int ci_len = 4 * (((cl_len+2)/3) + 1);
	if ((cleartext = (char *) HT_CALLOC(1, cl_len)) == NULL)
	    HT_OUTOFMEM("basic_credentials");
	*cleartext = '\0';
	if (basic->uid) strcpy(cleartext, basic->uid);
	strcat(cleartext, ":");
	if (basic->pw) strcat(cleartext, basic->pw);
	if ((cipher = (char *) HT_CALLOC(1, ci_len + 3)) == NULL)
	    HT_OUTOFMEM("basic_credentials");
	HTUU_encode((unsigned char *) cleartext, strlen(cleartext), cipher);

	/* Create the credentials and assign them to the request object */
	{
	    int cr_len = strlen("basic") + ci_len + 3;
	    char * cookie = (char *) HT_MALLOC(cr_len+1);
	    if (!cookie) HT_OUTOFMEM("basic_credentials");
	    strcpy(cookie, "Basic ");
	    strcat(cookie, cipher);
	    if (AUTH_TRACE) HTTrace("Basic Cookie `%s\'\n", cookie);

	    /* Check whether it is proxy or normal credentials */
	    if (basic->proxy)
		HTRequest_addCredentials(request, "Proxy-Authorization", cookie);
	    else
		HTRequest_addCredentials(request, "Authorization", cookie);

	    HT_FREE(cookie);
	}
	HT_FREE(cleartext);
	HT_FREE(cipher);
	return HT_OK;
    }
    return HT_ERROR;
}

/*
**	Prompt the user for username and password.
**	Returns	YES if user name was typed in, else NO
*/
PRIVATE int prompt_user (HTRequest * request, const char * realm,
			 HTBasic * basic)
{
    HTAlertCallback * cbf = HTAlert_find(HT_A_USER_PW);
    if (request && cbf) {
	HTAlertPar * reply = HTAlert_newReply();
	int code = basic->proxy ? HT_MSG_PROXY_PW : HT_A_USER_PW;
	BOOL res = (*cbf)(request, code, HT_MSG_NULL,
			  basic->uid, (char *) realm, reply);
	if (res) {
	    HT_FREE(basic->uid);
	    HT_FREE(basic->pw);
	    basic->uid = HTAlert_replyMessage(reply);
	    basic->pw = HTAlert_replySecret(reply);
	}
	HTAlert_deleteReply(reply);
	return res ? HT_OK : HT_ERROR;
    }
    return HT_OK;
}

/*	HTBasic_generate
**	----------------
**	This function generates "basic" credentials for the challenge found in
**	the authentication information base for this request. The result is
**	stored as an association list in the request object.
**	This is a callback function for the AA handler.
*/
PUBLIC int HTBasic_generate (HTRequest * request, void * context, int status)
{ 
    HTBasic * basic = (HTBasic *) context;
    if (request) {
	const char * realm = HTRequest_realm(request);

	/*
	** If we don't have a basic context then add a new one to the tree.
	** As we don't know whether this is for a proxy or not then try
	** normal and hope for the best. This should normal not happen!
	*/
	if (!basic) {
	    char *url = HTAnchor_address((HTAnchor*)HTRequest_anchor(request));
	    basic = HTBasic_new();
	    HTAA_updateNode(NO, BASIC_AUTH, realm, url, basic);
	    HT_FREE(url);
	}

	/*
	** If we have a set of credentials (or the user provides a new set)
	** then store it in the request object as the credentials
	*/
	if ((basic->retry && prompt_user(request, realm, basic) == HT_OK) ||
	    (!basic->retry && basic->uid)) {
	    basic->retry = NO;
	    return basic_credentials(request, basic);
	} else
	    return HT_ERROR;
    }
    return HT_OK;
}

/*	HTBasic_parse
**	-------------
**	This function parses the contents of a "basic" challenge 
**	and stores the challenge in our authentication information datebase.
**	We also store the realm in the request object which will help finding
**	the right set of credentials to generate.
**	The function is a callback function for the AA handler.
*/
PUBLIC int HTBasic_parse (HTRequest * request, void * context, int status)
{
    HTAssocList * challenge = HTRequest_challenge(request);
    HTBasic * basic = NULL;
    BOOL proxy = status==HT_NO_PROXY_ACCESS ? YES : NO;
    if (request && challenge) {
	char * p = HTAssocList_findObject(challenge, BASIC_AUTH);
	char * realm = HTNextField(&p);
	char * rm = HTNextField(&p);

	/*
	** If valid challenge then make a template for the resource and
	** store this information in our authentication URL Tree
	*/
	if (realm && !strcasecomp(realm, "realm") && rm) {
	    if (AUTH_TRACE) HTTrace("Basic Parse. Realm `%s\' found\n", rm);
	    HTRequest_setRealm(request, rm);

	    /*
	    **  If we are in proxy mode then add the proxy - not the final URL
	    */
	    if (proxy) {
		char * url = HTRequest_proxy(request);
		basic = (HTBasic *) HTAA_updateNode(proxy, BASIC_AUTH, rm,
						    url, NULL);
	    } else {
		char * url = HTAnchor_address((HTAnchor *)
					      HTRequest_anchor(request));
		char * tmplate = make_template(url);
		basic = (HTBasic *) HTAA_updateNode(proxy, BASIC_AUTH, rm,
						    tmplate, NULL);
		HT_FREE(url);
		HT_FREE(tmplate);
	    }
	}

	/*
	** For some reason the authentication failed so we have to ask the user
	** if we should try again. It may be because the user typed the wrong
	** user name and password
	*/
	if (basic) {
	    HTAlertCallback * prompt = HTAlert_find(HT_A_CONFIRM);

	    /*
	    ** Check if the reason for being here is because of lack of proxy
	    ** credentials or "normal" credentials
	    */
	    if (proxy) {
		if (AUTH_TRACE) HTTrace("Basic Parse. Proxy authentication\n");
		basic->proxy = YES;			    /* Remember this */
	    }

	    /* Can we ask the user? */
	    if (prompt) {
		int code = proxy ?
		    HT_MSG_RETRY_PROXY_AUTH : HT_MSG_RETRY_AUTHENTICATION;
		if ((*prompt)(request, HT_A_CONFIRM, code,
			      NULL, NULL, NULL) != YES)
		    return HT_ERROR;
		basic->retry = YES;
	    }
	}
	return HT_OK;
    }
    if (AUTH_TRACE) HTTrace("Auth........ No challenges found\n");
    return HT_ERROR;
}
