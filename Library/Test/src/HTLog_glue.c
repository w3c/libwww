#include "HTLog_glue.h"
#include "WWWApp.h"

#define max_keyname 20

#define bad_vars "Invalid variable names or non-existent entries."
#define err_string "wrong # args: should be"


/*****************
 *    HTLog      *
 *****************/


Tcl_HashTable HTableReq;




int HTLog_open_tcl(ClientData clientData, Tcl_Interp *interp, 
		   int argc, char **argv) {
  if (argc == 4) {
    const char * filename = argv[1];
    int local;
    int append;
    int success1 = Tcl_GetBoolean(interp, argv[2], &local);
    int success2 = Tcl_GetBoolean(interp, argv[3], &append);
    
    if (filename && (success1 == TCL_OK)  && (success2 == TCL_OK) ) {
      
      BOOL result = HTLog_open(filename, (BOOL) local, (BOOL) append);
      
      Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
      return TCL_OK;

    }
    
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


int HTLog_close_tcl(ClientData clientData, Tcl_Interp *interp, 
		    int argc, char **argv) {
  
  if(argc == 1) {
    
    BOOL result = HTLog_close();
    
    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}




int HTLog_isOpen_tcl(ClientData clientData, Tcl_Interp *interp, 
		    int argc, char **argv) {
  
  if(argc == 1) {
    
    BOOL result = HTLog_isOpen();
    
    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}



int HTLog_add_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if(argc == 3) {
    char * request_key = argv[1];
    int status;
    int success = Tcl_GetInt(interp, argv[2], &status);

    if(request_key && (success == TCL_OK) ) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, 
						       request_key);
      if (request_entry) {
	HTRequest *request = Tcl_GetHashValue(request_entry);
	
	BOOL result = HTLog_add(request, status);
	
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
