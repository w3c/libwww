/* Copyright (C) 1991, 1992 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with the GNU C Library; see the file COPYING.LIB.  If
not, write to the, 1992 Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

/*
 *	POSIX Standard: 9.2.1 Group Database Access	<grp.h>
 */

/*
 * Changed for VMS <grp.h>
 */

#ifndef	_GRP_H

#define	_GRP_H	1

#include <stdio.h>

/* The group structure.	 */
struct group
  {
    char *gr_name;		/* Group name.	*/
    char *gr_passwd;		/* Password.	*/
    int   gr_gid;		/* Group ID.	*/
    char **gr_mem;		/* Member list.	*/
  };


/* Return a new stream open on the group file.  */
extern FILE *grpopen(void);

/* Read a group entry from STREAM, filling in G.
   Return the `struct group' of G if successful, NULL on failure.  */
extern struct group *grpread(FILE *stream, void *g);

/* Return a chunk of memory containing pre-initialized data for grpread.  */
extern void *grpalloc(void);

/* Search for an entry with a matching group ID.  */
extern struct group *getgrgid(int gid);

/* Search for an entry with a matching group name.  */
extern struct group *getgrnam(const char *name);

#endif /* grp.h  */
