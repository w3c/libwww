/*
**	INITIALIZATIONG OF SSL TRANSPORT AS HTTPS ACCESS SCHEME
**
**	@(#) $Id$
**	
**	Copyright © 1995-1998 World Wide Web Consortium, (Massachusetts
**	Institute of Technology, Institut National de Recherche en
**	Informatique et en Automatique, Keio University). All Rights
**	Reserved. This program is distributed under the W3C's Software
**	Intellectual Property License. This program is distributed in the hope
**	that it will be useful, but WITHOUT ANY WARRANTY; without even the
**	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
**	PURPOSE. See W3C License http://www.w3.org/Consortium/Legal/ for more
**	details.
**
**	This module requires a SSL library in order to compile/link
**
**	AUTHOR:
**		Olga Antropova <olga@eai.com>
*/

/* System files */
#include "wwwsys.h"

/* Library include files */
#include "WWWUtil.h"
#include "WWWCore.h"
#include "WWWHTTP.h"
#include "WWWApp.h"
#include "WWWSSL.h"

#include "HTSSLhttps.h"

PRIVATE BOOL https_init = NO;

PUBLIC BOOL HTSSLhttps_init (BOOL preemptive)
{
    if (!https_init) {

	/* Set up the SSL context */
	HTSSL_init();

	/* Register as libwww transport */
	HTTransport_add("secure_tcp", HT_TP_SINGLE, HTSSLReader_new, HTSSLWriter_new);

	/* Register the HTTP protocol module for the "https" scheme */
	HTProtocol_add("https", "secure_tcp", SSL_PORT, preemptive, HTLoadHTTP, NULL);  

	/* Register libwww BEFORE filters that we want to use with SSL */
	HTNet_addBefore(HTCredentialsFilter,	"https://*", NULL, HT_FILTER_LATE);
	HTNet_addBefore(HTPEP_beforeFilter,	"https://*", NULL, HT_FILTER_LATE);

	/* Register libwww AFTER filters that we want to use with SSL */
	HTNet_addAfter(HTAuthFilter,		"https://*", NULL, HT_NO_ACCESS, HT_FILTER_MIDDLE);
	HTNet_addAfter(HTAuthFilter,		"https://*", NULL, HT_REAUTH, HT_FILTER_MIDDLE);
	HTNet_addAfter(HTPEP_afterFilter,	"https://*", NULL, HT_ALL, HT_FILTER_MIDDLE);
	HTNet_addAfter(HTRedirectFilter,	"https://*", NULL, HT_PERM_REDIRECT, HT_FILTER_MIDDLE);
	HTNet_addAfter(HTRedirectFilter,	"https://*", NULL, HT_FOUND, HT_FILTER_MIDDLE);
	HTNet_addAfter(HTRedirectFilter,	"https://*", NULL, HT_SEE_OTHER, HT_FILTER_MIDDLE);
	HTNet_addAfter(HTRedirectFilter,	"https://*", NULL, HT_TEMP_REDIRECT, HT_FILTER_MIDDLE);
	HTNet_addAfter(HTUseProxyFilter,	"https://*", NULL, HT_USE_PROXY, HT_FILTER_MIDDLE);

	https_init = YES;
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTSSLhttps_terminate (void)
{
    if (https_init) {

	/* Delete the application context */
	HTSSL_terminate();

	/* Unregister as libwww transport */
	HTTransport_delete("secure_tcp");

	/* Unregister the HTTP protocol module for the "https" scheme */
	HTProtocol_delete ("https");

	/* Unegister before and after filters based on template */

	/* @@@ */

	https_init = NO;
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTSSLhttps_isInitialized (void)
{
    return https_init;
}








