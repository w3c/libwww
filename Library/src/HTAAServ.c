
/* MODULE							HTAAServ.c
**		SERVER SIDE ACCESS AUTHORIZATION MODULE
**
**	Contains the means for checking the user access
**	authorization for a file.
**
** IMPORTANT:
**	Routines in this module use dynamic allocation, but free
**	automatically all the memory reserved by them.
**
**	Therefore the caller never has to (and never should)
**	free() any object returned by these functions.
**
**	Therefore also all the strings returned by this package
**	are only valid until the next call to the same function
**	is made. This approach is selected, because of the nature
**	of access authorization: no string returned by the package
**	needs to be valid longer than until the next call.
**
**	This also makes it easy to plug the AA package in:
**	you don't have to ponder whether to free() something
**	here or is it done somewhere else (because it is always
**	done somewhere else).
**
**	The strings that the package needs to store are copied
**	so the original strings given as parameters to AA
**	functions may be freed or modified with no side effects.
**
**	The AA package does not free() anything else than what
**	it has itself allocated.
**
** AUTHORS:
**	AL	Ari Luotonen	luotonen@dxcern.cern.ch
**
** HISTORY:
**
**
** BUGS:
**
**
*/

#include <stdio.h>		/* FILE */
#include <string.h>		/* strchr() */

#include "HTUtils.h"
#include "HTString.h"
#include "HTAccess.h"		/* HTSecure			*/
#include "HTFile.h"		/* HTLocalName			*/
#include "HTRules.h"		/* 				*/
#include "HTParse.h"		/* URL parsing function		*/
#include "HTList.h"		/* HTList object		*/

#include "HTAAUtil.h"		/* AA common parts		*/
#include "HTAuth.h"		/* Authentication		*/
#include "HTACL.h"		/* Access Control List		*/
#include "HTGroup.h"		/* Group handling		*/
#include "HTAAProt.h"		/* Protection file parsing	*/
#include "HTAAServ.h"		/* Implemented here		*/



/*
** Global variables
*/
PUBLIC time_t theTime;


/*
** Module-wide global variables
*/
PRIVATE FILE *  htaa_logfile	    = NULL; 		/* Log file */



/* SERVER PUBLIC					HTAA_statusMessage()
**		RETURN A STRING EXPLAINING ACCESS
**		AUTHORIZATION FAILURE
**		(Can be used in server reply status line
**		 with 401/403/404 replies.)
** ON ENTRY:
**	req	failing request.
** ON EXIT:
**	returns	a string containing the error message
**		corresponding to internal fail reason.
**		Sets the message also to req->reason_line.
*/
PUBLIC char *HTAA_statusMessage ARGS1(HTRequest *, req)
{
    if (!req)
	return "Internal error, request structure pointer is NULL!";

    if (req->reason_line)
	return req->reason_line;

    switch (req->reason) {

    /* 401 cases */
      case HTAA_NO_AUTH:
	req->reason_line = "Unauthorized -- authentication failed";
	break;
      case HTAA_NOT_MEMBER:
	req->reason_line = "Unauthorized to access the document";
	break;

    /* 403 cases */
      case HTAA_BY_RULE:
	req->reason_line = "Forbidden -- by rule";
	break;
      case HTAA_IP_MASK:
	req->reason_line = "Forbidden -- server refuses to serve to your IP address";
	break;
      case HTAA_NO_ACL:
      case HTAA_NO_ENTRY:
	req->reason_line = "Forbidden -- access to file is never allowed";
	break;
      case HTAA_SETUP_ERROR:
	req->reason_line = "Forbidden -- server protection setup error";
	break;
      case HTAA_DOTDOT:
	req->reason_line = "Forbidden -- URL containing /../ disallowed";
	break;
      case HTAA_HTBIN:
	req->reason_line = "Forbidden -- /htbin feature not enabled on this server";
	break;

    /* 404 cases */
      case HTAA_NOT_FOUND:
	req->reason_line = "Not found -- file doesn't exist or is read protected";
	break;

    /* Success */
      case HTAA_OK:
	req->reason_line = "AA: Access should be ok but something went wrong"; 
	break;

      case HTAA_OK_GATEWAY:
	req->reason_line = "AA check bypassed (gatewaying) but something went wrong";
	break;

    /* Others */
      default:
	req->reason_line = "Access denied -- unable to specify reason (bug)";

    } /* switch */

    return req->reason_line;
}



