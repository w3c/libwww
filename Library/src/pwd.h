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
 *	POSIX Standard: 9.2.2 User Database Access	<pwd.h>
 */

/*
 *	Changed for VMS <pwd.h>
 */

#ifndef	_PWD_H

#define	_PWD_H	1

#include <stdio.h>

/* The passwd structure.  */
struct passwd
{
  char *pw_name;		/* Username.  */
  char *pw_passwd;		/* Password.  */
  int 	pw_uid;			/* User ID.  */
  int	pw_gid;			/* Group ID.  */
  char *pw_gecos;		/* Real name.  */
  char *pw_dir;			/* Home directory.  */
  char *pw_shell;		/* Shell program.  */
};

/* Return a new stream open on the password file.  */
extern FILE *pwdopen(void);

/* Read a password entry from STREAM, filling in P.
   Return the `struct passwd' of P if successful, NULL on failure.  */
extern struct passwd *pwdread(FILE *stream, void *p);

/* Return a chunk of memory containing pre-initialized data for pwdread.  */
extern void *pwdalloc(void);

/* Search for an entry with a matching user ID.  */
extern struct passwd *getpwuid(int uid);

/* Search for an entry with a matching username.  */
extern struct passwd *getpwnam(const char *name);

#endif /* pwd.h  */
                      
