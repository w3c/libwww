/*			File Transfer Protocol (FTP) Client
**			for a WorldWideWeb browser
**			===================================
**
**	A cache of control connections is kept.
**
** Note: Port allocation
**
**	It is essential that the port is allocated by the system, rather
**	than chosen in rotation by us (POLL_PORTS), or the following
**	problem occurs.
**
**	It seems that an attempt by the server to connect to a port which has
**	been used recently by a listen on the same socket, or by another
**	socket this or another process causes a hangup of (almost exactly)
**	one minute. Therefore, we have to use a rotating port number.
**	The problem remains that if the application is run twice in quick
**	succession, it will hang for what remains of a minute.
**
** Authors
**	TBL	Tim Berners-lee <timbl@info.cern.ch>
**	DD	Denis DeLaRoca 310 825-4580 <CSP1DWD@mvs.oac.ucla.edu>
**      LM      Lou Montulli <montulli@ukanaix.cc.ukans.edu>
**      FM      Foteos Macrides <macrides@sci.wfeb.edu>
**	HF	Henrik Frystyk <frystyk@dxcern.cern.ch>
**	AL	Ari Luotonen <luotonen@www.cern.ch>
**
** History:
**	 2 May 91	Written TBL, as a part of the WorldWideWeb project.
**	15 Jan 92	Bug fix: close() was used for NETCLOSE for control soc
**	10 Feb 92	Retry if cached connection times out or breaks
**	 8 Dec 92	Bug fix 921208 TBL after DD
**	17 Dec 92	Anon FTP password now just WWWuser@ suggested by DD
**			fails on princeton.edu!
**	27 Dec 93 (FM)  Fixed up so FTP now works with VMS hosts.  Path
**			must be Unix-style and cannot include the device
**			or top directory.
**      ?? ??? ?? (LM)  Added code to prompt and send passwords for non
**			anonymous FTP
**      25 Mar 94 (LM)  Added code to recognize different ftp server types
**                      and code to parse dates and sizes on most hosts.
**	27 Mar 93 (FM)  Added code for getting dates and sizes on VMS hosts.
**	27 Apr 94 (HF)  The module is basically rewritten to conform with
**			rfc 959, 1123 and 1579 and turned into a state 
**			machine. New semantics of ftp URLs are supported.
**	 2 May 94 (AL)	Fixed possible security hole when the URL contains
**			a newline, that could cause multiple commands to be
**			sent to an FTP server.
**
** Options:
**	LISTEN		The default way to open a dats connection is by using
**			PASV, but if that fails, we try PORT. If the PORT part
**			is unwanted, it can be disabled by undefine LISTEN.
**
** Notes:
**     			Portions Copyright 1994 Trustees of Dartmouth College
** 			Code for recognizing different FTP servers and
**			parsing "ls -l" output taken from Macintosh Fetch
**			program with permission from Jim Matthews,
**			Dartmouth Software Development Team.
**
**	BUGS:	@@@  	Use configuration file for user names
**
*/

/* Implementation dependent include files */
#include "tcp.h"

/* Library include files */
#include "HTParse.h"
#include "HTUtils.h"
#include "HTTCP.h"
#include "HTAnchor.h"
#include "HTFile.h"
#include "HTBTree.h"
#include "HTChunk.h"
#include "HTAlert.h"
#include "HTDirBrw.h"
#include "HTError.h"
#include "HTTCP.h"
#include "HTFTP.h"					 /* Implemented here */

#ifdef VMS
#include "HTVMSUtils.h"
#endif /* VMS */

/* Macros and other defines */
/* If LISTEN is defined, then first 'PASV' then 'PORT' (if error) is tried,
   else ONLY 'PASV' is used in order to establish a data connection. */
#define LISTEN
#ifdef LISTEN
/* #define REPEAT_LISTEN */    		  /* Reuse the portnumber once found */
/* #define POLL_PORTS */      /* If allocation does not work, poll ourselves.*/
#endif

#define FTP_DEFAULT_TIMEOUT	1000L			    /* x/100 seconds */

#ifndef IPPORT_FTP
#define IPPORT_FTP		21
#endif

#define WWW_FTP_CLIENT "WWWuser"         /* If can't get user-info, use this */

/* Globals */
PUBLIC BOOL HTFTPUserInfo = YES;
PUBLIC long HTFTPTimeOut = FTP_DEFAULT_TIMEOUT;

/* Type definitions and global variables etc. local to this module */ 
#ifdef POLL_PORTS
#define FIRST_TCP_PORT	 1024	       /* Region to try for a listening port */
#define LAST_TCP_PORT	 5999
PRIVATE	unsigned short 	 port_number = FIRST_TCP_PORT;
#endif

#ifdef LISTEN
#ifdef REPEAT_LISTEN
PRIVATE int     master_socket = -1;	       /* Listening socket = invalid */
#endif
PRIVATE char *	this_addr;				    /* Local address */
#endif

/* This is the local definition of HTRequest->net_info. In the special
   case of FTP we need two structures: one for the data connection and one
   for the control connection */
typedef enum _HTFTPServerType {
    UNKNOWN = -1,
    GENERIC_SERVER = 0,
    MACHTEN_SERVER,
    UNIX_SERVER,
    VMS_SERVER,
    CMS_SERVER,
    DCTS_SERVER,
    TCPC_SERVER,
    PETER_LEWIS_SERVER,
    NCSA_SERVER,
    WINDOWS_NT
} HTFTPServerType;

typedef enum _HTFTPMainState {
    FTP_ERROR = -2,
    FTP_FAILURE = -1,
    FTP_IDLE = 0,
    FTP_BEGIN,
    FTP_LOGGED_IN,
    FTP_GOT_DATA_CON,
    FTP_GOT_SERVER_INFO,
    FTP_GOT_DATA
} HTFTPState;

typedef struct _user_info {
    char *		     	domain;
    char *			id;
    char *			passwd;
} user_info;

/* The ftp_ctrl_info and ftp_data_info are both subclasses of the HTNetInfo
   structure defined in HTAccess.html */
typedef struct _ftp_ctrl_info {
    int				sockfd;	  /* Socket number for communication */
    HTInputSocket *             isoc;			     /* Input buffer */
    int 			addressCount;	  /* Attempts if multi-homed */
    BOOL			CRLFdotCRLF;   /* Transmission end like this */
    HTRequest *			request;	      /* Link to the request */

    u_long			serv_node;           /* IP address of server */
    u_short			serv_port;	    /* Port number on server */
    char *			location;        /* Current escaped position */
    user_info *			user;           /* Userid, passwd and domain */
    HTChunk *			welcome;	      /* The welcome message */
    HTChunk *			reply;	           /* Last reply from server */
    HTFTPServerType		server;		           /* Type of server */
    BOOL			unsure_type;         /* Sure about the type? */
    BOOL			use_list;	  	  /* Can we use LIST */
    HTFTPState     		state;		  /* State of the connection */
    HTList *			data_cons;           /* The data connections */
} ftp_ctrl_info;

/* We assume that the data connection is established between the same hosts
   as the control connection */
typedef struct _ftp_data_info {
    int				sockfd;	  /* Socket number for communication */
    HTInputSocket *             isoc;			     /* Input buffer */
    int 			addressCount;	  /* Attempts if multi-homed */
    BOOL			CRLFdotCRLF;   /* Transmission end like this */
    HTRequest *			request;	          /* Link to request */

    char *			host;		 /* host to contact for data */
    HTFormat			fileformat;   /* File format of current file */
    char			passive; 	 /* Have we opened passively */
    BOOL 			directory;	         /* Yes if directory */
    char *			datatype;  /* See rfc959 p.48, but NO SPACE! */
    ftp_ctrl_info *		ctrl;		   /* Controlling connection */
} ftp_data_info;

PRIVATE user_info *old_user;	    /* Only used if HT_REUSE_USER_INFO is on */
PRIVATE HTList *session;	 /* List of control connections in a session */

/* Comments to the Data Structures

    location:	The current location on the FTP-site. This is kept up to date
                using CWD and CDUP, but as there is no garanty for a
		hierarchical file structure in FTP, it should be handled
		carefully.
    passive:	0 means we are active, 1 means we are passive but haven't done
                any accept, 2 means we are passive AND have done the accept. */

/* ------------------------------------------------------------------------- */
/*			   Directory Specific Functions			     */
/* ------------------------------------------------------------------------- */

/*							      HTFTPParseError
**
**	This function parses an (multiple line) error message and takes out
**	the error codes.
**
*/
PRIVATE void HTFTPParseError ARGS1(HTChunk **, error)
{
    HTChunk *oldtext;
    if (!error || !*error || !(*error)->data) {
	if (PROT_TRACE) fprintf(stderr, "FTP......... No error message?\n");
	return;
    }
    oldtext = *error;
    {
	int result;		       /* The first return code in the chunk */
	char *oldp = oldtext->data;
	HTChunk *newtext = HTChunkCreate(128);
	if (oldtext->size > 4 && sscanf(oldp, "%d", &result) == 1) {
	    oldp += 4;
	    while (*oldp) {
		if (*oldp == '\n') {
		    int tmpres;
		    if (sscanf(++oldp, "%d", &tmpres) == 1) {
			if (tmpres == result) {
			    HTChunkPutc(newtext, ' ');
			    oldp += 3;			   /* Skip this code */
			}
		    }
		} else
		    HTChunkPutc(newtext, *oldp);
		oldp++;
	    }
	}
	HTChunkFree(oldtext);
	HTChunkTerminate(newtext);
	*error = newtext;
    }
    return;
}


/*							      HTFTPParseWelcome
**
**	This function parses the welcome message stored in ctrl->welcome.
**	Only multi-line messages are considered interesting, and the starting
**	return code is removed.
**
*/
PRIVATE void HTFTPParseWelcome ARGS1(HTChunk **, welcome)
{
    HTChunk *oldtext;
    if (!welcome || !*welcome || !(*welcome)->data) {
	if (PROT_TRACE) fprintf(stderr, "FTP......... No welcome message?\n");
	return;
    }
    oldtext = *welcome;
    {
	int result;		       /* The first return code in the chunk */
	char cont;					/* Either ' ' or '-' */
	char *oldp = oldtext->data;
	HTChunk *newtext = HTChunkCreate(128);
	if (oldtext->size > 4 && sscanf(oldp, "%d%c", &result, &cont) == 2) {
	    oldp += 4;
	    while (cont == '-') {
		HTChunkPutc(newtext, *oldp);
		if (*oldp == '\n') {
		    int tmpres;
		    if (isdigit(*++oldp) &&
			sscanf(oldp, "%d%c", &tmpres, &cont) == 2) {
			if (tmpres == result && cont == ' ')
			    break;
			else
			    oldp +=3;			   /* Skip this code */
		    }
		}
		oldp++;
	    }
	}
	HTChunkTerminate(newtext);
	HTChunkFree(oldtext);
	*welcome = newtext;
    }
}


/*							      HTFTPAddWelcome
**
**	This function accumulates every welcome messages from the various
**	states in the login procedure.
**
*/
PRIVATE void HTFTPAddWelcome ARGS1(ftp_ctrl_info *, ctrl)
{
    if (!ctrl->welcome)				            /* If first time */
	ctrl->welcome = HTChunkCreate(128);

    HTFTPParseWelcome(&ctrl->reply);
    if (ctrl->reply->size > 1) {
	HTChunkPutc(ctrl->welcome, '\n');
	HTChunkPuts(ctrl->welcome, ctrl->reply->data);
    }
}


#if 0 /* NOT NEEDED FOR THE MOMENT */
/*
 * is_ls_date() --
 *      Return TRUE if s points to a string of the form:
 *              "Sep  1  1990 " or
 *              "Sep 11 11:59 " or
 *              "Dec 12 1989  " or
 *              "FCv 23 1990  " ...
 *
 * Thanks to James.W.Matthews@Dartmouth.EDU (James W. Matthews)
 */
PRIVATE BOOL is_ls_date ARGS1(char *, s)
{
    /* must start with three alpha characget_striters */
    if (!isalpha(*s++) || !isalpha(*s++) || !isalpha(*s++))
	return FALSE;
    
    /* space */
    if (*s++ != ' ')
	return FALSE;
    
    /* space or digit */
    if ((*s != ' ') && !isdigit(*s))
	return FALSE;
    s++;
    
    /* digit */
    if (!isdigit(*s++))
	return FALSE;
    
    /* space */
    if (*s++ != ' ')
	return FALSE;
    
    /* space or digit */
    if ((*s != ' ') && !isdigit(*s))
	return FALSE;
    s++;
    
    /* digit */
    if (!isdigit(*s++))
	return FALSE;
    
    /* colon or digit */
    if ((*s != ':') && !isdigit(*s))
	return FALSE;
    s++;
    
    /* digit */
    if (!isdigit(*s++))
	return FALSE;
    
    /* space or digit */
    if ((*s != ' ') && !isdigit(*s))
	return FALSE;
    s++;
    
    /* space */
    if (*s++ != ' ')
	return FALSE;
    
    return TRUE;
} /* is_ls_date() */
#endif

