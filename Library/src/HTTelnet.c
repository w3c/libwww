/*								     HTTelnet.c
**	TELNET ACCESS, ROLIGIN, etc.
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
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
#include "tcp.h"
#include "HTUtils.h"
#include "HTParse.h"
#include "HTAnchor.h"
#include "HTChunk.h"
#include "HTAccess.h"
#include "HTAlert.h"
#include "HTTelnet.h"					 /* Implemented here */

/* ------------------------------------------------------------------------- */

/*	make a string secure for passage to the
**	system() command.  Make it contain only alphanumneric
**	characters, or the characters '.', '-', '_', '+'.
**	Also remove leading '-' or '+'.
**	-----------------------------------------------------
**	Function taken from Mosaic's HTTelnet.c.
*/
PRIVATE void make_system_secure ARGS1(char *, str)
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
*/
PRIVATE int remote_session ARGS1(char *, url)
{
    int status = HT_NO_DATA;
    HTChunk *cmd = HTChunkCreate(64);
    char *access = HTParse(url, "", PARSE_ACCESS);
    char *host = HTParse(url, "", PARSE_HOST);
    char *hostname = strchr(host, '@');
    char *user = NULL;
    char *passwd = NULL;
    char *port = NULL;

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
    if (HTClientHost) {
	HTChunk *msg = HTChunkCreate(256);
	HTChunkPuts(msg, "Sorry, but the service you have selected is one ");
	HTChunkPuts(msg, "to which you have to log in. If you were running ");
	HTChunkPuts(msg, "locally on your own computer, you would be ");
	HTChunkPuts(msg, "connected automatically. For security reasons, ");
	HTChunkPuts(msg, "this is not allowed when you log in to this ");
	HTChunkPuts(msg, "information service remotely. ");
	if (hostname) {
	    HTChunkPuts(msg,"You can manually connect to this service using ");
	    HTChunkPuts(msg, access);
	    HTChunkPuts(msg, " to host ");
	    HTChunkPuts(msg, hostname);
	    if (port) {
		HTChunkPuts(msg, " using port ");
		HTChunkPuts(msg, port);
	    }
	    if (user) {
		HTChunkPuts(msg, " and user name ");
		HTChunkPuts(msg, user);
	    }
	    if (passwd) {
		HTChunkPuts(msg, " and passwd ");
		HTChunkPuts(msg, passwd);
	    }
	}
	HTAlert(msg->data);
	HTChunkFree(msg);
	free(access);
	free(host);
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

    if (!strcmp(access, "telnet")) {
#ifdef SIMPLE_TELNET
	HTChunkPuts(cmd, "TELNET ");
	HTChunkPuts(cmd, hostname);			  /* Port is ignored */
#else
#ifdef MULTINET
	HTChunkPuts(cmd, "TELNET ");
	if (port) {
	    HTChunkPuts(cmd, "/PORT=");
	    HTChunkPuts(cmd, port);
	    HTChunkPutc(cmd, ' ');
	}
	HTChunkPuts(cmd, hostname);
#else
	HTChunkPuts(cmd, "telnet ");
	HTChunkPuts(cmd, hostname);
	if (port) {
	    HTChunkPutc(cmd, ' ');
	    HTChunkPuts(cmd,  port);
	}
#endif /* MULTINET */
#endif /* SIMPLE_TELNET */

    } else if (!strcmp(access, "rlogin")) {
#ifdef MULTINET
	HTChunkPuts(cmd, "RLOGIN ");
	if (user) {
	    HTChunkPuts(cmd, "/USERNAME=");
	    HTChunkPuts(cmd, user);
	    HTChunkPutc(cmd, ' ');
	}
	if (port) {
	    HTChunkPuts(cmd, "/PORT=");
	    HTChunkPuts(cmd, port);
	    HTChunkPutc(cmd, ' ');
	}
	HTChunkPuts(cmd, hostname);
#else
#ifdef RLOGIN_USER			       /* format: "hostname -l user" */
	HTChunkPuts(cmd, "rlogin ");
	HTChunkPuts(cmd, hostname);
	if (user) {
	    HTChunkPuts(cmd, "-l ");
	    HTChunkPuts(cmd, user);
	    HTChunkPutc(cmd, ' ');
	}
#else					       /* format: "-l user hostname" */
	HTChunkPuts(cmd, "rlogin ");
	if (user) {
	    HTChunkPuts(cmd, "-l ");
	    HTChunkPuts(cmd, user);
	    HTChunkPutc(cmd, ' ');
	}
	HTChunkPuts(cmd, hostname);
#endif /* RLOGIN_AFTER */
#endif /* MULTINET */

    } else if (!strcmp(access, "tn3270")) {
#ifdef MULTINET
	HTChunkPuts(cmd, "TELNET/TN3270 ");
	if (port) {
	    HTChunkPuts(cmd, "/PORT=");
	    HTChunkPuts(cmd, port);
	    HTChunkPutc(cmd, ' ');
	}
	HTChunkPuts(cmd, hostname);
#else
	HTChunkPuts(cmd, "tn3270 ");
	HTChunkPuts(cmd, hostname);			  /* Port is ignored */
#endif /* MULTINET */

    } else {
	if (PROT_TRACE)
	    fprintf(TDEST, "Telnet...... Unknown access method: `%s\'\n",
		    access);
	status = HT_ERROR;
    }

    /* Now we are ready to execute the command */
    if (PROT_TRACE)
	fprintf(TDEST, "Telnet...... Command is `%s\'\n", cmd->data);
    if (user) {
	HTChunk *msg = HTChunkCreate(128);
	HTChunkPuts(msg, "When you are connected, log in");
	if (strcmp(access, "rlogin")) {
	    HTChunkPuts(msg, "as <");
	    HTChunkPuts(msg, user);
	    HTChunkPutc(msg, '>');
	}
	if (passwd) {
	    HTChunkPuts(msg, " using password <");
	    HTChunkPuts(msg, passwd);
	    HTChunkPutc(msg, '>');
	}
	HTAlert(msg->data);
	HTChunkFree(msg);
    }
    system(cmd->data);
    free(access);
    free(host);
    HTChunkFree(cmd);
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
PRIVATE int HTLoadTelnet ARGS1(HTRequest *, request)
{
    char *url;
    if (!request || !request->anchor) {
        if (PROT_TRACE) fprintf(TDEST, "HTLoadTelnet Bad argument\n");
        return HT_ERROR;
    }

    /* We must be in interactive mode */
    if (!HTInteractive) {
        HTAlert("Can't output a live session -- it has to be interactive");
	return HT_ERROR;
    }
    url = HTAnchor_physical(request->anchor);
    HTCleanTelnetString(url);
    return remote_session(url);
}


GLOBALDEF PUBLIC HTProtocol HTTelnet = {
    "telnet", SOC_BLOCK, HTLoadTelnet, NULL, NULL
};

GLOBALDEF PUBLIC HTProtocol HTRlogin = {
    "rlogin", SOC_BLOCK, HTLoadTelnet, NULL, NULL
};

GLOBALDEF PUBLIC HTProtocol HTTn3270 = {
    "tn3270", SOC_BLOCK, HTLoadTelnet, NULL, NULL
};


