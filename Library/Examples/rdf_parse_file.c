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
**      Parses one or more files of RDF and for each file, its RDF triples 
**      are output.
**      Test case for the HTRDF_parseFile API.  One or more files
**      are given via the command line and each file is sent to
**      the RDF parser.
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
 * For each file given on the command line, invoke the RDF parser on the
 * file.
 */
int main (int argc, char ** argv)
{
    BOOL status;
    int i;

    if (argc <= 1) {
        (void) fprintf(stderr, "Usage: %s file_name(s)_to_parse\n", argv[0]);
        return 1;
    }

    for (i=1; i < argc; i++) {
        status = HTRDF_parseFile(argv[i], new_triple_handler, NULL);
        if (!status) 
            (void) fprintf (stderr, "An error occured parsing: '%s'\n", argv[i]);
    }

    return 0;
}
