/*								     HTTelnet.c
**	TELNET ACCESS, ROLIGIN, etc.
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
** Authors
**	TBL	Tim Berners-Lee timbl@w3.org
**	JFG	Jean-Francois Groff jgh@next.com
**	DD	Denis DeLaRoca (310) 825-4580  <CSP1DWD@mvs.oac.ucla.edu>
**	HFN	Henrik Frystyk
** History
**       8 Jun 92 Telnet hopping prohibited as telnet is not secure (TBL)
**	26 Jun 92 When over DECnet, suppressed FTP, Gopher and News. (JFG)
**	 6 Oct 92 Moved HTClientHost and logfile into here. (TBL)
**	17 Dec 92 Tn3270 added, bug fix. (DD)
**	 2 Feb 93 Split from HTAccess.c. Registration.(TBL)
**	 2 May 94 Fixed security hole with illegal characters in host
**		  and user names (code from Mosaic/Eric Bina).
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"
#include "HTTelnet.h"					 /* Implemented here */
#include "HTNetMan.h"

/* ------------------------------------------------------------------------- */

/*	make a string secure for passage to the
**	system() command.  Make it contain only alphanumneric
**	characters, or the characters '.', '-', '_', '+'.
**	Also remove leading '-' or '+'.
**	-----------------------------------------------------
**	Function taken from Mosaic's HTTelnet.c.
*/
PRIVATE void make_system_secure (char * str)
{
    char *ptr1, *ptr2;
    if ((str == NULL)||(*str == '\0'))
	return;

    /*
     * remove leading '-' or '+' by making it into whitespace that
     * will be stripped later.
     */
    if (*str=='-' || *str=='+')
	*str = ' ';
    ptr1 = ptr2 = str;
    while (*ptr1) {
	if ((!isalpha((int)*ptr1))&&(!isdigit((int)*ptr1))&&
	    (*ptr1 != '.')&&(*ptr1 != '_')&&
	    (*ptr1 != '+')&&(*ptr1 != '-')) {
	    ptr1++;
	} else {
	    *ptr2 = *ptr1;
	    ptr2++;
	    ptr1++;
	}
    }
    *ptr2 = *ptr1;
}

