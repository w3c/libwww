/*         		    					     HTRobot.c
**	W3C MINI ROBOT
**
**	(c) COPRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	This program illustrates how to travers links using the Anchor object
**
**  Authors:
**	HFN		Henrik Frystyk Nielsen, (frystyk@w3.org)
**
**  History:
**	Dec 04 95	First version
*/

#include "WWWLib.h"			      /* Global Library Include file */
#include "WWWApp.h"				        /* Application stuff */
#include "WWWTrans.h"
#include "WWWInit.h"

#include "HText.h"

#include "HTRobot.h"			     		 /* Implemented here */

#ifdef HT_POSIX_REGEX
#ifdef HAVE_RXPOSIX_H
#include <rxposix.h>
#else
#ifdef HAVE_REGEX_H
#include <regex.h>
#endif
#endif
#define	W3C_REGEX_FLAGS		(REG_EXTENDED | REG_NEWLINE)
#endif

#ifndef W3C_VERSION
#define W3C_VERSION 		"Unspecified"
#endif

#define APP_NAME		"W3CRobot"
#define APP_VERSION		W3C_VERSION
#define COMMAND_LINE		"http://www.w3.org/Robot/User/CommandLine"

#define DEFAULT_OUTPUT_FILE	"robot.out"
#define DEFAULT_RULE_FILE	"robot.conf"
#define DEFAULT_LOG_FILE       	"log-clf.txt"
#define DEFAULT_HIT_FILE       	"log-hit.txt"
#define DEFAULT_REL_FILE      	"log-rel.txt"
#define DEFAULT_LM_FILE       	"log-lastmodified.txt"
#define DEFAULT_TITLE_FILE     	"log-title.txt"
#define DEFAULT_REFERER_FILE   	"log-referer.txt"
#define DEFAULT_REJECT_FILE   	"log-reject.txt"
#define DEFAULT_NOTFOUND_FILE  	"log-notfound.txt"
#define DEFAULT_CONNEG_FILE  	"log-conneg.txt"
#define DEFAULT_NOALTTAG_FILE  	"log-alt.txt"
#define DEFAULT_FORMAT_FILE  	"log-format.txt"
#define DEFAULT_CHARSET_FILE  	"log-charset.txt"
#define DEFAULT_MEMLOG		"robot.mem"
#define DEFAULT_PREFIX		""
#define DEFAULT_IMG_PREFIX	""
#define DEFAULT_DEPTH		0
#define DEFAULT_DELAY		50			/* Write delay in ms */

#define DEFAULT_SQL_SERVER	"localhost"
#define DEFAULT_SQL_DB		"webbot"
#define DEFAULT_SQL_USER	"webbot"
#define DEFAULT_SQL_PW		""

#if 0
#define HT_MEMLOG		/* Is expensive in performance! */
#endif

/* #define SHOW_MSG		(WWWTRACE || HTAlert_interactive()) */
#define SHOW_QUIET(mr)		((mr) && !((mr)->flags & MR_QUIET))
#define SHOW_REAL_QUIET(mr)	((mr) && !((mr)->flags & MR_REAL_QUIET))

#define DEFAULT_TIMEOUT		50000		       /* timeout in millis */

#if defined(__svr4__)
#define CATCH_SIG
#endif

typedef enum _MRFlags {
    MR_IMG		= 0x1,
    MR_LINK		= 0x2,
    MR_PREEMPTIVE	= 0x4,
    MR_TIME		= 0x8,
    MR_SAVE	  	= 0x10,
    MR_QUIET	  	= 0x20,
    MR_REAL_QUIET  	= 0x40,
    MR_VALIDATE		= 0x80,
    MR_END_VALIDATE	= 0x100,
    MR_KEEP_META	= 0x200,
    MR_LOGGING		= 0x400,
    MR_DISTRIBUTIONS	= 0x800
} MRFlags;

typedef struct _Robot {
    int			depth;			     /* How deep is our tree */
    int			cnt;				/* Count of requests */
    HTList *		hyperdoc;	     /* List of our HyperDoc Objects */
    HTList *		htext;			/* List of our HText Objects */
    HTList *		fingers;

    int 		timer;
    char *		cwd;			/* Current dir URL */
    char *		rules;
    char *		prefix;
    char *		img_prefix;

    char *		logfile;		/* clf log */
    HTLog *             log;
    char *		reffile;		/* referer log */
    HTLog *             ref;
    char *		rejectfile;		/* unchecked links */
    HTLog *	        reject;
    char *		notfoundfile;		/* links that returned 404 */
    HTLog *	        notfound;
    char *		connegfile;		/* links that were conneg'ed */
    HTLog *	        conneg;
    char *		noalttagfile;		/* images without alt tags*/
    HTLog *	        noalttag;

    char *		hitfile;		/* links sorted after hit counts */
    char *		relfile;		/* link sorted after relationships */
    HTLinkType		relation;		/* Specific relation to look for */
    char *		titlefile;		/* links with titles */
    char *		mtfile;			/* media types encountered */
    char *		charsetfile;		/* charsets encountered */
    char *		lmfile;			/* sortef after last modified dates */

    char *		outputfile;		
    FILE *	        output;

    MRFlags		flags;

    long		get_bytes;	/* Total number of bytes processed using GET*/
    long                get_docs;     	/* Total number of documents using GET */

    long		head_bytes;	/* bytes processed bytes processed using HEAD */
    long                head_docs;   	/* Total number of documents using HEAD*/

    long		other_docs;

    ms_t		time;		/* Time of run */

#ifdef HT_POSIX_REGEX
    regex_t *		include;
    regex_t *		exclude;
    regex_t *		check;
#endif

#ifdef HT_MYSQL
    HTSQLLog *		sqllog;
    char *		sqlserver;
    char *		sqldb;
    char *		sqluser;
    char *		sqlpw;
    char *		sqlrelative;
    BOOL		sqlexternals;
    int			sqlflags;
#endif

} Robot;

typedef struct _Finger {
    Robot * robot;
    HTRequest * request;
    HTParentAnchor * dest;
} Finger;

typedef enum _LoadState {
    L_INVALID	= -2,
    L_LOADING	= -1,
    L_SUCCESS 	= 0,
    L_ERROR
} LoadState;

/*
**  The HyperDoc object is bound to the anchor and contains information about
**  where we are in the search for recursive searches
*/
typedef struct _HyperDoc {
    HTParentAnchor * 	anchor;
    LoadState		state;
    int			depth;
    int                 hits;
} HyperDoc;

/*
** This is the HText object that is created every time we start parsing an 
** HTML object
*/
struct _HText {
    HTRequest *		request;
    BOOL		follow;
};

/*
**  A structure for calculating metadata distributions
*/
typedef struct _MetaDist {
    HTAtom *		name;
    int			hits;
} MetaDist;

/*
**  Some sorting algorithms
*/
PRIVATE HTComparer HitSort, FormatSort, LastModifiedSort, TitleSort;

PUBLIC HText * HTMainText = NULL;
PUBLIC HTParentAnchor * HTMainAnchor = NULL;
PUBLIC HTStyleSheet * styleSheet = NULL;

/* ------------------------------------------------------------------------- */

/*	Standard (non-error) Output
**	---------------------------
*/
PUBLIC int OutputData(const char  * fmt, ...)
{
    int ret;
    va_list pArgs;
    va_start(pArgs, fmt);
    ret = vfprintf(stdout, fmt, pArgs);
    va_end(pArgs);
    return ret;
}

/* ------------------------------------------------------------------------- */

