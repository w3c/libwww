/*             VMS specific routines
                                             
 */

#ifndef HTVMSUTIL_H
#define HTVMSUTIL_H

/* define some masks as given in Unix stat.h file... */

#define S_IFLNK		0120000
#define S_IFSOCK	0140000
#define S_IFIFO		0010000

#define S_IRWXU         0000700 /* rwx, owner */
#define         S_IRUSR 0000400 /* read permission, owner */
#define         S_IWUSR 0000200 /* write permission, owner */
#define         S_IXUSR 0000100 /* execute/search permission, owner */
#define S_IRWXG         0000070 /* rwx, group */
#define         S_IRGRP 0000040 /* read permission, group */
#define         S_IWGRP 0000020 /* write permission, grougroup */
#define         S_IXGRP 0000010 /* execute/search permission, group */
#define S_IRWXO         0000007 /* rwx, other */
#define         S_IROTH 0000004 /* read permission, other */
#define         S_IWOTH 0000002 /* write permission, other */
#define         S_IXOTH 0000001 /* execute/search permission, other */



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


PUBLIC int HTStat PARAMS((
	CONST char *, filename,
        stat_t *, info));

PUBLIC int HTVMS_checkDecnet PARAMS((
	CONST char *, filename));


/* PUBLIC							HTVMS_getUIC()
**		getUIC routine
** ON ENTRY:
**	username	Username specification
**	
**
** ON EXIT:
**	0		error
**	UIC		VMS User Identification Code for username
**	
*/
PUBLIC unsigned int HTVMS_getUIC PARAMS((
	CONST char *, username));


/* PUBLIC							HTVMS_isOwner()
**		CHECKS OWNERSHIP OF FILE FOR CERTAIN USER
** ON ENTRY:
**	FileName	The file or directory to be checked
**	UserName	Name of the user to check ownership for.
**			User nobody, represented by "" is given NO for an answer
**
** ON EXIT:
**	returns YES if FileName is owned by UserName
**	
*/
PUBLIC BOOL HTVMS_isOwner PARAMS((
	CONST char *, FileName,
	CONST char *, UserName));


#endif /* not HTVMSUTIL_H */
/* End of file HTVMSUtil.h.  */
