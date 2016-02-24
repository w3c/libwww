/*

  					W3C Sample Code Library libwww SSL Transport Wrapper


!
  SSL Transport Wrapper
!

This module declares a SSL (using the OpenSSL
library) transport wrapper so that libwww can use SSL as a
transport the same way it can use TCP and
local host as transports. This module keeps a context of the application
(SSL_CTX), Cipher negotiation, certificate directory path setting
could be added here.

The module is contributed by Olga Antropova
*/

#ifndef _HTSSL_H
#define _HTSSL_H

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  The HTSSL Class
.

The HTSSL Class is the libwww interface to the SSL layer.
*/

typedef struct _HTSSL HTSSL;

extern BOOL HTSSL_init (void);
extern BOOL HTSSL_isInitialized (void);
extern BOOL HTSSL_terminate (void);

/*

The following functions allow to set (or retrieve) the TSL/SSL protocol method
we want to use when talking with the server. By default, we use the
highest available protocol (TLSv1 in this version).
*/
 
typedef enum _HTSSL_PROTOCOL {
	      HTSSL_V2 = 0, 
	      HTSSL_V3,
	      HTSSL_V23, /* Brian Hawley: the SSLv23 method tries 
		            SSLv3/TLSv1 but can fall back to SSLV2 */
	      HTTLS_V1
} HTSSL_PROTOCOL;

extern void HTSSL_protMethod_set (HTSSL_PROTOCOL prot_method);
extern HTSSL_PROTOCOL HTSSL_protMethod (void);

/*

The following functions allow to set (or retrieve) the certificate
verification depth. By default, the verification depth has a value of 0.
*/
 
extern void HTSSL_verifyDepth_set (int depth);
extern int HTSSL_verifyDepth (void);

/*


Manipulate client-side cert/key
*/

extern void HTSSL_certFile_set (const char *cfile);
extern const char* HTSSL_certFile (void);
extern void HTSSL_keyFile_set (const char *kfile);
extern const char* HTSSL_keyFile (void);

/*


*/

extern HTSSL * HTSSL_new (int sd);
extern void HTSSL_free (HTSSL * htssl);

extern BOOL HTSSL_open (HTSSL * htssl, int sd);
extern BOOL HTSSL_close (HTSSL * htssl);
extern BOOL HTSSL_isOpen (HTSSL * htssl);

extern int HTSSL_read (HTSSL * htssl, int sd, char * buff, int len);
extern int HTSSL_write (HTSSL * htssl, int sd, char * buff, int len);
extern int HTSSL_getError (HTSSL * htssl, int status);

/*

*/


#ifdef __cplusplus
} /* end extern C definitions */
#endif

#endif /* _HTSSL_H */

/*

  

  @(#) $Id$

*/
