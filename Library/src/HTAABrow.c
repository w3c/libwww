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
**      JKO     Jose Kahan      
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
**      Nov 98 JKO      Added support for message digest authentication
**    Jun 2000 JKO      Changed the buffer size for HTUU_encode in order
**                      to avoid a potential SIGSEV when calling that 
**                      function (as advised by Heiner Kallweit).
**    Mar 2001 JKO      When doing pipelining digest requests, the stale
**                      nonce reply appears only for one of such requests,
**                      all the following ones in the pipe will receive a 
**                      401. I added some code to take into account these cases
**                      by trying to infer if a nonce is stale. 
**
*/

/* Portions of this code (as indicated) are derived from the Internet Draft
** draft-ietf-http-authentication-03 and are covered by the following
** copyright:

** Copyright (C) The Internet Society (1998). All Rights Reserved.

** This document and translations of it may be copied and furnished to
** others, and derivative works that comment on or otherwise explain it or
** assist in its implmentation may be prepared, copied, published and
** distributed, in whole or in part, without restriction of any kind,
** provided that the above copyright notice and this paragraph are included
** on all such copies and derivative works. However, this document itself
** may not be modified in any way, such as by removing the copyright notice
** or references to the Internet Society or other Internet organizations,
** except as needed for the purpose of developing Internet standards in
** which case the procedures for copyrights defined in the Internet
** Standards process must be followed, or as required to translate it into
** languages other than English.

** The limited permissions granted above are perpetual and will not be
** revoked by the Internet Society or its successors or assigns.

** This document and the information contained herein is provided on an "AS
** IS" basis and THE INTERNET SOCIETY AND THE INTERNET ENGINEERING TASK
** FORCE DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT
** LIMITED TO ANY WARRANTY THAT THE USE OF THE INFORMATION HEREIN WILL NOT
** INFRINGE ANY RIGHTS OR ANY IMPLIED WARRANTIES OF MERCHANTABILITY OR
** FITNESS FOR A PARTICULAR PURPOSE.
**/

/* Library include files */
#include "WWWLib.h"
#include "HTAAUtil.h"
#include "HTParse.h"
#include "HTAABrow.h"					 /* Implemented here */
#include "HTDigest.h"

#define BASIC_AUTH	"basic"
#define DIGEST_AUTH	"digest"
#define DIGEST_AI       "authentication-info"
#define PROXY_DIGEST_AI "proxy-authentication-info"

typedef struct _HTBasic {		  /* Basic challenge and credentials */
    char *	uid;
    char *	pw;
    BOOL	retry;			    /* Should we ask the user again? */
    BOOL	proxy;				     /* Proxy authentication */
} HTBasic;

typedef struct _HTDigest {		 /* Digest challenge and credentials */
  /* digest info can be shared by one or more UT entries */
    int         references;              
  /* client authentication data */
    char *	uid;
    char *	pw;
    char *      realm;
    char *      cnonce;
    long        nc;
  /* server authentication data */
    char *	nonce;
    char *	opaque;
  /* session authentication data */
    int         algorithm;
    char *      qop;
    BOOL	stale;
    BOOL	retry;			    /* Should we ask the user again? */
    BOOL	proxy;				     /* Proxy authentication */
} HTDigest;

#define HASHLEN 16
typedef char HASH[HASHLEN+1];
#define HASHHEXLEN 32
typedef char HASHHEX[HASHHEXLEN+1];

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
	    if (*(slash+1)) {
		strcpy(slash + 1, "*");
		StrAllocCat(host, path);
	    } else {
                StrAllocCat(host, path);
                StrAllocCat(host, "*");
	    }
