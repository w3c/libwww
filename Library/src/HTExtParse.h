/*                                        ExtParse: Module to get unparsed stream from libwww
                   EXTPARSE: MODULE TO GET UNPARSED STREAM FROM LIBWWW
                                             
   This version of the stream object is a hook for clients that want an unparsed stream
   from libwww. The HTExtParse_put_* and HTExtParse_write routines copy the content of the
   incoming buffer into a buffer that is realloced whenever necessary. This buffer is
   handed over to the client in HTExtParse_free
   
   Part of libwww. See also HTFWriter for writing to C files.
   
  BUGS:
  
      strings written must be less than buffer size.
      
 */

#ifndef HTEXTPARSE_H
#define HTEXTPARSE_H

#include "HTStream.h"


typedef struct {
        void            (*call_client)();
        int             used;      /* how much of the buffer is being used */
        BOOL            finished;   /* document loaded? */
        int             length;    /* how long the buffer is */
        char *          buffer;       /* buffer to store in until client takes over*/
        char *          content_type;
        
} HTExtParseStruct;

/*extern HTStream * HTExtParse PARAMS((void (*CallMeArg)()));*/

/* extern void HTExtParse PARAMS((HTExtParseStruct * eps)); */

extern HTStream* HTExtParse PARAMS((
        HTRequest *             request,
        void *                  param,
        HTFormat                input_format,
        HTFormat                output_format,
        HTStream *              output_stream));


#endif


/*

   end  The HTExtParse_put_* and HTExtParse_write routines are called from MIME*.
   The data pointed to is copied into a new buffer that         is realloced whenever
   necessary.  */
