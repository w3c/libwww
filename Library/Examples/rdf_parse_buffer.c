/*
**	@(#) $Id$
**	
**	Other libwww samples can be found at "http://www.w3.org/Library/Examples"
**	
**	Copyright (c© 1995-2000 World Wide Web Consortium, (Massachusetts
**	Institute of Technology, Institut National de Recherche en
**	Informatique et en Automatique, Keio University). All Rights
**	Reserved. This program is distributed under the W3C's Software
**	Intellectual Property License. This program is distributed in the hope
**	that it will be useful, but WITHOUT ANY WARRANTY; without even the
**	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
**	PURPOSE. See W3C License http://www.w3.org/Consortium/Legal/ for more
**	details.
**
**      Test case for the HTRDF_parseBuffer API.  One or more files
**      are given via the command line and for each file, it is opened,
**      its contents are stored in a buffer and then the API is
**      called.  Each buffer's triples are output to stdout.
**
**	Author: Arthur Barstow
*/

#include "WWWXML.h"

/*
 * Handle new triple events
 */
PRIVATE void new_triple_handler (HTRDF * rdfp, HTTriple * t, void * context)
{
    if (rdfp && t) {
	HTPrint("Received new triple:\n");
	if (HTTriple_predicate(t)) HTPrint("\tPredicate: `%s\'\n", HTTriple_predicate(t));
	if (HTTriple_subject(t))   HTPrint("\tSubject  : `%s\'\n", HTTriple_subject(t));
	if (HTTriple_object(t))    HTPrint("\tObject   : `%s\'\n", HTTriple_object(t));
	HTPrint("\n");
    }
}

/*
 * Given a file, open it and read all of its contents into a buffer
 * and then return the buffer and it length.
 *
 *@param file_name the name of the file
 *@param ret_buff MODIFIED the buffer to be filled
 *@param ret_length MODIFIED the length of the buffer
 *@return NULL if the file the buffer is successfully processed; otherwise
 * a pointer to a static error message is returned.
 */
PRIVATE char * parse_buffer(const char *file_name, char **ret_buff, int *ret_length)
{
    char tmp_buff[512];
    FILE *fp;

    *ret_buff = NULL;
    *ret_length = 0;

    /* If the file does not exist, return now */
    fp = fopen (file_name, "r");
    if (!fp)  /* annotation index file doesn't exist */
        return "File open failed";

    for (;;) {
        int done;
        int buff_len;

        (void) fgets(tmp_buff, sizeof(tmp_buff), fp);
        if (ferror(fp)) {
            (void) fclose (fp);
            return "Error reading file";
        }
        done = feof(fp);
        if (done)
            buff_len = 0;
        else 
            buff_len = strlen (tmp_buff);
        *ret_length = *ret_length + buff_len;
        if (*ret_buff == NULL)
        {
            if (buff_len > 0) {
                *ret_buff = HT_MALLOC(buff_len + 1);
                if (!*ret_buff) {
                    (void) fclose (fp);
                    return "Malloc failure";
                }
                (void) strcpy (*ret_buff, tmp_buff);
            }
        } else {
            if (buff_len > 0) {
                *ret_buff = HT_REALLOC(*ret_buff, (*ret_length) + 1);
                if (!*ret_buff) {
                    (void) fclose (fp);
                    return "Malloc failure";
                }
                (void) strcat (*ret_buff, tmp_buff);
            }
        }
        if (done)
            break;
    }

    /* Cleanup */
    (void) fclose (fp);

    return NULL;
}

/*
 * For each file given on the command line, open it, read its
 * contents into a buffer and then invoke the RDF parser on the
 * buffer.
 */
int main (int argc, char ** argv)
{
    BOOL status;
    char *msg;
    char *buffer = NULL;
    int buff_len = 0;
    int i;

    if (argc <= 1) {
        (void) fprintf(stderr, "Usage: %s file_name(s)_to_parse\n", argv[0]);
        return 1;
    }

    for (i=1; i < argc; i++) {
        msg = parse_buffer(argv[i], &buffer, &buff_len);
        if (!msg) {
            status = HTRDF_parseBuffer(buffer, argv[i], buff_len, new_triple_handler, NULL);
            if (!status)
                (void) fprintf (stderr, "ERROR parsing the buffer from file: '%s'\n", argv[i]);
        }
        else
            (void) fprintf (stderr, "ERROR creating the buffer: '%s'\n", msg);
    }

    return 0;
}
