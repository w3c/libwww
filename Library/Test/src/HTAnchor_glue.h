#include "Libdata.h"
#include "Enumerations.h"

/*HTAnchor*/

extern int HTAnchor_findAddress_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_findChild_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_findChildAndLink_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_delete_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_deleteAll_tcl(ClientData, Tcl_Interp *, int, char **);

/*Relate anchors to links*/

extern int HTAnchor_setMainLink_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_mainLink_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_followMainLink_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_setSubLinks_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_subLinks_tcl(ClientData, Tcl_Interp *, int, char **);

/* Relate parents to children */

extern int HTAnchor_parent_tcl(ClientData,Tcl_Interp *, int, char **);

extern int HTAnchor_hasChildren_tcl(ClientData, Tcl_Interp *, int, char **);

/* Addresses */

extern int HTAnchor_address_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_expandedAddress_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_physical_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_setPhysical_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_clearPhysical_tcl(ClientData, Tcl_Interp *, int, char **);

/* Entity Body */

extern int HTAnchor_setDocument_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_document_tcl(ClientData, Tcl_Interp *, int, char **);

/* Entity Header */

extern int HTAnchor_clearHeader_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_cacheHit_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_setCacheHit_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_clearIndex_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_setIndex_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_isIndex_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_title_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_setTitle_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_appendTitle_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_base_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_setBase_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_location_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_setLocation_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_format_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_setFormat_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_formatParam_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_addFormatParam_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_charset_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_setCharset_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_level_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_setLevel_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_language_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_addLanguage_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_encoding_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_addEncoding_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_transfer_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_setTransfer_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_length_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_setLength_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_addLength_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_md5_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_setMd5_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_methods_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_setMethods_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_appendMethods_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_version_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_setVersion_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_date_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_setDate_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_lastModified_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_setLastModified_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_etag_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_setEtag_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_isEtagWeak_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_expires_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_setExpires_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_derived_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_setDerived_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_extra_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTAnchor_addExtra_tcl(ClientData, Tcl_Interp *, int, char **);
