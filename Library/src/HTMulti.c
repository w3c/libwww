
/*
**		Multiformat handling
**
** History:
**	March 94  AL	Separated from HTFile.c because
**			multiformat handling would be a mess in VMS.
**
**
*/

#include "HTFile.h"
#include "HTList.h"

#ifdef USE_DIRENT		/* Set this for Sys V systems */
#define STRUCT_DIRENT struct dirent
#else
#define STRUCT_DIRENT struct direct
#endif


#ifdef GOT_READ_DIR

/* PRIVATE						multi_match()
**
**	Check if actual filename (split in parts) fulfills
**	the requirements.
*/
PRIVATE BOOL multi_match ARGS4(char **, required,
			       int,	m,
			       char **, actual,
			       int,	n)
{
    int c;
    int i,j;

    for(c=0;  c<m && c<n && !strcmp(required[c], actual[c]);  c++);

    if (!c) return NO;		/* Names differ rigth from start */

    for(i=c; i<m; i++) {
	BOOL found = NO;
	for(j=c; j<n; j++) {
	    if (!strcmp(required[i], actual[j])) {
		found = YES;
		break;
	    }
	}
	if (!found) return NO;
    }
    return YES;
}


/*
**	Get multi-match possibilities for a given file
**	----------------------------------------------
** On entry:
**	path	absolute path to one file in a directory,
**		may end in .multi.
** On exit:
**	returns	a list of ContentDesription structures
**		describing the mathing files.
**
*/
PRIVATE HTList * dir_matches ARGS1(char *, path)
{
    static char * required[MAX_SUFF+1];
    static char * actual[MAX_SUFF+1];
    int m,n;
    char * dirname = NULL;
    char * basename = NULL;
    int baselen;
    char * multi = NULL;
    DIR * dp;
    STRUCT_DIRENT * dirbuf;
    HTList * matches = NULL;

    if (!path) return NULL;

    StrAllocCopy(dirname, path);
    basename = (strrchr(dirname, '/'));
    if (!basename)
	goto dir_match_failed;
    *basename++ = 0;

    multi = strrchr(basename, MULTI_SUFFIX[0]);
    if (multi && !strcmp(multi, MULTI_SUFFIX))
	*multi = 0;
    baselen = strlen(basename);

    m = HTSplitFilename(basename, required);

    dp = opendir(dirname);
    if (!dp) {
	CTRACE(stderr,"Warning..... Can't open directory %s\n",
	       dirname);
	goto dir_match_failed;
    }

    matches = HTList_new();
    while ((dirbuf = readdir(dp))) {
	if (!dirbuf->d_ino) continue;	/* Not in use */
	if ((int)(dirbuf->d_namlen) >= baselen) {
	    n = HTSplitFilename(dirbuf->d_name, actual);
	    if (multi_match(required, m, actual, n)) {
		HTContentDescription * cd;
		cd = HTGetContentDescription(actual, n);
		if (cd) {
		    if (cd->content_type) {
			cd->filename = (char*)malloc(strlen(dirname) + 2 +
						     strlen(dirbuf->d_name));
			if (!cd->filename) outofmem(__FILE__, "dir_matches");
			sprintf(cd->filename, "%s/%s",
				dirname, dirbuf->d_name);
			HTList_addObject(matches, (void*)cd);
		    }
		    else free(cd);
		}
	    }
	}
    }
    closedir(dp);

  dir_match_failed:
    free(dirname);
    return matches;
}


