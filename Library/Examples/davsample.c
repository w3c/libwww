/* 
** HTDAV test file 
**
** Using this program, you may test all WebDAV high level functions available 
** in libwww, and also test your webserver, if you want. 
**
** Authors: 
**      MKP     Manuele Kirsch Pinheiro (Manuele.Kirsch_Pinheiro@inrialpes.fr, manuele@inf.ufrgs.br)
**              _ Project CEMT  (INRIA Rhone-Alpes,France / UFRGS-II,Brazil) _
**
** History:
**      fev, 2002       created                 MKP
**                                
** $Id$
*/

#include "WWWLib.h"
#include "WWWInit.h"
#include "WWWApp.h"
#include "HTDAV.h"                /* here - WebDAV high level functions*/



#define APP_NAME                "DAVSample"
#define APP_VERSION             "2.0"


#define ERR_UNKNOWN               0x0
#define ERR_FATAL                 0x1
#define ERR_NON_FATAL             0x2
#define ERR_WARN                  0x4
#define ERR_INFO                  0x8

/*
**  command line struct 
*/
typedef struct _cmdline {
    char   method;
    int    func;
    char * D;
    char * I;
    char * T;
    char   O;
    
    char * request_uri;
    char * base_str;
    char * arg1;
    char *arg2;
} Cmdline;



/* 
** callbacks 
*/
PRIVATE HTNetAfter terminate_handler;
PRIVATE HTNetAfter error_callback; 
PRIVATE int printer (const char * fmt, va_list pArgs);
PRIVATE int tracer (const char * fmt, va_list pArgs);


/*
** prototypes 
*/ 
PRIVATE void Init (void);
PRIVATE void my_headers (HTRequest *request);
PRIVATE char * create_body (char * owner);
PRIVATE char * create_propbody (char * prop);
PRIVATE HTRequest * create_request (void);

PRIVATE BOOL lock_request (Cmdline * arg); 
PRIVATE BOOL propfind_request (Cmdline * arg); 
PRIVATE BOOL unlock_request (Cmdline * arg); 
PRIVATE BOOL proppatch_request(Cmdline * arg); 
PRIVATE BOOL mkcol_request (Cmdline * arg); 
PRIVATE BOOL copy_request (Cmdline * arg); 

Cmdline * handle_args (int argc, char **argv);



/* -------------------------------------------------------------------------------- *
 *                                 Filters                                          *
 * -------------------------------------------------------------------------------- */

/*
** printer : output messages function
*/
PRIVATE int printer (const char * fmt, va_list pArgs)
{
    return (vfprintf(stdout, fmt, pArgs));
}

/*
** trace : trace messages function
*/
PRIVATE int tracer (const char * fmt, va_list pArgs)
{
    return (vfprintf(stderr, fmt, pArgs));
}


/*
** request_terminater : global filter to delete the requests
** Funtion's type : HTNetAfter
*/ 
PRIVATE int terminate_handler (HTRequest * request, HTResponse * response,
                               void * param, int status) 
{
    my_headers(request);

     if (response) {
        HTPrint ("\tStatus:%d\n\tContent-length:%d\n\tIs Cachable:%c\n\tis Cached:%c\n\tReason: %s\n",\
                  status, HTResponse_length(response),\
                  (HTResponse_isCachable(response))?'Y':'N',\
                  (HTResponse_isCached(response,YES))?'Y':'N', \
                  (HTResponse_reason(response))?HTResponse_reason(response):"NULL");
            HTPrint ("\tFormat : %s \n",(char *)HTAtom_name(HTResponse_format(response))); 
    }
    else 
        HTPrint ("\tResponse NULL\n");
    

    /* Terminate libwww */
    HTProfile_delete();

    exit(0);
}


