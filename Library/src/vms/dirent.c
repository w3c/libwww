/* file changed to make it VMS compatible. NOT ALL UNIX FEATURES WORK */

#include <ssdef.h>
#include <rmsdef.h>
#include "dirent.h"

/* routines are not reentrant... */

#define NO_VERSIONS


DIR *opendir(char *dirname)
{
static DIR dir;
char *closebracket;
long status;
struct dsc$descriptor_s entryname_desc;
struct dsc$descriptor_s dirname_desc;
char DirEntry[256];
char VMSentry[256];
char UnixEntry[256];
int index;
char *dot;

   /* check if directory exists */
   /* dirname can look like /disk$user/duns/www/test/multi    */
   /* or like               /disk$user/duns/www/test/multi/   */
   /* DirEntry should look like     disk$user:[duns.www.test]multi in both cases */
   /* dir.dirname should look like  disk$user:[duns.www.test.multi] */
   strcpy(UnixEntry,dirname);
   if (UnixEntry[strlen(UnixEntry)-1] != '/')
      strcat(UnixEntry,"/");

   strcpy(DirEntry, HTVMS_name("",UnixEntry));
   strcpy(dir.dirname, DirEntry);
   index = strlen(DirEntry) - 1;

   if (DirEntry[index] == ']')
      DirEntry[index] = '\0';

   if ((dot = strrchr(DirEntry,'.')) == NULL)
   {  /* convert disk$user:[duns] into disk$user:[000000]duns.dir */
      char *openbr = strrchr(DirEntry,'[');
      if (!openbr)
      { /* convert disk$user: into disk$user:[000000]000000.dir */
         strcpy(dir.dirname, DirEntry);
         strcat(dir.dirname, "[000000]");
         strcat(DirEntry,"[000000]000000.dir");
      }
      else
      {
         char End[256];
         strcpy(End,openbr+1);
         *(openbr+1) = '\0';
         strcat(DirEntry,"000000]");
         strcat(DirEntry,End);
         strcat(DirEntry,".dir");
      }
   }
   else
   {
      *dot = ']';   
      strcat(DirEntry,".dir");
   }

   dir.context = 0;
   dirname_desc.dsc$w_length = strlen(DirEntry);
   dirname_desc.dsc$b_dtype = DSC$K_DTYPE_T;
   dirname_desc.dsc$b_class = DSC$K_CLASS_S;
   dirname_desc.dsc$a_pointer = &(DirEntry);

   /* look for the directory */
   entryname_desc.dsc$w_length = 255;
   entryname_desc.dsc$b_dtype = DSC$K_DTYPE_T;
   entryname_desc.dsc$b_class = DSC$K_CLASS_S;
   entryname_desc.dsc$a_pointer = VMSentry;

   status = LIB$FIND_FILE(&(dirname_desc), 
                          &entryname_desc, 
                          &(dir.context),
                          0,0,0);
   if (!(status & 0x01))
   { /* directory not found */
      return(NULL);
   }

#if 0
   /* now correct dirname, which looks like disk$user:[duns.www.test]multi */
   /* and should look like disk$user:[duns.www.test.multi] */
   closebracket = strchr(dir.dirname,']');
   *closebracket = '.';
   closebracket = strstr(dir.dirname,".dir");
   *closebracket = '\0';
   strcat(dir.dirname,"]");
#endif

#ifdef NO_VERSIONS
   strcat(dir.dirname,"*.*");
#else
   strcat(dir.dirname,"*.*;*");
#endif
   dir.context = 0;
   dir.dirname_desc.dsc$w_length = strlen(dir.dirname);
   dir.dirname_desc.dsc$b_dtype = DSC$K_DTYPE_T;
   dir.dirname_desc.dsc$b_class = DSC$K_CLASS_S;
   dir.dirname_desc.dsc$a_pointer = &(dir.dirname);
   return(&dir);
}

struct dirent *readdir(DIR *dirp)
{
static struct dirent entry;
long status;
struct dsc$descriptor_s entryname_desc;
char *space, *slash;
char VMSentry[256];
char *UnixEntry;

   entryname_desc.dsc$w_length = 255;
   entryname_desc.dsc$b_dtype = DSC$K_DTYPE_T;
   entryname_desc.dsc$b_class = DSC$K_CLASS_S;
   entryname_desc.dsc$a_pointer = VMSentry;

   status = LIB$FIND_FILE(&(dirp->dirname_desc), 
                          &entryname_desc, 
                          &(dirp->context),
                          0,0,0);
   if (status == RMS$_NMF)
   { /* no more files */
      return(NULL);
   }
   else
   { /* ok */
      if (!(status & 0x01)) return(0);
#ifdef NO_VERSIONS
      space = strchr(VMSentry,';');
#else
      space = strchr(VMSentry,' ');
#endif
      if (space)
         *space = '\0';

      /* convert to unix style... */
      UnixEntry = HTVMS_wwwName(VMSentry);
      slash = strrchr(UnixEntry,'/') + 1;
      strcpy(entry.d_name,slash);
      entry.d_namlen = strlen(entry.d_name);
      entry.d_fileno = 1;
      return(&entry);
   }
}

int closedir(DIR *dirp)
{
long status;

   status = LIB$FIND_FILE_END(&(dirp->context));
   if (!(status & 0x01)) exit(status);
   dirp->context = 0;
   return(0);
}

/* rest of routines not implemented */
#if 0
#ifndef	_POSIX_SOURCE
extern	void seekdir(/* DIR *dirp, int loc */);
extern	long telldir(/* DIR *dirp */);
#endif	/* POSIX_SOURCE */
extern	void rewinddir(/* DIR *dirp */);

#ifndef	lint
#define	rewinddir(dirp)	seekdir((dirp), (long)0)
#endif /* not implemented */
#endif /* not defined for VMS */
