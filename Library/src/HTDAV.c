/*
** WebDAV MANAGER
**
**      (c) COPYRIGHT MIT 1995.
**      Please first read the full copyright statement in the file COPYRIGH.
**
** Authors
**      MKP     Manuele Kirsch Pinheiro, Manuele.Kirsch_Pinheiro@inrialpes.fr 
**                                       manuele@inf.ufrgs.br
**
** History
**      15 Fev 02 Writen
**      15 Mar 02 Changed - All methods will use entity callback and not the
**                message body functions. This modification was demanded by
**                Jose Kahan.
**      30 May 02 Changed - wwwsys.h becames the first include file
**
**      $Log$
**      Revision 1.2  2002/05/29 16:09:13  kirschpi
**
**      Fixes for windows plataform concerning WebDAV and Extension
**      methods. In HTMethod and HTRequest, functions defined for
**      the Extension Methods are now defined always, but return
**      fail values when HT_EXT is not defined. In addition, the
**      files "Library/src/WWWDAV.html" and "Library/src/windows/wwwdav.files"
**      have been added. These files and modifications were needed
**      to produce the correct "*.def" files, for windows plataform.
**
**      Revision 1.1  2002/03/21 14:16:27  kirschpi
**      Missing files
**      Manuele Kirsch
**
**
** $Id$
*/


/* Library include files */
#include "wwwsys.h"
#include "WWWLib.h"
#include "WWWInit.h"
#include "WWWUtil.h"
#include "WWWStream.h"
#include "HTDAV.h"              /* implemented here */

#ifdef HT_DAV

struct _HTStream {
    const HTStreamClass *       isa;
    HTStream *                  target;
    HTRequest *                 request;
    int                         version;
    BOOL                        endHeader;
    BOOL                        transparent;
};


/* --------------------------------------------------------------------------*/
/*                           WebDAV REQUEST HEADERS                          */
/* --------------------------------------------------------------------------*/

/* Headers defined in WebDAV - RC2518 
** If: state list _ may be a tagged or a non-tag list of state tokens and Etags
** Depth: how depth the method should be executed. Values are: 0,1,infinity
** Destination: destination URI for COPY and MOVE methods
** Overwrite: should the method overwrite any existant resource? Values: T or F
** LockToken: lock identification (used only in UNLOCK method)
** Timeout: lock timeout. Values: Second-nnn,Infinite or Extend file (RFC2068) 
*/
struct _HTDAVHeaders {
    char * If;
    char * Depth;
    char * Destination;
    char * LockToken;
    char * Timeout;
    char Overwrite;
};


/*
** Creates a new HTDAVHeaders struct 
*/ 
PUBLIC HTDAVHeaders * HTDAVHeaders_new (void) {
    HTDAVHeaders *me;

    if ( (me = (HTDAVHeaders *) HT_CALLOC (1,sizeof(HTDAVHeaders))) == NULL)
        HT_OUTOFMEM ("HTDAVHeaders_new");

    /* set everything to an empty value */
    me->If = NULL;
    me->Depth = NULL;
    me->Destination = NULL;
    me->LockToken = NULL;
    me->Timeout = NULL;
    me->Overwrite = ' ';

    HTTRACE (PROT_TRACE,"HTDAV.... HTDAVHeaders object created\n");

    return me;
} 


/*
** Deletes a HTDAVHeaders object
*/
PUBLIC BOOL HTDAVHeaders_delete (HTDAVHeaders *me) {
    if (me) {
        if (me->If) HT_FREE (me->If);
        if (me->Depth) HT_FREE (me->Depth);
        if (me->Destination) HT_FREE (me->Destination);
        if (me->LockToken) HT_FREE (me->LockToken);
        if (me->Timeout) HT_FREE (me->Timeout);
        HT_FREE (me);

        HTTRACE (PROT_TRACE,"HTDAV.... HTDAVHeaders object removed\n");

        return YES;
    }
    return NO;
}


/*
** Set the If header - see section 9.4 of RFC2518 
*/ 
PUBLIC BOOL HTDAV_setIfHeader (HTDAVHeaders *me, const char *If) {
    if (me && If && *If) {
        HTTRACE (PROT_TRACE,"HTDAV.... If Header set\n");
        StrAllocCopy (me->If,If);
        return YES;
    }
    return NO;
}

/*
** Removes the "If" header.
*/
PUBLIC BOOL HTDAV_deleteIfHeader (HTDAVHeaders * me) {
    if (me && me->If) {
        HT_FREE(me->If);
        me->If = NULL;
        return YES;
    }
    return NO;
}

/*
** Return the "If" header, NULL if this header is not set.
** The caller should FREE the returned string
*/
PUBLIC char * HTDAV_ifHeader (HTDAVHeaders *me) {
    char *copy = NULL;
    if (me && me->If) {
        StrAllocCopy (copy,me->If);
    }
    return copy;
}


