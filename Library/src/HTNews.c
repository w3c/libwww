/*			NEWS ACCESS				HTNews.c
**			===========
**
** History:
**	26 Sep 90	Written TBL
**	29 Nov 91	Downgraded to C, for portable implementation.
**	16 Feb 94  AL	Added Lou Montulli's Lynx & LIST NEWSGROUPS diffs.
**	 2 May 94  AL	Added HTUnEscape() to HTLoadNews(), and
**			fixed a possible security hole when the URL contains
**			a newline, that could cause multiple commands to be
**			sent to an NNTP server.
**	 8 Jul 94  FM	Insulate free() from _free structure element.
**	30 Aug 95  FTLO Added POST functionality and updated state machine
**	30 Aug 95  HFN	Cleaned up a whole lot and made a state machine
*/

/* Library Include files */
#include "tcp.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTParse.h"
#include "HTFWrite.h"
#include "HTWriter.h"
#include "HTConLen.h"
#include "HTSocket.h"
#include "HTTCP.h"
#include "HTError.h"
#include "HTChunk.h"
#include "HTEscape.h"
#include "HTReqMan.h"				/* @@@ */
#include "HTNetMan.h"				/* @@@ */
#include "HTNewsRq.h" 
#include "HTNews.h"					       /* Implements */

/* Macros and other defines */
#ifndef NEWS_PORT
#define NEWS_PORT		119			       /* See rfc977 */
#endif

#ifndef NEWS_LIST_FILE
#define NEWS_LIST_FILE		".www_news"	   /* Name of news list file */
#endif

#ifndef DEFAULT_NEWS_HOST
#define DEFAULT_NEWS_HOST	"news"
#endif

#ifndef SERVER_FILE
#define SERVER_FILE		"/usr/local/lib/rn/server"
#endif

#define MAX_NEWS_ARTICLES	0 	/* No default max number of articles */

#define PUTBLOCK(b, l)	(*me->target->isa->put_block)	     (me->target, b, l)
#define ABORT_TARGET    (*me->target->isa->abort)            (me->target, e)

/* Local context structure used in the HTNet object */
typedef enum _HTNewsState {
    NEWS_BEGIN,
    NEWS_SEEK_CACHE,
    NEWS_NEED_CONNECTION,
    NEWS_NEED_GREETING,
    NEWS_NEED_SWITCH,
    NEWS_NEED_ARTICLE,
#if HT_LISTGROUP
    NEWS_NEED_LGRP,
#endif
    NEWS_NEED_LIST,
    NEWS_NEED_GROUP,
    NEWS_NEED_XOVER,
    NEWS_NEED_HEAD,
    NEWS_NEED_POST,
    NEWS_NEED_BODY,
    NEWS_ERROR,
    NEWS_SUCCESS
} HTNewsState;

typedef struct _news_info {
    HTChunk *		cmd;
    int			repcode;
    char *		reply;
    HTNewsState         state;                    /* State of the connection */
    HTFormat		format;
    char *		name;		     /* Name of article or newsgroup */
    BOOL		sent;			    /* Read or write command */
    int                 first;            /* First article in the group list */
    int                 last;              /* Last article in the group list */
    int			total;		     /* Estimated number of articles */
    int                 current;                 /* To count # of HEADS sent */
} news_info;

/* This version of a stream is used for NEWS LIST conversion to HTML */
struct _HTStream {
    CONST HTStreamClass *	isa;
    HTStream *			target;
    HTRequest *			request;
    news_info *			news;
    HTSocketEOL			EOLstate;
    BOOL			semi_trans;
    BOOL			junk;
    char 			buffer[MAX_NEWS_LINE+1];
    int				buflen;
};

PRIVATE char *HTNewsHost = NULL;
PRIVATE int MaxArt = MAX_NEWS_ARTICLES;

/* ------------------------------------------------------------------------- */
/*			       NEWS INPUT STREAM			     */
/* ------------------------------------------------------------------------- */

