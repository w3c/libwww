#include "tcl.h"
#include "WWWLib.h"
#include "WWWInit.h"
#include "HText.h"
#include "HTParse_glue.h"
#include "HTAnchor_glue.h"
#include "HTReq_glue.h"
#include "HTLink_glue.h"
#include "HTList_glue.h"
#include "HTFWriter_glue.h"
#include "HTAssoc_glue.h"
#include "HTUser_glue.h"
#include "HTAccess_glue.h"
#include "HTBind_glue.h"
#include "HTHost_glue.h"
#include "URLgen.h"
#include "Tcl_Interface.h"
#include "HTFormat_glue.h"
#include "HTProt_glue.h"
#include "HTLog_glue.h"
#include "HTError_glue.h"
#include "HTAlert_glue.h"
#include "HTDNS_glue.h"
#include "HTUTree_glue.h"
#include "HTTrans_glue.h"
#include "HTNet_glue.h"
#include "HTInet_glue.h"

#define CMDLINE 1

#define APP_NAME     "WWWtest"
#define APP_VERSION  "1.0"
#define bad_vars    "Invalid variable names or non-existent entries."
#define err_string  "Wrong # of args\n"

char 		*HTAppVersion = NULL;
char 		*HTApp_Name = NULL;
HTStyleSheet 	*styleSheet = NULL;

Tcl_HashTable   HTableAnchor;
Tcl_HashTable	HTableLink;
Tcl_HashTable	HTableList;
Tcl_HashTable	HTableReq;
Tcl_HashTable	HTableStream;
Tcl_HashTable   HTableVoid;
Tcl_HashTable   HTableUser;
Tcl_HashTable   HTableAssoc;
Tcl_HashTable   HTableCallback;
Tcl_HashTable   HTableHost;
Tcl_HashTable   HTableChannel;
Tcl_HashTable   HTableChunk;
Tcl_HashTable   HTableProt;
Tcl_HashTable   HTableConverter;
Tcl_HashTable   HTableCoder;
Tcl_HashTable   HTableAlertCallback;
Tcl_HashTable   HTableAlertPar;
Tcl_HashTable   HTableError;
Tcl_HashTable	HTableMIMEParseSet;
Tcl_HashTable	HTableCallback;
Tcl_HashTable	HTableNet;
Tcl_HashTable   HTableDNS;
Tcl_HashTable   HTableTrans;

typedef struct{
    char           *name;
    Tcl_CmdProc    *proc;
    ClientData     data;
    int            flags;
} LibraryFunction;

static int version_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 1) {
    Tcl_AppendResult(interp, APP_NAME , APP_VERSION, NULL);
    return TCL_OK;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], NULL);
    return TCL_ERROR;
  }
}

/*HTEscape*/

static int HTEscape_tcl(ClientData clientData, Tcl_Interp *interp, 
			int argc, char **argv) {
  if (argc == 3) {
    int mask;
    char *str      = argv[1];
    if (str && (Tcl_GetInt(interp, argv[2], &mask) == TCL_OK)) {
      char *result = HTEscape(str, mask);
      Tcl_AppendResult(interp, result, NULL);
      return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " string mask", NULL);
    return TCL_ERROR;
  }
}

static int HTUnEscape_tcl(ClientData clientData, Tcl_Interp *interp, 
			  int argc, char **argv) {
  if (argc == 2) {
    char *str      = argv[1];
    if (str) {
      char *result = HTUnEscape(str);
      Tcl_AppendResult(interp, result, NULL);
      return TCL_OK;
    }
    Tcl_AppendResult(interp, bad_vars, NULL);
    return TCL_ERROR;
  }
  else {
    Tcl_AppendResult(interp, err_string, argv[0], " string", NULL);
    return TCL_ERROR;
  }
}

