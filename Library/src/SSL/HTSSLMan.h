
/*
  
    					The HTSSL Class Definition
  
  
    !The HTSSL Class Definition!

    Defines, manages and frees single SSL connection.


*/

#ifndef HTSSLMAN_H
#define HTSSLMAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "HTSSL.h"    

struct _HTSSL {
    SSL * ssl;
    int   sd;        /* socket descriptor */
    BOOL  connected;
    int   ref_count;
};

extern HTSSL * HTSSL_new(int sd);

extern void HTSSL_free(HTSSL *);

extern BOOL HTSSL_connected(HTSSL * ssl);

extern void HTSSL_set_disconnected(HTSSL * htssl);

extern BOOL HTSSL_connect(HTSSL * ssl, int sd);

extern int HTSSL_read(HTSSL * htssl, int sd, char * buff, int len);

extern int HTSSL_write(HTSSL * htssl, int sd, char * buff, int len);

#ifdef __cplusplus
}
#endif

#endif /*HTSSLMAN_H*/

/*


  

  @(#) $Id$

*/
