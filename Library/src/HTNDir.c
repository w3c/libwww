/*							       	     HTNDir.c
**	GENERIC NEWS LISTINGS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	Creates listings for all kind of News output.
**
** Authors:
**	HF	Henrik Frystyk, MIT, <frystyk@w3.org>
**	MP	Maciej Puzio <puzio@zodiac1.mimuw.edu.pl>
** History:
**	   Oct 95  HFN	written
**	   Mar 96  MP	modified heavily     
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "WWWHTML.h"
#include "HTNews.h"
#include "HTNewsLs.h"
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
    const HTStructuredClass *	isa;
    /* ... */
};

struct _HTNewsNode {
    int		index;
    char *	name;
    char *	subject;
    char *	from;
    time_t	date;
    int		refs;			 /* Number of references */
    BOOL	is_tmplate;

    /* Added by MP: */
    HTList* refNames;        /*  referee names list */
    HTList* refObjects;      /*  referee objects list */
    HTNewsNode * refParent;  /* direct parent (referee) */
    HTNewsNode * lastChild;  /* last child (referer) */
    BOOL    show;            /* show this node on the list? */
    BOOL    fake;            /* create anchor with this node? */
    int     refChildren;     /* number of referers */
    int     refLevel;        /* reference level - to speed sorting */
    int     minRefIndex;     /* minimal index in reference subtree */
    int     maxRefIndex;     /* maximal index in reference subtree */
    time_t  minRefDate;      /* minimal date in reference subtree */
    time_t  maxRefDate;      /* maximal date in reference subtree */
};

struct _HTNewsDir {
    HTStructured *	target;
    HTRequest *		request;
    HTNewsDirKey	key;				  /* Key for sorting */
    char *   	        name;        /* Name of the newsgroup(s) Added by MP */
    char *		tmplate;
    HTNewsNode *	tmplate_node;
    int 	        lastLevel;    /* Last printed ref level Added by MP. */
    HTArray *		array;			/* Array for sorted listings */
    HTArray *		cache;			  /* Only created on request */
};

PRIVATE int MaxLineW = DEFAULT_MAXW;

/*  Forward references - added by MP. */
PRIVATE void HTNewsDir_addLevelTags (HTNewsDir* dir, int level);
PRIVATE HTNewsNode* HTNewsDir_addFakeElement (HTNewsDir* dir, char* subject,
    char* name);
PRIVATE HTNewsNode* HTNewsDir_findNodeNamed (HTNewsDir* dir, char* name);

#define FNWS_ANY       0x0000
/* Date */
#define FNWS_MIN       0x0001
#define FNWS_MAX       0x0002
/* Fake/not-fake */
#define FNWS_ONLYFAKE  0x0010
#define FNWS_NOTFAKE   0x0020
/* Fake or not-fake with references */
#define FNWS_NOTORPHAN 0x0040
PRIVATE HTNewsNode* HTNewsDir_findNodeWithSubject (HTNewsDir* dir, 
    char* subject, int which, HTNewsNode* avoidNode);


/* ------------------------------------------------------------------------- */
/*				NODE  MANAGEMENT 			     */
/* ------------------------------------------------------------------------- */

PRIVATE HTNewsNode * HTNewsNode_new (int index, char * subject, char * from,
				     time_t date, char * name,
				     int refs, HTList * refNames)
{
    HTNewsNode * node;
    if ((node = (HTNewsNode *) HT_CALLOC(1, sizeof(HTNewsNode))) == NULL)
        HT_OUTOFMEM("HTNewsNode_new");
    if (name) StrAllocCopy(node->name, name);
    if (subject) {
	StrAllocCopy(node->subject, subject);
	node->subject = HTStrip(node->subject);
    }
    if (from) StrAllocCopy(node->from, from);
    node->index = index;
    node->date = date;
    node->refs = refs;
    node->refNames = refNames;
    node->show = (name != NULL);
    node->fake = (name == NULL);
    node->minRefIndex = index;
    node->maxRefIndex = index;
    node->minRefDate = date;
    node->maxRefDate = date;
    return node;
}

