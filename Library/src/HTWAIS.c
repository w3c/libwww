/*	WorldWideWeb - Wide Area Informaion Server Access	HTWAIS.c
**	==================================================
**
**	This module allows a WWW server or client to read data from a
**	remote  WAIS
**  server, and provide that data to a WWW client in hypertext form.
**  Source files, once retrieved, are stored and used to provide
**  information about the index when that is acessed.
**
** Authors
**	BK	Brewster Kahle, Thinking Machines, <Brewster@think.com>
**	TBL	Tim Berners-Lee, CERN <timbl@info.cern.ch>
**
** History
**	   Sep 91	TBL adapted shell-ui.c (BK) with HTRetrieve.c from WWW.
**	   Feb 91	TBL Generated HTML cleaned up a bit (quotes, escaping)
**			    Refers to lists of sources. 
**	   Mac 93	TBL   Lib 2.0 compatible module made.	
**
** Bugs
**	Uses C stream i/o to read and write sockets, which won't work
**	on VMS TCP systems.
**
**	Should cache connections.
**
**	ANSI C only as written
**
** WAIS comments:
**
**	1.	Separate directories for different system's .o would help
**	2.	Document ids are rather long!
**
** WWW Address mapping convention:
**
**	/servername/database/type/length/document-id
**
**	/servername/database?word+word+word
*/
/* WIDE AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   Brewster@think.com
*/


#define DIRECTORY "/quake.think.com:210/directory-of-servers"

#define BIG 1024	/* identifier size limit  @@@@@ */

/*				From WAIS:
*/
#ifdef NOT_NEEDED_HERE
#include <ctype.h>
#include <string.h>
#include <sockets.h>
#endif

#include <ui.h>

#define MAX_MESSAGE_LEN 100000
#define CHARS_PER_PAGE 10000 /* number of chars retrieved in each request */

#define WAISSEARCH_DATE "Fri Jul 19 1991"


/*				From WWW:
*/
#define BUFFER_SIZE 4096	/* Arbitrary size for efficiency */

#include "HTUtils.h"
#include "tcp.h"
#ifdef RULES			/* Use rules? Nope.*/
#include "HTRules.h"
#endif
#include "HTParse.h"

#include "ParseWSRC.h"

extern int WWW_TraceFlag;	/* Control diagnostic output */
extern FILE * logfile;		/* Log file output */
extern char HTClientHost[16];	/* Client name to be output */

PRIVATE FILE * client;		/* File pointer for client channel */
PRIVATE BOOL	wsrc;		/* Data being read is wais source info */
PRIVATE BOOL	as_gate;	/* Client is using us as gateway */

PRIVATE char	line[2048];	/* For building strings to display */
				/* Must be able to take id */
/*								showDiags
*/
/* modified from Jonny G's version in ui/question.c */

void showDiags ARGS1(
	diagnosticRecord **, d)
{
  long i;

  for (i = 0; d[i] != NULL; i++) {
    if (d[i]->ADDINFO != NULL) {
      PUTS("Diagnostic code is ");
      PUTS(d[i]->DIAG);
      PUTC(' ');
      PUTS(d[i]->ADDINFO);
      PUTC('\n'); ;
    }
  }
}

/*	Matrix of allowed characters in filenames
**	-----------------------------------------
*/

PRIVATE BOOL acceptable[256];
PRIVATE BOOL acceptable_inited = NO;

PRIVATE void init_acceptable NOARGS
{
    unsigned int i;
    char * good = 
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./-_$";
    for(i=0; i<256; i++) acceptable[i] = NO;
    for(;*good; good++) acceptable[(unsigned int)*good] = YES;
    acceptable_inited = YES;
}

