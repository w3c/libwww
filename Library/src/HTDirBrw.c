/*			Directory Browsing	       		HTDirBrw.c
**			==================
**
**	This is unix-specific code in general
**	The module is intended for use in HTFile.c and HTFTP.c where
**	it replaces the old directory browsing routine.
**	The module is only compiled if GOT_READ_DIR is defined
**
** History:
**	   Mar 94	Written by Henrik Frystyk, frystyk@dxcern.cern.ch,
**			but with some of the Directory stuff brought from
**			HTFile().
**
** BUGS:
**	- No VMS port yet
**
*/

/* Implementation dependent include files */
#include <pwd.h>
#include <grp.h>
#ifdef VMS
#include "HTVMSUtils.h"
#endif /* VMS */
#include "tcp.h"

/* Library include files */
#include "HTMLPDTD.h"
#include "HTUtils.h"
#include "HTFile.h"
#include "HTAnchor.h"
#include "HTParse.h"
#include "HTFWriter.h"
#include "HTInit.h"
#include "HTBTree.h"
#include "HTFormat.h"
#include "HTML.h"
#include "HTChunk.h"
#include "HTDirBrw.h"					 /* Implemented here */

/* Macros and other defines */
#ifdef USE_DIRENT			       /* Set this for Sys V systems */
#define STRUCT_DIRENT struct dirent
#else
#define STRUCT_DIRENT struct direct
#endif /* USE_DIRENT */

#define PUTC(c) (*target->isa->put_character)(target, c)
#define PUTS(s) (*target->isa->put_string)(target, s)
#define START(e) (*target->isa->start_element)(target, e, 0, 0)
#define END(e) (*target->isa->end_element)(target, e)
#define FREE_TARGET (*target->isa->free)(target)

struct _HTStructured {
	CONST HTStructuredClass *	isa;
	/* ... */
};

/* Globals */
PUBLIC int HTDirReadme = HT_DIR_README_TOP;
PUBLIC int HTDirAccess = HT_DIR_OK;
PUBLIC unsigned int HTDirShowMask = HT_DIR_SHOW_ICON+HT_DIR_SHOW_SIZE+HT_DIR_KEY_NAME+HT_DIR_SHOW_DATE;

/* Type definitions and global variables etc. local to this module */
typedef struct _HTDirKey {
    void *key;				     /* Can sort strings or integers */
    char *filename;
    char *body;			/* Contains all the stuff as date, size etc. */
    int  icon_number;			      /* What icon matches this file */
    char *symlink;
} HTDirKey;

typedef enum _HTDirIconEnum {
    HT_ICON_UNKNOWN = 0,
    HT_ICON_BINARY,
    HT_ICON_BINHEX,
    HT_ICON_COMPRESS,
    HT_ICON_DIR,
    HT_ICON_NLINK,
    HT_ICON_FTP,
    HT_ICON_IMAGE,
    HT_ICON_INDEX,
    HT_ICON_MOVIE,
    HT_ICON_TAR,
    HT_ICON_TEXT,
    HT_ICON_UU,
    HT_ICON_AUDIO,
    HT_ICON_PARENT,
    HT_ICON_BLANK,
    HT_MAX_ICON				     /* This item MUST be the last!! */
} HTDirIconEnum;

PRIVATE char *HTDirIcons[HT_MAX_ICON][2] = {
    {"/dir-icons/unknown.xbm", 		"[FILE ]"},	/* HT_ICON_UNKNOWN */
    {"/dir-icons/binary.xbm", 		"[BIN  ]"},	/* HT_ICON_BINARY */
    {"/dir-icons/binhex.xbm", 		"[HEX  ]"},	/* HT_ICON_BINHEX */
    {"/dir-icons/compressed.xbm", 	"[COMP ]"},	/* HT_ICON_COMPRESS */
    {"/dir-icons/directory.xbm", 	"[DIR  ]"},	/* HT_ICON_DIR */
    {"/dir-icons/index.xbm", 		"[INDEX]"},	/* HT_ICON_NLINK */
    {"/dir-icons/ftp.xbm", 		"[FTP  ]"},	/* HT_ICON_FTP */
    {"/dir-icons/image.xbm", 		"[IMAGE]"},	/* HT_ICON_IMAGE */
    {"/dir-icons/index.xbm", 		"[INDEX]"},	/* HT_ICON_INDEX */
    {"/dir-icons/movie.xbm",		"[MOVIE]"},	/* HT_ICON_MOVIE */
    {"/dir-icons/tar.xbm",		"[TAR  ]"},	/* HT_ICON_TAR */
    {"/dir-icons/text.xbm",		"[TEXT ]"},	/* HT_ICON_TEXT */
    {"/dir-icons/uu.xbm",		"[UU   ]"},	/* HT_ICON_UU */
    {"/dir-icons/sound.xbm",		"[AUDIO]"},	/* HT_ICON_AUDIO */
    {"/dir-icons/back.xbm",	 	"[UP   ]"},	/* HT_ICON_PARENT */
    {"/dir-icons/blank.xbm", 		"[BLANK]"}	/* HT_ICON_BLANK */
};

