#include "HTAssoc_glue.h"

#define max_keyname 20
#define bad_vars "Invalid variable names or non-existent entries."
#define err_string "wrong # args: should be "

Tcl_HashTable   HTableAssoc;

/*****************************
*        HTAssocList         *
*****************************/


/* NEW */

int HTAssocList_new_tcl(ClientData clientData, Tcl_Interp *interp, 
		    int argc, char **argv) {
  if (argc == 1) {
    char *keyname;
    HTAssocList *list = HTAssocList_new();
    Tcl_HashEntry *entryPtr = CreateNewAssocListEntry("HTAssocList", &keyname);
      if (entryPtr) {
	Tcl_SetHashValue(entryPtr, list);        

	Tcl_AppendResult(interp, keyname, NULL);	
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

/* DELETE */

int HTAssocList_delete_tcl(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {
  if (argc == 2) {
    char *keyname 	      = argv[1];
    if (keyname) {
      Tcl_HashEntry *entryPtr = Tcl_FindHashEntry(&HTableAssoc, keyname);
      if (entryPtr) {
 	HTAssocList *list    = Tcl_GetHashValue(entryPtr);
	HTAssocList_delete(list);
	Tcl_DeleteHashEntry(entryPtr);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " associated-list",  NULL);
    return TCL_ERROR;
  }
}

