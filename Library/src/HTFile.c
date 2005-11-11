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
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "WWWDir.h"
#include "WWWTrans.h"
#include "HTReqMan.h"
#include "HTBind.h"
#include "HTMulti.h"
#include "HTFile.h"		/* Implemented here */

/* Final states have negative value */
typedef enum _FileState {
    FS_RETRY		= -4,
    FS_ERROR		= -3,
    FS_NO_DATA		= -2,
    FS_GOT_DATA		= -1,
    FS_BEGIN		= 0,
    FS_PENDING,
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
    struct stat		stat_info;	      /* Contains actual file chosen */
    HTNet *		net;
    HTTimer *		timer;
} file_info;

struct _HTStream {
    const HTStreamClass *	isa;
};

struct _HTInputStream {
    const HTInputStreamClass *	isa;
};

PRIVATE HTDirReadme	dir_readme = HT_DIR_README_TOP;
PRIVATE HTDirAccess	dir_access = HT_DIR_OK;
PRIVATE HTDirShow	dir_show = HT_DS_SIZE+HT_DS_DATE+HT_DS_DES+HT_DS_ICON;
PRIVATE HTDirKey	dir_key = HT_DK_CINS;
PRIVATE BOOL		file_suffix_binding = YES;

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

/*
**  Should we find the bindings between file suffixes and media types
**  here or not?
*/
PUBLIC BOOL HTFile_doFileSuffixBinding (BOOL mode)
{
    file_suffix_binding = mode;
    return YES;
}

PUBLIC BOOL HTFile_fileSuffixBinding (void)
{
    return file_suffix_binding;
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
    HTTRACE(PROT_TRACE, "Reading..... directory\n");
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
	    HTTRACE(PROT_TRACE, "Read dir.... `%s\' not found\n" _ DEFAULT_DIR_FILE);
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
	struct dirent result;				    /* For readdir_r */
#endif

#ifdef HAVE_READDIR_R_2
        while ((dirbuf = (struct dirent *) readdir_r(dp, &result)))
#elif defined(HAVE_READDIR_R_3)
        while (readdir_r(dp, &result, &dirbuf) == 0)
#else
	while ((dirbuf = readdir(dp)))
#endif /* HAVE_READDIR_R_2 */
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
		HTTRACE(PROT_TRACE, "Read dir.... lstat failed: %s\n" _ fullname);
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
	return HT_LOADED;
    } else {
	HTRequest_addSystemError(request,  ERR_FATAL, errno, NO, "opendir");
	return HT_ERROR;
    }
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

#ifdef HTDEBUG
    if (PROT_TRACE) {
        int i;
	HTTRACE(PROT_TRACE,
		"File mode is 0%o, uid=%d, gid=%d. My uid=%d, %d groups (" _
		(unsigned int) fileptr->st_mode _
		(int) fileptr->st_uid _ (int) fileptr->st_gid _
		(int) myUid _ ngroups);
	for (i=0; i<ngroups; i++) HTTRACE(PROT_TRACE, " %d" _ (int) groups[i]);
	HTTRACE(PROT_TRACE, ")\n");
    }
#endif /* HTDEBUG */
    
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
    HTTRACE(PROT_TRACE, "\tFile is not editable.\n");
    return NO;					/* If no excuse, can't do */
#else
    /*
    ** We don't know and can't find out. Can we be sure that when opening
    ** a file in mode "a" that the file is not modified?
    */
    return NO;
#endif /* GETGROUPS_T */
}

