
/* MODULE							HTVMSUtil.c
**		VMS Utility Routines
**
** AUTHORS:
**	MD	Mark Donszelmann    duns@vxdeop.cern.ch
**
** HISTORY:
**	14 Nov 93  MD	Written
**
** BUGS:
**
**
*/

#include <ssdef.h>
#include <jpidef.h>
#include <prvdef.h>
#include <acldef.h>
#include <chpdef.h>
#include <descrip.h>

#include <unixlib.h>
#include <stdio.h>

#include "HTUtils.h"
#include "HTVMSUtils.h"

#define INFINITY 512            /* file name length @@ FIXME */


typedef struct {
   unsigned BufferLength : 16;
   unsigned ItemCode : 16;
   unsigned BufferAddress : 32;
   unsigned ReturnLengthAddress : 32;
} ItemStruct;

/* PUBLIC							HTVMS_authSysPrv()
**		CHECKS IF THIS PROCESS IS AUTHORIZED TO ENABLE SYSPRV
** ON ENTRY:
**	No arguments.
**
** ON EXIT:
**	returns	YES if SYSPRV is authorized
*/
PUBLIC BOOL HTVMS_authSysPrv NOARGS
{
unsigned long Result;
ItemStruct ItemList[2];
unsigned long Length;
unsigned long Buffer[2];

  /* fill Item */
  ItemList[0].BufferLength = sizeof(Buffer);
  ItemList[0].BufferAddress = Buffer;
  ItemList[0].ReturnLengthAddress = &Length;
  ItemList[0].ItemCode = JPI$_AUTHPRIV;

  /* terminate list */
  ItemList[1].ItemCode = 0;
  ItemList[1].BufferLength = 0;

  /* call system */
  Result = SYS$GETJPIW(0, 0, 0, ItemList, 0, 0, 0);

  if (Result != SS$_NORMAL)
     return(NO);  

  if (Buffer[0] & PRV$M_SYSPRV)
     return(YES);

  return(NO);  
}



/* PUBLIC							HTVMS_enableSysPrv()
**		ENABLES SYSPRV
** ON ENTRY:
**	No arguments.
**
** ON EXIT:
**	
*/
PUBLIC void HTVMS_enableSysPrv NOARGS
{
unsigned long Result;
unsigned long Prv[2], PreviousPrv[2];

   Prv[0] = PRV$M_SYSPRV;
   Prv[1] = 0;
   Result = SYS$SETPRV(1,&Prv,0,&PreviousPrv);

   if (TRACE) {
      if (Result == SS$_NORMAL) {
         if (!(PreviousPrv[0] & PRV$M_SYSPRV)) {
            fprintf(stderr, "HTVMS_enableSysPrv: Enabled SYSPRV\n");
         }
      }
   }
}



/* PUBLIC							HTVMS_disableSysPrv()
**		DISABLES SYSPRV
** ON ENTRY:
**	No arguments.
**
** ON EXIT:
**	
*/
PUBLIC void HTVMS_disableSysPrv NOARGS
{
unsigned long Result;
unsigned long Prv[2], PreviousPrv[2];

   Prv[0] = PRV$M_SYSPRV;
   Prv[1] = 0;
   Result = SYS$SETPRV(0,&Prv,0,&PreviousPrv);

   if (TRACE) {
      if (Result == SS$_NORMAL) {
         if (PreviousPrv[0] & PRV$M_SYSPRV) {
            fprintf(stderr, "HTVMS_disableSysPrv: Disabled SYSPRV\n");
         }
      }
   }
}



