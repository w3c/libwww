/* 								     HTAABrow.c
**		BROWSER SIDE ACCESS AUTHORIZATION MODULE
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	Containts the code for keeping track on server hostnames,
**	port numbers, scheme names, usernames, passwords
**	(and servers' public keys).
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
**	Oct 17	AL	Made corrections suggested by marca:
**			Added  if (!realm->username) return NULL;
**			Changed some ""s to NULLs.
**			Now doing calloc() to init uuencode source;
**			otherwise HTUU_encode() reads uninitialized memory
**			every now and then (not a real bug but not pretty).
**			Corrected the formula for uuencode destination size.
** BUGS:
**
**
*/

/* Library include files */
#include "WWWLib.h"
#include "HTReqMan.h"	/* @@@@ */
#include "HTAAUtil.h"
#include "HTAABrow.h"					 /* Implemented here */

PRIVATE HTList *server_table	= NULL;	/* Browser's info about servers	     */

/**************************** HTAAServer ***********************************/


/* PRIVATE						HTAAServer_new()
**		ALLOCATE A NEW NODE TO HOLD SERVER INFO
**		AND ADD IT TO THE LIST OF SERVERS
** ON ENTRY:
**	hostname	is the name of the host that the server
**			is running in.
**	portnumber	is the portnumber which the server listens.
**
** ON EXIT:
**	returns		the newly-allocated node with all the strings
**			duplicated.
**			Strings will be automatically freed by
**			the function HTAAServer_delete(), which also
**			frees the node itself.
*/
PRIVATE HTAAServer *HTAAServer_new (CONST char * hostname, int portnumber)
{
    HTAAServer *server;

    if ((server = (HTAAServer *) HT_MALLOC(sizeof(HTAAServer))) == NULL)
        HT_OUTOFMEM("HTAAServer_new");

    server->hostname	= NULL;
    server->portnumber	= (portnumber > 0 ? portnumber : 80);
    server->setups	= HTList_new();
    server->realms	= HTList_new();

    if (hostname) StrAllocCopy(server->hostname, hostname);

    if (!server_table) server_table = HTList_new();
    
    HTList_addObject(server_table, (void*)server);

    return server;
}


/* PRIVATE						HTAAServer_lookup()
**		LOOK UP SERVER BY HOSTNAME AND PORTNUMBER
** ON ENTRY:
**	hostname	obvious.
**	portnumber	if non-positive defaults to 80.
**
**	Looks up the server in the module-global server_table.
**
** ON EXIT:
**	returns		pointer to a HTAAServer structure
**			representing the looked-up server.
**			NULL, if not found.
*/
PRIVATE HTAAServer *HTAAServer_lookup (CONST char * hostname, int portnumber)
{
    if (hostname) {
	HTList *cur = server_table;
	HTAAServer *server;

	if (portnumber <= 0) portnumber = 80;

	while (NULL != (server = (HTAAServer*)HTList_nextObject(cur))) {
	    if (server->portnumber == portnumber  &&
		0==strcmp(server->hostname, hostname))
		return server;
	}
    }
    return NULL;	/* NULL parameter, or not found */
}




/*************************** HTAASetup *******************************/    


/* PRIVATE						HTAASetup_lookup()
**	FIGURE OUT WHICH AUTHENTICATION SETUP THE SERVER
**	IS USING FOR A GIVEN FILE ON A GIVEN HOST AND PORT
**
** ON ENTRY:
**	hostname	is the name of the server host machine.
**	portnumber	is the port that the server is running in.
**	docname		is the (URL-)pathname of the document we
**			are trying to access.
**
** 	This function goes through the information known about
**	all the setups of the server, and finds out if the given
**	filename resides in one of the protected directories.
**
** ON EXIT:
**	returns		NULL if no match.
**			Otherwise, a HTAASetup structure representing
**			the protected server setup on the corresponding
**			document tree.
**			
*/
PRIVATE HTAASetup *HTAASetup_lookup (CONST char * hostname,
				     int	  portnumber,
				     CONST char * docname)
{
    HTAAServer *server;
    HTAASetup *setup;

    if (portnumber <= 0) portnumber = 80;

    if (hostname && docname && *hostname && *docname &&
	NULL != (server = HTAAServer_lookup(hostname, portnumber))) {

	HTList *cur = server->setups;

	if (PROT_TRACE)
	    TTYPrint(TDEST, "Access Auth. resolving setup for (%s:%d:%s)\n",
		    hostname, portnumber, docname);

	while (NULL != (setup = (HTAASetup*)HTList_nextObject(cur))) {
	    if (HTAA_templateMatch(setup->tmplate, docname)) {
		if (PROT_TRACE)
		    TTYPrint(TDEST, "Access Auth. `%s' matched template `%s'\n",
			    docname, setup->tmplate);
		return setup;
	    }
	    else if (PROT_TRACE)
		TTYPrint(TDEST,"%s `%s' %s `%s'\n","HTAASetup_lookup:", docname,
			"did NOT match template", setup->tmplate);
	} /* while setups remain */
    } /* if valid parameters and server found */

    if (PROT_TRACE)
	TTYPrint(TDEST, "Access Auth. `%s' (so probably not protected)\n",
		(docname ? docname : "(null)"));
    return NULL;			 /* NULL in parameters, or not found */
}




