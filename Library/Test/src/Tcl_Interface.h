#include "tcl.h"
#include "WWWLib.h"

extern int getURL_tcl(ClientData, Tcl_Interp *, int, char **);

extern int putURL_tcl(ClientData, Tcl_Interp *, int, char **);

extern int deleteURL_tcl(ClientData, Tcl_Interp *, int, char **);

extern int postURL_tcl(ClientData, Tcl_Interp *, int, char **);

extern int optionsURL_tcl(ClientData, Tcl_Interp *, int, char **);

extern int traceURL_tcl(ClientData, Tcl_Interp *, int, char **);
