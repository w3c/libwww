/*								       HTFile.c
**	FILE ACCESS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
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
**		  HFN	Separated transport
**
** Bugs:
**	FTP: Cannot access VMS files from a unix machine.
**      How can we know that the
**	target machine runs VMS?
*/

/* Library Includes */
#include "sysdep.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "WWWDir.h"
#include "WWWTrans.h"
#include "HTReqMan.h"
#include "HTNetMan.h"
#include "HTMulti.h"
#include "HTFile.h"		/* Implemented here */

/* Final states have negative value */
typedef enum _FileState {
    FS_RETRY		= -4,
    FS_ERROR		= -3,
    FS_NO_DATA		= -2,
    FS_GOT_DATA		= -1,
    FS_BEGIN		= 0,
    FS_DO_CN,
    FS_NEED_OPEN_FILE,
    FS_NEED_BODY,
    FS_PARSE_DIR,
    FS_TRY_FTP
} FileState;

/* This is the context structure for the this module */
typedef struct _file_info {
    FileState		state;		  /* Current state of the connection */
    char *		local;		/* Local representation of file name */
} file_info;

struct _HTStream {
    const HTStreamClass *	isa;
};

struct _HTInputStream {
    const HTInputStreamClass *	isa;
};

#if 0
PRIVATE BOOL		HTTakeBackup = YES;
#endif

PRIVATE HTDirReadme	dir_readme = HT_DIR_README_TOP;
PRIVATE HTDirAccess	dir_access = HT_DIR_OK;
PRIVATE HTDirShow	dir_show = HT_DS_SIZE+HT_DS_DATE+HT_DS_DES+HT_DS_ICON;
PRIVATE HTDirKey	dir_key = HT_DK_CINS;

/* ------------------------------------------------------------------------- */

/*	Directory Access
**	----------------
*/
PUBLIC BOOL HTFile_setDirAccess (HTDirAccess mode)
{
    dir_access = mode;
    return YES;
}

PUBLIC HTDirAccess HTFile_dirAccess (void)
{
    return dir_access;
}

/*	Directory Readme
**	----------------
*/
PUBLIC BOOL HTFile_setDirReadme (HTDirReadme mode)
{
    dir_readme = mode;
    return YES;
}

PUBLIC HTDirReadme HTFile_dirReadme (void)
{
    return dir_readme;
}

