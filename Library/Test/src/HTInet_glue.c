#include "HTInet_glue.h"

#define max_keyname 20
#define bad_vars "Invalid variable names or non-existent entries."
#define err_string "wrong # args: should be "


/**************************
 *       HTInet           *
 *************************/

Tcl_HashTable HTableNet;


int HTErrnoString_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if ( argc == 2) {
    int errnum;
    if (Tcl_GetInt(interp, argv[1], &errnum) == TCL_OK) {
      const char * result = HTErrnoString(errnum);
      Tcl_AppendResult(interp, result, NULL);
      return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTInetStatus_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if ( argc == 3) {
    int errnum;
    char *where = argv[2];
    if (where && (Tcl_GetInt(interp, argv[1], &errnum) == TCL_OK) ) {
      int result = HTInetStatus(errnum, where);
      char *storage = malloc(sizeof(char) * max_keyname);
      if(!storage) { return TCL_ERROR; }
      sprintf(storage, "%d", result);
      Tcl_AppendResult(interp, result, NULL);
      return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}
      



/*extern unsigned int HTCardinal (int *           pstatus,
                                char **         pp,
                                unsigned int    max_value);


extern const char * HTInetString (struct sockaddr_in * sin);

*/


int HTParseInet_tcl(ClientData clientData, Tcl_Interp *interp,
		    int argc, char **argv) {
  if ( argc == 3) {
    char *net_key = argv[1];
    char *host = argv[2];
    if ( net_key && host) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      if ( net_entry) {
	HTNet *net = Tcl_GetHashValue(net_entry);
	int result = HTParseInet(net, host);
	char *storage = malloc(sizeof(char) *max_keyname);
	if (!storage) { return TCL_ERROR;}
	sprintf(storage, "%d", result);
	Tcl_AppendResult(interp, storage, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}
	
	
  

int HTGetTimeZoneOffset_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if ( argc == 1) {
    time_t time = HTGetTimeZoneOffset();
    char *storage = malloc(sizeof(char) *max_keyname);
    if (!storage) { return TCL_ERROR;}
    sprintf(storage, "%d", (int) time);
    Tcl_AppendResult(interp, storage, NULL);
    return TCL_OK;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTGetHostName_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 1) {
    char *result = HTGetHostName();
    Tcl_AppendResult(interp, result, NULL);
    return TCL_OK;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}

int HTGetMailAddress_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if ( argc ==1) {
    char *result = HTGetMailAddress();
    Tcl_AppendResult(interp, result, NULL);
    return TCL_OK;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTGetNewsServer_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if ( argc == 1) {
    char *result = HTGetNewsServer();
    Tcl_AppendResult(interp, result, NULL);
    return TCL_OK;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTGetTmpFileName_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if ( argc == 2) {
    const char *dir = argv[1]; 
    if (dir) {
      char *result = HTGetTmpFileName(dir);
      Tcl_AppendResult(interp, result, NULL);
      return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


