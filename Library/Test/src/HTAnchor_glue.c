#include "HTAnchor_glue.h"

#define max_keyname 20
#define bad_vars "Invalid variable names or non-existent entries."
#define err_string "wrong # args: should be "

/* Hash Tables to store data structures */

Tcl_HashTable   HTableAnchor;
Tcl_HashTable	HTableLink;
Tcl_HashTable	HTableList;
Tcl_HashTable   HTableVoid;

/*****************************
*         HTAnchor           *
*****************************/

/*****************************
*     Anchor Objects         *
*****************************/

/* FINDADDRESS */

int HTAnchor_findAddress_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 2) {
    char     *addr 		= argv[1];
    HTAnchor *anchor 		= HTAnchor_findAddress(addr);
    if (anchor) {
      char *keyname 		= Keyname_lookupAnchor(anchor);
      if (!keyname) {
	Tcl_HashEntry *entryPtr = CreateNewAnchorEntry("Anchor", &keyname);
	Tcl_SetHashValue(entryPtr, anchor);        
      }
      Tcl_AppendResult(interp, keyname, NULL);
      return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " URL", NULL);
    return TCL_ERROR;
  }
}

/* FINDCHILD */

int HTAnchor_findChild_tcl(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {
  if (argc == 3) {
    char *parent_key 		   = argv[1];
    char *tag        		   = argv[2];
    if (parent_key && tag) {
      Tcl_HashEntry *p_anchorEntry = Tcl_FindHashEntry(&HTableAnchor, parent_key);
      if (p_anchorEntry) {
	HTParentAnchor *p_anchor   = Tcl_GetHashValue(p_anchorEntry);	
	HTChildAnchor  *c_anchor   = HTAnchor_findChild(p_anchor, tag);
	char *keyname 		   = Keyname_lookupAnchor((HTAnchor *) c_anchor);
	if (!keyname) {
        Tcl_HashEntry *entryPtr    = CreateNewAnchorEntry("Child_Anchor", &keyname);
	  Tcl_SetHashValue(entryPtr, c_anchor);
	}
	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor tag", NULL);
    return TCL_ERROR;
  }
}

/* FINDCHILDANDLINK */

int HTAnchor_findChildAndLink_tcl(ClientData clientData, Tcl_Interp *interp,
				  int argc, char **argv) {
  if (argc == 5) {
    char *tag        		     = argv[2];
    char *href       		     = argv[3];
    char *linktype   		     = argv[4];
    if (argv[1] && tag && href && linktype) {
      Tcl_HashEntry  *p_anchor_entry = Tcl_FindHashEntry(&HTableAnchor, argv[1]);
      HTLinkType     link 	     = HTAtom_for(linktype);
      if (link && p_anchor_entry) {
	HTParentAnchor *p_anchor     = Tcl_GetHashValue(p_anchor_entry);
	HTChildAnchor  *c_anchor     = HTAnchor_findChildAndLink(p_anchor, tag, href, link);

	char *keyname 		     = Keyname_lookupAnchor((HTAnchor *) c_anchor);
	if (!keyname) {
          Tcl_HashEntry  *entryPtr   = CreateNewAnchorEntry("Child_Anchor", &keyname);
	  Tcl_SetHashValue(entryPtr, c_anchor);
	}
	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor tag address linktype", NULL);
    return TCL_ERROR;
  }
}

/* DELETE */

int HTAnchor_delete_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if (argc == 2) {
    char *keyname                = argv[1];
    if (keyname) {
      Tcl_HashEntry *entryPtr    = Tcl_FindHashEntry(&HTableAnchor, keyname);
      if (entryPtr) {
	HTParentAnchor *p_anchor = Tcl_GetHashValue(entryPtr);
	BOOL result 	         = HTAnchor_delete(p_anchor);

	Tcl_DeleteHashEntry(entryPtr);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " anchor", NULL);
    return TCL_ERROR;
  }
}

/* DELETEALL */

int HTAnchor_deleteAll_tcl(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {
  if (argc == 2) {
    BOOL result;
    char *keyname = malloc(50);
    char *check = malloc(50);
    HTList *list;
    char   *listname         = argv[1];
    Tcl_HashSearch search;	
    if (listname) {
      Tcl_HashEntry *listPtr = Tcl_FindHashEntry(&HTableList, listname);
      if (listPtr) {
	list 		     = Tcl_GetHashValue(listPtr);
	result 		     = HTAnchor_deleteAll(list);
      }
      else {
	listPtr 	     = CreateNewListEntry(listname, &keyname);
        result               = HTAnchor_deleteAll(list);
        Tcl_SetHashValue(listPtr, list);
      }

    for (listPtr = Tcl_FirstHashEntry(&HTableAnchor, &search); listPtr ; listPtr = Tcl_NextHashEntry(&search)) {
      Tcl_DeleteHashEntry(listPtr);
    }
    check                  = Tcl_SetVar(interp, listname, keyname, 0);
    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
    return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " HTList", NULL);
    return TCL_ERROR;
  }
}


/*****************************
*  Relate links to anchors   *
*****************************/

/* MAINLINK */

int HTAnchor_setMainLink_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 3) {
    char *keyname  = argv[1];
    char *linkname = argv[2];	

    /*Due to the fact that HTAnchor_makeMainLink swaps the pointers,
      the labels in Tcl are incorrect, thus the complex swapping of
      names...*/

    if (keyname && linkname) {
      char *oldmainkeyname;
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, keyname);
      Tcl_HashEntry *link_entry   = Tcl_FindHashEntry(&HTableLink, linkname);
      if (anchor_entry && link_entry) {
	Tcl_HashEntry *oldlink_entry;
	HTAnchor *anchor          = Tcl_GetHashValue(anchor_entry);
	HTLink *link	          = Tcl_GetHashValue(link_entry);
	HTLink *oldmain	          = HTAnchor_mainLink(anchor);
	BOOL result  	          = HTAnchor_setMainLink(anchor, link);
	if (oldmain != NULL) {
	  oldmainkeyname          = Keyname_lookupLinks(oldmain);
	  oldlink_entry           = Tcl_FindHashEntry(&HTableLink, oldmainkeyname);
	  Tcl_SetHashValue(oldlink_entry, link);
	  Tcl_SetHashValue(link_entry, oldmain);
	}    
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      } 
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " anchor link", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_mainLink_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 2) {
    char *keyname;
    char *anchor_name 	       	    = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *entryPtr;
      Tcl_HashEntry *anchorPtr 	    = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchorPtr) {
	HTAnchor *anchor       	    = Tcl_GetHashValue(anchorPtr);
	HTLink   *result       	    = HTAnchor_mainLink(anchor);

	if (result) {
	  keyname                   = Keyname_lookupLinks(result);
	  if (!keyname) {
	    entryPtr 		    = CreateNewLinkEntry("HTLink", &keyname);
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
    Tcl_AppendResult(interp, err_string, argv[0], " anchor", NULL);
    return TCL_ERROR;
  }
}

/* FOLLOWMAINLINK */

int HTAnchor_followMainLink_tcl(ClientData clientData, Tcl_Interp *interp, 
				int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name 	          = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchorPtr    = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchorPtr) {
	HTAnchor *anchor          = Tcl_GetHashValue(anchorPtr);
	HTAnchor *result          = HTAnchor_followMainLink(anchor);	
	
	char *keyname 	          = Keyname_lookupAnchor(result);
	if (!keyname) {
          Tcl_HashEntry *entryPtr = CreateNewAnchorEntry("Anchor", &keyname);
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
    Tcl_AppendResult(interp, err_string, argv[0], " anchor", NULL);
    return TCL_ERROR;
  }
}

/* SUBLINKS */

int HTAnchor_setSubLinks_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 3) {
    char *keyname  		= argv[1];
    char *listname 		= argv[2];
    if (keyname && listname) {
      Tcl_HashEntry *entryPtr   = Tcl_FindHashEntry(&HTableAnchor, keyname);
      Tcl_HashEntry *listPtr 	= Tcl_FindHashEntry(&HTableList, listname);
      if (entryPtr && listPtr) {
	HTAnchor *anchor 	= Tcl_GetHashValue(entryPtr);
	HTList *list 		= Tcl_GetHashValue(listPtr);
	BOOL result 		= HTAnchor_setSubLinks(anchor,list);

	Tcl_SetHashValue(listPtr, list);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " anchor list", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_subLinks_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 2) {
    char *keyname  		 = argv[1];
    if (keyname) {
      Tcl_HashEntry *entryPtr    = Tcl_FindHashEntry(&HTableAnchor, keyname);
      if (entryPtr) {
	HTAnchor *anchor 	 = Tcl_GetHashValue(entryPtr);
	HTList *list 		 = HTAnchor_subLinks(anchor);
	char *listname 		 = Keyname_lookupList(list);

	if ((!listname) && (list)) {
	  Tcl_HashEntry *listPtr = CreateNewListEntry("HTList", &listname);
	  Tcl_SetHashValue(listPtr, list);
	}
	Tcl_AppendResult(interp, listname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " anchor", NULL);
    return TCL_ERROR;
  }
}

/*****************************
*  Relate child and parents  *
*****************************/

/* PARENT */

int HTAnchor_parent_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name 		  = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTAnchor *anchor          = Tcl_GetHashValue(anchor_entry);
	HTParentAnchor *result    = HTAnchor_parent(anchor);

	char *keyname 		  = Keyname_lookupAnchor((HTAnchor *) result);
	if (!keyname) {
        Tcl_HashEntry *entryPtr   = CreateNewAnchorEntry("Parent_Anchor", &keyname);
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
    Tcl_AppendResult(interp, err_string, argv[0], " anchor", NULL);
    return TCL_ERROR;
  }
}

/* HASCHILDREN */

int HTAnchor_hasChildren_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name 		  = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	BOOL result 		  = HTAnchor_hasChildren(p_anchor);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

/*****************************
*   	Addresses  	     *
*****************************/

/* ADDRESS */

int HTAnchor_address_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name 		  = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTAnchor *anchor 	  = Tcl_GetHashValue(anchor_entry);
	char *result 		  = HTAnchor_address(anchor);
       		
	Tcl_AppendResult(interp, result, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " anchor", NULL);
    return TCL_ERROR;
  }
}

/* EXPANDEDADDRESS */

int HTAnchor_expandedAddress_tcl(ClientData clientData, Tcl_Interp *interp, 
				 int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name 		  = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTAnchor *anchor 	  = Tcl_GetHashValue(anchor_entry);
	char *result 		  = HTAnchor_expandedAddress(anchor);
       		
	Tcl_AppendResult(interp, result, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " anchor", NULL);
    return TCL_ERROR;
  }
}

/* PHYSICAL */

int HTAnchor_physical_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name 		  = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	char *result 		  = HTAnchor_physical(p_anchor);
       		
	Tcl_AppendResult(interp, result, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_setPhysical_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 3) {
    char *anchor_name 		  = argv[1];
    char *physical 		  = argv[2];
    if (anchor_name && physical) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTAnchor_setPhysical(p_anchor, physical);
       		
	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor physical_address", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_clearPhysical_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name 		  = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTAnchor_clearPhysical(p_anchor);
       		
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

/*****************************
*        Entity Body   	     *
*****************************/

/* DOCUMENT */

int HTAnchor_setDocument_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 3) {
    char *anchor_name 		   = argv[1];
    char *doc_name                 = argv[2];
    if (anchor_name && doc_name) {
      Tcl_HashEntry *anchor_entry  = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      Tcl_HashEntry *doc_entry     = Tcl_FindHashEntry(&HTableVoid, doc_name);
      if (anchor_entry && doc_entry) {
	HTParentAnchor *p_anchor   = Tcl_GetHashValue(anchor_entry);	
	void *doc  		   = Tcl_GetHashValue(doc_entry);

	HTAnchor_setDocument(p_anchor, doc);

	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor doc", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_document_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name 		  = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	void *result 		  = HTAnchor_document(p_anchor);

	char *keyname             = Keyname_lookupVoid((void *) result);
	if (!keyname) {
          Tcl_HashEntry *entryPtr = CreateNewVoidEntry("Doc", &keyname);
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
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

/*****************************
*        Entity Header       *
*****************************/

/* CLEARHEADER */

int HTAnchor_clearHeader_tcl(ClientData clientData, Tcl_Interp *interp, 
                             int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name             = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
        HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
        HTAnchor_clearHeader(p_anchor);
                
        Tcl_AppendResult(interp, NULL);
        return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

/* CACHEHIT */
#if 0
int HTAnchor_cacheHit_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name		  = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	BOOL result 	          = HTAnchor_cacheHit(p_anchor);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_setCacheHit_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 3) {
    char *anchor_name		  = argv[1];
    int cacheHit;
    if (anchor_name && (Tcl_GetBoolean(interp, argv[2], &cacheHit) == TCL_OK)) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTAnchor_setCacheHit(p_anchor, cacheHit);
	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor cachehit", NULL);
    return TCL_ERROR;
  }
}
#endif
/* INDEX */

int HTAnchor_clearIndex_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name 		  = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTAnchor_clearIndex(p_anchor);
       		
	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }

}

int HTAnchor_setIndex_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name 		  = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTAnchor_setIndex(p_anchor);
       		
	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_isIndex_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name 		  = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	BOOL result 		  = HTAnchor_isIndex(p_anchor);
       		
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

/* TITLE */

int HTAnchor_title_tcl(ClientData clientData, Tcl_Interp *interp, 	
		       int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name 		  = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
       	
	Tcl_AppendResult(interp, HTAnchor_title(p_anchor), NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_setTitle_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {

  if (argc == 3) {
    char *anchor_name 		  = argv[1];
    char *title		 	  = argv[2];
    if (anchor_name && title) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTAnchor_setTitle(p_anchor, title);
       		
	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor title", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_appendTitle_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 3) {
    char *anchor_name 		  = argv[1];
    char *title 		  = argv[2];
    if (anchor_name && title) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTAnchor_appendTitle(p_anchor, title);
       		
	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor append_title", NULL);
    return TCL_ERROR;
  }
}

/* BASE */

int HTAnchor_base_tcl(ClientData clientData, Tcl_Interp *interp, 	
		       int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name 		  = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
       	
	Tcl_AppendResult(interp, HTAnchor_base(p_anchor), NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_setBase_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {

  if (argc == 3) {
    char *anchor_name 		  = argv[1];
    char *base		 	  = argv[2];
    if (anchor_name && base) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	BOOL result 		  = HTAnchor_setBase(p_anchor, base);
       		
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor base", NULL);
    return TCL_ERROR;
  }
}

/* LOCATION */

int HTAnchor_location_tcl(ClientData clientData, Tcl_Interp *interp, 	
		       int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name 		  = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
       	
	Tcl_AppendResult(interp, HTAnchor_location(p_anchor), NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_setLocation_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {

  if (argc == 3) {
    char *anchor_name 		  = argv[1];
    char *location		 	  = argv[2];
    if (anchor_name && location) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	BOOL result 		  = HTAnchor_setLocation(p_anchor, location);
       		
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor location", NULL);
    return TCL_ERROR;
  }
}

/* FORMAT */

int HTAnchor_format_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name             = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTFormat result           = HTAnchor_format(p_anchor);
       		
	Tcl_AppendResult(interp, HTAtom_name(result), NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_setFormat_tcl(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {
  if (argc == 3) {
    char *anchor_name 	          = argv[1];
    char *format_name 	          = argv[2];
    if (anchor_name && format_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTFormat format 	  = HTAtom_for(format_name);
	HTAnchor_setFormat(p_anchor, format);
       		
	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor format", NULL);
    return TCL_ERROR;
  }
}

/* FORMATPARAM */

int HTAnchor_formatParam_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 2) {
    char *anchor_keyname	   = argv[1];
    char *listname 		   = malloc(max_keyname);
    if (anchor_keyname) {
      Tcl_HashEntry *anchor_entry  = Tcl_FindHashEntry(&HTableAnchor, anchor_keyname);
      Tcl_HashEntry *list_entry;
      if (anchor_entry) {
	HTParentAnchor *anchor 	   = (HTParentAnchor *) Tcl_GetHashValue(anchor_entry);
	HTAssocList *list      	   = HTAnchor_formatParam(anchor);
	listname 		   = Keyname_lookupAssocList(list);
	if (listname) {
	  list_entry 		   = CreateNewAssocListEntry("AssocList", &listname);
	  Tcl_SetHashValue(list_entry, list);
	}

	Tcl_AppendResult(interp, listname, NULL);
	return TCL_OK;
      }	
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_addFormatParam_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv) {
  if (argc == 4) {
    char *anchor_keyname	   = argv[1];
    char *name		   	   = argv[2];
    char *value		   	   = argv[3];
    if (anchor_keyname) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_keyname);
      if (anchor_entry) {
	HTParentAnchor *anchor 	   = (HTParentAnchor *) Tcl_GetHashValue(anchor_entry);
	BOOL result 		   = HTAnchor_addFormatParam(anchor, name, value);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " anchor name value", NULL);
    return TCL_ERROR;
  }
}

/* CHARSET */

int HTAnchor_charset_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name             = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTCharset result          = HTAnchor_charset(p_anchor);
       		
	Tcl_AppendResult(interp, HTAtom_name(result), NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_setCharset_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 3) {
    char *anchor_name 	          = argv[1];
    char *charset_name 	          = argv[2];
    if (anchor_name && charset_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTCharset charset         = HTAtom_for(charset_name);
	HTAnchor_setCharset(p_anchor, charset);
       		
	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor charset", NULL);
    return TCL_ERROR;
  }
}

/* LEVEL */

int HTAnchor_level_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name             = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTLevel result            = HTAnchor_level(p_anchor);
       		
	Tcl_AppendResult(interp, HTAtom_name(result), NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_setLevel_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 3) {
    char *anchor_name 	          = argv[1];
    char *level_name 	          = argv[2];
    if (anchor_name && level_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTLevel level 	          = HTAtom_for(level_name);
	HTAnchor_setLevel(p_anchor, level);
       		
	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor level", NULL);
    return TCL_ERROR;
  }
}

/* LANGUAGE */

int HTAnchor_language_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name             = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	Tcl_HashEntry *listPtr;
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTList *list              = HTAnchor_language(p_anchor);
	char *listname 		  = Keyname_lookupList(list);

	if ((!listname) && list) {
	  listPtr = CreateNewListEntry("HTList", &listname);
	  Tcl_SetHashValue(listPtr, list);
	}

	Tcl_AppendResult(interp, listname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_addLanguage_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 3) {
    char *anchor_name 	          = argv[1];
    char *language_name 	  = argv[2];
    if (anchor_name && language_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTLanguage language 	  = HTAtom_for(language_name);
	BOOL result = HTAnchor_addLanguage(p_anchor, language);
       		
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor language", NULL);
    return TCL_ERROR;
  }
}

/* ENCODING */

int HTAnchor_encoding_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name             = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	Tcl_HashEntry *listPtr;
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTList *list              = HTAnchor_encoding(p_anchor);
	char *listname 		  = Keyname_lookupList(list);

	if ((!listname) && (list)) {
	  listPtr = CreateNewListEntry("HTList", &listname);
	  Tcl_SetHashValue(listPtr, list);
	}
       		
	Tcl_AppendResult(interp, listname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_addEncoding_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 3) {
    char *anchor_name 	    	  = argv[1];
    char *encoding_name 	  = argv[2];
    if (anchor_name && encoding_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTEncoding encoding 	  = HTAtom_for(encoding_name);
	BOOL result = HTAnchor_addEncoding(p_anchor, encoding);
       		
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
       	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor encoding", NULL);
    return TCL_ERROR;
  }
}

/* TRANSFER */

int HTAnchor_transfer_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name             = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTEncoding result         = HTAnchor_transfer(p_anchor);
	Tcl_AppendResult(interp, HTAtom_name(result), NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_setTransfer_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 3) {
    char *anchor_name 	          = argv[1];
    char *transfer_name	    	  = argv[2];
    if (anchor_name && transfer_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTEncoding encoding    	  = HTAtom_for(transfer_name);
	HTAnchor_setTransfer(p_anchor, encoding);
       		
	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor transfer", NULL);
    return TCL_ERROR;
  }
}

/* LENGTH */

int HTAnchor_length_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name             = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	long int result           = HTAnchor_length(p_anchor);
	char *string=malloc(sizeof(result));
	sprintf(string, "%ld", result);
	Tcl_AppendResult(interp, string, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_setLength_tcl(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {
  if (argc == 3) {
    char *anchor_name 	          = argv[1];
    if (anchor_name && argv[2]) {
      long int length	 	  = atol(argv[2]);
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTAnchor_setLength(p_anchor, length);

	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor integer", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_addLength_tcl(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {
  if (argc == 3) {
    char *anchor_name 	          = argv[1];
    if (anchor_name && argv[2]) {
      long int length	 	  = atol(argv[2]);
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTAnchor_addLength(p_anchor, length);

	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor integer", NULL);
    return TCL_ERROR;
  }
}

/* MD5 */

int HTAnchor_md5_tcl(ClientData clientData, Tcl_Interp *interp, 	
		       int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name 		  = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
       	
	Tcl_AppendResult(interp, HTAnchor_md5(p_anchor), NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_setMd5_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {

  if (argc == 3) {
    char *anchor_name 		  = argv[1];
    char *md5		 	  = argv[2];
    if (anchor_name && md5) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTAnchor_setMd5(p_anchor, md5);
       		
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor md5", NULL);
    return TCL_ERROR;
  }
}

/* METHOD */

int HTAnchor_allow_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name             = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	int methods 	          = HTAnchor_allow(p_anchor);
	char *result = malloc(sizeof(methods));
	sprintf(result, "%d", methods);
	Tcl_AppendResult(interp, result, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_setAllow_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 3) {
    int methods;
    char *anchor_name 	          = argv[1];
    if (anchor_name && (Tcl_GetInt(interp, argv[2], &methods) == TCL_OK)) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTAnchor_setAllow(p_anchor, methods);
       		
	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor methods", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_appendMethods_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv) {
  if (argc == 3) {
    int methods;
    char *anchor_name 	          = argv[1];
    if (anchor_name && (Tcl_GetInt(interp, argv[2], &methods) == TCL_OK)) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTAnchor_appendMethods(p_anchor, methods);
       		
	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor methods", NULL);
    return TCL_ERROR;
  }
}

/* VERSION */

int HTAnchor_version_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name             = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	Tcl_AppendResult(interp, HTAnchor_version(p_anchor), NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_setVersion_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 3) {
    char *anchor_name 	          = argv[1];
    char *version	 	  = argv[2];
    if (anchor_name && version) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTAnchor_setVersion(p_anchor, version);
       		
	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor version", NULL);
    return TCL_ERROR;
  }
}

/* DATE */

int HTAnchor_date_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name             = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	time_t date 	          = HTAnchor_date(p_anchor);
	char *result	          = malloc(sizeof(date));	
	sprintf(result, "%ld", date); 
	Tcl_AppendResult(interp, result, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_setDate_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 3) {
    char *anchor_name 	          = argv[1];
    if (anchor_name && argv[2]) {
      time_t date	 	  = atol(argv[2]);
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTAnchor_setDate(p_anchor, date);
       		
	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor date", NULL);
    return TCL_ERROR;
  }
}

/* LASTMODIFIED */

int HTAnchor_lastModified_tcl(ClientData clientData, Tcl_Interp *interp, 
			      int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name             = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	time_t date 		  = HTAnchor_lastModified(p_anchor);
	char *result		  = malloc(sizeof(date));	
	sprintf(result, "%ld", date); 
	Tcl_AppendResult(interp, result, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_setLastModified_tcl(ClientData clientData, Tcl_Interp *interp, 
				 int argc, char **argv) {
  if (argc == 3) {
    char *anchor_name 	          = argv[1];
    if (anchor_name && argv[2]) {
      time_t date	 	  = atol(argv[2]);
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTAnchor_setLastModified(p_anchor, date);
       		
	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor date", NULL);
    return TCL_ERROR;
  }
}

/* ETAG */

int HTAnchor_etag_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name             = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	Tcl_AppendResult(interp, HTAnchor_etag(p_anchor), NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_setEtag_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 3) {
    char *anchor_name 	          = argv[1];
    char *etag	 	  = argv[2];
    if (anchor_name && etag) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTAnchor_setEtag(p_anchor, etag);
       		
	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor etag", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_isEtagWeak_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name             = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	BOOL result		  = HTAnchor_isEtagWeak(p_anchor);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

/* EXPIRES */

int HTAnchor_expires_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name             = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	time_t date 	          = HTAnchor_expires(p_anchor);
	char *result	          = malloc(sizeof(date));	
	sprintf(result, "%ld", date); 
	Tcl_AppendResult(interp, result, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_setExpires_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 3) {
    char *anchor_name 	          = argv[1];
    if (anchor_name && argv[2]) {
      time_t date	 	  = atol(argv[2]);
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTAnchor_setExpires(p_anchor, date);
       		
	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor date", NULL);
    return TCL_ERROR;
  }
}

/* DERIVED */

int HTAnchor_derived_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name             = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	Tcl_AppendResult(interp, HTAnchor_derived(p_anchor), NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_setDerived_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 3) {
    char *anchor_name 	          = argv[1];
    char *derived	 	  = argv[2];
    if (anchor_name && derived) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
	HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
	HTAnchor_setDerived(p_anchor, derived);
       		
	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor derived_from", NULL);
    return TCL_ERROR;
  }
}

/* EXTRA */
#if 0
int HTAnchor_extra_tcl(ClientData clientData, Tcl_Interp *interp, 
                       int argc, char **argv) {
  if (argc == 2) {
    char *anchor_name             = argv[1];
    if (anchor_name) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
        HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
        HTList *list              = HTAnchor_Extra(p_anchor);
	char *keyname             = Keyname_lookupList(list);

	if (!keyname) {
	  Tcl_HashEntry *entryPtr = CreateNewListEntry("HTList", &keyname);
	  Tcl_SetHashValue(entryPtr, list);       
	}
	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor", NULL);
    return TCL_ERROR;
  }
}

int HTAnchor_addExtra_tcl(ClientData clientData, Tcl_Interp *interp, 
                          int argc, char **argv) {
  if (argc == 3) {
    char *anchor_name             = argv[1];
    char *extra                   = argv[2];
    if (anchor_name && extra) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_name);
      if (anchor_entry) {
        HTParentAnchor *p_anchor  = Tcl_GetHashValue(anchor_entry);
        HTAnchor_addExtra(p_anchor, extra);
                
        Tcl_AppendResult(interp, NULL);
        return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " parent_anchor extra_header", NULL);
    return TCL_ERROR;
  }
}
#endif
/*Do we need HTAnchor_headerParsed? */
