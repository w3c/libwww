/*									HTFTP.c
**	FILE TRANSFER PROTOCOL (FTP) CLIENT
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	A cache of control connections is kept.
**
** Note: Port allocation (not used anymore)
**
**	It is essential that the port is allocated by the system, rather
**	than chosen in rotation by us (FTP_POLL_PORTS), or the following
**	problem occurs.
**
**	It seems that an attempt by the server to connect to a port which has
**	been used recently by a listen on the same socket, or by another
**	socket this or another process causes a hangup of (almost exactly)
**	one minute. Therefore, we have to use a rotating port number.
**	The problem remains that if the application is run twice in quick
**	succession, it will hang for what remains of a minute.
**
** Authors
**	TBL	Tim Berners-lee <timbl@w3.org>
**	DD	Denis DeLaRoca 310 825-4580 <CSP1DWD@mvs.oac.ucla.edu>
**      LM      Lou Montulli <montulli@ukanaix.cc.ukans.edu>
**      FM      Foteos Macrides <macrides@sci.wfeb.edu>
**	HF	Henrik Frystyk <frystyk@w3.org>
**	AL	Ari Luotonen <luotonen@www.cern.ch>
**
** History:
**	 2 May 91	Written TBL, as a part of the WorldWideWeb project.
**	15 Jan 92	Bug fix: close() was used for NETCLOSE for control soc
**	10 Feb 92	Retry if cached connection times out or breaks
**	 8 Dec 92	Bug fix 921208 TBL after DD
**	17 Dec 92	Anon FTP password now just WWWuser@ suggested by DD
**			fails on princeton.edu!
**	27 Dec 93 (FM)  Fixed up so FTP now works with VMS hosts.  Path
**			must be Unix-style and cannot include the device
**			or top directory.
**      ?? ??? ?? (LM)  Added code to prompt and send passwords for non
**			anonymous FTP
**      25 Mar 94 (LM)  Added code to recognize different ftp server types
**                      and code to parse dates and sizes on most hosts.
**	27 Mar 93 (FM)  Added code for getting dates and sizes on VMS hosts.
**	27 Apr 94 (HF)  The module is basically rewritten to conform with
**			rfc 959, 1123 and 1579 and turned into a state 
**			machine. New semantics of ftp URLs are supported.
**	 2 May 94 (AL)	Fixed possible security hole when the URL contains
**			a newline, that could cause multiple commands to be
**			sent to an FTP server.
**	Sep 95	HFN	Rewritten to support streams and persistent conenctions
**			and multiplexed IO
**	Mar 1998 NG	Neil Griffin - Bug fixes and additions for FTP support on NT.
**      Jan 2000 JB     Joe Bester - Fixed the protocol bug that appeared after
**                      after the CERT advisory warning on wu. Added code
**                      to do an incremental streaming of FTP data.
**                       
** Notes:
**     			Portions Copyright 1994 Trustees of Dartmouth College
** 			Code for recognizing different FTP servers and
**			parsing "ls -l" output taken from Macintosh Fetch
**			program with permission from Jim Matthews,
**			Dartmouth Software Development Team.
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "WWWStream.h"
#include "WWWTrans.h"
#include "WWWFile.h"
#include "HTReqMan.h"
#include "HTNetMan.h"
#include "HTFTPDir.h"
#include "HTFTP.h"					 /* Implemented here */

#ifndef FTP_PORT
#define FTP_PORT	21
#define FTP_DATA	20
#endif

#define WWW_FTP_CLIENT "libwww@"         /* If can't get user-info, use this */
#define FTP_DIR(me)	((me)->type=='L' || (me)->type=='N')

/*
** Local context structure used in the HTNet object.
*/
typedef enum _HTFTPState {
    FTP_SUCCESS	= -2,
    FTP_ERROR = -1,
    FTP_BEGIN = 0,
    FTP_NEED_CCON,				       /* Control connection */
    FTP_NEED_LOGIN,
    FTP_NEED_DCON,					  /* Data connection */
    FTP_NEED_DATA,
    FTP_NEED_SERVER				   /* For directory listings */
} HTFTPState;

typedef struct _ftp_ctrl {
    HTChunk *		cmd;
    int			repcode;
    char *		reply;
    char *		uid;
    char *		passwd;
    char *		account;
    HTFTPState     	state;			  /* State of the connection */
    int 		substate;		  /* For hierarchical states */
    BOOL		sent;			    /* Read or write command */
    BOOL		cwd;					 /* Done cwd */
    BOOL		reset;			 	  /* Expect greeting */
    FTPServerType	server;		         	   /* Type of server */
    HTNet *		cnet;			       /* Control connection */
    HTNet *		dnet;			   	  /* Data connection */
    BOOL		alreadyLoggedIn;
} ftp_ctrl;

typedef struct _ftp_data {
    char		host[30];		 /* Host to contact for data */
    char *		file;				 /* File or dir name */
    char *		offset;				 /* offset into file */
    BOOL		pasv;				/* Active or passive */
    char 		type;		     /* 'A', 'I', 'L'(IST), 'N'(LST) */
    int			complete;   /* Check if both ctrl and data is loaded */
    BOOL		stream_error;
} ftp_data;

struct _HTStream {
    const HTStreamClass *	isa;
    HTStream *		  	target;
    HTRequest *			request;
    ftp_ctrl *			ctrl;
    HTEOLState			state;
    HTChunk *			welcome;
    BOOL			junk;		       /* For too long lines */
    BOOL			first_line;
    char 			buffer[MAX_FTP_LINE+1];
    int				buflen;
    HTHost *			host;
};

struct _HTInputStream {
    const HTInputStreamClass *	isa;
};

/* Determine whether we should use passive or active data TCP connections */
typedef enum _FTPDataCon {
    FTP_DATA_PASV = 0x1,
    FTP_DATA_PORT = 0x2
} FTPDataCon;

PRIVATE FTPDataCon FTPMode = FTP_DATA_PORT;

/* Added by Neil Griffin */
PRIVATE FTPTransferMode g_FTPTransferMode = FTP_DEFAULT_TRANSFER_MODE;
PRIVATE FTPControlMode 	g_FTPControlMode = FTP_DEFAULT_CONTROL_MODE;

/* ------------------------------------------------------------------------- */
/* 			    FTP Status Line Stream			     */
/* ------------------------------------------------------------------------- */

/*	FTPCleanup
**	----------
**      This function closes the connection and frees memory.
**      Returns YES on OK, else NO
*/
PRIVATE int FTPCleanup (HTRequest * request, int status)
{
    if (request) {
	HTNet * cnet = HTRequest_net(request);
	ftp_ctrl * ctrl = (ftp_ctrl *) HTNet_context(cnet);
	HTStream * input = HTRequest_inputStream(request);

        if (status == HT_INTERRUPTED) {
            HTAlertCallback * cbf = HTAlert_find(HT_PROG_INTERRUPT);
            if (cbf) (*cbf)(request, HT_PROG_INTERRUPT,
                HT_MSG_NULL, NULL, NULL, NULL);
	} else if (status == HT_TIMEOUT) {
            HTAlertCallback * cbf = HTAlert_find(HT_PROG_TIMEOUT);
            if (cbf) (*cbf)(request, HT_PROG_TIMEOUT,
                HT_MSG_NULL, NULL, NULL, NULL);
	} else if (status == HT_LOADED) {
            HTAlertCallback * cbf = HTAlert_find(HT_PROG_DONE);
            if (cbf) (*cbf)(request, HT_PROG_DONE,
                HT_MSG_NULL, NULL, NULL, NULL);
        }	        

	/* Free control stream with data TO network */
	if (!HTRequest_isDestination(request) && input) {
	    if (status == HT_INTERRUPTED)
		(*input->isa->abort)(input, NULL);
	    else
		(*input->isa->_free)(input);
	}
	
	/* Remove the request object and our own context structure for ftp */
	if (cnet && ctrl) {
	    HTNet * dnet = ctrl->dnet;
	    ftp_data * data = (ftp_data *) HTNet_context(dnet);
	    HTChunk_delete(ctrl->cmd);
	    HT_FREE(ctrl->reply);
	    HT_FREE(ctrl->uid);
	    HT_FREE(ctrl->passwd);
	    HT_FREE(ctrl->account);
	    HT_FREE(ctrl);
	    if (dnet && data) {
		HT_FREE(data->file);
		HT_FREE(data);
	    }

	    /* See if we got a content length */
            if (status == HT_LOADED)
                HTAnchor_setLength(HTRequest_anchor(request), HTNet_bytesRead(dnet));

	    /* Delete the data net object */
	    HTChannel_deleteInput(HTNet_channel(dnet), status);
	    HTChannel_deleteOutput(HTNet_channel(dnet), status);
	    HTNet_delete(dnet, HT_IGNORE);

	}
	HTNet_delete(cnet, status);
	return YES;
    }
    return NO;
}

