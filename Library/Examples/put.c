/*
**  Puts a URI to an HTTP server
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
	fprintf(stderr, HTChunk_data(result));
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
    HTAnchor * src = NULL;
    HTAnchor * dst = NULL;
    char * src_str = NULL;
    char * dst_str = NULL;
    BOOL status = NO;

    /* Create a new premptive client */
    HTProfile_newNoCacheClient("libwww-PUT", "1.0");

    /* Add our own filter to update the history list */
    HTNet_addAfter(terminate_handler, NULL, NULL, HT_ALL, HT_FILTER_LAST);

    /* Handle command line args */
    if (argc >= 3) {
	src_str = argv[1];
	dst_str = argv[2];
    } else {
	printf("Type the URI of the source and the URI of the destination.\n");
	printf("\t%s <src> <dst>\n", argv[0]);
	printf("For example, %s http://www.w3.org http://myserver/destination.html\n",
	       argv[0]);
	return -1;
    }

    if (src_str && *src_str && dst_str && *dst_str) {

	/* Create a request */
	request = HTRequest_new();

	/* Get an anchor object for the src and dest URIs */
	src = HTAnchor_findAddress(src_str);
	dst = HTAnchor_findAddress(dst_str);

	/* PUT the source to the dest */
	status = HTPutDocumentAnchor(HTAnchor_parent(src), dst, request);

	/* Go into the event loop... */
	if (status == YES) HTEventList_loop(request);

    }

    return 0;
}
