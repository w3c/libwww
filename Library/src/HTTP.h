/*  */

/*      HyperText Tranfer Protocol                                      HTTP.h
**      ==========================
*/

#ifndef HTTP_H
#define HTTP_H

#include "HTAccess.h"


#ifdef OLD_CODE
/*      Load Document using HTTP
**      ------------------------
*/
extern int HTLoadHTTP PARAMS((
        CONST char *            arg,
/*      CONST char *            gateway, */
        HTParentAnchor *        anAnchor,
        int diag));
#endif

extern HTProtocol HTTP;

#endif /* HTTP_H */
/*

    */