/*
** Set the Depth header - see section 9.2 of RFC2518 
*/ 
PUBLIC BOOL HTDAV_setDepthHeader (HTDAVHeaders *me, const char *Depth) {
    if (me && Depth && *Depth) {
        HTTRACE (PROT_TRACE,"HTDAV.... Depth Header set\n");
        StrAllocCopy (me->Depth,Depth);
        return YES;
    }
    return NO;
}


/*
** Removes the "Depth" header.
*/
PUBLIC BOOL HTDAV_deleteDepthHeader (HTDAVHeaders * me) {
    if (me && me->Depth) {
        HT_FREE (me->Depth);
        me->Depth = NULL;
        return YES;
    }
    return NO;
}


/*
** Return the "Depth" header, NULL if this header is not set.
** The caller should FREE the returned string
*/
PUBLIC char * HTDAV_DepthHeader (HTDAVHeaders *me) {
    char *copy = NULL;
    if (me && me->Depth) {
        StrAllocCopy (copy,me->Depth);
    }
    return copy;
}


/*
** Set the LockToken header - see section 9.5 of RFC2518 
*/ 
PUBLIC BOOL HTDAV_setLockTokenHeader (HTDAVHeaders *me, const char *LockToken) {
    if (me && LockToken && *LockToken) {
        HTTRACE (PROT_TRACE,"HTDAV.... Lock-Token Header set\n");
        StrAllocCopy (me->LockToken,LockToken);
        return YES;
    }
    return NO;
}


/*
** Removes the "LockToken" header.
*/
PUBLIC BOOL HTDAV_deleteLockTokenHeader (HTDAVHeaders * me) {
    if (me && me->LockToken) {
        HT_FREE (me->LockToken);
        me->LockToken = NULL;
        return YES;
    }
    return NO;
}


/*
** Return the "LockToken" header, NULL if this header is not set.
** The caller should FREE the returned string
*/
PUBLIC char * HTDAV_LockTokenHeader (HTDAVHeaders *me) {
    char *copy = NULL;
    if (me && me->LockToken) {
        StrAllocCopy (copy,me->LockToken);
    }
    return copy;
}


/*
** Set the Destination header - see section 9.3 of RFC2518 
*/ 
PUBLIC BOOL HTDAV_setDestinationHeader (HTDAVHeaders *me, const char *Destination) {
    if (me && Destination && *Destination) {
        HTTRACE (PROT_TRACE,"HTDAV.... Destination Header set\n");
        StrAllocCopy (me->Destination,Destination);
        return YES;
    }
    return NO;
}


/*
** Removes the "Destination" header.
*/
PUBLIC BOOL HTDAV_deleteDestinationHeader (HTDAVHeaders * me) {
    if (me && me->Destination) {
        HT_FREE (me->Destination);
        me->Destination = NULL;
        return YES;
    }
    return NO;
}


/*
** Return the "Destination" header, NULL if this header is not set.
** The caller should FREE the returned string
*/
PUBLIC char * HTDAV_DestinationHeader (HTDAVHeaders *me) {
    char *copy = NULL;
    if (me && me->Destination) {
        StrAllocCopy (copy,me->Destination);
    }
    return copy;
}



/*
** Set the Timeout header - see section 9.8 of RFC2518 
*/ 
PUBLIC BOOL HTDAV_setTimeoutHeader (HTDAVHeaders *me, const char *Timeout) {
    if (me && Timeout && *Timeout) {
        HTTRACE (PROT_TRACE,"HTDAV.... Timeout Header set\n");
        StrAllocCopy (me->Timeout,Timeout);
        return YES;
    }
    return NO;
}


/*
** Removes the "Timeout" header.
*/
PUBLIC BOOL HTDAV_deleteTimeoutHeader (HTDAVHeaders * me) {
    if (me && me->Timeout) {
        HT_FREE (me->Timeout);
        me->Timeout = NULL;
        return YES;
    }
    return NO;
}


/*
** Return the "Timeout" header, NULL if this header is not set.
** The caller should FREE the returned string
*/
PUBLIC char * HTDAV_TimeoutHeader (HTDAVHeaders *me) {
    char *copy = NULL;
    if (me && me->Timeout) {
        StrAllocCopy (copy,me->Timeout);
    }
    return copy;
}


/*
** Set the Overwrite header - see section 9.6 of RFC2518
*/
PUBLIC BOOL HTDAV_setOverwriteHeader (HTDAVHeaders *me, BOOL Overwrite) {
    if (me) {
        HTTRACE (PROT_TRACE,"HTDAV.... Overwrite Header set\n");
        me->Overwrite = (Overwrite)?'T':'F';
        return YES;
    }
    return NO;
}

