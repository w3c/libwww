#include "Libdata.h"
#include "Enumerations.h"


extern int HTBind_int_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv);

extern int HTBind_deleteAll_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv);

extern int HTBind_caseSensitive_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv);

extern int HTBind_delimiters_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv);

extern int HTBind_setDelimiters_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv);

extern int HTBind_add_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv);

extern int HTBind_addType_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv);


extern int HTBind_addEncoding_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv);

extern int HTBind_addTransfer_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv);

extern int HTBind_addLanguage_tcl(ClientData clientData, Tcl_Interp *interp, 
			       int argc, char **argv);

extern int HTBind_getSuffix_tcl(ClientData clientData, Tcl_Interp *interp, 
				int argc, char **argv);

extern int HTBind_getBindings_tcl(ClientData clientData, Tcl_Interp *interp, 
				  int argc, char **argv);




extern int HTBind_getFormat_tcl(ClientData, Tcl_Interp *, int, char **);