/*	Transform file identifier into WWW address
**	------------------------------------------
**
**
** On exit,
**	returns		nil if error
**			pointer to malloced string (must be freed) if ok
*/
char * WWW_from_archie ARGS1 (char *, file)
{
    char * end;
    char * result;
    char * colon;
    for(end=file; *end > ' '; end++);	/* assumes ASCII encoding*/
    result = (char *)malloc(10 + (end-file));
    if (!result) return result;		/* Malloc error */
    strcpy(result, "file://");
    strncat(result, file, end-file);
    colon = strchr(result+7, ':');	/* Expect colon after host */
    if (colon) {
	for(; colon[0]; colon[0]=colon[1], colon++);	/* move down */
    }
    return result;
} /* WWW_from_archie */

/*	Transform document identifier into WWW address
**	----------------------------------------------
**
** Bugs: A static buffer of finite size is used!
**	The format of the docid MUST be good!
**
** On exit,
**	returns		nil if error
**			pointer to malloced string (must be freed) if ok
*/
char * WWW_from_WAIS ARGS1(any *, docid)
{
    static char buf[BIG];
    char * q = buf;
    char * p = (docid->bytes);
    int i, l;
    if (TRACE) {
	char *p;
	fprintf(stderr, "WAIS id (%d bytes) is ", docid->size);
	for(p=docid->bytes; p<docid->bytes+docid->size; p++) {
	    if ((*p >= ' ') && (*p<= '~')) /* Assume ASCII! */
		fprintf(stderr, "%c", *p);
	    else
		fprintf(stderr, "<%x>", *p);
	}
	fprintf(stderr, "\n");
    }	 
    for (p=docid->bytes; (p<docid->bytes+docid->size) && (q<&buf[BIG]);) {
	if (TRACE) fprintf(stderr, "    Record type %d, length %d\n",
		p[0], p[1]);
        if (*p>10) {
	    fprintf(stderr, "Eh? DOCID record type of %d!\n", *p);
	    return 0;
	}
	*q++ = (*p++) + '0';	/* Record type */
	*q++ = '=';		/* Separate */
	l = *p++;		/* Length */
	for(i=0; i<l; i++, p++){
	    if (!acceptable[*p]) {
		*q++ = HEX_ESCAPE;	/* Means hex commming */
		*q++ = hex[(*p) >> 4];
		*q++ = hex[(*p) & 15];
	    }
	    else *q++ = *p;
	}
	*q++= ';';		/* Terminate field */
    }
    *q++ = 0;			/* Terminate string */
    if (TRACE) fprintf(stderr, "WWW form of id: %s\n", buf); 
    {
        char * result = (char *)malloc(strlen(buf)+1);
	strcpy(result, buf);
	return result;
    }
} /* WWW_from_WAIS */


/*	Transform document name into document identifier
**	------------------------------------------------
**
** On entry,
**	docname		points to valid name produced originally by
**			WWW_from_WAIS
** On exit,
**	docid->size	is valid
**	docid->bytes	is malloced and must later be freed.
*/
PRIVATE any * WAIS_from_WWW ARGS2 (any *, docid, char *, docname)
{
    char *z; 	/* Output pointer */
    char *sor;	/* Start of record - points to size field. */
    char *p; 	/* Input pointer */
    char *q; 	/* Poisition of "=" */
    char *s; 	/* Position of semicolon */
    int n;	/* size */
    if (TRACE) fprintf(stderr, "WWW id (to become WAIS id): %s\n", docname); 
    for(n=0, p = docname; *p; p++) {	/* Count sizes of strings */
        n++;
	if (*p == ';')  n--;		/* Not converted */
	else if (*p == HEX_ESCAPE) n=n-2;	/* Save two bytes */
        docid->size = n;
    }
    
    docid->bytes = (char *) malloc(docid->size); /* result record */
    z = docid->bytes;
    
    for(p = docname; *p; ) {	/* Convert of strings */
        *z++ = *p++ - '0';	/* Record type */
	q = strchr(p , '=');
	if (!q) return 0;
	s = strchr(q, ';');	/* (Check only) */
	if (!s) return 0;	/* Bad! No ';';	*/
        sor = z;		/* Remember where the size field was */
	z++;			/* Skip record size for now	*/
	for(p=q+1; *p!=';' ; ) {
	   if (*p == HEX_ESCAPE) {
	        char c;
		unsigned int b;
		p++;
	        c = *p++;
		b =   from_hex(c);
		c = *p++;
		if (!c) break;	/* Odd number of chars! */
		*z++ = (b<<4) + from_hex(c);
	    } else {
	        *z++ = *p++;	/* Record */
	    }
	}
	*sor = (z-sor-1);	/* Fill in size -- not counting size itself */
	p++;			/* After semicolon: start of next record */
    }
    
    if (TRACE) {
	char *p;
	fprintf(stderr, "WAIS id (%d bytes) is ", docid->size);
	for(p=docid->bytes; p<docid->bytes+docid->size; p++) {
	    if ((*p >= ' ') && (*p<= '~')) /* Assume ASCII! */
		fprintf(stderr, "%c", *p);
	    else
		fprintf(stderr, "<%x>", *p);
	}
	fprintf(stderr, "\n");
    }	 
    return docid;		/* Ok */
    
} /* WAIS_from_WWW */