/*	ScanResponse
**	------------
**	Analyzes the response from the FTP server.
**	Returns HT_LOADED if OK, HT_OK if more, HT_ERROR if error
**	the control connection.
*/
PRIVATE int ScanResponse (HTStream * me)
{
    int reply = 0;
    char cont = '\0';
    char *ptr = me->buffer+4;
    *(me->buffer+me->buflen) = '\0';
/* begin _GM_ */
/* Note: libwww bug ID: GM3 */
    me->ctrl->alreadyLoggedIn = NO;
/* end _GM_ */
    if (isdigit((int) *(me->buffer))) sscanf(me->buffer, "%d%c", &reply, &cont);
    if (me->first_line) {
	HTTRACE(PROT_TRACE, "FTP Rx...... `%s\'\n" _ me->buffer);
	if (!reply) return HT_ERROR;
	me->first_line = NO;
	me->ctrl->repcode = reply;
	StrAllocCopy(me->ctrl->reply, ptr);
/* begin _GM_ */
/* Note: libwww bug ID: GM3 */
	if ( (reply == 530) && (HTStrCaseStr(me->buffer, "already") != NULL) ) {
	    me->ctrl->alreadyLoggedIn = YES;
	} else {
	    me->ctrl->alreadyLoggedIn = NO;
	}
/* end _GM_ */
    } else {
	HTChunk_puts(me->welcome, ptr);
	HTChunk_putc(me->welcome, '\n');
    }
    me->buflen = 0;
    me->state = EOL_BEGIN;
    if (cont != '-') {
	me->first_line = YES;
	return HT_LOADED;
    }
    return HT_OK;
}

/*
**	Searches for FTP header line until buffer fills up or a CRLF or LF
**	is found
*/
PRIVATE int FTPStatus_put_block (HTStream * me, const char * b, int l)
{
    int status;
    HTHost_setConsumed(me->host, l);
    while (l-- > 0) {
	if (me->state == EOL_FCR) {
	    if (*b == LF) {
		if (!me->junk) {
		    if ((status = ScanResponse(me)) != HT_OK) return status;
		} else {
		    me->buflen = 0;		
		    me->junk = NO;
		}
	    }
	} else if (*b == CR) {
	    me->state = EOL_FCR;
	} else if (*b == LF) {
	    if (!me->junk) {
		if ((status = ScanResponse(me)) != HT_OK) return status;
	    } else {
		me->buflen = 0;		
		me->junk = NO;
	    }
	} else {
	    *(me->buffer+me->buflen++) = *b;
	    if (me->buflen >= MAX_FTP_LINE) {
		HTTRACE(PROT_TRACE, "FTP Status.. Line too long - chopped\n");
		me->junk = YES;
		if ((status = ScanResponse(me)) != HT_OK) {
		    me->junk = NO;
		    return status;	
		}
	    }
	}
	b++;
    }
    return HT_OK;
}

PRIVATE int FTPStatus_put_string (HTStream * me, const char * s)
{
    return FTPStatus_put_block(me, s, (int) strlen(s));
}

PRIVATE int FTPStatus_put_character (HTStream * me, char c)
{
    return FTPStatus_put_block(me, &c, 1);
}

PRIVATE int FTPStatus_flush (HTStream * me)
{
    return (*me->target->isa->flush)(me->target);
}

PRIVATE int FTPStatus_free (HTStream * me)
{
    int status = HT_OK;
    if (me->target) {
	if ((status = (*me->target->isa->_free)(me->target)) == HT_WOULD_BLOCK)
	    return HT_WOULD_BLOCK;
    }
    HTChunk_delete(me->welcome);
    HT_FREE(me);
    return HT_OK;
}

PRIVATE int FTPStatus_abort (HTStream * me, HTList * e)
{
    if (me->target)
	(*me->target->isa->abort)(me->target, e);
    HTChunk_delete(me->welcome);
    HT_FREE(me);
    HTTRACE(PROT_TRACE, "FTPStatus... ABORTING...\n");
    return HT_ERROR;
}

/*	FTPStatus Stream
**	-----------------
*/
PRIVATE const HTStreamClass FTPStatusClass =
{		
    "FTPStatus",
    FTPStatus_flush,
    FTPStatus_free,
    FTPStatus_abort,
    FTPStatus_put_character,
    FTPStatus_put_string,
    FTPStatus_put_block
};

PRIVATE HTStream * FTPStatus_new (HTRequest * request, ftp_ctrl * ctrl, HTHost * host)
{
    HTStream * me;
    if ((me = (HTStream  *) HT_CALLOC(1, sizeof(HTStream))) == NULL)
        HT_OUTOFMEM("FTPStatus_new");
    me->isa = &FTPStatusClass;
    me->request = request;
    me->first_line = YES;
    me->welcome = HTChunk_new(256);
    me->ctrl = ctrl;
    me->state = EOL_BEGIN;
    me->host = host;
    return me;
}

/* ------------------------------------------------------------------------- */
/*	  FTP Client Functions for managing control and data connections     */
/* ------------------------------------------------------------------------- */

PRIVATE int SendCommand (HTRequest *request, ftp_ctrl *ctrl,
			 char *token, char *pars)
{
    int len = strlen(token) + (pars ? strlen(pars)+1:0) + 2;
    HTStream * input = HTRequest_inputStream(request);
    HTChunk_setSize(ctrl->cmd, len);
    if (pars && *pars)
	sprintf(HTChunk_data(ctrl->cmd), "%s %s%c%c", token, pars, CR, LF);
    else
	sprintf(HTChunk_data(ctrl->cmd), "%s%c%c", token, CR, LF);
    HTTRACE(PROT_TRACE, "FTP Tx...... %s" _ HTChunk_data(ctrl->cmd));
    return (*input->isa->put_block)(input, HTChunk_data(ctrl->cmd), len);
}

