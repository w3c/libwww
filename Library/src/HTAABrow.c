/* 								     HTAABrow.c
**	BROWSER SIDE ACCESS AUTHORIZATION MODULE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	Contains code for parsing challenges and creating credentials for 
**	basic and digest authentication schemes. See also the HTAAUtil module
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

typedef struct _HTBasic {		  /* Basic challenge and credentials */
    char *	uid;
    char *	pw;
} HTBasic;

typedef struct _HTDigest {		 /* Digest challenge and credentials */
    char *	nounce;
    char *	opaque;
    BOOL	stale;
    char *	uid;
    char *	pw;
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
    char *tmplate = NULL;
    char *slash = NULL;
    if (docname) {
	StrAllocCopy(tmplate, docname);
	slash = strrchr(tmplate, '/');
	if (slash) slash++;
	else slash = tmplate;
	*slash = '\0';
	StrAllocCat(tmplate, "*");
    }
    else StrAllocCopy(tmplate, "*");
    if (AUTH_TRACE)
	HTTrace("Template.... Made template `%s' for file `%s'\n",
		tmplate, docname);
    return tmplate;
}

/*
**	Make basic authentication scheme credentials and register this
**	information in the request object as credentials. They will then
**	be included in the request header. An example is 
**
**		"Basic AkRDIhEF8sdEgs72F73bfaS=="
**
**	Returns	YES if credentials created, else NO
*/
PRIVATE HTAssocList * basic_credentials (HTRequest * request, HTBasic * basic)
{
    if (request && basic) {
	HTAssocList * credentials = HTAssocList_new();
	char * cleartext = NULL;
	char * cipher = NULL;
	int cl_len = strlen(basic->uid ? basic->uid : "") +
	    strlen(basic->pw ? basic->pw : "") + 3;
	int ci_len = (4*(cl_len+2))/3;
	if ((cleartext = (char *) HT_CALLOC(1, cl_len+3)) == NULL)
	    HT_OUTOFMEM("basic_credentials");
	*cleartext = '\0';
	if (basic->uid) strcpy(cleartext, basic->uid);
	strcat(cleartext, ":");
	if (basic->pw) strcat(cleartext, basic->pw);
	if ((cipher = (char *) HT_MALLOC(ci_len+1)) == NULL)
	    HT_OUTOFMEM("basic_credentials");
	HTUU_encode((unsigned char *) cleartext, cl_len, cipher);

	/* Create the credentials and assign them to the request object */
	{
	    int cr_len = strlen("basic") + ci_len + 2;
	    char * cookie = (char *) HT_MALLOC(cr_len+1);
	    if (!cookie) HT_OUTOFMEM("basic_credentials");
	    strcpy(cookie, "Basic ");
	    strcat(cookie, cipher);
	    HTAssocList_addObject(credentials, "Authorization", cookie);
	    HT_FREE(cookie);
	}
	HT_FREE(cleartext);
	HT_FREE(cipher);
	return credentials;
    }
    return NULL;
}

/*
**	Make digest authentication scheme credentials and register this
**	information in the request object as credentials. They will then
**	be included in the request header.
**	Returns	YES if credentials created, else NO
*/
PRIVATE HTAssocList *digest_credentials (HTRequest * request, HTDigest *digest)
{
    if (request && digest) {
	HTAssocList * credentials = HTAssocList_new();
	char * cleartext = NULL;
	char * cipher = NULL;

	/* Do the generation */
#if 0	
	HTAssocList_addObject(credentials, "Authorization", cookie);
#endif
	HT_FREE(cleartext);
	HT_FREE(cipher);
	return credentials;
    }
    return NULL;
}

/*
**	Prompt the user for username and password.
**	Returns	YES if user name was typed in, else NO
*/
PRIVATE BOOL prompt_user (HTRequest * request, char * realm,
			  char ** uid, char ** pw)
{
    HTAlertCallback * cbf = HTAlert_find(HT_A_USER_PW);
    if (request && cbf) {
	HTAlertPar * reply = HTAlert_newReply();	
	BOOL res = (*cbf)(request, HT_A_USER_PW,HT_MSG_NULL,*uid,realm,reply);
	if (res) {
	    HT_FREE(*uid);
	    HT_FREE(*pw);
	    *uid = HTAlert_replyMessage(reply);
	    *pw = HTAlert_replySecret(reply);
	}
	HTAlert_deleteReply(reply);
	return res ? YES : NO;
    }
    return NO;
}

