/*							       	     HTDirBrw.c
**	DIRECTORY BROWSING
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**
**	This is unix-specific code in general
**	The module is intended for use in HTFile.c and HTFTP.c where
**	it replaces the old directory browsing routine.
**	The module is only compiled if GOT_READ_DIR is defined
**
** Authors:
**		HF	Henrik Frystyk, CERN, <frystyk@w3.org>
**		AL	Ari Luotonen, CERN, <luotonen@www.cern.ch>
** History:
**	   Mar 94  HF	Written by Henrik Frystyk, frystyk@w3.org,
**			but with some of the Directory stuff brought from
**			HTFile().
**	   Mar 94  AL	Configurable icons.
**	   Apr 94  HF	Icons moved to own module
**	   Jul 94  FM	Compile substitute strftime() for VMS && !DECC (and
**			use sys$share:vaxcrtl/share in .opt file for linking).
**			Insulate free() from _free structure element.
** BUGS:
**
*/

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTMLGen.h"
#include "HTBind.h"
#include "HTAnchor.h"
#include "HTParse.h"
#include "HTFWrite.h"
#include "HTBTree.h"
#include "HTFormat.h"
#include "HTReqMan.h"
#include "HTChunk.h"
#include "HTIcons.h"
#include "HTDescpt.h"
#include "HTError.h"
#include "HTDirBrw.h"					 /* Implemented here */

#ifdef VMS
#include "HTVMSUtils.h"
#endif

#ifdef NO_STRFTIME
PRIVATE char * months[12] = {
    "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
};
#endif


/* Macros and other defines */
#ifdef OLD_CODE
#ifdef USE_DIRENT			       /* Set this for Sys V systems */
#define STRUCT_DIRENT struct dirent
#else
#define STRUCT_DIRENT struct direct
#endif /* USE_DIRENT */
#endif /* OLD_CODE */

#define PUTC(c) (*target->isa->put_character)(target, c)
#define PUTS(s) (*target->isa->put_string)(target, s)
#define START(e) (*target->isa->start_element)(target, e, 0, 0)
#define END(e) (*target->isa->end_element)(target, e)
#define FREE_TARGET (*target->isa->_free)(target)


struct _HTStructured {
	CONST HTStructuredClass *	isa;
	/* ... */
};

/* Globals */
PUBLIC int HTDirReadme = HT_DIR_README_TOP;
PUBLIC int HTDirInfo = HT_DIR_INFO_TOP;
PUBLIC int HTDirAccess = HT_DIR_OK;
PUBLIC unsigned int HTDirShowMask = HT_DIR_SHOW_ICON+HT_DIR_SHOW_SIZE+HT_DIR_KEY_NAME+HT_DIR_SHOW_DATE;

#if 0
PUBLIC unsigned int HTDirShowMask = HT_DIR_SHOW_ICON+HT_DIR_SHOW_MODE+HT_DIR_SHOW_NLINK+HT_DIR_SHOW_OWNER+HT_DIR_SHOW_GROUP+HT_DIR_SHOW_SIZE+HT_DIR_KEY_NAME+HT_DIR_SHOW_DATE;
#endif

PUBLIC BOOL HTDirDescriptions = YES;
PUBLIC BOOL HTDirShowBytes = NO;
PUBLIC int HTDirMinFileLength = 15;
PUBLIC int HTDirMaxFileLength = 22;
PUBLIC int HTDirMaxDescrLength = 25;

/* Type definitions and global variables etc. local to this module */
typedef struct _HTDirKey {
    void *key;				     /* Can sort strings or integers */
    char *filename;
    BOOL is_dir;                                           /* If a directory */
    char *body;			/* Contains all the stuff as date, size etc. */
    HTIconNode *icon;
    HTHrefNode *href;
    char *symlink;
} HTDirKey;

typedef enum _HTShowLength {                        /* Width of each collumn */
    HT_LENGTH_MODE  = 10,
    HT_LENGTH_NLINK = 4,
    HT_LENGTH_OWNER = 8,
    HT_LENGTH_GROUP = 8,
    HT_LENGTH_SIZE  = 6,
    HT_LENGTH_DATE  = 15,
    HT_LENGTH_SPACE = 1
} HTShowLength;

PRIVATE int HTDirFileLength;      /* HTMinDirFileName < x < HTMaxDirFileName */
PRIVATE int HTBodyLength;
PRIVATE char *HTDirSpace = NULL;

/* ------------------------------------------------------------------------- */

/* 							     	FilePerm()
**	Writes the file permissions into strptr.
**	ISN'T THERE A FASTER METHOD???
*/
PRIVATE void FilePerm ARGS2(mode_t, mode, char *, strptr)
{
    if ((mode & S_IFMT) == S_IFREG)
	*strptr++ = '-';
    else if ((mode & S_IFMT) == S_IFDIR)
	*strptr++ = 'd';
    else if ((mode & S_IFMT) == S_IFLNK)
	*strptr++ = 'l';
    else
	*strptr++ = '?';			  /* Hmmm, any better ideas? */

    *strptr++ = (mode & S_IRUSR) ? 'r' : '-';			     /* User */
    *strptr++ = (mode & S_IWUSR) ? 'w' : '-';
    if (mode & S_ISUID) {
	if (mode & S_IXUSR)
	    *strptr++ = 's';
	else
	    *strptr++ = 'S';
    } else if (mode & S_IXUSR)
	*strptr++ = 'x';
    else
	*strptr++ = '-';	
    *strptr++ = (mode & S_IRGRP) ? 'r' : '-';			    /* Group */
    *strptr++ = (mode & S_IWGRP) ? 'w' : '-';
    if (mode & S_ISGID) {
	if (mode & S_IXGRP)
	    *strptr++ = 's';
	else
	    *strptr++ = 'S';
    } else if (mode & S_IXGRP)
	*strptr++ = 'x';
    else
	*strptr++ = '-';
    *strptr++ = (mode & S_IROTH) ? 'r' : '-';			    /* Other */
    *strptr++ = (mode & S_IWOTH) ? 'w' : '-';
    if (mode & S_ISVTX) {
	if (mode & S_IXOTH)
	    *strptr++ = 't';
	else
	    *strptr++ = 'T';
    } else if (mode & S_IXOTH)
	*strptr++ = 'x';
    else
	*strptr++ = '-';
}


