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
    char *		line;
    BOOL		top;				/* YES if first line */
    HTNewsDirKey	key;				  /* Key for sorting */
};

typedef struct _HTNewsNode {
    char *	key;
    int		index;
    char *	name;
    char *	subject;
    char *	from;
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
    if ((node = (HTNewsNode *) calloc(1, sizeof(HTNewsNode))) == NULL)
	outofmem(__FILE__, "HTNewsNode_new");
    return node;
}

/*
**	Free a sort key node
*/
PRIVATE BOOL HTNewsNode_free (HTNewsNode *node)
{
    if (node) {
	FREE(node->name);
	FREE(node->subject);
	FREE(node->from);
	free(node);
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
    char *tp = NULL;
    HTStructured *target = dir->target;
#if 1
    fprintf(TDEST, "%s\n", node->name);
#else
    if (dir->show & HT_DS_ICON) {
	HTFormat format = NULL;
	HTEncoding encoding = NULL;
	double q=1.0;
	HTIconNode *icon;
	HTHrefNode *href;
	if (node->mode == HT_IS_FILE)
	    HTBind_getFormat(node->fname, &format, &encoding, NULL, &q);
	icon = HTGetIcon(node->mode, format, encoding);
	href = HTGetHref(node->fname);

	/* Are we having a hot or a cold icon? */
	if (!(dir->show & HT_DS_HOTI)) {
	    HTMLPutImg(target, icon->icon_url,
		       HTIcon_alt_string(icon->icon_alt, YES), NULL);
	    PUTC(' ');
	}

	/* Start the anchor element */
	if (dir->base) {
	    char *escaped = HTEscape(node->fname, URL_XPALPHAS);
	    char *full = malloc(strlen(escaped)+strlen(dir->base)+1);
	    if (!full) outofmem(__FILE__, "HTNewsNode_print");
	    strcpy(full, dir->base);
	    strcat(full, escaped);
	    HTStartAnchor(target, NULL, full);
	    free(escaped);
	    free(full);
	} else {
	    char *escaped = HTEscape(node->fname, URL_XPALPHAS);
	    HTStartAnchor(target, NULL, escaped);
	    free(escaped);
	}

	if (dir->show & HT_DS_HOTI) {
	    HTMLPutImg(target, icon->icon_url,
		       HTIcon_alt_string(icon->icon_alt, YES), NULL);
	    PUTC(' ');
	}
    } else {
	if (dir->base) {
	    char *escaped = HTEscape(node->fname, URL_XPALPHAS);
	    char *full = malloc(strlen(escaped)+strlen(dir->base)+1);
	    if (!full) outofmem(__FILE__, "HTNewsNode_print");
	    strcpy(full, dir->base);
	    strcat(full, escaped);
	    HTStartAnchor(target, NULL, escaped);
	    free(escaped);
	    free(full);
	} else {
	    char *escaped = HTEscape(node->fname, URL_XPALPHAS);
	    HTStartAnchor(target, NULL, escaped);
	    free(escaped);
	}
    }
    
    /* Insert the anchor text and end anchor */
    {
	char *in = node->fname;
	char *out = dir->fnbuf;
	int l = dir->curfw;
	while (l-- > 0 && *in && (*out++ = *in++));
	if (*in)
	    *(out-1) = '>';
	else if (node->mode == HT_IS_DIR) {
	    *out++ = '/';
	    l--;
	}
	*out = '\0';
	PUTS(dir->fnbuf);
	END(HTML_A);
	out = dir->fnbuf;
	while (l-- >= 0) *out++ = ' ';
	LeftStr(&out, " ", HT_DLEN_SPACE);
	*out = '\0';
	PUTS(dir->fnbuf);
    }

    /* Print the rest of it */
    tp = dir->lnbuf;
    if (node->date) {
	RightStr(&tp, node->date, HT_DLEN_DATE);
	LeftStr(&tp, " ", HT_DLEN_SPACE);
    }
    if (node->size) {
	RightStr(&tp, node->size, HT_DLEN_SIZE);
	LeftStr(&tp, " ", HT_DLEN_SPACE);
    }
    if (node->note) {
	LeftStr(&tp, node->note, HT_DLEN_DES);
	LeftStr(&tp, " ", HT_DLEN_SPACE);
    }
    *tp = '\0';
    PUTS(dir->lnbuf);
    PUTC('\n');
#endif
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
	char *tp;
	HTStructured *target = dir->target;
#if 0
	START(HTML_PRE);
	if (dir->show & HT_DS_ICON) {
	    HTIconNode *icon = HTGetIcon(HT_IS_BLANK, NULL, NULL);
	    HTMLPutImg(target, icon->icon_url,
		       HTIcon_alt_string(icon->icon_alt, YES), NULL);
	    PUTC(' ');
	}

	tp = dir->fnbuf;
	LeftStr(&tp, "Name", dir->curfw);
	LeftStr(&tp, " ", HT_DLEN_SPACE);
	*tp = '\0';
	PUTS(dir->fnbuf);

	tp = dir->lnbuf;
	if (dir->show & HT_DS_DATE) {
	    LeftStr(&tp, "Last Modified", HT_DLEN_DATE);
	    LeftStr(&tp, " ", HT_DLEN_SPACE);
	}
	if (dir->show & HT_DS_SIZE) {
	    RightStr(&tp, "Size", HT_DLEN_SIZE);
	    LeftStr(&tp, " ", HT_DLEN_SPACE);
	}
	if (dir->show & HT_DS_DES) {
	    LeftStr(&tp, "Description", HT_DLEN_DATE);
	    LeftStr(&tp, " ", HT_DLEN_SPACE);
	}
	*tp = '\0';
	PUTS(dir->lnbuf);
	START(HTML_HR);
	PUTC('\n');
#endif
	return YES;
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
    if ((dir = (HTNewsDir *) calloc(1, sizeof (HTNewsDir))) == NULL ||
	(dir->line = (char *) malloc(MaxLineW)) == NULL)
	outofmem(__FILE__, "HTNewsDir_new");
    dir->target = HTMLGenerator(request, NULL, WWW_HTML,
				HTRequest_outputFormat(request),
				HTRequest_outputStream(request));
    dir->request = request;
    dir->top = YES;
    dir->key = key;
    if (key != HT_NDK_THREAD) {
	int total = HTNews_maxArticles();
	dir->array = HTArray_new(total > 0 ? total : 128);
    }

    /* Start the HTML stuff */
    {
	HTStructured *target = dir->target;
	char *msg = title ? title : "News Listing";
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
**    	This function accepts a news line. Everything except dir and nama can
**	can be 0 or NULL.
**	Returns YES if OK, else NO
*/
PUBLIC BOOL HTNewsDir_addElement (HTNewsDir * dir, char * name, int  index,
				  char * subject, char * from, int refs)
{
    HTNewsNode *node = HTNewsNode_new();
    if (!dir || !name) return NO;
    StrAllocCopy(node->name, name);				/* Mandatory */
    if (subject) StrAllocCopy(node->subject, subject);
    if (from) StrAllocCopy(node->from, from);
    if (dir->key == HT_NDK_THREAD) {
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

#if 0
PRIVATE int DirSort (CONST void *a, CONST void *b)
{
    HTNewsNode *aa = *(HTNewsNode **) a;
    HTNewsNode *bb = *(HTNewsNode **) b;
    return strcmp(aa->fname, bb->fname);
#if 0
    return strcmp((*(HTNewsNode**)a)->fname, (*(HTNewsNode**)a)->fname);
#endif
}

PRIVATE int DirCaseSort (CONST void *a, CONST void *b)
{
    HTNewsNode *aa = *(HTNewsNode **) a;
    HTNewsNode *bb = *(HTNewsNode **) b;
    return strcasecomp(aa->fname, bb->fname);
#if 0
    return strcasecomp((*(HTNewsNode**)a)->fname, (*(HTNewsNode**)a)->fname);
#endif
}
#endif

/*	HTNewsDir_free
**	--------------
**    	If we are sorting then do the sorting and put out the list,
**	else just append the end of the list.
*/
PUBLIC BOOL HTNewsDir_free (HTNewsDir * dir)
{
    if (!dir) return NO;
#if 0
    if (dir->key != HT_DK_THREAD) {
	HTArray *array = dir->array;
	void **data;
	HTNewsNode *node;
	HTNewsDir_headLine(dir);	
	HTArray_sort(array, (dir->key==HT_DK_CINS ? DirCaseSort : DirSort));
	node = (HTNewsNode *) HTArray_firstObject(array, data);
	while (node) {
	    HTNewsNode_print(dir, node);
	    HTNewsNode_free(node);
	    node = (HTNewsNode *) HTArray_nextObject(array, data);
	}
	dir->size = HTArray_size(array);
    	HTArray_delete(array);	
    }
#endif

    /* Put out the end of the HTML stuff */
    {
	HTStructured *target = dir->target;
	START(HTML_HR);
	END(HTML_BODY);
	END(HTML_HTML);
	FREE_TARGET;
    }

    FREE(dir->line);
    free(dir);
    return YES;
}
