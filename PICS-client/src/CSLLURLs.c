#include "WWWLib.h"
#include "CSLUtils.h"
#include "CSLL.h"
#include "CSLLSt.h" /* needs definition of SingleLabel and LLData */

struct State_Parms_s {
    CSLabel_t * pCSLabel;
    HTList * pList;
    };

PRIVATE CSLabel_callback_t CSLLHTList_outputSingleLabel;
PRIVATE CSError_t CSLLHTList_outputSingleLabel(CSLabel_t * pCSLabel, State_Parms_t * pParms, const char * identifier, void * pVoid)
{
    SingleLabel_t * pSingleLabel = CSLabel_getSingleLabel(pCSLabel);
    HTList_appendObject(pParms->pList, (void *)pSingleLabel);
    return CSError_OK;
}

PRIVATE CSLabel_callback_t CSLLHTList_outputLabel;
PRIVATE CSError_t CSLLHTList_outputLabel(CSLabel_t * pCSLabel, State_Parms_t * pParms, const char * identifier, void * pVoid)
{
    return CSLabel_iterateSingleLabels(pParms->pCSLabel, CSLLHTList_outputSingleLabel, pParms, 0, pVoid);
}

PRIVATE CSLabel_callback_t CSLLHTList_outputService;
PRIVATE CSError_t CSLLHTList_outputService(CSLabel_t * pCSLabel, State_Parms_t * pParms, const char * identifier, void * pVoid)
{
    return CSLabel_iterateLabels(pParms->pCSLabel, CSLLHTList_outputLabel, pParms, 0, pVoid);
}

PUBLIC HTList * CSLLData_getAllSingleLabels(CSLabel_t * pCSLabel)
{
    CSError_t err;
    State_Parms_t parms;

    if (CSLabel_getCSLLData(pCSLabel)->hasTree)
        return 0;
    parms.pCSLabel = pCSLabel;
    parms.pList = HTList_new();
    err = CSLabel_iterateServices(parms.pCSLabel, CSLLHTList_outputService, &parms, 0, 0); /* !!! - pVoid */
    return parms.pList;
}

