/*							       	    HTFWriter.c
**	FILE WRITER
**
**	(c) COPYRIGHT CERN 1994.
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

#include "sysdep.h"

#define CACHE_LIMIT 100		/* files */

#include "HTFWriter.h"

#include "HTFormat.h"
#include "HTAlert.h"
#include "HTFile.h"
#include "HTList.h"
#include "HTParse.h"

#define CACHE_INFO	".cache_info"
#define INDEX_FILE	".cache_dirindex"
#define WELCOME_FILE	".cache_welcome"
#define TMP_SUFFIX	".cache_tmp"
#define LOCK_SUFFIX	".cache_lock"
#define INFO_LINE_LEN	256


/************************************************************************
**
**	FILENAME GENERATION
**
**	Mapping between URLs and cache files
**
*************************************************************************
*/


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
PRIVATE char * cache_file_name ARGS2(char *, base, char *, url)
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

	if (res && TRACE)
	    fprintf(stderr,
		    "Cache....... Clash with reserved name (\"%s\")\n",url);

	return NULL;
    }

    host = HTParse(url, "", PARSE_HOST);
    path = HTParse(url, "", PARSE_PATH | PARSE_PUNCTUATION);
    if (path && path[strlen(path)-1] == '/')
	welcome = YES;

    if (host) {		/* Canonilize host name */
	char * cur = host;
	while (*cur) {
	    *cur = TOLOWER(*cur);
	    cur++;
	}
    }

    cfn = (char*)malloc(strlen(base) +
			strlen(access) +
			(host ? strlen(host) : 0) +
			(path ? strlen(path) : 0) +
			(welcome ? strlen(WELCOME_FILE) : 0) + 3);
    if (!cfn) outofmem(__FILE__, "cache_file_name");
   sprintf(cfn, "%s/%s/%s%s%s", base, access, host, path,
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
	    CTRACE(stderr, "Too long.... cache file name \"%s\"\n", cfn);
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
PRIVATE BOOL create_cache_place ARGS2(char *, base, char *, cfn)
{
    struct stat stat_info;
    char * cur = NULL;
    BOOL create = NO;

    if (!cfn  ||  (int)strlen(cfn) <= (int)strlen(base) + 1)
	return NO;

    cur = cfn + strlen(base) + 1;

    while ((cur = strchr(cur, '/'))) {
	*cur = 0;
	if (create || HTStat(cfn, &stat_info) == -1) {
	    create = YES;	/* To avoid doing stat()s in vain */
	    CTRACE(stderr, "Cache....... creating cache dir \"%s\"\n", cfn);
	    if (-1 == mkdir(cfn, 0777)) { /* howcome 30/10/94  changed from 755 to allow several browser to write */
		CTRACE(stderr, "Cache....... can't create dir \"%s\"\n", cfn);
		return NO;
	    }
	}
	else {
	    if (S_ISREG(stat_info.st_mode)) {
		int len = strlen(cfn);
		char * tmp1 = (char*)malloc(len + strlen(TMP_SUFFIX) + 1);
		char * tmp2 = (char*)malloc(len + strlen(INDEX_FILE) + 2);
		/* time_t t1,t2,t3,t4,t5; */


		sprintf(tmp1, "%s%s", cfn, TMP_SUFFIX);
		sprintf(tmp2, "%s/%s", cfn, INDEX_FILE);

		if (TRACE) {
		    fprintf(stderr,"Cache....... moving \"%s\" to \"%s\"\n",
			    cfn,tmp1);
		    fprintf(stderr,"and......... creating dir \"%s\"\n",
			    cfn);
		    fprintf(stderr,"and......... moving \"%s\" to \"%s\"\n",
			    tmp1,tmp2);
		}
		rename(cfn,tmp1);
		mkdir(cfn,0777);   /* howcome 30/10/94  changed from 755 to allow several browser to write */
		rename(tmp1,tmp2);

/*		if (HTCacheInfo_for(cfn,&t1,&t2,&t3,&t4,&t5)) {
		    CTRACE(stderr,"Adding...... info entry for %s\n",tmp2);
		    if (!HTCacheInfo_writeEntryFor(tmp2,t1,t2,t3,t4,t5))
			HTLog_error2("Can't write cache info entry for",tmp2);
		}
*/
		free(tmp1);
		free(tmp2);
	    }
	    else {
		CTRACE(stderr,"Cache....... dir \"%s\" already exists\n",cfn);
	    }
	}
	*cur = '/';
	cur++;
    }
    return YES;
}















/*		Stream Object
**		------------
*/

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
PRIVATE void HTBlackHole_put_character ARGS2(HTStream *, me, char, c)
{
    return;
}

PRIVATE void HTBlackHole_put_string ARGS2(HTStream *, me, CONST char*, s)
{
    return;
}

PRIVATE void HTBlackHole_write ARGS3(HTStream *, me, CONST char*, s, int, l)
{
    return;
}

PRIVATE int HTBlackHole_free ARGS1(HTStream *, me)
{
    return 0;
}

PRIVATE int HTBlackHole_abort ARGS2(HTStream *, me, HTError, e)
{
    return EOF;
}


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
	NO,				/* HENRIK 08/02-94 */
	NULL,
	NO,
	NO,
	NULL,
	NULL,
	NULL,
	NULL,
};

