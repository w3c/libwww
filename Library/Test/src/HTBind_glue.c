/*xing's first attempt at writing glue codes*/
/* alex editted for compilation error 8/1/96 */

#include "HTBind_glue.h"

#define max_keyname 20
#define bad_vars "Invalid variable names or non-existant entries"
#define err_string "wrong # args: should be "


/******************************
 *           HTBind           *
 *****************************/
Tcl_HashTable   HTableAnchor;


/*Initialization of the Module*/


int HTBind_int_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv) {
    if (argc == 1) {
	    BOOL result             = HTBind_init();
	    
	    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	    return TCL_OK;
    }
    else {
      Tcl_AppendResult(interp, err_string, argv[0], NULL);
      return TCL_ERROR;
    }
}




int HTBind_deleteAll_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv) {
    if (argc == 1) {
	    BOOL result             = HTBind_deleteAll();
	    
	    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	    return TCL_OK;
    }
    else {
      Tcl_AppendResult(interp, err_string, argv[0], NULL);
      return TCL_ERROR;
    }
}



/*case sensitivity*/


int HTBind_caseSensitive_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv) {
    if (argc == 2) {
        int conversion;
        BOOL *sensitive = malloc (sizeof(BOOL));
        conversion = Tcl_GetBoolean(interp, argv[1], sensitive);
	if (conversion == TCL_OK) {
	  HTBind_caseSensitive(*sensitive);
	  return TCL_OK;
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;
    }
    else {
      Tcl_AppendResult(interp, err_string, argv[0], NULL);
      return TCL_ERROR;
    }
}


/*Suffix Delimitters*/

int HTBind_delimiters_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv) {
    if (argc == 1) {
      const char *result=HTBind_delimiters();
      Tcl_AppendResult(interp, result, NULL);
      return TCL_OK;
    }
    else {
      Tcl_AppendResult(interp, err_string, argv[0], NULL);
      return TCL_ERROR;
    }
}

int HTBind_setDelimiters_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv) {
    if (argc == 2) {
      if(argv[1]){
	HTBind_setDelimiters(argv[1]);
	return TCL_OK;
      }
      Tcl_AppendResult(interp, bad_vars, NULL);
      return TCL_ERROR;
    }
    else {
      Tcl_AppendResult(interp, err_string, argv[0], NULL);
      return TCL_ERROR;
    }
}


/*Set up Bindings Associated with a File Suffix*/


int HTBind_add_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv) {
    if (argc == 7) {
      char *suffix 	   = argv[1];
      char *representation = argv[2];
      char *encoding = argv[3];
      char *transfer = argv[4];
      char *language = argv[5];
      int *value = malloc(sizeof(int));
      int conversion = Tcl_GetInt(interp, argv[6], value);
      if (suffix && representation && encoding && transfer && language && (conversion == TCL_OK)) {
	
	BOOL result             = HTBind_add(suffix, representation, encoding, transfer, language, *value);
	
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
      Tcl_AppendResult(interp, bad_vars, NULL);
      return TCL_ERROR;
    }
    else {
	Tcl_AppendResult(interp, err_string, argv[0], NULL);
	return TCL_ERROR;
    }
} 


int HTBind_addType_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv) {
    if (argc == 4) {
      char *suffix 	   = argv[1];
      char *format = argv[2];
      int *value = malloc(sizeof(int));
      int conversion = Tcl_GetInt(interp, argv[3], value);
      if (suffix && format && (conversion == TCL_OK)) {
	
	BOOL result             = HTBind_addType(suffix, format, *value);
	
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
      Tcl_AppendResult(interp, bad_vars, NULL);
      return TCL_ERROR;
    }
    else {
	Tcl_AppendResult(interp, err_string, argv[0], NULL);
	return TCL_ERROR;
    }
} 


int HTBind_addEncoding_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv) {
    if (argc == 4) {
      char *suffix 	   = argv[1];
      char *encoding = argv[2];
      int *value = malloc(sizeof(int));
      int conversion = Tcl_GetInt(interp, argv[3], value);
      if (suffix && encoding && (conversion == TCL_OK)) {
	
	BOOL result             = HTBind_addType(suffix, encoding, *value);
	
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
      Tcl_AppendResult(interp, bad_vars, NULL);
      return TCL_ERROR;
    }
    else {
	Tcl_AppendResult(interp, err_string, argv[0], NULL);
	return TCL_ERROR;
    }
} 


int HTBind_addTransfer_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv) {
    if (argc == 4) {
      char *suffix 	   = argv[1];
      char *transfer = argv[2];
      int *value = malloc(sizeof(int));
      int conversion = Tcl_GetInt(interp, argv[3], value);
      if (suffix && transfer && (conversion == TCL_OK)) {
	
	BOOL result             = HTBind_addTransfer(suffix, transfer, *value);
	
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
      Tcl_AppendResult(interp, bad_vars, NULL);
      return TCL_ERROR;
    }
    else {
	Tcl_AppendResult(interp, err_string, argv[0], NULL);
	return TCL_ERROR;
    }
} 



int HTBind_addLanguage_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv) {
    if (argc == 4) {
      char *suffix 	   = argv[1];
      char *language = argv[2];
      int *value = malloc(sizeof(int));
      int conversion = Tcl_GetInt(interp, argv[3], value);
      if (suffix && language && (conversion == TCL_OK)) {
	
	BOOL result             = HTBind_addLanguage(suffix, language, *value);
	
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
      Tcl_AppendResult(interp, bad_vars, NULL);
      return TCL_ERROR;
    }
    else {
	Tcl_AppendResult(interp, err_string, argv[0], NULL);
	return TCL_ERROR;
    }
} 

  /******************************
   *Determine a suitable suffix *
   ******************************/



int HTBind_getSuffix_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 2) {
    char     *parent_key		= argv[1];

    if (parent_key) {
	Tcl_HashEntry *p_anchorEntry = Tcl_FindHashEntry(&HTableAnchor, parent_key);
	if(p_anchorEntry) {
	  HTParentAnchor *p_anchor = Tcl_GetHashValue(p_anchorEntry);
	  char *Result = HTBind_getSuffix (p_anchor);
	  Tcl_AppendResult(interp, Result, NULL);
	  return TCL_OK;
	}
	Tcl_AppendResult(interp, "no hash table entry", NULL);
	return TCL_ERROR;
	
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], NULL);
    return TCL_ERROR;
  }
}


/*Determine the content of an Anchor */



int HTBind_getBindings_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 2) {
    char     *parent_key		= argv[1];

    if (parent_key) {
	Tcl_HashEntry *p_anchorEntry = Tcl_FindHashEntry(&HTableAnchor, parent_key);
	if(p_anchorEntry) {
	  HTParentAnchor *p_anchor = Tcl_GetHashValue(p_anchorEntry);
	  BOOL Result = HTBind_getBindings(p_anchor);
	  Tcl_AppendResult(interp, Result ? "YES" : "NO", NULL);
	  return TCL_OK;
	}
	Tcl_AppendResult(interp, "no hash table entry", NULL);
	return TCL_ERROR;
	
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], NULL);
    return TCL_ERROR;
  }
}



/*Determine the content of File*/

/*not finished yet, need hashtables for HTFormat, etc? */

/*

  extern BOOL HTBind_getFormat (const char *      filename,
                              HTFormat *        format,
                              HTEncoding *      enc,
                              HTEncoding *      cte,
                              HTLanguage *      lang,
                              double *          quality);

			      
			      */







