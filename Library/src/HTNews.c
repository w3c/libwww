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
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "WWWStream.h"
#include "WWWTrans.h"
#include "HTReqMan.h"				/* @@@ */
#include "HTNewsRq.h"
#include "HTNewsLs.h"
#include "HTNews.h"					       /* Implements */

/* Macros and other defines */
#ifndef NEWS_LIST_FILE
#define NEWS_LIST_FILE		".www_news"	   /* Name of news list file */
#endif

#define MAX_NEWS_ARTICLES	0 	/* No default max number of articles */

#define PUTBLOCK(b, l)	(*me->target->isa->put_block)	     (me->target, b, l)
#define ABORT_TARGET    (*me->target->isa->abort)            (me->target, e)

/* Local context structure used in the HTNet object */
typedef enum _HTNewsState {
    NEWS_ERROR		= -3,
    NEWS_SUCCESS	= -2,
    NEWS_NO_DATA	= -1,
    NEWS_BEGIN		= 0,
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
    NEWS_NEED_BODY
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
    HTNet *		net;
} news_info;

/* This version of a stream is used for NEWS LIST conversion to HTML */
struct _HTStream {
    const HTStreamClass *	isa;
    HTStream *			target;
    HTRequest *			request;
    news_info *			news;
    HTEOLState			EOLstate;
    BOOL			semi_trans;
    BOOL			junk;
    char 			buffer[MAX_NEWS_LINE+1];
    int				buflen;
    HTHost *			host;
};

struct _HTInputStream {
    const HTInputStreamClass *	isa;
};

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
    if (isdigit((int) *(me->buffer))) sscanf(me->buffer, "%d", &news->repcode);
    me->buflen = 0;
    news->reply = me->buffer+4;
    HTTRACE(PROT_TRACE, "News Rx..... `%s\'\n" _ news->reply);

    /* If 2xx code and we expect data then go into semi-transparent mode */
    if (me->news->format && news->repcode/100 == 2) {
	HTRequest *req = me->request;
	me->target = HTStreamStack(me->news->format, req->output_format,
				   req->output_stream, req, NO);
	me->semi_trans = YES;
	if (!me->target) return HT_ERROR;
    } else if (news->repcode/100 == 4) {
	HTRequest_addError(me->request, ERR_FATAL, NO, HTERR_NOT_FOUND,
			   news->reply, strlen(news->reply), "ScanResponse");
    }
    return HT_LOADED;
}

