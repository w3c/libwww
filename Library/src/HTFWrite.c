/*							       	    HTFWrite.c
**	FILE WRITER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
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
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTFormat.h"
#include "HTError.h"
#include "HTAlert.h"
#include "HTAccess.h"
#include "HTBind.h"
#include "HTList.h"
#include "HTParse.h"
#include "HTReq.h"
#include "HTFWrite.h"					 /* Implemented here */

#define HASH_SIZE 1001		/* Tunable */

/* The default directory for "save locally" and "save and execute" files: */
#ifndef HT_TMP_ROOT
#define HT_TMP_ROOT		"/tmp"
#endif

struct _HTStream {
    CONST HTStreamClass *isa;
    
    FILE *		fp;
    BOOL		leave_open;		    /* Close file when free? */
    char * 		end_command;		       /* Command to execute */
    BOOL 		remove_on_close;		     /* Remove file? */
    char *		filename;			     /* Name of file */
    HTRequest *		request;		       /* saved for callback */
    HTRequestCallback *	callback;
};

PRIVATE HTStream	HTBaseStreamInstance;		      /* Made static */

PRIVATE char *		HTTmpRoot = NULL;   	       /* Dest for tmp files */

/* ------------------------------------------------------------------------- */
/*  			     BASIC STREAM CLASSES			     */
/* ------------------------------------------------------------------------- */

/*
**
**		B L A C K    H O L E    C L A S S
**
**	There is only one black hole instance shared by anyone
**	who wants a black hole.  These black holes don't radiate,
**	they just absorb data.
*/
PRIVATE int HTBlackHole_put_character (HTStream * me, char c)
{
    return HT_OK;
}

PRIVATE int HTBlackHole_put_string (HTStream * me, CONST char * s)
{
    return HT_OK;
}

PRIVATE int HTBlackHole_write (HTStream * me, CONST char * s, int l)
{
    return HT_OK;
}

PRIVATE int HTBlackHole_flush (HTStream * me)
{
    return HT_OK;
}

PRIVATE int HTBlackHole_free (HTStream * me)
{
    return HT_OK;
}

PRIVATE int HTBlackHole_abort (HTStream * me, HTList * e)
{
    return HT_ERROR;
}


/*	Black Hole stream
**	-----------------
*/
PRIVATE CONST HTStreamClass HTBlackHoleClass =
{		
    "BlackHole",
    HTBlackHole_flush,
    HTBlackHole_free,
    HTBlackHole_abort,
    HTBlackHole_put_character,
    HTBlackHole_put_string,
    HTBlackHole_write
}; 

PUBLIC HTStream * HTBlackHole (void)
{
    if (STREAM_TRACE) TTYPrint(TDEST, "BlackHole... Created\n");
    HTBaseStreamInstance.isa = &HTBlackHoleClass;      /* The rest is random */
    return &HTBaseStreamInstance;
}

PUBLIC HTStream * HTBlackHoleConverter (HTRequest *	request,
					void *		param,
					HTFormat	input_format,
					HTFormat	output_format,
					HTStream *	output_stream)
{
    return HTBlackHole();
}

/*
**	ERROR STREAM
**	------------
**	There is only one error stream shared by anyone who wants a
**	generic error returned from all stream methods.
*/
PRIVATE int HTErrorStream_put_character (HTStream * me, char c)
{
    return HT_ERROR;
}

PRIVATE int HTErrorStream_put_string (HTStream * me, CONST char * s)
{
    return HT_ERROR;
}

PRIVATE int HTErrorStream_write (HTStream * me, CONST char * s, int l)
{
    return HT_ERROR;
}

PRIVATE int HTErrorStream_flush (HTStream * me)
{
    return HT_ERROR;
}

PRIVATE int HTErrorStream_free (HTStream * me)
{
    return HT_ERROR;
}

PRIVATE int HTErrorStream_abort (HTStream * me, HTList * e)
{
    return HT_ERROR;
}

PRIVATE CONST HTStreamClass HTErrorStreamClass =
{		
    "ErrorStream",
    HTErrorStream_flush,
    HTErrorStream_free,
    HTErrorStream_abort,
    HTErrorStream_put_character,
    HTErrorStream_put_string,
    HTErrorStream_write
}; 