/*	HTFile_readDir
**	--------------
**	Reads the directory "path"
**	Returns:
**		HT_ERROR	Error
**		HT_FORBIDDEN	Directory reading not allowed
**		HT_LOADED	Successfully read the directory
*/
PRIVATE int HTFile_readDir (HTRequest * request, file_info *file)
{
#ifdef HAVE_READDIR
    DIR * dp;
    struct stat file_info;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    char *url = HTAnchor_physical(anchor);
    char fullname[HT_MAX_PATH+1];
    char *name;
    if (PROT_TRACE) HTTrace("Reading..... directory\n");
    if (dir_access == HT_DIR_FORBID) {
	HTRequest_addError(request, ERR_FATAL, NO, HTERR_FORBIDDEN,
		   NULL, 0, "HTFile_readDir");
	return HT_FORBIDDEN;
    }
    
    /* Initialize path name for stat() */
    if (*(name = (url+strlen(url)-1)) != '/') {
	char *newurl = NULL;
	StrAllocCopy(newurl, url);
	StrAllocCat(newurl, "/");
	HT_FREE(file->local);
	file->local = HTWWWToLocal(newurl, "", HTRequest_userProfile(request));
	HT_FREE(newurl);
    }
    strcpy(fullname, file->local);
    name = fullname+strlen(fullname);		 /* Point to end of fullname */

    /* Check if access is enabled */
    if (dir_access == HT_DIR_SELECTIVE) {
	strcpy(name, DEFAULT_DIR_FILE);
	if (HT_STAT(fullname, &file_info)) {
	    if (PROT_TRACE)
		HTTrace(
			"Read dir.... `%s\' not found\n", DEFAULT_DIR_FILE);
	    HTRequest_addError(request, ERR_FATAL, NO, HTERR_FORBIDDEN,
		       NULL, 0, "HTFile_readDir");
	    return HT_FORBIDDEN;
	}
    }

    if ((dp = opendir(file->local))) {
	struct dirent * dirbuf;
	HTDir *dir = HTDir_new(request, dir_show, dir_key);
	char datestr[20];
	char sizestr[10];
	HTFileMode mode;
#ifdef HT_REENTRANT
	struct dirent * result;				    /* For readdir_r */
	while ((dirbuf = (dirent *) readdir_r(dp, &result)))
#else
	while ((dirbuf = readdir(dp)))
#endif /* HT_REENTRANT */
	{
	    /* Current and parent directories are never shown in list */
#ifdef HAVE_DIRENT_INO
	    if (!dirbuf->d_ino ||
		!strcmp(dirbuf->d_name, ".") || !strcmp(dirbuf->d_name, ".."))
#else
	    if (!strcmp(dirbuf->d_name, ".") || !strcmp(dirbuf->d_name, ".."))
#endif
		continue;

	    /* Make a lstat on the file */
	    strcpy(name, dirbuf->d_name);
	    if (HT_LSTAT(fullname, &file_info)) {
		if (PROT_TRACE)
		    HTTrace("Read dir.... lstat failed: %s\n",fullname);
		continue;
	    }

	    /* Convert stat info to fit our setup */
	    if (((mode_t) file_info.st_mode & S_IFMT) == S_IFDIR) {
#ifdef VMS
		char *dot = strstr(name, ".DIR");      /* strip .DIR part... */
		if (dot) *dot = '\0';
#endif /* VMS */
		mode = HT_IS_DIR;
		if (dir_show & HT_DS_SIZE) strcpy(sizestr, "-");
	    } else {
		mode = HT_IS_FILE;
		if (dir_show & HT_DS_SIZE)
		    HTNumToStr(file_info.st_size, sizestr, 10);
	    }
	    if (dir_show & HT_DS_DATE)
		HTDateDirStr(&file_info.st_mtime, datestr, 20);

	    /* Add to the list */
	    if (HTDir_addElement(dir, name, datestr, sizestr, mode) != YES)
		break;
	}
	closedir(dp);
	HTDir_free(dir);
    } else
	HTRequest_addSystemError(request,  ERR_FATAL, errno, NO, "opendir");
    return HT_LOADED;
#else
    return HT_ERROR;	/* needed for WWW_MSWINDOWS */
#endif /* HAVE_READDIR */
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
PRIVATE BOOL HTEditable (const char * filename, struct stat * stat_info)
{
#ifdef GETGROUPS_T
    int i;
    uid_t myUid;
    int	ngroups;			/* The number of groups  */
    struct stat	fileStatus;
    struct stat *fileptr = stat_info ? stat_info : &fileStatus;
    GETGROUPS_T groups[NGROUPS];
    if (!stat_info) {
	if (HT_STAT(filename, &fileStatus))
	    return NO;				  /* Can't even access file! */
    }
    ngroups = getgroups(NGROUPS, groups);	/* Groups to which I belong  */
    myUid = geteuid();				/* Get my user identifier */

    if (PROT_TRACE) {
        int i;
	HTTrace(
	    "File mode is 0%o, uid=%d, gid=%d. My uid=%d, %d groups (",
    	    (unsigned int) fileptr->st_mode, (int) fileptr->st_uid,
	    (int) fileptr->st_gid, (int) myUid, ngroups);
	for (i=0; i<ngroups; i++) HTTrace(" %d", (int) groups[i]);
	HTTrace(")\n");
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
    if (PROT_TRACE) HTTrace("\tFile is not editable.\n");
    return NO;					/* If no excuse, can't do */
#else
    /*
    ** We don't know and can't find out. Can we be sure that when opening
    ** a file in mode "a" that the file is not modified?
    */
    return NO;
#endif /* GETGROUPS_T */
}

#if 0
/*	Make a save stream
**	------------------
**
**	The stream must be used for writing back the file.
**	@@@ no backup done
*/
PRIVATE HTStream * HTFileSaveStream (HTRequest * request)
{
    
    const char * addr = HTAnchor_address((HTAnchor*)request->anchor);
    char * filename = HTWWWToLocal(addr, "", HTRequest_userProfile(request));
    FILE* fp;

/*	@ Introduce CVS handling here one day
*/
/*	Take a backup before we do anything silly   931205
*/        
    if (HTTakeBackup) {
	char * p;
    	char * backup_filename;
    	if ((backup_filename = (char  *) HT_MALLOC(strlen(filename)+2)) == NULL)
    	    HT_OUTOFMEM("taking backup");
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
	    if (PROT_TRACE) HTTrace("File `%s' exists\n", filename);
	    if (REMOVE(backup_filename)) {
		if (PROT_TRACE) HTTrace("Backup file `%s' removed\n",
				   backup_filename);
	    }
	    if (rename(filename, backup_filename)) {	/* != 0 => Failure */
		if (PROT_TRACE) HTTrace("Rename `%s' to `%s' FAILED!\n",
				   filename, backup_filename);
	    } else {					/* Success */
		if (PROT_TRACE)
		    HTTrace("Renamed `%s' to `%s'\n", filename,
			    backup_filename);
	    }
	}
    	HT_FREE(backup_filename);
    } /* if take backup */    
    
    if ((fp = fopen(filename, "wb")) == NULL) {
	HTRequest_addSystemError(request, ERR_FATAL, errno, NO, "fopen");
	return NULL;
    } else
    	return HTFWriter_new(request, fp, NO);
}
#endif

/*	FileCleanup
**	-----------
**      This function closes the connection and frees memory.
**      Returns YES on OK, else NO
*/
PRIVATE int FileCleanup (HTRequest *req, int status)
{
    HTNet *net = req->net;
    file_info *file = (file_info *) net->context;

    /* Free stream with data TO Local file system */
    if (HTRequest_isDestination(req))
	HTRequest_removeDestination(req);
    else  if (req->input_stream) {
	if (status == HT_INTERRUPTED)
	    (*req->input_stream->isa->abort)(req->input_stream, NULL);
	else
	    (*req->input_stream->isa->_free)(req->input_stream);
	req->input_stream = NULL;
    }

    if (file) {
	HT_FREE(file->local);
	HT_FREE(file);
    }
    HTNet_delete(net, req->internal ? HT_IGNORE : status);
    return YES;
}


/*	Load a document
**	---------------
**
** On entry,
**	request		This is the request structure
** On exit,
**	returns		HT_ERROR	Error has occured in call back
**			HT_OK		Call back was OK
*/
PUBLIC int HTLoadFile (SOCKET soc, HTRequest * request, SockOps ops)
{
    int status = HT_ERROR;
    HTNet * net = HTRequest_net(request);
    HTParentAnchor * anchor = HTRequest_anchor(request);
    file_info *file;			      /* Specific access information */

    /*
    ** Initiate a new file structure and bind to request structure
    ** This is actually state FILE_BEGIN, but it can't be in the state
    ** machine as we need the structure first.
    */
    if (ops == FD_NONE) {
	if (PROT_TRACE) HTTrace("HTLoadFile.. Looking for `%s\'\n",
				HTAnchor_physical(anchor));
	if ((file = (file_info *) HT_CALLOC(1, sizeof(file_info))) == NULL)
	    HT_OUTOFMEM("HTLoadFILE");
	file->state = FS_BEGIN;
	net->context = file;
    } if (ops == FD_CLOSE) {				      /* Interrupted */
	HTRequest_addError(request, ERR_FATAL, NO, HTERR_INTERRUPTED,
			   NULL, 0, "HTLoadHTTP");
	FileCleanup(request, HT_INTERRUPTED);
	return HT_OK;
    } else
	file = (file_info *) net->context;		/* Get existing copy */

    /* Now jump into the machine. We know the state from the previous run */
    while (1) {
	switch (file->state) {
	  case FS_BEGIN:
	    if (HTLib_secure()) {
		if (PROT_TRACE)
		    HTTrace("LoadFile.... No access to local file system\n");
		file->state = FS_TRY_FTP;
		break;
	    }
	    file->local = HTWWWToLocal(HTAnchor_physical(anchor), "",
				       HTRequest_userProfile(request));
	    if (!file->local) {
		file->state = FS_TRY_FTP;
		break;
	    }

	    /* If cache element then jump directly to OPEN FILE state */
	    file->state = HTAnchor_cacheHit(anchor) ?
		FS_NEED_OPEN_FILE : FS_DO_CN;
	    break;

	  case FS_DO_CN:
	    /*
	    ** If we have to do content negotiation then find the object that
	    ** fits best into either what the client has indicated in the
	    ** accept headers or what the client has registered on its own.
	    ** The object chosen can in fact be a directory! However, content
	    ** negotiation only makes sense it we can read the directory!
	    ** We stat the file in order to find the size and to see it if
	    ** exists.
	    */
	    {
		struct stat stat_info;	      /* Contains actual file chosen */
		if (HTRequest_negotiation(request)) {
		    char *new_path=HTMulti(request,file->local,&stat_info);
		    if (new_path) {
			HT_FREE(file->local);
			file->local = new_path;
			HTAnchor_setPhysical(anchor, new_path);
		    } else {
			file->state = FS_ERROR;
			break;
		    }
		} else {
		    if (HT_STAT(file->local, &stat_info) == -1) {
			if (PROT_TRACE)
			    HTTrace("HTLoadFile.. Can't stat %s\n",
				    file->local);
			HTRequest_addError(request, ERR_FATAL, NO, HTERR_NOT_FOUND,
				   NULL, 0, "HTLoadFile");
			file->state = FS_ERROR;
			break;
		    }
		}
		/* Check to see if the 'localname' is in fact a directory */
		if (((stat_info.st_mode) & S_IFMT) == S_IFDIR)
		    file->state = FS_PARSE_DIR;
		else {
		    /*
		    ** If empty file then only serve it if it is editable
		    */
		    BOOL editable = HTEditable(file->local, &stat_info);
		    HTBind_getBindings(anchor);
		    if (editable)
			HTAnchor_appendMethods(anchor, METHOD_PUT);
		    if (stat_info.st_size)
			HTAnchor_setLength(anchor, stat_info.st_size);

		    /* Done with relevant metainformation in anchor */
		    HTAnchor_setHeaderParsed(anchor);

		    if (!editable && !stat_info.st_size) {
			HTRequest_addError(request, ERR_FATAL, NO, HTERR_NO_CONTENT,
				   NULL, 0, "HTLoadFile");
			file->state = FS_NO_DATA;
		    } else
			file->state = FS_NEED_OPEN_FILE;
		}
	    }
	    break;

	  case FS_NEED_OPEN_FILE:
	    status = HTFileOpen(net, file->local, HT_FT_RDONLY);
	    if (status == HT_OK) {
		/* 
		** Create the stream pipe FROM the channel to the application.
		** The target for the input stream pipe is set up using the
		** stream stack. If we are reading from the cache then use the
		** MIME parser as well.
		*/
		if (HTAnchor_cacheHit(anchor))
		    HTAnchor_setFormat(anchor, WWW_MIME);
		HTNet_getInput(net,
			       HTStreamStack(HTAnchor_format(anchor),
					     HTRequest_outputFormat(request),
					     HTRequest_outputStream(request),
					     request, YES), NULL, 0);
		/*
		** Create the stream pipe TO the channel from the application
		** and hook it up to the request object
		*/
		{
		    HTOutputStream * output = HTNet_getOutput(net, NULL, 0);
		    HTRequest_setInputStream(request, (HTStream *) output);
		}

		/*
		** Set up concurrent read/write if this request isn't the
		** source for a PUT or POST. As source we don't start reading
		** before all destinations are ready. If destination then
		** register the input stream and get ready for read
		*/
		if (HTRequest_isDestination(request)) {
		    HTEvent_register(net->sockfd, request, (SockOps) FD_READ,
				     HTLoadFile, net->priority);
		    HTRequest_linkDestination(request);
		}
		if (HTRequest_isSource(request) &&
		    !HTRequest_destinationsReady(request))
		    return HT_OK;

		file->state = FS_NEED_BODY;

#ifndef NO_UNIX_IO
		/* If we are _not_ using preemptive mode and we are Unix fd's
		** then return here to get the same effect as when we are
		** connecting to a socket. That way, HTFile acts just like any
		** other protocol module even though we are in fact doing
		** blocking connect
		*/
		if (!net->preemptive) {
		    if (PROT_TRACE) HTTrace("HTLoadFile.. returning\n");
		    HTEvent_register(net->sockfd, request, (SockOps) FD_READ,
				     net->cbf, net->priority);
		    return HT_WOULD_BLOCK;
		}
#endif
	    } else if (status == HT_WOULD_BLOCK || status == HT_PERSISTENT)
		return HT_OK;
	    else
		file->state = FS_ERROR;		       /* Error or interrupt */
	    break;

	  case FS_NEED_BODY:
	    status = (*net->input->isa->read)(net->input);
	    if (status == HT_WOULD_BLOCK)
		return HT_OK;
	    else if (status == HT_LOADED || status == HT_CLOSED) {
		file->state = FS_GOT_DATA;
	    } else
		file->state = FS_ERROR;
	    break;

	  case FS_PARSE_DIR:
	    status = HTFile_readDir(request, file);
	    if (status == HT_LOADED)
		file->state = FS_GOT_DATA;
	    else
		file->state = FS_ERROR;
	    break;

	  case FS_TRY_FTP:
	    {
		char *url = HTAnchor_physical(anchor);
		HTAnchor *anchor;
		char *newname = NULL;
		StrAllocCopy(newname, "ftp:");
		if (!strncmp(url, "file:", 5))
		    StrAllocCat(newname, url+5);
		else
		    StrAllocCat(newname, url);
		anchor = HTAnchor_findAddress(newname);
		HTRequest_setAnchor(request, anchor);
		HT_FREE(newname);
		FileCleanup(request, HT_IGNORE);
		return HTLoad(request, YES);
	    }
	    break;

	  case FS_GOT_DATA:
	    if (HTRequest_isPostWeb(request)) {
		if (HTRequest_isDestination(request)) {
		    HTRequest * source = HTRequest_source(request);
		    HTLink *link =
			HTAnchor_findLink((HTAnchor *)HTRequest_anchor(source),
					  (HTAnchor *) anchor);
		    HTLink_setResult(link, HT_LINK_OK);
		}
	    }
	    FileCleanup(request, HT_LOADED);
	    return HT_OK;
	    break;

	  case FS_NO_DATA:
	    if (HTRequest_isPostWeb(request)) {
		if (HTRequest_isDestination(request)) {
		    HTRequest * source = HTRequest_source(request);
		    HTLink *link =
			HTAnchor_findLink((HTAnchor *)HTRequest_anchor(source),
					  (HTAnchor *) anchor);
		    HTLink_setResult(link, HT_LINK_OK);
		}
	    }
	    FileCleanup(request, HT_NO_DATA);
	    return HT_OK;
	    break;

	  case FS_RETRY:
	    if (HTRequest_isPostWeb(request)) {
		if (HTRequest_isDestination(request)) {
		    HTRequest * source = HTRequest_source(request);
		    HTLink *link =
			HTAnchor_findLink((HTAnchor *)HTRequest_anchor(source),
					  (HTAnchor *) anchor);
		    HTLink_setResult(link, HT_LINK_ERROR);
		}
		HTRequest_killPostWeb(request);
	    }
	    FileCleanup(request, HT_RETRY);
	    return HT_OK;
	    break;

	  case FS_ERROR:
	    if (HTRequest_isPostWeb(request)) {
		if (HTRequest_isDestination(request)) {
		    HTRequest * source = HTRequest_source(request);
		    HTLink *link =
			HTAnchor_findLink((HTAnchor *)HTRequest_anchor(source),
					  (HTAnchor *) anchor);
		    HTLink_setResult(link, HT_LINK_ERROR);
		}
		HTRequest_killPostWeb(request);
	    }
	    FileCleanup(request, HT_ERROR);
	    return HT_OK;
	    break;
	}
    } /* End of while(1) */
}