/*	Create a "HyperDoc" object
**	--------------------------
**	A HyperDoc object contains information about whether we have already
**	started checking the anchor and the depth in our search
*/
PRIVATE HyperDoc * HyperDoc_new (Robot * mr,HTParentAnchor * anchor, int depth)
{
    HyperDoc * hd;
    if ((hd = (HyperDoc *) HT_CALLOC(1, sizeof(HyperDoc))) == NULL)
	HT_OUTOFMEM("HyperDoc_new");
    hd->state = L_INVALID;
    hd->depth = depth;
    hd->hits = 1;
 
    /* Bind the HyperDoc object together with the Anchor Object */
    hd->anchor = anchor;
    HTAnchor_setDocument(anchor, (void *) hd);

    /* Add this HyperDoc object to our list */
    if (!mr->hyperdoc) mr->hyperdoc = HTList_new();
    HTList_addObject(mr->hyperdoc, (void *) hd);
    return hd;
}

/*	Delete a "HyperDoc" object
**	--------------------------
*/
PRIVATE BOOL HyperDoc_delete (HyperDoc * hd)
{
    if (hd) {
	HT_FREE (hd);
	return YES;
    }
    return NO;
}

/*
**  Sort the anchor array and log reference count
*/
PRIVATE BOOL calculate_hits (Robot * mr, HTArray * array)
{
    if (mr && array) {
        HTLog * log = HTLog_open(mr->hitfile, YES, YES);
        if (log) {
            void ** data = NULL;
            HTParentAnchor * anchor = NULL;
            HTArray_sort(array, HitSort);
            anchor = (HTParentAnchor *) HTArray_firstObject(array, data);
	    while (anchor) {
                char * uri = HTAnchor_address((HTAnchor *) anchor);
                HyperDoc * hd = (HyperDoc *) HTAnchor_document(anchor);
                if (uri && hd) HTLog_addText(log, "%8d %s\n", hd->hits, uri);
                HT_FREE(uri);
                anchor = (HTParentAnchor *) HTArray_nextObject(array, data);
            }
	}
        HTLog_close(log);
        return YES;
    }
    return NO;
}

PRIVATE int HitSort (const void * a, const void * b)
{
    HyperDoc * aa = HTAnchor_document(*(HTParentAnchor **) a);
    HyperDoc * bb = HTAnchor_document(*(HTParentAnchor **) b);
    if (aa && bb) return (bb->hits - aa->hits);
    return bb - aa;
}

/*
**  Sort the anchor array and log link relations
*/
PRIVATE BOOL calculate_linkRelations (Robot * mr, HTArray * array)
{
    if (mr && array) {
        HTLog * log = mr->relfile ? HTLog_open(mr->relfile, YES, YES) : NULL;
	void ** data = NULL;
	HTParentAnchor * anchor = NULL;
	anchor = (HTParentAnchor *) HTArray_firstObject(array, data);
	while (anchor) {

	    /*
	    **  If we have a specific link relation to look for then do this.
	    **  Otherwise look for all link relations.
	    */
	    if (mr->relation) {
		HTLink * link = HTAnchor_findLinkType((HTAnchor *) anchor, mr->relation);
		if (link) {
		    HTParentAnchor * dest = HTAnchor_parent(HTLink_destination(link));
		    char * src_uri = HTAnchor_address((HTAnchor *) anchor);
		    char * dest_uri = HTAnchor_address((HTAnchor *) dest);
		    if (src_uri && dest_uri) {
#ifdef HT_MYSQL
			if (mr->sqllog) {
			    HTSQLLog_addLinkRelationship (mr->sqllog,
							  src_uri, dest_uri,
							  HTAtom_name(mr->relation),
							  NULL);
			}
#endif
			if (log) {
			    HTFormat format = HTAnchor_format(dest);
			    HTLog_addText(log, "%s %s %s --> %s\n",
					  HTAtom_name(mr->relation),
					  format != WWW_UNKNOWN ?
					  HTAtom_name(format) : "<unknown>",
					  src_uri, dest_uri);
			}

			/* Cleanup */
			HT_FREE(src_uri);
			HT_FREE(dest_uri);
		    }
		}
	    } else {
		HTLink * link = HTAnchor_mainLink((HTAnchor *) anchor);
		HTList * sublinks = HTAnchor_subLinks((HTAnchor *) anchor);
		char * src_uri = HTAnchor_address((HTAnchor *) anchor);
		HTLinkType linktype;

		/* First look in the main link */
		if (link && (linktype = HTLink_type(link))) {		    
		    HTParentAnchor * dest = HTAnchor_parent(HTLink_destination(link));
		    char * dest_uri = HTAnchor_address((HTAnchor *) dest);
		    if (src_uri && dest_uri) {
#ifdef HT_MYSQL
			if (mr->sqllog) {
			    HTSQLLog_addLinkRelationship (mr->sqllog,
							  src_uri, dest_uri,
							  HTAtom_name(linktype),
							  NULL);
			}
#endif
			if (log) {
			    HTFormat format = HTAnchor_format(dest);
			    HTLog_addText(log, "%s %s %s --> %s\n",
					  HTAtom_name(linktype),
					  format != WWW_UNKNOWN ?
					  HTAtom_name(format) : "<unknown>",
					  src_uri, dest_uri);
			}
		    }
		    HT_FREE(dest_uri);
		}

		/* and then in any sublinks */
		if (sublinks) {
		    HTLink * pres;
		    while ((pres = (HTLink *) HTList_nextObject(sublinks))) {
			if ((linktype = HTLink_type(pres))) {
			    HTParentAnchor * dest = HTAnchor_parent(HTLink_destination(pres));
			    char * dest_uri = HTAnchor_address((HTAnchor *) dest);
			    if (src_uri && dest_uri) {
#ifdef HT_MYSQL
				if (mr->sqllog) {
				    HTSQLLog_addLinkRelationship (mr->sqllog,
								  src_uri, dest_uri,
								  HTAtom_name(linktype),
								  NULL);
				}
#endif
				if (log) {
				    HTFormat format = HTAnchor_format(dest);
				    HTLog_addText(log, "%s %s %s --> %s\n",
						  HTAtom_name(linktype),
						  format != WWW_UNKNOWN ?
						  HTAtom_name(format) : "<unknown>",
						  src_uri, dest_uri);
				}
				HT_FREE(dest_uri);
			    }
			}
		    }
		}

		/* Cleanup */
		HT_FREE(src_uri);
	    }
	    anchor = (HTParentAnchor *) HTArray_nextObject(array, data);
	}
        if (log) HTLog_close(log);
        return YES;
    }
    return NO;
}

/*
**  Sort the anchor array and log last modified date
*/
PRIVATE BOOL calculate_lm (Robot * mr, HTArray * array)
{
    if (mr && array) {
        HTLog * log = HTLog_open(mr->lmfile, YES, YES);
        if (log) {
            void ** data = NULL;
            HTParentAnchor * anchor = NULL;
            HTArray_sort(array, LastModifiedSort);
            anchor = (HTParentAnchor *) HTArray_firstObject(array, data);
	    while (anchor) {
                char * uri = HTAnchor_address((HTAnchor *) anchor);
                time_t lm = HTAnchor_lastModified(anchor);
                if (uri && lm > 0)
		    HTLog_addText(log, "%s %s\n", HTDateTimeStr(&lm, NO), uri);
                HT_FREE(uri);
                anchor = (HTParentAnchor *) HTArray_nextObject(array, data);
            }
	}
        HTLog_close(log);
        return YES;
    }
    return NO;
}

PRIVATE int LastModifiedSort (const void * a, const void * b)
{
    time_t aa = HTAnchor_lastModified(*(HTParentAnchor **) a);
    time_t bb = HTAnchor_lastModified(*(HTParentAnchor **) b);
    return bb - aa;
}

