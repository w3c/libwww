#include "Libdata.h"
#include "Enumerations.h"

/*HTAcess*/

extern int HTLibInit_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLibTerminate_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLib_name_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLib_version_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLib_appName_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLib_appVersion_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLib_secure_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLib_setSecure_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLib_userProfile_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLib_setUserProfile_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLoadAbsolute_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLoadToStream_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLoadRelative_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLoadAnchor_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLoadAnchorRecursive_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTSearch_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTSearchAbsolute_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTCopyAnchor_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTUploadAnchor_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTUpload_callback_tcl(ClientData, Tcl_Interp *, int, char **);


