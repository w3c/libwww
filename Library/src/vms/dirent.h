/* file changed to make it VMS compatible. NOT ALL UNIX FEATURES WORK */
/* NOTE: the routines are NOT re-entrant... */

/*
 * Filesystem-independent directory information.
 */

#ifndef	__dirent_h
#define	__dirent_h

#include <types.h>

#ifndef	_POSIX_SOURCE
#define	d_ino	d_fileno	/* compatability */
#ifndef	NULL
#define	NULL	0
#endif
#endif	/* !_POSIX_SOURCE */

/*
 * Definitions for library routines operating on directories.
 */
#include <descrip.h>

typedef	struct __dirdesc {
#if 0
	int	dd_fd;		/* file descriptor */
	long	dd_loc;		/* buf offset of entry from last readddir() */
	long	dd_size;	/* amount of valid data in buffer */
	long	dd_bsize;	/* amount of entries read at a time */
	long	dd_off;		/* Current offset in dir (for telldir) */
	char	*dd_buf;	/* directory data buffer */
#endif
	long 	context;	/* context descriptor for LIB$FIND_FILE calls */
	char	dirname[255+1];	/* keeps the directory name, including *.* */
	struct dsc$descriptor_s dirname_desc;	/* descriptor of dirname */
} DIR;

extern	DIR *opendir(char *dirname);
extern	struct dirent *readdir(DIR *dirp);
extern	int closedir(DIR *dirp);
#if 0
#ifndef	_POSIX_SOURCE
extern	void seekdir(/* DIR *dirp, int loc */);
extern	long telldir(/* DIR *dirp */);
#endif	/* POSIX_SOURCE */
extern	void rewinddir(/* DIR *dirp */);

#ifndef	lint
#define	rewinddir(dirp)	seekdir((dirp), (long)0)
#endif
#endif /* not defined for VMS */

#include "sys_dirent.h"

#endif	/* !__dirent_h */