/*
** Removes the "Overwirte" header.
*/
PUBLIC BOOL HTDAV_deleteOverwriteHeader (HTDAVHeaders * me) {
    if (me) {
        me->Overwrite = ' ';
        return YES;
    }
    return NO;
}


/*
** Returns the "Overwrite" header. If it is not set, returns the
** default value (YES == TRUE)
*/
PUBLIC BOOL HTDAV_OverwriteHeader (HTDAVHeaders * me) {
    if (me) {
      return (me->Overwrite==' ' || me->Overwrite=='T')?YES:NO;
    }
    return YES;
}



/* --------------------------------------------------------------------------*/
/*                           ENTITY CALLBACK                                 */
/* --------------------------------------------------------------------------*/


/*
** Entity Callback - IDEM HTAccess.c
*/
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
                len = strlen(document);                 /* Naive! */
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
        } else if (status > 0) {              /* Stream specific return code */
            HTTRACE(PROT_TRACE, "Posting Data. Target returns %d\n" _ status);
            return status;
        } else {                                     /* we have a real error */
            HTTRACE(PROT_TRACE, "Posting Data Target ERROR %d\n" _ status);
            return status;
        }
    }
}



/* --------------------------------------------------------------------------*/
/*                            LOCK REQUESTS                                  */
/* --------------------------------------------------------------------------*/


/*
** LOCK REQUESTS
** 
** LOCK requests may create a lock specified by "lockinfo" XML element
** on the Request URI. LOCK request should have a XML request entity body,
** which contains "owner" XML element, or the request should be a lock
** refresh request.
** Headers:
**      If header is mandatory for lock refresh request
**      Depth header may be "0" or "infinity" (default: infinity)
**      Timeout header may be used
*/

PUBLIC BOOL HTLOCKDocumentAnchor (HTRequest * request, 
                                  HTAnchor * dst,
                                  HTParentAnchor * xmlbody,
                                  HTDAVHeaders * headers) {
    
    if (request && dst) {
        /* set method and request-URI */
        HTRequest_setMethod (request,METHOD_LOCK);
        HTRequest_setAnchor (request,dst);
        HTTRACE (PROT_TRACE,"HTDAV.... Method set to LOCK\n");

        /* set headers */
        HTTRACE (PROT_TRACE,"HTDAV.... Seting default Headers \n");
        HTRequest_addCacheControl (request,"no-cache","");
        HTRequest_addEnHd (request,HT_E_CONTENT_ENCODING);
        HTRequest_addEnHd (request,HT_E_CONTENT_LENGTH);
        HTRequest_addGnHd (request,HT_G_PRAGMA_NO_CACHE);
 
        if (headers) { /* WebDAV specific headers */
            HTTRACE (PROT_TRACE,"HTDAV.... Setting WebDAV headers \n");
            if (headers->If)
                HTRequest_addExtraHeader (request,"If",headers->If);

            if (headers->Depth) { /* Depth: 0 or infinity only */
                if (!strcasecomp(headers->Depth,"0") || 
                            !strcasecomp(headers->Depth,"infinity"))
                    HTRequest_addExtraHeader (request,"Depth",headers->Depth);
            }

            if (headers->Timeout)
                HTRequest_addExtraHeader (request,"Timeout",headers->Timeout);
        }

        /* set body - if there is no body, we expect that is a lock refresh */
        if (xmlbody) {
            HTTRACE (PROT_TRACE,"HTDAV.... Setting Entity Body \n");
            HTRequest_setEntityAnchor (request,xmlbody); 
            HTRequest_setPostCallback(request, HTEntity_callback);
        }

        return HTLoad (request,NO);
    }
    return NO;
}


/*
** LOCK the URI indicated by HTAnchor *dst using the informations
** in xmlbody string. 
*/ 
PUBLIC BOOL HTLOCKAnchor (HTRequest * request,
                          HTAnchor * dst,
                          char * xmlbody,
                          HTDAVHeaders * headers) {

    if (request && dst) {
        HTParentAnchor * src = NULL;
        if (xmlbody) {
            src = HTTmpAnchor(NULL);
            HTAnchor_setDocument(src, xmlbody);
            HTAnchor_setFormat(src, HTAtom_for ("text/xml"));
            HTAnchor_setLength(src, strlen(xmlbody));
        }
        return (HTLOCKDocumentAnchor (request,dst,src,headers));
    }
    return NO;
}


