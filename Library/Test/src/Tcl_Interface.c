#include "HText.h"
#include "WWWApp.h"
#include "WWWInit.h"
#include "Tcl_Interface.h"

#define bad_vars "Invalid variable names or non-existent entries."
#define err_string "wrong # args: should be "

#define DEFAULT_FORMAT		WWW_SOURCE
#define STDOUT stdout
#define OUTPUT stdout
#define DEFAULT_TIMEOUT		10		       /* timeout in seconds */

#define SHOW_MSG                (WWWTRACE || HTAlert_interactive())

typedef struct _Robot {
    HTRequest *         request;
    HTRequest *         timeout;          /* Until we get a server eventloop */
    HTParentAnchor *    anchor;
    HTList *		urilist;
    int			count;
    HText *   	       	htext;
    struct timeval *    tv;                             /* Timeout on socket */
    char *              cwd;                              /* Current dir URL */
    char *              outputfile;
    FILE *              output;
} Robot;

typedef struct _TextAnchor {
    struct _TextAnchor *	next;
    int			number;		/* For user interface */
    HTChildAnchor *		anchor;
} TextAnchor;

struct _HText {
    HTRequest *         request;
    TextAnchor *	first_anchor;
    TextAnchor *	last_anchor;
    int                 anchors;
};

/* ------------------------------------------------------------------------- */
/*                              HTEXT INTERFACE                              */
/* ------------------------------------------------------------------------- */

int HText_sourceAnchors (HText * text)
{
    return (text ? text->anchors : -1);
}

HTChildAnchor * HText_childNumber (HText * text, int number)
{
    TextAnchor * a;
    for (a = text->first_anchor; a; a = a->next) {
        if (a->number == number) return a->anchor;
    }
    return (HTChildAnchor *)0;	/* Fail */
}

HText * HText_new2 (HTRequest * request, HTParentAnchor * anchor,
		    HTStream * stream)
{
    HText * me;
    Robot * mr = (Robot *) HTRequest_context(request);
    if ((me = (HText *) HT_CALLOC(1, sizeof(HText))) == NULL)
        HT_OUTOFMEM("HText_new2");

    /* Bind the HText object together with the Request Object */
    me->request = request;
    me->first_anchor = me->last_anchor = 0;
    me->anchors = 0;

    /* Add this HText object to our list */
    mr->htext = me;
    return me;
}

void HText_free (HText * me) {
    if (me) HT_FREE (me);
}

void HText_beginAnchor (HText * text, HTChildAnchor * anchor)
{
    TextAnchor * a;
    if (text && anchor) {
	Robot * mr = (Robot *) HTRequest_context(text->request);
	HTAnchor * dest = HTAnchor_followMainLink((HTAnchor *) anchor);
	HTParentAnchor * dest_parent = HTAnchor_parent(dest);
	char * uri = HTAnchor_address((HTAnchor *) dest_parent);

#if 0
	if (SHOW_MSG) HTTrace("Robot....... Found `%s\' \n", uri ? uri : "NULL");
#endif	
	if (uri) {
	    HTList_addObject(mr->urilist, (void *) uri);
	    mr->count++;
	}

	if ((a = (TextAnchor  *) HT_MALLOC(sizeof(*a))) == NULL)
	    HT_OUTOFMEM("HText_beginAnchor");
	if (text->last_anchor) {
	    text->last_anchor->next = a;
	} else {
	    text->first_anchor = a;
	}
	a->next = 0;
	a->anchor = anchor;
	text->last_anchor = a;
    
	if (HTAnchor_followMainLink((HTAnchor*)anchor)) {
	    a->number = ++(text->anchors);
	} else {
	    a->number = 0;
	}
    }
}