/*								HTStrpMonth()
**
**	Returns the number of the month given or -1 on error.
**
**	BUG: Handles US dates only!!!
*/
PRIVATE int HTStrpMonth ARGS1(char *, month)
{
    int ret;
    if (!strncmp(month, "JAN", 3))
	ret = 0;
    else if (!strncmp(month, "FEB", 3))
	ret = 1;
    else if (!strncmp(month, "MAR", 3))
	ret = 2;
    else if (!strncmp(month, "APR", 3))
	ret = 3;
    else if (!strncmp(month, "MAY", 3))
	ret = 4;
    else if (!strncmp(month, "JUN", 3))
	ret = 5;
    else if (!strncmp(month, "JUL", 3))
	ret = 6;
    else if (!strncmp(month, "AUG", 3))
	ret = 7;
    else if (!strncmp(month, "SEP", 3))
	ret = 8;
    else if (!strncmp(month, "OCT", 3))
	ret = 9;
    else if (!strncmp(month, "NOV", 3))
	ret = 10;
    else if (!strncmp(month, "DEC", 3))
	ret = 11;
    else {
	ret = -1;
	if (PROT_TRACE)
	    fprintf(stderr, "HTStrpMonth. Couldn't resolve date.\n");
    }
    return ret;
}


/*								HTStrpTime()
**
**	Converts a date string from 'ls -l' to a time_t number
**	This is needed in order to put out the date using the same format
**	for all directory listings.
**
**	Returns 0 on error.
*/
PRIVATE time_t HTStrpTime ARGS1(char *, datestr)
{
    struct tm *time_info;		    /* Points to static tm structure */
    char *bcol = datestr;			             /* Column begin */
    char *ecol;						       /* Column end */
    long tval;
    int cnt;
    time_t curtime = time(NULL);
    if ((time_info = gmtime(&curtime)) == NULL) {
	if (PROT_TRACE)
	    fprintf(stderr, "HTStrpTime.. Can't get current time.\n");
	return (time_t) 0;
    }
    time_info->tm_isdst = -1;			      /* Disable summer time */
    for (cnt=0; cnt<3; cnt++)
    {					    /* Month */
	*bcol = toupper(*bcol);
        bcol++;
    }
    if ((time_info->tm_mon = HTStrpMonth(datestr)) < 0)
	return (time_t) 0;
    ecol = bcol;		   			       	      /* Day */
    while (*ecol++ == ' ');		       /* Spool to other side of day */
    while (*ecol++ != ' ');
    *--ecol = '\0';
    time_info->tm_mday = atoi(bcol);
    time_info->tm_wday = 0;
    time_info->tm_yday = 0;
    bcol = ++ecol;				             	     /* Year */
    if ((ecol = strchr(bcol, ':')) == NULL) {
	time_info->tm_year = atoi(bcol)-1900;
	time_info->tm_sec = 0;
	time_info->tm_min = 0;
	time_info->tm_hour = 0;
    } else {							     /* Time */
	/* If the time is given as hh:mm, then the file is less than 1 year
	   old, but we might shift calandar year. This is avoided by checking 
	   if the date parsed is future or not. */
	*ecol = '\0';
	time_info->tm_sec = 0;
	time_info->tm_min = atoi(++ecol);	       	/* Right side of ':' */
	time_info->tm_hour = atoi(bcol);		 /* Left side of ':' */
	if (mktime(time_info) > curtime)
	    --time_info->tm_year;
    }
    return ((tval = mktime(time_info)) == -1 ? (time_t) 0 : tval); 
}


/*								HTVMSStrpTime()
**
**	Converts a date string from vms to a time_t number
**	This is needed in order to put out the date using the same format
**	for all directory listings.
**
**	Returns 0 on error
*/
PRIVATE time_t HTVMSStrpTime ARGS1(char *, datestr)
{
    struct tm *time_info;		    /* Points to static tm structure */
    char *col;
    long tval;
    time_t curtime = time(NULL);
    if ((time_info = gmtime(&curtime)) == NULL)
	return (time_t) 0;
    time_info->tm_isdst = -1;			      /* Disable summer time */
    if ((col = strtok(datestr, "-")) == NULL)
	return (time_t) 0;
    time_info->tm_mday = atoi(col);				      /* Day */
    time_info->tm_wday = 0;
    time_info->tm_yday = 0;
    if ((col = strtok(NULL, "-")) == NULL ||
	(time_info->tm_mon = HTStrpMonth(col)) < 0)
	return (time_t) 0;
    if ((col = strtok(NULL, " ")) == NULL)			     /* Year */
	return (time_t) 0;
    time_info->tm_year = atoi(col)-1900;
    if ((col = strtok(NULL, ":")) == NULL)			     /* Hour */
	return (time_t) 0;
    time_info->tm_hour = atoi(col);
    if ((col = strtok(NULL, " ")) == NULL)			     /* Mins */
	return (time_t) 0;
    time_info->tm_min = atoi(col);
    time_info->tm_sec = 0;
    return ((tval = mktime(time_info)) < 0 ? (time_t) 0 : tval);
}


/*								HTFTPFilePerm()
**
**	Converts the file type from 'ls -l' into a long. The reason for
**	doing this is to be able to handle the file type and icon selection
**	similar to the Unix way used in HTBrowseDirectory().
**
*/
PRIVATE long HTFTPFilePerm ARGS1(char *, permission)
{
    char *strptr = permission;
    long mode = 0L;
    
    /* Special files etc. are all handled like regular files */
    switch (*strptr++) {					/* File type */
      case 'd':	mode = S_IFMT & S_IFDIR; break;
      case 'l': mode = S_IFMT & S_IFLNK; break;
      default:  mode = S_IFMT & S_IFREG; break;
    }
    if (*strptr++ == 'r') mode |= S_IRUSR;			     /* User */
    if (*strptr++ == 'w') mode |= S_IWUSR;
    if (*strptr == 'x')
	mode |= S_IXUSR;
    else if (*strptr == 's')
	mode |= (S_IXUSR | S_ISUID);
    else if (*strptr == 'S')
	mode |= S_ISUID;
    strptr++;
    if (*strptr++ == 'r') mode |= S_IRGRP;		            /* Group */
    if (*strptr++ == 'w') mode |= S_IWGRP;
    if (*strptr == 'x')
	mode |= S_IXGRP;
    else if (*strptr == 's')
	mode |= (S_IXGRP | S_ISGID);
    else if (*strptr == 'S')
	mode |= S_ISGID;
    strptr++;
    if (*strptr++ == 'r') mode |= S_IROTH;			    /* Other */
    if (*strptr++ == 'w') mode |= S_IWOTH;
    if (*strptr == 'x')
	mode |= S_IXOTH;
    else if (*strptr == 't')
	mode |= (S_IXOTH | S_ISVTX);
    else if (*strptr == 'T')
	mode |= S_ISVTX;
    strptr++;
    return mode;
}


/*							     parse_unix_line()
**
**	Extract the name, size, and date from an 'ls'. The function expects
**	the following format of the ls-line:
**
**	<permission> <nlink> <owner> [<group>] <size> <date> <filename>
**
**	The group is not always present and is therefore optional. Both owner
**	and group can be numbers.
**
**	Returns YES if OK, NO on error
*/
PRIVATE BOOL parse_unix_line ARGS2(char *,line, dir_file_info *,f_info)
{
    char *column;
    char *strptr;
    int ival;
    unsigned long lval;
    if (!line || !*line || !f_info)
	return NO;

    if ((column = strtok(line, " ")) == NULL)      	      /* Permissions */
	return NO;
    f_info->f_mode = HTFTPFilePerm(column);
    if ((column = strtok(NULL, " ")) == NULL)		            /* Links */
	return NO;
    if (sscanf(column, "%d", &ival) == 1)
	f_info->f_nlink = ival;
    if ((column = strtok(NULL, " ")) == NULL)		            /* Owner */
	return NO;
    StrAllocCopy(f_info->f_uid, column);
    if ((column = strtok(NULL, " ")) == NULL)           /* Group and/or Size */
	return NO;
    if (sscanf(column, "%lu", &lval) != 1) {
	StrAllocCopy(f_info->f_gid, column);
	if ((column = strtok(NULL, " ")) == NULL)
	    return NO;
	if (sscanf(column, "%lu", &lval) == 1)
	    f_info->f_size = lval;
    } else {				           /* Group can be a number! */
	strptr = column+strlen(column)+1;
	while (*strptr++ == ' ');
	if (isdigit(*--strptr)) {      	   /* Month can't start with a digit */
	    StrAllocCopy(f_info->f_gid, column);
	    if ((column = strtok(NULL, " ")) == NULL)
		return NO;
	    if (sscanf(column, "%lu", &lval) == 1)
		f_info->f_size = lval;
	} else {
	    StrAllocCopy(f_info->f_gid, "");
	    f_info->f_size = lval;
	}
    }
    column = column+strlen(column)+1;
    while (*column++ == ' ');
    strptr = --column+12;		      	     /* Find the date column */
    *strptr++ = '\0';
    if ((f_info->f_mtime = HTStrpTime(column)) == (time_t) 0)
	return NO;
    while (*strptr++ == ' ');			        /* Spool to filename */
    if ((f_info->f_mode & S_IFMT) == S_IFLNK) {        /* Strip any '->' */
	char *link = strstr(strptr-1, " -> ");
	if (link)
	    *link = '\0';
    }
    StrAllocCopy(f_info->f_name, --strptr);
    return YES;				        /* We have a full structure! */
}


/*						   	      parse_vms_line()
**
**      Format the name, date, and size from a VMS LIST line
**      into the dir_file_info structure
**
**	BUGS: Group, user and permissions are not parsed!
**
**	Returns YES if OK, NO on error
*/
PRIVATE BOOL parse_vms_line ARGS2(char *, line, dir_file_info *, f_info)
{
    int i, j, ialloc;
    char *cp, *cpd, *cps, *cdir, *sp = " ";
    
    /**  Get rid of information lines by making them blank too **/
    /**  Valid lines have the semi-colon version number token  **/
    if (!line || !*line || !f_info || (cp = strchr(line, ';')) == NULL) {
	return NO;
    }

    /** Cut out file or directory name at VMS version number **/
    *cp++ ='\0';
    StrAllocCopy(f_info->f_name,line);
    
    /** Cast VMS file and directory names to lowercase **/
    for (i=0; f_info->f_name[i]; i++)
	f_info->f_name[i] = tolower(f_info->f_name[i]);
    
    /** Uppercase terminal .z's or _z's **/
    if ((--i > 2) && f_info->f_name[i] == 'z' &&
	(f_info->f_name[i-1] == '.' || f_info->f_name[i-1] == '_'))
	f_info->f_name[i] = 'Z';
    
    /* Trim off VMS directory extensions */
    if ((cdir = strstr(f_info->f_name, ".dir")) != NULL) { /* Strip any .dir */
	f_info->f_mode = S_IFMT & S_IFDIR;
	*cdir = '\0';
    } else
	f_info->f_mode = S_IFMT & S_IFREG;

    /** Convert any tabs in rest of line to spaces **/
    cps = cp-1;
    while ((cps=strchr(cps+1, '\t')) != NULL)
	*cps = ' ';
    
    /** Collapse serial spaces **/
    i = 0; j = 1;
    cps = cp;
    while (cps[j] != '\0') {
	if (cps[i] == ' ' && cps[j] == ' ')
	    j++;
	else
	    cps[++i] = cps[j++];
    }
    cps[++i] = '\0';
    
    /** Track down the date **/
    if ((cpd=strchr(cp, '-')) != NULL) {
	if ((int)strlen(cpd) > 9 && isdigit(*(cpd-1)) &&
	    isalpha(*(cpd+1)) && *(cpd+4) == '-') {
	    if ((f_info->f_mtime = HTVMSStrpTime(cpd-2)) == (time_t) 0)
		return NO;
	}
    }
    
    /** Track down the size **/
    if ((cpd = strchr(cp, '/')) != NULL) {
	/* Appears be in used/allocated format */
	cps = cpd;
	while (isdigit(*(cps-1)))
	    cps--;
	if (cps < cpd)
	    *cpd = '\0';
	f_info->f_size = atoi(cps);
	cps = cpd+1;
	while (isdigit(*cps))
	    cps++;
	*cps = '\0';
	ialloc = atoi(cpd+1);
	/* Check if used is in blocks or bytes */
	if (f_info->f_size <= ialloc)
	    f_info->f_size *= 512;
    }
    else if ((cps=strtok(cp, sp)) != NULL) {
	/* We just initialized on the version number */
	/* Now let's hunt for a lone, size number    */
	while ((cps=strtok(NULL, sp)) != NULL) {
	    cpd = cps;
	    while (isdigit(*cpd))
		cpd++;
	    if (*cpd == '\0') {
		/* Assume it's blocks */
		f_info->f_size = atoi(cps) * 512;
		break;
	    }
	}
    }
    return YES;				        /* We have a full structure! */
}


