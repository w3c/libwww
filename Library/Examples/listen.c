/*
**	SOCKET LISTEN TOOL
**
**	(c) COPRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**  Authors:
**	HFN		Henrik Frystyk Nielsen (frystyk@w3.org)
**
**  History:
**	Nov 24 95	First version
*/

#include "WWWLib.h"			      /* Global Library Include file */
#include "WWWTrans.h"
#include "WWWInit.h"
#include "WWWApp.h"

#ifndef W3C_VERSION
#define W3C_VERSION		"unspecified"
#endif

#define APP_NAME		"libwww-listen"
#define APP_VERSION		W3C_VERSION

#define OUTPUT stderr    /* output is a global target for output redirection */

#define DEFAULT_PORT		1024

#define DEFAULT_FORMAT		WWW_RAW

typedef struct _ListenTool {
    HTRequest *		request;
    int			port;
} ListenTool;


/* ------------------------------------------------------------------------- */

PRIVATE int printer (const char * fmt, va_list pArgs)
{
    return (vfprintf(stdout, fmt, pArgs));
}

PRIVATE int tracer (const char * fmt, va_list pArgs)
{
    return (vfprintf(stderr, fmt, pArgs));
}

/*	Create a Listen Tool Object
**	---------------------------
*/
PRIVATE ListenTool * ListenTool_new (void)
{
    ListenTool * me;
    if ((me = (ListenTool *) HT_CALLOC(1, sizeof(ListenTool))) == NULL)
	HT_OUTOFMEM("ListenTool_new");
    me->port = DEFAULT_PORT;
    return me;
}

/*	Delete a Listen Tool Object
**	---------------------------
*/
PRIVATE BOOL ListenTool_delete (ListenTool * me)
{
    if (me) {
	HTRequest_delete(me->request);
	HT_FREE(me);
	return YES;
    }
    return NO;
}

PRIVATE void Cleanup (ListenTool * me, int status)
{
    ListenTool_delete(me);
    HTEventTerminate();
    HTLibTerminate();
#ifdef VMS
    exit(status ? status : 1);
#else
    exit(status ? status : 0);
#endif
}

PRIVATE void VersionInfo (const char * name)
{
    HTPrint("\nW3C OpenSource Software");
    HTPrint("\n-----------------------\n\n");
    HTPrint("\tListen tool version %s\n", APP_VERSION);
    HTPrint("\tusing the W3C libwww library version %s.\n\n",HTLib_version());
    HTPrint("Format\n\n");
    HTPrint("\t%s -port n [-v[sopt]] [-version]\n\n", name ? name : "listen");
    HTPrint("Please send feedback to the <www-lib@w3.org> mailing list,\n");
    HTPrint("see \"http://www.w3.org/Library/#Forums\" for details\n\n");
}

PRIVATE int terminate_handler (HTRequest * request, HTResponse * response,
			       void * param, int status) 
{
    /* We are done with this request */
    HTRequest_delete(request);

    if (status == HT_ERROR) HTPrint("Can't listen on this port\n");

    /* Terminate libwww */
    HTProfile_delete();

    exit(0);
}

/* ------------------------------------------------------------------------- */
/*				  MAIN PROGRAM				     */
/* ------------------------------------------------------------------------- */

int main (int argc, char ** argv)
{    
    ListenTool * ms = ListenTool_new();
    int status = HT_ERROR;
    int	arg = 0;

    /*
    ** Initiate libwww. Note that we don't call a profile as these set up
    ** all the client side specific things. Here we just want a very basic
    ** server which doesn't do anything but forwarding what it reads to
    ** stdout.
    */
    HTLibInit(APP_NAME, APP_VERSION);

    /* Set up default event loop */
    HTEventInit();

    /* Need our own trace and print functions */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);

    /* Add our own filter to handle termination */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /* Check if we should just provide some help */
    if (argc==1) {
	VersionInfo(argv[0]);
	Cleanup(ms, 0);
    }

    /* Scan command Line for parameters */
    for (arg=1; arg<argc ; arg++) {
	if (*argv[arg] == '-') {

	    /* server port */
	    if (!strncmp(argv[arg], "-p", 2)) { 
		ms->port = (arg+1 < argc && *argv[arg+1] != '-') ?
		    atoi(argv[++arg]) : DEFAULT_PORT;

	    /* Print version and exit */
	    } else if (!strcmp(argv[arg], "-version")) { 
		VersionInfo(argv[0]);
		Cleanup(ms, 0);
		
#ifdef HTDEBUG
	    /* trace flags */
	    } else if (!strncmp(argv[arg], "-v", 2)) {
		HTSetTraceMessageMask(argv[arg]+2);
#endif
	    } else {
		VersionInfo(argv[0]);
		Cleanup(ms, 0);
	    }
        } else {
	    VersionInfo(argv[0]);
	    Cleanup(ms, 0);
	}
    }
	
    /* Set up a tool to listen on this port */
    if (ms->port >= 0) {

	/* Register TCP as the transport */
	HTTransport_add("tcp", HT_TP_SINGLE, HTReader_new, HTWriter_new);

	/* Register the "noop" application layer protocol for reading */
	HTProtocol_add("noop", "tcp", ms->port, NO, NULL, HTLoadSocket);

	/* Set up the request */
	ms->request = HTRequest_new();
	HTRequest_setOutputFormat(ms->request, DEFAULT_FORMAT);
	HTRequest_setOutputStream(ms->request,
				  HTFWriter_new(ms->request, OUTPUT, YES));

	/* Start listening on the socket */
	HTPrint("Listening on port %d\n", ms->port);
	if ((status = HTServeAbsolute("noop://localhost", ms->request)) == NO) {
	    HTPrint("Can't listen on port %d\n", ms->port);
	    Cleanup(ms, -1);
	}

	/* Go into the event loop... */
	if (status == YES) HTEventList_newLoop();
    }
    Cleanup(ms, 0);
    return 0;
}
