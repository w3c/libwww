/*
**	ACCESS MANAGER
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
** Authors
**	TBL	Tim Berners-Lee timbl@w3.org
**	JFG	Jean-Francois Groff jfg@dxcern.cern.ch
**	DD	Denis DeLaRoca (310) 825-4580  <CSP1DWD@mvs.oac.ucla.edu>
**	HFN	Henrik Frystyk, frystyk@w3.org
**      JK      Jose Kahan, kahan@w3.org
** History
**       8 Jun 92 Telnet hopping prohibited as telnet is not secure TBL
**	26 Jun 92 When over DECnet, suppressed FTP, Gopher and News. JFG
**	 6 Oct 92 Moved HTClientHost and HTlogfile into here. TBL
**	17 Dec 92 Tn3270 added, bug fix. DD
**	 4 Feb 93 Access registration, Search escapes bad chars TBL
**		  PARAMETERS TO HTSEARCH AND HTLOADRELATIVE CHANGED
**	28 May 93 WAIS gateway explicit if no WAIS library linked in.
**	   Dec 93 Bug change around, more reentrant, etc
**	09 May 94 logfile renamed to HTlogfile to avoid clash with WAIS
**	 8 Jul 94 Insulate HT_FREE();
**	   Sep 95 Rewritten, HFN
**      21 Jun 00 Added a Cache-Control: no-cache when doing PUT, as some
**                proxies do cache PUT requests, JK
*/

/* Library include files */
#include "WWWUtil.h"
#include "WWWCore.h"
#include "WWWStream.h"
#include "HTProxy.h"
#include "HTRules.h"
#include "HTReqMan.h"
#include "HTAccess.h"					 /* Implemented here */

#define PUTBLOCK(b, l)	(*target->isa->put_block)(target, b, l)

struct _HTStream {
    HTStreamClass * isa;
};

typedef enum _HTPutState {
    HT_LOAD_SOURCE	= 0,
    HT_SAVE_DEST,
    HT_ABORT_SAVE
} HTPutState;

typedef struct _HTPutContext {
    HTParentAnchor *	source;
    HTAnchor *		destination;
    HTChunk *		document;
    HTFormat		format;
    HTStream *		target;		       /* Any existing output stream */
    void *		placeholder;	       /* Any existing doc in anchor */
    HTPutState		state;
} HTPutContext;

/* --------------------------------------------------------------------------*/
/*				THE GET METHOD 				     */
/* --------------------------------------------------------------------------*/

/*	Request a document
**	-----------------
**	Private version that requests a document from the request manager
**	Returns YES if request accepted, else NO
*/
PRIVATE BOOL launch_request (HTRequest * request, BOOL recursive)
{
#ifdef HTDEBUG
    if (PROT_TRACE) {
	HTParentAnchor *anchor = HTRequest_anchor(request);
	char * full_address = HTAnchor_address((HTAnchor *) anchor);
	HTTRACE(PROT_TRACE, "HTAccess.... Accessing document %s\n" _ full_address);
	HT_FREE(full_address);
    }
#endif /* HTDEBUG */
    return HTLoad(request, recursive);
}

/*	Request a document from absolute name
**	-------------------------------------
**	Request a document referencd by an absolute URL.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTLoadAbsolute (const char * url, HTRequest * request)
{
    if (url && request) {
	HTAnchor * anchor = HTAnchor_findAddress(url);
	HTRequest_setAnchor(request, anchor);
	return launch_request(request, NO);
    }
    return NO;
}

/*	Request a document from relative name
**	-------------------------------------
**	Request a document referenced by a relative URL. The relative URL is 
**	made absolute by resolving it relative to the address of the 'base' 
**	anchor.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTLoadRelative (const char * 	relative,
			    HTParentAnchor *	base,
			    HTRequest *		request)
{
    BOOL status = NO;
    if (relative && base && request) {
	char * full_url = NULL;
	char * base_url = HTAnchor_address((HTAnchor *) base);
	full_url = HTParse(relative, base_url,
			 PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
	status = HTLoadAbsolute(full_url, request);
	HT_FREE(full_url);
	HT_FREE(base_url);
    }
    return status;
}

/*	Request a document from absolute name to stream
**	-----------------------------------------------
**	Request a document referencd by an absolute URL and sending the data
**	down a stream.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTLoadToStream (const char * url, HTStream * output,
			    HTRequest * request)
{
    if (url && output && request) {
	HTRequest_setOutputStream(request, output);
	return HTLoadAbsolute(url, request);
    }
    return NO;
}

/*	Load a document and save it ASIS in a local file
**	------------------------------------------------
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTLoadToFile (const char * url, HTRequest * request,
			  const char * filename)
{
    if (url && filename && request) {
	FILE * fp = NULL;
	
	/* Check if file exists. If so then ask user if we can replace it */
	if (access(filename, F_OK) != -1) {
	    HTAlertCallback * prompt = HTAlert_find(HT_A_CONFIRM);
	    if (prompt) {
		if ((*prompt)(request, HT_A_CONFIRM, HT_MSG_FILE_REPLACE, NULL,
			      NULL, NULL) != YES)
		    return NO;
	    }
	}

	/* If replace then open the file */
	if ((fp = fopen(filename, "wb")) == NULL) {
	    HTRequest_addError(request, ERR_FATAL, NO, HTERR_NO_FILE, 
			       (char *) filename, strlen(filename),
			       "HTLoadToFile"); 
	    return NO;
	}

	/* Set the output stream and start the request */
	HTRequest_setOutputFormat(request, WWW_SOURCE);
	HTRequest_setOutputStream(request, HTFWriter_new(request, fp, NO));
	if (HTLoadAbsolute(url, request) == NO) {
	    fclose(fp);
	    return NO;
	} else
	    return YES;
    }
    return NO;
}

/*
**	Load a URL to a mem buffer
**	--------------------------
**	Load a request and store the result in a memory buffer.
**	Returns chunk if OK - else NULL
*/
PUBLIC HTChunk * HTLoadToChunk (const char * url, HTRequest * request)
{
    if (url && request) {
	HTChunk * chunk = NULL;
	HTStream * target = HTStreamToChunk(request, &chunk, 0);
	HTAnchor * anchor = HTAnchor_findAddress(url);
	HTRequest_setAnchor(request, anchor);
	HTRequest_setOutputStream(request, target);
	if (launch_request(request, NO) == YES)
	    return chunk;
	else {
	    HTChunk_delete(chunk);
	    return NULL;
	}
    }
    return NULL;
}

/*	Request an anchor
**	-----------------
**	Request the document referenced by the anchor
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTLoadAnchor (HTAnchor * anchor, HTRequest * request)
{
    if (anchor && request) {
	HTRequest_setAnchor(request, anchor);
	return launch_request(request, NO);
    }
    return NO;
}

/*	Request an anchor
**	-----------------
**	Same as HTLoadAnchor but any information in the Error Stack in the 
**	request object is kept, so that any error messages in one 
**	This function is almost identical to HTLoadAnchor, but it doesn't
**	clear the error stack so that the information in there is kept.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTLoadAnchorRecursive (HTAnchor * anchor, HTRequest * request)
{
    if (anchor && request) {
	HTRequest_setAnchor(request, anchor);
        return launch_request(request, YES);
    }
    return NO;
}

/*
**	Load a URL to a mem buffer
**	--------------------------
**	Load a request and store the result in a memory buffer.
**	Returns chunk if OK - else NULL
*/
PUBLIC HTChunk * HTLoadAnchorToChunk (HTAnchor * anchor, HTRequest * request)
{
    HTChunk * chunk = NULL;
    if (anchor && request) {
	HTStream * target = HTStreamToChunk(request, &chunk, 0);
	HTRequest_setAnchor(request, anchor);
	HTRequest_setOutputStream(request, target);
	if (launch_request(request, NO) == YES)
	    return chunk;
	else {
	    HTChunk_delete(chunk);
	    return NULL;
	}
    }
    return NULL;
}

