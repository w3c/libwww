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
**
** Options:
**	LISTEN		We listen, the other guy connects for data.
**			Otherwise, other way round, but problem finding our
**			internet address!
**
** Notes:
**     			Portions Copyright 1994 Trustees of Dartmouth College
** 			Code for recognizing different FTP servers and
**			parsing "ls -l" output taken from Macintosh Fetch
**			program with permission from Jim Matthews,
**			Dartmouth Software Development Team.
**
**	BUGS:	@@@  	Limit connection cache size!
**			Error reporting to user.
**			400 & 500 errors are acked by user with windows.
**			Use configuration file for user names
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
#define REPEAT_PORT		       /* Give the port number for each file */
#define REPEAT_LISTEN	      /* Close each listen socket and open a new one */
/* define POLL_PORTS		 If allocation does not work, poll ourselves.*/
#define LISTEN_BACKLOG 2	/* Number of pending connect requests (TCP)*/
#define LISTEN	 /* @@@@ Test LJM */

#define DATA_BUFFER_SIZE 2048
#define FIRST_TCP_PORT	 1024	       /* Region to try for a listening port */
#define LAST_TCP_PORT	 5999	
#define LINE_LENGTH   	 256
#define NEXT_DATA_CHAR next_data_char()

#ifndef IPPORT_FTP
#define IPPORT_FTP    	 21
#endif

/* Globals */


/* Type definitions and global variables etc. local to this module */
typedef struct _connection {
    struct _connection *	next;	/* Link on list 	*/
    u_long			addr;	/* IP address		*/
    int				socket;	/* Socket number for communication */
    BOOL			binary; /* Binary mode? */
    HTInputSocket *             isoc;
    char *			uid;
    char *			passwd;
} connection;

typedef enum _HTFTPServerType {
    GENERIC_SERVER = 0,
    MACHTEN_SERVER,
    UNIX_SERVER,
    VMS_SERVER,
    CMS_SERVER,
    DCTS_SERVER,
    TCPC_SERVER,
    PETER_LEWIS_SERVER,
    NCSA_SERVER
} HTFTPServerType;

PRIVATE char    response_text[LINE_LENGTH+1];/* Last response from FTP host */
PRIVATE connection *control;		       /* Current control connection */
PRIVATE int	data_soc = -1;		/* Socket for data transfer =invalid */
PRIVATE int     server_type = GENERIC_SERVER;   /* the type of ftp host */
PRIVATE int     unsure_type = FALSE;            /* sure about the type? */
PRIVATE BOOLEAN use_list = FALSE;		/* use the LIST command? */
PRIVATE char data_buffer[DATA_BUFFER_SIZE];		/* Input data buffer */
PRIVATE char * data_read_pointer;
PRIVATE char * data_write_pointer;

#ifdef POLL_PORTS
PRIVATE	unsigned short	port_number = FIRST_TCP_PORT;
#endif

#ifdef LISTEN
PRIVATE int     master_socket = -1;	/* Listening socket = invalid	*/
PRIVATE char	port_command[255];	/* Command for setting the port */
PRIVATE fd_set	open_sockets; 		/* Mask of active channels */
PRIVATE int	num_sockets;  		/* Number of sockets to scan */
#else
PRIVATE	unsigned short	passive_port;	/* Port server specified for data */
#endif

#ifdef NEW_CODE
#define NEXT_CHAR HTGetCharacter()	/* Use function in HTFormat.c */
#endif

/* TEPORARY!!!!!! */
#define HT_INTERRUPTED -2
PRIVATE BOOL interrupted_in_htgetcharacter;
/* ************* */

/* ------------------------------------------------------------------------- */
/*			   Directory Specific Functions			     */
/* ------------------------------------------------------------------------- */

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
	if (TRACE) fprintf(stderr, "HTStrpMonth: Couldn't resolve date.\n");
    }
    return ret;
}


/*								HTStrpTime()
**
**	Converts a date string from 'ls -l' to a unsigned long number
**	This is needed in order to put out the date using the same format
**	for all directory listings.
**
**	Returns 0L on error.
*/
PRIVATE unsigned long HTStrpTime ARGS1(char *, datestr)
{
    struct tm *time_info;		    /* Points to static tm structure */
    char *bcol = datestr;			             /* Column begin */
    char *ecol;						       /* Column end */
    long tval;
    int cnt;
    unsigned long curtime = time(NULL);
    if ((time_info = gmtime(&curtime)) == NULL) {
	if (TRACE)
	return 0L;
    }
    time_info->tm_isdst = -1;			      /* Disable summer time */
    for (cnt=0; cnt<3; cnt++)					    /* Month */
	*bcol++ = toupper(*bcol);
    if ((time_info->tm_mon = HTStrpMonth(datestr)) < 0)
	return 0L;
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
	*ecol = '\0';
	time_info->tm_sec = 0;
	time_info->tm_min = atoi(++ecol);		/* Right side of ':' */
	time_info->tm_hour = atoi(bcol);		 /* Left side of ':' */
    }
    return ((tval = mktime(time_info)) == -1 ? 0L : tval); 
}


