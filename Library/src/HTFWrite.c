/*							       	    HTFWrite.c
**	FILE WRITER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This version of the stream object just writes to a C file.
**	The file is assumed open and left open.
**
**	Bugs:
**		strings written must be less than buffer size.
**
**      History:
**         HFN: wrote it
**         HWL: converted the caching scheme to be hierachical by taking
**              AL code from Deamon
**	   HFN: moved cache code to HTCache module
**
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "HTFormat.h"
#include "HTError.h"
#include "HTAlert.h"
#include "HTLib.h"
#include "HTInet.h"
#include "HTBind.h"
#include "HTParse.h"
#include "HTReq.h"
#include "HTFWrite.h"					 /* Implemented here */

#define HASH_SIZE 1001		/* Tunable */

struct _HTStream {
    const HTStreamClass *isa;
    
    FILE *		fp;
    BOOL		leave_open;		    /* Close file when free? */
    char * 		end_command;		       /* Command to execute */
    BOOL 		remove_on_close;		     /* Remove file? */
    char *		filename;			     /* Name of file */
    HTRequest *		request;		       /* saved for callback */
    HTRequestCallback *	callback;
};

#define DEFAULT_LAST_SEGMENT	"index"

/* ------------------------------------------------------------------------- */

PRIVATE int HTFWriter_put_character (HTStream * me, char c)
{
    return (fputc(c, me->fp) == EOF) ? HT_ERROR : HT_OK;
}

PRIVATE int HTFWriter_put_string (HTStream * me, const char* s)
{
    if (*s)				             /* For vms :-( 10/04-94 */
	return (fputs(s, me->fp) == EOF) ? HT_ERROR : HT_OK;
    return HT_OK;
}

PRIVATE int HTFWriter_flush (HTStream * me)
{
    return (fflush(me->fp) == EOF) ? HT_ERROR : HT_OK;
}

PRIVATE int HTFWriter_write (HTStream * me, const char* s, int l)
{
    int status ;
    status = (fwrite(s, 1, l, me->fp) != l) ? HT_ERROR : HT_OK ;
    if (l > 1 && status == HT_OK) (void) HTFWriter_flush(me);
    return status ;
}

PRIVATE int HTFWriter_free (HTStream * me)
{
    if (me) {
	if (me->leave_open != YES)
	    fclose(me->fp);
	else
	    HTFWriter_flush(me);
#ifdef HAVE_SYSTEM
	if (me->end_command) system(me->end_command);    /* SECURITY HOLE!!! */
#endif
	if (me->callback) (*me->callback)(me->request, me->filename);
	if (me->remove_on_close) REMOVE(me->filename);
	HT_FREE(me->end_command);
	HT_FREE(me->filename);
	HT_FREE(me);
    }
    return HT_OK;
}

PRIVATE int HTFWriter_abort (HTStream * me, HTList * e)
{
    if (STREAM_TRACE) HTTrace("FileWriter.. ABORTING...\n");
    if (me) {
	if (me->leave_open != YES) fclose(me->fp);
	if (me->remove_on_close) REMOVE(me->filename);
	HT_FREE(me->end_command);
	HT_FREE(me->filename);
	HT_FREE(me);
    }
    return HT_ERROR;
}

PRIVATE const HTStreamClass HTFWriter = /* As opposed to print etc */
{		
    "FileWriter",
    HTFWriter_flush,
    HTFWriter_free,
    HTFWriter_abort,
    HTFWriter_put_character,
    HTFWriter_put_string,
    HTFWriter_write
};

