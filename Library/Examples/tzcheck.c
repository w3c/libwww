/*
**	@(#) $Id$
**	
**	Other libwww samples can be found at "http://www.w3.org/Library/Examples"
**	
**	Copyright (c© 1995-1998 World Wide Web Consortium, (Massachusetts
**	Institute of Technology, Institut National de Recherche en
**	Informatique et en Automatique, Keio University). All Rights
**	Reserved. This program is distributed under the W3C's Software
**	Intellectual Property License. This program is distributed in the hope
**	that it will be useful, but WITHOUT ANY WARRANTY; without even the
**	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
**	PURPOSE. See W3C License http://www.w3.org/Consortium/Legal/ for more
**	details.
**
**      Simply prints the timezone reported by the system
*/

#include "WWWLib.h"
#include "WWWInit.h"

int main (int argc, char ** argv)
{
    time_t t = 0;
    HTLibInit("test","1.0");
    t = HTGetTimeZoneOffset();
    fprintf(stderr, "Timezone offset is %ld seconds or %ld hours\n", t, t/3600);
    HTLibTerminate();
    return 0;
}
