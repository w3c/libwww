/*

  
  					W3C Sample Code Library libwww ANSI C File Streams


!
  Basic ANSI C File Writer Stream
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module contains some local file writer streams based on the basic file
writer stream defined in the HTFWrite module.
They can be used to do save-as, save-and-callback etc.

This module is implemented by HTFSave.c, and it
is a part of the W3C Sample Code
Library.
*/

#ifndef HTFSAVE_H
#define HTFSAVE_H

#include "HTFWrite.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  Various Converters using the File Writer Stream
.

This is a set of functions that can be registered as converters. They all
use the basic ANSI C file writer stream for writing out to the local file
system.
*/

extern HTConverter HTSaveAndExecute, HTSaveLocally, HTSaveAndCallback;

/*


  
    HTSaveLocally
  
    Saves a file to local disk. This can for example be used to dump date objects
    of unknown media types to local disk. The stream prompts for a file name
    for the temporary file.
  
    HTSaveAndExecute
  
    Creates temporary file, writes to it and then executes system command (maybe
    an external viewer) when EOF has been reached. The stream finds
    a suitable name of the temporary file which preserves the suffix. This way,
    the system command can find out the file type from the name of the temporary
    file name.
  
    HTSaveAndCallback
  
    This stream works exactly like the HTSaveAndExecute stream but
    in addition when EOF has been reached, it checks whether a callback
    function has been associated with the request object in which case, this
    callback is being called. This can be use by the application to do some
    processing after the system command has terminated. The callback
    function is called with the file name of the temporary file as parameter.

*/

#ifdef __cplusplus
}
#endif

#endif  /* HTFSAVE_H */

/*

  

  @(#) $Id$

*/