typedef enum _HTShowLength {                        /* Width of each collumn */
    HT_LENGTH_MODE  = 10,
    HT_LENGTH_NLINK = 4,
    HT_LENGTH_OWNER = 8,
    HT_LENGTH_GROUP = 8,
    HT_LENGTH_SIZE  = 8,
    HT_LENGTH_DATE  = 15,
    HT_LENGTH_SPACE = 2
} HTShowLength;

PRIVATE int HTDirMinFileLength = 20;
PRIVATE int HTDirMaxFileLength = 40;
PRIVATE int HTDirFileLength;      /* HTMinDirFileName < x < HTMaxDirFileName */
PRIVATE int HTBodyLength;
PRIVATE char *HTDirSpace = NULL;

/* ------------------------------------------------------------------------- */
#ifdef GOT_READ_DIR

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

    *strptr++ = (mode & S_IRUSR) ? 'r' : '-';
    *strptr++ = (mode & S_IWUSR) ? 'w' : '-';
    *strptr++ = (mode & S_IXUSR) ? 'x' : '-';
    *strptr++ = (mode & S_IRGRP) ? 'r' : '-';
    *strptr++ = (mode & S_IWGRP) ? 'w' : '-';
    if (mode & (S_ISVTX | S_ISGID))
	*strptr++ = 's';
    else if (mode & S_IXGRP)
	*strptr++ = 'x';
    else
	*strptr++ = '-';
    *strptr++ = (mode & S_IROTH) ? 'r' : '-';
    *strptr++ = (mode & S_IWOTH) ? 'w' : '-';
    *strptr++ = (mode & S_IXOTH) ? 'x' : '-';
}


/* 							     	IconType()
**	Finds an appropiate icon for the file, directory etc.
**     	First mode is checked to see if it is a REG FILE, DIR, LINK or SPECIAL.
**	If it is a REG FILE then the content type and content encoding is
**	considered.
*/
PRIVATE HTDirIconEnum IconType ARGS2(mode_t, mode, char *, filename)
{
    if ((mode & S_IFMT) == S_IFREG) {
	char *formatstr;
	char *strptr;
	HTAtom *encoding;
	HTAtom *language;
	HTFormat format = HTFileFormat(filename, &encoding, &language);
	formatstr = HTAtom_name(format);
	if ((strptr = strrchr(formatstr, '/')) == NULL)
	    return HT_ICON_UNKNOWN;
	if (!strncasecomp(formatstr, "text", 4)) {
	    return HT_ICON_TEXT;
	} else if (!strncasecomp(formatstr, "application", 11)) {
	    if (encoding == WWW_ENC_BINARY) {
		return HT_ICON_BINARY;
	    } else if (encoding == WWW_ENC_COMPRESS) {
		return HT_ICON_COMPRESS;
	    } else {
		return HT_ICON_TEXT;
	    }
	} else if (!strncasecomp(formatstr, "image", 5)) {
	    return HT_ICON_IMAGE;
	} else if (!strncasecomp(formatstr, "video", 5)) {
	    return HT_ICON_MOVIE;
	} else if (!strncasecomp(formatstr, "audio", 5)) {
	    return HT_ICON_AUDIO;
	} else
	    return HT_ICON_UNKNOWN;
    } else if ((mode & S_IFMT) == S_IFDIR) {
	return HT_ICON_DIR;
    } else if ((mode & S_IFMT) == S_IFLNK) {
	return HT_ICON_NLINK;
    } else {
	return HT_ICON_UNKNOWN;
    }
}


