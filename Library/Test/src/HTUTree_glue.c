#include "HTUTree_glue.h"

#define max_keyname 20
#define bad_vars "Invalid variable names or non-existent entries."
#define err_string "wrong # args: should be "

/************************
 *      HTUTree         *
 ************************/


Tcl_HashTable HTableUTree;
Tcl_HashTable HTableVoid;

/*not sure how to handel HTUTree_gc, which is the type a fucntion 
 that returns an integer */

int HTUTree_new_tcl(ClientData clientData, Tcl_Interp *interp, 
		    int argc, char **argv) {
  if(argc == 5) {
    const char * root = argv[1];
    const char * host = argv[2];
    int port;
    int success = Tcl_GetInt(interp, argv[3], &port);
    char *gc_name  = argv[5];
    
    if ( root && host && (success == TCL_OK) && gc_name) {
      /*
      char *keyname;
      HTUTree_gc *gc = 
      HTUTree * utree = HTUTree_new(root, host, port, gc);
      Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableUTree, "UTree", &keyname);
      Tcl_SetHashValue(entryPtr, utree);
      Tcl_AppendResult(interp, keyname, NULL);
      */
      return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTUTree_delete_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char ** argv) {
  if (argc == 4) {
    const char *root = argv[1];
    const char *host = argv[2];
    int port;
    int success = Tcl_GetInt(interp, argv[3], &port);
    if (root && host && (success == TCL_OK) ) {
      BOOL result = HTUTree_delete(root, host, port);
      Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
      return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTUTree_deleteAll_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 1) {
    BOOL result = HTUTree_deleteAll();
    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
    return TCL_OK;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTUTree_find_tcl (ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 4) {
    const char * root = argv[1];
    const char * host = argv[2];
    int port;
    if ( root && host && (Tcl_GetInt(interp, argv[3], &port) == TCL_OK)) {
      char *keyname;
      HTUTree *utree  = HTUTree_find(root, host, port);
      Tcl_HashEntry *entryPtr =CreateNewEntry(&HTableUTree, "UTree", &keyname);
      Tcl_SetHashValue(entryPtr, utree);
      Tcl_AppendResult(interp, keyname, NULL);
      return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}



int HTUTree_findNode_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 4) {
    char *tree_key = argv[1];
    const char *realm = argv[2];
    const char *path = argv[3];
    if ( tree_key && realm && path) {
      Tcl_HashEntry *tree_entry = Tcl_FindHashEntry(&HTableUTree, tree_key);
      if ( tree_entry) {
	HTUTree *tree = Tcl_GetHashValue(tree_entry);
	
	HTUTree_findNode(tree, realm, path);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, bad_vars, NULL);
  return TCL_ERROR;
}



int HTUTree_addNode_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 5) {
    char *tree_key = argv[1];
    const char *realm = argv[2];
    const char *path = argv[3];
    char *void_key = argv[4];
    if ( tree_key && realm && path) {
      Tcl_HashEntry *tree_entry = Tcl_FindHashEntry(&HTableUTree, tree_key);
      Tcl_HashEntry *void_entry = Tcl_FindHashEntry(&HTableVoid, void_key);
      if ( tree_entry && void_entry ) {
	HTUTree *tree = Tcl_GetHashValue(tree_entry);
	void *context = Tcl_GetHashValue(void_entry);
	BOOL result = HTUTree_addNode(tree, realm, path, context);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, bad_vars, NULL);
  return TCL_ERROR;
}

int HTUTree_replaceNode_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 5) {
    char *tree_key = argv[1];
    const char *realm = argv[2];
    const char *path = argv[3];
    char *void_key = argv[4];
    if ( tree_key && realm && path) {
      Tcl_HashEntry *tree_entry = Tcl_FindHashEntry(&HTableUTree, tree_key);
      Tcl_HashEntry *void_entry = Tcl_FindHashEntry(&HTableVoid, void_key);
      if ( tree_entry && void_entry ) {
	HTUTree *tree = Tcl_GetHashValue(tree_entry);
	void *context = Tcl_GetHashValue(void_entry);
	BOOL result = HTUTree_replaceNode(tree, realm, path, context);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, bad_vars, NULL);
  return TCL_ERROR;
}






int HTUTree_deleteNode_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 4) {
    char *tree_key = argv[1];
    const char *realm = argv[2];
    const char *path = argv[3];
    if ( tree_key && realm && path) {
      Tcl_HashEntry *tree_entry = Tcl_FindHashEntry(&HTableUTree, tree_key);
      if ( tree_entry) {
	HTUTree *tree = Tcl_GetHashValue(tree_entry);
	
	BOOL result = HTUTree_deleteNode(tree, realm, path);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, bad_vars, NULL);
  return TCL_ERROR;
}