#endif
	}
	HT_FREE(path);
	tmplate = host;
    } else
	StrAllocCopy(tmplate, "*");
    HTTRACE(AUTH_TRACE, "Template.... Made template `%s' for file `%s'\n" _ 
		tmplate _ docname ? docname : "<null>");
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

    /* If no method for prompting the user then we might as well give up */
    if (!cbf) return HT_ERROR;

    /* Otherwise go ahead and ask the user */
    if (request) {
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
	int ci_len = 5 + 4 * (cl_len/3);
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
	    HTTRACE(AUTH_TRACE, "Basic Cookie `%s\'\n" _ cookie);

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
		basic = HTBasic_new();
	    if (proxy) {
		char * url = HTRequest_proxy(request);
		basic->proxy = YES;
		HTAA_updateNode(proxy, BASIC_AUTH, realm, url, basic);
	    } else {
		char * url = HTAnchor_address((HTAnchor*)HTRequest_anchor(request));
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
	} else {
	    char * url = HTAnchor_address((HTAnchor*)HTRequest_anchor(request));
	    HTAA_deleteNode(proxy, BASIC_AUTH, realm, url);
	    HT_FREE(url);
	    return HT_ERROR;
	}
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
	    HTTRACE(AUTH_TRACE, "Basic Parse. Realm `%s\' found\n" _ rm);
	    HTRequest_setRealm(request, rm);

	    /*
	    **  If we are in proxy mode then add the proxy - not the final URL
	    */
	    if (proxy) {
		char * url = HTRequest_proxy(request);
		HTTRACE(AUTH_TRACE, "Basic Parse. Proxy authentication\n");
		basic = (HTBasic *) HTAA_updateNode(proxy, BASIC_AUTH, rm,
						    url, NULL);
		/* if the previous authentication failed, then try again */
		if (HTRequest_AAretrys (request) > 1 
		    && status == HT_NO_ACCESS && basic)
		  basic->retry = YES;
	    } else {
		char * url = HTAnchor_address((HTAnchor *)
					      HTRequest_anchor(request));
		char * tmplate = make_template(url);
		basic = (HTBasic *) HTAA_updateNode(proxy, BASIC_AUTH, rm,
						    tmplate, NULL);
		/* if the previous authentication failed, then try again */
		if (HTRequest_AAretrys (request) > 1 
		    && status == HT_NO_ACCESS && basic)
		  basic->retry = YES;
		HT_FREE(url);
		HT_FREE(tmplate);
	    }
	}

	/*
	** For some reason the authentication failed so we have to ask the user
	** if we should try again. It may be because the user typed the wrong
	** user name and password
	*/
	if (basic && basic->retry) {
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
	    }
	}
	return HT_OK;
    }
    HTTRACE(AUTH_TRACE, "Auth........ No challenges found\n");
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

    /* If no method for prompting the user then we might as well give up */
    if (!cbf) return HT_ERROR;

    /* Otherwise go ahead and ask the user */
    if (request) {
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
    me->algorithm = HTDaMD5;                   /* use md5 as a default value */
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
	    HT_FREE(digest->realm);
	    HT_FREE(digest->cnonce);
	    HT_FREE(digest->nonce);
	    HT_FREE(digest->opaque);
	    HT_FREE(digest->qop);
	    HT_FREE(digest);
	    return YES;
	}
	else
	    digest->references--;
    }
    return NO;
}

/*	HTDigest_reset
**	--------------
**      When digest authentication fails, we simulate a new digest by
**      erasing the old one, but keeping the uid and the password. This is
**      so that we can take into account the stale nonce protocol, without
**      prompting the user for a new password.
*/

PRIVATE int HTDigest_reset (HTDigest *digest)
{
    if (digest) {
	digest->nc = 0l;
	digest->stale = 0;
	digest->retry = YES;
	HT_FREE(digest->cnonce);
	HT_FREE(digest->nonce);
	HT_FREE(digest->opaque);
	HT_FREE(digest->qop);
	return YES;
    }
    else
	return NO;
}

/*	HTDigest_updateInfo
**	--------------
**      This function updates the digest with whatever new 
** 	authentification information the server sent back.
*/

