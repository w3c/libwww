#include "HTNet_glue.h"


#define max_keyname 20
#define bad_vars "Invalid variable names or non-existent entries."
#define err_string "wrong # args: should be "


/*HTNet*/

Tcl_HashTable HTableNet;
Tcl_HashTable HTableReq;
Tcl_HashTable HTableList;
Tcl_HashTable HTableHost;
Tcl_HashTable HTableStream;
Tcl_HashTable HTableChannel;
Tcl_HashTable HTableDNS;
Tcl_HashTable HTableTrans;
Tcl_HashTable HTableVoid;

/*Functions not done yet
extern BOOL HTNetCall_add (HTList * list, HTNetCallback *cbf,
                          void * param, int status);
extern BOOL HTNetCall_delete (HTList * list, HTNetCallback *cbf);
*/

int HTNetCall_deleteAll_tcl(ClientData clientData, Tcl_Interp *interp, 
				    int argc, char **argv) {
  if ( argc == 2) {
    char * list_key = argv[1];
    if (list_key) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      if (list_entry) {
	HTList *list = Tcl_GetHashValue(list_entry);
	BOOL result = HTNetCall_deleteAll(list);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_ERROR;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}

int HTNetCall_execute_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if ( argc == 4) {
    char *list_key = argv[1];
    char *request_key = argv[2];
    int status;
    if ( list_key && request_key && 
	 ( Tcl_GetInt(interp, argv[3], &status) == TCL_OK) ) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      Tcl_HashEntry *request_entry =Tcl_FindHashEntry(&HTableReq, request_key);
      if ( list_entry && request_entry) {
	HTList *list = Tcl_GetHashValue(list_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);
	int result = HTNetCall_execute(list, request, status);
	char *storage = malloc(sizeof(char) * max_keyname);
	if ( !storage) {return TCL_ERROR;}
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


/*
extern BOOL HTNetCall_addBefore (HTNetCallback *cbf, void * param, int status);
extern BOOL HTNetCall_deleteBefore (HTNetCallback * cbf);
*/


int HTNet_setBefore_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if (argc == 2) {
    char * list_key = argv[1];
    if ( list_key) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      if ( list_entry) {
	HTList *list = Tcl_GetHashValue(list_entry) ;
	BOOL result = HTNet_setBefore(list);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_ERROR;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_before_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {
  if ( argc == 1) {
    char * keyname;
    HTList *list= HTNet_before();
    Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableList, "List", &keyname);
    Tcl_SetHashValue(entryPtr, list);
    Tcl_AppendResult(interp, keyname, NULL);
    return TCL_OK;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_callBefore_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {
  if (argc == 3) {
    char *request_key = argv[1];
    int status;
    if (request_key && (Tcl_GetInt(interp, argv[2], &status) == TCL_OK) ) {
      Tcl_HashEntry *request_entry =Tcl_FindHashEntry(&HTableReq, request_key);
      if ( request_entry) {
	HTRequest *request = Tcl_GetHashValue(request_entry);
	int result = HTNet_callBefore(request, status);
	char *storage = malloc(sizeof(char) * max_keyname);
	if(!storage) { return TCL_ERROR; }
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



/*
extern BOOL HTNetCall_addAfter  (HTNetCallback *cbf, void * param, int status);
extern BOOL HTNetCall_deleteAfter (HTNetCallback * cbf);

 */


int HTNet_setAfter_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if (argc == 2) {
    char * list_key = argv[1];
    if ( list_key) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      if ( list_entry) {
	HTList *list = Tcl_GetHashValue(list_entry) ;
	BOOL result = HTNet_setAfter(list);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_ERROR;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_after_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {
  if ( argc == 1) {
    char * keyname;
    HTList *list= HTNet_after();
    Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableList, "List", &keyname);
    Tcl_SetHashValue(entryPtr, list);
    Tcl_AppendResult(interp, keyname, NULL);
    return TCL_OK;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_callAfter_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {
  if (argc == 3) {
    char *request_key = argv[1];
    int status;
    if (request_key && (Tcl_GetInt(interp, argv[2], &status) == TCL_OK) ) {
      Tcl_HashEntry *request_entry =Tcl_FindHashEntry(&HTableReq, request_key);
      if ( request_entry) {
	HTRequest *request = Tcl_GetHashValue(request_entry);
	int result = HTNet_callAfter(request, status);
	char *storage = malloc(sizeof(char) * max_keyname);
	if(!storage) { return TCL_ERROR; }
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




int HTNet_setMaxSocket_tcl (ClientData clientData, Tcl_Interp *interp, 
			    int argc, char **argv) {
  if ( argc == 2) {
    int newmax;
    if (Tcl_GetInt(interp, argv[1], &newmax) == TCL_OK) {
      BOOL result = HTNet_setMaxSocket(newmax);
      Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
      return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_maxSocket_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if (argc == 1) {
    int result = HTNet_maxSocket();
    char *result_string = malloc(sizeof(char) *max_keyname);
    if(!result_string) return TCL_ERROR;
    sprintf(result_string, "%d", result);
    Tcl_AppendResult(interp, result_string, NULL);
    return TCL_OK;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}

int HTNet_increaseSocket_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if ( argc == 1) {
    HTNet_increaseSocket();
    return TCL_OK;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_decreaseSocket_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if ( argc == 1) {
    HTNet_decreaseSocket();
    return TCL_OK;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_availableSockets_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if (argc == 1) {
    int result = HTNet_availableSockets();
    char *result_string = malloc(sizeof(char) *max_keyname);
    if(!result_string) return TCL_ERROR;
    sprintf(result_string, "%d", result);
    Tcl_AppendResult(interp, result_string, NULL);
    return TCL_OK;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_increasePersistentSocket_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if ( argc == 1) {
    HTNet_increasePersistentSocket();
    return TCL_OK;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_decreasePersistentSocket_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if ( argc == 1) {
    HTNet_decreasePersistentSocket();
    return TCL_OK;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_availablePersistentSockets_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if (argc == 1) {
    int result = HTNet_availablePersistentSockets();
    char *result_string = malloc(sizeof(char) *max_keyname);
    if(!result_string) return TCL_ERROR;
    sprintf(result_string, "%d", result);
    Tcl_AppendResult(interp, result_string, NULL);
    return TCL_OK;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_isIdle_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {
  if (argc == 1) {
    BOOL result = HTNet_isIdle();
    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
    return TCL_OK;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_activeQueue_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc ==1) {
/* 
    char * keyname;
    HTList *list = HTNet_activeQueue();
    Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableList, "List", &keyname);
    Tcl_SetHashValue(entryPtr, list);
    Tcl_AppendResult(interp, keyname, NULL);
*/
    return TCL_OK;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_idle_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {
  if (argc == 1) {
    /*
    BOOL result = HTNet_idle();
    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
    */
    return TCL_OK;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_isEmpty_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {
  if (argc == 1) {
    BOOL result = HTNet_isEmpty();
    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
    return TCL_OK;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}

int HTNet_pendingQueue_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc ==1) {
    /* HTNet_pendingQueue is not defined in the library
    *******************************
    char * keyname;
    HTList *list = HTNet_pendingQueue();
    Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableList, "List", &keyname);
    Tcl_SetHashValue(entryPtr, list);
    Tcl_AppendResult(interp, keyname, NULL);
    */
    return TCL_OK;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}

int HTNet_newClient_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if ( argc==2) {
    char *request_key = argv[1];
    if ( request_key){
      Tcl_HashEntry *request_entry=Tcl_FindHashEntry(&HTableReq, request_key);
      if (request_entry) {
	HTRequest *request = Tcl_GetHashValue(request_entry);
	BOOL result = HTNet_newClient(request);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}

int HTNet_newServer_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if( argc == 4) {
    char *request_key = argv[1];
    char *net_key = argv[2];
    char *access = argv[3];
    if ( request_key && net_key && access) {
      Tcl_HashEntry *request_entry=Tcl_FindHashEntry(&HTableReq, request_key);
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      if ( request_entry && net_entry) {
	HTRequest *request = Tcl_GetHashValue(request_entry);
	HTNet *net = Tcl_GetHashValue(net_entry);
	BOOL result = HTNet_newServer(request, net, access);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_new_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if (argc == 3) {
    int sockfd;
    char *request_key = argv[2];
    if((Tcl_GetInt(interp, argv[1], &sockfd)==TCL_OK) && request_key) {
      Tcl_HashEntry *request_entry=Tcl_FindHashEntry(&HTableReq, request_key);
      if ( request_entry) {
	char *keyname;
	HTRequest *request = Tcl_GetHashValue(request_entry); 
	HTNet *net = HTNet_new((SOCKET) sockfd, request);
	Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableNet, "Net", &keyname);
	Tcl_SetHashValue(entryPtr, net);
	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}
					   
int HTNet_dup_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 2) {
    char *src_key = argv[1];
    if (src_key) {
      Tcl_HashEntry *src_entry = Tcl_FindHashEntry(&HTableNet, src_key);
      if ( src_entry) {
	char *keyname;
	HTNet *src = Tcl_GetHashValue(src_entry);
	HTNet *net = HTNet_dup(src);
	Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableNet, "Net", &keyname);
	Tcl_SetHashValue(entryPtr, net);
	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}

int HTNet_start_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 2) {
    char *net_key = argv[1];
    if (net_key) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      if ( net_entry) {
	HTNet *net = Tcl_GetHashValue(net_entry);
	BOOL result = HTNet_start(net);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}

int HTNet_delete_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 3) {
    char *net_key = argv[1];
    int status;
    if (net_key && (Tcl_GetInt(interp, argv[2], &status) ==TCL_OK )) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      if ( net_entry) {
	HTNet *me = Tcl_GetHashValue(net_entry);
	BOOL result = HTNet_delete(me, status);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_deleteAll_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 1) {
    BOOL result = HTNet_deleteAll();
    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
    return TCL_OK;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}




int HTNet_wait_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 2) {
    char *net_key = argv[1];
    if (net_key) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      if ( net_entry) {
	/* HTNet_wait not defined
	HTNet *net = Tcl_GetHashValue(net_entry);
	BOOL result = HTNet_wait(net);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	*/
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}



int HTNet_priority_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 2) {
    char *net_key = argv[1];
    if (net_key) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      if ( net_entry) {
	HTNet *net = Tcl_GetHashValue(net_entry);
	HTPriority priority = HTNet_priority(net);
	char *name = HTPriority_name(priority);
	Tcl_AppendResult(interp, name, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_setPriority_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 3) {
    char *net_key = argv[1];
    char *priority_name = argv[2];
    if (net_key && priority_name) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      HTPriority priority = HTPriority_enum(priority_name);
      if ( net_entry) {
	HTNet *net = Tcl_GetHashValue(net_entry);
	BOOL result = HTNet_setPriority(net, priority);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}



int HTNet_persistent_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 2) {
    char *net_key = argv[1];
    if (net_key) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      if ( net_entry) {
	HTNet *net = Tcl_GetHashValue(net_entry);
	BOOL result = HTNet_persistent(net);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_setPersistent_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 4) {
    char *net_key = argv[1];
    int persistent;
    int mode;
    if (net_key && (Tcl_GetBoolean(interp, argv[2], &persistent)==TCL_OK) 
	&& (Tcl_GetInt(interp, argv[3], &mode) == TCL_OK)) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      if ( net_entry) {
	HTNet *net = Tcl_GetHashValue(net_entry);
	BOOL result = HTNet_setPersistent(net, (BOOL) persistent, 
					  (HTTransportMode) mode);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}
    

int HTNet_kill_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 2) {
    char *net_key = argv[1];
    if (net_key) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      if ( net_entry) {
	HTNet *net = Tcl_GetHashValue(net_entry);
	BOOL result = HTNet_kill(net);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_killAll_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 1) {
    BOOL result = HTNet_killAll();
    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
    return TCL_OK;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}



/*functions not done:
extern HTInputStream * HTNet_getInput (HTNet * net, HTStream * target,
                                       void * param, int mode);

extern HTOutputStream * HTNet_getOutput (HTNet * net, void * param, int mode);

*/

int HTNet_setContext_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 3) {
    char *net_key = argv[1];
    char *void_key =argv[2];
    if (net_key && void_key) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      Tcl_HashEntry *void_entry = Tcl_FindHashEntry(&HTableVoid, void_key);
      if ( net_entry && void_entry) {
	HTNet *net = Tcl_GetHashValue(net_entry);
        void *context = Tcl_GetHashValue(void_entry);
	BOOL result = HTNet_setContext(net, context);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}



int HTNet_context_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 2) {
    char *net_key = argv[1];
    if (net_key) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      if ( net_entry) {
	char *keyname;
	HTNet *net = Tcl_GetHashValue(net_entry);
	void *context = HTNet_context(net);
	Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableVoid, 
						 "context", &keyname);
	Tcl_SetHashValue(entryPtr, context);

	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}



int HTNet_setSocket_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 3) {
    char *net_key = argv[1];
    int sockfd;
    if (net_key && (Tcl_GetInt(interp, argv[2], &sockfd) == TCL_OK)) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      if ( net_entry) {
	HTNet *net = Tcl_GetHashValue(net_entry);
	BOOL result = HTNet_setSocket(net, (SOCKET) sockfd);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_socket_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 2) {
    char *net_key = argv[1];
    if (net_key) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      if ( net_entry) {
	HTNet *net = Tcl_GetHashValue(net_entry);
	SOCKET sock = HTNet_socket(net);
	char *storage = malloc(sizeof(char) * max_keyname);
	if (!storage) return TCL_ERROR;
	sprintf(storage, "%d", (int) sock);
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


int HTNet_setRequest_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 3) {
    char *net_key = argv[1];
    char *request_key = argv[2];
    if (net_key && request_key) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      Tcl_HashEntry *request_entry =Tcl_FindHashEntry(&HTableReq, request_key);
      if ( net_entry && request_entry) {
	HTNet *net = Tcl_GetHashValue(net_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);
	BOOL result = HTNet_setRequest(net, request);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}



int HTNet_request_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 2) {
    char *net_key = argv[1];
    if (net_key) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      if ( net_entry) {
	char *keyname;
	HTNet *net = Tcl_GetHashValue(net_entry);
	HTRequest  *request = HTNet_request(net);
	Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableReq, "Request", 
						 &keyname);
	Tcl_SetHashValue(entryPtr, request);
	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}

int HTNet_setTransport_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 3) {
    char *net_key = argv[1];
    char *transport_key = argv[2];
    if (net_key && transport_key) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      Tcl_HashEntry *transport_entry =Tcl_FindHashEntry(&HTableTrans, transport_key);
      if ( net_entry && transport_entry) {
	HTNet *net = Tcl_GetHashValue(net_entry);
	HTTransport *tp = Tcl_GetHashValue(transport_entry);
	BOOL result = HTNet_setTransport(net, tp);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}



int HTNet_transport_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 2) {
    char *net_key = argv[1];
    if (net_key) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      if ( net_entry) {
	char *keyname;
	HTNet *net = Tcl_GetHashValue(net_entry);
	HTTransport  *transport = HTNet_transport(net);
	Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableTrans, "Transport", 
						 &keyname);
	Tcl_SetHashValue(entryPtr, transport);
	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_setChannel_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 3) {
    char *net_key = argv[1];
    char *channel_key = argv[2];
    if (net_key && channel_key) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      Tcl_HashEntry *channel_entry =Tcl_FindHashEntry(&HTableChannel, 
						      channel_key);
      if ( net_entry && channel_entry) {
	HTNet *net = Tcl_GetHashValue(net_entry);
	HTChannel *channel = Tcl_GetHashValue(channel_entry);
	BOOL result = HTNet_setChannel(net, channel);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}




int HTNet_channel_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 2) {
    char *net_key = argv[1];
    if (net_key) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      if ( net_entry) {
	char *keyname;
	HTNet *net = Tcl_GetHashValue(net_entry);
	HTChannel  *channel = HTNet_channel(net);
	Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableChannel, "Channel", 
						 &keyname);
	Tcl_SetHashValue(entryPtr, channel);
	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}



int HTNet_setHost_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 3) {
    char *net_key = argv[1];
    char *host_key = argv[2];
    if (net_key && host_key) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      Tcl_HashEntry *host_entry =Tcl_FindHashEntry(&HTableHost, host_key);
      if ( net_entry && host_entry) {
	HTNet *net = Tcl_GetHashValue(net_entry);
	HTHost *host = Tcl_GetHashValue(host_entry);
	BOOL result = HTNet_setHost(net, host);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}



int HTNet_host_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 2) {
    char *net_key = argv[1];
    if (net_key) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      if ( net_entry) {
	char *keyname;
	HTNet *net = Tcl_GetHashValue(net_entry);
	HTHost  *host = HTNet_host(net);
	Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableHost, "Host", 
						 &keyname);
	Tcl_SetHashValue(entryPtr, host);
	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_setDns_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 3) {
    char *net_key = argv[1];
    char *dns_key = argv[2];
    if (net_key && dns_key) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      Tcl_HashEntry *dns_entry =Tcl_FindHashEntry(&HTableDNS, dns_key);
      if ( net_entry && dns_entry) {
	HTNet *net = Tcl_GetHashValue(net_entry);
	HTdns *dns = Tcl_GetHashValue(dns_entry);
	BOOL result = HTNet_setDns(net, dns);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}



int HTNet_dns_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if ( argc == 2) {
    char *net_key = argv[1];
    if (net_key) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      if ( net_entry) {
	char *keyname;
	HTNet *net = Tcl_GetHashValue(net_entry);
	HTdns  *dns = HTNet_dns(net);
	Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableDNS, "Dns", 
						 &keyname);
	Tcl_SetHashValue(entryPtr, dns);
	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


int HTNet_home_tcl(ClientData clientData, Tcl_Interp *interp, 
		   int argc, char **argv) {
  if ( argc == 2) {
    char *net_key = argv[1];
    if (net_key) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      if ( net_entry) {
	HTNet *net = Tcl_GetHashValue(net_entry);
	int result = HTNet_home(net);
	char *result_string = malloc(sizeof(char) *max_keyname);
	if (!result_string) return TCL_ERROR;
	sprintf(result_string, "%d", result);
	Tcl_AppendResult(interp, result_string, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}
