/*
**	SSL TRANSPORT WRAPPER
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
**	   Olga Antropova <olga@eai.com>
**
**      HISTORY:
**         June 15 2000 Jose Kahan (kahan@w3.org)
**              Extended the API to be able to change the protocol method
**         August 3 2000 Jose Kahan (kahan@w3.org)
**              Extended the API to be able to change the verify depth.
*/

/* System files */
#include "wwwsys.h"

/* SSL includes */
#include <openssl/ssl.h>
#include <openssl/rand.h>

/* Library include files */
#include "WWWUtil.h"
#include "WWWCore.h"
#include "WWWHTTP.h"

#include "HTSSLReader.h"
#include "HTSSLWriter.h"
#include "HTSSL.h"
#include "HTSSLMan.h"

/* Our global SSL context */
PRIVATE SSL_CTX * app_ctx = NULL;

/* List of all HTSSL structures currently used by readers and writers */
PRIVATE HTList * ssl_list = NULL;

/* For certificate verification */
PRIVATE int verify_depth = 0;
PRIVATE int verify_error = X509_V_OK;

/* For choosing an SSL protocol method */
PRIVATE HTSSL_PROTOCOL ssl_prot_method = HTTLS_V1;

/* Client certificate/key files */
PRIVATE char *cert_file = NULL;
PRIVATE char *key_file = NULL;

/* ----------------------------------------------------------------- */

#ifdef HTDEBUG
/*
** Roughly copied from the openssl sample app in the openssl package
*/
PRIVATE void apps_ssl_info_callback (SSL * s, int where, int ret)
{
    char *str;
    int w = where & ~SSL_ST_MASK;
    if (w & SSL_ST_CONNECT)
	str="SSL_connect";
    else if (w & SSL_ST_ACCEPT)
	str="SSL_accept";
    else
	str="undefined";
    if (where & SSL_CB_LOOP) {
	HTTRACE(PROT_TRACE, "%s: %s\n" _ str _ SSL_state_string_long(s));
    } else if (where & SSL_CB_ALERT) {
	str = (where & SSL_CB_READ) ? "read" : "write";
	HTTRACE(PROT_TRACE, "SSL3 alert %s:%s:%s\n" _ str _
		SSL_alert_type_string_long(ret) _
		SSL_alert_desc_string_long(ret));
    } else if (where & SSL_CB_EXIT) {
	if (ret == 0) {
	    HTTRACE(PROT_TRACE, "%s: failed in %s\n" _ str _ SSL_state_string_long(s));
	} else if (ret < 0) {
	    HTTRACE(PROT_TRACE, "%s: error in %s\n" _ str _ SSL_state_string_long(s));
	}
    }
}
#endif

/*
** Roughly copied from the openssl sample app in the openssl package
*/
PRIVATE int verify_callback (int ok, X509_STORE_CTX * ctx)
{
    char buf[256];
    X509 * err_cert = X509_STORE_CTX_get_current_cert(ctx);
    int err = X509_STORE_CTX_get_error(ctx);
    int depth = X509_STORE_CTX_get_error_depth(ctx);
    X509_NAME_oneline(X509_get_subject_name(err_cert), buf, 256);

    HTTRACE(PROT_TRACE, "depth = %d %s\n" _ depth _ buf);
    if (!ok) {
	HTTRACE(PROT_TRACE, "verify error: num=%d:%s\n" _ err _
		X509_verify_cert_error_string(err));
	if (verify_depth >= depth) {
	    ok=1;
	    verify_error=X509_V_OK;
	} else {
	    ok=0;
	    verify_error=X509_V_ERR_CERT_CHAIN_TOO_LONG;
	}
    }
    switch (ctx->error) {

    case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT:
	X509_NAME_oneline(X509_get_issuer_name(ctx->current_cert), buf, 256);
	HTTRACE(PROT_TRACE, "issuer= %s\n" _ buf);
	break;

    case X509_V_ERR_CERT_NOT_YET_VALID:
    case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD:
	HTTRACE(PROT_TRACE, "notBefore=");
//	ASN1_TIME_print(bio_err,X509_get_notBefore(ctx->current_cert));
	HTTRACE(PROT_TRACE, "\n");
	break;

    case X509_V_ERR_CERT_HAS_EXPIRED:
    case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD:
	HTTRACE(PROT_TRACE, "notAfter=");
//	ASN1_TIME_print(bio_err,X509_get_notAfter(ctx->current_cert));
	HTTRACE(PROT_TRACE, "\n");
	break;
    }
    HTTRACE(PROT_TRACE, "verify return: %d\n" _ ok);
    return ok;
}

/* 
** Set or retrieve the SSL protocol we want to use
*/
PUBLIC void HTSSL_protMethod_set (HTSSL_PROTOCOL prot_method)
{
  ssl_prot_method = prot_method;
}