/*	Output a characater in HTML, escaping < and &
**
*/
PRIVATE void html_putc(char ch)
{
    switch(ch) {

    case '<':
    case '&': fprintf(client, "&#%d;", (int)ch);
	    break;
	    
    case '\n': fprintf(client, "\r\n");	/* CRLF a la telnet */
    	    break;
	    
    default:
	    putc(ch, client);
	    break;
    
    } /* switch */
}


/*	Send a plain text record to the client		output_text_record()
**	--------------------------------------
*/
#define PUT(c) wsrc ? WSRC_treat(c) : html_putc(c)
PRIVATE void output_text_record ARGS3(
    FILE *,		client,
    WAISDocumentText *,	record,
    boolean,		quote_string_quotes)
{
  long count;
  /* printf(" Text\n");
     print_any("     DocumentID:  ", record->DocumentID);
     printf("     VersionNumber:  %d\n", record->VersionNumber);
     */
  for(count = 0; count < record->DocumentText->size; count++){
    long ch = (unsigned char)record->DocumentText->bytes[count];
    if (ch == 27) {

	    /* then we have an escape code */
	    /* if the next letter is '(' or ')', then ignore two letters */
	    if('(' == record->DocumentText->bytes[count + 1] ||
		')' == record->DocumentText->bytes[count + 1])
	    count += 1;             /* it is a term marker */
	    else count += 4;		/* it is a paragraph marker */
    } else if (ch == '\n' || ch == '\r') {
	    PUT('\n');
    } else if ((ch=='\t') || isprint(ch)){
	    PUT(ch);
    } 
  }
} /* output text record */


/*	Format A Search response for the client		display_search_response
**	---------------------------------------
*/
/* modified from tracy shen's version in wutil.c
 * displays either a text record or a set of headlines.
 */
