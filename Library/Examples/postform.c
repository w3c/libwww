/*
**  Posts form data to an HTTP server and saves the result in a chunk
*/

#include "WWWLib.h"
#include "WWWInit.h"

#ifdef LIBWWW_SHARED
#include "HTextImp.h"
#endif

PRIVATE HTChunk * result = NULL;

PRIVATE int terminate_handler (HTRequest * request, HTResponse * response,
			       void * param, int status) 
{
    if (status == HT_LOADED && result && HTChunk_data(result)) {
	fprintf(stderr, "%s", HTChunk_data(result));
	HTChunk_delete(result);
    }

    /* We are done with this request */
    HTRequest_delete(request);

    /* Terminate libwww */
    HTProfile_delete();

    exit(0);
}

int main (int argc, char ** argv)
{
    HTRequest * request = NULL;
    HTAnchor * anchor = NULL;
    HTAssocList * formfields = NULL;
    char * uri = NULL;

    /* Create a new premptive client */
    HTProfile_newNoCacheClient("TestApp", "1.0");

    /* Add our own filter to update the history list */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /* Handle command line args */
    if (argc >= 2) {
	int arg;
	uri = argv[1];
	for (arg=2; arg<argc; arg++) {
	    char * string = argv[arg];
	    char * name = HTNextField(&string);
	    char * value = HTNextField(&string);
	    if (!value) value = "";
	    if (name) {
		char * escaped_name = HTEscape(name, URL_XPALPHAS);
		char * escaped_value = HTEscape(value, URL_XPALPHAS);

		/* Create a list to hold the form arguments */
		if (!formfields) formfields = HTAssocList_new();

		HTAssocList_addObject(formfields, escaped_name, escaped_value);

		HT_FREE(escaped_name);
		HT_FREE(escaped_value);
	    }
	}
    }

    if (uri && formfields) {

	/* Create a request */
	request = HTRequest_new();

	/* Set the default output to "asis" */
	HTRequest_setOutputFormat(request, WWW_SOURCE);

	/* Get an anchor object for the URI */
	anchor = HTAnchor_findAddress(uri);

	/* Post the data and get the result in a chunk */
	result = HTPostFormAnchorToChunk(formfields, anchor, request);

	/* Clean up the form fields */
	HTAssocList_delete(formfields);

	/* Go into the event loop... */
	HTEventList_loop(request);

    } else {
	printf("Type the URI to post to and the arguments for the POST operation. Encode spaces as '+'\n");
	printf("\t%s <uri> 'a=1' 'b=+2+' 'c=3 ...'\n", argv[0]);
    }

    return 0;
}
