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
#define DIGEST_AUTH	"digest"

typedef struct _HTBasic {		  /* Basic challenge and credentials */
    char *	uid;
    char *	pw;
    BOOL	retry;			    /* Should we ask the user again? */
    BOOL	proxy;				     /* Proxy authentication */
} HTBasic;

typedef struct _HTDigest {		 /* Digest challenge and credentials */
    char *	uid;
    char *	pw;
    char *	nounce;
    char *	opaque;
    BOOL	stale;
    BOOL	retry;			    /* Should we ask the user again? */
    BOOL	proxy;				     /* Proxy authentication */
    int		references;		/* Number of pointers to this object */
} HTDigest;

/* ------------------------------------------------------------------------- */

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
#if 0
	    if (*(slash+1)) {		
		strcpy(slash, "*");
		StrAllocCat(host, path);
	    } else
		StrAllocCat(host, "/*");
#else
	    if (slash > path) {		
		strcpy(slash, "*");
		StrAllocCat(host, path);
	    } else
		StrAllocCat(host, "/*");
#endif
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

/* ------------------------------------------------------------------------- */
/*				Basic Authentication                         */
/* ------------------------------------------------------------------------- */

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
	int msg = basic->proxy ? HT_MSG_PROXY_UID : HT_MSG_UID;
	BOOL res = (*cbf)(request, HT_A_USER_PW, msg,
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
	    strlen(basic->pw ? basic->pw : "") + 5;
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

/*	HTBasic_generate
**	----------------
**	This function generates "basic" credentials for the challenge found in
**	the authentication information base for this request. The result is
**	stored as an association list in the request object.
**	This is a callback function for the AA handler.
*/
PUBLIC int HTBasic_generate (HTRequest * request, void * context, int mode)
{ 
    HTBasic * basic = (HTBasic *) context;
    BOOL proxy = mode==HT_NO_PROXY_ACCESS ? YES : NO;
    if (request) {
	const char * realm = HTRequest_realm(request);

	/*
	**  If we were asked to explicitly ask the user again
	*/
	if (mode == HT_REAUTH || mode == HT_PROXY_REAUTH)
	    basic->retry = YES;

	/*
	** If we don't have a basic context then add a new one to the tree.
	** We use different trees for normal and proxy authentication
	*/
	if (!basic) {
	    if (proxy) {
		char * url = HTRequest_proxy(request);
		basic = HTBasic_new();
		basic->proxy = YES;
		HTAA_updateNode(proxy, BASIC_AUTH, realm, url, basic);
	    } else {
		char * url = HTAnchor_address((HTAnchor*)HTRequest_anchor(request));
		basic = HTBasic_new();
		HTAA_updateNode(proxy, BASIC_AUTH, realm, url, basic);
		HT_FREE(url);
	    }
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
PUBLIC int HTBasic_parse (HTRequest * request, HTResponse * response,
			  void * context, int status)
{
    HTAssocList * challenge = HTResponse_challenge(response);
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
		if (AUTH_TRACE) HTTrace("Basic Parse. Proxy authentication\n");
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
	    ** Do we haev a method registered for prompting the user whether
	    ** we should retry
	    */
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

/* ------------------------------------------------------------------------- */
/*				Digest Authentication                        */
/* ------------------------------------------------------------------------- */

/*
**	Prompt the user for username and password.
**	Returns	YES if user name was typed in, else NO
*/
PRIVATE int prompt_digest_user (HTRequest * request, const char * realm,
				HTDigest * digest)
{
    HTAlertCallback * cbf = HTAlert_find(HT_A_USER_PW);
    if (request && cbf) {
	HTAlertPar * reply = HTAlert_newReply();
	int msg = digest->proxy ? HT_MSG_PROXY_UID : HT_MSG_UID;
	BOOL res = (*cbf)(request, HT_A_USER_PW, msg,
			  digest->uid, (char *) realm, reply);
	if (res) {
	    HT_FREE(digest->uid);
	    HT_FREE(digest->pw);
	    digest->uid = HTAlert_replyMessage(reply);
	    digest->pw = HTAlert_replySecret(reply);
	}
	HTAlert_deleteReply(reply);
	return res ? HT_OK : HT_ERROR;
    }
    return HT_OK;
}

PRIVATE HTDigest * HTDigest_new()
{
    HTDigest * me = NULL;
    if ((me = (HTDigest *) HT_CALLOC(1, sizeof(HTDigest))) == NULL)
	HT_OUTOFMEM("HTDigest_new");
    me->retry = YES;			       /* Ask the first time through */
    return me;
}

/*	HTDigest_delete
**	--------------
**	Deletes a "digest" information object
**	A single object may be registered multiple places in the URL tree.
**	We keep a simple reference count on the object so that we know
**	when to delete the object.
*/
PUBLIC int HTDigest_delete (void * context)
{
    HTDigest * digest = (HTDigest *) context;
    if (digest) {
	if (digest->references <= 0) {
	    HT_FREE(digest->uid);
	    HT_FREE(digest->pw);
	    HT_FREE(digest->nounce);
	    HT_FREE(digest->opaque);
	    HT_FREE(digest);
	} else
	    digest->references--;
	return YES;
    }
    return NO;
}

/*
**	Make digest authentication scheme credentials and register this
**	information in the request object as credentials. They will then
**	be included in the request header.
**	The function can both create normal and proxy credentials
**	Returns	HT_OK or HT_ERROR
*/
PRIVATE BOOL digest_credentials (HTRequest * request, HTDigest * digest)
{
    if (request && digest) {

	/* THIS IS CURRENTLY FOR BASIC AUTH. CHANGE THIS TO DIGEST */

	char * cleartext = NULL;
	char * cipher = NULL;
	int cl_len = strlen(digest->uid ? digest->uid : "") +
	    strlen(digest->pw ? digest->pw : "") + 5;
	int ci_len = 4 * (((cl_len+2)/3) + 1);
	if ((cleartext = (char *) HT_CALLOC(1, cl_len)) == NULL)
	    HT_OUTOFMEM("digest_credentials");
	*cleartext = '\0';
	if (digest->uid) strcpy(cleartext, digest->uid);
	strcat(cleartext, ":");
	if (digest->pw) strcat(cleartext, digest->pw);
	if ((cipher = (char *) HT_CALLOC(1, ci_len + 3)) == NULL)
	    HT_OUTOFMEM("digest_credentials");
	HTUU_encode((unsigned char *) cleartext, strlen(cleartext), cipher);

	/* Create the credentials and assign them to the request object */
	{
	    int cr_len = strlen("digest") + ci_len + 3;
	    char * cookie = (char *) HT_MALLOC(cr_len+1);
	    if (!cookie) HT_OUTOFMEM("digest_credentials");
	    strcpy(cookie, "Digest ");
	    strcat(cookie, cipher);
	    if (AUTH_TRACE) HTTrace("Digest Cookie `%s\'\n", cookie);

	    /* Check whether it is proxy or normal credentials */
	    if (digest->proxy)
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

/*	HTDigest_generate
**	----------------
**	This function generates "digest" credentials for the challenge found in
**	the authentication information base for this request. The result is
**	stored as an association list in the request object.
**	This is a callback function for the AA handler.
*/
PUBLIC int HTDigest_generate (HTRequest * request, void * context, int mode)
{ 
    HTDigest * digest = (HTDigest *) context;
    BOOL proxy = mode==HT_NO_PROXY_ACCESS ? YES : NO;
    if (request) {
	const char * realm = HTRequest_realm(request);

	/*
	**  If we were asked to explicitly ask the user again
	*/
	if (mode == HT_REAUTH || mode == HT_PROXY_REAUTH)
	    digest->retry = YES;

	/*
	** If we don't have a digest context then add a new one to the tree.
	** We use different trees for normal and proxy authentication
	*/
	if (!digest) {
	    if (proxy) {
		char * url = HTRequest_proxy(request);
		digest = HTDigest_new();
		digest->proxy = YES;
		HTAA_updateNode(proxy, DIGEST_AUTH, realm, url, digest);
	    } else {
		char * url = HTAnchor_address((HTAnchor*)HTRequest_anchor(request));
		digest = HTDigest_new();
		HTAA_updateNode(proxy, DIGEST_AUTH, realm, url, digest);
		HT_FREE(url);
	    }
	}

	/*
	** If we have a set of credentials (or the user provides a new set)
	** then store it in the request object as the credentials
	*/
	if ((digest->retry &&
	     prompt_digest_user(request, realm, digest) == HT_OK) ||
	    (!digest->retry && digest->uid)) {
	    digest->retry = NO;
	    return digest_credentials(request, digest);
	} else
	    return HT_ERROR;
    }
    return HT_OK;
}

/*	HTDigest_parse
**	-------------
**	This function parses the contents of a "digest" challenge 
**	and stores the challenge in our authentication information datebase.
**	We also store the realm in the request object which will help finding
**	the right set of credentials to generate.
**	The function is a callback function for the AA handler.
*/
PUBLIC int HTDigest_parse (HTRequest * request, HTResponse * response,
			   void * context, int status)
{
    HTAssocList * challenge = HTResponse_challenge(response);
    HTDigest * digest = NULL;    
    BOOL proxy = status==HT_NO_PROXY_ACCESS ? YES : NO;
    if (request && challenge) {
	char * p = HTAssocList_findObject(challenge, DIGEST_AUTH);
	char * realm =  HTNextField(&p);
	char * value =  HTNextField(&p);
	char * token = NULL;
	char * uris = NULL;
	BOOL found = NO;

	/*
	**  Search for the realm and see if we have an entry for it. If not
	**  then create a new entry.
	*/
	if (realm && !strcasecomp(realm, "realm") && value) {
	    if (AUTH_TRACE) HTTrace("Basic Parse. Realm `%s\' found\n", value);
	    HTRequest_setRealm(request, value);
	    digest = (HTDigest *)
		HTAA_updateNode(proxy, DIGEST_AUTH, value, NULL, NULL);
	}
	if (!digest)
	    digest = HTDigest_new();
	else
	    found = YES;

	/*
	**  Search through the set of parameters in the digest header.
	**  If valid challenge then make a template for the resource and
	**  store this information in our authentication URL Tree
	*/
	while ((token = HTNextField(&p))) {
	    if (!strcasecomp(token, "domain")) {
		if ((value = HTNextField(&p)))
		    uris = value;
	    } else if (!strcasecomp(token, "nounce")) {
		if ((value = HTNextField(&p)))
		    StrAllocCopy(digest->nounce, value);
	    } else if (!strcasecomp(token, "opaque")) {
		if ((value = HTNextField(&p)))
		    StrAllocCopy(digest->opaque, value);
	    } else if (!strcasecomp(token, "stale")) {
		if ((value = HTNextField(&p)) && !strcasecomp(value, "true"))
		    digest->stale = YES;
	    } else if (!strcasecomp(token, "algorithm")) {
	        if ((value = HTNextField(&p)) && strcasecomp(value, "md5")) {
		    /*
		    **  We only support MD5 for the moment
		    */
		    if (AUTH_TRACE) HTTrace("Digest Parse Unknown algorithm `%s\'\n", value);
		    HTDigest_delete(digest);
		    return HT_ERROR;
		}
	    }
	}

	/*
	**  Now as we have parsed the full digest header we update the URL tree
	**  with the new information. If we didn't get a "domain" token then
	**  add the node using a template. If we have multiple URIs in the
	**  domain token then add a digest node at each URI.
	*/
	if (!uris) {
	    if (proxy) {
		char * location = HTRequest_proxy(request);
		if (AUTH_TRACE) HTTrace("Digest Parse Proxy authentication\n");
		HTAA_updateNode(proxy, DIGEST_AUTH, realm, location, digest);
	    } else {
		char * url = HTAnchor_address((HTAnchor *) HTRequest_anchor(request));
		char * tmplate = make_template(url);
		HTAA_updateNode(proxy, DIGEST_AUTH, realm, tmplate, digest);
		HT_FREE(url);
		HT_FREE(tmplate);
	    }
	} else {

	    /*
	    **  ADD THE DIGEST FOR EACH URL IN THE LIST AND INCREMENT THE
	    **  REFERENCE COUNT IN THE DIGEST BY ONE
	    */

	}

	/*
	** For some reason the authentication failed so we have to ask the user
	** if we should try again. It may be because the user typed the wrong
	** user name and password
	*/
	if (found) {
	    HTAlertCallback * prompt = HTAlert_find(HT_A_CONFIRM);

	    /*
	    ** Do we have a method registered for prompting the user whether
	    ** we should retry
	    */
	    if (prompt) {
		int code = proxy ?
		    HT_MSG_RETRY_PROXY_AUTH : HT_MSG_RETRY_AUTHENTICATION;
		if ((*prompt)(request, HT_A_CONFIRM, code,
			      NULL, NULL, NULL) != YES)
		    return HT_ERROR;
		digest->retry = YES;
	    }
	}
	return HT_OK;
    }
    if (AUTH_TRACE) HTTrace("Auth........ No challenges found\n");
    return HT_ERROR;
}

