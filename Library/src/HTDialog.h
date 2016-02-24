/*

  					W3C Sample Code Library libwww Messages and Dialogs


!
  Application side Error Messages And the like
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module provides some "make life easier" functions in order to get the
application going. The functionality of this module was originally in
HTAccess, but now It is part of the
application interface which the application
may use it if desired.

This module is implemented by HTHome.c, and it is
a part of the  W3C Sample Code
Library.
*/

#ifndef HTDIALOG_H
#define HTDIALOG_H
#include "WWWLib.h"

#ifdef __cplusplus
extern "C" { 
#endif 

/*
.
  Default English Error Messages and Progress Notifications
.

The following functions provide a default set of error messages and prompts
in plain English. You can of course change this as you like.
(
  Default English User Prompts and Questions
)

This list corresponds to the enumeration list defined in the
HTAlert module
*/

#define HT_MSG_ENGLISH_INITIALIZER \
    "Please enter username:", \
    "Please enter username for proxy authentication:", \
    "Please enter username for this FTP server:", \
    "Password:", \
    "Please give name of file to save in:", \
    "Plase enter account:", \
    "You might not be allowed to use this method here, continue?", \
    "Location has moved, continue?", \
    "A new set of rules is requested to be added to your setup - continue?", \
    "This file already exists - replace existing file?", \
    "Authentication failed - retry?", \
    "Proxy authentication failed - retry?", \
    "This method has already been performed - repeat operation?", \
    "This document is very big - continue operation?", \
    "The source document for this operation has moved - continue operation \
with new location?", \
    "The destination document for this operation has moved - continue \
operation with new location?", \
    "A redirection may change the behavior of this method - proceed anyway?", \
    "An automatic request for changing proxy has been encountered - continue?", \
    "The persistent cache is already in use by another user. If this is not \
the case then you can manually delete this lock and restart.", \
    "The server has sent you a cookie - accept?"

/*
(
  Default English Messages and Progress Notifications
)

This list corresponds to the enumeration list defined in the
HTError module
*/

/*    CODE  ERROR MESSAGE				ERROR URL */
#define HTERR_ENGLISH_INITIALIZER \
    { 100, "Continue", 					"information" }, \
    { 101, "Switching Protocols",			"information" }, \
    { 200, "OK", 					"success" }, \
    { 201, "Created", 					"success" }, \
    { 202, "Accepted", 					"success" }, \
    { 203, "Non-authoritative Information",		"success" }, \
    { 204, "Document Updated",				"success" }, \
    { 205, "Reset Content",				"success" }, \
    { 206, "Partial Content",				"success" }, \
    { 207, "Partial Update OK",				"success" }, \
    { 300, "Multiple Choices",				"redirection" }, \
    { 301, "Moved Permanently",				"redirection" }, \
    { 302, "Found",  			                "redirection" }, \
    { 303, "See Other",					"redirection" }, \
    { 304, "Not Modified",       			"redirection" }, \
    { 305, "Use Proxy",					"redirection" }, \
    { 306, "Proxy Redirect",				"redirection" }, \
    { 307, "Temporary Redirect",			"redirection" }, \
    { 400, "Bad Request", 				"client_error" }, \
    { 401, "Unauthorized",				"client_error" }, \
    { 402, "Payment Required", 				"client_error" }, \
    { 403, "Forbidden", 				"client_error" }, \
    { 404, "Not Found",		       			"client_error" }, \
    { 405, "Method Not Allowed",	 		"client_error" }, \
    { 406, "Not Acceptable",		 		"client_error" }, \
    { 407, "Proxy Authentication Required", 		"client_error" }, \
    { 408, "Request Timeout",		 		"client_error" }, \
    { 409, "Conflict",			 		"client_error" }, \
    { 410, "Gone",			 		"client_error" }, \
    { 411, "Length Required",		 		"client_error" }, \
    { 412, "Precondition Failed",	 		"client_error" }, \
    { 413, "Request Entity Too Large",	 		"client_error" }, \
    { 414, "Request-URI Too Large",	 		"client_error" }, \
    { 415, "Unsupported Media Type",	 		"client_error" }, \
    { 416, "Range Not Satisfiable",	 		"client_error" }, \
    { 417, "Expectation Failed",	 		"client_error" }, \
    { 418, "Reauthentication Required",	 		"client_error" }, \
    { 419, "Proxy Reauthentication Reuired", 		"client_error" }, \
    { 500, "Internal Server Error",			"server_error" }, \
    { 501, "Not Implemented", 				"server_error" }, \
    { 502, "Bad Gateway", 				"server_error" }, \
    { 503, "Service Unavailable",			"server_error" }, \
    { 504, "Gateway Timeout", 				"server_error" }, \
    { 505, "HTTP Version not supported",		"server_error" }, \
    { 506, "Partial update Not Implemented",		"server_error" }, \
 \
    /* Cache Warnings */ \
    { 10,  "Response is Stale",				"cache" }, \
    { 11,  "Revalidation Failed",			"cache" }, \
    { 12,  "Disconnected Opeartion",			"cache" }, \
    { 13,  "Heuristic Expiration",			"cache" }, \
    { 14,  "Transformation Applied",			"cache" }, \
    { 99,  "Cache warning", 				"cache" }, \
 \
    /* Non-HTTP Error codes and warnings */ \
    { 0,   "Can't locate remote host", 			"internal" }, \
    { 0,   "No host name found", 			"internal" }, \
    { 0,   "No file name found or file not accessible", "internal" }, \
    { 0,   "FTP server replies", 			"internal" }, \
    { 0,   "FTP server doesn't reply", 			"internal" }, \
    { 0,   "FTP login failure", 			"internal" }, \
    { 0,   "Server timed out", 				"internal" }, \
    { 0,   "Gopher-server replies", 			"internal" }, \
    { 0,   "Data transfer interrupted", 		"internal" }, \
    { 0,   "Connection establishment interrupted", 	"internal" }, \
    { 0,   "CSO-server replies", 			"internal" }, \
    { 0,   "This is probably a HTTP server 0.9 or less","internal" }, \
    { 0,   "Bad, Incomplete, or Unknown Response",	"internal" }, \
    { 0,   "Unknown access authentication scheme",	"internal" }, \
    { 0,   "News-server replies",			"internal" }, \
    { 0,   "Trying `ftp://' instead of `file://'",	"internal" }, \
    { 0,   "Too many redirections",			"internal" }, \
    { 0,   "Method not suited for automatic redirection","internal" }, \
    { 0,   "Premature End Of File",			"internal" }, \
    { 0,   "Response from WAIS Server too Large - Extra lines \
ignored","internal"}, \
    { 0,   "WAIS-server doesn't return any data", 	"internal" }, \
    { 0,   "Can't connect to WAIS-server",		"internal" }, \
    { 0,   "operation failed",				"internal" }, \
    { 0,   "Wrong or unknown access scheme",		"internal" }, \
    { 0,   "Access scheme not allowed in this context",	"internal" }, \
    { 0,   "When you are connected, you can log in",	"internal" }, \
    { 0,   "This version has expired and will be automatically reloaded", "internal" }, \
    { 0,   "Loading new rules must be explicitly acknowledged", "internal" }, \
    { 0,   "Automatic proxy redirection must be explicitly acknowledged", "internal" }

/*
(
  Generate a Default Error Message
)

This function provides an unformatted English string containing the possibly
nested status message that explains the result of a request. This is essentially
"flattening out" the information provided in the request
error strack. The string must be freed by the caller.
*/

extern char * HTDialog_errorMessage (HTRequest * request, HTAlertOpcode op,
			             int msgnum, const char * dfault,
				     void * input);

/*
(
  Generate a Default Progress Notification Message
)

This function provides a string containin an English progress message that
the application can present to the user if desired. The string must be freed
by the caller.
*/

extern char * HTDialog_progressMessage (HTRequest * request, HTAlertOpcode op,
			                int msgnum, const char * dfault,
				        void * input);

/*
.
  Default User Interaction Handlers
.

You can register a set of callback functions to handle user prompting, error
messages, confimations etc. Here we give a set of functions that can be used
on almost anu thinkable platform. If you want to provide your own platform
dependent implementation then fine :-)
(
  Display a message, then wait for 'YES' or 'NO'
)

This function prompts the user for a confirmation on the message passed as
a parameter. If the user reacts in the affirmative, returns TRUE,
returns FALSE otherwise.
*/

extern HTAlertCallback HTConfirm;

/*
(
  Prompt the User a Question
)

Prompt for answer and get text back. Reply text is either NULL on error or
a dynamic string which the caller must free.
*/
		
extern HTAlertCallback HTPrompt;

/*
(
  Prompt for a Password
)

Prompt for password without echoing the reply. Reply text is weither NULL
on error or a dynamic string which the caller must free.

NOTE: The current version uses getpass which on many systems
returns a string of 8 or 16 bytes.
*/

extern HTAlertCallback HTPromptPassword;

/*
(
  Prompt for a UserID and a Password
)

This is just a composite function using HTPrompt and
HTPromptPassword. The strings returned must be freed by caller.
*/

extern HTAlertCallback HTPromptUsernameAndPassword;

/*
(
  Display a Message
)

This function simply puts out the message passed.
*/

extern HTAlertCallback HTAlert;

/*
(
  Progress Notification
)

This function can be used to indicate the current status of a certain action.
In order to avoid having strings directly in the core parts of the Library,
this function is passed a "state" argument from which the message can be
generated in this module. The "param" argument is for additional information
to be passed.
*/

extern HTAlertCallback HTProgress;

/*
(
  Generating a User Error Message of a request
)

This function outputs the content of the error list to standard output (used
in Line Mode Browser), but smart clients and servers might overwrite this
function so that the error messages can be handled to the user in a nice(r)
way. That is the reason for putting the actual implementation in
HTAlert.c.

*/

extern HTAlertCallback HTError_print;

/*
(
  Generating a Server Error Message of a request
)

Default function that creates an error message using HTAlert() to put out
the contents of the error_stack messages. Furthermore, the error_info structure
contains a name of a help file that might be put up as a link. This file
can then be multi-linguistic.
*/

extern BOOL HTError_response (HTRequest * request, HTAlertOpcode op,
			      int msgnum, const char * dfault, void * input,
			      HTAlertPar * reply);

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTDIALOG_H */

/*

  

  @(#) $Id$

*/