/*
**  Sort the anchor array and log the document title
*/
PRIVATE BOOL calculate_title (Robot * mr, HTArray * array)
{
    if (mr && array) {
        HTLog * log = HTLog_open(mr->titlefile, YES, YES);
        if (log) {
            void ** data = NULL;
            HTParentAnchor * anchor = NULL;
            HTArray_sort(array, TitleSort);
            anchor = (HTParentAnchor *) HTArray_firstObject(array, data);
	    while (anchor) {
                char * uri = HTAnchor_address((HTAnchor *) anchor);
                const char * title = HTAnchor_title(anchor);
		HTCharset charset = HTAnchor_charset(anchor);
                if (uri) HTLog_addText(log, "%s `%s\' %s\n",
				       charset ? HTAtom_name(charset) : "<none>",
				       title ? title : "<none>",
				       uri);
                HT_FREE(uri);
                anchor = (HTParentAnchor *) HTArray_nextObject(array, data);
            }
	}
        HTLog_close(log);
        return YES;
    }
    return NO;
}

PRIVATE int TitleSort (const void * a, const void * b)
{
    const char * aa = HTAnchor_title(*(HTParentAnchor **) a);
    const char * bb = HTAnchor_title(*(HTParentAnchor **) b);
    return strcasecomp(bb?bb:"", aa?aa:"");
}

/*
**  Calculate distributions for media types. The same mechanism
**  can be used for other characteristics with relatively
**  few outcomes.
*/
PRIVATE HTList * mediatype_distribution (HTArray * array)
{
    if (array) {
	HTList * mt = HTList_new();
	MetaDist * pres = NULL;
	void ** data = NULL;
	HTParentAnchor * anchor = NULL;
	anchor = (HTParentAnchor *) HTArray_firstObject(array, data);
	while (anchor) {
	    HTFormat format = HTAnchor_format(anchor);
	    if (format && format != WWW_UNKNOWN) {
		HTList * cur = mt;

		/* If found then increase counter */
		while ((pres = (MetaDist *) HTList_nextObject(cur))) {
		    if (pres->name == format) {
			pres->hits++;
			break;
		    }
		}

		/* If not found then add new format to list */
		if (!pres) {
                    if ((pres = (MetaDist *) HT_CALLOC(1, sizeof(MetaDist))) == NULL)
        	         HT_OUTOFMEM("mediatype_distribution");
		    pres->name = format;
		    pres->hits = 1;
		    HTList_addObject(mt, pres);
		    HTList_insertionSort(mt, FormatSort);
		}
	    }

	    /* Find next anchor in array */
	    anchor = (HTParentAnchor *) HTArray_nextObject(array, data);
	}
	return mt;
    }
    return NULL;
}

/*
**  Calculate distributions for charsets. The same mechanism
**  can be used for other characteristics with relatively
**  few outcomes.
*/
PRIVATE HTList * charset_distribution (HTArray * array)
{
    if (array) {
	HTList * cs = HTList_new();
	MetaDist * pres = NULL;
	void ** data = NULL;
	HTParentAnchor * anchor = NULL;
	anchor = (HTParentAnchor *) HTArray_firstObject(array, data);
	while (anchor) {
	    HTCharset charset = HTAnchor_charset(anchor);
	    if (charset) {
		HTList * cur = cs;

		/* If found then increase counter */
		while ((pres = (MetaDist *) HTList_nextObject(cur))) {
		    if (pres->name == charset) {
			pres->hits++;
			break;
		    }
		}

		/* If not found then add new format to list */
		if (!pres) {
                    if ((pres = (MetaDist *) HT_CALLOC(1, sizeof(MetaDist))) == NULL)
        	         HT_OUTOFMEM("charset_distribution");
		    pres->name = charset;
		    pres->hits = 1;
		    HTList_addObject(cs, pres);
		    HTList_insertionSort(cs, FormatSort);
		}
	    }

	    /* Find next anchor in array */
	    anchor = (HTParentAnchor *) HTArray_nextObject(array, data);
	}
	return cs;
    }
    return NULL;
}

PRIVATE int FormatSort (const void * a, const void * b)
{
    MetaDist * aa = (MetaDist *) a;
    MetaDist * bb = (MetaDist *) b;
    return strcmp(HTAtom_name(bb->name), HTAtom_name(aa->name));
}

PRIVATE BOOL log_meta_distribution (const char * logfile, HTList * distribution)
{
    if (logfile && distribution) {
        HTLog * log = HTLog_open(logfile, YES, YES);
	if (log) {
	    HTList * cur = distribution;
	    MetaDist * pres;
	    while ((pres = (MetaDist *) HTList_nextObject(cur))) {
		if (pres->name) {
		    HTLog_addText(log, "%8d %s\n", pres->hits, HTAtom_name(pres->name));
		}
	    }
	    HTLog_close(log);
	}
    }
    return NO;
}

PRIVATE BOOL delete_meta_distribution (HTList * distribution)
{
    if (distribution) {
	HTList * cur = distribution;
	MetaDist * pres;
	while ((pres = (MetaDist *) HTList_nextObject(cur)))
	    HT_FREE(pres);
	HTList_delete(distribution);	
	return YES;	
    }
    return NO;
}


/*	Statistics
**	----------
**	Calculates a bunch of statistics for the anchors traversed
*/
PRIVATE BOOL calculate_statistics (Robot * mr)
{
    long total_docs = mr->get_docs + mr->head_docs + mr->other_docs;
    if (!mr) return NO;

    /* Calculate efficiency */
    if (mr->time > 0) {
	ms_t t = HTGetTimeInMillis() - mr->time;
	if (t > 0) {
	    double loadfactor = (mr->get_bytes / (t * 0.001));
	    double reqprsec = (total_docs / (t * 0.001));
	    double secs = t / 1000.0;
            char bytes[50];
	    if (SHOW_REAL_QUIET(mr))
		HTTrace("\nAccessed %ld documents in %.2f seconds (%.2f requests pr sec)\n",
			total_docs, secs, reqprsec);

            HTNumToStr(mr->get_bytes, bytes, 50);
	    if (SHOW_REAL_QUIET(mr))
		HTTrace("\tDid a GET on %ld document(s) and downloaded %s bytes of document bodies (%2.1f bytes/sec)\n",
			mr->get_docs, bytes, loadfactor);

            HTNumToStr(mr->head_bytes, bytes, 50);
	    if (SHOW_REAL_QUIET(mr))
		HTTrace("\tDid a HEAD on %ld document(s) with a total of %s bytes\n",
			mr->head_docs, bytes);
	}
    }

    /* Create an array of existing anchors */
    if (total_docs > 1) {
	HTArray * array = HTAnchor_getArray(total_docs);
        if (array) {

	    /* Distributions */
	    if (mr->flags & MR_DISTRIBUTIONS) {
		if (SHOW_REAL_QUIET(mr)) HTTrace("\nDistributions:\n");
	    }

            /* Sort after hit counts */
            if (mr->hitfile) {
		if (SHOW_REAL_QUIET(mr))
		    HTTrace("\tLogged hit count distribution in file `%s\'\n",
			    mr->hitfile);
		calculate_hits(mr, array);
	    }

            /* Sort after link relations */
#ifdef HT_MYSQL
            if (mr->relfile || mr->sqllog) {
		if (mr->relfile && SHOW_REAL_QUIET(mr))
		    HTTrace("\tLogged link relationship distribution in file `%s\'\n",
			    mr->relfile);
		calculate_linkRelations(mr, array);
	    }
#endif

            /* Sort after modified date */
            if (mr->lmfile) {
		if (SHOW_REAL_QUIET(mr))
		    HTTrace("\tLogged last modified distribution in file `%s\'\n",
			    mr->lmfile);
		calculate_lm(mr, array);
	    }

            /* Sort after title */
            if (mr->titlefile) {
		if (SHOW_REAL_QUIET(mr))
		    HTTrace("\tLogged title distribution in file `%s\'\n",
			    mr->titlefile);
		calculate_title(mr, array);
	    }

            /* Find mediatype distribution */
	    if (mr->mtfile) {
		HTList * mtdist = mediatype_distribution(array);
		if (mtdist) {
		    if (SHOW_REAL_QUIET(mr))
			HTTrace("\tLogged media type distribution in file `%s\'\n",
				mr->mtfile);
		    log_meta_distribution(mr->mtfile, mtdist);
		    delete_meta_distribution(mtdist);
		}
	    }

            /* Find charset distribution */
	    if (mr->charsetfile) {
		HTList * charsetdist = charset_distribution(array);
		if (charsetdist) {
		    if (SHOW_REAL_QUIET(mr))
			HTTrace("\tLogged charset distribution in file `%s\'\n",
				mr->charsetfile);
		    log_meta_distribution(mr->charsetfile, charsetdist);
		    delete_meta_distribution(charsetdist);
		}
	    }

            /* Add as may other stats here as you like */
	    /* ... */
	    
	    /* Delete the array */
            HTArray_delete(array);
        }
    }
    return YES;
}

