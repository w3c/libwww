/*

  
  					W3C Sample Code Library libwww Home Page Management


!
  Home Page Management
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module provides some "make life easier" functions in order to
get the application going. The functionality of this module was originally
in HTAccess, but now it has been moved here as
a part of the application interface where the
application may use it if desired.

This module is implemented by HTHome.c, and it is
a part of the  W3C Sample Code
Library.
*/

#ifndef HTHOME_H
#define HTHOME_H
#include "WWWLib.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*

The home page is special in that this is is the first page to visit
when &nbsp;a client application starts up. Note that a home page is a generic
URL and hence can be any resouce - not only resources on the local file system.
.
  Personal Home Page
.
*/
#define LOGICAL_DEFAULT 	"WWW_HOME"    /* Defined to be the home page */

#ifndef PERSONAL_DEFAULT
#define PERSONAL_DEFAULT 	"WWW/default.html"	/* in home directory */
#endif

/* If the home page isn't found, use this file: */
#ifndef LAST_RESORT
#define LAST_RESORT		"http://www.w3.org/"
#endif

/*
.
  Home Page for Remote Access
.

Some Web applications can also be run remotely - for example as a telnet
login shell. The Line Mode Browser is an example of such an application.&nbsp;In
that case, the home page is often more generic than a personal home page.
*/

/* If one telnets to an access point it will look in this file for home page */
#ifndef REMOTE_POINTER
#define REMOTE_POINTER 		 "/etc/www-remote.url"	    /* can't be file */
#endif

/* and if that fails it will use this. */
#ifndef REMOTE_ADDRESS
#define REMOTE_ADDRESS  	"http://www.w3.org/"	    /* can't be file */
#endif

#ifndef LOCAL_DEFAULT_FILE
#define LOCAL_DEFAULT_FILE 	"/usr/local/lib/WWW/default.html"
#endif

/*
.
  Get an Anchor for the Home Page
.

Getting an anchor for the home page involves looking for the (environment)
variables described in the section above. As this is something that almost
all client applications must do then we provide some simple methods that
do the work for you.
*/

extern HTParentAnchor * HTHomeAnchor (void);

/*
.
  Create a New Anchor for Temporary Local Files
.

When the user starts writing a new document, the client application should
create a new anchor which can contain the document while it is created. This
can also be the location for backups and for security "auto-save" functionality.
This functions creates a new anchor with a URL pointing to the temporary
location defined by this user profile and returns that anchor.
Andy Levine: I additionally found that calling HTTmpAnchor repeatedly without
freeing the newly allocated anchor will cause the anchor hash table to
continue to grow.
*/

extern HTParentAnchor * HTTmpAnchor (HTUserProfile * up);

/*
.
  Get The Current Directory in URL form
.

Creates a local file URL that can be used as a relative name when calling
expanding other URLs relative to the current location in the local file system
tree where the application is running. The code for this routine originates
from the Line Mode Browser and was moved here by howcome@w3.org
in order for all clients to take advantage.
*/

#define HTFindRelatedName	HTGetCurrentDirectoryURL
extern char *  HTGetCurrentDirectoryURL (void);

/*
.
  Handle HTML Form Input fields
.

Takes a string of the form "a=b" containing HTML form data,
escapes it accordingly and puts it into the association list so that it readily
can be passed to any of the HTAccess function that handles HTML form data.
The string should not be encoded in any way - this function encodes it according
to the HTML form encoding rules.

Examples are "foo=bar", "baz=foo and bar", "a= b ", " a = b ", "toto=", "four
= two + two", "six three + three", and "a=b=c"
*/

extern BOOL HTParseFormInput (HTAssocList * list, const char * str);

/*
.
  Handle Library Trace Messages
.

Standard interface to libwww TRACE messages.
Pass this function a string of characters.  It will set up the appropriate
TRACE flags. The following characters are used as follows:

  
    f
  
    Show BASIC UTILITIES Trace Messages
  
    l
  
    Show APPLICATION LEVEL Trace Messages
  
    c
  
    Show CACHE Trace Messages
  
    g
  
    Show SGML Trace Messages
  
    b
  
    Show BIND Trace Messages
  
    t
  
    Show THREAD Trace Messages
  
    s
  
    Show STREAM Trace Messages
  
    p
  
    Show PROTOCOL Trace Messages
  
    m
  
    Show MEMORY Trace Messages
  
    q
  
    Show SQL Trace Messages
  
    u
  
    Show URI Trace Messages
  
    h
  
    Show AUTH Trace Messages
  
    a
  
    Show ANCHOR Trace Messages
  
    i
  
    Show PICS Trace Messages
  
    o
  
    Show CORE Trace Messages
  
    x
  
    Show MUX Trace Messages
  
    *
  
    Show ALL Trace Messages


The string must be null terminated, an example is "sop".
*/

extern int HTSetTraceMessageMask (const char * shortnames);

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTHOME_H */

/*

  

  @(#) $Id$

*/