/*
**	Load a Rule File
**	----------------
**	Load a rule find with the URL specified and add the set of rules to
**	the existing set.
*/
PUBLIC BOOL HTLoadRules (const char * url)
{
    BOOL status = NO;
    if (url) {
	HTList * list = HTList_new();
	HTRequest * request = HTRequest_new();
	HTRequest_setPreemptive(request, YES);

	/*
	**  We do only accept a new rules files when we are in interactive
	**  mode and can ask the user for it. If HT_AUTOMATIC_RULES is 
	**  defined then we accept new rules files without explicit ack
	**  from the user
	*/
#ifdef HT_AUTOMATIC_RULES
	HTAlert_setInteractive(NO);
#endif

	/*
	**  Add the rules parsing stream for this particular request only.
	**  That is, we only accept a rules file when we have explicitly
	**  asked for it using this function.
	*/
	HTConversion_add(list, "application/x-www-rules", "*/*", HTRules,
			 1.0, 0.0, 0.0);
	HTRequest_setConversion(request, list, YES);
	status = HTLoadAbsolute(url, request);
	HTConversion_deleteAll(list);
	HTRequest_delete(request);
    }
    return status;
}

/*
**	Load a Rule File without asking the user 
**	----------------------------------------
**	Load a rule find with the URL specified and add the set of rules to
**	the existing set. We don't ask the user as she would have to call this
**	method explicitly anyway.
*/
PUBLIC BOOL HTLoadRulesAutomatically (const char * url)
{
    BOOL status = NO;
    if (url) {
	HTList * list = HTList_new();
	HTRequest * request = HTRequest_new();

	/*
	**  Stop all other loads and concentrate on this one
	*/
	HTRequest_setPreemptive(request, YES);

	/*
	**  Add the rules parsing stream for this particular request only.
	*/
	HTConversion_add(list, "application/x-www-rules", "*/*",
			 HTRules_parseAutomatically,
			 1.0, 0.0, 0.0);

	HTRequest_setConversion(request, list, YES);
	status = HTLoadAbsolute(url, request);
	HTConversion_deleteAll(list);
	HTRequest_delete(request);
    }
    return status;
}

/* --------------------------------------------------------------------------*/
/*			 GET WITH KEYWORDS (SEARCH)			     */
/* --------------------------------------------------------------------------*/

/*
**	This function creates a URL with a searh part as defined by RFC 1866
**	Both the baseurl and the keywords must be escaped.
**
**	1. The form field names and values are escaped: space
**	characters are replaced by `+', and then reserved characters
**	are escaped as per [URL]; that is, non-alphanumeric
**	characters are replaced by `%HH', a percent sign and two
**	hexadecimal digits representing the ASCII code of the
**	character. Line breaks, as in multi-line text field values,
**	are represented as CR LF pairs, i.e. `%0D%0A'.
**
**	2. The fields are listed in the order they appear in the
**	document with the name separated from the value by `=' and
**	the pairs separated from each other by `&'. Fields with null
**	values may be omitted. In particular, unselected radio
**	buttons and checkboxes should not appear in the encoded
**	data, but hidden fields with VALUE attributes present
**	should.
**
**	    NOTE - The URI from a query form submission can be
**	    used in a normal anchor style hyperlink.
**	    Unfortunately, the use of the `&' character to
**	    separate form fields interacts with its use in SGML
**	    attribute values as an entity reference delimiter.
**	    For example, the URI `http://host/?x=1&y=2' must be
**	    written `<a href="http://host/?x=1&#38;y=2"' or `<a
**	    href="http://host/?x=1&amp;y=2">'.
**
**	    HTTP server implementors, and in particular, CGI
**	    implementors are encouraged to support the use of
**	    `;' in place of `&' to save users the trouble of
**	    escaping `&' characters this way.
*/
PRIVATE char * query_url_encode (const char * baseurl, HTChunk * keywords)
{
    char * fullurl = NULL;
    if (baseurl && keywords && HTChunk_size(keywords)) {
	int len = strlen(baseurl);
	fullurl = (char *) HT_MALLOC(len + HTChunk_size(keywords) + 2);
	sprintf(fullurl, "%s?%s", baseurl, HTChunk_data(keywords));
	{
	    char * ptr = fullurl+len;
	    while (*ptr) {
		if (*ptr == ' ') *ptr = '+';
		ptr++;
	    }
	}
    }
    return fullurl;
}

PRIVATE char * form_url_encode (const char * baseurl, HTAssocList * formdata)
{
    if (formdata) {
	BOOL first = YES;
	int cnt = HTList_count((HTList *) formdata);
	HTChunk * fullurl = HTChunk_new(128);
	HTAssoc * pres;
	if (baseurl) {
	    HTChunk_puts(fullurl, baseurl);
	    HTChunk_putc(fullurl, '?');
	}
	while (cnt > 0) {
	    pres = (HTAssoc *) HTList_objectAt((HTList *) formdata, --cnt);
	    if (first)
		first = NO;
	    else
		HTChunk_putc(fullurl, '&');	    /* Could use ';' instead */
	    HTChunk_puts(fullurl, HTAssoc_name(pres));
	    HTChunk_putc(fullurl, '=');
	    HTChunk_puts(fullurl, HTAssoc_value(pres));
	}
	return HTChunk_toCString(fullurl);
    }
    return NULL;
}

/*	Search a document from absolute name
**	------------------------------------
**	Request a document referencd by an absolute URL appended with the
**	keywords given. The URL can NOT contain any fragment identifier!
**	The list of keywords must be a space-separated list and spaces will
**	be converted to '+' before the request is issued.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTSearchAbsolute (HTChunk *		keywords,
			      const char *	base,
			      HTRequest *	request)
{
    if (keywords && base && request) {
	char * full = query_url_encode(base, keywords);
	if (full) {
	    HTAnchor * anchor = HTAnchor_findAddress(full);
	    HTRequest_setAnchor(request, anchor);
	    HT_FREE(full);
	    return launch_request(request, NO);
	}
    }
    return NO;
}

/*	Search a document from relative name
**	-------------------------------------
**	Request a document referenced by a relative URL. The relative URL is 
**	made absolute by resolving it relative to the address of the 'base' 
**	anchor.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTSearchRelative (HTChunk * 	keywords,
			      const char * 	relative,
			      HTParentAnchor *	base,
			      HTRequest *	request)
{
    BOOL status = NO;
    if (keywords && relative && base && request) {
	char * full_url = NULL;
	char * base_url = HTAnchor_address((HTAnchor *) base);
	full_url = HTParse(relative, base_url,
			 PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
	status = HTSearchAbsolute(keywords, full_url, request);
	HT_FREE(full_url);
	HT_FREE(base_url);
    }
    return status;
}

/*
**	Search a string
**	---------------
**	This is the same as HTSearchAbsolute but instead of using a chunk
**	you can pass a string.
*/
PUBLIC BOOL HTSearchString (const char *	keywords,
			    HTAnchor *		anchor,
			    HTRequest *		request)
{
    BOOL status = NO;
    if (keywords && anchor && request) {	
	char * base_url = HTAnchor_address((HTAnchor *) anchor);
	HTChunk * chunk = HTChunk_new(strlen(keywords)+2);
	HTChunk_puts(chunk, keywords);
	status = HTSearchAbsolute(chunk, base_url, request);	
	HT_FREE(base_url);
	HTChunk_delete(chunk);
    }
    return status;
}	

