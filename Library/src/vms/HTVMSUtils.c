/* MODULE							HTVMSUtil.c
**		VMS Utility Routines
**
** AUTHORS:
**	MD	Mark Donszelmann    duns@vxdeop.cern.ch
**	FM	Foteos Macrides	    macrides@sci.wfeb.edu
**
** HISTORY:
**	14 Nov 93  MD	Written
**	31 Mar 94  FM	Added HTVMS_disableAllPrv
**	06 Jul 94  FM	Added HTVMS_getUIC and HTVMS_isOwner
**
** BUGS:
**
**
*/

#include <ssdef.h>
#include <jpidef.h>
#include <uaidef.h>
#include <prvdef.h>
#include <acldef.h>
#include <armdef.h>
#include <chpdef.h>
#include <lnmdef.h>
#include <libdtdef.h>
#include <descrip.h>

#include <unixlib.h>
#include <stdio.h>
#include <stat.h>
#include <time.h>

#include "HTUtils.h"
#include "HTVMSUtils.h"
#include "HTAccess.h"

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

      if (Result == SS$_NORMAL) {
         if (!(PreviousPrv[0] & PRV$M_SYSPRV)) {
            CTRACE(TDEST, "VMS......... Enabled SYSPRV\n");
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

      if (Result == SS$_NORMAL) {
         if (PreviousPrv[0] & PRV$M_SYSPRV) {
            CTRACE(TDEST, "VMS......... Disabled SYSPRV\n");
         }
   }
}



/* PUBLIC						HTVMS_disableAllPrv()
**
**		Disable all privileges except TMPMBX and NETMBX
**
** ON ENTRY:
**	No arguments.
**
** ON EXIT:
**
*/
PUBLIC void HTVMS_disableAllPrv NOARGS
{
#ifndef MULTINET
#define bzero(s,n) memset(s,'\0',n)
#endif
    union prvdef prvadr;

    bzero((char *) &prvadr, sizeof(prvadr));
    prvadr.prv$v_cmkrnl = 1;      
    prvadr.prv$v_cmexec = 1;      
    prvadr.prv$v_sysnam = 1;      
    prvadr.prv$v_grpnam = 1;      
    prvadr.prv$v_allspool = 1;    
    prvadr.prv$v_detach = 1;      
    prvadr.prv$v_diagnose = 1;    
    prvadr.prv$v_log_io = 1;      
    prvadr.prv$v_group = 1;       
    prvadr.prv$v_noacnt = 1;      
    prvadr.prv$v_prmceb = 1;      
    prvadr.prv$v_prmmbx = 1;      
    prvadr.prv$v_pswapm = 1;      
    prvadr.prv$v_setpri = 1;      
    prvadr.prv$v_setprv = 1;      
    prvadr.prv$v_world = 1;       
    prvadr.prv$v_mount = 1;       
    prvadr.prv$v_oper = 1;        
    prvadr.prv$v_exquota = 1;     
    prvadr.prv$v_volpro = 1;      
    prvadr.prv$v_phy_io = 1;      
    prvadr.prv$v_bugchk = 1;      
    prvadr.prv$v_prmgbl = 1;      
    prvadr.prv$v_sysgbl = 1;      
    prvadr.prv$v_pfnmap = 1;      
    prvadr.prv$v_shmem = 1;       
    prvadr.prv$v_sysprv = 1;      
    prvadr.prv$v_bypass = 1;      
    prvadr.prv$v_syslck = 1;      
    prvadr.prv$v_share = 1;       
    prvadr.prv$v_upgrade = 1;     
    prvadr.prv$v_downgrade = 1;   
    prvadr.prv$v_grpprv = 1;      
    prvadr.prv$v_readall = 1;     
    prvadr.prv$v_security = 1;    
}


/* PUBLIC							HTVMS_checkAccess()
**		CHECKS ACCESS TO FILE FOR CERTAIN USER
** ON ENTRY:
**	FileName	The file to be accessed
**	UserName	Name of the user to check access for.
**			User nobody, represented by "" is given NO for an answer
**	Method		Method to be checked for
**			'method'	'access reuired'
**			METHOD_GET	read
**			METHOD_HEAD	read
**
** ON EXIT:
**	returns YES if access is allowed
**
** Not only filename is checked but also filename.dir...
**	
*/
PUBLIC BOOL HTVMS_checkAccess ARGS3(
	CONST char *, FileName,
	CONST char *, UserName,
	HTMethod, Method)
{
unsigned long Result;
ItemStruct ItemList[2];
unsigned long Flags;
unsigned long FlagsLength;
unsigned long Access;
unsigned long AccessLength;
unsigned long ObjType;

char *VmsName;
char Fname[256];

struct dsc$descriptor_s FileNameDesc;
struct dsc$descriptor_s UserNameDesc;

char *colon;

   /* user nobody should access as from account under which server is running */
   if (0 == strcmp(UserName,""))
   {
      CTRACE(TDEST, "VMSAccess... No access allowed user nobody. Error in rulefile specifying 'nobody' as uid for protect rule\n");
      return(NO);
   }

   /* make local copy */
   strcpy(Fname,FileName);

   /* strip off last slash anyway */
   if (Fname[strlen(Fname)-1] == '/')
      Fname[strlen(Fname)-1] = '\0';

   /* check Filename and convert */
   colon = strchr(Fname,':');
   if (colon)
      VmsName = HTVMS_name("",colon+1);
   else
      VmsName = HTVMS_name("",Fname);

   /* check for GET */
   if ((Method == METHOD_GET) ||
       (Method == METHOD_HEAD)) 
   {
     Access = ARM$M_READ;
     Flags = CHP$M_READ;

     /* fill Access */
     ItemList[0].BufferLength = sizeof(Access);
     ItemList[0].BufferAddress = &Access;
     ItemList[0].ReturnLengthAddress = &AccessLength;
     ItemList[0].ItemCode = CHP$_ACCESS;

     /* fill Flags */
     ItemList[1].BufferLength = sizeof(Flags);
     ItemList[1].BufferAddress = &Flags;
     ItemList[1].ReturnLengthAddress = &FlagsLength;
     ItemList[1].ItemCode = CHP$_FLAGS;

     /* terminate list */
     ItemList[2].ItemCode = 0;
     ItemList[2].BufferLength = 0;

     /* fill input */
     ObjType = ACL$C_FILE;
     UserNameDesc.dsc$w_length = strlen(UserName);
     UserNameDesc.dsc$b_dtype = DSC$K_DTYPE_T;
     UserNameDesc.dsc$b_class = DSC$K_CLASS_S;
     UserNameDesc.dsc$a_pointer = UserName;
     FileNameDesc.dsc$w_length = strlen(VmsName);
     FileNameDesc.dsc$b_dtype = DSC$K_DTYPE_T;
     FileNameDesc.dsc$b_class = DSC$K_CLASS_S;
     FileNameDesc.dsc$a_pointer = VmsName;

     /* call system for filename */
     Result = SYS$CHECK_ACCESS(&ObjType,&FileNameDesc,&UserNameDesc,ItemList);

     if (Result == SS$_NORMAL)
        return(YES);

     /* try with extension .dir... */
     strcat(VmsName,".dir");
     FileNameDesc.dsc$w_length = strlen(VmsName);
     Result = SYS$CHECK_ACCESS(&ObjType,&FileNameDesc,&UserNameDesc,ItemList);

     if (Result == SS$_NORMAL)
        return(YES);

     /* failed for filename and .dir */
     CTRACE(TDEST, "VMSAccess... No access allowed for user '%s', file '%s' under method '%s'\n",UserName,Fname,HTMethod_name(Method));
     return(NO);
   }

   CTRACE(TDEST, "VMSAccess... No access allowed for method '%s'\n",HTMethod_name(Method));

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
**	[.DUNS.ECHO]TEST-BLAH.COM	duns/echo/test-blah.com
**	[.DUNS-ECHO]TEST.COM 		duns-echo/test.com
**	[.X-----]TEST.COM		x-----/test.com
**	TEST.COM 			test.com
**	DISK$USER:[DUNS].RHOSTS		/disk$user/duns/.rhosts 
**
**	
*/
PUBLIC char * HTVMS_wwwName ARGS1(
	char *, vmsname)
{
static char wwwname[256];
char *src, *dst;
int dir, up;
   dst = wwwname;
   src = vmsname;
   dir = 0;
   up = 0;
   if (strchr(src,':')) *(dst++) = '/';
   for ( ; *src != '\0' ; src++)
   {
      switch(*src)
      {
         case ':':  *(dst++) = '/'; break;
         case '-': /* dashes might exist in filename and directory names... */
	           if (up)
                   { /* directory and up */
		      if (*(src-1) == '-') *(dst++) = '/';
                      *(dst++) = '.'; 
                      *(dst++) = '.'; 
                   }
                   else
                   { /* filename or directory name with a dash */
                      *(dst++) = '-';
                   }
                   break;
         case '.': if (dir)
                   {
                      if (*(src-1) != '[') *(dst++) = '/';
                      up = 1;
                   }
                   else
                   {
  		      if (*(src-1) == ']') *(dst++) = '/';
                      *(dst++) = '.';
                   }
                   break;
         case '[': dir = 1; up = 1; break;
         case ']': dir = 0; up = 0; break;
         default:  up = 0;
		   if (*(src-1) == ']') *(dst++) = '/';
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
    
    char * hostname = HTGetHostName();

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


/* PUBLIC							HTVMS_putenv()
**		Create logical name
** ON ENTRY:
**	string		name value pair separated by equal sign. 
**			"name=value"
**	
**
** ON EXIT:
**	0 		ok
**	1		failed
**	
*/
PUBLIC int HTVMS_putenv ARGS1(
	CONST char *, string)
{
    $DESCRIPTOR(job_table,"LNM$JOB");
    char * name = (char*)malloc(strlen(string)+1);
    char * value = (char*)malloc(strlen(string)+1);	/* Copies to hack */
    char * equal;
    long result = 1;

    /* separate pair */
    strcpy(name,string);
    equal = strchr(name,'=');
    if (equal)
    {
    ItemStruct ItemList[2];
    struct dsc$descriptor_s log_name;

       /* get value */
       *equal = '\0';
       strcpy(value,equal+1);

       /* fill logical name */       
       log_name.dsc$w_length = strlen(name);
       log_name.dsc$b_dtype = DSC$K_DTYPE_T;
       log_name.dsc$b_class = DSC$K_CLASS_S;
       log_name.dsc$a_pointer = name;

       /* fill Item */
       ItemList[0].BufferLength = strlen(value);
       ItemList[0].BufferAddress = value;
       ItemList[0].ReturnLengthAddress = 0;
       ItemList[0].ItemCode = LNM$_STRING;

       /* terminate list */
       ItemList[1].ItemCode = 0;
       ItemList[1].BufferLength = 0;

       /* put into job logical name table */
       result = SYS$CRELNM(0, &job_table, &log_name, 0, ItemList);
       if ((result == SS$_NORMAL) || (result == SS$_SUPERSEDE))
          result = 0;
       else
          result = 1;
    }    

    free(name);
    free(value);

    return(result);
}




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
PUBLIC void HTVMS_start_timer ARGS3(
	CONST int, n,
	CONST char *, fun,
	CONST int, sec)
{
long Result;
long DeltaTime[2];
long Operation = LIB$K_DELTA_SECONDS;

   Result = LIB$CVT_TO_INTERNAL_TIME(&Operation, &sec, DeltaTime);
   
   Result = SYS$SETIMR(0, DeltaTime, fun, n, 0);
}


/* PUBLIC							HTVMS_cancel_timer()
**		Cancel a specific or all timers
** ON ENTRY:
**	n		timer number (0 to cancel all timers)
**	
**
** ON EXIT:
**	
*/
PUBLIC void HTVMS_cancel_timer ARGS1(
	CONST int, n)
{
long Result;

   Result = SYS$CANTIM(n, 0);
}



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
PUBLIC struct passwd *HTVMS_getpwnam ARGS1(
	CONST char *, username)
{
long Result;
struct dsc$descriptor_s UserNameDesc;
ItemStruct ItemList[3];
unsigned long DeviceLength;
char Device[33];
unsigned long DirLength;
char Dir[64];
char VMSName[100];
static struct passwd pw;
static char pw_dir[100];

   /* make sure pointers are correct */
   pw.pw_dir = pw_dir;

   /* construct UserName */
   UserNameDesc.dsc$w_length = strlen(username);
   UserNameDesc.dsc$b_dtype = DSC$K_DTYPE_T;
   UserNameDesc.dsc$b_class = DSC$K_CLASS_S;
   UserNameDesc.dsc$a_pointer = username;

   /* Default Device */
   ItemList[0].BufferLength = 33;
   ItemList[0].BufferAddress = Device;
   ItemList[0].ReturnLengthAddress = &DeviceLength;
   ItemList[0].ItemCode = UAI$_DEFDEV;

   /* Default Directory */
   ItemList[1].BufferLength = 64;
   ItemList[1].BufferAddress = Dir;
   ItemList[1].ReturnLengthAddress = &DirLength;
   ItemList[1].ItemCode = UAI$_DEFDIR;

   /* terminate list */
   ItemList[2].ItemCode = 0;
   ItemList[2].BufferLength = 0;

   /* get info */
   Result = SYS$GETUAI(0,0,&UserNameDesc,ItemList,0,0,0);
   if (Result != SS$_NORMAL)
      return(NULL);

   /* create vms name */
   strncpy(VMSName,&(Device[1]),Device[0]);
   VMSName[Device[0]] = '\0';
   strncat(VMSName,&(Dir[1]),Dir[0]);
   VMSName[Device[0]+Dir[0]] = '\0';
   
   /* convert it into www name */
   strcpy(pw_dir,HTVMS_wwwName(VMSName));

   return(&pw);
}



PUBLIC int HTStat ARGS2(
	char *, filename, 
	stat_t *, info)
{
   /* 
      the following stuff does not work in VMS with a normal stat...
      -->   /disk$user/duns/www if www is a directory
		is statted like: 	/disk$user/duns/www.dir 
		after a normal stat has failed
      -->   /disk$user/duns	if duns is a toplevel directory
		is statted like:	/disk$user/000000/duns.dir
      -->   /disk$user since disk$user is a device
		is statted like:	/disk$user/000000/000000.dir
      -->   /			
		searches all devices, no solution yet...
      -->   /vxcern!/disk$cr/wwwteam/login.com
		is not statted but granted with fake information...
   */
int Result;
int Len;
char *Ptr, *Ptr2;
char Name[256];

   /* try normal stat... */
   Result = stat(filename,info);
   if (Result == 0)
      return(Result);

   /* make local copy */
   strcpy(Name,filename);

   /* if filename contains a node specification (! or ::), we will try to access
      the file via DECNET, but we do not stat it..., just return success 
      with some fake information... */
   if (HTVMS_checkDecnet(Name))
   {
      /* set up fake info, only the one we use... */
      info->st_dev = NULL;
      info->st_ino[0] = 0;
      info->st_ino[1] = 0;
      info->st_ino[2] = 0;
      info->st_mode = S_IFREG | S_IREAD;	/* assume it is a regular Readable file */
      info->st_nlink = NULL;
      info->st_uid = 0;
      info->st_gid = 0;
      info->st_rdev = 0;
      info->st_size = 0;
      info->st_atime = time(NULL);
      info->st_mtime = time(NULL);
      info->st_ctime = time(NULL);

      return(0);
   }

   /* failed,so do device search in case root is requested */
   if (!strcmp(Name,"/"))
   {  /* root requested */
      return(-1);
   }
   
   /* failed so this might be a directory, add '.dir' */
   Len = strlen(Name);
   if (Name[Len-1] == '/')
      Name[Len-1] = '\0';
   
   /* fail in case of device */
   Ptr = strchr(Name+1,'/');
   if ((Ptr == NULL) && (Name[0] == '/'))
   {  /* device only... */
      strcat(Name,"/000000/000000");
   }
   
   if (Ptr != NULL)
   {  /* correct filename in case of toplevel dir */
      Ptr2 = strchr(Ptr+1,'/');
      if ((Ptr2 == NULL) && (Name[0] == '/'))
      {
         char End[256];
         strcpy(End,Ptr);
         *(Ptr+1) = '\0';
         strcat(Name,"000000");
         strcat(Name,End);
      }
   }

   /* try in case a file on toplevel directory or .DIR was alreadyt specified */
   Result = stat(Name,info);
   if (Result == 0)
      return(Result);

   /* add .DIR and try again */
   strcat(Name,".dir");
   Result = stat(Name,info);
   return(Result);
}



PUBLIC int HTVMS_checkDecnet ARGS1(
	char *, filename) 
{
    /* for VMS we take of the first slash in case a nodename is specified 
       to make sure we access over decnet */
    /* nodenames can only be specified if the first part in the filename
       contains either '!', just before the second slash, or '::' */
int slash = 1;
    if (filename[0] == '/')
       for (slash = 1; (filename[slash] != '\0') && (filename[slash] != '/'); slash++)
          ;

    /* second slash found */
    if (filename[slash] == '/')
    {  /* unix syntax */
       if ( ((slash >= 1) && (filename[slash-1] == '!')) || 
            ((slash >= 3) && (!strcmp(&filename[slash-3],"%21"))) )
       {
           char *p;
           for (p = filename; *(p+1) != '\0'; p++)
              *p = *(p+1);
           *p = *(p+1); 
           return(1);
       }
    }
    else
    {  /* vms syntax */
       if (strstr(filename,"::") ||
           strstr(filename,"%3A%3A") )
       {
           char *p;
           for (p = filename; *(p+1) != '\0'; p++)
              *p = *(p+1);
           *p = *(p+1); 
           return(1);
       }
    }
    return(0);
}



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
PUBLIC unsigned int HTVMS_getUIC ARGS1(
	CONST char *, username)
{
long Result;
struct dsc$descriptor_s UserNameDesc;
ItemStruct ItemList[2];
unsigned long UIC;
unsigned long UICLength;

   /* construct UserName */
   UserNameDesc.dsc$w_length = strlen(username);
   UserNameDesc.dsc$b_dtype = DSC$K_DTYPE_T;
   UserNameDesc.dsc$b_class = DSC$K_CLASS_S;
   UserNameDesc.dsc$a_pointer = username;

   /* UIC */
   ItemList[0].BufferLength = sizeof(UIC);
   ItemList[0].BufferAddress = &UIC;
   ItemList[0].ReturnLengthAddress = &UICLength;
   ItemList[0].ItemCode = UAI$_UIC;

   /* terminate list */
   ItemList[1].ItemCode = 0;
   ItemList[1].BufferLength = 0;

   /* get info */
   Result = SYS$GETUAI(0,0,&UserNameDesc,ItemList,0,0,0);
   if (Result != SS$_NORMAL)
      return(0);
   else
      return((unsigned int) UIC);
}



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
PUBLIC BOOL HTVMS_isOwner ARGS2(
	CONST char *, FileName,
	CONST char *, UserName)
{
struct stat buf;
int Result;

   /* user nobody should access as from account under which server is running */
   if (0 == strcmp(UserName,""))
   {
      CTRACE(TDEST, "VMSisOwner.. No access allowed user nobody.\n");
      return(NO);
   }

   /* load buf */
   Result = HTStat(FileName, &buf);
   if (Result) {
      CTRACE(TDEST, "VMSisOwner.. HTStat() failed for '%s'\n", FileName);
      return NO;  /* no file or directory, so UserName can't own it */
   }

   /* compare with UIC for UserName */
   if (buf.st_uid != HTVMS_getUIC(UserName)) {
      CTRACE(TDEST, "VMSisOwner.. '%s' does not own '%s'\n",
      		     UserName, FileName);
      return NO;
   }

   return YES;
}