PRIVATE BOOL HTNewsNode_delete (HTNewsNode * node, BOOL cache)
{
    if (node) {
    	if (!cache || node->is_tmplate) HT_FREE(node->name);
	HT_FREE(node->subject);
	HT_FREE(node->from);
        if (node->refNames) {
	    HTList * cur = node->refNames;
	    char * pres;	    
            while ((pres = (char *) HTList_nextObject(cur))) HT_FREE(pres);
            HTList_delete(node->refNames);
        }
        if (node->refObjects) HTList_delete(node->refObjects);
	HT_FREE(node);
	return YES;
    }
    return NO;
}

/* Added by MP. */
PRIVATE BOOL HTNewsNode_isAncestor (HTNewsNode* node, HTNewsNode* refered)
{
    HTNewsNode* p;
    for (p = refered; p; p = p->refParent)
        if (p == node) return YES;
    return NO;
}

/* Added by MP. */
PRIVATE BOOL HTNewsNode_linkRef (HTNewsNode* node, HTNewsNode* referer)
{
    if (node && referer) {
	node->refChildren++;
	node->lastChild = referer;
	node->minRefIndex = (node->minRefIndex != 0 ? 
			     HTMIN(node->minRefIndex, referer->index) :
			     referer->index);
	node->maxRefIndex = (node->maxRefIndex != 0 ? 
			     HTMAX(node->maxRefIndex, referer->index) :
			     referer->index);
	node->minRefDate = (node->minRefDate != 0 ? 
			    HTMIN(node->minRefDate, referer->date) :
			    referer->date);
	node->maxRefDate = (node->maxRefDate != 0 ? 
			    HTMAX(node->maxRefDate, referer->date) :
			    referer->date);
	referer->refParent = node;
	return YES;
    }
    return NO;
}

/* Added by MP. */
PRIVATE int HTNewsNode_refLevel (HTNewsNode* node)
{
    int cnt = 0;
    HTNewsNode* p;
    for (p = node->refParent; p && p->show; p = p->refParent)
        cnt++;
    return cnt;
}

/* Added by MP. */
/* Returns index field for non-fake nodes, minimal or maximal subtreee */
/* index for fake nodes. */
PRIVATE time_t HTNewsNode_getIndex (HTNewsNode* node, BOOL minimal)
{
    if (node->index != 0)
        return node->index;
    else if (minimal)
        return node->minRefIndex;
    else
        return node->maxRefIndex;
}

/* Added by MP. */
/* Returns date field for non-fake nodes, minimal or maximal subtreee */
/* date for fake nodes. */
PRIVATE time_t HTNewsNode_getDate (HTNewsNode* node, BOOL minimal)
{
    if (node->date != 0)
        return node->date;
    else if (minimal)
        return node->minRefDate;
    else
        return node->maxRefDate;
}

/* Helper function - added by MP. */
PRIVATE char* UnReSubject (char* subject)
{
    if (strlen(subject) >= 3 && strncasecomp(subject, "re:", 3) == 0)
    {
        char* p = subject + 3;  /* "Re:XXX" */
        if (*p == ' ')  p ++;   /* "Re: XXX" */
        return p;
    }
    return subject;
}

/* Added by MP. */
PRIVATE void HTNewsNode_setRefInfo_pass1 (HTNewsDir* dir, HTNewsNode* node)
{
    HTList* ptr = node->refNames;
    char* name = NULL;
    if (node->fake)
        return;
    if (ptr != NULL)
        name = (char*) HTList_nextObject(ptr);
    while (ptr != NULL)
    {
        HTNewsNode* parent;
        parent = HTNewsDir_findNodeNamed(dir, name);
        if (parent)
        {
            if (!node->refObjects)  node->refObjects = HTList_new();
            HTList_addObject (node->refObjects, parent);
        }
        name = (char*) HTList_nextObject(ptr);
    }
}

