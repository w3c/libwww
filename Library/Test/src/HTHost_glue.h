#include "Libdata.h"
#include "Enumerations.h"

extern int HTHost_new_tcl(ClientData clientData, Tcl_Interp *interp, 
		   int argc, char **argv) ;

extern int HTHost_class_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv);

extern int HTHost_setClass_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv);

extern int HTHost_version_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv);

extern int HTHost_setVersion_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv);

extern int HTHost_setChannel_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv);

extern int HTHost_clearChannel_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv);

extern int HTHost_channel_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv);

extern int HTHost_isPersistent_tcl(ClientData clientData, Tcl_Interp *interp, 
		     int argc, char **argv);

extern int HTHost_persistTimeout_tcl(ClientData clientData, 
				     Tcl_Interp *interp, 
				     int argc, char **argv);

extern int HTHost_setPersistTimeout_tcl(ClientData clientData, 
				       Tcl_Interp *interp, int argc, 
				       char **argv);

extern int HTHost_setPersistExpires_tcl(ClientData clientData, 
				       Tcl_Interp *interp, 
				       int argc, char **argv);

extern int HTHost_persistExpires_tcl(ClientData clientData, 
				       Tcl_Interp *interp, 
				       int argc, char **argv);


/*one more function unfinished */

/*extern int HTHost_catchClose()*/