/*	FileCleanup
**	-----------
**      This function closes the connection and frees memory.
**      Returns YES on OK, else NO
*/
PRIVATE int FileCleanup (HTRequest *req, int status)
{
    HTNet * net = HTRequest_net(req);
    file_info * file = (file_info *) HTNet_context(net);
    HTStream * input = HTRequest_inputStream(req);

    /* Free stream with data TO Local file system */
    if (input) {
        if (status == HT_INTERRUPTED)
            (*input->isa->abort)(input, NULL);
        else
            (*input->isa->_free)(input);
        HTRequest_setInputStream(req, NULL);
    }

    /*
    **  Remove if we have registered a timer function as a callback
    */
    if (file->timer) {
	HTTimer_delete(file->timer);
	file->timer = NULL;
    }

    if (file) {
	HT_FREE(file->local);
	HT_FREE(file);
    }
    HTNet_delete(net, status);
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
PRIVATE int FileEvent (SOCKET soc, void * pVoid, HTEventType type);

PUBLIC int HTLoadFile (SOCKET soc, HTRequest * request)
{
    file_info *file;			      /* Specific access information */
    HTNet * net = HTRequest_net(request);
    HTParentAnchor * anchor = HTRequest_anchor(request);

    HTTRACE(PROT_TRACE, "HTLoadFile.. Looking for `%s\'\n" _ 
			    HTAnchor_physical(anchor));
    if ((file = (file_info *) HT_CALLOC(1, sizeof(file_info))) == NULL)
	HT_OUTOFMEM("HTLoadFILE");
    file->state = FS_BEGIN;
    file->net = net;
    HTNet_setContext(net, file);
    HTNet_setEventCallback(net, FileEvent);
    HTNet_setEventParam(net, file);  /* callbacks get http* */

    return FileEvent(soc, file, HTEvent_BEGIN);	    /* get it started - ops is ignored */
}

PRIVATE int ReturnEvent (HTTimer * timer, void * param, HTEventType type)
{
    file_info * file = (file_info *) param;
    if (timer != file->timer)
	HTDEBUGBREAK("File timer %p not in sync\n" _ timer);
    HTTRACE(PROT_TRACE, "HTLoadFile.. Continuing %p with timer %p\n" _ file _ timer);

    /*
    **  Delete the timer
    */
    HTTimer_delete(file->timer);
    file->timer = NULL;

    /*
    **  Now call the event again
    */
    return FileEvent(INVSOC, file, HTEvent_READ);
}


PRIVATE int FileEvent (SOCKET soc, void * pVoid, HTEventType type)
{
    file_info *file = pVoid;			      /* Specific access information */
    int status = HT_ERROR;
    HTNet * net = file->net;
    HTRequest * request = HTNet_request(net);
    HTParentAnchor * anchor = HTRequest_anchor(request);

    /*
    ** Initiate a new file structure and bind to request structure
    ** This is actually state FILE_BEGIN, but it can't be in the state
    ** machine as we need the structure first.
    */
    if (type == HTEvent_CLOSE) {				      /* Interrupted */
	HTRequest_addError(request, ERR_FATAL, NO, HTERR_INTERRUPTED,
			   NULL, 0, "HTLoadFile");
	FileCleanup(request, HT_INTERRUPTED);
	return HT_OK;
    }


    /* Now jump into the machine. We know the state from the previous run */
    while (1) {
	switch (file->state) {
	case FS_BEGIN:

	    /* We only support safe (GET, HEAD, etc) methods for the moment */
	    if (!HTMethod_isSafe(HTRequest_method(request))) {
		HTRequest_addError(request, ERR_FATAL, NO, HTERR_NOT_ALLOWED,
				   NULL, 0, "HTLoadFile");
		file->state = FS_ERROR;
		break;
	    }

	    /* Check whether we have access to local disk at all */
	    if (HTLib_secure()) {
		HTTRACE(PROT_TRACE, "LoadFile.... No access to local file system\n");
		file->state = FS_TRY_FTP;
		break;
	    }
	    file->local = HTWWWToLocal(HTAnchor_physical(anchor), "",
				       HTRequest_userProfile(request));
	    if (!file->local) {
		file->state = FS_TRY_FTP;
		break;
	    }

	    /* Create a new host object and link it to the net object */
	    {
		HTHost * host = NULL;
		if ((host = HTHost_new("localhost", 0)) == NULL) return HT_ERROR;
		HTNet_setHost(net, host);
		if (HTHost_addNet(host, net) == HT_PENDING) {
		    HTTRACE(PROT_TRACE, "HTLoadFile.. Pending...\n");
		    /* move to the hack state */
		    file->state = FS_PENDING;
		    return HT_OK;
		}
	    }
	    file->state = FS_DO_CN;
	    break;

	case FS_PENDING:
	    /*
	    ** 2000/08/10 JK : This is a funny state. Because of the
	    ** internal libwww stacks, when doing multiple local
	    ** requests (e.g., while using the Robot), we need to ask
	    ** again for the host object. If we had jumped directly to
	    ** the FS_DO_CN state, libwww would have blocked because
	    ** of socket starvation.
	    ** This state is similar to FS_BEGINNING, but just requests 
	    ** the host object. 
	    ** YES. THIS IS AN UGLY HACK!!
	    */
	    {
		HTHost * host = NULL;
		if ((host = HTHost_new("localhost", 0)) == NULL) return HT_ERROR;
		HTNet_setHost(net, host);
		if (HTHost_addNet(host, net) == HT_PENDING) {
		    HTTRACE(PROT_TRACE, "HTLoadFile.. Pending...\n");
		    file->state = FS_PENDING;
		    return HT_OK;
		}
	    }
	    file->state = FS_DO_CN;
	    break;

	case FS_DO_CN:
	    /*
	    ** If we have to do content negotiation then find the object that
	    ** fits best into either what the client has indicated in the
	    ** accept headers or what the client has registered on its own.
	    ** The object chosen can in fact be a directory! However, content
	    ** negotiation only makes sense if we can read the directory!
	    ** We stat the file in order to find the size and to see it if
	    ** exists.
	    */
	    if (HTRequest_negotiation(request) &&
		HTMethod_isSafe(HTRequest_method(request))) {
 		char * conneg = HTMulti(request, file->local,&file->stat_info);
		if (conneg) {
		    HT_FREE(file->local);
		    file->local = conneg;
		    HTAnchor_setPhysical(anchor, conneg);
		    HTTRACE(PROT_TRACE, "Load File... Found `%s\'\n" _ conneg);
		} else {
		    HTTRACE(PROT_TRACE, "Load File... Not found - even tried content negotiation\n");
		    HTRequest_addError(request, ERR_FATAL, NO, HTERR_NOT_FOUND,
				       NULL, 0, "HTLoadFile");
		    file->state = FS_ERROR;
		    break;
		}
	    } else {
		if (HT_STAT(file->local, &file->stat_info) == -1) {
		    HTTRACE(PROT_TRACE, "Load File... Not found `%s\'\n" _ file->local);
		    HTRequest_addError(request, ERR_FATAL, NO, HTERR_NOT_FOUND,
				       NULL, 0, "HTLoadFile");
		    file->state = FS_ERROR;
		    break;
		}
	    }

	    /*
	    ** Check to see if the 'localname' is in fact a directory.
	    ** Note that we can't do a HEAD on a directory
	    */
	    if (((file->stat_info.st_mode) & S_IFMT) == S_IFDIR) {
		if (HTRequest_method(request) == METHOD_GET)
		    file->state = FS_PARSE_DIR;
		else {
		    HTRequest_addError(request, ERR_INFO, NO, HTERR_NO_CONTENT,
				       NULL, 0, "HTLoadFile");
		    file->state = FS_NO_DATA;
		}
		break;
	    }

	    /*
	    ** If empty file then only serve it if it is editable. We also get
	    ** the bindings for the file suffixes in lack of better bindings
	    */
	    {
		BOOL editable = HTEditable(file->local, &file->stat_info);
		if (file_suffix_binding) HTBind_getAnchorBindings(anchor);
		if (editable) HTAnchor_appendAllow(anchor, METHOD_PUT);

		/* Set the file size */
		if (file->stat_info.st_size)
		    HTAnchor_setLength(anchor, file->stat_info.st_size);

		/* Set the file last modified time stamp */
		if (file->stat_info.st_mtime > 0)
		    HTAnchor_setLastModified(anchor, file->stat_info.st_mtime);

		/* Check to see if we can edit it */
		if (!editable && !file->stat_info.st_size) {
		    HTRequest_addError(request, ERR_INFO, NO, HTERR_NO_CONTENT,
				       NULL, 0, "HTLoadFile");
		    file->state = FS_NO_DATA;
		} else {
		    file->state = (HTRequest_method(request)==METHOD_GET) ? 
			FS_NEED_OPEN_FILE : FS_GOT_DATA;
		}
	    }
	    break;

	  case FS_NEED_OPEN_FILE:
	    status = HTFileOpen(net, file->local, HT_FB_RDONLY);
	    if (status == HT_OK) {
		/* 
		** Create the stream pipe FROM the channel to the application.
		** The target for the input stream pipe is set up using the
		** stream stack.
		*/
		{
		    HTStream * rstream = HTStreamStack(HTAnchor_format(anchor),
						       HTRequest_outputFormat(request),
						       HTRequest_outputStream(request),
						       request, YES);
		    HTNet_setReadStream(net, rstream);
		    HTRequest_setOutputConnected(request, YES);
		}

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
		if (HTRequest_isSource(request) && !HTRequest_destinationsReady(request))
		    return HT_OK;
		HTRequest_addError(request, ERR_INFO, NO, HTERR_OK, NULL, 0,
				   "HTLoadFile");
		file->state = FS_NEED_BODY;

		/* If we are _not_ using preemptive mode and we are Unix fd's
		** then return here to get the same effect as when we are
		** connecting to a socket. That way, HTFile acts just like any
		** other protocol module even though we are in fact doing
		** blocking connect
		*/
		if (HTEvent_isCallbacksRegistered()) {
		    if (!HTRequest_preemptive(request)) {
			if (!HTNet_preemptive(net)) {
			    HTTRACE(PROT_TRACE, "HTLoadFile.. Returning\n");
			    HTHost_register(HTNet_host(net), net, HTEvent_READ);
			} else if (!file->timer) {
			    HTTRACE(PROT_TRACE, "HTLoadFile.. Returning\n");
			    file->timer =
				HTTimer_new(NULL, ReturnEvent, file, 1, YES, NO);
			}
			return HT_OK;
		    }
		}
	    } else if (status == HT_WOULD_BLOCK || status == HT_PENDING)
		return HT_OK;
	    else {
		HTRequest_addError(request, ERR_INFO, NO, HTERR_INTERNAL,
				   NULL, 0, "HTLoadFile");
		file->state = FS_ERROR;		       /* Error or interrupt */
	    }
	    break;

	  case FS_NEED_BODY:
	    status = HTHost_read(HTNet_host(net), net);
	    if (status == HT_WOULD_BLOCK)
		return HT_OK;
	    else if (status == HT_LOADED || status == HT_CLOSED) {
		file->state = FS_GOT_DATA;
	    } else {
		HTRequest_addError(request, ERR_INFO, NO, HTERR_FORBIDDEN,
				   NULL, 0, "HTLoadFile");
		file->state = FS_ERROR;
	    }
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
	    FileCleanup(request, HT_LOADED);
	    return HT_OK;
	    break;

	  case FS_NO_DATA:
	    FileCleanup(request, HT_NO_DATA);
	    return HT_OK;
	    break;

	  case FS_RETRY:
	    FileCleanup(request, HT_RETRY);
	    return HT_OK;
	    break;

	  case FS_ERROR:
	    FileCleanup(request, HT_ERROR);
	    return HT_OK;
	    break;
	}
    } /* End of while(1) */
}

    
/* Calculate the required buffer size (in bytes) for directory     
** entries read from the given directory handle.  Return -1 if this
** this cannot be done.                                              
*/    
PUBLIC size_t HTFile_dirent_buf_size(DIR * dirp)
{
    long name_max;
#if defined(HAVE_FPATHCONF) && defined(HAVE_DIRFD) && defined(_PC_NAME_MAX)
        name_max = fpathconf(dirfd(dirp), _PC_NAME_MAX);
        if (name_max == -1)
#if defined(NAME_MAX)
                name_max = (NAME_MAX > 255) ? NAME_MAX : 255;
#else
                return (size_t)(-1);
#endif
#else
#if defined(NAME_MAX)
            name_max = (NAME_MAX > 255) ? NAME_MAX : 255;
#else
#error "buffer size for readdir_r cannot be determined"
#endif
#endif
        return (size_t)offsetof(struct dirent, d_name) + name_max + 1;
}