/* PRIVATE						HTAASetup_new()
**			CREATE A NEW SETUP NODE
** ON ENTRY:
**	server		is a pointer to a HTAAServer structure
**			to which this setup belongs.
**	template	documents matching this template
**			are protected according to this setup.
**	valid_schemes	a list containing all valid authentication
**			schemes for this setup.
**			If NULL, all schemes are disallowed.
**	scheme_specifics is an array of assoc lists, which
**			contain scheme specific parameters given
**			by server in Authenticate: fields.
**			If NULL, all scheme specifics are
**			set to NULL.
** ON EXIT:
**	returns		a new HTAASetup node, and also adds it as
**			part of the HTAAServer given as parameter.
*/
PRIVATE HTAASetup *HTAASetup_new (HTAAServer *	server,
				  char *	tmplate,
				  HTList *	valid_schemes,
				  HTAssocList **scheme_specifics)
{
    HTAASetup *setup;

    if (!server || !tmplate || !*tmplate) return NULL;

    if ((setup = (HTAASetup *) HT_MALLOC(sizeof(HTAASetup))) == NULL)
        HT_OUTOFMEM("HTAASetup_new");

    setup->reprompt = NO;
    setup->server = server;
    setup->tmplate = NULL;
    if (tmplate) StrAllocCopy(setup->tmplate, tmplate);
    setup->valid_schemes = valid_schemes;
    setup->scheme_specifics = scheme_specifics;

    HTList_addObject(server->setups, (void*)setup);

    return setup;
}



/* PRIVATE						HTAASetup_delete()
**			FREE A HTAASetup STRUCTURE
** ON ENTRY:
**	killme		is a pointer to the structure to free;
**
** ON EXIT:
**	returns		nothing.
*/
#ifdef NOT_NEEDED_IT_SEEMS
PRIVATE void HTAASetup_delete (HTAASetup * killme)
{
    int scheme;

    if (killme) {
	if (killme->tmplate) HT_FREE(killme->tmplate);
	if (killme->valid_schemes)
	    HTList_delete(killme->valid_schemes);
	for (scheme=0; scheme < HTAA_MAX_SCHEMES; scheme++)
	    if (killme->scheme_specifics[scheme])
		HTAssocList_delete(killme->scheme_specifics[scheme]);
	HT_FREE(killme);
    }
}
#endif /*NOT_NEEDED_IT_SEEMS*/



/* PRIVATE					HTAASetup_updateSpecifics()
*		COPY SCHEME SPECIFIC PARAMETERS
**		TO HTAASetup STRUCTURE
** ON ENTRY:
**	setup		destination setup structure.
**	specifics	string array containing scheme
**			specific parameters for each scheme.
**			If NULL, all the scheme specific
**			parameters are set to NULL.
**
** ON EXIT:
**	returns		nothing.
*/
PRIVATE void HTAASetup_updateSpecifics (HTAASetup *	setup,
					HTAssocList **	specifics)
{
    int scheme;

    if (setup) {
	if (setup->scheme_specifics) {
	    for (scheme=0; scheme < HTAA_MAX_SCHEMES; scheme++) {
		if (setup->scheme_specifics[scheme])
		    HTAssocList_delete(setup->scheme_specifics[scheme]);
	    }
	    HT_FREE(setup->scheme_specifics);
	}
	setup->scheme_specifics = specifics;
    }
}