/*	Create a Command Line Object
**	----------------------------
*/
PRIVATE Robot * Robot_new (void)
{
    Robot * me;
    if ((me = (Robot *) HT_CALLOC(1, sizeof(Robot))) == NULL)
	HT_OUTOFMEM("Robot_new");
    me->hyperdoc = HTList_new();
    me->htext = HTList_new();
    me->timer = DEFAULT_TIMEOUT;
    me->cwd = HTGetCurrentDirectoryURL();
    me->output = OUTPUT;
    me->cnt = 0;
    me->fingers = HTList_new();
    return me;
}

/*	Delete a Command Line Object
**	----------------------------
*/
PRIVATE BOOL Robot_delete (Robot * mr)
{
    if (mr) {
	HTList_delete(mr->fingers);

       	/* Calculate statistics */
	calculate_statistics(mr);

        if (mr->hyperdoc) {
	    HTList * cur = mr->hyperdoc;
	    HyperDoc * pres;
	    while ((pres = (HyperDoc *) HTList_nextObject(cur)))
		HyperDoc_delete(pres);
	    HTList_delete(mr->hyperdoc);
	}
	if (mr->htext) {
	    HTList * cur = mr->htext;
	    HText * pres;
	    while ((pres = (HText *) HTList_nextObject(cur)))
		HText_free(pres);
	    HTList_delete(mr->htext);
	}

	/* Close all the log files */
	if (mr->flags & MR_LOGGING) {
	    if (SHOW_REAL_QUIET(mr)) HTTrace("\nRaw Log files:\n");
	}

	if (mr->log) {
	    if (SHOW_REAL_QUIET(mr))
		HTTrace("\tLogged %5d entries in general log file `%s\'\n",
			HTLog_accessCount(mr->log), mr->logfile);
	    HTLog_close(mr->log);
	}
	if (mr->ref) {
	    if (SHOW_REAL_QUIET(mr))
		HTTrace("\tLogged %5d entries in referer log file `%s\'\n",
			HTLog_accessCount(mr->ref), mr->reffile);
	    HTLog_close(mr->ref);
	}
	if (mr->reject) {
	    if (SHOW_REAL_QUIET(mr))
		HTTrace("\tLogged %5d entries in rejected log file `%s\'\n",
			HTLog_accessCount(mr->reject), mr->rejectfile);
	    HTLog_close(mr->reject);
	}
	if (mr->notfound) {
	    if (SHOW_REAL_QUIET(mr))
		HTTrace("\tLogged %5d entries in not found log file `%s\'\n",
			HTLog_accessCount(mr->notfound), mr->notfoundfile);
	    HTLog_close(mr->notfound);
	}
	if (mr->conneg) {
	    if (SHOW_REAL_QUIET(mr))
		HTTrace("\tLogged %5d entries in content negotiation log file `%s\'\n",
			HTLog_accessCount(mr->conneg), mr->connegfile);
	    HTLog_close(mr->conneg);
	}
	if (mr->noalttag) {
	    if (SHOW_REAL_QUIET(mr))
		HTTrace("\tLogged %5d entries in missing alt tag log file `%s\'\n",
			HTLog_accessCount(mr->noalttag), mr->noalttagfile);
	    HTLog_close(mr->noalttag);
	}

	if (mr->output && mr->output != STDOUT) fclose(mr->output);

	if (mr->flags & MR_TIME) {
	    time_t local = time(NULL);
	    if (SHOW_REAL_QUIET(mr))
		HTTrace("\nRobot terminated %s\n", HTDateTimeStr(&local, YES));
	}

#ifdef HT_POSIX_REGEX
	if (mr->include) {
	    regfree(mr->include);
	    HT_FREE(mr->include);
	}
	if (mr->exclude) {
	    regfree(mr->exclude);
	    HT_FREE(mr->exclude);
	}
	if (mr->check) {
	    regfree(mr->check);
	    HT_FREE(mr->check);
	}
#endif

#ifdef HT_MYSQL
	if (mr->sqllog) {
	    HTSQLLog_close(mr->sqllog);
	    mr->sqllog = NULL;
	}
#endif

	HT_FREE(mr->cwd);
	HT_FREE(mr->prefix);
	HT_FREE(mr->img_prefix);
	HT_FREE(mr);
	return YES;
    }
    return NO;
}

/*
**  This function creates a new finger object and initializes it with a new request
*/
PRIVATE Finger * Finger_new (Robot * robot, HTParentAnchor * dest, HTMethod method)
{
    Finger * me;
    HTRequest * request = HTRequest_new();
    if ((me = (Finger *) HT_CALLOC(1, sizeof(Finger))) == NULL)
	HT_OUTOFMEM("Finger_new");
    me->robot = robot;
    me->request = request;
    me->dest = dest;
    HTList_addObject(robot->fingers, (void *)me);

    /* Set the context for this request */
    HTRequest_setContext (request, me);

    /* Check the various flags to customize the request */
    if (robot->flags & MR_PREEMPTIVE)
	HTRequest_setPreemptive(request, YES);
    if (robot->flags & MR_VALIDATE)
	HTRequest_setReloadMode(request, HT_CACHE_VALIDATE);
    if (robot->flags & MR_END_VALIDATE)
	HTRequest_setReloadMode(request, HT_CACHE_END_VALIDATE);

    /* We wanna make sure that we are sending a Host header (default) */
    HTRequest_addRqHd(request, HT_C_HOST);

    /* Set the method for this request */
    HTRequest_setMethod(request, method);
    robot->cnt++;
    return me;
}

PRIVATE int Finger_delete (Finger * me)
{
    HTList_removeObject(me->robot->fingers, (void *)me);
    me->robot->cnt--;

    /*
    **  If we are down at one request then flush the output buffer
    */
    if (me->request) {
	if (me->robot->cnt == 1) HTRequest_forceFlush(me->request);
	HTRequest_delete(me->request);
    }

    /*
    **  Delete the request and free myself
    */
    HT_FREE(me);
    return YES;
}

/*
**  Cleanup and make sure we close all connections including the persistent
**  ones
*/
PRIVATE void Cleanup (Robot * me, int status)
{
    Robot_delete(me);
    HTProfile_delete();
#ifdef HT_MEMLOG
    HTMemLog_close();
#endif

#ifdef VMS
    exit(status ? status : 1);
#else
    exit(status ? status : 0);
#endif
}

#ifdef CATCH_SIG
#include <signal.h>
/*								    SetSignal
**  This function sets up signal handlers. This might not be necessary to
**  call if the application has its own handlers (lossage on SVR4)
*/
PRIVATE void SetSignal (void)
{
    /* On some systems (SYSV) it is necessary to catch the SIGPIPE signal
    ** when attemting to connect to a remote host where you normally should
    ** get `connection refused' back
    */
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
	if (PROT_TRACE) HTTrace("HTSignal.... Can't catch SIGPIPE\n");
    } else {
	if (PROT_TRACE) HTTrace("HTSignal.... Ignoring SIGPIPE\n");
    }

#ifdef HT_MEMLOG
    HTMemLog_flush();
#endif

}
#endif /* CATCH_SIG */

