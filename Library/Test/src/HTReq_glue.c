#include "HTReq_glue.h"

#define max_keyname 20
#define bad_vars "Invalid variable names or non-existent entries."
#define err_string "wrong # args: should be "

Tcl_HashTable   HTableReq;
Tcl_HashTable	HTableAnchor;
Tcl_HashTable	HTableLink;
Tcl_HashTable	HTableList;
Tcl_HashTable	HTableStream;
Tcl_HashTable	HTableAssoc;
Tcl_HashTable	HTableVoid;
Tcl_HashTable	HTableUser;
Tcl_HashTable	HTableMIMEParseSet;
Tcl_HashTable	HTableCallback;
Tcl_HashTable	HTableNet;

/*****************************
*         HTRequest          *
*****************************/


/* HTLOAD */

int HTLoad_tcl(ClientData clientData, Tcl_Interp *interp, 
	       int argc, char **argv) {
  if (argc == 3) {
    char *keyname 	      = argv[1];
    char *bool_str 	      = argv[2];
    int recursive;
    if (keyname && (Tcl_GetBoolean(interp, bool_str, &recursive) == TCL_OK)) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, keyname);
      if (request_entry) {
	HTRequest *request    = Tcl_GetHashValue(request_entry);
	BOOL result           = HTLoad(request, recursive);

	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request boolean", NULL);
    return TCL_ERROR;
  }
}

/* KILL */

int HTRequest_kill_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	BOOL result 		   = HTRequest_kill(request);
	Tcl_DeleteHashEntry(request_entry);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request", NULL);
    return TCL_ERROR;
  }
}

/* NEW */

int HTRequest_new_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
    if (argc == 1) {
	char *keyname;
	Tcl_HashEntry *request_entry;
	HTRequest *request = HTRequest_new();

	request_entry 	   = CreateNewRequestEntry("Request", &keyname);
	Tcl_SetHashValue(request_entry, request);        

	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
    }
    else {
	Tcl_AppendResult(interp, err_string, argv[0], NULL);
	return TCL_ERROR;
    }
}

/* CLEAR */

int HTRequest_clear_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 2) {
    char *keyname 	           = argv[1];
    if (keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, keyname);
      if (request_entry) {
 	HTRequest *request         = Tcl_GetHashValue(request_entry);
	BOOL result 		   = HTRequest_clear(request);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request",  NULL);
    return TCL_ERROR;
  }
}

/* DUP */

int HTRequest_dup_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 2) {
    char *keyname             = argv[1];
    if (keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, keyname);
      if (request_entry) {
 	Tcl_HashEntry *request_entry2;
	HTRequest *original   = Tcl_GetHashValue(request_entry);
	HTRequest *request    = HTRequest_dup(original);

	request_entry2        = CreateNewRequestEntry("Request", &keyname);
	Tcl_SetHashValue(request_entry2, request);        

	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR; 
    }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " source_request",  NULL);
    return TCL_ERROR;
  }
}

/* DUPINTERNAL */

int HTRequest_dupInternal_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 2) {
    char *keyname             = argv[1];
    if (keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, keyname);
      if (request_entry) {
 	Tcl_HashEntry *request_entry2;
	HTRequest *original   = Tcl_GetHashValue(request_entry);
	HTRequest *request    = HTRequest_dupInternal(original);

	request_entry2        = CreateNewRequestEntry("Request", &keyname);
	Tcl_SetHashValue(request_entry2, request);        

	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR; 
    }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " source_request",  NULL);
    return TCL_ERROR;
  }
}

/* DELETE */

int HTRequest_delete_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 2) {
    char *keyname 	      = argv[1];
    if (keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, keyname);
      if (request_entry) {
 	HTRequest *request    = Tcl_GetHashValue(request_entry);
	HTRequest_delete(request);
	Tcl_DeleteHashEntry(request_entry);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request",  NULL);
    return TCL_ERROR;
  }
}

/* USERPROFILE */

int HTRequest_setUserProfile_tcl(ClientData clientData, Tcl_Interp *interp, 
				 int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *user_name		   = argv[2];
    if (req_keyname && user_name) {
      Tcl_HashEntry *user_entry    = Tcl_FindHashEntry(&HTableUser, user_name);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry && user_entry) {
        HTUserProfile *user 	   = Tcl_GetHashValue(user_entry);
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTRequest_setUserProfile(request, user);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
    else {
      Tcl_AppendResult(interp, err_string, argv[0], " request userprofile", NULL);
      return TCL_ERROR;
    }
}

int HTRequest_userProfile_tcl(ClientData clientData, Tcl_Interp *interp,
			      int argc, char **argv) { 
  if (argc == 2) {
    char *req_keyname 		   = argv[1]; 
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq,req_keyname); 
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry); 
	HTUserProfile *user 	   = HTRequest_userProfile(request);
	char *user_name		   = Keyname_lookupUser(user);
	if (!user_name) {
	  Tcl_HashEntry *user_entry = CreateNewUserEntry("User", &user_name);
	  Tcl_SetHashValue(user_entry, user);
	}
	Tcl_AppendResult(interp, user_name, NULL); 
	return TCL_OK; 
      } 
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  } 
  else  { 
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return  TCL_ERROR; 
  } 
}

/* METHOD */

int HTRequest_setMethod_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *method_name		   = argv[2];
    if (req_keyname && method_name) {
      HTMethod method 		   = HTMethod_enum(method_name);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTRequest_setMethod(request, method);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
    else {
      Tcl_AppendResult(interp, err_string, argv[0], " request method", NULL);
      return TCL_ERROR;
    }
}

