/*
** Libwww extension test file 
**
** This program shows how to use the extension methods available in Libwww. 
** Through these methods (METHOD_EXT_0 to METHOD_EXT_6), applications may
** register (by calling HTMethod_setExtensionMethod) new methods, that may be 
** unknown for the Libwww, and use them for its request. See HTMethod.html for
** details.
**
**        More libwww samples can be found at "http://www.w3.org/Library/Examples/"
**        
**        Copyright © 1995-1998 World Wide Web Consortium, (Massachusetts
**        Institute of Technology, Institut National de Recherche en
**        Informatique et en Automatique, Keio University). All Rights
**        Reserved. This program is distributed under the W3C's Software
**        Intellectual Property License. This program is distributed in the hope
**        that it will be useful, but WITHOUT ANY WARRANTY; without even the
**        implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
**        PURPOSE. See W3C License http://www.w3.org/Consortium/Legal/ for more
**        details.
**
**
** Authors: 
**        MKP     Manuele Kirsch Pinheiro (Manuele.Kirsch_Pinheiro@inrialpes.fr, manuele@inf.ufrgs.br)
**                _ Project CEMT  (INRIA Rhone-Alpes,France / UFRGS-II,Brazil) _
**
** History:
**        fev, 2002        created                MKP
**        may, 2002        modifications for      MKP
**                         windows plataform.         
**                                
** $Id$
*/

#include "WWWLib.h"
#include "WWWInit.h"
#include "WWWApp.h"
#include "WWWXML.h"
#include "WWWHTTP.h"


#ifndef W3C_VERSION
#define W3C_VERSION             "Unspecified"
#endif

#define APP_NAME                "MyExt"
#define APP_VERSION             "3.0"

#if defined(__svr4__)
#define CATCH_SIG
#endif


/*
** error codes for use in error_callback filter
*/
#define ERR_UNKNOWN             0x0
#define ERR_FATAL               0x1
#define ERR_NON_FATAL           0x2
#define ERR_WARN                0x4
#define ERR_INFO                0x8


/*
** Some compilers, like MSVC, doesn't know STDIN_FILENO
*/
#ifndef STDIN_FILENO
#define STDIN_FILENO           fileno(stdin)
#endif

/*
** Request context
*/ 
typedef struct _Context {
    HTChunk * chunk;
    int depth;
} Context;


/*
** Application context
*/ 
typedef struct _App {
    HTRequest *             console_request;
    HTEvent *               console_event;
    HTList *                active;                          /* List of active contexts */
    HTMethod                method;  
} App;


/*
** Callback functions 
*/
PRIVATE HTEventCallback console_parser;
PRIVATE HTNetAfter request_terminater;
PRIVATE HTNetAfter local_request_terminater; 
PRIVATE HTNetAfter local_request_first; 
PRIVATE HTNetAfter error_callback; 
PRIVATE int printer (const char * fmt, va_list pArgs);
PRIVATE int tracer (const char * fmt, va_list pArgs);


/*
** Prototypes
*/ 
PRIVATE App * App_new (void);
PRIVATE BOOL App_delete (App * me);
PRIVATE void Context_new (HTRequest * request, HTChunk * chunk, App * app); 
PRIVATE void Context_delete (Context * ctx);
PRIVATE HTRequest * Request_new (App * app);
PRIVATE BOOL Request_delete (App * app, HTRequest * request);
PRIVATE int request_terminater (HTRequest * request, HTResponse * response,
                                void * param, int status); 
PRIVATE int console_parser (SOCKET s, void * param, HTEventType type);
PRIVATE int local_request_terminater (HTRequest * request, HTResponse * response,
                                          void * param, int status); 
PRIVATE int local_request_first (HTRequest * request, HTResponse * response,
                                          void * param, int status); 
PRIVATE int error_callback (HTRequest * request, HTResponse * response,
                                          void * param, int status); 

PRIVATE App * Init (void);
PRIVATE void my_headers (HTRequest *request);
PRIVATE void my_get_document (App *app, char *dst);
PRIVATE char * create_body (void);


/* --------------------------------------------------------------------------*/


/* ------------------------------------------------------------------------- */
/*                                EXPAT HANDLERS                             */
/* ------------------------------------------------------------------------- */

PRIVATE void XML_startElement (void * userData,
                                      const XML_Char *name, 
                                      const XML_Char ** atts)
{
    int *depth = (int *)userData;
    int i=0;
    if (name) {
        for (i=0;i<(*depth);i++) 
            HTPrint (" ");
        HTPrint("%s: ", name);
        for (i = 0; atts[i]; i += 2)
            HTPrint(" %s='%s'", atts[i], atts[i + 1]);
        HTPrint ("\n");
    }
    *depth += 1;
}