/*
**	Get the best match for a given file
**	-----------------------------------
** On entry:
**	req->conversions  accepted content-types
**	req->encodings	  accepted content-transfer-encodings
**	req->languages	  accepted content-languages
**	path		  absolute pathname of the filename for
**			  which the match is desired.
** On exit:
**	returns	a newly allocated absolute filepath.
*/
PRIVATE char * HTGetBest ARGS2(HTRequest *,	req,
			       char *,		path)
{
    HTList * matches;
    HTList * cur;
    HTContentDescription * cd;
    HTContentDescription * best = NULL;
    char * best_path = NULL;

    
    if (!path || !*path) return NO;

    matches = dir_matches(path);
    if (!matches) {
	CTRACE(stderr, "No matches.. for \"%s\"\n", path);
	return NO;
    }

    /* BEGIN DEBUG */
    cur = matches;
    CTRACE(stderr, "Multi....... Possibilities for \"%s\"\n", path);
    CTRACE(stderr, "\nCONTENT-TYPE  LANGUAGE  ENCODING  QUALITY  FILE\n");
    while ((cd = (HTContentDescription*)HTList_nextObject(cur))) {
	CTRACE(stderr, "%s\t%s\t%s\t  %.5f  %s\n",
	       cd->content_type    ?HTAtom_name(cd->content_type)  :"-\t",
	       cd->content_language?HTAtom_name(cd->content_language):"-",
	       cd->content_encoding?HTAtom_name(cd->content_encoding):"-",
	       cd->quality,
	       cd->filename        ?cd->filename                     :"-");
    }
    CTRACE(stderr, "\n");
    /* END DEBUG */

    /*
    ** Finally get best that is readable
    */
    if (HTRank(matches, req->conversions, req->languages, req->encodings)) {
	cur = matches;
	while ((best = (HTContentDescription*)HTList_nextObject(cur))) {
	    if (best && best->filename) {
		if (access(best->filename, R_OK) != -1) {
		    StrAllocCopy(best_path, best->filename);
		    break;
		}
		else CTRACE(stderr, "Bad News.... \"%s\" is not readable\n",
			    best->filename);
	    }
	}
    } /* Select best */

    cur = matches;
    while ((cd = (HTContentDescription*)HTList_nextObject(cur))) {
	if (cd->filename) free(cd->filename);
	free(cd);
    }
    HTList_delete(matches);

    return best_path;
}



/*
**	Do multiformat handling
**	-----------------------
** On entry:
**	req->conversions  accepted content-types
**	req->encodings	  accepted content-transfer-encodings
**	req->languages	  accepted content-languages
**	path		  absolute pathname of the filename for
**			  which the match is desired.
**	stat_info	  pointer to result space.
**
** On exit:
**	returns	a newly allocated absolute filepath of the best
**		match, or NULL if no match.
**	stat_info	  will contain inode information as
**			  returned by stat().
*/
PUBLIC char * HTMulti ARGS3(HTRequest *,	req,
			    char *,		path,
			    struct stat *,	stat_info)
{
    char * multi;
    char * new_path = NULL;
    int stat_status = -1;

    if (!req || !path || !stat_info)
	return NULL;

    multi = strrchr(path, MULTI_SUFFIX[0]);
    if (multi && !strcmp(multi, MULTI_SUFFIX)) {
	CTRACE(stderr, "Multi....... by %s suffix\n", MULTI_SUFFIX);
	if (!(new_path = HTGetBest(req, path))) {
	    CTRACE(stderr, "Multi....... failed -- giving up\n");
	    return NULL;
	}
	path = new_path;
    }
    else {
	stat_status = stat(path, stat_info);
	if (stat_status == -1) {
	    CTRACE(stderr,
		   "AutoMulti... because couldn't stat \"%s\" (errno %d)\n",
		   path, errno);
	    if (!(new_path = HTGetBest(req, path))) {
		CTRACE(stderr, "AutoMulti... failed -- giving up\n");
		return NULL;
	    }
	    path = new_path;
	}
    }

    if (stat_status == -1)
	stat_status = stat(path, stat_info);
    if (stat_status == -1) {
	CTRACE(stderr, "Stat fails.. on \"%s\" -- giving up (errno %d)\n",
	       path, errno);
	return NULL;
    }
    else {
	if (!new_path) {
	    StrAllocCopy(new_path, path);
	    return new_path;
	}
	else return path;
    }
}

#endif /* GOT_READ_DIR */

