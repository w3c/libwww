#include "HTLink_glue.h"

#define max_keyname 20
#define bad_vars "Invalid variable names or non-existent entries."
#define err_string "wrong # args: should be "

Tcl_HashTable   HTableLink;
Tcl_HashTable   HTableAnchor;

/*****************************
*           HTLink           *
*****************************/

/* NEW */

int HTLink_new_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
    if (argc == 1) {
	char *keyname;
	
	Tcl_HashEntry *link_entry;
	HTLink *link       	  = HTLink_new();

	link_entry 	   	  = CreateNewLinkEntry("Link", &keyname);
	Tcl_SetHashValue(link_entry, link);        

	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
    }
    else {
	Tcl_AppendResult(interp, err_string, argv[0], NULL);
	return TCL_ERROR;
    }
}

/* DELETE */

int HTLink_delete_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 2) {
    char *link_keyname          = argv[1];
    if (link_keyname) {
      Tcl_HashEntry *link_entry = Tcl_FindHashEntry(&HTableLink, link_keyname);
      if (link_entry) {
	HTLink *link 		= Tcl_GetHashValue(link_entry);
	BOOL result 		= HTLink_delete(link);
	Tcl_DeleteHashEntry(link_entry);

	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp,err_string, argv[0], " link", NULL);
    return TCL_ERROR;
  }
}

/* REMOVEALL */

int HTLink_removeAll_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 2) {
    char *keyname          	  = argv[1];
    if (keyname) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, keyname);
      if (anchor_entry) {
	HTAnchor *anchor 	  = Tcl_GetHashValue(anchor_entry);	
	BOOL result               = HTLink_removeAll(anchor);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp,err_string, argv[0], " source_anchor", NULL);
    return TCL_ERROR;
  }
}

/* ADD */

int HTLink_add_tcl(ClientData clientData, Tcl_Interp *interp, 
		   int argc, char **argv) {

  if (argc == 5) {
    char *linktype                = argv[3];
    char *methodname              = argv[4];	
    if (linktype && methodname && argv[1] && argv[2]) {
      Tcl_HashEntry *source_entry = Tcl_FindHashEntry(&HTableAnchor, argv[1]);
      Tcl_HashEntry *dest_entry   = Tcl_FindHashEntry(&HTableAnchor, argv[2]);
      HTLinkType link   	  = HTAtom_for(linktype);
      HTMethod method 	          = HTMethod_enum(methodname);
      if (source_entry && dest_entry) {
	HTAnchor *s_anchor        = Tcl_GetHashValue(source_entry);
	HTAnchor *d_anchor 	  = Tcl_GetHashValue(dest_entry);
	BOOL result               = HTLink_add(s_anchor, d_anchor, link, method);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp,err_string, argv[0], " source_anchor dest_anchor type method", NULL);
    return TCL_ERROR;
  }
}

/* REMOVE */

int HTLink_remove_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 3) {
    char *src_anchor                = argv[1];
    char *dest_anchor               = argv[2];
    if (src_anchor && dest_anchor) {
      Tcl_HashEntry *source_entry   = Tcl_FindHashEntry(&HTableAnchor, src_anchor);
      Tcl_HashEntry *dest_entry     = Tcl_FindHashEntry(&HTableAnchor, dest_anchor);
      if (source_entry && dest_entry) {
	HTAnchor *s_anchor 	    = Tcl_GetHashValue(source_entry);	
	HTAnchor *d_anchor 	    = Tcl_GetHashValue(dest_entry);
	HTLink *link 		    = HTLink_find(s_anchor, d_anchor);
	BOOL result 		    = HTLink_remove(s_anchor, d_anchor);
	Tcl_HashEntry *link_entry   = Tcl_FindHashEntry(&HTableLink, Keyname_lookupLinks(link));
	Tcl_DeleteHashEntry(link_entry);

	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp,err_string, argv[0], " source_anchor dest_anchor", NULL);
    return TCL_ERROR;
  }
}

/* FIND */

int HTLink_find_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 3) {
    char *src_anchor         = argv[1];
    char *dest_anchor	     = argv[2];
    if (src_anchor && dest_anchor) {
      char *keyname 	     = NULL;
      Tcl_HashEntry *entryPtr;
      Tcl_HashEntry *srcPtr  = Tcl_FindHashEntry(&HTableAnchor, src_anchor);
      Tcl_HashEntry *destPtr = Tcl_FindHashEntry(&HTableAnchor, dest_anchor);
      if (srcPtr && destPtr) {
	HTAnchor *s_anchor   = Tcl_GetHashValue(srcPtr);
	HTAnchor *p_anchor   = Tcl_GetHashValue(destPtr);
	HTLink *result       = HTLink_find(s_anchor, p_anchor);

	if (result) {
	  keyname = Keyname_lookupLinks(result);
	  if (!keyname) {
	    entryPtr = CreateNewLinkEntry("HTLink", &keyname);
	    Tcl_SetHashValue(entryPtr, result);
	  }
	}
	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp,err_string, argv[0], " source_anchor dest_anchor", NULL);
    return TCL_ERROR;
  }
}

/* DESTINATION */

int HTLink_setDestination_tcl(ClientData clientData, Tcl_Interp *interp, 
			      int argc, char **argv) {
  if (argc == 3) {
    char *link_name	       = argv[1];
    char *anchor_name	       = argv[2];
    if (link_name && anchor_name) {
      Tcl_HashEntry *anchorPtr = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      Tcl_HashEntry *linkPtr   = Tcl_FindHashEntry(&HTableLink, link_name);
      if (anchorPtr && linkPtr) {
	HTLink *link  	       = Tcl_GetHashValue(linkPtr);
	HTAnchor *anchor       = Tcl_GetHashValue(anchorPtr);
	BOOL result  	       = HTLink_setDestination(link, anchor);	
	    
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }	
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " link destination",  NULL);
    return TCL_ERROR;
  }
}