/*						 	     parse_dir_entry() 
**
**      Given a line of LIST/NLST output in entry, return results 
**      and a file/dir name in f_info struct
**
**      If first_entry is true, this it must be the first name in a directory.
**	Returns YES if OK, NO on error
*/
PRIVATE BOOL parse_dir_entry ARGS4(ftp_data_info *, data, char *, entry,
				   BOOL, first_entry, dir_file_info *, f_info)
{
    BOOL status = YES;
    switch (data->ctrl->server) {
      case WINDOWS_NT:		   /* Might not send the UNIX first line :-( */
	if (first_entry) {
	    if (strncmp(entry, "total ", 6) && !strstr(entry, "not available"))
		status = parse_unix_line(entry, f_info);
	    else
		status = NO;
	} else
	    status = parse_unix_line(entry, f_info);
	break;

      case UNIX_SERVER:
      case PETER_LEWIS_SERVER:
      case MACHTEN_SERVER:
	/* Interpret and edit LIST output from Unix server */
	if (first_entry) {
	    if (data->ctrl->unsure_type == YES &&
		strncmp(entry, "total ", 6) &&
		(strstr(entry, "not available") != NULL)) {
		/* this isn't really a unix server! */
		if (PROT_TRACE)
		    fprintf(stderr, "FTP......... No, this isn't a UNIX server anyway :-(\n");
		data->ctrl->server = GENERIC_SERVER;
	    }
	    /* We might as well say that it is not unsure any more, as we
	       can't (or don't) do anything about it, */
	    data->ctrl->unsure_type = NO;
	    status = NO;
	} else
	    status = parse_unix_line(entry, f_info);
	break;
	
      case VMS_SERVER:
	/* Interpret and edit LIST output from VMS server */
	/* and convert information lines to zero length.  */
	status = parse_vms_line(entry, f_info);
	break;
	
      case CMS_SERVER:
	/* Can't be directory... "entry" already equals the correct f_name */
	StrAllocCopy(f_info->f_name, entry);
	f_info->f_mode = S_IFMT & S_IFREG;
	break;
	
      case NCSA_SERVER:
      case TCPC_SERVER:
	/* Directories identified by trailing "/" characters */
	StrAllocCopy(f_info->f_name, entry);
	{
	    int len = strlen(entry);
	    if (*(entry+len-1) == '/') {
		*(entry+len-1) = '\0';
		f_info->f_mode = S_IFMT & S_IFDIR;
	    } else {
		f_info->f_mode = S_IFMT & S_IFREG;
	    }
	}
	break;
	
      default:
	/* We cant tell if it is a directory since we only did an NLST :-( */
	StrAllocCopy(f_info->f_name, entry);
	f_info->f_mode = S_IFMT & S_IFREG;
	break;
    }
    return status;
}


PRIVATE int HTFTP_get_dir_string ARGS2(HTNetInfo *, data,
				       dir_file_info *, f_info)
{
    int status = 1;
    int ch;			      /* Must be int in order to contain EOF */
    BOOL got_line = NO;
    static BOOL first = YES;		    /* Is it the first time through? */
    HTChunk *chunk = HTChunkCreate(128);

    do {			                /* Until we have a nice line */
	while ((ch = HTInputSocket_getCharacter(data->isoc)) >= 0) {
	    if (ch == CR || ch == LF) {			      /* Terminator? */
		if (chunk->size != 0) {			    /* got some text */
		    if (((ftp_data_info *) data)->ctrl->server == VMS_SERVER) {
			/* Deal with MultiNet's wrapping of long lines - F.M.*/
			if (isdigit(*(chunk->data+chunk->size-1)))
			    continue;
		    }
		    HTChunkTerminate(chunk);
		    if (parse_dir_entry((ftp_data_info *) data, chunk->data,
					first, f_info)) {
			first = NO;
			got_line = YES;
			break;
		    } else {
			first = NO;
			HTChunkClear(chunk);
		    }
		}
	    } else
		HTChunkPutc(chunk, ch);
	}
    } while (got_line == NO && ch >= 0);
    if (ch < 0) {
	first = YES;
	status = (ch == EOF) ? 0 : ch;
    }
    HTChunkFree(chunk);
    return status;
}

/* ------------------------------------------------------------------------- */
/*	  FTP Client Functions for managing control and data connections     */
/* ------------------------------------------------------------------------- */

/*								HTFTP_send_cmd
**
**	This function sends a command through the control connection 
**	specified. The Telnet terminating end of line code <CRLF> is
**	appended automaticly. The parameter argument is ignored if NULL.
**
**	Returns 0 on OK, else -1 but does NOT close the connection
*/
PRIVATE int HTFTP_send_cmd ARGS3(ftp_ctrl_info *, ctrl, char *, cmd,
				 char *, pars)
{
    char *command;
    if (!ctrl && ctrl->sockfd < 0) {
	if (PROT_TRACE)
	    fprintf(stderr, "HTFTP_send_cmd: Invalid socket\n");
	return -1;
    }
    if ((command = (char *) malloc(strlen(cmd) + (pars ? strlen(pars)+1 : 0) +
				   2 + 1)) == NULL)
	outofmem(__FILE__, "HTFTP_send_cmd");
    if (pars && *pars)
	sprintf(command, "%s %s%c%c", cmd, pars, CR, LF);
    else
	sprintf(command, "%s%c%c", cmd, CR, LF);
    if (PROT_TRACE) {
	if (!strcasecomp(cmd, "pass"))
	    fprintf(stderr, "FTP Tx...... PASS ********\n");
	else
	    fprintf(stderr, "FTP Tx...... %s", command);
    }
#ifdef NOT_ASCII
    {
	char *sp;
	for (sp = command; *sp; sp++) {
	    *sp = TOASCII(*sp);
	}
    }
#endif
    {
	int status = NETWRITE(ctrl->sockfd, command,
			      (int) strlen(command));
	if (status < 0) {
	    if (PROT_TRACE)
		fprintf(stderr, "FTP......... Error sending command\n");
	    HTErrorSysAdd(ctrl->request, ERR_FATAL, NO, "NETWRITE");
	    free(command);
	    return -1;
	}
	free(command);
	return 0;
    }
}


/*							   HTFTP_get_response
**
**	This function gets the response from the net to the control connection
**	specified. If text is not NULL, the response text is put into this 
**	chunk. In case of OK, the freeing is then left to the callee. The
**	response is read until a <LF> is encountered, since not all servers
**	use a full telnet <CRLF> code.
**
**	Returns the 3 digit return code on OK, else -1 but does NOT close
**	the control connection.
*/
PRIVATE int HTFTP_get_response ARGS2(ftp_ctrl_info *, ctrl_info,
				     HTChunk **, text)
{
    int result;				         /* Three-digit decimal code */
    int offset = 0;		      /* Offset for each newline in response */
    BOOL first_line = YES;
    int ch;
    HTChunk *chunk = HTChunkCreate(128);

    if (!ctrl_info && ctrl_info->sockfd < 0) {
	if (PROT_TRACE)
	    fprintf(stderr, "HTFTP_get_response: Invalid socket\n");
	return -1;
    }

    /* Read response */
    while ((ch = HTInputSocket_getCharacter(ctrl_info->isoc)) >= 0) {
	if (ch == LF) {
	    int tmpres;
	    char cont;
	    if (first_line == YES) {
		if (sscanf(chunk->data, "%d%c", &result, &cont) < 2) {
		    if (PROT_TRACE)
			fprintf(stderr,
				"FTP Rx...... `%s\' - no code found?\n",
				chunk->data);
		    HTChunkFree(chunk);
		    return -1;
		}
		if (cont == '-') {
		    HTChunkPutc(chunk, '\n');
		    offset = chunk->size;		  /* Remember offset */
		    first_line = NO;
		} else {
		    HTChunkTerminate(chunk);
		    break;
		}
	    } else {
		if (isdigit(*(chunk->data+offset)) &&
		    sscanf(chunk->data+offset, "%d%c", &tmpres, &cont) == 2 &&
		    tmpres == result && cont == ' ') {
		    HTChunkTerminate(chunk);
		    break;
		} else {
		    HTChunkPutc(chunk, '\n');
		    offset = chunk->size;		    /* Update offset */
		}
	    }
	} else
	    HTChunkPutc(chunk, ch);
    }
    if (!chunk->size || ch < 0) {		        /* No response read? */
	if (PROT_TRACE) fprintf(stderr, "FTP Rx...... No response?\n");
	HTChunkFree(chunk);
	return -1;
    }
    if (PROT_TRACE) fprintf(stderr, "FTP Rx...... %s\n", chunk->data);
    if (!text)		    /* Response text not wanted so we free the chunk */
	HTChunkFree(chunk);
    else {
	if (*text)				   /* Free old value, if any */
	    HTChunkFree(*text);
	*text = chunk;
    }
    return result;
}



/*							HTFTP_close_data_con
**	Closes the data connection and frees memory
**	Returns 0 if OK, -1 on error
*/
PRIVATE int HTFTP_close_data_con ARGS1(ftp_data_info *, data)
{
    int status = 0;
    if (data) {
	if (data->sockfd >= 0) {
	    if (PROT_TRACE)
		fprintf(stderr, "FTP......... Closing data socket %d\n",
			data->sockfd);
	    if ((status = NETCLOSE(data->sockfd)) < 0)
		HTErrorSysAdd(data->request, ERR_FATAL, NO, "NETCLOSE");
#ifdef REPEAT_LISTEN
	    if (master_socket == data->sockfd)
		master_socket = -1;
#endif
	}
	FREE(data->host);
	FREE(data->datatype);
	free(data);
    } else {
	if (PROT_TRACE) fprintf(stderr, "HTFTP_close_data_con: bad argument!");
	status = -1;
    }
    return status;
}


/*							HTFTP_close_ctrl_con
**	Only if the control connection has no data connection(s) pending
**	then it can be closed and the memory freed.
**	Returns 0 if OK, -1 on error
*/
PRIVATE int HTFTP_close_ctrl_con ARGS1(ftp_ctrl_info *, ctrl)
{
    int status = 0;
    if (ctrl && (!ctrl->data_cons ||
		 (ctrl->data_cons && !HTList_count(ctrl->data_cons)))) {
	if (PROT_TRACE)
	    fprintf(stderr,
		    "FTP......... Closing control socket %d\n", ctrl->sockfd);
	if (ctrl->sockfd >= 0) {
	    if ((status = NETCLOSE(ctrl->sockfd)) < 0)
		HTErrorSysAdd(ctrl->request, ERR_FATAL, NO, "NETCLOSE");
	}
	if (ctrl->isoc)
	    HTInputSocket_free(ctrl->isoc);
	FREE(ctrl->location);
	if (ctrl->user) {
	    FREE(ctrl->user->domain);
	    FREE(ctrl->user->id);
	    FREE(ctrl->user->passwd);
	    free(ctrl->user);
	}
	if (ctrl->welcome)
	    HTChunkFree(ctrl->welcome);
	if (ctrl->reply)
	    HTChunkFree(ctrl->reply);
	HTList_delete(ctrl->data_cons);
	free(ctrl);
    }
    return status;
}


/*							HTFTP_abort_ctrl_con
**	Closes the control connection without looking if any data connections
**	are pending => they are all removed, so be careful!
**	Returns 0 if OK, -1 on error
*/
PRIVATE int HTFTP_abort_ctrl_con ARGS1(ftp_ctrl_info *, ctrl)
{
    int status = 0;
    if (!ctrl) {
	if (PROT_TRACE)
	    fprintf(stderr, "HTFTP_abort_ctrl_con called with bad argument\n");
	return -1;
    }
    if (PROT_TRACE) fprintf(stderr, "FTP......... Aborting ctrl socket %d\n",
			    ctrl->sockfd);

    /* Close any pending data connections */
    if (ctrl->data_cons && HTList_count(ctrl->data_cons)) {
	HTList *cur = ctrl->data_cons;
	ftp_data_info *pres;
	while ((pres = (ftp_data_info *) HTList_nextObject(cur))) {
	    HTFTP_close_data_con(pres);
	}
	HTList_delete(ctrl->data_cons);
	ctrl->data_cons = NULL;
    }

    /* If a session is going on, the control connections are closed later */
    if (!session) {
	if (ctrl->sockfd >= 0) {
	    if ((status = NETCLOSE(ctrl->sockfd)) < 0)
		HTErrorSysAdd(ctrl->request, ERR_FATAL, NO, "NETCLOSE");
	}
	if (ctrl->isoc)
	    HTInputSocket_free(ctrl->isoc);
	FREE(ctrl->location);
	if (ctrl->user) {
	    FREE(ctrl->user->domain);
	    FREE(ctrl->user->id);
	    FREE(ctrl->user->passwd);
	    free(ctrl->user);
	}
	if (ctrl->welcome)
	    HTChunkFree(ctrl->welcome);
	if (ctrl->reply)
	    HTChunkFree(ctrl->reply);
	free(ctrl);
    }
    return status;
}


/*							   HTFTP_parse_login
**
**    	Scan 'login' part of URL for portnumber, uid and passwd. The
**	expected format is [user[:password]@]host[:port]. The 'domain' field
**	in the user structure is always filled out together with the
**	serv_port. The rest is optional.
**
**	Returns YES if anything BUT the domain and serv_port is specified,
**	else NO 
*/
PRIVATE BOOL HTFTP_parse_login ARGS3(char *, url, user_info *, user,
				     u_short *, serv_port)
{
    BOOL status = NO;
    char *login = HTParse(url, "", PARSE_HOST);
    char *host = strrchr(login, '@');
    
    if (host) {				      /* Uid and/or passwd specified */
	char *uid = login;
	char *passwd;
	FREE(user->id);					  /* Skip old values */
	FREE(user->passwd);
	*host++ = '\0';
	if ((passwd = strrchr(uid, ':')) != NULL) {	 /* Passwd specified */
	    *passwd++ = '\0';    
	    if (passwd-1 > uid) {	         /* Passwd AND uid specified */
		HTUnEscape(passwd);
		StrAllocCopy(user->passwd, passwd);
	    }
	}
	HTUnEscape(uid);
	StrAllocCopy(user->id, uid);
	status = YES;
    } else {
	host = login;
    }
    {
	char *portstr;
	if ((portstr = strrchr(host, ':')) != NULL) {	   /* Port specified */
	    char *endp = NULL;
	    *portstr++ = '\0';
	    *serv_port = (u_short) strtol(portstr, &endp, 10);
	    if (endp && *endp)	 /* If portstr is not good, use default port */
		*serv_port = (u_short) IPPORT_FTP;
	}
    }
    StrAllocCopy(user->domain, host);	              /* This is what's left */
    free(login);
    return status;
}


