#include "tcl.h"
#include "WWWLib.h"

extern int GetURL_tcl(ClientData, Tcl_Interp *, int, char **);

extern int PutURL_tcl(ClientData, Tcl_Interp *, int, char **);

extern int DeleteURL_tcl(ClientData, Tcl_Interp *, int, char **);

extern int PostURL_tcl(ClientData, Tcl_Interp *, int, char **);

extern int OptionsURL_tcl(ClientData, Tcl_Interp *, int, char **);

extern int TraceURL_tcl(ClientData, Tcl_Interp *, int, char **);
