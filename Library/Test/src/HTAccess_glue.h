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

extern int HTLoadRelative_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLoadToChunk_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLoadToFile_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLoadToStream_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLoadAnchor_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLoadAnchorToChunk_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLoadAnchorRecursive_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLoadToChunk_tcl(ClientData, Tcl_Interp *, int, char **);

/*extern int HTSearch_tcl(ClientData, Tcl_Interp *, int, char **);
 */

extern int HTLoadRules_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTSearchAbsolute_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTSearchRelative_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTSearchAnchor_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTSearchString_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTGetFormAbsolute_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTGetFormRelative_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTGetFormAnchor_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTPostFormAbsolute_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTPostFormRelative_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTPostFormAnchor_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTHeadAbsolute_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTHeadRelative_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTHeadAnchor_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTDeleteAbsolute_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTDeleteRelative_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTDeleteAnchor_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTPutAbsolute_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTPutRelative_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTPutAnchor_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTPutStructuredAbsolute_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTPutStructuredRelative_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTPutStructuredAnchor_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTPutDocumentAbsolute_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTPutDocumentRelative_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTPutDocumentAnchor_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTPostAbsolute_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTPostRelative_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTPostAnchor_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTOptionsAbsolute_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTOptionsRelative_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTOptionsAnchor_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTCopyAnchor_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTUploadAnchor_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTUpload_callback_tcl(ClientData, Tcl_Interp *, int, char **);


