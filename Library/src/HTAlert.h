/*

  
  					W3C Sample Code Library libwww Library Alert Class


!
  The Alert Class
!
*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

The Alert class defines a set of methods to be used by libwww for passing
prompts and message to the application. In order to maintain libwww application
independent and natural language independent, it does not know how to communicate
with a user. Note here that a user is a somewhat abstract notion
for &nbsp;something that can receive a message or prompt from libwww. A
user can for example be a person, but is may also be handled
automatically by a robot or a client receiving a response from a HTTP server.

Libwww has a set of opcodes that classifies the nature of the message,
for example that it is a question that must be confirmed in order to continue
a request or simply a progress notification. The application can register
a callback for any number of the defined opcodes - in case libwww has a message
for an opcode that does not have a method associated, the message is ignored.
You can also globally disable any message send from libwww.

Note: The library core does not define any message or dialog
methods - they are all considered part of the application. However, it comes
with a default set of methods which can be initiated
using the function HTAlertInit() in HTInit
module

This module is implemented by HTAlert.c, and it is
a part of the  W3C Sample Code
Library.
*/

#ifndef HTALERT_H
#define HTALERT_H

#include "HTReq.h"

#ifdef __cplusplus
extern "C" { 
#endif 


/*
.
  Message Opcodes and Messages
.

The callback functions are defined as a generic callback where the caller
can pass a set of input parameters and the callee can return a set of outptu
parameters. Also note that all the *_PROG_* opcodes are a subset
of HT_A_PROGRESS. This means that you easily can register a
callback for all progress reports. 

The callback handler for progress notifications SHOULD NOT be used
to interrupt the ongoing message as it is not guaranteed to be in a state
to do so. Instead you should use the event handlers
or the timers for this.
*/

typedef enum _HTAlertOpcode {
    HT_PROG_DNS		= 0x1,		/* Doing DNS resolution */
    HT_PROG_CONNECT	= 0x2,		/* Connecting Active */
    HT_PROG_ACCEPT	= 0x4,		/* Connecting Passive */
    HT_PROG_READ	= 0x8,		/* Read data */
    HT_PROG_WRITE	= 0x10,		/* Write data */
    HT_PROG_DONE	= 0x20,		/* Request finished */
    HT_PROG_INTERRUPT   = 0x40,         /* Request interrupted */
    HT_PROG_OTHER       = 0x80,         /* Other progress notes */
    HT_PROG_TIMEOUT     = 0x100,        /* Request timed out */
    HT_PROG_LOGIN	= 0x200,	/* Automatic login notifications */
    HT_A_PROGRESS	= 0xFFFF,	/* Get all progress reports - no reply */

    /* First word are reserved for progresss notifications */

    HT_A_MESSAGE	= 0x1<<16, /* Send a message - no reply */
    HT_A_CONFIRM	= 0x2<<16, /* Want YES or NO back */
    HT_A_PROMPT		= 0x4<<16, /* Want full dialog */
    HT_A_SECRET		= 0x8<<16, /* Secret dialog (e.g. password) */
    HT_A_USER_PW	= 0x10<<16 /* Atomic userid and password */
} HTAlertOpcode;

typedef struct _HTAlertPar HTAlertPar;

typedef BOOL HTAlertCallback   (HTRequest * request, HTAlertOpcode op,
				int msgnum, const char * dfault, void * input,
				HTAlertPar * reply);

/*

If you don't expect any return values then reply can be NULL.
The return value of the callback function can be used to indicate confirmation
on a prompt (Yes or No).
.
  User Prompts and Questions
.

This is an enumerated list of messages that can be converted into a string
table etc. See the HTDialog module for
default initialization of these strings.
*/

typedef enum _HTAlertMsg {
    HT_MSG_NULL = -1,
    HT_MSG_UID = 0,
    HT_MSG_PROXY_UID,
    HT_MSG_FTP_UID,
    HT_MSG_PW,
    HT_MSG_FILENAME,
    HT_MSG_ACCOUNT,
    HT_MSG_METHOD,
    HT_MSG_MOVED,
    HT_MSG_RULES,
    HT_MSG_FILE_REPLACE,
    HT_MSG_RETRY_AUTHENTICATION,
    HT_MSG_RETRY_PROXY_AUTH,
    HT_MSG_REDO,
    HT_MSG_BIG_PUT,
    HT_MSG_SOURCE_MOVED,
    HT_MSG_DESTINATION_MOVED,
    HT_MSG_REDIRECTION,
    HT_MSG_PROXY,
    HT_MSG_CACHE_LOCK,
    HT_MSG_ACCEPT_COOKIE,
    HT_MSG_ELEMENTS		            /* This MUST be the last element */
} HTAlertMsg;

/*
.
  Enable or Disable Messages
.

If you really don't want the library to prompt for anything at all then enable
this constant. The default value is Interactive.
*/

extern void HTAlert_setInteractive	(BOOL interative);
extern BOOL HTAlert_interactive		(void);

/*
.
  Creation and Deletion Methods
.

Message methods are registered in lists. By default a list is not enabled
before you assign it as being active. This allows
the application to maintain multiple lists of message handlers which can
be swapped in and out as neeeded.
(
  Add a Callback Function
)

Register a call back function that is to be called when generating messages,
dialog, prompts, progress reports etc. The opcode signifies which call back
function to call depending of the type of the message. Opcode can be any
combination of the bitflags defined by HTAlertOpcode. If you
register one callback for HT_A_PROGRESS then this will get called
on all progress notifications.
*/

extern BOOL HTAlertCall_add (HTList * list, HTAlertCallback * cbf,
			     HTAlertOpcode opcode);

/*
(
  Delete a Callback function
)

Unregister a call back function from a list
*/

extern BOOL HTAlertCall_delete (HTList * list, HTAlertCallback * cbf);

/*
(
  Delete all Callbacks With this Opcode
)

Unregister all handlers registered for a given opcode.
*/

extern BOOL HTAlertCall_deleteOpcode (HTList * list, HTAlertOpcode opcode);

/*
(
  Delete a list of Callback Functions
)

Unregisters all call back functions
*/

extern BOOL HTAlertCall_deleteAll (HTList * list);

/*
(
  Find a Callback Function
)

Finds a callback function corresponding to the opcode. If none has been
registered then NULL is returned.
*/

extern HTAlertCallback * HTAlertCall_find(HTList * list, HTAlertOpcode opcode);

/*
.
  The Reply Object
.

The reply object is used for communicating input from the user back
to the Library. This is only required to use when for example the user is
prompted for a file name etc. You can find several examples on how to use
this in the default message and dialog module
provided together with the Library.
*/
extern HTAlertPar * HTAlert_newReply	(void);
extern void HTAlert_deleteReply		(HTAlertPar * old);

/*
(
  Handle the Reply Message
)

These methods provide the API for handling the reply message. There are two
ways of assigning a message to the reply message - either by copying the
buffer or by reusing the same buffer. In the latter case, the caller must
make sure not to free the reply message before it has been used.
*/

extern BOOL HTAlert_setReplyMessage	(HTAlertPar * me, const char *message);
extern BOOL HTAlert_assignReplyMessage	(HTAlertPar * me, char * message);

/*

You can get the data back again by using this method:
*/

extern char * HTAlert_replyMessage	(HTAlertPar * me);

/*
*/

extern char * HTAlert_replySecret	(HTAlertPar * me);
extern BOOL HTAlert_setReplySecret	(HTAlertPar * me, const char * secret);

extern void * HTAlert_replyOutput	(HTAlertPar * me);
extern BOOL HTAlert_setReplyOutput	(HTAlertPar * me, void * output);

/*
.
  Active set of Callback Functions
.

A list can be assigned as being active in which case it is visible
for libwww by assigning the list as the global alert list. Libwww
does not know about inactive lists of alert handlers.
*/

extern void HTAlert_setGlobal	(HTList * list);
extern HTList * HTAlert_global	(void);

/*
(
  Global Alert List Methods
)

You can assign a callback directly to the global list in which case it becomes
immediately available to libwww. In this case you do not need to worry about
creating the list - it will be created as well as deleted automatically.

  Add an Alert Handler

*/

extern BOOL HTAlert_add	(HTAlertCallback * cbf, HTAlertOpcode opcode);

/*

  Delete an Alert Handler


You can either delete a handler by referring to its address or to the opcode
that it has been registered for.
*/

extern BOOL HTAlert_delete (HTAlertCallback * cbf);
extern BOOL HTAlert_deleteOpcode (HTAlertOpcode opcode);

/*

  Delete all Alert Handlers

*/

extern BOOL HTAlert_deleteAll (void);

/*

  Find an Alert Handler

*/

extern HTAlertCallback * HTAlert_find (HTAlertOpcode opcode);

/*
*/

#ifdef __cplusplus
}
#endif

#endif

/*

  

  @(#) $Id$

*/