#ifdef OLD_CODE
PRIVATE char *status_name ARGS1(HTAAFailReason, reason)
{
    switch (reason) {

    /* 401 cases */
      case HTAA_NO_AUTH:
	return "NO-AUTHENTICATION";
	break;
      case HTAA_NOT_MEMBER:
	return "NOT-AUTHORIZED";
	break;

    /* 403 cases */
      case HTAA_BY_RULE:
	return "FORB-RULE";
	break;
      case HTAA_IP_MASK:
	return "FORB-IP";
	break;
      case HTAA_NO_ACL:
	return "NO-ACL-FILE";
	break;
      case HTAA_NO_ENTRY:
	return "NO-ACL-ENTRY";
	break;
      case HTAA_SETUP_ERROR:
	return "SETUP-ERROR";
	break;
      case HTAA_DOTDOT:
	return "SLASH-DOT-DOT";
	break;
      case HTAA_HTBIN:
	return "HTBIN-OFF";
	break;

    /* 404 cases */
      case HTAA_NOT_FOUND:
	return "NOT-FOUND";
	break;

    /* Success */
      case HTAA_OK:
	return "OK";
	break;
      case HTAA_OK_GATEWAY:
	return "OK-GATEWAY";
	break;

    /* Others */
      default:
	return "SERVER-BUG";
    } /* switch */
}
#endif
    