/*	Search an Anchor
**	----------------
**	Performs a keyword search on word given by the user. Adds the keyword
**	to the end of the current address and attempts to open the new address.
**	The list of keywords must be a space-separated list and spaces will
**	be converted to '+' before the request is issued.
**	Search can also be performed by HTLoadAbsolute() etc.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTSearchAnchor (HTChunk *		keywords,
			    HTAnchor *		anchor,
			    HTRequest * 	request)
{
    BOOL status = NO;
    if (keywords && anchor && request) {
	char * base_url = HTAnchor_address(anchor);
	status = HTSearchAbsolute(keywords, base_url, request);	
	HT_FREE(base_url);
    }
    return status;
}

/* --------------------------------------------------------------------------*/
/*			 HANDLING FORMS USING GET AND POST		     */
/* --------------------------------------------------------------------------*/

/*	Send a Form request using GET from absolute name
**	------------------------------------------------
**	Request a document referencd by an absolute URL appended with the
**	formdata given. The URL can NOT contain any fragment identifier!
**	The list of form data must be given as an association list where 
**	the name is the field name and the value is the value of the field.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTGetFormAbsolute (HTAssocList *	formdata,
			       const char *	base,
			       HTRequest *	request)
{
    if (formdata && base && request) {
	char * full = form_url_encode(base, formdata);
	if (full) {
	    HTAnchor * anchor = HTAnchor_findAddress(full);
	    HTRequest_setAnchor(request, anchor);
	    HT_FREE(full);
	    return launch_request(request, NO);
	}
    }
    return NO;
}

/*	Send a Form request using GET from relative name
**	------------------------------------------------
**	Request a document referencd by a relative URL appended with the
**	formdata given. The URL can NOT contain any fragment identifier!
**	The list of form data must be given as an association list where 
**	the name is the field name and the value is the value of the field.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTGetFormRelative (HTAssocList * 	formdata,
			       const char * 	relative,
			       HTParentAnchor *	base,
			       HTRequest *	request)
{
    BOOL status = NO;
    if (formdata && relative && base && request) {
	char * full_url = NULL;
	char * base_url = HTAnchor_address((HTAnchor *) base);
	full_url=HTParse(relative, base_url,
			 PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
	status = HTGetFormAbsolute(formdata, full_url, request);
	HT_FREE(full_url);
	HT_FREE(base_url);
    }
    return status;
}

/*	Send a Form request using GET from an anchor
**	--------------------------------------------
**	Request a document referencd by an anchor object appended with the
**	formdata given. The URL can NOT contain any fragment identifier!
**	The list of form data must be given as an association list where 
**	the name is the field name and the value is the value of the field.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTGetFormAnchor (HTAssocList *	formdata,
			     HTAnchor *		anchor,
			     HTRequest * 	request)
{
    BOOL status = NO;
    if (formdata && anchor && request) {
	char * base_url = HTAnchor_address((HTAnchor *) anchor);
	status = HTGetFormAbsolute(formdata, base_url, request);	
	HT_FREE(base_url);
    }
    return status;
}

PRIVATE int HTEntity_callback (HTRequest * request, HTStream * target)
{
    HTParentAnchor * entity = HTRequest_entityAnchor(request);
    HTTRACE(APP_TRACE, "Posting Data from callback function\n");
    if (!request || !entity || !target) return HT_ERROR;
    {
	BOOL chunking = NO;
	int status;
	char * document = (char *) HTAnchor_document(entity);
	int len = HTAnchor_length(entity);
	if (!document) {
	    HTTRACE(PROT_TRACE, "Posting Data No document\n");
	    return HT_ERROR;
	}

	/*
	** If the length is unknown (-1) then see if the document is a text
	** type and in that case take the strlen. If not then we don't know
	** how much data we can write and must stop
	*/
	if (len < 0) {
	    HTFormat actual = HTAnchor_format(entity);
	    HTFormat tmplate = HTAtom_for("text/*");
	    if (HTMIMEMatch(tmplate, actual)) {
		len = strlen(document);			/* Naive! */
		chunking = YES;
	    } else {
		HTTRACE(PROT_TRACE, "Posting Data Must know the length of document %p\n" _ 
			    document);
		return HT_ERROR;
	    }
	}

	/* Send the data down the pipe */
	status = (*target->isa->put_block)(target, document, len);
	if (status == HT_WOULD_BLOCK) {
	    HTTRACE(PROT_TRACE, "Posting Data Target WOULD BLOCK\n");
	    return HT_WOULD_BLOCK;
	} else if (status == HT_PAUSE) {
	    HTTRACE(PROT_TRACE, "Posting Data Target PAUSED\n");
	    return HT_PAUSE;
	} else if (chunking && status == HT_OK) {
	    HTTRACE(PROT_TRACE, "Posting Data Target is SAVED using chunked\n");
	    return (*target->isa->put_block)(target, "", 0);
	} else if (status == HT_LOADED || status == HT_OK) {
	    HTTRACE(PROT_TRACE, "Posting Data Target is SAVED\n");
	    (*target->isa->flush)(target);
	    return HT_LOADED;
        } else if (status > 0) {	      /* Stream specific return code */
	    HTTRACE(PROT_TRACE, "Posting Data. Target returns %d\n" _ status);
	    return status;
	} else {				     /* we have a real error */
	    HTTRACE(PROT_TRACE, "Posting Data Target ERROR %d\n" _ status);
	    return status;
	}
    }
}

/*	Send a Form request using POST from absolute name
**	-------------------------------------------------
**	Request a document referencd by an absolute URL appended with the
**	formdata given. The URL can NOT contain any fragment identifier!
**	The list of form data must be given as an association list where 
**	the name is the field name and the value is the value of the field.
*/
PUBLIC HTParentAnchor * HTPostFormAbsolute (HTAssocList *	formdata,
					    const char *	base,
					    HTRequest *		request)
{
    if (formdata && base && request) {
	HTAnchor * anchor = HTAnchor_findAddress(base);
	return HTPostFormAnchor(formdata, anchor, request);
    }
    return NULL;
}