PRIVATE void XML_endElement (void * userData,
                             const XML_Char * name)
{
    int *depth = (int *)userData;
    *depth -= 1;
}

PRIVATE void XML_characterData (void * userData,
                                const XML_Char * s, 
                                int len)
{
    int *depth = (int *) userData;
    int i=0;

    if (len>1) {
        for (i=-1;i<(*depth);i++) 
            HTPrint (" ");        
        for (i=0;i<len;i++) HTPrint ("%c",s[i]);
        HTPrint ("\n");
    }
    else {
        for (i=-1;i<(*depth);i++) 
            HTPrint (" ");        
        if (s[0]!='\n' && s[0]!=' ')
            HTPrint ("%c\n",s[0]);    
    }
    return;
}


PRIVATE void XML_processingInstruction (void * userData,
                                        const XML_Char * target,
                                        const XML_Char * data)
{
    return;
}


PRIVATE void XML_default (void * userData,
                          const XML_Char * s, 
                          int len)
{   
    return;
}


PRIVATE void XML_unparsedEntityDecl (void * userData,
                                     const XML_Char * entityName,
                                     const XML_Char * base,
                                     const XML_Char * systemId,
                                     const XML_Char * publicId,
                                     const XML_Char * notationName)
{
    return;
}

PRIVATE void XML_notationDecl (void * userData,
                               const XML_Char * notationName,
                               const XML_Char * base,
                               const XML_Char * systemId,
                               const XML_Char * publicId)
{
    return;
}

PRIVATE int XML_externalEntityRef (XML_Parser parser,
                                   const XML_Char * openEntityNames,
                                   const XML_Char * base,
                                   const XML_Char * systemId,
                                   const XML_Char * publicId)
{
    return 0;
}

PRIVATE int XML_unknownEncoding (void * encodingHandlerData,
                                 const XML_Char * name,
                                 XML_Encoding * info)
{
    return 0;
}

/* ------------------------------------------------------------------------- */
/*                             HTXML STREAM HANDLERS                             */
/* ------------------------------------------------------------------------- */

PRIVATE void HTXML_setHandlers (XML_Parser me, HTRequest *request)
{
    XML_SetElementHandler(me, XML_startElement, XML_endElement);
    XML_SetCharacterDataHandler(me, XML_characterData);
    XML_SetProcessingInstructionHandler(me, XML_processingInstruction);
    XML_SetDefaultHandler(me, XML_default);
    XML_SetUnparsedEntityDeclHandler(me, XML_unparsedEntityDecl);
    XML_SetNotationDeclHandler(me, XML_notationDecl);
    XML_SetExternalEntityRefHandler(me, XML_externalEntityRef);
    XML_SetUnknownEncodingHandler(me, XML_unknownEncoding, NULL);

    if (request && HTRequest_context(request)) {
        XML_SetUserData(me, &(((Context *)(HTRequest_context(request)))->depth) );
    }
}

PRIVATE void HTXML_newInstance (HTStream  * me,
                                HTRequest * request,
                                HTFormat    target_format,
                                HTStream  * target_stream,
                                XML_Parser  xmlparser,
                                void *      context)
{
    HTPrint ("MyExt: HTXML_newInstance\n");
    if (me && xmlparser) HTXML_setHandlers(xmlparser,request);
}


/* ------------------------------------------------------------------------- */

/*
** CRCLCleanUp: removes CR, LF and CRLF characters from the string
*/
PUBLIC void CRLFCleanUp (char *buf,int size) {
    int i;        
    if(buf && *buf) {            
        for (i=0;i<size;i++)
            /* \n = default C character for line break 
             * \010 \013 = line break from HTTP RFC
             * CR LF = local representation defined in HTUtils.h 
             * */
            if (buf[i]=='\n' ||  
                buf[i]=='\010' || buf[i]=='\013' || 
                buf[i]==CR || buf[i]==LF ) 
                 buf[i]=' ';            
    }        
}


/*
** printer : output messages function
*/
PRIVATE int printer (const char * fmt, va_list pArgs) {
    return (vfprintf(stderr, fmt, pArgs));
}


/*
** trace : trace messages function
*/
PRIVATE int tracer (const char * fmt, va_list pArgs) {
    return (vfprintf(stderr, fmt, pArgs));
}


