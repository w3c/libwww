/*
**	HYPERTEXT OBJECT BUILDER
**
**	(c) COPYRIGHT MIT 1999.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	This generates of a hypertext object and calls the application
**	via callbacks.
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HText.h"
#include "HTextImp.h"

/* Default callbacks that the application can register */
PRIVATE HText_new * 			text_new;
PRIVATE HText_delete *			text_delete;
PRIVATE HText_build *		       	text_build;
PRIVATE HText_addText *			text_addText;
PRIVATE HText_foundLink *		text_foundLink;
PRIVATE HText_beginElement *		text_beginElement;
PRIVATE HText_endElement *		text_endElement;
PRIVATE HText_unparsedBeginElement *	text_unparsedBeginElement;
PRIVATE HText_unparsedEndElement *	text_unparsedEndElement;
PRIVATE HText_unparsedEntity *		text_unparsedEntity;

/* HText handler instance */
struct _HTextImp {
    HText *			app;
    HText_new * 		text_new;
    HText_delete *		text_delete;
    HText_build *		text_build;
    HText_addText *		text_addText;
    HText_foundLink *		text_foundLink;
    HText_beginElement *	text_beginElement;
    HText_endElement *		text_endElement;
    HText_unparsedBeginElement *text_unparsedBeginElement;
    HText_unparsedEndElement *	text_unparsedEndElement;
    HText_unparsedEntity *	text_unparsedEntity;
};

/* --------------------------------------------------------------------------- */

PUBLIC HTextImp * HTextImp_new (HTRequest *	 request,
				HTParentAnchor * anchor,
				HTStream *	 output_stream)
{
    HTextImp * me = NULL;
    if ((me = (HTextImp *) HT_CALLOC(1, sizeof (HTextImp))) == NULL)
	HT_OUTOFMEM("HTextImp_new");
    me->text_new = text_new;    
    me->text_delete = text_delete; 
    me->text_build = text_build;  
    me->text_addText = text_addText;
    me->text_foundLink = text_foundLink;
    me->text_beginElement = text_beginElement;
    me->text_endElement = text_endElement;
    me->text_unparsedBeginElement = text_unparsedBeginElement;
    me->text_unparsedEndElement = text_unparsedEndElement;
    me->text_unparsedEntity = text_unparsedEntity;
    if (me->text_new) me->app = (*me->text_new)(request, anchor, output_stream);
    return me;
}

PUBLIC BOOL HTextImp_delete (HTextImp * me)
{
    if (me) {

	/*
	** Note that we do not call the delete method on the app
	** HText object as this normally stays around after the
	** request has been deleted and certainly it should be
	** deleted by the app and not libwww
	*/
	HT_FREE(me);
	return YES;
    }
    return NO;
}

PUBLIC void HTextImp_build (HTextImp * me, HTextStatus status)
{
    if (me && me->text_build) 
	(*me->text_build)(me->app, status);
}

PUBLIC void HTextImp_addText (HTextImp * 	me,
			      const char * 	buffer,
			      int 		length)
{
    if (me && me->text_addText) 
	(*me->text_addText)(me->app, buffer, length);
}

PUBLIC void HTextImp_foundLink (HTextImp * 	me,
				int 		element_number,
				int 		attribute_number,
				HTChildAnchor *anchor,
				const BOOL *	present,
				const char **	value)
{
    if (me && me->text_foundLink) 
	(*me->text_foundLink)(me->app, element_number, attribute_number,
			   anchor, present, value);
}

PUBLIC void HTextImp_beginElement (HTextImp * 	me,
				   int		element_number,
				   const BOOL *	present,
				   const char **value)
{
    if (me && me->text_beginElement) 
	(*me->text_beginElement)(me->app, element_number, present, value);
}

PUBLIC void HTextImp_endElement (HTextImp * 	me,
				 int		element_number)
{
    if (me && me->text_endElement) 
	(*me->text_endElement)(me->app, element_number);
}

PUBLIC void HTextImp_unparsedBeginElement (HTextImp * 	me,
					   const char * buffer,
					   int 		length)
{
    if (me && me->text_unparsedBeginElement) 
	(*me->text_unparsedBeginElement)(me->app, buffer, length);
}

PUBLIC void HTextImp_unparsedEndElement (HTextImp * 	me,
					 const char * buffer,
					 int 		length)
{
    if (me && me->text_unparsedEndElement) 
	(*me->text_unparsedEndElement)(me->app, buffer, length);
}

PUBLIC void HTextImp_unparsedEntity (HTextImp * 	me,
				     const char * 	buffer,
				     int 		length)
{
    if (me && me->text_unparsedEntity) 
	(*me->text_unparsedEntity)(me->app, buffer, length);
}

/* --------------------------------------------------------------------------- */

PUBLIC BOOL HText_registerCDCallback (HText_new * ncb,
					  HText_delete * dcb)
{
    if (ncb && dcb) {
	text_new = ncb;
	text_delete = dcb;
	return YES;
    }
    return NO;
}

PUBLIC BOOL HText_unregisterCDCallback (void)
{
    text_new = NULL;
    text_delete = NULL;
    return YES;
}

PUBLIC BOOL HText_registerBuildCallback (HText_build * bcb)
{
    text_build = bcb;
    return YES;
}

PUBLIC BOOL HText_registerTextCallback (HText_addText * tcb)
{
    text_addText = tcb;
    return YES;
}

PUBLIC BOOL HText_unregisterTextCallback (void)
{
    text_addText = NULL;
    return YES;
}

PUBLIC BOOL HText_registerLinkCallback (HText_foundLink * lcb)
{
    text_foundLink = lcb;
    return YES;
}

PUBLIC BOOL HText_unregisterLinkCallback (void)
{
    text_foundLink = NULL;
    return YES;
}

PUBLIC BOOL HText_registerElementCallback (HText_beginElement * bcb,
					       HText_endElement * ecb)
{
    text_beginElement = bcb;
    text_endElement = ecb;
    return YES;
}

PUBLIC BOOL HText_unregisterElementCallback (void)
{
    text_beginElement = NULL;
    return YES;
}

PUBLIC BOOL HText_registerUnparsedElementCallback (HText_unparsedBeginElement * ubcb,
						   HText_unparsedBeginElement * uecb)
{
    text_unparsedBeginElement = ubcb;
    text_unparsedEndElement = uecb;
    return YES;
}

PUBLIC BOOL HText_unregisterUnparsedElementCallback (void)
{
    text_unparsedBeginElement = NULL;
    text_unparsedEndElement = NULL;
    return YES;
}

PUBLIC BOOL HText_registerUnparsedEntityCallback (HText_unparsedEntity * tcb)
{
    text_unparsedEntity = tcb;
    return YES;
}

PUBLIC BOOL HText_unregisterUnparsedEntityCallback (void)
{
    text_unparsedEntity = NULL;
    return YES;
}
