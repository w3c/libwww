#include "HTTrans_glue.h"


#define max_keyname 20
#define bad_vars "Invalid variable names or non-existent entries."
#define err_string "wrong # args: should be "

/*****************
 *  HTTrans      *
 *****************/

Tcl_HashTable HTableTrans;
Tcl_HashTable HTableReq;

/*
extern BOOL HTTransport_add (const char *               name,
                             HTTransportMode            mode,
                             HTInput_new *              get_input,
                             HTOutput_new *             get_output);
*/

int HTTransport_delete_tcl(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {
  if (argc == 2) {
    const char *name = argv[1];
    if (name){
      BOOL result = HTTransport_delete(name);
      Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
      return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}

int HTTransport_deleteAll_tcl(ClientData clientData, Tcl_Interp *interp, 
			      int argc, char **argv) {
  if (argc == 1) {
    BOOL result = HTTransport_deleteAll();
    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
    return TCL_OK;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}

int HTTransport_find_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if ( argc == 3) {
    char *request_key = argv[1];
    const char *name = argv[2];
    if ( request_key && name) {
      Tcl_HashEntry *request_entry=Tcl_FindHashEntry(&HTableReq, request_key);
      if(request_entry) {
	char *keyname;
	HTRequest *request = Tcl_GetHashValue(request_entry);
	HTTransport *transport = HTTransport_find(request, name);
	Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableTrans, "Transport", 
						 &keyname);
	Tcl_SetHashValue(entryPtr, transport);
	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTTransport_mode_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 2) {
    char *tp_key = argv[1];
    if (tp_key){
      Tcl_HashEntry *tp_entry = Tcl_FindHashEntry(&HTableTrans, tp_key);
      if(tp_entry) {
	HTTransport *tp = Tcl_GetHashValue(tp_entry);
	HTTransportMode mode = HTTransport_mode(tp);
	char *mode_name = HTTransportMode_name(mode);
	Tcl_AppendResult(interp, mode_name, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}
  


int HTTransport_setMode_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 3) {
    char *tp_key = argv[1];
    char *mode_name = argv[2];
    if (tp_key && mode_name){
      Tcl_HashEntry *tp_entry = Tcl_FindHashEntry(&HTableTrans, tp_key);
      HTTransportMode mode = HTTransportMode_enum(mode_name);
      if(tp_entry) {
	HTTransport *tp = Tcl_GetHashValue(tp_entry);
	BOOL result = HTTransport_setMode(tp, mode);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}
  