/* Black hole creation */
PUBLIC HTStream * HTBlackHole NOARGS
{
    if (TRACE)
	fprintf(stderr, "BlackHole... Created\n");
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



/*_________________________________________________________________________
**
**		F I L E     A C T I O N 	R O U T I N E S
**  Bug:
**	All errors are ignored.
*/




/* -------------------------------------------------------------------------
   This function tries really hard to find a non-existent filename relative
   to the path given.
       path:	path of directory in which to put temp file
       url:	used for a similar name or generating a hash within 'limit'
       suffix:	if != 0, this suffix is used, else no suffix
       limit:	if 0 => use last part of url
       		if >0 generate hash. Max value
                      of limit is max(unsigned int).
   Returns NULL if no filename could be found.
   Henrik 10/03-94
   ------------------------------------------------------------------------- */
PUBLIC char *HTFWriter_filename ARGS5(char *, path, char *, url,
				      CONST char *, suffix,
				      unsigned int, limit,
				      BOOL, flat)
{
#define HASHTRY 5			   /* Keep this less than 10, please */
    static int primes[HASHTRY] = {31, 37, 41, 43, 47};	      /* Some primes */
    char *urlfile = strrchr(url, '/');
    char *filename = NULL;

    /* HWL 22/9/94 */
    /* HWL added support for hierachical structure */
    if (!flat) {
	filename = cache_file_name(path, url);
	if (!filename)
            return NULL;
	if (create_cache_place(path, filename))
            return(filename);
	else
            return NULL;
    }

    StrAllocCopy(filename, path);
    if (filename && urlfile++) {		   /* We don't want two '/'s */
	int digits = 1;
	unsigned hash;
	char *hashstr = NULL;
	char *replace = 0;
	char *ptr;				/* Dummy used several places */
	char format[10];
	if (*(filename+strlen(filename)-1) != '/')  /* But one is OK, though */
	    StrAllocCat(filename, "/");
	if (limit) {					  /* Use hash method */
	    unsigned int residue = limit;
	    while ((residue /= 10))		    /* Find number of digits */
		digits++;
	    if ((hashstr = (char *) malloc(digits+1)) == NULL)
		outofmem(__FILE__, "HTFWriter_filename");
	    for(ptr=urlfile, hash=0; *ptr; ptr++)          /* Calculate hash */
		hash = *ptr + *primes * hash;
	    hash %= limit;
	    sprintf(format, "%%0%du", digits);     /* Convert hash to string */
	    sprintf(hashstr, format, hash);
	    *(hashstr+digits) = '\0';
	    StrAllocCat(filename, hashstr);
#ifdef HAVE_GETPID
/* RISK: Race conditions may occur if this is not added to the filename */
	    {
		char pidstr[10];
		sprintf(pidstr, "-%d", getpid());
		StrAllocCat(filename, pidstr);
	    }
#endif
	    if (suffix) {				       /* Add suffix */
		if (*suffix != '.')
		    StrAllocCat(filename, ".");
		StrAllocCat(filename, suffix);
	    }
	    replace = strrchr(filename, '/')+1;	   /* Remember place */
	} else {      					     /* Use url name */
	    char *urlptr = 0;			     /* Strip off any suffix */
	    StrAllocCopy(urlptr, urlfile);
	    ptr = strrchr(urlptr, '.');
	    if (ptr)
		*ptr = '\0';
	    StrAllocCat(filename, urlptr);
	    free(urlptr);
	    if (suffix) {			    /* Add new suffix if any */
		if (*suffix != '.')
		    StrAllocCat(filename, ".");
		StrAllocCat(filename, suffix);
	    }
	}
	{				   	       /* Try if file exists */
	    int cnt;
	    FILE *fp;
	    for (cnt=1; cnt<HASHTRY; cnt++) {
		if ((fp = fopen(filename, "r")) != NULL) {
		    fclose(fp);
		    if (limit) {          /* recalculate hash with new prime */
			for(ptr=urlfile, hash=0; *ptr; ptr++)
			    hash = *ptr + *(primes+cnt) * hash;
			hash %= limit;
			sprintf(hashstr, format, hash);
			*(hashstr+digits) = '\0';
			memcpy(replace, hashstr, digits);
		    } else { 		/* Add .n to the urlfile. n is a int */
			if (cnt == 1) {
			    StrAllocCat(filename, ".1");
			    replace = filename+strlen(filename)-1;
			} else
			    *replace = (char) cnt+0x30;   /* Works if cnt<10 */
		    }
		} else
		    break;		       /* File does not exist, so OK */
	    }
	    if (cnt >= HASHTRY) {      	   /* If no file name could be found */
		free(filename);
		filename = NULL;
	    }
	}
	FREE(hashstr);
    }
    return filename;
}



/*	Character handling
**	------------------
*/

PRIVATE void HTFWriter_put_character ARGS2(HTStream *, me, char, c)
{
    fputc(c, me->fp);
}



/*	String handling
**	---------------
**
**	Strings must be smaller than this buffer size.
*/
PRIVATE void HTFWriter_put_string ARGS2(HTStream *, me, CONST char*, s)
{
    if (*s)				             /* For vms :-( 10/04-94 */
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
        HTProgress(me->end_command);	/* Tell user what's happening */
	system(me->end_command);	/* @@ Beware of security hole */
	free (me->end_command);
	if (me->remove_on_close) {
	    unlink(me->filename);
	}
    }
    if (me->callback) {
        (*me->callback)(me->request, me->filename);
    }
    if (me->filename) free(me->filename);
    free(me);
    return 0;
}

