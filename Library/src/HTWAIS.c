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
**	   Mar 93	TBL   Lib 2.0 compatible module made.	
**
** Bugs
**	Uses C stream i/o to read and write sockets, which won't work
**	on VMS TCP systems.
**
**	Should cache connections.
**
**	ANSI C only as written
**
** Bugs fixed
**      NT Nathan Torkington (Nathan.Torkington@vuw.ac.nz)
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


#define DIRECTORY "/cnidr.org:210/directory-of-servers"
/* define DIRECTORY "/quake.think.com:210/directory-of-servers" */

#define BIG 1024	/* identifier size limit  @@@@@ */

/*			From WAIS
**			---------
*/

#include <ui.h>

#define MAX_MESSAGE_LEN 100000
#define CHARS_PER_PAGE 10000 /* number of chars retrieved in each request */
#define WAISSEARCH_DATE "Fri Jul 19 1991"


/*			FROM WWW
**			--------
*/
#define BUFFER_SIZE 4096	/* Arbitrary size for efficiency */

#define HEX_ESCAPE '%'

/* Platform dependent stuff */
#include "tcp.h"

/* Library includes */
#include "HTUtils.h"
#include "HTParse.h"
#include "HTAccess.h"		/* We implement a protocol */
#include "HTError.h"
#include "HTML.h"		/* The object we will generate */
#include "HTParse.h"
#include "HTFormat.h"
#include "HTTCP.h"
 
extern FILE * logfile;		/* Log file output */

PUBLIC int HTMaxWAISLines = 250;/* Max number of entries from a search */

PRIVATE BOOL	as_gate;	/* Client is using us as gateway */

PRIVATE char	line[2048];	/* For building strings to display */
				/* Must be able to take id */

/* Hypertext object building machinery */
#include "HTML.h"

#define PUTC(c) (*target->isa->put_character)(target, c)
#define PUTS(s) (*target->isa->put_string)(target, s)
#define START(e) (*target->isa->start_element)(target, e, 0, 0)
#define END(e) (*target->isa->end_element)(target, e)
#define FREE_TARGET (*target->isa->_free)(target)

struct _HTStructured {
	CONST HTStructuredClass *	isa;
	/* ... */
};

struct _HTStream {
	CONST HTStreamClass *	isa;
	/* ... */
};


/*								showDiags
*/
/* modified from Jonny G's version in ui/question.c */

void showDiags ARGS2(
	HTStream *, 		target,
	diagnosticRecord **, 	d)
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

/*	Transform document identifier into URL
**	--------------------------------------
**
** Bugs: A static buffer of finite size is used!
**	The format of the docid MUST be good!
**
** On exit,
**	returns		nil if error
**			pointer to malloced string (must be freed) if ok
*/
PRIVATE char hex [17] = "0123456789ABCDEF";
extern char from_hex PARAMS((char a));			/* In HTWSRC @@ */

PRIVATE char * WWW_from_WAIS ARGS1(any *, docid)

