/* this file was adapted for VMS. NOT ALL UNIX FEATURES WORK */

/*
 *------> THE DESCRIPTION BELOW IS FOR UNIX... <----- 
 * Filesystem-independent directory information.
 * Directory entry structures are of variable length.
 * Each directory entry is a struct dirent containing its file number, the
 * offset of the next entry (a cookie interpretable only the filesystem
 * type that generated it), the length of the entry, and the length of the
 * name contained in the entry.  These are followed by the name. The
 * entire entry is padded with null bytes to a 4 byte boundary. All names
 * are guaranteed null terminated. The maximum length of a name in a
 * directory is MAXNAMLEN, plus a null byte.
 *------> THE DESCRIPTION BELOW IS FOR VMS... <----- 
 * for VMS only d_name, d_namlen and d_ino are defined.
 * d_fileno is a nonzero number to make you think the file is there. The number
 *          has no other meaning.
 */

#ifndef	__sys_dirent_h
#define	__sys_dirent_h

struct	dirent {
#if 0
	off_t		d_off;		/* offset of next disk dir entry */
#endif
	unsigned long	d_fileno;	/* file number of entry */
#if 0
	unsigned short	d_reclen;	/* length of this record */
#endif
	unsigned short	d_namlen;	/* length of string in d_name */
	char		d_name[255+1];	/* name (up to MAXNAMLEN + 1) */
};

#ifndef	_POSIX_SOURCE
/*
 * It's unlikely to change, but make sure that sizeof d_name above is
 * at least MAXNAMLEN + 1 (more may be added for padding).
 */
#define	MAXNAMLEN	255
/*
 * The macro DIRSIZ(dp) gives the minimum amount of space required to represent
 * a directory entry.  For any directory entry dp->d_reclen >= DIRSIZ(dp).
 * Specific filesystem types may use this macro to construct the value
 * for d_reclen.
 */
#undef	DIRSIZ
#define	DIRSIZ(dp) \
	(((sizeof(struct dirent) - (MAXNAMLEN+1) + ((dp)->d_namlen+1)) +3) & ~3)

#endif	/* !_POSIX_SOURCE */
#endif	/* !__sys_dirent_h */
