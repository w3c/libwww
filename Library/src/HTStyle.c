/*
**	GENERIC STYLE IMPLEMENTATION FOR HYPERTEXT
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	Styles allow the translation between a logical property
**	of a piece of text and its physical representation.
**
**	A stylesheet is a collection of styles that can be applied
**	to the layout engine, for example provided by the HText
**	implementation.
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "HTStyle.h"

struct _HTStyle {
    char *	name;		/* Style name */
    int		element;	/* Element (if any) that this style applies to */
    void *	context;	/* Implementation specific style context */
};

struct _HTStyleSheet {
    char *	name;
    HTList *	styles;
};

/* ------------------------------------------------------------------------- */
/* 			      INDIVIDUAL STYLES				     */
/* ------------------------------------------------------------------------- */

PUBLIC HTStyle * HTStyle_new (const char * name, int element, void * context)
{
    HTStyle * style = NULL;
    if ((style = (HTStyle  *) HT_CALLOC(1, sizeof(HTStyle))) == NULL)
        HT_OUTOFMEM("HTStyleNew");
    StrAllocCopy(style->name, name ? name : "unknown");
    style->element = element;
    style->context = context;
    return style;
}

PUBLIC BOOL HTStyle_delete  (HTStyle * me)
{
    if (me) {
	HT_FREE(me->name);
	HT_FREE(me);
	return YES;
    }
    return NO;
}

/* ------------------------------------------------------------------------- */
/* 			          STYLE SHEETS				     */
/* ------------------------------------------------------------------------- */

PUBLIC HTStyleSheet * HTStyleSheet_new (const char * name)
{
    HTStyleSheet * ss;
    if ((ss = (HTStyleSheet *) HT_CALLOC(1, sizeof(HTStyleSheet))) == NULL)
        HT_OUTOFMEM("HTStyleSheet_new");
    StrAllocCopy(ss->name, name ? name : "unknown");
    ss->styles = HTList_new();
    return ss;
}

PUBLIC BOOL HTStyleSheet_delete (HTStyleSheet * me)
{
    if (me) {
	HTList * cur = me->styles;
	HTStyle * pres;
	while ((pres = (HTStyle *) HTList_nextObject(cur)))
	    HTStyle_delete(pres);
	HTList_delete(me->styles);
	HT_FREE(me);
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTStyleSheet_addStyle (HTStyleSheet * me, HTStyle * style)
{
    return (me && style) ? HTList_addObject(me->styles, style) : NO;
}

PUBLIC BOOL HTStyleSheet_deleteStyle (HTStyleSheet * me, HTStyle * style)
{
    if (me && style) {
	HTList_removeObject(me->styles, style);
	HTStyle_delete(style);
	return YES;
    }
    return NO;
}

PUBLIC HTStyle * HTStyleSheet_findStyleWithName (HTStyleSheet * me, const char * name)
{
    if (me && name) {
	HTList * cur = me->styles;
	HTStyle * pres;
	while ((pres = (HTStyle *) HTList_nextObject(cur))) {
	    if (!strcasecomp(pres->name, name)) return pres;
	}
	HTTRACE(SGML_TRACE, "StyleSheet.. No style named `%s' in stylesheet `%s'\n" _ 
		    name _ me->name);
    }
    return NULL;
}

PUBLIC HTStyle * HTStyleSheet_findStyleForElement (HTStyleSheet * me, int element)
{
    if (me) {
	HTList * cur = me->styles;
	HTStyle * pres;
	while ((pres = (HTStyle *) HTList_nextObject(cur))) {
	    if (pres->element==element) return pres;
	}
	HTTRACE(SGML_TRACE, "StyleSheet.. No style for element %d in stylesheet `%s'\n" _ 
		    element _ me->name);
    }
    return NULL;
}