/*
** LOCK the resource indicated by an absolute URI, using the informations
** in xmlbody string.
*/ 
PUBLIC BOOL HTLOCKAbsolute (HTRequest * request,
                            const char * uri,
                            char * xmlbody,
                            HTDAVHeaders * headers){
    if (request && uri && *uri) {
        HTAnchor *dst = HTAnchor_findAddress (uri);
        HTParentAnchor *src = NULL;

        if (xmlbody) {
            src = HTTmpAnchor(NULL);
            HTAnchor_setDocument(src, xmlbody);
            HTAnchor_setFormat(src, HTAtom_for ("text/xml"));
            HTAnchor_setLength(src, strlen(xmlbody));
        }
 
        return (HTLOCKDocumentAnchor (request,dst,src,headers));
    }
    return NO;
}


/*
** LOCK the resource indicated by a relative URI, which is made
** absolute by using the base anchor.
*/
PUBLIC BOOL HTLOCKRelative (HTRequest * request,
                            const char * relative,
                            HTParentAnchor * base,
                            char * xmlbody,
                            HTDAVHeaders * headers){
    BOOL status = NO;
    if (request && relative && base) {
         char * base_uri = HTAnchor_address ((HTAnchor *)base);
         char * full_uri = HTParse(relative,base_uri,PARSE_ACCESS|PARSE_HOST| \
                                                 PARSE_PATH|PARSE_PUNCTUATION);

         status = HTLOCKAbsolute (request,full_uri,xmlbody,headers);
         HT_FREE (full_uri);
         HT_FREE (base_uri);
    }
    return status;
}


/* --------------------------------------------------------------------------*/
/*                            UNLOCK REQUESTS                                */
/* --------------------------------------------------------------------------*/

/*
** UNLOCK REQUEST
**
** UNLOCK request removes the lock identified by Lock-Token header from
** the Request-URI.
** Headers:
**      Lock-Token header must be present
*/
PUBLIC BOOL HTUNLOCKAnchor (HTRequest * request,
                            HTAnchor * dst,
                            HTDAVHeaders * headers) {
    if (request && dst) {
        /* set method and request-uri */
        HTRequest_setMethod (request,METHOD_UNLOCK);
        HTRequest_setAnchor (request,dst);
        HTTRACE (PROT_TRACE,"HTDAV.... Method set to UNLOCK\n");

        /* set headers */
        HTTRACE (PROT_TRACE,"HTDAV.... Setting default Headers \n");
        HTRequest_addCacheControl (request, "no-cache","");
        HTRequest_addEnHd (request,HT_E_CONTENT_ENCODING);
        HTRequest_addEnHd (request,HT_E_CONTENT_LENGTH);
        HTRequest_addGnHd (request,HT_G_PRAGMA_NO_CACHE);
 
        if (headers && headers->LockToken) {
            HTTRACE (PROT_TRACE,"HTDAV.... Seting Lock-Token \n");
            if (headers->LockToken == NULL) {
                HTTRACE (PROT_TRACE,"HTDAV.... FAILED Lock-Token is NULL\n");
                return NO;
            }
            HTRequest_addExtraHeader (request,"Lock-Token",headers->LockToken);

            return HTLoad (request,NO);
        }
    }
    return NO;
}


/*
** UNLOCK the resource indicated by an absolute URI, using the lock tocken
** defined in HTDAVHeaders * headers parameter.
*/
PUBLIC BOOL HTUNLOCKAbsolute (HTRequest * request,
                              const char * uri,
                              HTDAVHeaders * headers) {
 
    if (request && uri && *uri) {
        HTAnchor *dst = HTAnchor_findAddress (uri);
        return HTUNLOCKAnchor (request,dst,headers);
    }
    return NO;
}


/*
** UNLOCK the resource indicated by a relative URI, which is made
** absolute by using the base anchor.
*/
PUBLIC BOOL HTUNLOCKRelative (HTRequest * request,
                              const char * relative,
                              HTParentAnchor * base,
                              HTDAVHeaders * headers) {
    BOOL status = NO;
    if (request && relative && base) {
         char * base_uri = HTAnchor_address ((HTAnchor *)base);
         char * full_uri = HTParse (relative,base_uri,PARSE_ACCESS|PARSE_HOST| \
                                                 PARSE_PATH|PARSE_PUNCTUATION);

         status = HTUNLOCKAbsolute (request,full_uri,headers);
         HT_FREE (full_uri);
         HT_FREE (base_uri);
    }
    return status;
}


/* --------------------------------------------------------------------------*/
/*                            PROPFIND REQUESTS                              */
/* --------------------------------------------------------------------------*/