/*	End writing
*/

PRIVATE int HTFWriter_abort ARGS2(HTStream *, me, HTError, e)
{
    if (me->leave_open != YES) fclose(me->fp);
    if (me->end_command) {		/* Temp file */
	if (TRACE) fprintf(stderr,
		"HTFWriter: Aborting: file %s not executed.\n",
		me->filename ? me->filename : "???" );
	free (me->end_command);
	if (me->remove_on_close) {
	    unlink(me->filename);
	}
    }

    if (me->filename) free(me->filename);
    free(me);
    return EOF;
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
PUBLIC HTStream* HTFWriter_new ARGS2(FILE *, fp, BOOL, leave_open)
{
    HTStream* me;
    
    if (!fp) return NULL;

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

#ifdef HAVE_UNLINK
{
    char *fnam;
    
    HTStream* me;
    
    if (HTSecure) {
        HTAlert("Can't save data to file -- please run WWW locally");
	return HTBlackHole();
    }
    
    if (!HTSaveLocallyDir) {
	if (TRACE) fprintf(stderr, "Save and execute turned off");
	return HTBlackHole();
    }
	
    me = (HTStream*)calloc(sizeof(*me),1);
    me = (HTStream*)calloc(sizeof(*me), 1);
    if (me == NULL) outofmem(__FILE__, "Save and execute");
    me->isa = &HTFWriter;  
    
    /* Let's find a hash name for this file */
    if ((fnam = HTFWriter_filename(HTSaveLocallyDir,
				   HTAnchor_physical(request->anchor),
				   HTFileSuffix(input_format),
				   1000, YES)) == NULL) {
	HTAlert("Can't find a suitable file name");
	return NULL;
    }

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
    me->end_command = (char *) malloc ((strlen((char *) param) + 10 +
					3*strlen(fnam)) * sizeof (char));
    if (me == NULL) outofmem(__FILE__, "SaveAndExecute");
    
    sprintf (me->end_command, (char *) param, fnam, fnam, fnam);

    me->remove_on_close = NO;

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
    char *fnam = NULL;
    char *answer = NULL;
    HTStream* me;
    
    if (HTClientHost) {
        HTAlert("Can't save data to file -- please run WWW locally");
	return HTBlackHole();
    }

    if (!HTSaveLocallyDir) {
	if (TRACE) fprintf(stderr, "Save locally turned off");
	return HTBlackHole();
    }
	
    me = (HTStream*)calloc(sizeof(*me),1);
    if (me == NULL) outofmem(__FILE__, "SaveLocally");
    me->isa = &HTFWriter;  
    me->announce = YES;
    
    /* Let's find a 'human' file name for this file */
    fnam = HTFWriter_filename(HTSaveLocallyDir,
			      HTAnchor_physical(request->anchor),
			      HTFileSuffix(input_format),
			      0, YES);
    
    /*	Save Panel */
    answer = HTPrompt("Give name of file to save in", fnam ? fnam : "");
    FREE(fnam);
    
    me->fp = fopen (answer, "w");
    if (!me->fp) {
	HTAlert("Can't open local file to write into.");
        FREE(answer);
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

    /* HWL 22/9/94: since we added a hierachical file structure, we should also clean up */
    {
	char * p;

	while ((p = strrchr(item->filename,'/')) && (p != NULL)){
	    item->filename[p - item->filename] = 0; /* this will be freed in a sec */
	    if (strcmp(item->filename, HTCacheDir) != 0) {
		if (TRACE) 
		    fprintf(stderr,"rmdir %s\n", item->filename);
		rmdir(item->filename);         /* this will luckily fail if directory is not empty */
	    }
	}
    }

    free(item->filename);
    free(item);
}

/* This can be called for the main list or an anchor's list
*/



PUBLIC void HTCacheClear ARGS1(HTList *, list)
{
    HTCacheItem * item;
    while ((item = (HTCacheItem *) HTList_objectAt(list, 0)) != NULL) {
        HTCache_remove(list, item);
    }
}


/* -------------------------------------------------------------------------
   This function removes ALL cache files known to this session. The anchors
   ARE updated, but normally this function is for exiting purposes.
   Henrik 09/03-94
   ------------------------------------------------------------------------- */
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
#ifdef HAVE_OPENDIR
    char *fnam;
    HTStream* me;

    if (HTClientHost) {
	if (TRACE) fprintf(stderr, "Only caching if WWW is run locally.\n");
	return HTBlackHole();
    }
    me = (HTStream*)calloc(sizeof(*me),1);
    if (me == NULL) outofmem(__FILE__, "CacheWriter");
    me->isa = &HTFWriter;  
    me->end_command = NULL;
    me->remove_on_close = NO;	/* If needed, put into end_command */
    me->announce = NO;
    
    /* Save the file under a suitably suffixed name */
    fnam = HTFWriter_filename(HTCacheDir,
			      HTAnchor_physical(request->anchor),
			      HTFileSuffix(input_format),
			      0, NO);
    if (!fnam) {                         /* HWL 22/9/94 */
	free(me);
	return NULL;
    }

    me->filename = NULL;
    limit_cache(HTCache);		 /* Limit number (not size) of files */
    me->fp = fopen (fnam, "w");
    if (!me->fp) {
	HTAlert("Can't open local file for writing.");
	if (TRACE) fprintf(stderr, "HTStream: can't open %s for writing\n",fnam);
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
#else /* HAVE_OPENDIR */
    return HTBlackHole();
#endif /* HAVE_OPENDIR */
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


