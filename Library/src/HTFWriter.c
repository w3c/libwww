/*		FILE WRITER				HTFWrite.h
**		===========
**
**	This version of the stream object just writes to a C file.
**	The file is assumed open and left open.
**
**	Bugs:
**		strings written must be less than buffer size.
*/

#include "HTFWriter.h"

#include "HTFormat.h"
#include "HTAlert.h"
#include "HTFile.h"

/*		HTML Object
**		-----------
*/

struct _HTStream {
	CONST HTStreamClass *	isa;
	
	FILE *			fp;
	char * 			end_command;
	char * 			remove_command;
};


/*_________________________________________________________________________
**
**			A C T I O N 	R O U T I N E S
**  Bug:
**	All errors are ignored.
*/

/*	Character handling
**	------------------
*/

PRIVATE void HTFWriter_put_character ARGS2(HTStream *, this, char, c)
{
    putc(c, this->fp);
}



/*	String handling
**	---------------
**
**	Strings must be smaller than this buffer size.
*/
PRIVATE void HTFWriter_put_string ARGS2(HTStream *, this, CONST char*, s)
{
    fputs(s, this->fp);
}


/*	Buffer write.  Buffers can (and should!) be big.
**	------------
*/
PRIVATE void HTFWriter_write ARGS3(HTStream *, this, CONST char*, s, int, l)
{
    fwrite(s, 1, l, this->fp); 
}




/*	Free an HTML object
**	-------------------
**
**	Note that the SGML parsing context is freed, but the created
**	object is not,
**	as it takes on an existence of its own unless explicitly freed.
*/
PRIVATE void HTFWriter_free ARGS1(HTStream *, this)
{
    fflush(this->fp);
    if (this->end_command) {		/* Temp file */
    	fclose(this->fp);
        HTProgress(this->end_command);	/* Tell user what's happening */
	system(this->end_command);
	free (this->end_command);
	if (this->remove_command) {
	    system(this->remove_command);
	    free(this->remove_command);
	}
    }

    free(this);
}

/*	End writing
*/

PRIVATE void HTFWriter_end_document ARGS1(HTStream *, this)
{
    fflush(this->fp);
}



/*	Structured Object Class
**	-----------------------
*/
PUBLIC CONST HTStreamClass HTFWriter = /* As opposed to print etc */
{		
	"FileWriter",
	HTFWriter_free,
	HTFWriter_end_document,
	HTFWriter_put_character, 	HTFWriter_put_string,
	HTFWriter_write
}; 


/*	Subclass-specific Methods
**	-------------------------
*/

PUBLIC HTStream* HTFWriter_new ARGS1(FILE *, fp)
{
    HTStream* this;
    
    if (!fp) return NULL;

    this = malloc(sizeof(*this));
    if (this == NULL) outofmem(__FILE__, "HTML_new");
    this->isa = &HTFWriter;       

    this->fp = fp;
    this->end_command = NULL;
    this->remove_command = NULL;

    return this;
}

/*	Make system command from template
**	---------------------------------
**
**	See mailcap spec for description of template.
*/
/* @@ to be written.  sprintfs will do for now.  */

/*	Take action using a system command
**	----------------------------------
**
**	originally from Ghostview handling by Marc Andreseen.
**	Creates temporary file, writes to it, executes system command
**	on end-document.  The suffix of the temp file can be given
**	in case the application is fussy, or so that a generic opener can
**	be used.
*/
PUBLIC HTStream* HTSaveAndExecute ARGS3(
	HTPresentation *,	pres,
	HTParentAnchor *,	anchor,	/* Not used */
	HTStream *,		sink)	/* Not used */

#ifdef unix
#define REMOVE_COMMAND "/bin/rm -f %s\n"
{
    char *fnam;
    CONST char * suffix;
    
    HTStream* this;
    
    this = malloc(sizeof(*this));
    if (this == NULL) outofmem(__FILE__, "Save and execute");
    this->isa = &HTFWriter;  
    
    /* Save the file under a suitably suffixed name */
    
    suffix = HTFileSuffix(pres->rep);

    fnam = (char *)malloc (L_tmpnam + 16 + strlen(suffix));
    tmpnam (fnam);
    if (suffix) strcat(fnam, suffix);
    
    this->fp = fopen (fnam, "w");
    if (!this->fp) {
	HTAlert("Can't open temporary file!");
        free(fnam);
	free(this);
	return NULL;
    }

/*	Make command to process file
*/
    this->end_command = (char *)malloc (
    			(strlen (pres->command) + 10+ 3*strlen(fnam))
    			 * sizeof (char));
    if (this == NULL) outofmem(__FILE__, "SaveAndExecute");
    
    sprintf (this->end_command, pres->command, fnam, fnam, fnam);

    this->remove_command = NULL;	/* If needed, put into end_command */
#ifdef NOPE
/*	Make command to delete file
*/ 
    this->remove_command = (char *)malloc (
    			(strlen (REMOVE_COMMAND) + 10+ strlen(fnam))
    			 * sizeof (char));
    if (this == NULL) outofmem(__FILE__, "SaveAndExecute");
    
    sprintf (this->remove_command, REMOVE_COMMAND, fnam);
#endif

    free (fnam);
    return this;
}

#else	/* Not unix */
{ return NULL; }
#endif


/*	Format Converter using system command
**	-------------------------------------
*/
