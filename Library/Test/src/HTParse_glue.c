#include "HTParse_glue.h"

#define max_keyname 20
#define bad_vars "Invalid variable names or non-existent entries."
#define err_string  "wrong # args: should be "

/* HTParse */

int HTParse_tcl(ClientData clientData, Tcl_Interp *interp, 
		int argc, char **argv) {

  if (argc > 2 && argc < 5) {
    char *aName 	        = argv[1];
    char *relatedName 		= argv[2];
    if (aName && relatedName) {
      char *result;
      int bitflag 		= -1;

      /* Don't know how to check otherwise, so dummy statement at end */
      if ((argc == 4) && (Tcl_GetInt(interp, argv[3], &bitflag) == TCL_OK)) {

      }
      result 			= HTParse(aName, relatedName, bitflag);
      Tcl_AppendResult(interp, result, NULL);
      return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " a_address absolute_address bit-flags", NULL);
    return TCL_ERROR;
  }
}

int HTSimplify_tcl(ClientData clientData, Tcl_Interp *interp, 
		   int argc, char **argv) {
  if (argc == 2) {
    char *str      = argv[1];
    if (str) {
      char *result = HTSimplify(&str);
      Tcl_AppendResult(interp, result, NULL);
      return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "string", NULL);
    return TCL_ERROR;
  }
}

int HTRelative_tcl(ClientData clientData, Tcl_Interp *interp, 
		   int argc, char **argv) {
  if (argc == 3) {
    char *aname    = argv[1];
    char *bname    = argv[2];
    if (aname && bname) {
      char *result = HTRelative(aname, bname);
      Tcl_AppendResult(interp, result, NULL);
      return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " absolute_address absolute_address", NULL);
    return TCL_ERROR;
  }
}

int HTCleanTelnetString_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 2) {
    char *str     = argv[1];
    if (str) {
      BOOL result = HTCleanTelnetString(str);
      Tcl_AppendResult(interp, result ? "YES" : "NO", str, NULL);
      return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " string", NULL);
    return TCL_ERROR;
  }
}

