/*	HyperText Tranfer Protocol	- Client implementation		HTTP.c
**	==========================
*/

/*	Module parameters:
**	-----------------
**
**  These may be undefined and redefined by syspec.h
*/
#include "HTParse.h"
#include "HTUtils.h"
#include "tcp.h"
#include "HTTCP.h"
#include "HTFormat.h"


/*		Load Document from HTTP Server			HTLoadHTTP()
**		==============================
**
**	Given a hypertext address, this routine loads a document.
**
**
** On entry,
**	arg	is the hypertext reference of the article to be loaded.
**	gate	is nill if no gateway, else the gateway address.
**
** On exit,
**	returns	>=0	If no error, a good socket number
**		<0	Error.
**
**	The socket must be closed by the caller after the document has been
**	read.
**
*/
PUBLIC int HTLoadHTTP ARGS3 (CONST char *, arg,
/*	CONST char *,	gate, */
	HTAnchor *,	anAnchor,
	int,		diag)
{
    int s;				/* Socket number for returned data */
    char *command;			/* The whole command */
    int status;				/* tcp return */
    int gateway = 0;			/* disable this feature */
    
    SockA soc_address;			/* Binary network address */
    SockA * sin = &soc_address;

    if (!arg) return -3;		/* Bad if no name sepcified	*/
    if (!*arg) return -2;		/* Bad if name had zero length	*/

/*  Set up defaults:
*/
#ifdef DECNET
	sin->sdn_family = AF_DECnet;	    /* Family = DECnet, host order */
	sin->sdn_objnum = DNP_OBJ;          /* Default: http object number */
#else  /* Internet */
	sin->sin_family = AF_INET;	    /* Family = internet, host order */
	sin->sin_port = htons(TCP_PORT);    /* Default: http port    */
#endif

    if (TRACE) {
        if (gate) fprintf(stderr,
		"HTTPAccess: Using gateway %s for %s\n", gate, arg);
        else fprintf(stderr, "HTTPAccess: Direct access for %s\n", arg);
    }
    
/* Get node name and optional port number:
*/
    {
	char *p1 = HTParse(gate ? gate : arg, "", PARSE_HOST);
	int status = HTParseInet(sin, p1);  /* TBL 920622 */
        free(p1);
	if (status) return status;   /* No such host for example */
    }
    
   
/*	Now, let's get a socket set up from the server for the sgml data:
*/      
#ifdef DECNET
    s = socket(AF_DECnet, SOCK_STREAM, 0);
#else
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif
    status = connect(s, (struct sockaddr*)&soc_address, sizeof(soc_address));
    if (status < 0) {
#ifndef DECNET
	/* This code is temporary backward-compatibility. It should
	   go away when no server runs on port 2784 alone */
	if (sin->sin_port == htons(TCP_PORT)) {  /* Try the old one */
	  if (TRACE) printf (
	    "HTTP: Port %d doesn't answer (errno = %d). Trying good old port %d...\n",
	    TCP_PORT, errno, OLD_TCP_PORT);
	  sin->sin_port = htons(OLD_TCP_PORT);
	  /* First close current socket and open a clean one */
	  status = NETCLOSE (s);
	  s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	  status = connect(s, (struct sockaddr*)&soc_address,
			   sizeof(soc_address));
	}
	if (status < 0)
#endif
	  {
	    if (TRACE) fprintf(stderr, 
	      "HTTP: Unable to connect to remote host for `%s' (errno = %d).\n", arg, errno);
	    /* free(command);   BUG OUT TBL 921121 */
	    return HTInetStatus("connect");
	  }
      }
    
    if (TRACE) fprintf(stderr, "HTTP connected, socket %d\n", s);

/*	Ask that node for the document,
**	omitting the host name & anchor if not gatewayed.
*/        
    if (gate) {
        command = malloc(4 + strlen(arg)+ 2 + 1);
        if (command == NULL) outofmem(__FILE__, "HTLoadHTTP");
        strcpy(command, "GET ");
	strcat(command, arg);
    } else { /* not gatewayed */
	char * p1 = HTParse(arg, "", PARSE_PATH|PARSE_PUNCTUATION);
        command = malloc(4 + strlen(p1)+ 2 + 1);
        if (command == NULL) outofmem(__FILE__, "HTLoadHTTP");
        strcpy(command, "GET ");
	strcat(command, p1);
	free(p1);
    }
    strcat(command, "\r\n");		/* Include CR for telnet compat. */
	    

    if (TRACE) fprintf(stderr, "HTTP writing command `%s' to socket %d\n", command, s);
    
#ifdef NOT_ASCII
    {
    	char * p;
	for(p = command; *p; p++) {
	    *p = TOASCII(*p);
	}
    }
#endif

    status = NETWRITE(s, command, (int)strlen(command));
    free(command);
    if (status<0) {
	if (TRACE) fprintf(stderr, "HTTPAccess: Unable to send command.\n");
	    return HTInetStatus("send");
    }

/*	Now load the data
*/
    HTParseFormat(diag ? WWW_PLAINTEXT : WWW_HTML,
                (HTParentAnchor *) anAnchor, s);
    
    if (TRACE) fprintf(stderr, "HTTP: close socket %d.\n", s);
    status = NETCLOSE(s);

    return HT_LOADED;			/* Good return */
}

/*	Protocol descriptor
*/

PUBLIC HTProtocol = { "http", HTLoadHTTP, 0 };