/*
** App_new : creates a new application context
** Returns : App *        application context
*/ 
PRIVATE App * App_new (void) {
    App * me = NULL;
    
    if ((me = (App *) HT_CALLOC(1, sizeof(App))) == NULL)        
        HT_OUTOFMEM("App_new");

    /* setting everybody */
    me->console_request = HTRequest_new();
    me->console_event = HTEvent_new(console_parser, me, HT_PRIORITY_MAX, -1);
    me->active = HTList_new();

    /* Register stdin as our console */
    if (isatty(STDIN_FILENO)) 
        HTEventList_register(STDIN_FILENO, HTEvent_READ, me->console_event);
    
    /* Add our own request terminate handler */
    HTNet_addAfter(request_terminater, NULL, me, HT_ALL, HT_FILTER_LAST); 

    HTPrint ("MyExt: Application context created\n");
    
    /* Set Extension method 0 to PROPFIND */    
    HTPrint ("MyExt: Setting method %x to PROPFIND\n",METHOD_EXT_0);
    if (HTMethod_setExtensionMethod (METHOD_EXT_0, "PROPFIND",NO))
        me->method = METHOD_EXT_0;
    else {
        HTPrint ("MyExt: Extension method FAILED - using GET\n");
        me->method = METHOD_GET;
    }   
   
    HTPrint ("ENTER A URL: \n");
    return me;
}



/*
** App_delete : removes an application context object
** Parameter : App * app   application context to be removed
** Returns : BOOL          YES - operation succed
**                         NO - operation failed
*/                        
PRIVATE BOOL App_delete (App * me) {
    HTRequest * req = NULL;        

    HTPrint ("MyExt: Removing application context\n");

    if (me) {
            
        /* killing any remaining active requests */
        HTNet_killAll();

        /* freeing all remaining request objects */
        while (!HTList_isEmpty(me->active)) {
            req = (HTRequest *) HTList_nextObject (me->active);
            if (req)  Request_delete (me,req);
        }

        /* clean up everything */
        HTRequest_delete(me->console_request);
        HTEvent_delete (me->console_event);
        HTList_free (me->active);
        
        /* clean up extension method */
        HTPrint ("Removing extension method\n");        
        if (HTMethod_deleteExtensionMethod (METHOD_EXT_0))
            HTPrint ("MyExt: Extension method deleted \n");
        
        HT_FREE(me);

        /* stopping event loop */    
        HTEventList_stopLoop();
        
        /* Terminate libwww */
        HTProfile_delete();

        return YES;
    }
    return NO;
}


/*
** Context_new : creates a new request context
** Parameters : HTRequest * request
**              HTChunk * chunk
**              App * app
*/
PRIVATE void Context_new (HTRequest * request, HTChunk * chunk, App * app) {
    Context * ctx = NULL;

    if ( (ctx = (Context *) HT_CALLOC(1, sizeof(Context))) == NULL ) {
        App_delete (app);
        HT_OUTOFMEM("Contect_new");
    }

    HTPrint ("MyExt: Setting request context...\n");
    ctx->depth = 0;
    ctx->chunk = chunk;
    HTRequest_setContext(request,ctx);    
}


/*
** Context_delete : removes the request context
** Parameters : Context * ctx
*/
PRIVATE void Context_delete (Context * ctx) {
    if (ctx && ctx->chunk) {
        HTPrint ("MyExt: Removing request context...\n");
        HTChunk_delete (ctx->chunk);
        HT_FREE (ctx);
    }
}


/*
** Request_new : creates a new request 
** Parameters : App * app         application context
** Returns : HTRequest *
*/ 
PRIVATE HTRequest * Request_new (App * app)
{
          
    HTRequest * request = HTRequest_new();

    HTPrint ("MyExt: creating a new request\n");
    
    /* setting request format */
    HTRequest_setOutputFormat(request, HTAtom_for ("text/xml"));
    HTRequest_setOutputFormat(request, WWW_SOURCE);     

    HTRequest_addConnection (request,"close","");
    
    /* method */    
    HTPrint ("MyExt: Setting method in the request\n");
    if (app)
        HTRequest_setMethod (request,app->method);
    else {
        HTPrint ("MyExt: No App, using default method\n");
        HTRequest_setMethod (request,METHOD_GET);
    }
    
   
    /* some headers */
    HTRequest_addExtraHeader (request,"Depth","0");
    HTRequest_addCacheControl (request, "no-cache","");

    /* proxy - no cache */
    HTRequest_addGnHd (request,HT_G_PRAGMA_NO_CACHE); 

    /* set some local filters */
    HTRequest_addAfter (request, local_request_terminater, NULL, app, \
                        HT_ALL ,HT_FILTER_LAST , NO);
    HTRequest_addAfter (request, local_request_first, NULL, app, \
                        HT_ALL ,HT_FILTER_FIRST , NO);

    HTRequest_addAfter (request,error_callback, NULL, app, \
                         HT_ERROR, HT_FILTER_LAST, NO); 

    
    if (!app->active) app->active = HTList_new();
    HTList_addObject(app->active, request);

    HTRequest_setFlush(request, YES);

    /* register XML callback */
    HTXMLCallback_registerNew (HTXML_newInstance, NULL);
        
    return request;
}