/*
** PROPFIND Requests
** PROPFIND requests returns properties defined for the resource.
** The request may contain xml message body with a "propfind" element,
** which may include an "allprop" element (to get all properties), a
** "propname" element (the name of all properties defined), and a "prop"
** element containing the desired properties.
** Headers:
**      Depth header may be "0", "1" or "infinity".
*/
PUBLIC BOOL HTPROPFINDDocumentAnchor (HTRequest * request,
                                      HTAnchor * dst,
                                      HTParentAnchor * xmlbody,
                                      HTDAVHeaders * headers) {
 
    if (request && dst) {
        /* set method and request-URI */
        HTRequest_setMethod (request,METHOD_PROPFIND);
        HTRequest_setAnchor (request,dst);
        HTTRACE (PROT_TRACE,"HTDAV.... Method set to PROPFIND\n");
 
        /* set headers */
        HTTRACE (PROT_TRACE,"HTDAV.... Seting default Headers \n");
        HTRequest_addCacheControl (request,"no-cache","");
        HTRequest_addGnHd (request,HT_G_PRAGMA_NO_CACHE);
 
        if (headers) { /* WebDAV specific headers */
            HTTRACE (PROT_TRACE,"HTDAV.... Setting WebDAV headers \n");
            if (headers->Depth) /* only Depth header may be used */
                if (!strcasecomp(headers->Depth,"0") || /* values 0, 1 */
                    !strcasecomp(headers->Depth,"1") || /* or infinity */
                    !strcasecomp(headers->Depth,"infinity"))
                        HTRequest_addExtraHeader(request,"Depth",headers->Depth);
        }

        /* set body - if there is a body */
        if (xmlbody) {
            HTTRACE (PROT_TRACE,"HTDAV.... Setting WebDAV Entity Request Body \n");
            HTRequest_setEntityAnchor (request,xmlbody); 
            HTRequest_setPostCallback (request, HTEntity_callback);
        }

        return HTLoad (request,NO);
    }

    return NO;
}



PUBLIC BOOL HTPROPFINDAnchor (HTRequest * request,
                              HTAnchor * dst,
                              const char * xmlbody,
                              HTDAVHeaders * headers) {
    if (request && dst) {
        HTParentAnchor * body = NULL;
        if (xmlbody) {    
            body = HTTmpAnchor(NULL);
            HTAnchor_setDocument(body, (void *)xmlbody);
            HTAnchor_setFormat(body, HTAtom_for ("text/xml"));
            HTAnchor_setLength(body, strlen(xmlbody));        
        }
        return HTPROPFINDDocumentAnchor (request,dst,body,headers);
    }

    return NO;
}


/*
** This PROPFIND request returns the properties of the resource 
** indicated by the absolute URI (parameter uri). 
*/
PUBLIC BOOL HTPROPFINDAbsolute (HTRequest * request,
                                const char * uri,
                                const char * xmlbody,
                                HTDAVHeaders * headers) {
    if (request && uri && *uri) {
        HTAnchor *dst = HTAnchor_findAddress (uri);
        return HTPROPFINDAnchor (request,dst,xmlbody,headers);
    }
    return NO;
}


/*
** This PROPFIND request returns the properties of the resource 
** indicated by a relative URI, which is made absolute by using 
** the base anchor.
*/
PUBLIC BOOL HTPROPFINDRelative (HTRequest * request, 
                                const char * relative,
                                HTParentAnchor * base,
                                const char * xmlbody,
                                HTDAVHeaders * headers) {
    BOOL status = NO;
    if (request && relative && base) {
         char * base_uri = HTAnchor_address ((HTAnchor *)base);
         char * full_uri = HTParse(relative,base_uri,PARSE_ACCESS|PARSE_HOST| \
                                                 PARSE_PATH|PARSE_PUNCTUATION);

         status = HTPROPFINDAbsolute (request,full_uri,xmlbody,headers);
         HT_FREE (full_uri);
         HT_FREE (base_uri);
    }
    return status;
}



/* --------------------------------------------------------------------------*/
/*                            PROPPATCH REQUESTS                             */
/* --------------------------------------------------------------------------*/


/*
** PROPPATCH Requests
** PROPPATCH requests sets/removes the properties values for the resource.
** The request must contain a xml message body with a "propertyupdate" element,
** which may include an "set" element (to set the properties value) or
** a "remove" element (to remove the properties).
** Headers: (the RFC is not very clair about it)
**      If header, indicating a state token for the resource.
*/
PUBLIC BOOL HTPROPPATCHDocumentAnchor (HTRequest * request,
                                      HTAnchor * dst,
                                      HTParentAnchor * xmlbody,
                                      HTDAVHeaders * headers) {
        
    if (request && dst && xmlbody) {

        /* set method and request-URI */
        HTRequest_setMethod (request,METHOD_PROPPATCH);
        HTRequest_setAnchor (request,dst);
        HTTRACE (PROT_TRACE,"HTDAV.... Method set to PROPPATCH\n");

        /* set headers */
        HTTRACE (PROT_TRACE,"HTDAV.... Seting default Headers \n");
        HTRequest_addCacheControl (request,"no-cache","");
        HTRequest_addGnHd (request,HT_G_PRAGMA_NO_CACHE);

        if (headers) { /* WebDAV specific headers*/
            HTTRACE (PROT_TRACE,"HTDAV.... Setting WebDAV headers \n");
            if (headers->If) 
                HTRequest_addExtraHeader (request,"If",headers->If);
        }

        /* set body - mandatory! */
        HTTRACE (PROT_TRACE,"HTDAV.... Setting Entity Body \n");
        HTRequest_setEntityAnchor (request,xmlbody); 
        HTRequest_setPostCallback (request, HTEntity_callback);

        return HTLoad (request,NO);
    }
    return NO;
 
}


 
PUBLIC BOOL HTPROPPATCHAnchor (HTRequest * request,
                               HTAnchor * dst,
                               const char * xmlbody,
                               HTDAVHeaders * headers) {
    if (request && dst && xmlbody) {
       HTParentAnchor * body = HTTmpAnchor(NULL);
       HTAnchor_setDocument (body, (void *)xmlbody);
       HTAnchor_setFormat (body, HTAtom_for ("text/xml"));
       HTAnchor_setLength (body, strlen(xmlbody));
               
       return HTPROPPATCHDocumentAnchor (request,dst,body,headers);
    }
    return NO;
}