static LibraryFunction www_commands[] = {

  /* TCL INTERFACE */
  
  { "getURL",			getURL_tcl,			NULL, 0 },
  { "putURL",			putURL_tcl,			NULL, 0 },
  { "deleteURL",		deleteURL_tcl,			NULL, 0 },
  { "postURL",			postURL_tcl,			NULL, 0 },
  { "optionsURL",		optionsURL_tcl,			NULL, 0 },
  { "traceURL",			traceURL_tcl,			NULL, 0 },

  /* RANDOM GENERAL FUNCTIONS */

  { "version",			version_tcl,			NULL, 0 },
  { "random",	        	random_tcl,			NULL, 0 },
  { "generate_char",		generate_char_tcl,		NULL, 0 },
  { "add_item",			add_item_tcl,			NULL, 0 },
  { "generate_login",		generate_login_tcl,		NULL, 0 },
  { "generate_genericurl",	generate_genericurl_tcl,	NULL, 0 },
  { "ftpURL",			ftpURL_tcl,			NULL, 0 },
  { "httpURL",			httpURL_tcl,			NULL, 0 },
  { "gopherURL",		gopherURL_tcl,			NULL, 0 },
  { "telnetURL",		telnetURL_tcl,			NULL, 0 },

  /*HTParse*/

  { "HTParse"   , 		HTParse_tcl,		   	NULL, 0 },
  { "HTSimplify", 		HTSimplify_tcl, 		NULL, 0 },
  { "HTRelative", 		HTRelative_tcl, 		NULL, 0 },
  { "HTCleanTelnetString", 	HTCleanTelnetString_tcl, 	NULL, 0 },

  /*HTEscape*/

  { "HTEscape"  , 		HTEscape_tcl,   		NULL, 0 },
  { "HTUnEscape",    		HTUnEscape_tcl, 		NULL, 0 },

  /*HTAnchor*/

  { "HTAnchor_findAddress",	HTAnchor_findAddress_tcl,	NULL, 0 },
  { "HTAnchor_findChild",	HTAnchor_findChild_tcl,		NULL, 0 },
  { "HTAnchor_findChildAndLink",HTAnchor_findChildAndLink_tcl, 	NULL, 0 },
  { "HTAnchor_delete",		HTAnchor_delete_tcl,		NULL, 0 },
  { "HTAnchor_deleteAll",	HTAnchor_deleteAll_tcl,		NULL, 0 },

  { "HTAnchor_setMainLink",	HTAnchor_setMainLink_tcl,	NULL, 0 },
  { "HTAnchor_mainLink",  	HTAnchor_mainLink_tcl,		NULL, 0 },
  { "HTAnchor_followMainLink",	HTAnchor_followMainLink_tcl,	NULL, 0 },
  { "HTAnchor_setSubLinks",  	HTAnchor_setSubLinks_tcl,      	NULL, 0 },
  { "HTAnchor_subLinks",  	HTAnchor_subLinks_tcl,		NULL, 0 },

  { "HTAnchor_parent",		HTAnchor_parent_tcl,		NULL, 0 },
  { "HTAnchor_hasChildren",	HTAnchor_hasChildren_tcl,	NULL, 0 },

  { "HTAnchor_address",		HTAnchor_address_tcl,		NULL, 0 },
  { "HTAnchor_expandedAddress",	HTAnchor_expandedAddress_tcl,	NULL, 0 },
  { "HTAnchor_physical",	HTAnchor_physical_tcl,		NULL, 0 },
  { "HTAnchor_setPhysical",	HTAnchor_setPhysical_tcl,	NULL, 0 },
  { "HTAnchor_clearPhysical",	HTAnchor_clearPhysical_tcl,	NULL, 0 },

  { "HTAnchor_setDocument",	HTAnchor_setDocument_tcl,	NULL, 0 },
  { "HTAnchor_document",	HTAnchor_document_tcl,		NULL, 0 },

  { "HTAnchor_clearHeader",	HTAnchor_clearHeader_tcl,       NULL, 0 },
#if 0
  { "HTAnchor_cacheHit",	HTAnchor_cacheHit_tcl,		NULL, 0 },
  { "HTAnchor_setCacheHit",	HTAnchor_setCacheHit_tcl,	NULL, 0 },
#endif
  { "HTAnchor_clearIndex",	HTAnchor_clearIndex_tcl,	NULL, 0 },
  { "HTAnchor_setIndex",	HTAnchor_setIndex_tcl,		NULL, 0 },
  { "HTAnchor_isIndex",		HTAnchor_isIndex_tcl,		NULL, 0 },
  { "HTAnchor_title",		HTAnchor_title_tcl,		NULL, 0 },
  { "HTAnchor_setTitle",	HTAnchor_setTitle_tcl,		NULL, 0 },
  { "HTAnchor_appendTitle",	HTAnchor_appendTitle_tcl,      	NULL, 0 },
  { "HTAnchor_base",		HTAnchor_base_tcl,		NULL, 0 },
  { "HTAnchor_setBase",		HTAnchor_setBase_tcl,		NULL, 0 },
  { "HTAnchor_location",	HTAnchor_location_tcl,		NULL, 0 },
  { "HTAnchor_setLocation",     HTAnchor_setLocation_tcl,	NULL, 0 },
  { "HTAnchor_format",		HTAnchor_format_tcl,		NULL, 0 },
  { "HTAnchor_setFormat",	HTAnchor_setFormat_tcl,		NULL, 0 },
  { "HTAnchor_formatParam",	HTAnchor_formatParam_tcl,	NULL, 0 },
  { "HTAnchor_addFormatParam",	HTAnchor_addFormatParam_tcl,	NULL, 0 },
  { "HTAnchor_charset",		HTAnchor_charset_tcl,		NULL, 0 },
  { "HTAnchor_setCharset",	HTAnchor_setCharset_tcl,       	NULL, 0 },
  { "HTAnchor_level",		HTAnchor_level_tcl,		NULL, 0 },
  { "HTAnchor_setLevel",	HTAnchor_setLevel_tcl,       	NULL, 0 },
  { "HTAnchor_language",	HTAnchor_language_tcl,		NULL, 0 },
  { "HTAnchor_addLanguage",	HTAnchor_addLanguage_tcl,       NULL, 0 },
  { "HTAnchor_encoding",	HTAnchor_encoding_tcl,		NULL, 0 },
  { "HTAnchor_addEncoding",	HTAnchor_addEncoding_tcl,     	NULL, 0 },
  { "HTAnchor_transfer",	HTAnchor_transfer_tcl,		NULL, 0 },
  { "HTAnchor_setTransfer",	HTAnchor_setTransfer_tcl,     	NULL, 0 },
  { "HTAnchor_length",       	HTAnchor_length_tcl,		NULL, 0 },
  { "HTAnchor_setLength",	HTAnchor_setLength_tcl,       	NULL, 0 },
  { "HTAnchor_addLength",	HTAnchor_addLength_tcl,       	NULL, 0 },
  { "HTAnchor_md5",       	HTAnchor_md5_tcl,		NULL, 0 },
  { "HTAnchor_setMd5",		HTAnchor_setMd5_tcl,       	NULL, 0 },
  { "HTAnchor_methods",    	HTAnchor_methods_tcl,		NULL, 0 },
  { "HTAnchor_setMethods",	HTAnchor_setMethods_tcl,       	NULL, 0 },
  { "HTAnchor_appendMethods", 	HTAnchor_appendMethods_tcl,   	NULL, 0 },
  { "HTAnchor_version",       	HTAnchor_version_tcl,		NULL, 0 },
  { "HTAnchor_setVersion",	HTAnchor_setVersion_tcl,       	NULL, 0 },
  { "HTAnchor_date",       	HTAnchor_date_tcl,		NULL, 0 },
  { "HTAnchor_setDate",       	HTAnchor_setDate_tcl,		NULL, 0 },
  { "HTAnchor_lastModified",  	HTAnchor_lastModified_tcl,	NULL, 0 },
  { "HTAnchor_setLastModified", HTAnchor_setLastModified_tcl,	NULL, 0 },
  { "HTAnchor_etag",       	HTAnchor_etag_tcl,		NULL, 0 },
  { "HTAnchor_setEtag",    	HTAnchor_setEtag_tcl,		NULL, 0 },
  { "HTAnchor_isEtagWeak",    	HTAnchor_isEtagWeak_tcl,	NULL, 0 },
  { "HTAnchor_expires",       	HTAnchor_expires_tcl,		NULL, 0 },
  { "HTAnchor_setExpires",    	HTAnchor_setExpires_tcl,	NULL, 0 },
  { "HTAnchor_derived",       	HTAnchor_derived_tcl,		NULL, 0 },
  { "HTAnchor_setDerived",	HTAnchor_setDerived_tcl,       	NULL, 0 },
#if 0
  { "HTAnchor_extra",         	HTAnchor_extra_tcl,		NULL, 0 },
  { "HTAnchor_addExtra",	HTAnchor_addExtra_tcl,       	NULL, 0 },
#endif

/*Do we need HTAnchor_headerParsed???*/

  /*HTLink -> Mostly removed from HTAnchor */

  { "HTLink_new",		HTLink_new_tcl,       		NULL, 0 },
  { "HTLink_delete",		HTLink_delete_tcl,       	NULL, 0 },
  { "HTLink_removeAll",  	HTLink_removeAll_tcl,		NULL, 0 },
  { "HTLink_add",		HTLink_add_tcl,			NULL, 0 },
  { "HTLink_remove",		HTLink_remove_tcl,		NULL, 0 },
  { "HTLink_find",		HTLink_find_tcl,		NULL, 0 },
  { "HTLink_setDestination",	HTLink_setDestination_tcl,	NULL, 0 },
  { "HTLink_destination",	HTLink_destination_tcl,      	NULL, 0 },
  { "HTLink_setType",		HTLink_setType_tcl,		NULL, 0 },
  { "HTLink_type",       	HTLink_type_tcl,		NULL, 0 },
  { "HTLink_setMethod",		HTLink_setMethod_tcl,		NULL, 0 },
  { "HTLink_method",		HTLink_method_tcl,		NULL, 0 },
  { "HTLink_setResult",		HTLink_setResult_tcl,		NULL, 0 },
  { "HTLink_result",		HTLink_result_tcl,		NULL, 0 },

  /* HTLink_moveAll is public, just not in the .h file...perhaps next time. */
  /*  { "HTLink_moveAll",		HTLink_moveAll_tcl,		NULL, 0 },*/



/*HTList -> Just to create and utilize empty lists*/

    { "HTList_new",		HTList_new_tcl,	    	        NULL, 0 },
    { "HTList_delete",		HTList_delete_tcl,		NULL, 0 },

/*HTFWriter -> Just to create and utilize streams*/

    { "HTBlackHole",		HTBlackHole_tcl,	    	NULL, 0 },
    { "HTErrorStream",		HTErrorStream_tcl,		NULL, 0 },
    { "HTFWriter_new",		HTFWriter_new_tcl,	    	NULL, 0 },

/*HTReq*/

    { "HTLoad",			HTLoad_tcl, 		      	NULL, 0 },
    { "HTRequest_kill",		HTRequest_kill_tcl, 	      	NULL, 0 },
    { "HTRequest_new",		HTRequest_new_tcl, 	      	NULL, 0 },
    { "HTRequest_clear",	HTRequest_clear_tcl, 	      	NULL, 0 },
    { "HTRequest_dup",		HTRequest_dup_tcl, 	      	NULL, 0 },
    { "HTRequest_dupInternal",	HTRequest_dupInternal_tcl,      NULL, 0 },
    { "HTRequest_delete",	HTRequest_delete_tcl,       	NULL, 0 },
    { "HTRequest_setUserProfile",HTRequest_setUserProfile_tcl,  NULL, 0 },
    { "HTRequest_userProfile",	HTRequest_userProfile_tcl,      NULL, 0 },
    { "HTRequest_setMethod",	HTRequest_setMethod_tcl,       	NULL, 0 },
    { "HTRequest_method",	HTRequest_method_tcl,       	NULL, 0 },
    { "HTRequest_setReloadMode",HTRequest_setReloadMode_tcl,    NULL, 0 },
    { "HTRequest_reloadMode",	HTRequest_reloadMode_tcl,      	NULL, 0 },
    { "HTRequest_setRedirection",HTRequest_setRedirection_tcl,  NULL, 0 },
    { "HTRequest_redirection",	HTRequest_redirection_tcl,     	NULL, 0 },

    { "HTRequest_addChallenge",	HTRequest_addChallenge_tcl,    	NULL, 0 },
    { "HTRequest_deleteChallenge",HTRequest_deleteChallenge_tcl,NULL, 0 },
    { "HTRequest_challenge",	HTRequest_challenge_tcl,       	NULL, 0 },
    { "HTRequest_addCredentials",HTRequest_addCredentials_tcl,  NULL, 0 },
    { "HTRequest_deleteCredentials",HTRequest_deleteCredentials_tcl,NULL, 0 },
    { "HTRequest_credentials",	HTRequest_credentials_tcl,      NULL, 0 },
    { "HTRequest_setRealm",	HTRequest_setRealm_tcl,         NULL, 0 },
    { "HTRequest_realm",	HTRequest_realm_tcl,            NULL, 0 },
    { "HTRequest_setScheme",	HTRequest_setScheme_tcl,        NULL, 0 },
    { "HTRequest_scheme",	HTRequest_scheme_tcl,           NULL, 0 },
    { "HTRequest_setMaxRetry",	HTRequest_setMaxRetry_tcl,     	NULL, 0 },
    { "HTRequest_maxRetry",	HTRequest_maxRetry_tcl,       	NULL, 0 },
    { "HTRequest_retrys",	HTRequest_retrys_tcl,       	NULL, 0 },
    { "HTRequest_doRetry",	HTRequest_doRetry_tcl,       	NULL, 0 },
    { "HTRequest_retryTime",	HTRequest_retryTime_tcl,       	NULL, 0 },
    { "HTRequest_setConversion",HTRequest_setConversion_tcl,    NULL, 0 },
    { "HTRequest_conversion",	HTRequest_conversion_tcl,      	NULL, 0 },
    { "HTRequest_setEncoding",  HTRequest_setEncoding_tcl,      NULL, 0 },
    { "HTRequest_encoding",	HTRequest_encoding_tcl,      	NULL, 0 },
    { "HTRequest_setLanguage",  HTRequest_setLanguage_tcl,      NULL, 0 },
    { "HTRequest_language",	HTRequest_language_tcl,      	NULL, 0 },
    { "HTRequest_setCharset",   HTRequest_setCharset_tcl,       NULL, 0 },
    { "HTRequest_charset",	HTRequest_charset_tcl,      	NULL, 0 },
    { "HTRequest_setGnHd",	HTRequest_setGnHd_tcl,      	NULL, 0 },
    { "HTRequest_addGnHd",	HTRequest_addGnHd_tcl,      	NULL, 0 },
    { "HTRequest_gnHd",		HTRequest_gnHd_tcl,      	NULL, 0 },
    { "HTRequest_setRqHd",	HTRequest_setRqHd_tcl,      	NULL, 0 },
    { "HTRequest_addRqHd",	HTRequest_addRqHd_tcl,      	NULL, 0 },
    { "HTRequest_rqHd",		HTRequest_rqHd_tcl,      	NULL, 0 },
    { "HTRequest_setRsHd",	HTRequest_setRsHd_tcl,      	NULL, 0 },
    { "HTRequest_addRsHd",	HTRequest_addRsHd_tcl,      	NULL, 0 },
    { "HTRequest_rsHd",		HTRequest_rsHd_tcl,      	NULL, 0 },
    { "HTRequest_setEnHd",	HTRequest_setEnHd_tcl,      	NULL, 0 },
    { "HTRequest_addEnHd",	HTRequest_addEnHd_tcl,      	NULL, 0 },
    { "HTRequest_enHd",		HTRequest_enHd_tcl,      	NULL, 0 },
    { "HTRequest_setParent",	HTRequest_setParent_tcl,       	NULL, 0 },
    { "HTRequest_parent",	HTRequest_parent_tcl,       	NULL, 0 },

    { "HTRequest_setGenerator", HTRequest_setGenerator_tcl,     NULL, 0 },
    { "HTRequest_generator",	HTRequest_generator_tcl,      	NULL, 0 },
    { "HTRequest_setMIMEParseSet",HTRequest_setMIMEParseSet_tcl,NULL, 0 },
    { "HTRequest_MIMEParseSet",	HTRequest_MIMEParseSet_tcl,     NULL, 0 },

    { "HTRequest_setEntityAnchor",HTRequest_setEntityAnchor_tcl,NULL, 0 },
    { "HTRequest_entityAnchor", HTRequest_entityAnchor_tcl,     NULL, 0 },
    { "HTRequest_setInputStream",HTRequest_setInputStream_tcl,  NULL, 0 },
    { "HTRequest_inputStream",	HTRequest_inputStream_tcl,      NULL, 0 },
    { "HTRequest_isPostWeb",	HTRequest_isPostWeb_tcl,      	NULL, 0 },
    { "HTRequest_setSource",	HTRequest_setSource_tcl,  	NULL, 0 },
    { "HTRequest_source",	HTRequest_source_tcl,      	NULL, 0 },

    { "HTRequest_setOutputStream",HTRequest_setOutputStream_tcl,NULL, 0 },
    { "HTRequest_outputStream",	HTRequest_outputStream_tcl,     NULL, 0 },
    { "HTRequest_setOutputConnected",HTRequest_setOutputConnected_tcl,NULL, 0 },
    { "HTRequest_outputConnected",HTRequest_outputConnected_tcl,NULL, 0 },
    { "HTRequest_setOutputFormat",HTRequest_setOutputFormat_tcl,NULL, 0 },
    { "HTRequest_outputFormat",	HTRequest_outputFormat_tcl,     NULL, 0 },
    { "HTRequest_setDebugStream",HTRequest_setDebugStream_tcl,  NULL, 0 },
    { "HTRequest_debugStream",	HTRequest_debugStream_tcl,      NULL, 0 },
    { "HTRequest_setDebugFormat",HTRequest_setDebugFormat_tcl,  NULL, 0 },
    { "HTRequest_debugFormat",	HTRequest_debugFormat_tcl,      NULL, 0 },

    { "HTRequest_addBefore",	HTRequest_addBefore_tcl,  	NULL, 0 },
    { "HTRequest_deleteBefore",	HTRequest_deleteBefore_tcl,  	NULL, 0 },
    { "HTRequest_deleteBeforeAll",HTRequest_deleteBeforeAll_tcl,NULL, 0 },
    { "HTRequest_before",	HTRequest_before_tcl,     	NULL, 0 },
    { "HTRequest_addAfter",	HTRequest_addAfter_tcl,  	NULL, 0 },
    { "HTRequest_deleteAfter",	HTRequest_deleteAfter_tcl,  	NULL, 0 },
    { "HTRequest_deleteAfterAll",HTRequest_deleteAfterAll_tcl,  NULL, 0 },
    { "HTRequest_after",	HTRequest_after_tcl,     	NULL, 0 },

    { "HTRequest_callback",     HTRequest_callback_tcl,         NULL, 0 },
    { "HTRequest_setCallback",  HTRequest_setCallback_tcl,      NULL, 0 },
    { "HTRequest_setContext",   HTRequest_setContext_tcl,       NULL, 0 },
    { "HTRequest_context",      HTRequest_context_tcl,          NULL, 0 },

    { "HTRequest_setPreemptive",HTRequest_setPreemptive_tcl,    NULL, 0 },
    { "HTRequest_preemptive",   HTRequest_preemptive_tcl,       NULL, 0 },
    { "HTRequest_setPriority",	HTRequest_setPriority_tcl,      NULL, 0 },
    { "HTRequest_priority",   	HTRequest_priority_tcl,         NULL, 0 },
    { "HTRequest_setAnchor",	HTRequest_setAnchor_tcl,        NULL, 0 },
    { "HTRequest_anchor",   	HTRequest_anchor_tcl,           NULL, 0 },
    { "HTRequest_net",   	HTRequest_net_tcl,              NULL, 0 },
    { "HTRequest_setNet",	HTRequest_setNet_tcl,           NULL, 0 },
    { "HTRequest_setNegotiation",HTRequest_setNegotiation_tcl,  NULL, 0 },
    { "HTRequest_negotiation",	HTRequest_negotiation_tcl,     	NULL, 0 },
    { "HTRequest_setFullURI",   HTRequest_setFullURI_tcl,  	NULL, 0 },
    { "HTRequest_fullURI",	HTRequest_fullURI_tcl,     	NULL, 0 },
    { "HTRequest_setError",	HTRequest_setError_tcl,      	NULL, 0 },
    { "HTRequest_error",	HTRequest_error_tcl,      	NULL, 0 },
    { "HTRequest_addError",	HTRequest_addError_tcl,      	NULL, 0 },
    { "HTRequest_addSystemError",HTRequest_addSystemError_tcl,  NULL, 0 },
    { "HTRequest_bytesRead",	HTRequest_bytesRead_tcl,      	NULL, 0 },
    { "HTRequest_bytesWritten",	HTRequest_bytesWritten_tcl,     NULL, 0 },

#if 0
    { "HTRequest_setAccess",	HTRequest_setAccess_tcl,       	NULL, 0 },
    { "HTRequest_access",	HTRequest_access_tcl,       	NULL, 0 },
#endif

    /* HTAssoc */
    
    { "HTAssocList_new",	HTAssocList_new_tcl,		NULL, 0 },
    { "HTAssocList_delete",	HTAssocList_delete_tcl,		NULL, 0 },

  /* HTUser */

    { "HTUserProfile_new",	HTUserProfile_new_tcl,		NULL, 0 },

  { "HTUserProfile_new",	HTUserProfile_new_tcl,		NULL, 0 },
  { "HTUserProfile_localize",	HTUserProfile_localize_tcl,	NULL, 0 },
  { "HTUserProfile_delete",	HTUserProfile_delete_tcl,	NULL, 0 },
  { "HTUserProfile_fqdn",	HTUserProfile_fqdn_tcl,	        NULL, 0 },
  { "HTUserProfile_setFqdn",	HTUserProfile_setFqdn_tcl,	NULL, 0 },
  { "HTUserProfile_email",	HTUserProfile_email_tcl,	NULL, 0 },
  { "HTUserProfile_setEmail",	HTUserProfile_setEmail_tcl,	NULL, 0 },
  { "HTUserProfile_news",	HTUserProfile_news_tcl,	        NULL, 0 },
  { "HTUserProfile_setNews",	HTUserProfile_setNews_tcl,	NULL, 0 },
  { "HTUserProfile_tmp",	HTUserProfile_tmp_tcl,	        NULL, 0 },
  { "HTUserProfile_setTmp",	HTUserProfile_setTmp_tcl,	NULL, 0 },

  { "HTUserProfile_timezone",	HTUserProfile_timezone_tcl,	NULL, 0 },
  { "HTUserProfile_setTimezone",HTUserProfile_setTimezone_tcl,	NULL, 0 },

  { "HTUserProfile_context",	HTUserProfile_context_tcl,	NULL, 0 },
  { "HTUserProfile_setContext",	HTUserProfile_setContext_tcl,	NULL, 0 },

  /* HTBind */
  { "HTBind_int",               HTBind_int_tcl,                 NULL, 0 },
  { "HTBind_deleteAll",         HTBind_deleteAll_tcl,           NULL, 0 },
  { "HTBind_caseSensitive",     HTBind_caseSensitive_tcl,       NULL, 0 },
  { "HTBind_setDelimiters",     HTBind_setDelimiters_tcl,       NULL, 0 },
  { "HTBind_add",               HTBind_add_tcl,                 NULL, 0 },
  { "HTBind_addType",           HTBind_addType_tcl,             NULL, 0 },
  { "HTBind_addEncoding",       HTBind_addEncoding_tcl,         NULL, 0 },
  { "HTBind_addTransfer",       HTBind_addTransfer_tcl,         NULL, 0 },
  { "HTBind_addLanguage",       HTBind_addLanguage_tcl,         NULL, 0 },
  { "HTBind_getSuffix",         HTBind_getSuffix_tcl,           NULL, 0 },
  { "HTBind_getBindings",       HTBind_getBindings_tcl,         NULL, 0 },
  { "HTBind_getFormat",         HTBind_getFormat_tcl,           NULL, 0 },

  /*HTHost*/
  {"HTHost_new",                HTHost_new_tcl,                 NULL, 0 },
  {"HTHost_class",              HTHost_class_tcl,               NULL, 0 },
  {"HTHost_setClass",           HTHost_setClass_tcl,            NULL, 0 },
  {"HTHost_version",            HTHost_version_tcl,             NULL, 0 },
  {"HTHost_setVersion",         HTHost_setVersion_tcl,          NULL, 0 },
  {"HTHost_setChannel",         HTHost_setChannel_tcl,          NULL, 0 },
  {"HTHost_clearChannel",       HTHost_clearChannel_tcl,        NULL, 0 }, 
  {"HTHost_channel",            HTHost_channel_tcl,             NULL, 0 }, 
  {"HTHost_isPersistent",       HTHost_isPersistent_tcl,        NULL, 0 },
  {"HTHost_persistTimeout",      HTHost_persistTimeout_tcl,       NULL, 0 },
  {"HTHost_setPersistTimeout",   HTHost_setPersistTimeout_tcl,    NULL, 0 },
  {"HTHost_setPersistExpires",   HTHost_setPersistExpires_tcl,    NULL, 0 },
  {"HTHost_persistExpires",      HTHost_persistExpires_tcl,       NULL, 0 },

  /* HTAccess */
  { "HTLibInit",              HTLibInit_tcl,                  NULL, 0 },
  { "HTLibTerminate",         HTLibTerminate_tcl,             NULL, 0 },
  { "HTLib_name",             HTLib_name_tcl,                 NULL, 0 }, 
  { "HTLib_version",          HTLib_version_tcl,              NULL, 0 },
  { "HTLib_appVersion",       HTLib_appVersion_tcl,           NULL, 0 },
  { "HTLib_secure",           HTLib_secure_tcl,               NULL, 0 }, 
  { "HTLib_setSecure",        HTLib_setSecure_tcl,            NULL, 0 },
  { "HTLib_userProfile",      HTLib_userProfile_tcl,          NULL, 0 },
  { "HTLib_setUserProfile",   HTLib_setUserProfile_tcl,       NULL, 0 },
  { "HTLoadAbsolute",         HTLoadAbsolute_tcl,             NULL, 0 },
  { "HTLoadRelative",         HTLoadRelative_tcl,             NULL, 0 },
  { "HTLoadToChunk",          HTLoadToChunk_tcl,              NULL, 0 },
  { "HTLoadToFile",           HTLoadToFile_tcl,               NULL, 0 },
  { "HTLoadToStream",         HTLoadToStream_tcl,             NULL, 0 },
  { "HTLoadAnchor",           HTLoadAnchor_tcl,               NULL, 0 },
  { "HTLoadAnchorToChunk",    HTLoadAnchorToChunk_tcl,        NULL, 0 },
  { "HTLoadAnchorRecursive",  HTLoadAnchorRecursive_tcl,      NULL, 0 },
  { "HTLoadRules",            HTLoadRules_tcl,                NULL, 0 },
  { "HTSearchAbsolute",       HTSearchAbsolute_tcl,           NULL, 0 }, 
  { "HTSearchRelative",       HTSearchRelative_tcl,           NULL, 0 },
  { "HTSearchAnchor",         HTSearchAnchor_tcl,             NULL, 0 },
  { "HTSearchString",         HTSearchString_tcl,             NULL, 0 },

  { "HTGetFormAbsolute",      HTGetFormAbsolute_tcl,          NULL, 0 },
  { "HTGetFormRelative",      HTGetFormRelative_tcl,          NULL, 0 },
  { "HTGetFormAnchor",        HTGetFormAnchor_tcl,            NULL, 0 },

  { "HTPostFormAbsolute",     HTPostFormAbsolute_tcl,         NULL, 0 },
  { "HTPostFormRelative",     HTPostFormRelative_tcl,         NULL, 0 },
  { "HTPostFormAnchor",       HTPostFormAnchor_tcl,           NULL, 0 },

  { "HTHeadAbsolute",         HTHeadAbsolute_tcl,             NULL, 0 },
  { "HTHeadRelative",         HTHeadRelative_tcl,             NULL, 0 },
  { "HTHeadAnchor",           HTHeadAnchor_tcl,               NULL, 0 },

  { "HTDeleteAbsolute",       HTDeleteAbsolute_tcl,           NULL, 0 },
  { "HTDeleteRelative",       HTDeleteRelative_tcl,           NULL, 0 },
  { "HTDeleteAnchor",         HTDeleteAnchor_tcl,             NULL, 0 },

  { "HTPutAbsolute",          HTPutAbsolute_tcl,              NULL, 0 },
  { "HTPutRelative",          HTPutRelative_tcl,              NULL, 0 },
  { "HTPutAnchor",            HTPutAnchor_tcl,                NULL, 0 },

  { "HTPutStructuredAbsolute",HTPutStructuredAbsolute_tcl,    NULL, 0 },
  { "HTPutStructuredRelative",HTPutStructuredRelative_tcl,    NULL, 0 },
  { "HTPutStructuredAnchor",  HTPutStructuredAnchor_tcl,      NULL, 0 },

  { "HTPutDocumentAbsolute",  HTPutDocumentAbsolute_tcl,      NULL, 0 },
  { "HTPutDocumentRelative",  HTPutDocumentRelative_tcl,      NULL, 0 },
  { "HTPutDocumentAnchor",    HTPutDocumentAnchor_tcl,        NULL, 0 },

  { "HTPostAbsolute",         HTPostAbsolute_tcl,             NULL, 0 },
  { "HTPostRelative",         HTPostRelative_tcl,             NULL, 0 },
  { "HTPostAnchor",           HTPostAnchor_tcl,               NULL, 0 },

  { "HTOptionsAbsolute",      HTOptionsAbsolute_tcl,          NULL, 0 },
  { "HTOptionsRelative",      HTOptionsRelative_tcl,          NULL, 0 },
  { "HTOptionsAnchor",        HTOptionsAnchor_tcl,            NULL, 0 },

 /*  { "HTSearch",               HTSearch_tcl,                  NULL, 0 }, */

  { "HTCopyAnchor",           HTCopyAnchor_tcl,               NULL, 0 },
  { "HTUploadAnchor",         HTUploadAnchor_tcl,             NULL, 0 },
  { "HTUpload_callback",      HTUpload_callback_tcl,          NULL, 0 },

/*HTProt */

  { "HTProtocol_add",         HTProtocol_add_tcl,             NULL, 0 },
  { "HTProtocol_delete",      HTProtocol_delete_tcl,          NULL, 0 },
  { "HTProtocol_deleteAll",   HTProtocol_deleteAll_tcl,       NULL, 0 },
  { "HTProtocol_find",        HTProtocol_find_tcl,            NULL, 0 },
  { "HTProtocol_client",      HTProtocol_client_tcl,          NULL, 0 },
  { "HTProtocol_server",      HTProtocol_server_tcl,          NULL, 0 },
  { "HTProtocol_preemptive",  HTProtocol_preemptive_tcl,      NULL, 0 },
  { "HTProtocol_setTransport",HTProtocol_setTransport_tcl,    NULL, 0 },
  { "HTProtocol_transport",   HTProtocol_transport_tcl,       NULL, 0 },
  /*HTFormat*/
  { "HTPresentation_add",     HTPresentation_add_tcl,         NULL, 0 },
  { "HTPresentation_deleteAll",HTPresentation_deleteAll_tcl,  NULL, 0 },
  { "HTConversion_add",       HTConversion_add_tcl,           NULL, 0 },
  { "HTConversion_deleteAll", HTConversion_deleteAll_tcl,     NULL, 0 },
  { "HTCoding_add",           HTCoding_add_tcl,               NULL, 0 },
  { "HTCoding_deleteAll",     HTCoding_deleteAll_tcl,         NULL, 0 },
  { "HTCharset_add",          HTCharset_add_tcl,              NULL, 0 },
  { "HTCharset_deleteAll",    HTCharset_deleteAll_tcl,        NULL, 0 },
  { "HTLanguage_add",         HTLanguage_add_tcl,             NULL, 0 },
  { "HTLanguage_deleteAll",   HTLanguage_deleteAll_tcl,       NULL, 0 },
  { "HTFormat_setConversion", HTFormat_setConversion_tcl,     NULL, 0 },
  { "HTFormat_conversion",    HTFormat_conversion_tcl,        NULL, 0 },
  { "HTFormat_addConversion", HTFormat_addConversion_tcl,     NULL, 0 },
  { "HTFormat_setContentCoding",HTFormat_setContentCoding_tcl,NULL, 0 },
  { "HTFormat_contentCoding", HTFormat_contentCoding_tcl,     NULL, 0 },
  { "HTFormat_addCoding",     HTFormat_addCoding_tcl,         NULL, 0 },
  { "HTFormat_setTransferCoding",HTFormat_setTransferCoding_tcl, NULL, 0},
  { "HTFormat_transferCoding",HTFormat_transferCoding_tcl,    NULL, 0 },
  { "HTFormat_addTransferCoding", HTFormat_addTransferCoding_tcl, NULL, 0},
  { "HTFormat_setLanguage",   HTFormat_setLanguage_tcl,       NULL, 0 },
  { "HTFormat_language",      HTFormat_language_tcl,          NULL, 0 },
  { "HTFormat_setCharset",    HTFormat_setCharset_tcl,        NULL, 0 },
  { "HTFormat_charset",       HTFormat_charset_tcl,           NULL, 0 },
  { "HTFormat_deleteAll",     HTFormat_deleteAll_tcl,         NULL, 0 },
  { "HTStreamStack",          HTStreamStack_tcl,              NULL, 0 },
  { "HTStackValue",           HTStackValue_tcl,               NULL, 0 },
  { "HTContentCodingStack",   HTContentCodingStack_tcl,       NULL, 0 },
  { "HTContentEncodingStack", HTContentEncodingStack_tcl,     NULL, 0 },
  { "HTContentDecodingStack", HTContentDecodingStack_tcl,     NULL, 0 },
  { "HTTransferCodingStack",  HTTransferCodingStack_tcl,      NULL, 0 },

  /*HTLog*/
  { "HTLog_open",             HTLog_open_tcl,                 NULL, 0 },
  { "HTLog_close",            HTLog_close_tcl,                NULL, 0 },
  { "HTLog_isOpen",           HTLog_isOpen_tcl,               NULL, 0 },
  { "HTLog_add",              HTLog_add_tcl,                  NULL, 0 }, 
  /*HTAlert*/
  { "HTAlert_setInteractive", HTAlert_setInteractive_tcl,     NULL, 0 },
  { "HTAlert_interactive",    HTAlert_interactive_tcl,        NULL, 0 },
  { "HTAlertCall_add",        HTAlertCall_add_tcl,            NULL, 0 },
  { "HTAlertCall_delete",     HTAlertCall_delete_tcl,         NULL, 0 },
  { "HTAlertCall_deleteAll",  HTAlertCall_deleteAll_tcl,      NULL, 0 },
  { "HTAlertCall_find",       HTAlertCall_find_tcl,           NULL, 0 }, 
  { "HTAlert_newReply",       HTAlert_newReply_tcl,           NULL, 0 },
  { "HTAlert_deleteReply",    HTAlert_deleteReply_tcl,        NULL, 0 },
  { "HTAlert_setReplyMessage",HTAlert_setReplyMessage_tcl,    NULL, 0 },
  { "HTAlert_assignReplyMessage",HTAlert_assignReplyMessage_tcl,NULL, 0},
  { "HTAlert_replyMessage",   HTAlert_replyMessage_tcl,       NULL, 0 },
  { "HTAlert_replySecret",    HTAlert_replySecret_tcl,        NULL, 0 },
  { "HTAlert_setReplySecret", HTAlert_setReplySecret_tcl,     NULL, 0 },
  { "HTAlert_replyOutput",    HTAlert_replyOutput_tcl,        NULL, 0 },
  { "HTAlert_setReplyOutput", HTAlert_setReplyOutput_tcl,     NULL, 0 },
  { "HTAlert_setGlobal",      HTAlert_setGlobal_tcl,          NULL, 0 },
  { "HTAlert_global",         HTAlert_global_tcl,             NULL, 0 },
  { "HTAlert_add",            HTAlert_add_tcl,                NULL, 0 }, 
  { "HTAlert_delete",         HTAlert_delete_tcl,             NULL, 0 },
  { "HTAlert_find",           HTAlert_find_tcl,               NULL, 0 },

  /*HTError*/
  { "HTError_add",            HTError_add_tcl,                NULL, 0 },
  { "HTError_addSystem",      HTError_addSystem_tcl,          NULL, 0 },
  { "HTError_deleteAll",      HTError_deleteAll_tcl,          NULL, 0 },
  { "HTError_deleteLast",     HTError_deleteLast_tcl,         NULL, 0 },
  { "HTError_doShow",         HTError_doShow_tcl,             NULL, 0 }, 
  { "HTError_ignoreLast",     HTError_ignoreLast_tcl,         NULL, 0 },
  { "HTError_setIgnore",      HTError_setIgnore_tcl,          NULL, 0 },
  { "HTError_index",          HTError_index_tcl,              NULL, 0 },
  { "HTError_severity",       HTError_severity_tcl,           NULL, 0 },
  { "HTError_hasSeverity",    HTError_hasSeverity_tcl,        NULL, 0 },
  { "HTError_parameter",      HTError_parameter_tcl,          NULL, 0 },
  { "HTError_location",       HTError_location_tcl,           NULL, 0 },
  /*HTDNS*/
  { "HTDNS_setTimeout",       HTDNS_setTimeout_tcl,           NULL, 0 },
  { "HTDNS_timeout",          HTDNS_timeout_tcl,              NULL, 0 },
  { "HTDNS_add",              HTDNS_add_tcl,                  NULL, 0 },
  { "HTDNS_delete",           HTDNS_delete_tcl,               NULL, 0 },
  { "HTDNS_deleteAll",        HTDNS_deleteAll_tcl,            NULL, 0 },
  { "HTDNS_updateWeigths",    HTDNS_updateWeigths_tcl,       NULL, 0 },
  { "HTGetHostBySock",        HTGetHostBySock_tcl,            NULL, 0 },
  { "HTGetHostByName",        HTGetHostByName_tcl,            NULL, 0 },
  /*HTUTree*/
  { "HTUTree_delete",         HTUTree_delete_tcl,             NULL, 0 },
  { "HTUTree_deleteAll",      HTUTree_deleteAll_tcl,          NULL, 0 },
  { "HTUTree_find",           HTUTree_find_tcl,               NULL, 0 },
  { "HTUTree_findNode",       HTUTree_findNode_tcl,           NULL, 0 },
  { "HTUTree_addNode",        HTUTree_addNode_tcl,            NULL, 0 },
  { "HTUTree_replaceNode",    HTUTree_replaceNode_tcl,        NULL, 0 },
  { "HTUTree_deleteNode",     HTUTree_deleteNode_tcl,         NULL, 0 },
  /*HTTrans*/
  { "HTTransport_delete",     HTTransport_delete_tcl,         NULL, 0 },
  { "HTTransport_deleteAll",  HTTransport_deleteAll_tcl,      NULL, 0 },
  { "HTTransport_find",       HTTransport_find_tcl,           NULL, 0 },
  { "HTTransport_mode",       HTTransport_mode_tcl,           NULL, 0 },
  { "HTTransport_setMode",    HTTransport_setMode_tcl,        NULL, 0 },
  /*HTNet*/
  { "HTNetCall_deleteAll",    HTNetCall_deleteAll_tcl,        NULL, 0 },
  { "HTNetCall_execute",      HTNetCall_execute_tcl,          NULL, 0 },
  { "HTNet_setBefore",        HTNet_setBefore_tcl,            NULL, 0 },
  { "HTNet_before",           HTNet_before_tcl,               NULL, 0 },
  { "HTNet_callBefore",       HTNet_callBefore_tcl,           NULL, 0 },
  { "HTNet_setAfter",         HTNet_setAfter_tcl,             NULL, 0 },
  { "HTNet_after",            HTNet_after_tcl,                NULL, 0 },
  { "HTNet_callAfter",        HTNet_callAfter_tcl,            NULL, 0 },
  { "HTNet_setMaxSocket",     HTNet_setMaxSocket_tcl,         NULL, 0 },
  { "HTNet_maxSocket",        HTNet_maxSocket_tcl,            NULL, 0 },
  { "HTNet_increaseSocket",   HTNet_increaseSocket_tcl,       NULL, 0 },
  { "HTNet_decreaseSocket",   HTNet_decreaseSocket_tcl,       NULL, 0 },
  { "HTNet_availableSockets", HTNet_availableSockets_tcl,     NULL, 0 },
  { "HTNet_increasePersistentSocket",HTNet_increasePersistentSocket_tcl,
    NULL, 0 },
  { "HTNet_decreasePersistentSocket",   HTNet_decreasePersistentSocket_tcl,   
    NULL, 0 },
  { "HTNet_availablePersistentSockets",  HTNet_availablePersistentSockets_tcl, 
    NULL, 0 },
  { "HTNet_isIdle",           HTNet_isIdle_tcl,               NULL, 0 },
  { "HTNet_activeQueue",      HTNet_activeQueue_tcl,          NULL, 0 },
  { "HTNet_idle",             HTNet_idle_tcl,                 NULL, 0 },
  { "HTNet_isEmpty",          HTNet_isEmpty_tcl,              NULL, 0 },
  { "HTNet_pendingQueue",     HTNet_pendingQueue_tcl,         NULL, 0 },
  { "HTNet_newClient",        HTNet_newClient_tcl,            NULL, 0 },
  { "HTNet_newServer",         HTNet_newServer_tcl,             NULL, 0 },
  { "HTNet_new",              HTNet_new_tcl,                  NULL, 0 },
  { "HTNet_dup",              HTNet_dup_tcl,                  NULL, 0 },
  { "HTNet_start",            HTNet_start_tcl,                NULL, 0 },
  { "HTNet_delete",           HTNet_delete_tcl,               NULL, 0 },
  { "HTNet_deleteAll",        HTNet_deleteAll_tcl,            NULL, 0 },
  { "HTNet_wait",             HTNet_wait_tcl,                 NULL, 0 },
  { "HTNet_priority",         HTNet_priority_tcl,             NULL, 0 },
  { "HTNet_setPriority",      HTNet_setPriority_tcl,          NULL, 0 },
  { "HTNet_persistent",       HTNet_persistent_tcl,           NULL, 0 },
  { "HTNet_setPersistent",    HTNet_setPersistent_tcl,        NULL, 0 },
  { "HTNet_kill",             HTNet_kill_tcl,                 NULL, 0 },
  { "HTNet_killAll",          HTNet_killAll_tcl,              NULL, 0 },
  { "HTNet_setContext",       HTNet_setContext_tcl,           NULL, 0 },
  { "HTNet_context",          HTNet_context_tcl,               NULL, 0 },
  { "HTNet_setSocket",        HTNet_setSocket_tcl,            NULL, 0 },
  { "HTNet_socket",           HTNet_socket_tcl,               NULL, 0 },
  { "HTNet_setRequest",       HTNet_setRequest_tcl,           NULL, 0 },
  { "HTNet_request",          HTNet_request_tcl,              NULL, 0 },
  { "HTNet_setTransport",     HTNet_setTransport_tcl,         NULL, 0 },
  { "HTNet_transport",        HTNet_transport_tcl,            NULL, 0 },
  { "HTNet_setChannel",       HTNet_setChannel_tcl,           NULL, 0 },
  { "HTNet_channel",          HTNet_channel_tcl,              NULL, 0 },
  { "HTNet_setHost",          HTNet_setHost_tcl,              NULL, 0 },
  { "HTNet_host",             HTNet_host_tcl,                 NULL, 0 },
  { "HTNet_setDns",           HTNet_setDns_tcl,               NULL, 0 },
  { "HTNet_dns",              HTNet_dns_tcl,                  NULL, 0 },
  { "HTNet_home",             HTNet_home_tcl,                 NULL, 0 },
  /*HTInet*/
  { "HTErrnoString",          HTErrnoString_tcl,              NULL, 0 },
  { "HTInetStatus",           HTInetStatus_tcl,               NULL, 0 },
  { "HTParseInet",            HTParseInet_tcl,                NULL, 0 },
  { "HTGetTimeZoneOffset",    HTGetTimeZoneOffset_tcl,        NULL, 0 },
  { "HTGetHostName",          HTGetHostName_tcl,              NULL, 0 },
  { "HTGetMailAddress",       HTGetMailAddress_tcl,           NULL, 0 },
  { "HTGetNewsServer",        HTGetNewsServer_tcl,            NULL, 0 },
  { "HTGetTmpFileName",       HTGetTmpFileName_tcl,           NULL, 0 },
    { 0 }
};

