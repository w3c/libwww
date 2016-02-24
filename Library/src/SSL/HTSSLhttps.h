/*

  					W3C Sample Code Library libwww Initializationg of SSL Transport As
  https Access Scheme


!
  Initializationg of SSL Transport As https Access Scheme
!

This module sets up the SSL transport for handling the https URI
access scheme. In fact the SSL module itself is completely independent and
could as well be negotiated instead of getting a new URI scheme.

The module is contributed by Olga Antropova
*/

#ifndef _HTSSLHTTPS_H
#define _HTSSLHTTPS_H

#ifdef __cplusplus
extern "C" { 
#endif 


/*

The initialization function initialies the SSL transports by doing the following:
	 
	   o 
	     Setting up a SSL application context
	   o 
	     Registering the transport in the libwww HTTransport
    Manager
  o 
	     Registering the HTTP protocol module as a protocol
    handler in the libwww HTProtocol Manager for
    the https: access scheme
  o 
	     Registering a set of filters for handling HTTP caching, redirection, and
	     authentication
	 
	 
The termination function removes the application context, unregisteres the
SSL transport in the libwww HTTransport Manager,
and unregisteres the other things set up when initializing the SSL transport
*/

#ifndef SSL_PORT
#define SSL_PORT        443
#endif

extern BOOL HTSSLhttps_init (BOOL preemptive);
extern BOOL HTSSLhttps_isInitialized (void);
extern BOOL HTSSLhttps_terminate (void);

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* _HTSSLHTTPS_H */

/*

  

  @(#) $Id$

*/