/*							   HTFTP_parse_datatype
**
**    	Scan 'ftptype' part of URL for the data type with parameters and
**	returns the result in accordance with the FTP standard. If nothing is
**	found then datatype = NULL.
**
**	Returns YES if type is found, else NO 
*/
PRIVATE BOOL HTFTP_parse_datatype ARGS2(char *, url, char **, datatype)
{
    BOOL retour = NO;
    char *path = HTParse(url, "", PARSE_PATH);
    char *type = strrchr(path, ';');
    char dtype[6];
    char *tptr = dtype;
    
    if (type && !strncasecomp(++type, "type=", 5)) {   	   /* type specified */
	*tptr++ = toupper(*(type+5));		    /* Look at the type-code */
	if (*dtype == 'L') {		     /* We must look for a byte_size */
	    int cnt;
	    *tptr++ = ' ';
	    for (cnt=0; cnt<3 && *(type+6+cnt); cnt++)       /* Max 3 digits */
		*tptr++ = *(type+6+cnt);
	} else if (*dtype == 'A' || *dtype == 'E') {
	    *tptr++ = ' ';
	    *tptr++ = toupper(*(type+6));     		    /* Get form-code */
	}
	*tptr = '\0';
	StrAllocCopy(*datatype, dtype);
	if (PROT_TRACE)
	    fprintf(stderr, "FTP......... Datatype found: `%s\'\n", *datatype);
	retour = YES;
    }
    free(path);
    return retour;
}


