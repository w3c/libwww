/*
**	@(#) $Id$
**	
**	W3C Webbot can be found at "http://www.w3.org/Robot/"
**	
**	Copyright © 1995-1998 World Wide Web Consortium, (Massachusetts
**	Institute of Technology, Institut National de Recherche en
**	Informatique et en Automatique, Keio University). All Rights
**	Reserved. This program is distributed under the W3C's Software
**	Intellectual Property License. This program is distributed in the hope
**	that it will be useful, but WITHOUT ANY WARRANTY; without even the
**	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
**	PURPOSE. See W3C License http://www.w3.org/Consortium/Legal/ for more
**	details.
**
**  Authors:
**	HFN		Henrik Frystyk Nielsen, (frystyk@w3.org)
**	BR		Bob Racko
**	JP		John Punin
**
**  History:
**	Dec 04 95	First version
**	Oct 1998	Split into separate files
*/

#include "HTRobMan.h"
#include "HTQueue.h"
#include "HTAncMan.h"

#define SHOW_QUIET(mr)		((mr) && !((mr)->flags & MR_QUIET))
#define SHOW_REAL_QUIET(mr)	((mr) && !((mr)->flags & MR_REAL_QUIET))

PRIVATE HTErrorMessage HTErrors[HTERR_ELEMENTS] = {HTERR_ENGLISH_INITIALIZER};

/*
**  Some sorting algorithms
*/
PRIVATE HTComparer HitSort, FormatSort, LastModifiedSort, TitleSort;

/*
**  Ths callbacks that we need from the libwww HTML parser
*/
PRIVATE HText_new	RHText_new;
PRIVATE HText_delete	RHText_delete;
PRIVATE HText_foundLink	RHText_foundLink;

/* ------------------------------------------------------------------------- */