/*								HTVMSStrpTime()
**
**	Converts a date string from vms to a unsigned long number
**	This is needed in order to put out the date using the same format
**	for all directory listings.
**
**	Returns 0L on error
*/
PRIVATE unsigned long HTVMSStrpTime ARGS1(char *, datestr)
{
    struct tm *time_info;		    /* Points to static tm structure */
    char *col;
    long tval;
    unsigned long curtime = time(NULL);
    if ((time_info = gmtime(&curtime)) == NULL)
	return 0L;
    time_info->tm_isdst = -1;			      /* Disable summer time */
    if ((col = strtok(datestr, "-")) == NULL)
	return 0L;
    time_info->tm_mday = atoi(col);				      /* Day */
    time_info->tm_wday = 0;
    time_info->tm_yday = 0;
    if ((col = strtok(NULL, "-")) == NULL ||
	(time_info->tm_mon = HTStrpMonth(col)) < 0)
	return 0L;
    if ((col = strtok(NULL, " ")) == NULL)			     /* Year */
	return 0L;
    time_info->tm_year = atoi(col)-1900;
    if ((col = strtok(NULL, ":")) == NULL)			     /* Hour */
	return 0L;
    time_info->tm_hour = atoi(col);
    if ((col = strtok(NULL, " ")) == NULL)			     /* Mins */
	return 0L;
    time_info->tm_min = atoi(col);
    time_info->tm_sec = 0;
    return ((tval = mktime(time_info)) < 0 ? 0L : tval);
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
	    f_info->f_gid = "";
	    f_info->f_size = lval;
	}
    }
    column = column+strlen(column)+1;
    while (*column++ == ' ');
    strptr = --column+12;		      	     /* Find the date column */
    *strptr++ = '\0';
    if ((f_info->f_mtime = HTStrpTime(column)) == 0L)
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
	if (strlen(cpd) > 9 && isdigit(*(cpd-1)) &&
	    isalpha(*(cpd+1)) && *(cpd+4) == '-') {
	    if ((f_info->f_mtime = HTVMSStrpTime(cpd-2)) == 0L)
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
**      If first is true, this is the first name in a directory.
**	Returns YES if OK, NO on error
*/
PRIVATE BOOL parse_dir_entry ARGS3(char *, entry, BOOL, first,
				   dir_file_info *, f_info)
{
    BOOL status = YES;
    switch (server_type) {
      case UNIX_SERVER:
      case PETER_LEWIS_SERVER:
      case MACHTEN_SERVER:
	/* Interpret and edit LIST output from Unix server */
	if (first == YES) {
	    if(!strncmp(entry, "total ", 6) ||
	       (strstr(entry, "not available") != NULL)) {
		return NO;
	    } else if(unsure_type) {
		/* this isn't really a unix server! */
		server_type = GENERIC_SERVER;
		return NO;
	    }
	}
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


/* THIS FUNCTION IS TEMPORARY!!!!!!!!!!!!!!! */

/*	Procedure: Read a character from the data connection
**	----------------------------------------------------
**
**	The type is int in order to be able to return EOF
*/
PRIVATE int next_data_char
NOARGS
{
    int status;
    if (data_read_pointer >= data_write_pointer) {
	status = NETREAD(data_soc, data_buffer, DATA_BUFFER_SIZE);
	if (status == HT_INTERRUPTED) {
	    if (TRACE) fprintf(stderr, "next_data_char: Interrupted\n");
	    return EOF;
	}
	if (status <= 0) {
	    if (TRACE) fprintf(stderr, "next_data_char: Error: %d\n", status);
	    return EOF;
	}
	data_write_pointer = data_buffer + status;
	data_read_pointer = data_buffer;
    }
#ifdef NOT_ASCII
    {
        char c = *data_read_pointer++;
	return (int) FROMASCII(c);
    }
#else
    return (int) *data_read_pointer++;
#endif
}


PRIVATE int HTFTP_get_dir_string ARGS1(dir_file_info *, f_info)
{
    int ch;			      /* Must be int in order to contain EOF */
    int status;
    static BOOL first = YES;		    /* Is it the first time through? */
    HTChunk *chunk = HTChunkCreate(128);

    for(;;) {			                /* Until we have a nice line */
	if ((ch = NEXT_DATA_CHAR) == EOF) {
	    first = YES;			        /* For the next time */
	    status = 0;
	    break;
	}
	if (ch == CR || ch == LF) {			      /* Terminator? */
	    if (chunk->size != 0) {			    /* got some text */
		if (server_type == VMS_SERVER) {
		    /* Deal with MultiNet's wrapping of long lines - F.M. */
		    if (data_read_pointer < data_write_pointer &&
			*(data_read_pointer+1) == ' ')
			data_read_pointer++;
		    else if (data_read_pointer >= data_write_pointer) {
			int ret = NETREAD(data_soc, data_buffer,
					  DATA_BUFFER_SIZE);
			if (ret == HT_INTERRUPTED) {
			    if (TRACE)
				fprintf(stderr,
					"HTFTP_get_string: Interrupted\n");
			    first = YES;
			    status = 0;
			    break;
			}
			if (ret <= 0) {
			    if (TRACE)
				fprintf(stderr,
					"HTFTP_get_string: Error %d\n", ret);
			    first = YES;
			    status = -1;
			    break;
			}
			data_write_pointer = data_buffer+ret;
			data_read_pointer = data_buffer;
			if (*data_read_pointer == ' ')
			    data_read_pointer++;
		    }
		}
		HTChunkTerminate(chunk);	 /* Finish getting one entry */
		if (parse_dir_entry(chunk->data, first, f_info) == YES) {
		    first = NO;
		    status = 1;
		    break;
		} else {
		    HTChunkClear(chunk);
		}
	    }
	} else {
	    HTChunkPutc(chunk, ch);
	}
    } /* end for(;;) */
    HTChunkFree(chunk);
    return status;
}

/* ------------------------------------------------------------------------- */
/*			       FTP Client Functions			     */
/* ------------------------------------------------------------------------- */

/*								set_mac_binary
**
**	This function should try to set the macintosh server into binary mode
 */
PRIVATE int set_mac_binary NOARGS
{
    return(2 == response("MACB\r\n"));
}


/* This function gets the current working directory to help
 * determine what kind of host it is
 */
PRIVATE void get_ftp_pwd ARGS2(int *,server_type, BOOLEAN *,use_list) {

    char *cp;
    /* get the working directory (to see what it looks like) */
    int status = response("PWD\r\n");
    if (status < 0)
        return;
    else 
     {

	cp = strchr(response_text+5,'"');
	if(cp) *cp = '\0';
        if (*server_type == TCPC_SERVER)
         {
            *server_type = response_text[5] == '/' ? NCSA_SERVER : TCPC_SERVER;
         }
        else if (response_text[5] == '/')
         {
            /* path names beginning with / imply Unix,
	     * right? 
	     */
	     if(set_mac_binary())
		   *server_type = NCSA_SERVER;
	     else
		{
                   *server_type = UNIX_SERVER;
                   *use_list = TRUE;
		}
	     return;
         }
        else if (response_text[strlen(response_text)-1] == ']')
         {
             /* path names ending with ] imply VMS, right? */
             *server_type = VMS_SERVER;
	     *use_list = TRUE;
         }
        else
             *server_type = GENERIC_SERVER;

        if ((*server_type == NCSA_SERVER) ||
               (*server_type == TCPC_SERVER) ||
                    (*server_type == PETER_LEWIS_SERVER))
            set_mac_binary();
     }
}


#ifdef NEW_CODE
/*							     HTFTPServerInfo()
**
**	This function finds out what server we are talking to. Returns
**	-1 on error, else the 'number' of the server.
**
**	Thanks to James.W.Matthews@Dartmouth.EDU (James W. Matthews) for using
**	his code.
*/
PUBLIC BOOL HTFTServerInfo NOARGS
{
    static short    state = FTP_INFO_START;  /* static var to remember state */
    char            hold[MAX_LINE_LENGTH];               /* for getting info */
    short           osstat;                              /* OS status return */
    
    check_reset();
    switch(state) {
      case FTP_INFO_START:                                     /* first call */
	osstat = send_cmd("SYST", NULL);
	if (osstat == noErr)		    /* success -- move to next state */
	    next_state(FTP_INFO_SENT_SYST);
	else
	    return(osstat);
	break;
	
      case FTP_INFO_SENT_SYST:
	/* try to get reply from server */
	osstat = get_reply(&ftp_server_code, ftp_server_text);
	if (osstat == NO_LINE_ERR)
	    return FTP_NOT_DONE;
	else if (osstat != noErr)
	    err_return(osstat);
		
	/* Got a line - what kind of server are we talking to? */
	if ((ftp_server_code == FTP_RP_SYST_TYPE) ||
		    (ftp_server_code == FTP_RP_COMMAND_OK)) {
	    if (strncmp(ftp_server_text,
			"UNIX Type: L8MAC-OSMachTen", 28) == 0) {
		server_type = MACHTEN_SERVER;
		use_list = TRUE;
	    } else if (strstr(ftp_server_text, "UNIX") !=
		       NULL) {
		server_type = UNIX_SERVER;
		use_list = TRUE;
	    } else if (strncmp(ftp_server_text, "VMS", 3) == 0)
		server_type = VMS_SERVER;
	    else if ((strncmp(ftp_server_text, "VM/CMS", 6) == 0) ||
		     (strncmp(ftp_server_text, "VM", 3) == 0))
		server_type = CMS_SERVER;
	    else if (strncmp(ftp_server_text, "DCTS", 4) == 0)
		server_type = DCTS_SERVER;
	    else if (strstr(ftp_server_text,
			    "MAC-OS TCP/ConnectII") != NULL) {
		server_type = TCPC_SERVER;
		unsure_type = TRUE;
		unknown_mb_mode = TRUE;
	    } else if (strncmp(ftp_server_text,
			       "MACOS Peter's Server", 20) == 0) {
		server_type = PETER_LEWIS_SERVER;
		use_list = TRUE;
		unknown_mb_mode = TRUE;
	    }
	    
	    can_see_dirs = (server_type == UNIX_SERVER) ||
		(server_type == VMS_SERVER) ||
		    (server_type == CMS_SERVER) ||
			(server_type == NCSA_SERVER) ||
			    (server_type == TCPC_SERVER) ||
				(server_type==MACHTEN_SERVER)||
				    (server_type ==
				     PETER_LEWIS_SERVER);
	    unsure_type = FALSE;
	    if ((server_type == TCPC_SERVER) ||
		(server_type ==
		 GENERIC_SERVER))
		/* need to check for old versions of TCP/C
		   that use / in pathnames */
		next_state(FTP_INFO_GET_WD)
	    else if ((server_type == NCSA_SERVER) ||
		     (server_type ==
		      PETER_LEWIS_SERVER))
		next_state(FTP_INFO_SET_MACB)
	    else
		err_return(noErr)
		}
	else if (server_type == GENERIC_SERVER)
	    /* no SYST command, generic server, so check the work dir */
	    next_state(FTP_INFO_GET_WD)
		break;
	
      case FTP_INFO_GET_WD:
	/* get the working directory (to see what it looks like) */
	osstat = ftp_pwd(hold);
	if (osstat > noErr)
	    return FTP_NOT_DONE;
	else if (osstat == noErr) {
	    if (server_type == TCPC_SERVER) {
		server_type = hold[0] == '/' ?
		    NCSA_SERVER :
			TCPC_SERVER;
		unsure_type = FALSE;
		unknown_mb_mode = TRUE;
	    } else if (hold[0] == '/') {
		/* path names beginning with / imply Unix, right? */
		server_type = UNIX_SERVER;
		unsure_type = TRUE;
		use_list = TRUE;
	    } else if (hold[strlen(hold)-1] == ']') {
		/* path names ending with ] imply VMS, right? */
		server_type = VMS_SERVER;
		unsure_type = TRUE;
	    } else
		server_type = GENERIC_SERVER;
	    can_see_dirs = (server_type == UNIX_SERVER) ||
		(server_type == VMS_SERVER) || (server_type == CMS_SERVER) ||
			(server_type == NCSA_SERVER) || 
			    (server_type == TCPC_SERVER) ||
				(server_type == MACHTEN_SERVER) ||
				    (server_type == PETER_LEWIS_SERVER);
	    if ((server_type == NCSA_SERVER) || (server_type == TCPC_SERVER) ||
		(server_type == PETER_LEWIS_SERVER))
		next_state(FTP_INFO_SET_MACB)
	    else
		err_return(noErr)
		}
	else if (osstat == FTP_PROTOCOL_ERR)
	    err_return(noErr)
	else
	    err_return(osstat)
		break;
	
      case FTP_INFO_SET_MACB:
	/*turn on MacBinary (to let server know that we know that it's a Mac)*/
	osstat = ftp_set_macb(TRUE);
	if (osstat > noErr)
	    return FTP_NOT_DONE;
	else
	    err_return(noErr)
		break;
    } /* switch state */
} /* ftp_info() */
#endif /* NEW_CODE */


/*	Execute Command and get Response
**	--------------------------------
**
**	See the state machine illustrated in RFC959, p57. This implements
**	one command/reply sequence.  It also interprets lines which are to
**	be continued, which are marked with a "-" immediately after the
**	status code.
**
**	Continuation then goes on until a line with a matching reply code
**	an a space after it.
**
** On entry,
**	con	points to the connection which is established.
**	cmd	points to a command, or is NULL to just get the response.
**
**	The command is terminated with the CRLF pair.
**
** On exit,
**	returns:  The first digit of the reply type,
**		  or negative for communication failure.
*/
PRIVATE int response ARGS1(char *, cmd)
{
    int result;				/* Three-digit decimal code */
    int	continuation_response = -1;
    int status;
    extern int interrupted_in_htgetcharacter;
    
    if (!control) {
          if(TRACE) fprintf(stderr, "FTP: No control connection set up!!\n");
	  return -99;
    }
    
    if (cmd) {
    
	if (TRACE) fprintf(stderr, "  Tx: %s", cmd);

#ifdef NOT_ASCII
	{
	    char * p;
	    for(p=cmd; *p; p++) {
	        *p = TOASCII(*p);
	    }
	}
#endif 
	status = NETWRITE(control->socket, cmd, (int)strlen(cmd));
	if (status<0) {
	    if (TRACE) fprintf(stderr, 
	    	"FTP: Error %d sending command: closing socket %d\n",
		status, control->socket);
	    close_connection(control);
	    return status;
	}
    }

    do {
	char *p = response_text;
	for(;;) {
#if NEW_CODE
	    if (((*p++=NEXT_CHAR) == LF)
			|| (p == &response_text[LINE_LENGTH])) {
#endif
	    if (((*p++=HTInputSocket_getCharacter(control->isoc)) == LF)
			|| (p == &response_text[LINE_LENGTH])) {
		char continuation;

	        if (interrupted_in_htgetcharacter)
                  {
                    if (TRACE)
			fprintf(stderr,
				"FTP: Interrupted in HTGetCharacter, apparently.\n");
                    NETCLOSE (control->socket);
                    control->socket = -1;
                    return HT_INTERRUPTED;
                  }

		*p++=0;			/* Terminate the string */
		if (TRACE) fprintf(stderr, "    Rx: %s", response_text);

#if 0
		if(server_type == UNIX_SERVER && 
					!strncmp(response_text,"250-",4))
		    help_message_cache_add(response_text+4);
#endif
		sscanf(response_text, "%d%c", &result, &continuation);
		if  (continuation_response == -1) {
			if (continuation == '-')  /* start continuation */
			    continuation_response = result;
		} else { 	/* continuing */
			if (continuation_response == result
				&& continuation == ' ')
			    continuation_response = -1;	/* ended */
		}	
		break;	    
	    } /* if end of line */
	    
	    if (interrupted_in_htgetcharacter)
               {
                    if (TRACE)
                      fprintf (stderr, "FTP: Interrupted in HTGetCharacter, apparently.\n");
                    NETCLOSE (control->socket);
                    control->socket = -1;
                    return HT_INTERRUPTED;
               }

	    if (*(p-1) == (char) EOF) {
		if(TRACE) fprintf(stderr, "Error on rx: closing socket %d\n",
		    control->socket);
		strcpy(response_text, "000 *** TCP read error on response\n");
	        close_connection(control);
	    	return -1;	/* End of file on response */
	    }
	} /* Loop over characters */

    } while (continuation_response != -1);
    
    if (result==421) {
	if(TRACE) fprintf(stderr, "FTP: They close so we close socket %d\n",
	    control->socket);
	close_connection(control);
	return -1;
    }
    return result/100;
}


/*	Close an individual connection
**
*/
PRIVATE int close_connection ARGS1(connection *, con)
{
    int status;
    if(con->isoc) {
        HTInputSocket_free(con->isoc);
        con->isoc = NULL;
    }
    if (TRACE)
	fprintf(stderr, "FTP: Closing control socket %d\n", con->socket);
    status = NETCLOSE(con->socket);
    return status;
}


/*	Get a valid connection to the host
**	----------------------------------
**
** On entry,
**	arg	points to the name of the host in a hypertext address
** On exit,
**	returns	<0 if error
**		socket number if success
**
**	This routine takes care of managing timed-out connections, and
**	limiting the number of connections in use at any one time.
**
**	It ensures that all connections are logged in if they exist.
**	It ensures they have the port number transferred.
*/
PRIVATE int get_connection ARGS1 (CONST char *,arg)
{
    int status;
    char * command;
    connection * con = (connection *)malloc(sizeof(*con));

    struct sockaddr_in soc_address;     /* Binary network address */
    struct sockaddr_in* sin = &soc_address;

    char * username = NULL;
    char * password = NULL;
    static char *user_entered_password=0;
    static char *last_username_and_host=0;
    
    if (!arg) return -1;		/* Bad if no name sepcified	*/
    if (!*arg) return -1;		/* Bad if name had zero length	*/

    if (TRACE) fprintf(stderr, "FTP: Looking for %s\n", arg);

/* Get node name:
*/
    {
	char *p1 = HTParse(arg, "", PARSE_HOST);
	char *p2 = strrchr(p1, '@');	/* user? */
	char * pw=0;

	if (p2!=NULL) {
	    username = p1;
	    *p2=0;			/* terminate */
	    p1 = p2+1;			/* point to host */
	    pw = strchr(username, ':');
	    if (pw) {
	        *pw++ = 0;
		password = pw;
	    }

	    /* if the password doesn't exist then we are going to have
	     * to ask the user for it.  The only problem is that we
	     * don't want to ask for it every time, so we will store
	     * away in a primitive fashion.
	     */
	    if(!password) {
		char tmp[256];

		sprintf(tmp,"%s@%s",username,p1);
		/* if the user@host is not equal to the last time through
		 * or user_entered_password has no data then we need
		 * to ask the user for the password
		 */
		if(!last_username_and_host ||
			strcmp(tmp,last_username_and_host) ||
						!user_entered_password) {

		    StrAllocCopy(last_username_and_host,tmp);
		    sprintf(tmp,"Enter password for user %s@%s:",username,p1);
		    if(user_entered_password) 
			free(user_entered_password);
		    user_entered_password = (char *)HTPromptPassword(tmp);

		} /* else we already know the password */
		password = user_entered_password;
	    }
	}
/* OLD STUFF */	
	if (HTParseInet(sin, p1)) { free(p1); return -1;} /* TBL 920622 */
/* END OS */
        if (!username) free(p1);
    } /* scope of p1 */
 
/* THIS IS OLD STUFF */
/*  Set up defaults:
*/
    sin->sin_family = AF_INET;                  /* Family, host order  */
    sin->sin_port = htons(IPPORT_FTP);          /* Well Known Number    */

        if (con == NULL) outofmem(__FILE__, "get_connection");   
        con->addr = sin->sin_addr.s_addr;       /* save it */
        con->binary = NO;
        status = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (status<0) {
            (void) HTInetStatus("socket");
            free(con);
            if (username) free(username);
            return status;
        }
        con->socket = status;

    status = connect(con->socket, (struct sockaddr*)&soc_address,
		     sizeof(soc_address));
/* END OLD_STUFF */
#ifdef NEW_CODE
    con->socket = -1;
    status = HTDoConnect (arg, "FTP", IPPORT_FTP, &con->socket);
#endif
  if (status < 0)
    {
      if (TRACE)
        {
          if (status == HT_INTERRUPTED)
            fprintf (stderr,
                     "FTP: Interrupted on connect\n");
          else
            fprintf(stderr,
                "FTP: Unable to connect to remote host for `%s' (errno %d).\n",
                    arg, errno);
        }
      if (status == HT_INTERRUPTED)
        HTProgress ("Connection interrupted.");
      if (con->socket != -1)
        {
          NETCLOSE(con->socket);
        }
	
      if (username)
          free(username);
      free(con);
      return status;                    /* Bad return */
    }

	
    if (TRACE) 
 	fprintf(stderr, "FTP conneced, socket %d\n", con->socket);

    /* Initialise buffering for contron connection */
#ifdef NEW_CODE
    HTInitInput(control->socket);
#endif
    control = con;		/* Current control connection */
    control->isoc = HTInputSocket_new(control->socket); /* buffering */

/*	Now we log in		Look up username, prompt for pw.
*/
  {
    int status = response((char *)0);	/* Get greeting */

    if (status == HT_INTERRUPTED)
      {
        if (TRACE)
          fprintf (stderr,
                   "FTP: Interrupted at beginning of login.\n");
        HTProgress ("Connection interrupted.");
        NETCLOSE(control->socket);
        control->socket = -1;
        return HT_INTERRUPTED;
      }
    if (status == 2) {		/* Send username */
	if (username) {
	    command = (char*)malloc(10+strlen(username)+2+1);
	    if (command == NULL) outofmem(__FILE__, "get_connection");
	    sprintf(command, "USER %s%c%c", username, CR, LF);
	} else {
	    command = (char*)malloc(24);
	    if (command == NULL) outofmem(__FILE__, "get_connection");
	    sprintf(command, "USER anonymous%c%c", CR, LF);
        }
	status = response(command);
	free(command);
        if (status == HT_INTERRUPTED)
          {
            if (TRACE)
              fprintf (stderr,
                       "FTP: Interrupted while sending username.\n");
            HTProgress ("Connection interrupted.");
            NETCLOSE(control->socket);
            control->socket = -1;
            return HT_INTERRUPTED;
          }
    }
    if (status == 3) {		/* Send password */
	if (password) {
	    command = (char*)malloc(10+strlen(password)+2+1);
	    if (command == NULL) outofmem(__FILE__, "get_connection");
	    sprintf(command, "PASS %s%c%c", password, CR, LF);
	} else {
	    char * user = getenv("USER");
	    CONST char *host = HTHostName();
	    if (!user) user = "WWWuser";
	    /* If not fully qualified, suppress it as ftp.uu.net
	       prefers a blank to a bad name */
	    if (!strchr(host, '.')) host = "";

	    command = (char*)malloc(20+strlen(host)+2+1);
	    if (command == NULL) outofmem(__FILE__, "get_connection");
	    sprintf(command, "PASS %s@%s%c%c", user ? user : "WWWuser",
	    						host, CR, LF); /*@@*/
        }
	status = response(command);
	free(command);
        if (status == HT_INTERRUPTED)
          {
            if (TRACE)
              fprintf (stderr,
                       "FTP: Interrupted while sending password.\n");
            HTProgress ("Connection interrupted.");
            NETCLOSE(control->socket);
            control->socket = -1;
            return HT_INTERRUPTED;
          }
    }
    FREE(username);

    if (status == 3) {
        char temp[80];
	sprintf(temp, "ACCT noaccount%c%c", CR, LF);
	status = response(temp);
	if (status == HT_INTERRUPTED)
          {
            if (TRACE)
              fprintf (stderr,
                       "FTP: Interrupted while sending password.\n");
            HTProgress ("Connection interrupted.");
            NETCLOSE(control->socket);
            control->socket = -1;
            return HT_INTERRUPTED;
          }

    }
    if (status !=2) {
        if (TRACE) fprintf(stderr, "FTP: Login fail: %s", response_text);
    	/* if (control->socket > 0) close_connection(control->socket); */
        return -1;		/* Bad return */
    }
    if (TRACE) fprintf(stderr, "FTP: Logged in.\n");

    /** Check for host type **/
    server_type = GENERIC_SERVER;	/* reset */
    use_list = FALSE; 			/* reset */
    {
#if 0
    char temp[80];
    sprintf(temp, "SYST%c%c", CR, LF);
#endif    
    if ((status=response("SYST\r\n")) == 2) {
                /* we got a line -- what kind of server are we talking to? */
         if (strncmp(response_text+4, "UNIX Type: L8 MAC-OS MachTen", 28) == 0)
          {
             server_type = MACHTEN_SERVER;
	     use_list = TRUE;
          }
         else if (strstr(response_text+4, "UNIX") != NULL)
          {
             server_type = UNIX_SERVER;
	     use_list = TRUE;
          }
         else if (strncmp(response_text+4, "VMS", 3) == 0)
	  {
             server_type = VMS_SERVER;
	     use_list = TRUE;
	  }
         else if ((strncmp(response_text+4, "VM/CMS", 6) == 0)
				 || (strncmp(response_text+4, "VM ", 3) == 0))
             server_type = CMS_SERVER;
         else if (strncmp(response_text+4, "DCTS", 4) == 0)
             server_type = DCTS_SERVER;
         else if (strstr(response_text+4, "MAC-OS TCP/Connect II") != NULL)
          {
             server_type = TCPC_SERVER;
             get_ftp_pwd(&server_type, &use_list);
	     unsure_type = TRUE;   
          }
         else if (strncmp(response_text+4, "MACOS Peter's Server", 20) == 0)
          {
             server_type = PETER_LEWIS_SERVER;
             use_list = TRUE;
             set_mac_binary();
          }
	 else 
	  {
	     server_type = GENERIC_SERVER;
             get_ftp_pwd(&server_type, &use_list);
	     unsure_type = TRUE;   
	  }
    } else {
	/* SYST fails :(  try to get the type from the PWD command */
         get_ftp_pwd(&server_type, &use_list);
    }
    }

/*	Now we inform the server of the port number we will listen on
*/
#ifdef NOTREPEAT_PORT
	{
	    int status = response(port_command);
	    if (status !=2) {
	    	if (control->socket) close_connection(control->socket);
	        return -status;		/* Bad return */
	    }
	    if (TRACE) fprintf(stderr, "FTP: Port defined.\n");
	}
#endif
	return con->socket;			/* Good return */
  } /* Scope of con */
}


#ifdef LISTEN

/*	Close Master (listening) socket
**	-------------------------------
**
**
*/
#ifdef __STDC__
PRIVATE int close_master_socket(void)
#else
PRIVATE int close_master_socket()
#endif
{
    int status;
    FD_CLR(master_socket, &open_sockets);
    status = NETCLOSE(master_socket);
    if (TRACE) fprintf(stderr, "FTP: Closed master socket %d\n", master_socket);
    master_socket = -1;
    if (status<0) return HTInetStatus("close master socket");
    else return status;
}


/*	Open a master socket for listening on
**	-------------------------------------
**
**	When data is transferred, we open a port, and wait for the server to
**	connect with the data.
**
** On entry,
**	master_socket	Must be negative if not set up already.
** On exit,
**	Returns		socket number if good
**			less than zero if error.
**	master_socket	is socket number if good, else negative.
**	port_number	is valid if good.
*/
#ifdef __STDC__
PRIVATE int get_listen_socket(void)
#else
PRIVATE int get_listen_socket()
#endif
{
    struct sockaddr_in soc_address;	/* Binary network address */
    struct sockaddr_in* sin = &soc_address;
    int new_socket;			/* Will be master_socket */
    
    
    FD_ZERO(&open_sockets);	/* Clear our record of open sockets */
    num_sockets = 0;
    
#ifndef REPEAT_LISTEN
    if (master_socket>=0) return master_socket;  /* Done already */
#endif

/*  Create internet socket
*/
    new_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
    if (new_socket<0)
	return HTInetStatus("socket for master socket");
    
    if (TRACE) fprintf(stderr, "FTP: Opened master socket number %d\n", new_socket);
    
/*  Search for a free port.
*/
    sin->sin_family = AF_INET;	    /* Family = internet, host order  */
    sin->sin_addr.s_addr = INADDR_ANY; /* Any peer address */
#ifdef POLL_PORTS
    {
        unsigned short old_port_number = port_number;
	for(port_number=old_port_number+1;;port_number++){ 
	    int status;
	    if (port_number > LAST_TCP_PORT)
		port_number = FIRST_TCP_PORT;
	    if (port_number == old_port_number) {
		return HTInetStatus("bind");
	    }
	    soc_address.sin_port = htons(port_number);
	    if ((status=bind(new_socket,
		    (struct sockaddr*)&soc_address,
			    /* Cast to generic sockaddr */
		    sizeof(soc_address))) == 0)
		break;
	    if (TRACE) fprintf(stderr, 
	    	"TCP bind attempt to port %d yields %d, errno=%d\n",
		port_number, status, SOCKET_ERRNO);
	} /* for */
    }
#else
    {
        int status;
	int address_length = sizeof(soc_address);
	status = getsockname(control->socket,
			(struct sockaddr *)&soc_address,
			 &address_length);
	if (status<0) return HTInetStatus("getsockname");
	CTRACE(tfp, "FTP: This host is %s\n",
	    HTInetString(sin));
	
	soc_address.sin_port = 0;	/* Unspecified: please allocate */
	status=bind(new_socket,
		(struct sockaddr*)&soc_address,
			/* Cast to generic sockaddr */
		sizeof(soc_address));
	if (status<0) return HTInetStatus("bind");
	
	address_length = sizeof(soc_address);
	status = getsockname(new_socket,
			(struct sockaddr*)&soc_address,
			&address_length);
	if (status<0) return HTInetStatus("getsockname");
    }
#endif    

    CTRACE(tfp, "FTP: bound to port %d on %s\n",
    		(int)ntohs(sin->sin_port),
		HTInetString(sin));

#ifdef REPEAT_LISTEN
    if (master_socket>=0)
        (void) close_master_socket();
#endif    
    
    master_socket = new_socket;
    
/*	Now we must find out who we are to tell the other guy
*/
    (void)HTHostName(); 	/* Make address valid - doesn't work*/
    sprintf(port_command, "PORT %d,%d,%d,%d,%d,%d%c%c",
		    (int)*((unsigned char *)(&sin->sin_addr)+0),
		    (int)*((unsigned char *)(&sin->sin_addr)+1),
		    (int)*((unsigned char *)(&sin->sin_addr)+2),
		    (int)*((unsigned char *)(&sin->sin_addr)+3),
		    (int)*((unsigned char *)(&sin->sin_port)+0),
		    (int)*((unsigned char *)(&sin->sin_port)+1),
		    CR, LF);


/*	Inform TCP that we will accept connections
*/
    if (listen(master_socket, 1)<0) {
	master_socket = -1;
	return HTInetStatus("listen");
    }
    CTRACE(tfp, "TCP: Master socket(), bind() and listen() all OK\n");
    FD_SET(master_socket, &open_sockets);
    if ((master_socket+1) > num_sockets) num_sockets=master_socket+1;

    return master_socket;		/* Good */

} /* get_listen_socket */
#endif


/*	Retrieve File from Server
**	-------------------------
**
** On entry,
**	name		WWW address of a file: document, including hostname
** On exit,
**	returns		Socket number for file if good.
**			<0 if bad.
*/

#ifdef NEW_PARS
PUBLIC int HTFTPLoad ARG6 (HTRequest *, 		request,
			   void *,			param,
			   CONST char *,		name,
			   HTParentAnchor *,		anchor,
			   HTFormat,			format_out,
			   HTStream *,			sink)
#endif
PUBLIC int HTFTPLoad ARGS1(HTRequest *, request)
{
    char *name = HTAnchor_physical(request->anchor);
    BOOL isDirectory = NO;
    int status;
    int retry;			/* How many times tried? */
    HTFormat format;
    char command[LINE_LENGTH+1];
    

    /* set use_list to NOT since we don't know what kind of server
     * this is yet.  And set the type to GENERIC
     */
    use_list = FALSE;
    server_type = GENERIC_SERVER;

    for (retry=0; retry<2; retry++) {	/* For timed out/broken connections */
    
	status = get_connection(name);
	if (status<0) return status;

#ifdef LISTEN
	status = get_listen_socket();
	if (status<0) {
	    NETCLOSE (control->socket);
            control->socket = -1;
            close_master_socket ();
            /* HT_INTERRUPTED would fall through, if we could interrupt
               somehow in the middle of it, which we currently can't. */
	    return status;
	}
    
#ifdef REPEAT_PORT
/*	Inform the server of the port number we will listen on
*/
	{
	    status = response(port_command);
	    if (status == HT_INTERRUPTED) {
              if (TRACE)
                fprintf (stderr, "FTP: Interrupted in response (port_command)\n");
              HTProgress ("Connection interrupted.");
              NETCLOSE (control->socket);
              control->socket = -1;
              close_master_socket ();
              return HT_INTERRUPTED;
            }
	    if (status !=2) {		/* Could have timed out */
		if (status<0) continue;		/* try again - net error*/
		return -status;			/* bad reply */
	    }
	    if (TRACE) fprintf(stderr, "FTP: Port defined.\n");
	}
#endif
#else	/* Use PASV */
/*	Tell the server to be passive
*/
	{
	    char *p;
	    int reply, h0, h1, h2, h3, p0, p1;	/* Parts of reply */
	    int status;
	    data_soc = status;

	    sprintf(command, "PASV%c%c", CR, LF);
	    status = response(command);
	    if (status !=2) {
		if (status<0) continue;		/* retry or Bad return */
		return -status;			/* bad reply */
	    }
	    for(p=response_text; *p && *p != ','; p++)
		; /* null body */

	    while (--p > response_text && '0' <= *p && *p <= '9')
		; /* null body */
	
           status = sscanf(p+1, "%d,%d,%d,%d,%d,%d",
                   &h0, &h1, &h2, &h3, &p0, &p1);
           if (status<4) {
               fprintf(stderr, "FTP: PASV reply has no inet address!\n");
               return -99;
           }
           passive_port = (p0<<8) + p1;
	   if(TRACE)
               fprintf(stderr, "FTP: Server is listening on port %d\n",
                    passive_port);


/*	Open connection for data:
*/
	    sprintf(command,
            "ftp://%d.%d.%d.%d:%d/",h0,h1,h2,h3,passive_port);
	    
	    status = connect(data_soc, (struct sockaddr *) &soc_address,
			     sizeof(soc_address));
#ifdef NEW_CODE
            status = HTDoConnect(name, "FTP", passive_port, &data_soc);
#endif
	    if (status<0){
		(void) HTInetStatus("connect for data");
		NETCLOSE(data_soc);
		return status;			/* Bad return */
	    }
	    
	    if (TRACE) fprintf(stderr, "FTP data connected, socket %d\n", data_soc);
	}
#endif /* use PASV */
	status = 0;
        break;	/* No more retries */

    } /* for retries */
    if (status<0) return status;	/* Failed with this code */
    
/*	Ask for the file:
*/    
    {
        char *filename = HTParse(name, "", PARSE_PATH + PARSE_PUNCTUATION);
	char *fname = filename;	/** Save for subsequent free() **/
	BOOL binary = NO;
	HTAtom * encoding;

	if (!*filename) StrAllocCopy(filename, "/");
	HTUnEscape(filename);
	if (TRACE) fprintf(stderr, "FTP: UnEscaped %s\n", filename);
	format = HTFileFormat(filename,
			      &request->content_encoding,
			      &request->content_language);
	binary = (request->content_encoding != HTAtom_for("8bit")
		  && request->content_encoding != HTAtom_for("7bit"));
        if (binary != control->binary) {
	    char * mode = binary ? "I" : "A";
	    sprintf(command, "TYPE %s%c%c", mode, CR, LF);
	    status = response(command);
	    if (status != 2) return -status;
	    control->binary = binary;
	}
	if (server_type == VMS_SERVER) {
	    char *cp, *cp1, *cp2;
	    /** Accept only Unix-style filename **/
	    if (strchr(filename, ':') != NULL ||
	        strchr(filename, '[') != NULL) {
		free(fname);
		return -1;
	    }
	    /** Trim trailing slash if filename is not the top directory **/
	    if (strlen(filename) > 1 && filename[strlen(filename)-1] == '/')
	        filename[strlen(filename)-1] = '\0';

#ifdef MAINTAIN_CONNECTION /* Don't need this if always new connection - F.M. */
	    /** Get the current default VMS device:[directory] **/ 
	    sprintf(command, "PWD%c%c", CR, LF);
	    status = response (command);
	    if (status != 2) {
	         free(fname);
		 return -status;
	    }
	    /** Go to the VMS account's top directory **/
	    if ((cp=strchr(response_text, '[')) != NULL &&
	        (cp1=strrchr(response_text, ']')) != NULL) {
		sprintf(command, "CWD %s", cp);
		if ((cp2=strchr(cp, '.')) != NULL && cp2 < cp1)
		    sprintf(command+(cp2-cp)+4, "]%c%c", CR, LF);
		else
		    sprintf(command+(cp1-cp)+4, "]%c%c", CR, LF);
		status = response (command);
		if (status != 2) {
		    free(fname);
		    return -status;
		}
	    }
#endif /* MAINTAIN_CONNECTION */

	    /** If we want the VMS account's top directory, list it now **/
	    if (strlen(filename) == 1 && *filename == '/') {
		isDirectory = YES;
		sprintf(command, "LIST%c%c", CR, LF);
		status = response (command);
		free(fname);
		if (status != 1) return -status;  /* Action not started */

		goto listen;  /* big goto */
	    }
	    /** Otherwise, go to appropriate directory and doctor filename **/
	    if ((cp=strchr(filename, '/')) != NULL &&
	        (cp1=strrchr(cp, '/')) != NULL && cp != cp1) {
		sprintf(command, "CWD [.%s", cp+1);
		sprintf(command+(cp1-cp)+5, "]%c%c", CR, LF);
		while ((cp2=strrchr(command, '/')) != NULL)
		    *cp2 = '.';
		status = response(command);
		if (status != 2) {
		    free(fname);
		    return -status;
		}
		filename = cp1+1;
	    }
	    else
	        filename += 1;
	}
	sprintf(command, "RETR %s%c%c", filename, CR, LF);
	status = response(command);
	if (status != 1) {  /* Failed : try to CWD to it */

	   /* save those handy help messages */
#if 0
	  if(server_type == UNIX_SERVER)
	      init_help_message_cache();
#endif
	  sprintf(command, "CWD %s%c%c", filename, CR, LF);
	  status = response(command);

	  if (status == 2) {  /* Successed : let's NAME LIST it */
	    isDirectory = YES;
	    if(use_list)
	        sprintf(command, "LIST%c%c", CR, LF);
	    else
	        sprintf(command, "NLST%c%c", CR, LF);
	    status = response (command);
	  }
	}
	free(fname);
	if (status != 1) return -status;		/* Action not started */
    }

listen:
#ifdef LISTEN
/*	Wait for the connection
*/
    {
	struct sockaddr_in soc_address;
        int	soc_addrlen=sizeof(soc_address);
	status = accept(master_socket,
			(struct sockaddr *)&soc_address,
			&soc_addrlen);
	if (status<0)
	    return HTInetStatus("accept");
	CTRACE(tfp, "TCP: Accepted new socket %d\n", status);
	data_soc = status;
    }
#else
/* @@ */
#endif
    if (isDirectory) {
#ifdef OLD_CODE
        status = read_directory (anchor, name, format_out, sink);
#endif
        char *filename = HTParse(name, "", PARSE_PATH + PARSE_PUNCTUATION);
	HTUnEscape(filename);
        if (TRACE)
	    fprintf(stderr, "name: %s\n", filename);
        status = HTFTPBrowseDirectory(request, filename,
				      (HTDirLineInput) HTFTP_get_dir_string);
        free(filename);
#if 0
        return response(NULL) == 2 ? HT_LOADED : -1;
#endif
	NETCLOSE(data_soc);
	NETCLOSE(control->socket);
        return status;
      /* returns HT_LOADED or error */
    } else {
        int rv;

	HTProgress ("Receiving FTP file.");
	rv = HTParseSocket(format, data_soc, request);

	if (rv == HT_INTERRUPTED)
	     HTProgress("Data transfer interrupted.");

#ifdef NEW_CODE
	HTInitInput(control->socket);
#endif
	/* Reset buffering to control connection DD 921208 */
    
	status = NETCLOSE(data_soc);
	if (TRACE) fprintf(stderr, "FTP: Closing data socket %d\n", data_soc);
	if (status<0 && rv != HT_INTERRUPTED && rv != -1)
	    (void) HTInetStatus("close");	/* Comment only */
	data_soc = -1;	/* invalidate it */
	
	status = response(NULL);		/* Pick up final reply */
	if (status!=2 && rv != HT_INTERRUPTED && rv != -1)
	    return HTLoadError(request, 500, response_text);

	NETCLOSE(control->socket);
	return HT_LOADED;
    }       
} /* open_file_read */

