#include "HTDNS_glue.h"

#define max_keyname 20
#define bad_vars "Invalid variable names or non-existent entries."
#define err_string "wrong # args: should be "


/****************************
 *        HTDNS             *
 ****************************/

Tcl_HashTable HTableDNS;
Tcl_HashTable HTableList;
Tcl_HashTable HTableNet;
Tcl_HashTable HTableHost;


int HTDNS_setTimeout_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 2) {
    if (argv[1]) {
      time_t timeout = atol(argv[2]);
      HTDNS_setTimeout(timeout);
      return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
}


int HTDNS_timeout_tcl(ClientData clientData, Tcl_Interp *interp, 
			 int argc, char **argv) {
  if (argc == 2) {
    if (argv[1]) {
      time_t timeout = atol(argv[2]);
      time_t result= HTDNS_timeout(timeout);
      char *result_string = malloc(sizeof(result));
      sprintf(result_string, "%d", (int) result);
      Tcl_AppendResult(interp, result_string, NULL);
      return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
}

/*not sure how to handel struct hostent */


int HTDNS_add_tcl(ClientData clientData, Tcl_Interp *interp, 
		  int argc, char **argv) {
  if (argc == 5) {
    char *list_key = argv[1];
    char *element_key = argv[2];
    char *host=argv[3];
    char *homes_name = argv[4];
    if (list_key && element_key && host && homes_name) {
      Tcl_HashEntry *list_entry = Tcl_FindHashEntry(&HTableList, list_key);
      Tcl_HashEntry *element_entry = Tcl_FindHashEntry(&HTableHost, 
						       element_key);
      if (list_entry && element_entry) {
	char *keyname;
	char *homes_string;
	int homes;
	HTList * list = Tcl_GetHashValue(list_entry);
	struct hostent *element = (struct hostent *) Tcl_GetHashValue(element_entry);
	HTdns *dns = HTDNS_add(list, element, host, &homes);
	Tcl_HashEntry *entryPtr = CreateNewEntry(&HTableDNS, "DNS", &keyname);
	Tcl_SetHashValue(entryPtr, dns);
	homes_string = malloc(sizeof(char) * max_keyname);
	if(!homes_string) return TCL_ERROR;
	sprintf(homes_string, "%d", homes);
	Tcl_SetVar(interp, homes_name, homes_string, 0);
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
      


int HTDNS_delete_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv) {
  if (argc ==2) {
    const char *host = argv[1];
    if (host) {
      BOOL result = HTDNS_delete(host);
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


int HTDNS_deleteAll_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if (argc == 1) {
    BOOL result = HTDNS_deleteAll();
    Tcl_AppendResult(interp, result ? "YES" : "NO", NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, NULL);
    return TCL_ERROR;
  }
}


int HTDNS_updateWeigths_tcl(ClientData clientData, Tcl_Interp *interp, 
			     int argc, char **argv) {
  if (argc == 4) {
    char *dns_key = argv[1];
    int cur; 
    int success = Tcl_GetInt(interp, argv[2], &cur);
    char *time_string=argv[4];
    if (dns_key && time_string && (success == TCL_OK)) {
      Tcl_HashEntry *dns_entry = Tcl_FindHashEntry(&HTableDNS, dns_key);
      time_t deltatime = atol(time_string);
      if (dns_entry) {
	HTdns *dns = Tcl_GetHashValue(dns_entry);
	BOOL result= HTDNS_updateWeigths(dns, cur, deltatime);
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


int HTGetHostBySock_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if ( argc == 2) {
    int soc;
    int success = Tcl_GetInt(interp, argv[1], &soc);
    if ( success == TCL_OK) {
      char *result = HTGetHostBySock(soc);
      Tcl_AppendResult(interp, result, NULL);
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



int HTGetHostByName_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if (argc == 3) {
    char *net_key = argv[1];
    char *host = argv[2];
    if ( net_key && host ) {
      Tcl_HashEntry *net_entry = Tcl_FindHashEntry(&HTableNet, net_key);
      if (net_entry) {
	struct _HTNet *net = Tcl_GetHashValue(net_entry);
	int result = HTGetHostByName(net, host);
	char *result_string = malloc(sizeof(result));
	sprintf(result_string, "%d", result);
	Tcl_AppendResult(interp, result_string, NULL);
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









