/*								   HTDescript.c
**	FILE DESCRIPTIONS
**
**	(c) COPYRIGHT CERN 1994.
**	Please first read the full copyright statement in the file COPYRIGH.
**
** Authors:
**	AL	Ari Luotonen <luotonen@dxcern.cern.ch>
**
**  History:
**	30 Mar 94  AL	Written from scratch.
**
**
*/

#include "sysdep.h"

#include "HTUtils.h"
#include "HTList.h"
#include "HTAAUtil.h"
#include "HTAccess.h"
#include "HTFile.h"

#define MAX_LINE_LEN 256

PUBLIC char * HTDescriptionFile = ".www_descript";
PUBLIC BOOL HTPeekTitles = YES;

/*
 *	Get the descriptions for files in the given directory.
 *	The return value is then later passed as an argument
 *	to HTGetDescription() which returns a description
 *	string for a single file.
 */
PUBLIC HTList * HTReadDescriptions ARGS1(char *, dirname)
{
    char * name = NULL;
    FILE * fp = NULL;
    HTList * list = NULL;
    char buf[MAX_LINE_LEN + 1];

    if (!dirname) return NULL;

    name = (char*)malloc(strlen(dirname) + strlen(HTDescriptionFile) + 2);
    if (!name) outofmem(__FILE__, "HTReadDescriptions");

    sprintf(name, "%s/%s", dirname, HTDescriptionFile);
    fp = fopen(name, "r");
    if (!fp) {
	CTRACE(stderr, "DirBrowse... No description file %s\n", name);
	FREE(name);
	return NULL;
    } else {
	if (TRACE)
	    fprintf(stderr, "DirBrowse... Description file found %s\n", name);
    }

    list = HTList_new();

    while (fgets(buf, MAX_LINE_LEN, fp)) {
	char * s = buf;
	char * t = NULL;
	char * d = NULL;

	while (*s && WHITE(*s)) s++;		/* Skip initial whitespace */
	if (*s!='d' && *s!='D') continue;	/* Junk non-description lines*/

	t = s+1;
	while (*t && !WHITE(*t)) t++;	/* Find the end of the keyword */
	while (*t && WHITE(*t)) t++;	/* Find the beginning of template */

	if (*t) {
	    d = t+1;
	    while (*d && !WHITE(*d)) d++;	/* Find end of template */
	    if (*d) {
		*d++ = 0;			/* Terminate template */
		while (*d && WHITE(*d)) d++;	/* Find start of description */
		if (*d) {
		    char * p = d;
		    while (*p && *p!='\r' && *p!='\n') p++;
		    *p = 0;			/* Terminate description */
		}
	    }
	}
	if (t && d && *t && *d) {
	    char * stuff = (char *) malloc(strlen(t) + strlen(d) + 2);
	    if (!stuff) outofmem(__FILE__, "HTDirReadDescriptions");
	    sprintf(stuff, "%s %s", t, d);
	    HTList_addObject(list, (void*)stuff);
	    CTRACE(stderr, "Description. %s\n", stuff);
	}
    }
    fclose(fp);
    FREE(name);
    return list;
}


PUBLIC void HTFreeDescriptions ARGS1(HTList *, descriptions)
{
    HTList * cur = descriptions;
    char * str;

    if (descriptions) {
	while ((str = (char*)HTList_nextObject(cur)))
	    free(str);
	HTList_delete(descriptions);
    }
}


PRIVATE char * HTPeekTitle ARGS2(char *, dirname,
				 char *, filename)
{
#define PEEK_BUF_SIZE 200
    char * name;
    FILE * fp;
    char buf[PEEK_BUF_SIZE + 1];
    int status;
    char * cur;
    char * end;
    static char * ret = NULL;
    char * p;
    BOOL space = YES;

    FREE(ret);	/* From previous call */

    CTRACE(stderr, "HTPeekTitle. called, dirname=%s filename=%s\n",
	   dirname ? dirname : "-null-",
	   filename ? filename : "-null-");

    if (!dirname || !filename) return NULL;

    name = (char*)malloc(strlen(dirname) + strlen(filename) + 2);
    if (!name) outofmem(__FILE__, "HTPeekTitle");
    sprintf(name, "%s/%s", dirname, filename);

    fp = fopen(name, "r");
    if (!fp) {
	CTRACE(stderr, "HTPeekTitle. fopen failed\n");
	goto cleanup;
    }

    status = fread(buf, 1, PEEK_BUF_SIZE, fp);
    fclose(fp);
    if (status <= 0) goto cleanup;
    buf[status] = 0;

    cur = buf;
    while ((cur = strchr(cur,'<'))) {
	if (!strncasecomp(cur+1,"TITLE>",6)) {
	    cur += 7;
	    end = strchr(cur,'<');
	    while (end && strncasecomp(end+1, "/TITLE>", 7))
		end = strchr(end+1, '<');
	    if (end) *end = 0;
	    p = ret = (char*)malloc(strlen(cur) + 1);
	    if (!ret) outofmem(__FILE__, "HTPeekTitle");
	    while (*cur) {
		if (WHITE(*cur)) {
		    if (!space) {
			space = YES;
			*p++ = ' ';
		    }
		}
		else {
		    if (space) space = NO;
		    *p++ = *cur;
		}
		cur++;
	    }
	    *p = 0;
	    goto cleanup;
	}
	cur++;
    }

  cleanup:
    CTRACE(stderr, "HTPeekTitle. returning %c%s%c\n",
	   ret ? '"' : '-',  ret ? ret : "null",  ret ? '"' : '-');
    FREE(name);
    return ret;
}


/*
 *	Returns a description string (that must not be freed!)
 *	for a file with name name in directory dirname.
 *	Description file contents is in descriptions list.
 */
PUBLIC char * HTGetDescription ARGS4(HTList *,	descriptions,
				     char *,	dirname,
				     char *,	filename,
				     HTFormat,	format)
{
    HTList * cur = descriptions;
    char * t;

    if (!dirname || !filename) return NULL;
    /*
     * descriptions may well be NULL in which case we may still
     * want to peek the titles.
     */

    while ((t = (char*)HTList_nextObject(cur))) {
	char * d = strchr(t,' ');
	if (!d) continue;
	*d = 0;
	if (HTAA_templateMatch(t,filename)) {
	    *d = ' ';
	    return d+1;
	}
	*d = ' ';
    }

    if (HTPeekTitles && format == WWW_HTML)
	return HTPeekTitle(dirname, filename);
    else
	return NULL;
}

