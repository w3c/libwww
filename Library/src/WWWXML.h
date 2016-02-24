/*

  					W3C Sample Code Library libwww XML/RDF Module


!
  Declaration of W3C Sample Code XML/RDF Module
!
*/

/*
**	(c) COPYRIGHT MIT 1999.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

Thanks so much to John Punin for writing this code!

We use James Clark's expat
XML parser which is very neat indeed. As the code doesn't come as a separate
library, I included it in the libwww CVS code base where I compile is as
one library: libexpat.a. See
the external modules that libwww works with for
details.
*/

#ifndef WWWXML_H
#define WWWXML_H

/*
*/

#ifdef __cplusplus
extern "C" { 
#endif

/*
.
  System dependencies
.

The wwwsys.h file includes system-specific include
files and flags for I/O to network and disk. The only reason for this file
is that the Internet world is more complicated than Posix and ANSI.
*/

#include "wwwsys.h"

/*
(
  The Libwww Expat Wrapper Stream
)
*/

#ifdef HT_EXPAT
#include "HTXML.h"
#endif /* HT_EXPAT */

/*
(
  The RDF Parser Using The XML Parser
)

This RDF parser is based on Janne Saarela's Java based
SiRPAC and
James Clark's expat XML
parser
*/

#ifdef HT_EXPAT
#include "HTRDF.h"
#endif /* HT_EXPAT */

/*

End of XML module
*/

#ifdef __cplusplus
} /* end extern C definitions */
#endif

#endif

/*

  

  @(#) $Id$

*/