PUBLIC HTSSL_PROTOCOL HTSSL_protMethod (void)
{
  return ssl_prot_method;
}

/* 
** Set or retrieve the certificate verify depth
*/
PUBLIC void HTSSL_verifyDepth_set (int depth)
{
  verify_depth = depth;
}

PUBLIC int HTSSL_verifyDepth (void)
{
  return verify_depth;
}

/*
**  Manipulate client-side cert/key
*/
PUBLIC void HTSSL_certFile_set (const char *cfile)
{
  StrAllocCopy(cert_file, cfile);
}

PUBLIC const char* HTSSL_certFile (void)
{
  return cert_file;
}

PUBLIC void HTSSL_keyFile_set (const char *kfile)
{
  StrAllocCopy(key_file, kfile);
}

PUBLIC const char* HTSSL_keyFile (void)
{
  return key_file;
}

/*
**  Create an SSL application context if not already done
*/
PUBLIC BOOL HTSSL_init (void)
{
    char rnd_filename[HT_MAX_PATH];

    /*
    ** Initialise OpenSSL 0.9.5 random number generator.
    ** The random generator of OpenSSL had to be initialised on platforms
    ** that do not support /dev/random, like Compaq True64 Unix.
    ** This is done in the default way, and means that the user of the
    ** libwww-ssl library needs to have a .rnd file in his/her home-directory.
    */
    RAND_file_name(rnd_filename, sizeof(rnd_filename));
    RAND_load_file(rnd_filename, -1);
    
    if (!app_ctx) {
	SSL_METHOD * meth = NULL;
        SSLeay_add_ssl_algorithms();
	/* Seems to provide English error messages */
        SSL_load_error_strings();

	/* select the protocol method */
	switch (ssl_prot_method) {
	case HTSSL_V2:
	  meth = SSLv2_client_method();
	  break;
	case HTSSL_V3:
	  meth = SSLv3_client_method();
	  break;
	case HTSSL_V23:
	  meth = SSLv23_client_method();
	  break;
	default:
	case HTTLS_V1:
	  meth = TLSv1_client_method();
	  break;
	}

        /* set up the application context */
	if ((app_ctx = SSL_CTX_new(meth)) == NULL) {
            HTTRACE(PROT_TRACE, "HTSSLContext Could not create context\n");
	    return NO;
	}
	HTTRACE(PROT_TRACE, "HTSSLContext Created context %p" _ app_ctx);

	/* See the SSL states in our own callback */
#ifdef HTDEBUG
	SSL_CTX_set_info_callback(app_ctx, apps_ssl_info_callback);
#endif
	
	/* Set the certificate verification callback */
	SSL_CTX_set_verify(app_ctx, SSL_VERIFY_PEER, verify_callback);

        /* Set client cert stuff if it exists */
        if (cert_file != NULL) {
            if (SSL_CTX_use_certificate_file(app_ctx,cert_file,
                SSL_FILETYPE_PEM) <= 0) {
                HTTRACE(PROT_TRACE,"HTSSL... unable to get certificate from '%s'\n" _ cert_file);
                return NO;
            }
            if (key_file == NULL) key_file=cert_file;
            if (SSL_CTX_use_PrivateKey_file(app_ctx,key_file,
                SSL_FILETYPE_PEM) <= 0) {
                HTTRACE(PROT_TRACE,"HTSSL... unable to get private key from '%s'\n" _ key_file);
                return NO;
            }
            if (!SSL_CTX_check_private_key(app_ctx)) {
                HTTRACE(PROT_TRACE,"HTSSL... Private key does not match the certificate public key\n");
                return NO;
            }
        }

	/* Not sure what this does */
        SSL_CTX_set_session_cache_mode(app_ctx, SSL_SESS_CACHE_CLIENT);
     }
    return YES;
}

