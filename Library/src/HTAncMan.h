/*

					W3C Sample Code Library libwww Anchor Object




!The Anchor Class Definition!

*/

/*
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
*/

/*

This module is the private part of the anchor object. It has the
functions declarations that are private to the Library and that
shouldn't be used by applications. See also the public part of the
declarition in the HTAnchorModule.

*/

#ifndef HTANCMAN_H
#define HTANCMAN_H

#include "HTAnchor.h"
#include "HTList.h"
#include "HTAtom.h"
#include "HTMethod.h"

#ifdef __cplusplus
extern "C" { 
#endif 


/*

We have a set of Anchor objects which of course should be sub classes
- but - hey - this is C - what do you expect!

(Generic Anchor type)

This is the super class of anchors. We often use this as an argument
to the functions that both accept parent anchors and child anchors. We
separate the first link from the others to avoid too many small
mallocs involved by a list creation. Most anchors only point to one
place.

*/

struct _HTAnchor {
  HTLink	mainLink;	/* Main (or default) destination of this */
  HTList *	links;  	/* List of extra links from this, if any */
  HTParentAnchor * parent;	/* Parent of this anchor (self for adults) */
};

/*

(Anchor for a Parent Object)

These anchors points to the whole contents of a graphic object
(document). The parent anchor of a parent anchor is itself. The parent
anchor now contains all meta information about the object. This is
largely the entity headers in the HTTP specification.

*/

struct _HTParentAnchor {
  /* Common part from the generic anchor structure */
  HTLink	mainLink;	/* Main (or default) destination of this */
  HTList *	links;  	/* List of extra links from this, if any */
  HTParentAnchor * parent;	/* Parent of this anchor (self) */

  /* ParentAnchor-specific information */
  HTList **	children;	/* Hash of subanchors of this, if any */
  HTList *	sources;	/* List of anchors pointing to this, if any */
  void *	document;	/* The document within this is an anchor */
  char *	physical;	/* Physical address */
  char * 	address;	/* Absolute address of this node */
  BOOL		isIndex;	/* Acceptance of a keyword search */

  HTAssocList * headers;        /* Unparsed headers */
  BOOL		header_parsed;	/* Are we done parsing? */

  /* We keep a list of variants of this anchor, if any */
  HTList *	variants;

  /* Entity header fields */
  char *	title;
  HTMethod	allow;	        /* Allowed methods (bit-flag) */

  HTFormat	content_type;	/* Content type */
  HTAssocList *	type_parameters;/* Content type parameters (charset etc.) */

  HTAssocList * meta_tags;      /* Set of metatags found in the HTML text */

  char *	content_base;
  HTList *	content_encoding;
  HTList *	content_language;
  long int	content_length;
  char *	content_location;
  char *	content_md5;

  HTEncoding	cte;	        /* Content-Transfer-Encoding */

  time_t	date;		/* When was the request issued */  
  time_t	expires;	/* When does the copy expire */
  time_t	last_modified;	/* When was this last modified */
  time_t	age;	        /* Cache estimate of age */
  char * 	etag;		/* entity tag */

  char *	derived_from;	/* Opaque string */
  char *	version;	/* Opaque string */
};

/*

(Anchor for a Child Object)

A child anchor is a anchor object that points to a subpart of a
graphic object (document)

*/

struct _HTChildAnchor {
  /* Common part from the generic anchor structure */
  HTLink	mainLink;	/* Main (or default) destination of this */
  HTList *	links;  	/* List of extra links from this, if any */
  HTParentAnchor * parent;	/* Parent of this anchor */

  /* ChildAnchor-specific information */
  char * 	tag;		/* Address of this anchor relative to parent */
};

/*

*/

#ifdef __cplusplus
}
#endif

#endif /* HTANCMAN_H */

/*



@(#) $Id$


*/