PUBLIC HTStream * HTErrorStream (void)
{
    if (STREAM_TRACE) TTYPrint(TDEST, "ErrorStream. Created\n");
    HTBaseStreamInstance.isa = &HTErrorStreamClass;    /* The rest is random */
    return &HTBaseStreamInstance;
}

/*	HTThroughLine
**	-------------
**
** This function is a dummy function that returns the same output stream
** as given as a parameter. Henrik 01/03-94
*/
PUBLIC HTStream* HTThroughLine (HTRequest *	request,
				void *		param,
				HTFormat	input_format,
				HTFormat	output_format,
				HTStream *	output_stream)
{
    return output_stream;
}

/* ------------------------------------------------------------------------- */
/*  			     SOCKET WRITER STREAM			     */
/* ------------------------------------------------------------------------- */

PRIVATE int HTFWriter_put_character (HTStream * me, char c)
{
    return (fputc(c, me->fp) == EOF) ? HT_ERROR : HT_OK;
}

PRIVATE int HTFWriter_put_string (HTStream * me, CONST char* s)
{
    if (*s)				             /* For vms :-( 10/04-94 */
	return (fputs(s, me->fp) == EOF) ? HT_ERROR : HT_OK;
    return HT_OK;
}

PRIVATE int HTFWriter_flush (HTStream * me)
{
    return (fflush(me->fp) == EOF) ? HT_ERROR : HT_OK;
}

PRIVATE int HTFWriter_write (HTStream * me, CONST char* s, int l)
{
    int status ;
    status = (fwrite(s, 1, l, me->fp) != l) ? HT_ERROR : HT_OK ;
    if (l > 1 && status == HT_OK) (void) HTFWriter_flush(me);
    return status ;
}

PRIVATE int HTFWriter_free (HTStream * me)
{
    if (me) {
	if (me->leave_open != YES) fclose(me->fp);
#ifdef GOT_SYSTEM
	if (me->end_command) system(me->end_command);    /* SECURITY HOLE!!! */
#endif
	if (me->remove_on_close) REMOVE(me->filename);
	if (me->callback) (*me->callback)(me->request, me->filename);
	HT_FREE(me->end_command);
	HT_FREE(me->filename);
	HT_FREE(me);
    }
    return HT_OK;
}

PRIVATE int HTFWriter_abort (HTStream * me, HTList * e)
{
    if (STREAM_TRACE) TTYPrint(TDEST, "FileWriter.. ABORTING...\n");
    if (me) {
	if (me->leave_open != YES) fclose(me->fp);
	if (me->remove_on_close) REMOVE(me->filename);
	HT_FREE(me->end_command);
	HT_FREE(me->filename);
	HT_FREE(me);
    }
    return HT_ERROR;
}

PRIVATE CONST HTStreamClass HTFWriter = /* As opposed to print etc */
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
	if (STREAM_TRACE)TTYPrint(TDEST, "FileWriter.. Bad file descriptor\n");
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

/*	Set TMP Root
**	--------------
**	If `tmp_root' is NULL use the current value (might be a define)
*/
PUBLIC BOOL HTTmp_setRoot (CONST char * tmp_root)
{
    StrAllocCopy(HTTmpRoot, tmp_root ? tmp_root : HT_TMP_ROOT);
    if (*(HTTmpRoot+strlen(HTTmpRoot)-1) != '/')
	StrAllocCat(HTTmpRoot, "/");
    if (STREAM_TRACE)
	TTYPrint(TDEST, "Tmp Root.... Root set to `%s\'\n", HTTmpRoot);
    return YES;
}


/*	Get Tmp Root
**	--------------
*/
PUBLIC CONST char * HTTmp_getRoot (void)
{
    return HTTmpRoot;
}


/*	Free Tmp Root
**	--------------
**	For clean up memory
*/
PUBLIC void HTTmp_freeRoot (void)
{
    HT_FREE(HTTmpRoot);
}

