/*		FILE WRITER				HTFWrite.h
**		===========
**
**	This version of the stream object just writes to a C file.
**	The file is assumed open and left open.
**
**	Bugs:
**		strings written must be less than buffer size.
*/
#define CACHE_LIMIT 100		/* files */

#include "HTFWriter.h"

#include "HTFormat.h"
#include "HTAlert.h"
#include "HTFile.h"
#include "HTList.h"

/*		Stream Object
**		------------
*/

struct _HTStream {
	CONST HTStreamClass *	isa;
	
	FILE *			fp;
	char * 			end_command;
	char * 			remove_command;
	BOOL			announce;
	char *			filename;
	HTRequest *		request;	/* saved for callback */
	BOOL (*callback) PARAMS((struct _HTRequest * req, void * filename));
	HTCacheItem *		cache;
};


/*_________________________________________________________________________
**
**		B L A C K    H O L E    C L A S S
**
**	There is only one black hole instance shared by anyone
**	who wants a black hole.  These black holes don't radiate,
**	they just absorb data.
*/
PRIVATE void HTBlackHole_put_character ARGS2(HTStream *, me, char, c)
{}
PRIVATE void HTBlackHole_put_string ARGS2(HTStream *, me, CONST char*, s)
{}
PRIVATE void HTBlackHole_write ARGS3(HTStream *, me, CONST char*, s, int, l)
{}
PRIVATE void HTBlackHole_free ARGS1(HTStream *, me)
{}
PRIVATE void HTBlackHole_abort ARGS2(HTStream *, me, HTError, e)
{}


/*	Black Hole stream
**	-----------------
*/
PRIVATE CONST HTStreamClass HTBlackHoleClass =
{		
	"BlackHole",
	HTBlackHole_free,
	HTBlackHole_abort,
	HTBlackHole_put_character, 	HTBlackHole_put_string,
	HTBlackHole_write
}; 

PRIVATE HTStream HTBlackHoleInstance =
{
	&HTBlackHoleClass,
	NULL,
	NULL,
	NULL,

	NO,
	NULL,
	NULL,
	NULL,
};

/*	Black hole creation
*/
PUBLIC HTStream * HTBlackHole NOARGS
{
    return &HTBlackHoleInstance;
}


/*_________________________________________________________________________
**
**		F I L E     A C T I O N 	R O U T I N E S
**  Bug:
**	All errors are ignored.
*/

/*	Character handling
**	------------------
*/

PRIVATE void HTFWriter_put_character ARGS2(HTStream *, me, char, c)
{
    putc(c, me->fp);
}



/*	String handling
**	---------------
**
**	Strings must be smaller than this buffer size.
*/
PRIVATE void HTFWriter_put_string ARGS2(HTStream *, me, CONST char*, s)
{
    fputs(s, me->fp);
}


/*	Buffer write.  Buffers can (and should!) be big.
**	------------
*/
PRIVATE void HTFWriter_write ARGS3(HTStream *, me, CONST char*, s, int, l)
{
    fwrite(s, 1, l, me->fp); 
}




/*	Free an HTML object
**	-------------------
**
**	Note that the SGML parsing context is freed, but the created
**	object is not,
**	as it takes on an existence of its own unless explicitly freed.
*/
PRIVATE void HTFWriter_free ARGS1(HTStream *, me)
{
    if (me->cache) {
        time_t finish_time;
	time(&finish_time);
	me->cache->load_delay = finish_time - me->cache->load_time;
	/* Actually, ought to use darf t ANSII difftime() */
	/* But that I bet is more portable in real life  (@@?) */
    }
    fclose(me->fp);
    if (me->end_command) {		/* Temp file */
        HTProgress(me->end_command);	/* Tell user what's happening */
	system(me->end_command);
	free (me->end_command);
	if (me->remove_command) {
	    system(me->remove_command);
	    free(me->remove_command);
	}
    }
    if (me->callback) {
        (*me->callback)(me->request, me->filename);
    }
    if (me->filename) free(me->filename);
    free(me);
}

/*	End writing
*/

PRIVATE void HTFWriter_abort ARGS2(HTStream *, me, HTError, e)
{
    fclose(me->fp);
    if (me->end_command) {		/* Temp file */
	if (TRACE) fprintf(stderr,
		"HTFWriter: Aborting: file %s not executed.\n",
		me->filename ? me->filename : "???" );
	free (me->end_command);
	if (me->remove_command) {
	    system(me->remove_command);
	    free(me->remove_command);
	}
    }

    if (me->filename) free(me->filename);
    free(me);
}



/*	Structured Object Class
**	-----------------------
*/
PRIVATE CONST HTStreamClass HTFWriter = /* As opposed to print etc */
{		
	"FileWriter",
	HTFWriter_free,
	HTFWriter_abort,
	HTFWriter_put_character, 	HTFWriter_put_string,
	HTFWriter_write
}; 


