/*								     CSUsrLst.c
*/

/* Library include files */
#include "WWWLib.h"
#include "CSLApp.h"

struct _HTStream {
    const HTStreamClass *	isa;
    HTRequest *			request;
    HTStream *			target;
    HTChunk *			buffer;
    HTEOLState			EOLstate;
/*    HTList *                    URLs; */
};

HTList * UserList = 0;

typedef struct {
	char * name;
	char * url;
} UserListStruct_t;

PRIVATE BOOL CSUserList_addLine(HTStream * me)
{
	UserListStruct_t * newEl;
	char * pURL;
	char * end;
	end = strrchr(HTChunk_data(me->buffer), ' ');
	pURL = end + 1;
	while (*(end-1) == ' ')
		end--;
	*end = 0;
	if ((newEl = (UserListStruct_t *)HT_CALLOC(1, sizeof(UserListStruct_t))) == NULL)
		HT_OUTOFMEM("UserListStruct_t");
    StrAllocCopy(newEl->name, HTChunk_data(me->buffer));
    StrAllocCopy(newEl->url, pURL);
/*    HTList_addObject(me->URLs, (void *)pURL); */
    HTList_addObject(UserList, (void *)newEl);
    return YES;
}

PUBLIC BOOL CSUserList_destroy(void)
{
    char * userName;

    while ((userName = (char *) HTList_removeLastObject(UserList)))
        HT_FREE(userName);
    HTList_delete(UserList);
    UserList = NULL;
    return YES;
}

PUBLIC char * CSUserList_findURL(char * username)
{
    HTList * cur = UserList;
    UserListStruct_t * pUser;
    while ((pUser = (UserListStruct_t *) HTList_nextObject(cur))) {
        if (!strcasecomp(username, pUser->name))
            return pUser->url;
    }
    return 0;
}

PUBLIC int CSUserList_enum(CSUserListCallback* pCallback, void * pVoid)
{
    HTList * cur = UserList;
    UserListStruct_t * pUser;
    int retVal, i = 0;
    while ((pUser = (UserListStruct_t *) HTList_nextObject(cur))) {
        if ((retVal = (*pCallback)(pUser->name, pUser->url, i, pVoid)) != CSError_OK)
            return (retVal);
        i++;
    }
    return 0;
}

/*
**	Folding is either of CF LWS, LF LWS, CRLF LWS
*/
PRIVATE int CSUserList_put_block (HTStream * me, const char * b, int l)
{
    while (l > 0) {
	if (me->EOLstate == EOL_FCR) {
	    if (*b == LF)				   	     /* CRLF */
			me->EOLstate = EOL_FLF;
	    else if (isspace((int) *b))				   /* Folding: CR SP */
			me->EOLstate = EOL_DOT;
	    else {						 /* New line */
			CSUserList_addLine(me);
			me->EOLstate = EOL_BEGIN;
			HTChunkClear(me->buffer);
			continue;
	    }
	} else if (me->EOLstate == EOL_FLF) {
	    if (isspace((int) *b))		       /* Folding: LF SP or CR LF SP */
			me->EOLstate = EOL_DOT;
	    else {						/* New line */
			CSUserList_addLine(me);
			me->EOLstate = EOL_BEGIN;
			HTChunkClear(me->buffer);
			continue;
			}
	} else if (me->EOLstate == EOL_DOT) {
	    if (isspace((int) *b)) {
			me->EOLstate = EOL_BEGIN;
			HTChunkPutc(me->buffer, ' ');
	    } else {
			CSUserList_addLine(me);
			me->EOLstate = EOL_BEGIN;
			HTChunkClear(me->buffer);
			continue;
	    }
	} else if (*b == CR) {
	    me->EOLstate = EOL_FCR;
	} else if (*b == LF) {
	    me->EOLstate = EOL_FLF;			       /* Line found */
	} else
	    HTChunkPutc(me->buffer, *b);
	l--; b++;
    }
    if (HTChunk_size(me->buffer))
        CSUserList_addLine(me);
    return HT_OK;
}

PRIVATE int CSUserList_put_character (HTStream * me, char c)
{
    return CSUserList_put_block(me, &c, 1);
}

PRIVATE int CSUserList_put_string (HTStream * me, const char * s)
{
    return CSUserList_put_block(me, s, (int) strlen(s));
}

PRIVATE int CSUserList_flush (HTStream * me)
{
    return (*me->target->isa->flush)(me->target);
}

PRIVATE int CSUserList_free (HTStream * me)
{
    int status = HT_OK;
    if (me->target) {
	if ((status = (*me->target->isa->_free)(me->target)) == HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
    }
    HTTRACE(APP_TRACE, "Rules....... FREEING....\n");
    HTChunkFree(me->buffer);
/*    HTList_delete(me->URLs); */
   HT_FREE(me);
    return status;
}

PRIVATE int CSUserList_abort (HTStream * me, HTList * e)
{
    int status = HT_ERROR;
    if (me->target) status = (*me->target->isa->abort)(me->target, e);
    HTTRACE(APP_TRACE, "Rules....... ABORTING...\n");
    HTChunkFree(me->buffer);
/*    HTList_delete(me->URLs); */
   HT_FREE(me);
    return status;
}

/*	Structured Object Class
**	-----------------------
*/
PRIVATE const HTStreamClass CSUserListClass =
{		
    "URLListCollector",
    CSUserList_flush,
    CSUserList_free,
    CSUserList_abort,
    CSUserList_put_character,
    CSUserList_put_string,
    CSUserList_put_block
};

PUBLIC HTStream * CSUserLists (HTRequest *	request,
               	              void *	param,
               	              HTFormat	input_format,
               	              HTFormat	output_format,
               	              HTStream *	output_stream)
{
    HTStream * me;
    if ((me = (HTStream *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("CSUserLists");
    me->isa = &CSUserListClass;
    me->request = request;
    me->target = output_stream;
    me->buffer = HTChunkCreate(512);
/*    me->URLs = HTList_new(); */
    if (!UserList)
        UserList = HTList_new();
    me->EOLstate = EOL_BEGIN;
/*
    if (HTRequest_context(request))
        *((HTList **)HTRequest_context(request)) = me->URLs;
*/
    return me;
}