/* PRIVATE						check_authorization()
**		CHECK IF USER IS AUTHORIZED TO ACCESS A FILE
** ON ENTRY:
**	req	data structure representing the request.
**
** ON EXIT:
**	returns	HTAA_OK on success.
**		Otherwise the reason for failing.
** NOTE:
**	This function does not check whether the file
**	exists or not -- so the status  404 Not found
**	must be returned from somewhere else (this is
**	to avoid unnecessary overhead of opening the
**	file twice).
*/
PRIVATE HTAAFailReason check_authorization ARGS1(HTRequest *, req)
{
    HTAAFailReason reason;
    GroupDef *allowed_groups;
    FILE *acl_file = NULL;
    char *check_this;

    req->user = NULL;

    if (req->script) {
	check_this = req->script;
	fprintf(stderr,
		"check_authorization: translated script name: `%s'\n",
		req->script);
    }
    else if (req->translated) {
	check_this = req->translated;
	fprintf(stderr,
		"check_authorization: translated path: `%s' %s `%s'\n",
		req->translated, "method:", HTAtom_name(req->method));
    }
    else {
	if (TRACE) fprintf(stderr,
			   "HTAA_checkAuthorization: Forbidden by rule\n");
	return HTAA_BY_RULE;
    }

    /*
    ** Check ACL existence
    */
    if (!(acl_file = HTAA_openAcl(check_this))) {
	if (req->prot) { /* protect rule, but no ACL */
	    if (req->prot->mask_group) {
		/*
		** Only mask enabled, check that
		*/
		GroupDefList *group_def_list =
		    HTAA_readGroupFile(HTAssocList_lookup(req->prot->values,
							  "group"));
		/*
		** Authenticate if authentication info given
		*/
		if (req->scheme != HTAA_UNKNOWN  && req->scheme != HTAA_NONE) {
		    HTAA_authenticate(req);
		    if (TRACE) fprintf(stderr, "Authentication returned: %s\n",
				       (req->user ? req->user->username
					          : "NOT-AUTHENTICATED"));
		}
		HTAA_resolveGroupReferences(req->prot->mask_group,
					    group_def_list);
		reason = HTAA_userAndInetInGroup(req->prot->mask_group,
						 req->user
						  ? req->user->username : "",
						 HTClientHost,
						 NULL);
		if (TRACE) {
		    if (reason != HTAA_OK)
			fprintf(stderr, "%s %s %s %s\n",
				"HTAA_checkAuthorization: access denied",
				"by mask (no ACL, only Protect rule)",
				"host", HTClientHost);
		    else fprintf(stderr, "%s %s %s %s\n",
				 "HTAA_checkAuthorization: request from",
				 HTClientHost, 
				 "accepted by only mask match (no ACL, only",
				 "Protect rule, and only mask enabled)");
		}
		return reason;
	    }
	    else {	/* 403 Forbidden */
		if (TRACE) fprintf(stderr, "%s %s\n",
				   "HTAA_checkAuthorization: Protected, but",
				   "no mask group nor ACL -- forbidden");
		return HTAA_NO_ACL;
	    }
	}
	else { /* No protect rule and no ACL => OK 200 */
	    if (TRACE) fprintf(stderr, "HTAA_checkAuthorization: %s\n",
			       "no protect rule nor ACL -- ok\n");
	    return HTAA_OK;
	}
    }

    /*
    ** Now we know that ACL exists
    */
    if (!req->prot) {		/* Not protected by "protect" rule */
	if (TRACE) fprintf(stderr,
			   "HTAA_checkAuthorization: default protection\n");
	req->prot = req->def_prot;  /* Get default protection */

	if (!req->prot) {	/* @@ Default protection not set ?? */
	    if (TRACE) fprintf(stderr, "%s %s\n",
			       "HTAA_checkAuthorization: default protection",
			       "not set (internal server error)!!");
	    return HTAA_SETUP_ERROR;
	}
    }

    /*
    ** Now we know that document is protected and ACL exists.
    ** Check against ACL entry.
    */
    {
	GroupDefList *group_def_list =
	    HTAA_readGroupFile(HTAssocList_lookup(req->prot->values, "group"));

	/*
	** Authenticate now that we know protection mode
	*/
	if (req->scheme != HTAA_UNKNOWN  &&  req->scheme != HTAA_NONE) {
	    HTAA_authenticate(req);
	    if (TRACE) fprintf(stderr, "Authentication returned: %s\n",
			       (req->user
				? req->user->username : "NOT-AUTHENTICATED"));
	}
	/* 
	** Check mask group
	*/
	if (req->prot->mask_group) {
	    HTAA_resolveGroupReferences(req->prot->mask_group, group_def_list);
	    reason=HTAA_userAndInetInGroup(req->prot->mask_group,
					   req->user ? req->user->username :"",
					   HTClientHost,
					   NULL);
	    if (reason != HTAA_OK) {
		if (TRACE) fprintf(stderr, "%s %s %s\n",
				   "HTAA_checkAuthorization: access denied",
				   "by mask, host:", HTClientHost);
		return reason;
	    }
	    else {
		if (TRACE) fprintf(stderr, "%s %s %s %s %s\n",
				   "HTAA_checkAuthorization: request from",
				   HTClientHost, 
				   "accepted by just mask group match",
				   "(no ACL, only Protect rule, and only",
				   "mask enabled)");
		/* And continue authorization checking */
	    }
	}
	/*
        ** Get ACL entries; get first one first, the loop others
	** Remember, allowed_groups is automatically freed by
	** HTAA_getAclEntry().
	*/
	allowed_groups =
	    HTAA_getAclEntry(acl_file, check_this, req->method);
	if (!allowed_groups) {
	    if (TRACE) fprintf(stderr, "%s `%s' %s\n",
			       "No entry for file", check_this, "in ACL");
	    HTAA_closeAcl(acl_file);
	    return HTAA_NO_ENTRY;	/* Forbidden -- no entry in the ACL */
	}
	else {
	    do {
		HTAA_resolveGroupReferences(allowed_groups, group_def_list);
		reason = HTAA_userAndInetInGroup(allowed_groups,
						 req->user
						 ? req->user->username : "",
						 HTClientHost,
						 NULL);
		if (reason == HTAA_OK) {
		    HTAA_closeAcl(acl_file);
		    return HTAA_OK;	/* OK */
		}
		allowed_groups =
		    HTAA_getAclEntry(acl_file, check_this, req->method);
	    } while (allowed_groups);
	    HTAA_closeAcl(acl_file);
	    return HTAA_NOT_MEMBER;	/* Unauthorized */
	}
    }
}