/* Added by MP. */
PRIVATE void HTNewsNode_setRefInfo_pass2 (HTNewsDir* dir, HTNewsNode* node)
{
    HTNewsNode* maxParent = NULL;
    HTList* ptr = node->refObjects;
    HTNewsNode* parent = NULL;
    if (node->fake)
        return;
    if (ptr != NULL)
        parent = (HTNewsNode*) HTList_nextObject(ptr);
    while (ptr != NULL)
    {
        if (!maxParent || maxParent->date < parent->date)
            maxParent = parent;
        parent = (HTNewsNode*) HTList_nextObject(ptr);
    }
    if (maxParent)
    {
        if (!HTNewsNode_isAncestor(node, maxParent))  /* better be careful */
            HTNewsNode_linkRef (maxParent, node);
    }
    else
    {
        char* refSubject;
        BOOL re;

        /* Here is the only place we REALLY have to check for circular */
        /* references. It is normally possible that a node refers to */
        /* orphan node and both have the same subject. In this situation */
        /* we can't make the orphan to refer to it's child. Without checking */
        /* for circular references this is likely to happen here. */

        refSubject = UnReSubject(node->subject);
        re = (strcasecomp(refSubject, node->subject) != 0);
        if (re)
            parent = HTNewsDir_findNodeWithSubject(dir, refSubject, 
                FNWS_MIN | FNWS_NOTFAKE, node);
        if (!parent || HTNewsNode_isAncestor(node, parent))
            parent = HTNewsDir_findNodeWithSubject(dir, refSubject,
                FNWS_MIN | FNWS_ONLYFAKE, node);
        if (!parent && re)
        {
            parent = HTNewsDir_findNodeWithSubject(dir, node->subject, 
                FNWS_MIN | FNWS_ONLYFAKE, node);
        }
    
        if (!parent) parent = HTNewsDir_addFakeElement (dir, refSubject, NULL);
	if (parent) {
	    HTNewsNode_linkRef (parent, node);
	    if (parent->refChildren > 1) /* Multi-children fake node visible */
		parent->show = YES;
	}
    }
}

/*
** Added by MP.
*/
#if 0
PRIVATE void HTNewsNode_setRefInfo_pass3 (HTNewsDir* dir, HTNewsNode* node)
{
    HTNewsNode* parent = NULL;
    char* refSubject;
    BOOL re;

    if (node->fake || node->refParent)
        return;  /* This is only for nodes not handled in pass 2 */
    
    /* Here is the only place we REALLY have to check for circular */
    /* references. It is normally possible that a node refers to */
    /* orphan node and both have the same subject. In this situation */
    /* we can't make the orphan to refer to it's child. Without checking */
    /* for circular references this is likely to happen here. */

    refSubject = UnReSubject(node->subject);
    re = (strcasecomp(refSubject, node->subject) != 0);
    if (re)
        parent = HTNewsDir_findNodeWithSubject(dir, refSubject, 
            FNWS_MIN | FNWS_NOTFAKE, node);
    if (!parent || HTNewsNode_isAncestor(node, parent))
        parent = HTNewsDir_findNodeWithSubject(dir, refSubject,
            FNWS_MIN | FNWS_ONLYFAKE, node);
    if (!parent && re)
    {
        parent = HTNewsDir_findNodeWithSubject(dir, node->subject, 
            FNWS_MIN | FNWS_ONLYFAKE, node);
    }
    
    if (!parent) parent = HTNewsDir_addFakeElement (dir, refSubject, NULL);
    if (parent) {
	HTNewsNode_linkRef (parent, node);
	if (parent->refChildren > 1)  	 /* multi-children fake node visible */
	    parent->show = YES;
    }
}
#else
PRIVATE void HTNewsNode_setRefInfo_pass3 (HTNewsDir* dir, HTNewsNode* node)
{
    if (!node->fake)
        node->refLevel = HTNewsNode_refLevel(node);
}
#endif

