/*							       	     HTNDir.c
**	GENERIC NEWS LISTINGS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	Creates listings for all kind of News output.
**
** Authors:
**	HF	Henrik Frystyk, MIT, <frystyk@w3.org>
** History:
**	   Oct 95  HFN	written
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTMLPDTD.h"
#include "HTMLGen.h"
#include "HTEscape.h"
#include "HTParse.h"
#include "HTFormat.h"
#include "HTReq.h"
#include "HTStruct.h"
#include "HTArray.h"
#include "HTError.h"
#include "HTNews.h"
#include "HTNDir.h"					 /* Implemented here */

/* Macros and other defines */
#define PUTC(c)		(*target->isa->put_character)(target, c)
#define PUTS(s)		(*target->isa->put_string)(target, s)
#define START(e)	(*target->isa->start_element)(target, e, 0, 0)
#define END(e)		(*target->isa->end_element)(target, e)
#define FREE_TARGET	(*target->isa->_free)(target)

#define DEFAULT_MAXW	80			       /* Default line width */

/* Type definitions and global variables etc. local to this module */
struct _HTStructured {
    CONST HTStructuredClass *	isa;
    /* ... */
};

struct _HTNewsDir {
    HTStructured *	target;
    HTRequest *		request;
    HTArray *		array;			/* Array for sorted listings */
    BOOL		top;				/* YES if first line */
    HTNewsDirKey	key;				  /* Key for sorting */
};

typedef struct _HTNewsNode {
    int		index;
    char *	name;
    char *	subject;
    char *	from;
    time_t	date;
    int		refs;				     /* Number of references */
    BOOL	filter;			      /* Is this entry filtered out? */
} HTNewsNode;

PRIVATE int MaxLineW = DEFAULT_MAXW;

/* ------------------------------------------------------------------------- */
/*				NODE  MANAGEMENT 			     */
/* ------------------------------------------------------------------------- */

/*
**	Create a sort key node
*/
PRIVATE HTNewsNode * HTNewsNode_new (void)
{
    HTNewsNode *node;
    if ((node = (HTNewsNode *) HT_CALLOC(1, sizeof(HTNewsNode))) == NULL)
        HT_OUTOFMEM("HTNewsNode_new");
    return node;
}

/*
**	Free a sort key node
*/
PRIVATE BOOL HTNewsNode_free (HTNewsNode *node)
{
    if (node) {
	HT_FREE(node->name);
	HT_FREE(node->subject);
	HT_FREE(node->from);
	HT_FREE(node);
	return YES;
    }
    return NO;
}

/*
**	Output an element in HTML
**	Returns YES if OK, else NO
*/
PRIVATE BOOL HTNewsNode_print (HTNewsDir *dir, HTNewsNode *node)
{
    HTStructured *target = dir->target;
    START(HTML_LI);

    /* Start the anchor and put the subject as anchor text */
    if (node->name && node->subject) {
	char *escaped = HTEscape(node->name, URL_XPALPHAS);
	HTStartAnchor(target, NULL, escaped);
	PUTS(node->subject);
	END(HTML_A);
	HT_FREE(escaped);
    }

    /* From field */
    if (node->from) {
	PUTC(' ');
	PUTS(node->from);
    }
    return YES;
}

/* ------------------------------------------------------------------------- */
/*				DIRECTORY MANAGEMENT 			     */
/* ------------------------------------------------------------------------- */

/*	HTNewsDir_headLine
**	---------------
**    	Puts out the header line of the list itself
**	Returns YES if OK, else NO
*/
PRIVATE BOOL HTNewsDir_headLine (HTNewsDir *dir)
{
    if (dir) {
	HTStructured *target = dir->target;
	PUTS("HERE WE CAN PUT INFORMATION AND EXTRA LINKS\n");
	return YES;
	START(HTML_UL);
    }
    return NO;
}

/*	HTNewsDir_setWidth
**	------------------
**	The module automatically ajusts the width of the directory listing as
**	a function of the file name. The width can flows dynamically between
**	an upper and a lower limit.
*/
PUBLIC BOOL HTNewsDir_setWidth (int max_width)
{
    MaxLineW = (max_width > 0) ? max_width : DEFAULT_MAXW;
    return YES;
}

/*	HTNewsDir_new
**	----------
**    	Creates a structured stream object and sets up the initial HTML stuff
**	Returns the newsdir object if OK, else NULL
*/
PUBLIC HTNewsDir * HTNewsDir_new (HTRequest * request, CONST char * title,
				  HTNewsDirKey key)
{
    HTNewsDir *dir;
    if (!request) return NULL;

    /* Create object */
    if ((dir = (HTNewsDir *) HT_CALLOC(1, sizeof (HTNewsDir))) == NULL)
        HT_OUTOFMEM("HTNewsDir_new");
    dir->target = HTMLGenerator(request, NULL, WWW_HTML,
				HTRequest_outputFormat(request),
				HTRequest_outputStream(request));
    HTAnchor_setFormat(HTRequest_anchor(request), WWW_HTML);
    dir->request = request;
    dir->top = YES;
    dir->key = key;
    if (key != HT_NDK_NONE) {			       /* Thread is unsorted */
	int total = HTNews_maxArticles();
	dir->array = HTArray_new(total > 0 ? total : 128);
    }

    /* Start the HTML stuff */
    {
	HTStructured *target = dir->target;
	CONST char *msg = title ? title : "News Listing";
	START(HTML_HTML);
	START(HTML_HEAD);
	START(HTML_TITLE);
	PUTS(msg);
	END(HTML_TITLE);
	END(HTML_HEAD);
	START(HTML_BODY);
	START(HTML_H1);
	PUTS(msg);
	END(HTML_H1);
    }
    return dir;
}