/*
** This PROPPATCH request set or removes properties from the resource 
** indicated by the absolute URI (parameter uri). 
*/
PUBLIC BOOL HTPROPPATCHAbsolute (HTRequest * request,
                                const char * uri,
                                const char * xmlbody,
                                HTDAVHeaders * headers) {
    if (request && uri && *uri && xmlbody && *xmlbody) {
        HTAnchor *dst = HTAnchor_findAddress (uri);
        return HTPROPPATCHAnchor (request,dst,xmlbody,headers);
    }
    return NO;
}


/*
** This PROPPATCH request sets/removes the properties from the resource 
** indicated by a relative URI, which is made absolute by using 
** the base anchor.
*/
PUBLIC BOOL HTPROPPATCHRelative (HTRequest * request, 
                                const char * relative,
                                HTParentAnchor * base,
                                const char * xmlbody,
                                HTDAVHeaders * headers) {
    BOOL status = NO;
    if (request && relative && base && xmlbody && *xmlbody) {
         char * base_uri = HTAnchor_address ((HTAnchor *)base);
         char * full_uri = HTParse (relative,base_uri,PARSE_ACCESS|PARSE_HOST| \
                                                 PARSE_PATH|PARSE_PUNCTUATION);

         status = HTPROPPATCHAbsolute (request,full_uri,xmlbody,headers);
         HT_FREE (full_uri);
         HT_FREE (base_uri);
    }
    return status;
}


/* --------------------------------------------------------------------------*/
/*                             MKCOL REQUESTS                                */
/* --------------------------------------------------------------------------*/


/*
** MKCOL Requests
** MKCOL requests creates a collection. The resource indicated by HTAnchor *
** dst parameter must not be a "non-null" resource, but all it ancestors
** must exist.
** Headers:
**      If header may be used.
*/
PUBLIC BOOL HTMKCOLAnchor (HTRequest * request, 
                           HTAnchor * dst,
                           HTDAVHeaders * headers) {
    if (request && dst) {
        /* set method and request-URI */
        HTRequest_setMethod (request,METHOD_MKCOL);
        HTRequest_setAnchor (request,dst);
        HTTRACE (PROT_TRACE,"HTDAV.... Method set to MKCOL\n");
        
        /* set headers */
        HTTRACE (PROT_TRACE,"HTDAV.... Seting default Headers \n");
        HTRequest_addCacheControl (request,"no-cache","");
        HTRequest_addGnHd (request,HT_G_PRAGMA_NO_CACHE);
        
        if (headers) { /* WebDAV specific headers */
            HTTRACE (PROT_TRACE,"HTDAV.... Setting WebDAV headers \n");
            if (headers->If) /* only IF header may be used */
                HTRequest_addExtraHeader (request,"If",headers->If);
        }

        return HTLoad (request,NO);
    }

    return NO;
}



/*
** This MKCOL request tries to create the resource 
** indicated by the absolute URI (parameter uri). 
*/
PUBLIC BOOL HTMKCOLAbsolute (HTRequest * request,
                             const char * uri,
                             HTDAVHeaders * headers) {
    if (request && uri && *uri) {
        HTAnchor *dst = HTAnchor_findAddress (uri);
        return HTMKCOLAnchor (request,dst,headers);
    }
    return NO;
}