/*	ScanResponse
**	------------
**	Analyzes the response from the NNTP server.
**	We only expect one line response codes.
**	Returns HT_LOADED if OK, HT_ERROR if error
*/
PRIVATE int ScanResponse (HTStream * me)
{
    news_info *news = me->news;
    *(me->buffer+me->buflen) = '\0';
    if (isdigit(*(me->buffer))) sscanf(me->buffer, "%d", &news->repcode);
    me->buflen = 0;
    news->reply = me->buffer+4;
    if (PROT_TRACE) TTYPrint(TDEST, "News Rx..... `%s\'\n", news->reply);

    /* If 2xx code and we expect data then go into semi-transparent mode */
    if (me->news->format && news->repcode/100 == 2) {
	HTRequest *req = me->request;
	me->target = HTStreamStack(me->news->format, req->output_format,
				   req->output_stream, req, NO);
	me->semi_trans = YES;
	if (!me->target) return HT_ERROR;
    }
    return HT_LOADED;
}

/*
**	Searches for NNTP header line until buffer fills up or a CRLF or LF
**	is found
*/
PRIVATE int HTNewsStatus_put_block (HTStream * me, CONST char * b, int l)
{
    while (!me->semi_trans && l-- > 0) {
	int status;
	if (me->EOLstate == EOL_FCR) {
	    if (*b == LF) {
		if (me->junk) me->junk = NO;
		me->EOLstate = EOL_BEGIN;
		if ((status = ScanResponse(me)) != HT_LOADED) return status;
	    }
	} else if (*b == CR) {
	    me->EOLstate = EOL_FCR;
	} else if (*b == LF) {
	    if (me->junk) me->junk = NO;
	    me->EOLstate = EOL_BEGIN;
	    if ((status = ScanResponse(me)) != HT_LOADED) return status;
	} else {
	    *(me->buffer+me->buflen++) = *b;
	    if (me->buflen >= MAX_NEWS_LINE) {
		if (PROT_TRACE)
		    TTYPrint(TDEST, "News Status. Line too long - chopped\n");
		me->junk = YES;
		if ((status = ScanResponse(me)) != HT_LOADED) return status;
	    }
	}
	b++;
    }	

    /*
    ** Now see if we have parts of the body to put down the stream pipe.
    ** At this point we are looking for CRLF.CRLF. We are guaranteed a stream
    */
    if (l > 0) {
	int rest = l;
	CONST char *ptr = b;
	while (rest-- > 0) {
	    if (*ptr == CR) {
		me->EOLstate = me->EOLstate==EOL_DOT ? EOL_SCR : EOL_FCR;
	    } else if (*ptr == '.') {
		me->EOLstate = me->EOLstate==EOL_FLF ? EOL_DOT : EOL_BEGIN;
	    } else if (*ptr == LF) {
		me->EOLstate = me->EOLstate>EOL_DOT ? EOL_SLF : EOL_FLF;
	    } else
		me->EOLstate = EOL_BEGIN;
	    ptr++;
	}
	if (me->EOLstate == EOL_SLF) {
	    int status = PUTBLOCK(b, l-5);
	    return status != HT_OK ? status : HT_LOADED;
	} else {
	    int status = PUTBLOCK(b, l);
	    return status;
	}
    }
    return HT_LOADED;
}

PRIVATE int HTNewsStatus_put_character (HTStream * me, char ch)
{
    return HTNewsStatus_put_block(me, &ch, 1);
}

PRIVATE int HTNewsStatus_put_string (HTStream * me, CONST char * str)
{
    return HTNewsStatus_put_block(me, str, (int) strlen(str));
}

PRIVATE int HTNewsStatus_flush (HTStream * me)
{
    return me->target ? (*me->target->isa->flush)(me->target) : HT_OK;
}

