#include "HTAccess_glue.h"

#define max_keyname 20
#define bad_vars "Invalid variable names or non-existent entries."
#define err_string "Wrong # of args. \n \n"

/***************************
 *        HTAccess         *
 **************************/

Tcl_HashTable  HTableUser;
Tcl_HashTable  HTableReq;
Tcl_HashTable  HTableAnchor;
Tcl_HashTable  HTablePostCallback;
Tcl_HashTable  HTableStream;


/***************************
 *Generic Library Functions*  
 **************************/


/*Initializing and Terminating the Library */


int HTLibInit_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if (argc == 3) {
    const char *AppName = argv[1];
    const char *AppVersion = argv[2];
    if (AppName && AppVersion) {
      BOOL result = HTLibInit (AppName, AppVersion);
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
 

int HTLibTerminate_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if (argc == 1) {
      BOOL result = HTLibTerminate();
      Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
      return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


/*Library Name and Version*/



int HTLib_name_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if (argc == 1) {
    const char * result = HTLib_name();
    Tcl_AppendResult(interp, result, NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}



int HTLib_version_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if (argc == 1) {
    const char *result = HTLib_version();
    Tcl_AppendResult(interp, result, NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


/*Application Name and Version*/



int HTLib_appName_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if (argc == 1) {
    const char *result = HTLib_appName();
    Tcl_AppendResult(interp, result, NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}

int HTLib_appVersion_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if (argc == 1) {
    const char *result = HTLib_appVersion();
    Tcl_AppendResult(interp, result, NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


/*accesing the local File system*/


int HTLib_secure_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {
  if (argc == 1) {
    BOOL result = HTLib_secure();
    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}

int HTLib_setSecure_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if (argc == 2) {
    BOOL mode;
    int conversion_success = Tcl_GetBoolean(interp, argv[1], &mode);
    if(conversion_success == TCL_OK) {
      HTLib_setSecure(mode);
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


/*default User Profile*/


int HTLib_userProfile_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {

  if(argc == 1) {
    char *keyname;
    Tcl_HashEntry *user_entry;
    HTUserProfile *user = HTLib_userProfile();
    user_entry = CreateNewUserEntry("HTUser", &keyname);
    Tcl_SetHashValue(user_entry, user);
    Tcl_AppendResult(interp, keyname, NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}




int HTLib_setUserProfile_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc==2) {
    char *keyname = argv[1];
    if (keyname) {
      Tcl_HashEntry *user_entry = Tcl_FindHashEntry(&HTableUser, keyname);
      if (user_entry) {
	HTUserProfile *up = (HTUserProfile *) Tcl_GetHashValue(user_entry);
	BOOL result = HTLib_setUserProfile(up);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	return TCL_OK;
      }
      Tcl_AppendResult(interp, "No entry", NULL);
      return TCL_ERROR;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


/**********************
 * Load Requet Method *
 *********************/

/*Request a document from absolute name*/

int HTLoadAbsolute_tcl(ClientData clientData, Tcl_Interp *interp, int argc, 
		       char **argv) {
  if(argc == 3) {
    char *url = argv[1];
    char *keyname = argv[2];
    if(url && keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, keyname);
      if(request_entry) {
	HTRequest *request = Tcl_GetHashValue(request_entry);
	BOOL result = HTLoadAbsolute(url, request);

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

/* Request a document from absolute name to stream */


int HTLoadToStream_tcl(ClientData clientData, Tcl_Interp *interp, int argc, 
		       char **argv) {
  if(argc == 4) {
    char *url = argv[1];
    char *filter_string = argv[2];
    char *keyname = argv[3];
    if(url && filter_string && keyname) {
      BOOL filter;
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, keyname);
      int conversion_success = Tcl_GetBoolean(interp, filter_string, &filter);
      if(request_entry && (conversion_success == TCL_OK)) {
	HTRequest *request = Tcl_GetHashValue(request_entry);
	BOOL result = HTLoadToStream(url, filter, request);

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



/*request a document from relative name */

int HTLoadRelative_tcl(ClientData clientData, Tcl_Interp *interp, 
		    int argc, char **argv) {
  if (argc == 4) {
    char *relative = argv[1];
    char *base_key = argv[2];
    char *request_key = argv[3];
    if(relative && base_key && request_key) {
      Tcl_HashEntry *base_entry = Tcl_FindHashEntry(&HTableAnchor, base_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if(base_entry && request_entry){
	HTParentAnchor *base = Tcl_GetHashValue(base_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);
	
	BOOL result = HTLoadRelative(relative, base, request);
	
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


/*request an anchor*/

int HTLoadAnchor_tcl (ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 3) {
    char *anchor_key = argv[1];
    char *request_key = argv[2];
    if(anchor_key && request_key) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (anchor_entry && request_entry) {
	HTAnchor * anchor = Tcl_GetHashValue(anchor_entry);
	HTRequest * request = Tcl_GetHashValue(request_entry);
	BOOL result = HTLoadAnchor(anchor, request);
	
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
  }
}




int HTLoadAnchorRecursive_tcl (ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 3) {
    char *anchor_key = argv[1];
    char *request_key = argv[2];
    if(anchor_key && request_key) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (anchor_entry && request_entry) {
	HTAnchor * anchor = Tcl_GetHashValue(anchor_entry);
	HTRequest * request = Tcl_GetHashValue(request_entry);
	BOOL result = HTLoadAnchorRecursive(anchor, request);
	
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
  }
}


/*search an anchor*/


int HTSearch_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 4) {
    char *keywords = argv[1];
    char *anchor_key = argv[2];
    char *request_key = argv[3];
    if(anchor_key && request_key) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (anchor_entry && request_entry) {
	HTAnchor * anchor = Tcl_GetHashValue(anchor_entry);
	HTRequest * request = Tcl_GetHashValue(request_entry);
	BOOL result = HTSearch(keywords, anchor, request);
	
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
  }
}


/*search a document from absolute name */

int HTSearchAbsolute_tcl (ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 3) {
    char *keywords = argv[1];
    char *url = argv[2];
    char *request_key = argv[3];
    if(keywords && url && request_key) {
 
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (request_entry) {

	HTRequest * request = Tcl_GetHashValue(request_entry);
	BOOL result = HTSearchAbsolute(keywords, url, request);
	
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
  }
}


/************************
 * POST Request Methods *
 ***********************/


/*Copy an anchor */



int HTCopyAnchor_tcl (ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 2) {
    char *anchor_key = argv[1];
    char *request_key = argv[2];
    if(anchor_key && request_key) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (anchor_entry && request_entry) {
	HTAnchor * src_anchor = Tcl_GetHashValue(anchor_entry);
	HTRequest * main_req = Tcl_GetHashValue(request_entry);
	BOOL result = HTCopyAnchor(src_anchor, main_req);
	
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
  }
}



/*Upload an Anchor*/



int HTUploadAnchor_tcl (ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 4) {
    char *anchor_key = argv[1];
    char *request_key = argv[2];
    char *callback_key = argv[3];

    if(anchor_key && request_key && callback_key) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      Tcl_HashEntry *callback_entry = Tcl_FindHashEntry(&HTablePostCallback, callback_key);

      if (anchor_entry && request_entry && callback_entry) {
	HTAnchor * anchor = Tcl_GetHashValue(anchor_entry);
	HTRequest * request = Tcl_GetHashValue(request_entry);
	HTPostCallback * callback = Tcl_GetHashValue(callback_entry);

	BOOL result = HTUploadAnchor(anchor, request, callback);
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);	
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
  }
}



/*POST Callback Handler*/


int HTUpload_callback_tcl (ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 3) {
    char *request_key = argv[1];
    char *target_key = argv[2];
    if(request_key && target_key) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      Tcl_HashEntry *target_entry = Tcl_FindHashEntry(&HTableStream, target_key);
      if (request_entry && target_entry) {
	HTRequest * request = Tcl_GetHashValue(request_entry);
	HTStream * target = Tcl_GetHashValue(target_entry);
	BOOL result = HTUpload_callback(request, target);
	
	Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
  }
}


