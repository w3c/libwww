
#include "stdlib.h"
#include "stdio.h"
#include "zlib.h"

z_stream z;

#define DEFAULT_BUFFER_SIZE	32*1024
#define DEFAULT_COMPRESSION	Z_DEFAULT_COMPRESSION;

#define MAX(a,b) ((a) >= (b) ? (a) : (b))
#define MIN(a,b) ((a) <= (b) ? (a) : (b))

char *input_buffer 	= NULL;
char *output_buffer 	= NULL;
int  buffer_size 	= DEFAULT_BUFFER_SIZE;
int  compression_rate	= DEFAULT_COMPRESSION;

FILE * fin = stdin;
FILE * fout = stdout;

void HelpScreen (const char *progname)
{
    fprintf(stderr, "\nDeflates using Zlib from stdin to stdout\n");
    fprintf(stderr, "Usage: %s\n", progname ? progname : "deflate");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "\t-b\tBuffer size (default 32K). This has no effect on compression result\n");
    fprintf(stderr, "\t-c\tCompression rate [1..9] where 1 is compress faster and 9 is compress better (default 6)\n");
    fprintf(stderr, "\t-h\tHelp - this help screen\n");
}

int main (int argc, char ** argv)
{
    int arg;
    int status;
    int count;

    /* Scan command line for parameters */
    for (arg=1; arg<argc; arg++) {
	
	if (*argv[arg] == '-') {
    
	    /* Buffer size */
	    if (!strcmp(argv[arg], "-b")) {
		buffer_size = (arg+1 < argc && *argv[arg+1] != '-') ?
		    atoi(argv[++arg]) : DEFAULT_BUFFER_SIZE;
		buffer_size = MAX(buffer_size, 256);

	    /* Compression rate */
	    } else if (!strcmp(argv[arg], "-c")) {
		compression_rate = (arg+1 < argc && *argv[arg+1] != '-') ?
		    atoi(argv[++arg]) : DEFAULT_COMPRESSION;
		compression_rate = MAX(compression_rate, Z_BEST_SPEED);
		compression_rate = MIN(compression_rate, Z_BEST_COMPRESSION);

	    /* Print the help screen and exit */
	    } else if (!strcmp(argv[arg], "-h")) {
		HelpScreen(*argv);
		exit(0);

	    } else {
		HelpScreen(*argv);
		exit(-1);
	    }
	} else {
	    HelpScreen(*argv);
	    exit(-1);
	}
    }

    if ((input_buffer = (char *) calloc(1, buffer_size)) == NULL ||
	(output_buffer = (char *) calloc(1, buffer_size)) == NULL) {
	fprintf(stderr, "Not enough memory\n");
	exit(-1);
    }

    deflateInit(&z, compression_rate);
    z.avail_in = 0;
    z.next_out = output_buffer;
    z.avail_out = buffer_size;
    for ( ; ; ) {
        if ( z.avail_in == 0 ) {
            z.next_in = input_buffer;
            z.avail_in = fread( input_buffer, 1, buffer_size, fin );
        }
	if ( z.avail_in == 0 ) {
	    status = deflate( &z, Z_FINISH );
	    count = buffer_size - z.avail_out;
	    if ( count ) fwrite( output_buffer, 1, count, fout );
	    break;
	}
        status = deflate( &z, Z_NO_FLUSH );
        count = buffer_size - z.avail_out;
        if ( count ) fwrite( output_buffer, 1, count, fout );
        z.next_out = output_buffer;
        z.avail_out = buffer_size;
    }
    fprintf(stderr, "Compressing data: raw data %lu, compressed %lu, factor %.2f, compression level (default = -1) %d, buffer size %d\n",
	    z.total_in, z.total_out,
	    z.total_in == 0 ? 0.0 :
	    (double)z.total_out / z.total_in,
	    compression_rate,
	    buffer_size);
    deflateEnd(&z);
    fflush(stdout);

    if (input_buffer) free(input_buffer);
    if (output_buffer) free(output_buffer);
    return 0;
}