/*
** This MKCOL request tries to create the resource indicated
** by a relative URI, which is made absolute by using the
** base anchor.
*/
PUBLIC BOOL HTMKCOLRelative (HTRequest * request, 
                             const char * relative,
                             HTParentAnchor * base,
                             HTDAVHeaders * headers) {
    BOOL status = NO;
    if (request && relative && base) {
         char * base_uri = HTAnchor_address ((HTAnchor *)base);
         char * full_uri = HTParse (relative,base_uri,PARSE_ACCESS|PARSE_HOST| \
                                                 PARSE_PATH|PARSE_PUNCTUATION);

         status = HTMKCOLAbsolute (request,full_uri,headers);
         HT_FREE (full_uri);
         HT_FREE (base_uri);
    }
    return status;
}



/* --------------------------------------------------------------------------*/
/*                              COPY REQUESTS                                */
/* --------------------------------------------------------------------------*/


/*
** COPY Requests
** COPY requests copies the Request-URI resource (indicated by the parameter
** HTAnchor *src) to the resource indicated by the Destination header (it must
** be set in HTDAVHeaders object - so, this object must NOT be NULL).
** A xml message body may also be set, with the propertybehavior xml element,
** which indicates what should be the server behavior when copying the resouce
** properties.
** Headers:
**      Destination header is mandatory!
**      If header may be used.
**      Depth header may be "0" or "infinity"
**      Overwrite header may be used
*/
PUBLIC BOOL HTCOPYDocumentAnchor (HTRequest * request,
                                  HTAnchor * src,
                                  HTParentAnchor * xmlbody,
                                  HTDAVHeaders * headers) {
        
    if (request && src && headers) {

        /* set method and request-URI */
        HTRequest_setMethod (request,METHOD_COPY);
        HTRequest_setAnchor (request,src);
        HTTRACE (PROT_TRACE,"HTDAV.... Method set to COPY\n");

        /* set headers */
        HTTRACE (PROT_TRACE,"HTDAV.... Seting default Headers \n");
        HTRequest_addCacheControl (request,"no-cache","");
        HTRequest_addGnHd (request,HT_G_PRAGMA_NO_CACHE);

         /* WebDAV specific headers - Destination is mandatory! */
        if (headers->Destination && *headers->Destination) { 
            HTTRACE (PROT_TRACE,"HTDAV.... Setting WebDAV headers \n");
            HTRequest_addExtraHeader (request,"Destination",headers->Destination);

            if (headers->If) /* If header may be used */
                HTRequest_addExtraHeader (request,"If",headers->If);

            if (headers->Overwrite != ' ') {
                char over[] = { headers->Overwrite, '\0' };
                HTRequest_addExtraHeader (request,"Overwirte", over );
             }

            if (headers->Depth) {
                if (!strcasecomp (headers->Depth,"0") || 
                    !strcasecomp (headers->Depth,"infinity")) 
                      HTRequest_addExtraHeader (request,"Depth",headers->Depth);
            }
        } 
        else return NO;

        /* set body - if there is a body */
        if (xmlbody) {
            HTTRACE (PROT_TRACE,"HTDAV.... Setting Entity Body \n");
            HTRequest_setEntityAnchor (request,xmlbody); 
            HTRequest_setPostCallback (request, HTEntity_callback);
        }
        return HTLoad (request,NO);
    }

    return NO;
}



PUBLIC BOOL HTCOPYAnchor (HTRequest * request, 
                           HTAnchor * src,
                           const char * xmlbody,
                           HTDAVHeaders * headers) {
     
    if (request && src && headers) {
        HTParentAnchor * body = NULL;
        if (xmlbody) {
            body = HTTmpAnchor(NULL);
            HTAnchor_setDocument (body, (void *)xmlbody);
            HTAnchor_setFormat (body, HTAtom_for ("text/xml"));
            HTAnchor_setLength (body, strlen(xmlbody));
        }
        return HTCOPYDocumentAnchor (request,src,body,headers);  
    }
    return NO;
}


/*
** This COPY request copies the resource indicated by an absolute URI
** (parameter uri) to the URI in Destination header. 
*/
PUBLIC BOOL HTCOPYAbsolute (HTRequest * request,
                            const char * uri,
                            const char * xmlbody,
                            HTDAVHeaders * headers) {
    if (request && uri && *uri && headers) {
        HTAnchor *src = HTAnchor_findAddress (uri);
        return HTCOPYAnchor (request,src,xmlbody,headers);
    }
    return NO;
}


/*
** This COPY request copies the resource indicated by a relative URI,
** which is made absolute by using the base anchor. 
*/
PUBLIC BOOL HTCOPYRelative (HTRequest * request, 
                                const char * relative,
                                HTParentAnchor * base,
                                const char * xmlbody,
                                HTDAVHeaders * headers) {
    BOOL status = NO;
    if (request && relative && base && headers) {
         char * base_uri = HTAnchor_address ((HTAnchor *)base);
         char * full_uri = HTParse (relative,base_uri,PARSE_ACCESS|PARSE_HOST| \
                                                 PARSE_PATH|PARSE_PUNCTUATION);

         status = HTCOPYAbsolute (request,full_uri,xmlbody,headers);
         HT_FREE (full_uri);
         HT_FREE (base_uri);
    }
    return status;
}



