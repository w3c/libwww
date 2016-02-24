/*

  
  					W3C Sample Code Library libwww Default Initialization


!
  Default Initialization Modules and Profiles
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

As mentioned in the Library Architecture,
libwww consists of a small core and a large set of hooks for adding
functionality. By itself, the core it not capable of performing any Web related
tasks like accessing a HTTP server or parsing a HTML document. All this
functionality must be registered by the application. This way, the core of
libwww is kept application independent and can be used as the basic building
block for any kind of Web application. The Library comes with a large set
of default functions, for example for accessing HTTP and FTP servers, parsing
MIME headers etc. This module helps the application programmer setting up
all this functionality, but it is important to note that none of it is
required in order to use libwww.
*/

#ifndef WWWINIT_H
#define WWWINIT_H

/*
*/

#ifdef __cplusplus
extern "C" { 
#endif

/*
.
  Default Protocols, Transports, MIME Headers, Dialogs etc.
.

This module contains a large set of default initialization functions for
protocol modules, stream converters, event managers etc.
*/

#include "HTInit.h"

/*
.
  Application Profiles
.

Application profiles are initialization functions that are "preset" to initialize
the Library with the set of features often used in clients,
servers, robots, and proxy servers. They are basically
collections of the more detailed initialization functions that you can find
later in this interface description. In many cases you do not need to use
anything else than the profiles, but if you have more specific requirements
then you can always fall back on using the initialization functions below
or even werite your own extensions.
*/

#include "HTProfil.h"

/*
*/

#ifdef __cplusplus
} /* end extern C definitions */
#endif

#endif

/*

  

  @(#) $Id$

*/