int WWWLib_Init(Tcl_Interp *interp) 
{  
    LibraryFunction *c;

#ifdef CMDLINE
    HTProfile_newClient(APP_NAME, APP_VERSION);
#else
    HTLibInit(APP_NAME, APP_VERSION);
#endif

    Tcl_InitHashTable(&HTableAnchor, TCL_STRING_KEYS);
    Tcl_InitHashTable(&HTableLink, TCL_STRING_KEYS);
    Tcl_InitHashTable(&HTableList, TCL_STRING_KEYS);
    Tcl_InitHashTable(&HTableReq, TCL_STRING_KEYS);
    Tcl_InitHashTable(&HTableStream, TCL_STRING_KEYS);
    Tcl_InitHashTable(&HTableVoid, TCL_STRING_KEYS);
    Tcl_InitHashTable(&HTableAssoc, TCL_STRING_KEYS);
    Tcl_InitHashTable(&HTableUser, TCL_STRING_KEYS);
    Tcl_InitHashTable(&HTableCallback, TCL_STRING_KEYS);
    Tcl_InitHashTable(&HTableHost, TCL_STRING_KEYS);
    Tcl_InitHashTable(&HTableChunk, TCL_STRING_KEYS);
    Tcl_InitHashTable(&HTableProt,  TCL_STRING_KEYS);
    Tcl_InitHashTable(&HTableConverter, TCL_STRING_KEYS);
    Tcl_InitHashTable(&HTableCoder, TCL_STRING_KEYS);
    Tcl_InitHashTable(&HTableAlertCallback, TCL_STRING_KEYS);
    Tcl_InitHashTable(&HTableAlertPar, TCL_STRING_KEYS);
    Tcl_InitHashTable(&HTableError, TCL_STRING_KEYS);
    Tcl_InitHashTable(&HTableMIMEParseSet, TCL_STRING_KEYS);
    Tcl_InitHashTable(&HTableCallback, TCL_STRING_KEYS);
    Tcl_InitHashTable(&HTableNet, TCL_STRING_KEYS);
    Tcl_InitHashTable(&HTableDNS, TCL_STRING_KEYS);
    Tcl_InitHashTable(&HTableTrans, TCL_STRING_KEYS);
    Tcl_InitHashTable(&HTableChannel, TCL_STRING_KEYS);

    random_seed();
    init_strings();
    for(c = www_commands; c->name; c++){
	Tcl_CreateCommand(interp, c->name, c->proc, c->data,
			  NULL /* delete proc???*/);
    }
    return TCL_OK;
}