/*	HTFTPParseURL
**	-------------
**    	Scan URL for uid and passwd, and any data type indication. The
**	expected format is [user[:password]@]host[:port].
**	If no values are found then use defaults.
**	Returns YES if OK, else NO
*/
PRIVATE BOOL HTFTPParseURL (HTRequest * request,
			    char *url, ftp_ctrl *ctrl, ftp_data *data)
{
    char *login = HTParse(url, "", PARSE_HOST);
    char *path = HTParse(url, "", PARSE_PATH+PARSE_PUNCTUATION);
    char *ptr = strchr(login, '@');
    if (ptr) {				      /* Uid and/or passwd specified */
	char *passwd;
	*ptr = '\0';
	if ((passwd = strchr(login, ':'))) {		 /* Passwd specified */
	    *passwd++ = '\0';    
	    HTUnEscape(passwd);
	    StrAllocCopy(ctrl->passwd, passwd);
	}
	HTUnEscape(login);
	StrAllocCopy(ctrl->uid, login);
    } else if (g_FTPControlMode & FTP_ALWAYS_ASK_UID_PW) {
	ctrl->uid=NULL;
	ctrl->passwd=NULL;
    } else {				    /* Use anonymous */
	HTUserProfile * up = HTRequest_userProfile(request);
        const char * mailaddress = HTUserProfile_email(up);
	StrAllocCopy(ctrl->uid, "anonymous");
	if (mailaddress)
	    StrAllocCopy(ctrl->passwd, mailaddress);
	else
	    StrAllocCopy(ctrl->passwd, WWW_FTP_CLIENT);
    }
    HTTRACE(PROT_TRACE, "FTPParse.... uid `%s\' pw `%s\'\n" _ 
	    ctrl->uid ? ctrl->uid : "<null>" _ 
	    ctrl->passwd ? ctrl->passwd : "<null>");	

    /*
    ** Look for any type in the URI. If not 'type' parameter then look for
    ** trailing slash.
    */
    if ((ptr = strchr(path, ';')) != NULL) {
	*ptr = '\0';
	if (strncasecomp(ptr, ";type=", 6))		    /* Look for type */
	    data->type = TOUPPER(*(ptr+6));
	else if (*(ptr-1) == '/')
	    data->type = 'D';
    } else if (*(path+strlen(path)-1) == '/') {
	*(path+strlen(path)-1) = '\0';
	data->type = 'D';
    }
    HTTRACE(PROT_TRACE, "FTPParse.... Datatype %c\n" _ data->type ? data->type : '?');
    StrAllocCopy(data->file, path);
    data->offset = data->file;
    HT_FREE(login);
    HT_FREE(path);
    return YES;
}

/*	Use LIST or NLST
**	----------------
**	This function sets the type field for what type of list we can use
**	Returns YES if OK, else NO
*/
PRIVATE BOOL FTPListType (ftp_data * data, FTPServerType type)
{
    if (!data) return NO;
    switch (type) {
      case FTP_GENERIC: 	data->type='N'; break;
      case FTP_MACHTEN: 	data->type='L'; break;
      case FTP_UNIX:		data->type='L'; break;
      case FTP_VMS:		data->type='L'; break;
      case FTP_CMS: 		data->type='N'; break;
      case FTP_DCTS: 		data->type='N'; break;
      case FTP_TCPC: 		data->type='N'; break;
      case FTP_PETER_LEWIS:	data->type='L'; break;
      case FTP_NCSA:		data->type='N'; break;
      case FTP_WINNT:		data->type='L'; break;
      default: 			data->type='N'; break;
    }
    return YES;
}

/*	Open a Data socket for listening on
**	-----------------------------------
**	Set up a port to listen for data
**	Returns YES if OK, else NO
*/
PRIVATE BOOL AcceptDataSocket (HTNet *cnet, HTNet *dnet, ftp_data *data)
{
    if (HTHost_listen(NULL, dnet, "ftp://localhost:0") == HT_ERROR)
	return NO;

    /*
    ** Now we must find out who we are to tell the other guy
    ** We have to get the local IP interface from the control connection as
    ** this is not yet set in the unaccepted data socket
    */
    {
	SockA local_port, local_host;
	int addr_size = sizeof(local_port);
	memset((void *) &local_host, '\0', addr_size);
	memset((void *) &local_port, '\0', addr_size);
	if (getsockname(HTNet_socket(cnet),
			(struct sockaddr *) &local_host, &addr_size) < 0 || 
	    getsockname(HTNet_socket(dnet),
			(struct sockaddr *) &local_port, &addr_size) < 0) {
	    HTRequest_addSystemError(HTNet_request(dnet), ERR_FATAL, socerrno,
				     NO, "getsockname");
	    return NO;
	}
	HTTRACE(PROT_TRACE, "FTP......... This host is `%s\'\n" _ HTInetString(&local_host));
	{
	    u_long addr = local_host.sin_addr.s_addr;
	    u_short port = local_port.sin_port;
	    sprintf(data->host, "%d,%d,%d,%d,%d,%d",
		    (int)*((unsigned char *)(&addr)+0),
		    (int)*((unsigned char *)(&addr)+1),
		    (int)*((unsigned char *)(&addr)+2),
		    (int)*((unsigned char *)(&addr)+3),
		    (int)*((unsigned char *)(&port)+0),
		    (int)*((unsigned char *)(&port)+1));
	}
    }
    return YES;
}

