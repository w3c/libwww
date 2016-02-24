
/*



					W3C Sample Code Library libwww Link Class



!The Link Class!
*/
/*
**        (c) COPYRIGHT MIT 1995.
**        Please first read the full copyright statement in the file COPYRIGH.
*/
/*

The HTLink class represents links between anchor objects. By keeping the link as a object and
not as part of the anchor we are capable of handling link semantics in a much
more organized way. For example, we can then search for link types among all
the link objects that we have created. Anchor
objects are bound together using Link objects. Each anchor can be the
source or destination of zero, one, or more links from and to other
anchors.

Link information can come from many places - two classic examples are the
HTML LINK element and the HTTP Link header field.
Libwww supports both - the HTML LINK element is handled by the HTML parser and the HTTP Link header field
is handled by the MIME parser.

The Webbot uses the link class to maintain the
information of the Web that it is seeing when traversing the Web as a
robot.

This module is implemented by HTLink.c, and it is a
part of the  W3C Sample Code
Library.
*/

#ifndef HTLINK_H
#define HTLINK_H

#ifdef __cplusplus
extern "C" { 
#endif 

typedef struct _HTLink        HTLink;

#include "WWWUtil.h"
#include "HTMethod.h"
#include "HTAnchor.h"
/*

.Creation and Deletion Methods.

These are the methods for crating and deleting new link objects

(Create a new Link Object)
*/
typedef HTAtom *         HTLinkType;

typedef enum _HTLinkResult {
    HT_LINK_INVALID = -1,
    HT_LINK_NONE = 0,
    HT_LINK_ERROR,
    HT_LINK_OK
} HTLinkResult;

struct _HTLink {
    HTAnchor *          dest;              /* The anchor to which this leads */
    HTLinkType          type;                      /* Semantics of this link */
    HTMethod            method;            /* Method for this link, e.g. PUT */
    HTLinkResult        result;    /* Result of any attempt to get this link */
};

HTLink * HTLink_new (void);
/*

(Delete a Link Object)

A link can be removed as any other object
*/
BOOL HTLink_delete (HTLink * link);
/*

(Remove All Link Information from an Anchor)

This is normally a part of deleting anchor objects.
*/
extern BOOL HTLink_removeAll (HTAnchor * me);
/*

.Predefined Link Types.

Just for ease of use, we define a seet of commonly used link types. You can
ofcourse use any other link type you want.
*/
#define HT_LR_PERM_REDIRECT        HTAtom_for("PERMANENT_REDIRECTION")
#define HT_LR_TEMP_REDIRECT        HTAtom_for("TEMPORARY_REDIRECTION")
#define HT_LR_SEE_OTHER            HTAtom_for("SEE_OTHER")
/*

.Handle Link Between Anchors.

As mentioned, link objects are the ones that bind anchor objects together
in a Web like structure

(Add a Link between two Anchors)

This method creates a new link between two anchor
objects.
*/
extern BOOL HTLink_add (
        HTAnchor *        source,
        HTAnchor *        destination, 
        HTLinkType        type,
        HTMethod          method);
/*

(Remove All Links Between two Anchors)

Removes link information from one anchor to another.
*/
extern BOOL HTLink_remove (
        HTAnchor *        source,
        HTAnchor *        destination);
/*

(Find a Link)

Find the anchor object between a destination and a source ancher. Return
link object if any, else NULL
*/
extern HTLink * HTLink_find (HTAnchor * source, HTAnchor * destination);
/*

(Find a Link with a given link type)

Returns a link with a given link type or NULL if nothing found
*/
extern HTLink * HTLink_findType (HTAnchor * me, HTLinkType type);
/*

.Link Information.

This is the set of methods for accessing the information carried by a link
object

(Link Destination)

The link destination is the destination anchor pointed to by the link.
*/
extern BOOL HTLink_setDestination (HTLink * link, HTAnchor * dest);
extern HTAnchor * HTLink_destination (HTLink * link);
/*

(Link Types and Semantic Links)

Each link has a sematic representation associated with it. This means that
the application can distinguish between pages based on the semantics of the
link. This is very similar to the LINK tag in HTML pages which
indicates the meaning if this pages to other pages.
*/
extern BOOL HTLink_setType (HTLink * link, HTLinkType type);
extern HTLinkType HTLink_type (HTLink * link);
/*

(Link Method)

The link method is the HTTP method we have performed between the two links.
For example, it can be a POST operation, or a PUT operation where the
operation on the first anchor created a new anchor.
*/
extern BOOL HTLink_setMethod (HTLink * link, HTMethod method);
extern HTMethod HTLink_method (HTLink * link);
/*

(Link Result)

When a link has been used for posting an object from a source to a
destination link, the result of the operation is stored as part of the link
information. This means that we can keep track of which operations we have
performed on a link and hence the application can ask the user whether he or
she wants to do a re-post, for example.
*/
extern BOOL HTLink_setResult (HTLink * link, HTLinkResult result);
extern HTLinkResult HTLink_result (HTLink * link);
/*
*/

#ifdef __cplusplus
}
#endif

#endif /* HTLINK_H */

/*





@(#) $Id$ 

*/
