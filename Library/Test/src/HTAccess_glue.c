#include "HTAccess_glue.h"
#include "WWWApp.h"

#define max_keyname 20
#define bad_vars "Invalid variable names or non-existent entries."
#define err_string "Wrong # of args. \n \n"

/***************************
 *        HTAccess         *
 **************************/

Tcl_HashTable  HTableUser;
Tcl_HashTable  HTableReq;
Tcl_HashTable  HTableAnchor;
Tcl_HashTable  HTableCallback;
Tcl_HashTable  HTableStream;
Tcl_HashTable  HTableChunk;
Tcl_HashTable  HTableAssoc;

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
    int mode;
    int conversion_success = Tcl_GetBoolean(interp, argv[1], &mode);
    if(conversion_success == TCL_OK) {
      HTLib_setSecure( (BOOL) mode);
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
 * Load Request Method *
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
    char *stream_string = argv[2];
    char *keyname = argv[3];
    if(url && stream_string && keyname) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, keyname);
      Tcl_HashEntry *stream_entry = Tcl_FindHashEntry(&HTableStream, stream_string);
      if(stream_entry && request_entry) {
	HTRequest *request = Tcl_GetHashValue(request_entry);
	HTStream *stream = Tcl_GetHashValue(stream_entry);
	BOOL result = HTLoadToStream(url, stream, request);

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


/*Load a Document into Memory*/




int HTLoadToChunk_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 3) {
    const char * url = argv[1];
    char *request_key = argv[2];

    if (url && request_key) {
       Tcl_HashEntry * request_entry = Tcl_FindHashEntry(&HTableReq, 
							request_key);
       if (request_entry) {
	 char * result_key;

	 HTRequest *request = Tcl_GetHashValue(request_entry);
	 
	 HTChunk * result = HTLoadToChunk(url, request);
	 
	 Tcl_HashEntry * result_entry = CreateNewEntry(&HTableChunk, "HTChunk",
						       &result_key);	 
	 Tcl_SetHashValue(result_entry, result);
	 Tcl_AppendResult(interp, result_key, NULL);
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

/* Load a Document and Save as a Local File */



int HTLoadToFile_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {
  if (argc == 4) {
    const char *url = argv[1];
    char *request_key = argv[2];
    const char *filename = argv[3];
    if (url && request_key && filename) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (request_entry) {
	HTRequest *request = Tcl_GetHashValue(request_entry);
	BOOL result = HTLoadToFile(url, request, filename);
	
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
 
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}

/*Load a Document into Memory using an Anchor*/

int HTLoadAnchorToChunk_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if (argc == 3) {
    char *anchor_key = argv[1];
    char *request_key = argv[2];
    if(anchor_key && request_key) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, 
						      anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, 
						       request_key);
      if(anchor_entry && request_entry) {
	char *result_key;

	HTAnchor *anchor = Tcl_GetHashValue(anchor_entry);
	HTRequest *request= Tcl_GetHashValue(request_entry);
	
	HTChunk *result = HTLoadAnchorToChunk(anchor, request);
	
	Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableChunk, "HTChunk", 
						 &result_key);
	Tcl_SetHashValue(entryPtr, result);
	Tcl_AppendResult(interp, result_key, NULL);
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
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}


/* Load a Rule File */




int HTLoadRules_tcl(ClientData clientData, Tcl_Interp *interp,
		    int argc, char **argv) {
  if (argc == 2) {
    const char * url = argv[1];
    if (url) {
      BOOL result = HTLoadRules(url);
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

/*search an anchor*/

/*
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
*/

/*search a document from absolute name */

int HTSearchAbsolute_tcl (ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 3) {
    char *chunk_key = argv[1];
    char *url = argv[2];
    char *request_key = argv[3];
    if(chunk_key && url && request_key) {
      Tcl_HashEntry *chunk_entry = Tcl_FindHashEntry(&HTableChunk, chunk_key);

      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (chunk_entry && request_entry) {
	HTChunk * keywords = Tcl_GetHashValue(chunk_entry);

	HTRequest * request = Tcl_GetHashValue(request_entry);

	BOOL result = HTSearchAbsolute(keywords, url, request);
	
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


int HTSearchRelative_tcl (ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 3) {
    char *chunk_key = argv[1];
    const char *relative = argv[2];
    char * anchor_key = argv[3];
    char * request_key = argv[4];
    if(chunk_key  && relative && anchor_key && request_key) {
      Tcl_HashEntry *chunk_entry = Tcl_FindHashEntry(&HTableChunk, chunk_key);
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, 
						      anchor_key);

      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, 
						       request_key);

      if (chunk_entry && anchor_entry && request_entry) {

	HTChunk * keywords = Tcl_GetHashValue(chunk_entry);
	HTParentAnchor * base =Tcl_GetHashValue(anchor_entry);
	HTRequest * request = Tcl_GetHashValue(request_entry);

	BOOL result = HTSearchRelative(keywords, relative, base, request);
	
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



/* Search a Document using an Anchor*/



int HTSearchAnchor_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if(argc == 4) {
    char *chunk_key = argv[1];
    char *anchor_key = argv[2];
    char *request_key = argv[3];
    
    if(chunk_key && anchor_key && request_key) {
      Tcl_HashEntry *chunk_entry = Tcl_FindHashEntry(&HTableChunk, 
						     chunk_key);
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor,
						      anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, 
						     request_key);
      if(chunk_entry && anchor_entry && request_entry) {
	HTChunk *keywords = Tcl_GetHashValue(chunk_entry);
	HTAnchor *anchor = Tcl_GetHashValue(anchor_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);
	
	BOOL result = HTSearchAnchor(keywords, anchor, request);
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



/*Search a Document using an Anchor Using a String */


int HTSearchString_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if(argc == 4) {
    const char *keywords = argv[1];
    char *anchor_key = argv[2];
    char *request_key = argv[3];
    
    if(keywords && anchor_key && request_key) {

      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor,
						      anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, 
						     request_key);

      if(anchor_entry && request_entry) {

	HTAnchor *anchor = Tcl_GetHashValue(anchor_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);
	
	BOOL result = HTSearchString(keywords, anchor, request);
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




/*Send a Form from Absolute URL using GET*/

			       
int HTGetFormAbsolute_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 4) {
    char * assoc_key = argv[1];
    const char * base = argv[2];
    char * request_key = argv[3];
    
    if (assoc_key && base && request_key) {
      Tcl_HashEntry *assoc_entry = Tcl_FindHashEntry(&HTableAssoc, assoc_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      
      if (assoc_entry && request_entry) {
	HTAssocList *formdata = Tcl_GetHashValue(assoc_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);
	
	BOOL result = HTGetFormAbsolute(formdata, base, request);
	
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


/*Send a Form from Relative URL using GET */

			       
int HTGetFormRelative_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 5) {
    char *assoc_key = argv[1];
    const char * relative = argv[2];
    char *anchor_key = argv[3];
    char *request_key = argv[4];
    
    if (assoc_key && relative && anchor_key && request_key) {
      Tcl_HashEntry *assoc_entry = Tcl_FindHashEntry(&HTableAssoc, assoc_key);
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, 
						     anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      
      if (assoc_entry && anchor_entry && request_entry) {
	HTAssocList *formdata = Tcl_GetHashValue(assoc_entry);
	HTParentAnchor *base = Tcl_GetHashValue(anchor_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);
	
	BOOL result = HTGetFormRelative(formdata, relative, base, request);
	
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

/*Send a Form using an Anchor and the GET Method*/

			       
int HTGetFormAnchor_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 4) {
    char *assoc_key = argv[1];
    char *anchor_key = argv[2];
    char *request_key = argv[3];
    if (assoc_key && anchor_key && request_key) {
      Tcl_HashEntry *assoc_entry = Tcl_FindHashEntry(&HTableAssoc, assoc_key);
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, 
						     anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      
      if (assoc_entry && anchor_entry && request_entry) {
	HTAssocList *formdata = Tcl_GetHashValue(assoc_entry);
	HTAnchor *anchor = Tcl_GetHashValue(anchor_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);
	
	BOOL result = HTGetFormAnchor(formdata, anchor, request);
	
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


/*Handle Forms Using POST Method */

/*send a form from absolte URL using Post*/

			       
int HTPostFormAbsolute_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 4) {
    char *assoc_key = argv[1];
    const char *base = argv[2];
    char *request_key = argv[3];
    
    if (assoc_key && base && request_key) {
      Tcl_HashEntry *assoc_entry = Tcl_FindHashEntry(&HTableAssoc, assoc_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      
      if (assoc_entry && request_entry) {
	char *keyname;
	HTAssocList *formdata = Tcl_GetHashValue(assoc_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);
	
	HTParentAnchor *result = HTPostFormAbsolute(formdata, base, request);
	
	Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableAnchor, "ParentAnchor",
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



/*Send a form from a relative URL using Post */

int HTPostFormRelative_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 5) {
    char *assoc_key = argv[1];
    const char *relative = argv[2];
    char *anchor_key = argv[3];
    char *request_key = argv[4];
    
    if (assoc_key && relative && anchor_key  && request_key) {
      Tcl_HashEntry *assoc_entry = Tcl_FindHashEntry(&HTableAssoc, assoc_key);
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, 
						     anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      
      if (assoc_entry && request_entry) {
	char *keyname;
	HTAssocList *formdata = Tcl_GetHashValue(assoc_entry);
	HTParentAnchor *base = Tcl_GetHashValue(anchor_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);
	
	HTParentAnchor *result = HTPostFormRelative(formdata, relative, base,
						    request);
	
	Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableAnchor, "ParentAnchor",
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


/*send a form using anchor and the Post method*/
int HTPostFormAnchor_tcl(ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 4) {
    char *assoc_key = argv[1];
    char *anchor_key = argv[2];
    char *request_key = argv[3];
    
    if (assoc_key && anchor_key && request_key) {
      Tcl_HashEntry *assoc_entry = Tcl_FindHashEntry(&HTableAssoc, assoc_key);
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, 
						     anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      
      if (assoc_entry && request_entry) {
	char *keyname;
	HTAssocList *formdata = Tcl_GetHashValue(assoc_entry);
	HTAnchor *anchor = Tcl_GetHashValue(anchor_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);
	
	HTParentAnchor *result = HTPostFormAnchor(formdata, anchor, request);
	
	Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableAnchor, "ParentAnchor",
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


/* Get Metainformation about a Document (Method = HEAD) */

/*Get MetainFormation about a Document from Absolute URL */

int HTHeadAbsolute_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 3) {
    const char *url = argv[1];
    char *request_key = argv[2];
    if(url && request_key) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (request_entry){
	HTRequest *request = Tcl_GetHashValue(request_entry);
	BOOL result = HTHeadAbsolute(url, request);
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

/* Get Metainformation about a Document from Relative URL */

int HTHeadRelative_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 4) {
    const char *relative = argv[1];
    char *anchor_key = argv[2];
    char *request_key = argv[3];
    if(relative && anchor_key && request_key) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, 
						     anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (anchor_entry && request_entry){
	HTParentAnchor *base  = Tcl_GetHashValue(anchor_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);

	BOOL result = HTHeadRelative(relative, base, request);
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


int HTHeadAnchor_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 3) {
    char *anchor_key = argv[1];
    char *request_key = argv[2];
    if(anchor_key && request_key) {
      Tcl_HashEntry *anchor_entry =Tcl_FindHashEntry(&HTableAnchor, anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (anchor_entry && request_entry){
	HTAnchor *anchor = Tcl_GetHashValue(anchor_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);
	BOOL result = HTHeadAnchor(anchor, request);
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



/* Delete a Document (Method = DELETE) */


int HTDeleteAbsolute_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 3) {
    const char *url = argv[1];
    char *request_key = argv[2];
    if(url && request_key) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (request_entry){
	HTRequest *request = Tcl_GetHashValue(request_entry);
	BOOL result = HTDeleteAbsolute(url, request);
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



int HTDeleteRelative_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 4) {
    const char *relative = argv[1];
    char *anchor_key = argv[2];
    char *request_key = argv[3];
    if(relative && anchor_key && request_key) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, 
						     anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (anchor_entry && request_entry){
	HTParentAnchor *base  = Tcl_GetHashValue(anchor_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);

	BOOL result = HTDeleteRelative(relative, base, request);
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


int HTDeleteAnchor_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 3) {
    char *anchor_key = argv[1];
    char *request_key = argv[2];
    if(anchor_key && request_key) {
      Tcl_HashEntry *anchor_entry =Tcl_FindHashEntry(&HTableAnchor, anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (anchor_entry && request_entry){
	HTAnchor *anchor = Tcl_GetHashValue(anchor_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);
	BOOL result = HTDeleteAnchor(anchor, request);
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


/* Save a Document ASIS (Method = PUT) */

int HTPutAbsolute_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 4) {
    char *anchor_key = argv[1];
    const char *destination = argv[2];
    char *request_key = argv[3];
    if(anchor_key && destination && request_key) {
      Tcl_HashEntry *anchor_entry =Tcl_FindHashEntry(&HTableAnchor, anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (anchor_entry && request_entry){
	HTParentAnchor *source = Tcl_GetHashValue(anchor_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);
	BOOL result = HTPutAbsolute(source, destination, request);
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


int HTPutRelative_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 5) {
    char *source_key = argv[1];
    const char *relative = argv[2];
    char *destination_key = argv[3];
    char *request_key = argv[4];
    if(source_key && relative && destination_key && request_key) {
      Tcl_HashEntry *source_entry = Tcl_FindHashEntry(&HTableAnchor, 
						     source_key);

      Tcl_HashEntry *destination_entry = Tcl_FindHashEntry(&HTableAnchor, 
						     destination_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (source_entry && destination_entry && request_entry){
	HTParentAnchor *source  = Tcl_GetHashValue(source_entry);
	HTParentAnchor *destination_base = Tcl_GetHashValue(destination_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);

	BOOL result = HTPutRelative(source, relative, destination_base, 
				    request);
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


int HTPutAnchor_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 4) {
    char *source_key = argv[1];
    char *dest_key = argv[2];
    char *request_key = argv[3];
    if(source_key && dest_key && request_key) {
      Tcl_HashEntry *source_entry =Tcl_FindHashEntry(&HTableAnchor, source_key);
      Tcl_HashEntry *dest_entry =Tcl_FindHashEntry(&HTableAnchor, dest_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (source_entry && dest_entry && request_entry){
	HTParentAnchor *source = Tcl_GetHashValue(source_entry);
	HTAnchor *dest = Tcl_GetHashValue(dest_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);

	BOOL result = HTPutAnchor(source, dest, request);
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



/* question? can HTCallBack and HTPostCallback be stored in the 
   same hash table? 
*/
/*save a structured Document (using put) */

int HTPutStructuredAbsolute_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 5) {
    char *anchor_key = argv[1];
    const char *destination = argv[2];
    char *request_key = argv[3];
    char *input_key = argv[4];

    if(anchor_key && destination && request_key && input_key) {
      Tcl_HashEntry *anchor_entry =Tcl_FindHashEntry(&HTableAnchor, anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      Tcl_HashEntry *input_entry = Tcl_FindHashEntry(&HTableCallback, 
						    input_key);
      if (input_entry && anchor_entry && request_entry){
	HTParentAnchor *source = Tcl_GetHashValue(anchor_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);
	HTPostCallback *input = (HTPostCallback *) Tcl_GetHashValue(input_entry);


	BOOL result = HTPutStructuredAbsolute(source, destination, 
					      request, input);
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


int HTPutStructuredRelative_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 6) {
    char *source_key = argv[1];
    const char *relative = argv[2];
    char *destination_key = argv[3];
    char *request_key = argv[4];
    char *input_key = argv[5];
    if(source_key && relative && destination_key && request_key && input_key) {
      Tcl_HashEntry *source_entry = Tcl_FindHashEntry(&HTableAnchor, 
						     source_key);

      Tcl_HashEntry *destination_entry = Tcl_FindHashEntry(&HTableAnchor, 
						     destination_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      Tcl_HashEntry *input_entry = Tcl_FindHashEntry(&HTableCallback, 
						    input_key);

      if (source_entry && destination_entry  && request_entry && input_entry){
	HTParentAnchor *source  = (HTParentAnchor *) Tcl_GetHashValue(source_entry);
	HTParentAnchor *destination_base = (HTParentAnchor *) Tcl_GetHashValue(destination_entry);
	HTRequest *request = (HTRequest *) Tcl_GetHashValue(request_entry);
	HTPostCallback *input = (HTPostCallback *) Tcl_GetHashValue(input_entry);
	
	BOOL result = HTPutStructuredRelative(source, relative, 
					      destination_base, 
					      request, input);
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


int HTPutStructuredAnchor_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 4) {
    char *source_key = argv[1];
    char *dest_key = argv[2];
    char *request_key = argv[3];
    char *input_key = argv[4];
    if(source_key && dest_key && request_key && input_key) {
      Tcl_HashEntry *source_entry =Tcl_FindHashEntry(&HTableAnchor, source_key);
      Tcl_HashEntry *dest_entry =Tcl_FindHashEntry(&HTableAnchor, dest_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      Tcl_HashEntry *input_entry = Tcl_FindHashEntry(&HTableCallback, 
						    input_key);
      if (source_entry && dest_entry && request_entry && input_entry){
	HTParentAnchor *source = Tcl_GetHashValue(source_entry);
	HTAnchor *dest = Tcl_GetHashValue(dest_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);
	HTPostCallback *input = (HTPostCallback *) Tcl_GetHashValue(input_entry);
	
	BOOL result = HTPutStructuredAnchor(source, dest, request, input);

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




/* save a document (Using PUT) */

int HTPutDocumentAbsolute_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 4) {
    char *anchor_key = argv[1];
    const char *destination = argv[2];
    char *request_key = argv[3];
    if(anchor_key && destination && request_key) {
      Tcl_HashEntry *anchor_entry =Tcl_FindHashEntry(&HTableAnchor, anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (anchor_entry && request_entry){
	HTParentAnchor *source = Tcl_GetHashValue(anchor_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);
	BOOL result = HTPutDocumentAbsolute(source, destination, request);
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


int HTPutDocumentRelative_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 5) {
    char *source_key = argv[1];
    const char *relative = argv[2];
    char *destination_key = argv[3];
    char *request_key = argv[4];
    if(source_key && relative && destination_key && request_key) {
      Tcl_HashEntry *source_entry = Tcl_FindHashEntry(&HTableAnchor, 
						     source_key);

      Tcl_HashEntry *destination_entry = Tcl_FindHashEntry(&HTableAnchor, 
						     destination_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (source_entry && request_entry && destination_entry){
	HTParentAnchor *source  = Tcl_GetHashValue(source_entry);
	HTParentAnchor *destination_base = Tcl_GetHashValue(destination_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);

	BOOL result = HTPutDocumentRelative(source, relative, destination_base, 
				    request);
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


int HTPutDocumentAnchor_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 4) {
    char *source_key = argv[1];
    char *dest_key = argv[2];
    char *request_key = argv[3];
    if(source_key && dest_key && request_key) {
      Tcl_HashEntry *source_entry =Tcl_FindHashEntry(&HTableAnchor, source_key);
      Tcl_HashEntry *dest_entry =Tcl_FindHashEntry(&HTableAnchor, dest_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (source_entry && dest_entry && request_entry){
	HTParentAnchor *source = Tcl_GetHashValue(source_entry);
	HTAnchor *dest = Tcl_GetHashValue(dest_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);

	BOOL result = HTPutDocumentAnchor(source, dest, request);
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



/*Save a Document ASIS (METHOD = POST) */


int HTPostAbsolute_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 4) {
    char *anchor_key = argv[1];
    const char *destination = argv[2];
    char *request_key = argv[3];
    if(anchor_key && destination && request_key) {
      Tcl_HashEntry *anchor_entry =Tcl_FindHashEntry(&HTableAnchor, anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (anchor_entry && request_entry){
	HTParentAnchor *source = Tcl_GetHashValue(anchor_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);
	BOOL result = HTPostAbsolute(source, destination, request);
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


int HTPostRelative_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 4) {
    char *source_key = argv[1];
    const char *relative = argv[2];
    char *destination_key = argv[3];
    char *request_key = argv[4];
    if(source_key && relative && destination_key && request_key) {
      Tcl_HashEntry *source_entry = Tcl_FindHashEntry(&HTableAnchor, 
						     source_key);

      Tcl_HashEntry *destination_entry = Tcl_FindHashEntry(&HTableAnchor, 
						     destination_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (source_entry && destination_entry && request_entry){
	HTParentAnchor *source  = Tcl_GetHashValue(source_entry);
	HTParentAnchor *destination_base = Tcl_GetHashValue(destination_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);

	BOOL result = HTPostRelative(source, relative, destination_base, 
				    request);
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


int HTPostAnchor_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 4) {
    char *source_key = argv[1];
    char *dest_key = argv[2];
    char *request_key = argv[3];
    if(source_key && dest_key && request_key) {
      Tcl_HashEntry *source_entry =Tcl_FindHashEntry(&HTableAnchor, source_key);
      Tcl_HashEntry *dest_entry =Tcl_FindHashEntry(&HTableAnchor, dest_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (source_entry && dest_entry && request_entry){
	HTParentAnchor *source = Tcl_GetHashValue(source_entry);
	HTAnchor *dest = Tcl_GetHashValue(dest_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);

	BOOL result = HTPostAnchor(source, dest, request);
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


/*Get Available Options for a document (Method = Options) */


int HTOptionsAbsolute_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 3) {
    const char *url = argv[1];
    char *request_key = argv[2];
    if(url && request_key) {
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (request_entry){
	HTRequest *request = Tcl_GetHashValue(request_entry);

	BOOL result = HTOptionsAbsolute(url, request);

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


int HTOptionsRelative_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 4) {
    const char *relative = argv[1];
    char *anchor_key = argv[2];
    char *request_key = argv[3];
    if(relative && anchor_key && request_key) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, 
						     anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (anchor_entry && request_entry){
	HTParentAnchor *base  = Tcl_GetHashValue(anchor_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);

	BOOL result = HTOptionsRelative(relative, base, request);

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


int HTOptionsAnchor_tcl(ClientData clientData, Tcl_Interp *interp, 
		       int argc, char **argv) {
  if (argc == 3) {
    char *anchor_key = argv[1];
    char *request_key = argv[2];
    if(anchor_key && request_key) {
      Tcl_HashEntry *anchor_entry =Tcl_FindHashEntry(&HTableAnchor, anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, request_key);
      if (anchor_entry && request_entry){
	HTAnchor *anchor = Tcl_GetHashValue(anchor_entry);
	HTRequest *request = Tcl_GetHashValue(request_entry);

	BOOL result = HTOptionsAnchor(anchor, request);
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





/*************************************
 * Save URL to MULTIPle Desinations  *
 *************************************/


/*Copy an anchor */



int HTCopyAnchor_tcl (ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 3) {
    char *anchor_key = argv[1];
    char *request_key = argv[2];
    if(anchor_key && request_key) {
      Tcl_HashEntry *anchor_entry = Tcl_FindHashEntry(&HTableAnchor, 
						      anchor_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, 
						       request_key);
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
  Tcl_AppendResult(interp, err_string, NULL);
  return TCL_ERROR;
}



/*Upload an Anchor*/



int HTUploadAnchor_tcl (ClientData clientData, Tcl_Interp *interp, 
		      int argc, char **argv) {
  if (argc == 4) {
    char *source_key = argv[1];
    char *request_key = argv[2];
    char *callback_key = argv[3];

    if(source_key && request_key && callback_key) {
      Tcl_HashEntry *source_entry = Tcl_FindHashEntry(&HTableAnchor, 
						      source_key);
      Tcl_HashEntry *request_entry = Tcl_FindHashEntry(&HTableReq, 
						       request_key);
      Tcl_HashEntry *callback_entry = Tcl_FindHashEntry(&HTableCallback, 
							callback_key);

      if (source_entry && request_entry && callback_entry) {
       	HTAnchor * source_anchor = Tcl_GetHashValue(source_entry);
	HTRequest * request = Tcl_GetHashValue(request_entry);

	HTPostCallback * callback = (HTPostCallback *) Tcl_GetHashValue(callback_entry);

	BOOL result = HTUploadAnchor(source_anchor, request, callback);
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

	int result = HTUpload_callback(request, target);
	char *storage = malloc(max_keyname * sizeof(char));  
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