PUBLIC int HTDigest_updateInfo (HTRequest *request, HTResponse *response,
				void * context, int status)
{
    HTAssocList * challenge = HTResponse_challenge(response);
    const char * realm =  HTRequest_realm (request);

    if (request && challenge && realm) {
        BOOL proxy = 0;
	char * value = NULL;
	char * token = NULL;
	char * auth_info = NULL;
	
	HTDigest *digest;
	char *url;

	/*
	** try to find the magic string in the challenge 
	*/
	HTTRACE(AUTH_TRACE, "Digest Update.. Processing authentication-info\n");
	if ((auth_info = HTAssocList_findObject(challenge, DIGEST_AI)))
	    proxy = 0;
	else if ((auth_info = HTAssocList_findObject(challenge, 
						     PROXY_DIGEST_AI)))
	    proxy = 1;
	else {
	    HTTRACE(AUTH_TRACE, "Digest Update.. Didn't find any authentication-info\n");
	    return HT_OK;
	}
    
	/* 
	** find the digest credentials 
	*/
	if (proxy) {
	    url = HTRequest_proxy(request);
	    digest = (HTDigest *) HTAA_updateNode (proxy, DIGEST_AUTH, realm,
						   url, NULL);
	} else {
	    url = HTAnchor_address((HTAnchor *)
				   HTRequest_anchor(request));
	    digest = (HTDigest *) HTAA_updateNode (proxy, DIGEST_AUTH, realm, 
						   url, NULL);
	    HT_FREE(url);
	}
	if (!digest) {
	    HTTRACE(AUTH_TRACE, "Digest Update.. Error: received authentication-info without having a local digest\n");	
	    return HT_ERROR;
	}

	/*
	**  Search through the set of parameters in the Authentication-info
	**  header.
	*/
	while ((token = HTNextField(&auth_info))) {
	    if (!strcasecomp(token, "nextnonce")) {
		if ((value = HTNextField(&auth_info))) {
		    HT_FREE (digest->nonce);
		    StrAllocCopy(digest->nonce, value);
		} else if (!strcasecomp(token, "qop")) {
		    value = HTNextField(&auth_info);
		    /* split, process  the qop, report errors */
		} else if (!strcasecomp(token, "rspauth")) {
		    value = HTNextField(&auth_info);
		    /* process rspauth */
		} else if (!strcasecomp(token, "cnonce")) {
		    value = HTNextField (&auth_info);
		    if (value && strcmp (digest->cnonce, value)) {
			/* print an alert?, bad cnonce? */
		    }	
		} else if (!strcasecomp(token, "nc")) {
		    value = HTNextField(&auth_info);
		    /* compare and printo some error? */
		}
	    }	
	}
    }
    return HT_OK;
}
    
/*
**    Simple function to add a parameter/value pair to a string
**
*/

PRIVATE BOOL add_param (char ** dest, char *param, char * value, BOOL quoted)
{
    char *tmp = *dest;

    if (!param || *param == '\0' || !value || *value == '\0')
	return NO;

    /* if there was a previous parameter, we add the next one in the
       following line */
    if (tmp) 
	StrAllocCat(tmp, ",");

    /* copy the new parameter and value */
    StrAllocCat(tmp, param);
    StrAllocCat(tmp, "=");
    if (quoted) {
    StrAllocCat(tmp, "\"");
    StrAllocCat(tmp, value);
    StrAllocCat(tmp, "\"");
    } else
	StrAllocCat(tmp, value);
    *dest = tmp;

    return YES;
}

/*
**  Code derived from draft-ietf-http-authentication-03 starts here
*/

PRIVATE void CvtHex (HASH Bin, HASHHEX Hex)
{
    unsigned short i;
    unsigned char j;

    for (i = 0; i < HASHLEN; i++) {
	j = (Bin[i] >> 4) & 0xf;
	if (j <= 9)
	    Hex[i*2] = (j + '0');
	else
	    Hex[i*2] = (j + 'a' - 10);
	j = Bin[i] & 0xf;
	if (j <= 9)
	    Hex[i*2+1] = (j + '0');
	else
	    Hex[i*2+1] = (j + 'a' - 10);
  }
    Hex[HASHHEXLEN] = '\0';
}

