/*								 HTEpToClient.c
**	EXTERNAL PARSE TO CLIENT
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	This module contains the interface between the 
**      the ExtParse module and the client. The dummy function is
**      only here so that clients that use the ExtParse module
**      can overWrite it.
**
** History:
**  	19 Sep 94	Written by Hakon W Lie, howcome@w3.org
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTStream.h"

PUBLIC void HTCallClient ARGS1(HTStream *, me)
{
    return;
}