/*	HTFTPLogin
**	-----------
**    	This function makes a login to a ftp-server. It takes the user name
**	and passwd specified in ctrl->user and if that fails or an additional
**	account is needed, the user is prompted.
**	Returns HT_OK, HT_ERROR, or HT_WOULD_BLOCK
*/
PRIVATE int HTFTPLogin (HTRequest *request, HTNet *cnet, ftp_ctrl *ctrl)
{
    int status;
    typedef enum _state {
	SUB_ERROR = -2,
	SUB_SUCCESS = -1,
	NEED_SELECT = 0,
	NEED_GREETING,
	NEED_REIN,
	NEED_UID,
	NEED_PASSWD,
	NEED_ACCOUNT,
	PROMPT_USER
    } state;

    /* Jump into a second level state machine */
    while (1) {
	switch ((state) ctrl->substate) {
	case NEED_SELECT:
	    {
		HTAlertCallback * cbf = HTAlert_find(HT_PROG_LOGIN);
		if (cbf) (*cbf)(request, HT_PROG_LOGIN, HT_MSG_NULL, NULL, NULL, NULL);
		HTTRACE(PROT_TRACE, "FTP Login... now in state NEED_SELECT\n");
		ctrl->substate = ctrl->reset ? NEED_REIN : NEED_GREETING;
	    }
	    break;

	  case NEED_GREETING:
	    HTTRACE(PROT_TRACE, "FTP Login... now in state NEED_GREETING\n");
	    status = HTHost_read(HTNet_host(cnet), cnet);
	    if (status == HT_WOULD_BLOCK)
		return HT_WOULD_BLOCK;
	    else if (status == HT_LOADED) {
		if (ctrl->repcode/100 == 2) {
		    ctrl->substate = (ctrl->uid && *ctrl->uid) ?
			NEED_UID : PROMPT_USER;
		} else {
		    ctrl->substate = SUB_ERROR;
		}
	    } else {
		ctrl->substate = SUB_ERROR;
	    }
	    break;

	  case NEED_REIN:
	    HTTRACE(PROT_TRACE, "FTP Login... now in state NEED_REIN\n");
	    if (!ctrl->sent) {
		status = SendCommand(request, ctrl, "REIN", NULL);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_ERROR)
		    ctrl->substate = SUB_ERROR;
		ctrl->sent = YES;
	    } else {
		status = HTHost_read(HTNet_host(cnet), cnet);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_LOADED) {
		    /* 
		    ** If the FTP server doesn't support the REIN command, then
		    ** the return code will be 502
		    */
		    if ((ctrl->repcode/100 == 2) || (ctrl->repcode == 502)) {
			ctrl->substate = (ctrl->uid && *ctrl->uid) ?
			    NEED_UID : PROMPT_USER;
		    } else {
			ctrl->substate = SUB_SUCCESS;	    /* hope the best */
		    }
		} else {
		    ctrl->substate = SUB_ERROR;
		}
		ctrl->sent = NO;
	    }
	    break;

	  case NEED_UID:
	    HTTRACE(PROT_TRACE, "FTP Login... now in state NEED_UID\n");
	    if (!ctrl->sent) {
		status = SendCommand(request, ctrl, "USER", ctrl->uid);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_ERROR)
		    ctrl->substate = SUB_ERROR;
		ctrl->sent = YES;
	    } else {
		status = HTHost_read(HTNet_host(cnet), cnet);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_LOADED) {
		    int code = ctrl->repcode/100;
		    if (code == 2) 		    /* Logged in w/o passwd! */
			ctrl->substate = SUB_SUCCESS;
		    else if (code == 3) {	 	/* Password demanded */
			ctrl->substate = (ctrl->passwd && *ctrl->passwd) ?
			    NEED_PASSWD : PROMPT_USER;
/* begin _GM_ */
/* Note: libwww bug ID: GM3 */
		    /* } else if (ctrl->repcode == 530) */
			/* ctrl->substate = PROMPT_USER;*/        /* User unknown */
		    } else if (ctrl->repcode == 530) {
			if (ctrl->alreadyLoggedIn == YES) {
			    ctrl->substate = SUB_SUCCESS;
			    HTTRACE(PROT_TRACE, "FTP Login... Already logged in\n");
			} else {
			    ctrl->substate = PROMPT_USER;
			    HTTRACE(PROT_TRACE, "FTP Login... User Unknown\n");
			}
		    }
/* end _GM_ */
		    else
			ctrl->substate = SUB_ERROR;
		} else
		    ctrl->substate = SUB_ERROR;
		ctrl->sent = NO;
	    }
	    break;

	  case NEED_PASSWD:
	    HTTRACE(PROT_TRACE, "FTP Login... now in state NEED_PASSWD\n");
	    if (!ctrl->sent) {
		status = SendCommand(request, ctrl, "PASS", ctrl->passwd);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_ERROR)
		    ctrl->substate = SUB_ERROR;
		ctrl->sent = YES;
	    } else {
		status = HTHost_read(HTNet_host(cnet), cnet);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_LOADED) {
		    int code = ctrl->repcode/100;
		    if (code == 2)		    /* Logged in with passwd */
			ctrl->substate = SUB_SUCCESS;
		    else if (code == 3) {      		 /* Account required */
			HTAlertCallback *cbf = HTAlert_find(HT_A_PROMPT);
			HTAlertPar * reply = HTAlert_newReply();
			if (cbf && (*cbf)(request, HT_A_PROMPT,
					  HT_MSG_ACCOUNT, NULL, NULL, reply)) {
			    ctrl->account = HTAlert_replyMessage(reply);
			    ctrl->substate = NEED_ACCOUNT;
			} else
			    ctrl->substate = SUB_ERROR;
			HTAlert_deleteReply(reply);
		    } else if (ctrl->repcode == 530)
			ctrl->substate = PROMPT_USER;
		    else
			ctrl->substate = SUB_ERROR;
		} else
		    ctrl->substate = SUB_ERROR;
		ctrl->sent = NO;
	    }
	    break;

	  case NEED_ACCOUNT:
	    HTTRACE(PROT_TRACE, "FTP Login... now in state NEED_ACCOUNT\n");
	    if (!ctrl->sent) {
		status = SendCommand(request, ctrl, "ACCT", ctrl->account);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_ERROR)
		    ctrl->substate = SUB_ERROR;
		ctrl->sent = YES;
	    } else {
		status = HTHost_read(HTNet_host(cnet), cnet);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_LOADED) {
		    int code = ctrl->repcode/100;
		    if (code == 2)		   /* Logged in with account */
			ctrl->substate = SUB_SUCCESS;
		    else
			ctrl->substate = SUB_ERROR;		 /* hopeless */
		} else
		    ctrl->substate = SUB_ERROR;
		ctrl->sent = NO;
	    }
	    break;

	  case PROMPT_USER:
	    HTTRACE(PROT_TRACE, "FTP Login... now in state PROMPT_USER\n");
	    {
		HTAlertCallback *cbf = HTAlert_find(HT_A_USER_PW);
		HTAlertPar * reply = HTAlert_newReply();
		HT_FREE(ctrl->uid);
		HT_FREE(ctrl->passwd);
		if (cbf && (*cbf)(request, HT_A_USER_PW, HT_MSG_FTP_UID,
				  NULL, NULL, reply)){
		    ctrl->uid = HTAlert_replyMessage(reply);
		    ctrl->passwd = HTAlert_replySecret(reply);
		}
		else {
		  ctrl->uid = NULL;
		  ctrl->passwd = NULL;
		}
		HTAlert_deleteReply(reply);
		if (ctrl->uid && *ctrl->uid && ctrl->passwd && *ctrl->passwd)
		    ctrl->substate = NEED_UID;
		else
		    ctrl->substate = SUB_ERROR;
	    }
	    break;

	  case SUB_ERROR:
	    HTTRACE(PROT_TRACE, "FTP Login... now in state SUB_ERROR\n");
	    HTRequest_addError(request, ERR_FATAL, NO,
			       HTERR_FTP_LOGIN_FAILURE, NULL, 0, "HTFTPLogin");
	    HTTRACE(PROT_TRACE, "FTP......... Login failed\n");
	    ctrl->substate = 0;
	    return HT_ERROR;
	    break;

	  case SUB_SUCCESS:
	    HTTRACE(PROT_TRACE, "FTP Login... now in state SUB_SUCCESS\n");
	    HTTRACE(PROT_TRACE, "FTP......... Logged in as `%s\'\n" _ ctrl->uid);
	    ctrl->substate = 0;
	    return HT_OK;
	    break;
	}
    }
}