/*	Telnet or "rlogin" access
**	-------------------------
**	Returns	HT_NO_DATA	OK
**		HT_ERROR	Error
*/
PRIVATE int remote_session (HTRequest * request, char * url)
{
    int status = HT_NO_DATA;
    HTChunk *cmd = HTChunk_new(64);
    char *access = HTParse(url, "", PARSE_ACCESS);
    char *host = HTParse(url, "", PARSE_HOST);
    char *hostname = strchr(host, '@');
    char *user = NULL;
    char *passwd = NULL;
    char *port = NULL;

    /* We must be in interactive mode */
    if (!HTAlert_interactive()) {
	HTTRACE(PROT_TRACE, "Telnet...... Not interactive\n");
	HT_FREE(access);
	HT_FREE(host);
	HTChunk_delete(cmd);
	return HT_ERROR;
    }

    /* Look for user name, password, and port number */
    if (hostname) {
	*hostname++ = '\0';
	user = host;
	if ((passwd = strchr(host, ':')) != NULL) {
	    *passwd++ = '\0';
	    HTUnEscape(passwd);
	}
	HTUnEscape(user);			/* Might have a funny userid */
    } else {
	hostname = host;
    }
    if ((port = strchr(hostname, ':')) != NULL)
	*port++ = '\0';

    /* If the person is already telnetting etc, forbid hopping */
    if (HTLib_secure()) {
	HTRequest_addError(request, ERR_FATAL, NO,
			   HTERR_ACCESS, NULL, 0, "HTLoadTelnet");
	HT_FREE(access);
	HT_FREE(host);
	HTChunk_delete(cmd);
	return HT_NO_DATA;
    }

    /*
    ** Make user and hostname secure by removing leading '-' or '+'.
    ** and allowing only alphanumeric, '.', '_', '+', and '-'.
    */
    make_system_secure(user);
    make_system_secure(passwd);
    make_system_secure(hostname);
    make_system_secure(port);

    if (!strcasecomp(access, "telnet")) {
#ifdef SIMPLE_TELNET
	HTChunk_puts(cmd, "TELNET ");
	HTChunk_puts(cmd, hostname);			  /* Port is ignored */
#else
#ifdef FULL_TELNET					    /* User and port */
	HTChunk_puts(cmd, "telnet ");
	HTChunk_puts(cmd, hostname);
	if (user) {
	    HTChunk_puts(cmd, " -l ");
	    HTChunk_puts(cmd, user);
	}
	if (port) {
	    HTChunk_putc(cmd, ' ');
	    HTChunk_puts(cmd,  port);
	}
#else
#ifdef MULTINET
	HTChunk_puts(cmd, "TELNET ");
	if (port) {
	    HTChunk_puts(cmd, "/PORT=");
	    HTChunk_puts(cmd, port);
	    HTChunk_putc(cmd, ' ');
	}
	HTChunk_puts(cmd, hostname);
#else							  /* User is ignored */
	HTChunk_puts(cmd, "telnet ");
	HTChunk_puts(cmd, hostname);
	if (port) {
	    HTChunk_putc(cmd, ' ');
	    HTChunk_puts(cmd,  port);
	}
#endif /* MULTINET */
#endif /* FULL_TELNET */
#endif /* SIMPLE_TELNET */

    } else if (!strcasecomp(access, "rlogin")) {
#ifdef MULTINET
	HTChunk_puts(cmd, "RLOGIN ");
	if (user) {
	    HTChunk_puts(cmd, "/USERNAME=");
	    HTChunk_puts(cmd, user);
	    HTChunk_putc(cmd, ' ');
	}
	if (port) {
	    HTChunk_puts(cmd, "/PORT=");
	    HTChunk_puts(cmd, port);
	    HTChunk_putc(cmd, ' ');
	}
	HTChunk_puts(cmd, hostname);
#else
#ifdef RLOGIN_USER			       /* format: "hostname -l user" */
	HTChunk_puts(cmd, "rlogin ");
	HTChunk_puts(cmd, hostname);
	if (user) {
	    HTChunk_puts(cmd, " -l ");
	    HTChunk_puts(cmd, user);
	}
#else					       /* format: "-l user hostname" */
	HTChunk_puts(cmd, "rlogin ");
	if (user) {
	    HTChunk_puts(cmd, "-l ");
	    HTChunk_puts(cmd, user);
	    HTChunk_putc(cmd, ' ');
	}
	HTChunk_puts(cmd, hostname);
#endif /* RLOGIN_AFTER */
#endif /* MULTINET */

    } else if (!strcasecomp(access, "tn3270")) {
#ifdef MULTINET
	HTChunk_puts(cmd, "TELNET/TN3270 ");
	if (port) {
	    HTChunk_puts(cmd, "/PORT=");
	    HTChunk_puts(cmd, port);
	    HTChunk_putc(cmd, ' ');
	}
	HTChunk_puts(cmd, hostname);
#else
	HTChunk_puts(cmd, "tn3270 ");
	HTChunk_puts(cmd, hostname);			  /* Port is ignored */
#endif /* MULTINET */

    } else {
	HTTRACE(PROT_TRACE, "Telnet...... Unknown access method: `%s\'\n" _ 
		    access);
	status = HT_ERROR;
    }

    /* Now we are ready to execute the command */
    HTTRACE(PROT_TRACE, "Telnet...... Command is `%s\'\n" _ HTChunk_data(cmd));
    if (user) {
	HTChunk *msg = HTChunk_new(128);
	if (strcasecomp(access, "rlogin")) {
	    HTChunk_puts(msg, "user <");
	    HTChunk_puts(msg, user);
	    HTChunk_putc(msg, '>');
	}
	if (passwd) {
	    HTChunk_puts(msg, " and password <");
	    HTChunk_puts(msg, passwd);
	    HTChunk_putc(msg, '>');
	}
	HTRequest_addError(request, ERR_INFO, NO,
			   HTERR_LOGIN, HTChunk_data(msg), HTChunk_size(msg),
			   "HTLoadTelnet");
	HTChunk_delete(msg);
    }
#ifdef HAVE_SYSTEM
    system(cmd->data);
#endif
    HT_FREE(access);
    HT_FREE(host);
    HTChunk_delete(cmd);
    return status;

}

/*	"Load a document" -- establishes a session
**	==========================================
**
** On entry,
**      request		This is the request structure
** On exit,
**	returns		HT_ERROR	Error has occured or interrupted
**			HT_NO_DATA	if return status 204 No Response
*/
PRIVATE int TelnetEvent (SOCKET soc, void * pVoid, HTEventType type);

PUBLIC int HTLoadTelnet (SOCKET soc, HTRequest * request)
{
    HTNet * net = HTRequest_net(request);
    HTParentAnchor * anchor = HTRequest_anchor(request);
    char * url = HTAnchor_physical(anchor);

    HTTRACE(PROT_TRACE, "Telnet...... Looking for `%s\'\n" _ url);
    HTNet_setEventCallback(net, TelnetEvent);
    HTNet_setEventParam(net, net);  /* callbacks get http* */

    HTCleanTelnetString(url);
    {
	int status = remote_session(request, url);
	HTNet_delete(net, status);
    }
    return HT_OK;
}

PRIVATE int TelnetEvent (SOCKET soc, void * pVoid, HTEventType type)
{
    HTNet * net = (HTNet *)pVoid;

    /* This is a trick as we don't have any socket! */
    if (type == HTEvent_CLOSE)				      /* Interrupted */
	HTNet_delete(net, HT_INTERRUPTED);
    else
	HTNet_delete(net, HT_ERROR);
    return HT_OK;
}


