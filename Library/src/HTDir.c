/*							       	     HTDir.c
**	DIRECTORY BROWSING
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This is unix-specific code in general
**	The module is intended for use in HTFile.c and HTFTP.c where
**	it replaces the old directory browsing routine.
**	The module is only compiled if GOT_READ_DIR is defined
**
** Authors:
**		HF	Henrik Frystyk, MIT, <frystyk@w3.org>
** History:
**	   Sep 95  HFN	written
**
** Note:
**	It could be a HTML table instead
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "WWWFile.h"
#include "WWWHTML.h"
#include "HTIcons.h"
#include "HTDescpt.h"
#include "HTDir.h"					 /* Implemented here */

/* Macros and other defines */
#define PUTC(c)		(*target->isa->put_character)(target, c)
#define PUTS(s)		(*target->isa->put_string)(target, s)
#define START(e)	(*target->isa->start_element)(target, e, 0, 0)
#define END(e)		(*target->isa->end_element)(target, e)
#define FREE_TARGET	(*target->isa->_free)(target)

#define DEFAULT_MINFW	15
#define DEFAULT_MAXFW	25

/* Type definitions and global variables etc. local to this module */

struct _HTStructured {
    const HTStructuredClass *	isa;
    /* ... */
};

struct _HTDir {
    HTStructured *	target;
    HTRequest *		request;
    HTArray *		array;			/* Array for sorted listings */
    char *		fnbuf;				 /* File name buffer */
    char *		lnbuf;				     /* Rest of line */
    char *		base;				  /* base url is any */
    HTDirShow		show;			  /* What do we want to show */
    HTDirKey		key;				  /* Key for sorting */
    int			size;				  /* Number of files */
    int 		curfw; 		     /* Max file name length in list */
};

typedef struct _HTDirNode {
    char *	fname;
    char *	date;
    char *	size;
    char *	note;
    HTFileMode	mode;
} HTDirNode;

typedef enum _HTShowLength {                        /* Width of each collumn */
    HT_DLEN_SIZE  = 6,
    HT_DLEN_DATE  = 15,
    HT_DLEN_SPACE = 1,
    HT_DLEN_DES	  = 25
} HTShowLength;

PRIVATE int MinFileW = DEFAULT_MINFW;
PRIVATE int MaxFileW = DEFAULT_MAXFW;

/* ------------------------------------------------------------------------- */
/*				LINE JUSTIFICATION 			     */
/* ------------------------------------------------------------------------- */

/*
**	Left-justifies str_in into str_out expecting str_out having size length
**	l is number of chars. No 0 termination, rest of line filled with ' '
*/
PRIVATE void LeftStr (char **outstr, char * instr, int l)
{
    char *out = *outstr;
    while (l-- > 0 && *instr && (*out++ = *instr++));
    while (l-- > 0) *out++ = ' ';
    *outstr = out;
}

/*
**	Like LeftStr(), but result is right-justified.
**	l is number of chars. No 0 termination
*/
PRIVATE void RightStr (char **outstr, char * instr, int l)
{
    char *start = *outstr+l-strlen(instr);
    char *out = *outstr;
    while (out<start) *out++ = ' ';
    while (*instr && (*out++ = *instr++));
    *outstr = out;
}

/* ------------------------------------------------------------------------- */
/*				NODE  MANAGEMENT 			     */
/* ------------------------------------------------------------------------- */

/*
**	Create a sort key node
*/
PRIVATE HTDirNode * HTDirNode_new (void)
{
    HTDirNode *node;
    if ((node = (HTDirNode *) HT_CALLOC(1, sizeof(HTDirNode))) == NULL)
        HT_OUTOFMEM("HTDirNode_new");
    return node;
}

/*
**	Free a sort key node
*/
PRIVATE BOOL HTDirNode_free (HTDirNode *node)
{
    if (node) {
	HT_FREE(node->fname);
	HT_FREE(node->date);
	HT_FREE(node->size);
	HT_FREE(node->note);
	HT_FREE(node);
	return YES;
    }
    return NO;
}

/*
**  Escape a filename and add a '/' if it's a directory
*/
PRIVATE char * expand_name (char * name, HTFileMode mode)
{
    char * escaped = HTEscape(name, URL_XPALPHAS);
    if (mode == HT_IS_DIR)
	if (*(name+strlen(name)-1) != '/') StrAllocCat(escaped, "/");
    return escaped;
}

