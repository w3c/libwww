/*								     HTMethod.c
**	MANAGES REQUEST METHODS
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**
** HISTORY:
**	6 June 95  HFN	Spawned off from HTAccess. Can be extended to allow
**			registration of new methods
**      15 Feb 2002 MKP Introduced methods from WebDAV protocol and 
**                      Extension Methods. (Manuele.Kirsch_Pinheiro@inrialpes.fr
**                      or manuele@inf.ufrgs.br)
**
*/

/* Library Include files */
#include "wwwsys.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTMethod.h"					 /* Implemented here */

PRIVATE char *method_names[] =
{
    "INVALID-METHOD",
    "GET",
    "HEAD",
    "POST",
    "PUT",
    "PATCH",
    "DELETE",
    "TRACE",
    "OPTIONS",
    "LINK",
    "UNLINK",
#ifdef HT_DAV
    "LOCK",                                             /* new methods for */
    "UNLOCK",                                           /* webDAV protocol */
    "PROPFIND",
    "PROPPATCH",
    "MKCOL",
    "COPY",
    "MOVE",
#endif /* HT_DAV */

#ifdef HT_EXT                                           /* extension methods */
    NULL,                                               /* METHOD_EXT_0 */
    NULL,                                               /* METHOD_EXT_1 */
    NULL,                                               /* METHOD_EXT_2 */
    NULL,                                               /* METHOD_EXT_3 */
    NULL,                                               /* METHOD_EXT_4 */
    NULL,                                               /* METHOD_EXT_5 */
    NULL,                                               /* METHOD_EXT_6 */    
#endif /* HT_EXT  */

    NULL
};


#ifdef HT_EXT
/* This array keeps the information about the presence of
** an entity body in each extension method
*/
PRIVATE BOOL HTEXT_EntityMethods [] = {
    NO,         /* METHOD_EXT_0 */
    NO,         /* METHOD_EXT_1 */
    NO,         /* METHOD_EXT_2 */
    NO,         /* METHOD_EXT_3 */
    NO,         /* METHOD_EXT_4 */
    NO,         /* METHOD_EXT_5 */
    NO          /* METHOD_EXT_6 */
};
#endif

/* ------------------------------------------------------------------------- */


/*	Get method enum value
**	---------------------
*/
PUBLIC HTMethod HTMethod_enum (const char * name)
{
    if (name) {
	if (!strcasecomp(name, *(method_names+1)))
	    return METHOD_GET;
	else if (!strcasecomp(name, *(method_names+2)))
	    return METHOD_HEAD;
	else if (!strcasecomp(name, *(method_names+3)))
	    return METHOD_POST;
	else if (!strcasecomp(name, *(method_names+4)))
	    return METHOD_PUT;
	else if (!strcasecomp(name, *(method_names+5)))
	    return METHOD_PATCH;
	else if (!strcasecomp(name, *(method_names+6)))
	    return METHOD_DELETE;
	else if (!strcasecomp(name, *(method_names+7)))
	    return METHOD_TRACE;
	else if (!strcasecomp(name, *(method_names+8)))
	    return METHOD_OPTIONS;
	else if (!strcasecomp(name, *(method_names+9)))
	    return METHOD_LINK;
	else if (!strcasecomp(name, *(method_names+10)))
	    return METHOD_UNLINK;
#ifdef HT_DAV
        else if (!strcasecomp(name, *(method_names+11)))        /* WebDAV lock/unlock */
            return METHOD_LOCK;
        else if (!strcasecomp(name, *(method_names+12)))
            return METHOD_UNLOCK;

        else if (!strcasecomp(name, *(method_names+13)))        /* other WebDAV methods */
            return METHOD_PROPFIND;
        else if (!strcasecomp(name, *(method_names+14)))
            return METHOD_PROPPATCH;    
        else if (!strcasecomp(name, *(method_names+15)))
            return METHOD_MKCOL;
        else if (!strcasecomp(name, *(method_names+16)))
            return METHOD_COPY;
        else if (!strcasecomp(name, *(method_names+17)))        
            return METHOD_MOVE;
#endif /* HT_DAV */

#ifdef HT_EXT
                      /* if there is no extension method set, return METHOD_INVALID */
        else if ( *(method_names+18) && !strcasecomp(name, *(method_names+18)))
            return METHOD_EXT_0;                                
        else if ( *(method_names+19) && !strcasecomp(name, *(method_names+19)))
            return METHOD_EXT_1;
        else if ( *(method_names+20) && !strcasecomp(name, *(method_names+20)))
            return METHOD_EXT_2;
        else if ( *(method_names+21) && !strcasecomp(name, *(method_names+21)))
            return METHOD_EXT_3;
        else if ( *(method_names+22) && !strcasecomp(name, *(method_names+22)))
            return METHOD_EXT_4;
        else if ( *(method_names+23) && !strcasecomp(name, *(method_names+23)))
            return METHOD_EXT_5;
        else if ( *(method_names+24) && !strcasecomp(name, *(method_names+24)))
            return METHOD_EXT_6;
#endif  
    }
    return METHOD_INVALID;
}



