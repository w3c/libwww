/*started on Aug 2, 1996, xinger*/

#include "HTHost_glue.h"

#define max_keyname 20
#define bad_vars "Invalid variable names or non-existent entries."
#define err_string "wrong # args \n\n"

/********************
 *     HTHOST       *
 *******************/


Tcl_HashTable HTableHost;
Tcl_HashTable HTableChannel;

/*add a host */

int HTHost_new_tcl(ClientData clientData, Tcl_Interp *interp, 
		   int argc, char **argv) {
  if (argc == 2) {
    char *name = argv[1];
    char *keyname;
    if(name) {
      Tcl_HashEntry *host_entry;
      HTHost *host = HTHost_new(name);
   
      host_entry = CreateNewEntry(&HTableHost, name, &keyname);

      Tcl_SetHashValue(host_entry, host);

      Tcl_AppendResult(interp, keyname,  NULL);

      return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " name", NULL);
    return TCL_ERROR;
  }
}

/*Remote Host Class and Version */


int HTHost_class_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {
  if (argc == 2) {
    char * keyname = argv[1];
    if ( keyname ) {
      Tcl_HashEntry *host_entry = Tcl_FindHashEntry(&HTableHost, keyname);
      if(host_entry) {
	HTHost *host = (HTHost *) Tcl_GetHashValue(host_entry);
	char *result = HTHost_class(host);
	Tcl_AppendResult(interp, result, NULL);
	return TCL_OK;
      }
      Tcl_AppendResult(interp, bad_vars, NULL);
      return TCL_ERROR;
    }
  }

  Tcl_AppendResult(interp, err_string, argv[0], " HTHost", NULL);
  return TCL_ERROR;

}


int HTHost_setClass_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {
  if (argc == 3) {
    char *keyname = argv[1];
    char *s_class = argv[2];
    if (keyname && s_class) {
      Tcl_HashEntry *host_entry = Tcl_FindHashEntry(&HTableHost, keyname);
      if(host_entry) {
	HTHost *host = (HTHost *) Tcl_GetHashValue(host_entry);
	HTHost_setClass(host, s_class);
	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
      Tcl_AppendResult(interp, bad_vars, NULL);
      return TCL_ERROR;
    }
  }
  Tcl_AppendResult(interp, err_string, argv[0], " HTHost", NULL);
  return TCL_ERROR;
}




int HTHost_version_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {
  if (argc == 2) {
    char *keyname = argv[1];
    if (keyname) {
      Tcl_HashEntry *host_entry = Tcl_FindHashEntry(&HTableHost, keyname);
      if(host_entry) {
	char *final_result=malloc(max_keyname * sizeof(char));
	HTHost *host = (HTHost *) Tcl_GetHashValue(host_entry);
	int result = HTHost_version(host);
	sprintf(final_result, "%d", result);

	Tcl_AppendResult(interp, final_result, NULL);
	return TCL_OK;
      }
      Tcl_AppendResult(interp, bad_vars, NULL);
      return TCL_ERROR;
    }
  }
  Tcl_AppendResult(interp, err_string, argv[0], " HTHost", NULL);
  return TCL_ERROR;
}


int HTHost_setVersion_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {
  if (argc == 3) {
    char *keyname = argv[1];
    char *string_version = argv[2];
    if (keyname && string_version) {
      int conversion_success;
      Tcl_HashEntry *host_entry = Tcl_FindHashEntry(&HTableHost, keyname);
      int *version = malloc(sizeof(int));
      conversion_success  = Tcl_GetInt(interp, string_version, version);
 
      if(host_entry && (conversion_success == TCL_OK)) {

	HTHost *host = (HTHost *) Tcl_GetHashValue(host_entry);

	HTHost_setVersion(host, *version);

	Tcl_AppendResult(interp, NULL);
	return TCL_OK;
      }
      Tcl_AppendResult(interp, bad_vars, NULL);
      return TCL_ERROR;
    }
  }

  Tcl_AppendResult(interp, err_string, argv[0], " HTHost", NULL);
  return TCL_ERROR;
}


/* Register a Persistent Channel */




int HTHost_setChannel_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {
  if (argc == 3) {
    char *keyname = argv[1];
    char *channel_keyname = argv[2];
    if (keyname && channel_keyname) {
      Tcl_HashEntry *host_entry = Tcl_FindHashEntry(&HTableHost, keyname);
      Tcl_HashEntry *channel_entry = Tcl_FindHashEntry(&HTableChannel, channel_keyname);
      if(host_entry && channel_entry) {
	HTHost *host = (HTHost *) Tcl_GetHashValue(host_entry);
	HTChannel *channel = (HTChannel *) Tcl_GetHashValue(channel_entry);

	BOOL result = HTHost_setChannel(host, channel, HT_TP_SINGLE);

	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);

	return TCL_OK;
      }
      Tcl_AppendResult(interp, bad_vars, NULL);
      return TCL_ERROR;
    }
  }
  Tcl_AppendResult(interp, err_string, argv[0], " HTHost", NULL);
  return TCL_ERROR;

}