/*
**	Output an element in HTML
**	Returns YES if OK, else NO
*/
PRIVATE BOOL HTDirNode_print (HTDir *dir, HTDirNode *node)
{
    char *tp = NULL;
    HTStructured *target = dir->target;
    if (dir->show & HT_DS_ICON) {
	HTFormat format = NULL;
	HTEncoding encoding = NULL;
	double q=1.0;
	HTIconNode * icon;
	if (node->mode == HT_IS_FILE)
	    HTBind_getFormat(node->fname, &format, &encoding, NULL, NULL, &q);
	icon = HTIcon_find(node->mode, format, encoding);

	/* Are we having a hot or a cold icon? */
	if (!(dir->show & HT_DS_HOTI)) {
	    if (icon) {
		char * alt = HTIcon_alternative(icon, YES);
		HTMLPutImg(target, HTIcon_url(icon), alt, NULL);
		HT_FREE(alt);
		PUTC(' ');
	    }
	}

	/* Start the anchor element */
	if (dir->base) {
	    char *escaped = expand_name(node->fname, node->mode);
	    char *full;
	    if ((full = (char *) HT_MALLOC(strlen(escaped)+strlen(dir->base)+1)) == NULL)
		HT_OUTOFMEM("HTDirNode_print");
	    strcpy(full, dir->base);
	    strcat(full, escaped);
	    HTStartAnchor(target, NULL, full);
	    HT_FREE(escaped);
	    HT_FREE(full);
	} else {
	    char *escaped = expand_name(node->fname, node->mode);
	    HTStartAnchor(target, NULL, escaped);
	    HT_FREE(escaped);
	}

	if (dir->show & HT_DS_HOTI) {
	    char * alt = HTIcon_alternative(icon, YES);
	    HTMLPutImg(target, HTIcon_url(icon), alt, NULL);
	    PUTC(' ');
	}
    } else {
	if (dir->base) {
	    char *escaped = expand_name(node->fname, node->mode);
	    char *full;
	    if ((full = (char *) HT_MALLOC(strlen(escaped)+strlen(dir->base)+1)) == NULL)
		HT_OUTOFMEM("HTDirNode_print");
	    strcpy(full, dir->base);
	    strcat(full, escaped);
	    HTStartAnchor(target, NULL, escaped);
	    HT_FREE(escaped);
	    HT_FREE(full);
	} else {
	    char *escaped = HTEscape(node->fname, URL_XPALPHAS);
	    HTStartAnchor(target, NULL, escaped);
	    HT_FREE(escaped);
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
    return YES;
}

/* ------------------------------------------------------------------------- */
/*				DIRECTORY MANAGEMENT 			     */
/* ------------------------------------------------------------------------- */

/*	HTDir_headLine
**	--------------
**    	Puts out the header line of the list itself
**	Returns YES if OK, else NO
*/
PRIVATE BOOL HTDir_headLine (HTDir *dir)
{
    if (dir) {
	char *tp;
	HTStructured *target = dir->target;
	START(HTML_PRE);
	if (dir->show & HT_DS_ICON) {
	    HTIconNode * icon = HTIcon_find(HT_IS_BLANK, NULL, NULL);
	    if (icon) {
		char * alt = HTIcon_alternative(icon, NO);
		HTMLPutImg(target, HTIcon_url(icon), alt, NULL);
		HT_FREE(alt);
		PUTC(' ');
	    }
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
	END(HTML_PRE);
	START(HTML_HR);
	START(HTML_PRE);
	return YES;
    }
    return NO;
}

/*	HTDir_setWidth
**	--------------
**	The module automatically ajusts the width of the directory listing as
**	a function of the file name. The width can flows dynamically between
**	an upper and a lower limit.
*/
PUBLIC BOOL HTDir_setWidth (int minfile, int maxfile)
{
    MinFileW = (minfile>=0) ? minfile : 0;
    MaxFileW = (maxfile>minfile) ? maxfile : minfile+1;
    return YES;
}

/*	HTDir_new
**	---------
**    	Creates a structured stream object and sets up the initial HTML stuff
**	Returns the dir object if OK, else NULL
*/
PUBLIC HTDir * HTDir_new (HTRequest * request, HTDirShow show, HTDirKey key)
{    
    HTDir *dir;
    char *title = NULL;
    if (!request) return NULL;

    /* Create object */
    if ((dir = (HTDir *) HT_CALLOC(1, sizeof (HTDir))) == NULL ||
	(dir->fnbuf = (char *) HT_MALLOC(MaxFileW+HT_DLEN_SPACE)) == NULL)
	HT_OUTOFMEM("HTDir_new");
    dir->target = HTMLGenerator(request, NULL, WWW_HTML,
			       HTRequest_outputFormat(request),
			       HTRequest_outputStream(request));
    HTRequest_setOutputConnected(request, YES);
    HTAnchor_setFormat(HTRequest_anchor(request), WWW_HTML);
    dir->request = request;
    dir->show = show;
    dir->key = key;
    if (key==HT_DK_NONE)
	dir->curfw = MaxFileW;
    else {
	dir->curfw = MinFileW;
	dir->array = HTArray_new(256);
    }

    /* We're all OK */
    HTRequest_addError(request, ERR_INFO, NO, HTERR_OK, NULL, 0, "HTDir_new");

    /* Find the length of the fields */
    {
	int len = HT_DLEN_SPACE+1;
	if (show & HT_DS_SIZE) len += (HT_DLEN_SIZE+HT_DLEN_SPACE);
	if (show & HT_DS_DATE) len += (HT_DLEN_DATE+HT_DLEN_SPACE);
	if (show & HT_DS_DES) len += HT_DLEN_DES;
	if ((dir->lnbuf = (char *) HT_MALLOC(len)) == NULL)
	    HT_OUTOFMEM("HTDir_new");
    }

    /* Find the title and the base URL */
    {
	char *addr = HTAnchor_address((HTAnchor *) HTRequest_anchor(request));
	char *path = HTParse(addr, "", PARSE_PATH+PARSE_PUNCTUATION);
	char *ptr;
	if ((ptr = strchr(path, ';')) || (ptr = strchr(path, '?')))
	    *ptr = '\0';
	StrAllocCopy(title, path);
	HTUnEscape(title);				 	    /* Title */
	if((ptr=strrchr(path, '/')) && (ptr<path+strlen(path)-1 || ptr==path)){
	    StrAllocCopy(dir->base, ++ptr);
	    StrAllocCat(dir->base, "/");
	}
	HTTRACE(PROT_TRACE, "HTDir_new... base is `%s\'\n" _ dir->base ? dir->base : "");
	HT_FREE(addr);
	HT_FREE(path);
    }

    /* Start the HTML stuff */
    {
	HTStructured *target = dir->target;
	START(HTML_HTML);
	START(HTML_HEAD);
	START(HTML_TITLE);
	PUTS("Current index is ");
	PUTS(title);
	END(HTML_TITLE);
	END(HTML_HEAD);
	START(HTML_BODY);
	START(HTML_H1);
	PUTS("Index of ");
	PUTS(title);
	END(HTML_H1);
    }
    HT_FREE(title);
    return dir;
}

/*	HTDir_addElement
**	---------------
**    	This function accepts a directory line. "data" and "size", and
**	"description" can all be NULL
**	Returns YES if OK, else NO
*/
PUBLIC BOOL HTDir_addElement (HTDir *dir, char *name, char *date, char *size,
			      HTFileMode mode)
{
    HTDirNode *node = HTDirNode_new();
    if (!dir || !name) return NO;
    if ((node->fname = (char *) HT_MALLOC(strlen(name) + 2)) == NULL)
	HT_OUTOFMEM("HTDir_addElement");
    strcpy(node->fname, name);					/* Mandatory */
    if (dir->show & HT_DS_DATE && date) StrAllocCopy(node->date, date);
    if (dir->show & HT_DS_SIZE && size) StrAllocCopy(node->size, size);
    if (dir->show & HT_DS_DES) {
#if 0

	/* FIND DESCRIPTION */

#endif
    }


    /* Set the mode of the file */
    node->mode = mode;

    /* Should we display now or later? */
    if (dir->key == HT_DK_NONE) {
	if (!dir->size++) HTDir_headLine(dir);
	HTDirNode_print(dir, node);
	HTDirNode_free(node);
    } else {
	int slen = strlen(name);
	if (slen > dir->curfw)
	    dir->curfw = slen < MaxFileW ? slen : MaxFileW;
	HTArray_addObject(dir->array, (void *) node);
    }
    return YES;
}

PRIVATE int DirSort (const void *a, const void *b)
{
#if 0
    HTDirNode *aa = *(HTDirNode **) a;
    HTDirNode *bb = *(HTDirNode **) b;
    return strcmp(aa->fname, bb->fname);
#else
    return strcmp((*((HTDirNode **) a))->fname,
		  (*((HTDirNode **) b))->fname);
#endif
}

PRIVATE int DirCaseSort (const void *a, const void *b)
{
#if 0
    HTDirNode *aa = *(HTDirNode **) a;
    HTDirNode *bb = *(HTDirNode **) b;
    return strcasecomp(aa->fname, bb->fname);
#else
    return strcasecomp((*((HTDirNode **) a))->fname,
		       (*((HTDirNode **) b))->fname);
#endif
}

/*	HTDir_free
**	----------
**    	If we are sorting then do the sorting and put out the list,
**	else just append the end of the list.
*/
PUBLIC BOOL HTDir_free (HTDir * dir)
{
    if (!dir) return NO;
    if (dir->key != HT_DK_NONE) {
	HTArray *array = dir->array;
	void **data = NULL;
	HTDirNode *node;
	HTDir_headLine(dir);	
	HTArray_sort(array, (dir->key==HT_DK_CINS ? DirCaseSort : DirSort));
	node = (HTDirNode *) HTArray_firstObject(array, data);
	while (node) {
	    HTDirNode_print(dir, node);
	    HTDirNode_free(node);
	    node = (HTDirNode *) HTArray_nextObject(array, data);
	}
	dir->size = HTArray_size(array);
    	HTArray_delete(array);	
    }

    /* Put out the end of the HTML stuff */
    {
	HTStructured *target = dir->target;
	END(HTML_PRE);
	START(HTML_HR);
	START(HTML_PRE);
	if (!dir->size)
	    PUTS("Empty directory");
	else if (dir->size == 1)
	    PUTS("1 File");
	else {
	    char buffer[20];
	    sprintf(buffer, "%u files", dir->size);
	    PUTS(buffer);
	}
	END(HTML_PRE);
	END(HTML_BODY);
	END(HTML_HTML);
	FREE_TARGET;
    }

    HT_FREE(dir->fnbuf);
    HT_FREE(dir->lnbuf);
    HT_FREE(dir->base);
    HT_FREE(dir);
    return YES;
}
