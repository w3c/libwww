/*								       HTIcon.c
**	ICON MANAGEMENT
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This module contains the functions for initializing, adding
**	and selecting the icon for local directory listings, FTP and Gopher.
**
**	History:
**	   Mar 94	Written by Ari Luotonen, luotonen@dxcern.cern.ch
**			Henrik Frystyk, frystyk@w3.org
**
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTIcons.h"					 /* Implemented here */

struct _HTIconNode {
    char *	icon_url;
    char *	icon_alt;
    char *	type_templ;
};

/* Globals */
PRIVATE HTIconNode * icon_unknown = NULL;	/* Unknown file type */
PRIVATE HTIconNode * icon_blank = NULL;		/* Blank icon in heading */
PRIVATE HTIconNode * icon_parent = NULL;	/* Parent directory icon */
PRIVATE HTIconNode * icon_dir = NULL;		/* Directory icon */

/* Type definitions and global variables etc. local to this module */
PRIVATE HTList * icons = NULL;
PRIVATE int alt_len = 0;			/* Longest ALT text */

/* ------------------------------------------------------------------------- */

PRIVATE void alt_resize (char * alt)
{
    if (alt) {
	int len = strlen(alt);
	if (len > alt_len) alt_len = len;
    }
}

PRIVATE BOOL match (char * templ,
		    const char * actual)
{
    static char * c1 = NULL;
    static char * c2 = NULL;
    char * slash1;
    char * slash2;

    StrAllocCopy(c1,templ);
    StrAllocCopy(c2,actual);

    slash1 = strchr(c1,'/');
    slash2 = strchr(c2,'/');

    if (slash1 && slash2) {
	*slash1++ = 0;
	*slash2++ = 0;
	return HTStrMatch(c1,c2) && HTStrMatch(slash1,slash2);
    }
    else if (!slash1 && !slash2)
	return HTStrMatch(c1,c2) ? YES : NO;
    else
	return NO;
}


PRIVATE char * prefixed (const char * name, const char * prefix)
{
    char * ret = NULL;
    if (name) {
	int len = prefix ? strlen(prefix) : 0;
	if ((ret = (char *) HT_MALLOC(len + strlen(name) + 2)) == NULL)
	    HT_OUTOFMEM("prefixed");
	if (prefix) {
	    strcpy(ret, prefix);
	    if (*prefix && prefix[len-1] != '/') strcat(ret, "/");
	    strcat(ret, name);
	} else
	    strcpy(ret, name);
    }
    return ret;
}

PUBLIC char * HTIcon_url (HTIconNode * node)
{
    return node ? node->icon_url : NULL;
}

/*
**	Returned string must be freed by caller
*/
PUBLIC char * HTIcon_alternative (HTIconNode * node, BOOL brackets)
{
    char * ret = NULL;
    if (node) {
	char * p = NULL;
	int len = node->icon_alt ? strlen(node->icon_alt) : 0;
	if ((p = ret = (char *) HT_MALLOC(alt_len + 3)) == NULL)
	    HT_OUTOFMEM("HTIcon_alt_string");
	*p++ = brackets ? '[' : ' ';
	if (node->icon_alt) strcpy(p, node->icon_alt);
	p += len;
	while (len++ < alt_len) *p++=' ';
	*p++ = brackets ? ']' : ' ';
	*p = 0;
    }
    return ret;
}

/*
**	HTAddIcon(url, alt, type_templ) adds icon:
**
**		<IMG SRC="url" ALT="[alt]">
**
**	for files for which content-type or content-encoding matches
**	type_templ.  If type_templ contains a slash, it is taken to be
**	a content-type template.  Otherwise, it is a content-encoding
**	template.
*/
PUBLIC BOOL HTIcon_add (const char * url, const char * prefix,
			char * alt, char * type_templ)
{
    if (url && type_templ) {
	HTIconNode * node;
	if ((node = (HTIconNode *) HT_CALLOC(1,sizeof(HTIconNode))) == NULL)
	    HT_OUTOFMEM("HTAddIcon");
	if (url) node->icon_url = prefixed(url, prefix);
	if (alt) StrAllocCopy(node->icon_alt, alt);
	if (type_templ) StrAllocCopy(node->type_templ, type_templ);
	if (!icons) icons = HTList_new();
	HTList_addObject(icons, (void *) node);
	alt_resize(alt);
	HTTRACE(PROT_TRACE, "AddIcon..... %s => SRC=\"%s\" ALT=\"%s\"\n" _ 
		    type_templ _ url _ alt ? alt : "");
	return YES;
    }
    return NO;
}