/*	HTFTPDataConnection
**	-------------------
**    	Prepares a data connection to the server and initializes the
**	transfer mode.
**	Returns HT_OK, HT_ERROR, or HT_WOULD_BLOCK
*/
PRIVATE int HTFTPDataConnection (HTRequest * request, HTNet *cnet,
				 ftp_ctrl *ctrl, ftp_data *data)
{
    int status;
    HTNet *dnet = ctrl->dnet;
    typedef enum _state {
	SUB_ERROR = -2,
	SUB_SUCCESS = -1,
	NEED_TYPE = 0,
	NEED_SELECT,
	NEED_PASV,
	NEED_PORT
    } state;
    
    /* Jump into a second level state machine */
    while (1) {
	switch ((state) ctrl->substate) {
	  case NEED_TYPE:
	    HTTRACE(PROT_TRACE, "FTP Data.... now in state NEED_TYPE\n");
	    if(!data->type|| data->pasv || data->type=='N' || data->type=='L'){
		ctrl->substate = NEED_SELECT;
		break;
	    }
	    if (!ctrl->sent) {
		char type[2];
		*type = data->type;
		*(type+1) = '\0';
		status = SendCommand(request, ctrl, "TYPE", type);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_ERROR)
		    ctrl->substate = SUB_ERROR;
		ctrl->sent = YES;
	    } else {
		status = HTHost_read(HTNet_host(cnet), cnet);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_LOADED) {
		    if (ctrl->repcode/100 == 2)
			ctrl->substate = NEED_SELECT;
		    else
			ctrl->substate = SUB_ERROR;
		} else
		    ctrl->substate = SUB_ERROR;
		ctrl->sent = NO;
	    }
	    break;
	    
	  case NEED_SELECT:
	    HTTRACE(PROT_TRACE, "FTP Data.... now in state NEED_SELECT\n");
	    if (FTPMode & FTP_DATA_PASV && !data->pasv)
		ctrl->substate = NEED_PASV;
	    else if (AcceptDataSocket(cnet, dnet, data))
		ctrl->substate = NEED_PORT;
	    else
		ctrl->substate = SUB_ERROR;
	    break;

	  case NEED_PASV:
	    HTTRACE(PROT_TRACE, "FTP Data.... now in state NEED_PASV\n");
	    if (!ctrl->sent) {
		status = SendCommand(request, ctrl, "PASV", NULL);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_ERROR)
		    ctrl->substate = SUB_ERROR;
		ctrl->sent = YES;
	    } else {
		status = HTHost_read(HTNet_host(cnet), cnet);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_LOADED) {
		    if (ctrl->repcode == 227) {		    
			/*
			** If succes, we have to scan for the returned number.
			** As the format for the response isn't standard,
			** the best thing to do is to scan for the first digit
			** after the status code, see RFC1123
			*/
			char *host = ctrl->reply;
			int h0, h1, h2, h3, p0=0, p1=0;
			while (*host && !isdigit((int) *host++));
			if (!*host || sscanf(--host, "%d,%d,%d,%d,%d,%d",
					     &h0,&h1,&h2,&h3,&p0,&p1) < 6) {
			    HTTRACE(PROT_TRACE, "FTP Data.... PASV No addr\n");
			    ctrl->substate = SUB_ERROR;
			    break;
			} else {
			    int port = (p0<<8)+p1;
			    sprintf(data->host, "ftp://%d.%d.%d.%d:%d/",
				    h0, h1, h2, h3, port);
			    data->pasv = YES;
			    ctrl->substate = SUB_SUCCESS;
			}
		    } else {
			ctrl->substate = AcceptDataSocket(cnet, dnet, data) ?
			    NEED_PORT : SUB_ERROR;
		    }
		} else
		    ctrl->substate = SUB_ERROR;
		ctrl->sent = NO;
	    }
	    break;

	  case NEED_PORT:
	    HTTRACE(PROT_TRACE, "FTP Data.... now in state NEED_PORT\n");
	    if (!ctrl->sent) {
		status = SendCommand(request, ctrl, "PORT", data->host);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_ERROR)
		    ctrl->substate = SUB_ERROR;
		ctrl->sent = YES;
	    } else {
		status = HTHost_read(HTNet_host(cnet), cnet);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_LOADED) {
		    data->pasv = NO;
		    ctrl->substate = (ctrl->repcode/100 == 2) ?
			SUB_SUCCESS : SUB_ERROR;
		} else
		    ctrl->substate = SUB_ERROR;
		ctrl->sent = NO;
	    }
	    break;

	  case SUB_ERROR:
	    HTTRACE(PROT_TRACE, "FTP Data.... now in state SUB_ERROR\n");
	    HTTRACE(PROT_TRACE, "FTP Data.... Can't setup data connection\n");
	    ctrl->substate = 0;
	    return HT_ERROR;
	    break;

	  case SUB_SUCCESS:
	    HTTRACE(PROT_TRACE, "FTP Data.... now in state SUB_SUCCESS\n");
	    HTTRACE(PROT_TRACE, "FTP Data.... Data connection negotiated\n");
	    ctrl->substate = 0;
	    return HT_OK;
	    break;
	}
    }
}


/*	HTFTPServerInfo
**	---------------
**	This function finds out what server we are talking to.
**	Maybe we can upgrade from NLST to LIST.
**	Returns HT_OK, HT_ERROR, or HT_WOULD_BLOCK
**	Thanks to James.W.Matthews@Dartmouth.EDU (James W. Matthews) for making
**	his code available.
*/
PRIVATE int HTFTPServerInfo (HTRequest *request, HTNet *cnet,
			     ftp_ctrl *ctrl, ftp_data *data)
{
    int status;
    typedef enum _state {
	SUB_ERROR = -2,
	SUB_SUCCESS = -1,
	NEED_SYST = 0,
	CHECK_SYST,
	NEED_PWD,
	CHECK_PWD
    } state;

    /* Jump into a second level state machine */
    while (1) {
	switch ((state) ctrl->substate) {
	  case NEED_SYST:
	    HTTRACE(PROT_TRACE, "FTP Server.. now in state NEED_SYST\n");
	    if (!ctrl->sent) {		
		if (ctrl->server != FTP_UNSURE) {
		    FTPListType(data, ctrl->server);
		    return HT_OK;
		}
		status = SendCommand(request, ctrl, "SYST", NULL);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_ERROR)
		    ctrl->substate = SUB_ERROR;
		ctrl->sent = YES;
	    } else {
		status = HTHost_read(HTNet_host(cnet), cnet);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_LOADED) {
		    ctrl->substate=ctrl->repcode==215 ? CHECK_SYST : NEED_PWD;
		} else
		    ctrl->substate = SUB_ERROR;
		ctrl->sent = NO;
	    }
	    break;

	  case CHECK_SYST:
	    HTTRACE(PROT_TRACE, "FTP Server.. now in state CHECK_SYST\n");
	    {
		char *reply = ctrl->reply;
		if (!*reply) {
		    HTTRACE(PROT_TRACE, "FTP Server.. No server info?\n");
		    ctrl->substate = NEED_PWD;
		    break;
		}
		if (strncmp(reply, "UNIX Type: L8MAC-OSMachTen", 28) == 0) {
		    ctrl->server = FTP_MACHTEN;
		} else if (strstr(reply, "UNIX") != NULL) {
		    ctrl->server = FTP_UNIX;
		} else if (strncmp(reply, "VMS", 3) == 0) {
		    ctrl->server = FTP_VMS;
		} else if ((strncmp(reply, "VM/CMS", 6) == 0) ||
			   (strncmp(reply, "VM", 2) == 0)) {
		    ctrl->server = FTP_CMS;
		} else if (strncmp(reply, "DCTS", 4) == 0) {
		    ctrl->server = FTP_DCTS;
		} else if (strstr(reply, "MAC-OS TCP/ConnectII") != NULL) {
		    /* Check old versions of TCP/C using / in pathnames */
		    ctrl->server = FTP_TCPC + FTP_UNSURE;
		} else if (strncmp(reply, "MACOS Peter's Server", 20) == 0) {
		    ctrl->server = FTP_PETER_LEWIS;
		} else if (strncmp(reply, "Windows_NT", 10) == 0) {
		    ctrl->server = FTP_WINNT;
		}
		
		/* If we are unsure, try PWD to get more information */
		if (ctrl->server & FTP_UNSURE)
		    ctrl->substate = NEED_PWD;
		else
		    ctrl->substate = SUB_SUCCESS;
	    }
	    break;

	  case NEED_PWD:
	    HTTRACE(PROT_TRACE, "FTP Server.. now in state NEED_PWD\n");
	    if (!ctrl->sent) {
		status = SendCommand(request, ctrl, "PWD", NULL);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_ERROR)
		    ctrl->substate = SUB_ERROR;
		ctrl->sent = YES;
	    } else {
		status = HTHost_read(HTNet_host(cnet), cnet);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_LOADED) {
		    ctrl->substate = (ctrl->repcode/100 == 2) ?
			CHECK_PWD : SUB_ERROR;
		} else
		    ctrl->substate = SUB_ERROR;
		ctrl->sent = NO;
	    }
	    break;

	  case CHECK_PWD:
	    HTTRACE(PROT_TRACE, "FTP Server.. now in state CHECK_PWD\n");
	    {
		char *start = strchr(ctrl->reply, '"');
		char *end;
		if (!start || (end = strchr(++start, '"')) == NULL) {
		    HTTRACE(PROT_TRACE, "FTP Server.. No current directory?\n");
		    ctrl->server = FTP_GENERIC;
		} else {
		    *end = '\0';
		    if (ctrl->server & FTP_TCPC) {
			ctrl->server = *start == '/' ? FTP_NCSA : FTP_TCPC;
		    } else if (*start == '/') {
			/* path names starting with / imply Unix, right? */
			ctrl->server = FTP_UNIX;
		    } else if (*(end-1) == ']') {
			/* path names ending with ] imply VMS, right? */
			ctrl->server = FTP_VMS;
		    } else
			ctrl->server = FTP_GENERIC;
		}
		ctrl->substate = SUB_SUCCESS;
	    }
	    break;

	  case SUB_ERROR:
	    HTTRACE(PROT_TRACE, "FTP Server.. now in state SUB_ERROR\n");
	    HTTRACE(PROT_TRACE, "FTP Server.. Can't get server information\n");
	    ctrl->substate = 0;
	    ctrl->server = FTP_GENERIC;
	    return HT_ERROR;
	    break;

	  case SUB_SUCCESS:
	    HTTRACE(PROT_TRACE, "FTP Server.. now in state SUB_SUCCESS\n");
	    {
	      HTHost * host = HTNet_host(cnet);
	      HTTRACE(PROT_TRACE, "FTP Server.. Guessed type %d\n" _ ctrl->server);
	      HTHost_setVersion(host, ctrl->server);
	      FTPListType(data, ctrl->server);
	      ctrl->substate = 0;
	      return HT_OK;
	      break;
	    }
	}
    }
}