/*
** error_callback : global after filter to treat the request's errors
** Function's type : HTNetAfter
*/
PRIVATE int error_callback (HTRequest * request, HTResponse * response,
                                  void * param, int status) {

    HTList * error_list = NULL;
    HTError * error = NULL;
    
    HTPrint ("%s: ERROR CALLBACK\n",APP_NAME);
    HTPrint ("\trequest %s \n\tresponse %s \n\tstatus %d\n", \
            (request)?"OK":"NULL",\
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



/* -------------------------------------------------------------------------------- *
 *                               Common Functions                                   *
 * -------------------------------------------------------------------------------- */

/*
** my_headers : shows anchor's headers
** Parameters : HTRequest * request
*/  
PRIVATE void my_headers (HTRequest *request) {
    HTAssoc * h = NULL;    
    HTAssocList * headers = NULL; 
    HTParentAnchor * anchor = NULL;

    HTPrint ("%s: Searching headers...\n",APP_NAME);  
    
    anchor = HTRequest_anchor (request);
    headers = HTAnchor_header(anchor);
    
    HTPrint ("\tanchor %s\n",(anchor)?"OK":"NULL");  
    
    h = HTAssocList_nextObject(headers);
    while (anchor && headers && h ) {
       HTPrint ("\t%s : %s\n",HTAssoc_name(h),HTAssoc_value(h));
       h = HTAssocList_nextObject(headers);
    }
}


/*
** Init : application start 
*/ 
PRIVATE void Init (void) {

    /* Create a new premptive client */
    HTProfile_newNoCacheClient(APP_NAME, APP_VERSION);

    /* Gotta set up our own traces */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);

    /* Add our own filter to update the history list */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);
    HTNet_addAfter(error_callback, NULL, NULL, HT_ERROR, HT_FILTER_LAST); 


    /* Set trace messages and alert messages*/
#if 0
    HTSetTraceMessageMask("spolc");
#endif

}



PRIVATE HTRequest * create_request (void) {
    HTRequest * me = HTRequest_new();

    /* set output and debug streans */
    HTRequest_setOutputStream(me, HTFWriter_new(me, stdout, YES));
    HTRequest_setDebugStream(me, HTFWriter_new(me, stderr, YES));

    HTRequest_setOutputFormat(me,HTAtom_for("text/xml"));
    HTRequest_setDebugFormat(me,WWW_SOURCE);

    return me;
       
}



/* -------------------------------------------------------------------------------- *
 *                               XML Body functions                                 *
 * -------------------------------------------------------------------------------- */


PRIVATE char * create_body (char * owner) {
     char tmp[4096];
     char * body = NULL;
     char nl[3];
     
     sprintf (nl,"%c%c",CR,LF);     
    
     if (owner && *owner) {
         sprintf (tmp,"<?xml version=\"1.0\" encoding=\"utf-8\" ?>");
         strcat (tmp,nl);
         strcat (tmp,"<D:lockinfo xmlns:D=\"DAV:\">");
         strcat (tmp,nl);
         strcat (tmp,"    <D:lockscope><D:exclusive/></D:lockscope>");
         strcat (tmp,nl);
         strcat (tmp,"    <D:locktype><D:write/></D:locktype>");
         strcat (tmp,nl);
         strcat (tmp,"    <D:owner>");
         strcat (tmp,nl);
         strcat (tmp,"        <D:href>");
         strcat (tmp,owner);
         strcat (tmp,"</D:href>");
         strcat (tmp,nl);
         strcat (tmp,"    </D:owner>");
         strcat (tmp,nl);
         strcat (tmp,"</D:lockinfo>");

         if ( (body = HT_MALLOC (strlen(tmp)+4)) != NULL)
             sprintf (body,"%s%c",tmp,'\0');
     }
     
     return body;
}


PRIVATE char * create_propbody (char * prop) {
     char tmp[5120];
     char * body = NULL;
     char nl[3];
   
     sprintf (nl,"%c%c",CR,LF);     
    
     if (prop && *prop) {
         sprintf (tmp,"<?xml version=\"1.0\" encoding=\"utf-8\" ?>");
         strcat (tmp,nl);
         strcat (tmp,"<D:propfind xmlns:D=\"DAV:\">");
         strcat (tmp,nl);
         if (!strcmp(prop,"allprop")) strcat (tmp,"<D:allprop />");
         else if (!strcmp(prop,"propname")) strcat (tmp,"<D:propname />"); 
         else strcat (tmp,prop);
         strcat (tmp,nl);
         strcat (tmp,"</D:propfind>");

         if ( (body = HT_MALLOC (strlen(tmp)+4)) != NULL)
             sprintf (body,"%s%c",tmp,'\0');
     }
     
     return body;
}



/* -------------------------------------------------------------------------------- *
 *                                WebDAV Methods                                    *
 * -------------------------------------------------------------------------------- */