/*************************** HTAARealm **********************************/

/* PRIVATE 						HTAARealm_lookup()
**		LOOKUP HTAARealm STRUCTURE BY REALM NAME
** ON ENTRY:
**	realm_table	a list of realm objects.
**	realmname	is the name of realm to look for.
**
** ON EXIT:
**	returns		the realm.  NULL, if not found.
*/
PRIVATE HTAARealm *HTAARealm_lookup (HTList * realm_table,
				     CONST char * realmname)
{
    if (realm_table && realmname) {
	HTList *cur = realm_table;
	HTAARealm *realm;
	
	while (NULL != (realm = (HTAARealm*)HTList_nextObject(cur))) {
	    if (0==strcmp(realm->realmname, realmname))
		return realm;
	}
    }
    return NULL;	/* No table, NULL param, or not found */
}



/* PRIVATE						HTAARealm_new()
**		CREATE A NODE CONTAINING USERNAME AND
**		PASSWORD USED FOR THE GIVEN REALM.
**		IF REALM ALREADY EXISTS, CHANGE
**		USERNAME/PASSWORD.
** ON ENTRY:
**	realm_table	a list of realms to where to add
**			the new one, too.
**	realmname	is the name of the password domain.
**	username	and
**	password	are what you can expect them to be.
**
** ON EXIT:
**	returns		the created realm.
*/
PRIVATE HTAARealm *HTAARealm_new (HTList *	realm_table,
				  CONST char *	realmname,
				  CONST char *	username,
				  CONST char *	password)
{
    HTAARealm *realm;

    realm = HTAARealm_lookup(realm_table, realmname);

    if (!realm) {
	if ((realm = (HTAARealm *) HT_MALLOC(sizeof(HTAARealm))) == NULL)
	    HT_OUTOFMEM("HTAARealm_new");
	realm->realmname = NULL;
	realm->username = NULL;
	realm->password = NULL;
	StrAllocCopy(realm->realmname, realmname);
	if (realm_table) HTList_addObject(realm_table, (void*)realm);
    }
    if (username) StrAllocCopy(realm->username, username);
    if (password) StrAllocCopy(realm->password, password);

    return realm;
}




/***************** Basic and Pubkey Authentication ************************/

/* PRIVATE						compose_Basic_auth()
**
**		COMPOSE Basic SCHEME AUTHENTICATION STRING
**
** ON ENTRY:
**	req		request, where
**	req->scheme	== HTAA_BASIC
**	req->realm	contains username and password.
**
** ON EXIT:
**	returns		a newly composed authorization string,
**			NULL, if something fails.
** NOTE:
**	Like throughout the entire AA package, no string or structure
**	returned by AA package needs to (or should) be freed.
**
*/
PRIVATE char *compose_Basic_auth (HTRequest * req)
{
    static char *result = NULL;	/* Uuencoded presentation, the result */
    char *cleartext = NULL;	/* Cleartext presentation */
    int len;

    HT_FREE(result);		/* from previous call */

    if (!req || req->scheme != HTAA_BASIC || !req->setup ||
	!req->setup->server)
	return NULL;

    if (!req->realm) {
	char *realmname;

	if (!req->setup || !req->setup->scheme_specifics ||
	    !(realmname =
	      HTAssocList_lookup(req->setup->scheme_specifics[HTAA_BASIC],
				 "realm")))
	    return NULL;

	req->realm = HTAARealm_lookup(req->setup->server->realms, realmname);
	if (!req->realm) {
	    req->realm = HTAARealm_new(req->setup->server->realms,
				       realmname, NULL, NULL);
	    return NULL;
	}
    }

    len = strlen(req->realm->username ? req->realm->username : "") +
	  strlen(req->realm->password ? req->realm->password : "") + 3;

    if ((cleartext = (char *) HT_CALLOC(len, 1)) == NULL)
        HT_OUTOFMEM("compose_Basic_auth");

    if (req->realm->username) strcpy(cleartext, req->realm->username);
    else *cleartext = (char)0;

    strcat(cleartext, ":");

    if (req->realm->password) strcat(cleartext, req->realm->password);

    if ((result = (char *) HT_MALLOC(4 * ((len+2)/3) + 1)) == NULL)
        HT_OUTOFMEM("compose_Basic_auth");
    HTUU_encode((unsigned char *)cleartext, strlen(cleartext), result);
    HT_FREE(cleartext);

    return result;
}