/*	HTFTPGetData
**	------------
**    	This function asks for the file or a directory. First we try in one go,
**	but if that doesn't work, then we use CWD for each segment and then
**	try to retrieve it. If that also fails, then we try if it is a
**	directory.
**	Returns HT_OK, HT_LOADED, HT_ERROR, or HT_WOULD_BLOCK
*/
PRIVATE int HTFTPGetData (HTRequest *request, HTNet *cnet, SOCKET sockfd,
			  ftp_ctrl *ctrl, ftp_data *data)
{
    int status;
    char *segment = NULL;
    HTNet *dnet = ctrl->dnet;
    BOOL data_is_active = (sockfd == HTNet_socket(dnet));
    HTPostCallback *pcbf;
    typedef enum _state {
	SUB_ERROR = -2,
	SUB_SUCCESS = -1,
	NEED_SELECT = 0,
	NEED_CONNECT,
	NEED_ACCEPT,
	NEED_ACTION,
        NEED_CWD,
	NEED_SEGMENT,
	NEED_STREAM,
	NEED_BODY
    } state;

    /* Jump into a second level state machine */
    while (1) {
	switch ((state) ctrl->substate) {
	  case NEED_SELECT:
	    HTTRACE(PROT_TRACE, "FTP Get Data now in state NEED_SELECT\n");
	    ctrl->substate = data->pasv ? NEED_CONNECT : NEED_ACTION;
	    break;

	  case NEED_CONNECT:
	    HTTRACE(PROT_TRACE, "FTP Get Data now in state NEED_CONNECT\n");
	    status = HTHost_connect(HTNet_host(dnet), dnet, data->host);
	    if (status == HT_WOULD_BLOCK)
		return HT_WOULD_BLOCK;
	    else if (status == HT_OK) {
		HTTRACE(PROT_TRACE, "FTP Get Data Active data socket %d\n" _ 
			    HTNet_socket(dnet));
		ctrl->substate = NEED_ACTION;
	    } else {			 	  /* Swap to PORT on the fly */
		NETCLOSE(HTNet_socket(dnet));
		HTNet_setSocket(dnet, INVSOC);
		HTTRACE(PROT_TRACE, "FTP Get Data Swap to PORT on the fly\n");
		ctrl->substate = NEED_SELECT;
		HT_FREE(segment);
		return HT_OK;
	    }
	    break;

	  case NEED_ACCEPT:
	    HTTRACE(PROT_TRACE, "FTP Get Data now in state NEED_ACCEPT\n");
	    status = HTHost_accept(HTNet_host(dnet), dnet, NULL);
	    if (status == HT_WOULD_BLOCK)
		return HT_WOULD_BLOCK;
	    else if (status == HT_OK) {
		HTTRACE(PROT_TRACE, "FTP Get Data Passive data socket %d\n" _
			HTNet_socket(dnet));
		ctrl->substate = NEED_STREAM;
	    } else
		ctrl->substate = SUB_ERROR;
	    break;

	  case NEED_ACTION:
	    HTTRACE(PROT_TRACE, "FTP Get Data now in state NEED_ACTION\n");
	    if (!ctrl->sent) {
		char *cmd = (data->type=='L') ? "LIST" :
		    (data->type=='N') ? "NLST" : "RETR";
	        if (HTRequest_method(request) == METHOD_PUT) cmd = "STOR";
		StrAllocCopy(segment, data->offset);
		HTUnEscape(segment);
		HTCleanTelnetString(segment);
		status = SendCommand(request, ctrl, cmd, segment);
		HT_FREE(segment);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_ERROR)
		    ctrl->substate = SUB_ERROR;
		ctrl->sent = YES;
	    } else {
		status = HTHost_read(HTNet_host(cnet), cnet);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_LOADED) {
		    int code = ctrl->repcode;
		    if (code==125 || code==150 || code==225)
			ctrl->substate = data->pasv ? NEED_STREAM : NEED_ACCEPT;
		    else if (code/100==5 && !ctrl->cwd)
			ctrl->substate = NEED_SEGMENT;
		    else {
			if (ctrl->repcode == 550) {
			    HTTRACE(PROT_TRACE, "FTP Get Data no such file or directory\n");
			    data->stream_error = YES;
			}
		    	ctrl->substate = SUB_ERROR;
		    }
		} else
		    ctrl->substate = SUB_ERROR;
		ctrl->sent = NO;
	    }
	    break;

	  case NEED_SEGMENT:
	    HTTRACE(PROT_TRACE, "FTP Get Data now in state NEED_SEGMENT\n");
	    {
		char *ptr;
		if (data->offset == data->file) {
		    if (ctrl->server == FTP_VMS) {	   /* Change to root */
			if ((segment = (char  *) HT_MALLOC(strlen(ctrl->uid)+3)) == NULL)
			    HT_OUTOFMEM("segment ");
			sprintf(segment, "[%s]", ctrl->uid);
		    } else
			StrAllocCopy(segment, "/");
		    data->offset++;
		    ctrl->substate = NEED_CWD;
		} else {
		    if ((ptr = strchr(data->offset, '/'))) {
			*ptr='\0';
			StrAllocCopy(segment, data->offset);
			*ptr='/';
			data->offset = ++ptr;
			HTUnEscape(segment);
			HTCleanTelnetString(segment);
			ctrl->substate = NEED_CWD;
		    } else
			ctrl->substate = NEED_ACTION;
		}
	    }
	    break;

	  case NEED_CWD:
	    HTTRACE(PROT_TRACE, "FTP Get Data now in state NEED_CWD\n");
	    if (!ctrl->sent) {
		status = SendCommand(request, ctrl, "CWD", segment);
		HT_FREE(segment);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_ERROR)
		    ctrl->substate = SUB_ERROR;
		ctrl->cwd = YES;
		ctrl->sent = YES;
	    } else {
		status = HTHost_read(HTNet_host(cnet), cnet);
		if (status == HT_WOULD_BLOCK)
		    return HT_WOULD_BLOCK;
		else if (status == HT_LOADED) {
		    if (ctrl->repcode/100 == 2)
			ctrl->substate = NEED_SEGMENT;
		    else
			ctrl->substate = SUB_ERROR;
		} else
		    ctrl->substate = SUB_ERROR;
		ctrl->sent = NO;
	    }
	    break;

	case NEED_STREAM:
	    HTTRACE(PROT_TRACE, "FTP Get Data now in state NEED_STREAM\n");
	    /* 
	    ** Create the stream pipe FROM the channel to the application.
	    ** The target for the input stream pipe is set up using the
	    ** stream stack.
	    */
	    {
		HTStream * target = FTP_DIR(data) ?
		    HTFTPDir_new(request, ctrl->server, data->type) :
		    HTStreamStack(HTAnchor_format(HTRequest_anchor(request)),
				  HTRequest_outputFormat(request),
				  HTRequest_outputStream(request),
				  request, YES);
		HTNet_setReadStream(dnet, target);
		HTRequest_setOutputConnected(request, YES);
	    }
	    data_is_active = YES;
	    ctrl->substate = NEED_BODY;
	    break;

	  case NEED_BODY:
	      HTTRACE(PROT_TRACE, "FTP Get Data now in state NEED_BODY\n");
	      if (data_is_active) {
		  if (HTRequest_method(request) == METHOD_PUT) {
		      HTParentAnchor * entity = HTRequest_entityAnchor(request);
		      const char * document = (const char *) HTAnchor_document(entity);
		      int length = (int)HTAnchor_length(entity);
		      HTStream * output = 
			  (HTStream *)HTChannel_output(HTNet_host(dnet)->channel);
		      pcbf = HTRequest_postCallback(request);
		      if (pcbf) {
			status = (*pcbf)(request, output);
		      } else {
			status = (*output->isa->put_block)(output,
							   document,
							   length);
			if (status == HT_OK) {
			  status = HT_LOADED;
			}
		      }

		      if (status == HT_WOULD_BLOCK) {
			  return HT_WOULD_BLOCK;
		      } else if ( status == HT_LOADED ) {
			ctrl->substate = SUB_SUCCESS;
			data->complete |= 3;
		      } else if ( status == HT_OK ) {
			return HT_WOULD_BLOCK;
		      } else {
			  ctrl->substate = SUB_ERROR;
			  data->stream_error = YES;
		      }
		      continue;
		  } else {
		  status = HTHost_read(HTNet_host(dnet), dnet);
		  }
		  if (status == HT_WOULD_BLOCK)
		      return HT_WOULD_BLOCK;
		  else if (status == HT_LOADED || status == HT_CLOSED || status == HT_OK) {
		      HTDoClose(dnet);
		      data->complete |= 1; 
		      if (data->complete >= 3)
			  ctrl->substate = SUB_SUCCESS;
		      else
			  data_is_active = NO;
		  } else {
		      ctrl->substate = SUB_ERROR;
		      data->stream_error = YES;
		  }
	      } else {
		  status = HTHost_read(HTNet_host(cnet), cnet);
		  if (status == HT_WOULD_BLOCK)
		      return HT_WOULD_BLOCK;
		  else if (status == HT_LOADED || status == HT_CLOSED) {
		      if (ctrl->repcode/100 == 2) {
			  data->complete |= 2;
			  if (data->complete >= 3)
			      ctrl->substate = SUB_SUCCESS;
			  else
			      data_is_active = YES;
		      } else
			  ctrl->substate = SUB_ERROR;
		  } else
		      ctrl->substate = SUB_ERROR;
	      }
	      break;

	  case SUB_ERROR:
	    HTTRACE(PROT_TRACE, "FTP Get Data now in state SUB_ERROR\n");
	    HTRequest_addError(request, ERR_FATAL, NO, HTERR_NOT_FOUND,
			       NULL, 0, "HTFTPGetData");
	    ctrl->substate = 0;
	    HT_FREE(segment);
	    return HT_ERROR;
	    break;

	  case SUB_SUCCESS:
	    HTTRACE(PROT_TRACE, "FTP Get Data now in state SUB_SUCCESS\n");
	    ctrl->substate = 0;
	    HT_FREE(segment);
	    return HT_LOADED;
	    break;
	}
    }
}