void
display_search_response ARGS3(
    SearchResponseAPDU *,	response,
    char *,			database,
    char *,	 		keywords)
{
  WAISSearchResponse  *info;
  long i, k;
  
  BOOL archie =  strstr(database, "archie")!=0;	/* Specical handling */
  
  if (TRACE) fprintf(stderr, "WAISGate: Displaying search response\n");
  sprintf(line,
  	"Index %s contains the following %d item%s relevant to '%s'.\n",
	 database,
	 response->NumberOfRecordsReturned,
	 response->NumberOfRecordsReturned ==1 ? "" : "s",
	 keywords);
#ifdef GEN_HT
  PUTS( line);
  START(HTML_DL);
#else
  fprintf(client, "%s<dl>\n", line);
#endif 
  if ( response->DatabaseDiagnosticRecords != 0 ) {
    info = (WAISSearchResponse *)response->DatabaseDiagnosticRecords;
    i =0; 

    if (info->Diagnostics != NULL)
      showDiags(info->Diagnostics);

    if ( info->DocHeaders != 0 ) {
      for (k=0; info->DocHeaders[k] != 0; k++ ) {
	WAISDocumentHeader* head = info->DocHeaders[k];
	char * headline = trim_junk(head->Headline);
	any * docid = head->DocumentID;
	char * docname;			/* printable version of docid */
	i++;

/*	Make a printable string out of the document id.
*/
	if (TRACE) fprintf(stderr, 
		"WAISGate:  %2d: Score: %4ld, lines:%4ld '%s'\n", 
	       i,
	       (info->DocHeaders[k]->Score),
	       info->DocHeaders[k]->Lines,
	       headline);

	if (archie) {
	    char * www_name = WWW_from_archie(headline);
	    if (www_name) {
		HTChildAnchor * anc = HTAnchor_findChildAndLink(
			HTMainAnchor, www_name, "");
		HText_beginAnchor(HT, anc);
		PUTS(headline);
		HText_endAnchor(HT);
		sprintf(line, "\tScore: %4ld, lines:%4ld\n",
		    head->Score,
		    head->Lines);
		PUTS( line);
		free(www_name);
	    } else {
		sprintf(line,
		    "\n%s\tScore: %4ld, lines:%4ld (bad file name)\n",
		    headline,
		    head->Score,
		    head->Lines);
		 PUTS( line);
	    }
	} else { /* Not archie */
	    docname =  WWW_from_WAIS(docid);
	    if (docname) {
		char * dbname = deslash(database);
		sprintf(line, "%s/%s/%d/%s",		/* W3 address */
				    dbname,
		    head->Types ? head->Types[0] : "TEXT",
		    head->DocumentLength,
		    docname);
		{
		    HTChildAnchor * anc = HTAnchor_findChildAndLink(
			HTMainAnchor, line, "");
		    HText_beginAnchor(HT, anc);
		}
		HText_appendText(headline);
		HText_endAnchor(HT);
		sprintf(line, "\tScore: %4ld, lines:%4ld\n",
		    head->Score,
		    head->Lines);
		PUTS( line);
		free(dbname);
		free(docname);
	    } else {
		sprintf(line,
		    "\n%s\tScore: %4ld, lines:%4ld (bad doc id)\n",
		    headline,
		    head->Score,
		    head->Lines);
		 PUTS( line);
	    }
	  }
      } /* next document header */
    } /* if there were any document headers */
    
    if ( info->ShortHeaders != 0 ) {
      k =0;
      while (info->ShortHeaders[k] != 0 ) {
	i++;
	PUTS( "\nShort Header record, can't display\n");
      }
    }
    if ( info->LongHeaders != 0 ) {
      k =0;
      while (info->LongHeaders[k] != 0) {
	i++;
#ifdef GEN_HT
	PUTS( "\nLong Header record, can't display\n");
#else
	fprintf(client, "\n    Longheader record %2d, (cant display) ", i);
	/* dsply_long_hdr_record( info->LongHeaders[k++]); */
#endif
      }
    }
    if ( info->Text != 0 ) {
      k =0;
      while (info->Text[k] != 0) {
	i++;
	PUTS( "\nText record\n");
	output_text_record(client, info->Text[k++], false);
      }
    }
    if ( info->Headlines != 0 ) {
      k =0;
      while (info->Headlines[k] != 0) {
	i++;
	PUTS( "\nHeadline record, can't display\n");
	/* dsply_headline_record( info->Headlines[k++]); */
      }
    }
    if ( info->Codes != 0 ) {
      k =0;
      while (info->Codes[k] != 0) {
	i++;
	PUTS( "\nCode record, can't display\n");
	/* dsply_code_record( info->Codes[k++]); */
      }
    }
  }				/* Loop: display user info */
  END(HTML_DL);
  PUTC('\n'); ;
}



/*	Retrieve a file or perform a search
**	-----------------------------------
*/
PUBLIC int HTRetrieve ARGS3(
	CONST char *,	arg,
	CONST char *,	key,
	int,		soc)

