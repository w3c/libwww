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
#include "HTFWrite.h"					 /* Implemented here */

#define HASH_SIZE 1001		/* Tunable */

#define CACHE_LIMIT 100		/* files */

#define CACHE_INFO	".cache_info"
#define INDEX_FILE	".cache_dirindex"
#define WELCOME_FILE	".cache_welcome"
#define TMP_SUFFIX	".cache_tmp"
#define LOCK_SUFFIX	".cache_lock"

PUBLIC int  HTCacheLimit = CACHE_LIMIT;

struct _HTStream {
	CONST HTStreamClass *	isa;
	
	FILE *			fp;
	BOOL			leave_open;     /* Close file? HFN 08/02-94 */
	char * 			end_command;
	BOOL 			remove_on_close;
	BOOL			announce;
	char *			filename;
	HTRequest *		request;	/* saved for callback */
	BOOL (*callback) PARAMS((struct _HTRequest * req, void * filename));
	HTCacheItem *		cache;
};

PRIVATE HTStream	HTBlackHoleInstance;		      /* Made static */

PRIVATE BOOL		HTCacheEnable = NO;
PRIVATE char *		HTCacheRoot = NULL;  	    /* Destination for cache */
PRIVATE HTList *	HTCache = NULL;		  /* List of cached elements */

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
PRIVATE int HTBlackHole_put_character ARGS2(HTStream *, me, char, c)
{
    return HT_OK;
}

PRIVATE int HTBlackHole_put_string ARGS2(HTStream *, me, CONST char*, s)
{
    return HT_OK;
}

PRIVATE int HTBlackHole_write ARGS3(HTStream *, me, CONST char*, s, int, l)
{
    return HT_OK;
}

PRIVATE int HTBlackHole_flush ARGS1(HTStream *, me)
{
    return HT_OK;
}

PRIVATE int HTBlackHole_free ARGS1(HTStream *, me)
{
    return HT_OK;
}

PRIVATE int HTBlackHole_abort ARGS2(HTStream *, me, HTError, e)
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

PUBLIC HTStream * HTBlackHole NOARGS
{
    if (TRACE)
	fprintf(TDEST, "BlackHole... Created\n");
    HTBlackHoleInstance.isa = &HTBlackHoleClass;       /* The rest is random */
    return &HTBlackHoleInstance;
}


/*	HTThroughLine
**	-------------
**
** This function is a dummy function that returns the same output stream
** as given as a parameter. Henrik 01/03-94
*/
PUBLIC HTStream* HTThroughLine ARGS5(
	HTRequest *,		request,
	void *,			param,
	HTFormat,		input_format,
	HTFormat,		output_format,
	HTStream *,		output_stream)	            /* Only one used */
{
    return output_stream;
}

/* ------------------------------------------------------------------------- */
/*  			     SOCKET WRITER STREAM			     */
/* ------------------------------------------------------------------------- */

PRIVATE int HTFWriter_put_character ARGS2(HTStream *, me, char, c)
{
    return (fputc(c, me->fp) == EOF) ? HT_ERROR : HT_OK;
}

PRIVATE int HTFWriter_put_string ARGS2(HTStream *, me, CONST char*, s)
{
    if (*s)				             /* For vms :-( 10/04-94 */
	return (fputs(s, me->fp) == EOF) ? HT_ERROR : HT_OK;
    return HT_OK;
}

PRIVATE int HTFWriter_flush ARGS1(HTStream *, me)
{
    return (fflush(me->fp) == EOF) ? HT_ERROR : HT_OK;
}


PRIVATE int HTFWriter_write ARGS3(HTStream *, me, CONST char*, s, int, l)
{
    int status ;
    status = (fwrite(s, 1, l, me->fp) != l) ? HT_ERROR : HT_OK ;
    if (l > 1 && status == HT_OK)
	(void)HTFWriter_flush( me) ;
    return status ;

/*    return (fwrite(s, 1, l, me->fp) != l) ? HT_ERROR : HT_OK; */
    
}