/* calculate H(A1) as per spec */
PRIVATE void DigestCalcHA1 (int algorithm, char * pszAlg, char * pszUserName,
			    char * pszRealm, char * pszPassword,
			    char * pszNonce, char * pszCNonce,
			    HASHHEX SessionKey)
{
    HTDigestContext MdCtx;
    HASH HA1;

    HTDigest_init (&MdCtx, algorithm);
    HTDigest_update (&MdCtx, pszUserName, strlen(pszUserName));
    HTDigest_update (&MdCtx, ":", 1);
    HTDigest_update (&MdCtx, pszRealm, strlen(pszRealm));
    HTDigest_update (&MdCtx, ":", 1);
    HTDigest_update (&MdCtx, pszPassword, strlen(pszPassword));
    HTDigest_final (HA1, &MdCtx);
    if (strcasecomp (pszAlg, "md5-sess") == 0) {
	HTDigest_init (&MdCtx, algorithm);
	HTDigest_update (&MdCtx, HA1, strlen (HA1));
	HTDigest_update (&MdCtx, ":", 1);
	HTDigest_update (&MdCtx, pszNonce, strlen(pszNonce));
	HTDigest_update (&MdCtx, ":", 1);
	HTDigest_update (&MdCtx, pszCNonce, strlen(pszCNonce));
	HTDigest_final (HA1, &MdCtx);
    }
    CvtHex (HA1, SessionKey);
}

/* calculate request-digest/response-digest as per HTTP Digest spec */
PRIVATE void DigestCalcResponse (
    int    algorithm,      /* message digest algorithm */
    HASHHEX HA1,           /* H(A1) */
    char * pszNonce,       /* nonce from server */
    char * pszNonceCount,  /* 8 hex digits */
    char * pszCNonce,      /* client nonce */
    char * pszQop,         /* qop-value: "", "auth", "auth-int" */
    char * pszMethod,      /* method from the request */
    char * pszDigestUri,   /* requested URL */
    char * HEntity,        /* H(entity body) if qop="auth-int" */
    char * Response        /* request-digest or response-digest */
    )
{
    HTDigestContext MdCtx;
    HASH HA2;
    HASH RespHash;
    HASHHEX HA2Hex;

    /* calculate H(A2) */

    HTDigest_init (&MdCtx, algorithm);
    HTDigest_update (&MdCtx, pszMethod, strlen(pszMethod));
    HTDigest_update (&MdCtx, ":", 1);
    HTDigest_update (&MdCtx, pszDigestUri, strlen(pszDigestUri));
    if (pszQop && strcasecomp (pszQop, "auth-int") == 0) {
	HTDigest_update (&MdCtx, ":", 1);
	HTDigest_update (&MdCtx, HEntity, HASHHEXLEN);
    }
    HTDigest_final (HA2, &MdCtx);
    CvtHex (HA2, HA2Hex);

    /* calculate response */
    HTDigest_init (&MdCtx, algorithm);
    HTDigest_update (&MdCtx, HA1, HASHHEXLEN);
    HTDigest_update (&MdCtx, ":", 1);
    HTDigest_update (&MdCtx, pszNonce, strlen(pszNonce));
    HTDigest_update (&MdCtx, ":", 1);
    if (pszQop && *pszQop) {
	HTDigest_update (&MdCtx, pszNonceCount, strlen(pszNonceCount));
	HTDigest_update (&MdCtx, ":", 1);
	HTDigest_update (&MdCtx, pszCNonce, strlen(pszCNonce));
	HTDigest_update (&MdCtx, ":", 1);
	HTDigest_update (&MdCtx, pszQop, strlen(pszQop));
	HTDigest_update (&MdCtx, ":", 1);
    }
    HTDigest_update (&MdCtx, HA2Hex, HASHHEXLEN);
    HTDigest_final (RespHash, &MdCtx);
    CvtHex (RespHash, Response);
}	

/*
**  Code derived from draft-ietf-http-authentication-03 ends here
*/