/*
**   This function tries really hard to find a non-existent filename relative
**   to the path given. Returns a string that must be freed by the caller or
**   NULL on error. The base must be '/' terminated which!
*/
PRIVATE char *get_filename (char * base, CONST char * url, CONST char * suffix)
{
    char *path=NULL;
    char filename[40];
    int hash=0;

    /* Do this until we find a name that doesn't exist */
    while (1)
    {
	CONST char *ptr=url;
	for( ; *ptr; ptr++)
	    hash = (int) ((hash * 31 + (* (unsigned char *) ptr)) % HASH_SIZE);

#ifndef NO_GETPID
	sprintf(filename, "%d-%d", hash, (int) getpid());
#else
	sprintf(filename, "%d-%d", hash, time(NULL));
#endif
	StrAllocCopy(path, base);
	StrAllocCat(path, filename);
	if (suffix) StrAllocCat(path, suffix);

	{
	    FILE *fp = fopen(path, "r");
	    if (fp)			     /* This file does already exist */
		fclose(fp);
	    else
		break;					/* Got the file name */
	}
    }
    return path;
}


/*	Save Locally
**	------------
**	Saves a file to local disk. This can for example be used to dump
**	date objects of unknown media types to local disk. The stream prompts
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
    if (HTLib_secure()) {
	HTRequest_addError(request, ERR_NON_FATAL, NO, HTERR_UNAUTHORIZED,
			   NULL, 0, "HTSaveLocally");
	return HTErrorStream();
    }
    if (!HTTmpRoot) {
	if (STREAM_TRACE) TTYPrint(TDEST, "Save File... turned off");
	return HTErrorStream();
    }
	
    /* Let's prompt the user for a file name for this file */
    {
	HTAlertCallback *cbf = HTAlert_find(HT_A_PROMPT);
	HTParentAnchor *anchor = (HTParentAnchor *) HTRequest_anchor(request);
	char *suffix = HTBind_getSuffix(anchor);
	char *deflt = get_filename(HTTmpRoot,HTAnchor_physical(anchor),suffix);
	if (cbf) {
	    HTAlertPar * reply = HTAlert_newReply();
	    if ((*cbf)(request, HT_A_PROMPT, HT_MSG_FILENAME,deflt,NULL,reply))
		filename = HTAlert_replyMessage(reply);
	    HTAlert_deleteReply(reply);
	}
	HT_FREE(suffix);
	HT_FREE(deflt);
	if (filename) {
	    if ((fp = fopen(filename, "wb")) == NULL) {
		HTRequest_addError(request, ERR_NON_FATAL, NO, HTERR_NO_FILE,
				   filename, strlen(filename),"HTSaveLocally");
		HT_FREE(filename);
		return HTErrorStream();
	    }
	} else {
	    if (STREAM_TRACE) TTYPrint(TDEST, "Save File... No file name\n");
	    return HTErrorStream();
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
    if (HTLib_secure()) {
	HTRequest_addError(request, ERR_NON_FATAL, NO, HTERR_UNAUTHORIZED,
			   NULL, 0, "HTSaveLocally");
	return HTErrorStream();
    }
    if (!HTTmpRoot) {
	if (STREAM_TRACE) TTYPrint(TDEST, "Save File... turned off");
	return HTErrorStream();
    }
	
    /* Let's find a hash name for this file without asking user */
    {
	HTParentAnchor *anchor = (HTParentAnchor *) HTRequest_anchor(request);
	char *suffix = HTBind_getSuffix(anchor);
	filename = get_filename(HTTmpRoot, HTAnchor_physical(anchor), suffix);
	HT_FREE(suffix);
	if (filename) {
	    if ((fp = fopen(filename, "wb")) == NULL) {
		HTRequest_addError(request, ERR_NON_FATAL, NO, HTERR_NO_FILE,
				   filename, strlen(filename),"HTSaveAndExecute");
		HT_FREE(filename);
		return HTErrorStream();
	    }
	} else {
	    if (STREAM_TRACE) TTYPrint(TDEST, "Save File... No file name\n");
	    return HTErrorStream();
	}
    }
    
    /* Now we are ready for creating the file writer stream */
    if (fp) {
	HTStream * me = HTFWriter_new(request, fp, NO);
	me->filename = filename;
	if ((me->end_command = (char  *) HT_MALLOC((strlen((char *) param) + 10 + 3*strlen(filename)))) == NULL)
	    HT_OUTOFMEM("SaveAndExecute");
        sprintf (me->end_command, (char *)param, filename, filename, filename);
	return me;
    }
    HT_FREE(filename);
    return HTErrorStream();
}


/*	Save and Call Back
**	------------------
**	This stream works exactly like the TSaveAndExecute
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

