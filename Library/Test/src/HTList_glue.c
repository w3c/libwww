#include "HTList_glue.h"

#define max_keyname 20
#define bad_vars "Invalid variable names or non-existent entries."
#define err_string "wrong # args: should be "

Tcl_HashTable	HTableList;

int HTList_new_tcl(ClientData clientData, Tcl_Interp *interp, 
		   int argc, char **argv) {
    if (argc == 1) {
	char *keyname;
	HTList *list 		= HTList_new();

	Tcl_HashEntry *entryPtr = CreateNewListEntry("List", &keyname);
	Tcl_SetHashValue(entryPtr, list);

	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
    }
    else {
      Tcl_AppendResult(interp, err_string, argv[0], NULL);
	return TCL_ERROR;
    }
}

int HTList_delete_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 2) {
    char *keyname 	      = argv[1];
    if (keyname) {
      Tcl_HashEntry *entryPtr = Tcl_FindHashEntry(&HTableList, keyname);
      if (entryPtr) {
	HTList *list 	      = Tcl_GetHashValue(entryPtr);
	BOOL result 	      = HTList_delete(list);
	Tcl_DeleteHashEntry(entryPtr);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " list", NULL);
    return TCL_ERROR;
  }
}
