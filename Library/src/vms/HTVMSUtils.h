/*             VMS specific routines
                                             
 */

#ifndef HTVMSUTIL_H
#define HTVMSUTIL_H


/* PUBLIC							HTVMS_authSysPrv()
**		CHECKS IF THIS PROCESS IS AUTHORIZED TO ENABLE SYSPRV
** ON ENTRY:
**	No arguments.
**
** ON EXIT:
**	returns	YES if SYSPRV is authorized
*/
PUBLIC BOOL HTVMS_authSysPrv NOPARAMS;


/* PUBLIC							HTVMS_enableSysPrv()
**		ENABLES SYSPRV
** ON ENTRY:
**	No arguments.
**
** ON EXIT:
**	
*/
PUBLIC void HTVMS_enableSysPrv NOPARAMS;


/* PUBLIC							HTVMS_disableSysPrv()
**		DISABLES SYSPRV
** ON ENTRY:
**	No arguments.
**
** ON EXIT:
**	
*/
PUBLIC void HTVMS_disableSysPrv NOPARAMS;

/* PUBLIC							HTVMS_checkAccess()
**		CHECKS ACCESS TO FILE FOR CERTAIN USER
** ON ENTRY:
**	FileName	The file to be accessed
**	UserName	Name of the user to check access for
**
** ON EXIT:
**	returns YES if access is allowed
**	
*/
PUBLIC BOOL HTVMS_checkAccess PARAMS((
	CONST char * FileName,
	CONST char * UserName,
	HTMethod Method));


/* PUBLIC							HTVMS_wwwName()
**		CONVERTS VMS Name into WWW Name 
** ON ENTRY:
**	vmsname		VMS file specification (NO NODE)
**
** ON EXIT:
**	returns 	www file specification
**
** EXAMPLES:
**	vmsname				wwwname
**	DISK$USER 			disk$user
**	DISK$USER: 			/disk$user/
**	DISK$USER:[DUNS] 		/disk$user/duns
**	DISK$USER:[DUNS.ECHO] 		/disk$user/duns/echo
**	[DUNS] 				duns
**	[DUNS.ECHO] 			duns/echo
**	[DUNS.ECHO.-.TRANS] 		duns/echo/../trans
**	[DUNS.ECHO.--.TRANS] 		duns/echo/../../trans
**	[.DUNS] 			duns
**	[.DUNS.ECHO] 			duns/echo
**	[.DUNS.ECHO]TEST.COM 		duns/echo/test.com 
**	TEST.COM 			test.com
**
**	
*/
PUBLIC char * HTVMS_wwwName PARAMS((
	char * vmsname));

/* PUBLIC							HTVMS_name()
**		CONVERTS WWW name into a VMS name
** ON ENTRY:
**	nn		Node Name (optional)
**	fn		WWW file name
**
** ON EXIT:
**	returns 	vms file specification
**
** Bug:	Returns pointer to static -- non-reentrant
*/
PUBLIC char * HTVMS_name PARAMS((
	CONST char * nn, 
	CONST char * fn));

/* PUBLIC							HTVMS_putenv()
**		Create logical name
** ON ENTRY:
**	string		name value pair separated by equal sign. 
**			"name=value"
**	
**
** ON EXIT:
**	
*/
PUBLIC int HTVMS_putenv PARAMS((
	CONST char * string));

/* PUBLIC							HTVMS_start_timer()
**		Starts a timer
** ON ENTRY:
**	n		A number to recognise the timer. 0 may be used to 
**			say that recognision is not needed
**	fun		ptr to a function to be called asynchronously
**	sec		number of seconds to wait before firing.
**	
**
** ON EXIT:
**	
*/
PUBLIC void HTVMS_start_timer PARAMS((
	CONST int n,
	CONST (*fun)(),
	CONST int sec));


/* PUBLIC							HTVMS_cancel_timer()
**		Cancel a specific or all timers
** ON ENTRY:
**	n		timer number (0 to cancel all timers)
**	
**
** ON EXIT:
**	
*/
PUBLIC void HTVMS_cancel_timer PARAMS((
	CONST int n));



#define getpwnam HTVMS_getpwnam
struct passwd
{
   char *pw_dir;		/* Home Directory */
};


/* PUBLIC							HTVMS_getpwnam()
**		getpwnam routine
** ON ENTRY:
**	username	Username specification
**	
**
** ON EXIT:
**	NULL		error
**	
*/
PUBLIC struct passwd *HTVMS_getpwnam PARAMS((
	CONST char *, username));


#endif /* not HTVMSUTIL_H */
/*
   End of file HTVMSUtil.h.  */
