#include "Libdata.h"
#include "Enumerations.h"

extern int HTNetCall_deleteAll_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNetCall_execute_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_setBefore_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_before_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_callBefore_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_setAfter_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_after_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_callAfter_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_setMaxSocket_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_maxSocket_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_increaseSocket_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_decreaseSocket_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_availableSockets_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_increasePersistentSocket_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_decreasePersistentSocket_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_availablePersistentSockets_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_isIdle_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_activeQueue_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_idle_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_isEmpty_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_pendingQueue_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_newClient_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_newServer_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_new_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_dup_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_start_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_delete_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_deleteAll_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_wait_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_priority_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_setPriority_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_persistent_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_setPersistent_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_kill_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_killAll_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_setContext_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_context_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_setSocket_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_socket_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_setRequest_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_request_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_setTransport_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_transport_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_setChannel_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_channel_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_setHost_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_host_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_setDns_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_dns_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTNet_home_tcl(ClientData, Tcl_Interp *, int, char **);

