
/* MODULE							HTAuth.c
**			USER AUTHENTICATION
**
** AUTHORS:
**	AL	Ari Luotonen	luotonen@dxcern.cern.ch
**
** HISTORY:
**	AL 14.10.93 Fixed the colon-not-allowed-in-password-bug.
**
** BUGS:
**
**
*/

#include <string.h>

#include "HTUtils.h"
#include "HTPasswd.h"	/* Password file routines	*/
#include "HTAssoc.h"
#include "HTAuth.h"	/* Implemented here		*/
#include "HTUU.h"	/* Uuencoding and uudecoding	*/


/* PRIVATE					    decompose_auth_string()
**		DECOMPOSE AUTHENTICATION STRING
**		FOR BASIC OR PUBKEY SCHEME
** ON ENTRY:
**	authstring	is the authorization string received
**			from browser.
**
** ON EXIT:
**	returns		a node representing the user information
**			(as always, this is automatically freed
**			by AA package).
*/
PRIVATE HTAAUser *decompose_auth_string ARGS2(char *,		authstring,
					      HTAAScheme,	scheme)
{
    static HTAAUser *user = NULL;
    static char *cleartext = NULL;
    char *username = NULL;
    char *password = NULL;
    char *inet_addr = NULL;
    char *timestamp = NULL;
    char *browsers_key = NULL;

    if (!user && !(user = (HTAAUser*)malloc(sizeof(HTAAUser))))	/* Allocated */
	outofmem(__FILE__, "decompose_auth_string");		/* only once */

    user->scheme = scheme;
    user->username = NULL;	/* Not freed, because freeing */
    user->password = NULL;	/* cleartext also frees these */
    user->inet_addr = NULL;	/* See below: ||              */
    user->timestamp = NULL;	/*            ||              */
    user->secret_key = NULL;	/*            ||              */
                                /*            \/              */
    FREE(cleartext);	/* From previous call.				*/
                        /* NOTE: parts of this memory are pointed to by	*/
                        /* pointers in HTAAUser structure. Therefore,	*/
                        /* this also frees all the strings pointed to	*/
			/* by the static 'user'.			*/

    if (!authstring || !*authstring || 
	scheme != HTAA_BASIC || scheme == HTAA_PUBKEY)
	return NULL;

    if (scheme == HTAA_PUBKEY) {    /* Decrypt authentication string */
	int bytes_decoded;
	char *ciphertext;
	int len = strlen(authstring) + 1;

	if (!(ciphertext = (char*)malloc(len)) ||
	    !(cleartext  = (char*)malloc(len)))
	    outofmem(__FILE__, "decompose_auth_string");

	bytes_decoded = HTUU_decode(authstring, ciphertext, len);
	ciphertext[bytes_decoded] = (char)0;
#ifdef PUBKEY
	HTPK_decrypt(ciphertext, cleartext, private_key);
#endif
	free(ciphertext);
    }
    else {   /* Just uudecode */
	int bytes_decoded;
	int len = strlen(authstring) + 1;
	
	if (!(cleartext = (char*)malloc(len)))
	    outofmem(__FILE__, "decompose_auth_string");
	bytes_decoded = HTUU_decode(authstring, cleartext, len);
	cleartext[bytes_decoded] = (char)0;
    }


/*
** Extract username and password (for both schemes)
*/
    username = cleartext;
    if (!(password = strchr(cleartext, ':'))) {
	if (TRACE)
	    fprintf(stderr, "%s %s\n",
		    "decompose_auth_string: password field",
		    "missing in authentication string.\n");
	return NULL;
    }
    *(password++) = '\0';

/*
** Extract rest of the fields
*/
    if (scheme == HTAA_PUBKEY) {
	if (                          !(inet_addr   =strchr(password, ':')) || 
	    (*(inet_addr++)   ='\0'), !(timestamp   =strchr(inet_addr,':')) ||
	    (*(timestamp++)   ='\0'), !(browsers_key=strchr(timestamp,':')) ||
	    (*(browsers_key++)='\0')) {

	    if (TRACE) fprintf(stderr, "%s %s\n",
			       "decompose_auth_string: Pubkey scheme",
			       "fields missing in authentication string");
	    return NULL;
	}
    }

/*
** Set the fields into the result
*/
    user->username   = username;
    user->password   = password;
    user->inet_addr  = inet_addr;
    user->timestamp  = timestamp;
    user->secret_key = browsers_key;

    if (TRACE) {
	if (scheme==HTAA_BASIC)
	    fprintf(stderr, "decompose_auth_string: %s '%s'\n",
		    "Got Basic scheme authentication string for user",
		    username);
	else
	    fprintf(stderr, "decompose_auth_string: %s (%s,PW,%s,%s,%s)\n",
		    "Pubkey scheme authentication string:",
		    username, inet_addr, timestamp, browsers_key);
    }
    
    return user;
}



PRIVATE BOOL HTAA_checkTimeStamp ARGS1(CONST char *, timestamp)
{
    return NO;		/* This is just a stub */
}


PRIVATE BOOL HTAA_checkInetAddress ARGS1(CONST char *, inet_addr)
{
    return NO;		/* This is just a stub */
}


/* SERVER PUBLIC					HTAA_authenticate()
**			AUTHENTICATE USER
** ON ENTRY:
**	req		request.
**	req->scheme	used authentication scheme.
**	req->scheme_specifics
**			the scheme specific parameters
**			(authentication string for Basic and
**			Pubkey schemes).
**	req->prot	is the protection information structure
**			for the file.
**
** ON EXIT:
**	returns		YES, if authentication succeeds and
**			req->user is set to point to the authenticated
**			user.  NO, if authentication fails.
** BUGS:
**	req->user must NOT be freed.
*/
PUBLIC BOOL HTAA_authenticate ARGS1(HTRequest *, req)
{
    if (HTAA_UNKNOWN == req->scheme || !req->prot ||
	-1 == HTList_indexOf(req->prot->valid_schemes, (void*)req->scheme))
	return NO;

    switch (req->scheme) {
      case HTAA_BASIC:
      case HTAA_PUBKEY:
	{
	    HTAAUser *user =
		decompose_auth_string(req->auth_string, req->scheme);
	                                   /* Remember, user is auto-freed */
	    if (user &&
		HTAA_checkPassword(user->username,
				   user->password,
				   HTAssocList_lookup(req->prot->values,
						      "passw")) &&
		(HTAA_BASIC == req->scheme ||
		 (HTAA_checkTimeStamp(user->timestamp) &&
		  HTAA_checkInetAddress(user->inet_addr)))) {
		req->user = user;
		return YES;
	    }
	    else return NO;
	}
	break;
      default:
	/* Other authentication routines go here */
	return NO;
    }
}