/*
** LOCK requests
*/
PRIVATE BOOL lock_request (Cmdline * arg) {

    HTDAVHeaders * headers = HTDAVHeaders_new();
    HTRequest * request = create_request ();
    HTAnchor * dst = HTAnchor_findAddress(arg->request_uri);
    HTParentAnchor * src = NULL;
    HTParentAnchor * base = NULL;
    BOOL status = NO;
    char * data = NULL;
        
    if (arg->I) {
        HTPrint ("Adding If header %s\n",arg->I);
        HTDAV_setIfHeader (headers,arg->I);
    }

    if (arg->arg1)  {
        data = create_body (arg->arg1);
        HTPrint ("xml body  %s\n",data);
           
        /* chose the func */
        if (arg->func==1) {
            src = HTTmpAnchor(NULL);
            HTAnchor_setDocument(src, data);
            HTAnchor_setFormat(src, HTAtom_for ("text/xml"));
            HTAnchor_setLength(src, strlen(data));
        } 
    }

    if (arg->base_str && *(arg->base_str)) 
        base = (HTParentAnchor *) HTAnchor_findAddress(arg->base_str);

    if (arg->D) HTDAV_setDepthHeader (headers,arg->D);
    if (arg->T) HTDAV_setTimeoutHeader (headers,arg->T);

    HTPrint ("function %d src? %s\n",arg->func,(src)?"yes":"no");

    switch (arg->func) {
        case 1: status = HTLOCKDocumentAnchor (request,dst,src,headers);
                break;
        case 2: status = HTLOCKAnchor (request,dst,data,headers);
                break;
        case 3: status = HTLOCKAbsolute (request,arg->request_uri,data,headers);
                break;
        case 4: status = HTLOCKRelative (request,arg->request_uri,base,data,headers);
                break;
    }

    return status;
}



/*
** UNLOCK requests
*/
PRIVATE BOOL unlock_request (Cmdline * arg ) {
        
    HTDAVHeaders * headers = HTDAVHeaders_new();
    HTRequest * request = create_request ();
    HTAnchor * dst = HTAnchor_findAddress(arg->request_uri);
    HTParentAnchor *base = NULL;
    BOOL status = NO;

    if (arg->arg1) {
        HTPrint ("Adding Lock-Token header %s\n",arg->arg1);
        HTDAV_setLockTokenHeader (headers,arg->arg1);
    }
        
    if (arg->base_str && *(arg->base_str)) 
            base = (HTParentAnchor *) HTAnchor_findAddress(arg->base_str);
                
    /* chose func */
    switch (arg->func) {
        case 1: status = HTUNLOCKAnchor (request,dst,headers);
                break;
        case 2: status = HTUNLOCKAbsolute (request,arg->request_uri,headers);
                break;
        case 3: status = HTUNLOCKRelative (request,arg->request_uri,base,headers);
                break;
                    
    }

    return status;
}


/*
** PROPFIND requests
*/
PRIVATE BOOL propfind_request (Cmdline * arg) {
    BOOL status = NO;
    HTDAVHeaders * headers = HTDAVHeaders_new();
    HTRequest * request = create_request ();
    HTAnchor * dst = HTAnchor_findAddress(arg->request_uri);
    HTParentAnchor *base = NULL;
    HTParentAnchor *src = NULL;
    char * xmlbody = NULL;

    /* chose the func */
    HTPrint ("should we set the xml body?\n");
    if (arg->arg1 && *(arg->arg1)) {
        if (!strcasecomp (arg->arg1,"allprop") || !strcasecomp (arg->arg1,"propname")) 
            xmlbody = create_propbody (arg->arg1);
        else
            xmlbody = arg->arg1;
        HTPrint ("xml body %s\n",xmlbody);        
    }        
    
    if (arg->func==2 && xmlbody && *xmlbody) {
        src = HTTmpAnchor(NULL);
        HTAnchor_setDocument(src, xmlbody);
        HTAnchor_setFormat(src, HTAtom_for ("text/xml"));
        HTAnchor_setLength(src, strlen(xmlbody));                
    } 

    if (arg->base_str && *(arg->base_str)) 
        base = (HTParentAnchor *) HTAnchor_findAddress(arg->base_str);

    HTPrint ("setting headers\n");
    if (arg->D) HTDAV_setDepthHeader (headers,arg->D);


    switch (arg->func) {
        case 1: status = HTPROPFINDAnchor (request,dst,xmlbody,headers);
                break; 
        case 2: status = HTPROPFINDDocumentAnchor (request,dst,src,headers);
                break; 
        case 3: status = HTPROPFINDAbsolute (request,arg->request_uri,xmlbody,headers);
                break; 
        case 4: status = HTPROPFINDRelative (request,arg->request_uri,base,xmlbody,headers);
                break; 
    }    

    return status;
}