/*
**	Make digest authentication scheme credentials and register this
**	information in the request object as credentials. They will then
**	be included in the request header. An example is
**
**                 "Digest nonce:cnonce:blahblahblhah:digest-response"
**
**	The function can both create normal and proxy credentials
**	Returns	HT_OK or HT_ERROR
*/

PRIVATE BOOL digest_credentials (HTRequest * request, HTDigest * digest)
{
    if (request && digest && digest->realm)
    {
        char * realm = (char *) digest->realm;
	char * uri;
	char * method = (char *) HTMethod_name (HTRequest_method (request));
	char * cleartext = NULL;
	char nc[9];
	HASHHEX HA1;
        HASHHEX HA2;
	HASHHEX response;

	/* @@ maybe optimize all my reallocs by preallocating the memory */

	if (digest->proxy)
	    uri = HTRequest_proxy(request);
	else {
	     char * tmp;
	     /* we get the absolute URL */
	     tmp = HTAnchor_address( (HTAnchor*)HTRequest_anchor(request));
	     /* and then remove what makes it absolute, to be backwards
		compatible */
	     uri = HTParse (tmp, "", PARSE_PATH | PARSE_PUNCTUATION);
	     HT_FREE(tmp);
	}

	/* increment the nonce counter */
	digest->nc++;
	sprintf (nc, "%08lx", digest->nc);
	add_param (&cleartext, "username", digest->uid, YES);
	add_param (&cleartext, "realm", realm, YES);
	add_param (&cleartext, "nonce", digest->nonce, YES);
	add_param (&cleartext, "uri", uri, YES);
	/* @@@  no support for auth-int yet */
	if (digest->qop) {
	    add_param (&cleartext, "qop", "auth", NO);
	    add_param (&cleartext, "nc", nc, NO);
	    add_param (&cleartext, "cnonce", digest->cnonce, YES);
	    if (strcasecomp (digest->qop, "auth")) {
	      /* we got some options, as we only know how to handle auth, we'll do so.
	       We should split the options and see if it was a valid Qop here, though,
	       to things correctly */
	      HT_FREE (digest->qop);
	      StrAllocCopy (digest->qop, "auth");
	    }
	}
	/* compute the response digest */
	/* @@@ md5 hard coded, change it to something from the answer, 
	   md5-sess, etc */
	DigestCalcHA1 (digest->algorithm, "md5", digest->uid, realm, digest->pw, digest->nonce,
		       digest->cnonce, HA1);
	DigestCalcResponse (digest->algorithm, HA1, digest->nonce, nc, digest->cnonce,
			    digest->qop, method, uri, HA2, response);
	add_param (&cleartext, "response", response, NO);
	add_param (&cleartext, "opaque", digest->opaque, NO);

	/* Create the credentials and assign them to the request object */
	{
 	    int cr_len = strlen ("Digest") + strlen (cleartext) + 3;
	    char * cookie = (char *) HT_MALLOC(cr_len+1);
	    if (!cookie) HT_OUTOFMEM("digest_credentials");
	    strcpy(cookie, "Digest ");
	    strcat (cookie, cleartext);
	    HTTRACE(AUTH_TRACE, "Digest Cookie `%s\'\n" _ cookie);

	    /* Check whether it is proxy or normal credentials */
	    if (digest->proxy)
		HTRequest_addCredentials(request, "Proxy-Authorization",
					 cookie);
	    else
		HTRequest_addCredentials(request, "Authorization", cookie);

	    HT_FREE(cookie);
	}
	if (!digest->proxy)
	  HT_FREE(uri);
	HT_FREE(cleartext);
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
	    digest = HTDigest_new();
	    if (proxy) {
		char * url = HTRequest_proxy(request);
		digest->proxy = YES;
		HTAA_updateNode(proxy, DIGEST_AUTH, realm, url, digest);
	    } else {
		char * url = HTAnchor_address((HTAnchor*)HTRequest_anchor(request));
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
	/* @@@ here we should generate a new cnonce value */
	    HTSACopy (&(digest->cnonce), "012345678");
	    digest->retry = NO;
	    return digest_credentials(request, digest);
	} else {
	    char * url = HTAnchor_address((HTAnchor*)HTRequest_anchor(request));
	    if (proxy)
		HTAA_deleteNode(proxy, DIGEST_AUTH, realm, url);
	    else
		HTAA_deleteNode(proxy, DIGEST_AUTH, realm, url);
	    HT_FREE(url);
	    return HT_ERROR;
	}
    }
    return HT_OK;
}