/* 							     	CenterStr()
**	Centers str_in into str_out expecting str_out having size length
**	inclusive the terminating '\0'. The output string MUST be long enough.
*/
PRIVATE void CenterStr ARGS3(char *, str_out, char *, str_in, int, length)
{
    char *inptr = str_in;
    char *outptr = str_out + (length-strlen(str_in))/2;
    memset(str_out, ' ', length);
    while ((*outptr++ = *inptr++) != '\0');
	*--outptr = ' ';
    *(str_out+length-1) = '\0';
}


/* 							     	ItoA()
**	Converts a positive int to a string backwards starting at start+len-1.
*/
PRIVATE void ItoA ARGS3(unsigned int, n, char *, start, char, len)
{
    char *sptr = start+len-1;
    do {
	*sptr-- = n%10 + '0';
    } while (n /= 10);
}


/* 							     	HTDirSize()
**	Converts a long to a string
*/
PRIVATE void HTDirSize ARGS3(unsigned long, n, char *, start, char, len)
{
    float size;
    if (n < 1024)
	ItoA((int) n, start, len);
    else if ((size = n/1024) < 1024) {
	sprintf(start+len-6, "%5.4gK", size);
	*(start+len) = ' ';
    } else if ((size = size/1024) < 1024) {
	sprintf(start+len-6, "%5.4gM", size);
	*(start+len) = ' ';
    } else {
	size /= 1024;
	sprintf(start+len-6, "%5.4gG", size);
	*(start+len) = ' ';
    }
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
	"Last Changed",
	"Permission",
	"Link",
	"Owner",
	"Group"
    };

    /* Calculate output line string length */
    HTBodyLength = HT_LENGTH_SPACE;
    StrAllocCat(topstr, HTDirSpace);
    if (HTDirShowMask & HT_DIR_SHOW_SIZE) {
	char size[HT_LENGTH_SIZE+1];
	CenterStr(size, *header, HT_LENGTH_SIZE+1);
	StrAllocCat(topstr, size);
	HTBodyLength += HT_LENGTH_SIZE;
	StrAllocCat(topstr, HTDirSpace);
	HTBodyLength += HT_LENGTH_SPACE;
    }
    if (HTDirShowMask & HT_DIR_SHOW_DATE) {
	char date[HT_LENGTH_DATE+1];
	CenterStr(date, *(header+1), HT_LENGTH_DATE+1);
	StrAllocCat(topstr, date);
	HTBodyLength += HT_LENGTH_DATE;
	StrAllocCat(topstr, HTDirSpace);
	HTBodyLength += HT_LENGTH_SPACE;
    }
    if (HTDirShowMask & HT_DIR_SHOW_MODE) {
	char mode[HT_LENGTH_MODE+1];
	CenterStr(mode, *(header+2), HT_LENGTH_MODE+1);
	StrAllocCat(topstr, mode);
	HTBodyLength += HT_LENGTH_MODE;
	StrAllocCat(topstr, HTDirSpace);
	HTBodyLength += HT_LENGTH_SPACE;
    }
    if (HTDirShowMask & HT_DIR_SHOW_NLINK) {
	char nlink[HT_LENGTH_NLINK+1];
	CenterStr(nlink, *(header+3), HT_LENGTH_NLINK+1);
	StrAllocCat(topstr, nlink);
	HTBodyLength += HT_LENGTH_NLINK;
	StrAllocCat(topstr, HTDirSpace);
	HTBodyLength += HT_LENGTH_SPACE;
    }
    if (HTDirShowMask & HT_DIR_SHOW_OWNER) {
	char owner[HT_LENGTH_OWNER+1];
	CenterStr(owner, *(header+4), HT_LENGTH_OWNER+1);
	StrAllocCat(topstr, owner);
	HTBodyLength += HT_LENGTH_OWNER;
	StrAllocCat(topstr, HTDirSpace);
	HTBodyLength += HT_LENGTH_SPACE;
    }
    if (HTDirShowMask & HT_DIR_SHOW_GROUP) {
	char group[HT_LENGTH_GROUP+1];
	CenterStr(group, *(header+5), HT_LENGTH_GROUP+1);
	StrAllocCat(topstr, group);
	HTBodyLength += HT_LENGTH_GROUP;
	StrAllocCat(topstr, HTDirSpace);
	HTBodyLength += HT_LENGTH_SPACE;
    }
    return topstr;
}