PUBLIC HTStream * HTFWriter_new (HTRequest * request, FILE * fp,
				 BOOL leave_open)
{
    HTStream * me = NULL;
    if (!fp) {
	if (STREAM_TRACE)HTTrace("FileWriter.. Bad file descriptor\n");
	return HTErrorStream();
    }
    if ((me = (HTStream *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTFWriter_new");
    me->isa = &HTFWriter;       
    me->fp = fp;
    me->leave_open = leave_open;
    me->request = request;
    return me;
}

/* ------------------------------------------------------------------------- */
/*  			     FILE WRITER ROUTINES			     */
/* ------------------------------------------------------------------------- */

/*
**   This function tries really hard to find a non-existent filename relative
**   to the path given. Returns a string that must be freed by the caller or
**   NULL on error.
*/
PRIVATE char * get_filename (char * base, const char * uri,
			     const char * suffix, BOOL use_last_segment)
{
    char * path = NULL;
    if (use_last_segment) {
	char * uri_path = NULL;
	if (uri && (uri_path = HTParse(uri, "", PARSE_PATH|PARSE_PUNCTUATION))) {
	    char * last_segment = strrchr(uri_path, '/');
	    BOOL slash = (base && *(base+strlen(base)-1)=='/'); 
	    if (last_segment && *(last_segment+1)) {
		StrAllocMCopy(&path, base ? base : "",
			      slash ? "" : "/", ++last_segment, NULL);
	    } else {
		StrAllocMCopy(&path, base ? base : "",
			      slash ? "" : "/", DEFAULT_LAST_SEGMENT,
			      suffix ? suffix : "", NULL);
	    }
	}
    } else {
	path = HTGetTmpFileName(base);
        if (path && suffix) StrAllocCat(path, suffix);
    }

    if (STREAM_TRACE)
	HTTrace("Save file... Temporaray file `%s\'\n", path ? path : "<null>");
    return path;
}

/*	Save Locally
**	------------
**	Saves a file to local disk. This can for example be used to dump
**	data objects of unknown media types to local disk. The stream prompts
**	for a file name for the temporary file.
*/
PUBLIC HTStream* HTSaveLocally (HTRequest *	request,
				void *		param,
				HTFormat	input_format,
				HTFormat	output_format,
				HTStream *	output_stream)
{
    FILE * fp = NULL;
    char * filename = NULL;
    HTUserProfile * up = HTRequest_userProfile(request);
    char * tmproot = HTUserProfile_tmp(up);
    if (HTLib_secure()) {
	HTRequest_addError(request, ERR_NON_FATAL, NO, HTERR_UNAUTHORIZED,
			   NULL, 0, "HTSaveLocally");
	return HTErrorStream();
    }
    if (!tmproot) {
	if (STREAM_TRACE) HTTrace("Save File... turned off\n");
	return HTErrorStream();
    }
	
    /* Let's prompt the user for a file name for this file */
    {
	HTAlertCallback *cbf = HTAlert_find(HT_A_PROMPT);
	HTParentAnchor *anchor = (HTParentAnchor *) HTRequest_anchor(request);

	/*
	**  If we found an alert handler for prompting the user then call it.
	**  If not then either we are in non-interactive mode or no handler
	**  has been registered. For now we then return a blackhole which may
	**  not be the best thing to do.
	*/
	if (cbf) {
	    HTAlertPar * reply = HTAlert_newReply();
	    char * suffix = HTBind_getSuffix(anchor);
	    char * deflt = get_filename(tmproot, HTAnchor_physical(anchor), suffix, YES);
	    if ((*cbf)(request, HT_A_PROMPT, HT_MSG_FILENAME,deflt,NULL,reply))
		filename = HTAlert_replyMessage(reply);
	    HTAlert_deleteReply(reply);
	    HT_FREE(suffix);
	    HT_FREE(deflt);
	}
	if (filename) {
	    if ((fp = fopen(filename, "wb")) == NULL) {
		HTRequest_addError(request, ERR_NON_FATAL, NO, HTERR_NO_FILE,
				   filename, strlen(filename),"HTSaveLocally");
		HT_FREE(filename);
		return HTErrorStream();
	    }
	} else if (cbf) {
	    if (STREAM_TRACE) HTTrace("Save File... No file name - error stream\n");
	    return HTErrorStream();
	} else {
	    if (STREAM_TRACE) HTTrace("Save File... No file name - black hole\n");
	    return HTBlackHole();
	}
    }
    
    /* Now we are ready for creating the file writer stream */
    if (fp) {
	HTStream * me = HTFWriter_new(request, fp, NO);
	me->filename = filename;
	return me;
    }
    HT_FREE(filename);
    return HTErrorStream();
}


/*	Take action using a system command
**	----------------------------------
**	Creates temporary file, writes to it and then executes system
**	command (maybe an external viewer) when EOF has been reached. The
**	stream finds a suitable name of the temporary file which preserves the
**	suffix. This way, the system command can find out the file type from
**	the name of the temporary file name.
*/
PUBLIC HTStream* HTSaveAndExecute (HTRequest *	request,
				   void *	param,
				   HTFormat	input_format,
				   HTFormat	output_format,
				   HTStream *	output_stream)
{
    FILE * fp = NULL;
    char * filename = NULL;
    HTUserProfile * up = HTRequest_userProfile(request);
    char * tmproot = HTUserProfile_tmp(up);
    if (HTLib_secure()) {
	HTRequest_addError(request, ERR_NON_FATAL, NO, HTERR_UNAUTHORIZED,
			   NULL, 0, "HTSaveLocally");
	return HTErrorStream();
    }
    if (!tmproot) {
	if (STREAM_TRACE) HTTrace("Save File... turned off");
	return HTErrorStream();
    }
	
    /* Let's find a hash name for this file without asking user */
    {
	HTParentAnchor *anchor = (HTParentAnchor *) HTRequest_anchor(request);
	char *suffix = HTBind_getSuffix(anchor);
	filename = get_filename(tmproot, HTAnchor_physical(anchor), suffix, NO);
	HT_FREE(suffix);
	if (filename) {
	    if ((fp = fopen(filename, "wb")) == NULL) {
		HTRequest_addError(request, ERR_NON_FATAL, NO, HTERR_NO_FILE,
				   filename, strlen(filename),"HTSaveAndExecute");
		HT_FREE(filename);
		return HTErrorStream();
	    }
	} else {
	    if (STREAM_TRACE) HTTrace("Save File... No file name\n");
	    return HTErrorStream();
	}
    }
    
    /* Now we are ready for creating the file writer stream */
    if (fp) {
	HTStream * me = HTFWriter_new(request, fp, NO);
	me->filename = filename;
	if (param) {
	    if ((me->end_command = (char  *) HT_MALLOC((strlen((char *) param) + 10 + 3*strlen(filename)))) == NULL)
		HT_OUTOFMEM("SaveAndExecute");
	    sprintf (me->end_command,
		     (char *)param, filename, filename, filename);
	}
	return me;
    }
    HT_FREE(filename);
    return HTErrorStream();
}


/*	Save and Call Back
**	------------------
**	This stream works exactly like the HTSaveAndExecute
**	stream but in addition when EOF has been reached, it checks whether a
**	callback function has been associated with the request object in which
**	case, this callback is being called. This can be use by the
**	application to do some processing after the system command
**	has terminated. The callback function is called with the file name of
**	the temporary file as parameter.
*/
PUBLIC HTStream* HTSaveAndCallback (HTRequest *		request,
				    void *		param,
				    HTFormat		input_format,
				    HTFormat		output_format,
				    HTStream *		output_stream)
{
    HTStream * me = HTSaveAndExecute(request, param, input_format,
				     output_format, output_stream);
    if (me) {
	me->callback = HTRequest_callback(request);
	return me;
    }
    return HTErrorStream();
}