int HTRequest_method_tcl(ClientData clientData, Tcl_Interp *interp,
			 int argc, char **argv) { 
  if (argc == 2) {
    char *req_keyname 		   = argv[1]; 
    if (req_keyname) {
      char *method_name; 
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq,req_keyname); 
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry); 
	HTMethod method 	   = HTRequest_method(request);
	sprintf(method_name, "%x", method);
	Tcl_AppendResult(interp, method_name, NULL); 
	return TCL_OK; 
      } 
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  } 
  else  { 
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return  TCL_ERROR; 
  } 
}

/* RELOADMODE */

int HTRequest_setReloadMode_tcl(ClientData clientData, Tcl_Interp *interp, 
				int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *mode_name		   = argv[2];
    if (req_keyname && mode_name) {
      HTReload reload 		   = HTReload_enum(mode_name);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTRequest_setReloadMode(request, reload);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request HTReload_mode", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_reloadMode_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname  	     	   = argv[1];
    char *mode_name;
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTReload reload 	   = HTRequest_reloadMode(request);
	sprintf(mode_name, "%x", reload);
	Tcl_AppendResult(interp, mode_name, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

/* REDIRECTION */

int HTRequest_setRedirection_tcl(ClientData clientData, Tcl_Interp *interp, 
			      int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *anchor_keyname	   = argv[2];
    if (req_keyname && anchor_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      Tcl_HashEntry *anchor_entry  = Tcl_FindHashEntry(&HTableAnchor, anchor_keyname);
      if (request_entry && anchor_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTAnchor *anchor 	   = Tcl_GetHashValue(anchor_entry);
	BOOL result		   = HTRequest_setRedirection(request, anchor);

	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request anchor", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_redirection_tcl(ClientData clientData, Tcl_Interp *interp, 
			      int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      Tcl_HashEntry *anchor_entry;
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTAnchor *anchor 	   = HTRequest_redirection(request);
	char *keyname 		   = Keyname_lookupAnchor(anchor);
	if (!keyname) {
	  anchor_entry = CreateNewAnchorEntry("Anchor", &keyname);
	  Tcl_SetHashValue(anchor_entry, anchor);
	}
	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

/********************************
*    Access Authentication      *
********************************/

/* CHALLENGE */

int HTRequest_addChallenge_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *token		   	   = argv[2];
    char *value		   	   = argv[3];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	BOOL result 		   = HTRequest_addChallenge(request, token, value);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request challenge_list", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_deleteChallenge_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	BOOL result      	   = HTRequest_deleteChallenge(request);

	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_challenge_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    char *listname 		   = malloc(max_keyname);
    if (req_keyname) {
      Tcl_HashEntry *list_entry;
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTAssocList *list      	   = HTRequest_challenge(request);
	listname 		   = Keyname_lookupAssocList(list);
	if (!listname) {
	  list_entry = CreateNewAssocListEntry("AssocList", &listname);
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
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

/* CREDENTIALS */

int HTRequest_addCredentials_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv) {
  if (argc == 4) {
    char *req_keyname		   = argv[1];
    char *token		   	   = argv[2];
    char *value			   = argv[3];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	BOOL result 		   = HTRequest_addCredentials(request, token, value);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request token value", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_deleteCredentials_tcl(ClientData clientData, Tcl_Interp *interp, 
				    int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	BOOL result      	   = HTRequest_deleteCredentials(request);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_credentials_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    char *listname 		   = malloc(max_keyname);
    if (req_keyname) {
      Tcl_HashEntry *list_entry;
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTAssocList *list      	   = HTRequest_credentials(request);
	listname 		   = Keyname_lookupAssocList(list);
	if (!listname) {
	  list_entry = CreateNewAssocListEntry("AssocList", &listname);
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
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

/* REALM */

int HTRequest_setRealm_tcl(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *realm 		   = argv[2];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	BOOL result 		   = HTRequest_setRealm(request, realm);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request realm", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_realm_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname	   	   = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	CONST char *realm	   = HTRequest_realm(request);
	Tcl_AppendResult(interp, realm, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

/* SCHEME */

int HTRequest_setScheme_tcl(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *scheme 		   = argv[2];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	BOOL result 		   = HTRequest_setScheme(request, scheme);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request scheme", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_scheme_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname	   	   = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	CONST char *scheme	   = HTRequest_scheme(request);
	Tcl_AppendResult(interp, scheme, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

/* MAXRETRY */

int HTRequest_setMaxRetry_tcl(ClientData clientData, Tcl_Interp *interp, 
			      int argc, char **argv) {
  if (argc == 2) {
    int  maxretry;
    if (Tcl_GetInt(interp, argv[1], &maxretry) == TCL_OK) {
      BOOL result = HTRequest_setMaxRetry(maxretry);
      Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
      return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " max_retry", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_maxRetry_tcl(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {
  if (argc == 1) {
    int  maxretry = HTRequest_maxRetry();
    char *result=malloc(sizeof(maxretry));
    sprintf(result, "%d", maxretry);
    Tcl_AppendResult(interp, result, NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], NULL);
    return TCL_ERROR;
  }
}

/* RETRY */

int HTRequest_retrys_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	int result 		   = HTRequest_retrys(request);
	char *output;
	sprintf(output, "%d", result);
	Tcl_AppendResult(interp, output, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else { 
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_doRetry_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	BOOL result 		   = HTRequest_doRetry(request);

	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else { 
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

/* RETRYTIME */

int HTRequest_retryTime_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	time_t time 		   = HTRequest_retryTime(request);
	char *result 		   = malloc(sizeof(time));
	sprintf(result, "%ld", time);
	Tcl_AppendResult(interp, result, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

/* CONVERSION */

int HTRequest_setConversion_tcl(ClientData clientData, Tcl_Interp *interp, 
				int argc, char **argv) {
  if (argc == 4) {
    int override;
    char *req_keyname		   = argv[1];
    char *list_keyname		   = argv[2];
    char *override_str     	   = argv[3];
    if (req_keyname && list_keyname && (Tcl_GetBoolean(interp, override_str, &override) == TCL_OK)) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      Tcl_HashEntry *list_entry    = Tcl_FindHashEntry(&HTableList, list_keyname);	
      if (request_entry && list_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTList *list    	   = Tcl_GetHashValue(list_entry);
	HTRequest_setConversion(request, list, override);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request conversion_list override", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_conversion_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      char *listname 		   = malloc(max_keyname);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTList *list 		   = HTRequest_conversion(request);
	listname 		   = Keyname_lookupList(list);
	Tcl_AppendResult(interp, listname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

/* ENCODING */

int HTRequest_setEncoding_tcl(ClientData clientData, Tcl_Interp *interp, 
				int argc, char **argv) {
  if (argc == 4) {
    int override;
    char *req_keyname		   = argv[1];
    char *list_keyname		   = argv[2];
    char *override_str     	   = argv[3];
    if (req_keyname && list_keyname && (Tcl_GetBoolean(interp, override_str, &override) == TCL_OK)) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      Tcl_HashEntry *list_entry    = Tcl_FindHashEntry(&HTableList, list_keyname);	
      if (request_entry && list_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTList *list    	   = Tcl_GetHashValue(list_entry);
	HTRequest_setEncoding(request, list, override);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request encoding_list override", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_encoding_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    char *listname 		   = malloc(max_keyname);
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTList *list 	   	   = HTRequest_encoding(request);
	listname 		   = Keyname_lookupList(list);
	Tcl_AppendResult(interp, listname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

/* TRANSFER */

int HTRequest_setTransfer_tcl(ClientData clientData, Tcl_Interp *interp, 
				int argc, char **argv) {
  if (argc == 4) {
    int override;
    char *req_keyname		   = argv[1];
    char *list_keyname		   = argv[2];
    char *override_str     	   = argv[3];
    if (req_keyname && list_keyname && (Tcl_GetBoolean(interp, override_str, &override) == TCL_OK)) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      Tcl_HashEntry *list_entry    = Tcl_FindHashEntry(&HTableList, list_keyname);	
      if (request_entry && list_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTList *list    	   = Tcl_GetHashValue(list_entry);
	HTRequest_setTransfer(request, list, override);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request transfer_list override", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_transfer_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    char *listname 		   = malloc(max_keyname);
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTList *list 	   	   = HTRequest_transfer(request);
	listname 		   = Keyname_lookupList(list);
	Tcl_AppendResult(interp, listname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

/* LANGUAGE */

int HTRequest_setLanguage_tcl(ClientData clientData, Tcl_Interp *interp, 
			      int argc, char **argv) {
  if (argc == 4) {
    int override;
    char *req_keyname		   = argv[1];
    char *list_keyname		   = argv[2];
    char *override_str     	   = argv[3];
    if (req_keyname && list_keyname && (Tcl_GetBoolean(interp, override_str, &override) == TCL_OK)) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      Tcl_HashEntry *list_entry    = Tcl_FindHashEntry(&HTableList, list_keyname);	
      if (request_entry && list_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTList *list    	   = Tcl_GetHashValue(list_entry);
	HTRequest_setLanguage(request, list, override);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request language_list override", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_language_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname	= argv[1];
    if (req_keyname) {
      char *listname 		= malloc(max_keyname);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request = Tcl_GetHashValue(request_entry);
	HTList *list = HTRequest_language(request);
	listname = Keyname_lookupList(list);
	Tcl_AppendResult(interp, listname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

/* CHARSET */

int HTRequest_setCharset_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 4) {
    int override;
    char *req_keyname		   = argv[1];
    char *list_keyname		   = argv[2];
    char *override_str     	   = argv[3];
    if (req_keyname && list_keyname && (Tcl_GetBoolean(interp, override_str, &override) == TCL_OK)) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      Tcl_HashEntry *list_entry    = Tcl_FindHashEntry(&HTableList, list_keyname);	
      if (request_entry && list_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTList *list    	   = Tcl_GetHashValue(list_entry);
	HTRequest_setCharset(request, list, override);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request charset_list override", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_charset_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      char *listname  	 	   = malloc(max_keyname);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTList *list 		   = HTRequest_charset(request);
	listname 		   = Keyname_lookupList(list);
	Tcl_AppendResult(interp, listname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

/*****************************
 * Handling Metainformation  *
 ****************************/

/* GNHD */

int HTRequest_setGnHd_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *mode_name		   = argv[2];
    if (req_keyname && mode_name) {
      HTGnHd gnHd = HTGnHd_enum(mode_name);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTRequest_setGnHd(request, gnHd);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request HTGnHd_mode", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_addGnHd_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname	           = argv[1];
    char *mode_name		   = argv[2];
    if (req_keyname && mode_name) {
      HTGnHd gnHd 		   = HTGnHd_enum(mode_name);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTRequest_addGnHd(request, gnHd);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request HTGnHd_mode", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_gnHd_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      char *mode_name;
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request   	   = Tcl_GetHashValue(request_entry);
	HTGnHd gnHd 		   = HTRequest_gnHd(request);
	sprintf(mode_name, "%x", gnHd);
	Tcl_AppendResult(interp, mode_name, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

/* RQHD */

int HTRequest_setRqHd_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *mode_name		   = argv[2];
    if (req_keyname && mode_name) {
      HTRqHd rqhd 		   = HTRqHd_enum(mode_name);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTRequest_setRqHd(request, rqhd);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
    else {
    Tcl_AppendResult(interp, err_string, argv[0], " request HTRqHd_mode", NULL);
    return TCL_ERROR;
  }
}


int HTRequest_addRqHd_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *mode_name		   = argv[2];
    if (req_keyname && mode_name) {
      HTRqHd rqhd 		   = HTRqHd_enum(mode_name);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTRequest_addRqHd(request, rqhd);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request HTRqHd_mode", NULL);
    return TCL_ERROR;
  }
}


int HTRequest_rqHd_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      char *mode_name;
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTRqHd rqhd 		   = HTRequest_rqHd(request);
	sprintf(mode_name, "%x", rqhd);
	Tcl_AppendResult(interp, mode_name, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

/* RSHD */

int HTRequest_setRsHd_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *mode_name		   = argv[2];
    if (req_keyname && mode_name) {
      HTRsHd rsHd 		   = HTRsHd_enum(mode_name);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTRequest_setRsHd(request, rsHd);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request HTRsHd_mode", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_addRsHd_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *mode_name		   = argv[2];
    if (req_keyname && mode_name) {
      HTRsHd rsHd 		   = HTRsHd_enum(mode_name);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTRequest_addRsHd(request, rsHd);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request HTRsHd_mode", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_rsHd_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      char *mode_name;
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTRsHd rsHd 		   = HTRequest_rsHd(request);
	sprintf(mode_name, "%x", rsHd);
	Tcl_AppendResult(interp, mode_name, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request HTRsHd_mode", NULL);
    return TCL_ERROR;
  }
}

  
/* ENHD */

int HTRequest_setEnHd_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *mode_name		   = argv[2];
    if (req_keyname && mode_name) {
      HTEnHd enHd 		   = HTEnHd_enum(mode_name);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTRequest_setEnHd(request, enHd);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request HTEnHd_mode", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_addEnHd_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *mode_name		   = argv[2];
    if (req_keyname && mode_name) {
      HTEnHd enHd 		   = HTEnHd_enum(mode_name);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTRequest_addEnHd(request, enHd);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request HTEnHd_mode", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_enHd_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      char *mode_name;
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTEnHd enHd 		   = HTRequest_enHd(request);
	sprintf(mode_name, "%x", enHd);
	Tcl_AppendResult(interp, mode_name, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

/* PARENT */

int HTRequest_setParent_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *anchor_keyname	   = argv[2];
    if (req_keyname && anchor_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      Tcl_HashEntry *anchor_entry  = Tcl_FindHashEntry(&HTableAnchor, anchor_keyname);
      if (request_entry && anchor_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTAnchor *anchor 	   = Tcl_GetHashValue(anchor_entry);
	HTRequest_setParent(request, (HTParentAnchor *) anchor);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request anchor", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_parent_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      char *anchor_keyname 	   = malloc(max_keyname);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTParentAnchor *anchor 	   = HTRequest_parent(request);
	anchor_keyname 		   = Keyname_lookupAnchor((HTAnchor *) anchor);
	Tcl_AppendResult(interp, anchor_keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

/* GENERATOR */

int HTRequest_setGenerator_tcl(ClientData clientData, Tcl_Interp *interp, 
				int argc, char **argv) {
  if (argc == 4) {
    int override;
    char *req_keyname		   = argv[1];
    char *list_keyname		   = argv[2];
    char *override_str     	   = argv[3];
    if (req_keyname && list_keyname && (Tcl_GetBoolean(interp, override_str, &override) == TCL_OK)) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      Tcl_HashEntry *list_entry    = Tcl_FindHashEntry(&HTableList, list_keyname);	
      if (request_entry && list_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTList *list    	   = Tcl_GetHashValue(list_entry);
	HTRequest_setGenerator(request, list, override);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request generator_list override", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_generator_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 3) {
    BOOL override;
    char *req_keyname		   = argv[1];
    char *override_str		   = argv[2];
    if (req_keyname && override_str) {
      char *listname 		   = malloc(max_keyname);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTList *list 		   = HTRequest_generator(request, &override);
	listname 		   = Keyname_lookupList(list);
	Tcl_AppendResult(interp, listname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

/* MIMEParseSet */

int HTRequest_setMIMEParseSet_tcl(ClientData clientData, Tcl_Interp *interp, 
				  int argc, char **argv) {
  if (argc == 4) {
    int override;
    char *req_keyname		   = argv[1];
    char *set_keyname		   = argv[2];
    char *override_str     	   = argv[3];
    if (req_keyname && set_keyname && (Tcl_GetBoolean(interp, override_str, &override) == TCL_OK)) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      Tcl_HashEntry *set_entry    = Tcl_FindHashEntry(&HTableMIMEParseSet, set_keyname);	
      if (request_entry && set_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTMIMEParseSet *set    	   = Tcl_GetHashValue(set_entry);
	HTRequest_setMIMEParseSet(request, set, override);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request parser_list override", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_MIMEParseSet_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv) {
  if (argc == 3) {
    BOOL local;
    char *req_keyname		   = argv[1];
    char *local_str		   = argv[2];
    if (req_keyname && local_str) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTMIMEParseSet *set 	   = HTRequest_MIMEParseSet(request, &local);
	char *setname 		   = Keyname_lookupHTMIMEParseSet(set);
	char *check		   = Tcl_SetVar(interp, local_str, local ? "YES" : "NO", 0);

	if (check) Tcl_AppendResult(interp, setname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

/********************************
* Sending Data to the Network   *
********************************/


/**************************************************

typedef int HTPostCallback (HTRequest * request, HTStream * target);

extern void HTRequest_setPostCallback (HTRequest * request, HTPostCallback * cbf);
extern HTPostCallback * HTRequest_postCallback (HTRequest * request);

*/

/* ENTITYANCHOR */

int HTRequest_setEntityAnchor_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *anchor_keyname	   = argv[2];
    if (req_keyname && anchor_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      Tcl_HashEntry *anchor_entry  = Tcl_FindHashEntry(&HTableAnchor, anchor_keyname);
      if (request_entry && anchor_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTAnchor *anchor 	   = Tcl_GetHashValue(anchor_entry);
	HTRequest_setEntityAnchor(request, (HTParentAnchor *) anchor);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request anchor", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_entityAnchor_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      char *anchor_keyname 	   = malloc(max_keyname);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTParentAnchor *anchor 	   = HTRequest_entityAnchor(request);
	anchor_keyname 		   = Keyname_lookupAnchor((HTAnchor *) anchor);
	Tcl_AppendResult(interp, anchor_keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

/* INPUTSTREAM */

int HTRequest_setInputStream_tcl(ClientData clientData, Tcl_Interp *interp, 
				 int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *stream_keyname	   = argv[2];
    if (req_keyname && stream_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      Tcl_HashEntry *stream_entry  = Tcl_FindHashEntry(&HTableStream, stream_keyname);
      if (request_entry && stream_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTStream *stream 	   = Tcl_GetHashValue(stream_entry);
	HTRequest_setInputStream(request, stream);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request stream", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_inputStream_tcl(ClientData clientData, Tcl_Interp *interp, 
			      int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      char *stream_name;
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTStream *stream 	   = HTRequest_inputStream(request);
	stream_name 		   = Keyname_lookupStream(stream);
	Tcl_AppendResult(interp, stream_name, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request", NULL);
    return TCL_ERROR;
  }
}

/* ISPOSTWEB */

int HTRequest_isPostWeb_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 2) {
    char *keyname 	      = argv[1];
    if (keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, keyname);
      if (request_entry) {
 	HTRequest *request    = Tcl_GetHashValue(request_entry);
	BOOL result = HTRequest_isPostWeb(request);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request",  NULL);
    return TCL_ERROR;
  }
}

/* SOURCE */

int HTRequest_setSource_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 3) {
    char *keyname              = argv[1];
    char *source               = argv[2];
    if (keyname && source) {
      Tcl_HashEntry *request_entry  = Tcl_FindHashEntry(&HTableReq, keyname);
      Tcl_HashEntry *source_entry = Tcl_FindHashEntry(&HTableReq, source);
      if (request_entry && source_entry) {
	HTRequest *request     = Tcl_GetHashValue(request_entry);
	HTRequest *source      = Tcl_GetHashValue(source_entry);
	BOOL result 	       = HTRequest_setSource(request, source);

	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR; 
    }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request source_request",  NULL);
    return TCL_ERROR;
  }
}

int HTRequest_source_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 2) {
    char *keyname              = argv[1];
    if (keyname) {
      Tcl_HashEntry *request_entry  = Tcl_FindHashEntry(&HTableReq, keyname);
      if (request_entry) {
	HTRequest *request     = Tcl_GetHashValue(request_entry);
	HTRequest *source      = HTRequest_source(request);
	char *source_name      = Keyname_lookupRequest(source);
	if (!source_name) {
	  Tcl_HashEntry *source_entry = CreateNewRequestEntry("Request", &source_name);
	  Tcl_SetHashValue(source_entry, source);
	}
	  
	Tcl_AppendResult(interp, source_name, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR; 
    }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request source_request",  NULL);
    return TCL_ERROR;
  }
}

/********************************
* From network to application   *
********************************/


/* OUTPUTSTREAM */

int HTRequest_setOutputStream_tcl(ClientData clientData, Tcl_Interp *interp, 
				  int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *stream_keyname	   = argv[2];
    if (req_keyname && stream_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      Tcl_HashEntry *stream_entry  = Tcl_FindHashEntry(&HTableStream, stream_keyname);
      if (request_entry && stream_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTStream *stream 	   = Tcl_GetHashValue(stream_entry);
	HTRequest_setOutputStream(request, stream);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request stream", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_outputStream_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      char *stream_name;
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTStream *stream 	   = HTRequest_outputStream(request);
	stream_name 		   = Keyname_lookupStream(stream);
	Tcl_AppendResult(interp, stream_name, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request", NULL);
    return TCL_ERROR;
  }
}

/* OUTPUTCONNECTED */

int HTRequest_setOutputConnected_tcl(ClientData clientData, Tcl_Interp *interp, 
				     int argc, char **argv) {
  if (argc == 3) {
    int Connected;
    char *req_keyname		   = argv[1];
    char *outputConnected_str      = argv[2];
    if (req_keyname && (Tcl_GetBoolean(interp, outputConnected_str, &Connected) == TCL_OK)) { 
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTRequest_setOutputConnected(request, Connected);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request outputConnected?", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_outputConnected_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	BOOL outputConnected 	   = HTRequest_outputConnected(request);
	Tcl_AppendResult(interp, outputConnected ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request", NULL);
    return TCL_ERROR;
  }
}

/* OUTPUTFORMAT */

int HTRequest_setOutputFormat_tcl(ClientData clientData, Tcl_Interp *interp, 
				  int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *format_keyname	   = argv[2];
    if (req_keyname && format_keyname) {
      HTFormat format		   = HTAtom_for(format_keyname);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTRequest_setOutputFormat(request, format);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request format", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_outputFormat_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTFormat format 	   = HTRequest_outputFormat(request);
	Tcl_AppendResult(interp, HTAtom_name(format), NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request", NULL);
    return TCL_ERROR;
  }
}

/* DEBUGSTREAM */

int HTRequest_setDebugStream_tcl(ClientData clientData, Tcl_Interp *interp, 
				  int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname	           = argv[1];
    char *stream_keyname	   = argv[2];
    if (req_keyname && stream_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      Tcl_HashEntry *stream_entry  = Tcl_FindHashEntry(&HTableStream, stream_keyname);
      if (request_entry && stream_entry) {
	HTRequest *request         = Tcl_GetHashValue(request_entry);
	HTStream *stream 	   = Tcl_GetHashValue(stream_entry);
	HTRequest_setDebugStream(request, stream);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request stream", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_debugStream_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      char *stream_name;
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTStream *stream 	   = HTRequest_debugStream(request);
	stream_name 	 	   = Keyname_lookupStream(stream);
	Tcl_AppendResult(interp, stream_name, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request", NULL);
    return TCL_ERROR;
  }
}

/* DEBUGFORMAT */

int HTRequest_setDebugFormat_tcl(ClientData clientData, Tcl_Interp *interp, 
				  int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *format_keyname	   = argv[2];
    if (req_keyname && format_keyname) {
      HTFormat Format		   = HTAtom_for(format_keyname);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTRequest_setDebugFormat(request, Format);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request format", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_debugFormat_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTFormat format 	   = HTRequest_debugFormat(request);
	Tcl_AppendResult(interp, HTAtom_name(format), NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request", NULL);
    return TCL_ERROR;
  }
}

/********************************
*   Before and After filters    *
********************************/

/* BEFORE */

int HTRequest_addBefore_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 3) {
    int override;
    int status;
    char *req_keyname		   = argv[1];
    char *net_keyname		   = argv[2];
    char *param_keyname		   = argv[3];
    char *status_str		   = argv[4];
    char *override_str     	   = argv[5];
    if (req_keyname && net_keyname && (Tcl_GetBoolean(interp, override_str, &override) == TCL_OK) && (Tcl_GetInt(interp, status_str, &status) == TCL_OK)) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      Tcl_HashEntry *void_entry    = Tcl_FindHashEntry(&HTableVoid, param_keyname);
      if (request_entry && void_entry) {
	HTNetCallback *filter;
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	void *param    	   	   = Tcl_GetHashValue(void_entry);
	BOOL result 		   = HTRequest_addBefore(request, filter, param, status, override);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request netcallback param status override", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_deleteBefore_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 6) {

    char *req_keyname		   = argv[1];
    char *net_keyname		   = argv[2];
    if (req_keyname && net_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTNetCallback *filter;
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	BOOL result		   = HTRequest_deleteBefore(request, filter);

	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request netcallback", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_deleteBeforeAll_tcl(ClientData clientData, Tcl_Interp *interp, 
				  int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	BOOL result 		   = HTRequest_deleteBeforeAll(request);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_before_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 3) {
    BOOL override;
    char *req_keyname		   = argv[1];
    char *override_str		   = argv[2];
    if (req_keyname && override_str) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTList *list 		   = HTRequest_before(request, &override);
	char *listname 		   = Keyname_lookupList(list);
	Tcl_SetVar(interp, override_str, override ? "YES" : "NO", 0); 
	Tcl_AppendResult(interp, listname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request override", NULL);
    return TCL_ERROR;
  }
}

/* AFTER */

int HTRequest_addAfter_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 3) {
    int override;
    int status;
    char *req_keyname		   = argv[1];
    char *net_keyname		   = argv[2];
    char *param_keyname		   = argv[3];
    char *status_str		   = argv[4];
    char *override_str     	   = argv[5];
    if (req_keyname && net_keyname && (Tcl_GetBoolean(interp, override_str, &override) == TCL_OK) && (Tcl_GetInt(interp, status_str, &status) == TCL_OK)) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      Tcl_HashEntry *void_entry    = Tcl_FindHashEntry(&HTableVoid, param_keyname);
      if (request_entry && void_entry) {
	HTNetCallback *filter;
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	void *param    	   	   = Tcl_GetHashValue(void_entry);
	BOOL result 		   = HTRequest_addAfter(request, filter, param, status, override);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request netcallback param status override", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_deleteAfter_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 6) {
    char *req_keyname		   = argv[1];
    char *net_keyname		   = argv[2];
    if (req_keyname && net_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTNetCallback *filter;
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	BOOL result		   = HTRequest_deleteAfter(request, filter);

	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request netcallback", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_deleteAfterAll_tcl(ClientData clientData, Tcl_Interp *interp, 
				  int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	BOOL result 		   = HTRequest_deleteAfterAll(request);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_after_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 3) {
    BOOL override;
    char *req_keyname		   = argv[1];
    char *override_str		   = argv[2];
    if (req_keyname && override_str) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTList *list 		   = HTRequest_after(request, &override);
	char *listname 		   = Keyname_lookupList(list);
	Tcl_SetVar(interp, override_str, override ? "YES" : "NO", 0);
	Tcl_AppendResult(interp, listname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request override", NULL);
    return TCL_ERROR;
  }
}

/********************************
*      Context Swapping         *
********************************/

/* CALLBACK */

int HTRequest_setCallback_tcl(ClientData clientData, Tcl_Interp *interp, 
			      int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *callback_keyname	   = argv[2];
    if (req_keyname && callback_keyname) {
      Tcl_HashEntry *callback_entry = Tcl_FindHashEntry(&HTableCallback, callback_keyname);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry && callback_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTRequestCallback *callback = Tcl_GetHashValue(callback_entry);
	HTRequest_setCallback(request, callback);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request callback", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_callback_tcl(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) 
{
    if (argc == 2) {
	char *req_keyname		   = argv[1];
	if (req_keyname) {
	    Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
	    if (request_entry) {
		HTRequest *request 	   = Tcl_GetHashValue(request_entry);
		HTRequestCallback *callback = HTRequest_callback(request);
		 char *callback_str = Keyname_lookupCallback(callback);
		
		Tcl_AppendResult(interp, callback_str, NULL);
		return TCL_OK;
	    }
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;
    }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request", NULL);
    return TCL_ERROR;
  }
}

/* CONTEXT */

int HTRequest_setContext_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 3) {
    char *request_name 		   = argv[1];
    char *context_name             = argv[2];
    if (request_name && context_name) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_name);
      Tcl_HashEntry *context_entry = Tcl_FindHashEntry(&HTableVoid, context_name);
      if (request_entry && context_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);	
	void *context  		   = Tcl_GetHashValue(context_entry);

	HTRequest_setContext(request, context);
	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp,err_string, argv[0], " request context", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_context_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 2) {
    char *request_name 		   = argv[1];
    if (request_name) {
      Tcl_HashEntry *context_entry;
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_name);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	void *result 		   = HTRequest_context(request);

	char *keyname              = Keyname_lookupVoid((void *) result);
	if (!keyname) {
	  context_entry        	   = CreateNewVoidEntry("Context", &keyname);
	  Tcl_SetHashValue(context_entry, result);
	}

	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp,err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

/* PREEMPTIVE */

int HTRequest_setPreemptive_tcl(ClientData clientData, Tcl_Interp *interp, 
				 int argc, char **argv) {
  if (argc == 3) {
    int preemptive;
    char *req_keyname		   = argv[1];
    char *preemptive_str    	   = argv[2];
    if (req_keyname && (Tcl_GetBoolean(interp, preemptive_str, &preemptive) == TCL_OK)) { 
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTRequest_setPreemptive(request, preemptive);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request preemptive?", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_preemptive_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	BOOL preemptive 	   = HTRequest_preemptive(request);
	Tcl_AppendResult(interp, preemptive ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request", NULL);
    return TCL_ERROR;
  }
}

/* PRIORITY */

int HTRequest_setPriority_tcl(ClientData clientData, Tcl_Interp *interp, 
			      int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *priority_str		   = argv[2];
    if (req_keyname && priority_str) {
      HTPriority priority     	   = HTPriority_enum(priority_str);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	BOOL result 		   = HTRequest_setPriority(request, priority);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request priority", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_priority_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTPriority priority 	   = HTRequest_priority(request);
	Tcl_AppendResult(interp, HTPriority_name(priority) , NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request", NULL);
    return TCL_ERROR;
  }
}

/* ANCHOR */

int HTRequest_setAnchor_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname	           = argv[1];
    char *anchor_keyname	   = argv[2];
    if (req_keyname && anchor_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      Tcl_HashEntry *anchor_entry  = Tcl_FindHashEntry(&HTableAnchor, anchor_keyname);
      if (request_entry && anchor_entry) {
	HTRequest *request         = Tcl_GetHashValue(request_entry);
	HTAnchor *anchor       	   = Tcl_GetHashValue(anchor_entry);
	HTRequest_setAnchor(request, anchor);
	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request anchor", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_anchor_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 2) {
    char *anchor_keyname           = malloc(max_keyname);
    char *req_keyname	     	   = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request         = Tcl_GetHashValue(request_entry);
	HTParentAnchor *anchor     = HTRequest_anchor(request);
	anchor_keyname             = Keyname_lookupAnchor((HTAnchor *) anchor);
	Tcl_AppendResult(interp, anchor_keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}

/* NET */

int HTRequest_setNet_tcl(ClientData clientData, Tcl_Interp *interp, 
			      int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *net_keyname		   = argv[2];

    if (req_keyname && net_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_keyname);
      if (request_entry && net_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTNet *net		   = Tcl_GetHashValue(net_entry);
	BOOL result 		   = HTRequest_setNet(request, net);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request HTNet", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_net_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTNet *net 		   = HTRequest_net(request);
	char *net_str		   = Keyname_lookupNet(net);

	Tcl_AppendResult(interp, net_str, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request", NULL);
    return TCL_ERROR;
  }
}

/* NEGOTIATION */

int HTRequest_setNegotiation_tcl(ClientData clientData, Tcl_Interp *interp, 
				 int argc, char **argv) {
  if (argc == 3) {
    int negotiation;
    char *req_keyname		   = argv[1];
    char *neg_str     		   = argv[2];
    if (req_keyname && (Tcl_GetBoolean(interp, neg_str, &negotiation) == TCL_OK)) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTRequest_setNegotiation(request, negotiation);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request negotiation", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_negotiation_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	BOOL negotiation 	   = HTRequest_negotiation(request);
	Tcl_AppendResult(interp, negotiation ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request", NULL);
    return TCL_ERROR;
  }
}

/* FULLURI */

int HTRequest_setFullURI_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 3) {
    int FullURI;
    char *req_keyname		   = argv[1];
    char *FullURI_str    	   = argv[2];
    if (req_keyname && (Tcl_GetBoolean(interp, FullURI_str, &FullURI) == TCL_OK)) { 
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTRequest_setFullURI(request, FullURI);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request FullURI?", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_fullURI_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	BOOL FullURI 		   = HTRequest_fullURI(request);
	Tcl_AppendResult(interp, FullURI ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request", NULL);
    return TCL_ERROR;
  }
}

/* ERROR */

int HTRequest_error_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      char *listname 		   = malloc(max_keyname);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTList *list 	  	   = HTRequest_error(request);
	listname 		   = Keyname_lookupList(list);
	Tcl_AppendResult(interp, listname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_setError_tcl(ClientData clientData, Tcl_Interp *interp, 
				int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname		   = argv[1];
    char *list_keyname		   = argv[2];
    if (req_keyname && list_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      Tcl_HashEntry *list_entry    = Tcl_FindHashEntry(&HTableList, list_keyname);	
      if (request_entry && list_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	HTList *list    	   = Tcl_GetHashValue(list_entry);
	HTRequest_setError(request, list);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request error_list", NULL);
    return TCL_ERROR;
  }
}

int HTRequest_addError_tcl(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {
  if (argc == 8) {
    int ignore, element, length;
    char *req_keyname		   = argv[1];
    char *severity_string	   = argv[2];
    char *ignore_string   	   = argv[3];
    char *par_string		   = argv[5];
    char *where			   = argv[7];
    if (req_keyname && severity_string && par_string && where \
	&& (Tcl_GetBoolean(interp, ignore_string,&ignore) == TCL_OK) \
	&& (Tcl_GetInt(interp, argv[4], &element) == TCL_OK) \
	&& (Tcl_GetInt(interp, argv[6], &length) == TCL_OK)) {

      HTSeverity severity 	   = HTSeverity_enum(severity_string);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	BOOL result 		   = HTRequest_addError(request, severity, ignore, element, NULL, length, where);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request severity ignore_boolean element par length where", NULL);
    return TCL_ERROR;
  }
}

/* SYSTEMERROR */

int HTRequest_addSystemError_tcl(ClientData clientData, Tcl_Interp *interp, 
				 int argc, char **argv) {
  if (argc == 6) {
    int ignore, errornumber;
    char *req_keyname		   = argv[1];
    char *severity_string	   = argv[2];
    char *ignore_string   	   = argv[4];
    char *syscall		   = argv[5];
    if (req_keyname && severity_string && syscall \
	&& (Tcl_GetBoolean(interp, ignore_string, &ignore) == TCL_OK) \
	&& (Tcl_GetInt(interp, argv[3], &errornumber) == TCL_OK)) {

      HTSeverity severity 	   = HTSeverity_enum(severity_string);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	BOOL result 		   = HTRequest_addSystemError(request, severity, errornumber, ignore, syscall);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request severity errornumber ignore_boolean syscall", NULL);
    return TCL_ERROR;
  }
}

/* BYTESREAD */

int HTRequest_bytesRead_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 2) {
    char *result_string;
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	long result 		   = HTRequest_bytesRead(request);
	sprintf(result_string, "%ld", result);
	Tcl_AppendResult(interp, result_string, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request", NULL);
    return TCL_ERROR;
  }
}

/* BYTESWRITTEN */

int HTRequest_bytesWritten_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 2) {
    char *result_string;
    char *req_keyname		   = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	long result 		   = HTRequest_bytesWritten(request);
	sprintf(result_string, "%ld", result);
	Tcl_AppendResult(interp, result_string, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], "request", NULL);
    return TCL_ERROR;
  }
}

/*
Internal Request Objects 

The library may under certain circumstances create its own Request objects. These are all handled
internal and does not show up on the application side at all. 

extern BOOL HTRequest_setInternal (HTRequest * request, BOOL mode);
extern BOOL HTRequest_internal (HTRequest * request);
*/










#if 0
/* ACCESS */

int HTRequest_setAccess_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 3) {
    char *req_keyname	   	   = argv[1];
    char *access		   = argv[2];
    if (req_keyname && access) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request         = Tcl_GetHashValue(request_entry);
	HTRequest_setAccess(request, access);
/*        Only for servers...*/
	return TCL_ERROR;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, argv[0], " request access", NULL);
  return TCL_ERROR;
}


int HTRequest_access_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 2) {
    char *req_keyname	           = argv[1];
    if (req_keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, req_keyname);
      if (request_entry) {
	HTRequest *request 	   = Tcl_GetHashValue(request_entry);
	char *access   		   = HTRequest_access(request);
	Tcl_AppendResult(interp, access, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " request", NULL);
    return TCL_ERROR;
  }
}
#endif

