#include "HTFWriter_glue.h"

#define bad_vars "Invalid variable names or non-existent entries."
#define err_string "wrong # args: should be "

Tcl_HashTable	HTableStream;
Tcl_HashTable	HTableReq;

int HTBlackHole_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 1) {
    char *keyname;
    HTStream *stream        = HTBlackHole();

    Tcl_HashEntry *entryPtr = CreateNewStreamEntry("BlackHole", &keyname);
    Tcl_SetHashValue(entryPtr, stream);        

    Tcl_AppendResult(interp, keyname, NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], NULL);
    return TCL_ERROR;
  }
}

int HTErrorStream_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 1) {
    char *keyname;
    HTStream *stream 	    = HTErrorStream();
	
    Tcl_HashEntry *entryPtr = CreateNewStreamEntry("ErrorStream", &keyname);
    Tcl_SetHashValue(entryPtr, stream);        

    Tcl_AppendResult(interp, keyname, NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], NULL);
    return TCL_ERROR;
  }
}

int HTFWriter_new_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 3) {
    int leave_open;
    char *req_keyname     	   = argv[1];
    char *filename	           = argv[2];
    char *boolean_str     	   = argv[3];
    if (req_keyname && filename && (Tcl_GetBoolean(interp, boolean_str, &leave_open) == TCL_OK)) {
      char *keyname;
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);

      FILE *fp = fopen(filename, "w");
      if (request_entry) {
	HTRequest *req 		   = Tcl_GetHashValue(request_entry);
	HTStream *stream 	   = HTFWriter_new(req, fp, leave_open);
	Tcl_HashEntry *entryPtr    = CreateNewStreamEntry("HTFWriter", &keyname);
	Tcl_SetHashValue(entryPtr, stream);        
	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], NULL);
    return TCL_ERROR;
  }
}




