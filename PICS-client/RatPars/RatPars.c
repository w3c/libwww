#include "malloc.h"
#include "wwwsys.h"
#include "CSLUtils.h"
#include "CSMR.h"	/* to define states in stateChange */
#include "CSParse.h"

int Total;

extern int ParseDebug;

CSError_t spit(char* text, CSMachRead_t * pCSMR, BOOL closed)
{
    printf("%s %s\n", text, closed ? "closed" : "opened");
    return CSDoMore_more;
}

MachReadTargetCallback_t targetCallback;
StateRet_t targetCallback(CSMachRead_t * pCSMR, CSParse_t * pCSParse, CSMRTC_t target, BOOL closed, void * pVoid)
{
    int change = closed ? -target : target;

    Total += change;
    if (!ParseDebug);
        printf("%3d ", change);
/*	printf("%s %s (%d)\n", closed ? "  ending" : "starting", pCSParse->pParseState->note, closed ? -target : target); */
    return StateRet_OK;
}

/* LLErrorHandler_t parseErrorHandler; */
StateRet_t parseErrorHandler(CSMachRead_t * pCSMR, CSParse_t * pCSParse, 
			     const char * token, char demark, 
			     StateRet_t errorCode)
{
    char space[256];
    printf("%20s - %s:", pCSParse->pTargetObject->note, 
	   pCSParse->currentSubState == SubState_X ? "SubState_X" : 
	   pCSParse->currentSubState == SubState_N ? "SubState_N" : 
	   pCSParse->currentSubState == SubState_A ? "SubState_A" : 
	   pCSParse->currentSubState == SubState_B ? "SubState_B" : 
	   pCSParse->currentSubState == SubState_C ? "SubState_C" : 
	   pCSParse->currentSubState == SubState_D ? "SubState_D" : 
	   pCSParse->currentSubState == SubState_E ? "SubState_E" : 
	   pCSParse->currentSubState == SubState_F ? "SubState_F" : 
	   pCSParse->currentSubState == SubState_G ? "SubState_G" : 
	   pCSParse->currentSubState == SubState_H ? "SubState_H" : 
	   "???");
    switch (errorCode) {
        case StateRet_WARN_NO_MATCH:
            if (token)
	        sprintf(space, "Unexpected token \"%s\".\n", token);
	    else
	        sprintf(space, "Unexpected lack of token.\n");
            break;
        case StateRet_WARN_BAD_PUNCT:
            sprintf(space, "Unextected punctuation \"%c\"", demark);
	    if (token)
	        printf("after token \"%s\".\n", token);
	    else
	        printf(".\n");
            break;
        case StateRet_ERROR_BAD_CHAR:
            sprintf(space, "Unextected character \"%c\" in token \"%s\".\n", 
		    *pCSParse->pParseContext->pTokenError, token);
            break;
        default:
            sprintf(space, "Internal error: demark:\"%c\" token:\"%s\".\n", 
		    demark, token);
            break;
    }
    printf(space);
/*
    CSMR_dump(pCSMR);
    HTTrace(pParseState->note);
*/
  return errorCode;
}

#if 1
/* use this main to test input with a series of machReads, each on a line. */
int main(int argc, char** argv)
{
    char lineBuf[512];
    CSParse_t * pCSParse = 0;
    CSDoMore_t last = CSDoMore_done;
    FILE * input;

    if (argc > 1) {
        if ((input = fopen(argv[1], "r")) == NULL) {
	    printf("Couldn't open \"%s\".\n", argv[1]);
	    exit(1);
	}
    } else {
	input = stdin;
    }
    if (argc > 2)
        ParseDebug = 1;
    pCSParse = CSParse_newMachRead(&targetCallback, &parseErrorHandler);
    while (fgets(lineBuf, sizeof(lineBuf), input)){
        int len;
        if (lineBuf[0] == ';')
	    continue;
	len = strlen(lineBuf);
	if (lineBuf[len - 1] == '\r' || lineBuf[len - 1] == '\n') {
	    lineBuf[len-- - 1] = 0;
	}
	if (!lineBuf[0]) {
	    if (last != CSDoMore_done)
	        printf("parsing end error\n");
	    if (pCSParse)
	        CSParse_deleteMachRead(pCSParse);
	    pCSParse = CSParse_newMachRead(&targetCallback, &parseErrorHandler);
	    last = CSDoMore_done;
	} else {
	    printf("%s ", lineBuf); if (ParseDebug) printf("\n");
	    switch (last = CSParse_parseChunk(pCSParse, lineBuf, (int) strlen(lineBuf), 0)) {
	    case CSDoMore_done:
	        printf("= %d - parsing end\n", Total);
		break;
	    case CSDoMore_error:
		printf("= %d - parsing error\n", Total);
		exit (1);
	    case CSDoMore_more:
		printf("\n");
		break;
	    }
	}
    }
    if (pCSParse)
        CSParse_deleteMachRead(pCSParse);
    if (last != CSDoMore_done)
        printf("parsing end error\n");
    return (0);
}
#else
/* use this main to test input of a machRead list spread out over multiple lines*/
int main(int argc, char** argv)
{
    char lineBuf[512];
    while (gets(lineBuf)){
        CSParse_t * pCSParse;
	if (lineBuf[0] == ';')
	    continue;
	Total = 0;
	printf("%s", lineBuf);
        pCSParse = CSParse_newMachRead(&targetCallback, &parseErrorHandler);
        if (CSParse_parseChunk(pCSParse, lineBuf, 
			       (int)strlen(lineBuf), 0) != CSDoMore_done) {
	    printf("parsing end error\n");
	    break;
	}
        CSParse_deleteMachRead(pCSParse);
	printf("= %d\n", Total);
    }
    return (0);
}
#endif

#if 0 /* a not-needed but often useful sample implementation of HTTrace */
int HTTrace(const char * fmt, ...)
{
    va_list pArgs;
    va_start(pArgs, fmt);
    return (vfprintf(stderr, fmt, pArgs));
}
#endif
void * HTMemory_malloc (size_t size)
{
    return malloc(size);
}

void * HTMemory_calloc (size_t nobj, size_t size)
{
    return calloc(nobj, size);
}

void * HTMemory_realloc (void * p, size_t size)
{
    return realloc(p, size);
}

void HTMemory_free (void * ptr)
{
    free(ptr);
}

void HTMemory_outofmem (char * name, char * file, unsigned long line)
{
    HTTrace("%s:%ld failed allocation for \"%s\".\n\
Program aborted.\n",
	     file, line, name);
    exit(1);
}