#if 0	/* Not needed currently */
/* 							     	CenterStr()
**	Centers str_in into str_out expecting str_out having size length
**	inclusive the terminating '\0'. The output string MUST be long enough.
*/
PRIVATE void CenterStr ARGS3(char *, str_out, char *, str_in, int, length)
{
    char *inptr = str_in;
    char *outptr = str_out + (length-strlen(str_in))/2;
    memset((void *) str_out, ' ', length);
    while ((*outptr++ = *inptr++) != '\0');
    *--outptr = ' ';
    *(str_out+length-1) = '\0';
}
#endif


/*								LeftStr()
**	Like CenterStr(), but result is left-justified.
*/
PRIVATE void LeftStr ARGS3(char *, str_out, char *, str_in, int, length)
{
    char *inptr = str_in;
    char *outptr = str_out;
    memset((void *) str_out, ' ', length);
    while ((*outptr++ = *inptr++));
    *--outptr = ' ';
    str_out[length-1] = 0;
}


/*								RightStr()
**	Like CenterStr(), but result is right-justified.
*/
PRIVATE void RightStr ARGS3(char *, str_out, char *, str_in, int, length)
{
    char *inptr = str_in;
    char *outptr = str_out + length - strlen(str_in) - 1;
    memset((void *) str_out, ' ', length);
    while ((*outptr++ = *inptr++));
}


/* 							     	ItoA()
**	Converts a positive int to a string backwards starting at start+len-1.
*/
PRIVATE void ItoA ARGS3(unsigned int, n, char *, start, char, len)
{
    char *sptr = start+len-1;
    do {
	*sptr-- = n%10 + '0';
    } while ((n /= 10));
}


/* 							     	HTDirSize()
 *	Converts a long (byte count) to a string
 *
 *	This function was a PAIN!  In computer-world 1K is 1024 bytes
 *	and 1M is 1024K -- however, sprintf() still formats in base-10.
 *	Therefore I output only until 999, and then start using the
 *	next unit.  This doesn't work wrong, it's just a feature.
 */
PRIVATE void HTDirSize ARGS3(unsigned long, n, char *, start, char, len)
{
    double size = n/1024.0;
    if (n < 1000) {
	if (HTDirShowBytes)
	    ItoA((int) n, start, len);
	else
	    sprintf(start+len-6, "%5dK", n>0 ? 1 : 0);
    }
    else if (size + 0.999 < 1000)
	sprintf(start+len-6, "%5dK", (int)(size + 0.5));
    else if ((size /= 1024) < 9.9)
	sprintf(start+len-6, "%5.1fM", (size + 0.05));
    else if (size < 1000)
	sprintf(start+len-6, "%5dM", (int)(size + 0.5));
    else if ((size /= 1024) < 9.9)
	sprintf(start+len-6, "%5.1fG", (size + 0.05));
    else
	sprintf(start+len-6, "%5dG", (int)(size + 0.5));
    *(start+len) = ' ';
}


/* 							     	KeyFree()
**	Frees the contents of the key structure AND the key it self
*/
PRIVATE void  KeyFree ARGS1(HTDirKey *, key)
{
    if (key->key != key->filename)
	FREE(key->key);
    FREE(key->filename);
    FREE(key->body);
    FREE(key->symlink);
    FREE(key);
}


/* 							     DirFileInfoClear()
**	Frees the contents of the dir_file_info structure and puts ints to 0
*/
PRIVATE void  DirFileInfoClear ARGS1(dir_file_info *, file_info)
{
    FREE(file_info->f_name);
    file_info->f_mode = 0L;
    file_info->f_nlink = 0;
    FREE(file_info->f_uid);
    FREE(file_info->f_gid);
    file_info->f_size = 0L;
    file_info->f_mtime = (time_t) 0;
}


/* 							     	DirAbort()
**	Goes through the whole tree and frees whatever is left.
*/
PRIVATE void  DirAbort ARGS1(HTBTree *, tree)
{
    HTBTElement *next_node = HTBTree_next(tree, NULL);

    while (next_node) {
	KeyFree((HTDirKey *) next_node->object);
	next_node = HTBTree_next(tree, next_node);
    }
}


/* 								HTDirLintCmp()
**	Acts like  strcmp but operates on positive long integers.
**   	Though, in order to get the biggest element first, the
**    	return values are reversed, so it returns >0 if a<b, 0 if a==b
**    	and <0 if a>b 
*/
PRIVATE long HTDirLintCmp ARGS2(unsigned long **, a, unsigned long **, b)
{
    return (long) (**b)-(**a);
}


/* 								HTDirStrCmp()
**	Acts like strcmp but operates on pointers to pointers.
*/
PRIVATE long HTDirStrCmp ARGS2(char **, a, char **, b)
{
    return (int) strcmp(*a, *b);
}


/* 							    HTDirStrCaseCmp()
**	Acts like ctrcasecomp but operates on pointers to pointers.
*/
PRIVATE long HTDirStrCaseCmp ARGS2(char **, a, char **, b)
{
    return (int) strcasecomp(*a, *b);
}


