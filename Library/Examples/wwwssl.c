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
**	Sample showing how to use libwww with ssl
*/

#include "WWWLib.h"			      /* Global Library Include file */
#include "WWWMIME.h"				    /* MIME parser/generator */
#include "WWWNews.h"				       /* News access module */
#include "WWWHTTP.h"				       /* HTTP access module */
#include "WWWFTP.h"
#include "WWWFile.h"
#include "WWWGophe.h"
#include "WWWInit.h"
#include "WWWSSL.h"

#define APP_NAME		"libwww-ssl"
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

    /* Close down SSL */
    HTSSLhttps_terminate();

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

    /* And the traces... */
#if 1
    HTSetTraceMessageMask("sop");
#endif

    /* Set the SSL protocol method. By default, it is the highest
       available protocol. Setting it up to SSL_V23 allows the client
       to negotiate with the server and set up either TSLv1, SSLv3, 
       or SSLv2 */
    HTSSL_protMethod_set (HTSSL_V23);

    /* Set the certificate verification depth to 2 in order to be able to
       validate self signed certificates */
    HTSSL_verifyDepth_set (2);

    /* Register SSL stuff for handling ssl access */
    HTSSLhttps_init(YES);

    /* Need our own trace and print functions */
    HTPrint_setCallback(printer);
    HTTrace_setCallback(tracer);

    /* Add our own filter to terminate the application */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

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