/* BROWSER PRIVATE					HTAA_selectScheme()
**		SELECT THE AUTHENTICATION SCHEME TO USE
** ON ENTRY:
**	setup	is the server setup structure which can
**		be used to make the decision about the
**		used scheme.
**
**	When new authentication methods are added to library
**	this function makes the decision about which one to
**	use at a given time.  This can be done by inspecting
**	environment variables etc.
**
**	Currently only searches for the first valid scheme,
**	and if nothing found suggests Basic scheme;
**
** ON EXIT:
**	returns	the authentication scheme to use.
*/
PRIVATE HTAAScheme HTAA_selectScheme (HTAASetup * setup)
{
    HTAAScheme scheme;
    if (setup && setup->valid_schemes) {
	for (scheme = HTAA_BASIC; scheme < HTAA_MAX_SCHEMES; scheme++)
	    if (-1 < HTList_indexOf(setup->valid_schemes, (void *) scheme))
		return (HTAAScheme) scheme;
    }
    return HTAA_NONE;
}




/* BROWSER PUBLIC					HTAA_composeAuth()
**
**	COMPOSE Authorization: HEADER LINE CONTENTS
**	IF WE ALREADY KNOW THAT THE HOST REQUIRES AUTHENTICATION
**
** ON ENTRY:
**	req		request, which contains
**	req->setup	protection setup info on browser.
**	req->scheme	selected authentication scheme.
**	req->realm	for Basic scheme the username and password.
**
** ON EXIT:
**	returns	NO, if no authorization seems to be needed, and
**		req->authorization is NULL.
**		YES, if it has composed Authorization field,
**		in which case the result is in req->authorization,
**		e.g.
**
**		   "Basic AkRDIhEF8sdEgs72F73bfaS=="
*/
PUBLIC BOOL HTAA_composeAuth (HTRequest * req)
{
    char *auth_string = NULL;
    static char *docname;
    static char *hostname;
    int portnumber;
    char *colon;
    char *gate = NULL;	/* Obsolite? */
    char *arg = NULL;

    HT_FREE(hostname);	/* From previous call */
    HT_FREE(docname);	/*	- " -	      */

    if (!req  ||  !req->anchor)
	return NO;

    arg = HTAnchor_physical(req->anchor);
    docname = HTParse(arg, "", PARSE_PATH);
    hostname = HTParse((gate ? gate : arg), "", PARSE_HOST);
    if (hostname &&
	NULL != (colon = strchr(hostname, ':'))) {
	*(colon++) = '\0';	/* Chop off port number */
	portnumber = atoi(colon);
    }
    else portnumber = 80;
	
    if (PROT_TRACE)
	TTYPrint(TDEST, "Access Auth. composing authorization for %s:%d/%s\n",
		hostname, portnumber, docname);

#ifdef OLD_CODE
    if (current_portnumber != portnumber ||
	!current_hostname || !current_docname ||
	!hostname         || !docname         ||
	0 != strcmp(current_hostname, hostname) ||
	0 != strcmp(current_docname, docname)) {

	retry = NO;

	current_portnumber = portnumber;
	
	if (hostname) StrAllocCopy(current_hostname, hostname);
	else HT_FREE(current_hostname);

	if (docname) StrAllocCopy(current_docname, docname);
	else HT_FREE(current_docname);
    }
    else retry = YES;
#endif /*OLD_CODE*/

    if (!req->setup)
	req->setup = HTAASetup_lookup(hostname, portnumber, docname);
    if (!req->setup)
	return NO;

    if (req->scheme == HTAA_NONE || req->scheme == HTAA_UNKNOWN)
	req->scheme = HTAA_selectScheme(req->setup);

    switch (req->scheme) {
      case HTAA_BASIC:
	auth_string = compose_Basic_auth(req);
	break;
      case HTAA_PUBKEY:
      case HTAA_KERBEROS_V4:
	/* OTHER AUTHENTICATION ROUTINES ARE CALLED HERE */
      default:
	{
	    char msg[100];
	    sprintf(msg, "%s %s `%s'",
		    "This client doesn't know how to compose authentication",
		    "information for scheme", HTAAScheme_name(req->scheme));
	    HTRequest_addError(req, ERR_FATAL, NO, HTERR_NOT_IMPLEMENTED,
			       msg, 0, "HTLoadHTTP");
	    auth_string = NULL;
	}
    } /* switch scheme */

    req->setup->reprompt = NO;

    /* Added by marca. */
    if (!auth_string)
	return NO;
    
    HT_FREE(req->authorization); /* Free from previous call, Henrik 14/03-94 */
    if ((req->authorization = (char *) HT_MALLOC(sizeof(char) * (strlen(auth_string)+40))) == NULL)
        HT_OUTOFMEM("HTAA_composeAuth");

    strcpy(req->authorization, HTAAScheme_name(req->scheme));
    strcat(req->authorization, " ");
    strcat(req->authorization, auth_string);

    return YES;
}