#ifdef HT_POSIX_REGEX
PRIVATE char * get_regerror (int errcode, regex_t * compiled)
{
    size_t length = regerror (errcode, compiled, NULL, 0);
    char * str = NULL;
    if ((str = (char *) HT_MALLOC(length+1)) == NULL)
	HT_OUTOFMEM("get_regerror");
    (void) regerror (errcode, compiled, str, length);
    return str;
}

PRIVATE regex_t * get_regtype (Robot * mr, const char * regex_str, int cflags)
{
    regex_t * regex = NULL;
    if (regex_str && *regex_str) {
	int status;
	if ((regex = (regex_t *) HT_CALLOC(1, sizeof(regex_t))) == NULL)
	    HT_OUTOFMEM("get_regtype");
	if ((status = regcomp(regex, regex_str, cflags))) {
	    char * err_msg = get_regerror(status, regex);
	    if (SHOW_REAL_QUIET(mr))
		HTTrace("Regular expression error: %s\n", err_msg);
	    HT_FREE(err_msg);
	    Cleanup(mr, -1);
	}
    }
    return regex;
}
#endif

PRIVATE void VersionInfo (void)
{
    OutputData("W3C Sample Software\n\n");
    OutputData("\tW3C Mini Robot (%s) version %s\n", APP_NAME, APP_VERSION);
    OutputData("\tW3C Sample Library (libwww) version %s\n\n", HTLib_version());
    OutputData("For command line options, see\n\t%s\n\n", COMMAND_LINE);
    OutputData("Please send feedback to <libwww@w3.org>\n");
}

/*	terminate_handler
**	-----------------
**	This function is registered to handle the result of the request.
**	If no more requests are pending then terminate program
*/
PRIVATE int terminate_handler (HTRequest * request, HTResponse * response,
			       void * param, int status) 
{
    Finger * finger = (Finger *) HTRequest_context(request);
    Robot * mr = finger->robot;
    if (SHOW_QUIET(mr)) HTTrace("Robot....... done with %s\n", HTAnchor_physical(finger->dest));

#ifdef HT_MYSQL
    if (mr->sqllog) HTSQLLog_addEntry(mr->sqllog, request, status);
#endif

    /* Check if negotiated resource and whether we should log that*/
    if (mr->conneg) {
	HTAssocList * cur = HTResponse_variant(response);
	if (cur) {
	    BOOL first = YES;
	    HTChunk * buffer = HTChunk_new(128);
	    char * uri = HTAnchor_address((HTAnchor *) finger->dest);
	    HTAssoc * pres;
	    HTChunk_puts(buffer, uri);
	    while ((pres = (HTAssoc *) HTAssocList_nextObject(cur))) {
		char * value = HTAssoc_value(pres);
		if (first) {
		    HTChunk_puts(buffer, "\t(");
		    first = NO;
		} else
		    HTChunk_puts(buffer, ", ");

		/* Output the name */
		HTChunk_puts(buffer, HTAssoc_name(pres));

		/* Only output the value if not empty string */
		if (value && *value) {
		    HTChunk_puts(buffer, "=");
		    HTChunk_puts(buffer, value);
		}
	    }
	    if (!first) HTChunk_puts(buffer, ")");
	    HTLog_addLine(mr->conneg, HTChunk_data(buffer));
	    HTChunk_delete(buffer);
	    HT_FREE(uri);
	}
    }

    /* Count the amount of body data that we have read */
    if (HTRequest_method(request) == METHOD_GET) {
	int length = HTAnchor_length(HTRequest_anchor(request));
	if (length > 0) mr->get_bytes += length;
	mr->get_docs++;
    } else if (HTRequest_method(request) == METHOD_HEAD) {
	int length = HTAnchor_length(HTRequest_anchor(request));
	if (length > 0) mr->head_bytes += length;
	mr->head_docs++;
    } else {
	mr->other_docs++;
    }

    /* Cleanup the anchor so that we don't drown in metainformation */
    if (!(mr->flags & MR_KEEP_META))
	HTAnchor_clearHeader(HTRequest_anchor(request));

    /* Delete this thread */
    Finger_delete(finger);

    /* Should we stop? */
    if (mr->cnt <= 0) {
	if (SHOW_QUIET(mr)) HTTrace("             Everything is finished...\n");
	Cleanup(mr, 0);			/* No way back from here */
    }
    if (SHOW_QUIET(mr)) HTTrace("             %d outstanding request%s\n", mr->cnt, mr->cnt == 1 ? "" : "s");
    return HT_OK;
}

/* ------------------------------------------------------------------------- */
/*				HTEXT INTERFACE				     */
/* ------------------------------------------------------------------------- */

PUBLIC HText * HText_new2 (HTRequest * request, HTParentAnchor * anchor,
			   HTStream * stream)
{
    HText * me;
    Finger * finger = (Finger *) HTRequest_context(request);
    Robot * mr = finger->robot;
    char * robots = NULL;

    if ((me = (HText *) HT_CALLOC(1, sizeof(HText))) == NULL)
	HT_OUTOFMEM("HText_new2");

    /* Bind the HText object together with the Request Object */
    me->request = request;
    me->follow = YES;

    /* Check to see if we have any meta tags */
    if ((robots = HTAnchor_robots(anchor)) != NULL) {
	char * strval = NULL;
	char * ptr = NULL;
	char * token = NULL;
	StrAllocCopy(strval, robots);
	ptr = strval;
	while ((token = HTNextField(&ptr)) != NULL) {
	    if (!strcasecomp(token, "nofollow")) {
		me->follow = NO;
		break;
	    }
	}
	HT_FREE(strval);
    }

    /* Add this HyperDoc object to our list */
    if (!mr->htext) mr->htext = HTList_new();
    HTList_addObject(mr->htext, (void *) me);
    return me;
}

PUBLIC void HText_free (HText * me) {
    if (me) HT_FREE (me);
}