/*	Send README file			       		HTDirOutReadme
**
**  If a README file exists, then it is inserted into the document here.
*/
PRIVATE void HTDirOutReadme ARGS2(HTStructured *, target,
				  CONST char *, localname)
{ 
    FILE * fp;
    char * readme_file_name = (char *) 
	malloc(strlen(localname)+1+strlen(HT_DIR_README_FILE)+1);
    strcpy(readme_file_name, localname);
    strcat(readme_file_name, "/");
    strcat(readme_file_name, HT_DIR_README_FILE);
    
    fp = fopen(readme_file_name,  "r");
    
    if (fp) {
	HTStructuredClass targetClass;
	
	targetClass =  *target->isa;	/* (Can't init agregate in K&R) */
	START(HTML_PRE);
	for(;;){
	    char c = fgetc(fp);
	    if (c == (char)EOF) break;
	    switch (c) {
	    	case '&':
		case '<':
		case '>':
			PUTC('&');
			PUTC('#');
			PUTC((char)(c / 10));
			PUTC((char) (c % 10));
			PUTC(';');
			break;
/*	    	case '\n':
			PUTC('\r');    
Bug removed thanks to joe@athena.mit.edu */			
		default:
			PUTC(c);
	    }
	}
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
PRIVATE void HTDirOutTop ARGS3(HTStructured *, target, HTAnchor *, anchor,
			       char *, top_body)
{
    char *logical = HTAnchor_address(anchor);
    char *path = HTParse(logical, "", PARSE_PATH + PARSE_PUNCTUATION);
    char *current = strrchr(path, '/');
    char *parent = NULL;
    BOOL hotparent = NO;

    /* Find current and parent if any */
    if (current) {
	if (*(current+1)) {		 /* If not root, make link to parent */
	    *current++ = '\0';
	    if ((parent = strrchr(path, '/')) != NULL) {
		HTUnEscape(++parent);
		if (strlen(parent) > HTDirFileLength)
		    *(parent+HTDirFileLength-1) = '\0';
	    } else {
		parent = "Welcome Directory";
	    }
	    hotparent = YES;
	} else {       						  /* If root */
	    current = "Welcome Directory";
	    parent  = "None";
	}
    } else {						     /* Last attempt */
	current = "Unknown?";
	parent = "Unknown?";
    }
    /* Output title */
    START(HTML_TITLE);
    PUTS(current);
    END(HTML_TITLE);
    START(HTML_H1);
    PUTS(current);
    END(HTML_H1);

    /* Output parent directory */
    START(HTML_PRE);
    if (HTDirShowMask & HT_DIR_SHOW_ICON) {
	HTMLPutImg(target, *HTDirIcons[HT_ICON_PARENT],
		   *(HTDirIcons[HT_ICON_PARENT]+1), NULL);
	PUTS(HTDirSpace);
    }
    PUTS("Up to: ");
    if (hotparent) {
	char *relative;
	if ((relative = (char *) malloc(strlen(current) + 4)) == NULL)
	    outofmem(__FILE__, "HTDirOutTop");
	sprintf(relative, "%s/..", current);
	HTStartAnchor(target, "", relative);
	free(relative);
	PUTS(parent);
	END(HTML_A);
    } else
	PUTS(parent);
    PUTS("\n\n");

    /* Output the header line of the list */
    if (HTDirShowMask & HT_DIR_SHOW_ICON) {
	HTMLPutImg(target, *HTDirIcons[HT_ICON_BLANK],
		   *(HTDirIcons[HT_ICON_BLANK]+1), NULL);
	PUTS(HTDirSpace);
    }
    {
	char *name;
	if ((name = (char *) malloc(HTDirFileLength+1)) == NULL)
	    outofmem(__FILE__, "HTDirOutTop");
	CenterStr(name, "Name", HTDirFileLength+1);
	PUTS(name);
	free(name);
    }
    PUTS(top_body);
    END(HTML_PRE);
    START(HTML_HR);
    free(logical);
    free(path);
}


/*      							HTDirOutList()
**
**    	This function goes through the BTRee and puts each directory entry out
**    	on line.
**
**	BUGS:
**
**	If filename is anchor and filename is longer than HTMaxFileLength,
**	the body collumns are shifted to the right
*/
PRIVATE void HTDirOutList ARGS3(HTStructured *, target, HTBTree *, bt,
				char *, pathtail)
{
    char *escaped = NULL;	      		 	  /* Used for anchor */
    char *tail = NULL;
    int tailend;
    int filelen;
    HTBTElement *next_node = HTBTree_next(bt, NULL);
    HTDirKey *nkey;
    StrAllocCopy(tail, pathtail);
    tailend = strlen(tail);
    
    START(HTML_PRE);
    do {
	nkey = (HTDirKey *) next_node->object;
	escaped = HTEscape(nkey->filename, URL_XPALPHAS);
	*(tail+tailend) = '\0';
	StrAllocCat(tail, escaped);
	if (HTDirShowMask & HT_DIR_ICON_ANCHOR) {          /* Icon as anchor */
	    HTStartAnchor(target, NULL, tail);
	    HTMLPutImg(target, *HTDirIcons[nkey->icon_number],
		       *(HTDirIcons[nkey->icon_number]+1), NULL);
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
	    if (HTDirShowMask & HT_DIR_SHOW_ICON) {
		HTMLPutImg(target, *HTDirIcons[nkey->icon_number],
			   *(HTDirIcons[nkey->icon_number]+1), NULL);
		PUTS(HTDirSpace);
	    }
	    if (HTDirShowMask & HT_DIR_SHOW_SLINK && nkey->symlink)
		START(HTML_I);
	    HTStartAnchor(target, NULL, tail);
	    PUTS(nkey->filename);
	    END(HTML_A);
	    if (HTDirShowMask & HT_DIR_SHOW_SLINK && nkey->symlink)
		END(HTML_I);
	}
	if (HTBodyLength) {
	    filelen = strlen(nkey->filename);
	    while (filelen++ < HTDirFileLength)
		PUTC(' ');
	    PUTS(HTDirSpace);
	    PUTS(nkey->body);
	}
	PUTC('\n');
	KeyFree(nkey);
	FREE(escaped);
    } while ((next_node = HTBTree_next(bt, next_node)) != NULL);
    END(HTML_PRE);
    free(tail);
}


/*      							HTDirOutBottom
**
**    This function outputs the last part of the directory listings
*/
PRIVATE void HTDirOutBottom ARGS2(HTStructured *, target, unsigned int, files)
{
    char *outstr;
    if ((outstr = (char *) malloc(100)) == NULL)
	outofmem(__FILE__, "HTDirOutBottom");
    sprintf(outstr, "A total of: %u ", files);
    if (files == 1)
	strcat(outstr, "file");
    else
	strcat(outstr, "files");
    START(HTML_HR);
    PUTS(outstr);
    free(outstr);
}


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
**	NOTE:	The function expects that the URL given as directory IS escaped
**		AND Simplified and so are all URLs generated within this 
**		function BUT the UP-directory that is given as <file>/..
**
**	Returns < 0 on error, HT_LOADED on succes
*/
PUBLIC int HTBrowseDirectory ARGS2(HTRequest *, req, char *, directory)
{
    char *pathname = NULL;
    int  pathend;
    char *tail = NULL;
    HTStructured *target;				      /* HTML object */
    HTStructuredClass targetClass;
    DIR *dp;
    struct stat file_info;
    
    if (TRACE)
	fprintf(stderr,"HTBrowseDirectory: %s is a directory\n", directory);
        
    if (HTDirAccess == HT_DIR_FORBID)
	return HTLoadError(req, 403, "Directory browsing is not allowed.");
    
    /* Initialize path name for stat() */
    StrAllocCopy(pathname, directory);
    HTUnEscape(pathname);
    pathend = strlen(pathname);
    if (*(pathname+pathend-1) != '/') {
	StrAllocCat(pathname, "/");
	pathend++;
    }
    
    /* Set up the offset string of the anchor reference */
    {
	char *tptr = strrchr(directory, '/');
	if (!tptr) {					    /* '/' not found */
	    StrAllocCopy(tail, directory);
	    StrAllocCat(tail, "/");
	} else if (!*(tptr+1)) {
	    if (tptr != directory) {			     /* Trailing '/' */
		StrAllocCopy(tail, "./");
	    } else {						     /* Root */
		StrAllocCopy(tail, "/");
	    }
	} else {			     /* Relative to parent directory */
	    StrAllocCopy(tail, ++tptr);
	    StrAllocCat(tail, "/");
	}
    }

    if (HTDirAccess == HT_DIR_SELECTIVE) {
	StrAllocCat(pathname, HT_DIR_ENABLE_FILE);
	if (stat(pathname, &file_info)) {
	    if (TRACE) fprintf(stderr,
	        "HTBrowseDirectory: Can't stat() file: %s (errno: %d)\n",
			       pathname, errno);
	    free(pathname);
	    return HTLoadError(req, 403,
	        "Selective access is not enabled for this directory");
	}
    }

    if ((dp = opendir(directory)) == NULL) {
	if (TRACE) fprintf(stderr,
	    "HTBrowseDirectory: Can't open directory: %s. (errno: %d)\n",
			   directory, errno);
	return HTLoadError(req, 403, "This directory is not readable.");
    }    

    target = HTML_new(req, NULL, WWW_HTML, req->output_format,
		      req->output_stream);
    targetClass = *target->isa;			/* Copy routine entry points */
    
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
	memset(HTDirSpace, ' ', HT_LENGTH_SPACE);
	*(HTDirSpace+HT_LENGTH_SPACE) = '\0';
	topstr = InitBody();
	HTDirFileLength = 0;

	/* Build tree */
	while ((dirbuf = readdir(dp))) {
	    HTDirKey *nodekey;
	    if (!dirbuf->d_ino)		 		 /* Skip if not used */
		continue;
	    
	    /* Current and parent directories are never shown in list */
	    if (dottest &&
		(strcmp(dirbuf->d_name, ".") || strcmp(dirbuf->d_name, ".."))){
		dottest--;
		continue;
	    }
	    if (!(HTDirShowMask & HT_DIR_SHOW_HID) && *dirbuf->d_name == '.')
		continue;

	    /* First make a lstat() and get a key ready. */
	    *(pathname+pathend) = '\0';
	    StrAllocCat(pathname, dirbuf->d_name);
	    if (lstat(pathname, &file_info)) {
		if (TRACE) fprintf(stderr,
		"HTBrowseDirectory: OUPS, lstat failed on %s (errno: %d)\n",
				   pathname, errno);
		KeyFree(nodekey);
		DirAbort(bt);
		goto cleanup;
	    }
	    if ((nodekey = (HTDirKey *) calloc(1, sizeof(HTDirKey))) == NULL)
		outofmem(__FILE__, "HTFileBrowseDirectory");

	    /* Check if symbolic link, if so do a stat(). If this fails, don't
	       show the item in the list */
	    if ((file_info.st_mode & S_IFMT) == S_IFLNK) {
		int symend;		
		if (stat(pathname, &file_info)) {
		    if (TRACE)
			fprintf(stderr,
				"stat failed on symbolic link %s, errno: %d\n",
				pathname, errno);
		    KeyFree(nodekey);
		    continue;
		}
		if ((nodekey->symlink = 
		     (char *) malloc(HT_MAX_PATH+2)) == NULL)
		    outofmem(__FILE__, "HTFileBrowseDirectory");
		symend = readlink(pathname, nodekey->symlink, HT_MAX_PATH);
		if (symend < 0) {
		    if (TRACE)
			fprintf(stderr,
			        "HTBrowseDirectory: readlink errno: %d (%s)\n",
				errno, pathname);
		    FREE(nodekey->symlink);
		} else {
		    *(nodekey->symlink+symend) = '\0';
		}
	    }

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
		strcpy(nodekey->key, dirbuf->d_name);
		nodekey->filename = nodekey->key;
	    }

	    /* Update current max filename length */
	    {
		int filestrlen = strlen(nodekey->filename);
		if (filestrlen > HTDirFileLength)
		    HTDirFileLength = filestrlen;
	    }

	    /* Get Icon type */
	    if (HTDirShowMask & HT_DIR_SHOW_ICON) {
		nodekey->icon_number = IconType(file_info.st_mode,
						dirbuf->d_name);
	    }

	    /* Generate body entry in nodekey */
	    if (HTBodyLength) {
		char *bodyptr;
		if  ((nodekey->body = (char *) malloc(HTBodyLength+1)) == NULL)
		    outofmem(__FILE__, "HTBrowseDirectory");
		bodyptr = nodekey->body;
		memset(bodyptr, ' ', HTBodyLength);
		if (HTDirShowMask & HT_DIR_SHOW_SIZE) {
		    HTDirSize(file_info.st_size, bodyptr, HT_LENGTH_SIZE);
		    bodyptr += HT_LENGTH_SIZE+HT_LENGTH_SPACE;
		}
		if (HTDirShowMask & HT_DIR_SHOW_DATE) {
		    strftime(bodyptr, HT_LENGTH_DATE+1, "%b %d %y %H:%M",
			     localtime(&file_info.st_mtime));
		    bodyptr += HT_LENGTH_DATE;
		    *bodyptr = ' ';
		    bodyptr += HT_LENGTH_SPACE;
		}
		if (HTDirShowMask & HT_DIR_SHOW_MODE) {
		    FilePerm(file_info.st_mode, bodyptr);
		    bodyptr += HT_LENGTH_MODE+HT_LENGTH_SPACE;
		}
		if (HTDirShowMask & HT_DIR_SHOW_NLINK) {
		    ItoA(file_info.st_nlink, bodyptr, HT_LENGTH_NLINK);
		    bodyptr += HT_LENGTH_NLINK+HT_LENGTH_SPACE;
		}
		if (HTDirShowMask & HT_DIR_SHOW_OWNER) {
		    char *bp = bodyptr;
		    char *pwptr;
		    if ((pw_info = getpwuid(file_info.st_uid)) == NULL) {
			if (TRACE) fprintf(stderr,
			    "HTBrowseDirectory: getpwuid() failed on %s\n",
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
			if (TRACE) fprintf(stderr,
			    "HTBrowseDirectory: getgrgid() failed on %s\n",
					   pathname);
			ItoA(file_info.st_gid, bodyptr, HT_LENGTH_GROUP);
		    } else {
			grptr = gr_info->gr_name;
			while ((*bp++ = *grptr++) != '\0');
			*--bp = ' ';
		    }
		    bodyptr += HT_LENGTH_GROUP+HT_LENGTH_SPACE;
		}
		*bodyptr = '\0';
	    }	    
	    /* Now, update the BTree etc. */
	    filecnt++;
	    HTBTree_add(bt, (void *) nodekey);
	} /* End while readdir() */

	if (HTDirFileLength > HTDirMaxFileLength)
	    HTDirFileLength = HTDirMaxFileLength;
	if (HTDirFileLength < HTDirMinFileLength)
	    HTDirFileLength = HTDirMinFileLength;
   
	/* Put out the header for the HTML object */
	HTDirOutTop(target, (HTAnchor *) req->anchor, topstr);
	if (HTDirReadme == HT_DIR_README_TOP)
	    HTDirOutReadme(target, directory);

	/* Run through tree printing out in order, hopefully :-) */
	if (filecnt) {
	    HTDirOutList(target, bt, tail);
	}
	if (HTDirReadme == HT_DIR_README_BOTTOM)
	    HTDirOutReadme(target, directory);
	HTDirOutBottom(target, filecnt);

cleanup:
	FREE(HTDirSpace);
	FREE(topstr);
	free(tail);
	free(pathname);
	HTBTree_free(bt);
	closedir(dp);
    } /* End of two big loops */
    FREE_TARGET;
    return HT_LOADED;
} /* End of directory reading section */


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
**	NOTE:	The function expects that the URL given as directory IS escaped
**		AND Simplified and so are all URLs generated within this 
**		function BUT the UP-directory that is given as <file>/..
**
**	NOTE: THIS IS NOT FINISHED
**
**	Returns < 0 on error, HT_LOADED on succes
*/
PUBLIC int HTFTPBrowseDirectory ARGS3(HTRequest *, req, char *, directory,
				      HTDirLineInput, input)
{
    char *tail = NULL;
    HTStructured *target;				      /* HTML object */
    HTStructuredClass targetClass;
    
    if (TRACE)
	fprintf(stderr,"HTFTPBrowseDirectory: %s is a directory\n", directory);
        
    /* Set up the offset string of the anchor reference */
    {
	char *tptr = strrchr(directory, '/');
	if (!tptr) {					    /* '/' not found */
	    StrAllocCopy(tail, directory);
	    StrAllocCat(tail, "/");
	} else if (!*(tptr+1)) {
	    if (tptr != directory) {			     /* Trailing '/' */
		StrAllocCopy(tail, "./");
	    } else {						     /* Root */
		StrAllocCopy(tail, "/");
	    }
	} else {			     /* Relative to parent directory */
	    StrAllocCopy(tail, ++tptr);
	    StrAllocCat(tail, "/");
	}
    }

    target = HTML_new(req, NULL, WWW_HTML, req->output_format,
		      req->output_stream);
    targetClass = *target->isa;			/* Copy routine entry points */
    
    /* Now, generate the Btree and put it out to the output stream. */
    {
	unsigned int old_show_mask = HTDirShowMask;
	unsigned int filecnt = 0;
	char *topstr = NULL;
	int status;
	char dottest = 2;		  /* To avoid two strcmp() each time */
	HTChunk *dirbuf = HTChunkCreate(128);
	HTBTree *bt;

	/* TEMPORARY */
	HTDirShowMask = HT_DIR_SHOW_ICON+HT_DIR_KEY_NAME;

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
	memset(HTDirSpace, ' ', HT_LENGTH_SPACE);
	*(HTDirSpace+HT_LENGTH_SPACE) = '\0';
	topstr = InitBody();
	HTDirFileLength = 0;

	/* Build tree */
	while ((status = input(dirbuf)) > 0) {
	    HTDirKey *nodekey;
	    
	    /* Current and parent directories are never shown in list */
	    if (dottest &&
		(strcmp(dirbuf->data, ".") || strcmp(dirbuf->data, ".."))) {
		dottest--;
		continue;
	    }
	    if (!(HTDirShowMask & HT_DIR_SHOW_HID) && *dirbuf->data == '.')
		continue;

	    /* Get a key ready. */
	    if ((nodekey = (HTDirKey *) calloc(1, sizeof(HTDirKey))) == NULL)
		outofmem(__FILE__, "HTFTPBrowseDirectory");

	    /* Generate key entry in nodekey */
	    if ((nodekey->key =
		 (void *) malloc(dirbuf->size)) == NULL)
		outofmem(__FILE__, "HTFTPBrowseDirectory");
	    strcpy(nodekey->key, dirbuf->data);
	    nodekey->filename = nodekey->key;

	    /* Update current max filename length */
	    if (dirbuf->size > HTDirFileLength)
		HTDirFileLength = dirbuf->size;

	    /* Get Icon type */
	    if (HTDirShowMask & HT_DIR_SHOW_ICON) {
		nodekey->icon_number = IconType(S_IFMT | S_IFREG,
						dirbuf->data);
	    }

	    /* Generate body entry in nodekey */
	    if (HTBodyLength) {
		char *bodyptr;
		if  ((nodekey->body = (char *) malloc(HTBodyLength+1)) == NULL)
		    outofmem(__FILE__, "HTBrowseDirectory");
		bodyptr = nodekey->body;
		memset(bodyptr, ' ', HTBodyLength);
		if (HTDirShowMask & HT_DIR_SHOW_SIZE) {
		    bodyptr += HT_LENGTH_SIZE+HT_LENGTH_SPACE;
		}
		if (HTDirShowMask & HT_DIR_SHOW_DATE) {
		    bodyptr += HT_LENGTH_DATE+HT_LENGTH_SPACE;
		}
		if (HTDirShowMask & HT_DIR_SHOW_MODE) {
		    bodyptr += HT_LENGTH_MODE+HT_LENGTH_SPACE;
		}
		if (HTDirShowMask & HT_DIR_SHOW_NLINK) {
		    bodyptr += HT_LENGTH_NLINK+HT_LENGTH_SPACE;
		}
		if (HTDirShowMask & HT_DIR_SHOW_OWNER) {
		    bodyptr += HT_LENGTH_OWNER+HT_LENGTH_SPACE;
		}
		if (HTDirShowMask & HT_DIR_SHOW_GROUP) {
		    bodyptr += HT_LENGTH_GROUP+HT_LENGTH_SPACE;
		}
		*bodyptr = '\0';
	    }	    

	    /* Now, update the BTree etc. */
	    filecnt++;
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

	/* Put out the header for the HTML object */
	HTDirOutTop(target, (HTAnchor *) req->anchor, topstr);
	if (HTDirReadme == HT_DIR_README_TOP)
	    HTDirOutReadme(target, directory);

	/* Run through tree printing out in order, hopefully :-) */
	if (filecnt) {
	    HTDirOutList(target, bt, tail);
	}
	if (HTDirReadme == HT_DIR_README_BOTTOM)
	    HTDirOutReadme(target, directory);
	HTDirOutBottom(target, filecnt);

cleanup:
	FREE(HTDirSpace);
	FREE(topstr);
	free(tail);
	HTChunkFree(dirbuf);
	HTBTree_free(bt);
	HTDirShowMask = old_show_mask;		                /* TEMPORARY */
    } /* End of two big loops */
    FREE_TARGET;
    return HT_LOADED;
} /* End of FTP directory listing */

#endif /* GOT_READ_DIR */