{
    static unsigned char buf[BIG];
    char num[10];
    unsigned char * q = buf;
    char * p = (docid->bytes);
    int i, l;
    if (PROT_TRACE) {
	char *p;
	fprintf(stderr, "HTLoadWAIS.. id (%d bytes) is ", (int)docid->size);
	for(p=docid->bytes; p<docid->bytes+docid->size; p++) {
	    if ((*p >= ' ') && (*p<= '~')) /* Assume ASCII! */
		fprintf(stderr, "%c", *p);
	    else
		fprintf(stderr, "<%x>", (unsigned)*p);
	}
	fprintf(stderr, "\n");
    }	 
    for (p=docid->bytes; (p<docid->bytes+docid->size) && (q<&buf[BIG]);) {
	if (PROT_TRACE)
	    fprintf(stderr, "............ Record type %d, length %d\n",
		    (unsigned char) p[0], (unsigned char) p[1]);
	sprintf(num, "%d", (int)*p);
	memcpy(q, num, strlen(num));
	q += strlen(num);
	p++;
	*q++ = '=';		/* Separate */
	l = (int)((unsigned char)*p);
	p++;
	if (l > 127)
	    {
		l = (l - 128) * 128;
		l = l + (int)((unsigned char)*p);
		p++;
	    }
	
	for (i = 0; i < l; i++, p++)
	    {
		if (!acceptable[(unsigned char)*p]) 
		    {
			*q++ = HEX_ESCAPE;
			*q++ = hex[((unsigned char)*p) >> 4];
			*q++ = hex[((unsigned char)*p) & 15];
		    }
		else *q++ = (unsigned char)*p;
	    }
	*q++= ';';		/* Terminate field */
#ifdef OLD_CODE
        if (*p>10) {
	    if (PROT_TRACE)
		fprintf(stderr, "WAIS........ DOCID record type of %d!\n", *p);
	    return 0;
	}
	{	/* Bug fix -- allow any byte value 15 Apr 93 */
	    unsigned int i = (unsigned) *p++;
	    
	    if (i > 99) {
		*q++ = (i/100) + '0';
		i = i % 100;
	    }
	    if (i > 9) {
		*q++ = (i/10) + '0';
		i = i % 10;
	    }
	    *q++ = i + '0';	/* Record type */
	}
	*q++ = '=';		/* Separate */
	l = *p++;		/* Length */
	for(i=0; i<l; i++, p++){
	    if (!acceptable[(int)*p]) {
		*q++ = HEX_ESCAPE;	/* Means hex commming */
		*q++ = hex[(*p) >> 4];
		*q++ = hex[(*p) & 15];
	    }
	    else *q++ = *p;
	}
	*q++= ';';		/* Terminate field */
#endif /* OLD_CODE */
    }
    *q++ = 0;			/* Terminate string */
    if (PROT_TRACE) fprintf(stderr, "HTLoadWAIS.. WWW form of id: %s\n", buf); 
    {
        char *result;
	if ((result = (char *) malloc((int) strlen(buf)+1)) == NULL)
	    outofmem(__FILE__, "WWW_from_WAIS");
	strcpy(result, buf);
	return result;
    }
} /* WWW_from_WAIS */


/*	Transform URL into WAIS document identifier
**	-------------------------------------------
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
    if (PROT_TRACE)
	fprintf(stderr, "HTLoadWAIS.. WWW id (to become WAIS id): %s\n",
		docname); 
    for(n=0, p = docname; *p; p++) {	/* Count sizes of strings */
        n++;
	if (*p == ';')  n--;		/* Not converted */
	else if (*p == HEX_ESCAPE) n=n-2;	/* Save two bytes */
        docid->size = n;
    }
    
    docid->bytes = (char *) malloc(docid->size+32); /* result record */
    z = docid->bytes;
    
    for(p = docname; *p; ) {
	q = strchr(p, '=');
	if (!q) 
	    return 0;
	*q = '\0';
	*z++ = atoi(p);
	*q = '=';
	s = strchr(q, ';');	/* (Check only) */
	if (!s) 
	    return 0;	/* Bad! No ';';	*/
	sor = z;          /* Remember where the size field was */
	z++;              /* Skip record size for now */
	
	{
	    int len;
	    int tmp;
	    for(p=q+1; *p!=';' ; ) {
		if (*p == HEX_ESCAPE) {
		    char c;
		    unsigned int b;
		    p++;
		    c = *p++;
		    b = from_hex(c);
		    c = *p++;
		    if (!c) 
			break;	/* Odd number of chars! */
		    *z++ = (b<<4) + from_hex(c);
		} else {
		    *z++ = *p++;	/* Record */
		}
	    }
	    len = (z-sor-1);
	    
	    z = sor;
	    if (len > 127) {
		tmp = (len / 128);
		len = len - (tmp * 128);
		tmp = tmp + 128;
		*z++ = (char)tmp;
		*z = (char)len;
	    } else {
		*z = (char)len;
	    }
	    z++;
	}
	
	for(p=q+1; *p!=';' ; )  {
	    if (*p == HEX_ESCAPE) {
		char c;
		unsigned int b;
		p++;
		c = *p++;
		b = from_hex(c);
		c = *p++;
		if (!c) 
		    break;	/* Odd number of chars! */
		*z++ = (b<<4) + from_hex(c);
	    } else {
		*z++ = *p++;	/* Record */
	    }
	}
	p++;			/* After semicolon: start of next record */
    }

