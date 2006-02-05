/*
**	RDF PARSER
**
**	@(#) $Id$
**	
**	Copyright © 1995-1998 World Wide Web Consortium, (Massachusetts
**	Institute of Technology, Institut National de Recherche en
**	Informatique et en Automatique, Keio University). All Rights
**	Reserved. This program is distributed under the W3C's Software
**	Intellectual Property License. This program is distributed in the hope
**	that it will be useful, but WITHOUT ANY WARRANTY; without even the
**	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
**	PURPOSE. See W3C License http://www.w3.org/Consortium/Legal/ for more
**	details.
**
**	This module requires expat 
**
**	AUTHOR:
**		John Punin <puninj@cs.rpi.edu>,
**		Henrik
*/

#include "WWWLib.h"
#include "WWWInit.h"
#include "WWWXML.h"
#include "HTRDF.h"

struct _HTStream {
    const HTStreamClass *	isa;
    int 	 		state;
    HTRequest *			request;
    HTStream *                  target;
    HTRDF *                     rdfparser;
};

#define PUTC(t,c)	(*(t)->target->isa->put_character)((t)->target, (c))
#define PUTS(t,s)	(*(t)->target->isa->put_string)((t)->target, (s))

struct _HTTriple {
    char * 			m_sPredicate;
    char * 			m_sSubject;
    char * 			m_sObject;
};

struct _HTElement {
    char * 			m_sName;
    HTAssocList *		m_attributes;
    HTList *			m_children;
    HTList *			m_lastChild;
    char * 			m_sID;
    char * 			m_sBagID;
    HTList *			m_vTargets;
    BOOL   			m_bDone;
    char * 			m_sPrefix;
    char * 			m_sContent;
};

struct _HTRDFParser {
    HTList *			m_namespaceStack;
    HTList *			m_elementStack;
    HTList *			m_literalStack;
    HTElement *			m_root;
    HTList *			m_triples;
    char * 			m_sSource;
    HTList *			m_vAllNameSpaces;

    BOOL			m_bCreateBags;
    BOOL 			m_bFetchSchemas;

    HTList *			m_parseTypeStack;
    HTList *			m_parseElementStack;
    char * 			m_sLiteral;

    HTList *			m_vResources;
    HTList * 			m_vResolveQueue;
    HTHashtable * 		m_hIDtable;
    int 			m_iReificationCounter;

    HTStream *			ostream;

    HTTripleCallback_new * 	newTripleInstance;
    void *			tripleContext;
};

/* @@@ Should not be global but controlled by name spaces @@@ */
PRIVATE HTRDFCallback_new *	RDFInstance = NULL;
PRIVATE void *			RDFInstanceContext = NULL;

PRIVATE char * HTRDF_processContainer (HTRDF *me, HTElement *e);
PRIVATE char * HTRDF_processPredicate (HTRDF *me, HTElement *predicate,
				    HTElement *description,
				    char * sTarget,
				    BOOL reificate);
PRIVATE void HTRDF_processListItem (HTRDF *me,char * sID, HTElement *listitem, 
				    int iCounter);
PRIVATE void HTRDF_checkAttributes (HTRDF *me,HTElement *Element);
PRIVATE BOOL HTRDF_expandAttributes (HTRDF *me, HTElement *parent, HTElement *ele);
PRIVATE char * HTRDF_reificate (HTRDF *me, char * sPredicate, char * sSubject,
			       char * sObject, char * sNodeID);
/* ------------------------------------------------------------------------- */

/*
** Append the markup for the given element and its attribute to the
** parser's "Literal" buffer.  This buffer is filled in when parseType="Literal".
*/
PRIVATE void addMarkupStart (HTRDF *rdfp, const char *name, const char **atts)
{
    int i=0;

    if (!rdfp || !name) return;

    StrAllocMCat(&rdfp->m_sLiteral, "<", name, NULL);

    while (atts[i]) {
        StrAllocMCat(&rdfp->m_sLiteral, " ", atts[i], "=\"", atts[i+1], "\"", NULL);
        i+=2;
    }

    StrAllocCat(rdfp->m_sLiteral, ">");
}

/*
** Terminate this element's "Literal" buffer.  This buffer is filled in when 
** parseType="Literal".
*/
PRIVATE void addMarkupEnd (HTRDF *rdfp, const char *name)
{
    if (!rdfp || !name) return;

    StrAllocMCat(&rdfp->m_sLiteral, "</", name, ">", NULL);
}

/* ------------------------------------------------------------------------- */

/*
**  Searches a whole list of Strings and returns true if the String is found.
*/
PRIVATE BOOL HTList_contains (HTList *list, char * s)
{
    HTList *cur = list;
    char * cs = NULL;
    while ((cs = (char *) HTList_nextObject(cur))) {
	if (!strcmp(cs, s)) return YES;	
    }
    return NO;
}

/*
** Useful function that Trims a string
** @@@ Should use HTStrip() @@@
*/
PRIVATE char * trim (char *s)
{
    char *p = NULL, *t = NULL;
    int len = s ? strlen(s) : -1;	
    if (s && len > 0) {
	StrAllocCopy(t, s);
	p = &(s[len-1]);
	while(p!=s) {
	    if (!isspace((int)(*p)))
		break;
	    p--;
	}
	t[(int)(p-s)+1] = '\0';
	if (isspace((int) t[(int)(p-s)]))
	    t[(int)(p-s)] = '\0';
    }
    return t;
}

/* ------------------------------------------------------------------------- */
/*				TRIPLE of RDF 				     */
/* ------------------------------------------------------------------------- */

PUBLIC HTTriple * HTTriple_new (char * p, char * s, char * o) 
{
    HTTriple * me = NULL;
    if (p && s && o) {
	if ((me = (HTTriple *) HT_CALLOC(1, sizeof(HTTriple))) == NULL)
	    HT_OUTOFMEM("HTTriple_new");
        StrAllocCopy(me->m_sPredicate, p);
	StrAllocCopy(me->m_sSubject, s);
	StrAllocCopy(me->m_sObject, o);
    }
    return me;
}

PUBLIC BOOL HTTriple_delete (HTTriple * me)
{
    if (me) {
	HT_FREE(me->m_sPredicate);
	HT_FREE(me->m_sSubject);
	HT_FREE(me->m_sObject);
	HT_FREE(me);
	return YES;
    }
    return NO;
}

PUBLIC void HTTriple_print (HTTriple * me)
{
    if (me)
	HTPrint("TRIPLE(%s,%s,%s)\n", me->m_sPredicate, me->m_sSubject,
		me->m_sObject);	
}

PUBLIC char * HTTriple_subject (HTTriple * me)
{
    return me ? me->m_sSubject : NULL;
}

PUBLIC char *  HTTriple_predicate (HTTriple * me)
{
    return me ? me->m_sPredicate : NULL;
}

PUBLIC char * HTTriple_object (HTTriple * me)
{
    return me ? me->m_sObject : NULL;
}

/* ------------------------------------------------------------------------- */
/*				ELEMENT of RDF 				     */
/* ------------------------------------------------------------------------- */

PUBLIC HTElement * HTElement_new (char * sName, HTAssocList * al)
{
    HTElement * me = NULL;
    if (sName) {
	if ((me = (HTElement *) HT_CALLOC(1, sizeof(HTElement))) == NULL)
	    HT_OUTOFMEM("HTElement_new");
	StrAllocCopy(me->m_sName, sName);
	me->m_attributes = al ? al : HTAssocList_new();
	me->m_children = HTList_new();
	me->m_lastChild = me->m_children;
	/*me->m_nodes = HTAssocList_new();*/ /* Sirpac does not use nodes list */
	me->m_vTargets = HTList_new();
	me->m_bDone = FALSE;
    }
    return me;
}

/*
** Creates a Data Element and saves the data in the Content field. 
** Data Element does not have attributes
*/
PUBLIC HTElement * HTElement_new2 (char * sContent)
{
    HTElement * me = NULL;
    if (sContent) {
	if ((me = (HTElement *) HT_CALLOC(1, sizeof(HTElement))) == NULL)
	    HT_OUTOFMEM("HTElement_new2");
	StrAllocMCopy(&me->m_sName, "[DATA: ", sContent, "]", NULL);
	me->m_attributes = NULL;
	me->m_children = HTList_new();
	me->m_lastChild = me->m_children;
	/*me->m_nodes = HTAssocList_new();*/
	me->m_vTargets = HTList_new();
	me->m_bDone = FALSE;
 	StrAllocCopy(me->m_sContent, sContent);
    }
    return me;
}