/* 								InitBody()
**	Finds the lenght of the body part of the key and generates a
**	header line used in HTDirOutTop().
*/
PRIVATE char *InitBody NOARGS
{
    char *topstr = NULL;
    PRIVATE char *header[] = {
	"Size",
	"Last modified",
	"Permission",
	"Link",
	"Owner",
	"Group",
	"Description"
    };

    /* Calculate output line string length */
    HTBodyLength = HT_LENGTH_SPACE;
    StrAllocCat(topstr, HTDirSpace);
    if (HTDirShowMask & HT_DIR_SHOW_DATE) {
	char date[HT_LENGTH_DATE+1];
	LeftStr(date, *(header+1), HT_LENGTH_DATE+1);
	StrAllocCat(topstr, date);
	HTBodyLength += HT_LENGTH_DATE;
	StrAllocCat(topstr, HTDirSpace);
	HTBodyLength += HT_LENGTH_SPACE;
    }
    if (HTDirShowMask & HT_DIR_SHOW_SIZE) {
	char size[HT_LENGTH_SIZE+1];
	RightStr(size, *header, HT_LENGTH_SIZE+1);
	StrAllocCat(topstr, size);
	HTBodyLength += HT_LENGTH_SIZE;
	StrAllocCat(topstr, HTDirSpace);
	HTBodyLength += HT_LENGTH_SPACE;
    }
    if (HTDirShowMask & HT_DIR_SHOW_MODE) {
	char mode[HT_LENGTH_MODE+1];
	LeftStr(mode, *(header+2), HT_LENGTH_MODE+1);
	StrAllocCat(topstr, mode);
	HTBodyLength += HT_LENGTH_MODE;
	StrAllocCat(topstr, HTDirSpace);
	HTBodyLength += HT_LENGTH_SPACE;
    }
    if (HTDirShowMask & HT_DIR_SHOW_NLINK) {
	char nlink[HT_LENGTH_NLINK+1];
	LeftStr(nlink, *(header+3), HT_LENGTH_NLINK+1);
	StrAllocCat(topstr, nlink);
	HTBodyLength += HT_LENGTH_NLINK;
	StrAllocCat(topstr, HTDirSpace);
	HTBodyLength += HT_LENGTH_SPACE;
    }
    if (HTDirShowMask & HT_DIR_SHOW_OWNER) {
	char owner[HT_LENGTH_OWNER+1];
	LeftStr(owner, *(header+4), HT_LENGTH_OWNER+1);
	StrAllocCat(topstr, owner);
	HTBodyLength += HT_LENGTH_OWNER;
	StrAllocCat(topstr, HTDirSpace);
	HTBodyLength += HT_LENGTH_SPACE;
    }
    if (HTDirShowMask & HT_DIR_SHOW_GROUP) {
	char group[HT_LENGTH_GROUP+1];
	LeftStr(group, *(header+5), HT_LENGTH_GROUP+1);
	StrAllocCat(topstr, group);
	HTBodyLength += HT_LENGTH_GROUP;
	StrAllocCat(topstr, HTDirSpace);
	HTBodyLength += HT_LENGTH_SPACE;
    }
    if (HTDirDescriptions) {
	char * descr = (char*)malloc(HTDirMaxDescrLength+1);
	if (!descr) outofmem(__FILE__, "InitBody");

	if (HT_LENGTH_SPACE > 1) {
	    LeftStr(descr, *(header+6), HTDirMaxDescrLength+1);
	}
	else {	/* Description simply always needs at least two spaces */
	    *descr = ' ';
	    LeftStr(descr+1, *(header+6), HTDirMaxDescrLength);
	}
	StrAllocCat(topstr, descr);
	HTBodyLength += HTDirMaxDescrLength;
	StrAllocCat(topstr, HTDirSpace);
	HTBodyLength += HT_LENGTH_SPACE;
	free(descr);
    }
    return topstr;
}


/*      							HTDirOutMessage
**
**    Puts out the message to default output.
*/
PRIVATE void HTDirOutMessage ARGS2(HTStructured *, target,
				   HTChunk *, message)
{
    if (!message || !message->data)
	return;
    START(HTML_PRE);
    PUTS(message->data);
    END(HTML_PRE);
    return;
}


/*	Send README file			       		HTDirOutReadme
**
**  If a README file exists, then it is inserted into the document here.
*/
PRIVATE void HTDirOutReadme ARGS2(HTStructured *, target,
				  CONST char *, localname)
{ 
    FILE *fp;
    int ch;
    char *readme_file_name = (char *) 
	malloc(strlen(localname)+1+strlen(HT_DIR_README_FILE)+1);

    if (TRACE) fprintf(TDEST, "HTReadMe.... Looking for file `%s'\n",
		       localname ? localname : "-null-");
    if (!target || !localname) {
	free(readme_file_name);
	return;
    }
    strcpy(readme_file_name, localname);
    strcat(readme_file_name, "/");
    strcat(readme_file_name, HT_DIR_README_FILE);
    
    if ((fp = fopen(readme_file_name, "r")) != NULL) {
	START(HTML_PRE);
	while ((ch = fgetc(fp)) >= 0)
	    PUTC(ch);
	END(HTML_PRE);
	fclose(fp);
    } 
    free(readme_file_name);	/* Leak fixed AL 6 Feb 1994 */
}

 
/*      							HTDirOutTop()
**
**    This gives the TITLE and H1 header, and also a link
**    to the parent directory if appropriate.
*/
PRIVATE void HTDirOutTop ARGS5(HTStructured *, target,
			       HTAnchor *, anchor,
			       char *, top_body,
			       char *, directory,
			       HTChunk *, message)
{
    char *logical = HTAnchor_address(anchor);
    char *path = HTParse(logical, "", PARSE_PATH + PARSE_PUNCTUATION);
    char *title = NULL;
    char *current = strrchr(path, '/');
    char *parent = NULL;

    /*
     * Make title
     */
    if (path) {
	StrAllocCopy(title,path);
	HTUnEscape(title);
    }
    else {
	StrAllocCopy(title,"Welcome Directory");
    }

    /*
     * Find parent directory if any
     */
    if (current) {
	if (current != path) {	/* Not root, make link to parent */
	    *current++ = 0;
	    if ((parent = strrchr(path,'/')))
		parent++;
	    else
		parent = path;
	    HTUnEscape(++parent);
	    if ((int)strlen(parent) > HTDirFileLength)
		*(parent+HTDirFileLength-1) = '\0';
	}
	else if (*(current+1)) {  /* In a subdir under root, link to root */
	    current++;
	    parent = "/";
	}
    }

    /* Output title */
    START(HTML_HTML);
    START(HTML_HEAD);
    START(HTML_TITLE);
    PUTS("Index of ");
    PUTS(title);
    END(HTML_TITLE);
    END(HTML_HEAD);

    START(HTML_BODY);
    START(HTML_H1);
    PUTS("Index of ");
    PUTS(title);
    END(HTML_H1);

    if (message && HTDirInfo == HT_DIR_INFO_TOP)
	HTDirOutMessage(target, message);
    else if (HTDirReadme == HT_DIR_README_TOP)
	HTDirOutReadme(target, directory);

    /* Output parent directory */
    START(HTML_PRE);

    /* Output the header line of the list */
    if (!icon_blank) icon_blank = icon_unknown;
    if (HTDirShowMask & HT_DIR_SHOW_ICON && icon_blank) {
	HTMLPutImg(target,
		   icon_blank->icon_url,
		   HTIcon_alt_string(icon_blank->icon_alt, NO),
		   NULL);
	PUTS(HTDirSpace);
    }
    {
	char *name;
	if ((name = (char *) malloc(HTDirFileLength+1)) == NULL)
	    outofmem(__FILE__, "HTDirOutTop");
	LeftStr(name, "Name", HTDirFileLength+1);
	PUTS(name);
	free(name);
    }
    PUTS(top_body);
    PUTC('\n');
    START(HTML_HR);
    PUTC('\n');

    if (parent) {
	if (!icon_parent)
	    icon_parent = icon_dir ? icon_dir :
		icon_blank ? icon_blank : icon_unknown;
	if (HTDirShowMask & HT_DIR_SHOW_ICON  &&  icon_parent) {
	    HTMLPutImg(target,
		       icon_parent->icon_url,
		       HTIcon_alt_string(icon_parent->icon_alt, YES),
		       NULL);
	    PUTS(HTDirSpace);
	}
	{
	    char *relative;
	    if ((relative = (char *) malloc(strlen(current) + 4)) == NULL)
		outofmem(__FILE__, "HTDirOutTop");
	    if (*current)
		sprintf(relative, "%s/..", current);
	    else
		strcpy(relative, "..");
	    HTStartAnchor(target, NULL, relative);
	    free(relative);
	    PUTS("Parent directory");
	    END(HTML_A);
	    PUTC('\n');
	}
    }
    free(logical);
    free(path);
    free(title);
}


