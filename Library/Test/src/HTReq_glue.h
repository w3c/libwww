#include "Libdata.h"
#include "Enumerations.h"

extern int HTLoad_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_kill_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_new_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_clear_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_dup_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_dupInternal_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_delete_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setUserProfile_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_userProfile_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setMethod_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_method_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setReloadMode_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_reloadMode_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setRedirection_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_redirection_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_addChallenge_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_deleteChallenge_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_challenge_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_addCredentials_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_deleteCredentials_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_credentials_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setRealm_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_realm_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setScheme_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_scheme_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setMaxRetry_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_maxRetry_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_retrys_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_doRetry_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_retryTime_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setConversion_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_conversion_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setEncoding_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_encoding_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setTransfer_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_transfer_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setLanguage_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_language_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setCharset_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_charset_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setGnHd_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_addGnHd_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_gnHd_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setRqHd_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_addRqHd_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_rqHd_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setRsHd_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_addRsHd_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_rsHd_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setEnHd_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_addEnHd_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_enHd_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setParent_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_parent_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setGenerator_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_generator_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setMIMEParseSet_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_MIMEParseSet_tcl(ClientData, Tcl_Interp *, int, char **);



extern int HTRequest_setEntityAnchor_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_entityAnchor_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setInputStream_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_inputStream_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_isPostWeb_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setSource_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_source_tcl(ClientData, Tcl_Interp *, int, char **);




extern int HTRequest_setOutputStream_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_outputStream_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setOutputConnected_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_outputConnected_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setOutputFormat_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_outputFormat_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setDebugStream_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_debugStream_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setDebugFormat_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_debugFormat_tcl(ClientData, Tcl_Interp *, int, char **);




extern int HTRequest_addBefore_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_deleteBefore_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_deleteBeforeAll_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_before_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_addAfter_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_deleteAfter_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_deleteAfterAll_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_after_tcl(ClientData, Tcl_Interp *, int, char **);



extern int HTRequest_setCallback_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_callback_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setContext_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_context_tcl(ClientData, Tcl_Interp *, int, char **);



extern int HTRequest_setPreemptive_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_preemptive_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_priority_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setPriority_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setAnchor_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_anchor_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_net_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setNet_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setNegotiation_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_negotiation_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setFullURI_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_fullURI_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_error_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_setError_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_addError_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_addSystemError_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_bytesRead_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_bytesWritten_tcl(ClientData, Tcl_Interp *, int, char **);





extern int HTRequest_setAccess_tcl(ClientData, Tcl_Interp *, int, char **);

extern int HTRequest_access_tcl(ClientData, Tcl_Interp *, int, char **);