/*
**  Delete the application context if already initialized
*/
PUBLIC BOOL HTSSL_terminate (void)
{
    if (app_ctx) {
	SSL_CTX_free(app_ctx);
	app_ctx = NULL;
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTSSL_isInitialized (void)
{
    return app_ctx ? YES : NO;
}

/*
** Initialization of HTSSL structure. Try to do SSL_connect. If fails
** still OK - will connect later.
*/
PRIVATE BOOL HTSSL_connectSetup (HTSSL * htssl, int sd)
{
    HTTRACE(PROT_TRACE, "HTSSL....... Setting up %p on socket %d\n" _ htssl _ sd);
    htssl->sd = sd;
    htssl->connected = NO;
    htssl->ref_count = 0;
    htssl->ssl = SSL_new(app_ctx);
    if (!htssl->ssl) return NO;

    /* Tell that we are in connect mode */
    SSL_set_connect_state(htssl->ssl);
    
    /* Set socket descriptor with the socket we already have open */
    SSL_set_fd(htssl->ssl, sd);
    
    return YES;
}

/*
** This function should be called whenever HTSSL_new creates a new structure
** or returns a pointer to an existing structure to the caller.
** This function is not PUBLIC because I want to centralize reference counting
** in one module in order to minimize possibility of  errors. 
*/
PRIVATE void HTSSL_addRef(HTSSL * htssl)
{
    if (htssl) {
	(htssl->ref_count)++;
	HTTRACE (PROT_TRACE, "HTSSL....... New reference count = %d\n" _ htssl->ref_count);
    }
}

/*
** Using a socket descriptor as a key try to find the HTSSL object in ssl_table.
** If not there then create a new object and add it to ssl_table.
*/ 
PUBLIC HTSSL * HTSSL_new (int sd) 
{
    HTSSL * htssl = NULL;
    HTList * ssls = NULL;

    /* If application context doesn't exist */
    if (!app_ctx) return NULL;
    
    /* Check ssl_table */
    if (!ssl_list) ssl_list = HTList_new();

    ssls = ssl_list;
    
    while ((htssl = HTList_nextObject(ssls))) {
	if (htssl->sd == sd) {
            HTTRACE(PROT_TRACE, "HTSSL New... Found SSL %p with sd =  %d\n" _ (void *) htssl _ sd);
            /* add reference count before giving pointer to caller */
            HTSSL_addRef(htssl);
            return htssl;
	}
    }
        
    /* ssl not found : create new */
    if ((htssl = (HTSSL *) HT_CALLOC(1, sizeof (HTSSL))) == NULL)
	HT_OUTOFMEM("HTSSL_new");
    HTTRACE(PROT_TRACE, "HTSSL New... Created new SSL Object %p\n" _ htssl);
    
    /* If initialization fails... */
    if (HTSSL_connectSetup(htssl, sd) != YES) {
        HTSSL_free(htssl);
        return NULL;
    }

    HTSSL_addRef(htssl);
    
    HTList_addObject (ssl_list, htssl);
    
    return htssl;    
}

/*
** Destroy HTSSL object if no references to it remain in application
*/
PUBLIC void HTSSL_free (HTSSL * htssl)
{
    (htssl->ref_count)--;
    HTTRACE(PROT_TRACE, "HTSSL Free.. ref_count = %d\n" _ htssl->ref_count);

    if (htssl->ref_count == 0) {
        HTTRACE(PROT_TRACE, "HTSSL.Free.. FINAL RELEASE\n");

        if (htssl->ssl) {
            SSL_free(htssl->ssl);
            htssl->ssl = NULL;
        }
       
        HTList_removeObject(ssl_list, htssl);          

        /* releases itself */
        HT_FREE(htssl);
    }
}

PUBLIC BOOL HTSSL_open (HTSSL * htssl, int sd)
{
    int err = 0;
    if (!htssl) return NO;
    if (htssl->connected) {
	HTTRACE(PROT_TRACE, "HTSSL Open.. SSL Object %p already connected\n" _ htssl);
	return YES;
    }

    if (!htssl->ssl) {
	HTTRACE(PROT_TRACE, "HTSSL....... SSL object %p has non-NULL ssl\n" _ htssl);
	return NO;
    }

    HTTRACE(PROT_TRACE, "HTSSL Open.. Connecting %p\n" _ htssl);
 
    if ((htssl->ssl = SSL_new(app_ctx)) == NULL) {
	HTTRACE(PROT_TRACE, "HTSSL Open.. SSL_new failed\n");
        return NO;
    }

    /* Set socket descriptor with our socket that we already have */
    SSL_set_fd (htssl->ssl, sd);
    htssl->sd = sd;
    
    /* Do SSL using certificate and key exchange */
    if ((err = SSL_connect (htssl->ssl)) != -1) {
	htssl->connected = YES;
	return YES;
    } else {
	HTTRACE(PROT_TRACE, "HTSSL Open.. SSL_connect failed with code %d" _ err);
	return NO;
    }        
}

PUBLIC BOOL HTSSL_close (HTSSL * htssl)
{
    if (htssl) {
	HTTRACE(PROT_TRACE, "HTSSL....... Closing SSL Object %p\n" _ htssl);
	SSL_free(htssl->ssl);
	htssl->connected = NO;
	htssl->ssl = NULL;
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTSSL_isOpen (HTSSL * htssl)
{
    return htssl ? htssl->connected : NO;
}

PUBLIC int HTSSL_read (HTSSL * htssl, int sd, char * buff, int len)
{
    return htssl && htssl->ssl ? SSL_read(htssl->ssl, buff, len) : -1;
}

PUBLIC int HTSSL_write (HTSSL * htssl, int sd, char * buff, int len)
{
    return htssl && htssl->ssl ? SSL_write(htssl->ssl, buff, len) : -1;
}

PUBLIC int HTSSL_getError (HTSSL * htssl, int status)
{
    return htssl && htssl->ssl ? SSL_get_error(htssl->ssl, status) : -1;
}