/*      							HTDirOutList()
**
**    	This function goes through the BTRee and puts each directory entry out
**    	on line.
**
**	BUGS:
**
**	If filename is anchor and filename is longer than HTMaxFileLength,
**	the body columns are shifted to the right
*/
PRIVATE void HTDirOutList ARGS4(HTStructured *, target, HTBTree *, bt,
				char *, pathtail, char *, directory)
{
    char *escaped = NULL;	      		 	  /* Used for anchor */
    char *tail = NULL;
    int tailend;
    int filelen;
    HTBTElement *next_node = HTBTree_next(bt, NULL);
    HTDirKey *nkey;
    StrAllocCopy(tail, pathtail);
    tailend = strlen(tail);

    do {
	nkey = (HTDirKey *) next_node->object;
	escaped = HTEscape(nkey->filename, URL_XPALPHAS);
	*(tail+tailend) = '\0';
	StrAllocCat(tail, escaped);

	if (HTDirShowMask & HT_DIR_ICON_ANCHOR  &&
	    nkey->icon && nkey->icon->icon_url) {	/* Icon as anchor */
	    HTStartAnchor(target, NULL, tail);
	    HTMLPutImg(target,
		       nkey->icon->icon_url,
		       HTIcon_alt_string(nkey->icon->icon_alt, YES),
		       NULL);
	    END(HTML_A);
	    PUTS(HTDirSpace);
	    if (HTBodyLength)
		PUTS(nkey->body);
	    filelen = strlen(nkey->filename);
	    if (filelen > HTDirFileLength) {
		*(nkey->filename+HTDirFileLength-2) = '>';
		*(nkey->filename+HTDirFileLength-1) = '\0';
	    }
	    if (HTDirShowMask & HT_DIR_SHOW_SLINK && nkey->symlink) {
		START(HTML_I);
		PUTS(nkey->filename);
		END(HTML_I);
	    } else {
		PUTS(nkey->filename);
	    } 
	} else { 	      			   /* Use filename as anchor */
	    if (HTDirShowMask & HT_DIR_SHOW_ICON  &&
		nkey->icon && nkey->icon->icon_url) {
 		if (nkey->href) {
		    char url[500];
 
		    strcpy(url, nkey->href->href_url);
		    strcat(url, directory);
		    strcat(url,"/");
		    strcat(url, nkey->filename); 
		    if (TRACE) fprintf(TDEST,"Href: %s\n", url);
		    HTStartAnchor(target,NULL,url);
 		}
		HTMLPutImg(target,
			   nkey->icon->icon_url,
			   HTIcon_alt_string(nkey->icon->icon_alt, YES),
			   NULL);
		PUTS(HTDirSpace);
		if (nkey->href)
		    END(HTML_A);
	    }
	    if (HTDirShowMask & HT_DIR_SHOW_SLINK && nkey->symlink)
		START(HTML_I);
	    HTStartAnchor(target, NULL, tail);
	    {
		int extra = nkey->is_dir ? 1 : 0;
		if ((int)strlen(nkey->filename) + extra > HTDirFileLength) {
		    char * fn = (char*)malloc(HTDirFileLength + 1);
		    if (!fn) outofmem(__FILE__, "HTDirOutList");

		    strncpy(fn, nkey->filename, HTDirFileLength);
		    if (extra) {
			fn[HTDirFileLength-2] = '>';
			fn[HTDirFileLength-1] = '/';
		    }
		    else {
			fn[HTDirFileLength-1] = '>';
		    }
		    fn[HTDirFileLength] = 0;
		    PUTS(fn);
		    free(fn);
		}
		else {
		    PUTS(nkey->filename);
		    if (extra)
			PUTC('/');
		}
	    }
	    END(HTML_A);
	    if (HTDirShowMask & HT_DIR_SHOW_SLINK && nkey->symlink)
		END(HTML_I);
	}
	if (HTBodyLength) {
	    filelen = strlen(nkey->filename) + (nkey->is_dir ? 1 : 0);
	    while (filelen++ < HTDirFileLength)
		PUTC(' ');
	    PUTS(HTDirSpace);
	    PUTS(nkey->body);
	}
	PUTC('\n');
	KeyFree(nkey);
	FREE(escaped);
    } while ((next_node = HTBTree_next(bt, next_node)) != NULL);
    free(tail);
}



/*      							HTDirOutBottom
**
**    This function outputs the last part of the directory listings
*/
PRIVATE void HTDirOutBottom ARGS4(HTStructured *, target,
				  unsigned int, files,
				  char *, directory,
				  HTChunk *, message)
{
    char *outstr;

    if ((outstr = (char *) malloc(100)) == NULL)
	outofmem(__FILE__, "HTDirOutBottom");
    if (files == 0)
	sprintf(outstr, "Empty directory");
    else if (files == 1)
	sprintf(outstr, "1 file");
    else
	sprintf(outstr, "%u files", files);
    START(HTML_HR);
    PUTS(outstr);
    free(outstr);
    END(HTML_PRE);

    if (message && HTDirInfo == HT_DIR_INFO_BOTTOM)
	HTDirOutMessage(target, message);
    else if (HTDirReadme == HT_DIR_README_BOTTOM)
	HTDirOutReadme(target, directory);
    END(HTML_BODY);
    END(HTML_HTML);
}