/*
** Request_delete : removes a request 
** Parameters : App * app        application context
**                  HTRequest * request
** Returns : BOOL        YES if operation succeed
**                       NO if operation failed
*/
PRIVATE BOOL Request_delete (App * app, HTRequest * request)
{
    HTPrint ("MyExt: removing request\n");
    
    if (app && app->active && request) {
        HTPrint ("MyExt: Request deleted \n");        
        HTList_removeObject(app->active, request);
        
        HTRequest_delete(request);
        return YES;
    }
    return NO;
}



/*
** console_parser : treats the user's interaction on the console
** Function's type : HTEventCallback
*/ 
PRIVATE int console_parser (SOCKET s, void * param, HTEventType type)
{
    App * app = (App *) param;
    char buf[128];
    char * full_dst;
    char * cwd;

    /* reading console */
    if (!fgets(buf, sizeof(buf), stdin)) return HT_ERROR;

    if (toupper(buf[0]) == 'Q') {        /* Quit the program */
        App_delete(app);
        exit (0);
    }
    else { /* take the target address */
        CRLFCleanUp (buf,strlen(buf));
        HTPrint ("MyExt: Console readed **%s**\n",buf);

        cwd =  HTGetCurrentDirectoryURL();        
        full_dst = HTParse (buf,cwd,PARSE_ALL);    

        HTPrint ("Getting %s\n",full_dst);        /* load the document */
        my_get_document (app,full_dst);                

        HT_FREE (full_dst);
        HT_FREE (cwd);
    }


    return HT_OK;
}


/*
** request_terminater : global filter to delete the requests
** Funtion's type : HTNetAfter
*/ 
PRIVATE int request_terminater (HTRequest * request, HTResponse * response,
                                void * param, int status) 
{
    App * app = (App *) param;
    Context * ctx = NULL;
    
    HTPrint ("MyExt: GLOBAL FILTER (request_terminater)\n");
    
    ctx = (Context *) HTRequest_context(request);
    
    HTPrint ("MyExt: context object deleted\n");
    Context_delete (ctx);    
        
    Request_delete(app, request);
       
    HTPrint ("Ready! Enter a New URL: ");
    
    return HT_OK;
}


/*
** local_request_terminater : local after filter to treat the request's results
** Function's type : HTNetAfter
*/
PRIVATE int local_request_terminater (HTRequest * request, HTResponse * response,
                                void * param, int status) 
{
    Context * ctx = NULL;        
    
    HTPrint ("\tstatus: %d\n",status);
    
    if (response) {
        HTPrint ("\tContent-length: %d \n\tIs Cachable: %c\n\tis Cached: %c \n\tReason: %s\n", \
                   HTResponse_length(response),\
                  (HTResponse_isCachable(response))?'Y':'N',
                  (HTResponse_isCached(response,YES))?'Y':'N',
                  (HTResponse_reason(response))?HTResponse_reason(response):"NULL");
        HTPrint ("\tFormat : %s \n",(char *)HTAtom_name(HTResponse_format(response))); 
    }
    else  
        HTPrint ("\tResponse NULL\n");
   
    ctx = (Context *) HTRequest_context(request);
    if ( ctx && ctx->chunk && HTChunk_data(ctx->chunk))    
        HTPrint ("MyExt: Loaded: \n%s\n",HTChunk_data(ctx->chunk));            
    else
        HTPrint ("MyExt: No text/xml data available at Chunk\n");
        
    return HT_OK;
}


/*
** local_request_first : local after filter to treat the request's headers
** Function's type : HTNetAfter
*/
PRIVATE int local_request_first (HTRequest * request, HTResponse * response,
                                          void * param, int status) {
    my_headers (request);
    return HT_OK;
}


