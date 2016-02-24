/*

  
  					W3C Sample Code Library libwww DNS Class


!
  Domain Name Service Class
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The DNS Class defines generic access to &nbsp;the DNS system. It maintains
a cache of all visited hosts so that subsequent connects to the same host
doesn't imply a new request to the DNS every time. Multihomed hosts are treated
specially in that the time spend on every connect is measured and kept in
the cache. On the next request to the same host, the IP-address with the
lowest average connect time is chosen. If one IP-address fails completely,
e.g. connection refused then it disabled and HTDoConnect tries one
of the other IP-addresses to the same host.

Every entry in the cache has its own time to live (TTL) and hence the cache
manages its own automatic garbage collection. Currently the TTL is not
bound to the DNS records which should be changed. You can set the DNS object
TTL

This module is implemented by HTDNS.c, and it is a
part of the  W3C Sample Code Library.
*/

#ifndef HTDNS_H
#define HTDNS_H

#ifdef __cplusplus
extern "C" { 
#endif 

/*
*/
typedef struct _HTdns HTdns;

/*
.
  DNS Cache Expiration Time
.

When to remove an entry in the DNS cache. We maintain our own DNS cache as
we keep track of the connect time, pick the fastet host on multi-homed hosts
etc. However we DO NOT HONOR DNS TTL Records which is the
reason for why the expiration must be faily short (the default value is 30
mins), so that it doesn't collide with the DNS mechanism for timing out DNS
records befoew swapping IP addresses around.
*/

extern void HTDNS_setTimeout (time_t timeout);
extern time_t HTDNS_timeout  (time_t timeout);

/*
.
  Creation and Deletion Methods
.
(
  Add a DNS Object
)

Add an element to the cache of visited hosts. The homes variable
indicates the number of IP addresses found when looking up the name. A host
name must NOT contain a port number.
*/

extern HTdns * HTDNS_add (HTList * list, struct hostent * element,
			  char * host, int * homes);

/*
(
  Delete a DNS object
)

This function flushes the DNS object from the cache and frees up memory
*/

extern BOOL HTDNS_delete (const char * host);

/*
(
  Delete ALL DNS objects
)

This function is called from  HTLibTerminate. It
can be called at any point in time if the DNS cache is going to be flushed.
*/

extern BOOL HTDNS_deleteAll (void);

/*
.
  DNS Class Methods
.
(
  Recalculating the Time Weights on Multihomed Host
)

On every connect to a multihomed host, the average connect time is updated
exponentially for all the entries.
*/

extern BOOL HTDNS_updateWeigths (HTdns *dns, int cur, ms_t deltatime);

/*
.
  IDN (Internationalized Domain Names) Functions
.
(
  Calculate ACE (punycode) from UTF-8 encoded IDN
)

Converts to ACE (Ascii-compatible encoding, punycode) from an
Internationalized Domain name. %hh-escaping in domain name is
accepted for backwards compatibility. Returns 0 if successful.

*/

extern int HTACEfromUTF8 (char *hostUTF8, char *punycode, size_t punyLength);

/*
.
  Resolver Functions
.

These are the functions that resolve a host name
(
  Get Host By Socket
)

This function should have been called HTGetHostByAddr but for historical
reasons this is not the case.
*/

extern char * HTGetHostBySock (int soc);

/*
(
  Get Host By Name
)

This function gets the address of the host and puts it in to the socket
structure. It maintains its own cache of connections so that the communication
to the Domain Name Server is minimized. Returns the number of homes or -1
if error.
*/

extern int HTGetHostByName (HTHost * host, char *hostname, HTRequest * request);

/*
*/

#ifdef __cplusplus
}
#endif

#endif  /* HTDNS_H */

/*

  

  @(#) $Id$

*/