/* BROWSER OVERLOADED					HTPasswordDialog()
**
**		PROMPT USERNAME AND PASSWORD, AND MAKE A
**		CALLBACK TO FUNCTION HTLoadHTTP().
**
**	This function must be redifined by GUI clients, which
**	call HTLoadHTTP(req) when user presses "Ok".
**
** ON ENTRY:
**	req		request.
**	req->dialog_msg	prompting message.
**	req->setup	information about protections of this request.
**	req->realm	structure describing one password realm.
**			This function should only be called when
**			server has replied with a 401 (Unauthorized)
**			status code, and req structure has been filled
**			up according to server reply, especially the
**			req->valid_shemes list must have been set up
**			according to WWW-Authenticate: headers.
** ON EXIT:
**
**	returns	YES or NO
**
*/
PUBLIC BOOL HTPasswordDialog (HTRequest * req)
{
    HTAlertCallback *cbf = HTAlert_find(HT_A_USER_PW);
    if (!req || !req->setup || !req->realm || !req->dialog_msg) {
	if (PROT_TRACE)
	    TTYPrint(TDEST, "Access...... called with an illegal parameter");
	return NO;
    }
    if (cbf) {
	HTAlertPar * reply = HTAlert_newReply();
	HT_FREE(req->realm->username);
	HT_FREE(req->realm->password);
	if (((*cbf)(req, HT_A_USER_PW, HT_MSG_NULL, NULL,
		    req->realm->realmname, reply))) {
	    req->realm->username = HTAlert_replyMessage(reply);
	    req->realm->password = HTAlert_replySecret(reply);
	}
	HTAlert_deleteReply(reply);
	/* Suggested by marca; thanks! */
	return req->realm->username ? YES : NO;
    }
    return NO;
}



