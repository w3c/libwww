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
#include "HTAlert.h"
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
	CONST HTStreamClass *	isa;
	
	FILE *			fp;
	BOOL			leave_open;     /* Close file? HFN 08/02-94 */
	char * 			end_command;
	BOOL 			remove_on_close;
	BOOL			announce;
	char *			filename;
	HTRequest *		request;	/* saved for callback */
	HTRequestCallback	*callback;
};

PRIVATE HTStream	HTBlackHoleInstance;		      /* Made static */
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
    if (STREAM_TRACE)
	TTYPrint(TDEST, "BlackHole... Created\n");
    HTBlackHoleInstance.isa = &HTBlackHoleClass;       /* The rest is random */
    return &HTBlackHoleInstance;
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
    if (l > 1 && status == HT_OK)
	(void)HTFWriter_flush( me) ;
    return status ;

/*    return (fwrite(s, 1, l, me->fp) != l) ? HT_ERROR : HT_OK; */
    
}


PRIVATE int HTFWriter_free (HTStream * me)
{
    if (me->leave_open != YES) fclose(me->fp);

    if (me->end_command) {		/* Temp file */
#ifdef GOT_SYSTEM
	system(me->end_command);	/* @@ Beware of security hole */
#endif
	free (me->end_command);
	if (me->remove_on_close) {
	    REMOVE(me->filename);
	}
    }
    if (me->callback) {
        (*me->callback)(me->request, me->filename);
    }
    if (me->filename) free(me->filename);
    free(me);
    return HT_OK;
}

PRIVATE int HTFWriter_abort (HTStream * me, HTList * e)
{
    if (me->leave_open != YES) fclose(me->fp);
    if (me->end_command) {		/* Temp file */
	if (STREAM_TRACE)
	    TTYPrint(TDEST,"FileWriter.. Aborting: file %s not executed.\n",
		    me->filename ? me->filename : "???" );
	free (me->end_command);
	if (me->remove_on_close) {
	    REMOVE(me->filename);
	}
    }

    if (me->filename) free(me->filename);
    free(me);
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

PUBLIC HTStream* HTFWriter_new (FILE * fp, BOOL leave_open)
{
    HTStream* me;
    
    if (!fp) {
	if (STREAM_TRACE)
	    TTYPrint(TDEST, "FileWriter.. Bad file descriptor\n");
	return NULL;
    }
    me = (HTStream*)calloc(sizeof(*me),1);
    if (me == NULL) outofmem(__FILE__, "HTFWriter_new");
    me->isa = &HTFWriter;       

    me->fp = fp;
    me->leave_open = leave_open;		/* HENRIK 08/02-94 */
    me->end_command = NULL;
    me->remove_on_close = NO;
    me->announce = NO;
    me->callback = NULL;
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
    FREE(HTTmpRoot);
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


/*	Take action using a system command
**	----------------------------------
**
**	Creates temporary file, writes to it, executes system command
**	on end-document.  The suffix of the temp file can be given
**	in case the application is fussy, or so that a generic opener can
**	be used.
*/
PUBLIC HTStream* HTSaveAndExecute (HTRequest *	request,
				   void *	param,
				   HTFormat	input_format,
				   HTFormat	output_format,
				   HTStream *	output_stream)
{
    char *fnam;
    HTStream* me;
    
    if (HTSecure) {
        HTAlert(request, "Can't save data to file -- please run WWW locally");
	return HTBlackHole();
    }
    
    if (!HTTmpRoot) {
	if (STREAM_TRACE) TTYPrint(TDEST, "Save and execute turned off");
	return HTBlackHole();
    }
	
    /* Let's find a hash name for this file */
    {
	HTParentAnchor *anchor = (HTParentAnchor *) HTRequest_anchor(request);
	char *suffix = HTBind_getSuffix(anchor);
	fnam = get_filename(HTTmpRoot, HTAnchor_physical(anchor), suffix);
	FREE(suffix);
	if (!fnam) {
	    HTAlert(request, "Can't find a suitable file name");
	    return HTBlackHole();
	}
    }

    me = (HTStream*)calloc(sizeof(*me), 1);
    if (me == NULL) outofmem(__FILE__, "Save and execute");
    me->isa = &HTFWriter;  
    me->request = request;	/* won't be freed */    
    me->fp = fopen (fnam, "wb");
    if (!me->fp) {
	HTAlert(request, "Can't open temporary file!");
        free(fnam);
	free(me);
	return HTBlackHole();
    }
    StrAllocCopy(me->filename, fnam);

    /* Make command to process file */
    me->end_command = (char *) malloc ((strlen((char *) param) + 10 +
					3*strlen(fnam)) * sizeof (char));
    if (me == NULL) outofmem(__FILE__, "SaveAndExecute");
    
    sprintf (me->end_command, (char *) param, fnam, fnam, fnam);
    me->remove_on_close = NO;
    me->announce = NO;
    free (fnam);
    return me;
}


/*	Save Locally
**	------------
**
**  Bugs:
**	GUI Apps should open local Save panel here really.
**
*/
PUBLIC HTStream* HTSaveLocally (HTRequest *	request,
				void *		param,
				HTFormat	input_format,
				HTFormat	output_format,
				HTStream *	output_stream)
{
    char *fnam = NULL;
    char *answer = NULL;
    HTStream* me;
    
    if (HTSecure) {
        HTAlert(request, "Can't save data to file -- please run WWW locally");
	return HTBlackHole();
    }

    if (!HTTmpRoot) {
	if (STREAM_TRACE) TTYPrint(TDEST, "Save locally turned off");
	return HTBlackHole();
    }
	
    /* Let's find a file name for this file */
    {
	HTParentAnchor *anchor = (HTParentAnchor *) HTRequest_anchor(request);
	char *suffix = HTBind_getSuffix(anchor);
	fnam = get_filename(HTTmpRoot, HTAnchor_physical(anchor), suffix);
	answer = HTPrompt(request, "Give name of file to save in",
			  fnam ? fnam : "");
	if (!answer) {
	    FREE(fnam);
	    return HTBlackHole();
	}
	FREE(suffix);
	FREE(fnam);
    }
    
    me = (HTStream*)calloc(sizeof(*me),1);
    if (me == NULL) outofmem(__FILE__, "SaveLocally");
    me->isa = &HTFWriter;  
    me->announce = YES;
    
    me->fp = fopen (answer, "wb");
    if (!me->fp) {
	HTAlert(request, "Can't open local file to write into.");
        FREE(answer);
	free(me);
	return HTBlackHole();
    }
    me->callback = NULL;
    me->request = request;	/* won't be freed */
    me->filename = answer;	/* Will be freed */
    return me;
}


/*	Save and Call Back
**	------------------
**
**
**	The special case is a kludge. Better is everything uses streams
**	and nothing uses files.  Then this routine will go too. :-))
*/
PUBLIC HTStream* HTSaveAndCallback (HTRequest *		request,
				    void *		param,
				    HTFormat		input_format,
				    HTFormat		output_format,
				    HTStream *		output_stream)
{
   HTStream * me;
   
#if 0
   if (request->using_cache) {  /* Special case! file wanted && cache hit */
        (*request->callback)(request,
			 ((HTCache*) request->using_cache)->filename);
	return HTBlackHole();
   } else {
   	me = HTCacheWriter(request, param,
			    input_format, output_format, output_stream);
	if (me) {
	    me->callback = request->callback;
	}
   }
#endif
   return me;   
}

