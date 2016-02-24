/*

  
  					W3C Sample Code Library libwww Request Access Methods


!
  Request Access Methods
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module keeps a list of valid methods to be performed on a
request object, for example GET, HEAD,
PUT, POST, DELETE, etc.&nbsp;You can assign which method
to be performed on the request object directly or
you can go through the Access module for higher
level functions.

This module is implemented by HTMethod.c, and it
is a part of the  W3C Sample Code
Library.
*/

#ifndef HTMETHOD_H
#define HTMETHOD_H

#ifdef __cplusplus
extern "C" { 
#endif 

/*

NOTE: The anchor list of allowed methods is a bitflag, not at list!
*/

typedef enum {
    METHOD_INVALID	= 0x0,
    METHOD_GET		= 0x1,
    METHOD_HEAD		= 0x2,    
    METHOD_POST		= 0x4,    
    METHOD_PUT		= 0x8,
    METHOD_PATCH	= 0x10,
    METHOD_DELETE	= 0x20,
    METHOD_TRACE	= 0x40,
    METHOD_OPTIONS	= 0x80,
    METHOD_LINK		= 0x100,

#ifdef HT_DAV
    METHOD_LOCK         = 0x400,              /* WebDAV Methods */
    METHOD_UNLOCK       = 0x800,

    METHOD_PROPFIND     = 0x1000,
    METHOD_PROPPATCH    = 0x2000,
    METHOD_MKCOL        = 0x4000,
    METHOD_COPY         = 0x8000,
    METHOD_MOVE         = 0x10000,
#endif

#ifdef HT_EXT
    METHOD_EXT_0        = 0x20000,            /* Extension methods */
    METHOD_EXT_1        = 0x40000,    
    METHOD_EXT_2        = 0x80000,
    METHOD_EXT_3        = 0x100000,
    METHOD_EXT_4        = 0x200000,
    METHOD_EXT_5        = 0x400000,
    METHOD_EXT_6        = 0x800000,
#endif

    METHOD_UNLINK	= 0x200
} HTMethod;

/*
(
  Get Method Enumeration
)

Gives the enumeration value of the method as a function of the (char *) name.
*/

extern HTMethod HTMethod_enum (const char * name);

/*
(
  Get Method String
)

The reverse of HTMethod_enum()
*/

extern const char * HTMethod_name (HTMethod method);

/*
(
  Is Method "Safe"?
)

If a method is safe, it doesn't produce any side effects on the server
*/

#define HTMethod_isSafe(me)	((me) & (METHOD_GET|METHOD_HEAD))

/*
(
  Does the Method Replace or Add to Metainformation
)

Most methods override the current set of metainformation stored in an
anchor. However, a few methods actually only
add to the anchor metainformation. We have a small macro to make the distinction.
*/

#define HTMethod_addMeta(me)  ((me) & (METHOD_TRACE | METHOD_OPTIONS))

/*
(
  Does a Method include Entity?
)

Does a method include an entity to be sent from the client to the server?

If not using WebDAV functions, neither extension methods, the
HTMethod_hasEntity is not changed, because a macro is much more performant
than a function. The function is interesting only when using WebDAV (many
methods) or extension methods (in this case, a dynamic structure is
needed).
*/

extern BOOL HTMethod_hasEntity(HTMethod me);

/*
(
Extension Methods
)


These methods have been introduced in Libwww for extension purposes.
Through these methods, application may register new methods, even libwww
unknown methods, and use them. It's recomended for applications that need
only a few new methods, that aren't yet in the libwww. The application should
register the desired methos, an before finish, it must unregister these
methods.
*/

extern BOOL HTMethod_setExtensionMethod (HTMethod method, const char * name, BOOL hasEntity);
extern BOOL HTMethod_deleteExtensionMethod (HTMethod method);

/*

*/

#ifdef __cplusplus
}
#endif

#endif /* HTMETHOD_H */

/*

  

  @(#) $Id$

*/