/*
** PROPPATCH requests
*/
PRIVATE BOOL proppatch_request (Cmdline * arg) {
    BOOL status = NO;
    HTDAVHeaders * headers = HTDAVHeaders_new();
    HTRequest * request = create_request ();
    HTAnchor * dst = HTAnchor_findAddress(arg->request_uri);
    HTParentAnchor *base = NULL;
    HTParentAnchor *src = NULL;
    char * xmlbody = NULL;
    
    if (arg->arg1 && *(arg->arg1)) 
        xmlbody = arg->arg1;        
    else 
        return NO;
    
    HTPrint ("xml body **%s**\n",xmlbody);        
    
    if (arg->func==2) {
        src = HTTmpAnchor(NULL);
        HTAnchor_setDocument(src, xmlbody);
        HTAnchor_setFormat(src, HTAtom_for ("text/xml"));
        HTAnchor_setLength(src, strlen(xmlbody));                
    } 

    if (arg->base_str && *(arg->base_str)) 
        base = (HTParentAnchor *) HTAnchor_findAddress(arg->base_str);

    HTPrint ("setting headers\n");
    if (arg->I && *(arg->I)) {           
        HTPrint ("Adding If header %s\n",arg->I);
        HTDAV_setIfHeader (headers,arg->I);
    }

    HTPrint ("Chosing func\n");
    switch (arg->func) {
        case 1: status = HTPROPPATCHAnchor (request,dst,xmlbody,headers);
                break; 
        case 2: status = HTPROPPATCHDocumentAnchor (request,dst,src,headers);
                break; 
        case 3: status = HTPROPPATCHAbsolute (request,arg->request_uri,xmlbody,headers);
                break; 
        case 4: status = HTPROPPATCHRelative (request,arg->request_uri,base,xmlbody,headers);
                break; 
    }    

    return status;
}



/*
** MKCOL requests
*/
PRIVATE BOOL mkcol_request (Cmdline * arg) {
    BOOL status = NO;
    HTDAVHeaders * headers = HTDAVHeaders_new();
    HTRequest * request = create_request ();
    HTAnchor * dst = HTAnchor_findAddress(arg->request_uri);
    HTParentAnchor *base = NULL;
    
    if (arg->base_str && *arg->base_str) {
        base = (HTParentAnchor *) HTAnchor_findAddress(arg->base_str);
    } 

    if (arg->I && *(arg->I)) {
        HTPrint ("If header: %s\n",arg->I);
        HTDAV_setIfHeader (headers,arg->I);
    }

    switch (arg->func) {
        case 1: status = HTMKCOLAnchor (request,dst,headers);
                break; 
        case 2: status = HTMKCOLAbsolute (request,arg->request_uri,headers);
                break; 
        case 3: status = HTMKCOLRelative (request,arg->request_uri,base,headers);
                break; 
    }    

    return status;
}



/*
** COPY requests
*/
PRIVATE BOOL copy_request (Cmdline * arg ) { 
        
    HTDAVHeaders * headers = HTDAVHeaders_new();
    HTRequest * request = create_request ();
    HTAnchor * src = HTAnchor_findAddress(arg->request_uri);
    HTParentAnchor * body = NULL;
    HTParentAnchor * base = NULL;
    BOOL status = NO;
    
    if (arg->arg1 && *(arg->arg1)) {
        HTPrint ("Adding Destination header %s\n",arg->arg1);
        HTDAV_setDestinationHeader (headers,arg->arg1);
    }
        

    /* chose the func */
    if (arg->func==2 && arg->arg2) {
        body = HTTmpAnchor(NULL);
        HTAnchor_setDocument(body, arg->arg2);
        HTAnchor_setFormat(body, HTAtom_for ("text/xml"));
        HTAnchor_setLength(body, strlen(arg->arg2));                
     } 

    if (arg->base_str && *(arg->base_str)) 
        base = (HTParentAnchor *) HTAnchor_findAddress(arg->base_str);

    if (arg->I) {
       HTPrint ("Adding If header %s\n",arg->I);
       HTDAV_setIfHeader (headers,arg->I);
    }
        
    if (arg->D)HTDAV_setDepthHeader (headers,arg->D);
    if (arg->O!=' ')HTDAV_setOverwriteHeader (headers,arg->O);

    switch (arg->func) {
        case 1: status = HTCOPYAnchor (request,src,arg->arg2,headers);
                break;
        case 2: status = HTCOPYDocumentAnchor (request,src,body,headers);
                break;
        case 3: status = HTCOPYAbsolute (request,arg->request_uri,arg->arg2,headers);
                break;
        case 4: status = HTCOPYRelative (request,arg->request_uri,base,arg->arg2,headers);
                break;
    }

    return status;
}