/*	HTNewsDir_addElement
**	--------------------
**    	This function accepts a news line. Everything except dir and name can
**	can be 0 or NULL.
**	Returns YES if OK, else NO
*/
PUBLIC BOOL HTNewsDir_addElement (HTNewsDir * dir, int index, char * subject,
				  char * from, time_t date, char * name,
				  int refs)
{
    HTNewsNode *node = HTNewsNode_new();
    if (!dir || !name) return NO;
    StrAllocCopy(node->name, name);				/* Mandatory */
    if (subject) StrAllocCopy(node->subject, subject);
    if (from) StrAllocCopy(node->from, from);
    node->date = date;
    if (dir->key == HT_NDK_NONE) {
	if (dir->top) {
	    HTNewsDir_headLine(dir);
	    dir->top = NO;
	}
	HTNewsNode_print(dir, node);
	HTNewsNode_free(node);
    } else
	HTArray_addObject(dir->array, (void *) node);
    return YES;
}

PRIVATE int NDirIndexSort (CONST void *a, CONST void *b)
{
    int aa = (*((HTNewsNode **)a))->index;
    int bb = (*((HTNewsNode **)b))->index;
    return aa-bb;
}

PRIVATE int NDirSubjectSort (CONST void *a, CONST void *b)
{
#if 0
    char *aa = (*((HTNewsNode **)a))->subject;
    char *bb = (*((HTNewsNode **)b))->subject;
#endif
    return 0;
}

PRIVATE int NDirFromSort (CONST void *a, CONST void *b)
{
#if 0
    HTNewsNode *aa = *(HTNewsNode **) a;
    HTNewsNode *bb = *(HTNewsNode **) b;
    return strcasecomp(aa->fname, bb->fname);
    return strcasecomp((*(HTNewsNode**)a)->fname, (*(HTNewsNode**)a)->fname);

    char *aa = (*((HTNewsNode **)a))->name;
    char *bb = (*((HTNewsNode **)b))->name;
#endif
    return 1;
}

PRIVATE int NDirDateSort (CONST void *a, CONST void *b)
{
    time_t aa = (*((HTNewsNode **)a))->date;
    time_t bb = (*((HTNewsNode **)b))->date;
    return bb-aa;
}

PRIVATE int NDirGroupSort (CONST void *a, CONST void *b)
{
    char *aa = (*((HTNewsNode **)a))->name;
    char *bb = (*((HTNewsNode **)b))->name;
    while (*aa && *bb && TOLOWER(*aa)==TOLOWER(*bb)) aa++, bb++;
    return (*aa=='.' && *bb) ? -1 : (*aa && *bb=='.') ?
	1 : TOLOWER(*aa)-TOLOWER(*bb);
}

/*	HTNewsDir_free
**	--------------
**    	If we are sorting then do the sorting and put out the list,
**	else just append the end of the list.
*/
PUBLIC BOOL HTNewsDir_free (HTNewsDir * dir)
{
    if (!dir) return NO;
    if (dir->key != HT_NDK_NONE) {
	HTArray *array = dir->array;
	HTComparer * comp = NULL;
	HTNewsDir_headLine(dir);	
	if (dir->key == HT_NDK_INDEX)		   /* Sort by Message Number */
	    comp = NDirIndexSort;
	if (dir->key == HT_NDK_DATE)		       	     /* Sort by Date */
	    comp = NDirDateSort;
	if (dir->key == HT_NDK_SUBJECT)		       /* Sort after Subject */
	    comp = NDirSubjectSort;
	else if (dir->key == HT_NDK_FROM)		  /* Sort after From */
	    comp = NDirFromSort;
	else if (dir->key == HT_NDK_GROUP)	  /* Sort as group hierarchi */
	    comp = NDirGroupSort;
	else {
	    if (STREAM_TRACE) TTYPrint(TDEST,"NewsListing. Invalid sortkey\n");
	    return NO;
	}
	HTArray_sort(array, comp);
	{
	    void **data;
	    HTNewsNode *node = (HTNewsNode *) HTArray_firstObject(array, data);
	    while (node) {
		HTNewsNode_print(dir, node);
		HTNewsNode_free(node);
		node = (HTNewsNode *) HTArray_nextObject(array, data);
	    }
	}
    	HTArray_delete(array);	
    }

    /* Put out the end of the HTML stuff */
    {
	HTStructured *target = dir->target;
	END(HTML_UL);
	START(HTML_HR);
	END(HTML_BODY);
	END(HTML_HTML);
	FREE_TARGET;
    }
    HT_FREE(dir);
    return YES;
}