/* Added by MP. */
PRIVATE int HTNewsNode_compareRefThread (HTNewsNode* node1, HTNewsNode* node2)
{
    int level1 = node1->refLevel;
    int level2 = node2->refLevel;
    int level = HTMAX(level1, level2);
    int i;
    HTNewsNode* parent1 = NULL;
    HTNewsNode* parent2 = NULL;
    int diff = 0;
    for (i = level; i >= 0; i--)
    {
        parent1 = (i < level1 ? parent1->refParent : node1);
        parent2 = (i < level2 ? parent2->refParent : node2);
        if (parent1 == parent2)
            return diff;  /* related messages (in same subtree) */
        else
        {
            time_t date1 = (i > level1 ? 0 : HTNewsNode_getDate(parent1, YES));
            time_t date2 = (i > level2 ? 0 : HTNewsNode_getDate(parent2, YES));
            diff = date1 - date2;
            if (diff == 0)
            {
                int idx1 = (i > level1 ? 0 : HTNewsNode_getIndex(parent1, YES));
                int idx2 = (i > level2 ? 0 : HTNewsNode_getIndex(parent2, YES));
                diff = idx1 - idx2;
            }
        }
    }
    return diff;  /* completely unrelated messages */
}

/*
**	Output an element in HTML
**	Returns YES if OK, else NO
*/
PRIVATE BOOL HTNewsNode_print (HTNewsDir * dir, HTNewsNode * node)
{
    if (node && node->show) {
	HTStructured *target = dir->target;
	char  * escaped = NULL;
    
	HTNewsDir_addLevelTags (dir, node->refLevel);  /* Added by MP. */
	START(HTML_LI);

	/* Start the anchor and put the subject as anchor text */
	/*  Changed by MP to allow nodes without names */
	if (!node->fake && node->name && node->subject)  {
	    escaped = HTEscape(node->name, URL_XPALPHAS);
	    HTStartAnchor(target, NULL, escaped);
	}
	if (node->subject) PUTS(node->subject);
	if (!node->fake && node->name && node->subject) {
	    END(HTML_A);
	    HT_FREE(escaped);
	}

	/* From field */
	if (node->from)  {
	    PUTS (" by ");  /* Changed by MP. */
	    PUTS(node->from);
	}

	/*  In group listing, put number of groups in the set; added by MP. */
	if (node->name && strrchr(node->name, '*')) {
	    char buf[16];
	    sprintf (buf, " (%d groups)", node->refChildren);
	    PUTS (buf);
	}
    }
    return YES;
}