int HTHost_clearChannel_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {
  if (argc == 2) {
    char *keyname = argv[1];
    if (keyname) {
      Tcl_HashEntry *host_entry = Tcl_FindHashEntry(&HTableHost, keyname);
      if(host_entry) {
	HTHost *host = (HTHost *) Tcl_GetHashValue(host_entry);

	BOOL result = HTHost_clearChannel(host, HT_OK);

	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);

	return TCL_OK;
      }
      Tcl_AppendResult(interp, bad_vars, NULL);
      return TCL_ERROR;
    }
  }

  Tcl_AppendResult(interp, err_string, argv[0], " HTHost", NULL);
  return TCL_ERROR;
  
}







int HTHost_channel_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {
  if (argc == 2) {
    char *keyname = argv[1];
    if (keyname) {
      Tcl_HashEntry *channel_entry;
      Tcl_HashEntry *host_entry = Tcl_FindHashEntry(&HTableHost, keyname);
      if(host_entry) {
	char *channel_key = malloc(max_keyname * sizeof(char));
	int newPtr;
	HTHost *host = (HTHost *) Tcl_GetHashValue(host_entry);
	HTChannel *channel = HTHost_channel(host);

	channel_entry = Tcl_CreateHashEntry(&HTableChannel, channel_key, &newPtr);
	Tcl_SetHashValue(channel_entry, channel);

	Tcl_AppendResult(interp, channel_key, NULL);

	return TCL_OK;
      }
      Tcl_AppendResult(interp, bad_vars, NULL);
      return TCL_ERROR;
    }
  }
  
  Tcl_AppendResult(interp, err_string, argv[0], " HTHost", NULL);
  return TCL_ERROR;
  
}



/*Is this host Persistent?*/

int HTHost_isPersistent_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {
  if (argc == 2) {
    char *keyname = argv[1];
    if (keyname) {
      Tcl_HashEntry *host_entry = Tcl_FindHashEntry(&HTableHost, keyname);
      if(host_entry) {
	HTHost *host = (HTHost *) Tcl_GetHashValue(host_entry);

	BOOL result = HTHost_isPersistent(host);

	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);

	return TCL_OK;
      }
      Tcl_AppendResult(interp, bad_vars, NULL);
      return TCL_ERROR;
    }
  }

  Tcl_AppendResult(interp, err_string, argv[0], " HTHost", NULL);
  return TCL_ERROR;

}


/* Timing Persistent Channels */



int HTHost_persistTimeout_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 2) {
    char *time_string = argv[1];
    if(time_string) {
      time_t timeout;
      int conversion_success;
      int *time_storage = malloc(sizeof(int));
      conversion_success = Tcl_GetInt(interp, time_string, time_storage);
      if(conversion_success == TCL_OK) {
	time_t time_output;
	char *result;
	timeout = (time_t) *time_storage;
	time_output = HTHost_persistTimeout(timeout);
	result = malloc(sizeof(time_output));
	sprintf(result, "%d", (int) time_output);
	Tcl_AppendResult(interp, result, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }

  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}





int HTHost_setPersistTimeout_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 2) {
    char *time_string = argv[1];
    if(time_string) {
      time_t timeout;
      int conversion_success;
      int *time_storage = malloc(sizeof(int));
      conversion_success = Tcl_GetInt(interp, time_string, time_storage);

      if(conversion_success == TCL_OK) {
	
	timeout = (time_t) *time_storage;
	HTHost_setPersistTimeout(timeout);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
 
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}








int HTHost_setPersistExpires_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 3) {
    char *keyname = argv[1];
    char *time_string = argv[2];

    if(keyname && time_string) {
      time_t expires;
      int conversion_success;
      int *time_storage = malloc(sizeof(int));
      Tcl_HashEntry *host_entry = Tcl_FindHashEntry(&HTableHost, keyname);
      conversion_success = Tcl_GetInt(interp, time_string, time_storage);

      if(host_entry && conversion_success == TCL_OK) {
	
	HTHost *host = (HTHost *) Tcl_GetHashValue(host_entry);

	expires = (time_t) *time_storage;
	HTHost_setPersistExpires(host, expires);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }

  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}







int HTHost_persistExpires_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 2) {
    char *keyname = argv[1];

    if(keyname) {
      Tcl_HashEntry *host_entry = Tcl_FindHashEntry(&HTableHost, keyname);
      if(host_entry) {
	time_t result_time;
	char *result;
	
	HTHost *host = (HTHost *) Tcl_GetHashValue(host_entry);
       

	result_time = HTHost_persistExpires(host);
	result = malloc(sizeof(result_time));
	sprintf(result, "%d", (int) result_time);
	
	Tcl_AppendResult(interp, result, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }

  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}



/* Catching a Close Event */

/*need to understand what socket , sockops means */

/*

extern int HTHost_catchClose (SOCKET soc, HTRequest * request, SockOps ops);

int HTHost_catchClose_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc = 4){
    SOCKET soc 

    */





















