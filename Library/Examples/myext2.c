/*
** Libwww extension test file 
**
** This program shows how to use the extension methods available in Libwww. 
** Through these methods (METHOD_EXT_0 to METHOD_EXT_6), applications may
** register (by calling HTMethod_setExtensionMethod) new methods, that may be 
** unknown for the Libwww, and use them for its request. See HTMethod.html for
** details.
**
**      More libwww samples can be found at "http://www.w3.org/Library/Examples/"
**      
**      Copyright © 1995-1998 World Wide Web Consortium, (Massachusetts
**      Institute of Technology, Institut National de Recherche en
**      Informatique et en Automatique, Keio University). All Rights
**      Reserved. This program is distributed under the W3C's Software
**      Intellectual Property License. This program is distributed in the hope
**      that it will be useful, but WITHOUT ANY WARRANTY; without even the
**      implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
**      PURPOSE. See W3C License http://www.w3.org/Consortium/Legal/ for more
**      details.
**
**
** Authors: 
**      MKP     Manuele Kirsch Pinheiro (Manuele.Kirsch_Pinheiro@inrialpes.fr, 
**                                       manuele@inf.ufrgs.br)
**              _ Project CEMT  (INRIA Rhone-Alpes,France / UFRGS-II,Brazil) _
**
** History:
**      fev, 2002       created         MKP
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
#define APP_VERSION             "4.0"

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


/* commands */
#define VIEW    1
#define SET     2
#define DEL     3


#define PROPPATCH       METHOD_EXT_0
#define PROPFIND        METHOD_EXT_1


/*
** Our stream
*/
struct _HTStream {
    const HTStreamClass *       isa;
    HTStream *                  target;
    HTRequest *                 request;
    int                         version;
    BOOL                        endHeader;
    BOOL                        transparent;
};



/*
** Command line structure
*/
typedef struct _CmdLine {
    int cmd;    /* command */
    char *url;  /* url */
    char *prop; /* property name */
    char *ns;   /* property namespace */
    char *val;  /* property val */
} CmdLine;


/*
** Request context
*/ 
typedef struct _Context {
    int depth;
} Context;


/*
** Application context
*/ 
typedef struct _App {
    HTRequest *         console_request;
    HTEvent *           console_event;
    HTList *            active;                   /* List of active contexts */
} App;


/*
** Callback functions 
*/
PRIVATE HTEventCallback console_parser;
PRIVATE HTNetAfter request_terminater;
PRIVATE HTNetAfter error_callback; 
PRIVATE int printer (const char * fmt, va_list pArgs);
PRIVATE int tracer (const char * fmt, va_list pArgs);


/*
** Prototypes
*/ 
PRIVATE CmdLine * CmdLine_new (const char *buffer);
PRIVATE BOOL CmdLine_delete (CmdLine * me);

PRIVATE App * App_new (void);
PRIVATE BOOL App_delete (App * me);

PRIVATE void Context_new (HTRequest * request, App * app); 
PRIVATE void Context_delete (Context * ctx);

PRIVATE HTRequest * Request_new (App * app,HTMethod method);
PRIVATE BOOL Request_delete (App * app, HTRequest * request);

PRIVATE int request_terminater (HTRequest * request, HTResponse * response,
                                void * param, int status); 
PRIVATE int console_parser (SOCKET s, void * param, HTEventType type);
PRIVATE int error_callback (HTRequest * request, HTResponse * response,
                                      void * param, int status); 

PRIVATE int mypcbf (HTRequest * request, HTStream * target);

PRIVATE App * Init (void);
PRIVATE void my_get_document (App *app, CmdLine * line);
PRIVATE char * create_body (CmdLine *line);


/* --------------------------------------------------------------------------*/


/* ------------------------------------------------------------------------- */
/*                              EXPAT HANDLERS                               */
/* ------------------------------------------------------------------------- */