/* ------------------------------------------------------------------------- */
/*				DIRECTORY MANAGEMENT 			     */
/* ------------------------------------------------------------------------- */

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
PUBLIC HTNewsDir * HTNewsDir_new (HTRequest * request, const char * title,
				  HTNewsDirKey key, BOOL cache)
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
    dir->key = key;
    dir->lastLevel = -1;  /* Added by MP. */

    /*  Get the newsgroup(s) name; added by MP. */
    {
        char* url = HTAnchor_physical(HTRequest_anchor(request));
        char* p = url+strlen(url);
        while (p > url && p[-1] != ':' && p[-1] != '/' && p[-1] != '\\')
            p--;
        StrAllocCopy (dir->name, p);
    }

    if (key != HT_NDK_NONE) {			       /* Thread is unsorted */
	int total = HTNews_maxArticles();
	dir->array = HTArray_new(total > 0 ? total : 128);
    }

    /* If we are asked to prepare a cache entry then create the cache array */
    if (cache) {
	int total = HTNews_maxArticles();
	dir->cache = HTArray_new(total > 0 ? total : 128);
    }

    /* Start the HTML stuff */
    {
	HTStructured *target = dir->target;
	const char *msg = title ? title : "News Listing";
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
**	Returns new node pointer if OK, else NULL
**	Changed by MP: reference list added.
**	Note: Unlike other parameters, refNames is not copied, but assigned, so
**	it has to contain copies of message names, not the originals.
*/
PUBLIC HTNewsNode* HTNewsDir_addElement (HTNewsDir * dir, int index, 
					 char * subject, char * from,
					 time_t date, char * name,
					 int refs, HTList * refNames)
{
    if (dir && name) {
	HTNewsNode * node = HTNewsNode_new(index, subject, from,
					   date, name, refs, refNames);
	if (dir->key == HT_NDK_NONE) {
	    HTNewsNode_print(dir, node);
	    HTNewsNode_delete(node, (dir->cache!=NULL));
	} else
	    HTArray_addObject(dir->array, (void *) node);
	return node;
    }
    return NULL;
}

/* Helper function - added by MP. */
PRIVATE HTNewsNode* HTNewsDir_addFakeElement (HTNewsDir * dir, 
					      char * subject, char * name)
{
    HTNewsNode * node =
	HTNewsDir_addElement(dir, 0, subject, NULL, 0, name, 0, NULL);
    if (node) {
	node->show = NO;
	node->fake = YES;
    }
    return node;
}

/* Helper function - added by MP. */
PUBLIC HTNewsNode * HTNewsDir_addGroupElement (HTNewsDir * dir, char * group,
					       BOOL tmplate)
{
    HTNewsNode * node = NULL;
    if (dir && group) {
	if (HTNewsDir_belongsToSet(dir, group))
	    node=HTNewsDir_addElement (dir, 0, group, NULL, 0, group, 0, NULL);

	/* If we are building a cache object then add the entry */
	if (dir->cache && !tmplate) {
	    char * name = node ? node->name : NULL;
	    if (!name) StrAllocCopy(name, group);
	    HTArray_addObject(dir->cache, name);
	}
    }
    return node;
}

/* Added by MP. */
PUBLIC BOOL HTNewsDir_belongsToSet (HTNewsDir* dir, char* group)
{
    char* p;
    if (!dir->name || !*(dir->name))
        return YES;
    p = strrchr(dir->name, '*');
    if (!p)
        return strcasecomp(group, dir->name) == 0;
    else
    {
        int len = p - dir->name;
        return strncasecomp(group, dir->name, len) == 0;
    }
}


/* Added by MP. */
PRIVATE void HTNewsDir_addLevelTags (HTNewsDir* dir, int level)
{
    HTStructured *target = dir->target;
    int i = level;
    while (i > dir->lastLevel)
    {
        START(HTML_UL);
        i--;
    }
    while (i < dir->lastLevel)
    {
        END(HTML_UL);
        i++;
    }
    dir->lastLevel = level;
}

/* Added by MP. */
PRIVATE HTNewsNode* HTNewsDir_findNodeNamed (HTNewsDir* dir, char* name)
{
    int i;
    for (i = 0; i < HTArray_size(dir->array); i++)
    {
        HTNewsNode* node = (HTNewsNode*)(HTArray_data(dir->array)[i]);
        if (node->name && strcasecomp(node->name, name) == 0)
            return node;
    }
    return NULL;
}

/* Added by MP. */
PRIVATE HTNewsNode* HTNewsDir_findNodeWithSubject (HTNewsDir* dir, 
    char* subject, int which, HTNewsNode* avoidNode)
{
    int i;
    int whichDate = (which & FNWS_MIN ? -1 : (which & FNWS_MAX ? 1 : 0));
    HTNewsNode* foundNode = NULL;
    for (i = 0; i < HTArray_size(dir->array); i++)
    {
        HTNewsNode* node = (HTNewsNode*)(HTArray_data(dir->array)[i]);
        if (!(which & FNWS_ONLYFAKE && !node->fake) && 
            !(which & FNWS_NOTFAKE && node->fake) &&
            !(which & FNWS_NOTORPHAN && !node->fake && !node->refNames) &&
            node != avoidNode && node->subject && 
            strcasecomp(node->subject, subject) == 0)
        {
            if (which == FNWS_ANY)
                return node;
            else if (!foundNode || (node->date != 0 && 
                (node->date - foundNode->date) * (long)whichDate > 0))
                foundNode = node;
        }                  
    }
    return foundNode;
}

/* Added by MP. */
PRIVATE void HTNewsDir_setRefInfo (HTNewsDir* dir)
{
    /* Array grows when fake elements are added.  */
    /* We don't want to set reference info for fake elements. */
    int size = HTArray_size(dir->array);
    int i;
    for (i = 0; i < size; i++)
        HTNewsNode_setRefInfo_pass1 (dir, (HTNewsNode*)(HTArray_data(dir->array)[i]));
    for (i = 0; i < size; i++)
        HTNewsNode_setRefInfo_pass2 (dir, (HTNewsNode*)(HTArray_data(dir->array)[i]));
    for (i = 0; i < size; i++)
        HTNewsNode_setRefInfo_pass3 (dir, (HTNewsNode*)(HTArray_data(dir->array)[i]));
}

PRIVATE void make_template (HTNewsDir * dir, HTNewsNode * node)
{
    HT_FREE(dir->tmplate);
    if ((dir->tmplate = (char *) HT_MALLOC(strlen(node->name) + 3)) == NULL)
	HT_OUTOFMEM("HTNewsNode_setGroupInfo");
    {
	char * p1 = dir->name;
	char * p2 = dir->tmplate;
	strcpy(p2, node->name);
	while (*p1 && *p2 && *p1 == *p2)  p1++, p2++;
	while (*p2 && *p2 != '.') p2++;
	if (*p2) {
	    strcpy(p2, ".*");
	    dir->tmplate_node=HTNewsDir_addGroupElement(dir, dir->tmplate,YES);
	    dir->tmplate_node->is_tmplate = YES;
	} else {
	    HT_FREE(dir->tmplate);
	    dir->tmplate_node = node;
	}
	dir->tmplate_node->show = YES;
    }
}

/*
**	Runs through a sorted list of news groups and identifies the group
**	hierarchy. Template groups are added to the list, for example as
**	"alt.*"
*/
PRIVATE void HTNewsDir_setGroupInfo (HTNewsDir * dir)
{
    HTArray * array = dir->array;
    HTNewsNode * node;
    int cur_size = HTArray_size(array);
    int cnt;

    /*
    ** If we don't have a template to test against then create one
    ** A template can be something like "alt.*" for example
    */
    for (cnt=0; cnt<cur_size; cnt++) {
	node = (HTNewsNode *) HTArray_data(array)[cnt];

	/*
	** Make a template if we don't have any
	*/
	if (!dir->tmplate) make_template(dir, node);
    
	/*
	** Now, if we do have a template then test the node name against
	** it to see if we have this group already or it is a new group
	** at this level in the hierarchy
	*/
	if (dir->tmplate) {
	    if (HTStrCaseMatch(dir->tmplate, node->name) == NULL) {
		make_template(dir, node);
	    } else {
		HTNewsNode * tmp_node = dir->tmplate_node;
		
		/* Should we show this element in the list? */
		if (tmp_node->lastChild) {
		    tmp_node->lastChild->show = NO;
		    node->show = NO;
		}
	    }
	    HTNewsNode_linkRef(dir->tmplate_node, node);
	}
    }
}

PRIVATE int NDirIndexSort (const void *a, const void *b)
{
    int aa = (*((HTNewsNode **)a))->index;
    int bb = (*((HTNewsNode **)b))->index;
    return aa-bb;
}

PRIVATE int NDirSubjectSort (const void *a, const void *b)
{
    char *aa = (*((HTNewsNode **)a))->subject;
    char *bb = (*((HTNewsNode **)b))->subject;
    return strcasecomp(aa?aa:"", bb?bb:"");
}

PRIVATE int NDirFromSort (const void *a, const void *b)
{
    char *aa = (*((HTNewsNode **)a))->from;
    char *bb = (*((HTNewsNode **)b))->from;
    return strcasecomp(aa?aa:"", bb?bb:"");
}

PRIVATE int NDirDateSort (const void *a, const void *b)
{
    time_t aa = (*((HTNewsNode **)a))->date;
    time_t bb = (*((HTNewsNode **)b))->date;
    return bb-aa;
}

PRIVATE int NDirGroupSort (const void *a, const void *b)
{
    char *aa = (*((HTNewsNode **)a))->name;
    char *bb = (*((HTNewsNode **)b))->name;
    while (*aa && *bb && TOLOWER(*aa)==TOLOWER(*bb)) aa++, bb++;
    return (*aa=='.' && *bb) ? -1 : (*aa && *bb=='.') ?
	1 : TOLOWER(*aa)-TOLOWER(*bb);
}

/* Added by MP. */
PRIVATE int NDirRefThreadSort (const void* a, const void* b)
{
    HTNewsNode* aa = *((HTNewsNode**)a);
    HTNewsNode* bb = *((HTNewsNode**)b);
    return HTNewsNode_compareRefThread(aa,bb);
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
    	HTArray * array = dir->array;
	HTArray * cache = NULL;
    	HTComparer * comp = NULL;

	/*
	** Find a suitable sort key for this listing. The sort function
	** depends on the type of new listing we have received.
	*/
    	if (dir->key == HT_NDK_INDEX)	           /* Sort by Message Number */
    	    comp = NDirIndexSort;
    	else if (dir->key == HT_NDK_DATE)	             /* Sort by Date */
    	    comp = NDirDateSort;
    	else if (dir->key == HT_NDK_SUBJECT)           /* Sort after Subject */
    	    comp = NDirSubjectSort;         
    	else if (dir->key == HT_NDK_FROM)		  /* Sort after From */
    	    comp = NDirFromSort;
    	else if (dir->key == HT_NDK_GROUP) {	  /* Sort as group hierarchi */
	    comp = NDirGroupSort;
        } else if (dir->key == HT_NDK_REFTHREAD) {    /* Added by MP. */
            HTNewsDir_setRefInfo (dir);
            comp = NDirRefThreadSort;
        } else {
    	    HTTRACE(STREAM_TRACE, "NewsListing. Invalid sortkey\n");
    	    return NO;
    	}

	/*
	** Now sort the array of news items that we have read with the sort
	** function defined by the sort key above.
	*/
    	HTArray_sort(array, comp);

	/*
	** If we are showing a group listing then run through the list and
	** identify group hierarchy. We have to sort the thing again in order
	** to get the new template groups included
	*/
	if (dir->key == HT_NDK_GROUP) {
	    HTNewsDir_setGroupInfo(dir);
	    HTArray_sort(array, comp);
	}
	
	/*
	** After we have sorted the listing, we can write out the result and
	** free the array.
	*/
    	{
    	    void ** data = NULL;
    	    HTNewsNode *node = (HTNewsNode *) HTArray_firstObject(array, data);
    	    while (node) {
		HTNewsNode_print(dir, node);

		/*
		** Create a special array for the cache containing the group
		** names only and no templates
		*/
		if (dir->key == HT_NDK_GROUP && !node->is_tmplate)
		    HTArray_addObject(cache, node->name);

		HTNewsNode_delete(node, (dir->cache!=NULL));
		node = (HTNewsNode *) HTArray_nextObject(array, data);
    	    }
	    HTArray_delete(array);	
	}

	/* Update the cache */
	if (dir->cache) HTNewsCache_after(dir->request, NULL, dir->cache, 0);
    }

    /* Put out the end of the HTML stuff */
    {
	HTStructured *target = dir->target;
	/* END(HTML_UL); */
        HTNewsDir_addLevelTags (dir, -1);
	START(HTML_HR);
	END(HTML_BODY);
	END(HTML_HTML);
	FREE_TARGET;
    }

    /* Clean up the dir object */
    HT_FREE(dir->name);
    HT_FREE(dir->tmplate);
    HT_FREE(dir);
    return YES;
}