void WWWLib_Terminate() 
{
    HTLibTerminate();
    Tcl_DeleteHashTable(&HTableAnchor);    
    Tcl_DeleteHashTable(&HTableLink);    
    Tcl_DeleteHashTable(&HTableList);    
    Tcl_DeleteHashTable(&HTableReq);    
    Tcl_DeleteHashTable(&HTableStream);    
    Tcl_DeleteHashTable(&HTableVoid);
    Tcl_DeleteHashTable(&HTableAssoc);
    Tcl_DeleteHashTable(&HTableUser);
    Tcl_DeleteHashTable(&HTableCallback);
    Tcl_DeleteHashTable(&HTableHost);
    Tcl_DeleteHashTable(&HTableChannel);
    Tcl_DeleteHashTable(&HTableChunk);
    Tcl_DeleteHashTable(&HTableProt);
    Tcl_DeleteHashTable(&HTableCoder);
    Tcl_DeleteHashTable(&HTableAlertCallback);
    Tcl_DeleteHashTable(&HTableAlertPar);
    Tcl_DeleteHashTable(&HTableError);
    Tcl_DeleteHashTable(&HTableMIMEParseSet);
    Tcl_DeleteHashTable(&HTableCallback);
    Tcl_DeleteHashTable(&HTableNet);
    Tcl_DeleteHashTable(&HTableDNS);
    Tcl_DeleteHashTable(&HTableTrans);
}

/*=================================================*/


#if 0
void HText_beginAppend (HText * text) {
}

void HText_endAppend(HText *text) {
}

void HText_appendCharacter (HText * text, char ch) {
}

void HText_setStyle (HText * text, HTStyle * style) {
}

void HText_appendText (HText * text, CONST char * str) {
}

void HText_appendParagraph (HText * text) {
}

void HText_appendImage (HText *         text,
        HTChildAnchor * anc,
        CONST char *    alternative_text,
        CONST char *    alignment,
        BOOL            isMap) {
}

void HText_beginAnchor (HText * text, HTChildAnchor * anc) {
}

void HText_endAnchor (HText * text) {
}

#endif

void HText_appendLineBreak (HText * text) {
}

void HText_appendHorizontalRule (HText * text) {
}



void HText_setNextId (HText *         text,
                      CONST char *    s) {
}

BOOL HText_selectAnchor(HText * text, HTChildAnchor* anchor) {
  return YES;
}

BOOL HText_select (HText * text) {
  return YES;
}
