/*	Create a "HyperDoc" object
**	--------------------------
**	A HyperDoc object contains information about whether we have already
**	started checking the anchor and the depth in our search
*/
PUBLIC HyperDoc * HyperDoc_new (Robot * mr,HTParentAnchor * anchor, int depth)
{
    HyperDoc * hd;
    if ((hd = (HyperDoc *) HT_CALLOC(1, sizeof(HyperDoc))) == NULL)
	HT_OUTOFMEM("HyperDoc_new");
    hd->depth = depth;
    hd->hits = 1;

    hd->code = NO_CODE;
    hd->index = ++mr->cindex;

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
PUBLIC BOOL HyperDoc_delete (HyperDoc * hd)
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
		HTPrint("\nAccessed %ld documents in %.2f seconds (%.2f requests pr sec)\n",
			total_docs, secs, reqprsec);

            HTNumToStr(mr->get_bytes, bytes, 50);
	    if (SHOW_REAL_QUIET(mr))
		HTPrint("\tDid a GET on %ld document(s) and downloaded %s bytes of document bodies (%2.1f bytes/sec)\n",
			mr->get_docs, bytes, loadfactor);

            HTNumToStr(mr->head_bytes, bytes, 50);
	    if (SHOW_REAL_QUIET(mr))
		HTPrint("\tDid a HEAD on %ld document(s) with a total of %s bytes\n",
			mr->head_docs, bytes);
	}
    }

    /* Create an array of existing anchors */
    if (total_docs > 1) {
	HTArray * array = HTAnchor_getArray(total_docs);
        if (array) {

	    /* Distributions */
	    if (mr->flags & MR_DISTRIBUTIONS) {
		if (SHOW_REAL_QUIET(mr)) HTPrint("\nDistributions:\n");
	    }

            /* Sort after hit counts */
            if (mr->hitfile) {
		if (SHOW_REAL_QUIET(mr))
		    HTPrint("\tLogged hit count distribution in file `%s\'\n",
			    mr->hitfile);
		calculate_hits(mr, array);
	    }

            /* Sort after link relations */
#ifdef HT_MYSQL
            if (mr->relfile || mr->sqllog) {
#else
            if (mr->relfile) {
#endif
		if (mr->relfile && SHOW_REAL_QUIET(mr))
		    HTPrint("\tLogged link relationship distribution in file `%s\'\n",
			    mr->relfile);
		calculate_linkRelations(mr, array);
	    }

            /* Sort after modified date */
            if (mr->lmfile) {
		if (SHOW_REAL_QUIET(mr))
		    HTPrint("\tLogged last modified distribution in file `%s\'\n",
			    mr->lmfile);
		calculate_lm(mr, array);
	    }

            /* Sort after title */
            if (mr->titlefile) {
		if (SHOW_REAL_QUIET(mr))
		    HTPrint("\tLogged title distribution in file `%s\'\n",
			    mr->titlefile);
		calculate_title(mr, array);
	    }

            /* Find mediatype distribution */
	    if (mr->mtfile) {
		HTList * mtdist = mediatype_distribution(array);
		if (mtdist) {
		    if (SHOW_REAL_QUIET(mr))
			HTPrint("\tLogged media type distribution in file `%s\'\n",
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
			HTPrint("\tLogged charset distribution in file `%s\'\n",
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

PRIVATE HTParentAnchor *
get_last_parent(HTParentAnchor *anchor)
{
  HTAnchor *anc;
  HTList *sources = anchor->sources;

  while((anc = (HTAnchor *) HTList_nextObject(sources)) != NULL)
    {
      HTParentAnchor *panchor = HTAnchor_parent(anc);
      return panchor;
    }
  return NULL;
}

PRIVATE HTLink *
HTLink_find_type(HTAnchor * src, HTAnchor * dest, char *linktype)
{
    if(src && dest && linktype)
    {
	HTLink * link = HTAnchor_mainLink(src);
	HTList * sublinks = HTAnchor_subLinks(src);
	HTLinkType type = (HTLinkType)HTAtom_caseFor(linktype);
	HTAnchor *sdest = HTLink_destination(link);
	if (link && sdest == dest && type == HTLink_type(link))
	    return link;
	else if (sublinks) {
	    while ((link = (HTLink *) HTList_nextObject (sublinks))) {
		sdest = HTLink_destination(link);
		if (sdest == dest && HTLink_type(link) == type) 
		    return link;

	    }
	}
    }
    return NULL;
}

PRIVATE void
update_incoming_links(HTParentAnchor *anchor, HTParentAnchor *nanchor)
{
    if(anchor && nanchor) {
	HTAnchor *anc;
	HTList *sources = anchor->sources;
	while((anc = (HTAnchor *) HTList_nextObject(sources)) != NULL) {
	    HTParentAnchor *panchor = HTAnchor_parent(anc);
	    if((HTLink_find((HTAnchor *)panchor,(HTAnchor *)anchor)) &&
	       (!HTLink_find_type((HTAnchor *)panchor,
				  (HTAnchor *)nanchor,"redirection"))) {
		HTLink_add((HTAnchor *)panchor,(HTAnchor *)nanchor, 
			   (HTLinkType) HTAtom_caseFor("redirection"), 	
			    METHOD_HEAD);
	    }
	}
    }
}	

PRIVATE void
update_hyperdoc(HyperDoc *hd,HTRequest *request)
{
    if(hd && request) {
	HTParentAnchor *anchor = hd->anchor;
	HTParentAnchor *nanchor = HTRequest_anchor(request);
	HTParentAnchor *parent = HTRequest_parent(request);
	HyperDoc *nhd = HTAnchor_document(nanchor);

	char *tit = (char *) HTAnchor_title(nanchor);

	if(nhd && tit)
	    StrAllocCopy(nhd->title,tit);

	if (anchor != nanchor) {
	    if(nhd) { 	    /* The redirected anchor has a Hyperdoc */
		if(nhd != hd) {
		    hd->code = REDIR_CODE;

		    HTAnchor_setDocument(anchor,(void *)nhd);

		    if(!HTLink_find_type((HTAnchor *)parent,
					 (HTAnchor *)nanchor,"redirection")) {
			HTLink_add((HTAnchor *)parent,(HTAnchor *)nanchor, 
				   (HTLinkType) HTAtom_caseFor("redirection"), 
				   METHOD_HEAD);
		    }
		}
	    } else { /* The redirected anchor does not have a Hyperdoc */
		hd->anchor = nanchor;
		HTAnchor_setDocument(nanchor,(void *) hd);

		if(!HTLink_find_type((HTAnchor *)parent,(HTAnchor *)nanchor,
				     "redirection")) {
		    HTLink_add((HTAnchor *)parent,(HTAnchor *)nanchor, 
			      (HTLinkType) HTAtom_caseFor("redirection") , 
			       METHOD_HEAD);
		}
	    }
	    update_incoming_links(anchor,nanchor);
	}
    }
}

PRIVATE void
set_error_state_hyperdoc(HyperDoc * hd, HTRequest *request)
{
    HTList * cur = HTRequest_error(request);
    HTError *pres;
    Finger * finger = (Finger *) HTRequest_context(request);
    Robot * mr = finger->robot;

    while((pres = (HTError *) HTList_nextObject(cur)) != NULL) {
	int code =HTErrors[HTError_index(pres)].code;

	hd->code = code;

	if((mr->flags & MR_REDIR) && code >= 200 && code < 300 )
	    update_hyperdoc(hd,request);
    }
}

#if 0
PRIVATE int
test_for_blank_spaces(char *uri)
{
  char *ptr = uri;
  for(;*ptr!='\0';ptr++)
    if(*ptr == ' ')
      return 1;
  return 0;
}
#endif

/*	Create a Command Line Object
**	----------------------------
*/
PUBLIC Robot * Robot_new (void)
{
    Robot * me;
    if ((me = (Robot *) HT_CALLOC(1, sizeof(Robot))) == NULL)
	HT_OUTOFMEM("Robot_new");
    me->hyperdoc = HTList_new();
    me->htext = HTList_new();
    me->timer = DEFAULT_TIMEOUT*MILLIES;
    me->waits = 0;
    me->cwd = HTGetCurrentDirectoryURL();
    me->output = OUTPUT;
    me->cnt = 0;
    me->ndoc = -1;
    me->fingers = HTList_new();
 
   /* This is new */
    me->queue = HTQueue_new();
    me->cq = 0;
    me->furl = NULL;

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
		RHText_delete(pres);
	    HTList_delete(mr->htext);
	}

	/* Close all the log files */
	if (mr->flags & MR_LOGGING) {
	    if (SHOW_REAL_QUIET(mr)) HTPrint("\nRaw Log files:\n");
	}

	if (mr->log) {
	    if (SHOW_REAL_QUIET(mr))
		HTPrint("\tLogged %5d entries in general log file `%s\'\n",
			HTLog_accessCount(mr->log), mr->logfile);
	    HTLog_close(mr->log);
	}
	if (mr->ref) {
	    if (SHOW_REAL_QUIET(mr))
		HTPrint("\tLogged %5d entries in referer log file `%s\'\n",
			HTLog_accessCount(mr->ref), mr->reffile);
	    HTLog_close(mr->ref);
	}
	if (mr->reject) {
	    if (SHOW_REAL_QUIET(mr))
		HTPrint("\tLogged %5d entries in rejected log file `%s\'\n",
			HTLog_accessCount(mr->reject), mr->rejectfile);
	    HTLog_close(mr->reject);
	}
	if (mr->notfound) {
	    if (SHOW_REAL_QUIET(mr))
		HTPrint("\tLogged %5d entries in not found log file `%s\'\n",
			HTLog_accessCount(mr->notfound), mr->notfoundfile);
	    HTLog_close(mr->notfound);
	}
	if (mr->conneg) {
	    if (SHOW_REAL_QUIET(mr))
		HTPrint("\tLogged %5d entries in content negotiation log file `%s\'\n",
			HTLog_accessCount(mr->conneg), mr->connegfile);
	    HTLog_close(mr->conneg);
	}
	if (mr->noalttag) {
	    if (SHOW_REAL_QUIET(mr))
		HTPrint("\tLogged %5d entries in missing alt tag log file `%s\'\n",
			HTLog_accessCount(mr->noalttag), mr->noalttagfile);
	    HTLog_close(mr->noalttag);
	}

	if (mr->output && mr->output != STDOUT) fclose(mr->output);

	if (mr->flags & MR_TIME) {
	    time_t local = time(NULL);
	    if (SHOW_REAL_QUIET(mr))
		HTPrint("\nRobot terminated %s\n", HTDateTimeStr(&local, YES));
	}

	/* This is new */
	HT_FREE(mr->cdepth);
	HT_FREE(mr->furl);

#ifdef HT_POSIX_REGEX
	if (mr->include) {
	    regfree(mr->include);
	    HT_FREE(mr->include);
	}
	if (mr->exclude) {
	    regfree(mr->exclude);
	    HT_FREE(mr->exclude);
	}
	if (mr->exc_robot) {
	    regfree(mr->exc_robot);
	    HT_FREE(mr->exc_robot);
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

	if (mr->queue) HTQueue_delete(mr->queue);
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
PUBLIC Finger * Finger_new (Robot * robot, HTParentAnchor * dest, HTMethod method)
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

    /* Done with one more */
    me->robot->cnt--;

    /* See if we don't need to keep all the metadata around in the anchors */
    if (!(me->robot->flags & MR_KEEP_META))
	HTAnchor_clearHeader(HTRequest_anchor(me->request));

    /*
    **  If we are down at one request then flush the output buffer
    */
    if (me->request) {
	if (me->robot->cnt == 1) HTRequest_forceFlush(me->request);
	HTRequest_delete(me->request);
	me->request = NULL;
    }

    /*
    **  Delete the request and free myself
    */
    HT_FREE(me);
    return YES;
}

PRIVATE BOOL check_constraints(Robot * mr, char *prefix, char *uri)
{
    BOOL match = YES;
    /* Check for prefix match */
    if (prefix) {
	match = HTStrMatch(prefix, uri) ? YES : NO;
    }
  
#ifdef HT_POSIX_REGEX
    /* Check for any regular expression */
    if (match && mr->include) {
	match = regexec(mr->include, uri, 0, NULL, 0) ? NO : YES;
    }
    if (match && mr->exc_robot) {
	match = regexec(mr->exc_robot, uri, 0, NULL, 0) ? YES : NO;
    }
    if (match && mr->exclude) {
	match = regexec(mr->exclude, uri, 0, NULL, 0) ? YES : NO;
    }
  
#endif
    return match;
}

/*
**  Cleanup and make sure we close all connections including the persistent
**  ones
*/
PUBLIC void Cleanup (Robot * me, int status)
{
    /*
    **  First we clean up the robot itself and calculate the various
    **  statistics. This can actually take some time as a lot of data
    **  has to be manipulated
    */
    Robot_delete(me);

    /*
    **  Then we shut down libwww
    */
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

PUBLIC regex_t * get_regtype (Robot * mr, const char * regex_str, int cflags)
{
    regex_t * regex = NULL;
    if (regex_str && *regex_str) {
	int status;
	if ((regex = (regex_t *) HT_CALLOC(1, sizeof(regex_t))) == NULL)
	    HT_OUTOFMEM("get_regtype");
	if ((status = regcomp(regex, regex_str, cflags))) {
	    char * err_msg = get_regerror(status, regex);
	    if (SHOW_REAL_QUIET(mr))
		HTPrint("Regular expression error: %s\n", err_msg);
	    HT_FREE(err_msg);
	    Cleanup(mr, -1);
	}
    }
    return regex;
}
#endif

PUBLIC void VersionInfo (void)
{
    HTPrint("\nW3C OpenSource Software");
    HTPrint("\n-----------------------\n\n");
    HTPrint("\tWebbot version %s\n", APP_VERSION);
    HTPrint("\tusing the W3C libwww library version %s.\n\n",HTLib_version());
    HTPrint("\tSee \"%s\" for help\n", COMMAND_LINE);
    HTPrint("\tSee \"http://www.w3.org/Robot/User/\" for user information\n");
    HTPrint("\tSee \"http://www.w3.org/Robot/\" for general information\n\n");
    HTPrint("\tPlease send feedback to the <www-lib@w3.org> mailing list,\n");
    HTPrint("\tsee \"http://www.w3.org/Library/#Forums\" for details\n\n");
}

/*	redirection_handler
**	-------------------
**	If we are set up to handle redirections then handle it here.
*/
PUBLIC int redirection_handler (HTRequest * request, HTResponse * response,
				void * param, int status) 
{
    Finger * finger = (Finger *) HTRequest_context(request);
    Robot * mr = finger->robot;
    HTParentAnchor * me = HTRequest_anchor(request);
    HTAnchor * redirection = HTResponse_redirection(response);
    HTParentAnchor * redirection_parent = HTAnchor_parent(redirection);
    HyperDoc * redirection_hd = HTAnchor_document(redirection_parent);
    char * uri = NULL;
    char * redirection_parent_addr = NULL;
    BOOL match = YES;
    BOOL check = NO;

    /* In case we didn't get any redirection destination */
    if (!redirection) return HT_OK;

    /* Get the addresses */
    uri = HTAnchor_address((HTAnchor *) me);
    redirection_parent_addr = HTAnchor_address((HTAnchor *) redirection_parent);
    if (SHOW_QUIET(mr))
	HTPrint("Robot....... Checking redirecting from `%s\' to `%s\'\n",
		uri, redirection_parent_addr);

    /* Log the event */
#ifdef HT_MYSQL
    if (mr->sqllog && redirection_parent_addr)
	HTSQLLog_addLinkRelationship(mr->sqllog, redirection_parent_addr,
				     uri, "redirection", NULL);
#endif

    /* Check our constraints matcher */
    match = check_constraints(mr,mr->prefix, redirection_parent_addr);

#ifdef HT_POSIX_REGEX
    /* See if we should do a HEAD or a GET on this URI */
    if (match && mr->check) {
	check = regexec(mr->check, uri, 0, NULL, 0) ? NO : YES;
    }
#endif

    /*
    ** If we already have a HyperDoc for the redirected anchor
    ** then update it
    */
    if (match) {
	if ((redirection_hd = HTAnchor_document(redirection_parent)) != NULL) {
	    if (SHOW_QUIET(mr)) HTPrint("............ Already checked\n");
	    redirection_hd->hits++;
	    HT_FREE(redirection_parent_addr);
	    HT_FREE(uri);
	    return HT_OK;
	}

	/* Now call the default libwww handler for actually carrying it out */
	if (mr->redir_code==0 || mr->redir_code==status) {
	    HyperDoc * me_hd = HTAnchor_document(me);
	    HyperDoc *nhd = HyperDoc_new(mr, redirection_parent, me_hd->depth);

	    if(mr->flags & MR_BFS) {
		nhd->method = METHOD_HEAD;
		HTQueue_append(mr->queue, (void *) nhd);
		(mr->cq)++;
	    }

	    if (check) {
		if (SHOW_QUIET(mr)) HTPrint("Checking redirection using HEAD\n");
		HTRequest_setMethod(request, METHOD_HEAD);
	    }
	    HT_FREE(redirection_parent_addr);
	    HT_FREE(uri);
	    return HTRedirectFilter(request, response, param, status);
	}
    } else {
	if (SHOW_QUIET(mr)) HTPrint("............ does not fulfill constraints\n");
#ifdef HT_MYSQL
	if (mr->reject || mr->sqllog)
#else	
	if (mr->reject)
#endif
	{
	    if (mr->reject && redirection_parent_addr)
		HTLog_addText(mr->reject, "%s --> %s\n", redirection_parent_addr, uri);
	}
    }

    /* Just fall through */
    HT_FREE(redirection_parent_addr);
    HT_FREE(uri);
    return HT_OK;
}

/*	terminate_handler
**	-----------------
**	This function is registered to handle the result of the request.
**	If no more requests are pending then terminate program
*/
PUBLIC int terminate_handler (HTRequest * request, HTResponse * response,
			       void * param, int status) 
{
    Finger * finger = (Finger *) HTRequest_context(request);
    Robot * mr = finger->robot;
    if (SHOW_QUIET(mr)) HTPrint("Robot....... done with %s\n", HTAnchor_physical(finger->dest));

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

    if (!(mr->flags & MR_BFS)) {

#if 0
        HyperDoc * hd = HTAnchor_document(finger->dest);
	if (hd) set_error_state_hyperdoc(hd,request);
#endif

	/* Delete this thread */
	Finger_delete(finger);

	/* Should we stop? */
	if (mr->cnt <= 0) {
	    if (SHOW_QUIET(mr)) HTPrint("             Everything is finished...\n");
	    Cleanup(mr, 0);			/* No way back from here */
	}
    }

    if (SHOW_QUIET(mr)) HTPrint("             %d outstanding request%s\n", mr->cnt, mr->cnt == 1 ? "" : "s");
    return HT_OK;

}

PUBLIC int bfs_terminate_handler (HTRequest * request, HTResponse * response,
				  void * param, int status) 
{
    Finger * finger = (Finger *) HTRequest_context(request);
    Robot * mr = finger->robot;
    HTParentAnchor * dest = finger->dest;
    HyperDoc * hd = HTAnchor_document(dest);
    int depth = (hd ? hd->depth : -1);

    if (hd) set_error_state_hyperdoc(hd,request);
      
    if(hd && (HTRequest_method(request)== METHOD_HEAD) && 
       (depth < mr->depth))
      {
	hd->method = METHOD_GET;
	HTQueue_append(mr->queue, (void *)hd); (mr->cq)++;
      }

    Finger_delete(finger);

    if(!(mr->flags & MR_PREEMPTIVE))
      Serving_queue(mr);

    return HT_OK;
}

PUBLIC void Serving_queue(Robot *mr)
{
  BOOL abort = NO;
  Finger *nfinger;
  
  while(!abort)
    {
      if(!HTQueue_isEmpty(mr->queue))
	{
	  HTRequest *newreq;
	  
	  HyperDoc *nhd = (HyperDoc *)HTQueue_headOfQueue(mr->queue);
	  
	  if(nhd)
	    {
	      char *uri = HTAnchor_address((HTAnchor *)nhd->anchor);
	      HTQueue_dequeue(mr->queue); (mr->cq)--;

	      nfinger = Finger_new(mr, nhd->anchor, nhd->method); 
	      
	      newreq = nfinger->request;

	      if(SHOW_QUIET(mr))  HTPrint("Request from QUEUE  %s\n",uri);
	      HT_FREE(uri);
	      if(SHOW_QUIET(mr)) HTPrint("%d elements in queue \n", mr->cq);

	      HTRequest_setParent(newreq,get_last_parent(nhd->anchor));

	      /* @@@ Should be done using a timer and not sleep! @@@ */
#if 0
	      if(mr->waits)
		  sleep(mr->waits);
#endif
	      
	      if (HTLoadAnchor((HTAnchor *)nhd->anchor , newreq) != YES) 
		{
		  if (SHOW_QUIET(mr)) HTPrint("not tested!\n");
		  Finger_delete(nfinger);
		}
	    }
	  else
	    abort = YES;
	}
      else
	abort = YES;
    }

  if(SHOW_QUIET(mr)) HTPrint("Queue size: %d \n", mr->cq);

    if (mr->cnt <= 0 || (abort && (mr->flags & MR_PREEMPTIVE)))
      {
	if(mr->cnt > 0)
	  if(SHOW_QUIET(mr)) HTPrint("%d requests were not served\n", mr->cnt);

	if (SHOW_QUIET(mr)) HTPrint("             Everything is finished...\n");
	Cleanup(mr, 0);			/* No way back from here */
      }
}

/* ------------------------------------------------------------------------- */
/*				HTEXT INTERFACE				     */
/* ------------------------------------------------------------------------- */

PUBLIC BOOL Robot_registerHTMLParser (void)
{
    HText_registerCDCallback(RHText_new, RHText_delete);
    HText_registerLinkCallback(RHText_foundLink);
    return YES;
}

PRIVATE HText * RHText_new (HTRequest * request, HTParentAnchor * anchor,
			    HTStream * stream)
{
    HText * me;
    Finger * finger = (Finger *) HTRequest_context(request);
    Robot * mr = finger->robot;
    char * robots = NULL;

    if ((me = (HText *) HT_CALLOC(1, sizeof(HText))) == NULL)
	HT_OUTOFMEM("RHText_new");

    /* Bind the HText object together with the Request Object */
    me->request = request;
    me->follow = YES;

    /* Check to see if we have any meta tags */
    if (!(mr->flags & MR_NOMETATAGS) && (robots = HTAnchor_robots(anchor)) != NULL) {
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

PRIVATE BOOL RHText_delete (HText * me) {
    if (me) {
	HT_FREE(me);
	return YES;
    }
    return NO;
}

PRIVATE void RHText_foundAnchor (HText * text, HTChildAnchor * anchor)
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

	/* These are new variables */
	HyperDoc * nhd = NULL;
	BOOL follow = YES;

	/* These three variables were moved */
	/*HTParentAnchor * last_anchor = HTRequest_parent(text->request);*/
	HTParentAnchor * last_anchor = HTRequest_anchor(text->request);
	HyperDoc * last_doc = HTAnchor_document(last_anchor);
	int depth = last_doc ? last_doc->depth+1 : 0;

	if (!uri) return;
	if (SHOW_QUIET(mr)) HTPrint("Robot....... Found `%s\' - \n", uri ? uri : "NULL\n");

        if (hd) {
	    if (SHOW_QUIET(mr)) HTPrint("............ Already checked\n");
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

	/* Check our constraints matcher */
	match = check_constraints(mr,mr->prefix, uri);

#ifdef HT_POSIX_REGEX
	/* See if we should do a HEAD or a GET on this URI */
        if (match && mr->check) {
            check = regexec(mr->check, uri, 0, NULL, 0) ? NO : YES;
	}
#endif

#if 0
	/* This is already checked in HTParse.c */
	if(uri && test_for_blank_spaces(uri))
	  follow = NO;
	else
#endif
	if (mr->ndoc == 0) /* Number of Documents is reached */
	  follow = NO;

	/* Test whether we already have a hyperdoc for this document */
	if (!hd && dest_parent) {
	    nhd = HyperDoc_new(mr, dest_parent, depth);
	    mr->cdepth[depth]++;
	}

	/* Test whether we already have a hyperdoc for this document */
        if (mr->flags & MR_LINK && match && dest_parent && follow && !hd) {
	    if (mr->flags & MR_BFS) {
		nhd->method = METHOD_HEAD;
		HTQueue_enqueue(mr->queue, (void *) nhd);
		(mr->cq)++;
		if(mr->ndoc > 0) mr->ndoc--;
	    } else {
		Finger * newfinger = Finger_new(mr, dest_parent, METHOD_GET);
		HTRequest * newreq = newfinger->request;
		HTRequest_setParent(newreq, referer);		
		nhd->method = METHOD_GET;

		if (check || depth >= mr->depth) {
		    if (SHOW_QUIET(mr)) HTPrint("loading at depth %d using HEAD\n", depth);
		    HTRequest_setMethod(newreq, METHOD_HEAD);
		    nhd->method = METHOD_HEAD;    

		} else {
		    if (SHOW_QUIET(mr)) HTPrint("loading at depth %d\n", depth);
		}
		if (HTLoadAnchor((HTAnchor *) dest_parent, newreq) != YES) {
		    if (SHOW_QUIET(mr)) HTPrint("not tested!\n");
		    Finger_delete(newfinger);
		}
	    }

	} else {
	    if (SHOW_QUIET(mr)) HTPrint("............ does not fulfill constraints\n");
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

PRIVATE void RHText_foundImage (HText * text, HTChildAnchor * anchor,
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

	    if (!uri) return;
	    if (hd) {
		if (SHOW_QUIET(mr)) HTPrint("............ Already checked\n");
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

	    /* Check our constraints matcher */
	    match = check_constraints(mr, mr->img_prefix, uri);

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
		
		if (SHOW_QUIET(mr)) HTPrint("Robot....... Checking Image `%s\'\n", uri);
		if (HTLoadAnchor((HTAnchor *) dest, newreq) != YES) {
		    if (SHOW_QUIET(mr)) HTPrint("Robot....... Image not tested!\n");
		    Finger_delete(newfinger);
		}
	    } else {
		if (SHOW_QUIET(mr)) HTPrint("............ does not fulfill constraints\n");
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

PRIVATE void RHText_foundLink (HText * text,
			       int element_number, int attribute_number,
			       HTChildAnchor * anchor,
			       const BOOL * present, const char ** value)
{
    if (text && anchor) {
	Finger * finger = (Finger *) HTRequest_context(text->request);
	Robot * mr = finger->robot;
	if (SHOW_QUIET(mr))
	    HTPrint("Robot....... Received element %d, attribute %d with anchor %p\n",
		    element_number, attribute_number, anchor);
	if ((element_number==HTML_IMG && attribute_number==HTML_IMG_SRC) || 
	    (element_number==HTML_BODY && attribute_number==HTML_BODY_BACKGROUND) ||
	    (element_number==HTML_INPUT && attribute_number==HTML_INPUT_SRC))
	    RHText_foundImage(text, anchor, NULL, NULL, NO);
	else
	    RHText_foundAnchor(text, anchor);
    }
}

PUBLIC char * get_robots_txt(char * uri)
{
    char *str = NULL;
    HTChunk * chunk;
    HTParentAnchor *anchor = HTAnchor_parent(HTAnchor_findAddress(uri));
    HTRequest *request = HTRequest_new();
    HTRequest_setOutputFormat(request, WWW_SOURCE);
    HTRequest_setPreemptive(request, YES);
    HTRequest_setMethod(request, METHOD_GET);
    chunk = HTLoadAnchorToChunk ((HTAnchor *)anchor, request);
    str = HTChunk_toCString(chunk);
    HTRequest_delete(request);
    return str;
}