int HTLink_destination_tcl(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {
  if (argc == 2) {
    char *link_name	      = argv[1];
    if (link_name) {
      Tcl_HashEntry *entryPtr;	
      Tcl_HashEntry *linkPtr  = Tcl_FindHashEntry(&HTableLink, link_name);
      if (linkPtr) {
	HTLink *link  	      = Tcl_GetHashValue(linkPtr);
	HTAnchor *result      = HTLink_destination(link);	
	
	/* Standard method to find if a keyname already exists for
	   the destination, otherwise it will create it. */

	char *keyname 	      = Keyname_lookupAnchor(result);
	if (!keyname) {
	  entryPtr = CreateNewAnchorEntry("Anchor", &keyname);
	  Tcl_SetHashValue(entryPtr, result);
	}
	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " link",  NULL);
    return TCL_ERROR;
  }
}

/* TYPE */

int HTLink_setType_tcl(ClientData clientData, Tcl_Interp *interp,
		       int argc, char **argv) {
  if (argc == 3) {
    char *keyname   	      = argv[1];
    char *type_name 	      = argv[2];
    if (keyname && type_name) {
      HTLinkType type         = HTAtom_for(type_name);
      Tcl_HashEntry *entryPtr = Tcl_FindHashEntry(&HTableLink, keyname);
      if (entryPtr) {
	HTLink *link	      = Tcl_GetHashValue(entryPtr);
	BOOL result	      = HTLink_setType(link, type);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp,err_string, argv[0], " link type", NULL);
    return TCL_ERROR;
  }
}

int HTLink_type_tcl(ClientData clientData, Tcl_Interp *interp, 
		    int argc, char **argv) {
  if (argc == 2) {
    char *keyname             = argv[1];
    if (keyname) {
      Tcl_HashEntry *entryPtr = Tcl_FindHashEntry(&HTableLink, keyname);
      if (entryPtr) {
	HTLink *link	      = Tcl_GetHashValue(entryPtr);
	HTLinkType link_type  = HTLink_type(link);
	
	Tcl_AppendResult(interp, HTAtom_name(link_type), NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp,err_string, argv[0], " link", NULL);
    return TCL_ERROR;
  }
}

/* METHOD */

int HTLink_setMethod_tcl(ClientData clientData, Tcl_Interp *interp,
			 int argc, char **argv) {
  if (argc == 3) {
    char *keyname     	      = argv[1];
    char *method_name 	      = argv[2];
    if (keyname && method_name) {
      HTMethod method         = HTMethod_enum(method_name);
      Tcl_HashEntry *entryPtr = Tcl_FindHashEntry(&HTableLink, keyname);
      if (entryPtr) {
	HTLink *link	      = Tcl_GetHashValue(entryPtr);
	BOOL result	      = HTLink_setMethod(link, method);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " link method", NULL);
    return TCL_ERROR;
  }
}

int HTLink_method_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 2) {
    char *keyname 		= argv[1];
    if (keyname) {
      Tcl_HashEntry *entryPtr 	= Tcl_FindHashEntry(&HTableLink, keyname);
      if (entryPtr) {
	HTLink *link		= Tcl_GetHashValue(entryPtr);
	HTMethod method    	= HTLink_method(link);
	
	Tcl_AppendResult(interp, HTMethod_name(method), NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp,err_string, argv[0], " link", NULL);
    return TCL_ERROR;
  }
}

/* RESULT */

int HTLink_setResult_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 3) {
    char *keyname    	      = argv[1];
    char *linkresult 	      = argv[2];
    if (keyname && linkresult) {
      Tcl_HashEntry *entryPtr = Tcl_FindHashEntry(&HTableLink, keyname);
      HTLink *link            = Tcl_GetHashValue(entryPtr);
      HTLinkResult l_result   = HTLinkResult_enum(linkresult);
      BOOL result 	      = HTLink_setResult(link, l_result);
    
      Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
      return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }	
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " link link_result", NULL);
    return TCL_ERROR;
  }
}

int HTLink_result_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 2) {
    char *keyname 		= argv[1];
    if (keyname) {
      Tcl_HashEntry *entryPtr 	= Tcl_FindHashEntry(&HTableLink, keyname);
      if (entryPtr) {
	HTLink *link	        = Tcl_GetHashValue(entryPtr);
	HTLinkResult linkresult = HTLink_result(link);	
	char *result 		= HTLinkResult_name(linkresult);
	Tcl_AppendResult(interp, result, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "link", NULL);
    return TCL_ERROR;
  }
}

#if 0
/* MOVEALL */

int HTLink_moveall_tcl(ClientData clientData, Tcl_Interp *interp, 
			      int argc, char **argv) {
  if (argc == 3) {
    char *src_anchor                = argv[1];
    char *dest_anchor               = argv[2];
    if (src_anchor && dest_anchor) {
      Tcl_HashEntry *source_entry = Tcl_FindHashEntry(&HTableAnchor, src_anchor);
      Tcl_HashEntry *dest_entry   = Tcl_FindHashEntry(&HTableAnchor, dest_anchor);
      if (source_entry && dest_entry) {
	HTAnchor *s_anchor      = Tcl_GetHashValue(source_entry);	
	HTAnchor *d_anchor      = Tcl_GetHashValue(dest_entry);

	BOOL result = HTLink_moveall(s_anchor, d_anchor);
	BOOL result = TRUE;
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " source_anchor dest_anchor", NULL);
    return TCL_ERROR;
  }
}
#endif