#ifdef GOT_READ_DIR
/*						    	HTBrowseDirectory()
**	This function scrolls through the directory file given and
**	generates an HTML-object. It uses the global variables:
**
**	  - HTDirShowMask	see enum HTDirShow for details
**	  - HTDirAccess		HT_DIR_[FORBID | SELECTIVE | OK]
**	  - HTDirReadme
**	
**	Generated URLs for non symbolic links are made relative to directory:
**
**		.../xxx/yyy	=>	yyy/<element>
**		.../xxx/yyy/	=>	./<element>
**		/		=>	/<element>
**     
**	Symbolic links are given as are (relative or absolute), but see note.
**
**	NOTE:	The function expects that the URL given as directory IS NOT
**		escaped BUT Simplified.
**
**	Returns < 0 on error, HT_LOADED on succes
*/
PUBLIC int HTBrowseDirectory ARGS2(HTRequest *, req, char *, directory)
{
    char *pathname = NULL;
    int  pathend;
    char *tail = NULL;
    DIR *dp;
    struct stat file_info;
    HTList * descriptions = NULL;

    if (TRACE)
	fprintf(TDEST,"HTBrowse.... Browsing `%s\'\n", directory);
        
    if (HTDirAccess == HT_DIR_FORBID)
	return HTErrorAdd(req, ERR_FATAL, NO, HTERR_FORBIDDEN,
			  NULL, 0, "HTBrowseDirectory");
    
    /* Initialize path name for stat() */
    StrAllocCopy(pathname, directory);
    pathend = strlen(pathname);
    if (*(pathname+pathend-1) != '/') {
	StrAllocCat(pathname, "/");
	pathend++;
    }
    
    /* Set up the offset string of the anchor reference */
    {
	char *tptr = strrchr(directory, '/');
	char *tailstr = NULL;
	if (!tptr) {					    /* '/' not found */
	    StrAllocCopy(tailstr, directory);
	    StrAllocCat(tailstr, "/");
	} else if (!*(tptr+1)) {
	    if (tptr != directory) {			     /* Trailing '/' */
		StrAllocCopy(tailstr, "./");
	    } else {						     /* Root */
		StrAllocCopy(tailstr, "/");
	    }
	} else {			     /* Relative to parent directory */
	    StrAllocCopy(tailstr, ++tptr);
	    StrAllocCat(tailstr, "/");
	}
	tail = HTEscape(tailstr, URL_PATH);
	free(tailstr);
    }

    if (HTDirAccess == HT_DIR_SELECTIVE) {
	StrAllocCat(pathname, HT_DIR_ENABLE_FILE);
	if (HT_STAT(pathname, &file_info)) {
	    if (TRACE) fprintf(TDEST,
	        "HTBrowse.... Can't stat() file: %s (errno: %d)\n",
			       pathname, errno);
	    free(pathname);
	    FREE(tail);
	    return HTErrorAdd(req, ERR_FATAL, NO, HTERR_FORBIDDEN,
			      NULL, 0, "HTBrowseDirectory");
	}
    }

    if ((dp = opendir(directory)) == NULL)
	return HTErrorSysAdd(req,  ERR_FATAL, errno, NO, "opendir");

    if (HTDirDescriptions)
	descriptions = HTReadDescriptions(directory);
    
    /* Now, generate the Btree and put it out to the output stream. */
    {
	unsigned int filecnt = 0;
	char dottest = 2;		  /* To avoid two strcmp() each time */
	char *topstr;
	void *keyptr = NULL;		   /* Points to the key in file_info */
	STRUCT_DIRENT *dirbuf;
	struct passwd *pw_info;
	struct group *gr_info;
	HTBTree *bt;
#ifdef HT_REENTRANT
	STRUCT_DIRENT result;				    /* For readdir_r */
#endif

	/* Set up sort key and initialize BTree */
	if (HTDirShowMask & HT_DIR_KEY_SIZE) {
	    keyptr = &file_info.st_size;
	    bt = HTBTree_new((HTComparer) HTDirLintCmp);   
	} else if (HTDirShowMask & HT_DIR_KEY_DATE) {
	    keyptr = &file_info.st_mtime;
	    bt = HTBTree_new((HTComparer) HTDirLintCmp); 
	} else if (HTDirShowMask & HT_DIR_SHOW_CASE) {
	    bt = HTBTree_new((HTComparer) HTDirStrCmp);
	} else {
	    bt = HTBTree_new((HTComparer) HTDirStrCaseCmp);
	}
	if ((HTDirSpace = (char *) malloc(HT_LENGTH_SPACE+1)) == NULL)
	    outofmem(__FILE__, "HTBrowseDirectory");
	memset((void *) HTDirSpace, ' ', HT_LENGTH_SPACE);
	*(HTDirSpace+HT_LENGTH_SPACE) = '\0';
	topstr = InitBody();
	HTDirFileLength = 0;

	/* Build tree */
#ifdef HT_REENTRANT
	while ((dirbuf = (STRUCT_DIRENT *) readdir_r(dp, &result))) {
#else
	while ((dirbuf = readdir(dp))) {
#endif /* HT_REENTRANT */
	    HTDirKey *nodekey;
	    HTFormat format=NULL;
	    HTEncoding encoding=NULL;
	    HTLanguage language=NULL;
	    double q=1.0;

	    if (!dirbuf->d_ino)		 		 /* Skip if not used */
		continue;
	    
	    /* Current and parent directories are never shown in list */
	    if (dottest && (!strcmp(dirbuf->d_name, ".") ||
			    !strcmp(dirbuf->d_name, ".."))) {
		dottest--;
		continue;
	    }
	    if (!(HTDirShowMask & HT_DIR_SHOW_HID) && *dirbuf->d_name == '.')
		continue;

	    HTBind_getFormat(dirbuf->d_name, &format, &encoding, &language,&q);

	    /* First make a lstat() and get a key ready. */
	    *(pathname+pathend) = '\0';
	    StrAllocCat(pathname, dirbuf->d_name);
	    if (HT_LSTAT(pathname, &file_info)) {
#ifndef VMS
		if (TRACE) fprintf(TDEST,
		"HTBrowse.... OUPS, lstat failed on %s (errno: %d)\n",
				   pathname, errno);
		DirAbort(bt);
		goto cleanup;
#else /* VMS */
		/* for VMS the failure here means the file is not readable...
		   we however continue to browse through the directory... */
                continue;
#endif /* VMS */
	    }
	    if ((nodekey = (HTDirKey *) calloc(1, sizeof(HTDirKey))) == NULL)
		outofmem(__FILE__, "HTFileBrowseDirectory");

#ifndef VMS
	    /* Check if symbolic link, if so do a stat(). If this fails, don't
	       show the item in the list */
	    if ((file_info.st_mode & S_IFMT) == S_IFLNK) {
		int symend;		
		if (HT_STAT(pathname, &file_info)) {
		    if (TRACE)
			fprintf(TDEST, "HTBrowse.... stat failed on symbolic link %s, errno: %d\n", pathname, errno);
		    KeyFree(nodekey);
		    continue;
		}
		if ((nodekey->symlink = 
		     (char *) malloc(HT_MAX_PATH+2)) == NULL)
		    outofmem(__FILE__, "HTFileBrowseDirectory");
		symend = readlink(pathname, nodekey->symlink, HT_MAX_PATH);
		if (symend < 0) {
		    if (TRACE)
			fprintf(TDEST,
			        "HTBrowse.... readlink errno: %d (%s)\n",
				errno, pathname);
		    FREE(nodekey->symlink);
		} else {
		    *(nodekey->symlink+symend) = '\0';
		}
	    }
#endif /* not VMS */

	    /* Generate key entry in nodekey */
	    if (keyptr) {	             /* Use content of keyptr as key */
		if ((nodekey->key = (void *) malloc(sizeof keyptr)) == NULL ||
		    (nodekey->filename =
		     (char *) malloc(strlen(dirbuf->d_name)+1)) == NULL)
		    outofmem(__FILE__, "HTFileBrowseDirectory");
		memcpy(nodekey->key, keyptr, sizeof keyptr);
		strcpy(nodekey->filename, dirbuf->d_name);
	    } else {				/* Use dirbuf->d_name as key */
		if ((nodekey->key =
		     (void *) malloc(strlen(dirbuf->d_name)+1)) == NULL)
		    outofmem(__FILE__, "HTFileBrowseDirectory");
		strcpy((char *) nodekey->key, dirbuf->d_name);
		nodekey->filename = (char *) nodekey->key;
	    }


	    /* Update current max filename length */
	    {
		int filestrlen = strlen(nodekey->filename);

		if ((file_info.st_mode & S_IFMT) == S_IFDIR) {
#ifdef VMS  
		    /* strip .DIR part... */
                    char *dot;
                    dot = strstr(nodekey->filename,".DIR");
                    if (dot)
                    {
                       *dot = '\0';
                       filestrlen -= 4;
                    }
#endif /* VMS */
		    nodekey->is_dir = YES;	/* We need the trailing slash*/
		    filestrlen++;
		}
		if (filestrlen > HTDirFileLength)
		    HTDirFileLength = filestrlen;
	    }

	    /* Get Icon type */
	    if (HTDirShowMask & HT_DIR_SHOW_ICON) {
		nodekey->icon = HTGetIcon(file_info.st_mode, format, encoding);
		nodekey->href = HTGetHref(nodekey->filename);
	    }

	    /* Generate body entry in nodekey */
	    if (HTBodyLength) {
		char *bodyptr;
		if  ((nodekey->body = (char *) malloc(HTBodyLength+1)) == NULL)
		    outofmem(__FILE__, "HTBrowseDirectory");
		bodyptr = nodekey->body;
		memset((void *) bodyptr, ' ', HTBodyLength);
		if (HTDirShowMask & HT_DIR_SHOW_DATE) {
#ifndef NO_STRFTIME
#if defined(HT_REENTRANT) || defined(SOLARIS)
		    struct tm loctime;
		    localtime_r(&file_info.st_mtime, &loctime);
		    strftime(bodyptr, HT_LENGTH_DATE+1, "%d-%b-%y %H:%M",
			     &loctime);
#else
		    strftime(bodyptr, HT_LENGTH_DATE+1, "%d-%b-%y %H:%M",
			     localtime(&file_info.st_mtime));
#endif  /* HT_REENTRANT || SOLARIS */
#else
#if defined(HT_REENTRANT) || defined(SOLARIS)
		    struct tm loctime;
		    localtime_r(&file_info.st_mtime, &loctime);
		    sprintf(bodyptr,"%02d-%s-%02d %02d:%02d",
			    loctime.tm_mday,
			    months[loctime.tm_mon],
			    loctime.tm_year % 100,
			    loctime.tm_hour,
			    loctime.tm_min);
#else
		    struct tm *loctime = localtime(&file_info.st_mtime);
		    sprintf(bodyptr,"%02d-%s-%02d %02d:%02d",
			    loctime->tm_mday,
			    months[loctime->tm_mon],
			    loctime->tm_year % 100,
			    loctime->tm_hour,
			    loctime->tm_min);
#endif  /* HT_REENTRANT || SOLARIS */
#endif /* NO_STRFTIME */
		    bodyptr += HT_LENGTH_DATE;
		    *bodyptr = ' ';
		    bodyptr += HT_LENGTH_SPACE;
		}
		if (HTDirShowMask & HT_DIR_SHOW_SIZE) {
		    if ((file_info.st_mode & S_IFMT) != S_IFDIR)
			HTDirSize(file_info.st_size, bodyptr, HT_LENGTH_SIZE);
		    else
			bodyptr[HT_LENGTH_SIZE-1] = '-';
		    bodyptr += HT_LENGTH_SIZE+HT_LENGTH_SPACE;
		}
		if (HTDirShowMask & HT_DIR_SHOW_MODE) {
		    FilePerm(file_info.st_mode, bodyptr);
		    bodyptr += HT_LENGTH_MODE+HT_LENGTH_SPACE;
		}
		if (HTDirShowMask & HT_DIR_SHOW_NLINK) {
		    ItoA(file_info.st_nlink, bodyptr, HT_LENGTH_NLINK);
		    bodyptr += HT_LENGTH_NLINK+HT_LENGTH_SPACE;
		}
#ifdef GOT_OWNER
		if (HTDirShowMask & HT_DIR_SHOW_OWNER) {
		    char *bp = bodyptr;
		    char *pwptr;
		    if ((pw_info = getpwuid(file_info.st_uid)) == NULL) {
			if (TRACE) fprintf(TDEST,
				      "HTBrowse.... getpwuid() failed on %s\n",
					   pathname);
			ItoA(file_info.st_uid, bodyptr, HT_LENGTH_OWNER);
		    } else {
			pwptr = pw_info->pw_name;
			while ((*bp++ = *pwptr++) != '\0');
			*--bp = ' ';
		    }
		    bodyptr += HT_LENGTH_OWNER+HT_LENGTH_SPACE;
		}
		if (HTDirShowMask & HT_DIR_SHOW_GROUP) {
		    char *bp = bodyptr;
		    char *grptr;
		    if ((gr_info = getgrgid(file_info.st_gid)) == NULL) {
			if (TRACE) fprintf(TDEST,
			    "HTBrowse.... getgrgid() failed on %s\n",
					   pathname);
			ItoA(file_info.st_gid, bodyptr, HT_LENGTH_GROUP);
		    } else {
			grptr = gr_info->gr_name;
			while ((*bp++ = *grptr++) != '\0');
			*--bp = ' ';
		    }
		    bodyptr += HT_LENGTH_GROUP+HT_LENGTH_SPACE;
		}
#endif /* GOT_OWNER */
		*bodyptr = '\0';
		if (HTDirDescriptions) {
		    char * d = HTGetDescription(descriptions,
						directory,
						dirbuf->d_name,
						format);
		    if (d) {
			int len = strlen(d);
			if (HT_LENGTH_SPACE > 1) {
			    if (len > HTDirMaxDescrLength)
				len = HTDirMaxDescrLength;
			}
			else {
			    /* Description always needs at least two spaces */
			    if (len > HTDirMaxDescrLength-1)
				len = HTDirMaxDescrLength-1;
			    *bodyptr++ = ' ';
			}
			strncpy(bodyptr, d, len);
			bodyptr[len] = '\0';
		    }
		}
	    }
	    /* Now, update the BTree etc. */
	    filecnt++;
	    HTBTree_add(bt, (void *) nodekey);
	} /* End while readdir() */

	if (HTDirFileLength > HTDirMaxFileLength)
	    HTDirFileLength = HTDirMaxFileLength;
	if (HTDirFileLength < HTDirMinFileLength)
	    HTDirFileLength = HTDirMinFileLength;

	{
	    HTStructured *target = HTMLGenerator(req, NULL, WWW_HTML,
						 req->output_format,
						 req->output_stream);
	    
	    /* Put out the header for the HTML object */
	    HTDirOutTop(target, (HTAnchor *) req->anchor, topstr, directory,
			NULL);
	    
	    /* Run through tree printing out in order, hopefully :-) */
	    if (filecnt) {
		HTDirOutList(target, bt, tail, directory);
	    }
	    
	    HTDirOutBottom(target, filecnt, directory, NULL);
	    FREE_TARGET;
	}

#ifndef VMS /* Not used for VMS */
cleanup:
#endif
	if (descriptions)
	    HTFreeDescriptions(descriptions);
	FREE(HTDirSpace);
	FREE(topstr);
	free(tail);
	free(pathname);
	HTBTree_free(bt);
	closedir(dp);
    } /* End of two big loops */
    return HT_LOADED;
} /* End of directory reading section */

#endif /* GOT_READ_DIR */

/*						    	HTFTPBrowseDirectory()
**	This function scrolls through the directory file given in a FTP session
**	and generates an HTML-object. It uses the global variables:
**
**	  - HTDirShowMask	see enum HTDirShow for details
**	  - HTDirAccess		HT_DIR_[FORBID | SELECTIVE | OK]
**	  - HTDirReadme
**	
**	Generated URLs for non symbolic links are made relative to directory:
**
**		.../xxx/yyy	=>	yyy/<element>
**		.../xxx/yyy/	=>	./<element>
**		/		=>	/<element>
**     
**	Symbolic links are given as are (relative or absolute), but see note.
**
**	NOTE:	The function expects that the URL given as directory IS
**		UNescaped AND Simplified.
**
**	Returns < 0 on error, HT_LOADED on succes
*/
PUBLIC int HTFTPBrowseDirectory ARGS3(HTRequest *, req, char *, directory,
				      HTDirLineInput, input)
{
    int status;
    char *tail = NULL;
    
    if (TRACE) fprintf(TDEST, "HTFTPBrowse. Browsing `%s\'\n", directory);

#if 0        
    /* Set up the offset string of the anchor reference */
    {
	char *tptr = strrchr(directory, '/');
	char *tailstr = NULL;
	if (!tptr) {					    /* '/' not found */
	    StrAllocCopy(tailstr, directory);
	    StrAllocCat(tailstr, "/");
	} else if (!*(tptr+1)) {
	    if (tptr != directory) {			     /* Trailing '/' */
		StrAllocCopy(tailstr, "./");
	    } else {						     /* Root */
		StrAllocCopy(tailstr, "/");
	    }
	} else {			     /* Relative to parent directory */
	    StrAllocCopy(tailstr, ++tptr);
	    StrAllocCat(tailstr, "/");
	}
	tail = HTEscape(tailstr, URL_PATH);
	free(tailstr);
    }
    
    /* Now, generate the Btree and put it out to the output stream. */
    {
	BOOL old_descr = HTDirDescriptions;
	unsigned int filecnt = 0;
	char *topstr = NULL;
	char dottest = 2;		  /* To avoid two strcmp() each time */
	dir_file_info file_info;
	HTBTree *bt;

	/* TEMPORARY */
	HTDirDescriptions = NO;

	/* Set up sort key and initialize BTree */
	if (HTDirShowMask & HT_DIR_KEY_SIZE) {
	    bt = HTBTree_new((HTComparer) HTDirLintCmp);   
	} else if (HTDirShowMask & HT_DIR_KEY_DATE) {
	    bt = HTBTree_new((HTComparer) HTDirLintCmp); 
	} else if (HTDirShowMask & HT_DIR_SHOW_CASE) {
	    bt = HTBTree_new((HTComparer) HTDirStrCmp);
	} else {
	    bt = HTBTree_new((HTComparer) HTDirStrCaseCmp);
	}
	if ((HTDirSpace = (char *) malloc(HT_LENGTH_SPACE+1)) == NULL)
	    outofmem(__FILE__, "HTFTPBrowseDirectory");
	memset((void *) HTDirSpace, ' ', HT_LENGTH_SPACE);
	memset((void *) &file_info, '\0', sizeof(dir_file_info));
	*(HTDirSpace+HT_LENGTH_SPACE) = '\0';
	topstr = InitBody();
	HTDirFileLength = 0;

	/* Build tree */
	while ((status = input(req->net, &file_info)) > 0) {
	    HTDirKey *nodekey;
	    HTFormat format=NULL;
	    HTEncoding encoding=NULL;
	    HTLanguage language=NULL;
	    double q=1.0;
	    
	    /* Current and parent directories are never shown in list */
	    if (dottest && (!strcmp(file_info.f_name, ".") ||
			    !strcmp(file_info.f_name, ".."))) {
		dottest--;
		DirFileInfoClear(&file_info);
		continue;
	    }

	    HTBind_getFormat(file_info.f_name,&format,&encoding,&language,&q);

	    /* Get a key ready. */
	    if ((nodekey = (HTDirKey *) calloc(1, sizeof(HTDirKey))) == NULL)
		outofmem(__FILE__, "HTFTPBrowseDirectory");

	    /* Generate key entry in nodekey */
	    if ((nodekey->key =
		 (void *) malloc(strlen(file_info.f_name)+1)) == NULL)
		outofmem(__FILE__, "HTFTPBrowseDirectory");
	    strcpy((char *) nodekey->key, file_info.f_name);
	    nodekey->filename = (char *) nodekey->key;

	    /* Update current max filename length */
            {
                int filestrlen = strlen(nodekey->filename);

                if ((file_info.f_mode & S_IFMT) == S_IFDIR) {
                    nodekey->is_dir = YES;      /* We need the trailing slash*/
                    filestrlen++;
                }
                if (filestrlen > HTDirFileLength)
                    HTDirFileLength = filestrlen;
            }

	    /* Get Icon type */
	    if (HTDirShowMask & HT_DIR_SHOW_ICON) {
		nodekey->icon = HTGetIcon(file_info.f_mode, format, encoding);
		nodekey->href = HTGetHref(nodekey->filename);
	    }

	    /* Generate body entry in nodekey */
	    if (HTBodyLength) {
		char *bodyptr;
		if  ((nodekey->body = (char *) malloc(HTBodyLength+1)) == NULL)
		    outofmem(__FILE__, "HTBrowseDirectory");
		bodyptr = nodekey->body;
		memset((void *) bodyptr, ' ', HTBodyLength);
		if (HTDirShowMask & HT_DIR_SHOW_DATE) {
		    if (file_info.f_mtime) {
#ifndef NO_STRFTIME
#if defined(HT_REENTRANT) || defined(SOLARIS)
			struct tm loctime;
			localtime_r(&file_info.f_mtime, &loctime);
			strftime(bodyptr, HT_LENGTH_DATE+1, "%d-%b-%y %H:%M",
				 &loctime);
#else
		        strftime(bodyptr, HT_LENGTH_DATE+1, "%d-%b-%y %H:%M",
				 localtime(&file_info.f_mtime));
#endif  /* HT_REENTRANT || SOLARIS */
#else
#if defined(HT_REENTRANT) || defined(SOLARIS)
		        struct tm loctime;
		        localtime_r(&file_info.f_mtime, &loctime);
		        sprintf(bodyptr,"%02d-%s-%02d %02d:%02d",
				loctime.tm_mday,
				months[loctime.tm_mon],
				loctime.tm_year % 100,
				loctime.tm_hour,
				loctime.tm_min);
#else
		        struct tm *loctime = localtime(&file_info.f_mtime);
		        sprintf(bodyptr,"%02d-%s-%02d %02d:%02d",
				loctime->tm_mday,
				months[loctime->tm_mon],
				loctime->tm_year % 100,
				loctime->tm_hour,
				loctime->tm_min);
#endif  /* HT_REENTRANT || SOLARIS */
#endif /* NO_STRFTIME */
		    } else {
			*bodyptr = '-';
		    }
		    bodyptr += HT_LENGTH_DATE;
		    *bodyptr = ' ';
		    bodyptr += HT_LENGTH_SPACE;
	        }

		/* If we are using NLST, then don't show any size */
		if (HTDirShowMask & HT_DIR_SHOW_SIZE) {
                    if ((file_info.f_mode & S_IFMT) != S_IFDIR &&
			HTFTUseList(req->net) == YES)
                        HTDirSize(file_info.f_size, bodyptr, HT_LENGTH_SIZE);
                    else
                        bodyptr[HT_LENGTH_SIZE-1] = '-';
                    bodyptr += HT_LENGTH_SIZE+HT_LENGTH_SPACE;
		}
		if (HTDirShowMask & HT_DIR_SHOW_MODE) {
                    FilePerm(file_info.f_mode, bodyptr);
		    bodyptr += HT_LENGTH_MODE+HT_LENGTH_SPACE;
		}
		if (HTDirShowMask & HT_DIR_SHOW_NLINK) {
		    ItoA(file_info.f_nlink, bodyptr, HT_LENGTH_NLINK);
		    bodyptr += HT_LENGTH_NLINK+HT_LENGTH_SPACE;
		}
		if (HTDirShowMask & HT_DIR_SHOW_OWNER) {
		    char *bp = bodyptr;
		    char *owner = file_info.f_uid;
		    if (owner) {
			while ((*bp++ = *owner++) != '\0');
			*--bp = ' ';
		    } else {
			*bp++ = '-';
		    }
		    bodyptr += HT_LENGTH_OWNER+HT_LENGTH_SPACE;
		}
		if (HTDirShowMask & HT_DIR_SHOW_GROUP) {
		    char *bp = bodyptr;
		    char *group = file_info.f_gid;
		    if (group) {
			while ((*bp++ = *group++) != '\0');
			*--bp = ' ';
		    } else {
			*bp = '-';
		    }
		    bodyptr += HT_LENGTH_GROUP+HT_LENGTH_SPACE;
		}
		*bodyptr = '\0';
	    }	    

	    /* Now, update the BTree etc. */
	    filecnt++;
	    DirFileInfoClear(&file_info);
	    HTBTree_add(bt, (void *) nodekey);
	} /* End while input() */

	if (status < 0) {
	    DirAbort(bt);
	    goto cleanup;
	}

	if (HTDirFileLength > HTDirMaxFileLength)
            HTDirFileLength = HTDirMaxFileLength;
        if (HTDirFileLength < HTDirMinFileLength)
            HTDirFileLength = HTDirMinFileLength;

#if 0
	/* Now, put up the stream if no error has occured */
	if (!req->error_stack)
#endif
	{
	    HTStructured *target = HTMLGenerator(req, NULL, WWW_HTML,
						 req->output_format,
						 req->output_stream);

	    /* Put out the header for the HTML object */
	    HTDirOutTop(target, (HTAnchor *) req->anchor, topstr, directory,
			HTFTPWelcomeMsg(req->net));
	    
	    /* Run through tree printing out in order, hopefully :-) */
	    if (filecnt) {
		HTDirOutList(target, bt, tail, directory);
	    }
	    HTDirOutBottom(target, filecnt, directory,
			   HTFTPWelcomeMsg(req->net));
	    FREE_TARGET;
	}

cleanup:
	FREE(HTDirSpace);
	FREE(topstr);
	free(tail);
	HTBTree_free(bt);

	/* TEMPORARY */
	HTDirDescriptions = old_descr;
    } /* End of two big loops */

#endif

    return status ? status : HT_LOADED;
} /* End of FTP directory listing */