/* BROWSER PUBLIC					HTAA_retryWithAuth()
**
**		RETRY THE SERVER WITH AUTHORIZATION (OR IF
**		ALREADY RETRIED, WITH A DIFFERENT USERNAME
**		AND/OR PASSWORD (IF MISSPELLED)) OR CANCEL
** ON ENTRY:
**	req		request.
**	req->valid_schemes
**			This function should only be called when
**			server has replied with a 401 (Unauthorized)
**			status code, and req structure has been filled
**			up according to server reply, especially the
**			req->valid_shemes list must have been set up
**			according to WWW-Authenticate: headers.
** ON EXIT:
**	On GUI clients pops up a username/password dialog box
**	with "Ok" and "Cancel".
**	"Ok" button press should do a callback
**
**		HTLoadHTTP(req);
**
**	This actually done by function HTPasswordDialog(),
**	which GUI clients redefine.
**
**	returns		YES, if dialog box was popped up.
**			NO, on failure.
*/
PUBLIC BOOL HTAA_retryWithAuth (HTRequest * req)
{
    int len;
    char *realmname;
    char *arg = NULL;

    if (!req || !req->anchor ||
	!req->valid_schemes || HTList_count(req->valid_schemes) == 0) {
	req->setup = NULL;
	return NO;
    }

    arg = HTAnchor_physical(req->anchor);

    if (req->setup && req->setup->server) {
	/* So we have already tried with authorization.	*/
	/* Either we don't have access or username or	*/
	/* password was misspelled.			*/
	    
	/* Update scheme-specific parameters	*/
	/* (in case they have expired by chance).	*/
	HTAASetup_updateSpecifics(req->setup, req->scheme_specifics);
	req->scheme = HTAA_selectScheme(req->setup);
	req->setup->reprompt = YES;
    }
    else { /* current_setup == NULL, i.e. we have a	 */
	   /* first connection to a protected server or  */
	   /* the server serves a wider set of documents */
	   /* than we expected so far.                   */

	static char *hostname;
	static char *docname;
	int portnumber;
	char *colon;
	HTAAServer *server;

	HT_FREE(hostname);	/* From previous call */
	HT_FREE(docname);	/*	- " -	      */

	docname = HTParse(arg, "", PARSE_PATH);
	hostname = HTParse(arg, "", PARSE_HOST);
	if (hostname &&
	    NULL != (colon = strchr(hostname, ':'))) {
	    *(colon++) = '\0';	/* Chop off port number */
	    portnumber = atoi(colon);
	}
	else portnumber = 80;
	
	if (PROT_TRACE)
	    TTYPrint(TDEST, "HTAA_retryWithAuth: first retry of %s:%d/%s\n",
		    hostname, portnumber, docname);

	if (!(server = HTAAServer_lookup(hostname, portnumber))) {
	    server = HTAAServer_new(hostname, portnumber);
	}
#if 0
	else 
	    HTAlert(req, "Access without authorization denied -- retrying");
	}
#endif

	if (!req->prot_template)
	    req->prot_template = HTAA_makeProtectionTemplate(docname);
	req->setup = HTAASetup_new(server, 
				   req->prot_template,
				   req->valid_schemes,
				   req->scheme_specifics);
	req->setup->reprompt = NO;
	req->scheme = HTAA_selectScheme(req->setup);

    } /* else current_setup == NULL */

    realmname = HTAssocList_lookup(req->setup->scheme_specifics[req->scheme],
				   "realm");
    if (!realmname)
	return NO;

    req->realm = HTAARealm_lookup(req->setup->server->realms, realmname);
    if (!req->realm)
	req->realm = HTAARealm_new(req->setup->server->realms,
				   realmname, NULL, NULL);

    len = strlen(realmname) + 100;
    if (req->setup->server->hostname)
	len += strlen(req->setup->server->hostname);

    HT_FREE(req->dialog_msg);	 /* Free from previous call, Henrik 14/03-94 */
    if ((req->dialog_msg = (char *) HT_MALLOC(len)) == NULL)
        HT_OUTOFMEM("HTAA_retryWithAuth");
    if (!req->realm->username)
	sprintf(req->dialog_msg, "\n%s %s at %s",
		"Document is protected. Enter username for",
		req->realm->realmname,
		req->setup->server->hostname
		? req->setup->server->hostname : "??");
    else sprintf(req->dialog_msg, "\n%s %s at %s",
		 "Authorization failed. Enter username for",
		 req->realm->realmname,
		 req->setup->server->hostname
		 ? req->setup->server->hostname : "??");
    return (HTPasswordDialog(req));
}

/*
**	Setup HTTP access authentication
*/
PUBLIC BOOL HTAA_authentication (HTRequest * request)
{
    HTAAScheme scheme;
    HTList *valid_schemes = HTList_new();
    HTAssocList **scheme_specifics = NULL;
    char *tmplate = NULL;

    if (request->WWWAAScheme) {
	if ((scheme = HTAAScheme_enum(request->WWWAAScheme)) != HTAA_UNKNOWN) {
	    HTList_addObject(valid_schemes, (void *) scheme);
	    if (!scheme_specifics) {
		int i;
		scheme_specifics = (HTAssocList**) HT_MALLOC(HTAA_MAX_SCHEMES * sizeof(HTAssocList*));
		if (!scheme_specifics)
		    outofmem(__FILE__, "HTTPAuthentication");
		for (i=0; i < HTAA_MAX_SCHEMES; i++)
		    scheme_specifics[i] = NULL;
	    }
	    scheme_specifics[scheme] = HTAA_parseArgList(request->WWWAARealm);
	} else if (PROT_TRACE) {
	    HTRequest_addError(request, ERR_INFO, NO, HTERR_UNKNOWN_AA,
		       (void *) request->WWWAAScheme, 0, "HTTPAuthentication");
	    return NO;
	}
    }
    if (request->WWWprotection) {
	if (PROT_TRACE)
	    TTYPrint(TDEST, "Protection template set to `%s'\n",
		    request->WWWprotection);
	StrAllocCopy(tmplate, request->WWWprotection);
    }
    request->valid_schemes = valid_schemes;
    request->scheme_specifics = scheme_specifics;
    request->prot_template = tmplate;
    return YES;
}