/*
**	Add the icon used for files for which
**	no other icon seems appropriate (unknown type).
*/
PUBLIC BOOL HTIcon_addUnknown (const char * url, const char * prefix,
			       char * alt)
{
    if ((icon_unknown = (HTIconNode *) HT_CALLOC(1,sizeof(HTIconNode)))==NULL)
        HT_OUTOFMEM("HTAddUnknownIcon");
    if (url) icon_unknown->icon_url = prefixed(url, prefix);
    if (alt) StrAllocCopy(icon_unknown->icon_alt, alt);
    alt_resize(alt);
    HTTRACE(PROT_TRACE, "Icon add.... UNKNOWN => SRC=\"%s\" ALT=\"%s\"\n" _ url _ 
		alt ? alt : "");
    return YES;
}


/*
**	Add the blank icon used in the heading of the listing.
*/
PUBLIC BOOL HTIcon_addBlank (const char * url, const char * prefix, char * alt)
{
    if ((icon_blank = (HTIconNode *) HT_CALLOC(1,sizeof(HTIconNode))) == NULL)
        HT_OUTOFMEM("HTAddBlankIcon");
    if (url) icon_blank->icon_url = prefixed(url, prefix);
    if (alt) StrAllocCopy(icon_blank->icon_alt, alt);
    alt_resize(alt);
    HTTRACE(PROT_TRACE, "Icon add.... BLANK => SRC=\"%s\" ALT=\"%s\"\n" _ url _ 
		alt ? alt : "");
    return YES;
}


/*
**	Add the parent directory icon.
*/
PUBLIC BOOL HTIcon_addParent (const char * url, const char * prefix, char * alt)
{
    if ((icon_parent = (HTIconNode *) HT_CALLOC(1,sizeof(HTIconNode))) == NULL)
        HT_OUTOFMEM("HTAddBlankIcon");
    if (url) icon_parent->icon_url = prefixed(url, prefix);
    if (alt) StrAllocCopy(icon_parent->icon_alt, alt);
    alt_resize(alt);
    HTTRACE(PROT_TRACE, "Icon add.... PARENT => SRC=\"%s\" ALT=\"%s\"\n" _ url _ 
		alt ? alt : "");
    return YES;
}


/*
**	Add the directory icon.
*/
PUBLIC BOOL HTIcon_addDir (const char * url, const char * prefix, char * alt)
{
    if ((icon_dir = (HTIconNode *) HT_CALLOC(1,sizeof(HTIconNode))) == NULL)
        HT_OUTOFMEM("HTAddBlankIcon");
    if (url) icon_dir->icon_url = prefixed(url, prefix);
    if (alt) StrAllocCopy(icon_dir->icon_alt, alt);
    alt_resize(alt);
    HTTRACE(PROT_TRACE, "Icon add.... DIRECTORY => SRC=\"%s\" ALT=\"%s\"\n" _ url _ 
		alt ? alt : "");
    return YES;
}

/*
**	Returns the icon corresponding to content_type or content_encoding.
**	If no match is found then use "unknown icon"
*/
PUBLIC HTIconNode * HTIcon_find (HTFileMode	mode,
				 HTFormat	content_type,
				 HTEncoding	content_encoding)
{
    if (!icon_unknown) icon_unknown = icon_blank;
    if (mode == HT_IS_FILE) {
	const char * ct = content_type ? HTAtom_name(content_type) : NULL;
	const char * ce = content_encoding ? HTAtom_name(content_encoding) : NULL;
	HTList * cur = icons;
	HTIconNode * node;

	while ((node = (HTIconNode*)HTList_nextObject(cur))) {
	    char * slash = strchr(node->type_templ,'/');
	    if ((ct && slash && match(node->type_templ,ct)) ||
		(ce && !slash && HTStrMatch(node->type_templ,ce))) {
		return node;
	    }
	}
    } else if (mode == HT_IS_DIR) {
	return icon_dir ? icon_dir : icon_unknown;
    } else if (mode == HT_IS_BLANK) {
	return icon_blank ? icon_blank : icon_unknown;
    } else if (mode == HT_IS_PARENT) {
	return icon_parent ? icon_parent : icon_unknown;
    }
    return icon_unknown;
}

PRIVATE void HTIconNode_delete (HTIconNode* pNode)
{
    if (pNode) {
      HT_FREE(pNode->icon_url);
      HT_FREE(pNode->icon_alt);
      HT_FREE(pNode->type_templ);
      HT_FREE(pNode);
    }
}
/*
**  cleans up all memory used by icons. Should be called by
**  HTLibTerminate() (but it isn't)
**
*/
PUBLIC void HTIcon_deleteAll (void)
{
    if(icons != NULL) {
	HTList * iconList = icons;
	HTIconNode * node;
	while((node = (HTIconNode*)HTList_removeLastObject(iconList))) {
	  HTIconNode_delete(node);
	}
	/* delete the list as well */
	HTList_delete(icons);
	icons = NULL;
    }
    HTIconNode_delete(icon_unknown);
    icon_unknown = NULL;
    HTIconNode_delete(icon_blank);
    icon_blank = NULL;
    HTIconNode_delete(icon_parent);
    icon_parent = NULL;
    HTIconNode_delete(icon_dir);
    icon_dir = NULL;
}
