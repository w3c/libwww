#include "HTError_glue.h"


#define max_keyname 20

#define bad_vars "Invalid variable names or non-existent entries."
#define err_string "wrong # args: should be "

/*************************
 *         HTAnchor      *
 *************************/

Tcl_HashTable HTableList;
Tcl_HashTable HTableError;
Tcl_HashTable HTableVoid;



int HTError_add_tcl(ClientData clientData, Tcl_Interp *interp, 
		    int argc, char **argv) {
  if (argc == 8 ) {
    char *list_key = argv[1];
    char *severity_key = argv[2];
    int ignore;
    int success1 = Tcl_GetBoolean(interp, argv[3], &ignore);
    int element;
    int success2 = Tcl_GetInt(interp, argv[4], &element);
    char *void_key = argv[5];
    int length;
    int success3 = Tcl_GetInt(interp, argv[5], &length);
    char *where = argv[6];
    
    if ( list_key && severity_key &&  void_key && where &&
	 (success1 == TCL_OK) && (success2 == TCL_OK) &&
	 (success3 == TCL_OK) ) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, 
						    list_key);
      HTSeverity severity = HTSeverity_enum(severity_key);
      Tcl_HashEntry *void_entry = Tcl_FindHashEntry(&HTableVoid, 
						    void_key);
      if(list_entry && void_entry) {
	
	HTList * list = Tcl_GetHashValue(list_entry);
	void *par = Tcl_GetHashValue(void_entry);
	
	BOOL result = HTError_add(list, severity, ignore, element, 
				  par, (unsigned int) length, where);

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



int HTError_addSystem_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if(argc == 6) {
    char * list_key = argv[1];
    char * severity_key = argv[2];
    int errornumber;
    int success1 = Tcl_GetInt(interp, argv[3], &errornumber);
    int ignore;
    int success2 = Tcl_GetBoolean(interp, argv[4], &ignore);
    char *syscall = argv[5];
    if (list_key && severity_key && syscall &&
	(success1 == TCL_OK) && (success2 == TCL_OK)) {
      HTSeverity severity = HTSeverity_enum(severity_key);
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      
      if (list_entry) {
	HTList *list = Tcl_GetHashValue(list_entry);
	
	BOOL result= HTError_addSystem(list, severity, errornumber, 
				       (BOOL) ignore, syscall);
	
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


/*delete an entire Error Stack */

int HTError_deleteAll_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 2) {
    char * list_key = argv[1];
    if (list_key) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      
      if (list_entry) {
	HTList *list = Tcl_GetHashValue(list_entry);
	BOOL result = HTError_deleteAll(list);
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


/*delete lst added entry */

int HTError_deleteLast_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 2) {
    char * list_key = argv[1];
    if (list_key) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      
      if (list_entry) {
	HTList *list = Tcl_GetHashValue(list_entry);
	BOOL result = HTError_deleteLast(list);
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


/*object methods*/

/*show the error entry*/

int HTError_doShow_tcl(ClientData clientData, Tcl_Interp *interp,
		       int argc, char **argv) {
  if (argc == 2) {
    char *error_key = argv[1];
    if( error_key) {
      Tcl_HashEntry *error_entry = Tcl_FindHashEntry(&HTableError, 
						     error_key);
      if( error_entry) {
	HTError *info = Tcl_GetHashValue(error_entry);
	BOOL result = HTError_doShow(info);
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
	

/*Ignore last added error*/


int HTError_ignoreLast_tcl(ClientData clientData, Tcl_Interp *interp,
		       int argc, char **argv) {
  if (argc == 2) {
    char *list_key = argv[1];
    if( list_key) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, 
						     list_key);
      if( list_entry) {
	HTList *list = Tcl_GetHashValue(list_entry);
	BOOL result = HTError_ignoreLast(list);
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



int HTError_setIgnore_tcl(ClientData clientData, Tcl_Interp *interp,
		       int argc, char **argv) {
  if (argc == 2) {
    char *error_key = argv[1];
    if( error_key) {
      Tcl_HashEntry *error_entry = Tcl_FindHashEntry(&HTableError, 
						     error_key);
      if( error_entry) {
	HTError *info = Tcl_GetHashValue(error_entry);
	BOOL result = HTError_setIgnore(info);
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


int HTError_index_tcl(ClientData clientData, Tcl_Interp *interp,
		       int argc, char **argv) {
  if (argc == 2) {
    char *error_key = argv[1];
    if( error_key) {
      Tcl_HashEntry *error_entry = Tcl_FindHashEntry(&HTableError, 
						     error_key);
      if( error_entry) {
	HTError *info = Tcl_GetHashValue(error_entry);
	int result = HTError_index(info);
	char *storage = malloc(result * sizeof(char) );
	sprintf(storage, "%d", result);
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




int HTError_severity_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {
  if (argc == 2) {
    char *error_key = argv[1];
    if(error_key) {
      Tcl_HashEntry *error_entry = Tcl_FindHashEntry(&HTableError, 
						     error_key);
      if(error_entry){
	HTError *info = Tcl_GetHashValue(error_entry);
       
	HTSeverity severity = HTError_severity(info);
	
	char *result = HTSeverity_name(severity);
	
	Tcl_AppendResult(interp, result, NULL);
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
	

int HTError_hasSeverity_tcl(ClientData clientData, Tcl_Interp *interp,
		       int argc, char **argv) {
  if (argc == 3) {
    char *list_key = argv[1];
    char *severity_name = argv[2];
    if( list_key && severity_name) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, 
						     list_key);
      HTSeverity severity = HTSeverity_enum(severity_name);

      if( list_entry) {
	HTList *list = Tcl_GetHashValue(list_entry);
	BOOL result = HTError_hasSeverity(list, severity);

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

/*parameter associated with Error*/

/* note to myself, not sure what to do about an integer pointer, 
   do relize that the value might be resulted throught the integer
*/


int HTError_parameter_tcl(ClientData clientData, Tcl_Interp *interp,
		       int argc, char **argv) {
  if (argc == 2) {
    char *error_key = argv[1];

    int * length = malloc(sizeof(int));

    if( error_key) {
      Tcl_HashEntry *error_entry = Tcl_FindHashEntry(&HTableError, 
						     error_key);
      if( error_entry) {
	char * storage = malloc(max_keyname * sizeof(char));
	HTError *info = Tcl_GetHashValue(error_entry);
	
	HTError_parameter(info, length);
    	sprintf(storage, "%d", *length);
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

/* where did it happen */


int HTError_location_tcl(ClientData clientData, Tcl_Interp *interp,
		       int argc, char **argv) {
  if (argc == 2) {
    char *error_key = argv[1];
    if( error_key) {
      Tcl_HashEntry *error_entry = Tcl_FindHashEntry(&HTableError, 
						     error_key);
      if( error_entry) {
	HTError *info = Tcl_GetHashValue(error_entry);
	const char *result = HTError_location(info);
	Tcl_AppendResult(interp, result, NULL);
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
