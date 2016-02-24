/*

  					W3C Sample Code Library libwww RDF Parser


!
  RDF Parser Based on Expat and SiRPAC
!

Written and integrated into libwww by John Punin - thanks!

This module is implemented by HTRDF.c, and is a part
of the  W3C Sample Code Library.

This RDF parser is based on Janne Saarela's Java based
SiRPAC and
James Clark's expat XML
parser which isincluded in the libwww CVS code base where I compile is
as one library: libexpat.a.
See the external modules that libwww works with
for details.
*/

#ifndef HTRDF_H
#define HTRDF_H

#ifdef __cplusplus
extern "C" { 
#endif 

#include "HTHash.h"

#define RDFMS "http://www.w3.org/1999/02/22-rdf-syntax-ns#"
#define RDFSCHEMA "http://www.w3.org/TR/WD-rdf-schema#"
#define XMLSCHEMA "xml"

/*
.
  RDF Triple Class
.
*/

typedef struct _HTTriple HTTriple;

extern HTTriple * HTTriple_new (char * p, char * s, char * o);
extern BOOL HTTriple_delete (HTTriple * me);
extern void HTTriple_print (HTTriple * me);
extern char * HTTriple_subject (HTTriple * me);
extern char * HTTriple_predicate (HTTriple * me);
extern char * HTTriple_object (HTTriple * me);

/*
.
  RDF Element Class
.
*/

typedef struct _HTElement HTElement;

extern HTElement * HTElement_new (char * sName, HTAssocList * al);
extern HTElement * HTElement_new2 (char * sContent);
extern BOOL HTElement_addData (HTElement *me, char * sContent);
extern BOOL HTElement_delete (HTElement * me);
extern BOOL HTElement_addChild (HTElement * me, HTElement * element);
extern BOOL HTElement_addAttribute (HTElement * me, char * sName, char * sValue);
extern BOOL HTElement_removeAttribute (HTElement * me, char * sName);
extern char * HTElement_getAttribute (HTElement * me, char * sName);
extern char * HTElement_getAttribute2 (HTElement * me, char * sNamespace, char * sName);
extern BOOL HTElement_addTarget (HTElement * me, HTElement * element);
extern HTElement * HTElement_target (HTElement * me);
extern BOOL HTElement_instanceOfData (HTElement * me);

/*
.
  RDF Parser & Compiler Definition
.

These methods create and deletes an RDF Parser/Compiler (SIRPAC)
(
  Create and Delete Parser Instance
)
*/

typedef struct _HTRDFParser HTRDF;

extern HTRDF * HTRDF_new (void);
extern BOOL HTRDF_delete (HTRDF * me);

/*
(
  Callback Handler Announcing a new RDF Parser Object
)

When a RDF parser object is created, the stream
checks to see if there are any callbacks registered which should be notified
about the new stream instance. If that is the case then this callback is
called and a pointer to the RDF parser passed along. The output stream is
the target that was originally set for the request object before the request
was issued.
*/

typedef void HTRDFCallback_new (
	HTStream *		me,
	HTRequest *		request,
	HTFormat 		target_format,
	HTStream *		target_stream,
	HTRDF *                 rdfparser,
	void *                  context);

/*

  Register RDF Parser Creation Notification Callback


@@@Should be handled via XML names spaces@@@
*/

extern BOOL HTRDF_registerNewParserCallback (HTRDFCallback_new *, void * context);

/*
(
  Callback Handler Announcing a new RDF Triple
)

Handler announcing that a new triple has been generated.
*/

typedef void HTTripleCallback_new (
	HTRDF *		rdfp,
	HTTriple *	t,
	void *		context);

/*

  Register RDF Triple Creation Notification Callback

*/

extern BOOL HTRDF_registerNewTripleCallback (
	HTRDF *			me,
	HTTripleCallback_new * 	cbf,
	void * 			context);

/*
(
  Set Address
)

Saves the name of the source document for later inspection if needed
*/

extern BOOL HTRDF_setSource (HTRDF * me, char * source);

/*
(
  Resolve Symbolic References
)

Go through the m_vResolveQueue and assign direct object reference for each
symbolic reference
*/

extern BOOL HTRDF_resolve(HTRDF *me);

/*
(
  Find Suitable Start Element
)

Given an XML document (well-formed HTML, for example), look for a suitable
element to start parsing from
*/

extern BOOL HTRDF_processXML(HTRDF *me, HTElement *root);

/*
(
  Return the root element pointer.
)

This requires the parsing has been already done.
*/

extern HTElement * HTRDF_root(HTRDF *me);

/*
(
  Return the full namespace URI for a given prefix sPrefix.
)

The default namespace is identified with xmlns prefix. The namespace of xmlns
attribute is an empty string.
*/

extern char * HTRDF_namespace (HTRDF * me, char * sPrefix);

/*
(
  Parsing Literal or Resource?
)

Methods to determine whether we are parsing parseType="Literal" or
parseType="Resource"
*/

extern BOOL HTRDF_parseLiteral(HTRDF *me);
extern BOOL HTRDF_parseResource(HTRDF *me);

/*
(
  Resolve Later
)

Add the element e to the m_vResolveQueue to be resolved later.
*/

extern void HTRDF_resolveLater(HTRDF *me,HTElement *e);

/*
(
  Register ID
)

Add an element e to the Hashtable m_hIDtable which stores all nodes with
an ID
*/

extern void HTRDF_registerID(HTRDF *me, char * sID,HTElement *e);

/*
(
  Register Resource
)

Add an element e to the Vector m_vResources which stores all nodes with an
URI
*/

extern void HTRDF_registerResource(HTRDF *me,HTElement *e);

/*
(
  Look for Node
)

Look for a node by name sID from the Hashtable m_hIDtable of all registered
IDs.
*/

extern HTElement *HTRDF_lookforNode(HTRDF *me, char * sID);

/*
(
  If Element from RDF Schema?
)

Check if the element e is from the namespace of the RDF schema by comparing
only the beginning of the expanded element name with the canonical RDFMS
URI
*/

extern BOOL HTRDF_isRDF(HTRDF *me, HTElement *ele);
extern BOOL HTRDF_isRDFroot(HTRDF *me, HTElement *ele);

/*
(
  Is the element a Description?
)
*/

extern BOOL HTRDF_isDescription(HTRDF *me, HTElement *ele);

/*
(
  Is the element a Predicate? 
)

This method matches all properties but those from RDF namespace
*/

extern BOOL HTRDF_isTypedPredicate(HTRDF *me, HTElement *e);

/*
(
  Is the element a Container?
)
*/

extern BOOL HTRDF_isContainer(HTRDF *me, HTElement *e);

/*
(
  Is the element a Bag?
)
*/

extern BOOL HTRDF_isBag(HTRDF *me, HTElement *e);

/*
(
  Is the element an Alternative?
)
*/

extern BOOL HTRDF_isAlternative(HTRDF *me, HTElement *e);

/*
(
  Is the element a Sequence?
)
*/

extern BOOL HTRDF_isSequence(HTRDF *me, HTElement *e);

/*
(
  Is the element a ListItem?
)
*/

extern BOOL HTRDF_isListItem (HTRDF *me, HTElement *e);

/*
(
  Start processing an RDF/XML document instance from the root element rdf.
)
*/

extern BOOL HTRDF_processRDF(HTRDF *me, HTElement *ele);

/*

processDescription manages Description elements

  
    description
  
    The Description element itself
  
    inPredicate
  
    Is this is a nested description
  
    reificate
  
    Do we need to reificate
  
    createBag
  
    Do we create a bag container
  
    return
  
    An ID for the description

*/

extern char * HTRDF_processDescription (
	HTRDF *		me,
	HTElement *	description,
	BOOL		inPredicate,
	BOOL		reificate,
	BOOL		createBag);

/*
(
  Manage the typedNode production in the RDF grammar.
)
*/

extern char * HTRDF_processTypedNode(HTRDF *me, HTElement *e);

/*
(
  Special method to deal with rdf:resource attribute
)
*/

extern char * HTRDF_getResource(HTRDF *me,HTElement *e);

/*
(
  Create a new reification ID
)

Using a name part and an incremental counter m_iReificationCounter.
*/

extern char * HTRDF_newReificationID (HTRDF *me);

/*
(
  Create a new triple
)

and add it to the m_triples List
*/

extern void HTRDF_addTriple (HTRDF *me, char * sPredicate, char * sSubject,
			     char * sObject);

/*
(
  Create New Bag
)

allows one to determine whether SiRPAC produces Bag instances for each
Description block. The default setting is not to generate them.
*/

extern void HTRDF_createBags (HTRDF *me, BOOL b);

/*
(
  Set Output Stream
)

Set output stream for RDF parser
*/

extern void HTRDF_setOutputStream (HTRDF *me, HTStream *ostream);

/*
.
  RDF Converter Streams
.

A set of converter streams using the
HTRDF Parser object
(
  RDF To Triple Converter
)

This stream converter converts an RDF stream to triples that are passed to
the application using the new triples callback
handler. The RDF object itself can be obtained using the
new RDF parser object callback
*/

extern HTConverter HTRDFParser_new;

/*
(
  Print RDF Triple Converter
)

This stream converter converts an RDF stream to triples and sends them downstrem
as (predicate, subject, object). This can for example be used to
print them out to stdout etc.
*/

extern HTConverter HTRDFToTriples;

/*

(
  Parse a file of RDF
)

This function parses a file of RDF in a synchronous, non-blocking
way.  In other words, the file is not asynchronously loaded.  

If new_triple_callback is NULL, the default triple handler is 
invoked.  The context pointer will be available in the triple
callback function. 

Returns YES if the file is successfully parsed; otherwise NO is
returned and an error message is logged. 
*/

extern BOOL HTRDF_parseFile (const char *file_name, 
                             HTTripleCallback_new * new_triple_callback,
                             void *context);

/*
(
  Parse a buffer of RDF
)

This function parses a buffer of RDF in a synchronous, non-blocking way.  

If new_triple_callback is NULL, the default triple handler is 
invoked.  The context pointer will be available in the triple
callback function. 

Returns YES if the buffer is successfully parsed; otherwise NO is
returned and an error message is logged. 
*/

extern BOOL HTRDF_parseBuffer (const char *buffer, 
		               const char *buffer_name, 
                               int buffer_len, 
                               HTTripleCallback_new * new_triple_callback,
                               void *context);

/*

*/

#ifdef __cplusplus
}
#endif

#endif  /* HTRDF_H */

/*

  

  @(#) $Id$

*/