/*	Subclass-specific Methods
**	-------------------------
*/

PUBLIC HTStream* HTFWriter_new ARGS1(FILE *, fp)
{
    HTStream* me;
    
    if (!fp) return NULL;

    me = (HTStream*)calloc(sizeof(*me),1);
    if (me == NULL) outofmem(__FILE__, "HTML_new");
    me->isa = &HTFWriter;       

    me->fp = fp;
    me->end_command = NULL;
    me->remove_command = NULL;
    me->announce = NO;
    me->callback = NULL;
    return me;
}

/*	Make system command from template
**	---------------------------------
**
**	See mailcap spec for description of template.
*/
/* @@ to be written.  sprintfs will do for now.  */



/*	Take action using a system command
**	----------------------------------
**
**	Creates temporary file, writes to it, executes system command
**	on end-document.  The suffix of the temp file can be given
**	in case the application is fussy, or so that a generic opener can
**	be used.
*/
PUBLIC HTStream* HTSaveAndExecute ARGS5(
	HTRequest *,		request,
	void *,			param,
	HTFormat,		input_format,
	HTFormat,		output_format,
	HTStream *,		output_stream)

#ifdef unix
#define REMOVE_COMMAND "/bin/rm -f %s\n"	/* @@@ security @@@ */
#endif
#ifdef VMS
#define REMOVE_COMMAND "delete/noconfirm/nolog %s.."
#endif

#ifdef REMOVE_COMMAND
{
    char *fnam;
    CONST char * suffix;
    
    HTStream* me;
    
    if (HTSecure) {
        HTAlert("Can't save data to file -- please run WWW locally");
	return HTBlackHole();
    }
    
    me = (HTStream*)calloc(sizeof(*me), 1);
    if (me == NULL) outofmem(__FILE__, "Save and execute");
    me->isa = &HTFWriter;  
    
    /* Save the file under a suitably suffixed name */
    
    suffix = HTFileSuffix(input_format);

    fnam = (char *)malloc (L_tmpnam + 16 + strlen(suffix));
    tmpnam (fnam);
    if (suffix) strcat(fnam, suffix);
    me->request = request;	/* won't be freed */
    
    me->fp = fopen (fnam, "w");
    if (!me->fp) {
	HTAlert("Can't open temporary file!");
        free(fnam);
	free(me);
	return NULL;
    }
    StrAllocCopy(me->filename, fnam);

/*	Make command to process file
*/
    me->end_command = (char *)malloc (
    			(strlen (param) + 10+ 3*strlen(fnam))
    			 * sizeof (char));
    if (me == NULL) outofmem(__FILE__, "SaveAndExecute");
    
    sprintf (me->end_command, param, fnam, fnam, fnam);

    me->remove_command = NULL;	/* If needed, put into end_command */
#ifdef NOPE
/*	Make command to delete file
*/ 
    me->remove_command = (char *)malloc (
    			(strlen (REMOVE_COMMAND) + 10+ strlen(fnam))
    			 * sizeof (char));
    if (me == NULL) outofmem(__FILE__, "SaveAndExecute");
    
    sprintf (me->remove_command, REMOVE_COMMAND, fnam);
#endif

    me->announce = NO;
    free (fnam);
    return me;
}

#else	/* can't do remove */
{ return NULL; }
#endif


/*	Save Locally
**	------------
**
**  Bugs:
**	GUI Apps should open local Save panel here really.
**
*/
PUBLIC HTStream* HTSaveLocally ARGS5(
	HTRequest *,		request,
	void *,			param,
	HTFormat,		input_format,
	HTFormat,		output_format,
	HTStream *,		output_stream)	/* Not used */

{
    char *fnam;
    CONST char * suffix;
    char * answer;
    HTStream* me;
    
    if (HTClientHost) {
        HTAlert("Can't save data to file -- please run WWW locally");
	return HTBlackHole();
    }
    
    me = (HTStream*)calloc(sizeof(*me),1);
    if (me == NULL) outofmem(__FILE__, "SaveLocally");
    me->isa = &HTFWriter;  
    me->announce = YES;
    
    /* Save the file under a suitably suffixed name */
    
    suffix = HTFileSuffix(input_format);

    fnam = (char *)malloc (L_tmpnam + 16 + strlen(suffix));
    tmpnam (fnam);
    if (suffix) strcat(fnam, suffix);
    
    /*	Save Panel */
    answer = HTPrompt("Give name of file to save in", fnam);
    
    free(fnam);
    
    me->fp = fopen (answer, "w");
    if (!me->fp) {
	HTAlert("Can't open local file to write into.");
        free(answer);
	free(me);
	return NULL;
    }
    me->callback = NULL;
    me->request = request;	/* won't be freed */
    me->filename = answer;	/* Will be freed */
    return me;
}