/* ------------------------------------------------------------------------- */

/*	Retrieve File from Server as an atomic action. 
**	-----------------------------------------------
**	Given a hypertext address, this routine loads a document.
**
** On entry,
**      request		This is the request structure
**	returns		HT_ERROR	Error has occured in call back
**			HT_OK		Call back was OK
*/
PRIVATE int FTPEvent (SOCKET soc, void * pVoid, HTEventType type);

PUBLIC int HTLoadFTP (SOCKET soc, HTRequest * request)
{
    HTNet * cnet = HTRequest_net(request);
    ftp_ctrl * ctrl = NULL;
    ftp_data * data = NULL;
    HTParentAnchor * anchor = HTRequest_anchor(request);
    char * url = HTAnchor_physical(anchor);

    /*
    ** Initiate a new FTP ctrl and data structure and bind to request structure
    ** This is actually state FTP_BEGIN, but it can't be in the state
    ** machine as we need the structure first.
    */
    HTTRACE(PROT_TRACE, "FTP......... Looking for `%s\'\n" _ url);
    if ((ctrl = (ftp_ctrl *) HT_CALLOC(1, sizeof(ftp_ctrl))) == NULL ||
	(data = (ftp_data *) HT_CALLOC(1, sizeof(ftp_data))) == NULL)
	HT_OUTOFMEM("HTLoadFTP");
    ctrl->cmd = HTChunk_new(128);
    ctrl->state = FTP_BEGIN;
    ctrl->server = FTP_UNSURE;
    ctrl->dnet = HTNet_dup(cnet);
    ctrl->cnet = cnet;
    HTNet_setContext(cnet, ctrl);
    HTNet_setEventCallback(cnet, FTPEvent);
    HTNet_setEventParam(cnet, ctrl);
    HTNet_setRawBytesCount(ctrl->dnet, YES);

    /* for now, the dnet comes back to the same place
    ** - vestigial from when the callback was from the request object
    */
    HTNet_setContext(ctrl->dnet, data);
    HTNet_setEventCallback(ctrl->dnet, FTPEvent);
    HTNet_setEventParam(ctrl->dnet, ctrl);
    return FTPEvent(soc, ctrl, HTEvent_BEGIN);
}