PRIVATE int HTNewsStatus_free (HTStream * me)
{
    int status = HT_OK;
    if (me->target) {
	if ((status = (*me->target->isa->_free)(me->target)) == HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
    }
    HT_FREE(me);
    return status;
}

PRIVATE int HTNewsStatus_abort (HTStream * me, HTList * e)
{
    if (me->target)
        ABORT_TARGET;
    HT_FREE(me);
    if (PROT_TRACE) TTYPrint(TDEST, "NewsStatus.. ABORTING...\n");
    return HT_ERROR;
}

PRIVATE CONST HTStreamClass HTNewsStatusClass =
{		
    "NewsStatus",
    HTNewsStatus_flush,
    HTNewsStatus_free,
    HTNewsStatus_abort,
    HTNewsStatus_put_character,
    HTNewsStatus_put_string,
    HTNewsStatus_put_block
};

PUBLIC HTStream *HTNewsStatus_new (HTRequest * request, news_info * news)
{
    HTStream *me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTNewsStatus_new");
    me->isa = &HTNewsStatusClass;
    me->request = request;
    me->news = news;
    me->EOLstate = EOL_BEGIN;
    return me;
}

/* ------------------------------------------------------------------------- */
/*				PROTOCOL FUNCTIONS			     */
/* ------------------------------------------------------------------------- */

/*
**	Set the max number of articles at the same time.
**	Default is MAX_NEWS_ARTICLES
*/
PUBLIC BOOL HTNews_setMaxArticles (int new_max)
{ 
    if (new_max >= 0) {
	MaxArt = new_max;
	return YES;
    }
    return NO;
}

/*
**	Get current max number of articles at the same time.
*/
PUBLIC int HTNews_maxArticles (void)
{ 
    return MaxArt;
}

/*
**	Sets the current NEWS server.
*/
PUBLIC BOOL HTNews_setHost (CONST char * newshost)
{
    if (newshost && *newshost) {
	StrAllocCopy(HTNewsHost, newshost);
	{
	    char *strptr = HTNewsHost;
	    while (*strptr) {
		*strptr = TOLOWER(*strptr);
		strptr++;
	    }
	    
	    /* Remove final dot or paste in domain name */
	    if (strchr(HTNewsHost, '.')) {
		if (*(HTNewsHost+strlen(HTNewsHost)-1) == '.')
		    *(HTNewsHost+strlen(HTNewsHost)-1) = '\0';
	    } else {
		CONST char *domain = HTGetDomainName();
		if (domain) {
		    StrAllocCat(HTNewsHost, ".");
		    StrAllocCat(HTNewsHost, domain);
		}
	    }		
	}
	if (PROT_TRACE)
	    TTYPrint(TDEST, "SetNewsHost. Host name is `%s\'\n", HTNewsHost);
	return YES;
    } else {
	if (PROT_TRACE)
	    TTYPrint(TDEST, "SetNewsHost. Bad argument ignored\n");
	return NO;
    }
}

/*
**	Except on the NeXT, we pick up the NewsHost name from
**
**	1.	Environment variable NNTPSERVER
**	2.	File SERVER_FILE
**	3.	Compilation time macro DEFAULT_NEWS_HOST
**
**	On the NeXT, we pick up the NewsHost name from, in order:
**
**	1.	WorldWideWeb default "NewsHost"
**	2.	News default "NewsHost"
**	3.	Compilation time macro DEFAULT_NEWS_HOST
**
**	Return:	HTNewsHost if success else NULL
*/
PUBLIC CONST char *HTNews_host (void)
{
    if (HTNewsHost) {
	if (*HTNewsHost) {
	    if (PROT_TRACE)
		TTYPrint(TDEST, "GetNewsHost. found as `%s\'\n", HTNewsHost);
	    return HTNewsHost;
	} else
	    return NULL;		 /* We couldn't get it the last time */
    }
    {
	char *newshost = NULL;
        char buffer[80];

#ifdef NeXTStep
	if ((newshost = NXGetDefaultValue("WorldWideWeb","NewsHost")) == 0)
	    if ((newshost = NXGetDefaultValue("News","NewsHost")) == 0)
		newshost = DEFAULT_NEWS_HOST;
#else
	if ((newshost = (char *) getenv("NNTPSERVER")) == NULL) {
	    FILE *fp = fopen(SERVER_FILE, "r");
	    *(buffer+79) = '\0';
	    if (fp) {
		if (fgets(buffer, 79, fp)) {
		    char *end;
		    newshost = buffer;
		    while (*newshost == ' ' || *newshost == '\t')
			newshost++;
		    end = newshost;
		    while (*end && !isspace(*end))
			end++;
		    *end = '\0';
		}
		fclose(fp);
	    }
	}
#endif /* NestStep */

	if (!newshost || !*newshost)
	    newshost = DEFAULT_NEWS_HOST;
	if (HTNews_setHost(newshost))
	    return HTNewsHost;
	StrAllocCopy(HTNewsHost, "");
	return NULL;
    }
}

/*
**	Free Newshostname
*/
PUBLIC void HTFreeNewsHost (void)
{
    HT_FREE(HTNewsHost);
}

/*	HTNewsCleanup
**	-------------
**      This function closes the connection and frees memory.
**      Returns YES on OK, else NO
*/
PRIVATE int HTNewsCleanup (HTRequest * req, int status)
{
    HTNet *net = req->net;
    news_info *news = (news_info *) net->context;

    /* Free stream with data TO network */
    if (!HTRequest_isDestination(req) && req->input_stream) {
	if (status == HT_INTERRUPTED)
	    (*req->input_stream->isa->abort)(req->input_stream, NULL);
	else
	    (*req->input_stream->isa->_free)(req->input_stream);
    }

    /* Remove the request object and our own context structure for nntp */
    HTNet_delete(net, status);
    if (news) {
	HT_FREE(news->name);
	HTChunk_delete(news->cmd);
	HT_FREE(news);
    }
    return YES;
}

PRIVATE int SendCommand (HTRequest *request, news_info *news,
			 char *token, char *pars)
{
    int len = strlen(token) + (pars ? strlen(pars)+1:0) + 2;
    HTChunk_clear(news->cmd);
    HTChunk_ensure(news->cmd, len);
    if (pars && *pars)
	sprintf(HTChunk_data(news->cmd), "%s %s%c%c", token, pars, CR, LF);
    else
	sprintf(HTChunk_data(news->cmd), "%s%c%c", token, CR, LF);
    if (PROT_TRACE) TTYPrint(TDEST, "News Tx..... %s", HTChunk_data(news->cmd));
    return (*request->input_stream->isa->put_block)
	(request->input_stream, HTChunk_data(news->cmd), len);
}

/*		Load data object from NNTP Server		     HTLoadNews
**		=================================
**
**	Given a hypertext addres, this routine loads a document
**
** On Entry,
**	request		The request structure
**
**	returns		HT_ERROR	Error has occured or interrupted
**			HT_WOULD_BLOCK  if operation would have blocked
**			HT_LOADED	if 200 OK
**			HT_NO_DATA	if No Response
**			HT_RETRY	if Service Unavail.
*/
PUBLIC int HTLoadNews (SOCKET soc, HTRequest * request, SockOps ops)
{
    int status = HT_ERROR;
    HTNet *net = request->net;
    HTParentAnchor *anchor = HTRequest_anchor(request);
    char *url = HTAnchor_physical(anchor);
    news_info *news;
    
    /*
    ** Initiate a new nntp structure and bind to request structure
    ** This is actually state NNTP_BEGIN, but it can't be in the state
    ** machine as we need the structure first.
    */
    if (ops == FD_NONE) {
	if (PROT_TRACE) 
	    TTYPrint(TDEST, "NNTP........ Looking for `%s\'\n", url);
	if ((news = (news_info *) HT_CALLOC(1, sizeof(news_info))) == NULL)
	    HT_OUTOFMEM("HTLoadNews");
	news->cmd = HTChunk_new(128);
	news->state = NEWS_BEGIN;
	net->context = news;
    } else if (ops == FD_CLOSE) {			      /* Interrupted */
	if(HTRequest_isPostWeb(request)&&!HTRequest_isMainDestination(request))
	    HTNewsCleanup(request, HT_IGNORE);
	else
	    HTNewsCleanup(request, HT_INTERRUPTED);
	return HT_OK;
    } else
	news = (news_info *) net->context;		/* Get existing copy */
     
    /* Now start the state machine */
    while (1) {
        switch (news->state) {
          case NEWS_BEGIN:
	    news->state = (!strchr(url, '@') && strchr(url, '*')) ?
		NEWS_SEEK_CACHE : NEWS_NEED_CONNECTION;
	    break;

          case NEWS_SEEK_CACHE:			       /* @@@ DO THIS @@@@@@ */
	    news->state = NEWS_NEED_CONNECTION;
	    break;

	  case NEWS_NEED_CONNECTION: 		/* Let's set up a connection */
	    if (!strncasecomp(url, "news:", 5)) {
		CONST char *newshost = HTNews_host();
		StrAllocCopy(news->name, url+5);
		if (newshost) {
		    char *newshack = NULL;    /* Then we can use HTParse :-) */
		    StrAllocCopy(newshack, "news://");
		    StrAllocCat(newshack, newshost);
		    status = HTDoConnect(net, (char *) newshack, NEWS_PORT);
		    HT_FREE(newshack);
		} else
		    news->state = NEWS_ERROR;
	    } else if (!strncasecomp(url, "nntp:", 5)) {
		news->name = HTParse(url, "", PARSE_PATH);
		status = HTDoConnect(net, url, NEWS_PORT);
	    } else {
		if (PROT_TRACE) TTYPrint(TDEST, "News........ Huh?");
		news->state = NEWS_ERROR;
            }
            if (status == HT_OK) {
		BOOL greeting = NO;
		char *s_class = HTDNS_serverClass(net->dns);
		if (s_class && strcasecomp(s_class, "nntp")) {
		    HTRequest_addError(request, ERR_FATAL, NO, HTERR_CLASS,
				       NULL, 0, "HTLoadNews");
		    news->state = NEWS_ERROR;
		    break;
		}
		HTDNS_setServerClass(net->dns, "nntp");
		if (HTDNS_socket(net->dns) == INVSOC) {
		    HTDNS_setSocket(net->dns, net->sockfd);
		    greeting = YES;
		}
		if (PROT_TRACE)
		    TTYPrint(TDEST, "News........ Connected, socket %d\n",
			    net->sockfd);

		/* Set up stream TO network */
		request->input_stream = HTWriter_new(net, YES);
	      
		/*
		** Set up concurrent read/write if this request isn't the
		** source for a PUT or POST. As source we don't start reading
		** before all destinations are ready. If destination then
		** register the input stream and get ready for read
		*/
		if (HTRequest_isPostWeb(request)) {
		    HTEvent_Register(net->sockfd, request, (SockOps) FD_READ,
				     HTLoadNews, net->priority);
		    HTRequest_linkDestination(request);
		}

		/* Set up stream FROM network and corresponding read buffer */
		net->isoc = HTInputSocket_new(net->sockfd);
		net->target = HTNewsStatus_new(request, news);
		news->state = greeting ? NEWS_NEED_GREETING : NEWS_NEED_SWITCH;
	    } else if (status == HT_WOULD_BLOCK || status == HT_PERSISTENT)
		return HT_OK;
	    else
		news->state = NEWS_ERROR;
	    break;

	  case NEWS_NEED_GREETING:
	    status = HTSocketRead(request, net);
	    if (status == HT_WOULD_BLOCK)
		return HT_OK;
	    else if (status == HT_LOADED) {
		if (news->repcode/100 == 2)
		    news->state = NEWS_NEED_SWITCH;
		else
		    news->state = NEWS_ERROR;
	    } else
		news->state = NEWS_ERROR;
	    break;

	  case NEWS_NEED_SWITCH:
	    /*
	    ** Find out what to ask the news server. Syntax of address is
	    **	xxx@yyy		Article
	    **	<xxx@yyy>	Same article
	    **	xxxxx		News group (no "@")
	    */
	    if (request->method == METHOD_GET) {
		if (strchr(url, '@')) {				  /* ARTICLE */
		    if (*(news->name) != '<') {		  /* Add '<' and '>' */
			char *newart;
			if ((newart = (char  *) HT_MALLOC(strlen(news->name)+3)) == NULL)
			    HT_OUTOFMEM("HTLoadNews");
			sprintf(newart, "<%s>", news->name);
			HT_FREE(news->name);
			news->name = newart;
		    }
		    news->state = NEWS_NEED_ARTICLE;
		} else if (strchr(url, '*'))
		    news->state = NEWS_NEED_LIST;
		else
		    news->state = NEWS_NEED_GROUP;
	    } else if (request->method == METHOD_POST)
		news->state = NEWS_NEED_POST;
	    else {
		HTRequest_addError(request, ERR_FATAL, NO,
				   HTERR_NOT_IMPLEMENTED,NULL, 0,"HTLoadNews");
		news->state = NEWS_ERROR;
	    }
	    HTUnEscape(news->name);
	    HTCleanTelnetString(news->name);
	    break;

	  case NEWS_NEED_ARTICLE:
	    if (!news->sent) {
		status = SendCommand(request, news, "ARTICLE", news->name);
		if (status == HT_WOULD_BLOCK)
		    return HT_OK;
		else if (status == HT_ERROR)
		    news->state = NEWS_ERROR;
		news->format = WWW_MIME;
		news->sent = YES;
	    } else {
		status = HTSocketRead(request, net);
		if (status == HT_WOULD_BLOCK)
		    return HT_OK;
		else if (status == HT_OK)
		    news->state = NEWS_NEED_BODY;
		else if (status == HT_LOADED) {
		    news->state = (news->repcode/100 == 2) ?
			NEWS_SUCCESS : NEWS_ERROR;
		} else
		    news->state = NEWS_ERROR;
		news->sent = NO;
	    }
	    break;

#if HT_LISTGROUP
	  case NEWS_NEED_LGRP:
	    if (!news->sent) {
		status = SendCommand(request, news, "LIST", "NEWSGROUPS");
		if (status == HT_WOULD_BLOCK)
		    return HT_OK;
		else if (status == HT_ERROR)
		    news->state = NEWS_ERROR;
 		news->format = WWW_NNTP_LIST;
		news->sent = YES;
	    } else {
		status = HTSocketRead(request, net);
		if (status == HT_WOULD_BLOCK)
		    return HT_OK;
		else if (status == HT_OK)
		    news->state = NEWS_NEED_BODY;
		else if (status == HT_LOADED) {
		    news->state = (news->repcode/100 == 2) ?
			NEWS_SUCCESS : NEWS_NEED_LIST;
		} else
		    news->state = NEWS_ERROR;
		news->sent = NO;
	    }
	    break;
#endif /* HT_LISTGROUP */

	  case NEWS_NEED_LIST:
	    if (!news->sent) {
		status = SendCommand(request, news, "LIST", NULL);
		if (status == HT_WOULD_BLOCK)
		    return HT_OK;
		else if (status == HT_ERROR)
		    news->state = NEWS_ERROR;
		news->format = WWW_NNTP_LIST;
		news->sent = YES;
	    } else {
		status = HTSocketRead(request, net);
		if (status == HT_WOULD_BLOCK)
		    return HT_OK;
		else if (status == HT_OK)
		    news->state = NEWS_NEED_BODY;
		else if (status == HT_LOADED) {
		    news->state = (news->repcode/100 == 2) ?
			NEWS_SUCCESS : NEWS_ERROR;
		} else
		    news->state = NEWS_ERROR;
		news->sent = NO;
	    }
	    break;

	  case NEWS_NEED_GROUP:
	    if (!news->sent) {
		status = SendCommand(request, news, "GROUP", news->name);
		if (status == HT_WOULD_BLOCK)
		    return HT_OK;
		else if (status == HT_ERROR)
		    news->state = NEWS_ERROR;
		news->sent = YES;
	    } else {
		status = HTSocketRead(request, net);
		if (status == HT_WOULD_BLOCK)
		    return HT_OK;
		else if (status == HT_LOADED) {
		    if (news->repcode/100 == 2) {
			if (sscanf(news->reply, "%d%d%d", &news->total,
				   &news->first, &news->last) == 3) {
			    if (MaxArt && news->total>MaxArt)
				news->last = news->first-MaxArt;
			    news->current = news->first;
			    news->state = NEWS_NEED_XOVER;
			} else
			    news->state = NEWS_ERROR;
		    } else
			news->state = NEWS_ERROR;
		} else
		    news->state = NEWS_ERROR;
		news->sent = NO;
	    }
	    break;

	  case NEWS_NEED_XOVER:
	    if (!news->sent) {
		char buf[20];
		sprintf(buf, "%d-%d", news->first, news->last);
		status = SendCommand(request, news, "XOVER", buf);
		if (status == HT_WOULD_BLOCK)
		    return HT_OK;
		else if (status == HT_ERROR)
		    news->state = NEWS_ERROR;
		news->format = WWW_NNTP_OVER;
		news->sent = YES;
	    } else {
		status = HTSocketRead(request, net);
		if (status == HT_WOULD_BLOCK)
		    return HT_OK;
		else if (status == HT_OK)
		    news->state = NEWS_NEED_BODY;
		else if (status == HT_LOADED) {
		    if (news->repcode/100 == 2)
			news->state = NEWS_SUCCESS;
		    else {
			news->format = WWW_NNTP_HEAD;
			news->state = NEWS_NEED_HEAD;
		    }
		} else
		    news->state = NEWS_ERROR;
		news->sent = NO;
	    }
	    break;

	  case NEWS_NEED_HEAD:
	    if (!news->sent) {
		char buf[10];
		sprintf(buf, "%d", news->current++);
		status = SendCommand(request, news, "HEAD", buf);
		if (status == HT_WOULD_BLOCK)
		    return HT_OK;
		else if (status == HT_ERROR)
		    news->state = NEWS_ERROR;
		news->sent = YES;
	    } else {
		status = HTSocketRead(request, net);
		if (status == HT_WOULD_BLOCK)
		    return HT_OK;
		else if (status == HT_LOADED) {
		    if (news->repcode/100 == 2) {
			if (news->current > news->last)
			    news->state = NEWS_SUCCESS;
		    } else
			news->state = NEWS_ERROR;
		} else
		    news->state = NEWS_ERROR;
		news->sent = NO;
	    }
	    break;

	  case NEWS_NEED_POST:
	    request->input_stream =
		HTNewsPost_new(request, HTBuffer_new(request->input_stream,
						     request, 512));

	    /* Remember to convert to CRLF */

	    news->state = NEWS_NEED_BODY;
	    break;

          case NEWS_NEED_BODY:
            if (ops == FD_WRITE || ops == FD_NONE) {
		if (HTRequest_isDestination(request)) {
		    HTNet *srcnet = request->source->net;
		    HTEvent_Register(srcnet->sockfd, request->source,
				     (SockOps) FD_READ,
				     HTLoadNews, srcnet->priority);
		    return HT_OK;
		}
		status = request->PostCallback ?
                    request->PostCallback(request, request->input_stream) :
			(*request->input_stream->isa->flush)(request->input_stream);
 		if (status == HT_WOULD_BLOCK)
                    return HT_OK;
                else 	
                    ops = FD_READ;	  /* Trick to ensure that we do READ */
	    } else if (ops == FD_READ) {
                status = HTSocketRead(request, net);
		if (status == HT_WOULD_BLOCK)
		    return HT_OK;
                else if (status == HT_LOADED)
		    news->state = NEWS_SUCCESS;
		else
		    news->state = NEWS_ERROR;
	    } else {
		news->state = NEWS_ERROR;
	    }
	    break;
		
	  case NEWS_SUCCESS:
	    if (HTRequest_isPostWeb(request)) {
		BOOL main = HTRequest_isMainDestination(request);
		if (HTRequest_isDestination(request)) {
		    HTLink *link =
			HTAnchor_findLink((HTAnchor *) request->source->anchor,
					  (HTAnchor *) request->anchor);
		    HTLink_setResult(link, HT_LINK_OK);
		}
		HTRequest_removeDestination(request);
		HTNewsCleanup(request, main ? HT_LOADED : HT_IGNORE);
	    } else
		HTNewsCleanup(request, HT_LOADED);
	    return HT_OK;
	    break;
	    
	  case NEWS_ERROR:
	    /* Clean up the other connections or just this one */
	    if (HTRequest_isPostWeb(request)) {
		BOOL main = HTRequest_isMainDestination(request);
		HTRequest_killPostWeb(request);
		if (HTRequest_isDestination(request)) {
		    HTLink *link =
			HTAnchor_findLink((HTAnchor *) request->source->anchor,
					  (HTAnchor *) request->anchor);
		    HTLink_setResult(link, HT_LINK_ERROR);
		}
		HTRequest_removeDestination(request);
		HTNewsCleanup(request, main ? HT_ERROR : HT_IGNORE);
	    } else
		HTNewsCleanup(request, HT_ERROR);
	    return HT_OK;
	    break;
	}
    } /* End of while(1) */
}
