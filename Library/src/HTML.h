/*		The HTML Parser					HTML.h
**		---------------
*/

#ifndef HTML_H
#define HTML_H

#include "HTUtils.h"
#include "HTAnchor.h"
#include "SGML.h"

typedef struct _HTML *HTML_id;	/* Hidden type */

extern SGML_dtd HTML_dtd;	/* The DTD */
extern HTML_id HTML_new PARAMS((HTParentAnchor * anchor));
extern void HTML_free PARAMS((HTML_id this));
extern HTSGMLContext HTML_SGMLContext PARAMS((HTML_id this));
extern BOOL HTML_Parse PARAMS((
	HTParentAnchor * anchor,
	char (*next_char)() ));
#endif