/*
**	Evaluates the existing authentication info (nonce, uid, pwd) and
**      returns TRUE if we evaluate that the nonce is stale, FALSE
**      otherwise.
*/
PRIVATE BOOL nonce_is_stale (HTRequest *request, HTDigest * digest, char * old_nonce)
{
  if (!digest->uid || !digest->pw)
      return FALSE;
  if (!digest->nonce || !old_nonce)
     return FALSE;
  if (strcmp (digest->nonce, old_nonce))
     return TRUE;
  /* because of a pipelining implementation bug, we don't send any good
     credentials on requests following the first one in the pipeline  */
  if (!HTRequest_credentials (request) && HTRequest_AAretrys (request) == 1)
     return TRUE;
  
  return FALSE;
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
	char * rm    =  HTNextField(&p);
	char * value = NULL;
	char * token = NULL;
	char * uris = NULL;
	/* the value of the previous nonce in case the server has changed its
	   challenge */
	char * old_nonce = NULL;

	/*
	** If valid challenge then make a template for the resource and
	** store this information in our authentication URL Tree
	*/
	if (realm && !strcasecomp(realm, "realm") && rm) {
	    HTTRACE(AUTH_TRACE, "Digest Parse. Realm `%s\' found\n" _ rm);
	    HTRequest_setRealm(request, rm);

	    /*
	    **  If we are in proxy mode then add the proxy - not the final URL
	    */
	    if (proxy) {
		char * url = HTRequest_proxy(request);
		HTTRACE(AUTH_TRACE, "Digest Parse. Proxy authentication\n");
		digest = (HTDigest *) HTAA_updateNode(proxy, DIGEST_AUTH, rm,
						      url, NULL);
		/* if the previous authentication failed, then try again */
		if (HTRequest_AAretrys (request) > 1 
		    && status == HT_NO_ACCESS && digest)
		  digest->retry = YES;
	    } else {
		char * url = HTAnchor_address((HTAnchor *)
					      HTRequest_anchor(request));
		char * tmplate = make_template(url);
		digest = (HTDigest *) HTAA_updateNode(proxy, DIGEST_AUTH, rm,
						      tmplate, NULL);
		/* if the previous authentication failed, then try again */
		if (HTRequest_AAretrys (request) > 1 
		    && status == HT_NO_ACCESS && digest)
		  digest->retry = YES;
		HT_FREE(tmplate);
		HT_FREE(url);
	    }
	} else {
	    HTTRACE(AUTH_TRACE, "Digest Parse. Missing or incomplete realm\n");
	    return HT_ERROR;
	}


	/* if we get here it's because there's no digest */
	/* we parse the digest parameters from the challenge */

	if (digest) {
	    /* it's an old digest, so we clean all in it except for the
	       uid and the password, hoping that the server send back
	       that data */
	    old_nonce = digest->nonce;
	    digest->nonce = NULL;
	    HTDigest_reset (digest);
	} else {
	    /* it's a brand new digest */
	    digest = HTDigest_new();
	    StrAllocCopy (digest->realm, rm);
	}

	/*
	**  Search through the set of parameters in the digest header.
	**  If valid challenge then make a template for the resource and
	**  store this information in our authentication URL Tree
	*/
	while ((token = HTNextField(&p))) {
	    if (!strcasecomp(token, "domain")) {
		if ((value = HTNextField(&p)))
		    uris = value;
	    } else if (!strcasecomp(token, "nonce")) {
		if ((value = HTNextField(&p)))
		    StrAllocCopy(digest->nonce, value);
	    } else if (!strcasecomp(token, "opaque")) {
		if ((value = HTNextField(&p)))
		    StrAllocCopy(digest->opaque, value);
	    } else if (!strcasecomp(token, "qop")) {
		/* split the qop */
		if ((value = HTNextField(&p)))
		    StrAllocCopy(digest->qop, value);
	    } else if (!strcasecomp(token, "stale")) {
		if ((value = HTNextField(&p)) && !strcasecomp(value, "true")) {
		    /* only true if we already had a digest with uid and pw info */
		    if (digest->uid && digest->pw) {
			digest->stale = YES;		
		    }
		}
	    } else if (!strcasecomp(token, "algorithm")) {
		if ((value = HTNextField(&p)) && strcasecomp(value, "md5")) {
		    /*
		    **  We only support MD5 for the moment
		    */
		    HTTRACE(AUTH_TRACE, "Digest Parse Unknown algorithm `%s\'\n" _ value);
		    HTDigest_delete(digest);
		    if (old_nonce)
		      HT_FREE (old_nonce);
		    return HT_ERROR;
		} else
		    digest->algorithm = HTDaMD5;
	    }
	}

	/* Pipelining support. If a nonce becomes stale When sending 
	** several requests thru the pipeline, we may miss the stale
	** reply in the server's answer. To avoid this, we keep a copy
	** of the nonce in each request. If the nonce wasn't explicitly
	** marked stale and if it's the same that we sent, then we 
	** consider that the uid/pwd pairs were false. Otherwise, we
	** assume the stole went stale before 
	*/
	if (!digest->stale && nonce_is_stale (request, digest, old_nonce))
	    digest->stale = YES;

	if (old_nonce)
	  HT_FREE (old_nonce);

	if (digest->stale) {
	    digest->stale = NO;
	    digest->retry = NO;
	    return HT_OK;
	}
	else if (digest->uid || digest->pw) {
	    /*
	    ** For some reason there was no stale nonce header and the
	    ** authentication failed so we have to ask the user if we should
	    ** try again. It may be because the user typed the wrong user name
	    ** and password
	    */
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
		return HT_OK;
	    }
	    return HT_ERROR;
	}

	/*
	** It's the first time we go this way, so we check the domain field to 
	** create the digest node entries for each URI.
	*/
	if (!uris) {
	    if (proxy) {
		/* we ignore the domain */
		char * location = HTRequest_proxy(request);
		HTTRACE(AUTH_TRACE, "Digest Parse Proxy authentication\n");
		HTAA_updateNode(proxy, DIGEST_AUTH, rm, location, digest);
	    } else {
		char * url = HTAnchor_address((HTAnchor *) HTRequest_anchor(request));
		char * tmplate = make_template(url);
		HTAA_updateNode(proxy, DIGEST_AUTH, rm, tmplate, digest);
		HT_FREE(url);
		HT_FREE(tmplate);
	    }
	} else {
	    char * base_url =
		HTAnchor_address((HTAnchor *) HTRequest_anchor(request));
	    char * domain_url;
	    char * full_url;

	    while ((domain_url = HTNextField (&uris))) {
		/* complete the URL if it's an absolute one */
		full_url = HTParse (domain_url, base_url, PARSE_ALL);
		digest->references++;
		if (proxy) {
		    HTTRACE(AUTH_TRACE, "Digest Parse Proxy authentication\n");
		    HTAA_updateNode(proxy, DIGEST_AUTH, rm, full_url, digest);
		} else {
		    char * tmplate = make_template(full_url);
		    HTAA_updateNode (proxy, DIGEST_AUTH, rm, tmplate, digest);
		    HT_FREE (tmplate);
		}
		HT_FREE (full_url);
	    }
	    HT_FREE (base_url);
	}
	return HT_OK;
    }	
    HTTRACE(AUTH_TRACE, "Auth........ No challenges found\n");
    return HT_ERROR;
}
