/*								       HTFile.c
**	FILE ACCESS
**
**	(c) COPYRIGHT MIT 1995.
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
#include "HTFormat.h"
#include "HTMulti.h"
#include "HTDirBrw.h"
#include "HTBind.h"
#include "HTSocket.h"
#include "HTThread.h"
#include "HTError.h"
#include "HTFile.h"		/* Implemented here */

/* This is the local definition of HTRequest->net_info */
typedef enum _FileState {
    FS_FILE_RETRY		= -4,
    FS_FILE_ERROR		= -3,
    FS_FILE_NO_DATA	= -2,
    FS_FILE_GOT_DATA	= -1,
    FS_FILE_BEGIN		= 0,
    FS_FILE_NEED_OPEN_FILE,
    FS_FILE_NEED_TARGET,
    FS_FILE_NEED_BODY,
    FS_FILE_PARSE_DIR,
    FS_FILE_TRY_FTP
} FileState;

typedef struct _file_info {
    SOCKFD		sockfd;				/* Socket descripter */
    SockA 		sock_addr;		/* SockA is defined in tcp.h */
    HTInputSocket *	isoc;				     /* Input buffer */
    SocAction		action;			/* Result of the select call */
    HTStream *		target;				    /* Target stream */
    int 		addressCount;	     /* Attempts if multi-homed host */
    time_t		connecttime;		 /* Used on multihomed hosts */
    struct _HTRequest *	request;	   /* Link back to request structure */

    FileState		state;		  /* Current state of the connection */
    char *		localname;	/* Local representation of file name */
    FILE *		fp;        /* If we can't use sockets on local files */
} file_info;

/* Local definition */
struct _HTStream {
    CONST HTStreamClass *	isa;
    HTStream *		  	target;
};

/* Controlling globals */
PUBLIC BOOL HTTakeBackup = YES;

PRIVATE char *HTMountRoot = "/Net/";		/* Where to find mounts */

/* ------------------------------------------------------------------------- */

