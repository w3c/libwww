/*		Telnet Acees, Roligin, etc			HTAccess.c
**		==========================
**
** Authors
**	TBL	Tim Berners-Lee timbl@info.cern.ch
**	JFG	Jean-Francois Groff jgh@next.com
**	DD	Denis DeLaRoca (310) 825-4580  <CSP1DWD@mvs.oac.ucla.edu>
** History
**       8 Jun 92 Telnet hopping prohibited as telnet is not secure (TBL)
**	26 Jun 92 When over DECnet, suppressed FTP, Gopher and News. (JFG)
**	 6 Oct 92 Moved HTClientHost and logfile into here. (TBL)
**	17 Dec 92 Tn3270 added, bug fix. (DD)
**	 2 Feb 93 Split from HTAccess.c. Registration.(TBL)
*/

/* Implements:
*/
#include "HTTelnet.h"

#include "HTParse.h"
#include "HTUtils.h"
#include "HTAnchor.h"
#include "HTTP.h"
#include "HTFile.h"
#include <errno.h>
#include <stdio.h>

#include "tcp.h"
#include "HText.h"

#include "HTAccess.h"
#include "HTAlert.h"

#define HT_NO_DATA -9999


/*	Telnet or "rlogin" access
**	-------------------------
*/
PRIVATE int remote_session ARGS2(char *, access, char *, host)
{
	char * user = host;
	char * hostname = strchr(host, '@');
	char * port = strchr(host, ':');
	char   command[256];
	enum _login_protocol { telnet, rlogin, tn3270 } login_protocol =
		strcmp(access, "rlogin") == 0 ? rlogin :
		strcmp(access, "tn3270") == 0 ? tn3270 : telnet;
	
	if (hostname) {
	    *hostname++ = 0;	/* Split */
	} else {
	    hostname = host;
	    user = 0;		/* No user specified */
	}
	if (port) *port++ = 0;	/* Split */


/* If the person is already telnetting etc, forbid hopping */
/* This is a security precaution, for us and remote site */

	if (HTSecure) {

#ifdef TELNETHOPPER_MAIL
	    sprintf(command, 
	      "finger @%s | mail -s \"**telnethopper %s\" tbl@dxcern.cern.ch",
	       HTClientHost, HTClientHost);
	    system(command);
#endif
	    printf("\n\nSorry, but the service you have selected is one\n");
	    printf("to which you have to log in.  If you were running www\n");
	    printf("on your own computer, you would be automatically connected.\n");
	    printf("For security reasons, this is not allowed when\n");
	    printf("you log in to this information service remotely.\n\n");

	    printf("You can manually connect to this service using %s\n",
		   access);
	    printf("to host %s", hostname);
	    if (user) printf(", user name %s", user);
	    if (port) printf(", port %s", port);
	    printf(".\n\n");
	    return HT_NO_DATA;
	}

/* Not all telnet servers get it even if user name is specified
** so we always tell the guy what to log in as
*/
        if (user) printf("When you are connected, log in as %s\n", user);
	
#ifdef NeXT
#define TELNET_MINUS_L
#endif
#ifdef ultrix
#define TELNET_MINUS_L
#endif

#ifdef TELNET_MINUS_L
	sprintf(command, "%s%s%s %s %s", access,
		user ? " -l " : "",
		user ? user : "",
		hostname,
		port ? port : "");

	if (TRACE) fprintf(stderr, "HTaccess: Command is: %s\n", command);
	system(command);
	return HT_NO_DATA;		/* Ok - it was done but no data */
#define TELNET_DONE
#endif

/* Most unix machines suppport username only with rlogin */
#ifdef unix
#ifndef TELNET_DONE
	if (login_protocol != rlogin) {
	    sprintf(command, "%s %s %s", access,
		hostname,
		port ? port : "");
	} else {
	    sprintf(command, "%s%s%s %s %s", access,
		user ? " -l " : "",
		user ? user : "",
		hostname,
		port ? port : "");
	}
	if (TRACE) fprintf(stderr, "HTaccess: Command is: %s\n", command);
	system(command);
	return HT_NO_DATA;		/* Ok - it was done but no data */
#define TELNET_DONE
#endif
#endif

#ifdef MULTINET				/* VMS varieties */
	if (login_protocol == telnet) {
	    sprintf(command, "TELNET %s%s %s",
		port ? "/PORT=" : "",
		port ? port : "",
		hostname);
	} else if (login_protocol == tn3270) {
	    sprintf(command, "TELNET/TN3270 %s%s %s",
		port ? "/PORT=" : "",
		port ? port : "",
		hostname);
	} else {
	    sprintf(command, "RLOGIN%s%s%s%s %s",  /*lm 930713 */
		user ? "/USERNAME=" : "",
		user ? user : "",
		port ? "/PORT=" : "",
		port ? port : "",
		hostname);
	}
	if (TRACE) fprintf(stderr, "HTaccess: Command is: %s\n", command);
	system(command);
	return HT_NO_DATA;		/* Ok - it was done but no data */
#define TELNET_DONE
#endif

#ifdef UCX
#define SIMPLE_TELNET
#endif
#ifdef VM
#define SIMPLE_TELNET
#endif
#ifdef SIMPLE_TELNET
	if (login_protocol == telnet) {			/* telnet only */
	    sprintf(command, "TELNET  %s",	/* @@ Bug: port ignored */
		hostname);
	    if (TRACE) fprintf(stderr, "HTaccess: Command is: %s\n", command);
	    system(command);
	    return HT_NO_DATA;		/* Ok - it was done but no data */
	}
#endif

#ifndef TELNET_DONE
	fprintf(stderr,
	"Sorry, this browser was compiled without the %s access option.\n",
		access);
	fprintf(stderr,
	"\nTo access the information you must %s to %s", access, hostname);
	if (port) fprintf(stderr," (port %s)", port);
	if (user) fprintf(stderr," logging in with username %s", user);
	fprintf(stderr, ".\n");
	return -1;
#endif
}

/*	"Load a document" -- establishes a session
**	------------------------------------------
**
** On entry,
**	addr		must point to the fully qualified hypertext reference.
**
** On exit,
**	returns		<0	Error has occured.
**			>=0	Value of file descriptor or socket to be used
**				 to read data.
**	*pFormat	Set to the format of the file, if known.
**			(See WWW.h)
**
*/
PRIVATE int HTLoadTelnet
ARGS4
(
 CONST char *,		addr,
 HTParentAnchor *,	anchor,
 HTFormat,		format_out,
 HTStream *,		sink			/* Ignored */
)
{
    char * access;
    
    char * host;
    int status;
    
    if (sink) {
        HTAlert("Can't output a live session -- it has to be interactive");
	return HT_NO_ACCESS;
    }
    access =  HTParse(addr, "file:", PARSE_ACCESS);
    
    host = HTParse(addr, "", PARSE_HOST);
    status = remote_session(access, host);

    free(host);	
    free(access);
    return status;
}


GLOBALDEF PUBLIC HTProtocol HTTelnet = { "telnet", HTLoadTelnet, NULL };
GLOBALDEF PUBLIC HTProtocol HTRlogin = { "rlogin", HTLoadTelnet, NULL };
GLOBALDEF PUBLIC HTProtocol HTTn3270 = { "tn3270", HTLoadTelnet, NULL };


