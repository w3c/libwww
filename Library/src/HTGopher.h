/*			GOPHER ACCESS				HTGopher.h
**			=============
**
** History:
**	 8 Jan 92	Adapted from HTTP TBL
*/

#ifndef HTGOPHER_H
#define HTGOPHER_H

#include "HTAnchor.h"
extern int HTLoadGopher PARAMS((const char *arg,
	HTParentAnchor * anAnchor,
	int diag));

#endif /* HTGOPHER_H */
