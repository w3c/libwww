/*			NEWS ACCESS				HTNews.h
**			===========
**
** History:
**	26 Sep 90	Written TBL
**	29 Nov 91	Downgraded to C, for portable implementation.
*/

#ifndef HTNEWS_H
#define HTNEWS_H

#include "HTAnchor.h"
extern int HTLoadNews PARAMS((const char *arg,
	HTParentAnchor * anAnchor,
	int diag));

#endif /* HTNEWS_H */
