
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
    HTChunk *           chunk = NULL;

    /* Initiate W3C Reference Library with a client profile */
    HTProfile_newNoCacheClient(APP_NAME, APP_VERSION);

    /* Add our own filter to terminate the application */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /* Turn off any interactions */
    HTAlert_setInteractive(NO);

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

        /* Delete our profile if no load */
        HTProfile_delete();
    }

    return 0;
}