#ifdef OLD_CODE
    for(p = docname; *p; ) {	/* Convert of strings */
    				/* Record type */
				
	*z = 0;			/* Initialize record type */
	while (*p >= '0' && *p <= '9') {
	    *z = *z*10 + (*p++ - '0');	/* Decode decimal record type */
	}
	z++;
	if (*p != '=') return 0;
	q = p;
	
/*        *z++ = *p++ - '0';
	q = strchr(p , '=');
	if (!q) return 0;
*/
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
#endif /* OLD_CODE */
    if (PROT_TRACE) {
	char *p;
	fprintf(stderr, "WAIS........ id (%d bytes) is ", (int)docid->size);
	for(p=docid->bytes; p<docid->bytes+docid->size; p++) {
	    if ((*p >= ' ') && (*p<= '~')) /* Assume ASCII! */
		fprintf(stderr, "%c", *p);
	    else
		fprintf(stderr, "<%x>", (unsigned)*p);
	}
	fprintf(stderr, "\n");
    }	 
    return docid;		/* Ok */
    
} /* WAIS_from_WWW */


/*	Send a plain text record to the client		output_text_record()
**	--------------------------------------
*/

PRIVATE void output_text_record ARGS4(
    HTStream *,			target,
    WAISDocumentText *,		record,
    boolean,			quote_string_quotes,
    boolean,                    binary)
{
  long count;
  /* printf(" Text\n");
     print_any("     DocumentID:  ", record->DocumentID);
     printf("     VersionNumber:  %d\n", record->VersionNumber);
     */

  if (binary) {
    (*target->isa->put_block)(target,
			      record->DocumentText->bytes,
			      record->DocumentText->size);
    return;
  }

  for(count = 0; count < record->DocumentText->size; count++){
    long ch = (unsigned char)record->DocumentText->bytes[count];
    if (ch == 27) {	/* What is this in for? Tim */

	    /* then we have an escape code */
	    /* if the next letter is '(' or ')', then ignore two letters */
	    if('(' == record->DocumentText->bytes[count + 1] ||
		')' == record->DocumentText->bytes[count + 1])
	    count += 1;             /* it is a term marker */
	    else count += 4;		/* it is a paragraph marker */
    } else if (ch == '\n' || ch == '\r') {
	    PUTC('\n');
    } else if ((ch=='\t') || isprint(ch)){
	    PUTC(ch);
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
display_search_response ARGS4(
    HTStructured *,		target,
    SearchResponseAPDU *,	response,
    char *,			database,
    char *,	 		keywords)
{
  WAISSearchResponse  *info;
  long i, k;
  
  BOOL archie =  strstr(database, "archie")!=0;	/* Specical handling */
  
  if (PROT_TRACE) fprintf(stderr, "WAIS........ Displaying search response\n");
  sprintf(line,
	  "Index %s contains the following %d item%s relevant to '%s'.\n",
	  database,
	  (int)(response->NumberOfRecordsReturned),
	  response->NumberOfRecordsReturned ==1 ? "" : "s",
	  keywords);
  PUTS(line);
  PUTS("The first figure for each entry is its relative score, ");
  PUTS("the second the number of lines in the item.");
  START(HTML_MENU);

  if ( response->DatabaseDiagnosticRecords != 0 ) {
    info = (WAISSearchResponse *)response->DatabaseDiagnosticRecords;
    i =0; 

    if (info->Diagnostics != NULL)
      showDiags((HTStream*)target, info->Diagnostics);

    if ( info->DocHeaders != 0 ) {
      for (k=0; info->DocHeaders[k] != 0; k++ ) {
	WAISDocumentHeader* head = info->DocHeaders[k];
	char * headline = trim_junk(head->Headline);
	any * docid = head->DocumentID;
	char * docname;			/* printable version of docid */
	i++;

/*	Make a printable string out of the document id.
*/
	if (PROT_TRACE)
	    fprintf(stderr, "HTWAIS:  %2ld: Score: %4ld, lines:%4ld '%s'\n", i,
		    (long int)(info->DocHeaders[k]->Score),
		    (long int)(info->DocHeaders[k]->Lines),
		    headline);

	START(HTML_LI);
	sprintf(line, "%4ld  %4ld  ",
	    head->Score,
	    head->Lines);
	PUTS(line);

	if (archie) {
	    char * www_name = WWW_from_archie(headline);
	    if (www_name) {
		HTStartAnchor(target, NULL, www_name);
		PUTS(headline);
		
		END(HTML_A);
		free(www_name);
	    } else {
		 PUTS(headline);
		 PUTS(" (bad file name)");
	    }
	} else { /* Not archie */
	    docname =  WWW_from_WAIS(docid);
	    if (docname) {
		char * dbname = HTEscape(database, URL_XPALPHAS);
                char types_array[1000]; /* bad */
                char *type_escaped;
                types_array[0] = 0;
                if (head->Types) {
                    int i;
                    for (i = 0; head->Types[i]; i++)
                      {
                        if (i)
                          strcat (types_array, ",");

                        type_escaped = HTEscape (head->Types[i], URL_XALPHAS);
                        strcat (types_array, type_escaped);
                        free (type_escaped);
                      }
                    if (PROT_TRACE)
                      fprintf (stderr, "WAIS........ Types_array `%s\'\n",
			       types_array);
		} else {
                    strcat (types_array, "TEXT");
		}
		sprintf(line, "%s/%s/%d/%s",
                        dbname,
                        types_array,
                        (int)(head->DocumentLength),
                        docname);
#ifdef OLD_CODE
		sprintf(line, "%s/%s/%d/%s",		/* W3 address */
				    dbname,
		    head->Types ? head->Types[0] : "TEXT",
		    (int)(head->DocumentLength),
		    docname);
#endif /* OLD_CODE */
		HTStartAnchor(target, NULL, ( (head->Types) 
		      && (!strcmp(head->Types[0], "URL"))) ? 
			      headline : line); /* NT, Sep 93 */
		PUTS(headline);
		END(HTML_A);
		free(dbname);
		free(docname);
	    } else {
		 PUTS("(bad doc id)");
	    }
	  }
      } /* next document header */
    } /* if there were any document headers */
    
    if ( info->ShortHeaders != 0 ) {
      k =0;
      while (info->ShortHeaders[k] != 0 ) {
	i++;
	PUTS( "(Short Header record, can't display)");
      }
    }
    if ( info->LongHeaders != 0 ) {
      k =0;
      while (info->LongHeaders[k] != 0) {
	i++;
	PUTS( "\nLong Header record, can't display\n");
      }
    }
    if ( info->Text != 0 ) {
      k =0;
      while (info->Text[k] != 0) {
	i++;
	PUTS( "\nText record\n");
	output_text_record((HTStream*)target, info->Text[k++], false, false);
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
  END(HTML_MENU);
  PUTC('\n'); ;
}




/*	Load Document from WAIS Server				HTLoadWAIS()
**	------------------------------
**
** On entry,
**	request		This is the request structure
** On exit,
**	returns		<0		Error has occured
**			HT_LOADED	OK
*/
PUBLIC int HTLoadWAIS ARGS1(HTRequest * , request)

#define MAX_KEYWORDS_LENGTH 4000
#define MAX_SERVER_LENGTH 1000
#define MAX_DATABASE_LENGTH 1000
#define MAX_SERVICE_LENGTH 1000

{
    CONST char * arg = HTAnchor_physical(request->anchor);
    HTFormat		format_out = request->output_format;
    HTStream*		sink = request->output_stream;
#if 0    
    static CONST char * error_header =
"<h1>Access error</h1>\nThe following error occured in accesing a WAIS server:<P>\n";
#endif
    char * key;			  /* pointer to keywords in URL */
    char* request_message = NULL; /* arbitrary message limit */
    char* response_message = NULL; /* arbitrary message limit */
    long request_buffer_length;	/* how of the request is left */
    SearchResponseAPDU  *retrieval_response = 0;
    char keywords[MAX_KEYWORDS_LENGTH + 1];
    char *server_name;	
    char *wais_database = NULL;		/* name of current database */
    char *www_database;			/* Same name escaped */
    char *service;
    char *doctype;
    char *doclength;
    long document_length;
    char *docname;
    FILE *connection = 0;
    char * names;		/* Copy of arg to be hacked up */
    BOOL ok = NO;
    int status = -1;
    char *basetitle = NULL;
    
    extern FILE * connect_to_server();
    
    if (!acceptable_inited) init_acceptable();
    
        
/*	Decipher and check syntax of WWW address:
**	----------------------------------------
**
**	First we remove the "wais:" if it was spcified.  920110
*/  
    names = HTParse(arg, "", PARSE_HOST | PARSE_PATH | PARSE_PUNCTUATION);
    key = strchr(names, '?');
    
    if (key) {
    	char * p;
	*key++ = 0;	/* Split off keywords */
	for (p=key; *p; p++) if (*p == '+') *p = ' ';
	HTUnEscape(key);
    }
    if (names[0]== '/') {
	server_name = names+1;
	if ((as_gate =(*server_name == '/')))
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
	char *unescaped = NULL;
	StrAllocCopy(unescaped, arg);
	HTUnEscape(unescaped);
	HTErrorAdd(request, ERR_FATAL, NO, HTERR_BAD_REQUEST,
		   (void *) unescaped, (int) strlen(unescaped),
		   "HTLoadWAIS");
	free(unescaped);
	free(names);
	return -1;
    }
    
    if (PROT_TRACE) fprintf(stderr, "HTLoadWAIS.. URL Parsed OK\n");
     
     service = strchr(names, ':');
     if (service)  *service++ = 0;
     else service = "210";
     
     if (server_name[0] == 0)
        connection = NULL;

     else if (!(key && !*key))
      if ((connection=connect_to_server(server_name,atoi(service))) == NULL)  {
	  char *host = HTParse(arg, "", PARSE_HOST);
	  if (PROT_TRACE)
	      fprintf (stderr, "HTLoadWAIS.. Can't open connection to %s via service %s.\n",
		       server_name, service);
	  HTErrorAdd(request, ERR_FATAL, NO, HTERR_WAIS_NO_CONNECT,
		     (void *) host, (int) strlen(host), "HTLoadWAIS");
	  goto cleanup;
      }

    StrAllocCopy(wais_database,www_database);
    HTUnEscape(wais_database);

    /* Make title name without the .src */
    {
	char *srcstr;
	StrAllocCopy(basetitle, wais_database);
	if ((srcstr = strstr(basetitle, ".src")) != NULL)
	    *srcstr = '\0';
    }
    
    /* This below fixed size stuff is terrible */
    request_message = (char*)s_malloc((size_t)MAX_MESSAGE_LEN * sizeof(char));
    response_message = (char*)s_malloc((size_t)MAX_MESSAGE_LEN * sizeof(char));

/*	If keyword search is performed but there are no keywords,
**	the user has followed a link to the index itself. It would be
**	appropriate at this point to send him the .SRC file - how?
*/

    if (key && !*key) {				/* I N D E X */
    
#ifdef CACHE_FILE_PREFIX
	char filename[256];
	FILE * fp;
#endif
	HTStructured * target = HTML_new(request, NULL,
					WWW_HTML, format_out, sink);
	
	{
	    START(HTML_TITLE);
	    PUTS(basetitle);
	    PUTS(" Index");
	    END(HTML_TITLE);
	    
	    START(HTML_H1);
	    PUTS("WAIS Index: ");
	    PUTS(basetitle);
	    END(HTML_H1);
	    
	}
	START(HTML_ISINDEX);

	/* If we have seen a source file for this database, use that: */

#ifdef CACHE_FILE_PREFIX
	sprintf(filename, "%sWSRC-%s:%s:%.100s.txt",
		CACHE_FILE_PREFIX,
		server_name, service, www_database);

	fp = fopen(filename, "r");	/* Have we found this already? */
	if (PROT_TRACE) fprintf(stderr,
		"HTLoadWAIS.. Description of server %s %s.\n",
		filename,
		fp ? "exists already" : "does NOT exist!");

	if (fp) {
	    int c;
	    START(HTML_PRE);			 /* Preformatted description */
	    while((c=getc(fp)) != EOF)
		PUTC(c);				    /* Transfer file */
	    END(HTML_PRE);
	    fclose(fp);
	}
#endif
	FREE_TARGET;
	
    } else if (key) {					/* S E A R C H */
	char *p;
	HTStructured * target;
	
	strncpy(keywords, key, MAX_KEYWORDS_LENGTH);
	while ((p = strchr(keywords,'+'))) *p = ' ';
    
        /* Send advance title to get something fast to the other end */
	
	target = HTML_new(request, NULL, WWW_HTML, format_out, sink);
	
	START(HTML_HTML);
	START(HTML_HEAD);
	START(HTML_TITLE);
	PUTS(keywords);
	PUTS(" in ");
	PUTS(basetitle);
	END(HTML_TITLE);
	END(HTML_HEAD);
	
	START(HTML_BODY);
	START(HTML_H1);
	PUTS("WAIS Search of \"");
	PUTS(keywords);
	PUTS("\" in ");
	PUTS(basetitle);
	END(HTML_H1);

	START(HTML_ISINDEX);

	request_buffer_length = MAX_MESSAGE_LEN; /* Amount left */
	if (PROT_TRACE)
	    fprintf(stderr, "HTLoadWAIS.. Search for `%s' in `%s'\n",
		    keywords, wais_database);
	if(generate_search_apdu(request_message + HEADER_LENGTH, 
				&request_buffer_length, 
				keywords, wais_database, NULL,
				HTMaxWAISLines) == NULL) {
	    if (PROT_TRACE)
		fprintf(stderr, "WAIS Search. Too many lines in response\n");
	    HTErrorAdd(request, ERR_WARNING, NO, HTERR_WAIS_OVERFLOW, 
		       NULL, 0, "HTLoadWAIS");
	}

	if(!interpret_message(request_message, 
				MAX_MESSAGE_LEN - request_buffer_length, 
				response_message,
				MAX_MESSAGE_LEN,
				connection,
				false	/* true verbose */
				)) {
	    if (PROT_TRACE)
		fprintf(stderr, "WAIS Search. Too many lines in response\n");
	    HTErrorAdd(request, ERR_WARNING, NO, HTERR_WAIS_OVERFLOW, 
		       NULL, 0, "HTLoadWAIS");
        } else {	/* returned message ok */
	    SearchResponseAPDU  *query_response = 0;
	    readSearchResponseAPDU(&query_response,
	    	response_message + HEADER_LENGTH);
	    display_search_response(target, 
	    	query_response, wais_database, keywords);
	    if (query_response->DatabaseDiagnosticRecords)
		freeWAISSearchResponse(
			query_response->DatabaseDiagnosticRecords);         
	    freeSearchResponseAPDU( query_response);
	}	/* returned message not too large */
    
	END(HTML_BODY);
	END(HTML_HTML);
	FREE_TARGET;

    } else {			/* D O C U M E N T    F E T C H */
    
	HTFormat format_in;
	boolean binary;     /* how to transfer stuff coming over */
	HTStream * target;
	long count;
	any   doc_chunk;
	any * docid = &doc_chunk;
	if (PROT_TRACE)
	    fprintf(stderr, 
		    "HTLoadWAIS.. Retrieve document `%s'\n............ type `%s' length %ld\n", docname, doctype, document_length);
		
	format_in = 
	  !strcmp(doctype, "WSRC") ? HTAtom_for("application/x-wais-source") :
	  !strcmp(doctype, "TEXT") ? WWW_UNKNOWN :
	  !strcmp(doctype, "HTML") ? HTAtom_for("text/html") :
	  !strcmp(doctype, "GIF")  ? HTAtom_for("image/gif") :
	   		             HTAtom_for("application/octet-stream");
	binary = 
	  0 != strcmp(doctype, "WSRC") &&
	  0 != strcmp(doctype, "TEXT") &&
	  0 != strcmp(doctype, "HTML") ;

	/* Guess on TEXT format as it might be HTML */
	if ((target = HTStreamStack(format_in, request, YES)) == NULL) {
	    status = -1;
	    goto cleanup;
	}

	/* Decode hex or litteral format for document ID */
	WAIS_from_WWW(docid, docname);

	/* Loop over slices of the document */
	for (count = 0; count * CHARS_PER_PAGE < document_length; count++) {
	    char *type = s_strdup(doctype);
	    request_buffer_length = MAX_MESSAGE_LEN;	      /* Amount left */
	    if (PROT_TRACE) fprintf(stderr, "HTLoadWAIS.. Slice number %ld\n",
			       count);
	    if (generate_retrieval_apdu(request_message + HEADER_LENGTH,
					&request_buffer_length, 
					docid, CT_byte,
					count * CHARS_PER_PAGE,
					HTMIN((count + 1) * CHARS_PER_PAGE,
					      document_length),
					type,
					wais_database) == 0) {
		HTErrorAdd(request, ERR_WARNING, NO, HTERR_WAIS_OVERFLOW, 
			   NULL, 0, "HTLoadWAIS");
	    }
	    FREE(type);
	    
	    /* Actually do the transaction given by request_message */   
	    if (interpret_message(request_message, 
				  MAX_MESSAGE_LEN - request_buffer_length, 
				  response_message,
				  MAX_MESSAGE_LEN,
				  connection,
				  false /* true verbose */	
				  ) == 0) {
		HTErrorAdd(request, ERR_WARNING, NO, HTERR_WAIS_OVERFLOW, 
			   NULL, 0, "HTLoadWAIS");
	    }
	    
	    /* Parse the result which came back into memory. */
	    readSearchResponseAPDU(&retrieval_response, 
				   response_message + HEADER_LENGTH);
	    {
		WAISSearchResponse *searchres = (WAISSearchResponse *) retrieval_response->DatabaseDiagnosticRecords;
		if (!searchres->Text) {
		    if (searchres->Diagnostics && *searchres->Diagnostics &&
			(*searchres->Diagnostics)->ADDINFO) {
			char *errmsg = (*searchres->Diagnostics)->ADDINFO;
			HTErrorAdd(request, ERR_WARNING, NO, HTERR_WAIS_MODULE,
				   (void *) errmsg, (int) strlen(errmsg),
				   "HTLoadWAIS");
		    } else {
			HTErrorAdd(request, ERR_WARNING, NO, HTERR_WAIS_MODULE,
				   NULL, 0, "HTLoadWAIS");
		    }
		    (*target->isa->_free)(target);
		    request->output_stream = NULL;
		    free (docid->bytes);
		    freeWAISSearchResponse(retrieval_response->DatabaseDiagnosticRecords); 
		    freeSearchResponseAPDU( retrieval_response);
		    goto cleanup;
		} else {
		    output_text_record(target, *searchres->Text,
				       false, binary);
		    freeWAISSearchResponse( retrieval_response->DatabaseDiagnosticRecords);
		    freeSearchResponseAPDU( retrieval_response);
		} /* If text existed */
	    }
	    
	} /* Loop over slices */

	(*target->isa->_free)(target);
	request->output_stream = NULL;
	free (docid->bytes);
    } /* If document rather than search */
    status = HT_LOADED;

  cleanup:
    if (connection) close_connection(connection);
    if (wais_database) free(wais_database);
    if (request_message) s_free(request_message);
    if (response_message) s_free(response_message);
    FREE(names);
    FREE(basetitle);
    if (status < 0) {
	char *unescaped = NULL;
	StrAllocCopy(unescaped, arg);
	HTUnEscape(unescaped);
	HTErrorAdd(request, ERR_FATAL, NO, HTERR_INTERNAL, (void *) unescaped,
		   (int) strlen(unescaped), "HTLoadWAIS");
	free(unescaped);
    }
    return status;
}

GLOBALDEF PUBLIC HTProtocol HTWAIS = { "wais", HTLoadWAIS, NULL };


