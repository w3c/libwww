/* HTDLL - module to be included only when making wwwuitls.dll
 * purpose: provide instances of the following exported globals:
 * 	    PTTYPrint - 
 * 	    WWW_TraceFlag - 
 */

typedef int (TTYPrint_t)(unsigned int target, const char* fmt, ...);
TTYPrint_t* PTTYPrint;
int WWW_TraceFlag = 0;	/* Global trace flag for ALL W3 code */
