/*			Icon Management		       		HTIcon.c
**			===============
**
**	This module contains the functions for initializing, adding
**	and selecting the icon for local directory listings, FTP and Gopher.
**
**	History:
**	   Mar 94	Written by Ari Luotonen, luotonen@ptsun00.cern.ch
**
*/

/* Library include files */
#include "HTMLPDTD.h"
#include "HTUtils.h"
#include "HTAnchor.h"
#include "HTParse.h"
#include "HTFile.h"
#include "HTFormat.h"
#include "HTChunk.h"
#include "HTIcons.h"					 /* Implemented here */

#ifdef VMS
typedef unsigned long mode_t;
#include "HTVMSUtils.h"
#endif /* VMS */

/* Globals */
PUBLIC BOOL HTDirShowBrackets = YES;
PUBLIC HTIconNode * icon_unknown = NULL;	/* Unknown file type */
PUBLIC HTIconNode * icon_blank = NULL;		/* Blank icon in heading */
PUBLIC HTIconNode * icon_parent = NULL;		/* Parent directory icon */
PUBLIC HTIconNode * icon_dir = NULL;		/* Directory icon */

/* Type definitions and global variables etc. local to this module */
PRIVATE HTList * icons = NULL;
PRIVATE int alt_len = 0;			/* Longest ALT text */

/* 
 * Global variable for the AddHref nodes
 * AddHref URL suff1 suff2 ...
 */
PRIVATE HTList * hrefs = NULL;


/* ------------------------------------------------------------------------- */

PRIVATE void alt_resize ARGS1(char *, alt)
{
    if (alt) {
	int len = strlen(alt);
	if (len > alt_len) alt_len = len;
    }
}