PRIVATE int HTFWriter_free ARGS1(HTStream *, me)
{
    if (me->cache) {
        time_t finish_time;
	time(&finish_time);
	me->cache->load_delay = finish_time - me->cache->load_time;
	/* Actually, ought to use draft ANSI-C difftime() */
	/* But that I bet is more portable in real life  (@@?) */
    }

    if (me->leave_open != YES) fclose(me->fp);

    if (me->end_command) {		/* Temp file */
	system(me->end_command);	/* @@ Beware of security hole */
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

PRIVATE int HTFWriter_abort ARGS2(HTStream *, me, HTError, e)
{
    if (me->leave_open != YES) fclose(me->fp);
    if (me->end_command) {		/* Temp file */
	if (TRACE)
	    fprintf(TDEST,"FileWriter.. Aborting: file %s not executed.\n",
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

PUBLIC HTStream* HTFWriter_new ARGS2(FILE *, fp, BOOL, leave_open)
{
    HTStream* me;
    
    if (!fp) {
	if (TRACE)
	    fprintf(TDEST, "FileWriter.. Bad file descriptor\n");
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
/*  				 CACHE MANAGER				     */
/* ------------------------------------------------------------------------- */

/*
**	Check that the name we're about to generate doesn't
**	clash with anything used by the caching system.
*/
PRIVATE BOOL reserved_name ARGS1(char *, url)
{
    char * name = strrchr(url, '/');
    char * suff = NULL;

    if (name) name++;
    else name = url;

    if (!strcmp(name, CACHE_INFO) ||
	!strcmp(name, INDEX_FILE) ||
	!strcmp(name, WELCOME_FILE))
	return YES;

    suff = strrchr(name, TMP_SUFFIX[0]);
    if (suff && !strcmp(suff, TMP_SUFFIX))
	return YES;

    suff = strrchr(name, LOCK_SUFFIX[0]);
    if (suff && !strcmp(suff, LOCK_SUFFIX))
	return YES;

    return NO;
}


/*
**	Map url to cache file name.
*/
PRIVATE char * cache_file_name ARGS1(char *, url)
{
    char * access = NULL;
    char * host = NULL;
    char * path = NULL;
    char * cfn = NULL;
    BOOL welcome = NO;
    BOOL res = NO;

    if (!url ||  strchr(url, '?')  ||  (res = reserved_name(url))  ||
	!(access = HTParse(url, "", PARSE_ACCESS)) ||
	(0 != strcmp(access, "http") &&
	 0 != strcmp(access, "ftp")  &&
	 0 != strcmp(access, "gopher"))) {

	if (access) free(access);

	if (res && CACHE_TRACE)
	    fprintf(TDEST,
		    "Cache....... Clash with reserved name (\"%s\")\n",url);

	return NULL;
    }

    host = HTParse(url, "", PARSE_HOST);
    path = HTParse(url, "", PARSE_PATH | PARSE_PUNCTUATION);
    if (path && path[strlen(path)-1] == '/')
	welcome = YES;

    cfn = (char*)malloc(strlen(HTCacheRoot) +
			strlen(access) +
			(host ? strlen(host) : 0) +
			(path ? strlen(path) : 0) +
			(welcome ? strlen(WELCOME_FILE) : 0) + 3);
    if (!cfn) outofmem(__FILE__, "cache_file_name");

    /* Removed extra slash - HF May2,95 */
    sprintf(cfn, "%s%s/%s%s%s", HTCacheRoot, access, host, path,
	    (welcome ? WELCOME_FILE : ""));

    FREE(access); FREE(host); FREE(path);

    /*
    ** This checks that the last component is not too long.
    ** It could check all the components, but the last one
    ** is most important because it could later blow up the
    ** whole gc when reading cache info files.
    ** Operating system handles other cases.
    ** 64 = 42 + 22  and  22 = 42 - 20  :-)
    ** In other words I just picked some number, it doesn't
    ** really matter that much.
    */
    {
	char * last = strrchr(cfn, '/');
	if (!last) last = cfn;
	if ((int)strlen(last) > 64) {
	    if (CACHE_TRACE)
		fprintf(TDEST, "Too long.... cache file name \"%s\"\n", cfn);
	    free(cfn);
	    cfn = NULL;
	}
    }
    return cfn;
}


/*
**	Create directory path for cache file
**
** On exit:
**	return YES
**		if directories created -- after that caller
**		can rely on fopen(cfn,"w") succeeding.
**
*/
PRIVATE BOOL create_cache_place ARGS1(char *, cfn)
{
    struct stat stat_info;
    char * cur = NULL;
    BOOL create = NO;

    if (!cfn  ||  (int)strlen(cfn) <= (int)strlen(HTCacheRoot) + 1)
	return NO;

    cur = cfn + strlen(HTCacheRoot) + 1;

    while ((cur = strchr(cur, '/'))) {
	*cur = 0;
	if (create || HTStat(cfn, &stat_info) == -1) {
	    create = YES;	/* To avoid doing stat()s in vain */
	    if (CACHE_TRACE)
		fprintf(TDEST,"Cache....... creating cache dir \"%s\"\n",cfn);
	    if (MKDIR(cfn, 0777) < 0) {
		if (CACHE_TRACE)
		    fprintf(TDEST,"Cache....... can't create dir `%s\'\n",cfn);
		return NO;
	    }
	} else {
	    if (S_ISREG(stat_info.st_mode)) {
		int len = strlen(cfn);
		char * tmp1 = (char*)malloc(len + strlen(TMP_SUFFIX) + 1);
		char * tmp2 = (char*)malloc(len + strlen(INDEX_FILE) + 2);
		/* time_t t1,t2,t3,t4,t5; */


		sprintf(tmp1, "%s%s", cfn, TMP_SUFFIX);
		sprintf(tmp2, "%s/%s", cfn, INDEX_FILE);

		if (CACHE_TRACE) {
		    fprintf(TDEST,"Cache....... moving \"%s\" to \"%s\"\n",
			    cfn,tmp1);
		    fprintf(TDEST,"and......... creating dir \"%s\"\n",
			    cfn);
		    fprintf(TDEST,"and......... moving \"%s\" to \"%s\"\n",
			    tmp1,tmp2);
		}
		rename(cfn,tmp1);
		(void) MKDIR(cfn, 0777);
		rename(tmp1,tmp2);

/*		if (HTCacheInfo_for(cfn,&t1,&t2,&t3,&t4,&t5)) {
		    if (CACHE_TRACE)
		       fprintf(TDEST,"Adding...... info entry for %s\n",tmp2);
		    if (!HTCacheInfo_writeEntryFor(tmp2,t1,t2,t3,t4,t5))
			HTLog_error2("Can't write cache info entry for",tmp2);
		}
*/
		free(tmp1);
		free(tmp2);
	    }
	    else {
		if (CACHE_TRACE)
		    fprintf(TDEST,"Cache....... dir \"%s\" already exists\n",
			    cfn);
	    }
	}
	*cur = '/';
	cur++;
    }
    return YES;
}


/*	Create a cache path
**	-------------------
**	Find a full path name for the cache file and create the path if it
**	does not already exist. Returns name or NULL
**	HWL 22/9/94
**	HWL added support for hierachical structure
*/
PRIVATE char *HTCache_getName ARGS1(char *, url)
{
     char *filename = cache_file_name(url);
     if (!filename)
	 return NULL;
     if (create_cache_place(filename))
	 return(filename);
     return NULL;
}


PRIVATE void HTCache_remove ARGS2(HTList *, list, HTCacheItem *, item)
{
    if (CACHE_TRACE)
	fprintf(TDEST, "Cache: Removing %s\n", item->filename);
    HTList_removeObject(list, item);
    HTList_removeObject(item->anchor->cacheItems, item);
    REMOVE(item->filename);

    /* HWL 22/9/94: since we added a hierachical file structure, we should also clean up */
    {
	char * p;

	while ((p = strrchr(item->filename,'/')) && (p != NULL)){
	    item->filename[p - item->filename] = 0; /* this will be freed in a sec */
	    if (strcmp(item->filename, HTCacheRoot) != 0) {
		if (CACHE_TRACE) 
		    fprintf(TDEST, "rmdir %s\n", item->filename);
		RMDIR(item->filename);         /* this will luckily fail if directory is not empty */
	    }
	}
    }

    free(item->filename);
    free(item);
}

/*
**	This can be called for the main list or an anchor's list
*/
PUBLIC void HTCacheClear ARGS1(HTList *, list)
{
    HTCacheItem * item;
    while ((item = (HTCacheItem *) HTList_objectAt(list, 0)) != NULL) {
        HTCache_remove(list, item);
    }
}


/*
**   This function removes ALL cache files known to this session. The anchors
**   ARE updated, but normally this function is for exiting purposes.
*/
PUBLIC void HTCacheDeleteAll NOARGS
{
    HTCacheItem * item;
    while ((item = (HTCacheItem *) HTList_objectAt(HTCache, 0)) != NULL) {
        HTCache_remove(HTCache, item);
    }
}

/*  Remove a file from the cache to prevent too many files from being cached
*/
PRIVATE void limit_cache ARGS1(HTList * , list)
{
    HTList * cur = list;
    HTCacheItem * item;
    time_t best_delay = 0;   /* time_t in principle can be any arith type */
    HTCacheItem* best_item = NULL;

    if (HTList_count(list) < HTCacheLimit) return;   /* Limit not reached */

    while (NULL != (item = (HTCacheItem*)HTList_nextObject(cur))) {
        if (best_delay == 0  ||  item->load_delay < best_delay) {
            best_delay = item->load_delay;
            best_item = item;
        }
    }

    if (best_item) HTCache_remove(list, best_item);
}

/*	Enable Cache
**	------------
**	If `cache_root' is NULL then reuse old value or use HT_CACHE_ROOT.
**	An empty string will make '/' as cache root
*/
PUBLIC BOOL HTCache_enable ARGS1(CONST char *, cache_root)
{
    if (cache_root)
	HTCache_setRoot(cache_root);
    HTCacheEnable = YES;
    return YES;
}


/*	Disable Cache
**	------------
**	Turns off the cache. Note that the cache can be disabled and enabled
**	at any time. The cache root is kept and can be reused during the
**	execution.
*/
PUBLIC BOOL HTCache_disable NOARGS
{
    HTCacheEnable = NO;
    return YES;
}

/*	Is Cache Enabled
**	----------------
**	Returns YES or NO. Also makes sure that we have a root value
**	(even though it might be invalid)
*/
PUBLIC BOOL HTCache_isEnabled NOARGS
{
    if (!HTSecure && HTCacheEnable) {
	if (!HTCacheRoot)
	    HTCache_setRoot(NULL);
	return YES;
    }
    return NO;
}


/*	Set Cache Root
**	--------------
**	If `cache_root' is NULL then the current value (might be a define)
**	Should we check if the cache_root is actually OK? I think not!
*/
PUBLIC BOOL HTCache_setRoot ARGS1(CONST char *, cache_root)
{
    StrAllocCopy(HTCacheRoot, cache_root ? cache_root : HT_CACHE_ROOT);
    if (*(HTCacheRoot+strlen(HTCacheRoot)-1) != '/')
	StrAllocCat(HTCacheRoot, "/");
    if (CACHE_TRACE)
	fprintf(TDEST, "Cache Root.. Root set to `%s\'\n", HTCacheRoot);
    return YES;
}


/*	Get Cache Root
**	--------------
*/
PUBLIC CONST char * HTCache_getRoot NOARGS
{
    return HTCacheRoot;
}


/*	Free Cache Root
**	--------------
**	For clean up memory
*/
PUBLIC void HTCache_freeRoot NOARGS
{
    FREE(HTCacheRoot);
}


/*	Cache Writer
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
    HTStream* me;

    if (HTSecure) {
	if (CACHE_TRACE)
	    fprintf(TDEST, "Only caching if WWW is run locally.\n");
	return HTBlackHole();
    }

    /* Get a file name */
    if ((fnam = HTCache_getName(HTAnchor_physical(request->anchor))) == NULL)
	return HTBlackHole();

    me = (HTStream*)calloc(sizeof(*me),1);
    if (me == NULL) outofmem(__FILE__, "CacheWriter");
    me->isa = &HTFWriter;  
    me->end_command = NULL;
    me->remove_on_close = NO;	/* If needed, put into end_command */
    me->announce = NO;
    me->filename = NULL;
    if ((me->fp = fopen (fnam, "w")) == NULL) {
	HTAlert("Can't open local file for writing.");
	if (CACHE_TRACE)
	    fprintf(TDEST, "HTStream: can't open %s for writing\n",fnam);
	free(fnam);
	free(me);
	return HTBlackHole();
    }
    
    /* Set up a cache record */
    if (CACHE_TRACE)
	fprintf(TDEST, "Cache....... Creating file %s\n", fnam);
    if ((me->cache = (HTCacheItem*)calloc(sizeof(*me->cache),1)) == NULL)
	outofmem(__FILE__, "cache");
    time(&me->cache->load_time);
    StrAllocCopy(me->cache->filename, fnam);
    me->cache->format = input_format;
    me->callback = request->callback;
    me->request = request;	/* won't be freed */
    me->filename = fnam;   /* will be freed */

    /* Make binding to he anchor structure */
    me->cache->anchor = request->anchor;
    if (!request->anchor->cacheItems)
    	request->anchor->cacheItems = HTList_new();
    HTList_addObject(request->anchor->cacheItems, me->cache);

    /* Keep a global list of all cache items */
    if (!HTCache) HTCache = HTList_new();
    HTList_addObject(HTCache, me->cache);
    limit_cache(HTCache);		 /* Limit number (not size) of files */
    
    return me;
}

/* ------------------------------------------------------------------------- */
/*  			     FILE WRITER ROUTINES			     */
/* ------------------------------------------------------------------------- */

/*	Set TMP Root
**	--------------
**	If `tmp_root' is NULL use the current value (might be a define)
*/
PUBLIC BOOL HTTmp_setRoot ARGS1(CONST char *, tmp_root)
{
    StrAllocCopy(HTTmpRoot, tmp_root ? tmp_root : HT_TMP_ROOT);
    if (*(HTTmpRoot+strlen(HTTmpRoot)-1) != '/')
	StrAllocCat(HTTmpRoot, "/");
    if (TRACE)
	fprintf(TDEST, "Tmp Root.... Root set to `%s\'\n", HTTmpRoot);
    return YES;
}


/*	Get Tmp Root
**	--------------
*/
PUBLIC CONST char * HTTmp_getRoot NOARGS
{
    return HTTmpRoot;
}


/*	Free Tmp Root
**	--------------
**	For clean up memory
*/
PUBLIC void HTTmp_freeRoot NOARGS
{
    FREE(HTTmpRoot);
}

/*
**   This function tries really hard to find a non-existent filename relative
**   to the path given. Returns a string that must be freed by the caller or
**   NULL on error. The base must be '/' terminated which!
*/
PRIVATE char *get_filename ARGS3(char *, base, CONST char *, url,
				 CONST char *, suffix)
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
PUBLIC HTStream* HTSaveAndExecute ARGS5(
	HTRequest *,		request,
	void *,			param,
	HTFormat,		input_format,
	HTFormat,		output_format,
	HTStream *,		output_stream)
{
    char *fnam;
    HTStream* me;
    
    if (HTSecure) {
        HTAlert("Can't save data to file -- please run WWW locally");
	return HTBlackHole();
    }
    
    if (!HTTmpRoot) {
	if (TRACE) fprintf(TDEST, "Save and execute turned off");
	return HTBlackHole();
    }
	
    /* Let's find a hash name for this file */
    {
	char *suffix = HTBind_getSuffix(request->anchor);
	fnam = get_filename(HTTmpRoot, HTAnchor_physical(request->anchor),
			    suffix);
	FREE(suffix);
	if (!fnam) {
	    HTAlert("Can't find a suitable file name");
	    return HTBlackHole();
	}
    }

    me = (HTStream*)calloc(sizeof(*me), 1);
    if (me == NULL) outofmem(__FILE__, "Save and execute");
    me->isa = &HTFWriter;  
    me->request = request;	/* won't be freed */    
    me->fp = fopen (fnam, "w");
    if (!me->fp) {
	HTAlert("Can't open temporary file!");
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
PUBLIC HTStream* HTSaveLocally ARGS5(
	HTRequest *,		request,
	void *,			param,
	HTFormat,		input_format,
	HTFormat,		output_format,
	HTStream *,		output_stream)	/* Not used */

{
    char *fnam = NULL;
    char *answer = NULL;
    HTStream* me;
    
    if (HTSecure) {
        HTAlert("Can't save data to file -- please run WWW locally");
	return HTBlackHole();
    }

    if (!HTTmpRoot) {
	if (TRACE) fprintf(TDEST, "Save locally turned off");
	return HTBlackHole();
    }
	
    /* Let's find a file name for this file */
    {
	char *suffix = HTBind_getSuffix(request->anchor);
	fnam = get_filename(HTTmpRoot, HTAnchor_physical(request->anchor),
			    suffix);
	answer = HTPrompt("Give name of file to save in", fnam ? fnam : "");
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
    
    me->fp = fopen (answer, "w");
    if (!me->fp) {
	HTAlert("Can't open local file to write into.");
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
	return HTBlackHole();
   } else {
   	me = HTCacheWriter(request, param,
			    input_format, output_format, output_stream);
	if (me) {
	    me->callback = request->callback;
	}
   }
   return me;   
}