/*	HTBasic_generate
**	----------------
**	This function generates "basic" credentials for the challenge found in
**	the authentication information base for this request. The result is
**	stored as an association list in the request object.
**	This is a callback function for the AA handler.
*/
PUBLIC BOOL HTBasic_generate (HTRequest * request, const char * scheme,
			      char * realm, void * challenge)
{ 
    if (request && scheme && realm && challenge) {
	HTBasic * basic = (HTBasic *) challenge;
	if (AUTH_TRACE) HTTrace("Auth........ Generating basic credentials\n");
	/*
	** If we have a set of credentials then store it in the request object
	*/
	if (basic->uid || prompt_user(request, realm,&basic->uid,&basic->pw)) {
	    HTAssocList * credentials = HTRequest_credentials(request);
	    if (credentials) HTAssocList_delete(credentials);
	    if ((credentials = basic_credentials(request, basic))) {
		HTRequest_setCredentials(request, credentials);
		return YES;
	    }
	}
    }
    return NO;
}

/*	HTBasic_parse
**	-------------
**	This function parses the contents of a "basic" challenge 
**	and stores the challenge in our authentication information datebase.
**	We also store the realm in the request object which will help finding
**	the right set of credentials to generate.
**	The function is a callback function for the AA handler.
*/
PUBLIC BOOL HTBasic_parse (HTRequest * request, const char * scheme)
{
    HTAssocList * challenge;
    if (request && (challenge = HTRequest_challenge(request))) {
	char * p = HTAssocList_findObject(challenge, "WWW-authenticate");
	char * value = HTNextField(&p);
	if (AUTH_TRACE)
	    HTTrace("Auth........ Parsing %s challenge\n", value?value:"NULL");
	if (p && (value = HTNextField(&p))) {
	    /*
	    ** If valid challenge then make a template for the resource and
	    ** store this information in our authentication base
	    */
	    if (!strcasecomp(value, "realm") && (value = HTNextField(&p))) {
		char * url = HTAnchor_physical(HTRequest_anchor(request));
		char * tmplate = make_template(url);
		HTBasic * me = (HTBasic *) HT_CALLOC(1, sizeof(HTBasic));
		if (!me) HT_OUTOFMEM("HTBasic_parse");
		HTAuthInfo_add(scheme, tmplate, value, me);
		HTRequest_setRealm(request, value);
		HT_FREE(tmplate);
		return YES;
	    }
	}
    }
    if (AUTH_TRACE) HTTrace("Auth........ None basic challenge found\n");
    return NO;
}

/*	HTBasic_delete
**	--------------
**	Deletes a "basic" information object
*/
PUBLIC BOOL HTBasic_delete (const char * scheme, void * data)
{
    HTBasic * basic = (HTBasic *) data;
    if (basic) {
	HTBasic * basic = (HTBasic *) data;
	HT_FREE(basic->uid);
	HT_FREE(basic->pw);
	HT_FREE(basic);
	return YES;
    }
    return NO;
}

/*	HTDigest_generate
**	-----------------
**	This function generates "basic" credentials for the challenge found in
**	the authentication information base for this request. The result is
**	stored as an association list in the request object.
**	This is a callback function for the AA handler.
*/
PUBLIC BOOL HTDigest_generate (HTRequest * request, const char * scheme,
			       char * realm, void * challenge)
{ 
    if (request && scheme && realm && challenge) {
	HTDigest * digest = (HTDigest *) challenge;
	if (AUTH_TRACE)HTTrace("Auth........ Generating digest credentials\n");
	/*
	** If we have a set of credentials then store it in the request object
	*/
#if 0
	if (basic->uid || prompt_user(request, realm,&basic->uid,&basic->pw)) {
	    HTAssocList * credentials = HTRequest_credentials(request);
	    if (credentials) HTAssocList_delete(credentials);
	    if ((credentials = basic_credentials(request, basic))) {
		HTRequest_setCredentials(request, credentials);
		return YES;
	    }
	}
#endif
    }
    return NO;
}

/*	HTDigest_parse
**	--------------
**	This function parses the contents of a "digest" challenge 
**	and stores the challenge in our authentication information datebase.
**	We also store the realm in the request object which will help finding
**	the right set of credentials to generate.
**	The function is a callback function for the AA handler.
*/
PUBLIC BOOL HTDigest_parse (HTRequest * request, const char * scheme)
{
    if (request) {

	/* Do the parsing */

    }
    if (AUTH_TRACE) HTTrace("Auth........ No digest challenge found\n");
    return NO;
}

/*	HTDigest_delete
**	---------------
**	Deletes a "digest" information object
*/
PUBLIC BOOL HTDigest_delete (const char * scheme, void * data)
{
    HTDigest * digest = (HTDigest *) data;
    if (digest) {
	HT_FREE(digest->nounce);
	HT_FREE(digest->opaque);
	HT_FREE(digest->uid);
	HT_FREE(digest->pw);
	HT_FREE(digest);
	return YES;
    }
    return NO;
}