/*
**	Searches for NNTP header line until buffer fills up or a CRLF or LF
**	is found
*/
PRIVATE int HTNewsStatus_put_block (HTStream * me, const char * b, int l)
{
    int status;
    HTHost_setConsumed(me->host, l);
    while (!me->semi_trans && l-- > 0) {
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
		HTTRACE(PROT_TRACE, "News Status. Line too long - chopped\n");
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
	const char *ptr = b;
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

PRIVATE int HTNewsStatus_put_string (HTStream * me, const char * str)
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
    HTTRACE(PROT_TRACE, "NewsStatus.. ABORTING...\n");
    return HT_ERROR;
}

PRIVATE const HTStreamClass HTNewsStatusClass =
{		
    "NewsStatus",
    HTNewsStatus_flush,
    HTNewsStatus_free,
    HTNewsStatus_abort,
    HTNewsStatus_put_character,
    HTNewsStatus_put_string,
    HTNewsStatus_put_block
};

PRIVATE HTStream * HTNewsStatus_new (HTRequest * request, news_info * news,
				     HTHost * host)
{
    HTStream *me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("HTNewsStatus_new");
    me->isa = &HTNewsStatusClass;
    me->request = request;
    me->news = news;
    me->EOLstate = EOL_BEGIN;
    me->host = host;
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

/*	HTNewsCleanup
**	-------------
**      This function closes the connection and frees memory.
**      Returns YES on OK, else NO
*/
PRIVATE int HTNewsCleanup (HTRequest * req, int status)
{
    HTNet * net = HTRequest_net(req);
    news_info *news = (news_info *) HTNet_context(net);
    HTStream * input = HTRequest_inputStream(req);

    /* Free stream with data TO network */
    if (!HTRequest_isDestination(req))
	HTRequest_removeDestination(req);
    else if (input) {
	if (status == HT_INTERRUPTED)
	    (*input->isa->abort)(input, NULL);
	else
	    (*input->isa->_free)(input);
	HTRequest_setInputStream(req, NULL);
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
    HTStream * input = HTRequest_inputStream(request);
    int len = strlen(token) + (pars ? strlen(pars)+1:0) + 2;
    HTChunk_setSize(news->cmd, len);
    if (pars && *pars)
	sprintf(HTChunk_data(news->cmd), "%s %s%c%c", token, pars, CR, LF);
    else
	sprintf(HTChunk_data(news->cmd), "%s%c%c", token, CR, LF);
    HTTRACE(PROT_TRACE, "News Tx..... %s" _ HTChunk_data(news->cmd));
    return (*input->isa->put_block)(input, HTChunk_data(news->cmd), len);
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
PRIVATE int NewsEvent (SOCKET soc, void * pVoid, HTEventType type);

PUBLIC int HTLoadNews (SOCKET soc, HTRequest * request)
{
    news_info *news;
    HTParentAnchor *anchor = HTRequest_anchor(request);
    HTNet * net = HTRequest_net(request);
    char * url = HTAnchor_physical(anchor);

    HTTRACE(PROT_TRACE, "NNTP........ Looking for `%s\'\n" _ url);
    if ((news = (news_info *) HT_CALLOC(1, sizeof(news_info))) == NULL)
	HT_OUTOFMEM("HTLoadNews");
    news->cmd = HTChunk_new(128);
    news->state = NEWS_BEGIN;
    news->net = net;
    HTNet_setContext(net, news);
    HTNet_setEventCallback(net, NewsEvent);
    HTNet_setEventParam(net, news);  /* callbacks get http* */

    return NewsEvent(soc, news, HTEvent_BEGIN);		/* get it started - ops is ignored */
}

PRIVATE int NewsEvent (SOCKET soc, void * pVoid, HTEventType type)
{
    news_info *news = (news_info *)pVoid;
    int status = HT_ERROR;
    HTNet * net = news->net;
    HTRequest * request = HTNet_request(net);
    HTParentAnchor * anchor = HTRequest_anchor(request);
    char * url = HTAnchor_physical(anchor);
    HTHost * host = HTNet_host(net);
    
    /*
    ** Initiate a new nntp structure and bind to request structure
    ** This is actually state NNTP_BEGIN, but it can't be in the state
    ** machine as we need the structure first.
    */
    if (type == HTEvent_CLOSE) {			      /* Interrupted */
	HTRequest_addError(request, ERR_FATAL, NO, HTERR_INTERRUPTED,
			   NULL, 0, "HTLoadHTTP");
	HTNewsCleanup(request, HT_INTERRUPTED);
	return HT_OK;
    } else
	news = (news_info *) HTNet_context(net);		/* Get existing copy */

    /* Now jump into the machine. We know the state from the previous run */
    while (1) {
        switch (news->state) {
          case NEWS_BEGIN:
	    news->state = (!strchr(url, '@') && strchr(url, '*')) ?
		NEWS_SEEK_CACHE : NEWS_NEED_CONNECTION;
	    break;

	case NEWS_SEEK_CACHE:
	    if (HTNewsCache_before(request, NULL, 0) == HT_LOADED)
		news->state = NEWS_SUCCESS;
	    else
		news->state = NEWS_NEED_CONNECTION;
	    break;

	  case NEWS_NEED_CONNECTION: 		/* Let's set up a connection */
	    if (!strncasecomp(url, "news:", 5)) {
		HTUserProfile * up = HTRequest_userProfile(request);
		char * newshost = HTUserProfile_news(up);
		StrAllocCopy(news->name, url+5);
		if (newshost) {
		    char *newshack = NULL;    /* Then we can use HTParse :-) */
		    StrAllocCopy(newshack, "news://");
		    StrAllocCat(newshack, newshost);
		    status = HTHost_connect(host, net, (char *) newshack);
		    host = HTNet_host(net);
		    HT_FREE(newshack);
		} else
		    news->state = NEWS_ERROR;
	    } else if (!strncasecomp(url, "nntp:", 5)) {
		news->name = HTParse(url, "", PARSE_PATH);
		status = HTHost_connect(host, net, url);
		host = HTNet_host(net);
	    } else {
		HTTRACE(PROT_TRACE, "News........ Huh?");
		news->state = NEWS_ERROR;
            }
            if (status == HT_OK) {
		BOOL greeting = NO;

		/* Set up the persistent connection */
		if (!HTNet_persistent(net)) {
		    HTNet_setPersistent(net, YES, HT_TP_SINGLE);
		    greeting = YES;
		}

		/*
		** Check the protocol class to see if we have connected to a
		** the right class of server, in this case HTTP.
		*/
		{
		    HTHost * host = HTNet_host(net);
		    char * s_class = HTHost_class(host);
		    if (s_class && strcasecomp(s_class, "nntp")) {
			HTRequest_addError(request, ERR_FATAL, NO, HTERR_CLASS,
					   NULL, 0, "HTLoadNews");
			news->state = NEWS_ERROR;
			break;
		    }
		    HTHost_setClass(host, "nntp");
		}

		/* 
		** Create the stream pipe FROM the channel to the application.
		** The target for the input stream pipe is set up using the
		** stream stack.
		*/
		{
		    HTStream * rstream = HTNewsStatus_new(request, news, host);
		    HTNet_setReadStream(net, rstream);
		    HTRequest_setOutputConnected(request, YES);
		}

		/*
		** Create the stream pipe TO the channel from the application
		** and hook it up to the request object
		*/
		{
		    HTOutputStream * output = HTNet_getOutput(net, NULL, 0);
		    HTRequest_setInputStream(request, (HTStream *) output);
		}

		news->state = greeting ? NEWS_NEED_GREETING : NEWS_NEED_SWITCH;

	    } else if (status == HT_WOULD_BLOCK || status == HT_PENDING)
		return HT_OK;
	    else
		news->state = NEWS_ERROR;
	    break;

	  case NEWS_NEED_GREETING:
	    status = HTHost_read(HTNet_host(net), net);
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
	  {
	      HTMethod method = HTRequest_method(request);
	      /*
	      ** Find out what to ask the news server. Syntax of address is
	      **	xxx@yyy		Article
	      **	<xxx@yyy>	Same article
	      **	xxxxx		News group (no "@")
	      */
	      if (method == METHOD_GET) {
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
	      } else if (method == METHOD_POST)
		  news->state = NEWS_NEED_POST;
	      else {
		  HTRequest_addError(request, ERR_FATAL, NO,
				     HTERR_NOT_IMPLEMENTED,NULL, 0,"HTLoadNews");
		  news->state = NEWS_ERROR;
	      }
	      HTUnEscape(news->name);
	      HTCleanTelnetString(news->name);
	  }
	  break;

	  case NEWS_NEED_ARTICLE:
	    if (!news->sent) {
		status = SendCommand(request, news, "ARTICLE", news->name);
		if (status == HT_WOULD_BLOCK)
		    return HT_OK;
		else if (status == HT_ERROR)
		    news->state = NEWS_ERROR;
		news->format = WWW_MIME;

		/*
		** Set the default content type to plain text as news servers
		** almost never send any useful information about the length
		** of the body or the type - the success of MIME!
		*/
		HTAnchor_setFormat(anchor, WWW_PLAINTEXT);
		news->sent = YES;
	    } else {
		status = HTHost_read(HTNet_host(net), net);
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
		status = HTHost_read(HTNet_host(net), net);
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
		status = HTHost_read(HTNet_host(net), net);
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
		status = HTHost_read(HTNet_host(net), net);
		if (status == HT_WOULD_BLOCK)
		    return HT_OK;
		else if (status == HT_LOADED) {
		    if (news->repcode/100 == 2) {
			if (sscanf(news->reply, "%d%d%d", &news->total,
				   &news->first, &news->last) == 3) {
			    if (MaxArt && news->total>MaxArt)
				news->last = news->first-MaxArt;
			    news->current = news->first;

			    /* If no content in this group */
			    if (news->first == news->last) {
				HTRequest_addError(request, ERR_FATAL, NO,
						   HTERR_NO_CONTENT,
						   NULL, 0, "HTLoadNews");
				news->state = NEWS_NO_DATA;
				break;
			    }
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
		status = HTHost_read(HTNet_host(net), net);
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
		status = HTHost_read(HTNet_host(net), net);
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
	  {
	      HTStream * oldinput = HTRequest_inputStream(request);
	      HTStream * newinput =
		  HTNewsPost_new(request, HTBuffer_new(oldinput, request,512));
	      HTRequest_setInputStream(request, newinput);
	      
	      /* Remember to convert to CRLF */

	  }
	  news->state = NEWS_NEED_BODY;
	  break;

          case NEWS_NEED_BODY:
            if (type == HTEvent_WRITE || type == HTEvent_BEGIN) {
		if (HTRequest_isDestination(request)) {
		    HTRequest * source = HTRequest_source(request);
		    HTNet * srcnet = HTRequest_net(source);
		    if (srcnet) {
			HTHost_register(HTNet_host(srcnet), srcnet, HTEvent_READ);
			HTHost_unregister(HTNet_host(srcnet), srcnet, HTEvent_WRITE);
		    }
		    return HT_OK;
		}

		/*
		**  Should we use the input stream directly or call the post
		**  callback function to send data down to the network?
		*/
		{
		    HTStream * input = HTRequest_inputStream(request);
		    HTPostCallback * pcbf = HTRequest_postCallback(request);
		    if (pcbf) {
			status = pcbf(request, input);
			if (status == HT_PAUSE || status == HT_LOADED)
			    type = HTEvent_READ;
		    } else {
			status = (*input->isa->flush)(input);
			type = HTEvent_READ;
		    }
		    if (status == HT_WOULD_BLOCK) return HT_OK;
		}
		status = request->PostCallback ?
                    request->PostCallback(request, request->input_stream) :
			(*request->input_stream->isa->flush)(request->input_stream);
 		if (status == HT_WOULD_BLOCK)
                    return HT_OK;
                else 	
                    type = HTEvent_READ;	  /* Trick to ensure that we do READ */
	    } else if (type == HTEvent_READ) {
                status = HTHost_read(HTNet_host(net), net);
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
	    HTNewsCleanup(request, HT_LOADED);
	    return HT_OK;
	    break;

	case NEWS_NO_DATA:
	    HTNewsCleanup(request, HT_NO_DATA);
	    return HT_OK;
	    break;

	  case NEWS_ERROR:
	    HTNewsCleanup(request, HT_NOT_FOUND);
	    return HT_OK;
	    break;
	}
    } /* End of while(1) */
}
