#include "Libdata.h"
#include "Enumerations.h"


/*HTFormat*/

extern int HTPresentation_add_tcl(ClientData, Tcl_Interp *, int, char **);



extern int HTPresentation_deleteAll_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTConversion_add_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTConversion_deleteAll_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTCoding_add_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTCoding_deleteAll_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTCharset_add_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTCharset_deleteAll_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLanguage_add_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTLanguage_deleteAll_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTFormat_setConversion_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTFormat_conversion_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTFormat_addConversion_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTFormat_setContentCoding_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTFormat_contentCoding_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTFormat_addCoding_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTFormat_setTransferCoding_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTFormat_transferCoding_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTFormat_addTransferCoding_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTFormat_setLanguage_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTFormat_language_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTFormat_setCharset_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTFormat_charset_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTFormat_deleteAll_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTStreamStack_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTStackValue_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTContentCodingStack_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTContentEncodingStack_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTContentDecodingStack_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTTransferCodingStack_tcl(ClientData, Tcl_Interp *, int, char **);

