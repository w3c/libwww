#include "HTUser_glue.h"

#define max_keyname 20
#define bad_vars "Invalid variable names or non-existent entries."
#define err_string "wrong # args: should be "

Tcl_HashTable   HTableUser;
Tcl_HashTable   HTableVoid;

/*****************************
*           HTUser           *
*****************************/

/* NEW */

int HTUserProfile_new_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 3) {
    char *name 	   = argv[1];
    char *void_keyname = argv[2]; 
    char *keyname;
    if (name && void_keyname) {
      Tcl_HashEntry *user_entry;
      Tcl_HashEntry *void_entry = Tcl_FindHashEntry(&HTableVoid, void_keyname);
      if (void_entry) {
	void *void_obj 		  = (void *) Tcl_GetHashValue(void_entry);
	HTUserProfile *user       = HTUserProfile_new(name, void_obj);


	user_entry 	   	  = CreateNewUserEntry("HTUser", &keyname);
	Tcl_SetHashValue(user_entry, user);        

	Tcl_AppendResult(interp, keyname, NULL);
	return TCL_OK;
      }
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }

  Tcl_AppendResult(interp, err_string, argv[0], " name void_object", NULL);
  return TCL_ERROR;

}

/* LOCALIZE */

int HTUserProfile_localize_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv) {
    if (argc == 2) {
        char *keyname 	   = argv[1];
	if (keyname) {
	  Tcl_HashEntry *user_entry = Tcl_FindHashEntry(&HTableUser, keyname); 
	  if (user_entry) {
	    HTUserProfile *user     = (HTUserProfile *) Tcl_GetHashValue(user_entry);
	    BOOL result             = HTUserProfile_localize(user);

	    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	    return TCL_OK;
	  }
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;
    }

    Tcl_AppendResult(interp, err_string, argv[0], " HTUserProfile", NULL);
    return TCL_ERROR;

}

/* DELETE */

int HTUserProfile_delete_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
    if (argc == 2) {
        char *keyname 	   = argv[1];
	if (keyname) {
	  Tcl_HashEntry *user_entry = Tcl_FindHashEntry(&HTableUser, keyname); 
	  if (user_entry) {
	    HTUserProfile *user     = (HTUserProfile *) Tcl_GetHashValue(user_entry);
	    BOOL result             = HTUserProfile_delete(user);

	    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	    return TCL_OK;
	  }
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;
    }

    Tcl_AppendResult(interp, err_string, argv[0], " HTUserProfile", NULL);
    return TCL_ERROR;
    
}

/* FQDN */

int HTUserProfile_fqdn_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
    if (argc == 2) {
        char *keyname 	   = argv[1];
	if (keyname) {
	  Tcl_HashEntry *user_entry = Tcl_FindHashEntry(&HTableUser, keyname); 
	  if (user_entry) {
	    HTUserProfile *user     = (HTUserProfile *) Tcl_GetHashValue(user_entry);
	    char *result            = HTUserProfile_fqdn(user);

	    Tcl_AppendResult(interp, result, NULL);
	    return TCL_OK;
	  }
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;
    }

    Tcl_AppendResult(interp, err_string, argv[0], " HTUserProfile", NULL);
    return TCL_ERROR;
    
}

int HTUserProfile_setFqdn_tcl(ClientData clientData, Tcl_Interp *interp, 
			      int argc, char **argv) {
    if (argc == 3) {
        char *keyname 	   = argv[1];
        char *fqdn 	   = argv[2];
	if (keyname) {
	  Tcl_HashEntry *user_entry = Tcl_FindHashEntry(&HTableUser, keyname); 
	  if (user_entry) {
	    HTUserProfile *user     = (HTUserProfile *) Tcl_GetHashValue(user_entry);
	    BOOL result            = HTUserProfile_setFqdn(user, fqdn);

	    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	    return TCL_OK;
	  }
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;
    }

    Tcl_AppendResult(interp, err_string, argv[0], " HTUserProfile fqdn", NULL);
    return TCL_ERROR;
    
}

/* EMAIL */

int HTUserProfile_email_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
    if (argc == 2) {
        char *keyname 	   = argv[1];
	if (keyname) {
	  Tcl_HashEntry *user_entry = Tcl_FindHashEntry(&HTableUser, keyname); 
	  if (user_entry) {
	    HTUserProfile *user     = (HTUserProfile *) Tcl_GetHashValue(user_entry);
	    char *result            = HTUserProfile_email(user);

	    Tcl_AppendResult(interp, result, NULL);
	    return TCL_OK;
	  }
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;
    }

    Tcl_AppendResult(interp, err_string, argv[0], " HTUserProfile", NULL);
    return TCL_ERROR;

}

int HTUserProfile_setEmail_tcl(ClientData clientData, Tcl_Interp *interp, 
			      int argc, char **argv) {
    if (argc == 3) {
        char *keyname 	   = argv[1];
        char *email 	   = argv[2];
	if (keyname) {
	  Tcl_HashEntry *user_entry = Tcl_FindHashEntry(&HTableUser, keyname); 
	  if (user_entry) {
	    HTUserProfile *user     = (HTUserProfile *) Tcl_GetHashValue(user_entry);
	    BOOL result            = HTUserProfile_setEmail(user, email);

	    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	    return TCL_OK;
	  }
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;
    }

    Tcl_AppendResult(interp, err_string, argv[0], " HTUserProfile email", NULL);
    return TCL_ERROR;

}

/* NEWS */

int HTUserProfile_news_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
    if (argc == 2) {
        char *keyname 	   = argv[1];
	if (keyname) {
	  Tcl_HashEntry *user_entry = Tcl_FindHashEntry(&HTableUser, keyname); 
	  if (user_entry) {
	    HTUserProfile *user     = (HTUserProfile *) Tcl_GetHashValue(user_entry);
	    char *result            = HTUserProfile_news(user);

	    Tcl_AppendResult(interp, result, NULL);
	    return TCL_OK;
	  }
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;
    }
    
    Tcl_AppendResult(interp, err_string, argv[0], " HTUserProfile", NULL);
    return TCL_ERROR;
}

int HTUserProfile_setNews_tcl(ClientData clientData, Tcl_Interp *interp, 
			      int argc, char **argv) {
    if (argc == 3) {
        char *keyname 	   = argv[1];
        char *news 	   = argv[2];
	if (keyname) {
	  Tcl_HashEntry *user_entry = Tcl_FindHashEntry(&HTableUser, keyname); 
	  if (user_entry) {
	    HTUserProfile *user     = (HTUserProfile *) Tcl_GetHashValue(user_entry);
	    BOOL result            = HTUserProfile_setNews(user, news);

	    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	    return TCL_OK;
	  }
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;
    }
    Tcl_AppendResult(interp, err_string, argv[0], " HTUserProfile news", NULL);
    return TCL_ERROR;

}

/* TMP */

int HTUserProfile_tmp_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
    if (argc == 2) {
        char *keyname 	   = argv[1];
	if (keyname) {
	  Tcl_HashEntry *user_entry = Tcl_FindHashEntry(&HTableUser, keyname); 
	  if (user_entry) {
	    HTUserProfile *user     = (HTUserProfile *) Tcl_GetHashValue(user_entry);
	    char *result            = HTUserProfile_tmp(user);

	    Tcl_AppendResult(interp, result, NULL);
	    return TCL_OK;
	  }
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;
    }
    Tcl_AppendResult(interp, err_string, argv[0], " HTUserProfile", NULL);
    return TCL_ERROR;
    
}

int HTUserProfile_setTmp_tcl(ClientData clientData, Tcl_Interp *interp, 
			      int argc, char **argv) {
    if (argc == 3) {
        char *keyname 	   = argv[1];
        char *tmp 	   = argv[2];
	if (keyname) {
	  Tcl_HashEntry *user_entry = Tcl_FindHashEntry(&HTableUser, keyname); 
	  if (user_entry) {
	    HTUserProfile *user     = (HTUserProfile *) Tcl_GetHashValue(user_entry);
	    BOOL result            = HTUserProfile_setTmp(user, tmp);

	    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	    return TCL_OK;
	  }
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;

    }
    Tcl_AppendResult(interp, err_string, argv[0], " HTUserProfile tmp", NULL);
    return TCL_ERROR;
 
}

/* TIMEZONE */

int HTUserProfile_timezone_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
    if (argc == 2) {
        char *keyname 	   = argv[1];
	if (keyname) {
	  Tcl_HashEntry *user_entry = Tcl_FindHashEntry(&HTableUser, keyname); 
	  if (user_entry) {
	    HTUserProfile *user     = (HTUserProfile *) Tcl_GetHashValue(user_entry);
	    time_t timezone        = HTUserProfile_timezone(user);
	    char *result = malloc(sizeof(timezone));
	    sprintf(result, "%d", (int) timezone); 

	    Tcl_AppendResult(interp, result, NULL);
	    return TCL_OK;
	  }
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;
    }
    Tcl_AppendResult(interp, err_string, argv[0], " HTUserProfile", NULL);
    return TCL_ERROR;
    
}

int HTUserProfile_setTimezone_tcl(ClientData clientData, Tcl_Interp *interp, 
			      int argc, char **argv) {
    if (argc == 3) {
        char *keyname 	   = argv[1];
        char *timezone_name 	   = argv[2];
	if (keyname && timezone) {
	  time_t timezone	 	    = atol(timezone_name);
	  Tcl_HashEntry *user_entry = Tcl_FindHashEntry(&HTableUser, keyname); 
	  if (user_entry) {
	    HTUserProfile *user     = (HTUserProfile *) Tcl_GetHashValue(user_entry);
	    BOOL result             = HTUserProfile_setTimezone(user, timezone);

	    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	    return TCL_OK;
	  }
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;
    }
    Tcl_AppendResult(interp, err_string, argv[0], " HTUserProfile timezone", NULL);
    return TCL_ERROR;
}

/* CONTEXT */

int HTUserProfile_context_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
    if (argc == 2) {
        char *keyname 	   = argv[1];
	if (keyname) {
	  Tcl_HashEntry *user_entry = Tcl_FindHashEntry(&HTableUser, keyname); 
	  if (user_entry) {
	    HTUserProfile *user     = (HTUserProfile *) Tcl_GetHashValue(user_entry);
	    time_t *context        = HTUserProfile_context(user);
	    char *result = malloc(sizeof(context));
	    sprintf(result, "%d", (int) context); 

	    Tcl_AppendResult(interp, result, NULL);
	    return TCL_OK;
	  }
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;
    }
    Tcl_AppendResult(interp, err_string, argv[0], " HTUserProfile", NULL);
    return TCL_ERROR;
    
}


int HTUserProfile_setContext_tcl(ClientData clientData, Tcl_Interp *interp, 
			      int argc, char **argv) {
    if (argc == 3) {
        char *keyname 	   = argv[1];
        char *context 	   = argv[2];
	if (keyname && context) {
	  Tcl_HashEntry *user_entry = Tcl_FindHashEntry(&HTableUser, keyname); 
	  Tcl_HashEntry *void_entry = Tcl_FindHashEntry(&HTableVoid, context); 
	  if (user_entry && void_entry) {
	    HTUserProfile *user     = (HTUserProfile *) Tcl_GetHashValue(user_entry);
	    void *void_obj          = (void *) Tcl_GetHashValue(void_entry);
	    BOOL result             = HTUserProfile_setContext(user, void_obj);

	    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
	    return TCL_OK;
	  }
	}
	Tcl_AppendResult(interp, bad_vars, NULL);
	return TCL_ERROR;
	
    }
    Tcl_AppendResult(interp, err_string, argv[0], " HTUserProfile context", NULL);
    return TCL_ERROR;
	
}