void HText_appendImage (HText * text, HTChildAnchor * anchor,
			const char *alt, const char * align, BOOL isMap)
{
    if (text && anchor) {
        HTParentAnchor * dest = (HTParentAnchor *) HTAnchor_followMainLink((HTAnchor *) anchor);
	char * uri = HTAnchor_address((HTAnchor *) dest);
	if (SHOW_MSG) {
	    HTTrace("Image %s", uri);
	}
	HT_FREE(uri);
    }
}

void HText_endAnchor (HText * text) {}
void HText_appendText (HText * text, const char * str) {}
void HText_appendCharacter (HText * text, char ch) {}
void HText_endAppend (HText * text) {}
void HText_setStyle (HText * text, HTStyle * style) {}
void HText_beginAppend (HText * text) {}
void HText_appendParagraph (HText * text) {}

PRIVATE Robot * Robot_new (void)
{
    Robot * me;
    if ((me = (Robot *) HT_CALLOC(1, sizeof(Robot))) == NULL ||
	(me->tv = (struct timeval*) HT_CALLOC(1, sizeof(struct timeval))) == NULL)
	HT_OUTOFMEM("Robot_new");
    me->htext = 0;
    me->tv->tv_sec = DEFAULT_TIMEOUT;
    me->cwd = HTGetCurrentDirectoryURL();
    me->output = OUTPUT;
    me->urilist = HTList_new();
    me->count = 0;

    /* We keep an extra timeout request object for the timeout_handler */
    me->timeout = HTRequest_new();

    /* Bind the Robot object together with the Request Object */
    me->request = HTRequest_new();
    HTRequest_setContext (me->request, me);
    HTRequest_setPreemptive(me->request, YES);

    /* Make a new profile */
    HTProfile_newPreemptiveRobot ("w3clibtcl", "1.0");

    return me;
}

/*	Delete a Command Line Object
**	----------------------------
*/
PRIVATE BOOL Robot_delete (Robot * me)
{
    if (me) {
	if (me->urilist) {
	    HTList *cur = me->urilist;
	    char *temp;
	    while ((temp = (char *) HTList_nextObject(cur))) {
		HT_FREE(temp);
	    }
	    HTList_delete(cur);	
	}
	if (me->htext) {
	    HText_free(me->htext);
	}
	if (me->output && me->output != STDOUT) fclose(me->output);
	    
	HT_FREE(me->cwd);
	HT_FREE(me->tv);

	/* Delete the profile */
#if 0
	HTProfile_delete();
#endif
	HT_FREE(me);
	return YES;
    }
    return NO;
}

/* MAIN TCL PROCS */

#if 0
char *Reference_List (HText *text, BOOL titles)
{
    char *temp = malloc(1000);
    char *output = malloc(1000);
    int refs = HText_sourceAnchors(text);
    if (refs <= 0) {
        return("\n\nThere are no references from this document.\n\n");
    } else {
        int cnt;
        StrAllocCat(output,"\n*** References from this document ***\n");
        for (cnt=1; cnt<=refs; cnt++) {
            HTAnchor *dest =
                HTAnchor_followMainLink((HTAnchor *)
                                        HText_childNumber(text, cnt));
            HTParentAnchor * parent = HTAnchor_parent(dest);
            char * address =  HTAnchor_address(dest);
            const char * title = titles ? HTAnchor_title(parent) : NULL;
            sprintf(temp, "[%d] ", cnt);
            StrAllocCat(output, temp);
	    sprintf(temp, "%s\n",
                    (char *)(title ? title : address));
            StrAllocCat(output, temp);
            HT_FREE(address);
        }
    }
}      
 #endif

#if 1
char *Reference_List (Robot *mr)
{
    HTList *copy = mr->urilist;
    char *output = NULL;
    char *number = malloc(sizeof("9999 :"));
    char *index = malloc(1000);
    int i = 1;

    int refs = HText_sourceAnchors(mr->htext);
    if (refs <= 0) {
        return("\n\nThere are no references from this document.\n\n");
    } else {
	StrAllocCat(output, "List of references: \n");
	sprintf(number, "%d total references\n", mr->count);
    
	while ((index = (char *) HTList_nextObject(copy))) {

	    sprintf(number, "[%d] : ", i++);
	    StrAllocCat(output, number);
	    StrAllocCat(output, index);
	    StrAllocCat(output, "\n");
	    HT_FREE(index);
	}
	HTList_delete(copy);
	return(output);
    }
}      
#endif

