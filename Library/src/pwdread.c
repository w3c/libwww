/* Copyright (C) 1991 Free Software Foundation, Inc.
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
not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

/* CHANGED FOR VMS */

/*
 * <pwdread.c>
 */

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../vms/pwd.h"
#include <types.h>


/* This is the function that all the others are based on.
   The format of the password file is known only here.  */

/* Structure containing info kept by each pwdread caller.  */
typedef struct
  {
    char *buf;
    size_t buflen;
    struct passwd p;
  } pwdread_info;


/* Return a chunk of memory containing a pre-initialized `pwdread_info'.  */
void *pwdalloc(void)
{
  pwdread_info *info = (void *) malloc (sizeof(pwdread_info));
  if (info == NULL)
    return NULL;
  info->buf = NULL;
  info->buflen = 0;
  return info;
}

/* Read a password entry from STREAM, filling in P.  */
struct passwd *pwdread(FILE *stream, void *p)
{
  register pwdread_info *const info = (pwdread_info *) p;
  char *start, *end;

  /* Idiocy checks.  */
  if (stream == NULL)
    {
      errno = EINVAL;
      return NULL;
    }

  do
    if (getline (&info->buf, &info->buflen, stream) == -1)
      return NULL;
  while (info->buf[0] == '#');

  start = info->buf;
  end = strchr (start, ':');
  if (end == NULL)
    return NULL;
  *end = '\0';
  info->p.pw_name = start;

  start = end + 1;
  end = strchr (start, ':');
  if (end == NULL)
    return NULL;
  *end = '\0';
  info->p.pw_passwd = start;

  info->p.pw_uid = (int) strtol (end + 1, &end, 10);
  if (*end != ':')
    return NULL;
  info->p.pw_gid = (int) strtol (end + 1, &end, 10);
  if (*end != ':')
    return NULL;

  start = end + 1;
  end = strchr (start, ':');
  if (end == NULL)
    return NULL;
  *end = '\0';
  info->p.pw_gecos = start;

  start = end + 1;
  end = strchr (start, ':');
  if (end == NULL)
    return NULL;
  *end = '\0';
  info->p.pw_dir = start;

  start = end + 1;
  end = strchr (start, '\n');
  if (end == NULL)
    return NULL;
  *end = '\0';
  info->p.pw_shell = start;

  return &info->p;
}