/*	Cache handling
**	--------------
*/

PUBLIC HTList * HTCache = NULL;
PUBLIC int	HTCacheLimit = CACHE_LIMIT;

PRIVATE void HTCache_remove ARGS2(HTList *, list, HTCacheItem *, item)
{
    if (TRACE) fprintf(stderr, "Cache: Removing %s\n", item->filename);
    HTList_removeObject(list, item);
    HTList_removeObject(item->anchor->cacheItems, item);
    unlink(item->filename);
    free(item->filename);
    free(item);
}

/* This can be called for the main list or an anchor's list
*/

int fred;

PUBLIC void HTCacheClear (HTList * list)
{
    HTCacheItem * item;
    while ((item=HTList_objectAt(list, 0)) != NULL) {
        HTCache_remove(list, item);
    }
}

/*  Remove a file from the cache to prevent too many files from being cached
*/
PRIVATE void limit_cache ARGS1(HTList * , list)
{
    HTCacheItem * item;
    int i;
    int n = HTList_count(list);
    time_t best_delay = 0;   /* time_t in principle can be any arith type */
    HTCacheItem* best_item = NULL;
    
    if (n < HTCacheLimit) return;		/* Limit not reached */
    
    for (i=0; i<n; i++) {
	if (item->load_delay < best_delay) {
	    best_delay = item->load_delay;
	    best_item = item;
	}
    }
    
    if (best_item) HTCache_remove(list, best_item);
}


/*	Save and Call Back
**	------------------
**
*/
PUBLIC HTStream* HTCacheWriter ARGS5(
	HTRequest *,		request,
	void *,			param,
	HTFormat,		input_format,
	HTFormat,		output_format,
	HTStream *,		output_stream)

{
    char *fnam;
    CONST char * suffix;
    
    HTStream* me;
    
    if (HTClientHost) {
        HTAlert("Can't save data to file -- please run WWW locally");
	return HTBlackHole();
    }
    
    me = (HTStream*)calloc(sizeof(*me),1);
    if (me == NULL) outofmem(__FILE__, "SaveLocally");
    me->isa = &HTFWriter;  
    me->end_command = NULL;
    me->remove_command = NULL;	/* If needed, put into end_command */
    me->announce = NO;
    
    /* Save the file under a suitably suffixed name */
    
    suffix = HTFileSuffix(input_format);

    fnam = (char *)malloc (L_tmpnam + 16 + strlen(suffix));
    tmpnam (fnam);
    if (suffix) strcat(fnam, suffix);
    me->filename = NULL;
    
    limit_cache(HTCache);		/* Limit number (not size) of files */
    
    me->fp = fopen (fnam, "w");
    if (!me->fp) {
	HTAlert("Can't open local file to write into for callback.");
	free(fnam);
	free(me);
	return NULL;
    }
    
    /* Set up a cache record */
    
    if (TRACE) fprintf(stderr, "Cache: Creating file %s\n", fnam);
    me->cache = (HTCacheItem*)calloc(sizeof(*me->cache),1);
    if (!me->cache)outofmem(__FILE__, "cache");
    time(&me->cache->load_time);
    StrAllocCopy(me->cache->filename, fnam);
    me->cache->anchor = request->anchor;
    if (!request->anchor->cacheItems)
    	request->anchor->cacheItems = HTList_new();
    HTList_addObject(request->anchor->cacheItems, me->cache);
    me->cache->format = input_format;
    
    if (!HTCache) HTCache = HTList_new();
    HTList_addObject(HTCache, me->cache);
    
    me->callback = request->callback;
    me->request = request;	/* won't be freed */
    me->filename = fnam;   /* will be freed */
    return me;
}

/*	Save and Call Back
**	------------------
**
**
**	The special case is a kludge. Better is everything uses streams
**	and nothing uses files.  Then this routine will go too. :-))
*/


PUBLIC HTStream* HTSaveAndCallBack ARGS5(
	HTRequest *,		request,
	void *,			param,
	HTFormat,		input_format,
	HTFormat,		output_format,
	HTStream *,		output_stream)
{
   HTStream * me;
   
   if (request->using_cache) {  /* Special case! file wanted && cache hit */
        (*request->callback)(request,
			 ((HTCacheItem*)request->using_cache)->filename);
	return &HTBlackHoleInstance;	/* @@@@@@@@@@@@@@ */
   } else {
   	me = HTCacheWriter(request, param,
			    input_format, output_format, output_stream);
	if (me) {
	    me->callback = request->callback;
	}
   }
   return me;   
}

