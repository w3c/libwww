#include "Libdata.h"
#include "Enumerations.h"

extern int HTUserProfile_new_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTUserProfile_localize_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTUserProfile_delete_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTUserProfile_fqdn_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTUserProfile_setFqdn_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTUserProfile_email_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTUserProfile_setEmail_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTUserProfile_news_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTUserProfile_setNews_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTUserProfile_tmp_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTUserProfile_setTmp_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTUserProfile_timezone_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTUserProfile_setTimezone_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTUserProfile_context_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTUserProfile_setContext_tcl(ClientData, Tcl_Interp *, int, char **);