PUBLIC void HText_beginAnchor (HText * text, HTChildAnchor * anchor)
{
    if (text && anchor) {
	Finger * finger = (Finger *) HTRequest_context(text->request);
	Robot * mr = finger->robot;
	HTAnchor * dest = HTAnchor_followMainLink((HTAnchor *) anchor);
	HTParentAnchor * dest_parent = HTAnchor_parent(dest);
	char * uri = HTAnchor_address((HTAnchor *) dest_parent);
	HyperDoc * hd = HTAnchor_document(dest_parent);
	HTParentAnchor * referer = HTRequest_anchor(text->request);
	BOOL match = text->follow;
	BOOL check = NO;

	if (!uri) return;
	if (SHOW_QUIET(mr)) HTTrace("Robot....... Found `%s\' - ", uri ? uri : "NULL\n");

        if (hd) {
	    if (SHOW_QUIET(mr)) HTTrace("Already checked\n");
            hd->hits++;
#ifdef HT_MYSQL
	    if (mr->sqllog) {
		char * ref_addr = HTAnchor_address((HTAnchor *) referer);
		if (ref_addr) {
		    HTSQLLog_addLinkRelationship(mr->sqllog, ref_addr, uri,
						 "referer", NULL);
		    HT_FREE(ref_addr);
		}
	    }
#endif
	    HT_FREE(uri);
	    return;
	}
	    
	/* Check for prefix match */
	if (match && mr->prefix) {
	    match = HTStrMatch(mr->prefix, uri) ? YES : NO;
	}

#ifdef HT_POSIX_REGEX
	/* Check for any regular expression */
	if (match && mr->include) {
	    match = regexec(mr->include, uri, 0, NULL, 0) ? NO : YES;
	}
	if (match && mr->exclude) {
	    match = regexec(mr->exclude, uri, 0, NULL, 0) ? YES : NO;
	}
	if (match && mr->check) {
	    check = regexec(mr->check, uri, 0, NULL, 0) ? NO : YES;
	}
#endif

	/* Test whether we already have a hyperdoc for this document */
        if (mr->flags & MR_LINK && match && dest_parent) {
	    HTParentAnchor * last_anchor = HTRequest_parent(text->request);
	    HyperDoc * last_doc = HTAnchor_document(last_anchor);
	    int depth = last_doc ? last_doc->depth+1 : 0;
	    Finger * newfinger = Finger_new(mr, dest_parent, METHOD_GET);
	    HTRequest * newreq = newfinger->request;
	    HyperDoc_new(mr, dest_parent, depth);
	    HTRequest_setParent(newreq, referer);
	    if (check || depth >= mr->depth) {
		if (SHOW_QUIET(mr)) HTTrace("loading at depth %d using HEAD\n", depth);
		HTRequest_setMethod(newreq, METHOD_HEAD);
	    } else {
		if (SHOW_QUIET(mr)) HTTrace("loading at depth %d\n", depth);
	    }
	    if (HTLoadAnchor((HTAnchor *) dest_parent, newreq) != YES) {
		if (SHOW_QUIET(mr)) HTTrace("not tested!\n");
		Finger_delete(newfinger);
	    }
	} else {
	    if (SHOW_QUIET(mr)) HTTrace("does not fulfill constraints\n");
#ifdef HT_MYSQL
	    if (mr->reject || mr->sqllog) {
#else	
	    if (mr->reject) {
#endif
		if (referer) {
		    char * ref_addr = HTAnchor_address((HTAnchor *) referer);
		    if (mr->reject && ref_addr)
			HTLog_addText(mr->reject, "%s --> %s\n", ref_addr, uri);
#ifdef HT_MYSQL
		    if (mr->sqllog && mr->sqlexternals && ref_addr)
			HTSQLLog_addLinkRelationship(mr->sqllog,
						     ref_addr, uri,
						     "referer", NULL);
#endif

		    HT_FREE(ref_addr);
		}
	    }
	}
	HT_FREE(uri);
    }
}

PUBLIC void HText_appendImage (HText * text, HTChildAnchor * anchor,
			       const char *alt, const char * align, BOOL isMap)
{
    if (text && anchor) {
	Finger * finger = (Finger *) HTRequest_context(text->request);
	Robot * mr = finger->robot;
	if (mr->flags & MR_IMG) {
	    HTAnchor * dest = HTAnchor_followMainLink((HTAnchor *) anchor);
	    HTParentAnchor * dest_parent = HTAnchor_parent(dest);
	    char * uri = HTAnchor_address((HTAnchor *) dest_parent);
	    HyperDoc * hd = HTAnchor_document(dest_parent);
	    HTParentAnchor * referer = HTRequest_anchor(text->request);
	    BOOL match = YES;

	    if (hd) {
		if (SHOW_QUIET(mr)) HTTrace("Already checked\n");
		hd->hits++;
#ifdef HT_MYSQL
		if (mr->sqllog) {
		    char * ref_addr = HTAnchor_address((HTAnchor *) referer);
		    if (ref_addr) {
			HTSQLLog_addLinkRelationship(mr->sqllog,
						     ref_addr, uri,
						     "image", alt);
			HT_FREE(ref_addr);
		    }
		}
#endif
		HT_FREE(uri);
		return;
	    }

	    /* Check for prefix match */
	    if (mr->img_prefix) match = HTStrMatch(mr->img_prefix, uri) ? YES : NO;

	    /* Test whether we already have a hyperdoc for this document */
	    if (match && dest) {
		Finger * newfinger = Finger_new(mr, dest_parent,
						mr->flags & MR_SAVE ?
						METHOD_GET : METHOD_HEAD);
		HTRequest * newreq = newfinger->request;
		HyperDoc_new(mr, dest_parent, 1);
		HTRequest_setParent(newreq, referer);

		/* Check whether we should report missing ALT tags */
		if (mr->noalttag && (alt==NULL || *alt=='\0')) {
		    if (referer) {
			char * ref_addr = HTAnchor_address((HTAnchor *) referer);
			if (ref_addr) HTLog_addText(mr->noalttag, "%s --> %s\n", ref_addr, uri);
			HT_FREE(ref_addr);
		    }
		}
		
		if (SHOW_QUIET(mr)) HTTrace("Robot....... Checking Image `%s\'\n", uri);
		if (HTLoadAnchor((HTAnchor *) dest, newreq) != YES) {
		    if (SHOW_QUIET(mr)) HTTrace("Robot....... Image not tested!\n");
		    Finger_delete(newfinger);
		}
	    } else {
		if (SHOW_QUIET(mr)) HTTrace("does not fulfill constraints\n");
#ifdef HT_MYSQL
		if (mr->reject || mr->sqllog) {
#else	
		if (mr->reject) {
#endif
		    if (referer) {
			char * ref_addr = HTAnchor_address((HTAnchor *) referer);
			if (mr->reject && ref_addr)
			    HTLog_addText(mr->reject, "%s --> %s\n", ref_addr, uri);
#ifdef HT_MYSQL
			if (mr->sqllog && mr->sqlexternals && ref_addr)
			    HTSQLLog_addLinkRelationship(mr->sqllog,
							 ref_addr, uri,
							 "image", alt);
#endif

			HT_FREE(ref_addr);
		    }
		}
	    }
	    HT_FREE(uri);
	}
    }
}

PUBLIC void HText_endAnchor (HText * text) {}
PUBLIC void HText_appendText (HText * text, const char * str) {}
PUBLIC void HText_appendCharacter (HText * text, char ch) {}
PUBLIC void HText_endAppend (HText * text) {}
PUBLIC void HText_setStyle (HText * text, HTStyle * style) {}
PUBLIC void HText_beginAppend (HText * text) {}
PUBLIC void HText_appendParagraph (HText * text) {}

PRIVATE int RobotTrace (const char * fmt, va_list pArgs)
{
    return (vfprintf(stderr, fmt, pArgs));
}

/* ------------------------------------------------------------------------- */
/*				  MAIN PROGRAM				     */
/* ------------------------------------------------------------------------- */

int main (int argc, char ** argv)
{
    int		status = 0;
    int		arg;
    BOOL	cache = NO;			     /* Use persistent cache */
    BOOL	flush = NO;		       /* flush the persistent cache */
    char *	cache_root = NULL;
    HTChunk *	keywords = NULL;			/* From command line */
    int		keycnt = 0;
    Robot *	mr = NULL;
    Finger *	finger = NULL;
    HTParentAnchor * startAnchor = NULL;

    /* Starts Mac GUSI socket library */
#ifdef GUSI
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef __MWERKS__ /* STR */
    InitGraf((Ptr) &qd.thePort); 
    InitFonts(); 
    InitWindows(); 
    InitMenus(); TEInit(); 
    InitDialogs(nil); 
    InitCursor();
    SIOUXSettings.asktosaveonclose = false;
    argc=ccommand(&argv);
#endif /* __MWERKS__ */

#ifdef HT_MEMLOG
    HTMemLog_open(DEFAULT_MEMLOG, 8192, YES);
#endif

    /* Initiate W3C Reference Library with a robot profile */
    HTProfile_newRobot(APP_NAME, APP_VERSION);
    HTTrace_setCallback(RobotTrace);

    /* Add the default HTML parser to the set of converters */
    {
	HTList * converters = HTFormat_conversion();
	HTMLInit(converters);
    }

    /* Build a new robot object */
    mr = Robot_new();

    /* Scan command Line for parameters */
    for (arg=1; arg<argc; arg++) {
	if (*argv[arg] == '-') {
	    
	    /* non-interactive */
	    if (!strcmp(argv[arg], "-n")) {
		HTAlert_setInteractive(NO);

  	    /* help */
	    } else if (!strcmp(argv[arg], "-h") || !strcmp(argv[arg], "-?")) {
		VersionInfo();
		Cleanup(mr, 0);

  	    /* clf log file */
	    } else if (!strcmp(argv[arg], "-l")) {
		mr->logfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_LOG_FILE;
		mr->flags |= MR_LOGGING;

  	    /* referer log file */
	    } else if (!strncmp(argv[arg], "-ref", 4)) {
		mr->reffile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_REFERER_FILE;
		mr->flags |= MR_LOGGING;

  	    /* Not found error log file */
	    } else if (!strncmp(argv[arg], "-404", 4)) {
		mr->notfoundfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_NOTFOUND_FILE;
		mr->flags |= MR_LOGGING;

  	    /* reject log file */
	    } else if (!strncmp(argv[arg], "-rej", 4)) {
		mr->rejectfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_REJECT_FILE;
		mr->flags |= MR_LOGGING;

  	    /* no alt tags log file */
	    } else if (!strncmp(argv[arg], "-alt", 4)) {
		mr->noalttagfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_NOALTTAG_FILE;
		mr->flags |= MR_LOGGING;

  	    /* negotiated resource log file */
	    } else if (!strncmp(argv[arg], "-neg", 4)) {
		mr->connegfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_CONNEG_FILE;
		mr->flags |= MR_LOGGING;

  	    /* hit file log */
	    } else if (!strcmp(argv[arg], "-hit")) {
		mr->hitfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_HIT_FILE;
		mr->flags |= MR_DISTRIBUTIONS;

  	    /* link relations file log */
	    } else if (!strcmp(argv[arg], "-rellog")) {
		mr->relfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_REL_FILE;
		mr->flags |= MR_DISTRIBUTIONS;

  	    /* Specific link relation to look for (only used i also -rellog) */
	    } else if (!strcmp(argv[arg], "-relation")) {
		mr->relation = (arg+1 < argc && *argv[arg+1] != '-') ?
		    (HTLinkType) HTAtom_caseFor(argv[++arg]) : NULL;
		mr->flags |= MR_DISTRIBUTIONS;

  	    /* last modified log file */
	    } else if (!strcmp(argv[arg], "-lm")) {
		mr->lmfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_LM_FILE;
		mr->flags |= MR_DISTRIBUTIONS;

  	    /* title log file */
	    } else if (!strcmp(argv[arg], "-title")) {
		mr->titlefile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_TITLE_FILE;
		mr->flags |= MR_DISTRIBUTIONS;

  	    /* mediatype distribution log file */
	    } else if (!strncmp(argv[arg], "-for", 4)) {
		mr->mtfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_FORMAT_FILE;
		mr->flags |= (MR_KEEP_META | MR_DISTRIBUTIONS);

  	    /* charset distribution log file */
	    } else if (!strncmp(argv[arg], "-char", 5)) {
		mr->charsetfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_CHARSET_FILE;
		mr->flags |= (MR_KEEP_META | MR_DISTRIBUTIONS);

            /* rule file */
	    } else if (!strcmp(argv[arg], "-r")) {
		mr->rules = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_RULE_FILE;

	    /* output filename */
	    } else if (!strcmp(argv[arg], "-o")) { 
		mr->outputfile = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_OUTPUT_FILE;

	    /* URI prefix */
	    } else if (!strcmp(argv[arg], "-prefix")) {
		char * prefix = NULL;
		prefix = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_PREFIX;
		if (*prefix && *prefix != '*') {
		    StrAllocCopy(mr->prefix, prefix);
		    StrAllocCat(mr->prefix, "*");
		}

	    /* timeout -- Change the default request timeout */
	    } else if (!strcmp(argv[arg], "-timeout")) {
		int timeout = (arg+1 < argc && *argv[arg+1] != '-') ?
		    atoi(argv[++arg]) : DEFAULT_TIMEOUT;
		if (timeout > 0) mr->timer = timeout;

	    /* Force no pipelined requests */
	    } else if (!strcmp(argv[arg], "-nopipe")) {
		HTTP_setConnectionMode(HTTP_11_NO_PIPELINING);

	    /* Start the persistent cache */
	    } else if (!strcmp(argv[arg], "-cache")) {
		cache = YES;

	    /* Determine the cache root */
	    } else if (!strcmp(argv[arg], "-cacheroot")) { 
		cache_root = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : NULL;

	    /* Stream write flush delay in ms */
	    } else if (!strcmp(argv[arg], "-delay")) {
		int delay = (arg+1 < argc && *argv[arg+1] != '-') ?
		    atoi(argv[++arg]) : DEFAULT_DELAY;
		HTHost_setDefaultWriteDelay(delay);

	    /* Persistent cache flush */
	    } else if (!strcmp(argv[arg], "-flush")) {
		flush = YES;

	    /* Do a cache validation */
	    } else if (!strcmp(argv[arg], "-validate")) {
		mr->flags |= MR_VALIDATE;

	    /* Do an end-to-end cache-validation */
	    } else if (!strcmp(argv[arg], "-endvalidate")) {
		mr->flags |= MR_END_VALIDATE;

	    /* preemptive or non-preemptive access */
	    } else if (!strcmp(argv[arg], "-single")) {
		mr->flags |= MR_PREEMPTIVE;

	    /* test inlined images */
	    } else if (!strcmp(argv[arg], "-img")) {
		mr->flags |= MR_IMG;

	    /* load inlined images */
	    } else if (!strcmp(argv[arg], "-saveimg")) {
		mr->flags |= (MR_IMG | MR_SAVE);

	    /* URI prefix for inlined images */
	    } else if (!strcmp(argv[arg], "-imgprefix")) {
		char * prefix = NULL;
		prefix = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_IMG_PREFIX;
		if (*prefix && *prefix!='*') {
		    StrAllocCopy(mr->img_prefix, prefix);
		    StrAllocCat(mr->img_prefix, "*");
		}

	    /* load anchors */
	    } else if (!strcmp(argv[arg], "-link") || !strcmp(argv[arg], "-depth")) {
		mr->flags |= MR_LINK;
		mr->depth = (arg+1 < argc && *argv[arg+1] != '-') ?
		    atoi(argv[++arg]) : DEFAULT_DEPTH;

	    /* Output start and end time */
	    } else if (!strcmp(argv[arg], "-ss")) {
		mr->flags |= MR_TIME;

	    /* print version and exit */
	    } else if (!strcmp(argv[arg], "-version")) { 
		VersionInfo();
		Cleanup(mr, 0);

	    /* run in quiet mode */
	    } else if (!strcmp(argv[arg], "-q")) { 
		mr->flags |= MR_QUIET;

	    /* run in really quiet mode */
	    } else if (!strcmp(argv[arg], "-Q")) { 
		mr->flags |= MR_REAL_QUIET;

#ifdef WWWTRACE
	    /* trace flags */
	    } else if (!strncmp(argv[arg], "-v", 2)) {
		HTSetTraceMessageMask(argv[arg]+2);
#endif

#ifdef HT_POSIX_REGEX

	    /* If we can link against a POSIX regex library */
	    } else if (!strncmp(argv[arg], "-inc", 4)) {
		if (arg+1 < argc && *argv[arg+1] != '-') {
		    mr->include = get_regtype(mr, argv[++arg], W3C_REGEX_FLAGS);
		}
	    } else if (!strncmp(argv[arg], "-exc", 4)) {
		if (arg+1 < argc && *argv[arg+1] != '-') {
		    mr->exclude = get_regtype(mr, argv[++arg], W3C_REGEX_FLAGS);
		}
	    } else if (!strncmp(argv[arg], "-check", 6)) {
		if (arg+1 < argc && *argv[arg+1] != '-') {
		    mr->check = get_regtype(mr, argv[++arg], W3C_REGEX_FLAGS);
		}
#endif

#ifdef HT_MYSQL
	    /* If we can link against a MYSQL database library */
	    } else if (!strncmp(argv[arg], "-sqldb", 5)) {
		mr->sqldb = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_SQL_DB;

	    } else if (!strncmp(argv[arg], "-sqlclearlinks", 10)) {
		mr->sqlflags |= HTSQLLOG_CLEAR_LINKS_TABLE;

	    } else if (!strncmp(argv[arg], "-sqlclearrequests", 12)) {
		mr->sqlflags |= HTSQLLOG_CLEAR_REQUESTS_TABLE;

	    } else if (!strncmp(argv[arg], "-sqlclearresources", 12)) {
		mr->sqlflags |= HTSQLLOG_CLEAR_RESOURCES_TABLE;

	    } else if (!strncmp(argv[arg], "-sqlclearuris", 10)) {
		mr->sqlflags |= HTSQLLOG_CLEAR_URIS_TABLE;

	    } else if (!strncmp(argv[arg], "-sqlexternals", 5)) {
		mr->sqlexternals = YES;

	    } else if (!strncmp(argv[arg], "-sqlpassword", 5)) {
		mr->sqlpw = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_SQL_PW;

	    } else if (!strncmp(argv[arg], "-sqlrelative", 5)) {
		mr->sqlrelative = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : NULL;

	    } else if (!strncmp(argv[arg], "-sqlserver", 5)) {
		mr->sqlserver = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_SQL_SERVER;

	    } else if (!strncmp(argv[arg], "-sqluser", 5)) {
		mr->sqluser = (arg+1 < argc && *argv[arg+1] != '-') ?
		    argv[++arg] : DEFAULT_SQL_USER;

#endif

	    } else {
		if (SHOW_REAL_QUIET(mr)) HTTrace("Bad Argument (%s)\n", argv[arg]);
	    }
       } else {	 /* If no leading `-' then check for URL or keywords */
    	    if (!keycnt) {
		char * ref = HTParse(argv[arg], mr->cwd, PARSE_ALL);
		startAnchor = HTAnchor_parent(HTAnchor_findAddress(ref));
		HyperDoc_new(mr, startAnchor, 0);
		keycnt = 1;
		HT_FREE(ref);
	    } else {		   /* Check for successive keyword arguments */
		char *escaped = HTEscape(argv[arg], URL_XALPHAS);
		if (keycnt++ <= 1)
		    keywords = HTChunk_new(128);
		else
		    HTChunk_putc(keywords, ' ');
		HTChunk_puts(keywords, HTStrip(escaped));
		HT_FREE(escaped);
	    }
	}
    }

#ifdef CATCH_SIG
    SetSignal();
#endif

    if (!keycnt) {
	if (SHOW_REAL_QUIET(mr)) HTTrace("Please specify URL to check.\n");
	Cleanup(mr, -1);
    }

    if (mr->depth != DEFAULT_DEPTH && 
	(mr->prefix == NULL || *mr->prefix == '*')) {
	if (SHOW_REAL_QUIET(mr))
	    HTTrace("A depth of more than 0 requires that you also specify a URI prefix.\n",
		    mr->depth);
	Cleanup(mr, -1);
    }

    /* Testing that HTTrace is working */
    if (mr->flags & MR_TIME) {
	if (SHOW_REAL_QUIET(mr)) {
	    time_t local = time(NULL);
	    HTTrace("Welcome to the W3C mini Robot version %s - started on %s\n",
		    APP_VERSION, HTDateTimeStr(&local, YES));
	}
    }

    /* Rule file specified? */
    if (mr->rules) {
	char * rules = HTParse(mr->rules, mr->cwd, PARSE_ALL);
	if (!HTLoadRules(rules))
	    if (SHOW_REAL_QUIET(mr)) HTTrace("Can't access rules\n");
	HT_FREE(rules);
    }

    /* Output file specified? */
    if (mr->outputfile) {
	if ((mr->output = fopen(mr->outputfile, "wb")) == NULL) {
	    if (SHOW_REAL_QUIET(mr)) HTTrace("Can't open `%s'\n", mr->outputfile);
	    mr->output = OUTPUT;
	}
    }

    /* Should we use persistent cache? */
    if (cache) {
	HTCacheInit(cache_root, 20);
	HTNet_addBefore(HTCacheFilter, "http://*", NULL, HT_FILTER_MIDDLE);
	HTNet_addAfter(HTCacheUpdateFilter, "http://*", NULL,
		       HT_NOT_MODIFIED, HT_FILTER_MIDDLE);

	/* Should we start by flushing? */
	if (flush) HTCache_flushAll();
    }

    /* SQL Log specified? */
#ifdef HT_MYSQL
    if (mr->sqlserver) {
	if ((mr->sqllog =
	     HTSQLLog_connect(mr->sqlserver,
			      mr->sqluser ? mr->sqluser : DEFAULT_SQL_USER,
			      mr->sqlpw ? mr->sqlpw : DEFAULT_SQL_PW)) != NULL) {
	    HTSQLLog_openDB(mr->sqllog, mr->sqldb ? mr->sqldb : DEFAULT_SQL_DB,
			    mr->sqlflags);
	    if (mr->sqlrelative) HTSQLLog_makeRelativeTo(mr->sqllog, mr->sqlrelative);
	}
    }
#endif

    /* CLF Log file specified? */
    if (mr->logfile) {
        mr->log = HTLog_open(mr->logfile, YES, YES);
        if (mr->log) HTNet_addAfter(HTLogFilter, NULL, mr->log, HT_ALL, HT_FILTER_LATE);
    }

    /* Referer Log file specified? */
    if (mr->reffile) {
        mr->ref = HTLog_open(mr->reffile, YES, YES);
        if (mr->ref)
	    HTNet_addAfter(HTRefererFilter, NULL, mr->ref, HT_ALL, HT_FILTER_LATE);
    }

    /* Not found error log specified? */
    if (mr->notfoundfile) {
        mr->notfound = HTLog_open(mr->notfoundfile, YES, YES);
        if (mr->notfound)
	    HTNet_addAfter(HTRefererFilter, NULL, mr->notfound, -404, HT_FILTER_LATE);
    }

    /* Negotiated resource log specified? */
    if (mr->connegfile) mr->conneg = HTLog_open(mr->connegfile, YES, YES);

    /* No alt tags log file specified? */
    if (mr->noalttagfile) mr->noalttag = HTLog_open(mr->noalttagfile, YES, YES);

    /* Reject Log file specified? */
    if (mr->rejectfile) mr->reject = HTLog_open(mr->rejectfile, YES, YES);

    /* Register our own terminate filter */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /* Setting event timeout */
    HTHost_setEventTimeout(mr->timer);

    mr->time = HTGetTimeInMillis();

    /* Start the request */
    finger = Finger_new(mr, startAnchor, METHOD_GET);

    /*
    ** Make sure that the first request is flushed immediately and not
    ** buffered in the output buffer
    */
    HTRequest_setFlush(finger->request, YES);

    /*
    ** Check whether we should do some kind of cache validation on
    ** the load
    */
    if (mr->flags & MR_VALIDATE)
	HTRequest_setReloadMode(finger->request, HT_CACHE_VALIDATE);
    if (mr->flags & MR_END_VALIDATE)
	HTRequest_setReloadMode(finger->request, HT_CACHE_END_VALIDATE);

    /*
    **  Now do the load
    */
    if (mr->flags & MR_PREEMPTIVE)
	HTRequest_setPreemptive(finger->request, YES);

    if (keywords)						   /* Search */
	status = HTSearchAnchor(keywords, (HTAnchor *)startAnchor, finger->request);
    else
	status = HTLoadAnchor((HTAnchor *)startAnchor, finger->request);

    if (keywords) HTChunk_delete(keywords);
    if (status != YES) {
	if (SHOW_REAL_QUIET(mr)) HTTrace("Can't access resource\n");
	Cleanup(mr, -1);
    }

    /* Go into the event loop... */
    HTEventList_loop(finger->request);

    /* Only gets here if event loop fails */
    Cleanup(mr, 0);
    return 0;
}