/*
** error_callback : global after filter to treat the request's errors
** Function's type : HTNetAfter
*/
PRIVATE int error_callback (HTRequest * request, HTResponse * response,
                                          void * param, int status) {

    App * app = (App *) param;
    HTList * error_list = NULL;
    HTError * error = NULL;
        
    HTPrint ("MyExt: ERROR CALLBACK\n");
    HTPrint ("\tapp %s \n\trequest %s \n\tresponse %s \n\tstatus %d\n", \
                     (app)?"OK":"NULL",(request)?"OK":"NULL",\
                     (response)?"OK":"NULL",status);
        
    if (request) {
        error_list = HTRequest_error (request);
        while (error_list && (error = (HTError *) HTList_nextObject(error_list))) {
            HTPrint ("\tError location %s\n",HTError_location(error));
            switch (HTError_severity(error)) {
                case ERR_UNKNOWN :
                        HTPrint ("\tSeverity : UNKNOWN\n");
                        break;
                        
                case ERR_FATAL :
                        HTPrint ("\tSeverity : FATAL\n");
                        break;
                        
                case ERR_NON_FATAL :
                        HTPrint ("\tSeverity : NON FATAL\n");
                        break;
                        
                case ERR_WARN :
                        HTPrint ("\tSeverity : WARN\n");
                        break;
                        
                case ERR_INFO :
                        HTPrint ("\tSeverity : INFO\n");
                        break;

                default :
                        HTPrint ("\tSeverity : %Xd\n",HTError_severity(error));
                        break;
            }        
        }
    }

    return HT_OK;
}


/*
** Init : application initialization 
** Returns : App *         application context
*/ 
PRIVATE App * Init (void) {
    App * app = NULL;

    HTPrint ("MyExt: starting application\n");

    /* Initiate libwww */
    HTProfile_newHTMLNoCacheClient(APP_NAME, APP_VERSION);

    HTTP_setConnectionMode(HTTP_11_NO_PIPELINING);


    app = App_new();
    
    /* Gotta set up our own traces */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);    
    
#if 0
    HTSetTraceMessageMask("sapol");
#endif
    
     HTEventList_newLoop();
    
    return app;
}


/*
** my_headers : shows anchor's headers
** Parameters : HTRequest * request
*/  
PRIVATE void my_headers (HTRequest *request) {
    HTAssoc * h = NULL;    
    HTAssocList * headers = NULL; 
    HTParentAnchor * anchor = NULL;

    HTPrint ("MyExt: Searching headers...\n");          
    
    anchor = HTRequest_anchor (request);
    headers = HTAnchor_header(anchor);
    
    h = HTAssocList_nextObject(headers);
    while (anchor && headers && h ) {
       HTPrint ("\t%s : %s\n",HTAssoc_name(h),HTAssoc_value(h));
       h = HTAssocList_nextObject(headers);
    }
}


/*
** my_get_document: retrives the document
** Parameters: App * app        application context
**             char * dst        URL destiny
*/             
PRIVATE void my_get_document (App *app, char *dst) {
    char * body = create_body();        
    HTRequest * request = NULL;
    HTChunk * chunk = NULL;

    HTPrint ("MyExt: loading document\n");

    request = Request_new (app);
    
    /* set body */
    HTRequest_setMessageBody(request,body);
    HTRequest_setMessageBodyFormat (request,HTAtom_for("text/xml"));
    HTRequest_setMessageBodyLength (request,strlen(body));
    
    if ( request && (chunk = HTLoadToChunk (dst,request))) {        
        HTPrint ("MyExt: setting context\n");
        Context_new (request,chunk,app);        
    }
    else
        HTPrint ("MyExt: load to chunk failed\n");    
    
    HT_FREE (body);
}



PRIVATE char * create_body (void) {
     char tmp[4096];
     char * body = NULL;
     char nl[3];
             
     sprintf (nl,"%c%c",CR,LF);     
                    
     sprintf (tmp,"<?xml version=\"1.0\" encoding=\"utf-8\" ?>");
     strcat (tmp,nl);
     strcat (tmp,"<D:propfind xmlns:D=\"DAV:\">");
     strcat (tmp,nl);
     strcat (tmp,"    <D:allprop />");
     strcat (tmp,nl);
     strcat (tmp,"</D:propfind>");

     if ( (body = HT_MALLOC (strlen(tmp)+4)) != NULL)
         sprintf (body,"%s%c",tmp,'\0');
     
     return body;        
}


/* ******************************************************************** */
int main (int argc, char ** argv)
{
    App * app = NULL;
    
    app = Init();

    return 0;
}
