#include "HTAlert_glue.h"

#define max_keyname 20
#define bad_vars "Invalid variable names or non-existent entries."
#define err_string "wrong # args: should be "


/************************
 *        HTAlert       *
 ************************/

Tcl_HashTable HTableList;
Tcl_HashTable HTableAlertCallback;
Tcl_HashTable HTableAlertPar;
Tcl_HashTable HTableVoid;


/*enable or disable messages*/

int HTAlert_setInteractive_tcl(ClientData clientData, Tcl_Interp *interp, 
			   int argc, char ** argv) {
  if(argc == 2) {
    int interactive;
    int success = Tcl_GetInt(interp, argv[1], &interactive);
    if(success == TCL_OK) {
      
      HTAlert_setInteractive( (BOOL) interactive);
      return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


int HTAlert_interactive_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 1) {
    
    BOOL result = HTAlert_interactive();
    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}



int HTAlertCall_add_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if (argc == 4) {
    char *list_key = argv[1];
    char *cbf_key = argv[2];
    char *opcode_name = argv[3];
    if (list_key && cbf_key && opcode_name) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      Tcl_HashEntry *cbf_entry = Tcl_FindHashEntry(&HTableAlertCallback, 
						   cbf_key);
      /*HTAlertOpcode opcode = HTAlertOpcode_enum(opcode_name);*/
      
      if (list_entry && cbf_entry) {
      /*	HTList *list = Tcl_GetHashValue(list_entry);
		HTAlertCallback *cbf = Tcl_GetHashValue(cbf_entry);
	
		BOOL result = HTAlertcall_add(list, cbf, opcode);
		Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);

	*/
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}
    
						    
int HTAlertCall_delete_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if (argc == 3) {
    char *list_key = argv[1];
    char *cbf_key = argv[2];
    if (list_key && cbf_key) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      Tcl_HashEntry *cbf_entry = Tcl_FindHashEntry(&HTableAlertCallback, 
						   cbf_key);
      if (list_entry && cbf_entry) {
	/*HTList *list = Tcl_GetHashValue(list_entry);
	  HTAlertCallback *cbf = Tcl_GetHashValue(cbf_entry);
      	
	  BOOL result = HTAlertcall_delete(list, cbf);
	  Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	*/
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}



int HTAlertCall_deleteAll_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if (argc == 2) {
    char *list_key = argv[1];
    if (list_key ){
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      
      if (list_entry) {
	/*
	  HTList *list = Tcl_GetHashValue(list_entry);
	
	  BOOL result = HTAlertcall_deleteAll(list);
	  Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	*/
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}



int HTAlertCall_find_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 3) {
    char *list_key = argv[1];
    char *opcode_name = argv[2];
    
    if (list_key && opcode_name) {
      char *keyname;
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      HTAlertOpcode opcode = HTAlertOpcode_enum(opcode_name);
      if (list_entry) {
	HTList *list = Tcl_GetHashValue(list_entry);
	HTAlertCallback *result = HTAlertCall_find(list, opcode);
	
	Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableAlertCallback, 
						 "AlertCallback", &keyname);
	Tcl_SetHashValue(entryPtr, result);
	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}
      

int HTAlert_newReply_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 1) {
    char *keyname;
    HTAlertPar *result = HTAlert_newReply();
    Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableAlertPar, "AlertPar", 
					     &keyname);
    Tcl_SetHashValue(entryPtr, result);
    Tcl_AppendResult(interp, keyname, NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}

    

int HTAlert_deleteReply_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc ==2) {
    char *AlertPar_key = argv[1];
    if (AlertPar_key) {
      Tcl_HashEntry *AlertPar_entry = Tcl_FindHashEntry(&HTableAlertPar, 
							AlertPar_key);
      if (AlertPar_entry) {
	HTAlertPar * old = Tcl_GetHashValue(AlertPar_entry);
	HTAlert_deleteReply(old);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


int HTAlert_setReplyMessage_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc == 3) {
    char *AlertPar_key = argv[1];
    const char *message = argv[2];
    if (AlertPar_key && message) {
      Tcl_HashEntry *AlertPar_entry = Tcl_FindHashEntry(&HTableAlertPar, 
							AlertPar_key);
      if(AlertPar_entry) {
	HTAlertPar * me = Tcl_GetHashValue(AlertPar_entry);
	
	BOOL result = HTAlert_setReplyMessage(me, message);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


int HTAlert_assignReplyMessage_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc ==3) {
    char *AlertPar_key = argv[1];
    char *message = argv[2];
    if (AlertPar_key && message) {
      Tcl_HashEntry *AlertPar_entry = Tcl_FindHashEntry(&HTableAlertPar, 
							AlertPar_key);
      if(AlertPar_entry) {
	HTAlertPar * me = Tcl_GetHashValue(AlertPar_entry);
	
	BOOL result = HTAlert_assignReplyMessage(me, message);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


int HTAlert_replyMessage_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc ==2) {
    char *AlertPar_key = argv[1];
    if (AlertPar_key) {
      Tcl_HashEntry *AlertPar_entry = Tcl_FindHashEntry(&HTableAlertPar, 
							AlertPar_key);
      if(AlertPar_entry) {
	HTAlertPar * me = Tcl_GetHashValue(AlertPar_entry);	
	char *result = HTAlert_replyMessage(me);
	Tcl_AppendResult(interp, result, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}

int HTAlert_replySecret_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc ==2) {
    char *AlertPar_key = argv[1];
    if (AlertPar_key) {
      Tcl_HashEntry *AlertPar_entry = Tcl_FindHashEntry(&HTableAlertPar, 
							AlertPar_key);
      if(AlertPar_entry) {
	HTAlertPar * me = Tcl_GetHashValue(AlertPar_entry);
	
	char * result = HTAlert_replySecret(me);
	Tcl_AppendResult(interp, result, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}

int HTAlert_setReplySecret_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc ==3) {
    char *AlertPar_key = argv[1];
    const char *secret = argv[2];
    if (AlertPar_key && secret) {
      Tcl_HashEntry *AlertPar_entry = Tcl_FindHashEntry(&HTableAlertPar, 
							AlertPar_key);
      if(AlertPar_entry) {
	HTAlertPar * me = Tcl_GetHashValue(AlertPar_entry);
	
	BOOL result = HTAlert_setReplySecret(me, secret);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}

int HTAlert_replyOutput_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc ==2) {
    char *AlertPar_key = argv[1];
    if (AlertPar_key) {
      Tcl_HashEntry *AlertPar_entry = Tcl_FindHashEntry(&HTableAlertPar, 
							AlertPar_key);
      if(AlertPar_entry) {
	char *keyname;
	HTAlertPar * me = Tcl_GetHashValue(AlertPar_entry);
	
	void *result = HTAlert_replyOutput(me);
	Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableVoid, "Output", 
						 &keyname);
	Tcl_SetHashValue(entryPtr, result);

	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


int HTAlert_setReplyOutput_tcl(ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if (argc ==3) {
    char *AlertPar_key = argv[1];
    char *void_key = argv[2];
    if (AlertPar_key && void_key) {
      Tcl_HashEntry *AlertPar_entry = Tcl_FindHashEntry(&HTableAlertPar, 
							AlertPar_key);
      Tcl_HashEntry *void_entry = Tcl_FindHashEntry(&HTableVoid, 
						    void_key);
      if(AlertPar_entry && void_entry) {
	HTAlertPar * me = Tcl_GetHashValue(AlertPar_entry);
	void *output = Tcl_GetHashValue(void_entry);
	BOOL result = HTAlert_setReplyOutput(me, output);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}





/*Active set of Callback functions*/

int HTAlert_setGlobal_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 2) {
    char *list_key = argv[1];
    if(list_key) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      if(list_entry) {
	HTList *list = Tcl_GetHashValue(list_entry);
	
	HTAlert_setGlobal(list);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


int HTAlert_global_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 1) {
    char *keyname;
    HTList *list = HTAlert_global();
    Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableList, "List", &keyname);
    Tcl_SetHashValue(entryPtr, list);
    
    Tcl_AppendResult(interp, keyname, NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}



int HTAlert_add_tcl(ClientData clientData, Tcl_Interp *interp, 
		int argc, char **argv) {
  if (argc ==3) {
    char *cbf_key = argv[1];
    char *opcode_name = argv[2];
    if (cbf_key && opcode_name) {
      Tcl_HashEntry *cbf_entry = Tcl_FindHashEntry(&HTableAlertCallback, 
						   cbf_key);
      if (cbf_entry) {

	HTAlertCallback *cbf = (HTAlertCallback *) Tcl_GetHashValue(cbf_entry);
	HTAlertOpcode opcode = HTAlertOpcode_enum(opcode_name);
	BOOL result = HTAlert_add(cbf, opcode);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
  
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}
      



int HTAlert_delete_tcl(ClientData clientData, Tcl_Interp *interp, 
		int argc, char **argv) {
  if (argc ==2) {
    char *cbf_key = argv[1];
    if (cbf_key) {
      Tcl_HashEntry *cbf_entry = Tcl_FindHashEntry(&HTableAlertCallback, 
						   cbf_key);
      if (cbf_entry) {
	HTAlertCallback *cbf = (HTAlertCallback *) Tcl_GetHashValue(cbf_entry);
 
	BOOL result = HTAlert_delete(cbf);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
        return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}
      


int HTAlert_find_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {
  if (argc == 2) {
    char *opcode_name = argv[1];
    if(opcode_name) {
      char *keyname;
      HTAlertOpcode opcode = HTAlertOpcode_enum(opcode_name);
      HTAlertCallback *result = HTAlert_find(opcode);
      Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableAlertCallback, 
					       "AlertCallback", &keyname);
      Tcl_SetHashValue(entryPtr, result);
      Tcl_AppendResult(interp, keyname, NULL);
      return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}