/*
** MOVE requests
*/
PRIVATE BOOL move_request ( Cmdline * arg ) { 
        
    HTDAVHeaders * headers = HTDAVHeaders_new();
    HTRequest * request = create_request ();
    HTAnchor * src = HTAnchor_findAddress(arg->request_uri);
    HTParentAnchor * body = NULL;
    HTParentAnchor * base = NULL;
    BOOL status = NO;
        
    if (arg->arg1) {
       HTPrint ("Adding Destination header %s\n",arg->arg1);
       HTDAV_setDestinationHeader (headers,arg->arg1);
    }
        
    if (arg->I) {
       HTPrint ("Adding If header %s\n",arg->I);
       HTDAV_setIfHeader (headers,arg->I);
    }

    /* chose the func */
    if (arg->func==2 && arg->arg2 ) {
        body = HTTmpAnchor(NULL);
        HTAnchor_setDocument(body, arg->arg2);
        HTAnchor_setFormat(body, HTAtom_for ("text/xml"));
        HTAnchor_setLength(body, strlen(arg->arg2));                
     } 

    if (arg->base_str && *(arg->base_str)) 
        base = (HTParentAnchor *) HTAnchor_findAddress(arg->base_str);

    if (arg->D) HTDAV_setDepthHeader (headers,arg->D);
    if (arg->O == 'T') HTDAV_setOverwriteHeader (headers,YES);
    else if (arg->O == 'F') HTDAV_setOverwriteHeader (headers,NO);

    switch (arg->func) {
        case 1: status = HTMOVEAnchor (request,src,arg->arg2,headers);
                break;
        case 2: status = HTMOVEDocumentAnchor (request,src,body,headers);
                break;
        case 3: status = HTMOVEAbsolute (request,arg->request_uri,arg->arg2, headers);
                break;
        case 4: status = HTMOVERelative (request,arg->request_uri,base,arg->arg2,headers);
                break;
    }

    return status;
}


/* ---------------------------------------------------------------- *
 *                    COMMAND LINE HANDLING                         *
 * ---------------------------------------------------------------- */
Cmdline * handle_args (int argc, char **argv) {
    int status = 0;
    Cmdline *opt = NULL;
    Cmdline *arg = NULL;
    
    printf ("Handling Arguments\n");

    if (argc > 3) {
        if ( (opt = arg = HT_CALLOC (1,sizeof (Cmdline))) ) {
            int i = 1;
            status = 1;
            
            /*clean up everything */
            opt->func = 0;
            opt->method = opt->O = ' ';
            opt->D = opt->T = opt->I = NULL;
            arg->request_uri = arg->base_str = NULL;
            arg->arg1 = arg->arg2 = NULL;
            
            /* method */
            if (argv[i][0]=='-') opt->method = argv[i++][1];
            else status = 0;
            argc--;
            
            /* function */
            opt->func = atoi (argv[i++]) * (-1);
            argc--;

            /* options */
            while (argc>0 && argv[i][0]=='-') {
                if (argv[i][1] == 'D') { opt->D=argv[++i]; argc--; }
                else if (argv[i][1] == 'T') { opt->T=argv[++i]; argc--; }
                else if (argv[i][1] == 'I') { opt->I=argv[++i]; argc--; }
                else if (argv[i][1] == 'O') { opt->O=argv[++i][0]; argc--; }
                else status = 0;
                i++; argc--;
            }
            

            /* func args _ request_uri */
            if (argc>0) { arg->request_uri = argv[i++]; argc--; }
            else status = 0;

            /* func args _ base? */
            if ( argc > 0 && 
                 ( ( (opt->method=='l' || opt->method=='f' || opt->method=='p' || 
                      opt->method=='c' || opt->method=='m') && opt->func==4 )  ||
                   ( (opt->method=='u' || opt->method=='k') && opt->func==3 ) ) )
             { arg->base_str = argv[i++];
               argc--;
             }

            /* func arg - first arg */
            if (argc>0) { arg->arg1 = argv[i++]; argc--; }            
            
            /* func arg - second arg */
            if (argc>0) { arg->arg2 = argv[i++]; argc--; }
            
        }
    } 

    return arg;
}


