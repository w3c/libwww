#include "Libdata.h"
#include "Enumerations.h"


extern int HTProtocol_add_tcl(ClientData, Tcl_Interp *, int, char **);


extern int HTProtocol_delete_tcl(ClientData, Tcl_Interp *, int, char **);


extern int HTProtocol_deleteAll_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTProtocol_find_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTProtocol_client_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTProtocol_server_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTProtocol_preemptive_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTProtocol_setTransport_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTProtocol_transport_tcl(ClientData, Tcl_Interp *, int, char **);