/*	Send a Form request using POST from relative name
**	-------------------------------------------------
**	Request a document referencd by a relative URL appended with the
**	formdata given. The URL can NOT contain any fragment identifier!
**	The list of form data must be given as an association list where 
**	the name is the field name and the value is the value of the field.
*/
PUBLIC HTParentAnchor * HTPostFormRelative (HTAssocList * 	formdata,
					    const char * 	relative,
					    HTParentAnchor *	base,
					    HTRequest *		request)
{
    HTParentAnchor * postanchor = NULL;
    if (formdata && relative && base && request) {
	char * full_url = NULL;
	char * base_url = HTAnchor_address((HTAnchor *) base);
	full_url=HTParse(relative, base_url,
			 PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
	postanchor = HTPostFormAbsolute(formdata, full_url, request);
	HT_FREE(full_url);
	HT_FREE(base_url);
    }
    return postanchor;
}

/*	Send a Form request using POST from an anchor
**	---------------------------------------------
**	Request a document referencd by an anchor object appended with the
**	formdata given. The URL can NOT contain any fragment identifier!
**	The list of form data must be given as an association list where 
**	the name is the field name and the value is the value of the field.
*/
PUBLIC HTParentAnchor * HTPostFormAnchor (HTAssocList *	formdata,
					  HTAnchor *	anchor,
					  HTRequest * 	request)
{
    HTParentAnchor * postanchor = NULL;
    if (formdata && anchor && request) {
	HTUserProfile * up = HTRequest_userProfile(request);
	char * tmpfile = HTGetTmpFileName(HTUserProfile_tmp(up));
	char * tmpurl = HTParse(tmpfile, "file:", PARSE_ALL);
	char * form_encoded = form_url_encode(NULL, formdata);
	if (form_encoded) {

	    /*
	    **  Now create a new anchor for the post data and set up
	    **  the rest of the metainformation we know about this anchor. The
	    **  tmp anchor may actually already exist from previous postings.
	    */
	    postanchor = (HTParentAnchor *) HTAnchor_findAddress(tmpurl);
	    HTAnchor_clearHeader(postanchor);
	    HTAnchor_setDocument(postanchor, form_encoded);
	    HTAnchor_setLength(postanchor, strlen(form_encoded));
	    HTAnchor_setFormat(postanchor, WWW_FORM);

	    /*
	    **  Bind the temporary anchor to the anchor that will contain the
	    **  response 
	    */
	    HTLink_removeAll((HTAnchor *) postanchor);
	    HTLink_add((HTAnchor *) postanchor, (HTAnchor *) anchor, 
		       NULL, METHOD_POST);

	    /* Set up the request object */
	    HTRequest_addGnHd(request, HT_G_DATE);	 /* Send date header */
	    HTRequest_setAnchor(request, anchor);
	    HTRequest_setEntityAnchor(request, postanchor);
	    HTRequest_setMethod(request, METHOD_POST);

	    /* Add the post form callback function to provide the form data */
	    HTRequest_setPostCallback(request, HTEntity_callback);

	    /* Now start the load normally */
	    launch_request(request, NO);
	}
	HT_FREE(tmpfile);
	HT_FREE(tmpurl);
    }
    return postanchor;
}

/*
**	POST a URL and save the response in a mem buffer
**	------------------------------------------------
**	Returns chunk if OK - else NULL
*/
PUBLIC HTChunk * HTPostFormAnchorToChunk (HTAssocList * formdata,
					  HTAnchor *	anchor,
					  HTRequest *	request)
{
    if (formdata && anchor && request) {
	HTChunk * chunk = NULL;
	HTStream * target = HTStreamToChunk(request, &chunk, 0);
	HTRequest_setOutputStream(request, target);
	if (HTPostFormAnchor(formdata, anchor, request) != NULL)
	    return chunk;
	else {
	    HTChunk_delete(chunk);
	    return NULL;
	}
    }
    return NULL;
}


/* --------------------------------------------------------------------------*/
/*				PUT A DOCUMENT 				     */
/* --------------------------------------------------------------------------*/ 

/* 
**  If we use our persistent cache then we can protect
**  against the lost update problem by saving the etag
**  or last modified date in the cache and use it on all
**  our PUT operations.
*/
PRIVATE BOOL set_preconditions (HTRequest * request)
{
    if (request) {
	HTPreconditions precons = HTRequest_preconditions(request);
        HTRqHd if_headers = HTRequest_rqHd (request);
        HTRqHd all_if_headers =
           (HT_C_IF_MATCH | HT_C_IF_MATCH_ANY |
            HT_C_IF_NONE_MATCH | HT_C_IF_NONE_MATCH_ANY |
            HT_C_IMS | HT_C_IF_UNMOD_SINCE);
        switch (precons) {
	case HT_NO_MATCH:
            if_headers &= ~(all_if_headers);
	    break;

	case HT_MATCH_THIS:
            if_headers &= ~(all_if_headers);
            if_headers |= (HT_C_IF_MATCH | HT_C_IF_UNMOD_SINCE);
	    break;
	    
	case HT_MATCH_ANY:
            if_headers &= ~(all_if_headers);
            if_headers |= (HT_C_IF_MATCH_ANY);
	    break;
	    
	case HT_DONT_MATCH_THIS:
            if_headers &= ~(all_if_headers);
            if_headers |= (HT_C_IF_NONE_MATCH | HT_C_IMS);
	    break;
	    
	case HT_DONT_MATCH_ANY:
            if_headers &= ~(all_if_headers);
	    if_headers |= (HT_C_IF_NONE_MATCH_ANY);
	    break;

	default:
	    HTTRACE(APP_TRACE, "Precondition %d not understood\n" _ precons);

	}

        /* Set the if-* bit flag */
        HTRequest_setRqHd(request, if_headers);
        
        return YES;
    }
    return NO;
}

PRIVATE BOOL setup_anchors (HTRequest * request,
			    HTParentAnchor * source, HTParentAnchor * dest,
			    HTMethod method)
{
    if (!(method & (METHOD_PUT | METHOD_POST))) {
	HTTRACE(APP_TRACE, "Posting..... Bad method\n");
	return NO;
    }

    /*
    **  Check whether we know if it is possible to PUT to this destination.
    **  We both check the local set of allowed methods in the anchor and any
    **  site information that we may have about the location of the origin 
    **  server.
    */
    {
	char * addr = HTAnchor_address((HTAnchor *) source);
	char * hostname = HTParse(addr, "", PARSE_HOST);
#if 0
	HTHost * host = HTHost_find(hostname);
	HTMethod public_methods = HTHost_publicMethods(host);
	HTMethod private_methods = HTAnchor_allow(dest);
#endif
	HT_FREE(hostname);
	HT_FREE(addr);

#if 0
	/*
	**  Again, this may be too cautios for normal operations
	*/
	if (!(method & (private_methods | public_methods))) {
	    HTAlertCallback * prompt = HTAlert_find(HT_A_CONFIRM);
	    if (prompt) {
		if ((*prompt)(request, HT_A_CONFIRM, HT_MSG_METHOD,
			      NULL, NULL, NULL) != YES)
		    return NO;
	    }
	}
#endif
    }

    /*
    **  Bind the source anchor to the dest anchor that will contain the
    **  response. If link already exists then ask is we should do it again.
    **  If so then remove the old link and replace it with a new one.
    */
    {
	HTLink *link = HTLink_find((HTAnchor *) source, (HTAnchor *) dest);
	if (link && HTLink_method(link) == METHOD_PUT) {
	    HTAlertCallback * prompt = HTAlert_find(HT_A_CONFIRM);
	    if (prompt) {
		if ((*prompt)(request, HT_A_CONFIRM, HT_MSG_REDO,
			      NULL, NULL, NULL) != YES)
		    return NO;
	    }
	    HTLink_remove((HTAnchor *) source, (HTAnchor *) dest);
	}
	HTLink_add((HTAnchor*) source, (HTAnchor*) dest, NULL, METHOD_PUT);
    }
    return YES;
}

/*	Send an Anchor using PUT from absolute name
**	-------------------------------------------
**	Upload a document referenced by an absolute URL appended.
**	The URL can NOT contain any fragment identifier!
**	The list of form data must be given as an association list where 
**	the name is the field name and the value is the value of the field.
*/
PUBLIC BOOL HTPutAbsolute (HTParentAnchor *	source,
			   const char *		destination,
			   HTRequest *		request)
{
    if (source && destination && request) {
	HTAnchor * dest = HTAnchor_findAddress(destination);
	return HTPutAnchor(source, dest, request);
    }
    return NO;
}

/*	Send an Anchor using PUT from relative name
**	-------------------------------------------
**	Upload a document referenced by a relative URL appended.
**	The URL can NOT contain any fragment identifier!
**	The list of form data must be given as an association list where 
**	the name is the field name and the value is the value of the field.
*/
PUBLIC BOOL HTPutRelative (HTParentAnchor *	source,
			   const char * 	relative,
			   HTParentAnchor *	destination_base,
			   HTRequest *		request)
{
    if (source && relative && destination_base && request) {
	BOOL status;
	char * full_url = NULL;
	char * base_url = HTAnchor_address((HTAnchor *) destination_base);
	full_url=HTParse(relative, base_url,
			 PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
	status = HTPutAbsolute(source, full_url, request);
	HT_FREE(full_url);
	HT_FREE(base_url);
	return status;
    }
    return NO;
}

/*	Send an Anchor using PUT from an anchor
**	---------------------------------------
**	Upload a document referenced by an anchor object appended
**	The URL can NOT contain any fragment identifier!
**	The list of form data must be given as an association list where 
**	the name is the field name and the value is the value of the field.
*/
PUBLIC BOOL HTPutAnchor (HTParentAnchor *	source,
			 HTAnchor *		destination,
			 HTRequest *	 	request)
{
    HTParentAnchor * dest = HTAnchor_parent(destination);
    if (source && dest && request) {
	if (setup_anchors(request, source, dest, METHOD_PUT) == YES) {

	    /* Set up the request object */
	    HTRequest_addGnHd(request, HT_G_DATE);
	    HTRequest_setEntityAnchor(request, source);
	    HTRequest_setMethod(request, METHOD_PUT);
	    HTRequest_setAnchor(request, destination);

            /* Setup preconditions */
    	    set_preconditions(request);

	    /* Add the entity callback function to provide the form data */
	    HTRequest_setPostCallback(request, HTEntity_callback);

	    /* Now start the load normally */
	    return launch_request(request, NO);
	}
    }
    return NO;
}

/*	Send an Anchor using POST from absolute name
**	-------------------------------------------
**	Upload a document referenced by an absolute URL appended.
**	The URL can NOT contain any fragment identifier!
**	The list of form data must be given as an association list where 
**	the name is the field name and the value is the value of the field.
*/
PUBLIC BOOL HTPostAbsolute (HTParentAnchor *	source,
			   const char *		destination,
			   HTRequest *		request)
{
    if (source && destination && request) {
	HTAnchor * dest = HTAnchor_findAddress(destination);
	return HTPostAnchor(source, dest, request);
    }
    return NO;
}

/*	Send an Anchor using POST from relative name
**	-------------------------------------------
**	Upload a document referenced by a relative URL appended.
**	The URL can NOT contain any fragment identifier!
**	The list of form data must be given as an association list where 
**	the name is the field name and the value is the value of the field.
*/
PUBLIC BOOL HTPostRelative (HTParentAnchor *	source,
			   const char * 	relative,
			   HTParentAnchor *	destination_base,
			   HTRequest *		request)
{
    if (source && relative && destination_base && request) {
	BOOL status;
	char * full_url = NULL;
	char * base_url = HTAnchor_address((HTAnchor *) destination_base);
	full_url=HTParse(relative, base_url,
			 PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
	status = HTPostAbsolute(source, full_url, request);
	HT_FREE(full_url);
	HT_FREE(base_url);
	return status;
    }
    return NO;
}

/*	Send an Anchor using POST from an anchor
**	---------------------------------------
**	Upload a document referenced by an anchor object appended
**	The URL can NOT contain any fragment identifier!
**	The list of form data must be given as an association list where 
**	the name is the field name and the value is the value of the field.
*/
PUBLIC BOOL HTPostAnchor (HTParentAnchor *	source,
			 HTAnchor *		destination,
			 HTRequest *	 	request)
{
    HTParentAnchor * dest = HTAnchor_parent(destination);
    if (source && dest && request) {
	if (setup_anchors(request, source, dest, METHOD_POST) == YES) {

	    /* Set up the request object */
	    HTRequest_addGnHd(request, HT_G_DATE);
	    HTRequest_setEntityAnchor(request, source);
	    HTRequest_setMethod(request, METHOD_POST);
	    HTRequest_setAnchor(request, destination);

	    /* Add the entity callback function to provide the form data */
	    HTRequest_setPostCallback(request, HTEntity_callback);

	    /* Now start the load normally */
	    return launch_request(request, NO);
	}
    }
    return NO;
}

/*	Send an Anchor using PUT from absolute name
**	-------------------------------------------
**	Upload a document referenced by an absolute URL appended.
**	The URL can NOT contain any fragment identifier!
**	The list of form data must be given as an association list where 
**	the name is the field name and the value is the value of the field.
*/
PUBLIC BOOL HTPutStructuredAbsolute (HTParentAnchor *	source,
				     const char *	destination,
				     HTRequest *	request,
				     HTPostCallback *	input)
{
    if (source && destination && request && input) {
	HTAnchor * dest = HTAnchor_findAddress(destination);
	return HTPutStructuredAnchor(source, dest, request, input);
    }
    return NO;
}

/*	Send an Anchor using PUT from relative name
**	-------------------------------------------
**	Upload a document referenced by a relative URL appended.
**	The URL can NOT contain any fragment identifier!
**	The list of form data must be given as an association list where 
**	the name is the field name and the value is the value of the field.
*/
PUBLIC BOOL HTPutStructuredRelative (HTParentAnchor *	source,
				     const char * 	relative,
				     HTParentAnchor *	destination_base,
				     HTRequest *	request,
				     HTPostCallback *	input)
{
    if (source && relative && destination_base && request && input) {
	BOOL status;
	char * full_url = NULL;
	char * base_url = HTAnchor_address((HTAnchor *) destination_base);
	full_url=HTParse(relative, base_url,
			 PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
	status = HTPutStructuredAbsolute(source, full_url, request, input);
	HT_FREE(full_url);
	HT_FREE(base_url);
	return status;
    }
    return NO;
}

/*	Send an Anchor using PUT from an anchor
**	---------------------------------------
**	Upload a document referenced by an anchor object appended
**	The URL can NOT contain any fragment identifier!
**	The list of form data must be given as an association list where 
**	the name is the field name and the value is the value of the field.
*/
PUBLIC BOOL HTPutStructuredAnchor (HTParentAnchor *	source,
				   HTAnchor *		destination,
				   HTRequest *	 	request,
				   HTPostCallback *	input)
{
    HTParentAnchor * dest = HTAnchor_parent(destination);
    if (source && dest && request) {
	if (setup_anchors(request, source, dest, METHOD_PUT) == YES) {

	    /* Set up the request object */
	    HTRequest_addGnHd(request, HT_G_DATE);
	    HTRequest_setEntityAnchor(request, source);
	    HTRequest_setMethod(request, METHOD_PUT);
	    HTRequest_setAnchor(request, destination);

            /* Setup preconditions */
            set_preconditions(request);

	    /* Add the entity callback function to provide the form data */
	    HTRequest_setPostCallback(request, input);

	    /* Now start the load normally */
	    return launch_request(request, NO);
	}
    }
    return NO;
}

/*
**	After filter for handling PUT of document.
*/
PRIVATE int HTSaveFilter (HTRequest * request, HTResponse * response,
			  void * param, int status)
{
    HTPutContext * me = (HTPutContext *) param;
    HTTRACE(APP_TRACE, "Save Filter. Using context %p with state %c\n" _ 
		me _ me->state+0x30);

    /*
    **  Just ignore authentication in the hope that some other filter will
    **  handle this.
    */
    if (status == HT_NO_ACCESS || status == HT_NO_PROXY_ACCESS ||
        status == HT_REAUTH || status == HT_PROXY_REAUTH) {
	HTTRACE(APP_TRACE, "Save Filter. Waiting for authentication\n");
	return HT_OK;
    }

    /*
    **  If either the source or the destination has moved then ask the user
    **  what to do. If there is no user then stop
    */
    if (status == HT_TEMP_REDIRECT || status == HT_PERM_REDIRECT ||
	status == HT_FOUND || status == HT_SEE_OTHER) {
	HTAlertCallback * prompt = HTAlert_find(HT_A_CONFIRM);
	HTAnchor * redirection = HTResponse_redirection(response);
	if (prompt && redirection) {
	    if (me->state == HT_LOAD_SOURCE) {
		if ((*prompt)(request, HT_A_CONFIRM, HT_MSG_SOURCE_MOVED,
			      NULL, NULL, NULL) == YES) {
		    me->source = HTAnchor_parent(redirection);
		} else {
		    /*
		    ** Make sure that the operation stops 
		    */
		    me->state = HT_ABORT_SAVE;
		}
	    } else {
#if 0
		/*
		** If you are very precautios then you can ask here whether
		** we should continue or not in case of a redirection
		*/
		if ((*prompt)(request, HT_A_CONFIRM, HT_MSG_DESTINATION_MOVED,
			      NULL, NULL, NULL) == YES) {
		    me->destination = redirection;
		} else {
		    /*
		    ** Make sure that the operation stops 
		    */
		    me->state = HT_ABORT_SAVE;
		}
#else
		HTTRACE(APP_TRACE, "Save Filter. Destination hae moved!\n");
		me->destination = redirection;
#endif
	    }
	}
	return HT_OK;
    }

    /*
    ** If we succeeded getting the source then start the PUT itself. Otherwise
    ** cleanup the mess
    */
    if (me->state == HT_LOAD_SOURCE && 
	(status == HT_LOADED || status == HT_NOT_MODIFIED) &&
	!HTError_hasSeverity(HTRequest_error(request), ERR_INFO)) {

	/* Swap the document in the anchor with the new one */
	me->placeholder = HTAnchor_document(me->source);
	HTAnchor_setDocument(me->source, HTChunk_data(me->document));

	/* Set up the request object */
	HTRequest_addGnHd(request, HT_G_DATE);
	HTRequest_setEntityAnchor(request, me->source);
	HTRequest_setMethod(request, METHOD_PUT);
	HTRequest_setAnchor(request, me->destination);
	HTRequest_setOutputFormat(request, me->format);
	HTRequest_setOutputStream(request, me->target);

        /* Set up preconditions */
	set_preconditions(request);

        /* Delete existing credentials as they are generated anew */
        HTRequest_deleteCredentialsAll(request);

	/* Make sure we flush the output immediately */
	HTRequest_forceFlush(request);

	/* Turn progress notifications back on */
	HTRequest_setInternal(request, NO);

	/* Add the entity callback function to provide the form data */
	HTRequest_setPostCallback(request, HTEntity_callback);

	/* Now start the load normally */
	if (launch_request(request, NO) == YES)
	    me->state = HT_SAVE_DEST;
        else {
	    HTAnchor_setDocument(me->source, me->placeholder);
	    HTChunk_delete(me->document);
	    HT_FREE(me);
	}
#if 0    
	me->state = launch_request(request, NO) ?
	    HT_SAVE_DEST : HT_LOAD_SOURCE;
#endif
	/*
	**  By returning HT_ERROR we make sure that this is the last handler to
	**  be called. We do this as we don't want any other filter to delete
	**  the request object now when we have just started a new one
	**  ourselves
	*/	
	return HT_ERROR;

    } else {
#if 0
        /* @@ JK 28/03/2000: invalidated this code as we're doing this exact
           treatment later on. In addition, it was a source of
           dangling pointer error  */
	/* @@ JK: Added it again, because it's now working! */
#endif
        HTAnchor_setDocument(me->source, me->placeholder);
        HTChunk_delete(me->document);
        HT_FREE(me);
    }
    return HT_OK;
}

/*	Send an Anchor using PUT from absolute name
**	-------------------------------------------
**	Upload a document referenced by an absolute URL appended.
**	The URL can NOT contain any fragment identifier!
**	The list of form data must be given as an association list where 
**	the name is the field name and the value is the value of the field.
*/
PUBLIC BOOL HTPutDocumentAbsolute (HTParentAnchor *	source,
				   const char *		destination,
				   HTRequest *		request)
{
    if (source && destination && request) {
	HTAnchor * dest = HTAnchor_findAddress(destination);
	return HTPutDocumentAnchor(source, dest, request);
    }
    return NO;
}

/*	Send an Anchor using PUT from relative name
**	-------------------------------------------
**	Upload a document referenced by a relative URL appended.
**	The URL can NOT contain any fragment identifier!
**	The list of form data must be given as an association list where 
**	the name is the field name and the value is the value of the field.
*/
PUBLIC BOOL HTPutDocumentRelative (HTParentAnchor *	source,
				   const char * 	relative,
				   HTParentAnchor *	destination_base,
				   HTRequest *		request)
{
    if (source && relative && destination_base && request) {
	BOOL status;
	char * full_url = NULL;
	char * base_url = HTAnchor_address((HTAnchor *) destination_base);
	full_url=HTParse(relative, base_url,
			 PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
	status = HTPutDocumentAbsolute(source, full_url, request);
	HT_FREE(full_url);
	HT_FREE(base_url);
	return status;
    }
    return NO;
}

/*	Send an Anchor using PUT from an anchor
**	---------------------------------------
**	Upload a document referenced by an anchor object appended
**	The URL can NOT contain any fragment identifier!
**	The source document is first loaded into memory and then the PUT
**	to the remote server is done using the memory version
*/
PUBLIC BOOL HTPutDocumentAnchor (HTParentAnchor *	source,
				 HTAnchor *		destination,
				 HTRequest *	 	request)
{
    HTParentAnchor * dest = HTAnchor_parent(destination);
    if (source && dest && request) {
	if (setup_anchors(request, source, dest, METHOD_PUT) == YES) {
	    HTPutContext * context = NULL;

	    /*
	    **  First we register an AFTER filter that can check the result
	    **  of the source load if success then it can start the PUT
	    ** operation to the destination.
	    */
	    if (!(context=(HTPutContext *) HT_CALLOC(1, sizeof(HTPutContext))))
		HT_OUTOFMEM("HTPutDocumentAnchor");
	    context->source = source;
	    context->destination = destination;

	    /*
	    **  We register the after filter with a NULL template as we
	    **  don't know the source of the data.
	    */
	    HTRequest_addAfter(request, HTSaveFilter, NULL, context, HT_ALL,
			       HT_FILTER_FIRST, NO);

	    /* Turn off progress notifications */
	    HTRequest_setInternal(request, YES);

	    /*
	    **  We make sure that we are not using a memory cached element.
	    **  It's OK to use a file cached element!
	    */
	    HTRequest_setReloadMode(request, HT_CACHE_FLUSH_MEM);

	    /*
	    **  Some proxy servers don't clean up their cache
	    **  when receiving a PUT, specially if this PUT is
	    **  redirected. We remove this problem by adding 
	    **  an explicit Cache-Control: no-cache header to
	    **  all PUT requests.
	    */
	    HTRequest_addCacheControl(request, "no-cache", "");

	    /*
	    ** Now we load the source document into a chunk. We specify that
	    ** we want the document ASIS from the source location. 
	    */
	    context->format = HTRequest_outputFormat(request);
	    context->target = HTRequest_outputStream(request);
	    HTRequest_setOutputFormat(request, WWW_SOURCE);
	    context->document = HTLoadAnchorToChunk((HTAnchor*)source,request);
	    if (context->document == NULL) {
		HTTRACE(APP_TRACE, "Put Document No source\n");
		HT_FREE(context);
		return NO;
	    }
	    return YES;
	}
    }
    return NO;
}

/* ------------------------------------------------------------------------- */

/*	Copy an anchor
**	--------------
**	Fetch the URL (possibly local file URL) and send it using either PUT
**	or POST to the remote destination using HTTP. The caller can decide the
**	exact method used and which HTTP header fields to transmit by setting
**	the user fields in the request structure.
**	If posting to NNTP then we can't dispatch at this level but must pass
**	the source anchor to the news module that then takes all the refs
**	to NNTP and puts into the "newsgroups" header
*/
PUBLIC BOOL HTCopyAnchor (HTAnchor * src_anchor, HTRequest * main_dest)
{ 
    HTRequest * src_req;
    HTList * cur;
    if (!src_anchor || !main_dest) {
	HTTRACE(APP_TRACE, "Copy........ BAD ARGUMENT\n");
	return NO;
    }

    /* Set the source anchor */
    main_dest->source_anchor = HTAnchor_parent(src_anchor);

    /* Build the POST web if not already there */
    if (!main_dest->source) {
	src_req = HTRequest_dupInternal(main_dest);	  /* Get a duplicate */
	HTAnchor_clearHeader((HTParentAnchor *) src_anchor);
	src_req->method = METHOD_GET;
	src_req->reload = HT_CACHE_FLUSH_MEM;
	src_req->output_stream = NULL;
	src_req->output_format = WWW_SOURCE;	 /* We want source (for now) */

	/* Set up the main link in the source anchor */
	{
	    HTLink * main_link = HTAnchor_mainLink((HTAnchor *) src_anchor);
	    HTAnchor *main_anchor = HTLink_destination(main_link);
	    HTMethod method = HTLink_method(main_link);
	    if (!main_link || method==METHOD_INVALID) {
		HTTRACE(APP_TRACE, "Copy Anchor. No destination found or unspecified method\n");
		HTRequest_delete(src_req);
		return NO;
	    }
	    main_dest->GenMask |= HT_G_DATE;		 /* Send date header */
	    main_dest->reload = HT_CACHE_VALIDATE;
	    main_dest->method = method;
	    main_dest->input_format = WWW_SOURCE;
	    HTRequest_addDestination(src_req, main_dest);
	    if (HTLoadAnchor(main_anchor, main_dest) == NO)
		return NO;
	}

	/* For all other links in the source anchor */
	if ((cur = HTAnchor_subLinks(src_anchor))) {
	    HTLink * pres;
	    while ((pres = (HTLink *) HTList_nextObject(cur))) {
		HTAnchor *dest = HTLink_destination(pres);
		HTMethod method = HTLink_method(pres);
		HTRequest *dest_req;
		if (!dest || method==METHOD_INVALID) {
		    HTTRACE(APP_TRACE, "Copy Anchor. Bad anchor setup %p\n" _ 
				dest);
		    return NO;
		}
		dest_req = HTRequest_dupInternal(main_dest);
		dest_req->GenMask |= HT_G_DATE;		 /* Send date header */
		dest_req->reload = HT_CACHE_VALIDATE;
		dest_req->method = method;
		dest_req->output_stream = NULL;
		dest_req->output_format = WWW_SOURCE;
		HTRequest_addDestination(src_req, dest_req);

		if (HTLoadAnchor(dest, dest_req) == NO)
		    return NO;
	    }
	}
    } else {			 /* Use the existing Post Web and restart it */
	src_req = main_dest->source;
	if (src_req->mainDestination)
	    if (launch_request(main_dest, NO) == NO)
		return NO;
	if (src_req->destinations) {
	    HTRequest * pres;
	    cur = HTAnchor_subLinks(src_anchor);
	    while ((pres = (HTRequest *) HTList_nextObject(cur)) != NULL) {
		if (launch_request(pres, NO) == NO)
		    return NO;
	    }
	}
    }

    /* Now open the source */
    return HTLoadAnchor(src_anchor, src_req);
}

/*	Upload an Anchor
**	----------------
**	This function can be used to send data along with a request to a remote
**	server. It can for example be used to POST form data to a remote HTTP
**	server - or it can be used to post a newsletter to a NNTP server. In
**	either case, you pass a callback function which the request calls when
**	the remote destination is ready to accept data. In this callback
**	you get the current request object and a stream into where you can 
**	write data. It is very important that you return the value returned
**	by this stream to the Library so that it knows what to do next. The
**	reason is that the outgoing stream might block or an error may
**	occur and in that case the Library must know about it. The source
**	anchor represents the data object in memory and it points to 
**	the destination anchor by using the POSTWeb method. The source anchor
**	contains metainformation about the data object in memory and the 
**	destination anchor represents the reponse from the remote server.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTUploadAnchor (HTAnchor *		source_anchor,
			    HTRequest * 	request,
			    HTPostCallback *	callback)
{
    HTLink * link = HTAnchor_mainLink((HTAnchor *) source_anchor);
    HTAnchor * dest_anchor = HTLink_destination(link);
    HTMethod method = HTLink_method(link);
    if (!link || method==METHOD_INVALID || !callback) {
	HTTRACE(APP_TRACE, "Upload...... No destination found or unspecified method\n");
	return NO;
    }
    request->GenMask |= HT_G_DATE;			 /* Send date header */
    request->reload = HT_CACHE_VALIDATE;
    request->method = method;
    request->source_anchor = HTAnchor_parent(source_anchor);
    request->PostCallback = callback;
    return HTLoadAnchor(dest_anchor, request);
}

/*	POST Callback Handler
**	---------------------
**	If you do not want to handle the stream interface on your own, you
**	can use this function which writes the source anchor hyperdoc to the
**	target stream for the anchor upload and also handles the return value
**	from the stream. If you don't want to write the source anchor hyperdoc
**	then you can register your own callback function that can get the data
**	you want.
*/
PUBLIC int HTUpload_callback (HTRequest * request, HTStream * target)
{
    HTTRACE(APP_TRACE, "Uploading... from callback function\n");
    if (!request || !request->source_anchor || !target) return HT_ERROR;
    {
	int status;
	HTParentAnchor * source = request->source_anchor;
	char * document = (char *) HTAnchor_document(request->source_anchor);
	int len = HTAnchor_length(source);
	if (len < 0) {
	    len = strlen(document);
	    HTAnchor_setLength(source, len);
	}
	status = (*target->isa->put_block)(target, document, len);
	if (status == HT_OK)
	    return (*target->isa->flush)(target);
	if (status == HT_WOULD_BLOCK) {
	    HTTRACE(PROT_TRACE, "POST Anchor. Target WOULD BLOCK\n");
	    return HT_WOULD_BLOCK;
	} else if (status == HT_PAUSE) {
	    HTTRACE(PROT_TRACE, "POST Anchor. Target PAUSED\n");
	    return HT_PAUSE;
	} else if (status > 0) {	      /* Stream specific return code */
	    HTTRACE(PROT_TRACE, "POST Anchor. Target returns %d\n" _ status);
	    return status;
	} else {				     /* we have a real error */
	    HTTRACE(PROT_TRACE, "POST Anchor. Target ERROR\n");
	    return status;
	}
    }
}

/* ------------------------------------------------------------------------- */
/*				HEAD METHOD 				     */
/* ------------------------------------------------------------------------- */

/*	Request metainformation about a document from absolute name
**	-----------------------------------------------------------
**	Request a document referencd by an absolute URL.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTHeadAbsolute (const char * url, HTRequest * request)
{
    if (url && request) {
	HTAnchor * anchor = HTAnchor_findAddress(url);
	return HTHeadAnchor(anchor, request);
    }
    return NO;
}

/*	Request metainformation about a document from relative name
**	-----------------------------------------------------------
**	Request a document referenced by a relative URL. The relative URL is 
**	made absolute by resolving it relative to the address of the 'base' 
**	anchor.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTHeadRelative (const char * 	relative,
			    HTParentAnchor *	base,
			    HTRequest *		request)
{
    BOOL status = NO;
    if (relative && base && request) {
	char * rel = NULL;
	char * full_url = NULL;
	char * base_url = HTAnchor_address((HTAnchor *) base);
	StrAllocCopy(rel, relative);
	full_url = HTParse(HTStrip(rel), base_url,
			 PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
	status = HTHeadAbsolute(full_url, request);
	HT_FREE(rel);
	HT_FREE(full_url);
	HT_FREE(base_url);
    }
    return status;
}

/*	Request metainformation about an anchor
**	--------------------------------------
**	Request the document referenced by the anchor
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTHeadAnchor (HTAnchor * anchor, HTRequest * request)
{
    if (anchor && request) {
	HTRequest_setAnchor(request, anchor);
	HTRequest_setMethod(request, METHOD_HEAD);
	return launch_request(request, NO);
    }
    return NO;
}

/* ------------------------------------------------------------------------- */
/*				DELETE METHOD 				     */
/* ------------------------------------------------------------------------- */

/*	Delete a document on a remote server
**	------------------------------------
**	Request a document referencd by an absolute URL.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTDeleteAbsolute (const char * url, HTRequest * request)
{
    if (url && request) {
	HTAnchor * anchor = HTAnchor_findAddress(url);
	return HTDeleteAnchor(anchor, request);
    }
    return NO;
}

/*	Request metainformation about a document from relative name
**	-----------------------------------------------------------
**	Request a document referenced by a relative URL. The relative URL is 
**	made absolute by resolving it relative to the address of the 'base' 
**	anchor.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTDeleteRelative (const char * 	relative,
			    HTParentAnchor *	base,
			    HTRequest *		request)
{
    BOOL status = NO;
    if (relative && base && request) {
	char * rel = NULL;
	char * full_url = NULL;
	char * base_url = HTAnchor_address((HTAnchor *) base);
	StrAllocCopy(rel, relative);
	full_url = HTParse(HTStrip(rel), base_url,
			 PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
	status = HTDeleteAbsolute(full_url, request);
	HT_FREE(rel);
	HT_FREE(full_url);
	HT_FREE(base_url);
    }
    return status;
}

/*	Request metainformation about an anchor
**	--------------------------------------
**	Request the document referenced by the anchor
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTDeleteAnchor (HTAnchor * anchor, HTRequest * request)
{
    if (anchor && request) {
	HTRequest_setAnchor(request, anchor);
	HTRequest_setMethod(request, METHOD_DELETE);
	return launch_request(request, NO);
    }
    return NO;
}

/* ------------------------------------------------------------------------- */
/*				OPTIONS METHOD 				     */
/* ------------------------------------------------------------------------- */

/*	Options availeble for document from absolute name
**	-------------------------------------------------
**	Request a document referencd by an absolute URL.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTOptionsAbsolute (const char * url, HTRequest * request)
{
    if (url && request) {
	HTAnchor * anchor = HTAnchor_findAddress(url);
	return HTOptionsAnchor(anchor, request);
    }
    return NO;
}

/*	Options available for document from relative name
**	-------------------------------------------------
**	Request a document referenced by a relative URL. The relative URL is 
**	made absolute by resolving it relative to the address of the 'base' 
**	anchor.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTOptionsRelative (const char * 	relative,
			    HTParentAnchor *	base,
			    HTRequest *		request)
{
    BOOL status = NO;
    if (relative && base && request) {
	char * rel = NULL;
	char * full_url = NULL;
	char * base_url = HTAnchor_address((HTAnchor *) base);
	StrAllocCopy(rel, relative);
	full_url = HTParse(HTStrip(rel), base_url,
			 PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
	status = HTOptionsAbsolute(full_url, request);
	HT_FREE(rel);
	HT_FREE(full_url);
	HT_FREE(base_url);
    }
    return status;
}

/*	Options available for document using Anchor
**	-------------------------------------------
**	Request the document referenced by the anchor
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTOptionsAnchor (HTAnchor * anchor, HTRequest * request)
{
    if (anchor && request) {
	HTRequest_setAnchor(request, anchor);
	HTRequest_setMethod(request, METHOD_OPTIONS);
	return launch_request(request, NO);
    }
    return NO;
}

/* ------------------------------------------------------------------------- */
/*				TRACE METHOD 				     */
/* ------------------------------------------------------------------------- */

/*	Traces available for document from absolute name
**	------------------------------------------------
**	Request a document referencd by an absolute URL.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTTraceAbsolute (const char * url, HTRequest * request)
{
    if (url && request) {
	HTAnchor * anchor = HTAnchor_findAddress(url);
	return HTTraceAnchor(anchor, request);
    }
    return NO;
}

/*	Traces available for document from relative name
**	------------------------------------------------
**	Request a document referenced by a relative URL. The relative URL is 
**	made absolute by resolving it relative to the address of the 'base' 
**	anchor.
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTTraceRelative (const char * 	relative,
			     HTParentAnchor *	base,
			     HTRequest *	request)
{
    BOOL status = NO;
    if (relative && base && request) {
	char * rel = NULL;
	char * full_url = NULL;
	char * base_url = HTAnchor_address((HTAnchor *) base);
	StrAllocCopy(rel, relative);
	full_url = HTParse(HTStrip(rel), base_url,
			 PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
	status = HTTraceAbsolute(full_url, request);
	HT_FREE(rel);
	HT_FREE(full_url);
	HT_FREE(base_url);
    }
    return status;
}

/*	Trace available for document using Anchor
**	-------------------------------------------
**	Request the document referenced by the anchor
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTTraceAnchor (HTAnchor * anchor, HTRequest * request)
{
    if (anchor && request) {
	HTRequest_setAnchor(request, anchor);
	HTRequest_setMethod(request, METHOD_TRACE);
	return launch_request(request, NO);
    }
    return NO;
}


/* ------------------------------------------------------------------------- */
/*				SERVER METHODS 				     */
/* ------------------------------------------------------------------------- */

PRIVATE BOOL launch_server (HTRequest * request, BOOL recursive)
{
#ifdef HTDEBUG
    if (PROT_TRACE) {
	HTParentAnchor *anchor = HTRequest_anchor(request);
	char * full_address = HTAnchor_address((HTAnchor *) anchor);
	HTTRACE(PROT_TRACE, "HTAccess.... Serving %s\n" _ full_address);
	HT_FREE(full_address);
    }
#endif /* HTDEBUG */
    return HTServe(request, recursive);
}

/*	Serving a request
**	-----------------
**	Returns YES if request accepted, else NO
*/
PUBLIC BOOL HTServeAbsolute (const char * url, HTRequest * request)
{
    if (url && request) {
	HTAnchor * anchor = HTAnchor_findAddress(url);
	HTRequest_setAnchor(request, anchor);
	return launch_server(request, NO);
    }
    return NO;
}