PRIVATE int FTPEvent (SOCKET soc, void * pVoid, HTEventType type)
{
    ftp_ctrl * ctrl = (ftp_ctrl *) pVoid;
    ftp_data * data = (ftp_data *) HTNet_context(ctrl->dnet);
    int status = HT_ERROR;
    HTNet * cnet = ctrl->cnet;
    HTRequest * request = HTNet_request(cnet);
    HTParentAnchor * anchor = HTRequest_anchor(request);
    char * url = HTAnchor_physical(anchor);

    HTHost *host = HTNet_host(cnet);


    if (type == HTEvent_CLOSE) {			      /* Interrupted */
        if (soc == HTNet_socket(cnet) && data->complete<1)
	    FTPCleanup(request, HT_INTERRUPTED);
	else
	    FTPCleanup(request, HT_LOADED);
	return HT_OK;
    } else if (type == HTEvent_TIMEOUT) {

	/*
	** Don't time out the control connection if we are actually recieving data
	** on the data connection
	*/
	if (!(soc == HTNet_socket(cnet) && !(data->complete & 1) && HTNet_bytesRead(ctrl->dnet)>0)) {
	    HTRequest_addError(request, ERR_FATAL, NO, HTERR_TIME_OUT, NULL, 0, "HTLoadHTTP");
	    FTPCleanup(request, HT_TIMEOUT);
	}
	return HT_OK;

    } else {
	ctrl = (ftp_ctrl *) HTNet_context(cnet);	/* Get existing copy */
	data = (ftp_data *) HTNet_context(ctrl->dnet);
    }

    /* Now jump into the machine. We know the state from the previous run */
    while (1) {
	switch (ctrl->state) {
	  case FTP_BEGIN:
	      HTTRACE(PROT_TRACE, "FTP Event... now in state FTP_BEGIN\n");

	      /* Only handle GET requests for now */
	      if (HTRequest_method(request) != METHOD_GET &&
	          HTRequest_method(request) != METHOD_PUT ) {
		  HTTRACE(PROT_TRACE, "FTP Event... This module only supports the GET or PUT methods\n");
		  ctrl->state = FTP_ERROR;
		  break;
	      }

	      HTFTPParseURL(request, url, ctrl, data);

	      /* The following is added by Neil Griffin, GAIN Software */

	      /*
	      ** If the user hasn't specified a permanent transfer type, then
	      ** use the transfer type specified at the end of the URL.
	      */
	      if (g_FTPTransferMode == FTP_DEFAULT_TRANSFER_MODE) {
		  switch (data->type) {
		  case 'a' : data->type = 'A'; break;
		  case 'A' : data->type = 'A'; break;
		  case 'i' : data->type = 'I'; break;
		  case 'I' : data->type = 'I'; break;
		  case 'd' : FTPListType(data, ctrl->server); break;
		  case 'D' : FTPListType(data, ctrl->server); break;
		  default  : data->type = 'I'; break;
		  }

		  /* Otherwise, use the permanent transfer type specified by the user. */
	      } else {
		  switch (g_FTPTransferMode) {
		  case FTP_ASCII_TRANSFER_MODE  : data->type = 'A'; break;
		  case FTP_BINARY_TRANSFER_MODE : data->type = 'I'; break;
		  case FTP_DIR_TRANSFER_MODE    : FTPListType(data, ctrl->server); break;
		  default                       : data->type = 'I'; break;
		  }
	      }

	      HTTRACE(PROT_TRACE, "FTP Event... Transfer mode set to '%c'\n" _ data->type);

	      /*
	      **  See if we can get any hints to what we might expect content wise.
	      */
	      if (!FTP_DIR(data)) HTBind_getAnchorBindings(anchor);

              /* Ready for next state */
              ctrl->state = FTP_NEED_CCON;
              break;

	case FTP_NEED_CCON:
	    HTTRACE(PROT_TRACE, "FTP Event... now in state FTP_NEED_CONN\n");
	    status = HTHost_connect(host, cnet, url);
	    host = HTNet_host(cnet);
	    if (status == HT_OK) {

		/*
		** Check the protocol class to see if we have connected to a
		** the right class of server, in this case HTTP.
		*/
		{
		    char * s_class = HTHost_class(host);
		    if (s_class && strcasecomp(s_class, "ftp")) {
			HTRequest_addError(request, ERR_FATAL, NO, HTERR_CLASS,
					   NULL, 0, "HTLoadNews");
			ctrl->state = FTP_ERROR;
			break;
		    }
		    HTHost_setClass(host, "ftp");
		}

		/* Check persistent connection */
		if (HTNet_persistent(cnet)) {
		    ctrl->server = HTHost_version(host);
		    HTTRACE(PROT_TRACE, "FTP Server.. Cache says type %d server\n" _ 
				ctrl->server);
		    ctrl->reset = 1;
		} else
		    HTNet_setPersistent(cnet, YES, HT_TP_SINGLE);

		/* 
		** Create the stream pipe FROM the channel to the application.
		** The target for the input stream pipe is set up using the
		** stream stack.
		*/
		{
		    HTStream * readstream = FTPStatus_new(request, ctrl, host);
		    HTNet_setReadStream(cnet, readstream);
		}

		/*
		** Create the stream pipe TO the channel from the application
		** and hook it up to the request object
		*/
		{
		    HTOutputStream * output = HTNet_getOutput(cnet, NULL, 0);
		    HTRequest_setInputStream(request, (HTStream *) output);
		}

		/*
		** Set up concurrent read/write if this request isn't the
		** source for a PUT or POST. As source we don't start reading
		** before all destinations are ready. If destination then
		** register the input stream and get ready for read
		*/
		if (HTRequest_isPostWeb(request)) {
		    HTEvent * event = HTNet_event(cnet);
		    HTEvent_register(HTNet_socket(cnet), HTEvent_READ, event);
		    HTRequest_linkDestination(request);
		}

		ctrl->state = FTP_NEED_LOGIN;
	    } else if (status == HT_WOULD_BLOCK || status == HT_PENDING)
		return HT_OK;
	    else
		ctrl->state = FTP_ERROR;	       /* Error or interrupt */
	    break;

	  case FTP_NEED_LOGIN:
	    HTTRACE(PROT_TRACE, "FTP Event... now in state FTP_NEED_LOGIN\n");
	    status = HTFTPLogin(request, cnet, ctrl);
 	    if (status == HT_WOULD_BLOCK) return HT_OK;
	    ctrl->state = (status == HT_OK) ? FTP_NEED_DCON : FTP_ERROR;
	    break;

	  case FTP_NEED_DCON:
	    HTTRACE(PROT_TRACE, "FTP Event... now in state FTP_NEED_DCON\n");
	    status = HTFTPDataConnection(request, cnet, ctrl, data);
	    if (status == HT_WOULD_BLOCK) return HT_OK;
	    if (status == HT_OK)
		ctrl->state = (data->type=='N') ?
		    FTP_NEED_SERVER : FTP_NEED_DATA;
	    else
		ctrl->state = FTP_ERROR;
	    break;

	  case FTP_NEED_DATA:
	    HTTRACE(PROT_TRACE, "FTP Event... now in state FTP_NEED_DATA\n");
	    status = HTFTPGetData(request, cnet, soc, ctrl, data);
 	    if (status == HT_WOULD_BLOCK) return HT_OK;
	    if (status == HT_LOADED)
		ctrl->state = FTP_SUCCESS;
	    else if (status == HT_OK)
		ctrl->state = FTP_NEED_DCON;
	    else if (HTRequest_method(request) == METHOD_PUT)
		ctrl->state = FTP_ERROR;
	    else if (!FTP_DIR(data) && !data->stream_error) {
		FTPListType(data, ctrl->server);
		ctrl->state = FTP_NEED_SERVER;         /* Try a dir instead? */
	    } else
		ctrl->state = FTP_ERROR;
	    break;

	  case FTP_NEED_SERVER:
	    HTTRACE(PROT_TRACE, "FTP Event... now in state FTP_NEED_SERVER\n");
	    status = HTFTPServerInfo(request, cnet, ctrl, data);
	    if (status == HT_WOULD_BLOCK) return HT_OK;
	    ctrl->state = FTP_NEED_DATA;
	    break;

	  case FTP_SUCCESS:
	    HTTRACE(PROT_TRACE, "FTP Event... now in state FTP_SUCCESS\n");
	    FTPCleanup(request, HT_LOADED);
	    return HT_OK;
	    break;
	    
	  case FTP_ERROR:
	    HTTRACE(PROT_TRACE, "FTP Event... now in state FTP_ERROR\n");
	    FTPCleanup(request, HT_ERROR);
	    return HT_OK;
	    break;
	}
    } /* End of while(1) */
}

PUBLIC void HTFTP_setTransferMode(FTPTransferMode mode)
{
    g_FTPTransferMode = mode;
}

PUBLIC FTPTransferMode HTFTP_transferMode (void)
{
    return g_FTPTransferMode;
}

PUBLIC void HTFTP_setControlMode (FTPControlMode mode)
{
    g_FTPControlMode = mode;
}

PUBLIC FTPControlMode HTFTP_controlMode (void)
{
    return g_FTPControlMode;
}