PRIVATE void XML_startElement (void * userData,
                               const XML_Char * name, 
                               const XML_Char ** atts)
{
    int *depth = (int *)userData;
    int i=0;
    if (name) {
        HTPrint ("\n");
        for (i=0;i<(*depth);i++) 
            HTPrint (" ");
        HTPrint("%s ", name);
        for (i = 0; atts[i]; i += 2)
            HTPrint(" %s='%s'", atts[i], atts[i + 1]);
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
    int i=0;

    if (len>1) {
        HTPrint (": ");             
        for (i=0;i<len;i++) HTPrint ("%c",s[i]);
        if (s[i-2]!='\n') HTPrint ("\n");
    }
    else {
        if (s[0]!='\n' && s[0]!=' ') HTPrint (": %c\n",s[0]);    
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
/*                           HTXML STREAM HANDLERS                           */
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

PRIVATE void HTXML_newInstance (HTStream  *             me,
                                HTRequest *             request,
                                HTFormat                target_format,
                                HTStream  *             target_stream,
                                XML_Parser              xmlparser,
                                void *                  context)
{
    /*HTPrint ("MyExt: HTXML_newInstance\n");*/
    if (me && xmlparser) {
        if (request && HTRequest_method(request)==PROPFIND)   
            HTXML_setHandlers(xmlparser,request);
    }
}


/* ------------------------------------------------------------------------- *
**                             New/Delete functions                          *
** ------------------------------------------------------------------------- */

/*
** CmdLine_new: creates a command line object
** A command line has this syntax : <command> <url> [propname:namespace][= value] 
** A command is v (view), s (set) or d (delete)
** An example : s http://foo.bar/ name:http://host/mydtd.dtd = my_file
*/
PRIVATE CmdLine * CmdLine_new (const char *buffer) {
    char *line = (char *)buffer;
    char *url, *ns, *prop, *val;
    char *rurl, *rns, *rprop, *rval;
    int cmd;
    int i,j,len;
    CmdLine *res = NULL;
    BOOL status = YES;
    
    url = ns = prop = val = NULL;
    rurl = rns = rprop = rval = NULL;
    len = strlen (line);
    cmd = 0;
    i = j = 0;

    /*--- get command ---*/
    while (line[i]==' ' && i<len) i++;
    switch (line[i++]) {
        case 'v':
        case 'V': cmd = VIEW;
                  break;

        case 's':
        case 'S': cmd = SET;
                  break;

        case 'd':
        case 'D': cmd = DEL;
                  break;
 
        default: status = NO; /* wrong command - error */
    }

   
    /*--- get url ---*/
    while (status && line[i]==' ' && i<len) i++;
    if (status && i<len)  url = &line[i];
    else status = NO; /* no url - error */


    j = i; /* search url ending position */
    while (status && line[i]!=' ' && line[i]!='\n' && i<len) i++;
    
    /* alloc url return string */
    if (status && (rurl = HT_CALLOC ((i-j+1),sizeof(char)))!=NULL) {    
        strncpy (rurl,url,i-j);
        rurl[i-j]='\0';
    } 
    else status = NO;
   
    if (status && cmd!=VIEW) {
            
        /*--- get property name ---*/
        while (line[i]==' ' && i<len) i++;    
        if (status && i<len) prop = &line[i]; 
        else status = NO; /* no property - error */
             
        j = i; /* search prop ending position */
        while (status && line[i]!=':' && i<len) i++;

        /* alloc prop return string */
        if (status && i<len && (rprop = HT_CALLOC ((i-j+1),sizeof(char)))!=NULL) {
            strncpy (rprop,prop,i-j);
            rprop[i-j]='\0';
        }
        else status = NO;

        
        /*--- get property namespace ---*/
        i++;    
        if (status && i<len)  
            ns = &line[i];
        else status = NO; /* no property namespace - error */

        j = i; /* search ns ending position */
        while (status && line[i]!=' ' && line[i]!='=' && line[i]!='\n' && i<len) i++;

        /* alloc ns return string */
        if (status && (rns = HT_CALLOC ((i-j+1),sizeof(char)))!=NULL) {
            strncpy (rns,ns,i-j);
            rns[i-j]='\0';
        }
        else status = NO;


       if (status && cmd==SET) {
           /*--- get property value ---*/
           while (line[i]!='=' && i<len) i++;
           i++; /* jump = */
           while (line[i]==' ' && i<len) i++; 
           if (i<len)
               val = &line[i];
           else status = NO; /* no property value - error */

           j = i; /* search val ending position */
           while (status && line[i]!='\n' && i<len) i++;

           /* alloc val return string */
           if (status && (rval = HT_CALLOC ((i-j+1),sizeof(char)))!=NULL) {
               strncpy (rval,val,i-j);
               rval[i-j]='\0';
           }
           else status = NO;
           
       }
    }

    if (status && (res = HT_CALLOC (1,sizeof(CmdLine))) != NULL ) {
        res->cmd = cmd;
        res->url = rurl;
        res->prop = rprop;
        res->ns = rns;
        res->val = rval;
    }
    else {
        if (rurl) HT_FREE(rurl);
        if (rprop) HT_FREE(rprop);
        if (rns) HT_FREE(rns);
        if (rval) HT_FREE(rval);
        res = NULL;
    }
    return res;
}


/*
** CmdLine_delete : removes a command line object
*/
PRIVATE BOOL CmdLine_delete (CmdLine * me) {
    if (me) {
        if (me->url) free (me->url);
        if (me->prop) free (me->prop);
        if (me->ns) free (me->ns);
        if (me->val) free (me->val);
        free (me);
        return YES;
    }
    return NO;
}


/*
** App_new : creates a new application context
** Returns : App *      application context
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
    
    /* Set Extension method 0 to PROPPATCH and 1 to PROPFIND */    
    HTPrint ("MyExt: Setting method %x to PROPPATCH\n",PROPPATCH);
    if (!HTMethod_setExtensionMethod (PROPPATCH, "PROPPATCH",YES)) {
        HTPrint ("MyExt: ERROR _ Unable to set extension method"); 
        exit(1);
    }   
   
    HTPrint ("MyExt: Setting method %x to PROPFIND\n",PROPFIND);
    if (!HTMethod_setExtensionMethod (PROPFIND, "PROPFIND",NO)) {
        HTPrint ("MyExt: ERROR _ Unable to set extension method"); 
        exit(1);
    }   

    HTPrint ("Enter: <command>  <url> [propname:namespace][= valor]\n");
    HTPrint ("   or  Q to exit  \n");
    HTPrint ("command := v (view), s (set) or d (delete)\n");
    HTPrint ("examples : s http://foo.bar/ propertyname:http://foo.bar/foodtd.dtd = property_value\n");
    HTPrint ("           d http://foo.bar/ propertyname:http://foo.bar/foodtd.dtd \n");
    HTPrint ("           v http://foo.bar/\n");
    HTPrint (">\n ");
   
    return me;
}



/*
** App_delete : removes an application context object
** Parameter : App * app        application context to be removed
** Returns : BOOL       YES - operation succed
**                      NO - operation failed
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
        HTPrint ("Removing extension methods\n");       
        if (HTMethod_deleteExtensionMethod (PROPPATCH) && HTMethod_deleteExtensionMethod (PROPFIND))
            HTPrint ("MyExt: Extension methods deleted \n");
        
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
**              App * app
*/
PRIVATE void Context_new (HTRequest * request, App * app) {
    Context * ctx = NULL;

    if ( (ctx = (Context *) HT_CALLOC(1, sizeof(Context))) == NULL ) {
        App_delete (app);
        HT_OUTOFMEM("Contect_new");
    }

    /*HTPrint ("MyExt: Setting request context...\n");*/
    ctx->depth = 0;
    HTRequest_setContext(request,ctx);    
}


/*
** Context_delete : removes the request context
** Parameters : Context * ctx
*/
PRIVATE void Context_delete (Context * ctx) {
    if (ctx) {
        /*HTPrint ("MyExt: Removing request context...\n");*/
        HT_FREE (ctx);
    }
}


/*
** Request_new : creates a new request 
** Parameters : App * app       application context
** Returns : HTRequest *
*/ 
PRIVATE HTRequest * Request_new (App * app, HTMethod method)
{
        
    HTRequest * request = HTRequest_new();

    HTPrint ("MyExt: creating a new request\n");
   
    HTRequest_addConnection(request,"close","");


    /* setting request format */
    HTRequest_setOutputFormat(request, HTAtom_for ("text/xml"));
    HTRequest_setOutputFormat(request, WWW_SOURCE);     
    
    /* method */    
   /* HTPrint ("MyExt: Setting method in the request\n"); */
    HTRequest_setMethod (request,method);
   
    /* some headers */
    HTRequest_addExtraHeader (request,"Depth","0");
    HTRequest_addCacheControl (request, "no-cache","");
    HTRequest_addGnHd (request,HT_G_PRAGMA_NO_CACHE); 

    
    /* set local filters */
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
** Parameters : App * app       application context
**               HTRequest * request
** Returns : BOOL       YES if operation succeed
**                      NO if operation failed
*/
PRIVATE BOOL Request_delete (App * app, HTRequest * request)
{
   /* HTPrint ("MyExt: removing request\n"); */
    
    if (app && app->active && request) {
        HTPrint ("MyExt: Request deleted \n");  
        HTList_removeObject(app->active, request);
        
        HTRequest_delete(request);
        return YES;
    }
    return NO;
}



/* ------------------------------------------------------------------------- *
**                             Global/local Filters                          *
** ------------------------------------------------------------------------- */

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
** console_parser : treats the user's interaction on the console
** Function's type : HTEventCallback
*/ 
PRIVATE int console_parser (SOCKET s, void * param, HTEventType type)
{
    App * app = (App *) param;
    char buf[2048];
    CmdLine * line;

    /* reading console */
    if (!fgets(buf, sizeof(buf), stdin)) {
        HTPrint ("Error reading stdin\n");
        return HT_ERROR;
    }

    if (toupper(buf[0]) == 'Q') {       /* Quit the program */
        App_delete(app);
        exit (0);
    }
    else { /* take the target address */
        
        HTPrint ("MyExt: Console readed **%s**\n",buf);
        if ( (line = CmdLine_new (buf))==NULL ) {
            HTPrint ("Wrong command line\n");
            return HT_ERROR;
        }

        my_get_document (app,line);             
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
    
   /* HTPrint ("MyExt: GLOBAL FILTER (request_terminater)\n"); */
   
    if (status/100 == 2) HTPrint ("MyExt: SUCCED!\n");
    if (response) HTPrint ("\tReason %s\n",(HTResponse_reason(response))? \
                                           HTResponse_reason(response):"NULL");
    HTPrint ("\tStatus %d\n",status);
    
    ctx = (Context *) HTRequest_context(request);

    /*HTPrint ("MyExt: context object deleted\n");*/
    Context_delete (ctx);    
        
    Request_delete(app, request);

    HTPrint ("Ready!\n");
    HTPrint ("Enter: <command>  <url> [propname:namespace][= valor]\n");
    HTPrint ("   or  Q to exit\n");    
    HTPrint ("> ");
    
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


/* --------------------------------------------------------------------------*/
/*                           ENTITY CALLBACK                                 */
/* --------------------------------------------------------------------------*/


/*
** Entity Callback 
*/

PRIVATE int mypcbf (HTRequest * request, HTStream * target)
{
    HTParentAnchor * entity = HTRequest_entityAnchor(request);
    HTTRACE(APP_TRACE, "Posting Data from MY callback function\n");
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






/* ------------------------------------------------------------------------- *
**                               Other Functions                             *
** ------------------------------------------------------------------------- */

/*
** Init : application initialization 
** Returns : App *      application context
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
** my_get_document: retrives the document
** Parameters: App * app        application context
**             char * dst       URL destiny
*/             
PRIVATE void my_get_document (App *app, CmdLine * line) {
    HTRequest * request = NULL;
    HTAnchor * dst = NULL;
    char * full_dst;
    char * cwd;
    char * body = create_body(line);

    cwd =  HTGetCurrentDirectoryURL();  
    full_dst = HTParse (line->url,cwd,PARSE_ALL);    
    dst = HTAnchor_findAddress (full_dst);
    
  /*  HTPrint ("MyExt: loading document\n");*/

    request = Request_new (app, (line->cmd==VIEW)?PROPFIND:PROPPATCH);
    HTRequest_setAnchor (request,dst);
    
    /* set body */
    if (body) {
        HTPrint ("MyExt: Setting entity message body  to %s\n",body,strlen(body));
        if (line->cmd==VIEW) { /* set message body */
            HTRequest_setMessageBody (request,body);
            HTRequest_setMessageBodyLength (request,strlen(body));
            HTRequest_setMessageBodyFormat (request,HTAtom_for("text/xml"));
        }
        else {
            HTParentAnchor *entity = HTTmpAnchor(NULL);
            HTAnchor_setDocument(entity, body);
            HTAnchor_setFormat(entity, HTAtom_for ("text/xml"));
            HTAnchor_setLength(entity, strlen(body));
            HTRequest_setEntityAnchor (request,entity);

           /* HTPrint ("MyExt: Entity format %s length %d\n", \
                                    HTAtom_name(HTAnchor_format(entity)), \
                                    HTAnchor_length(entity));
           */                       
            /* if we do not register our own PostCallback, libwww will not
             * send the request entity body */
            HTRequest_setPostCallback(request, mypcbf); 
        }
    }
    
    
    if ( request && (HTLoad (request,NO))) {    
        /* HTPrint ("MyExt: setting context\n");*/
        Context_new (request,app);        
    }
    else
        HTPrint ("MyExt: load failed\n");    
    
    /* HTParentAnchor entity keep using it - do not delete! */
    /* HT_FREE (body); */
    CmdLine_delete (line);
}



PRIVATE char * create_body (CmdLine * line) {
    char * body = NULL;
    char buf [2048];
    
    if (line) { 
        strcpy (buf,"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n");    
        if (line->cmd==VIEW) {
            strcat (buf,"<D:propfind xmlns:D=\"DAV:\"> ");      
            strcat (buf,"<D:allprop/> </D:propfind>");
        }
        else {
            strcat (buf,"<D:propertyupdate xmlns:D=\"DAV:\">\n");

            if (line->cmd==SET) strcat (buf,"  <D:set>\n");
            else if (line->cmd==DEL) strcat (buf,"  <D:remove>\n");
            else return NULL; /* invalid command */

            strcat (buf,"    <D:prop>\n");
            strcat (buf,"      <ns:");
            if (line->prop && line->ns) {
                strcat (buf,line->prop);               
                strcat (buf," xmlns:ns=\"");
                strcat (buf,line->ns);
                strcat (buf,"\">");
            }
            else return NULL; /* no property or property namespace */
                
            if (line->cmd==SET) {
                strcat (buf,(line->val)?line->val:"");
            }   

            strcat (buf,"</ns:");
            strcat (buf,line->prop);
            strcat (buf,">\n");

            strcat (buf,"    </D:prop>\n");
            strcat (buf,(line->cmd==SET)?"  </D:set>\n":"  </D:remove>\n");
            strcat (buf,"</D:propertyupdate>");
        }
        StrAllocCopy (body,buf);
    }
    return body;

}


/* ******************************************************************** */
int main (int argc, char ** argv)
{
    App * app = NULL;
    
    app = Init();

    return 0;
}