PUBLIC BOOL HTElement_addData (HTElement *me, char * sContent)
{
    if (me && sContent) {
	int l = strlen(me->m_sName);
	StrAllocCat(me->m_sContent, sContent);
	me->m_sName[l-1]='\0';
	StrAllocMCat(&me->m_sName, sContent, "]", NULL);
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTElement_delete (HTElement * me)
{
    if (me) {
	HT_FREE(me->m_sName);
	if (me->m_attributes) HTAssocList_delete(me->m_attributes);
	if (me->m_children) HTList_delete(me->m_children);
	HT_FREE(me->m_sID);
	HT_FREE(me->m_sBagID);
	if (me->m_vTargets) HTList_delete(me->m_vTargets);
	HT_FREE(me->m_sPrefix);
	HT_FREE(me->m_sContent);
	HT_FREE(me);
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTElement_addChild (HTElement * me, HTElement * element)
{
  if (me && element) {
    HTList *lastChild;
    if ((lastChild = HTList_addList(me->m_lastChild, element))) {
      me->m_lastChild = lastChild;
      return YES;
    }
  }
  return NO;
}

PUBLIC BOOL HTElement_addAttribute (HTElement * me, char * sName, char * sValue)
{
    return (me && sName && sValue) ?
	HTAssocList_addObject(me->m_attributes, sName, sValue) : NO;
}

PUBLIC BOOL HTElement_removeAttribute (HTElement * me, char * sName)
{
    return (me && sName) ? HTAssocList_removeObject(me->m_attributes, sName) : NO;
}

PUBLIC char * HTElement_getAttribute (HTElement * me, char * sName)
{
    return (me && sName) ? HTAssocList_findObjectCaseSensitiveExact(me->m_attributes, sName) : NULL;
}

PUBLIC char * HTElement_getAttribute2 (HTElement * me, char * sNamespace, char * sName)
{
    char * fValue = NULL;
    char * fName = NULL;
    if (me && sNamespace && sName) {
	StrAllocMCopy(&fName, sNamespace, sName, NULL);	
	fValue = HTAssocList_findObjectCaseSensitiveExact(me->m_attributes, fName);
	HT_FREE(fName);
    }
    return fValue;
}

PUBLIC BOOL HTElement_addTarget (HTElement * me, HTElement * element)
{
    return (me && element) ? HTList_addObject(me->m_vTargets, element) : NO;
}

PUBLIC HTElement * HTElement_target (HTElement * me)
{
    return me ? (HTElement *) HTList_lastObject(me->m_vTargets) : NULL;
}

PUBLIC BOOL HTElement_instanceOfData (HTElement * me)
{
    return (me && me->m_sContent) ? YES : NO;
}

/* ------------------------------------------------------------------------- */
/*				EXPAT HANDLERS				     */
/* ------------------------------------------------------------------------- */

/*
 * Called for each new element.
 * Build up the document tree using an element stack
 */
PRIVATE void XML_startElement (void * userData,
	   	    	       const XML_Char *	name, const XML_Char ** atts)
{
    HTRDF * rdfp = (HTRDF *) userData;
    HTAssocList * namespaces = HTAssocList_new();
    HTAssocList * newAL = HTAssocList_new();
    int i = 0;

    /**
     * The following loop tries to identify special xmlns prefix
     * attributes and update the namespace stack accordingly.
     * While doing all this, it builds another AttributeList instance
     * which will hold the expanded names of the attributes
     * (I think this approach is only useful for RDF which uses
     * attributes as an abbreviated syntax for element names)
     */
    if (atts) {
	while (atts[i]) {
	    char * aName = (char * ) atts[i];
	    if (!strcmp(aName, "xmlns")) {
		char * aValue = (char *) atts[i+1];
		int len = aValue ? strlen(aValue) : -1;
		if (len == 0 && !rdfp->m_sSource)
		    aValue = rdfp->m_sSource;
		HTAssocList_addObject(namespaces, aName, aValue);
		/* save all non-RDF schema addresses */
		if (!HTList_contains(rdfp->m_vAllNameSpaces, aValue) &&
		   strncmp(aValue, RDFMS, strlen(RDFMS)) &&
		   strncmp(aValue, RDFSCHEMA, strlen(RDFSCHEMA))) {
		    char * nname = NULL;
		    StrAllocCopy(nname, aValue);
		    HTList_addObject(rdfp->m_vAllNameSpaces, nname);
		}

		/* Special case: Don't save document's own address  */
		if (rdfp->m_sSource && 
		   !strncmp(aValue, rdfp->m_sSource, strlen(rdfp->m_sSource))) {
		    char * nname = NULL;
		    StrAllocCopy(nname, aValue);
		    HTList_addObject(rdfp->m_vAllNameSpaces, nname);
		}
	    } else if (!strncmp(aName, "xmlns:", 6)) {
		char * aValue = (char *) atts[i+1];
		char * nName = NULL;
		int len = aValue ? strlen(aValue) : -1;
		if (len == 0 && !rdfp->m_sSource)
		    aValue = rdfp->m_sSource;
		StrAllocCopy(nName, &(aName[6]));
		HTAssocList_addObject(namespaces, nName, aValue);
		HT_FREE(nName);

		/* Save all non-RDF schema addresses */
		if (!HTList_contains(rdfp->m_vAllNameSpaces, aValue) &&
		   strncmp(aValue, RDFMS, strlen(RDFMS)) &&
		   strncmp(aValue, RDFSCHEMA, strlen(RDFSCHEMA))) {
		    char * nname = NULL;
		    StrAllocCopy(nname, aValue);
		    HTList_addObject(rdfp->m_vAllNameSpaces, nname);
		}

		/* Special case: Don't save document's own address  */
		if (rdfp->m_sSource && 
		   !strncmp(aValue, rdfp->m_sSource, strlen(rdfp->m_sSource))) {
		    char * nname = NULL;
		    StrAllocCopy(nname, aValue);
		    HTList_addObject(rdfp->m_vAllNameSpaces, nname);
		}
	    }
	    i+=2;
	} /* end of while */
    } /* end of if */

    /*
    ** Place new namespace declarations into the stack
    ** (Yes, I could optimize this a bit, not it wastes space
    ** if there are no xmlns definitions)
    */
    HTList_addObject(rdfp->m_namespaceStack, namespaces);

    /*
    ** Figure out the prefix part if it exists and
    ** determine the namespace of the element accordingly
    */
    {
	char * sNamespace = NULL;
	char * sElementName = NULL;
	char * sPrefix2 = NULL;
	HTElement *newElement = NULL;
	char *pindex = strchr(name, ':');
	int ix = pindex ? (int) (pindex - name) : -1 ;
	if (ix > 0) {
	    if (!(sPrefix2 = HT_MALLOC(ix+1)))
		HT_OUTOFMEM("XML_startELement");
	    strncpy(sPrefix2, name, ix);
	    sPrefix2[ix]='\0';
	    sNamespace = HTRDF_namespace(rdfp, sPrefix2);
	    StrAllocCopy(sElementName, &(name[ix+1]));
	    HT_FREE(sPrefix2);
	} else {
	    sNamespace = HTRDF_namespace(rdfp, "xmlns");
	    StrAllocCopy(sElementName, name);
	}

	/*
	 * Finally look for attributes other than the special xmlns, 
	 * expand them, and place to the new Attribute List
	 */
	i = 0;
	if (atts) {
	    while (atts[i]) {
		char * aName = (char *) atts[i];
		char * sAttributeNamespace = NULL;
		if (strncmp(aName, "xmlns", 5)) {
		    char * aValue = (char *) atts[i+1];
		    char * sPrefix = NULL;
		    /* Expat does not have type for attributes */	
		    pindex = strchr(aName, ':');
		    ix = pindex ? (int) (pindex - aName) : -1;
		    if (ix > 0) {
			if (!(sPrefix = HT_MALLOC(ix+1)))
			    HT_OUTOFMEM("XML_startELement");
			strncpy(sPrefix, aName, ix);
			sPrefix[ix] = '\0';
			sAttributeNamespace = HTRDF_namespace(rdfp, sPrefix);
			aName = &(aName[ix+1]);
			HT_FREE(sPrefix);
		    } else {
			if (!sNamespace)
			   sAttributeNamespace = HTRDF_namespace(rdfp, "xmlns");
			else
			    StrAllocCopy(sAttributeNamespace, sNamespace);
		    }

		    if (HTRDF_parseLiteral(rdfp)) {
			if (!sPrefix) {
			    if (!(sPrefix = HT_MALLOC(8)))
				HT_OUTOFMEM("XML_startELement");
			    sprintf(sPrefix, "gen%d\n", i);
			}
			{
			    char * fName = NULL;	
			    StrAllocMCopy(&fName, sPrefix, ":", aValue, NULL);
			    HTAssocList_addObject(newAL, fName, aValue);
			    HT_FREE(fName);
			    StrAllocMCopy(&fName, "xmlns:", sPrefix, NULL);
			    HTAssocList_addObject(newAL, fName, sAttributeNamespace);
			    HT_FREE(fName);
			}
		    } else {
			char * fName = NULL;	
			StrAllocMCopy(&fName, sAttributeNamespace, aName, NULL);
			HTAssocList_addObject(newAL, fName, aValue);
			HT_FREE(fName);
		    }

		    HT_FREE(sAttributeNamespace);

		    /*
		    ** This call will try to see if the user is using
		    ** RDF look-alike elements from another namespace
		    **
		    ** Note: you can remove the call if you wish
		    */
#if 0
		    HTRDF_likeRDF (rdfp, sAttributeNamespace, aName);
#endif
		    
		} /* end of if */
		i+=2;
	    } /* end of while */
	} /* end of if atts */

	/*
	 * If we have parseType="Literal" set earlier, this element
	 * needs some additional attributes to make it stand-alone
	 * piece of XML
	 */
	if (HTRDF_parseLiteral(rdfp)) {
	    char * fName = NULL;

	    if (!sPrefix2) {
		if (sNamespace)
		    HTAssocList_addObject(newAL, "xmlns:gen", sNamespace);
		StrAllocMCopy(&fName, "gen", sElementName, NULL);
		newElement = HTElement_new(fName, newAL);
		StrAllocCopy(newElement->m_sPrefix, "gen");
		HT_FREE(fName);
	    } else {
		char * sAttributeNamespace = HTRDF_namespace(rdfp, sPrefix2);
		if (sAttributeNamespace) {
		    StrAllocMCopy(&fName, "xmlns:", sPrefix2, NULL);
		    HTAssocList_addObject(newAL, fName, sAttributeNamespace);
		    HT_FREE(fName);
		}
		StrAllocMCopy(&fName, sPrefix2, ":", sElementName, NULL);
		newElement = HTElement_new(fName, newAL);
		HT_FREE(fName);
	    }
	} else {
	    char * fName = NULL;
	    StrAllocMCopy(&fName, sNamespace, sElementName, NULL);
	    newElement = HTElement_new(fName, newAL);
	    HT_FREE(fName);
	    /* 	HTRDF_likeRDF (rdfp, sNamespace, sElementName);    */
	}
	HT_FREE(sElementName);
	HT_FREE(sNamespace);
	HTRDF_checkAttributes(rdfp, newElement);

	/*
	** Check parseType
	*/
	{
	    char * fName = NULL;
	    char * sLiteralValue = NULL;
	    StrAllocMCopy(&fName, RDFMS, "parseType", NULL);
	    sLiteralValue = HTElement_getAttribute(newElement, fName);
	    HT_FREE(fName);
	    if (sLiteralValue && strcmp(sLiteralValue, "Resource")) {
		/**
		 * This is the management of the element where
		 * parseType="Literal" appears
		 *
		 * You should notice RDF V1.0 conforming implementations
		 * must treat other values than Literal and Resource as
		 * Literal. This is why the condition is !equals("Resource")
		 */
		
		HTList_addObject(rdfp->m_parseTypeStack, sLiteralValue);
		if (!HTList_isEmpty(rdfp->m_elementStack)) {
		    HTElement *e = (HTElement *)
			HTList_lastObject(rdfp->m_elementStack);
		    HTElement_addChild(e, newElement);
		}
		HTList_addObject(rdfp->m_elementStack, newElement);
		HTList_addObject(rdfp->m_parseElementStack, newElement);
		HT_FREE(rdfp->m_sLiteral);	
		StrAllocCopy(rdfp->m_sLiteral, "");
		return;
	    }

	    if (HTRDF_parseLiteral(rdfp)) {	
		/*
		 * This is the management of any element nested within
		 * a parseType="Literal" declaration
		 */
                /* Add the element to the parser's literal buffer */
                addMarkupStart (rdfp, name, atts);

                /* Add this literal element to the literal stack */
		if (!HTList_isEmpty(rdfp->m_literalStack)) {
		    HTElement *e = (HTElement *)
			HTList_lastObject(rdfp->m_literalStack);
		    HTElement_addChild(e, newElement);
		}
		HTList_addObject(rdfp->m_literalStack, newElement);

		HTList_addObject(rdfp->m_elementStack, newElement);
		return;
	    }

	    /*
	    ** Update the containment hierarchy with the stack.
	    */
	    if (!HTList_isEmpty(rdfp->m_elementStack)) {
		HTElement *e = (HTElement *)
		    HTList_lastObject(rdfp->m_elementStack);
		HTElement_addChild(e, newElement);
	    }	

	    /*
	    ** Place the new element into the stack
	    */
	    HTList_addObject(rdfp->m_elementStack, newElement);
	    if (sLiteralValue && !strcmp(sLiteralValue, "Resource")) {
		HTList_addObject(rdfp->m_parseTypeStack, sLiteralValue);
		HTList_addObject(rdfp->m_parseElementStack, newElement);
		HT_FREE(rdfp->m_sLiteral);	
		StrAllocCopy(rdfp->m_sLiteral, "");

		/*
		 * Since parseType="Resource" implies the following
		 * production must match Description, let's create
		 * an additional Description node here in the document tree.
		 */
		{
		    char * fName = NULL;
		    HTElement *desc = NULL;
		    HTAssocList * al = HTAssocList_new ();
		    StrAllocMCopy(&fName, RDFMS, "Description", NULL);
		    desc = HTElement_new(fName, al);
		    HT_FREE(fName);
		    if (!HTList_isEmpty(rdfp->m_elementStack)) {
			HTElement *e = (HTElement *)
			    HTList_lastObject(rdfp->m_elementStack);
			HTElement_addChild(e, desc);
		    }
		    HTList_addObject(rdfp->m_elementStack, desc);
		}
	    } /* end of if */
	} /* end of block */
    } /* end of block */
}

/*
 * For each end of an element scope step back in the 
 * element and namespace stack
 */
PRIVATE void XML_endElement (void * userData, 
			     const XML_Char * name)
{
    HTRDF * rdfp = (HTRDF *) userData;
    BOOL bParseLiteral = rdfp ? HTRDF_parseLiteral(rdfp) : NO;
    HTAssocList * namespaces = HTList_removeLastObject(rdfp->m_namespaceStack);
    rdfp->m_root = (HTElement *) HTList_removeLastObject(rdfp->m_elementStack);
    if (namespaces) HTAssocList_delete(namespaces);

    if (bParseLiteral) {
	HTElement *pe = (HTElement *)
	    HTList_lastObject(rdfp->m_parseElementStack);
	if (pe != rdfp->m_root) {
            /* Terminate the literal */
            addMarkupEnd (rdfp, name);
	} else {
	    HTElement *de = HTElement_new2(rdfp->m_sLiteral);
	    HTElement_addChild(pe, de);

	    HT_FREE(rdfp->m_sLiteral);	
	    StrAllocCopy(rdfp->m_sLiteral, "");
	    HTList_removeLastObject(rdfp->m_parseElementStack);
	    HTList_removeLastObject(rdfp->m_parseTypeStack);
	}
    } else if (HTRDF_parseResource(rdfp)) {
	/**
	 * If we are doing parseType="Resource"
	 * we need to explore whether the next element in
	 * the stack is the closing element in which case
	 * we remove it as well (remember, there's an
	 * extra Description element to be removed)
	 */
	if (!HTList_isEmpty(rdfp->m_elementStack)) {
	    HTElement *pe = (HTElement *)
		HTList_lastObject(rdfp->m_parseElementStack);
	    HTElement *e = (HTElement *)
		HTList_lastObject(rdfp->m_elementStack);
	    if (pe == e) {
		e = (HTElement *) HTList_removeLastObject(rdfp->m_elementStack);
		HTList_removeLastObject(rdfp->m_parseElementStack);
		HTList_removeLastObject(rdfp->m_parseTypeStack);
	    }
	}
    }
}	

PRIVATE void XML_characterData (void * userData, 
				const XML_Char * s, int len)
{
    /*
     * Place all characters as Data instance to the containment
     * hierarchy with the help of the stack.
     */
    HTRDF * rdfp = (HTRDF *) userData;
    HTElement * e = (HTElement *) HTList_lastObject(rdfp->m_elementStack);
    char * tstr = NULL;
    char * str = NULL;
    if (!(str = (char *) HT_MALLOC(len+1)))
	HT_OUTOFMEM("XML_characterData");
    strncpy(str, s, len);
    str[len]='\0';
    if (HTRDF_parseLiteral(rdfp)) {
	StrAllocCat(rdfp->m_sLiteral, str);
	HT_FREE(str);
	return;
    }
    /* JUST FOR EXPAT */
    {
	HTElement *lch = (HTElement *) HTList_lastObject(e->m_children);
	if (lch && HTElement_instanceOfData(lch)) {
	    HTElement_addData(lch, str);
	    HT_FREE(str);
	    return;
	}
    }
    /*
     * Warning: this is not correct procedure according to XML spec.
     * All whitespace matters!
     */
    tstr = trim(str);
    if (strlen(tstr) > 0) {
	HTElement * de = HTElement_new2(tstr);
	HTElement_addChild(e, de);
    }
    HT_FREE(str); HT_FREE(tstr);
}

PRIVATE void XML_processingInstruction (void * userData,
					const XML_Char * target,
					const XML_Char * data)
{
    return;
}

/*
** This is called for any characters in the XML document for
** which there is no applicable handler.  This includes both
** characters that are part of markup which is of a kind that is
** not reported (comments, markup declarations), or characters
** that are part of a construct which could be reported but
** for which no handler has been supplied. The characters are passed
** exactly as they were in the XML document except that
** they will be encoded in UTF-8.  Line boundaries are not normalized.
** Note that a byte order mark character is not passed to the default handler.
** If a default handler is set, internal entity references
** are not expanded. There are no guarantees about
** how characters are divided between calls to the default handler:
** for example, a comment might be split between multiple calls.
*/
PRIVATE void XML_default (void * userData,
			  const XML_Char * s, int len)
{
    return;
}

/* 
** This is called for a declaration of an unparsed (NDATA)
** entity.  The base argument is whatever was set by XML_SetBase.
** The entityName, systemId and notationName arguments will never be null.
** The other arguments may be.
*/
PRIVATE void XML_unparsedEntityDecl (void * userData,
				     const XML_Char * entityName,
				     const XML_Char * base,
				     const XML_Char * systemId,
				     const XML_Char * publicId,
				     const XML_Char * notationName)
{
    return;
}

/* 
** This is called for a declaration of notation.
** The base argument is whatever was set by XML_SetBase.
** The notationName will never be null.  The other arguments can be.
*/
PRIVATE void XML_notationDecl (void * userData,
			       const XML_Char * notationName,
			       const XML_Char * base,
			       const XML_Char * systemId,
			       const XML_Char * publicId)
{
    return;
}

/* 
** This is called for a reference to an external parsed general entity.
** The referenced entity is not automatically parsed.
** The application can parse it immediately or later using
** XML_ExternalEntityParserCreate.
** The parser argument is the parser parsing the entity containing the reference;
** it can be passed as the parser argument to XML_ExternalEntityParserCreate.
** The systemId argument is the system identifier as specified in the entity 
** declaration; it will not be null.
** The base argument is the system identifier that should be used as the base for
** resolving systemId if systemId was relative; this is set by XML_SetBase;
** it may be null.
** The publicId argument is the public identifier as specified in the entity declaration,
** or null if none was specified; the whitespace in the public identifier
** will have been normalized as required by the XML spec.
** The openEntityNames argument is a space-separated list of the names of the entities
** that are open for the parse of this entity (including the name of the referenced
** entity); this can be passed as the openEntityNames argument to
** XML_ExternalEntityParserCreate; openEntityNames is valid only until the handler
** returns, so if the referenced entity is to be parsed later, it must be copied.
** The handler should return 0 if processing should not continue because of
** a fatal error in the handling of the external entity.
** In this case the calling parser will return an XML_ERROR_EXTERNAL_ENTITY_HANDLING
** error.
** Note that unlike other handlers the first argument is the parser, not userData.
*/
PRIVATE int XML_externalEntityRef (XML_Parser parser,
				   const XML_Char * openEntityNames,
				   const XML_Char * base,
				   const XML_Char * systemId,
				   const XML_Char * publicId)
{
    return 0;
}

/* 
** This is called for an encoding that is unknown to the parser.
** The encodingHandlerData argument is that which was passed as the
** second argument to XML_SetUnknownEncodingHandler.
** The name argument gives the name of the encoding as specified in
** the encoding declaration.
** If the callback can provide information about the encoding,
** it must fill in the XML_Encoding structure, and return 1.
** Otherwise it must return 0.
** If info does not describe a suitable encoding,
** then the parser will return an XML_UNKNOWN_ENCODING error.
*/
PRIVATE int XML_unknownEncoding (void * encodingHandlerData,
				 const XML_Char * name,
				 XML_Encoding * info)
{
    return 0;
}

/* ------------------------------------------------------------------------- */
/*			     HTXML STREAM HANDLERS			     */
/* ------------------------------------------------------------------------- */

PRIVATE void rdf_setHandlers (XML_Parser me)
{
    XML_SetElementHandler(me, XML_startElement, XML_endElement);
    XML_SetCharacterDataHandler(me, XML_characterData);
    XML_SetProcessingInstructionHandler(me, XML_processingInstruction);
    XML_SetDefaultHandler(me, XML_default);
    XML_SetUnparsedEntityDeclHandler(me, XML_unparsedEntityDecl);
    XML_SetNotationDeclHandler(me, XML_notationDecl);
    XML_SetExternalEntityRefHandler(me, XML_externalEntityRef);
    XML_SetUnknownEncodingHandler(me, XML_unknownEncoding, NULL);
}

PRIVATE void rdf_newInstance (HTStream *		me,
			      HTRequest *		request,
			      HTFormat 			target_format,
			      HTStream *		target_stream,
			      XML_Parser		xmlparser,
			      void *			context)
{
    if (me && xmlparser) {
	rdf_setHandlers(xmlparser);
	XML_SetUserData(xmlparser, context);

	/* Call the new RDF instance callback (if any) with this new stream */
	if (RDFInstance)
	    (*RDFInstance)(me, request, target_format, target_stream, context, RDFInstanceContext);
    }
}

/* ------------------------------------------------------------------------- */
/*				RDF PARSER				     */
/* ------------------------------------------------------------------------- */

PRIVATE void visit_element_children (HTList *children)
{
    HTElement *child = NULL;
    HTList *cur = children;
    while ((child = (HTElement *) HTList_nextObject(cur))) {
	if (!HTList_isEmpty(child->m_children)) 
	    visit_element_children(child->m_children);
	HTElement_delete(child);
    }
}

PRIVATE void delete_elements (HTRDF * me)
{
    if (me && me->m_root) {
	HTElement *r = me->m_root;
	if (!HTList_isEmpty(r->m_children)) 
	    visit_element_children(r->m_children);
	HTElement_delete(r);
    }
}

PRIVATE void delete_literal_elements (HTRDF * me)
{
    if (me && me->m_literalStack) {
	HTList *cur = me->m_literalStack;
	HTElement *e = NULL;
	while ((e = (HTElement *) HTList_nextObject(cur))) {
	    HTElement_delete(e);
        }
	HTList_delete(me->m_literalStack);
    }
}


PUBLIC HTRDF * HTRDF_new (void)
{
    HTRDF * me;
    if ((me = (HTRDF *) HT_CALLOC(1, sizeof(HTRDF))) == NULL)
	HT_OUTOFMEM("HTRDF_new");
    me->m_namespaceStack = HTList_new();
    me->m_elementStack = HTList_new();
    me->m_literalStack = HTList_new();

    me->m_triples = HTList_new();
    me->m_vAllNameSpaces = HTList_new();

    me->m_bCreateBags = FALSE;
    me->m_bFetchSchemas = FALSE;
    
    me->m_parseTypeStack = HTList_new();
    me->m_parseElementStack = HTList_new();

    me->m_vResources = HTList_new();    
    me->m_vResolveQueue = HTList_new();
    me->m_hIDtable = HTHashtable_new(0);

    return me;
}

PUBLIC BOOL HTRDF_delete (HTRDF * me)
{
    if (me) {
	delete_elements(me);
	if (me->m_namespaceStack) {
	    HTList *cur = me->m_namespaceStack;
	    HTAssocList *alist = NULL;
	    while ((alist = (HTAssocList *) HTList_nextObject(cur))) {
		HTAssocList_delete(alist);
	    }
	    HTList_delete(me->m_namespaceStack);
	}
	if (me->m_elementStack) HTList_delete(me->m_elementStack);

        delete_literal_elements(me);

	me->m_root = NULL;
	if (me->m_triples) {
	    HTList *cur = me->m_triples;
	    HTTriple *t = NULL;
	    while ((t = (HTTriple *) HTList_nextObject(cur))) {
		/*HTTriple_print(t);*/
		HTTriple_delete(t);
	    }
	    HTList_delete(me->m_triples); 
	}
	HT_FREE(me->m_sSource);
	if (me->m_vAllNameSpaces) {
	    HTList *cur = me->m_vAllNameSpaces;
	    char * s = NULL;
	    while ((s = (char *) HTList_nextObject(cur))) {
		HT_FREE(s);
	    }
	    HTList_delete(me->m_vAllNameSpaces);
	}
	if (me->m_parseTypeStack)
	    HTList_delete(me->m_parseTypeStack);
	if (me->m_parseElementStack)
	    HTList_delete(me->m_parseElementStack);
	if (me->m_vResources) 
	    HTList_delete(me->m_vResources);    
	if (me->m_vResolveQueue)
	    HTList_delete(me->m_vResolveQueue);
	if (me->m_hIDtable) 
	    HTHashtable_delete(me->m_hIDtable);
	HT_FREE(me->m_sLiteral);
	HT_FREE(me);
	return YES;
    }
    return NO;
}

/*
 * setSource method saves the name of the source document for
 * later inspection if needed
 */
PUBLIC BOOL HTRDF_setSource(HTRDF *me, char * source)
{
    if (me && source) {
	StrAllocCopy (me->m_sSource, source);
	return YES;
    }
    return NO;
}

/*
 * Go through the m_vResolveQueue and assign
 * direct object reference for each symbolic reference
 */
PUBLIC BOOL HTRDF_resolve (HTRDF * me)
{
    if (me) {
	HTList * cur = me->m_vResolveQueue;
	HTElement *e = NULL;
	HTElement *e2 = NULL;
	while ((e = (HTElement *) HTList_nextObject(cur))) {
	    char * sAbout = HTElement_getAttribute2(e, RDFMS, "about");
	    char * sResource = HTElement_getAttribute2(e, RDFMS, "resource");
	    char * sAboutEach = HTElement_getAttribute2(e, RDFMS, "aboutEach");
	    char * sAboutEachPrefix = HTElement_getAttribute2(e, RDFMS,
							      "aboutEachPrefix");
	    if (sAbout) {
		if (sAbout[0]=='#') 
		    sAbout = &(sAbout[1]);
		e2 = (HTElement *) HTRDF_lookforNode(me, sAbout);
		if (e2)
		    HTElement_addTarget(e, e2);
		else
		    HTPrint("Unresolved internal reference %s\n", sAbout);
	    }
	    if (sResource) {
		if (sResource[0]=='#') 
		    sResource = &(sResource[1]);
		e2 = (HTElement *) HTRDF_lookforNode(me, sResource);
		if (e2)
		    HTElement_addTarget(e, e2);
	    }

	    if (sAboutEach) {
		sAboutEach = &(sAboutEach[1]);
		e2 = (HTElement *) HTRDF_lookforNode(me, sAboutEach);
		if (e2)
		    HTElement_addTarget(e, e2);
	    }
	    if (sAboutEachPrefix) {
		HTList * curr = me->m_vResources;
		HTElement *ele = NULL;
		while ((ele = (HTElement *) HTList_nextObject(curr))) {
		    char * sA = HTElement_getAttribute2(ele, RDFMS, "about");
		    if (sA && 
		       !strncmp(sA, sAboutEachPrefix, strlen(sAboutEachPrefix))) {
			HTElement_addTarget(e, ele);		    
		    }
		}
	    }
	}
	HTList_delete(me->m_vResources);
	me->m_vResources = HTList_new(); 
	return YES;
    }
    return NO;
}

/**
 * Check if the element e is from the namespace
 * of the RDF schema by comparing only the beginning of
 * the expanded element name with the canonical RDFMS
 * URI
 */
PUBLIC BOOL HTRDF_isRDF(HTRDF * me, HTElement *e)
{
    return (me && e && e->m_sName) ?
	(!strncmp(e->m_sName, RDFMS, strlen(RDFMS))) : NO;
}

PUBLIC BOOL HTRDF_isRDFroot (HTRDF * me, HTElement *e)
{
    if (me && e && e->m_sName) {
	int len = strlen(e->m_sName);
	if (len > 3) return (HTRDF_isRDF(me, e) && !strcmp(&(e->m_sName[len-3]), "RDF"));
    }
    return NO;
}

/**
 * Is the element a Description
 */
PUBLIC BOOL HTRDF_isDescription (HTRDF *me, HTElement *e)
{
    if (me && e && e->m_sName) {
	int len = strlen(e->m_sName);
	if (len > 11) return (HTRDF_isRDF(me, e) && !strcmp(&(e->m_sName[len-11]), "Description"));
    }
    return NO;
}

/*
 * Is the element a ListItem
 */
PUBLIC BOOL HTRDF_isListItem (HTRDF *me, HTElement *e) 
{
    if (me && e && e->m_sName) {
	int len = strlen(e->m_sName);
	if (len > 2)
	    return (HTRDF_isRDF(me, e) && (!strcmp(&(e->m_sName[len-2]), "li") || strchr(e->m_sName,'_')));
    }
    return NO;
}

/**
 * Is the element a Sequence
 */
PUBLIC BOOL HTRDF_isSequence (HTRDF *me, HTElement *e)
{
    if (me && e && e->m_sName) {
	int len = strlen(e->m_sName);
	if (len > 3) return (HTRDF_isRDF(me, e) && !strcmp(&(e->m_sName[len-3]), "Seq"));
    }
    return NO;
}

/*
 * Is the element an Alternative
 */
PUBLIC BOOL HTRDF_isAlternative (HTRDF *me, HTElement *e)
{
    if (me && e && e->m_sName) {
	int len = strlen(e->m_sName);
	if (len > 3) return (HTRDF_isRDF(me, e) && !strcmp(&(e->m_sName[len-3]), "Alt"));
    }
    return NO;
}

/*
 * Is the element a Bag
 */
PUBLIC BOOL HTRDF_isBag (HTRDF *me, HTElement *e)
{
    if (me && e && e->m_sName) {
	int len = strlen(e->m_sName);
	if (len > 3) return (HTRDF_isRDF(me, e) && !strcmp(&(e->m_sName[len-3]), "Bag"));
    }
    return NO;
}

/**
 * Is the element a Container
 */
PUBLIC BOOL HTRDF_isContainer (HTRDF *me, HTElement *e)
{
    return (HTRDF_isSequence(me, e) ||
	    HTRDF_isAlternative(me, e) ||
	    HTRDF_isBag(me, e));
}

/*
 * This method matches all properties but those from RDF namespace
 */
PUBLIC BOOL HTRDF_isTypedPredicate(HTRDF *me, HTElement *e)
{
    if (me && e && e->m_sName) {
	int len = strlen(e->m_sName);
	char * tp[] = {"predicate", "subject", "object",
		       "value", "type", "Property", "Statement"};
	int i;
	if (HTRDF_isRDF(me, e)) {
	    for(i = 0; i< 7; i++) {
		int ntp = strlen(tp[i]);
		if (len > ntp) {
		    if (!strcmp(&(e->m_sName[len-ntp]), tp[i]))
			return YES;
		}
	    }
	    return NO;
	}
	if (len > 0) return YES;
    }
    return NO;
}

PRIVATE void HTRDF_processListItem (HTRDF * me, char * sID, HTElement *listitem, 
				    int iCounter)
{
  /*
   * Two different cases for
   * 1. LI element without content (resource available)
   * 2. LI element with content (resource unavailable)
   */
    char * cName = NULL;
    char * sResource = HTRDF_getResource(me, listitem);
    char sdig[20];
    sprintf(sdig, "_%d", iCounter);
    StrAllocMCopy(&cName, RDFMS, sdig, NULL);
    if (sResource) {
	HTRDF_addTriple(me, cName, sID, sResource);
	/* validity checking */
	if (!HTList_isEmpty(listitem->m_children)){
	    HTPrint("Listitem with resource attribute can not have child nodes");
	}
	StrAllocCopy(listitem->m_sID, sResource);
    } else {
	HTList *cur = listitem->m_children;	
	HTElement *n = NULL;
	while ((n = (HTElement *) HTList_nextObject(cur))) {
	    if (HTElement_instanceOfData(n)) {
		HTRDF_addTriple(me, cName, sID, n->m_sContent);
	    } else if (HTRDF_isDescription(me, n)) {
		char * sNodeID = HTRDF_processDescription(me, n, NO, YES, NO);
		HTRDF_addTriple(me, cName, sID, sNodeID);
		StrAllocCopy(listitem->m_sID, sNodeID);
	    } else if (HTRDF_isListItem(me, n)) {
		HTPrint("Can not nest list item inside list item\n");
	    } else if (HTRDF_isContainer(me, n)) {
		char * c = HTRDF_processContainer(me, n);
		HTRDF_addTriple(me, cName, sID, n->m_sID);
		HT_FREE(c);
	    } else if (HTRDF_isTypedPredicate(me, n)) {
		char * sNodeID = HTRDF_processTypedNode(me, n);
		HTRDF_addTriple(me, cName, sID, sNodeID);
		HT_FREE(sNodeID);
	    }
	}
    }
    HT_FREE(cName);
}

PRIVATE char * HTRDF_processContainer(HTRDF *me, HTElement *n)
{
    char * sID = NULL;
    char * tName = NULL;
    char * aName = NULL;
    char * sName = NULL;
    char * bName = NULL;
    StrAllocMCopy(&tName, RDFMS, "type", NULL);
    StrAllocMCopy(&aName, RDFMS, "Alt", NULL);
    StrAllocMCopy(&sName, RDFMS, "Seq", NULL);
    StrAllocMCopy(&bName, RDFMS, "Bag", NULL);

    StrAllocCopy(sID, n->m_sID);
    if (!sID)
	sID = HTRDF_newReificationID(me);
    /*
     * Do the instantiation only once
     */
    if (!n->m_bDone) {
	if (HTRDF_isSequence(me, n)) {
	    HTRDF_addTriple(me, tName, sID, sName);
	} else if (HTRDF_isAlternative(me, n)) {
	    HTRDF_addTriple(me, tName, sID, aName);
	} else if (HTRDF_isBag(me, n)) {
	    HTRDF_addTriple(me, tName, sID, bName);
	}
	n->m_bDone = YES;
    }
    HTRDF_expandAttributes(me, n, n);

    {
	HTList *cur = n->m_children;
	HTElement *n2 = NULL;
	int iCounter = 1;
	if (HTList_isEmpty(cur) && HTRDF_isAlternative(me, n))
	    HTPrint("An RDF:Alt container must have at least one list item\n");
	while ((n2 = (HTElement *) HTList_nextObject(cur))) {
	    if (HTRDF_isListItem(me, n2)) {
		HTRDF_processListItem(me, sID, n2, iCounter);
		iCounter++;
	    } else {
		HTPrint("Can not nest %s, inside container\n", n2->m_sName);
	    }
	}
    } /* end of block */
    
    HT_FREE(tName);    HT_FREE(sName);    HT_FREE(aName);    HT_FREE(bName);

    return sID;
}
/*
 * Manage the typedNode production in the RDF grammar.
 *
 */
PUBLIC char * HTRDF_processTypedNode(HTRDF *me, HTElement *typedNode)
{
    char * sID = HTElement_getAttribute2(typedNode, RDFMS, "ID");
    char * sBagID = HTElement_getAttribute2(typedNode, RDFMS, "bagID");
    char * sAbout = HTElement_getAttribute2(typedNode, RDFMS, "about");
    char * sAboutEach = HTElement_getAttribute2(typedNode, RDFMS, "aboutEach");
    /*char * sAboutEachPrefix = HTElement_getAttribute2(typedNode, RDFMS,
						      "aboutEachPrefix");*/
    char * resource =  HTElement_getAttribute2(typedNode, RDFMS, "resource");
    char * iName = NULL;
    char * bName = NULL;
    char * tName = NULL;

    char * sObject = NULL;

    StrAllocMCopy(&iName, RDFMS, "ID", NULL);
    StrAllocMCopy(&bName, RDFMS, "bagID", NULL);
    StrAllocMCopy(&tName, RDFMS, "type", NULL);

    if (resource)
	HTPrint("resource attribute not allowed for a typedNode %s\n",
		typedNode->m_sName);
    
        /*
	 * We are going to manage this typedNode using the processDescription
	 * routine later on. Before that, place all properties encoded as
	 * attributes to separate child nodes.
	 */
    {
	HTAssoc * assoc;
	HTAssocList *cur = typedNode->m_attributes;
	char * sAttribute = NULL;
	char * tValue = NULL;
	char * sValue = NULL;
	while((assoc= (HTAssoc *) HTList_nextObject(cur))) {
	    sAttribute = HTAssoc_name(assoc);
	    sValue = HTAssoc_value(assoc);
	    tValue = trim(sValue);
	    if (strncmp(sAttribute, RDFMS, strlen(RDFMS)) &&
	       strncmp(sAttribute, XMLSCHEMA, strlen(XMLSCHEMA))) {
		if (strlen(tValue) > 0) {
		    HTAssocList *newAL = HTAssocList_new();
		    HTElement *newPredicate = HTElement_new(sAttribute, newAL);
		    HTElement *d = NULL;
		    HTElement_addAttribute(newPredicate, iName,
					   sAbout ? sAbout : sID);
		    HTElement_addAttribute(newPredicate, bName, sBagID);
		    d = HTElement_new2(tValue);
		    HTElement_addChild(newPredicate, d);
		    HTElement_addChild(typedNode, newPredicate);
		}
	    }	
	    HT_FREE(tValue);
	} /* end of while */
    }/* end of block */
    {
	if (sAbout)
	    StrAllocCopy(sObject, sAbout);
	else if (sID)
	    StrAllocCopy(sObject, sID);
	else
	    sObject = HTRDF_newReificationID(me);
	StrAllocCopy(typedNode->m_sID, sObject);

	/* special case: should the typedNode have aboutEach attribute,
	** the type predicate should distribute to pointed
	** collection also -> create a child node to the typedNode
	*/
	if (sAboutEach && !HTList_isEmpty(typedNode->m_vTargets)) {
	    HTAssocList *newAL = HTAssocList_new();
	    HTElement *newPredicate = HTElement_new(tName, newAL);
	    HTElement *d = HTElement_new2(typedNode->m_sName);
	    HTElement_addChild(newPredicate, d);
	    HTElement_addChild(typedNode, newPredicate);
	} else {
	    HTRDF_addTriple(me, tName, sObject, typedNode->m_sName);
	}
	HTRDF_processDescription(me, typedNode, NO, NO, YES);
    }/* end of block */
	
    HT_FREE(iName);     HT_FREE(bName);     HT_FREE(tName);

    return sObject;
}

/*
 * Start processing an RDF/XML document instance from the
 * root element rdf.
 *
 */
PUBLIC BOOL HTRDF_processRDF (HTRDF *me, HTElement *e)
{
    if (me && e) {
	HTList *cur = e->m_children;
	HTElement *ele = NULL;
	if (HTList_isEmpty(e->m_children)) {
	    HTPrint("Empty RDF Element\n");
	    return NO;
	}
	while ((ele= (HTElement *) HTList_nextObject(cur))) {
	    if (HTRDF_isDescription(me, ele)) {
		HTRDF_processDescription(me, ele, NO, me->m_bCreateBags,
					 me->m_bCreateBags);
	    } else if (HTRDF_isContainer(me, ele)) {
		char * c = HTRDF_processContainer(me, ele);
		HT_FREE(c);
	    } else 	if (HTRDF_isTypedPredicate(me, ele)) {
		char * t = HTRDF_processTypedNode(me, ele);
		HT_FREE(t);
	    }
	}
	return YES;
    }
    return NO;
}

/*
 * processPredicate handles all elements not defined as special
 * RDF elements.
 *
 * predicate	The predicate element itself
 * description Context for the predicate
 * sTarget	The target resource
 * reificate	Should this predicate be reificated
 *
 * return the new ID which can be used to identify the predicate
 *
 */
PRIVATE char * HTRDF_processPredicate (HTRDF * 		me,
				       HTElement * 	predicate,
				       HTElement *	description,
				       char * 		sTarget,
				       BOOL 		reificate)
{
    char * sStatementID = HTElement_getAttribute2(predicate, RDFMS, "ID");
    char * nsStatementID = NULL;
    char * sBagID = HTElement_getAttribute2(predicate, RDFMS, "bagID");
    char * sResource = HTRDF_getResource(me, predicate);

    /*
    ** If a predicate has other attributes than rdf:ID, rdf:bagID,
    ** or xmlns... -> generate new triples according to the spec.
    ** (See end of Section 6)
    */
    {
	HTElement * place_holder = NULL;
	HTAssocList * newAL = HTAssocList_new();
	char * fName = NULL;
	char * aName = NULL;

	StrAllocMCopy(&fName, RDFMS, "Description", NULL);
	place_holder = HTElement_new(fName, newAL);
	HT_FREE(fName);

	if (HTRDF_expandAttributes(me, place_holder, predicate)) {

	    /* error checking */
	    if (!HTList_isEmpty(predicate->m_children)) {
		HTPrint("%s must be an empty element since it uses propAttr grammar production", predicate->m_sName);
		HTElement_delete(place_holder);
		return NULL;
	    }	
	    StrAllocMCopy(&aName, RDFMS, "about", NULL);    

	    /* determine the 'about' part for the new statements */
	    if (sStatementID) {
		HTElement *data = HTElement_new2(sStatementID);
		HTElement_addAttribute(place_holder, aName, sStatementID);

		/* hack: make rdf:ID the value of the predicate */
		HTElement_addChild(predicate, data);
	    } else if (sResource) {
		HTElement_addAttribute(place_holder, aName, sResource);
	    } else {
		nsStatementID = HTRDF_newReificationID(me);
		HTElement_addAttribute(place_holder, aName, nsStatementID);
		HT_FREE(nsStatementID);
	    }
	    HT_FREE(aName);

	    if (sBagID) {
		StrAllocMCopy(&fName, RDFMS, "bagID", NULL);    
		HTElement_addAttribute(place_holder, fName, sBagID);
		HT_FREE(fName);
		StrAllocCopy(place_holder->m_sBagID, sBagID);
	    }
	    HTRDF_processDescription(me, place_holder, NO, NO, me->m_bCreateBags);
	} else {

	    /* Nothing but xmlns or RDF stuff, so we don't need new element */
	    HTElement_delete(place_holder);
	}
    }

    /*
    ** Tricky part: if the resource attribute is present for a predicate
    ** AND there are no children, the value of the predicate is either
    ** 1. the URI in the resource attribute OR
    ** 2. the node ID of the resolved #resource attribute
    */
    if (sResource && HTList_isEmpty(predicate->m_children)) {
	if (!HTElement_target(predicate)) {
	    if (reificate) {
		HT_FREE(nsStatementID);
		nsStatementID = HTRDF_reificate (me, predicate->m_sName, 
						 sTarget, sResource, 
						 predicate->m_sID);
		StrAllocCopy(predicate->m_sID, nsStatementID);
	    } else {	
		HTRDF_addTriple(me, predicate->m_sName, sTarget, sResource);
	    }
	} else {
	    HTElement *target = HTElement_target(predicate);
	    if (reificate) {
		nsStatementID = HTRDF_reificate (me, predicate->m_sName, 
						sTarget,
						target->m_sID,
						predicate->m_sID);
		StrAllocCopy(predicate->m_sID, nsStatementID);
	    } else {
		HTRDF_addTriple(me, predicate->m_sName, sTarget, target->m_sID);
	    }
	}
	if (nsStatementID && predicate->m_sID)
	    StrAllocCopy(nsStatementID, predicate->m_sID);
	return nsStatementID;
    }

    /*
    ** Does this predicate make a reference somewhere using the
    ** sResource attribute
    */
    if (sResource && HTElement_target(predicate)) {
	char * dStatementID = HTRDF_processDescription(me,
						       HTElement_target(predicate),
						       YES, NO, NO);
	if (reificate) {
	    HT_FREE(nsStatementID);
	    nsStatementID = HTRDF_reificate (me, predicate->m_sName, 
					    sTarget, dStatementID,
					    predicate->m_sID);
	    StrAllocCopy(predicate->m_sID, nsStatementID);
	} else {
	    StrAllocCopy(nsStatementID, dStatementID);
	    HTRDF_addTriple(me, predicate->m_sName, sTarget, nsStatementID);
	}
	return nsStatementID;
    }

    /*
    ** Before looping through the children, let's check
    ** if there are any. If not, the value of the predicate is
    ** an anonymous node
    */
    {
	HTList *cur = predicate->m_children;
	BOOL bUsedTypedNodeProduction = NO;
	HTElement *n2;
	if (nsStatementID && sStatementID)
	    StrAllocCopy(nsStatementID, sStatementID);
	if (HTList_isEmpty(cur)) {
	    if (reificate) {
		char * nr = HTRDF_newReificationID(me);
		HT_FREE(nsStatementID);
		nsStatementID = HTRDF_reificate (me, predicate->m_sName, 
						sTarget, nr,
						predicate->m_sID);
		HT_FREE(nr);
	    } else {
		char * nr = HTRDF_newReificationID(me);
		HTRDF_addTriple(me, predicate->m_sName, sTarget, nr);
		HT_FREE(nr);
	    }
	}
	while ((n2= (HTElement *) HTList_nextObject(cur))) {
	    if (HTRDF_isDescription(me, n2)) {
		HTElement *d2 = n2;
		char * dStatementID =HTRDF_processDescription(me, d2, YES, NO, NO);
		if (d2->m_sID && dStatementID && d2->m_sID != dStatementID)
		    StrAllocCopy(d2->m_sID, dStatementID);
		
		if (reificate) {
		    HT_FREE(nsStatementID);
		    nsStatementID = HTRDF_reificate (me, predicate->m_sName, 
						    sTarget, dStatementID,
						    predicate->m_sID);
		} else {
		    StrAllocCopy(nsStatementID, dStatementID);
		    HTRDF_addTriple(me, predicate->m_sName, sTarget,
				    nsStatementID);
		}
	    } else if (HTElement_instanceOfData(n2)) {
		char * tValue = NULL;
		char * sValue = n2->m_sContent;
		/* we've  got real data */
                /*
		 * Only if the content is not empty PCDATA (whitespace that is)
		 * print the triple
		 */
		tValue = trim(sValue);
		if (tValue && strlen(tValue) > 0) {
		    if (reificate) {
			HT_FREE(nsStatementID);
			nsStatementID = HTRDF_reificate (me, predicate->m_sName,
							 sTarget, tValue,
							 predicate->m_sID);
			StrAllocCopy(predicate->m_sID, nsStatementID);
		    } else {
			HTRDF_addTriple(me, predicate->m_sName, sTarget, tValue);
		    }
		}
		HT_FREE(tValue);
	    } else if (HTRDF_isContainer(me, n2)) {
		HTElement *target = HTElement_target(description);
		char * aboutTarget  = 
		    target ? 
		    HTElement_getAttribute2(target, RDFMS, "about") : NULL;
		char * sCollectionID = HTRDF_processContainer(me, n2);
		StrAllocCopy(nsStatementID, sCollectionID);
		/* Attach the collection to the current predicate */
		if (target) {
		    if (reificate) {
			HT_FREE(nsStatementID);
			nsStatementID=HTRDF_reificate (me, predicate->m_sName, 
						       aboutTarget,
						       sCollectionID, 
						       predicate->m_sID);
			StrAllocCopy(predicate->m_sID, nsStatementID);
		    } else {
			HTRDF_addTriple(me, predicate->m_sName, aboutTarget,
					sCollectionID);
		    }
		} else {
		    if (reificate) {
			HT_FREE(nsStatementID);
			nsStatementID=HTRDF_reificate (me, predicate->m_sName,
						       sTarget, sCollectionID, 
						       predicate->m_sID);
			StrAllocCopy(predicate->m_sID, nsStatementID);
		    } else {
			HTRDF_addTriple(me, predicate->m_sName, sTarget,
					sCollectionID);
		    }
		}
		HT_FREE(sCollectionID);
	    } else if (HTRDF_isTypedPredicate(me, n2)) {
		if (bUsedTypedNodeProduction) {
		    HTPrint("Only one typedNode allowed inside a predicate (Extra typedNode: %s )\n", n2->m_sName);
		} else {
		    bUsedTypedNodeProduction = YES;
		}
		HT_FREE(nsStatementID);
		nsStatementID = HTRDF_processTypedNode(me, n2);
		HTRDF_addTriple(me, predicate->m_sName, sTarget, nsStatementID);
	    }
	}
	return nsStatementID;
    } /* end of block */
    return NULL;
}

/*
 * processDescription manages Description elements
 *
 *  description	The Description element itself
 *  inPredicate	Is this is a nested description
 *  reificate		Do we need to reificate
 *  createBag		Do we create a bag container
 *
 * return		An ID for the description
 *
 */
PUBLIC char * HTRDF_processDescription (HTRDF *		me,
				        HTElement *	description,
					BOOL 		inPredicate,
					BOOL 		reificate,
					BOOL 		createBag)
{
    int iChildCount = 1;
    BOOL bOnce = YES;

    char * sAbout = HTElement_getAttribute2(description, RDFMS, "about");
    char * sAboutEach = HTElement_getAttribute2(description, RDFMS, "aboutEach");
    char * sAboutEachPrefix = HTElement_getAttribute2(description, RDFMS,
						      "aboutEachPrefix");
    char * sBagid = HTElement_getAttribute2(description, RDFMS, "bagID");
    char * sID = HTElement_getAttribute2(description, RDFMS, "ID");
    HTElement *target = HTElement_target(description);
    BOOL hasTarget = HTList_isEmpty(description->m_vTargets) ? NO : YES;
    BOOL targetIsContainer = NO;
    char * sTargetAbout = NULL;
    char * sTargetBagID = NULL;
    char * sTargetID = NULL;
    char * dName = NULL;
    char * aName = NULL;

    /*
    ** Return immediately if the description has already been managed
    */
    if (description->m_bDone) return description->m_sID;

    StrAllocMCopy(&dName, RDFMS, "Description", NULL);
    StrAllocMCopy(&aName, RDFMS, "about", NULL);

    /*
    ** Determine what the target of the Description reference is
    */
    if (hasTarget) {
	char * sTargetID2 = HTElement_getAttribute2(target, RDFMS, "ID");
	sTargetAbout = HTElement_getAttribute2(target, RDFMS, "about");
        sTargetBagID = HTElement_getAttribute2(target, RDFMS, "bagID");
	if (me->m_sSource && sTargetID2) {
	    StrAllocMCopy(&sTargetID, me->m_sSource, sTargetID2, NULL);
	} else {
	    StrAllocCopy(sTargetID, sTargetID2);
	}
        /*
	 * Target is collection if
	 * 1. it is identified with bagID attribute
	 * 2. it is identified with ID attribute and is a collection
	 */
	if (sTargetBagID && sAbout) {
	    targetIsContainer = !strcmp(&(sAbout[1]), sTargetBagID);
	} else {
	    if (sTargetID && sAbout && !strcmp(&(sAbout[1]), sTargetID) &&
	       HTRDF_isContainer(me, target))
		targetIsContainer = YES;
	}
	HT_FREE(sTargetID);
    }

    /*
     * Check if there are properties encoded using the abbreviated
     * syntax
     */    
    HTRDF_expandAttributes(me, description, description);	

    /*
     * Manage the aboutEach attribute here
     */
    if (sAboutEach && hasTarget) {
	if (HTRDF_isContainer(me, target)) {
	    HTList *cur = target->m_children;
	    HTElement *ele = NULL;
	    while ((ele= (HTElement *) HTList_nextObject(cur))) {
		if (HTRDF_isListItem(me, ele)) {
		    char * sResource = HTRDF_getResource(me, ele);
		    if (sResource) {
			HTElement * newDescription = NULL;
			HTElement * ele2;
			HTList * cur2 = description->m_children;

                        /*
			 * Manage <li resource="..." /> case
			 */
			if (sResource) {
			    HTAssocList *newAL = HTAssocList_new();
			    newDescription = HTElement_new(dName, newAL);
			    HTElement_addAttribute(newDescription, aName, sResource);
			}

			while ((ele2 = (HTElement *) HTList_nextObject(cur2))){
			    if (newDescription) HTElement_addChild(newDescription, ele2);
			}

			if (newDescription)
			    HTRDF_processDescription(me, newDescription, NO, NO, NO);

			/* Not needed anymore */
			HTElement_delete(newDescription);

		    } else {
			/**
			 * Otherwise we have a structured value inside <li>
			 *
			 * loop through the children of <li>
			 * (can be only one)
			*/
			HTList *cur2 = ele->m_children;
			HTElement *ele2 = NULL;
          	       while ((ele2 = (HTElement *) HTList_nextObject(cur2))) {
			    HTAssocList *newAL = HTAssocList_new();
			    HTElement *newNode = HTElement_new(dName, newAL);
			    HTList *cur3 = description->m_children;
			    HTElement *ele3 = NULL;
			    /* loop through the items in the
			     * description with aboutEach
			     * and add them to the target
			     */
			    while ((ele3 = (HTElement *) 
				    HTList_nextObject(cur3))) {
				HTElement_addChild(newNode, ele3);
			    }
			    HTElement_addTarget(newNode, ele2);
			    HTRDF_processDescription(me, newNode, YES, NO, NO);
			}
		    }
		} else if (HTRDF_isTypedPredicate(me, ele)) {
		    HTAssocList *newAL = HTAssocList_new();
		    HTElement *newNode = HTElement_new(dName, newAL);
		    HTList *cur2 = description->m_children;
		    HTElement *ele2 = NULL;
		    while ((ele2 = (HTElement *) HTList_nextObject(cur2))) {
			HTElement_addChild(newNode, ele2);
		    }
		    HTElement_addTarget(newNode, ele);
		    HTRDF_processDescription(me, newNode, YES, NO, NO);
		}
	    } /* end of while */
	} else if (HTRDF_isDescription(me, target)) {
	    HTList *cur = target->m_children;
	    HTElement *ele = NULL;
	    while ((ele = (HTElement *) HTList_nextObject(cur))) {
		HTAssocList *newAL = HTAssocList_new();
		HTElement *newNode = HTElement_new(dName, newAL);
		HTList *cur2 = description->m_children;
		HTElement *ele2 = NULL;
		while ((ele2 = (HTElement *) HTList_nextObject(cur2))) {
		    HTElement_addChild(newNode, ele2);
		}
		HTElement_addTarget(newNode, ele);
		HTRDF_processDescription(me, newNode, YES, NO, NO);
	    } /* end of while */
	}

	HT_FREE(dName);
	HT_FREE(aName);
	return NULL;
    }

    /*
     * Manage the aboutEachPrefix attribute here
     */
    if (sAboutEachPrefix) {
	if (hasTarget) {
	    HTList *cur = description->m_vTargets;
	    HTElement *target = NULL;
	    while ((target = (HTElement *) HTList_nextObject(cur))) {
		HTList *cur2 = description->m_children;
		HTElement *ele2 = NULL;
		HTElement *newDescription = NULL;
		HTAssocList *newAL = HTAssocList_new();
		sTargetAbout = HTElement_getAttribute2(target, RDFMS, "about");
		newDescription = HTElement_new(dName, newAL);
		HTElement_addAttribute(newDescription, aName, sTargetAbout);
		while ((ele2 = (HTElement *) HTList_nextObject(cur2))) {
		    HTElement_addChild(newDescription, ele2);
		}
		HTRDF_processDescription(me, newDescription, NO, NO, NO);
	    }
	}

	HT_FREE(dName);
	HT_FREE(aName);
	return NULL;
    }
    /*
     * Enumerate through the children
     */
    {
	HTList *cur = description->m_children;
	HTElement *n = NULL;
	while ((n = (HTElement *) HTList_nextObject(cur))) {
	    if (HTRDF_isDescription(me, n))
		HTPrint("Can not nest Description inside Description\n");
	    else if (HTRDF_isListItem(me, n))
		HTPrint("Can not nest List Item inside Description\n");
	    else if (HTRDF_isContainer(me, n))
		HTPrint("Can not nest Container inside Description\n");
	    else if (HTRDF_isTypedPredicate(me, n)) {
		char * sChildID = NULL;
		if (hasTarget && targetIsContainer) {
		    sChildID = HTRDF_processPredicate(me, n, description,
						     target->m_sBagID ? 
						     target->m_sBagID :
						     target->m_sID, NO);
		    StrAllocCopy(description->m_sID, sChildID);
		    createBag = NO;
		} else if (hasTarget) {
		    sChildID = HTRDF_processPredicate(me, n, description,
						      target->m_sBagID ? 
						      target->m_sBagID :
						      target->m_sID, reificate);
		    StrAllocCopy(description->m_sID, sChildID);
		} else if (!hasTarget && !inPredicate) {
		    if (!description->m_sID) {
			char * nr = HTRDF_newReificationID(me);
			StrAllocCopy(description->m_sID, nr);
			HT_FREE(nr);
		    }
		    if (!sAbout) {
			if (sID)
			    sAbout = sID;
			else
			    sAbout = description->m_sID;
		    }
		    sChildID = HTRDF_processPredicate(me, n, description,
						      sAbout, sBagid ? 
						      YES : reificate);
		    
		} else if (!hasTarget && inPredicate) {
		    if (!sAbout) {
			if (sID) {
			    StrAllocCopy(description->m_sID, sID);
			    sAbout = sID;
			} else {
			    if (!description->m_sID) {
				char * nr = HTRDF_newReificationID(me);
				StrAllocCopy(description->m_sID, nr);
				HT_FREE(nr);
			    }
			    sAbout = description->m_sID;
			}
		    } else {
                        if (description->m_sID != sAbout)
			    StrAllocCopy(description->m_sID, sAbout);
		    }
		    sChildID = HTRDF_processPredicate(me, n, description, sAbout, NO);
		}
                 /*
		 * Each Description block creates also a Bag node which
		 * has links to all properties within the block IF
		 * the m_bCreateBags variable is true
		 */
		if (sBagid || (me->m_bCreateBags && createBag)) {
		    char * sNamespace = RDFMS;
		    if (bOnce && sChildID) {
			char * tName = NULL;
			char * bName = NULL;
			bOnce = NO;
			if (!description->m_sBagID) {
			    char * nr = HTRDF_newReificationID(me);
			    StrAllocCopy(description->m_sBagID, nr);
			    HT_FREE(nr);
			}
			if (!description->m_sID)
			    StrAllocCopy(description->m_sID,
					 description->m_sBagID);
			StrAllocMCopy(&tName, sNamespace, "type", NULL);
			StrAllocMCopy(&bName, sNamespace, "Bag", NULL);
			HTRDF_addTriple(me, tName, description->m_sBagID, bName);
			HT_FREE(tName);
			HT_FREE(bName);
			    
		    }
		    if (sChildID) {
			char * tName = NULL;
			char si[20];
			sprintf(si, "%d", iChildCount);
			StrAllocMCopy(&tName, sNamespace, "_", si, NULL);
			HTRDF_addTriple(me, tName, description->m_sBagID, sChildID);
			iChildCount++;
			HT_FREE(tName);
		    }
		}
		HT_FREE(sChildID);
	    }
	}
    } /* end of block*/
    
    description->m_bDone = YES;
    
    HT_FREE(dName);
    HT_FREE(aName);
    return (description->m_sID);
}

/*
 * Given an XML document (well-formed HTML, for example),
 * look for a suitable element to start parsing from
 * 
 */
PUBLIC BOOL HTRDF_processXML (HTRDF *me, HTElement *ele)
{
    if (me && ele) {
	if (HTRDF_isRDF(me, ele)) {
	    if (HTRDF_isRDFroot(me, ele)) {
		HTRDF_processRDF(me, ele);
	    } else if (HTRDF_isDescription(me, ele)) {
		HTRDF_processDescription(me, ele, NO, me->m_bCreateBags,
					 me->m_bCreateBags);
	    }
	} else {
	    HTList *cur = ele->m_children;
	    HTElement *child = NULL;
	    while ((child = (HTElement *) HTList_nextObject(cur))) {
		HTRDF_processXML(me, child);
	    }
	}

	/* MISSING RECURSION */

	return YES;
    }
    return NO;
}

/*
 * Return the root element pointer. This requires the parsing
 * has been already done.
 */
PUBLIC HTElement * HTRDF_root (HTRDF *me)
{
    return me ? me->m_root : NULL;
}

/*
 * Return the full namespace URI for a given prefix sPrefix.
 * The default namespace is identified with xmlns prefix.
 * The namespace of xmlns attribute is an empty string.
 */

PUBLIC char * HTRDF_namespace(HTRDF * me, char * sPrefix)
{
    char * nPrefix = NULL;
    HTAssocList * calist;
    HTList * cur = me->m_namespaceStack;
    
    if (!sPrefix) 
	StrAllocCopy(nPrefix, "xmlns");

    while ((calist = (HTAssocList *) HTList_nextObject(cur))) {
	char * sValue = HTAssocList_findObjectCaseSensitiveExact(calist, sPrefix);
	if (sValue) {
	    StrAllocCopy(nPrefix, sValue);
	    return nPrefix;
	}
    }
    /*
     * Give error only if
     * 1. the prefix is not from the reserved xml namespace
     * 2. the prefix is not xmlns which is to look for the default
     *    namespace
     */
    if (!strcmp(sPrefix, XMLSCHEMA)) {
	StrAllocCopy(nPrefix, sPrefix);
	return nPrefix;
    } else if (!strcmp(sPrefix, "xmlns")) {
	StrAllocCopy(nPrefix, "");
	return nPrefix;
    } else
	HTPrint("Unresolved Namespace prefix %s\n", sPrefix);

    StrAllocCopy(nPrefix, "");
    return nPrefix;
}

/*
 * Methods to determine whether we are parsing
 * parseType="Literal" or parseType="Resource"
 */

PUBLIC BOOL HTRDF_parseLiteral(HTRDF *me)
{
    HTElement *e = NULL;
    HTList *cur = me->m_elementStack;
    if (!HTList_isEmpty(me->m_elementStack)) {
	while((e = (HTElement *) HTList_nextObject(cur))) {
	    char * sParseType = NULL;
	    sParseType = HTElement_getAttribute2(e, RDFMS, "parseType");
	    if (sParseType) {
		if (strcmp(sParseType, "Resource"))
		    return YES;
	    }
	}
    }
    return NO;
}

/*
 * Methods to determine whether we are parsing
 * parseType="Literal" or parseType="Resource"
 */

PUBLIC BOOL HTRDF_parseResource(HTRDF *me)
{
    HTElement *e = NULL;
    HTList *cur = me->m_elementStack;
    if (!HTList_isEmpty(me->m_elementStack)) {
	while((e = (HTElement *) HTList_nextObject(cur))) {
	    char * sParseType = NULL;	
	    sParseType = HTElement_getAttribute2(e, RDFMS, "parseType");
	    if (sParseType) {
		if (!strcmp(sParseType, "Resource"))
		    return YES;
	    }
	}
    }
    return NO;
}
/*
 * checkAttributes goes through the attributes of element e<
 * to see 
 * 1. if there are symbolic references to other nodes in the data model.
 *    in which case they must be stored for later resolving with
 *    resolveLater method.
 * 2. if there is an identity attribute, it is registered using
 *    registerResource or registerID method.
 *
 */

PRIVATE void HTRDF_checkAttributes(HTRDF *me, HTElement *e)
{
    {
	char * sResource = HTElement_getAttribute2(e, RDFMS, "resource");
    
	if (sResource && sResource[0] == '#') 
	    HTRDF_resolveLater(me, e);
    }
    {
	char * sAboutEach = HTElement_getAttribute2(e, RDFMS, "aboutEach");
    
	if (sAboutEach && sAboutEach[0] == '#') 
	    HTRDF_resolveLater(me, e);
    }
    {
        char * sAboutEachPrefix = HTElement_getAttribute2(e, RDFMS,
							  "aboutEachPrefix");
    
	if (sAboutEachPrefix && sAboutEachPrefix[0] == '#') 
	    HTRDF_resolveLater(me, e);
    }
    {
        char * sAbout = HTElement_getAttribute2(e, RDFMS, "about");
	if (sAbout) {
	    if (sAbout[0] == '#') 
		HTRDF_resolveLater(me, e);
	    else
		HTRDF_registerResource(me, e);
	}
    }

    {
        char * sBagID = HTElement_getAttribute2(e, RDFMS, "bagID");
    
	if (sBagID) {
	    HTRDF_registerID(me, sBagID, e);
	    StrAllocCopy(e->m_sBagID, sBagID);
	}
    }
    {
        char * sID = HTElement_getAttribute2(e, RDFMS, "ID");
	if (sID) {
	    HTRDF_registerID(me, sID, e);
	    StrAllocCopy(e->m_sID, sID);
	}
    }
}
/*
 * Add the element e to the m_vResolveQueue
 * to be resolved later.
 */
PUBLIC void HTRDF_resolveLater(HTRDF *me, HTElement *e)
{
    HTList_addObject(me->m_vResolveQueue, e);
}
/*
 * Add an element e to the Hashtable m_hIDtable
 * which stores all nodes with an ID
 */

PUBLIC void HTRDF_registerID(HTRDF *me, char * sID, HTElement *e)
{
    if (HTHashtable_object(me->m_hIDtable, sID))
	HTPrint("Node ID %s redefined", sID);
    HTHashtable_addObject(me->m_hIDtable, sID, e);
}
/*
 * Add an element e to the Vector m_vResources
 * which stores all nodes with an URI
 */
PUBLIC void HTRDF_registerResource(HTRDF *me, HTElement *e)
{
    HTList_addObject(me->m_vResources, e);
}

/*
 * Look for a node by name sID from the Hashtable
 * m_hIDtable of all registered IDs.
 */

PUBLIC HTElement *HTRDF_lookforNode(HTRDF *me, char * sID)
{
    if (sID)
	return (HTElement *) HTHashtable_object(me->m_hIDtable, sID);
    return NULL;
}

/*
** Special method to deal with rdf:resource attribute
*/
PUBLIC char * HTRDF_getResource(HTRDF *me, HTElement *e)
{
   char * sResource = HTElement_getAttribute2(e, RDFMS, "resource");
   if (sResource != NULL && sResource[0] == '\0')
       sResource = me->m_sSource;
   return sResource; 
}

/*
** Take an element ele with its parent element parent
** and evaluate all its attributes to see if they are non-RDF specific
** and non-XML specific in which case they must become children of
** the ele node.
*/
PRIVATE BOOL HTRDF_expandAttributes (HTRDF * me, HTElement * parent, HTElement * ele)
{
    BOOL foundAbbreviation = NO;
    char * sAttribute = NULL;
    char * sValue = NULL;
    HTAssoc * assoc;
    HTAssocList * cur = ele->m_attributes;
    int lxmlschema = strlen(XMLSCHEMA);
    int lrdfms = strlen(RDFMS);

    while ((assoc= (HTAssoc *) HTList_nextObject(cur))) {
	int latt;
	sAttribute = HTAssoc_name(assoc);
	sValue = HTAssoc_value(assoc);

	latt = strlen(sAttribute);
	if (!strncmp(sAttribute, XMLSCHEMA, lxmlschema))
	    continue;	

	if (!strncmp(sAttribute, RDFMS, lrdfms) &&
	   (sAttribute[lrdfms]!='_') &&
	   latt > 5 && strcmp(&(sAttribute[latt-5]), "value") &&
	   strcmp(&(sAttribute[latt-4]), "type")) 
	    continue;

	if (strlen(sValue) > 0) {
	    HTAssocList * newAL = HTAssocList_new();
	    HTElement * newElement = HTElement_new(sAttribute, newAL);
	    HTElement * newData = HTElement_new2(sValue);
	    HTElement_addChild(newElement, newData);
	    HTElement_addChild(parent, newElement);
	    foundAbbreviation = YES;
	}
    }
    return foundAbbreviation;
}

/**
 * Create a new reification ID by using a name part and an
 * incremental counter m_iReificationCounter.
 */
PUBLIC char * HTRDF_newReificationID (HTRDF *me) 
{
    char * nsid = NULL;
    char nsrc[20];
    me->m_iReificationCounter++;
    sprintf(nsrc, "%d", me->m_iReificationCounter);
    if (!me->m_sSource) {
	StrAllocMCopy(&nsid, "genid", nsrc, NULL);
    } else {
	StrAllocMCopy(&nsid, me->m_sSource, "#genid", nsrc, NULL);
    }
    return nsid;
}

/*
 * reificate creates one new node and four new triples
 * and returns the ID of the new node
 */

PRIVATE char * HTRDF_reificate(HTRDF *me, char * sPredicate, char * sSubject,
			      char * sObject, char * sNodeID)
{
    char * sName = NULL;
    char * pName = NULL;
    char * oName = NULL;
    char * tName = NULL;
    char * stName = NULL;
    char * tNodeID = NULL;

    if (!sNodeID)
	tNodeID = HTRDF_newReificationID(me);
    else
	StrAllocCopy(tNodeID, sNodeID);

    StrAllocMCopy(&sName, RDFMS, "subject", NULL);
    StrAllocMCopy(&pName, RDFMS, "predicate", NULL);
    StrAllocMCopy(&oName, RDFMS, "object", NULL);
    StrAllocMCopy(&tName, RDFMS, "type", NULL);
    StrAllocMCopy(&stName, RDFMS, "Statement", NULL);

    /*
     * The original statement must remain in the data model
     */
    HTRDF_addTriple(me, sPredicate, sSubject, sObject);

    /*
     * Do not reificate reificated properties
     */
    if (strcmp(sPredicate, sName) && strcmp(sPredicate, pName) &&
	strcmp(sPredicate, oName) && strcmp(sPredicate, tName)) {

	/* Reificate by creating 4 new triples */
	HTRDF_addTriple(me, pName, tNodeID, sPredicate);
	HTRDF_addTriple(me, sName, tNodeID, (sSubject[0]=='\0' ? me->m_sSource: sSubject));
	HTRDF_addTriple(me, oName, tNodeID, sObject);
	HTRDF_addTriple(me, tName, tNodeID, stName);
    } else
	HT_FREE(tNodeID);	

    HT_FREE(sName);
    HT_FREE(pName);
    HT_FREE(oName);
    HT_FREE(tName);
    HT_FREE(stName);

    return tNodeID;
}
/*
 * Create a new triple and add it to the m_triples List
 * Send the triple to the Output stream
 */

PUBLIC void HTRDF_addTriple (HTRDF *me, char * sPredicate, char * sSubject, 
			     char * sObject) 
{
    HTTriple *t = NULL;

    /*
     * If there is no subject (about=""), then use the URI/filename where
     * the RDF description came from
     */
    if (!sPredicate || !sSubject || !sObject) {
	HTPrint("Predicate %s when subject %s and object %s \n", 
		sPredicate ? sPredicate : "null",
		sSubject ? sSubject : "null",
		sObject ? sObject : "null");
	return;
    }

    if (sSubject[0]=='\0')
	sSubject = me->m_sSource;

    t = HTTriple_new(sPredicate, sSubject, sObject);
    
    /* Call the triple callback handler (if any) with this new triple */
    if (me->newTripleInstance && t) (*(me->newTripleInstance))(me, t, me->tripleContext);

    HTList_addObject(me->m_triples, t);
}

/*
 * createBags method allows one to determine whether SiRPAC
 * produces Bag instances for each Description block.
 * The default setting is not to generate them.
 */

PUBLIC void HTRDF_createBags(HTRDF *me, BOOL b)
{
    if (me)
	me->m_bCreateBags = b;
}

/*
  Set output stream for RDF parser
*/

PUBLIC void HTRDF_setOutputStream(HTRDF *me, HTStream *ostream)
{
    if (me)
	me->ostream = ostream;
}

PUBLIC BOOL HTRDF_registerNewTripleCallback (HTRDF * me, HTTripleCallback_new * cbf, void * context)
{
    if (me) {
	me->newTripleInstance = cbf;
	me->tripleContext = context;
	return YES;
    }
    return NO;
}

PUBLIC BOOL HTRDF_registerNewParserCallback (HTRDFCallback_new * me, void * context)
{
    RDFInstance = me;
    RDFInstanceContext = context;
    return YES;
}

/* ------------------------------------------------------------------------- */
/*			     HTRDFTriples STREAM HANDLERS		     */
/* ------------------------------------------------------------------------- */

PRIVATE int generate_triples(HTStream *me)
{
    HTRDF *rdfp = me ? me->rdfparser : NULL;
    if (rdfp) {

	HTRDF_resolve(rdfp);

	HTRDF_processXML(rdfp, HTRDF_root(rdfp));

	return HT_OK;
    }
    return HT_ERROR;
}

PRIVATE int HTRDFTriples_flush (HTStream * me)
{
    if (me->target)
	return (*me->target->isa->flush)(me->target);
    return HT_OK;
}

PRIVATE int HTRDFTriples_free (HTStream * me)
{
    int status = HT_OK;

    status = generate_triples(me);

    HTRDF_delete(me->rdfparser);

    if (me->target) {
	if ((status = (*me->target->isa->_free)(me->target)) == HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
    }
    HTTRACE(XML_TRACE, "RDF Parser.. FREEING...\n");
    HT_FREE(me);
    return status;
}

PRIVATE int HTRDFTriples_abort (HTStream * me, HTList * e)
{
    HTTRACE(XML_TRACE, "RDF Parser.. ABORTING...\n");
    HTRDF_delete(me->rdfparser);
    if (me->target)	
	(*me->target->isa->abort)(me->target, NULL);
    HT_FREE(me);
    return HT_ERROR;
}

PRIVATE int HTRDFTriples_write (HTStream * me, const char * buf, int len)
{
    return HT_OK;
}

PRIVATE int HTRDFTriples_putCharacter (HTStream * me, char c)
{
    return HTRDFTriples_write(me, &c, 1);
}

PRIVATE int HTRDFTriples_putString (HTStream * me, const char * s)
{
    return HTRDFTriples_write(me, s, (int) strlen(s));	
}

PRIVATE const HTStreamClass HTRDFTriplesClass =
{		
    "rdf",
    HTRDFTriples_flush,
    HTRDFTriples_free,
    HTRDFTriples_abort,
    HTRDFTriples_putCharacter,
    HTRDFTriples_putString,
    HTRDFTriples_write
}; 

PRIVATE HTStream * RDFParser_new (HTRequest *		request,
				  void *		param,
				  HTFormat		input_format,
				  HTFormat		output_format,
				  HTStream *		output_stream)
{
    HTStream * me = NULL;
    if ((me = (HTStream *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
	HT_OUTOFMEM("HTRDFTriples_new");
    me->isa = &HTRDFTriplesClass;
    me->state = HT_OK;
    me->request = request;
    me->target = output_stream ? output_stream : HTErrorStream();

    /* Now create the RDF parser instance */
    if ((me->rdfparser = HTRDF_new()) == NULL) {
	HT_FREE(me);
	return HTErrorStream();
    }

    /* Set the source (I guess mostly to follow SiRPAC API) */
    {
	char * uri = HTAnchor_address((HTAnchor *) HTRequest_anchor(request));
	HTRDF_setSource(me->rdfparser, uri);
	HT_FREE(uri);
    }

    /* Where are we putting data? */
    HTRDF_setOutputStream(me->rdfparser, me);

    /* If you want to create Bags, change it to YES */
    HTRDF_createBags(me->rdfparser, NO); 

    /* Register our new XML Instance handler */
    /* @@@ THIS SHOULD BE DONE USING XML NAMESPACE SO THAT WE DON'T CONFLICT @@@ */
    HTXMLCallback_registerNew(rdf_newInstance, me->rdfparser);

    HTTRACE(XML_TRACE, "RDF Parser.. Stream created\n");

    return me;
}

PUBLIC HTStream * HTRDFParser_new (HTRequest *		request,
				   void *		param,
				   HTFormat		input_format,
				   HTFormat		output_format,
				   HTStream *		output_stream)
{
    return HTXML_new(request, param, input_format, output_format,
		     RDFParser_new(request, param, input_format, output_format, output_stream));
}

PRIVATE void triple_newInstance (HTRDF * rdfp, HTTriple * t, void * context)
{
    if (rdfp && t) {
	HTStream *ostream = rdfp->ostream;
	if (ostream) {
	    PUTC(ostream,'(');
	    PUTS(ostream, t->m_sPredicate);
	    PUTC(ostream,',');
	    PUTS(ostream, t->m_sSubject);
	    PUTC(ostream,',');
	    PUTS(ostream, t->m_sObject);
	    PUTC(ostream,')');
	    PUTC(ostream,'\n');
	}
    }
}

PUBLIC HTStream * HTRDFToTriples (HTRequest *		request,
				  void *		param,
				  HTFormat		input_format,
				  HTFormat		output_format,
				  HTStream *		output_stream)
{
    HTStream * me = RDFParser_new(request, param, input_format, output_format, output_stream);
    HTTRACE(XML_TRACE, "RDF Converter. To Triples\n");

    /* Register our own tripple instance handler */
    HTRDF_registerNewTripleCallback(me->rdfparser, triple_newInstance, NULL);

    /* Create an XML parser instance and return */
    return HTXML_new(request, param, input_format, output_format, me);
}

/*	
**	This function initializes the XML parser and RDF parsers
**      that are used to synchronously parse a file of RDF or a
**      a buffer of RDF.
**
**      Parameters:
**        xmlparser - MODIFIED the XML parser to create.  The caller is 
**            responsible for free'ing this pointer.
**        rdfparser - MODIFIED the RDF parser to create.  The caller is 
**            responsible for free'ing this pointer.
**        stream - MODIFIED the HTStream needed by the RDF parser.  The
**            caller is responsible for free'ing this pointer.
**        uri - the URI created from name.  It is used by the RDF parser
**             when creating anonymous node names.  The caller is 
**             responsible for freeing this pointer.
**        new_triple_callback - the callback invoked when a new triple
**             is created.  If NULL, the default handler will be invoked.
**        context - a void pointer to pass to the new_triple_callback.
**             Should set NULL if no data needs to be passed to the
**             callback.
**        name - the file name or buffer name to be used when the RDF
**             parser needs a document name
**
**      Returns:
**        YES if the initialization succeeds; otherwise NO is returned 
**        and an error message is logged.
**/

PRIVATE BOOL initialize_parsers(XML_Parser *xmlparser, HTRDF **rdfparser, 
    HTStream **stream, char **uri, HTTripleCallback_new * new_triple_callback, 
    void *context, const char * name)
{
    /* Create an XML parser */
#ifdef USE_NS
    *xmlparser = XML_ParserCreateNS (NULL, ':');
#else
    *xmlparser = XML_ParserCreate (NULL);
#endif /* USE_NS */

    if (!*xmlparser) {
	HTTRACE(XML_TRACE, "RDF_Parser.  Could not allocate memory for XML parser.\n");
        return NO;
    }

    /* We need also need RDF parser to create the triples */
    *rdfparser = HTRDF_new();
    if (!*rdfparser) {
        XML_ParserFree(*xmlparser);
	HTTRACE(XML_TRACE, "RDF_Parser.  Could not allocate memory for RDF parser.\n");
        return NO;
    }

    /* Must construct a URI from name for the parser */
    *uri = HTLocalToWWW (name, "file:");
    HTRDF_setSource(*rdfparser, *uri);

    HTRDF_createBags(*rdfparser, NO); 

    if (new_triple_callback)
        HTRDF_registerNewTripleCallback(*rdfparser, new_triple_callback, context);
    else
        HTRDF_registerNewTripleCallback(*rdfparser, triple_newInstance, context);

    rdf_setHandlers(*xmlparser);
    XML_SetUserData(*xmlparser, *rdfparser);

    /* Create a stream to be used to process the triple output */
    if ((*stream = (HTStream *) HT_CALLOC(1, sizeof(HTStream))) == NULL) {
        HT_FREE(*uri);
        XML_ParserFree(*xmlparser);
        HTRDF_delete(*rdfparser);
	HTTRACE(XML_TRACE, "RDF_Parser.  Could not allocate memory for HTStream.\n");
        return NO;
    }
    (*stream)->isa = &HTRDFTriplesClass;
    (*stream)->state = HT_OK;
    (*stream)->request = NULL;    /* Don't have a request */
    (*stream)->target = NULL;     /* Don't have another stream */
    (*stream)->rdfparser = *rdfparser;

    return YES;
}

/*	HTRDF_parseFile
**	---------------
**      This function parses a file of RDF in a synchronous, non-blocking
**      way.  In other words, the file is not asynchronously loaded.
**
**      Parameters:
**        file_name      the name of the file to parse
**        new_triple_callback the callback that is invoked when a triple
**                       is created. If NULL, the default triple handler is
**                        invoked.
**        context - a void pointer to pass to the new_triple_callback.
**             Should set NULL if no data needs to be passed to the
**             callback.
**      Returns:
**        Returns YES if the file is successfully parsed; otherwise NO is
**        returned and an error message is logged. 
*/

PUBLIC BOOL HTRDF_parseFile (const char *file_name, 
  HTTripleCallback_new * new_triple_callback, void *context)
{
    char buff[512]; /* the file input buffer */
    FILE *fp;
    XML_Parser xmlparser;
    HTRDF *rdfparser;
    HTStream * stream = NULL;
    char *uri = NULL;
    BOOL status;

    /* Sanity check */
    if (!file_name) {
	HTTRACE(XML_TRACE, "HTRDF_parseFile.  file_name is NULL\n");
	return NO;
    }

    /* If the file does not exist, return now */
    fp = fopen (file_name, "r");
    if (!fp)  { /* annotation index file doesn't exist */
	HTTRACE(XML_TRACE, "HTRDF_parseFile.  File open failed.");
	return NO;
    }

    /* Initialize the XML and RDF parsers */
    status = initialize_parsers(&xmlparser, &rdfparser, &stream, &uri, 
			   new_triple_callback, context, file_name);
    if (!status) {
	fclose (fp);
        return NO;
    }

    /* 
     * The parsing occurs on one read buffer at a time instead of 
     * reading everything into memory and then parsing
     */
    for (;;) {
        int done;
        int buff_len;
        fgets(buff, sizeof(buff), fp);
        if (ferror(fp)) {
            HT_FREE(uri);
            fclose (fp);
            XML_ParserFree(xmlparser);
            HTRDF_delete(rdfparser);
            HT_FREE(stream);
	    HTTRACE(XML_TRACE, "HTRDF_parseFile.  Error reading file.");
            return NO;
        }
        done = feof(fp);
        if (done)
            buff_len = 0;
        else
            buff_len = strlen (buff);
        if (! XML_Parse(xmlparser, buff, buff_len, done)) {
            fprintf (stderr, "Parse error at line %d:\n%s\n",
                     XML_GetCurrentLineNumber(xmlparser),
                     XML_ErrorString(XML_GetErrorCode(xmlparser)));
            HT_FREE(uri);
            fclose(fp);
            XML_ParserFree(xmlparser);
            HTRDF_delete(rdfparser);
            HT_FREE(stream);
	    HTTRACE(XML_TRACE, "HTRDF_parseFile.  Parse error.");
            return NO;
        }
        if (done)
            break;
    }

    /* The file has been parsed, generate the triples */
    generate_triples(stream);

    /* Cleanup */
    HT_FREE(uri);
    fclose (fp);
    XML_ParserFree(xmlparser);
    HTRDF_delete(rdfparser);
    HT_FREE(stream);

    return YES;
}

/*	HTRDF_parseBuffer
**	---------------
**      This function parses a buffer of RDF in a synchronous, non-blocking
**      way.
**
**      Parameters:
**       buffer       the buffer to parse
**       buffer_name  the buffer's name.  This is used by the parser
**                    when naming "anonymous" subjects
**       buffer_len   the buffer's length (number of bytes)
**       new_triple_callback the callback that is invoked when a triple
**                    is created. If NULL, the default triple handler is
**                     invoked.
**        context - a void pointer to pass to the new_triple_callback.
**             Should set NULL if no data needs to be passed to the
**             callback.
**      Returns:
**       Returns YES if the buffer is successfully parsed; otherwise NO is
**       returned and an error message is logged. 
*/

PUBLIC BOOL HTRDF_parseBuffer (const char *buffer, const char *buffer_name, 
  int buffer_len, HTTripleCallback_new * new_triple_callback, void *context)
{
    XML_Parser xmlparser;
    HTRDF *rdfparser;
    HTStream * stream = NULL;
    char *uri;
    BOOL status;

    /* Sanity checks */
    if (!buffer) {
	HTTRACE(XML_TRACE, "HTRDF_parseBuffer.  buffer is NULL");
        return NO;
    }
    if (buffer_len <= 0) {
	HTTRACE(XML_TRACE, "HTRDF_parseBuffer.  buffer_len is <=0");
        return NO;
    }
    if (!buffer_name) {
	HTTRACE(XML_TRACE, "HTRDF_parseBuffer.  buffer_name is NULL");
        return NO;
    }

    status = initialize_parsers(&xmlparser, &rdfparser, &stream, &uri, 
			        new_triple_callback, context, buffer_name);
    if (!status)
        return NO;

    if (! XML_Parse(xmlparser, buffer, buffer_len, 1)) {
        fprintf(stderr, "Parse error at line %d:\n%s\n",
                XML_GetCurrentLineNumber(xmlparser),
                XML_ErrorString(XML_GetErrorCode(xmlparser)));
        HT_FREE(uri);
        XML_ParserFree(xmlparser);
        HTRDF_delete(rdfparser);
        HT_FREE(stream);
	HTTRACE(XML_TRACE, "HTRDF_parseBuffer.  Parse error.");
        return NO;
    }

    /* The buffer has been parsed, generate the triples */
    generate_triples(stream);

    /* Cleanup */
    HT_FREE(uri);
    XML_ParserFree(xmlparser);
    HTRDF_delete(rdfparser);
    HT_FREE(stream);

    return YES;
}