/* ---------------------------------------------------------------- *
 *                           MAIN                                   *
 * ---------------------------------------------------------------- */

int main (int argc, char ** argv)
{
    BOOL status = NO;
    Cmdline * opt = NULL;
    Cmdline * arg = NULL;
    
    /* Create a new premptive client */
    Init();
    
    /* Handle command line args */
    if (argc >= 3) {
       arg = opt = handle_args (argc,argv);
       if (opt && arg) {            
           HTPrint ("method: %c (func %d)\n",opt->method,opt->func);
           HTPrint ("\tDepth: %s \n\tTimeout: %s \n",(opt->D)?opt->D:"UNDEFINED", \
                                                 (opt->T)?opt->T:"UNDEFINED");
           HTPrint ("\tIf: %s \n\tOverwrite: %c \n",(opt->I)?opt->I:"UNDEFINED",opt->O);

           HTPrint ("\tRequest URI: %s%s \n",(arg->base_str)?arg->base_str:"", \
                                         arg->request_uri);
           HTPrint ("\t1° argument: %s \n\t2° argument: %s\n",(arg->arg1)?arg->arg1:"UNDEFINED",
                                                          (arg->arg2)?arg->arg2:"UNDEFINED");

       }
       else {
           HTPrint ("Bad argument\n");
           return -1;
       }           
    }//if argc>3
    else {        
        HTPrint ("Usage: \n");
        HTPrint ("%s <method> <method_function> [options_headers] <function_arguments>\n\n",argv[0]);
        HTPrint ("methods are: -[lufpkcm]\n");
        HTPrint ("method_function are: -[1-4] (see functions order in HTDAV.html)\n");
        HTPrint ("options_headers are:\n");
        HTPrint ("\t-D <depth_header_value>    (for Depth header)\n");
        HTPrint ("\t-T <timeout_header_value>  (for Timeout header)\n");
        HTPrint ("\t-I <if_header_value>       (for If header)\n");
        HTPrint ("\t-O <'T' | 'F'>             (for Overwrite header)\n\n");
        HTPrint ("function_arguments are: \n");
        HTPrint ("for LOCK: %s -l -<[1-3]> [options] <dest-uri> <owner|\"REFRESH\"> \n",argv[0]);
        HTPrint ("\t %s -l -4 [options] <relative-uri> <base_uri> <owner|\"REFRESH\">\n",argv[0]);
        HTPrint ("\t example: %s -l -4 -D 0 /dav_collection/myfile.html http://foo.bar/ mailto:myself@foo.bar\n\n",argv[0]);
       
        HTPrint ("for UNLOCK: %s -u -<[1-2]> [options] <dest-uri> <lock-token>\n",argv[0]);
        HTPrint ("\t %s -u -3 [options] <relative-uri> <base-uri> <lock-token> \n",argv[0]);
        HTPrint ("\t example: %s -u -3 /dav_collection/myfile.html http://foo.bar/ <opaquelocken:blablabla>\n\n",argv[0]);
       
        HTPrint ("for PROPFIND:%s -f -<[1-3]> [options] <dest-uri> [\"allprop\"|\"propname\"|xml body]\n",\
                                                                                            argv[0]);
        HTPrint ("\t %s -f -4 [options] <relative-uri> <base-uri> [\"allprop\"|\"propname\"|xml body]\n",\
                                                                                                argv[0]);
        HTPrint ("\t example: %s -f -4 -D 0 /dav_collection/myfile.html http://foo.bar/ allprop\n\n",argv[0]);
            
        HTPrint ("for PROPPATCH:%s -p -<[1-4]> [options] <dest-uri> <xml body>\n",argv[0]);
        HTPrint ("\t %s -p -4 [options] <relative-uri> <base-uri> <xml body>\n",argv[0]);
        HTPrint ("\t example: %s -p -4 -I <opaquelock:blablabla> /dav_collection/myfile.html http://foo.bar/\\ \n "
                 "\"<?xml version='1.0'?>\n <D:propertyupdate xmlns:D='DAV:' xmlns:Z='http://foo.bar/propertydtd/'>\n"
                 "  <D:set>\n   <D:prop>\n    <Z:author> MyName </Z:author>\n   </D:prop>\n  <D:set>\n"
                 "</D:propertyupdate>\" \n\n",argv[0]);
            
        HTPrint ("for MKCOL: %s -k -<[1-2]> [options] <dest-uri>\n",argv[0]);
        HTPrint ("\t %s -k -3 [options] <relative-uri> <base-uri>\n",argv[0]);
        HTPrint ("\t example: %s -k -3 /dav_collection/newcollection http://foo.bar/\n\n",argv[0]);

        HTPrint ("for COPY: %s -c -<[1-3]> [options] <src-uri> <dst-uri> <xml body|\"NOBODY\">\n",\
                                                                                            argv[0]);
        HTPrint ("\t %s -c -4 [options] <relative-uri> <base-uri> <dst-uri> <xml body|\"NOBODY\">\n",\
                                                                                            argv[0]);
        HTPrint ("\t example: %s -c -4 /dav_collection/myfile.html http://foo.bar/ http://foo.bar/copyfile.html NOBODY\n\n",argv[0]);
        
        HTPrint ("for MOVE: %s -m -<[1-3]> [options] <src-uri> <dst-uri> <xml body|\"NOBODY\">\n",
                                                                                            argv[0]);
        HTPrint ("\t %s -m -4 [options] <relative-uri> <base-uri> <dst-uri> <xml body|\"NOBODY\">\n",\
                                                                                            argv[0]);
        HTPrint ("\t example: %s -m -4 /dav_collection/myfile.html http://foo.bar/ http://foo.bar/movefile.html NOBODY\n\n",argv[0]);
            

        HTPrint ("If, Depth and Timeout headers as defined in RFC2518 section 9\n");
        HTPrint ("lock-token as defined in RFC2518 section 9.5\n");
        HTPrint ("xml body as defined in RFC2518 section 12\n\n");
        return -1;        
    }
        

    
    if (arg) {
        switch (arg->method) {
          case 'l':        
            HTPrint ("LOCK REQUEST\n");   
            if (arg->arg1) {
                arg->arg1 = (!strcasecomp(arg->arg1,"REFRESH"))?NULL:arg->arg1;
                status = lock_request (arg);
            }
            else
                HTPrint ("Bad argument! Please, execute %s for help\n",argv[0]);
            break;
       
          case 'u': 
            HTPrint ("UNLOCK REQUEST\n");    
            status = unlock_request (arg); 
            break;
          
          case 'f':
            HTPrint ("PROPFIND REQUEST\n");  
            status = propfind_request (arg);
            break;
          
          case 'p':
            HTPrint ("PROPPATCH REQUEST\n");  
            status = proppatch_request (arg);
            break;
            
          case 'k':
            HTPrint ("MKCOL REQUEST\n");  
            status = mkcol_request (arg);
            break;

          case 'c':        
            HTPrint ("COPY REQUEST\n");    
            if (arg->arg1) {
                arg->arg2 = (!strcasecomp(arg->arg2,"NOBODY"))?NULL:arg->arg2;
                status = copy_request (arg);
            }
            else
                HTPrint ("Bad argument! Please, execute %s for help\n",argv[0]);

            break;
          
          case 'm':        
            HTPrint ("MOVE REQUEST\n");    
            if (arg->arg1) {
                arg->arg2 = (!strcasecomp(arg->arg2,"NOBODY"))?NULL:arg->arg2;
                status = move_request (arg);
            }
            else
                HTPrint ("Bad argument! Please, execute %s for help...\n",argv[0]);
            break;
             
        }
        
      if (status) HTEventList_newLoop();       
      else    HTPrint ("FAILED\n");                   
      
      HT_FREE (arg); 
   }
   return 0;
}

