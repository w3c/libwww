#include "HTProt_glue.h"

#define max_keyname 20
#define bad_vars "Invalid variable names or non-existent entries."
#define err_string "Wrong # of args. \n\n"

/***************************
 *        HTProt           *
 **************************/

Tcl_HashTable HTableReq;
Tcl_HashTable HTableProt;
Tcl_HashTable HTableCallback;



/*add an protocol*/

int HTProtocol_add_tcl(ClientData clientData, Tcl_Interp *interp, 
		   int argc, char **argv) {
  if (argc == 6) {
    const char *name = argv[1];
    const char *transport = argv[2];
    int preemptive;
    int conversion_success = Tcl_GetBoolean(interp, argv[3], &preemptive);
    char *client_keyname = argv[4];
    char *server_keyname = argv[5];
   
    if( name && transport && (conversion_success == TCL_OK) &&
	client_keyname && server_keyname) {
      Tcl_HashEntry *client_entry = Tcl_FindHashEntry(&HTableCallback, 
						      client_keyname);
      Tcl_HashEntry *server_entry = Tcl_FindHashEntry(&HTableCallback,
						      server_keyname);
    
      if(client_entry && server_entry){
	HTEventCallback *client = (HTEventCallback *) Tcl_GetHashValue(client_entry);
	HTEventCallback *server = (HTEventCallback *) Tcl_GetHashValue(server_entry);

	BOOL result = HTProtocol_add(name, transport, (BOOL) preemptive, 
				     client, server);

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



/*delete a protocol*/ 

int HTProtocol_delete_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc ==2) {
    char *name = argv[1];
    if(name) {
      BOOL result = HTProtocol_delete(name);
      Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
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



/*remove all registered protocols*/

int HTProtocol_deleteAll_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if(argc==1) {
    BOOL result = HTProtocol_deleteAll();
    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}



/*find a protocol object*/



int HTProtocol_find_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if (argc == 3) {
    char *request_key = argv[1];
    const char *access = argv[2];
    if (request_key && access) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, 
						       request_key);
      if(request_entry) {
	char *keyname;

	HTRequest *request = Tcl_GetHashValue(request_entry);

	HTProtocol *protocol = HTProtocol_find(request, access);

	Tcl_HashEntry *protocol_entry = CreateNewEntry(&HTableProt,"Protocol", 
						       &keyname);

	Tcl_SetHashValue(protocol_entry, protocol);
	
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


/*get callback functions */



int HTProtocol_client_tcl (ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {
  if (argc == 2) {
    char *protocol_key = argv[1];
    if (protocol_key) {
      Tcl_HashEntry *protocol_entry = Tcl_FindHashEntry(&HTableProt, 
							protocol_key);
      if (protocol_entry) {
        char *client_key;
	HTProtocol *protocol = Tcl_GetHashValue(protocol_entry);
	
	HTEventCallback *client = HTProtocol_client(protocol);
	
	Tcl_HashEntry *client_entry = CreateNewEntry(&HTableCallback, 
						     "client", &client_key);

	Tcl_SetHashValue (client_entry, client);
	
	Tcl_AppendResult(interp, client_key, NULL);
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



int HTProtocol_server_tcl (ClientData clientData, Tcl_Interp *interp, 
			   int argc, char **argv) {
  if (argc == 2) {
    char *protocol_key = argv[1];
    if (protocol_key) {
      Tcl_HashEntry *protocol_entry = Tcl_FindHashEntry(&HTableProt, 
							protocol_key);
      if (protocol_entry) {
        char *server_key;
	HTProtocol *protocol = Tcl_GetHashValue(protocol_entry);
	
	HTEventCallback *server = HTProtocol_server(protocol);
	
	Tcl_HashEntry *server_entry = CreateNewEntry(&HTableCallback, 
						     "server", &server_key);

	Tcl_SetHashValue (server_entry, server);
	
	Tcl_AppendResult(interp, server_key, NULL);
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



/* Is Access Scheme Preemptive */

int HTProtocol_preemptive_tcl(ClientData clientData, Tcl_Interp *interp, 
			      int argc, char **argv) {
  if (argc == 2) {
    char *protocol_key = argv[1];
    if (protocol_key) {
      Tcl_HashEntry *protocol_entry = Tcl_FindHashEntry(&HTableProt, 
							protocol_key);
      if(protocol_entry) {
	HTProtocol *protocol = Tcl_GetHashValue(protocol_entry);
	
	BOOL result = HTProtocol_preemptive(protocol);
	
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

/*Binding to the Transport Class */

int HTProtocol_setTransport_tcl(ClientData clientData, Tcl_Interp *interp,
				int argc, char **argv) {
  if (argc == 3) {
    char *protocol_key = argv[1];
    const char *transport = argv[2];
    if (protocol_key && transport) {
      Tcl_HashEntry *protocol_entry = Tcl_FindHashEntry(&HTableProt, 
							protocol_key);
      if(protocol_entry) {
	HTProtocol *protocol = Tcl_GetHashValue(protocol_entry);
	BOOL result = HTProtocol_setTransport(protocol, transport);
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


int HTProtocol_transport_tcl(ClientData clientData, Tcl_Interp *interp,
			     int argc, char **argv) {
  if (argc == 2) {
    char *protocol_key = argv[1];
    if(protocol_key) {
      Tcl_HashEntry *protocol_entry = Tcl_FindHashEntry(&HTableProt, 
							protocol_key);
      if(protocol_entry) {
	HTProtocol *protocol = Tcl_GetHashValue(protocol_entry);
	const char * result = HTProtocol_transport(protocol);
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