/* PUBLIC					      HTAA_checkAuthorization()
**		CHECK IF USER IS AUTHORIZED TO ACCESS A FILE
** ON ENTRY:
**	req	data structure representing the request.
**
** ON EXIT:
**	returns	status codes uniform with those of HTTP:
**	  200 OK	   if file access is ok.
**	  401 Unauthorized if user is not authorized to
**			   access the file.
**	  403 Forbidden	   if there is no entry for the
**			   requested file in the ACL.
**
** NOTE:
**	This function does not check whether the file
**	exists or not -- so the status  404 Not found
**	must be returned from somewhere else (this is
**	to avoid unnecessary overhead of opening the
**	file twice).
**
*/
PUBLIC int HTAA_checkAuthorization ARGS1(HTRequest *, req)
{
    if (!req  ||  !req->argument  ||  req->reason != HTAA_OK) {
	if (req && req->status_code == 200)
	    req->status_code = 403;
	if (req && !req->reason_line)
	    HTAA_statusMessage(req);
	return 403;
    }

#ifdef THIS_IS_DONE_BY_DAEMON
    /*
    ** Parse Authorization: field
    */
    if (req->authorization) {
	char *tmp = strdup(req->authorization);
	char *scheme_specifics = tmp;
	char *scheme_name = HTNextField(&scheme_specifics);

	req->scheme = HTAAScheme_enum(HTStrip(scheme_name));
	StrAllocCopy(req->auth_string, HTStrip(scheme_specifics));
	free(tmp);
    }
    else req->scheme = HTAA_NONE;
#endif

    /*
    ** Translate into absolute pathname, and
    ** get protection by "protect" and "defprot" rules.
    */
    if (HTTranslateReq(req)) {
	if (req->script) {	/* Script request */
	    req->reason = check_authorization(req);
	}
	else {			/* Not a script request */
	    char *access = HTParse(req->translated, "", PARSE_ACCESS);
	    if (!*access || 0 == strcmp(access,"file")) {
		/* Local file, do AA check */
		if (!HTSecure) {
		    char *localname = HTLocalName(req->translated);
		    free(req->translated);
		    req->translated = localname;
		}
		req->reason = check_authorization(req);
	    }
	    else {  /* Not local access */
		req->reason = HTAA_OK_GATEWAY;
		fprintf(stderr,
			"HTAA_checkAuthorization: %s (%s access)\n",
			"Gatewaying -- skipping authorization check",
			access);
	    }
	} /* Not a script request */
    } /* if translation succeeded */

#ifdef OLD_CODE_LETS_DO_THIS_OFFICIALLY_NOW
    if (htaa_logfile) {
	time(&theTime);
	fprintf(htaa_logfile, "%24.24s %s %s %s %s %s\n",
		ctime(&theTime),
		HTClientHost ? HTClientHost : "local",
		HTMethod_name(req->method),
		req->argument,
		status_name(reason),
		req->user && req->user->username
		? req->user->username : "");
	fflush(htaa_logfile);	/* Actually update it on disk */
	if (TRACE) fprintf(stderr, "Log: %24.24s %s %s %s %s %s\n",
			   ctime(&theTime),
			   HTClientHost ? HTClientHost : "local",
			   HTMethod_name(req->method),
			   req->argument,
			   status_name(reason),
			   req->user && req->user->username
			   ? req->user->username : "");
    }
#endif /* OLD_CODE */

    switch (req->reason) {

      case HTAA_NO_AUTH:
      case HTAA_NOT_MEMBER:
	req->status_code = 401;
	break;

      case HTAA_BY_RULE:
      case HTAA_IP_MASK:
      case HTAA_NO_ACL:
      case HTAA_NO_ENTRY:
      case HTAA_SETUP_ERROR:
      case HTAA_DOTDOT:
      case HTAA_HTBIN:
	req->status_code = 403;
	break;

      case HTAA_NOT_FOUND:
	req->status_code = 404;
	break;

      case HTAA_OK:
      case HTAA_OK_GATEWAY:
	req-> status_code = 200;
	break;

      default:
	req->status_code = 500;
    } /* switch */

    HTAA_statusMessage(req);
    return req->status_code;
}





