#include "HTFormat_glue.h"

#define max_keyname 20
#define bad_vars "Invalid variable names or non-existent entries."
#define err_string "wrong # args: should be " 


Tcl_HashTable HTableList;
Tcl_HashTable HTableConverter;
Tcl_HashTable HTableCoder;
Tcl_HashTable HTableStream;
Tcl_HashTable HTableReq;
Tcl_HashTable HTableVoid;

/******************************
 *       HTFormat             *
 * stream pipe manager        *
 *****************************/

/* Register Presenters */



int HTPresentation_add_tcl(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {
  if (argc == 8) {
    char *keyname = argv[1];
    const char *representation = argv[2];
    const char *command = argv[3];
    const char *test_command=argv[4];
    double quality;
    double secs;
    double secs_per_byte;
    int success1=Tcl_GetDouble(interp, argv[5], &quality);
    int success2=Tcl_GetDouble(interp, argv[6], &secs);
    int success3=Tcl_GetDouble(interp, argv[7], &secs_per_byte);

    if(keyname && representation && command && test_command &&
       (success1 == TCL_OK) && (success2 == TCL_OK) &&
       (success3 == TCL_OK)) {
      Tcl_HashEntry *entryPtr = Tcl_FindHashEntry(&HTableList, keyname);
      if(entryPtr) {
	HTList * conversions = Tcl_GetHashValue(entryPtr);
	
	HTPresentation_add(conversions, representation, command, 
			   test_command, quality, secs, 
			   secs_per_byte);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}



int HTPresentation_deleteAll_tcl(ClientData clientData, Tcl_Interp *interp,
				 int argc, char **argv) {
  if(argc==2) {
    char *list_key = argv[1];
    if(list_key) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      if(list_entry) {
	HTList *list = Tcl_GetHashValue(list_entry);
	HTPresentation_deleteAll(list);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else{
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}
	

/*Register Converters*/

int HTConversion_add_tcl(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {
  if (argc == 8) {
    char *list_key = argv[1];
    const char *rep_in = argv[2];
    const char *rep_out = argv[3];
    char *converter_key=argv[4];
    double quality;
    double secs;
    double secs_per_byte;
    int success1=Tcl_GetDouble(interp, argv[5], &quality);
    int success2=Tcl_GetDouble(interp, argv[6], &secs);
    int success3=Tcl_GetDouble(interp, argv[7], &secs_per_byte);

    if(list_key && rep_in && rep_out && converter_key &&
       (success1 == TCL_OK) && (success2 == TCL_OK) && 
       (success3 == TCL_OK)) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      Tcl_HashEntry *converter_entry = Tcl_FindHashEntry(&HTableConverter, 
							 converter_key);
      if(list_entry && converter_entry ) {
	HTList * conversions = Tcl_GetHashValue(list_entry);
	HTConverter * converter = (HTConverter *) Tcl_GetHashValue(converter_entry);
	
	HTConversion_add(conversions, rep_in, rep_out, 
			 converter, quality, secs, secs_per_byte);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


int HTConversion_deleteAll_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv) {
  if (argc == 2) {
    char *list_key = argv[1];
    if(list_key) {
      Tcl_HashEntry * list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      if (list_entry) {
	HTList *list = Tcl_GetHashValue(list_entry);
	
	HTConversion_deleteAll(list);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


/*Register Content Coders */

/* not really sure about what to do with HTCoding*/

int HTCoding_add_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv){
  if (argc == 6) {
    char *list_key = argv[1];
    const char * encoding = argv[2];
    char *encoder_key = argv[3];
    char *decoder_key = argv[4];
    double quality;
    int success = Tcl_GetDouble(interp, argv[5], &quality);
    
    if(list_key && encoding && encoder_key && decoder_key &&
       (success == TCL_OK)) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      Tcl_HashEntry *encoder_entry = Tcl_FindHashEntry(&HTableCoder, 
						       encoder_key);
      Tcl_HashEntry *decoder_entry = Tcl_FindHashEntry(&HTableCoder, 
						       decoder_key);
      if(list_entry && encoder_entry && decoder_entry) {
	HTList *list = Tcl_GetHashValue(list_entry);
	HTCoder *encoder = (HTCoder *) Tcl_GetHashValue(encoder_entry);
	HTCoder *decoder = (HTCoder *) Tcl_GetHashValue(decoder_entry);
	
        BOOL result = HTCoding_add(list, encoding, encoder, decoder, 
				   quality);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}



int HTCoding_deleteAll_tcl(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {
  if (argc ==2) {
    char *list_key = argv[1];
    if (list_key) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      if (list_entry) {
	HTList *list = Tcl_GetHashValue(list_entry);
	HTCoding_deleteAll(list);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}
  
/*this function is not done yet.*/

/*extern const char * HTCoding_name (HTCoding * me);*/


/*Content Charsets */

/*Register a Charset*/

int HTCharset_add_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 4) {
    char *list_key = argv[1];
    const char * charset = argv[2];
    double quality;
    int success = Tcl_GetDouble(interp, argv[3], &quality);
    if(list_key && charset && (success == TCL_OK)) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      if(list_entry) {
	HTList * list = Tcl_GetHashValue(list_entry);
	HTCharset_add(list, charset, quality);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


/*Delete a list of Charsets */


int HTCharset_deleteAll_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 2) {
    char *list_key = argv[1];

    if(list_key) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      if(list_entry) {
	HTList * list = Tcl_GetHashValue(list_entry);
	HTCharset_deleteAll(list);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


/* Content Languages */

/* Register a Language */



int HTLanguage_add_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 4) {
    char *list_key = argv[1];
    const char * language = argv[2];
    double quality;
    int success = Tcl_GetDouble(interp, argv[3], &quality);
    if(list_key && language && (success == TCL_OK)) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      if(list_entry) {
	HTList * list = Tcl_GetHashValue(list_entry);
	HTLanguage_add(list, language, quality);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


/*Delete a list of Languages */

extern void HTLanguage_deleteAll (HTList * list);

int HTLanguage_deleteAll_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 2) {
    char *list_key = argv[1];
    if(list_key) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      if(list_entry) {
	HTList * list = Tcl_GetHashValue(list_entry);
	HTLanguage_deleteAll(list);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}

/*Global Preferences*/


/*Converters and Presenters*/


int HTFormat_setConversion_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv){
  if (argc == 2) {
    char *list_key = argv[1];
    if(list_key) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      if(list_entry) {
	HTList *list = Tcl_GetHashValue(list_entry);
	HTFormat_setConversion(list);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}



int HTFormat_conversion_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 1) {
    char *keyname;

    HTList *list = HTFormat_conversion();
    
    Tcl_HashEntry *list_entry = CreateNewListEntry("conversion_list", 
						   &keyname);

    Tcl_SetHashValue(list_entry, list);
    
    Tcl_AppendResult(interp, keyname, NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}



/* not sure what to do about the arguement HTConverter */


int HTFormat_addConversion_tcl(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {
  if (argc == 7) {
    const char * input_format = argv[1];
    const char * output_format = argv[2];
    char * converter_key = argv[3];
    double quality;
    double secs;
    double secs_per_byte;
    int success1 = Tcl_GetDouble(interp, argv[4], &quality);
    int success2 = Tcl_GetDouble(interp, argv[5], &secs);
    int success3 = Tcl_GetDouble(interp, argv[6], &secs_per_byte);

    if(input_format && output_format && converter_key &&
       (success1 == TCL_OK) && (success2 == TCL_OK) && 
       (success3 == TCL_OK)) {
      Tcl_HashEntry *converter_entry = Tcl_FindHashEntry(&HTableConverter, 
							 converter_key);
      if(converter_entry) {
	HTConverter *converter =(HTConverter *) Tcl_GetHashValue(converter_entry);
	
	HTFormat_addConversion(input_format, output_format, converter, 
			       quality, secs, secs_per_byte);
	
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}

    

/* Content Codings */


int HTFormat_setContentCoding_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv){
  if (argc == 2) {
    char *list_key = argv[1];
    if(list_key) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      if(list_entry) {
	HTList *list = Tcl_GetHashValue(list_entry);
	HTFormat_setContentCoding(list);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


int HTFormat_contentCoding_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 1) {
    char *keyname;

    HTList *list = HTFormat_contentCoding();
    
    Tcl_HashEntry *list_entry = CreateNewListEntry("contentCoding_list", 
						   &keyname);

    Tcl_SetHashValue(list_entry, list);
    
    Tcl_AppendResult(interp, keyname, NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}




int HTFormat_addCoding_tcl(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {
  if (argc == 5) {
    char * encoding = argv[1];
    char * encoder_key = argv[2];
    char * decoder_key = argv[3];
    double quality;
    int success = Tcl_GetDouble(interp, argv[4], &quality);
    
    if (encoding && encoder_key && decoder_key &&
	success == TCL_OK ) {
      Tcl_HashEntry *encoder_entry = Tcl_FindHashEntry (&HTableCoder, 
							encoder_key);
      Tcl_HashEntry *decoder_entry = Tcl_FindHashEntry (&HTableCoder, 
							decoder_key);

      if (encoder_entry && decoder_entry) {
	HTCoder *encoder = (HTCoder *) Tcl_GetHashValue(encoder_entry);
	HTCoder *decoder = (HTCoder *) Tcl_GetHashValue(decoder_entry);
	
	BOOL result = HTFormat_addCoding(encoding, encoder, decoder, 
					 quality);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}
	
	


/* Content Transfer Codings */



int HTFormat_setTransferCoding_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv){
  if (argc == 2) {
    char *list_key = argv[1];
    if(list_key) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      if(list_entry) {
	HTList *list = Tcl_GetHashValue(list_entry);
	HTFormat_setTransferCoding(list);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}




int HTFormat_transferCoding_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 1) {
    char *keyname;

    HTList *list = HTFormat_transferCoding();
    
    Tcl_HashEntry *list_entry = CreateNewListEntry("transferCoding_list", 
						   &keyname);

    Tcl_SetHashValue(list_entry, list);
    
    Tcl_AppendResult(interp, keyname, NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}




int HTFormat_addTransferCoding_tcl(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {
  if (argc == 5) {
    char * encoding = argv[1];
    char * encoder_key = argv[2];
    char * decoder_key = argv[3];
    double quality;
    int success = Tcl_GetDouble(interp, argv[4], &quality);
    
    if (encoding && encoder_key && decoder_key &&
	success == TCL_OK ) {
      Tcl_HashEntry *encoder_entry = Tcl_FindHashEntry (&HTableCoder, 
							encoder_key);
      Tcl_HashEntry *decoder_entry = Tcl_FindHashEntry (&HTableCoder, 
							decoder_key);

      if (encoder_entry && decoder_entry) {
	HTCoder *encoder = (HTCoder *) Tcl_GetHashValue(encoder_entry);
	HTCoder *decoder = (HTCoder *) Tcl_GetHashValue(decoder_entry);
	
	BOOL result = HTFormat_addTransferCoding(encoding, encoder, decoder, 
					 quality);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}




/*Content Languages */


int HTFormat_setLanguage_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv){
  if (argc == 2) {
    char *list_key = argv[1];
    if(list_key) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      if(list_entry) {
	HTList *list = Tcl_GetHashValue(list_entry);
	HTFormat_setLanguage(list);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


int HTFormat_language_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 1) {
    char *keyname;

    HTList *list = HTFormat_language();
    
    Tcl_HashEntry *list_entry = CreateNewListEntry("language_list", 
						   &keyname);

    Tcl_SetHashValue(list_entry, list);
    
    Tcl_AppendResult(interp, keyname, NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


/*Content Charsets */


int HTFormat_setCharset_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv){
  if (argc == 2) {
    char *list_key = argv[1];
    if(list_key) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      if(list_entry) {
	HTList *list = Tcl_GetHashValue(list_entry);
	HTFormat_setCharset(list);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


int HTFormat_charset_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 1) {
    char *keyname;

    HTList *list = HTFormat_charset();
    
    Tcl_HashEntry *list_entry = CreateNewListEntry("charset_list", 
						   &keyname);

    Tcl_SetHashValue(list_entry, list);
    
    Tcl_AppendResult(interp, keyname, NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


/*Delete All Global Lists */


int HTFormat_deleteAll_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if(argc == 1) {
    HTFormat_deleteAll();
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


/* The Content Type Stream Stack */



int HTStreamStack_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 6) {
    char * rep_in_string = argv[1];
    char * rep_out_string = argv[2];
    char * output_stream_key = argv[3];
    char * request_key = argv[4];
    int guess;
    int success = Tcl_GetBoolean(interp, argv[5], &guess);

    if(rep_in_string && rep_out_string && output_stream_key &&
       request_key && (success == TCL_OK) ) {
      HTFormat rep_in = HTAtom_for(rep_in_string);
      HTFormat rep_out = HTAtom_for(rep_out_string);
      Tcl_HashEntry *output_stream_entry = Tcl_FindHashEntry(&HTableStream, 
							    output_stream_key);

      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq,
						       request_key);
      if(output_stream_entry && request_entry) {
	HTStream * output_stream = Tcl_GetHashValue(output_stream_entry);
	HTRequest * request = Tcl_GetHashValue(request_entry);
	
	HTStream * result = HTStreamStack(rep_in, rep_out, output_stream,
					  request, (BOOL) guess);
	char *keyname;
	Tcl_HashEntry *entryPtr = CreateNewStreamEntry("StreamStack", 
						       &keyname);
	
	Tcl_SetHashValue(entryPtr, result);
	
	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}



/* Cost of a Stream Stack */


int HTStackValue_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {
  if (argc == 6) {
    char *list_key = argv[1];
    char *format_in_string = argv[2];
    char *format_out_string = argv[3];
    double initial_value;
    int success1 = Tcl_GetDouble(interp, argv[5], &initial_value);

    /* problem: how to get long integer from string */
    
    long int length;
    int tmp_length;
    int success2 = Tcl_GetInt(interp, argv[6], &tmp_length);
    if (list_key && format_in_string && format_out_string && 
	(success1 == TCL_OK) && (success2 == TCL_OK)) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      HTFormat format_in = HTAtom_for(format_in_string);
      HTFormat format_out = HTAtom_for(format_out_string);
      length = (long int) tmp_length;

      if (list_entry) {
	HTList * conversions = Tcl_GetHashValue(list_entry);
	
        double result = HTStackValue(conversions, format_in, format_out, 
				     initial_value, length);
	char *storage = malloc(2 * max_keyname * sizeof(char) );
	
	sprintf(storage, "%f", result);
	
	Tcl_AppendResult(interp, storage, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}
      





/* question, what to do about HTEncoding, I think it is HTAtom */


int HTContentCodingStack_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if ( argc == 6) {
    char *coding_key = argv[1];
    char *stream_key = argv[2];
    char *request_key= argv[3];
    char *void_key = argv[4];
    int encoding;
    int success = Tcl_GetBoolean(interp, argv[5], &encoding);
    if (coding_key && stream_key && request_key && void_key 
	&& (success == TCL_OK)) {
      HTEncoding coding = HTAtom_for(coding_key);
      Tcl_HashEntry *stream_entry = Tcl_FindHashEntry(&HTableStream,
						      stream_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq,
						     request_key);
      Tcl_HashEntry *void_entry = Tcl_FindHashEntry(&HTableVoid, 
						     void_key);
      if(stream_entry && request_entry && void_entry) {
	
	char *keyname;

	HTStream *target = Tcl_GetHashValue(stream_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);
	void *param = Tcl_GetHashValue(void_entry);

	HTStream *result = HTContentCodingStack(coding, target, 
						request, param, 
						encoding);

	Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableStream, 
						 "Stream", 
						 &keyname);
	Tcl_SetHashValue(entryPtr, result);
	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}




int HTContentEncodingStack_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if ( argc == 5) {
    char *list_key = argv[1];
    char *stream_key = argv[2];
    char *request_key= argv[3];
    char *void_key = argv[4];

    if (list_key && stream_key && request_key && void_key){
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, 
						    list_key);

      Tcl_HashEntry *stream_entry = Tcl_FindHashEntry(&HTableStream,
						      stream_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq,
						     request_key);
      Tcl_HashEntry *void_entry = Tcl_FindHashEntry(&HTableVoid, 
						     void_key);
      if(list_entry && stream_entry && request_entry && void_entry) {
	
	char *keyname;
	HTList *encodings = Tcl_GetHashValue(list_entry);
	HTStream *target = Tcl_GetHashValue(stream_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);
	void *param = Tcl_GetHashValue(void_entry);

	HTStream *result = HTContentEncodingStack(encodings, target, 
						request, param);

	Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableStream, 
						 "Stream", 
						 &keyname);
	Tcl_SetHashValue(entryPtr, result);
	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


int HTContentDecodingStack_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if ( argc == 5) {
    char *list_key = argv[1];
    char *stream_key = argv[2];
    char *request_key= argv[3];
    char *void_key = argv[4];

    if (list_key && stream_key && request_key && void_key){
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, 
						    list_key);

      Tcl_HashEntry *stream_entry = Tcl_FindHashEntry(&HTableStream,
						      stream_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq,
						     request_key);
      Tcl_HashEntry *void_entry = Tcl_FindHashEntry(&HTableVoid, 
						     void_key);
      if(list_entry && stream_entry && request_entry && void_entry) {
	
	char *keyname;
	HTList *encodings = Tcl_GetHashValue(list_entry);
	HTStream *target = Tcl_GetHashValue(stream_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);
	void *param = Tcl_GetHashValue(void_entry);

	HTStream *result = HTContentDecodingStack(encodings, target, 
						request, param);

	Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableStream, 
						 "Stream", 
						 &keyname);
	Tcl_SetHashValue(entryPtr, result);
	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}




int HTTransferCodingStack_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if ( argc == 6) {
    char *coding_key = argv[1];
    char *stream_key = argv[2];
    char *request_key= argv[3];
    char *void_key = argv[4];
    int encoding;
    int success = Tcl_GetBoolean(interp, argv[5], &encoding);
    if (coding_key && stream_key && request_key && void_key 
	&& (success == TCL_OK)) {
      HTEncoding coding = HTAtom_for(coding_key);
      Tcl_HashEntry *stream_entry = Tcl_FindHashEntry(&HTableStream,
						      stream_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq,
						     request_key);
      Tcl_HashEntry *void_entry = Tcl_FindHashEntry(&HTableVoid, 
						     void_key);
      if(stream_entry && request_entry && void_entry) {
	
	char *keyname;

	HTStream *target = Tcl_GetHashValue(stream_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);
	void *param = Tcl_GetHashValue(void_entry);

	HTStream *result = HTTransferCodingStack(coding, target, 
						request, param, 
						encoding);

	Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableStream, 
						 "Stream", 
						 &keyname);
	Tcl_SetHashValue(entryPtr, result);
	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}