int getURL_tcl(ClientData clientData, Tcl_Interp *interp, 
 	       int argc, char **argv)
{
    if (argc >= 3) {
	char *listcheck = argv[1];
	char *url       = argv[2];
	char * traceflags = argc==4 ? argv[3] : "";
    
	if (url && (strcmp(listcheck, "listrefs")==0)) {

	    /* Create a new robot object */
	    Robot *robot = Robot_new();

	    /* Turn on trace messages */
	    if (argc==4) HTSetTraceMessageMask(traceflags);

	    /* Set up the HTML parser */
	    {
		HTList * converters = HTFormat_conversion();
		HTMLInit(converters);
	    }

	    /* Find an anchor and load it */
	    robot->anchor = (HTParentAnchor *) HTAnchor_findAddress(url);
	    HTLoadAnchor((HTAnchor *) robot->anchor, robot->request);

	    /* Update TcL return */
	    Tcl_AppendResult(interp, Reference_List(robot), NULL);
	    Robot_delete(robot);
	    return TCL_OK;
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;
    } 
    if (argc == 2) {
	char *url = argv[1];
    
	if (url) {
	    /* Create a new robot object */
	    Robot *robot = Robot_new();
	    HTChunk *chunk = 0;

	    /* Find an anchor and load it */
	    HTRequest_setOutputFormat(robot->request, WWW_SOURCE);
	    chunk = HTLoadToChunk(url, robot->request);

	    /* Update TcL return */
	    Tcl_AppendResult(interp, HTChunkData(chunk), NULL);
	    Robot_delete(robot);
	    HTChunk_delete(chunk);
	    return TCL_OK;
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;
    }
    Tcl_AppendResult(interp, err_string, argv[0], " [listrefs] url traceflags", NULL);
    return TCL_ERROR;
}	

int putURL_tcl(ClientData clientData, Tcl_Interp *interp, 
 	       int argc, char **argv) 
{
    if (argc == 3) {
	char *url 	      = argv[1];
	char *dest 	      = argv[2];
	if (url && dest) {

	    HTParentAnchor *source = (HTParentAnchor *) HTAnchor_findAddress(url);
	    HTRequest *req    = HTRequest_new();
	    HTChunk *chunk	= HTChunk_new(0x2000);
	    HTStream *stream = HTStreamToChunk(req, &chunk, 0);
	    BOOL work;
	    HTRequest_setOutputFormat(req, DEFAULT_FORMAT);
	    HTRequest_setOutputStream(req, stream);
	    HTRequest_setPreemptive(req, YES);
	    work = HTPutAbsolute (source, dest, req);

	    Tcl_AppendResult(interp, work ? "YES" : "NO", HTChunkData(chunk), NULL);
	    HT_FREE(url);
	    HTRequest_kill(req);
	    HTAnchor_delete(source);
	    HT_FREE(stream);
	    HTChunkFree(chunk);
	    return TCL_OK;
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;
    }
    else {
	Tcl_AppendResult(interp, err_string, argv[0], " url destination", NULL);
	return TCL_ERROR;
    }
}

int deleteURL_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {

    if (argc == 2) {
	char *url 	      = argv[1];
	if (url) {

	    HTRequest *req    = HTRequest_new();
	    HTChunk *chunk	= HTChunk_new(0x2000);
	    HTStream *stream = HTStreamToChunk(req, &chunk, 0);
	    BOOL work;
	    HTRequest_setOutputFormat(req, DEFAULT_FORMAT);
	    HTRequest_setOutputStream(req, stream);
	    HTRequest_setPreemptive(req, YES);

	    work = HTDeleteAbsolute(url, req);

	    Tcl_AppendResult(interp, work ? "YES" : "NO", HTChunkData(chunk), NULL);
	    HT_FREE(url);
	    HTRequest_kill(req);
	    HT_FREE(stream);
	    HTChunkFree(chunk);
	    return TCL_OK;
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;
    }
    else {
	Tcl_AppendResult(interp, err_string, argv[0], " url", NULL);
	return TCL_ERROR;
    }
}

int postURL_tcl(ClientData clientData, Tcl_Interp *interp, 
		int argc, char **argv) {

    if (argc == 3) {
	char *url 	      = argv[1];
	char *dest 	      = argv[2];
	if (url && dest) {

	    HTParentAnchor *source = (HTParentAnchor *) HTAnchor_findAddress(url);
	    HTRequest *req    = HTRequest_new();
	    HTChunk *chunk	= HTChunk_new(0x2000);
	    HTStream *stream = HTStreamToChunk(req, &chunk, 0);
	    BOOL work;
	    HTRequest_setOutputFormat(req, DEFAULT_FORMAT);
	    HTRequest_setOutputStream(req, stream);
	    HTRequest_setPreemptive(req, YES);
	    work = HTPostAbsolute (source, dest, req);

	    Tcl_AppendResult(interp, work ? "YES" : "NO", HTChunkData(chunk), NULL);
	    HT_FREE(url);
	    HTRequest_kill(req);
	    HTAnchor_delete(source);
	    HT_FREE(stream);
	    HTChunkFree(chunk);
	    return TCL_OK;
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;
    }
    else {
	Tcl_AppendResult(interp, err_string, argv[0], " url destination", NULL);
	return TCL_ERROR;
    }
}

int traceURL_tcl(ClientData clientData, Tcl_Interp *interp, 
		 int argc, char **argv) {

    if (argc == 2) {
	char *url 	      = argv[1];
	if (url) {

	    HTRequest *req    = HTRequest_new();
	    BOOL work;
	    HTChunk *chunk	= HTChunk_new(0x2000);
	    HTStream *stream = HTStreamToChunk(req, &chunk, 0);
	    HTRequest_setOutputFormat(req, DEFAULT_FORMAT);
	    /*	    HTRequest_setOutputStream(req, stream);*/
	    HTRequest_setPreemptive(req, YES);

	    work = HTTraceAbsolute(url, req);

	    Tcl_AppendResult(interp, work ? "YES" : "NO", HTChunkData(chunk), NULL);
	    HT_FREE(url);
	    HTRequest_kill(req);
	    HT_FREE(stream);
	    HTChunkFree(chunk);
	    return TCL_OK;
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;
    }
    else {
	Tcl_AppendResult(interp, err_string, argv[0], " url", NULL);
	return TCL_ERROR;
    }
}

int optionsURL_tcl(ClientData clientData, Tcl_Interp *interp, 
		   int argc, char **argv) {

    if (argc == 2) {
	char *url 	      = argv[1];
	if (url) {

	    HTRequest *req    = HTRequest_new();
	    BOOL work;
	    HTChunk *chunk	= HTChunk_new(0x2000);
	    HTStream *stream = HTStreamToChunk(req, &chunk, 0);
	    HTRequest_setOutputFormat(req, DEFAULT_FORMAT);
	    HTRequest_setOutputStream(req, stream);
	    HTRequest_setPreemptive(req, YES);

	    work = HTOptionsAbsolute(url, req);

	    Tcl_AppendResult(interp, work ? "YES" : "NO", HTChunkData(chunk), NULL);
	    HT_FREE(url);
	    HTRequest_kill(req);
	    HT_FREE(stream);
	    HTChunkFree(chunk);
	    return TCL_OK;
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;
    }
    else {
	Tcl_AppendResult(interp, err_string, argv[0], " url", NULL);
	return TCL_ERROR;
    }
}










