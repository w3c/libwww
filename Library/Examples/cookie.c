/*
**	@(#) $Id$
**	
**	More libwww samples can be found at "http://www.w3.org/Library/Examples/"
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
**	Sample showing how to load a document and save it to local file
*/

#include "WWWLib.h"			      /* Global Library Include file */
#include "WWWMIME.h"				    /* MIME parser/generator */
#include "WWWNews.h"				       /* News access module */
#include "WWWHTTP.h"				       /* HTTP access module */
#include "WWWFTP.h"
#include "WWWFile.h"
#include "WWWGophe.h"
#include "WWWInit.h"

#define APP_NAME		"GETTOOL"
#define APP_VERSION		"1.0"
#define DEFAULT_OUTPUT_FILE     "get.out"

PRIVATE int printer (const char * fmt, va_list pArgs)
{
    return (vfprintf(stdout, fmt, pArgs));
}

PRIVATE int tracer (const char * fmt, va_list pArgs)
{
    return (vfprintf(stderr, fmt, pArgs));
}

PRIVATE BOOL setCookie (HTRequest * request, HTCookie * cookie, void * param)
{
    if (cookie) {
	char * addr = HTAnchor_address((HTAnchor *) HTRequest_anchor(request));
	HTPrint("While accessing `%s\', we received a cookie with parameters:\n", addr);
	if (HTCookie_name(cookie))
	    HTPrint("\tName   : `%s\'\n", HTCookie_name(cookie));
	if (HTCookie_value(cookie))
	    HTPrint("\tValue  : `%s\'\n", HTCookie_value(cookie));
	if (HTCookie_domain(cookie))
	    HTPrint("\tDomain : `%s\'\n", HTCookie_domain(cookie));
	if (HTCookie_path(cookie))
	    HTPrint("\tPath   : `%s\'\n", HTCookie_path(cookie));
	if (HTCookie_expiration(cookie) > 0) {
	    time_t t = HTCookie_expiration(cookie);
	    HTPrint("\tExpires: `%s\'\n", HTDateTimeStr(&t, NO));
	}
	HTPrint("\tCookie is %ssecure\n\n", HTCookie_isSecure(cookie) ? "" : "not ");
	HT_FREE(addr);
    }
    return YES;
}

PRIVATE HTAssocList * findCookie (HTRequest * request, void * param)
{
    HTAssocList * alist = HTAssocList_new();	/* Is deleted by the cookie module */
    HTAssocList_addObject(alist, "dummy-name", "dummy-value");
    return alist;
}

/*
**  We get called here from the event loop when we are done
**  loading. Here we terminate the program as we have nothing
**  better to do.
*/
int terminate_handler (HTRequest * request, HTResponse * response,
    		       void * param, int status)
{
    /* Delete our request again */
    HTRequest_delete(request);

    /* Delete our profile */
    HTProfile_delete();

    exit(status ? status : 0);
}

int main (int argc, char ** argv)
{
    int		        status = 0;	
    int		        arg = 0;
    char *              outputfile = NULL;
    char *              getme = NULL;
    HTRequest *         request = NULL;

    /* Initiate W3C Reference Library with a client profile */
    HTProfile_newNoCacheClient(APP_NAME, APP_VERSION);

    /* Need our own trace and print functions */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);

#if 0
    HTSetTraceMessageMask("sop");
#endif

    /* Add our own filter to terminate the application */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /* Setup cookies */
    HTCookie_init();
    HTCookie_setCallbacks(setCookie, NULL, findCookie, NULL);

    /* Set the timeout for long we are going to wait for a response */
    HTHost_setEventTimeout(10000);

    /* Scan command line for parameters */
    for (arg=1; arg<argc; arg++) {
        if (!strcmp(argv[arg], "-o")) { 
            outputfile = (arg+1 < argc && *argv[arg+1] != '-') ?
                argv[++arg] : DEFAULT_OUTPUT_FILE;
            
        } else {
            getme = argv[arg];
        }
    }

    /* Make sure we have an output */
    if (!outputfile) outputfile = DEFAULT_OUTPUT_FILE;

    if (getme && *getme) {
        request = HTRequest_new();

        /* Start the load */
        status = HTLoadToFile(getme, request, outputfile);

        /* Go into the event loop... */
        HTEventList_loop(request);

    } else {
	HTPrint("Type the URI of document you want to load and the name of the local file.\n");
	HTPrint("\t%s <address> -o <localfile>\n", argv[0]);
	HTPrint("For example, %s http://www.w3.org -o w3chome.html\n", argv[0]);

        /* Delete our profile if no load */
        HTProfile_delete();
    }

    return 0;
}
