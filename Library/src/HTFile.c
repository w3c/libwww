/*								       HTFile.c
**	FILE ACCESS
**
**	(c) COPYRIGHT CERN 1994.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	This is unix-specific code in general, with some VMS bits.
**	These are routines for file access used by browsers.
**
** History:
**	   Feb 91	Written Tim Berners-Lee CERN/CN
**	   Apr 91	vms-vms access included using DECnet syntax
**	26 Jun 92 (JFG) When running over DECnet, suppressed FTP.
**			Fixed access bug for relative names on VMS.
**	   Sep 93 (MD)  Access to VMS files allows sharing.
**	15 Nov 93 (MD)	Moved HTVMSname to HTVMSUTILS.C
**	22 Feb 94 (MD)  Excluded two routines if we are not READING directories
**	18 May 94 (HF)	Directory stuff removed and stream handling updated,
**			error messages introduced etc.
**
** Bugs:
**	FTP: Cannot access VMS files from a unix machine.
**      How can we know that the
**	target machine runs VMS?
*/

/* Library Includes */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTParse.h"
#include "HTTCP.h"
#include "HTAnchor.h"
#include "HTAtom.h"
#include "HTWriter.h"
#include "HTFWrite.h"
#include "HTInit.h"
#include "HTBTree.h"
#include "HTFormat.h"
#include "HTMulti.h"
#include "HTDirBrw.h"
#include "HTBind.h"
#include "HTError.h"
#include "HTFile.h"		/* Implemented here */

typedef struct _file_info {
    SOCKFD		sockfd;				/* Socket descripter */
    SockA 		sock_addr;		/* SockA is defined in tcp.h */
    HTInputSocket *	isoc;				     /* Input buffer */
    SocAction		action;			/* Result of the select call */
    HTStream *		target;				    /* Target stream */
    int 		addressCount;	     /* Attempts if multi-homed host */
    time_t		connecttime;		 /* Used on multihomed hosts */
    struct _HTRequest *	request;	   /* Link back to request structure */
} file_info;

/* Controlling globals */
PUBLIC BOOL HTTakeBackup = YES;

PRIVATE char *HTMountRoot = "/Net/";		/* Where to find mounts */

/* ------------------------------------------------------------------------- */

/*	Convert filenames between local and WWW formats
**	-----------------------------------------------
**	Make up a suitable name for saving the node in
**
**	E.g.	$(HOME)/WWW/news/1234@cernvax.cern.ch
**		$(HOME)/WWW/http/crnvmc/FIND/xx.xxx.xx
**
** On exit,
**	returns	a malloc'ed string which must be freed by the caller.
**
**	BUG: FILENAME IS NOT UNESCAPED!!!!!!
**
*/
PRIVATE char * HTLocalName ARGS1(CONST char *,name)
{
    char * access = HTParse(name, "", PARSE_ACCESS);
    char * host = HTParse(name, "", PARSE_HOST);
    char * path = HTParse(name, "", PARSE_PATH+PARSE_PUNCTUATION);
    
    HTUnEscape(path);	/* Interpret % signs */

    /*
     * CERN httpd has it's own enhanced rule system which has already
     * done the mappings, and the local filename ("physical address")
     * is stored in HTImServer.
     */
    if (HTImServer && !*access) {
	free(access);
	free(host);
	StrAllocCopy(path, HTImServer);
	HTUnEscape(path);

#if VMS
        HTVMS_checkDecnet(path);
#endif /* VMS */
#ifdef _WINDOWS
	/* an absolute pathname with logical drive */
	if (*path == '/' && path[2] == ':')
	    /* NB need memmove because overlaps */
	    memmove(path, path+1, strlen(path) + 1) ; 
#endif
	if (PROT_TRACE)
	    fprintf(TDEST, "Local filename is \"%s\"\n", path);
	return(path);
    }

    if (0==strcmp(access, "file")) { /* local file */
        free(access);	
	if ((0==strcasecomp(host, HTGetHostName())) ||
	    (0==strcasecomp(host, "localhost")) || !*host) {
	    free(host);

#if VMS
        HTVMS_checkDecnet(path);
#endif /* VMS */
#ifdef _WINDOWS 
	    /* an absolute pathname with logical drive */
	    if (*path == '/' && path[2] == ':')    
		/* NB. need memmove because overlaps */
		memmove( path, path+1, strlen(path) + 1 );
#endif
	    if (TRACE)
		fprintf(TDEST, "Node........ `%s' means path `%s'\n",name,path);
	    return(path);
	} else {
	    char * result = (char *)malloc(
	    			strlen("/Net/")+strlen(host)+strlen(path)+1);
              if (result == NULL) outofmem(__FILE__, "HTLocalName");
	    sprintf(result, "%s%s%s", "/Net/", host, path);
	    free(host);
	    free(path);
	    if (TRACE) fprintf(TDEST, "Node........ `%s' means file `%s'\n", name, result);
	    return result;
	}
    } else {  /* other access */
	char * result;
        CONST char * home =  (CONST char*)getenv("HOME");
	if (!home) home = "/tmp";
	result = (char *)malloc(
		strlen(home)+strlen(access)+strlen(host)+strlen(path)+6+1);
	if (result == NULL) outofmem(__FILE__, "HTLocalName");
	sprintf(result, "%s/WWW/%s/%s%s", home, access, host, path);
	free(path);
	free(access);
	free(host);
	return result;
    }
}