/* --------------------------------------------------------------------------*/
/*                              MOVE REQUESTS                                */
/* --------------------------------------------------------------------------*/


/*
** MOVE Requests
** MOVE requests moves the Request-URI resource (indicated by the parameter
** HTAnchor *src) to the resource indicated by the Destination header (it must
** be set in HTDAVHeaders object - so, this object must NOT be NULL).
** A xml message body may also be set, with the propertybehavior xml element,
** which indicates what should be the server behavior when copying the resouce
** properties.
** Headers:
**      Destination header is mandatory!
**      If header may be used.
**      Depth header may be "0" or "infinity" (for collections, it MUST be "infinity")
**      Overwrite header may be used
*/
PUBLIC BOOL HTMOVEDocumentAnchor (HTRequest * request,
                                  HTAnchor * src,
                                  HTParentAnchor * xmlbody,
                                  HTDAVHeaders * headers) {

    if (request && src && headers) {

        /* set method and request-URI */
        HTRequest_setMethod (request,METHOD_MOVE);
        HTRequest_setAnchor (request,src);
        HTTRACE (PROT_TRACE,"HTDAV.... Method set to MOVE\n");

        /* set headers */
        HTTRACE (PROT_TRACE,"HTDAV.... Seting default Headers \n");
        HTRequest_addCacheControl (request,"no-cache","");
        HTRequest_addGnHd (request,HT_G_PRAGMA_NO_CACHE);

         /* WebDAV specific headers - Destination is mandatory! */
        if (headers->Destination && *headers->Destination) { 
            HTTRACE (PROT_TRACE,"HTDAV.... Setting WebDAV headers \n");
            HTRequest_addExtraHeader (request,"Destination",headers->Destination);

            if (headers->If) /* If header may be used */
                HTRequest_addExtraHeader (request,"If",headers->If);

            if (headers->Overwrite != ' ') {
                char over[] = { headers->Overwrite, '\0' };
                HTRequest_addExtraHeader (request,"Overwirte", over );
             }

            if (headers->Depth) {
                if (!strcasecomp (headers->Depth,"0") || 
                    !strcasecomp (headers->Depth,"infinity")) 
                     HTRequest_addExtraHeader(request,"Depth",headers->Depth);
            }
        } 
        else return NO;
        
        /* set body - if there is a body */
        if (xmlbody) {
            HTTRACE (PROT_TRACE,"HTDAV.... Setting Entity Body \n");
            HTRequest_setEntityAnchor (request,xmlbody); 
            HTRequest_setPostCallback (request,HTEntity_callback);              
        }       
        return HTLoad (request,NO);
    }

    return NO;  
}



PUBLIC BOOL HTMOVEAnchor (HTRequest * request, 
                          HTAnchor * src,
                          const char * xmlbody,
                          HTDAVHeaders * headers) {
 
    if (request && src && headers) {
        HTParentAnchor * body = NULL;
        if (xmlbody) {
            body = HTTmpAnchor (NULL);
            HTAnchor_setDocument(body, (void *) xmlbody);
            HTAnchor_setFormat (body, HTAtom_for ("text/xml"));
            HTAnchor_setLength (body, strlen(xmlbody));
        }    
        return HTMOVEDocumentAnchor (request,src,body,headers);
    }
    return NO;
}


/*
** This MOVE request moves the resource indicated by an absolute URI
** (parameter uri) to the URI in Destination header. 
*/
PUBLIC BOOL HTMOVEAbsolute (HTRequest * request,
                            const char * uri,
                            const char * xmlbody,
                            HTDAVHeaders * headers) {
    if (request && uri && *uri && headers) {
        HTAnchor *src = HTAnchor_findAddress (uri);
        return HTMOVEAnchor (request,src,xmlbody,headers);
    }
    return NO;
}


/*
** This MOVE request moves the resource indicated by a relative URI,
** which is made absolute by using the base anchor. 
*/
PUBLIC BOOL HTMOVERelative (HTRequest * request, 
                            const char * relative,
                            HTParentAnchor * base,
                            const char * xmlbody,
                            HTDAVHeaders * headers) {
    BOOL status = NO;
    if (request && relative && base && headers) {
         char * base_uri = HTAnchor_address ((HTAnchor *)base);
         char * full_uri = HTParse (relative,base_uri,PARSE_ACCESS|PARSE_HOST| \
                                                 PARSE_PATH|PARSE_PUNCTUATION);

         status = HTMOVEAbsolute (request,full_uri,xmlbody,headers);
         HT_FREE (full_uri);
         HT_FREE (base_uri);
    }
    return status;
}


#endif /* HT_DAV */
