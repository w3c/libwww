/*		FILE WRITER			HTWrite.c
**		===========
**
*/
#include "HTWriter.h"

#define BUFFER_SIZE 4096	/* Tradeoff */

#include "HTUtils.h"
#include "tcp.h"
#include <stdio.h>


/*		HTML Object
**		-----------
*/

struct _HTStream {
	CONST HTStreamClass *	isa;

	int	soc;
	char	*write_pointer;
	char 	buffer[BUFFER_SIZE];
#ifdef NOT_ASCII
	BOOL	make_ascii;	/* Are we writing to the net? */
#endif
};


/*	Write the buffer out to the socket
**	----------------------------------
*/

PRIVATE void flush ARGS1(HTStream *, this)
{
    char *read_pointer 	= this->buffer;
    char *write_pointer = this->write_pointer;

#ifdef NOT_ASCCII
    if (this->make_ascii) {
    	char * p;
	for(p = this->buffer; p < this->write_pointer; p++)
	    *p = TOASCII(*p);
    }
#endif
    while (read_pointer < write_pointer) {
        int status;
	status = NETWRITE(this->soc, this->buffer,
			write_pointer - read_pointer);
	if (status<0) {
	    if(TRACE) fprintf(stderr,
	    "HTWrite: Error on socket output stream!!!\n");
	    return;
	}
	read_pointer = read_pointer + status;
    }
    this->write_pointer = this->buffer;
}


/*_________________________________________________________________________
**
**			A C T I O N 	R O U T I N E S
*/

/*	Character handling
**	------------------
*/

PRIVATE void HTWriter_put_character ARGS2(HTStream *, this, char, c)
{
    if (this->write_pointer == &this->buffer[BUFFER_SIZE]) flush(this);
    *this->write_pointer++ = c;
}



/*	String handling
**	---------------
**
**	Strings must be smaller than this buffer size.
*/
PRIVATE void HTWriter_put_string ARGS2(HTStream *, this, CONST char*, s)
{
    int l = strlen(s);
    if (this->write_pointer + l > &this->buffer[BUFFER_SIZE]) flush(this);
    strcpy(this->write_pointer, s);
    this->write_pointer = this->write_pointer + l;
}


/*	Buffer write.  Buffers can (and should!) be big.
**	------------
*/
PRIVATE void HTWriter_write ARGS3(HTStream *, this, CONST char*, s, int, l)
{
 
    CONST char *read_pointer 	= s;
    CONST char *write_pointer = s+l;

    flush(this);		/* First get rid of our buffer */

    while (read_pointer < write_pointer) {
        int status = NETWRITE(this->soc, read_pointer,
			write_pointer - read_pointer);
	if (status<0) {
	    if(TRACE) fprintf(stderr,
	    "HTWriter_write: Error on socket output stream!!!\n");
	    return;
	}
	read_pointer = read_pointer + status;
    }
}




/*	Free an HTML object
**	-------------------
**
**	Note that the SGML parsing context is freed, but the created object is not,
**	as it takes on an existence of its own unless explicitly freed.
*/
PRIVATE void HTWriter_free ARGS1(HTStream *, this)
{
    NETCLOSE(this->soc);
    flush(this);
    free(this);
}

/*	End writing
*/

PRIVATE void HTWriter_end_document ARGS1(HTStream *, this)
{
    flush(this);
}



/*	Structured Object Class
**	-----------------------
*/
PUBLIC CONST HTStreamClass HTWriter = /* As opposed to print etc */
{		
	"SocketWriter",
	HTWriter_free,
	HTWriter_end_document,
	HTWriter_put_character, 	HTWriter_put_string,
	HTWriter_write
}; 


/*	Subclass-specific Methods
**	-------------------------
*/

PUBLIC HTStream* HTWriter_new ARGS1(int, soc)
{
    HTStream* this = malloc(sizeof(*this));
    if (this == NULL) outofmem(__FILE__, "HTML_new");
    this->isa = &HTWriter;       
    
#ifdef NOT_ASCII
    this->make_ascii = NO;
#endif    
    this->soc = soc;
    this->write_pointer = this->buffer;
    return this;
}

/*	Subclass-specific Methods
**	-------------------------
*/

PUBLIC HTStream* HTASCIIWriter ARGS1(int, soc)
{
    HTStream* this = malloc(sizeof(*this));
    if (this == NULL) outofmem(__FILE__, "HTML_new");
    this->isa = &HTWriter;       

#ifdef NOT_ASCII
    this->make_ascii = YES;
#endif    
    this->soc = soc;
    this->write_pointer = this->buffer;
    return this;
}
