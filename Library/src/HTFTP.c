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
#include "HTFTP.h"					 /* Implemented here */

/* Macros and other defines */
/* If LISTEN is defined, then first 'PASV' then 'PORT' (if error) is tried,
   else ONLY 'PASV' is used in order to establish a data connection. */
#define LISTEN
#ifdef LISTEN
/* #define REPEAT_LISTEN */    		  /* Reuse the portnumber once found */
/* #define POLL_PORTS */      /* If allocation does not work, poll ourselves.*/
#endif

#ifndef IPPORT_FTP
#define IPPORT_FTP    	 21
#endif

/* Globals */
PUBLIC BOOL HTFTPUserInfo = YES;

/* Type definitions and global variables etc. local to this module */ 
PRIVATE user_info *old_user;	    /* Only used if HT_REUSE_USER_INFO is on */
PRIVATE HTList *session;	 /* List of control connections in a session */

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

/* ------------------------------------------------------------------------- */
/*		  ************** TEMPORARY STUFF *************	      	     */
/* ------------------------------------------------------------------------- */
#define MAX_ACCEPT_POLL		30
#define HT_INTERRUPTED		-29998
#define FCNTL(r, s, t) fcntl(r, s, t)

PRIVATE int HTDoConnect ARGS5(char *, url, char *, protocol,
			      u_short, default_port, int *, s,
			      u_long *, addr)
{
    int status;
    struct sockaddr_in sock_addr;
    char *p1 = HTParse(url, "", PARSE_HOST);
    char *at_sign;
    char *host;

    /* if theres an @ then use the stuff after it as a hostname */
    if((at_sign = strchr(p1,'@')) != NULL)
	host = at_sign+1;
    else
	host = p1;
    if (TRACE) fprintf(stderr, "HTDoConnect. Looking up `%s\'\n", host);

   /* Set up defaults: */
    bzero((char *) &sock_addr, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(default_port);

    /* Get node name */
    if (HTParseInet(&sock_addr, host)) {
	if (TRACE) fprintf(stderr, "HTDoConnect. Can't locate remote host `%s\'\n", host);
	free (p1);
	*s = -1;
	return -1;
    }

    if ((*s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
	free (p1);
	return HTInetStatus("socket");
    }
    if (addr)
	*addr = ntohl(sock_addr.sin_addr.s_addr);

    if (TRACE)
	fprintf(stderr, "HTDoConnect. Created socket number %d\n", *s);

    if ((status = connect(*s, (struct sockaddr *) &sock_addr,
			  sizeof(sock_addr))) < 0) {
	HTInetStatus("connect");
	if (NETCLOSE(*s) < 0)
	    HTInetStatus("close");
	free(p1);
	*s = -1;
	return -1;
    }
    free(p1);
    return status;
}


/*								HTDoAccept()
**
**	This function makes a non-blocking accept on a port and polls every
**	second until MAX_ACCEPT_POLL or interrupted by user.
**
**	BUGS Interrupted is not yet implemented!!!
**
**	Returns 0 if OK, -1 on error
*/
PRIVATE int HTDoAccept ARGS1(int, sockfd)
{
    struct sockaddr_in soc_address;
    int status;
    int cnt;
    int soc_addrlen = sizeof(soc_address);
    if (sockfd < 0) {
	if (TRACE) fprintf(stderr, "HTDoAccept.. Bad socket number\n");
	return -1;
    }
	
    /* First make the socket non-blocking */
    if((status = FCNTL(sockfd, F_GETFL, 0)) != -1) {
	status |= FNDELAY;					/* O_NDELAY; */
	status = FCNTL(sockfd, F_SETFL, status);
    }
    if (status == -1) {
	HTInetStatus("fcntl");
	return -1;
    }

    /* Now poll every sekund */
    for(cnt=0; cnt<MAX_ACCEPT_POLL; cnt++) {
	if ((status = accept(sockfd, (struct sockaddr*) &soc_address,
			     &soc_addrlen)) >= 0) {
	    if (TRACE) fprintf(stderr,
			       "HTDoAccept.. Accepted new socket %d\n",	
			       status);
	    return status;
	} else
	    HTInetStatus("Accept");
	sleep(1);
    }	
    
    /* If nothing has happened */
    if (TRACE)
	fprintf(stderr, "HTDoAccept.. Timed out, no connection!\n");
    return -1;
}

/* ------------------------------------------------------------------------- */
/*			   Directory Specific Functions			     */
/* ------------------------------------------------------------------------- */
/*							      HTParseWelcome
**
**	This function parses the welcome message stored in ctrl->welcome.
**	Only multi-line messages are considered interesting, and the starting
**	return code is removed.
**
*/
PRIVATE void HTParseWelcome ARGS1(ftp_ctrl_info *, ctrl)
{
    HTChunk *oldtext = ctrl->welcome;
    if (!oldtext) {
	if (TRACE) fprintf(stderr, "FTP......... No welcome message?\n");
	return;
    }
    {
	BOOL lastline;
	char *oldp = oldtext->data+3;	    /* Start after first return code */
	HTChunk *newtext = HTChunkCreate(128);
	if (oldtext->size > 4) {
	    lastline = *oldp++ == '-' ? NO : YES;
	    while (!lastline) {
		HTChunkPutc(newtext, *oldp);
		if (*oldp == '\n') {
		    oldp += 4;
		    lastline = *oldp == '-' ? NO : YES;
		}
		++oldp;
	    }
	}
	HTChunkTerminate(newtext);
	HTChunkFree(oldtext);
	ctrl->welcome = newtext;
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
	if (TRACE) fprintf(stderr, "HTStrpMonth. Couldn't resolve date.\n");
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
	if (TRACE)
	    fprintf(stderr, "HTStrpTime.. Can't get current time.\n");
	return (time_t) 0;
    }
    time_info->tm_isdst = -1;			      /* Disable summer time */
    for (cnt=0; cnt<3; cnt++)					    /* Month */
	*bcol++ = toupper(*bcol);
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
**      If first_entry is true, this is the first name in a directory.
**	Returns YES if OK, NO on error
*/
PRIVATE BOOL parse_dir_entry ARGS4(ftp_data_info *, data, char *, entry,
				   BOOL, first_entry, dir_file_info *, f_info)
{
    BOOL status = YES;
    switch (data->ctrl->server) {
      case UNIX_SERVER:
      case PETER_LEWIS_SERVER:
      case MACHTEN_SERVER:
	/* Interpret and edit LIST output from Unix server */
	if (first_entry) {
	    if (data->ctrl->unsure_type == YES &&
		strncmp(entry, "total ", 6) &&
		(strstr(entry, "not available") != NULL)) {
		/* this isn't really a unix server! */
		if (TRACE)
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


PRIVATE int HTFTP_get_dir_string ARGS2(ftp_data_info *, data,
				       dir_file_info *, f_info)
{
    int status = 1;
    int ch;			      /* Must be int in order to contain EOF */
    BOOL got_line = NO;
    static BOOL first = YES;		    /* Is it the first time through? */
    static HTInputSocket *isoc;
    HTChunk *chunk = HTChunkCreate(128);

    if (first == YES)
	isoc = HTInputSocket_new(data->socket);	         /* Set up buffering */
    do {			                /* Until we have a nice line */
	while ((ch = HTInputSocket_getCharacter(isoc)) >= 0) {
	    if (ch == CR || ch == LF) {			      /* Terminator? */
		if (chunk->size != 0) {			    /* got some text */
		    if (data->ctrl->server == VMS_SERVER) {
			/* Deal with MultiNet's wrapping of long lines - F.M.*/
			if (isdigit(*(chunk->data+chunk->size-1)))
			    continue;
		    }
		    HTChunkTerminate(chunk);
		    if (parse_dir_entry(data, chunk->data, first, f_info)) {
			got_line = YES;
			break;
		    } else {
			HTChunkClear(chunk);
		    }
		    first = NO;
		}
	    } else
		HTChunkPutc(chunk, ch);
	}
    } while (got_line == NO && ch >= 0);
    if (ch < 0) {
	first = YES;
	status = (ch == EOF) ? 0 : ch;
    }
    if (first) {
	HTInputSocket_free(isoc);
	isoc = NULL;
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
PRIVATE int HTFTP_send_cmd ARGS3(ftp_ctrl_info *, ctrl_info, char *, cmd,
				 char *, pars)
{
    char *command;
    if (!ctrl_info && ctrl_info->socket < 0) {
	if (TRACE)
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
    if (TRACE) {
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
	int status = NETWRITE(ctrl_info->socket, command,
			      (int) strlen(command));
	if (status < 0) {
	    if (TRACE)
		fprintf(stderr, "FTP......... Error %d sending command: %s\n",
			status, command);
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

    if (!ctrl_info && ctrl_info->socket < 0) {
	if (TRACE)
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
		    HTChunkFree(chunk);
		    if (TRACE)
			fprintf(stderr,
				"FTP Rx...... `%s\' - no code found?\n",
				chunk->data);
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
		if (sscanf(chunk->data+offset, "%d%c", &tmpres, &cont) == 2 &&
		    tmpres == result && cont == ' ') {
		    HTChunkTerminate(chunk);
		    break;
		} else {
		    HTChunkPutc(chunk, '\n');
		    offset = chunk->size;		    /* Update offset */
		}
	    }
	} else
	    HTChunkPutc(chunk, (char) ch);
    }
    if (!chunk->size || ch < 0) {		        /* No response read? */
	HTChunkFree(chunk);
	if (TRACE) fprintf(stderr, "FTP Rx...... No response?\n");
	return -1;
    }
    if (TRACE) fprintf(stderr, "FTP Rx...... %s\n", chunk->data);
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
	if (data->socket >= 0) {
	    if (TRACE)
		fprintf(stderr, "FTP......... Closing data socket %d\n",
			data->socket);
	    if ((status = NETCLOSE(data->socket)) < 0)
		HTInetStatus("close data socket");
#ifdef REPEAT_LISTEN
	    if (master_socket == data->socket)
		master_socket = -1;
#endif
	}
	FREE(data->datatype);
	free(data);
    } else {
	if (TRACE) fprintf(stderr, "HTFTP_close_data_con: bad argument!");
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
	if (TRACE)
	    fprintf(stderr,
		    "FTP......... Closing control socket %d\n", ctrl->socket);
	if (ctrl->socket >= 0) {
	    if ((status = NETCLOSE(ctrl->socket)) < 0)
		HTInetStatus("close control socket");
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
	if (TRACE)
	    fprintf(stderr, "HTFTP_abort_ctrl_con called with bad argument\n");
	return -1;
    }
    if (TRACE) fprintf(stderr, "FTP......... Aborting control socket %d\n",
		       ctrl->socket);

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
	if (ctrl->socket >= 0) {
	    if ((status = NETCLOSE(ctrl->socket)) < 0)
		HTInetStatus("abot control socket");
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
		StrAllocCopy(user->passwd, passwd);
	    }
	}
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
	if (TRACE)
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
PRIVATE ftp_ctrl_info *HTFTP_init_con ARGS1(char *, url)
{
    int status;
    BOOL use_url = NO;	   /* YES if uid and passwd are specified in the URL */
    u_short serv_port = IPPORT_FTP;		/* Might be changed from URL */
    
    ftp_ctrl_info *ctrl;
    ftp_data_info *data;
    user_info user;		    /* Contains userid, passwd etc. from URL */

    if (!url || !*url) {
	if (TRACE)
	    fprintf(stderr, "HTFTP_get_connection: Bad server address!\n");
	return NULL;
    }

    /* Initiate new data connection structure */
    if ((data = (ftp_data_info *) calloc(1, sizeof(ftp_data_info))) == NULL)
	outofmem(__FILE__, "HTFTP_get_ctrl_con");
    data->socket = -1;			            /* Illigal socket number */
    data->active = YES;				    /* We do the active open */
    
    /* Scan URL for uid, pw and portnumber */
    memset(&user, '\0', sizeof(user_info));
    use_url = HTFTP_parse_login(url, &user, &serv_port);

    {
	char *filename = HTParse(url, "", PARSE_PATH);
	char *strptr;

	/* Check if file name is a directory */
	if (!*(strptr = filename) || 
	    *(strptr = filename+strlen(filename)-1) == '/') {
	    data->directory = YES;
	} else {
	    /* If data type is not specified in URL let's find it ourselves. */
	    HTAtom *encoding;
	    HTAtom *language;
	    HTUnEscape(filename);
	    data->fileformat = HTFileFormat(filename, &encoding, &language);
	    if (HTFTP_parse_datatype(filename, &data->datatype) != YES) {
		if ((encoding != HTAtom_for("8bit") &&
		     encoding != HTAtom_for("7bit"))) {
		    if (TRACE)
			fprintf(stderr, "FTP......... File is binary\n");
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
	struct sockaddr_in sock_addr;
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
	bzero((char *) &sock_addr, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(serv_port);
	
	/* Get node name */
	if (HTParseInet(&sock_addr, host)) {
	    if (TRACE) fprintf(stderr,
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
	    if (TRACE) fprintf(stderr,
			       "FTP......... Already have connection for %d.%d.%d.%d. on port %d, socket %d at location `%s\'\n",
			       (int)*((unsigned char *)(&ctrl->serv_node)+0),
			       (int)*((unsigned char *)(&ctrl->serv_node)+1),
			       (int)*((unsigned char *)(&ctrl->serv_node)+2),
			       (int)*((unsigned char *)(&ctrl->serv_node)+3),
			       ctrl->serv_port,
			       ctrl->socket,
			       ctrl->location);
	    data->ctrl = ctrl;			       /* Link them together */
	    HTList_addObject(ctrl->data_cons, (void *) data); /* Add to list */
	    FREE(user.domain);
	    FREE(user.id);
	    FREE(user.passwd);
	    return ctrl;           /* This should return the found structure */
	} else {
	    if (TRACE)
		fprintf(stderr, "FTP......... No existing connection found, so I build a new\n");
	}
    }
    
    /* Set up data structure for control connection */
    if ((ctrl = (ftp_ctrl_info *) calloc(1, sizeof(ftp_ctrl_info))) == NULL ||
	(ctrl->user = (user_info *) calloc(1, sizeof(user_info))) == NULL)
	outofmem(__FILE__, "HTFTP_init_con");
    ctrl->serv_port = serv_port;
    ctrl->socket = -1;			            /* Illigal socket number */
    StrAllocCopy(ctrl->location, "");			  /* Default is root */
    ctrl->server = UNKNOWN;
    ctrl->unsure_type = YES;
    ctrl->use_list = NO;
    ctrl->state = IDLE;
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
	if (uid)
	    StrAllocCopy(ctrl->user->passwd, uid);
	else
	    StrAllocCopy(ctrl->user->passwd, WWW_FTP_CLIENT);
	StrAllocCat(ctrl->user->passwd, "@");
    }
    FREE(user.domain);
    FREE(user.id);
    FREE(user.passwd);

    /* Now get ready for a connect */
    if (TRACE) fprintf(stderr, "FTP......... Looking for `%s\'\n", url);
    if ((status = HTDoConnect (url, "FTP", serv_port, &ctrl->socket,
			       &ctrl->serv_node)) < 0)
    {
	if (TRACE)
	    fprintf(stderr, "HTFTP_init_con: Connection not established!\n");
	HTFTP_abort_ctrl_con(ctrl);
	return NULL;
    }
    ctrl->isoc = HTInputSocket_new(ctrl->socket);               /* buffering */
    
    if (TRACE) 
 	fprintf(stderr, "FTP......... Control connected, socket %d\n",
		ctrl->socket);
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
    struct sockaddr_in local_addr;		   /* Binary network address */
    int status = -1;

#ifdef REPEAT_LISTEN
    if (master_socket >= 0) {				     /* Done already */
	data->socket = master_socket;
	if (TRACE)
	    fprintf(stderr, "FTP......... Reusing passive data socket: %d\n",
		    data->socket);
	return data->socket;
    }
#endif

    /* Create internet socket */
    if ((data->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	return HTInetStatus("socket for ftp data");
    if (TRACE)
	fprintf(stderr, "HTDoConnect. Created socket number %d\n",
		data->socket);
    
    /* Search for a free port. */
    bzero((char *) &local_addr, sizeof(local_addr));
    local_addr.sin_family = AF_INET;	    /* Family = internet, host order */
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* For multi homed hosts */

    /* Inherit the local address information from the control socket */
    {
	int addr_size = sizeof(local_addr);
	if (getsockname(data->ctrl->socket, (struct sockaddr *)
			&local_addr, &addr_size) < 0) {
	    status = HTInetStatus("getsockname");
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
		if (TRACE)
		    fprintf(stderr, "FTP......... No data port available.\n");
		goto errorfin;
	    }
	    local_addr.sin_port = htons(port_number);

	    /* The socket address is casted to generic sockaddr */
	    if (bind(data->socket, (struct sockaddr *) &local_addr,
		     sizeof(local_addr)) == 0)
		break;				     /* We have found a port */
	    status = HTInetStatus("bind");    /* else, what error did we get */
	}
    }
#else
    {
	local_addr.sin_port = 0;	     /* Unspecified: please allocate */

	/* The socket address is casted to a generic address */
	if (bind(data->socket, (struct sockaddr *) &local_addr,
		 sizeof(local_addr)) < 0) {
	    status = HTInetStatus("bind");
	    goto errorfin;
	}
    }
#endif
    /* Now we must find out who we are to tell the other guy. */
    {
	int addr_size = sizeof(local_addr);
	if (getsockname(data->socket, (struct sockaddr *) &local_addr,
			&addr_size) < 0) {
	    status = HTInetStatus("getsockname");
	    goto errorfin;
	}
	if (TRACE) fprintf(stderr, "FTP......... This host is `%s\'\n",
			   HTInetString(&local_addr));
    }
    if (TRACE) fprintf(stderr, "FTP......... Bound to port %d on %s\n",
		       (int) ntohs(local_addr.sin_port),
		       HTInetString(&local_addr));

    /* this_addr is a static global, we can refer to later */
    if (!this_addr && (this_addr = (char *) malloc(24)) == NULL)
	outofmem(__FILE__, "get_listen_socket");
    {
	u_long addr = ntohl(local_addr.sin_addr.s_addr);
	u_short port = ntohs(local_addr.sin_port);
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
    if (listen(data->socket, 1) < 0) {
	status = HTInetStatus("listen");
	goto errorfin;
    }
    if (TRACE) fprintf(stderr,
		       "FTP......... Data socket number %d listening\n",
		       data->socket);

#ifdef REPEAT_LISTEN
    master_socket = data->socket;		     /* Update master_socket */
#endif
    return data->socket;			       		     /* Good */

  errorfin:
    NETCLOSE(data->socket);
    data->socket = -1;
    return -1;
}
#endif


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
    HTChunk *newtext = HTChunkCreate(128);
    BOOL asked = YES;		    /* Have we already asked for uid/passwd? */
    int status = HTFTP_get_response(ctrl, &ctrl->welcome);   /* Get greeting */
    if (status < 0) {
        if (TRACE) fprintf (stderr, "FTP......... Interrupted at beginning of login.\n");
	return ERROR;
    }

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
	    status = HTFTP_get_response(ctrl, &newtext);
	    if (newtext->size > ctrl->welcome->size) {
		HTChunkFree(ctrl->welcome);
		ctrl->welcome = newtext;
	    }
	    if (status/100 == 2)		    /* Logged in w/o passwd! */
		state = SUCCESS;
	    else if (status/100 == 3)			/* Password demanded */
		state = NEED_PASSWD;
	    else if (status == 530 && asked == YES)
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
	       the end of the passwd */
	    if (!strcasecomp(ctrl->user->id, "anonymous")) {
		if (*(ctrl->user->passwd+strlen(ctrl->user->passwd)-1) != '@')
		    StrAllocCat(ctrl->user->passwd, "@");
	    }
	    if (!HTFTP_send_cmd(ctrl, "PASS", ctrl->user->passwd))
		state = SENT_PASSWD;
	    else
		state = ERROR;
	    break;

	  case SENT_PASSWD:
	    status = HTFTP_get_response(ctrl, &newtext);
	    if (newtext->size > ctrl->welcome->size) {
		HTChunkFree(ctrl->welcome);
		ctrl->welcome = newtext;
	    }
	    if (status/100 == 2)		    /* Logged in with passwd */
		state = SUCCESS;
	    else if (status/100 == 3)			 /* Account demanded */
		state = NEED_ACCOUNT;
	    else if (status == 530 && asked == YES)
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
	    status = HTFTP_get_response(ctrl, &newtext);
	    if (newtext->size > ctrl->welcome->size) {
		HTChunkFree(ctrl->welcome);
		ctrl->welcome = newtext;
	    }
	    if (status/100 == 2)
		state = SUCCESS;
	    else if (status/100 == 4)
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
	if (TRACE)
	    fprintf(stderr, "FTP......... Logged in at `%s\' as `%s\'\n",
		    ctrl->user->domain, ctrl->user->id);
    }

    /* This is a real pain this reuse user stuff :-( */
    if (HTFTPUserInfo) {
	StrAllocCopy(old_user->domain, ctrl->user->domain);
	StrAllocCopy(old_user->id, ctrl->user->id);
	StrAllocCopy(old_user->passwd, ctrl->user->passwd);
    }
    if (newtext != ctrl->welcome)
	HTChunkFree(newtext);
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
	    status = HTFTP_get_response(ctrl, NULL);
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
PRIVATE int HTFTP_get_data_con ARGS2(ftp_data_info *, data, char *, url)
{
    enum _state {
	ERROR = -2,
	FAILURE = -1,
	SUCCESS = 0,
	BEGIN,
	SENT_TYPE,
	SENT_PASV,
	SENT_PORT,
	NEED_ACTIVE,
	NEED_PASSIVE,
    } state = BEGIN;
    int serv_port;
    int status;
    
    /* This loop only stops if state is ERROR, FAILURE or SUCCESS */
    while (state > 0) {
	switch (state) {
	  case BEGIN:
	    /* First check if it is necessary to send TYPE, else send PASV */
	    if (data->datatype) {
		if (!HTFTP_send_cmd(data->ctrl, "TYPE", data->datatype))
		    state = SENT_TYPE;
		else
		    state = ERROR;
	    } else {
		if (!HTFTP_send_cmd(data->ctrl, "PASV", NULL))
		    state = SENT_PASV;
		else
		    state = ERROR;
	    }
	    break;
	    
	  case SENT_PASV:
	    {
		HTChunk *text = NULL;
		status = HTFTP_get_response(data->ctrl, &text);
		if (status == 227) {		    
		    /* If succes, we have to scan for the returned port number.
		       However, the format for the response is not standard, so
		       the best thing to do is to scan for the first digit
		       after the status code, see RFC1123 */
		    char *portstr;
		    int h0, h1, h2, h3, p0, p1;
		    portstr = text->data+3;
		    while (*portstr && !isdigit(*portstr++));
		    if (!*portstr || sscanf(--portstr, "%d,%d,%d,%d,%d,%d",
					    &h0, &h1, &h2, &h3, &p0, &p1)<4) {
			if (TRACE) fprintf(stderr,
			    "FTP......... PASV reply has no inet address!\n");
			state = ERROR;
		    } else {
			serv_port = (p0<<8)+p1;
			state = NEED_ACTIVE;
		    }
		} else if (status/100 == 4)
		    state = FAILURE;
		else if (state == 530)			 /* Not logged in??? */
		    state = ERROR;
		else
		    state = NEED_PASSIVE;      /* If error, try PORT instead */
		if (text)
		    HTChunkFree(text);
	    }
	    break;

	  case NEED_ACTIVE:
	    /* Now get ready for a connect */
	    if (TRACE) fprintf(stderr,
			       "FTP......... Server is listening on port %d\n",
			       serv_port);
	    status = HTDoConnect(url, "FTP", serv_port,
				 &data->socket, (u_long *) NULL);
	    if (status == -1) {
		if (TRACE) fprintf(stderr,
				   "FTP......... Data connection failed using PASV, let's try PORT instead\n");
		state = NEED_PASSIVE;
	    } else if (status >= 0) {
		if (TRACE) fprintf(stderr, "FTP......... Data connected using PASV, socket %d\n", data->socket);
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
		HTFTP_send_cmd(data->ctrl, "PORT", this_addr))
		state = ERROR;
	    else
		state = SENT_PORT;
#else
	    /* If PORT is not compiled, then there is nothing we can do! */
	    if (TRACE) fprintf(stderr, "FTP......... PORT is not possible!\n");
	    state = ERROR;
#endif
	    break;

	  case SENT_PORT:
	    status = HTFTP_get_response(data->ctrl, NULL);
	    if (status/100 == 2) {
		data->active = NO;
		state = SUCCESS;
	    } else if (status/100 == 4)
		state = FAILURE;
	    else
		state = ERROR;
	    break;

	  case SENT_TYPE:
	    status = HTFTP_get_response(data->ctrl, NULL);
	    /* If OK, then tell the server to be passive */
	    if (status/100 == 2) {
		if (!HTFTP_send_cmd(data->ctrl, "PASV", NULL))
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
		HTChunk *text = NULL;
		char *info;
		status = HTFTP_get_response(ctrl, &text);
		if (status/100 != 2) {
		    HTChunkFree(text);
		    state = NEED_PWD;
		    break;
		}

		/* Got a line - what kind of server are we talking to? */
		info = text->data+3;		       /* Skip response code */
		while (*info && *info++ == ' ');
		if (!*info) {
		    if (TRACE)
			fprintf(stderr, "FTP......... No server info?\n");
		    HTChunkFree(text);
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
		}
		
		/* If we are unsure, try PWD to get more information */
		if (ctrl->server == UNKNOWN || ctrl->unsure_type == YES)
		    state = NEED_PWD;
		else
		    state = SUCCESS;
		HTChunkFree(text);
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
		HTChunk *path = NULL;
		status = HTFTP_get_response(ctrl, &path);
		if (status/100 != 2)
		    state = ERROR;
		else {
		    char *start, *end;

		    /* Now analyze response information between "'s */
		    if ((start = strchr(path->data, '"')) == NULL ||
			(end = strchr(++start, '"')) == NULL) {
			if (TRACE)
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
		HTChunkFree(path);
	    }		    
	    break;
	    
	  case FAILURE:			      /* Otherwise gcc complains :-( */
	  case ERROR:
	  case SUCCESS:
	    break;
	}
    }
    if (TRACE) {
	static char *servers[] = {
	    "UNKNOWN",
	    "GENERIC",
	    "MACHTEN",
	    "UNIX",
	    "VMS",
	    "CMS",
	    "DCTS",
	    "TCPC",
	    "PETER_LEWIS",
	    "NCSA"
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
	    status = HTFTP_get_response(ctrl, NULL);
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
	if (*(relative+strlen(relative)-1) == '/')
	    *(relative+strlen(relative)-1) = '\0';
    }
    if (TRACE)
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
PRIVATE int HTFTP_get_dir ARGS4(ftp_ctrl_info *, ctrl, HTRequest *, req,
				char *, relative, char, retry)
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
	GOT_DATA
    } state = BEGIN;
    ftp_data_info *data = (ftp_data_info *) ctrl->data_cons->next->object; 
    int status;
    char *unescaped = NULL;
    StrAllocCopy(unescaped, relative);
    HTUnEscape(unescaped);

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
	    /* If we are listening, then do a non-blocking accept now, as the
	       accept on some systems (DYNIX) completes the connection. On
	       BSD systems, the completion is done independently of the
	       accept. (thanks to Bill Rushka, wcr@aps.org) */
	    if (data->active == NO && !retry) {
		int newfd;
		if ((newfd = HTDoAccept(data->socket)) >= 0) {
#ifdef REPEAT_LISTEN
		    if (TRACE) fprintf(stderr, "FTP......... Passive data channel number %d stays open.\n", data->socket);
#else
		    if (TRACE) fprintf(stderr, "FTP......... Passive data channel number %d closed.\n", data->socket);
		    if (NETCLOSE(data->socket) < 0) {
			HTInetStatus("close");
			state = ERROR;
			break;
		    }
#endif
		    data->socket = newfd;        /* Switch to new socket */
		    if (TRACE)
			fprintf(stderr, "FTP......... New data socket: %d\n",
				data->socket);
		} else {
		    state = ERROR;
		    break;
		}
	    }
	    status = HTFTP_get_response(ctrl, NULL);
	    if (status/100 == 1)
		state = READY_FOR_DATA;
	    else if (status/100 == 4)
		state = FAILURE;
	    else
		state = ERROR;
	    break;
	    
	  case SENT_CWD:
	    status = HTFTP_get_response(ctrl, &ctrl->welcome);
	    if (status/100 == 2) {
		/* Update current location */
		if (*ctrl->location)
		    StrAllocCat(ctrl->location, "/");
		StrAllocCat(ctrl->location, relative);
		state = NEED_LIST;
	    } else if (status/100 == 4)
		state = FAILURE;
	    else
		state = MULTIPLE_CWD;
	    break;

	  case MULTIPLE_CWD:
	    /* We must use the escaped version when looking for '/' as
	       delimiter between segments, and then unescape each segment */
	    if (TRACE) fprintf(stderr, "FTP......... Can't jump directly to location, try multiple CD's instead\n");
	    state = NEED_LIST;		     /* This is overwritten if error */
	    {
		char *path = NULL;
		char *segment;
		StrAllocCopy(path, relative);
		segment = strtok(path, "/");
		while (segment && *segment) {
		    HTUnEscape(segment);
		    if (HTFTP_send_cmd(ctrl, "CWD", segment)) {
			state = ERROR;
			break;
		    }
		    status = HTFTP_get_response(ctrl, &ctrl->welcome);
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
		free(path);
	    }
	    break;

	  case READY_FOR_DATA:
	    /* Now, the browsing module can be called */
	    {
		char *url = HTAnchor_physical(req->anchor);
		char *path = HTParse(url, "", PARSE_PATH+PARSE_PUNCTUATION);
		HTUnEscape(path);
		if (TRACE)
		    fprintf(stderr, "FTP......... Receiving directory `%s\'\n",
			    path);
		HTParseWelcome(ctrl);
		status = HTFTPBrowseDirectory(req, path, data,
					      HTFTP_get_dir_string);
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
	    status = HTFTP_get_response(ctrl, NULL);
	    if (status/100 == 2)
		state = SUCCESS;       			   /* Directory read */
	    else if (status/100 == 4)
		state = FAILURE;
	    else
		state = ERROR;
	    break;

	  case SENT_ABOR:
	    status = HTFTP_get_response(ctrl, NULL);
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
PRIVATE int HTFTP_get_file ARGS4(ftp_ctrl_info *, ctrl, HTRequest *, req,
				 char *, relative, char, retry)
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
	GOT_DATA
    } state = BEGIN;
    BOOL multiple = NO;		      /* Have we already tried multiple CWD? */
    ftp_data_info *data = (ftp_data_info *) ctrl->data_cons->next->object; 
    int status;
    char *unescaped = NULL;
    StrAllocCopy(unescaped, relative);
    HTUnEscape(unescaped);

    /* Thsi loop only stops if state is ERROR, FAILURE or SUCCESS */
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
	    /* If we are listening, then do a non-blocking accept now, as the
	       accept on some systems (DYNIX) completes the connection. On
	       BSD systems, the completion is done independently of the
	       accept. (thanks to Bill Rushka, wcr@aps.org) */
	    if (data->active == NO && !retry) {
		int newfd;
		if ((newfd = HTDoAccept(data->socket)) >= 0) {
#ifdef REPEAT_LISTEN
		    if (TRACE) fprintf(stderr, "FTP......... Passive data channel number %d stays open.\n", data->socket);
#else
		    if (TRACE) fprintf(stderr, "FTP......... Passive data channel number %d closed.\n", data->socket);
		    if (NETCLOSE(data->socket) < 0) {
			HTInetStatus("close");
			state = ERROR;
			break;
		    }
#endif
		    data->socket = newfd;        /* Switch to new socket */
		    if (TRACE)
			fprintf(stderr, "FTP......... New data socket: %d\n",
				data->socket);
		} else {
		    state = ERROR;
		    break;
		}
	    }
	    status = HTFTP_get_response(ctrl, NULL);
	    if (status/100 == 1)
		state = READY_FOR_DATA;
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
	    if (TRACE) fprintf(stderr, "FTP......... Can't jump directly to location, try multiple CD's instead\n");
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
		    if (HTFTP_send_cmd(ctrl, "CWD", segment)) {
			state = ERROR;
			break;
		    }
		    status = HTFTP_get_response(ctrl, NULL);
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
		    if (!HTFTP_send_cmd(ctrl, "RETR", segment))
			state = SENT_RETR;
		    else
			state = ERROR;
		} else {
		    if (TRACE) fprintf(stderr, "FTP......... Strange error, filename not found?\n");
		    state = ERROR;
		}
		free(path);
	    }
	    break;

	  case READY_FOR_DATA:
	    /* Now, the browsing module can be called */
	    if (TRACE) fprintf(stderr, "FTP......... Receiving file `%s\'\n",
			       unescaped);
	    status = HTParseSocket(data->fileformat, data->socket, req);
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
	    status = HTFTP_get_response(ctrl, NULL);
	    if (status/100 == 2)
		state = SUCCESS;       			   /* Directory read */
	    else if (status/100 == 4)
		state = FAILURE;
	    else
		state = ERROR;
	    break;

	  case SENT_ABOR:
	    status = HTFTP_get_response(ctrl, NULL);
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
    FREE(unescaped);
    return state;
}


/* ------------------------------------------------------------------------- */
/*			       PUBLIC FTP functions			     */
/* ------------------------------------------------------------------------- */

/*						   	   HTFTP_enable_session
**
**    	This function makes it possible to reuse the same control connections
**	until they are either timed out by the server, or that the session
**	is closed by HTFTP_end_session. Note that HTFTPLoad can run
**	independently of start and end session, and then each load runs like
**	an atomic action.
*/
PUBLIC void HTFTP_enable_session NOARGS
{
    if (session) {
	if (TRACE)
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
	if (TRACE)
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
PUBLIC int HTFTPLoad ARGS1(HTRequest *, request)
{
    char *url = HTAnchor_physical(request->anchor);
    int status = -1;
    int retry;			/* How many times tried? */
    ftp_ctrl_info *ctrl;

    /* Initiate a (possibly already exsisting) control connection and a
       corresponding data connection */
    HTSimplify(url);
    if((ctrl = HTFTP_init_con(url)) == NULL)
	return -1;

    /* Only if the control connection is in IDLE state, a new
       transfer can be started. The control connection can be in another
       mode if (session), and then the request is getting queued in
       ctrl->data_cons. */
    if (ctrl->state == IDLE || (session && ctrl->state == LOGGED_IN)) {
	ftp_data_info *data = ctrl->data_cons->next->object;
	if (ctrl->state == IDLE)
	    ctrl->state = BEGIN;
	while (ctrl->state != IDLE) {		       	/* Do until finished */
	    switch (ctrl->state) {
	      case BEGIN:
		if (!HTFTP_login(ctrl))
		    ctrl->state = LOGGED_IN;
		else
		    ctrl->state = ERROR;
		break;

	      case LOGGED_IN:
		if (!HTFTP_get_data_con(data, url))
		    ctrl->state = GOT_DATA_CON;
		else
		    ctrl->state = ERROR;
		break;

	      case GOT_DATA_CON:
		{
		    /* Now we must ask for the URL requested. If FAILURE, then
		       we try twice to see, if it helps */
		    char *rel;
		    for (retry=0; retry<2; retry++) {
			if ((rel = HTFTPLocation(ctrl, url)) == NULL) {
			    ctrl->state = ERROR;
			    break;
			}
			if (retry == 1 && TRACE) fprintf(stderr,
							 "FTP......... First attempt to get URL failed, let's try again\n");

			if (data->directory == YES) {
			    /* If we haven't already got server-info */
			    if (ctrl->server == UNKNOWN) {
				if (HTFTPServerInfo(ctrl)) {
				    ctrl->state = ERROR;
				    break;
				}
			    }
			    status = HTFTP_get_dir(ctrl, request, rel, retry);
			}
			else
			    status = HTFTP_get_file(ctrl, request, rel, retry);
			if (!status) {
			    ctrl->state = GOT_DATA;
			    break;
			} else if (status == -2) {		    /* Error */
			    ctrl->state = ERROR;
			    break;
			} else {
			    free(rel);
			    ctrl->state = FAILURE;		/* Try twice */
			}
		    }
		    free(rel);
		}
		if (retry == 2 && ctrl->state == FAILURE)
		    ctrl->state = ERROR;
		break;

	      case GOT_DATA:
		if (HTFTP_close_data_con(data))
		    ctrl->state = ERROR;
		else {
		    HTList_removeLastObject(ctrl->data_cons);
		    if (!session) {
			if (!HTFTP_logout(ctrl)) {
			    ctrl->state = IDLE;
			    status = HT_LOADED;
			} else
			    ctrl->state = ERROR;
		    } else {
			ctrl->state = LOGGED_IN;   /* Ready for next request */
			return HT_LOADED;
		    }
		    break;
		}
		break;

	      case ERROR:
		HTFTP_abort_ctrl_con(ctrl);
		return -1;				 /* Exit immediately */
		break;

	      default:
		if (TRACE) fprintf(stderr, "FTP......... Unknown state, what happened?\n");
		break;
	    }
	}

	/* The control connection is only closed if the load is atomic */
	if (!session && HTFTP_close_ctrl_con(ctrl))
	    status = -1;
    }
    if (status < 0 && status != HT_INTERRUPTED)
	return HTLoadError(request, 500,
              "Document not loaded due to strange behavior from FTP-server\n");
    return status;
}

/* END OF MODULE */


