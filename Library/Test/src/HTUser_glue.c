#include "HTUser_glue.h"

#define max_keyname 20
#define bad_vars "Invalid variable names or non-existent entries."
#define err_string "Wrong # of args. \n\n"

Tcl_HashTable   HTableUser;
Tcl_HashTable   HTableVoid;

/*****************************
*           HTUser           *
*****************************/

/* NEW */

int HTUserProfile_new_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
    if (argc == 3) {
        char *name 	   = argv[1];
        char *void_keyname = argv[2]; 
	char *keyname;
	
	Tcl_HashEntry *user_entry;
	Tcl_HashEntry *void_entry = Tcl_FindHashEntry(&HTableVoid, void_keyname);
	void *void_obj 		  = (void *) Tcl_GetHashValue(void_entry);
	HTUserProfile *user       = HTUserProfile_new(name, void_obj);


	user_entry 	   	  = CreateNewUserEntry("HTUser", &keyname);
	Tcl_SetHashValue(user_entry, user);        

	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
    }
    else {
	Tcl_AppendResult(interp, err_string, argv[0], NULL);
	return TCL_ERROR;
    }
}