PUBLIC char * HTIcon_alt_string ARGS2(char *,	alt,
				      BOOL,	brackets)
{
    static char * ret = NULL;
    char * p = NULL;
    int len = alt ? strlen(alt) : 0;

    if (ret) free(ret);			/* From previous call */
    p = ret = (char*)malloc(alt_len + 3);
    if (!ret) outofmem(__FILE__, "HTIcon_alt_string");

    if (HTDirShowBrackets)
	*p++ = brackets ? '[' : ' ';
    if (alt) strcpy(p,alt);
    p += len;
    while (len++ < alt_len) *p++=' ';
    if (HTDirShowBrackets)
	*p++ = brackets ? ']' : ' ';
    *p = 0;

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
PUBLIC void HTAddIcon ARGS3(char *,	url,
			    char *,	alt,
			    char *,	type_templ)
{
    HTIconNode * node;

    if (!url || !type_templ) return;

    node = (HTIconNode*)calloc(1,sizeof(HTIconNode));
    if (!node) outofmem(__FILE__, "HTAddIcon");

    if (url) StrAllocCopy(node->icon_url, url);
    if (alt) StrAllocCopy(node->icon_alt, alt);
    if (type_templ) StrAllocCopy(node->type_templ, type_templ);

    if (!icons) icons = HTList_new();
    HTList_addObject(icons, (void*)node);
    alt_resize(alt);
    CTRACE(stderr,
	   "AddIcon..... %s => SRC=\"%s\" ALT=\"%s\"\n",type_templ,url,
	   alt ? alt : "");
}


/*
 * Put the AddHrefs in a list. It can be used for indexing to
 * present special filetypes through a CGI.
 */
PUBLIC void HTAddHref ARGS2(char *,     url,
                            char *,     type_templ)
{
    HTHrefNode * node;

    if (!url || !type_templ) return;

    node = (HTHrefNode*)calloc(1,sizeof(HTHrefNode));
    if (!node) outofmem(__FILE__, "HTAddHref");

    if (url) StrAllocCopy(node->href_url, url);
    if (type_templ) StrAllocCopy(node->type_templ, type_templ);

    if (!hrefs) hrefs = HTList_new();
    HTList_addObject(hrefs, (void*)node);
    CTRACE(stderr,
           "AddHref..... %s => URL=\"%s\" \n",type_templ,url);
}



/*
**	HTAddUnknownIcon(url,alt) adds the icon used for files for which
**	no other icon seems appropriate (unknown type).
*/
PUBLIC void HTAddUnknownIcon ARGS2(char *, url,
				   char *, alt)
{
    icon_unknown = (HTIconNode*)calloc(1,sizeof(HTIconNode));
    if (!icon_unknown) outofmem(__FILE__, "HTAddUnknownIcon");

    if (url) StrAllocCopy(icon_unknown->icon_url, url);
    if (alt) StrAllocCopy(icon_unknown->icon_alt, alt);
    alt_resize(alt);
    CTRACE(stderr,"AddIcon..... UNKNOWN => SRC=\"%s\" ALT=\"%s\"\n",url,
	   alt ? alt : "");

}


/*
**	HTAddBlankIcon(url,alt) adds the blank icon used in the
**	heading of the listing.
*/
PUBLIC void HTAddBlankIcon ARGS2(char *, url,
				 char *, alt)
{
    icon_blank = (HTIconNode*)calloc(1,sizeof(HTIconNode));
    if (!icon_blank) outofmem(__FILE__, "HTAddBlankIcon");

    if (url) StrAllocCopy(icon_blank->icon_url, url);
    if (alt) StrAllocCopy(icon_blank->icon_alt, alt);
    alt_resize(alt);
    CTRACE(stderr,"AddIcon..... BLANK => SRC=\"%s\" ALT=\"%s\"\n",url,
	   alt ? alt : "");
}


/*
**	HTAddParentIcon(url,alt) adds the parent directory icon.
*/
PUBLIC void HTAddParentIcon ARGS2(char *, url,
				  char *, alt)
{
    icon_parent = (HTIconNode*)calloc(1,sizeof(HTIconNode));
    if (!icon_parent) outofmem(__FILE__, "HTAddBlankIcon");

    if (url) StrAllocCopy(icon_parent->icon_url, url);
    if (alt) StrAllocCopy(icon_parent->icon_alt, alt);
    alt_resize(alt);
    CTRACE(stderr,"AddIcon..... PARENT => SRC=\"%s\" ALT=\"%s\"\n",url,
	   alt ? alt : "");
}


/*
**	HTAddDirIcon(url,alt) adds the directory icon.
*/
PUBLIC void HTAddDirIcon ARGS2(char *, url,
			       char *, alt)
{
    icon_dir = (HTIconNode*)calloc(1,sizeof(HTIconNode));
    if (!icon_dir) outofmem(__FILE__, "HTAddBlankIcon");

    if (url) StrAllocCopy(icon_dir->icon_url, url);
    if (alt) StrAllocCopy(icon_dir->icon_alt, alt);
    alt_resize(alt);
    CTRACE(stderr,
	   "AddIcon..... DIRECTORY => SRC=\"%s\" ALT=\"%s\"\n",url,
	   alt ? alt : "");
}


PRIVATE BOOL match ARGS2(char *, templ,
			 char *, actual)
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
	return HTAA_templateMatch(c1,c2) && HTAA_templateMatch(slash1,slash2);
    }
    else if (!slash1 && !slash2)
	return HTAA_templateMatch(c1,c2);
    else
	return NO;
}


PRIVATE char * prefixed ARGS2(CONST char *,	prefix,
			      char *,		name)
{
    static char * ret = NULL;
    FREE(ret);	/* From previous call */

    ret = (char *)malloc(strlen(prefix) + strlen(name) + 2);
    if (!ret) outofmem(__FILE__, "prefixed");

    strcpy(ret,prefix);
    if (*prefix && prefix[strlen(prefix)-1] != '/')
	strcat(ret,"/");
    strcat(ret,name);
    return ret;
}


