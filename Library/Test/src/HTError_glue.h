#include "Libdata.h"
#include "Enumerations.h"


extern int HTError_add_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTError_addSystem_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTError_deleteAll_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTError_deleteLast_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTError_doShow_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTError_ignoreLast_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTError_setIgnore_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTError_index_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTError_severity_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTError_hasSeverity_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTError_parameter_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTError_location_tcl(ClientData, Tcl_Interp *, int, char **);


