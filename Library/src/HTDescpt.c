/*								   HTDescpt.c
**	FILE DESCRIPTIONS
**
**	(c) COPYRIGHT MIT 1995.
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

/* Library include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTFormat.h"
#include "HTList.h"

#define MAX_LINE_LEN 256

PRIVATE char * HTDescriptionFile = ".www_descript";
PRIVATE BOOL HTPeekTitles = YES;

/*
 *	Get the descriptions for files in the given directory.
 *	The return value is then later passed as an argument
 *	to HTGetDescription() which returns a description
 *	string for a single file.
 */
PUBLIC HTList * HTReadDescriptions (char * dirname)
{
    char * name = NULL;
    FILE * fp = NULL;
    HTList * list = NULL;
    char buf[MAX_LINE_LEN + 1];

    if (!dirname) return NULL;

    if ((name = (char *) HT_MALLOC(strlen(dirname) + strlen(HTDescriptionFile) + 2)) == NULL)
        HT_OUTOFMEM("HTReadDescriptions");

    sprintf(name, "%s/%s", dirname, HTDescriptionFile);
    fp = fopen(name, "r");
    if (!fp) {
	if (PROT_TRACE)
	    TTYPrint(TDEST, "DirBrowse... No description file %s\n", name);
	HT_FREE(name);
	return NULL;
    } else {
	if (WWWTRACE)
	    TTYPrint(TDEST, "DirBrowse... Description file found %s\n", name);
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
	    char * stuff;
	    if ((stuff = (char  *) HT_MALLOC(strlen(t) + strlen(d) + 2)) == NULL)
	        HT_OUTOFMEM("HTDirReadDescriptions");
	    sprintf(stuff, "%s %s", t, d);
	    HTList_addObject(list, (void*)stuff);
	    if (PROT_TRACE)
		TTYPrint(TDEST, "Description. %s\n", stuff);
	}
    }
    fclose(fp);
    HT_FREE(name);
    return list;
}


PUBLIC void HTFreeDescriptions (HTList * descriptions)
{
    HTList * cur = descriptions;
    char * str;

    if (descriptions) {
	while ((str = (char*)HTList_nextObject(cur)))
	    HT_FREE(str);
	HTList_delete(descriptions);
    }
}


PRIVATE char * HTPeekTitle (char * dirname,
				 char * filename)
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

    HT_FREE(ret);	/* from previous call */

    if (PROT_TRACE)
	TTYPrint(TDEST, "HTPeekTitle. called, dirname=%s filename=%s\n",
		dirname ? dirname : "-null-",
		filename ? filename : "-null-");

    if (!dirname || !filename) return NULL;

    if ((name = (char *) HT_MALLOC(strlen(dirname) + strlen(filename) + 2)) == NULL)
        HT_OUTOFMEM("HTPeekTitle");
    sprintf(name, "%s/%s", dirname, filename);

    fp = fopen(name, "r");
    if (!fp) {
	if (PROT_TRACE)
	    TTYPrint(TDEST, "HTPeekTitle. fopen failed\n");
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
	    if ((p = ret = (char*) HT_MALLOC(strlen(cur) + 1)) == NULL)
		HT_OUTOFMEM("HTPeekTitle");
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
    if (PROT_TRACE)
	TTYPrint(TDEST, "HTPeekTitle. returning %c%s%c\n",
		ret ? '"' : '-',  ret ? ret : "null",  ret ? '"' : '-');
    HT_FREE(name);
    return ret;
}


/*
 *	Returns a description string (that must not be freed!)
 *	for a file with name name in directory dirname.
 *	Description file contents is in descriptions list.
 */
PUBLIC char * HTGetDescription (HTList *	descriptions,
				     char *	dirname,
				     char *	filename,
				     HTFormat	format)
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
#if 0
	if (HTAA_templateMatch(t,filename)) {
#else
	if (HTStrMatch(t, filename)) {
#endif
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

