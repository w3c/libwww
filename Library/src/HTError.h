/*

  
  					W3C Sample Code Library libwww Error Class


!
  The Error Class
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The Error class provides an easy API for registering errors ocurring while
libwww serves a request. All errors are registered in an "error stack"&nbsp;in
the Request object which allows for nested errors.
The Error class is both natural language independent and application independent
in that it uses enumerations in order to refer to specific errors. It is
for the application to provide an error presentation module which interprets
the errors associated with a request. An eample of such an implementation
can be found in the HTDialog module.

This module is implemented by HTError.c, and it is
a part of the  W3C Sample Code
Library.
*/

#ifndef HTERROR_H
#define HTERROR_H

#ifdef __cplusplus
extern "C" { 
#endif 

typedef struct _HTError HTError;

typedef enum _HTSeverity {
    ERR_UNKNOWN		  = 0x0,
    ERR_FATAL		  = 0x1,
    ERR_NON_FATAL	  = 0x2,
    ERR_WARN	 	  = 0x4,
    ERR_INFO	 	  = 0x8
} HTSeverity;

#include "HTReq.h"

/*
.
  The Error Message Object
.

An error consists of a messsage code, a short, natural language specific
message, and a URI which can point to more information. This module also
provides a default set of error objects with English text.
Errors are registered together with a severity and you can also set the mode
for what class of error severities should be shown and which
should be ignored.
*/

typedef struct _HTErrorMessage {
    int  	code;           	/* Error number */
    char *	msg;          		/* Short explanation */
    char *	url;			/* Explaning URL */
} HTErrorMessage;

/*
(
  Error Message Index
)

Note: All non-HTTP error codes have (index number >
HTERR_HTTP_CODES), and they will not be shown in the error-message
generated.

Error codes are registered in an array where the following enumerations serve
as an index. They must not be replaced! See the
HTDialog module for default initialization
of these messages.
*/

typedef enum _HTErrorElement {
	HTERR_CONTINUE = 0,					/* 100 */
	HTERR_SWITCHING,					/* 101 */
	HTERR_OK,						/* 200 */
	HTERR_CREATED,						/* 201 */
	HTERR_ACCEPTED,						/* 202 */
	HTERR_NON_AUTHORITATIVE,				/* 203 */
	HTERR_NO_CONTENT,					/* 204 */
	HTERR_RESET,						/* 205 */
	HTERR_PARTIAL,						/* 206 */
	HTERR_PARTIAL_OK,					/* 207 */
	HTERR_MULTIPLE,						/* 300 */
	HTERR_MOVED,						/* 301 */
	HTERR_FOUND,						/* 302 */
	HTERR_METHOD,						/* 303 */
	HTERR_NOT_MODIFIED,					/* 304 */
	HTERR_USE_PROXY,					/* 305 */
	HTERR_PROXY_REDIRECT,					/* 306 */
	HTERR_TEMP_REDIRECT,					/* 307 */
	HTERR_BAD_REQUEST,					/* 400 */
	HTERR_UNAUTHORIZED,					/* 401 */
	HTERR_PAYMENT_REQUIRED,					/* 402 */
	HTERR_FORBIDDEN,					/* 403 */
	HTERR_NOT_FOUND,					/* 404 */
	HTERR_NOT_ALLOWED,					/* 405 */
	HTERR_NONE_ACCEPTABLE,					/* 406 */
	HTERR_PROXY_UNAUTHORIZED,				/* 407 */
	HTERR_TIMEOUT,						/* 408 */
	HTERR_CONFLICT,						/* 409 */
	HTERR_GONE,						/* 410 */
	HTERR_LENGTH_REQUIRED,					/* 411 */
	HTERR_PRECON_FAILED,					/* 412 */
	HTERR_TOO_BIG,						/* 413 */
	HTERR_URI_TOO_BIG,					/* 414 */
	HTERR_UNSUPPORTED,					/* 415 */
	HTERR_BAD_RANGE,					/* 416 */
	HTERR_EXPECTATION_FAILED,				/* 417 */
	HTERR_REAUTH,				                /* 418 */
	HTERR_PROXY_REAUTH,				        /* 419 */
	HTERR_INTERNAL,						/* 500 */
	HTERR_NOT_IMPLEMENTED,					/* 501 */
	HTERR_BAD_GATE,						/* 502 */
	HTERR_DOWN,						/* 503 */
	HTERR_GATE_TIMEOUT,					/* 504 */
	HTERR_BAD_VERSION,					/* 505 */
	HTERR_NO_PARTIAL_UPDATE,				/* 506 */

#ifdef HT_DAV 
	/* WebDAV error codes */
	HTERR_UNPROCESSABLE,					/* 422 */
	HTERR_LOCKED,           				/* 423 */
	HTERR_FAILED_DEPENDENCY,				/* 424 */
	HTERR_INSUFFICIENT_STORAGE,				/* 507 */
#endif

	/* Cache warnings */
	HTERR_STALE,						/* 10 */
	HTERR_REVALIDATION_FAILED,				/* 11 */
	HTERR_DISCONNECTED_CACHE,				/* 12 */
	HTERR_HEURISTIC_EXPIRATION,				/* 13 */
	HTERR_TRANSFORMED,					/* 14 */
	HTERR_CACHE,						/* 99 */

	/* Put all non-HTTP status codes after this */
	HTERR_NO_REMOTE_HOST,
	HTERR_NO_HOST,
	HTERR_NO_FILE,
	HTERR_FTP_SERVER,
	HTERR_FTP_NO_RESPONSE,
        HTERR_FTP_LOGIN_FAILURE,
	HTERR_TIME_OUT,
	HTERR_GOPHER_SERVER,
	HTERR_INTERRUPTED,
	HTERR_CON_INTR,
	HTERR_CSO_SERVER,
	HTERR_HTTP09,
	HTERR_BAD_REPLY,
	HTERR_UNKNOWN_AA,
	HTERR_NEWS_SERVER,
	HTERR_FILE_TO_FTP,
	HTERR_AUTO_REDIRECT,
	HTERR_MAX_REDIRECT,
	HTERR_EOF,
	HTERR_WAIS_OVERFLOW,
	HTERR_WAIS_MODULE,
	HTERR_WAIS_NO_CONNECT,
	HTERR_SYSTEM,
	HTERR_CLASS,
	HTERR_ACCESS,
	HTERR_LOGIN,
        HTERR_CACHE_EXPIRED,
        HTERR_NO_AUTO_RULES,
        HTERR_NO_AUTO_PROXY,
	HTERR_ELEMENTS		            /* This MUST be the last element */
} HTErrorElement;

/*
(
  Default English Language Messages
)

Default set of error messages arranged in an array into which the
index codes serve as index. See the
HTDialog module for default initialization
of these strings.
.
  What Errors should be Ignored or Passed Through?
.

This variable dictates which errors should be put out when generating the
message to the user. The first four enumerations make it possible to see
`everything as bad or worse than' this level, e.g. HT_ERR_SHOW_NON_FATAL
shows messages of type HT_ERR_SHOW_NON_FATAL and
HT_ERR_SHOW_FATAL.

Note: The default value is made so that it only puts a message to
stderr if a `real' error has occurred. If a separate widget is available
for information and error messages then probably
HT_ERR_SHOW_DETAILED would be more appropriate.
*/

typedef enum _HTErrorShow {
    HT_ERR_SHOW_FATAL     = 0x1,
    HT_ERR_SHOW_NON_FATAL = 0x3,
    HT_ERR_SHOW_WARNING   = 0x7,
    HT_ERR_SHOW_INFO 	  = 0xF,
    HT_ERR_SHOW_PARS	  = 0x10,
    HT_ERR_SHOW_LOCATION  = 0x20,
    HT_ERR_SHOW_IGNORE    = 0x40,
    HT_ERR_SHOW_FIRST     = 0x80,
    HT_ERR_SHOW_LINKS     = 0x100,
    HT_ERR_SHOW_DEFAULT	  = 0x13,
    HT_ERR_SHOW_DETAILED  = 0x1F,
    HT_ERR_SHOW_DEBUG	  = 0x7F
} HTErrorShow;

extern HTErrorShow HTError_show (void);
extern BOOL HTError_setShow (HTErrorShow mask);

/*
.
  Creation and Deletion Methods
.
(
  Add an Error
)

Add an error message to the error list. `par' and `where' might be set to
NULL. If par is a string, it is sufficient to let length be unspecified,
i.e., 0. If only a part of the string is wanted then specify a length inferior
to strlen((char *) par). The string is '\0' terminated automaticly. See also
HTError_addSystem for system errors. Returns YES if OK, else NO.
*/

extern BOOL HTError_add (HTList * 	list,
			 HTSeverity	severity,
			 BOOL		ignore,
			 int		element,
			 void *		par,
			 unsigned int	length,
			 char *		where);


/*
(
  Add a System Error
)

Add a system error message to the error list. syscall is the name of the
system call, e.g. "close". The message put to the list is that corresponds
to the error number passed. See also HTError_add. Returns YES if OK, else
NO.
*/

extern BOOL HTError_addSystem (HTList *		list,
			       HTSeverity 	severity,
			       int		errornumber,
			       BOOL		ignore,
			       char *		syscall);

/*
(
  Delete an Entire Error Stack
)

Deletes all errors in a list.
*/

extern BOOL HTError_deleteAll (HTList * list);

/*
(
  Deletes the Last Edded Entry
)

Deletes the last error entry added to the list. Return YES if OK, else NO
*/

extern BOOL HTError_deleteLast (HTList * list);

/*
.
  Object Methods
.
(
  Show the Error Entry?
)

Should we show this entry in the list or just continue to the next one?
*/

extern BOOL HTError_doShow (HTError * info);

/*
(
  Ignore last Added Error
)

Turns on the `ignore' flag for the most recent error entered the error list.
Returns YES if OK else NO
*/

extern BOOL HTError_ignoreLast	(HTList * list);
extern BOOL HTError_setIgnore	(HTError * info);

/*
(
  Error Index Number
)

Each error object is assigned an index number as defined by the
HTErrorElement above. The mapping from this index to an error
code and a message must be done by the application. The Library provides
a default implementation in the  HTDialog module,
but that can of course be changed depending on the application.
*/

extern int HTError_index		(HTError * info);

/*
(
  Error Severity
)

This function returns the severity of the error object passed by the caller
*/
extern HTSeverity HTError_severity	(HTError * info);

/*

You can ask whether a list of errors contains any error object with a severity
level which is higher than the one specifed.
*/
extern BOOL HTError_hasSeverity 	(HTList * list, HTSeverity severity);

/*
(
  Parameters Asscotiated with Error
)
*/

extern void * HTError_parameter		(HTError * info, int * length);

/*
(
  Where did it happen?
)
*/

extern const char * HTError_location	(HTError * info);

/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTERROR_H */

/*

  

  @(#) $Id$

*/