/*	Get method name
**	---------------
**	Returns pointer to entry in static table in memory
*/
PUBLIC const char * HTMethod_name (HTMethod method)
{
    if (method & METHOD_GET)
	return *(method_names+1);
    else if (method == METHOD_HEAD)
	return *(method_names+2);
    else if (method == METHOD_POST)
	return *(method_names+3);
    else if (method == METHOD_PUT)
	return *(method_names+4);
    else if (method == METHOD_PATCH)
	return *(method_names+5);
    else if (method == METHOD_DELETE)
	return *(method_names+6);
    else if (method == METHOD_TRACE)
	return *(method_names+7);
    else if (method == METHOD_OPTIONS)
	return *(method_names+8);
    else if (method == METHOD_LINK)
	return *(method_names+9);
    else if (method == METHOD_UNLINK)
	return *(method_names+10);
    
#ifdef HT_DAV    
    else if (method == METHOD_LOCK)                     /* webdav lock/unlock */
        return *(method_names+11);                      /* methods (locking)  */
    else if (method == METHOD_UNLOCK)
        return *(method_names+12);
    else if (method == METHOD_PROPFIND)                 /* webdav proppatch/propfind */
        return *(method_names+13);                      /* methods (properties)      */
    else if (method == METHOD_PROPPATCH)
        return *(method_names+14);
     else if (method == METHOD_MKCOL)                   /* webdav mkcol method */
        return *(method_names+15);                      /* (collencions)       */
    else if (method == METHOD_COPY)     
        return *(method_names+16);                      /* webdav copy/move    */               
    else if (method == METHOD_MOVE)                     /* methods (namespace) */
        return *(method_names+17);
#endif /* HT_DAV */

#ifdef HT_EXT    
    else if (method == METHOD_EXT_0 && (*(method_names+18)))    /* extension methods */
        return *(method_names+18);                              /* customizable for the app. */
    else if (method == METHOD_EXT_1 && (*(method_names+19)))
        return *(method_names+19);
    else if (method == METHOD_EXT_2 && (*(method_names+20)))    /* if the method is not set */
        return *(method_names+20);                              /* then it will return      */
    else if (method == METHOD_EXT_3 && ((*method_names+21)))    /* "INVALID-METHOD"         */
        return *(method_names+21);
    else if (method == METHOD_EXT_4 && ((*method_names+22)))
        return *(method_names+22);
    else if (method == METHOD_EXT_5 && ((*method_names+23)))
        return *(method_names+23);
    else if (method == METHOD_EXT_6 && ((*method_names+24)))
        return *(method_names+24);
#endif /* HT_EXT */
    
    else
	return *method_names;
}


/* 
** Does the method include an entity body? 
*/
PUBLIC BOOL HTMethod_hasEntity (HTMethod me) {
    if (me) {
        /* normal HTTP methods */
        if (me & (METHOD_PUT | METHOD_POST)) return YES;

#ifdef HT_DAV
        /* WebDAV methods */
        if (me & (METHOD_LOCK | METHOD_PROPFIND | METHOD_PROPPATCH |
            METHOD_COPY | METHOD_MOVE)) return YES;
#endif

#ifdef HT_EXT
        /* extension methods */
        HTTRACE (PROT_TRACE, "HTMethod.... Has this extension method a body?\n");
        if (me == METHOD_EXT_0) return HTEXT_EntityMethods[0];
        else if (me == METHOD_EXT_1) return HTEXT_EntityMethods[1];
        else if (me == METHOD_EXT_2) return HTEXT_EntityMethods[2];
        else if (me == METHOD_EXT_3) return HTEXT_EntityMethods[3];
        else if (me == METHOD_EXT_4) return HTEXT_EntityMethods[4];
        else if (me == METHOD_EXT_5) return HTEXT_EntityMethods[5];
        else if (me == METHOD_EXT_6) return HTEXT_EntityMethods[6];
        else return (NO);
#endif
    }
    return (NO);
}