/* PUBLIC							HTVMS_checkAccess()
**		CHECKS ACCESS TO FILE FOR CERTAIN USER
** ON ENTRY:
**	FileName	The file to be accessed
**	UserName	Name of the user to check access for.
**			User nobody, represented by "" is given NO for an answer
**	Method		Name of the method to be chceked
**
** ON EXIT:
**	returns YES if access is allowed
**	
*/
PUBLIC BOOL HTVMS_checkAccess ARGS3(
	CONST char *, FileName,
	CONST char *, UserName,
	CONST char *, Method)
{
unsigned long Result;
ItemStruct ItemList[2];
unsigned long Length;
unsigned long Buffer;
unsigned long ObjType;

char *VmsName;

struct dsc$descriptor_s FileNameDesc;
struct dsc$descriptor_s UserNameDesc;

char *colon;

   /* user nobody should access as from account under which server is running */
   if (0 == strcmp(UserName,""))
      return(NO);

   /* check Filename and convert */
   colon = strchr(FileName,':');
   if (colon)
      VmsName = HTVMS_name("",colon+1);
   else
      VmsName = HTVMS_name("",FileName);

   /* check for GET */
   if (0 == strcmp(Method,"GET"))
   {
     /* fill Item */
     ItemList[0].BufferLength = sizeof(Buffer);
     ItemList[0].BufferAddress = &Buffer;
     ItemList[0].ReturnLengthAddress = &Length;
     ItemList[0].ItemCode = CHP$_FLAGS;

     /* terminate list */
     ItemList[1].ItemCode = 0;
     ItemList[1].BufferLength = 0;

     /* fill input */
     ObjType = ACL$C_FILE;
     Buffer = CHP$M_READ;
     UserNameDesc.dsc$w_length = strlen(UserName);
     UserNameDesc.dsc$b_dtype = DSC$K_DTYPE_T;
     UserNameDesc.dsc$b_class = DSC$K_CLASS_S;
     UserNameDesc.dsc$a_pointer = UserName;
     FileNameDesc.dsc$w_length = strlen(VmsName);
     FileNameDesc.dsc$b_dtype = DSC$K_DTYPE_T;
     FileNameDesc.dsc$b_class = DSC$K_CLASS_S;
     FileNameDesc.dsc$a_pointer = VmsName;

     /* call system */
     Result = SYS$CHECK_ACCESS(&ObjType,&FileNameDesc,&UserNameDesc,ItemList);

     if (Result == SS$_NORMAL)
        return(YES);
     else
        return(NO);
   }

   return(NO);
}




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
PUBLIC char * HTVMS_wwwName ARGS1(
	char *, vmsname)
{
static char wwwname[256];
char *src, *dst;
int dir;
   dst = wwwname;
   src = vmsname;
   dir = 0;
   if (strchr(src,':')) *(dst++) = '/';
   for ( ; *src != '\0' ; src++)
   {
      switch(*src)
      {
         case ':':  *(dst++) = '/'; break;
         case '-': if (*(src-1) == '-') *(dst++) = '/';
                   *(dst++) = '.'; 
                   *(dst++) = '.'; 
                   break;
         case '.': if (dir)
                   {
                      if (*(src-1) != '[') *(dst++) = '/';
                   }
                   else
                      *(dst++) = '.';
                   break;
         case '[': dir = 1; break;
         case ']': dir = 0; break;
         default:  if (*(src-1) == ']') *(dst++) = '/';
                   *(dst++) = *src; 
                   break;
      }
   }
   *(dst++) = '\0';
   return(wwwname);
}


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
PUBLIC char * HTVMS_name ARGS2(
	CONST char *, nn, 
	CONST char *, fn)
{

/*	We try converting the filename into Files-11 syntax. That is, we assume
**	first that the file is, like us, on a VMS node. We try remote
**	(or local) DECnet access. Files-11, VMS, VAX and DECnet
**	are trademarks of Digital Equipment Corporation. 
**	The node is assumed to be local if the hostname WITHOUT DOMAIN
**	matches the local one. @@@
*/
    static char vmsname[INFINITY];	/* returned */
    char * filename = (char*)malloc(strlen(fn)+1);
    char * nodename = (char*)malloc(strlen(nn)+2+1);	/* Copies to hack */
    char *second;		/* 2nd slash */
    char *last;			/* last slash */
    
    char * hostname = HTHostName();

    if (!filename || !nodename) outofmem(__FILE__, "HTVMSname");
    strcpy(filename, fn);
    strcpy(nodename, "");	/* On same node? Yes if node names match */
    {
        char *p, *q;
        for (p=hostname, q=nn; *p && *p!='.' && *q && *q!='.'; p++, q++){
	    if (TOUPPER(*p)!=TOUPPER(*q)) {
	        strcpy(nodename, nn);
		q = strchr(nodename, '.');	/* Mismatch */
		if (q) *q=0;			/* Chop domain */
		strcat(nodename, "::");		/* Try decnet anyway */
		break;
	    }
	}
    }

    second = strchr(filename+1, '/');		/* 2nd slash */
    last = strrchr(filename, '/');	/* last slash */
        
    if (!second) {				/* Only one slash */
	sprintf(vmsname, "%s%s", nodename, filename + 1);
    } else if(second==last) {		/* Exactly two slashes */
	*second = 0;		/* Split filename from disk */
	sprintf(vmsname, "%s%s:%s", nodename, filename+1, second+1);
	*second = '/';	/* restore */
    } else { 				/* More than two slashes */
	char * p;
	*second = 0;		/* Split disk from directories */
	*last = 0;		/* Split dir from filename */
	sprintf(vmsname, "%s%s:[%s]%s",
		nodename, filename+1, second+1, last+1);
	*second = *last = '/';	/* restore filename */
	for (p=strchr(vmsname, '['); *p!=']'; p++)
	    if (*p=='/') *p='.';	/* Convert dir sep.  to dots */
    }
    free(nodename);
    free(filename);
    return vmsname;
}


