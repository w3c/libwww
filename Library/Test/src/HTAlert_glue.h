#include "Libdata.h"
#include "Enumerations.h"


extern int HTAlert_setInteractive_tcl(ClientData, Tcl_Interp *, int, char**);

extern int HTAlert_interactive_tcl(ClientData, Tcl_Interp *, int, char**);

extern int HTAlertCall_add_tcl(ClientData, Tcl_Interp *, int, char**);

extern int HTAlertCall_delete_tcl(ClientData, Tcl_Interp *, int, char**);

extern int HTAlertCall_deleteAll_tcl(ClientData, Tcl_Interp *, int, char**);

extern int HTAlertCall_find_tcl(ClientData, Tcl_Interp *, int, char**);

extern int HTAlert_newReply_tcl(ClientData, Tcl_Interp *, int, char**);

extern int HTAlert_deleteReply_tcl(ClientData, Tcl_Interp *, int, char**);

extern int HTAlert_setReplyMessage_tcl(ClientData, Tcl_Interp *, int, char**);

extern int HTAlert_assignReplyMessage_tcl(ClientData, Tcl_Interp *, int, char**);
extern int HTAlert_replyMessage_tcl(ClientData, Tcl_Interp *, int, char**);

extern int HTAlert_replySecret_tcl(ClientData, Tcl_Interp *, int, char**);

extern int HTAlert_setReplySecret_tcl(ClientData, Tcl_Interp *, int, char**);

extern int HTAlert_replyOutput_tcl(ClientData, Tcl_Interp *, int, char**);

extern int HTAlert_setReplyOutput_tcl(ClientData, Tcl_Interp *, int, char**);

extern int HTAlert_setGlobal_tcl(ClientData, Tcl_Interp *, int, char**);

extern int HTAlert_global_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAlert_add_tcl(ClientData, Tcl_Interp *, int, char**);

extern int HTAlert_delete_tcl(ClientData, Tcl_Interp *, int, char**);

extern int HTAlert_find_tcl(ClientData, Tcl_Interp *, int, char**);