/*
** Set an extension method: HTMethod structure has 7 methods (METHOD_EXT_0
** to METHOD_EXT_6) that can be set by the application.
** This function set one of this extension methods. 
**
** Parameters: 
**             HTMethod method - extension method to be set
**             const char name - new name for the method
** Return:
**         BOOL - YES, if the operation succeed
**              - NO, if it failed, for example, if the parameter method is
**                                  not a extension method (METHOD_EXT_0 -
**                                  METHOD_EXT_6)
**
*/
PUBLIC BOOL HTMethod_setExtensionMethod (HTMethod method, const char * name, BOOL hasEntity) {
#ifdef HT_EXT
    if (name && *name) {
        if (method == METHOD_EXT_0) { 
            StrAllocCopy ((char *)(*(method_names+18)), name);
            HTEXT_EntityMethods[0] = hasEntity;
        }
        else if (method == METHOD_EXT_1) {
            StrAllocCopy ((char *)(*(method_names+19)), name);
            HTEXT_EntityMethods[1] = hasEntity;
        }
        else if (method == METHOD_EXT_2) {
            StrAllocCopy ((char *)(*(method_names+20)), name);
            HTEXT_EntityMethods[2] = hasEntity;
        }
        else if (method == METHOD_EXT_3) {
            StrAllocCopy ((char *)(*(method_names+21)), name);
            HTEXT_EntityMethods[3] = hasEntity;
        }
        else if (method == METHOD_EXT_4) {
            StrAllocCopy ((char *)(*(method_names+22)), name);
            HTEXT_EntityMethods[4] = hasEntity;
        }
        else if (method == METHOD_EXT_5) {
            StrAllocCopy ((char *)(*(method_names+23)), name);
            HTEXT_EntityMethods[5] = hasEntity;
        }
        else if (method == METHOD_EXT_6) {
            StrAllocCopy ((char *)(*(method_names+24)), name);  
            HTEXT_EntityMethods[6] = hasEntity;
        }
        else return NO;
        return YES;     
    }   
#endif /* HT_EXT */

    return NO;    
}


/*
** Delete the extension method: this funcion unset the extension method
*/
PUBLIC BOOL HTMethod_deleteExtensionMethod (HTMethod method) {
#ifdef HT_EXT
    char *ptr = NULL;   

    if ( (method == METHOD_EXT_0) && (*(method_names+18)) )  {
         ptr = (char *) (*(method_names+18));
         (*(method_names+18)) = NULL;
    }
    else if ( (method == METHOD_EXT_1) && (*(method_names+19)) ) {
        ptr = (char *) (*(method_names+19));
        (*(method_names+19)) = NULL;
    }   
    else if ( (method == METHOD_EXT_2) && (*(method_names+20)) ) {
        ptr = (char *) (*(method_names+20));
        (*(method_names+20)) = NULL;
    }
    else if ( (method == METHOD_EXT_3) && (*(method_names+21)) ) {
        ptr = (char *) (*(method_names+21));
        (*(method_names+21)) = NULL;
    }
    else if ( (method == METHOD_EXT_4) && (*(method_names+22)) ) {
        ptr = (char *) (*(method_names+22));
        (*(method_names+22)) = NULL;    
    }
    else if ( (method == METHOD_EXT_5) && (*(method_names+23)) ) {
        ptr = (char *) (*(method_names+23));
        (*(method_names+23)) = NULL;
    }
    else if ( (method == METHOD_EXT_6) && (*(method_names+24)) ) {
        ptr = (char *) (*(method_names+24));
        (*(method_names+24)) = NULL;
    }
    
    if (ptr) {
        HT_FREE(ptr);
        return YES;
    }
    
#endif /* HT_EXT */

    return NO;
}


