/*
**	@(#) $Id$
**	
**	W3C Webbot can be found at "http://www.w3.org/Robot/"
**	
**	Copyright Å© 1995-1998 World Wide Web Consortium, (Massachusetts
**	Institute of Technology, Institut National de Recherche en
**	Informatique et en Automatique, Keio University). All Rights
**	Reserved. This program is distributed under the W3C's Software
**	Intellectual Property License. This program is distributed in the hope
**	that it will be useful, but WITHOUT ANY WARRANTY; without even the
**	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
**	PURPOSE. See W3C License http://www.w3.org/Consortium/Legal/ for more
**	details.
**
**  Authors:
**	JP		John Punin
**
**  History:
**	Oct 1998	Written
*/

#include "HTQueue.h"

HTList * HTQueue_new(void)
{
  return HTList_new();
}

BOOL HTQueue_delete(HTList *me)
{
  return HTList_delete(me);
}

BOOL HTQueue_enqueue(HTList *me,void *newObject)
{
  return  HTList_addObject(me,newObject);
}
BOOL HTQueue_append(HTList *me,void *newObject)
{
  return  HTList_appendObject(me,newObject);
}

BOOL HTQueue_dequeue(HTList *me)
{
  return HTList_removeFirstObject(me) ? YES : NO;
}

BOOL HTQueue_isEmpty(HTList *me)
{
  return HTList_isEmpty(me);
}

void * HTQueue_headOfQueue(HTList *me)
{
  return HTList_firstObject(me);
}

int HTQueue_count(HTList *me)
{
  return HTList_count(me);
}