/* PRIVATE					compose_scheme_specifics()
**		COMPOSE SCHEME-SPECIFIC PARAMETERS
**		TO BE SENT ALONG WITH SERVER REPLY
**		IN THE WWW-Authenticate: FIELD.
** ON ENTRY:
**	scheme		is the authentication scheme for which
**			parameters are asked for.
**	prot		protection setup structure.
**
** ON EXIT:
**	returns		scheme specific parameters in an
**			auto-freed string.
*/
PRIVATE char *compose_scheme_specifics ARGS2(HTAAScheme,	scheme,
					     HTAAProt *,	prot)
{
    static char *result = NULL;

    FREE(result);	/* From previous call */

    switch (scheme) {
      case HTAA_BASIC:
	{
	    char *realm = HTAssocList_lookup(prot->values, "server");
	    result = (char*)malloc(60);
	    sprintf(result, "realm=\"%s\"",
		    (realm ? realm : "UNKNOWN"));
	    return result;
	}
	break;

      case HTAA_PUBKEY:
	{
	    char *realm = HTAssocList_lookup(prot->values, "server");
	    result = (char*)malloc(200);
	    sprintf(result, "realm=\"%s\", key=\"%s\"",
		    (realm ? realm : "UNKNOWN"),
		    "PUBKEY-NOT-IMPLEMENTED");
	    return result;
	}
	break;
      default:
	return NULL;
    }
}


/* SERVER PUBLIC				    HTAA_composeAuthHeaders()
**		COMPOSE WWW-Authenticate: HEADER LINES
**		INDICATING VALID AUTHENTICATION SCHEMES
**		FOR THE REQUESTED DOCUMENT
** ON ENTRY:
**	req	request structure.
**
** ON EXIT:
**	returns	a buffer containing all the WWW-Authenticate:
**		fields including CRLFs (this buffer is auto-freed).
**		NULL, if authentication won't help in accessing
**		the requested document.
**
*/
PUBLIC char *HTAA_composeAuthHeaders ARGS1(HTRequest *, req)
{
    static char *result = NULL;
    HTAAScheme scheme;
    char *scheme_name;
    char *scheme_params;

    if (!req) return NULL;

    if (!req->prot) {
	if (TRACE) fprintf(stderr, "%s %s\n",
			   "HTAA_composeAuthHeaders: Document not protected",
			   "-- why was this function called??");
	return NULL;
    }
    else if (TRACE) fprintf(stderr, "HTAA_composeAuthHeaders: for file `%s'\n",
			    (req->translated
			     ? req->translated : "(null)"));

    FREE(result);	/* From previous call */
    if (!(result = (char*)malloc(4096)))	/* @@ */
	outofmem(__FILE__, "HTAA_composeAuthHeaders");
    *result = (char)0;

    for (scheme=0; scheme < HTAA_MAX_SCHEMES; scheme++) {
	if (-1 < HTList_indexOf(req->prot->valid_schemes, (void*)scheme)) {
	    if ((scheme_name = HTAAScheme_name(scheme))) {
		scheme_params = compose_scheme_specifics(scheme,req->prot);
		strcat(result, "WWW-Authenticate: ");
		strcat(result, scheme_name);
		if (scheme_params) {
		    strcat(result, " ");
		    strcat(result, scheme_params);
		}
		strcat(result, "\r\n");
	    } /* scheme name found */
	    else if (TRACE) fprintf(stderr, "HTAA_composeAuthHeaders: %s %d\n",
				    "No name found for scheme number", scheme);
	} /* scheme valid for requested document */
    } /* for every scheme */
    
    return result;
}



#ifdef OLD_CODE_LETS_DO_THIS_OFFICIALLY_NOW
/* PUBLIC						HTAA_startLogging()
**		START UP ACCESS AUTHORIZATION LOGGING
** ON ENTRY:
**	fp	is the open log file.
**
*/
PUBLIC void HTAA_startLogging ARGS1(FILE *, fp)
{
    htaa_logfile = fp;
}
#endif /*OLD_CODE*/