/*							   HTFTP_init_con
**
**	This function returns a control connection structure linked with a
**	data connection structure. The control connection might already be
**	open if HTFTPReuseCtrlCon == YES, but that is indicated in the state
**	variable. ctrl->user->domain is always filled out but id and passwd
**	are optional.
**	If error, NULL is returned.
*/
PRIVATE ftp_ctrl_info *HTFTP_init_con ARGS2(HTRequest *, req, char *, url)
{
    int status;
    BOOL use_url = NO;	   /* YES if uid and passwd are specified in the URL */
    u_short serv_port = IPPORT_FTP;		/* Might be changed from URL */
    
    ftp_ctrl_info *ctrl;
    ftp_data_info *data;
    user_info user;		    /* Contains userid, passwd etc. from URL */

    if (!url || !*url) {
	if (PROT_TRACE)
	    fprintf(stderr, "HTFTP_get_connection: Bad server address!\n");
	return NULL;
    }

    /* Initiate new data connection structure and bin to request */
    if ((data = (ftp_data_info *) calloc(1, sizeof(ftp_data_info))) == NULL)
	outofmem(__FILE__, "HTFTP_get_ctrl_con");
    data->sockfd = -1;			            /* Illigal socket number */
    data->passive = 0;			 /* We do the active open pr default */
    data->request = req;
    req->net_info = (HTNetInfo *) data;
    
    /* Scan URL for uid, pw and portnumber */
    memset((void *) &user, '\0', sizeof(user_info));
    use_url = HTFTP_parse_login(url, &user, &serv_port);

    {
	char *filename = HTParse(url, "", PARSE_PATH+PARSE_PUNCTUATION);
	char *strptr;

	/* Check if file name is a directory */
	if (!*(strptr = filename) || 
	    *(strptr = filename+strlen(filename)-1) == '/') {
	    data->directory = YES;
	} else {
	    /* If data type is not specified in URL let's find it ourselves. */
	    HTUnEscape(filename);
	    data->fileformat = HTFileFormat(filename,
					    &req->content_encoding,
					    &req->content_language);
	    if (HTFTP_parse_datatype(filename, &data->datatype) != YES) {
		if ((req->content_encoding != HTAtom_for("8bit") &&
		     req->content_encoding != HTAtom_for("7bit"))) {
		    if (PROT_TRACE)
			fprintf(stderr, "FTP......... Binary data mode\n");
		    StrAllocCopy(data->datatype, "I");
		}
	    } else {
		/* Chop off data type */
		strptr = strrchr(filename, ';');
		*strptr = '\0';
	    }
	}
	FREE(filename);
    }

    /* Look if control connection already exists else generate new one */
    if (session) {
	BOOL found = NO;
	HTList *cur = session;
	SockA sock_addr;			/* SockA is defined in tcp.h */
	char *host;
	    
	/* if theres an @ then use the stuff after it as a hostname */
	{
	    char *fullhost = HTParse(url, "", PARSE_HOST);
	    char *at_sign;
	    if((at_sign = strchr(fullhost, '@')) != NULL)
		host = at_sign+1;
	    else
		host = fullhost;
	}
	
	/* Set up defaults: */
	memset((void *) &sock_addr, '\0', sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(serv_port);
	
	/* Get node name */
	if (HTParseInet(&sock_addr, host, NO) < 0) {
	    if (PROT_TRACE) fprintf(stderr,
			       "FTP......... Can't locate remote host `%s\'\n", host);
	    FREE(user.domain);
	    FREE(user.id);
	    FREE(user.passwd);
	    free(host);
	    return NULL;
	}
	{
	    /* Check if host, port and user info is the same */
	    u_long new_node = ntohl(sock_addr.sin_addr.s_addr);
	    while ((ctrl = (ftp_ctrl_info *) HTList_nextObject(cur))) {
		if (new_node==ctrl->serv_node && serv_port==ctrl->serv_port) {
		    if ((user.id && strcmp(user.id, ctrl->user->id)) ||
			(user.passwd && strcmp(user.id, ctrl->user->passwd))) {
			found = NO;
		    } else {
			found = YES;
			break;
		    }
		}
	    }
	}
	FREE(host);
	if (found) {
	    if (PROT_TRACE) fprintf(stderr,
			       "FTP......... Already have connection for %d.%d.%d.%d. on port %d, socket %d at location `%s\'\n",
			       (int)*((unsigned char *)(&ctrl->serv_node)+0),
			       (int)*((unsigned char *)(&ctrl->serv_node)+1),
			       (int)*((unsigned char *)(&ctrl->serv_node)+2),
			       (int)*((unsigned char *)(&ctrl->serv_node)+3),
			       ctrl->serv_port,
			       ctrl->sockfd,
			       ctrl->location);
	    data->ctrl = ctrl;			       /* Link them together */
	    HTList_addObject(ctrl->data_cons, (void *) data); /* Add to list */
	    FREE(user.domain);
	    FREE(user.id);
	    FREE(user.passwd);
	    return ctrl;           /* This should return the found structure */
	} else {
	    if (PROT_TRACE)
		fprintf(stderr, "FTP......... No existing connection found, so I build a new\n");
	}
    }
    
    /* Set up data structure for control connection and bind to request */
    if ((ctrl = (ftp_ctrl_info *) calloc(1, sizeof(ftp_ctrl_info))) == NULL ||
	(ctrl->user = (user_info *) calloc(1, sizeof(user_info))) == NULL)
	outofmem(__FILE__, "HTFTP_init_con");
    ctrl->serv_port = serv_port;
    ctrl->sockfd = -1;			            /* Illigal socket number */
    ctrl->request = req;
    StrAllocCopy(ctrl->location, "");			  /* Default is root */
    ctrl->server = UNKNOWN;
    ctrl->unsure_type = YES;
    ctrl->use_list = NO;
    ctrl->state = FTP_IDLE;
    if (session)
	HTList_addObject(session, (void *) ctrl);
    data->ctrl = ctrl;				       /* Link them together */
    ctrl->data_cons = HTList_new();
    HTList_addObject(ctrl->data_cons, (void *) data);	    /* First element */

    /* Initialize user info */
    if (HTFTPUserInfo && !old_user) {   /* If first time */
	if ((old_user = (user_info *) calloc(1, sizeof(user_info))) == NULL)
	    outofmem(__FILE__, "HTFTP_init_con");
	StrAllocCopy(old_user->domain, "");/* Can't strcmp with NULL, can I? */
    }
    if (use_url) {
	StrAllocCopy(ctrl->user->domain, user.domain);
	StrAllocCopy(ctrl->user->id, user.id);
	if (user.passwd) {
	    StrAllocCopy(ctrl->user->passwd, user.passwd);
	}
    } else if (HTFTPUserInfo && !strcmp(old_user->domain, user.domain)) {
	StrAllocCopy(ctrl->user->domain, user.domain);
	if (old_user->id) {
	    StrAllocCopy(ctrl->user->id, old_user->id);
	    if (old_user->passwd)
		StrAllocCopy(ctrl->user->passwd, old_user->passwd);
	}
    } else {
	char *uid = getenv("USER");
	StrAllocCopy(ctrl->user->domain, user.domain);
	StrAllocCopy(ctrl->user->id, "anonymous");
	if (HTGetMailAddress())
	    StrAllocCopy(ctrl->user->passwd, HTGetMailAddress());
	else if (uid) {
	    StrAllocCopy(ctrl->user->passwd, uid);
	    StrAllocCat(ctrl->user->passwd, "@");
	} else {
	    StrAllocCopy(ctrl->user->passwd, WWW_FTP_CLIENT);
	    StrAllocCat(ctrl->user->passwd, "@");
	}
    }
    FREE(user.domain);
    FREE(user.id);
    FREE(user.passwd);

    /* Now get ready for a connect */
    if ((status = HTDoConnect((HTNetInfo *) ctrl, url, serv_port,
			      &ctrl->serv_node, NO)) < 0)
    {
	if (PROT_TRACE)
	    fprintf(stderr, "HTFTP_init_con: Connection not established!\n");
	HTFTP_abort_ctrl_con(ctrl);
	return NULL;
    }
    ctrl->isoc = HTInputSocket_new(ctrl->sockfd);               /* buffering */
    
    if (PROT_TRACE) 
 	fprintf(stderr, "FTP......... Control connected, socket %d\n",
		ctrl->sockfd);
    return ctrl;
}


#ifdef LISTEN
/*	Open a master socket for listening on
**	-------------------------------------
**
**	When data is transferred, we open a port, and wait for the server to
**	connect with the data.
**
** On entry,
**	master_socket	Must be negative if not set up already.
** On exit,
**	Returns		The listening data socket if OK
**			Less than zero if error.
**	master_socket	is socket number if good, else negative.
**	port_number	is valid if good.
*/
PRIVATE BOOL get_listen_socket ARGS1(ftp_data_info *, data)
{
    SockA local_addr;				   /* Binary network address */
    int status = -1;

#ifdef REPEAT_LISTEN
    if (master_socket >= 0) {				     /* Done already */
	data->sockfd = master_socket;
	if (PROT_TRACE)
	    fprintf(stderr, "FTP......... Reusing passive data socket: %d\n",
		    data->sockfd);
	return data->sockfd;
    }
#endif /* REPEAT_LISTEN */

    /* Create internet socket */
    if ((data->sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	return HTErrorSysAdd(data->request, ERR_FATAL, NO, "socket");
    if (PROT_TRACE)
	fprintf(stderr, "HTListen.... Created socket number %d\n",
		data->sockfd);
    
    /* Search for a free port. */
    memset((void *) &local_addr, '\0', sizeof(local_addr));
    local_addr.sin_family = AF_INET;	    /* Family = internet, host order */
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* For multi homed hosts */

    /* Inherit the local address information from the control socket */
    {
	int addr_size = sizeof(local_addr);
	if (getsockname(data->ctrl->sockfd, (struct sockaddr *)
			&local_addr, &addr_size) < 0) {
	    status = HTErrorSysAdd(data->request, ERR_FATAL, NO,"getsockname");
	    goto errorfin;
	}
    }

#ifdef POLL_PORTS
    {
        unsigned short old_port_number = port_number;
	for (port_number=old_port_number+1;;port_number++) {
	    if (port_number > LAST_TCP_PORT)
		port_number = FIRST_TCP_PORT;
	    if (port_number == old_port_number) {
		if (PROT_TRACE)
		    fprintf(stderr, "FTP......... No data port available.\n");
		goto errorfin;
	    }
	    local_addr.sin_port = htons(port_number);

	    /* The socket address is casted to generic sockaddr */
	    if (bind(data->sockfd, (struct sockaddr *) &local_addr,
		     sizeof(local_addr)) == 0)
		break;				     /* We have found a port */
	    status = HTErrorSysAdd(data->request, ERR_FATAL, NO, "bind");
	}
    }
#else
    {
	local_addr.sin_port = 0;	     /* Unspecified: please allocate */

	/* The socket address is casted to a generic address */
	if (bind(data->sockfd, (struct sockaddr *) &local_addr,
		 sizeof(local_addr)) < 0) {
	    status = HTErrorSysAdd(data->request, ERR_FATAL, NO, "bind");
	    goto errorfin;
	}
    }
#endif /* POLL_PORTS */
    /* Now we must find out who we are to tell the other guy. */
    {
	int addr_size = sizeof(local_addr);
	if (getsockname(data->sockfd, (struct sockaddr *) &local_addr,
			&addr_size) < 0) {
	    status = HTErrorSysAdd(data->request, ERR_FATAL, NO,"getsockname");
	    goto errorfin;
	}
	if (PROT_TRACE) fprintf(stderr, "FTP......... This host is `%s\'\n",
			   HTInetString(&local_addr));
    }
    if (PROT_TRACE) fprintf(stderr, "FTP......... Bound to port %d on %s\n",
		       (int) ntohs(local_addr.sin_port),
		       HTInetString(&local_addr));

    /* this_addr is a static global, we can refer to later */
    if (!this_addr && (this_addr = (char *) malloc(24)) == NULL)
	outofmem(__FILE__, "get_listen_socket");
    {
	u_long addr = local_addr.sin_addr.s_addr;
	u_short port = local_addr.sin_port;
	sprintf(this_addr, "%d,%d,%d,%d,%d,%d",
		(int)*((unsigned char *)(&addr)+0),
		(int)*((unsigned char *)(&addr)+1),
		(int)*((unsigned char *)(&addr)+2),
		(int)*((unsigned char *)(&addr)+3),
		(int)*((unsigned char *)(&port)+0),
		(int)*((unsigned char *)(&port)+1));
    }

    /* Inform TCP that we will accept connections. Backlog is 1 as we only
       want (and expect) one connection. If a 3rd host makes a connect
       to this port, we have problems! */
    if (listen(data->sockfd, 1) < 0) {
        status = HTErrorSysAdd(data->request, ERR_FATAL, NO, "listen");
	goto errorfin;
    }
    if (PROT_TRACE) fprintf(stderr,
		       "FTP......... Data socket number %d listening\n",
		       data->sockfd);

#ifdef REPEAT_LISTEN
    master_socket = data->sockfd;		     /* Update master_socket */
#endif /* REPEAT_LISTEN */
    return data->sockfd;			       		     /* Good */

  errorfin:
    NETCLOSE(data->sockfd);
    data->sockfd = -1;
    return -1;
}
#endif /* LISTEN */


/*						   		HTFTP_login
**
**    	This function makes a login to a ftp-server. It takes the user name
**	and passwd specified in ctrl->user and if that fails or an additional
**	account is needed, the user is prompted. As it is difficult, when
**	the server sends it's welcome message, we receive them all and choose
**	the longest.
**
**	Returns -2 on ERROR, -1 on FAILURE, 0 on SUCCESS.
*/
PRIVATE int HTFTP_login ARGS1(ftp_ctrl_info *, ctrl)
{
    enum _state {
	ERROR  = -2,
	FAILURE = -1,
	SUCCESS = 0,
	BEGIN,
	SENT_UID,
	SENT_PASSWD,
	NEED_USER_INFO,
	NEED_PASSWD,
	NEED_ACCOUNT,
	SENT_ACCOUNT
    } state = BEGIN;
    BOOL asked = YES;		    /* Have we already asked for uid/passwd? */
    int status = HTFTP_get_response(ctrl, &ctrl->reply);     /* Get greeting */
    if (status < 0) {
        if (PROT_TRACE) fprintf (stderr, "FTP......... Interrupted at beginning of login.\n");
	return ERROR;
    } else
	HTFTPAddWelcome(ctrl);

    /* This loop only stops if state is ERROR, FAILURE or SUCCESS */
    while (state > 0) {
	switch (state) {
	  case BEGIN:
	    if (!HTFTP_send_cmd(ctrl, "USER", ctrl->user->id))
		state = SENT_UID;
	    else
		state = ERROR;
	    break;

	  case SENT_UID:
	    status = HTFTP_get_response(ctrl, &ctrl->reply);
	    if (status/100 == 2) {		    /* Logged in w/o passwd! */
		HTFTPAddWelcome(ctrl);
		state = SUCCESS;
	    } else if (status/100 == 3) {      		/* Password demanded */
		HTFTPAddWelcome(ctrl);
		state = NEED_PASSWD;
	    } else if (status == 530 && asked == YES)
		state = NEED_USER_INFO;			     /* User unknown */
	    else if (status/100 == 4)
		state = FAILURE;
	    else
		state = ERROR;
	    break;

	  case NEED_PASSWD:
	    if (!ctrl->user->passwd) {			/* Got to ask for it */
		char *prompt = NULL;
		StrAllocCopy(prompt, "Enter password for user: ");
		StrAllocCat(prompt, ctrl->user->id);
		StrAllocCat(prompt, "@");
		StrAllocCat(prompt, ctrl->user->domain);
		StrAllocCat(prompt, ": ");
		if ((ctrl->user->passwd = HTPromptPassword(prompt)) == NULL) {
		    state = ERROR;
		    free(prompt);
		    break;
		}
		free(prompt);
	    }
	    /* If userid = "anonymous" then make sure that there is a '@' at
	       the end of the passwd if it is without domain name */
	    if (!strcasecomp(ctrl->user->id, "anonymous")) {
		char *astr;
		if ((astr = strchr(ctrl->user->passwd, '@')) == NULL)
		    StrAllocCat(ctrl->user->passwd, "@");
	    }
	    if (!HTFTP_send_cmd(ctrl, "PASS", ctrl->user->passwd))
		state = SENT_PASSWD;
	    else
		state = ERROR;
	    break;

	  case SENT_PASSWD:
	    status = HTFTP_get_response(ctrl, &ctrl->reply);
	    if (status/100 == 2) {		    /* Logged in with passwd */
		HTFTPAddWelcome(ctrl);
		state = SUCCESS;
	    } else if (status/100 == 3) {      		 /* Account demanded */
		HTFTPAddWelcome(ctrl);
		state = NEED_ACCOUNT;
	    } else if (status == 530 && asked == YES)
		state = NEED_USER_INFO;			     /* User unknown */
	    else if (status/100 == 4)
		state = FAILURE;
	    else
		state = ERROR;
	    break;
	    
	  case NEED_ACCOUNT:
	    {
		char *prompt = NULL;
		char *account = NULL;
		StrAllocCopy(prompt, "Enter account for user: ");
		StrAllocCat(prompt, ctrl->user->domain);
		StrAllocCat(prompt, "@");
		StrAllocCat(prompt, ctrl->user->id);
		if ((account = HTPrompt(prompt, NULL)) != NULL &&
		    !HTFTP_send_cmd(ctrl, "ACCT", account)) {
		    state = SENT_ACCOUNT;
		} else {
		    state = ERROR;
		}
		free(prompt);
		free(account);
	    }
	    break;

	  case SENT_ACCOUNT:
	    status = HTFTP_get_response(ctrl, &ctrl->reply);
	    if (status/100 == 2) {
		HTFTPAddWelcome(ctrl);
		state = SUCCESS;
	    } else if (status/100 == 4)
		state = FAILURE;
	    else
		state = ERROR;
	    break;
	    
	  case NEED_USER_INFO:
	    {
		char *prompt = NULL;
		StrAllocCopy(prompt, "Enter username and password for: ");
		StrAllocCat(prompt, ctrl->user->domain);
		FREE(ctrl->user->id);
		FREE(ctrl->user->passwd);
		HTPromptUsernameAndPassword(prompt, &ctrl->user->id,
					     &ctrl->user->passwd);
		if (ctrl->user->id && ctrl->user->passwd &&
		    !HTFTP_send_cmd(ctrl, "USER", ctrl->user->id))
		    state = SENT_UID;
		else
		    state = ERROR;
		free(prompt);
	    }
	    asked = NO;
	    break;

	  case FAILURE:			      /* Otherwise gcc complains :-( */
	  case ERROR:
	  case SUCCESS:
	    break;
	} /* end of switch */
    }
    if (state == SUCCESS) {
	if (PROT_TRACE)
	    fprintf(stderr, "FTP......... Logged in at `%s\' as `%s\'\n",
		    ctrl->user->domain, ctrl->user->id);
    }

    /* This is a real pain this reuse user stuff :-( */
    if (HTFTPUserInfo) {
	StrAllocCopy(old_user->domain, ctrl->user->domain);
	StrAllocCopy(old_user->id, ctrl->user->id);
	StrAllocCopy(old_user->passwd, ctrl->user->passwd);
    }
    return state;
}

/*						   		HTFTP_logout
**
**    	This function logs out from a ftp-server.
**
**	Returns -2 on ERROR, -1 on FAILURE, 0 on SUCCESS.
*/
PRIVATE int HTFTP_logout ARGS1(ftp_ctrl_info *, ctrl)
{
    enum _state {
	ERROR  = -2,
	FAILURE = -1,
	SUCCESS = 0,
	BEGIN,
	SENT_QUIT
    } state = BEGIN;
    int status;

    /* This loop only stops if state is ERROR, FAILURE or SUCCESS */
    while (state > 0) {
	switch (state) {
	  case BEGIN:
	    if (!HTFTP_send_cmd(ctrl, "QUIT", NULL))
		state = SENT_QUIT;
	    else
		state = ERROR;
	    break;

	  case SENT_QUIT:
	    status = HTFTP_get_response(ctrl, &ctrl->reply);
	    if (status/100 == 2)
		state = SUCCESS;
	    else if (status/100 == 4)
		state = FAILURE;
	    else
		state = ERROR;
	    break;

	  case FAILURE:			      /* Otherwise gcc complains :-( */
	  case ERROR:
	  case SUCCESS:
	    break;
	}
    }
    return state;
}


/*						   	    HTFTP_get_data_con
 **
 **    	Gets a valid data connection to the server and initializes the
 **	transfer mode.
 **
 **	Returns -2 on ERROR, -1 on FAILURE, 0 on SUCCESS.
 */
PRIVATE int HTFTP_get_data_con ARGS3(HTRequest *, request,
				     ftp_data_info *, data, char *, url)
{
    enum _state {
	ERROR = -2,
	FAILURE = -1,
	SUCCESS = 0,
	BEGIN,
	SENT_TYPE,
	SENT_PASV,
	SENT_PORT,
	NEED_ACTIVE,		 /* We are the active ones in the connection */
	NEED_PASSIVE					   /* We are passive */
    } state = BEGIN;
    int serv_port;
    int status;
    ftp_ctrl_info *ctrl = data->ctrl;
    
    /* This loop only stops if state is ERROR, FAILURE or SUCCESS */
    while (state > 0) {
	switch (state) {
	  case BEGIN:

	    /* First check if it is necessary to send TYPE, else send PASV */
	    if (data->datatype) {
		if (!HTFTP_send_cmd(ctrl, "TYPE", data->datatype))
		    state = SENT_TYPE;
		else
		    state = ERROR;
	    } else {
		if (!HTFTP_send_cmd(ctrl, "PASV", NULL))
		    state = SENT_PASV;
		else
		    state = ERROR;
	    }
	    break;
	    
	  case SENT_PASV:
	    {
		status = HTFTP_get_response(ctrl, &ctrl->reply);
		if (status == 227) {		    
		    /* If succes, we have to scan for the returned port number.
		       However, the format for the response is not standard, so
		       the best thing to do is to scan for the first digit
		       after the status code, see RFC1123 */
		    char *portstr;
		    int h0, h1, h2, h3, p0, p1;
		    portstr = ctrl->reply->data+3;
		    while (*portstr && !isdigit(*portstr++));
		    if (!*portstr || sscanf(--portstr, "%d,%d,%d,%d,%d,%d",
					    &h0, &h1, &h2, &h3, &p0, &p1)<6) {
			if (PROT_TRACE) fprintf(stderr,
			    "FTP......... PASV reply has no IP-address or port number\n");
			state = ERROR;
		    } else {
			if ((data->host = (char *) malloc(30)) == NULL)
			    outofmem(__FILE__, "HTFTP_get_data_con");

			/* Dummy URL containing the host returned by PASV */
			sprintf(data->host, "ftp://%d.%d.%d.%d/",
				h0, h1, h2, h3);
			serv_port = (p0<<8)+p1;
			state = NEED_ACTIVE;
		    }
		} else if (status/100 == 4)
		    state = FAILURE;
		else if (status == 530)			 /* Not logged in??? */
		    state = ERROR;
		else
		    state = NEED_PASSIVE;      /* If error, try PORT instead */
	    }
	    break;

	  case NEED_ACTIVE:
	    /* Now get ready for a connect */
	    if (PROT_TRACE) fprintf(stderr,
			       "FTP......... Server is listening on port %d\n",
			       serv_port);

#ifdef OLD_CODE
	    /* We can't generally expect that the data connection is going to
	       be on the same host as the control connection. Use therefore
	       not the URL but the host port returned by the PASV call */

	    /* Got to strip any host port indication as this is for the control
	       connection. Thanks to ramey@jello.csc.ti.com (Joe Ramey) */
	    {
		char *host, *port;
		char *fullhost = HTParse(url, "",
				 PARSE_ACCESS+PARSE_HOST+PARSE_PUNCTUATION);
		/* Must spool by a UserID and PW if any (J.A.Stubbs) */
		if ((host = strchr(fullhost, '@')) == NULL)
		    host = fullhost;
		else
		    host++;
		port = strrchr(host, ':');
		if (port && strncmp(port, "://", 3))
		    *port = '\0';
		status = HTDoConnect((HTNetInfo *) data, fullhost, serv_port,
				     (u_long *) NULL, YES);
		free(fullhost);
	    }
#endif /* OLD_CODE */
	    status = HTDoConnect((HTNetInfo *) data, data->host, serv_port,
				 (u_long *) NULL, YES);
	    if (status < 0) {
		if (PROT_TRACE) fprintf(stderr,
				   "FTP......... Data connection failed using PASV, let's try PORT instead\n");
		HTErrorFree(request);	     /* Don't generate error message */
		state = NEED_PASSIVE;
	    } else if (status >= 0) {
		if (PROT_TRACE) fprintf(stderr, "FTP......... Data connected using PASV, socket %d\n", data->sockfd);
		state = SUCCESS;
	    } else {
		state = ERROR;     	       		      /* Interrupted */
	    }
	    break;

	  case NEED_PASSIVE:
#ifdef LISTEN
	    /* The server didn't accept our PASV so now we try ourselves to be
	       passive using PORT */
	    if (get_listen_socket(data) < 0 ||
		HTFTP_send_cmd(ctrl, "PORT", this_addr))
		state = ERROR;
	    else
		state = SENT_PORT;
#else
	    /* If PORT is not compiled, then there is nothing we can do! */
	    if (PROT_TRACE) fprintf(stderr, "FTP......... PORT is not possible!\n");
	    state = ERROR;
#endif
	    break;

	  case SENT_PORT:
	    status = HTFTP_get_response(ctrl, &ctrl->reply);
	    if (status/100 == 2) {
		data->passive = 1;
		state = SUCCESS;
	    } else if (status/100 == 4)
		state = FAILURE;
	    else
		state = ERROR;
	    break;

	  case SENT_TYPE:
	    status = HTFTP_get_response(ctrl, &ctrl->reply);
	    /* If OK, then tell the server to be passive */
	    if (status/100 == 2) {
		if (!HTFTP_send_cmd(ctrl, "PASV", NULL))
		    state = SENT_PASV;
		else
		    state = ERROR;
	    } else if (status/100 == 4)
		state = FAILURE;
	    else
		state = ERROR;
	    break;
	    		
	  case ERROR:			      /* Otherwise gcc complains :-( */
	  case FAILURE:
	  case SUCCESS:
	    break;
	}
    }
    return state;
}


#ifdef LISTEN
/*						   	  HTFTP_switch_to_port
**
**    	This function changes the current data connection from being PASV to
**	PORT. This is to handle servers that doesn't tell if they can handle
**	a PASV data connection before very late in the state diagram.
**
**	Returns -2 on ERROR, -1 on FAILURE, 0 on SUCCESS.
*/
PRIVATE int HTFTP_switch_to_port ARGS2(ftp_data_info *, data,
				       HTRequest *, 	req)
{
    enum _state {
	ERROR = -2,
	SUCCESS = 0
    } state = ERROR;
    int status;
    ftp_ctrl_info *ctrl = data->ctrl;

    if (data->passive) {
	if (PROT_TRACE)
	    fprintf(stderr, "FTP Switch.. We are already passive, so PORT won't help :-(\n");
	return state;
    }

    if (PROT_TRACE)
	fprintf(stderr, "FTP Switch.. Closing PASV data connection number %d, and try to reopen it on the fly using PORT\n",
		data->sockfd);
    if ((status = NETCLOSE(data->sockfd)) < 0) {
        HTErrorSysAdd(req, ERR_FATAL, NO, "NETCLOSE");
    } else
	data->sockfd = -1;	   /* Invalid socket */
    
    /* Now get new data connection using PORT */
    if (status >= 0 && get_listen_socket(data) >= 0 &&
	!HTFTP_send_cmd(ctrl, "PORT", this_addr)) {
	status = HTFTP_get_response(ctrl,&ctrl->reply);
	if (status/100 == 2) {
	    data->passive = 1;
	    state = SUCCESS;
	}
    }
    return state;
}
#endif /* LISTEN */


/*						   	  HTFTP_look_for_data
**
**    	This function puts up a select on the data connetcion and the control
**	connection in order to find out on which one data is transmitted.
**
**	Returns -1 on ERROR, 0 if data-connection, 1 if control connection.
*/
PRIVATE int HTFTP_look_for_data ARGS2(HTRequest *, 	request,
				      ftp_data_info *, 	data)
{
    int status = -1;
    fd_set read_socks;
    struct timeval max_wait;
    ftp_ctrl_info *ctrl = data->ctrl;
    int maxfdpl = HTMAX(data->sockfd, ctrl->sockfd) + 1;
    if (data->sockfd < 0 || ctrl->sockfd < 0) {
	if (PROT_TRACE)
	    fprintf(stderr, "FTP Select.. Invalid socket\n");
	return -1;
    }

    /* Initialize the set of sockets */
    FD_ZERO(&read_socks);
    FD_SET(data->sockfd, &read_socks);	      /* Turn on bit for data socket */
    FD_SET(ctrl->sockfd, &read_socks);	   /* Turn on bit for control socket */

    /* Set up timer */
    if (HTFTPTimeOut <= 0)
	HTFTPTimeOut = FTP_DEFAULT_TIMEOUT;
    max_wait.tv_sec = HTFTPTimeOut/100;
    max_wait.tv_usec = (HTFTPTimeOut%100)*10000;

    /* This sleep is necessary as data is usually arriving more slow on control
       connection than on data connection. Even on an error, the data socket 
       might indicate that data is ready, even though they are not :-( */
    sleep(1);

    /* Now make the select */
#ifdef __hpux
    if ((status = select(maxfdpl, (int *) &read_socks, (int *) NULL,
			 (int *) NULL, &max_wait)) < 0)
#else
    if ((status = select(maxfdpl, &read_socks, (fd_set *) NULL,
			 (fd_set *) NULL, &max_wait)) < 0)
#endif /* __hpux */
        HTErrorSysAdd(request, ERR_FATAL, NO, "select");
    else if (!status) {
	HTErrorAdd(request, ERR_FATAL, NO,
		   HTERR_FTP_NO_RESPONSE, NULL, 0, "HTFTP_look_for_data");
	status = -1;
    } else if (status > 1) {
	if (PROT_TRACE) fprintf(stderr, "FTP Select.. Both data connetion and control data connection has data, let's grab the control\n");
	status = 1;
    } else if (FD_ISSET(data->sockfd, &read_socks)) {
	if (PROT_TRACE)
	    fprintf(stderr, "FTP Select.. Data connection %d ready\n",
		    data->sockfd);
	status = 0;
    } else if (FD_ISSET(ctrl->sockfd, &read_socks)) {
	if (PROT_TRACE)
	    fprintf(stderr, "FTP Select.. Control connection %d ready\n",
		    ctrl->sockfd);
	status = 1;
    } else {
	if (PROT_TRACE)
	    fprintf(stderr, "FTP Select.. Unknown socket returned\n");
	status = -1;
    }
    return status;
}


/*							     HTFTPServerInfo()
**
**	This function finds out what server we are talking to.
**	Returns -2 on ERROR, -1 on FAILURE, 0 on SUCCESS.
**
**	Thanks to James.W.Matthews@Dartmouth.EDU (James W. Matthews) for making
**	his code available.
*/
PRIVATE int HTFTPServerInfo ARGS1(ftp_ctrl_info *, ctrl)
{
    enum _state {
	ERROR  = -2,
	FAILURE = -1,
	SUCCESS = 0,
	BEGIN,
	SENT_SYST,
	NEED_PWD,
	SENT_PWD
    } state = BEGIN;
    int status;

    /* This loop only stops if state is ERROR, FAILURE or SUCCESS */
    while (state > 0) {
	switch (state) {
	  case BEGIN:
	    if (!HTFTP_send_cmd(ctrl, "SYST", NULL))
		state = SENT_SYST;
	    else
		state = ERROR;
	    break;
	    
	  case SENT_SYST:
	    {
		char *info;
		status = HTFTP_get_response(ctrl, &ctrl->reply);
		if (status/100 != 2) {
		    state = NEED_PWD;
		    break;
		}

		/* Got a line - what kind of server are we talking to? */
		info = ctrl->reply->data+3;	       /* Skip response code */
		while (*info && *info++ == ' ');
		if (!*info) {
		    if (PROT_TRACE)
			fprintf(stderr, "FTP......... No server info?\n");
		    state = NEED_PWD;
		    break;
		}
		--info;
		if (strncmp(info, "UNIX Type: L8MAC-OSMachTen", 28) == 0) {
		    ctrl->server = MACHTEN_SERVER;
		    ctrl->use_list = YES;
		    ctrl->unsure_type = NO;
		} else if (strstr(info, "UNIX") != NULL) {
		    ctrl->server = UNIX_SERVER;
		    ctrl->use_list = YES;
		    ctrl->unsure_type = NO;
		} else if (strncmp(info, "VMS", 3) == 0) {
		    ctrl->server = VMS_SERVER;
		    ctrl->use_list = YES;
		    ctrl->unsure_type = NO;
		} else if ((strncmp(info, "VM/CMS", 6) == 0) ||
			   (strncmp(info, "VM", 2) == 0)) {
		    ctrl->server = CMS_SERVER;
		    ctrl->unsure_type = NO;
		} else if (strncmp(info, "DCTS", 4) == 0) {
		    ctrl->server = DCTS_SERVER;
		    ctrl->unsure_type = NO;
		} else if (strstr(info, "MAC-OS TCP/ConnectII") != NULL) {
		    ctrl->server = TCPC_SERVER;
		    /* Check old versions of TCP/C using / in pathnames */
		    ctrl->unsure_type = YES;
		} else if (strncmp(info, "MACOS Peter's Server", 20) == 0) {
		    ctrl->server = PETER_LEWIS_SERVER;
		    ctrl->use_list = YES;
		    ctrl->unsure_type = NO;
		} else if (strncmp(info, "Windows_NT", 10) == 0) {
		    ctrl->server = WINDOWS_NT;
		    ctrl->use_list = YES;
		    ctrl->unsure_type = NO;
		}
		
		/* If we are unsure, try PWD to get more information */
		if (ctrl->server == UNKNOWN || ctrl->unsure_type == YES)
		    state = NEED_PWD;
		else
		    state = SUCCESS;
	    }
	    break;

	  case NEED_PWD:
	    /* get the working directory (to see what it looks like) */
	    if (!HTFTP_send_cmd(ctrl, "PWD", NULL))
		state = SENT_PWD;
	    else
		state = ERROR;
	    break;

	  case SENT_PWD:
	    {
		status = HTFTP_get_response(ctrl, &ctrl->reply);
		if (status/100 != 2)
		    state = ERROR;
		else {
		    char *start, *end;

		    /* Now analyze response information between "'s */
		    if ((start = strchr(ctrl->reply->data, '"')) == NULL ||
			(end = strchr(++start, '"')) == NULL) {
			if (PROT_TRACE)
			    fprintf(stderr,
				    "FTP......... No current directory?\n");
			ctrl->server = GENERIC_SERVER;
		    } else {
			*end = '\0';
			if (ctrl->server == TCPC_SERVER) {
			    ctrl->server = *start == '/' ?
				NCSA_SERVER : TCPC_SERVER;
			    ctrl->unsure_type = NO;
			} else if (*start == '/') {
			    /* path names starting with / imply Unix, right? */
			    ctrl->server = UNIX_SERVER;
			    ctrl->use_list = YES;
			} else if (*(end-1) == ']') {
			    /* path names ending with ] imply VMS, right? */
			    ctrl->server = VMS_SERVER;
			} else
			    ctrl->server = GENERIC_SERVER;
		    }
		    state = SUCCESS;
		}
	    }		    
	    break;
	    
	  case FAILURE:			      /* Otherwise gcc complains :-( */
	  case ERROR:
	  case SUCCESS:
	    break;
	}
    }
    if (PROT_TRACE) {
	static char *servers[] = {
	    "UNKNOWN",
	    "GENERIC",
	    "MACHTEN",
	    "UNIX",
	    "VMS",
	    "CMS",
	    "DCTS",
	    "TCPC",
	    "PETER LEWIS",
	    "NCSA",
	    "WINDOWS NT"
	    };
	if (ctrl->unsure_type == YES)
	    fprintf(stderr, "FTP......... This might be a %s server\n",
		    *(servers+ctrl->server+1));
	else
	    fprintf(stderr, "FTP......... We are talking to a %s server\n",
		    *(servers+ctrl->server+1));
    }
    return state;
}


/*							     HTFTPLocation()
**
**	This function compares the current directory location in the
**	ftp-session to the new URL and returns the relative position. If
**	the new URL is at a higher location, the function performs CDUP's
**	until this location is reached so that the relative position is '.'
**
**	Returns relative path name if OK, else current location
**
*/
PRIVATE char *HTFTPLocation ARGS2(ftp_ctrl_info *, ctrl, char *, url)
{
    unsigned char getup = 0;
    char *current;
    char *new;
    char *relative;
    char *result = NULL;
    char *strptr;

    /* Make a full URL out of current location */ 
    current = HTParse(url, "", PARSE_ACCESS+PARSE_HOST+PARSE_PUNCTUATION);
    StrAllocCat(current, "/");
    StrAllocCat(current, ctrl->location);
    if (*(current+strlen(current)-1) != '/')
	StrAllocCat(current, "/");

    /* Make a temporary URL without any type indication */
    new = HTParse(url, "", PARSE_ALL);
    if ((strptr = strrchr(new, ';')) != NULL)
	*strptr = '\0';

    /* Compare those two URLs */
    relative = HTRelative(new, current);
    {
	/* Now send any CDUP if necessary */
	char *tail = relative;
	int status;
	while (tail && (strptr = strstr(tail, "../")) != NULL) {
	    if (HTFTP_send_cmd(ctrl, "CDUP", NULL)) {
		break;
	    }
	    status = HTFTP_get_response(ctrl, &ctrl->reply);
	    if (status/100 != 2) {
		break;
	    }
	    ++getup;
	    tail += 3;
	}
    }

    /* Now update current location if we have used CDUP and make relative 
       return value. */
    if (getup) {
	char *location = HTParse(new, "", PARSE_PATH);
	free(ctrl->location);
	if (*location == '/')
	    ctrl->location = ++location;
	else
	    ctrl->location = location;
	if (*ctrl->location &&
	    *(ctrl->location+strlen(ctrl->location)-1) == '/')
	    *(ctrl->location+strlen(ctrl->location)-1) = '\0';
	StrAllocCopy(result, "");
	free(relative);
    } else {
	if (*relative == '/') {
	    StrAllocCopy(result, relative+1);
	    free(relative);
	} else
	    result = relative;
	if (*relative && *(relative+strlen(relative)-1) == '/')
	    *(relative+strlen(relative)-1) = '\0';
    }
    if (PROT_TRACE)
	fprintf(stderr, "FTP......... current location on server: `%s\'\n",
		ctrl->location);
    free(current);
    free(new);
    return result;
}


/* ------------------------------------------------------------------------- */
/*	              FTP Client Functions for receiving data     	     */
/* ------------------------------------------------------------------------- */

/*						   		HTFTP_get_dir
**
**    	This function asks for the directory specified and calls
**	HTFTPBrowseDirectory. First we try in one go, but if that doesn't
**	work, then we use CWD for each segment. The directory is searched
**	relative to the login directory, that is without a starting '/'.
**
**	Returns -2 on ERROR, -1 on FAILURE, 0 on SUCCESS.
*/
PRIVATE int HTFTP_get_dir ARGS3(ftp_ctrl_info *, ctrl, HTRequest *, req,
				char *, relative)
{
    enum _state {
	ERROR  = -2,
	FAILURE = -1,
	SUCCESS = 0,
	BEGIN,
	NEED_LIST,
	SENT_LIST,
	SENT_CWD,
        MULTIPLE_CWD,
	READY_FOR_DATA,
	SENT_ABOR,
	WAIT_FOR_CONNECTION,
	HANDLE_CTRL,
	GOT_DATA
    } state = BEGIN;
    BOOL handle_ctrl = NO;
    ftp_data_info *data = (ftp_data_info *) ctrl->data_cons->next->object; 
    int status;
    char *unescaped = NULL;
    StrAllocCopy(unescaped, relative);
    HTUnEscape(unescaped);
    HTCleanTelnetString(unescaped);	/* Prevent security holes */

    /* This loop only stops if state is ERROR, FAILURE or SUCCESS */
    while (state > 0) {
	switch (state) {
	  case BEGIN:
	    /* Only if the root directory is requested, we can use LIST right
	       away, else we must first use CWD */
	    if (!*unescaped || !strcmp(unescaped, "/"))
		state = NEED_LIST;
	    else {
		/* We first try to CWD to the location in one go. */
		if (!HTFTP_send_cmd(ctrl, "CWD", unescaped))
		    state = SENT_CWD;
		else
		    state = ERROR;
	    }
	    break;

	  case NEED_LIST:
	    if (ctrl->use_list == YES) {
		if (!HTFTP_send_cmd(ctrl, "LIST", NULL))
		    state = SENT_LIST;
		else
		    state = ERROR;
	    } else {
		if (!HTFTP_send_cmd(ctrl, "NLST", NULL))
		    state = SENT_LIST;
		else
		    state = ERROR;
	    }
	    break;

	  case SENT_LIST:
#ifdef SEQUENT
	    
	    /* If we are listening, do a non-blocking accept now, as the
	       accept on some systems (DYNIX) completes the connection. On
	       BSD systems, the completion is done independently of the
	       accept. (thanks to Bill Rushka, wcr@aps.org) */
	    if (data->passive == 1) {
		int newfd;
		if ((newfd = HTDoAccept(req, data->sockfd)) >= 0) {
#ifdef REPEAT_LISTEN
		    if (PROT_TRACE) fprintf(stderr, "FTP......... Passive data channel number %d stays open.\n", data->sockfd);
#else
		    if (PROT_TRACE) fprintf(stderr, "FTP......... Passive data channel number %d closed.\n", data->sockfd);
		    if (NETCLOSE(data->sockfd) < 0) {
			HTErrorSysAdd(data->request, ERR_FATAL, NO,"NETCLOSE");
			state = ERROR;
			break;
		    }
#endif
		    data->sockfd = newfd;        /* Switch to new socket */
		    data->passive = 2;
		    if (PROT_TRACE)
			fprintf(stderr, "FTP......... New data socket: %d\n",
				data->sockfd);
		} else {
		    HTChunkClear(ctrl->reply);
		    ctrl->reply = NULL;
		    state = ERROR;
		    break;
		}
	    }
#endif /* SEQUENT */

	    status = HTFTP_get_response(ctrl, &ctrl->reply);
	    if (status == 150)			 /* About to open connection */
		state = WAIT_FOR_CONNECTION;
	    else if (status == 125)			/* Transfer starting */
		state = READY_FOR_DATA;
	    else if (status/100 == 4)
		state = FAILURE;
	    else
		state = ERROR;
	    break;
	    
	  case SENT_CWD:
	    status = HTFTP_get_response(ctrl, &ctrl->reply);
	    if (status/100 == 2) {
		/* Update current location */
		if (*ctrl->location)
		    StrAllocCat(ctrl->location, "/");
		StrAllocCat(ctrl->location, relative);
		HTChunkClear(ctrl->welcome);
		HTFTPAddWelcome(ctrl);
		state = NEED_LIST;
	    } else if (status/100 == 4)
		state = FAILURE;
	    else
		state = MULTIPLE_CWD;
	    break;

	  case MULTIPLE_CWD:
	    /* We must use the escaped version when looking for '/' as
	       delimiter between segments, and then unescape each segment */
	    if (PROT_TRACE) fprintf(stderr, "FTP......... Can't jump directly to location, try multiple CD's instead\n");
	    state = NEED_LIST;		     /* This is overwritten if error */
	    {
		char *path = NULL;
		char *segment;
		StrAllocCopy(path, relative);
		segment = strtok(path, "/");
		while (segment && *segment) {
		    HTUnEscape(segment);
		    HTCleanTelnetString(segment);  /* Prevent security holes */
		    if (HTFTP_send_cmd(ctrl, "CWD", segment)) {
			state = ERROR;
			break;
		    }
		    status = HTFTP_get_response(ctrl, &ctrl->reply);
		    if (status/100 != 2) {
			if (status/100 == 4)
			    state = FAILURE;
			else
			    state = ERROR;
			break;
		    } else {			  /* Update current location */
			char *new_seg = HTEscape(segment, URL_XPALPHAS);
			if (*ctrl->location)
			    StrAllocCat(ctrl->location, "/");
			StrAllocCat(ctrl->location, new_seg);
			free(new_seg);
			HTChunkClear(ctrl->welcome);
			HTFTPAddWelcome(ctrl);
		    }
		    segment = strtok(NULL, "/");	   /* Get next token */
		}
		free(path);
	    }
	    break;

	  case READY_FOR_DATA:
	    if (data->passive == 1) {
		int newfd;
		if ((newfd = HTDoAccept((HTNetInfo *) data)) >= 0) {
#ifdef REPEAT_LISTEN
		    if (PROT_TRACE) fprintf(stderr, "FTP......... Passive data channel number %d stays open.\n", data->sockfd);
#else
		    if (PROT_TRACE) fprintf(stderr, "FTP......... Passive data channel number %d closed.\n", data->sockfd);
		    if (NETCLOSE(data->sockfd) < 0) {
			HTErrorSysAdd(data->request, ERR_FATAL, NO,"NETCLOSE");
			state = ERROR;
			break;
		    }
#endif
		    data->sockfd = newfd;            /* Switch to new socket */
		    data->passive = 2;
		    if (PROT_TRACE)
			fprintf(stderr, "FTP......... New data socket: %d\n",
				data->sockfd);
		} else {
		    HTChunkClear(ctrl->reply);
		    ctrl->reply = NULL;
		    state = ERROR;
		    break;
		}
	    }

	    /* Now, the browsing module can be called */
	    {
		char *url = HTAnchor_physical(req->anchor);
		char *path = HTParse(url, "", PARSE_PATH+PARSE_PUNCTUATION);
		HTUnEscape(path);
		if (PROT_TRACE)
		    fprintf(stderr, "FTP......... Receiving directory `%s\'\n",
			    path);
		data->isoc = HTInputSocket_new(data->sockfd);
		status = HTFTPBrowseDirectory(req, path, HTFTP_get_dir_string);
		HTInputSocket_free(data->isoc);
		if (status == -1)
		    state = ERROR;
		else if (status == HT_INTERRUPTED) {
		    if (!HTFTP_send_cmd(ctrl, "ABOR", NULL))
			state = SENT_ABOR;
		    else
			state = ERROR;
		} else
		    state = GOT_DATA;
		free(path);
	    }
	    break;

	  case GOT_DATA:
	    if (!handle_ctrl) {
		status = HTFTP_get_response(ctrl, &ctrl->reply);
		if (status/100 == 2)
		    state = SUCCESS;       	       	   /* Directory read */
		else if (status/100 == 4)
		    state = FAILURE;
		else
		    state = ERROR;
	    } else
		state = SUCCESS;
	    break;

	  case SENT_ABOR:
	    status = HTFTP_get_response(ctrl, &ctrl->reply);
	    if (status/100 == 2)
		state = SUCCESS;
	    else if (status/100 == 4)
		state = FAILURE;
	    else
		state = ERROR;
	    break;

	  case WAIT_FOR_CONNECTION:
	    /* Now we have to wait to see whether the FTP-server sends on the
	       data port or the control port */
	    status = HTFTP_look_for_data(req, data);
	    if (!status)
		state = READY_FOR_DATA;
	    else if (status == 1)
		state = HANDLE_CTRL;
	    else
		state = ERROR;
	    break;

	  case HANDLE_CTRL:
	    status = HTFTP_get_response(ctrl, &ctrl->reply);
	    if (status/100 == 2) {
		state = READY_FOR_DATA;
#ifdef LISTEN
	    } else if (status == 425) {	   /* Connection could not be opened */
		if (HTFTP_switch_to_port(data, req))
		    state = ERROR;
		else
		    state = NEED_LIST;
#endif /* LISTEN */
	    } else if (status/100 == 4)
		state = FAILURE;
	    else
		state = ERROR;
	    handle_ctrl = YES;
	    break;

	  case FAILURE:			      /* Otherwise gcc complains :-( */
	  case ERROR:
	  case SUCCESS:
	    break;
	}
    }
    FREE(unescaped);
    return state;
}


/*						   		HTFTP_get_file
**
**    	This function asks for the file specified. First we try in one go,
**	but if that doesn't work, then we use CWD for each segment and then
**	try to retrieve it. If that also fails, then we try if it is a
**	directory. This procedure causes that directory links generated in
**	HTFTPBrowseDirectory should have a '/' at the end in order to go
**	directly to HTFTP_get_dir. The relative is searched relative to
**	the login directory, that is without a starting '/'.
**
**	Returns -2 on ERROR, -1 on FAILURE, 0 on SUCCESS.
*/
PRIVATE int HTFTP_get_file ARGS3(ftp_ctrl_info *, ctrl, HTRequest *, req,
				 char *, relative)
{
    enum _state {
	ERROR  = -2,
	FAILURE = -1,
	SUCCESS = 0,
	BEGIN,
	SENT_RETR,
        MULTIPLE_CWD,
	READY_FOR_DATA,
	SENT_ABOR,
	WAIT_FOR_CONNECTION,
	HANDLE_CTRL,
	GOT_DATA
    } state = BEGIN;
    BOOL handle_ctrl = NO;
    BOOL multiple = NO;		      /* Have we already tried multiple CWD? */
    ftp_data_info *data = (ftp_data_info *) ctrl->data_cons->next->object; 
    int status;
    char *unescaped = NULL;
    StrAllocCopy(unescaped, relative);
    HTUnEscape(unescaped);
    HTCleanTelnetString(unescaped);  /* Prevent security holes */

    /* This loop only stops if state is ERROR, FAILURE or SUCCESS */
    while (state > 0) {
	switch (state) {
	  case BEGIN:
	    /* First we try to retrieve the file in one go. */
	    if (!HTFTP_send_cmd(ctrl, "RETR", unescaped))
		state = SENT_RETR;
	    else
		state = ERROR;
	    break;

	  case SENT_RETR:
#ifdef SEQUENT	    
	    /* If we are listening, do a non-blocking accept now, as the
	       accept on some systems (DYNIX) completes the connection. On
	       BSD systems, the completion is done independently of the
	       accept. (thanks to Bill Rushka, wcr@aps.org) */
	    if (data->passive == 1) {
		int newfd;
		if ((newfd = HTDoAccept(req, data->sockfd)) >= 0) {
#ifdef REPEAT_LISTEN
		    if (PROT_TRACE) fprintf(stderr, "FTP......... Passive data channel number %d stays open.\n", data->sockfd);
#else
		    if (PROT_TRACE) fprintf(stderr, "FTP......... Passive data channel number %d closed.\n", data->sockfd);
		    if (NETCLOSE(data->sockfd) < 0) {
			HTErrorSysAdd(data->request, ERR_FATAL, NO,"NETCLOSE");
			state = ERROR;
			break;
		    }
#endif
		    data->sockfd = newfd;            /* Switch to new socket */
		    data->passive = 2;
		    if (PROT_TRACE)
			fprintf(stderr, "FTP......... New data socket: %d\n",
				data->sockfd);
		} else {
		    HTChunkClear(ctrl->reply);
		    ctrl->reply = NULL;
		    state = ERROR;
		    break;
		}
	    }
#endif /* SEQUENT */

	    status = HTFTP_get_response(ctrl, &ctrl->reply);
	    if (status == 150)			 /* About to open connection */
		state = WAIT_FOR_CONNECTION;
	    else if (status == 125)
		state = READY_FOR_DATA;			/* Transfer starting */
	    else if (status/100 == 4)
		state = FAILURE;

	    /* If there is no '/' in unescaped, it won't help to try
	       multiple CWD's, as it either doesn't exist or is a directory */
	    else if (multiple == NO && strchr(unescaped, '/') != NULL)
		state = MULTIPLE_CWD;
	    else {
		data->directory = YES;
		state = FAILURE;
	    }
	    break;
	    
	  case MULTIPLE_CWD:
	    /* We must use the escaped version when looking for '/' as
	       delimiter between segments, and then unescape each segment */
	    if (PROT_TRACE) fprintf(stderr, "FTP......... Can't jump directly to location, try multiple CD's instead\n");
	    multiple = YES;
	    {
		char *path = NULL;
		char *segment;
		char *last_slash;	/* Used to identify the last segment */
		StrAllocCopy(path, relative);
		if ((last_slash = strrchr(path, '/')) == NULL)
		    last_slash = path;
		else
		    last_slash++;
		segment = strtok(path, "/");
		while (segment && *segment && segment != last_slash) {
		    HTUnEscape(segment);
		    HTCleanTelnetString(segment);  /* Prevent security holes */
		    if (HTFTP_send_cmd(ctrl, "CWD", segment)) {
			state = ERROR;
			break;
		    }
		    status = HTFTP_get_response(ctrl, &ctrl->reply);
		    if (status/100 != 2) {
			if (status/100 == 4)
			    state = FAILURE;
			else
			    state = ERROR;
			break;
		    } else {			  /* Update current location */
			char *new_seg = HTEscape(segment, URL_XPALPHAS);
			if (*ctrl->location)
			    StrAllocCat(ctrl->location, "/");
			StrAllocCat(ctrl->location, new_seg);
			free(new_seg);
		    }
		    segment = strtok(NULL, "/");	   /* Get next token */
		}
		/* Now try to retrieve the last segment */
		if (segment == last_slash) {
		    HTUnEscape(segment);
		    HTCleanTelnetString(segment);  /* Prevent security holes */
		    if (!HTFTP_send_cmd(ctrl, "RETR", segment)) {
			StrAllocCopy(unescaped, segment);
			state = SENT_RETR;
		    } else
			state = ERROR;
		} else {
		    if (PROT_TRACE) fprintf(stderr, "FTP......... Strange error, filename not found?\n");
		    state = ERROR;
		}
		free(path);
	    }
	    break;

	  case READY_FOR_DATA:
	    if (data->passive == 1) {
		int newfd;
		if ((newfd = HTDoAccept((HTNetInfo *) data)) >= 0) {
#ifdef REPEAT_LISTEN
		    if (PROT_TRACE) fprintf(stderr, "FTP......... Passive data channel number %d stays open.\n", data->sockfd);
#else
		    if (PROT_TRACE) fprintf(stderr, "FTP......... Passive data channel number %d closed.\n", data->sockfd);
		    if (NETCLOSE(data->sockfd) < 0) {
			HTErrorSysAdd(data->request, ERR_FATAL, NO,"NETCLOSE");
			state = ERROR;
			break;
		    }
#endif
		    data->sockfd = newfd;        /* Switch to new socket */
		    data->passive = 2;
		    if (PROT_TRACE)
			fprintf(stderr, "FTP......... New data socket: %d\n",
				data->sockfd);
		} else {
		    HTChunkClear(ctrl->reply);
		    ctrl->reply = NULL;
		    state = ERROR;
		    break;
		}
	    }

	    /* Now, the net parse module can be called */
	    if (PROT_TRACE) fprintf(stderr, "FTP......... Receiving file `%s\'\n",
			       unescaped);
	    status = HTParseSocket(data->fileformat, data->sockfd, req);
	    if (status != HT_LOADED) {
		if (status == HT_INTERRUPTED) {
		    if (!HTFTP_send_cmd(ctrl, "ABOR", NULL))
			state = SENT_ABOR;
		    else
			state = ERROR;
		} else
		    state = ERROR;
	    } else
		state = GOT_DATA;
	    break;

	  case GOT_DATA:
	    if (!handle_ctrl) {
		status = HTFTP_get_response(ctrl, &ctrl->reply);
		if (status/100 == 2)
		    state = SUCCESS;   			   	/* File read */
		else if (status/100 == 4)
		    state = FAILURE;
		else
		    state = ERROR;
	    } else
		state = SUCCESS;
	    break;

	  case SENT_ABOR:
	    status = HTFTP_get_response(ctrl, &ctrl->reply);
	    if (status/100 == 2)
		state = SUCCESS;
	    else if (status/100 == 4)
		state = FAILURE;
	    else
		state = ERROR;
	    break;

          case WAIT_FOR_CONNECTION:
            /* Now we have to wait to see whether the FTP-server sends on the
               data port or the control port */
            status = HTFTP_look_for_data(req, data);
            if (!status)
                state = READY_FOR_DATA;
            else if (status == 1)
                state = HANDLE_CTRL;
	    else
		state = ERROR;
            break;

          case HANDLE_CTRL:
            status = HTFTP_get_response(ctrl, &ctrl->reply);
            if (status/100 == 2) {
                state = READY_FOR_DATA;
#ifdef LISTEN
            } else if (status == 425) {    /* Connection could not be opened */
                if (HTFTP_switch_to_port(data, req))
                    state = ERROR;
                else {
		    if (!HTFTP_send_cmd(ctrl, "RETR", unescaped))
			state = SENT_RETR;
		    else
			state = ERROR;
		}
#endif /* LISTEN */
            } else if (status/100 == 4)
                state = FAILURE;
            else
                state = ERROR;
            handle_ctrl = YES;
            break;

	  case FAILURE:			      /* Otherwise gcc complains :-( */
	  case ERROR:
	  case SUCCESS:
	    break;
	}
    }
    FREE(unescaped);
    return state;
}


/* ------------------------------------------------------------------------- */
/*			       PUBLIC FTP functions			     */
/* ------------------------------------------------------------------------- */

/*						   	   HTFTPWelcomeMsg
**
**    	Returns the welcome message from the login sequence
*/
PUBLIC HTChunk *HTFTPWelcomeMsg ARGS1(HTNetInfo *, data)
{
    return ((ftp_data_info *) data)->ctrl->welcome;
}


/*						   	   HTFTPUseList
**
**    	Can we use long listings in HTDirBrw.c?
*/
PUBLIC BOOL HTFTUseList ARGS1(HTNetInfo *, data)
{
    return ((ftp_data_info *) data)->ctrl->use_list;
}


/*						   	   HTFTP_enable_session
**
**    	This function makes it possible to reuse the same control connections
**	until they are either timed out by the server, or that the session
**	is closed by HTFTP_end_session. Note that HTLoadFTP can run
**	independently of start and end session, and then each load runs like
**	an atomic action.
*/
PUBLIC void HTFTP_enable_session NOARGS
{
    if (session) {
	if (PROT_TRACE)
	    fprintf(stderr, "FTP......... Session is already enabled?\n");
	return;
    }
    session = HTList_new();
}


/*						   	  HTFTP_disable_session
**
**    	This function is the closing function for HTFTP_enable_session.
**
**	Returns YES if OK, else NO
*/
PUBLIC BOOL HTFTP_disable_session NOARGS
{
    BOOL status = YES;
    if (!session) {
	if (PROT_TRACE)
	    fprintf(stderr, "FTP......... No session to disable?\n");
	return NO;
    }
    {
	HTList *cur = session;
	ftp_ctrl_info *pres;
	while ((pres = (ftp_ctrl_info *) HTList_nextObject(cur))) {
	    if (HTFTP_close_ctrl_con(pres))
		status = NO;
	}
	HTList_delete(session);
    }
    session = NULL;
    return status;
}


/*	Retrieve File from Server as an atomic action. 
**	-----------------------------------------------
**
** On entry,
**	request		This is the request structure
** On exit,
**	returns		<0		Error has occured
**			HT_LOADED	OK
*/
PUBLIC int HTLoadFTP ARGS1(HTRequest *, request)
{
    char *url;
    int status = -1;
    int retry;		       			    /* How many times tried? */
    ftp_ctrl_info *ctrl;

    if (!request || !request->anchor) {
	if (PROT_TRACE) fprintf(stderr, "HTLoadFTP... Bad argument\n");
	return -1;
    }
    url = HTAnchor_physical(request->anchor);
    if (PROT_TRACE) fprintf(stderr, "FTP......... Looking for `%s\'\n", url);

    /* Initiate a (possibly already exsisting) control connection and a
       corresponding data connection */
    if((ctrl = HTFTP_init_con(request, url)) == NULL) {
	goto endfunc;
    }

    /* Only if the control connection is in IDLE state, a new
       transfer can be started. The control connection can be in another
       mode if (session), and then the request is getting queued in
       ctrl->data_cons. */
    if (ctrl->state == FTP_IDLE || (session && ctrl->state == FTP_LOGGED_IN)) {
	ftp_data_info *data = ctrl->data_cons->next->object;
	if (ctrl->state == FTP_IDLE)
	    ctrl->state = FTP_BEGIN;
	while (ctrl->state != FTP_IDLE) {	       	/* Do until finished */
	    switch (ctrl->state) {
	      case FTP_BEGIN:
		if (!HTFTP_login(ctrl))
		    ctrl->state = FTP_LOGGED_IN;
		else
		    ctrl->state = FTP_ERROR;
		break;

	      case FTP_LOGGED_IN:
		if (!HTFTP_get_data_con(request, data, url))
		    ctrl->state = FTP_GOT_DATA_CON;
		else
		    ctrl->state = FTP_ERROR;
		break;

	      case FTP_GOT_DATA_CON:
		{
		    /* Now we must ask for the URL requested. If FAILURE, then
		       we try twice to see, if it helps */
		    char *rel = NULL;
		    for (retry=0; retry<2; retry++) {
			if ((rel = HTFTPLocation(ctrl, url)) == NULL) {
			    ctrl->state = FTP_ERROR;
			    break;
			}
			if (retry == 1 && TRACE)
			    fprintf(stderr,
				    "FTP......... First attempt to get URL failed, let's try again\n");

			if (data->directory == YES) {
			    /* If we haven't already got server-info */
			    if (ctrl->server == UNKNOWN) {
				if (HTFTPServerInfo(ctrl)) {
				    ctrl->state = FTP_ERROR;
				    break;
				}
			    }
			    status = HTFTP_get_dir(ctrl, request, rel);
			}
			else
			    status = HTFTP_get_file(ctrl, request, rel);
			if (!status) {
			    ctrl->state = FTP_GOT_DATA;
			    break;
			} else if (status == -2) {		    /* Error */
			    ctrl->state = FTP_ERROR;
			    break;
			} else {
			    FREE(rel);
			    ctrl->state = FTP_FAILURE;		/* Try twice */
			}
		    }
		    FREE(rel);
		}
		if (retry == 2 && ctrl->state == FTP_FAILURE)
		    ctrl->state = FTP_ERROR;
		break;

	      case FTP_GOT_DATA:
		if (HTFTP_close_data_con(data))
		    ctrl->state = FTP_ERROR;
		else {
		    HTList_removeLastObject(ctrl->data_cons);
		    if (!session) {
			if (!HTFTP_logout(ctrl)) {
			    ctrl->state = FTP_IDLE;
			    status = HT_LOADED;
			} else
			    ctrl->state = FTP_ERROR;
		    } else {
			ctrl->state = FTP_LOGGED_IN; /*Ready for next request*/
			return HT_LOADED;
		    }
		    break;
		}
		break;

	      case FTP_ERROR:
		{
		    if (ctrl->reply && ctrl->reply->data) {
			HTFTPParseError(&ctrl->reply);
			HTErrorAdd(request, ERR_FATAL, NO, HTERR_FTP_SERVER,
				   (void *) ctrl->reply->data,
				   ctrl->reply->size-1, "HTLoadFTP");
		    } else {
			char *hoststr = HTParse(url, "", PARSE_HOST);
			HTUnEscape(hoststr);
			HTErrorAdd(request, ERR_FATAL, NO,
				   HTERR_FTP_NO_RESPONSE,
				   (void *) hoststr, strlen(hoststr),
				   "HTLoadFTP");
			free(hoststr);
		    }
		    HTFTP_abort_ctrl_con(ctrl);
		    status = -1;
		    goto endfunc;
		}
		break;

	      default:
		if (PROT_TRACE)
		    fprintf(stderr, "FTP......... Unknown state, what happened?\n");
		break;
	    }
	}

	/* The control connection is only closed if the load is atomic */
	if (!session && HTFTP_close_ctrl_con(ctrl))
	    status = -1;
    }

  endfunc:
    if (status < 0 && status != HT_INTERRUPTED) {
	char *unescaped = NULL;
	StrAllocCopy(unescaped, url);
	HTUnEscape(unescaped);
	HTErrorAdd(request, ERR_FATAL, NO, HTERR_INTERNAL, (void *) unescaped,
		   (int) strlen(unescaped), "HTLoadFTP");
	free(unescaped);
    }
    return status;
}

/* Protocol descriptors */
GLOBALDEF PUBLIC HTProtocol HTFTP  = { "ftp", HTLoadFTP, 0 , 0 };

/* END OF MODULE */