#define MAX_KEYWORDS_LENGTH 1000
#define MAX_SERVER_LENGTH 1000
#define MAX_DATABASE_LENGTH 1000
#define MAX_SERVICE_LENGTH 1000
#define MAXDOCS 40

{
    static CONST char * error_header =
"<h1>Access error</h1>\nThe WWW-WAIS gateway reports the following error:<P>\n";

    char* request_message = NULL; /* arbitrary message limit */
    char* response_message = NULL; /* arbitrary message limit */
    long request_buffer_length;	/* how of the request is left */
    SearchResponseAPDU  *retrieval_response = 0;
    char keywords[MAX_KEYWORDS_LENGTH + 1];
    char *server_name;	
    char *wais_database;		/* name of current database */
    char *www_database;			/* Same name with deslashed */
    char *service;
    char *doctype;
    char *doclength;
    long document_length;
    char *docname;
    FILE *connection = 0;
    char * names;		/* Copy of arg to be hacked up */
    BOOL ok = NO;
    
    extern FILE * connect_to_server();
    
    wsrc = NO;
    if (!acceptable_inited) init_acceptable();
    
    client = fdopen(soc, "w");	/* Convert file descriptor to file pointer */
    				/* Can't be done under VMS/UCX */
				
    if (!client) {
        fprintf(stderr,
		"WAISGate: Can't open file on socket, errno=%d",
		errno);
        return (-1);
    }
        
/*	Decipher and check syntax of WWW address:
**	----------------------------------------
**
**	First we remove the "wais:" if it was spcified.  920110
*/  
    names = HTParse(arg, "", PARSE_HOST | PARSE_PATH | PARSE_PUNCTUATION);

    if (names[0]== '/') {
	server_name = names+1;
	if (as_gate =(*server_name == '/'))
	    server_name++;	/* Accept one or two */
	www_database = strchr(server_name,'/');
	if (www_database) {
	    *www_database++ = 0;		/* Separate database name */
	    doctype = strchr(www_database, '/');
	    if (key) ok = YES;	/* Don't need doc details */
	    else if (doctype) {	/* If not search parse doc details */
		*doctype++ = 0;	/* Separate rest of doc address */
		doclength = strchr(doctype, '/');
		if(doclength) {
		    *doclength++ = 0;
		    document_length = atol(doclength);
		    if (document_length) {
			docname=strchr(doclength, '/');
			if (docname) {
			    *docname++ = 0;
			    ok = YES;	/* To avoid a goto! */
			} /* if docname */
		    } /* if document_length valid */
		} /* if doclength */
	    } else { /* no doctype?  Assume index required */
	        if (!key) key = "";
		ok = YES;
	    } /* if doctype */
	} /* if database */
     }
     
     if (!ok) {
	 fprintf(client, "%sSyntax error in WWW document address\n",
	 	error_header);
	 fprintf(client, "<P>Address received was\n<XMP>\n%s\n</XMP>\n",
	 	arg);
	 fprintf(client,
	 "<P>Syntax is //node:port/databse\n");
	 fprintf(client,
	 "<P>or //node:port/databse?query+query\n");
	 fprintf(client,
	 "<P>or //node:port/database/doctype/doclength/docid\n");
	 fprintf(stderr,
	 	"WAISGate: Syntax error in WWW document address `%s'\n",
	 	arg);
	 fclose(client);
	 free(names);
	  return -1;
     } 
     if (TRACE) fprintf(stderr, "WAISGate: Parsed OK\n");
     
     service = strchr(names, ':');
     if (service)  *service++ = 0;
     else service = "210";
     
     if (server_name[0] == 0)
        connection = NULL;

     else if (!(key && !*key))
      if ((connection=connect_to_server(server_name,atoi(service)))
      	 == NULL)  {
	 fprintf (client,
	     "%sCan't open connection to %s via service %s.\n",
	     error_header, server_name, service);
	 fclose(client);
	 free(names);
        return (-1);
    }

    wais_database = enslash(www_database);

    request_message = (char*)s_malloc((size_t)MAX_MESSAGE_LEN * sizeof(char));
    response_message = (char*)s_malloc((size_t)MAX_MESSAGE_LEN * sizeof(char));

/*	If keyword search is performed but there are no keywords,
**	the user has followed a link to the index itself. It would be
**	appropriate at this point to send him the .SRC file - how?
*/

    if (key && !*key) {				/* I N D E X */
    
	char filename[256];
	FILE * fp;
	
	/* If we have seen a source file for this database, use that:
	*/
	sprintf(filename, "%s%s:%s:%s.html",
		WAIS_CACHE_ROOT,
		server_name, service, www_database);

	fp = fopen(filename, "r");	/* Have we found this already? */
	if (TRACE) fprintf(stderr,
		"WAISGate: Description of server %s %s.\n",
		filename,
		fp ? "exists already" : "does NOT exist!");
	fprintf(client, "<ISINDEX>\n");
	if (fp) {
	    char c;
	    while((c=getc(fp))!=EOF) putc(c, client);	/* Transfer file */
	    fclose(fp);
	} else {
	    fprintf(client, "<TITLE>%s index</TITLE>\n",
		    wais_database);
	    fprintf(client, "<H1>%s</H1>\n", wais_database);
	    fprintf(client, "%s\n%s%s?%s\">full cover page</a>)<p>\n",
 "This is a document index whose cover page has not yet been retrieved.",
	      "(Retrieve <a href=\"", DIRECTORY, wais_database);
	    fprintf(client,"Please specify search words to find documents.\n");
	}
	fprintf(client, "%s%s%s",
	    "<P>(The search matches your words against full document text.\n",
	    "Access to this index is provided by the\n",
	    "WWW to WAIS gateway.\n");
	if (0!=strcmp(wais_database, "directory-of-servers")) {
	    fprintf(client, "%s%s%s%s%s%s%s",
"Specify search words to find documents.\nFor other databases, see the ",
"<a\r\nhref=\"http://info.cern.ch/hypertext/DataSources/WAIS/ByHost.html\">",
"list by internet domain</a>, or the <a href=\"",
"<a\r\nhref=\"http://info.cern.ch/hypertext/DataSources/WAIS/BySource.html\">",
"list by index name</a>, or the <a\r\nhref=\"",
DIRECTORY,
"\">directory of servers</a>.\r\n");
		
	} else {
	    fprintf(client, 
	    "Please specify search words to find other databases.");
	}
	fprintf(client, ")\n");
	
	fflush(client);
	
    } else if (key) {					/* S E A R C H */
	char *p;
	strncpy(keywords, key, MAX_KEYWORDS_LENGTH);
	while(p=strchr(keywords, '+')) *p = ' ';
    
        /* Send advance title to get something fast to the other end */
	
	fprintf(client, "<ISINDEX><TITLE>%s (in %s)</TITLE>\n",
		keywords, wais_database);
	fprintf(client, "<H1>%s</H1>\n", keywords);
	fflush(client);

	request_buffer_length = MAX_MESSAGE_LEN; /* Amount left */
	if (TRACE) fprintf(stderr, "WAISGate: Search for `%s' in `%s'\n",
		keywords, wais_database);
	if(NULL ==
	generate_search_apdu(request_message + HEADER_LENGTH, 
				&request_buffer_length, 
				keywords, wais_database, NULL, MAXDOCS))
	panic("request too large");
	

	if(!interpret_message(request_message, 
				MAX_MESSAGE_LEN - request_buffer_length, 
				response_message,
				MAX_MESSAGE_LEN,
				connection,
				false	/* true verbose */
				)) {
	    panic("returned message too large");
    
        } else {	/* returned message ok */
	
	    SearchResponseAPDU  *query_response = 0;
	    readSearchResponseAPDU(&query_response,
	    	response_message + HEADER_LENGTH);
	    display_search_response(query_response, wais_database, keywords);
	    if (query_response->DatabaseDiagnosticRecords)
		freeWAISSearchResponse(
			query_response->DatabaseDiagnosticRecords);         
	    freeSearchResponseAPDU( query_response);
	}	/* returned message not too large */
    
    } else {			/* D O C U M E N T    F E T C H */
    
	long count;
	any   doc_chunk;
	any * docid = &doc_chunk;
	if (TRACE) printf(
		"WAISGate: Retrieve document id `%s' type `%s' length %d\n",
		docname, doctype, document_length);
	wsrc = 0==strcmp(doctype, "WSRC");	/* WAIS "source" file? */

/*	Decode hex or litteral format for document ID
*/	
	WAIS_from_WWW(docid, docname);

	if (wsrc) {
	    WSRC_init();	/* Clear state machine */
	    fprintf(client, "<TITLE>Index description file</TITLE>\n");
	    
	} else if (strcmp(doctype, "HTML")!=0) {
	    fprintf(client, "
	    <HEAD>\r\n<TITLE>Document</TITLE>\r\n</HEAD>\r\n"); /* BORING! */
	    fprintf(client, "<BODY>\r\n<PRE>\r\n");
	}

/*	Loop over slices of the document
*/	
	for(count = 0; 
	    count * CHARS_PER_PAGE < document_length;
	    count++){
	  char *type = s_strdup(doctype);	/* Gets freed I guess */
	  request_buffer_length = MAX_MESSAGE_LEN; /* Amount left */
	  if (TRACE) fprintf(stderr, "    Slice at %d\n", count);
	  if(0 ==
	      generate_retrieval_apdu(request_message + HEADER_LENGTH,
		    &request_buffer_length, 
		    docid, 
		    CT_byte,
		    count * CHARS_PER_PAGE,
		    MIN((count + 1) * CHARS_PER_PAGE,document_length),
		    type,
		    wais_database
		    ))
		panic("request too long");
	     
	  if(0 ==
	     interpret_message(request_message, 
			       MAX_MESSAGE_LEN - request_buffer_length, 
			       response_message,
			       MAX_MESSAGE_LEN,
			       connection,
			       false /* true verbose */	
			       ))
	    panic("Returned message too large");

	  readSearchResponseAPDU(&retrieval_response, 
				 response_message + HEADER_LENGTH);

	  if(NULL == ((WAISSearchResponse *)
	  	retrieval_response->DatabaseDiagnosticRecords)->Text){
		display_search_response(retrieval_response,
					wais_database, keywords);
#ifdef GEN_HT
		PUTS( "No text was returned!\n");
#else
		fprintf(client, "<h2>No text was returned!</h2>\n");
#endif
		/* panic("No text was returned"); */
	  } else {
	  
		output_text_record(client,
		((WAISSearchResponse *)
		    retrieval_response->DatabaseDiagnosticRecords)->Text[0],
		false);
	  
	  } /* If text existed */
	  
	}	/* Loop over slices */


	if (wsrc) {
	    if (!WSRC_gen_html()) {
	        fprintf(client,
"The index cover page has been retrieved. Please see the <a href=\"%s\">index itself</a><p>\n",
	    	WSRC_address);
	    } else {
	        if (TRACE) fprintf(stderr,
		     "WAISGate: Insufficient info in source file for %s!\n",
		     WSRC_address);
		fprintf(client, "Insufficient info in wais source file!\n");
	    }
	} else {  /* not wsrc */
	    fprintf(client, "</PRE>\r\n</BODY>\r\n");
	}
	free (docid->bytes);
	
	freeWAISSearchResponse( retrieval_response->DatabaseDiagnosticRecords); 
	freeSearchResponseAPDU( retrieval_response);

    } /* If document rather than search */

    fclose(client);

/*	(This could be done only after a timeout:)
*/
    if (connection) close_connection(connection);
    if (wais_database) free(wais_database);
    s_free(request_message);
    s_free(response_message);

    free(names);
    return 0;
}