PUBLIC void HTStdIconInit ARGS1(CONST char *, url_prefix)
{
    CONST char * p = url_prefix ? url_prefix : "/internal-icon/";

    HTAddBlankIcon  (prefixed(p,"blank.xbm"),	NULL	);
    HTAddDirIcon    (prefixed(p,"directory.xbm"),"DIR"	);
    HTAddParentIcon (prefixed(p,"back.xbm"),	"UP"	);
    HTAddUnknownIcon(prefixed(p,"unknown.xbm"),	NULL	);
    HTAddIcon(prefixed(p,"unknown.xbm"),	NULL,	"*/*");
    HTAddIcon(prefixed(p,"binary.xbm"),		"BIN",	"binary");
    HTAddIcon(prefixed(p,"unknown.xbm"),	NULL,	"www/unknown");
    HTAddIcon(prefixed(p,"text.xbm"),		"TXT",	"text/*");
    HTAddIcon(prefixed(p,"image.xbm"),		"IMG",	"image/*");
    HTAddIcon(prefixed(p,"movie.xbm"),		"MOV",	"video/*");
    HTAddIcon(prefixed(p,"sound.xbm"),		"AU",	"audio/*");
    HTAddIcon(prefixed(p,"tar.xbm"),		"TAR",	"multipart/x-tar");
    HTAddIcon(prefixed(p,"tar.xbm"),		"TAR",	"multipart/x-gtar");
    HTAddIcon(prefixed(p,"compressed.xbm"),	"CMP",	"x-compress");
    HTAddIcon(prefixed(p,"compressed.xbm"),	"GZP",	"x-gzip");
    HTAddIcon(prefixed(p,"index.xbm"),		"IDX",	"application/x-gopher-index");
    HTAddIcon(prefixed(p,"index2.xbm"),		"CSO",	"application/x-gopher-cso");
    HTAddIcon(prefixed(p,"telnet.xbm"),		"TEL",	"application/x-gopher-telnet");
    HTAddIcon(prefixed(p,"unknown.xbm"),       	"DUP",	"application/x-gopher-duplicate");
    HTAddIcon(prefixed(p,"unknown.xbm"),	"TN",	"application/x-gopher-tn3270");
}


/*								 HTGetIcon()
** returns the icon corresponding to content_type or content_encoding.
*/
PUBLIC HTIconNode * HTGetIcon ARGS3(mode_t,	mode,
				    HTFormat,	content_type,
				    HTFormat,	content_encoding)
{
    if (!icon_unknown) icon_unknown = icon_blank;

    if ((mode & S_IFMT) == S_IFREG) {
	char * ct = content_type ? HTAtom_name(content_type) : NULL;
	char * ce = content_encoding ? HTAtom_name(content_encoding) : NULL;
	HTList * cur = icons;
	HTIconNode * node;

	while ((node = (HTIconNode*)HTList_nextObject(cur))) {
	    char * slash = strchr(node->type_templ,'/');
	    if ((ct && slash && match(node->type_templ,ct)) ||
		(ce && !slash && HTAA_templateMatch(node->type_templ,ce))) {
		return node;
	    }
	}
    } else if ((mode & S_IFMT) == S_IFDIR) {
	return icon_dir ? icon_dir : icon_unknown;
    } else if ((mode & S_IFMT) == S_IFLNK) {
	return icon_dir ? icon_dir : icon_unknown;	/* @@ */
    }

    return icon_unknown;
}


/*
 * Find the URL for a given type. Called from HTDirBrw.c
 */
PUBLIC HTHrefNode * HTGetHref ARGS1( char *,	filename)
{
    HTHrefNode * node;
    char *c;

    HTList * cur = hrefs;

    c = strrchr(filename, '.');
    if (c) {
	while ((node = (HTHrefNode*)HTList_nextObject(cur))) {
	    if ((!strcmp(node->type_templ,c)) ) {
		return node;
	    }
	}
    }
    return NULL;
}

/* END OF MODULE */