/*	Convert file URLs into a local representation
**	---------------------------------------------
**	The URL has already been translated through the rules in get_physical
**	in HTAccess.c and all we need to do now is to map the path to a local
**	representation, for example if must translate '/' to the ones that
**	turn the wrong way ;-)
**
** On Entry
** 	The full URL that's is going to be translated
** On Exit,
**	-1	Error
**	 0	URL is not pointing to the local file system
**	 1	A local name is returned in `local' which must be freed
**		by caller
*/
PRIVATE int HTLocalName ARGS2(CONST char *, url, char **, local)
{
    if (url) {
	char * access = HTParse(url, "", PARSE_ACCESS);
	char * host = HTParse(url, "", PARSE_HOST);
	char * path = HTParse(url, "", PARSE_PATH+PARSE_PUNCTUATION);
	CONST char *myhost;
	
	/* Find out if this is a reference to the local file system */
	if ((*access && strcmp(access, "file")) ||
	    (*host && strcasecomp(host, "localhost") &&
	     (myhost=HTGetHostName()) && strcmp(host, myhost))) {
	    if (PROT_TRACE)
		fprintf(TDEST, "LocalName... Not on local file system\n");
	    free(access);
	    free(host);
	    free(path);
	    return 0;
	} else {
	    /*
	     ** Do whatever translation is required here in order to fit your
	     ** platform _before_ the path is unescaped.
	     */
#if VMS
	    HTVMS_checkDecnet(path);
#endif
#ifdef _WINDOWS
	    /* an absolute pathname with logical drive */
	    if (*path == '/' && path[2] == ':')    
		/* NB. need memmove because overlaps */
		memmove( path, path+1, strlen(path) + 1);
#endif
	    
	    HTUnEscape(path);		  /* Take out the escaped characters */
	    if (PROT_TRACE)
		fprintf(TDEST, "Node........ `%s' means path `%s'\n",url,path);
	    free(access);
	    free(host);
	    *local = path;
	    return 1;
	}
    }
    return -1;
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
**	If stat_info is NULL then the function calls stat() on it's own,
**	otherwise it uses the information found in stat_info
** On exit,
**	return value	YES if file can be accessed and can be written to.
**
** Bugs:
**	1.	No code for non-unix systems.
**	2.	Isn't there a quicker way?
*/
PUBLIC BOOL HTEditable ARGS2(CONST char *, filename, struct stat *, stat_info)
{
#ifndef NO_UNIX_IO
#ifdef NO_GROUPS
    return NO;		   /* Safe answer till we find the correct algorithm */
#else
#ifdef NeXT
    int groups[NGROUPS];
#else
    gid_t groups[NGROUPS];
#endif	
    int i;
    uid_t myUid;
    int	ngroups;			/* The number of groups  */
    struct stat	fileStatus;
    struct stat *fileptr = stat_info ? stat_info : &fileStatus;
        
    if (!stat_info) {
	if (HTStat(filename, &fileStatus))
	    return NO;				  /* Can't even access file! */
    }
    ngroups = getgroups(NGROUPS, groups);	/* Groups to which I belong  */
    myUid = geteuid();				/* Get my user identifier */

    if (TRACE) {
        int i;
	fprintf(TDEST, 
	    "File mode is 0%o, uid=%d, gid=%d. My uid=%d, %d groups (",
    	    (unsigned int) fileptr->st_mode, (int) fileptr->st_uid,
	    (int) fileptr->st_gid, (int) myUid, ngroups);
	for (i=0; i<ngroups; i++) fprintf(TDEST, " %d", (int) groups[i]);
	fprintf(TDEST, ")\n");
    }
    
    if (fileptr->st_mode & 0002)		/* I can write anyway? */
    	return YES;
	
    if ((fileptr->st_mode & 0200)		/* I can write my own file? */
     && (fileptr->st_uid == myUid))
    	return YES;

    if (fileptr->st_mode & 0020)		/* Group I am in can write? */
    {
   	for (i=0; i<ngroups; i++) {
            if (groups[i] == fileptr->st_gid)
	        return YES;
	}
    }
    if (TRACE) fprintf(TDEST, "\tFile is not editable.\n");
    return NO;					/* If no excuse, can't do */
#endif
#else
    /*
    ** We don't know and can't find out. Can we be sure that when opening
    ** a file in mode "a" that the file is not modified?
    */
    return NO;
#endif /* !NO_UNIX_IO */
}


/*	Make a save stream
**	------------------
**
**	The stream must be used for writing back the file.
**	@@@ no backup done
*/
PUBLIC HTStream * HTFileSaveStream ARGS1(HTRequest *, request)
{

    CONST char * addr = HTAnchor_address((HTAnchor*)request->anchor);
    char *  filename;
    FILE* fp;
    HTLocalName(addr, &filename);

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
	    if (REMOVE(backup_filename)) {
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


/*                                                                  FileCleanup
**
**      This function closes the connection and frees memory.
**
**      Returns 0 on OK, else -1
*/
PRIVATE int FileCleanup ARGS2(HTRequest *, req, BOOL, abort)
{
    file_info *file;
    int status = 0;
    if (!req || !req->net_info) {
	if (PROT_TRACE) fprintf(TDEST, "FileCleanup. Bad argument!\n");
	status = -1;
    } else {
	file = (file_info *) req->net_info;
	if (file->sockfd != INVSOC || file->fp) {

	    if (file->target) {

		/* Free stream with data FROM local file system */
		if (abort)
		    (*file->target->isa->abort)(file->target, NULL);
		else
		    (*file->target->isa->_free)(file->target);
	    }

#ifndef NO_UNIX_IO
	    if (PROT_TRACE)
		fprintf(TDEST,"FILE........ Closing socket %d\n",file->sockfd);
	    if ((status = NETCLOSE(file->sockfd)) < 0)
		HTErrorSysAdd(file->request, ERR_FATAL, socerrno, NO,
			      "NETCLOSE");
	    HTThreadState(file->sockfd, THD_CLOSE);
	    file->sockfd = INVSOC;
#else
	    if (PROT_TRACE)
		fprintf(TDEST,"FILE........ Closing file %p\n", file->fp);
	    fclose(file->fp);
	    file->fp = NULL;
#endif
	}
	    HTThread_clear((HTNetInfo *) file);
	if (file->isoc)
	    HTInputSocket_free(file->isoc);
	FREE(file->localname);
	free(file);
	req->net_info = NULL;
    }
    return status;
}


/*	Load a document
**	---------------
**
** On entry,
**	request		This is the request structure
** On exit,
**	returns		HT_ERROR	Error has occured or interrupted
**			HT_WOULD_BLOCK  We are using blocking I/O so this
**					indicates that we have a read a chunk
**					of data and now want to see what's up
**					in the eventloop.
**			HT_LOADED	if file has been loaded successfully
**			HT_NO_DATA	if file is empty
**			HT_RETRY	if file service is unavailable
*/
PUBLIC int HTLoadFile ARGS1 (HTRequest *, request)
{
    int status = HT_ERROR;
    file_info *file;			      /* Specific access information */

    if (!request || !request->anchor) {
	if (TRACE) fprintf(TDEST, "HTLoadFile.. Called with bad argument\n");
	return HT_ERROR;
    }

    /* Only do the setup first time through. This is actually state FILE_BEGIN
       but it can't be in the state machine as we need the structure first */
    if (!request->net_info) {    
	if (PROT_TRACE) {
	    char *url = HTAnchor_physical(request->anchor);
	    fprintf(TDEST, "LoadFile.... Looking for `%s\'\n", url);
	}
	if ((file = (file_info *) calloc(1, sizeof(file_info))) == NULL)
	    outofmem(__FILE__, "HTLoadFILE");
	file->sockfd = INVSOC;			    /* Invalid socket number */
	file->request = request;
	file->state = FS_FILE_BEGIN;
	request->net_info = (HTNetInfo *) file;
	HTThread_new((HTNetInfo *) file);
    } else {
	file = (file_info *) request->net_info;		/* Get existing copy */

	/* @@@ NEED ALSO TO CHECK FOR ANSI FILE DESCRIPTORS @@@ */
	if (HTThreadIntr(file->sockfd)) {
	    FileCleanup(request, YES);
	    return HTRequest_isMainDestination(request) ? HT_ERROR : HT_OK;
	}
    }

    /* Now jump into the machine. We know the state from the previous run */
    while (1) {
	switch (file->state) {
	  case FS_FILE_BEGIN:
	    if (HTSecure) {
		if (PROT_TRACE)
		    fprintf(TDEST, "LoadFile.... No access to local file system\n");
		file->state = FS_FILE_TRY_FTP;
		break;
	    }
	    if ((status = HTLocalName(HTAnchor_physical(request->anchor),
				      &file->localname)) == -1) {
		file->state = FS_FILE_ERROR;
		break;
	    } else if (status == 0) {
		file->state = FS_FILE_TRY_FTP;
		break;
	    }
	    /*
	    ** If we have to do content negotiation then find the object that
	    ** fits best into either what the client has indicated in the
	    ** accept headers or what the client has registered on it own.
	    ** The object chosen can in fact be a directory! However, content
	    ** negotiation only makes sense it we can read the directory!
	    ** We stat the file in order to find the size and to see it if
	    ** exists.
	    */
	    {
		struct stat stat_info;	      /* Contains actual file chosen */
		if (request->ContentNegotiation) {
		    char *new_path=HTMulti(request,file->localname,&stat_info);
		    if (new_path) {
			FREE(file->localname);
			file->localname = new_path;
			HTAnchor_setPhysical(request->anchor, new_path);
		    } else {
			file->state = FS_FILE_ERROR;
			break;
		    }
		} else {
		    if (HTStat(file->localname, &stat_info) == -1) {
			if (PROT_TRACE)
			    fprintf(TDEST, "HTLoadFile.. Can't stat %s\n",
				    file->localname);
			HTErrorAdd(request, ERR_FATAL, NO, HTERR_NOT_FOUND,
				   NULL, 0, "HTLoadFile");
			file->state = FS_FILE_ERROR;
			break;
		    }
		}
		/* Check to see if the 'localname' is in fact a directory */
		if (((stat_info.st_mode) & S_IFMT) == S_IFDIR)
		    file->state = FS_FILE_PARSE_DIR;
		else {
		    /*
		    ** If empty file then only serve it if it is editable
		    */
		    BOOL editable = HTEditable(file->localname, &stat_info);
		    HTBind_getBindings(request->anchor);
		    if (editable)
			HTAnchor_appendMethods(request->anchor, METHOD_PUT);
		    if (stat_info.st_size)
			HTAnchor_setLength(request->anchor, stat_info.st_size);

		    /* Done with relevant metainformation in anchor */
		    HTAnchor_setHeaderParsed(request->anchor);

		    if (!editable && !stat_info.st_size) {
			HTErrorAdd(request, ERR_FATAL, NO, HTERR_NO_CONTENT,
				   NULL, 0, "HTLoadFile");
			file->state = FS_FILE_NO_DATA;
		    } else
			file->state = FS_FILE_NEED_OPEN_FILE;
		}
	    }
	    break;

	  case FS_FILE_NEED_OPEN_FILE:
	    /*
	    ** If we have unix file descriptors then use this otherwise use
	    ** the ANSI C file descriptors
	    */
#ifndef NO_UNIX_IO
	    if ((file->sockfd = open(file->localname, O_RDONLY)) == -1) {
		HTErrorSysAdd(request, ERR_FATAL, errno, NO, "open");
		file->state = FS_FILE_ERROR;
		break;
	    }
	    if (PROT_TRACE)
		fprintf(TDEST,"HTLoadFile.. `%s' opened using fd %d \n",
			file->localname, file->sockfd);

	    /* If non-blocking protocol then change socket status
	    ** I use FCNTL so that I can ask the status before I set it.
	    ** See W. Richard Stevens (Advan. Prog. in UNIX env, p.364)
	    ** Be CAREFULL with the old `O_NDELAY' - it wont work as read()
	    ** returns 0 when blocking and NOT -1. FNDELAY is ONLY for BSD
	    ** and does NOT work on SVR4 systems. O_NONBLOCK is POSIX.
	    */
#ifndef NO_FCNTL
	    if (!HTProtocol_isBlocking(request)) {
		if ((status = FCNTL(file->sockfd, F_GETFL, 0)) != -1) {
		    status |= O_NONBLOCK;			    /* POSIX */
		    status = FCNTL(file->sockfd, F_SETFL, status);
		}
		if (PROT_TRACE) {
		    if (status == -1)
			fprintf(TDEST, "HTLoadFile.. Can't make socket non-blocking\n");
		    else
			fprintf(TDEST,"HTLoadFile.. Using NON_BLOCKING I/O\n");
		}
	    }
#endif /* NO_FCNTL */
#else
#ifdef VMS	
	    if (!(file->fp = fopen(file->localname,"r","shr=put","shr=upd"))) {
#else
	    if ((file->fp = fopen(file->localname,"r")) == NULL) {
#endif /* !VMS */
		HTErrorSysAdd(request, ERR_FATAL, errno, NO, "fopen");
		file->state = FS_FILE_ERROR;
		break;
	    }
	    if (PROT_TRACE)
		fprintf(TDEST,"HTLoadFile.. `%s' opened using FILE %p\n",
			file->localname, file->fp);
#endif /* !NO_UNIX_IO */
	    file->state = FS_FILE_NEED_TARGET;
	    break;

	  case FS_FILE_NEED_TARGET:
	    /*
	    ** We need to wait for the destinations to get ready
	    */
	    if (HTRequest_isSource(request) && !request->output_stream)
		return HT_WOULD_BLOCK;
	    /*
	    ** Set up concurrent read/write if this request isn't the
	    ** source for a PUT or POST. As source we don't start reading
	    ** before all destinations are ready. If destination then
	    ** register the input stream and get ready for read
	    */
	    if (HTRequest_isPostWeb(request)) {
		HTThreadState(file->sockfd, THD_SET_READ);
		HTRequest_linkDestination(request);
	    }

	    /* Set up read buffer and streams. If ANSI then sockfd=INVSOC */
	    file->isoc = HTInputSocket_new(file->sockfd);
	    file->target = HTStreamStack(HTAnchor_format(request->anchor),
					 request->output_format,
					 request->output_stream, request, YES);
	    file->state = file->target ? FS_FILE_NEED_BODY : FS_FILE_ERROR;
	    break;

	  case FS_FILE_NEED_BODY:
#ifndef NO_UNIX_IO
	    status = HTSocketRead(request, file->target);
#else
	    status = HTFileRead(file->fp, request, file->target);
#endif
	    if (status == HT_WOULD_BLOCK)
		return HT_WOULD_BLOCK;
	    else if (status == HT_INTERRUPTED)
		file->state = FS_FILE_ERROR;
	    else if (status == HT_LOADED) {
		file->state = FS_FILE_GOT_DATA;
	    } else
		file->state = FS_FILE_ERROR;
	    break;

	  case FS_FILE_PARSE_DIR:
#ifdef GOT_READ_DIR
	    file->state = HTBrowseDirectory(request, file->localname) < 0 ?
		FS_FILE_ERROR : FS_FILE_GOT_DATA;
#else
	    file->state = FS_FILE_ERROR;
#endif
	    break;

	  case FS_FILE_TRY_FTP:
	    {
		char *url = HTAnchor_physical(request->anchor);
		HTAnchor *anchor;
		char *newname = NULL;
		StrAllocCopy(newname, "ftp:");
		if (!strncmp(url, "file:", 5))
		    StrAllocCat(newname, url+5);
		else
		    StrAllocCat(newname, url);
		anchor = HTAnchor_findAddress(newname);
		free(newname);
		FileCleanup(request, NO);
		return HTLoadAnchorRecursive(anchor, request);
	    }
	    break;

	  case FS_FILE_GOT_DATA:
	    FileCleanup(request, NO);
	    if (HTRequest_isPostWeb(request)) {
		BOOL main = HTRequest_isMainDestination(request);
		HTRequest_removeDestination(request);
		return main ? HT_LOADED : HT_OK;
	    }
	    return HT_LOADED;
	    break;

	  case FS_FILE_NO_DATA:
	    FileCleanup(request, NO);
	    if (HTRequest_isPostWeb(request)) {
		BOOL main = HTRequest_isMainDestination(request);
		HTRequest_removeDestination(request);
		return main ? HT_NO_DATA : HT_OK;
	    }
	    return HT_NO_DATA;
	    break;

	  case FS_FILE_RETRY:
	    if (HTRequest_isPostWeb(request))
		HTRequest_killPostWeb(request);
	    else
		FileCleanup(request, YES);
	    if (HTRequest_isPostWeb(request)) {
		BOOL main = HTRequest_isMainDestination(request);
		HTRequest_removeDestination(request);
		return main ? HT_RETRY : HT_OK;
	    }
	    return HT_RETRY;
	    break;

	  case FS_FILE_ERROR:
	    /* Clean up the other connections or just this one */
	    if (HTRequest_isPostWeb(request)) {
		if (file->sockfd == INVSOC)
		    FileCleanup(request, YES);         /* If no valid socket */
		HTRequest_killPostWeb(request);
	    } else
		FileCleanup(request, YES);
	    if (HTRequest_isPostWeb(request)) {
		BOOL main = HTRequest_isMainDestination(request);
		HTRequest_removeDestination(request);
		return main ? HT_ERROR : HT_OK;
	    }
	    return HT_ERROR;
	    break;
	}
    } /* End of while(1) */
}


/*
**  Protocol descriptor
*/

GLOBALDEF PUBLIC HTProtocol HTFile = {
    "file", SOC_NON_BLOCK, HTLoadFile, HTFileSaveStream, NULL
};