/*	Make a WWW name from a full local path name
**
** Bugs:
**	At present, only the names of two network root nodes are hand-coded
**	in and valid for the NeXT only. This should be configurable in
**	the general case.
*/

PUBLIC char * WWW_nameOfFile ARGS1 (CONST char *,name)
{
    char * result;
#ifdef NeXT
    if (0==strncmp("/private/Net/", name, 13)) {
	result = (char *)malloc(7+strlen(name+13)+1);
	if (result == NULL) outofmem(__FILE__, "WWW_nameOfFile");
	sprintf(result, "file://%s", name+13);
    } else
#endif
    if (0==strncmp(HTMountRoot, name, 5)) {
	result = (char *)malloc(7+strlen(name+5)+1);
	if (result == NULL) outofmem(__FILE__, "WWW_nameOfFile");
	sprintf(result, "file://%s", name+5);
    } else {
        result = (char *)malloc(7+strlen(HTGetHostName())+strlen(name)+1);
	if (result == NULL) outofmem(__FILE__, "WWW_nameOfFile");
	sprintf(result, "file://%s%s", HTGetHostName(), name);
    }
    if (TRACE) fprintf(TDEST, "File `%s'\n\tmeans node `%s'\n", name, result);
    return result;
}


/*	Determine write access to a file
**	--------------------------------
**
** On exit,
**	return value	YES if file can be accessed and can be written to.
**
** Bugs:
**	1.	No code for non-unix systems.
**	2.	Isn't there a quicker way?
*/
#ifndef NO_UNIX_IO
PUBLIC BOOL HTEditable ARGS1 (CONST char *,filename)
{
#ifdef NO_GROUPS
    return NO;		/* Safe answer till we find the correct algorithm */
#else
#ifdef NeXT
    int		groups[NGROUPS];
#else
    gid_t 	groups[NGROUPS];
#endif	
    uid_t	myUid;
    int		ngroups;			/* The number of groups  */
    struct stat	fileStatus;
    int		i;
        
    if (HTStat(filename, &fileStatus))		/* Get details of filename */
    	return NO;				/* Can't even access file! */

    ngroups = getgroups(NGROUPS, groups);	/* Groups to which I belong  */
    myUid = geteuid();				/* Get my user identifier */

    if (TRACE) {
        int i;
	fprintf(TDEST, 
	    "File mode is 0%o, uid=%d, gid=%d. My uid=%d, %d groups (",
    	    (unsigned int) fileStatus.st_mode, (int) fileStatus.st_uid,
	    (int) fileStatus.st_gid, (int) myUid, ngroups);
	for (i=0; i<ngroups; i++) fprintf(TDEST, " %d", (int) groups[i]);
	fprintf(TDEST, ")\n");
    }
    
    if (fileStatus.st_mode & 0002)		/* I can write anyway? */
    	return YES;
	
    if ((fileStatus.st_mode & 0200)		/* I can write my own file? */
     && (fileStatus.st_uid == myUid))
    	return YES;

    if (fileStatus.st_mode & 0020)		/* Group I am in can write? */
    {
   	for (i=0; i<ngroups; i++) {
            if (groups[i] == fileStatus.st_gid)
	        return YES;
	}
    }
    if (TRACE) fprintf(TDEST, "\tFile is not editable.\n");
    return NO;					/* If no excuse, can't do */
#endif
}
#endif /* NO_UNIX_IO */

