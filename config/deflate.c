
#include "stdlib.h"
#include "stdio.h"
#include "zlib.h"

z_stream z;

#define BUFFER_SIZE	8196

char input_buffer [BUFFER_SIZE];
char output_buffer [BUFFER_SIZE];

FILE * fin = stdin;
FILE * fout = stdout;

int main (int argc, char ** argv)
{
    int status;
    int count;
    deflateInit(&z, Z_DEFAULT_COMPRESSION);
    z.avail_in = 0;
    z.next_out = output_buffer;
    z.avail_out = BUFFER_SIZE;
    for ( ; ; ) {
        if ( z.avail_in == 0 ) {
            z.next_in = input_buffer;
            z.avail_in = fread( input_buffer, 1, BUFFER_SIZE, fin );
        }
	if ( z.avail_in == 0 ) {
	    status = deflate( &z, Z_FINISH );
	    count = BUFFER_SIZE - z.avail_out;
	    if ( count ) fwrite( output_buffer, 1, count, fout );
	    break;
	}
        status = deflate( &z, Z_NO_FLUSH );
        count = BUFFER_SIZE - z.avail_out;
        if ( count ) fwrite( output_buffer, 1, count, fout );
        z.next_out = output_buffer;
        z.avail_out = BUFFER_SIZE;
    }
    fprintf(stderr, "Compressing data: raw data %lu, compressed %lu, factor %.2f\n",
	    z.total_in, z.total_out,
	    z.total_in == 0 ? 0.0 :
	    (double)z.total_out / z.total_in);
    deflateEnd(&z);
    fflush(stdout);
    return 0;
}