/*	Make a save stream
**	------------------
**
**	The stream must be used for writing back the file.
**	@@@ no backup done
*/
PUBLIC HTStream * HTFileSaveStream ARGS1(HTRequest *, request)
{

    CONST char * addr = HTAnchor_address((HTAnchor*)request->anchor);
    char *  filename = HTLocalName(addr);
    FILE* fp;

/*	@ Introduce CVS handling here one day
*/
/*	Take a backup before we do anything silly   931205
*/        
    if (HTTakeBackup) {
	char * p;
    	char * backup_filename = (char *) malloc(strlen(filename)+2);
	if (!backup_filename) outofmem(__FILE__, "taking backup");
	strcpy(backup_filename, filename);
	for(p=backup_filename+strlen(backup_filename);; p--) {
	    if ((*p=='/') || (p<backup_filename)) {
	        p[1]=',';		/* Insert comma after slash */
		break;
	    }
	    p[1] = p[0];	/* Move up everything to the right of it */
	}
	

#ifdef VMS
	if ((fp=fopen(filename, "r", "shr=put", "shr=upd"))) {	/* File exists */
#else /* not VMS */
	if ((fp=fopen(filename, "r"))) {		/* File exists */
#endif /* not VMS */
	    fclose(fp);
	    if (TRACE) fprintf(TDEST, "File `%s' exists\n", filename);
	    if (REMOVE_FILE(backup_filename)) {
		if (TRACE) fprintf(TDEST, "Backup file `%s' removed\n",
				   backup_filename);
	    }
	    if (rename(filename, backup_filename)) {	/* != 0 => Failure */
		if (TRACE) fprintf(TDEST, "Rename `%s' to `%s' FAILED!\n",
				   filename, backup_filename);
	    } else {					/* Success */
		if (TRACE) fprintf(TDEST, "Renamed `%s' to `%s'\n", filename,
				   backup_filename);
	    }
	}
    	free(backup_filename);
    } /* if take backup */    
    
    if ((fp = fopen(filename, "w")) == NULL) {
	HTErrorSysAdd(request, ERR_FATAL, errno, NO, "fopen");
	return NULL;
    } else
    	return HTFWriter_new(fp, NO);
}


/*	Load a document
**	---------------
**
** On entry,
**	addr		must point to the fully qualified hypertext reference.
**			This is the physsical address of the file
**
** On exit,
**	returns		HT_ERROR	Error has occured or interrupted
**			HT_LOADED	if file has been loaded successfully
**			HT_WOULD_BLOCK  We are using blocking I/O so this
**					indicates that we have a read a chunk
**					of data and now want to see what's up
**					in the eventloop.
*/
PUBLIC int HTLoadFile ARGS1 (HTRequest *, request)
{
    int status = -1;
    char *url;

    if (!request || !request->anchor) {
	if (TRACE) fprintf(TDEST, "HTLoadFile.. Called with bad argument\n");
	return HT_ERROR;
    }
    url = HTAnchor_physical(request->anchor);
    if (PROT_TRACE)
	fprintf(TDEST, "LoadFile.... Looking for `%s\'\n", url);
    HTBind_getBindings(request->anchor);     /* Get type, encoding, language */

/*	For unix, we try to translate the name into the name of a transparently
**	mounted file.
**
**	Not allowed in secure (HTClientHost) situations TBL 921019
*/
#ifndef NO_UNIX_IO
    /*  Need protection here for telnet server but not httpd server */
	 
    if (!HTSecure) {
	char * localname = HTLocalName(url);		   /* Does unescape! */
	struct stat stat_info;
	char * multi;

	if (TRACE)
	    fprintf(TDEST, "HTLoadFile.. Accessing local file system.\n");


/*	Multiformat handling. If suffix matches MULTI_SUFFIX then scan
**	directory to find a good file.
**
**	Bug:  we don't stat the file to find the length
*/
	multi = strrchr(localname, MULTI_SUFFIX[0]);
	if (multi && !strcmp(multi, MULTI_SUFFIX)) {
	    char * new_path = HTMulti(request, localname, &stat_info);
	    if (new_path) {
		FREE(localname);
		localname = new_path;
		HTAnchor_setPhysical(request->anchor, localname);
		goto open_file;
	    }
	    else {			       /* If not found suitable file */
		FREE(localname);
		goto cleanup;
	    }
	}

#ifdef GOT_READ_DIR
/*	Check to see if the 'localname' is in fact a directory.  If it is
**	create a new hypertext object containing a list of files and 
**	subdirectories contained in the directory.  All of these are links
**      to the directories or files listed.
*/
	if (HTStat(localname, &stat_info) == -1) {
	    if (TRACE)
		fprintf(TDEST, "HTLoadFile.. Can't stat %s\n", localname);
	} else {
	    if (((stat_info.st_mode) & S_IFMT) == S_IFDIR) {
		status = HTBrowseDirectory(request, localname);
		FREE(localname);
		goto cleanup;
	    }
	}

#endif /* GOT_READ_DIR */

open_file:
	{
#ifdef VMS
	    FILE * fp = fopen(localname,"r","shr=put","shr=upd");
#else /* not VMS */
	    FILE * fp = fopen(localname,"r");
#endif /* not VMS */

	    if (fp) {
		if(TRACE) fprintf(TDEST, "HTLoadFile.. Opened `%s' on local file system\n", localname);
		if (HTEditable(localname))
		    request->anchor->methods += METHOD_PUT;
		status = HTParseFile(request->anchor->content_type, fp, request);
		fclose(fp);

		FREE(localname);
		goto cleanup;
	    } else
		HTErrorSysAdd(request, ERR_FATAL, errno, NO, "fopen");
	}
	FREE(localname);
    } /* End of local file system */
#endif /* NO_UNIX_IO */

#ifndef DECNET
    /* Now, as transparently mounted access has failed, we try FTP. */
    {
	char *nodename = HTParse(url, "", PARSE_HOST);
	CONST char *hostname = HTGetHostName();
	if (nodename && *nodename && hostname &&
	    strcmp(nodename, hostname) && strcmp(nodename, "localhost")) {
	    HTAnchor *anchor;
	    char *newname = NULL;
	    char *unescaped = NULL;
	    StrAllocCopy(unescaped, url);
	    HTUnEscape(unescaped);
	    HTErrorAdd(request, ERR_WARN, NO, HTERR_FILE_TO_FTP,
		       (void *) unescaped,
		       (int) strlen(unescaped), "HTLoadFile");
	    StrAllocCopy(newname, "ftp:");
	    if (!strncmp(url, "file:", 5))
		StrAllocCat(newname, url+5);
	    else
		StrAllocCat(newname, url);
	    anchor = HTAnchor_findAddress(newname);
	    free(newname);
	    free(nodename);
	    free(unescaped);
	    return HTLoadAnchorRecursive(anchor, request);
	}
	free(nodename);
    }
#endif

  cleanup:
    return status;
}

/* Protocol descriptor */
GLOBALDEF PUBLIC HTProtocol HTFile = {
    "file", SOC_BLOCK, HTLoadFile, HTFileSaveStream, NULL
};
