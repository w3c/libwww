#include "WWWLib.h"
#include "CSLUtils.h"
#include "CSUser.h"
#include "CSUserSt.h"
#include "CSLL.h"
#include "CSLLSt.h"

extern void CSLabel_dump(CSLabel_t * pCSLabel);

struct State_Parms_s {
    CSLabel_t * pCSLabel;
    CSUser_t * pCSUser;
    int matchedServices;
    int matchedRatings;
    CSError_t iteratorError;
    };

PRIVATE CSLabel_callback_t CSCheckLabel_checkRatings;
PRIVATE CSError_t CSCheckLabel_checkRatings(CSLabel_t * pCSLabel, State_Parms_t * pParms, const char * identifier, void * pVoid)
{
    UserServiceRating_t * pUserServiceRating = CSUser_getUserServiceRating(pParms->pCSUser);
    CSError_t ret;
    FVal_t fVal;
    fVal = CSLabel_ratingsIncludeRanges(pCSLabel, pUserServiceRating->ranges);
    ret = FVal_isZero(&fVal) ? CSError_OK : CSError_RATING_RANGE;
    if (ret != CSError_OK)
        HTTRACE(PICS_TRACE, "PICS: Access denial - service:\"%s\" label number:%d rating identifier:\"%s\"\n" _
		CSLabel_getServiceName(pCSLabel) _
		CSLabel_getLabelNumber(pCSLabel) _
		CSLabel_getRatingName(pCSLabel));
    return ret;
}

PRIVATE CSUser_callback_t CSCheckLabel_findLabelServiceRating;
PRIVATE CSError_t CSCheckLabel_findLabelServiceRating(CSUser_t * pCSUser, State_Parms_t * pParms, const char * identifier, void * pVoid)
{
    UserServiceRating_t * pUserServiceRating = CSUser_getUserServiceRating(pParms->pCSUser);
    CSError_t ret;
    if ((ret = CSLabel_iterateLabelRatings(pParms->pCSLabel, CSCheckLabel_checkRatings, pParms, SVal_value(&pUserServiceRating->identifier), 0)) != CSError_SERVICE_MISSING)
        pParms->matchedRatings++;
    else
        if (!BVal_value(&pUserServiceRating->missing_scale))
            ret = CSError_OK;        /* keep checking */
    return ret;
}

PRIVATE CSLabel_callback_t CSCheckLabel_checkSingleLabel;
PRIVATE CSError_t CSCheckLabel_checkSingleLabel(CSLabel_t * pCSLabel, State_Parms_t * pParms, const char * identifier, void * pVoid)
{
    return CSUser_iterateServiceRatings(pParms->pCSUser, CSCheckLabel_findLabelServiceRating, pParms, 0, 0);
}

PRIVATE CSLabel_callback_t CSCheckLabel_checkLabel;
PRIVATE CSError_t CSCheckLabel_checkLabel(CSLabel_t * pCSLabel, State_Parms_t * pParms, const char * identifier, void * pVoid)
{
    return CSLabel_iterateSingleLabels(pParms->pCSLabel, CSCheckLabel_checkSingleLabel, pParms, 0, 0);
}

PRIVATE CSLabel_callback_t CSCheckLabel_checkService;
PRIVATE CSError_t CSCheckLabel_checkService(CSLabel_t * pCSLabel, State_Parms_t * pParms, const char * identifier, void * pVoid)
{
    return CSLabel_iterateLabels(pParms->pCSLabel, CSCheckLabel_checkLabel, pParms, 0, 0);
}

PRIVATE CSUser_callback_t CSCheckLabel_findLabelService;
PRIVATE CSError_t CSCheckLabel_findLabelService(CSUser_t * pCSUser, State_Parms_t * pParms, const char * identifier, void * pVoid)
{
    UserService_t * pUserService = CSUser_getUserService(pParms->pCSUser);
    CSError_t ret;
    if ((ret = CSLabel_iterateServices(pParms->pCSLabel, CSCheckLabel_checkService, pParms, SVal_value(&pUserService->rating_service), 0)) == CSError_SERVICE_MISSING)
        return BVal_value(&pUserService->missing_service) ? CSError_SERVICE_MISSING : CSError_OK;
    if (ret == CSError_OK)
        pParms->matchedServices++;
    return ret;
}

PUBLIC CSError_t CSCheckLabel_checkLabelAndUser(CSLabel_t * pCSLabel, CSUser_t * pCSUser)
{
    CSError_t ret;
    State_Parms_t parms = {0, 0, 0, 0, CSError_OK};
    parms.pCSLabel = pCSLabel;
    parms.pCSUser = pCSUser;
    parms.iteratorError = CSError_SERVICE_NONE;
/*
    CSLabel_output(parms.pCSLabel, pKwik);
*/
    if (PICS_TRACE) CSLabel_dump(parms.pCSLabel);
/*
    HTList * tmp;
    tmp = CSLLData_getAllSingleLabels(parms.pCSLabel);
    HTList_delete(tmp); */
    if ((ret = CSUser_iterateServices(parms.pCSUser, CSCheckLabel_findLabelService, &parms, 0, 0)) != CSError_OK)
        return ret;
    if (FVal_initialized(&CSUser_getCSUserData(pCSUser)->minimum_services) && 
        parms.matchedServices < FVal_value(&CSUser_getCSUserData(pCSUser)->
					   minimum_services))
        return CSError_SERVICE_NONE;
    return CSError_OK;
}

PUBLIC CSError_t CSCheckLabel_parseAndValidateLabelStr(const char * label, CSUser_t * pCSUser)
{
    CSParse_t * pCSParse = CSParse_newLabel(0, 0);
    CSError_t ret;

    CSParse_parseChunk(pCSParse, label, (int) strlen(label), 0);
    ret = CSCheckLabel_checkLabelAndUser(CSParse_getLabel(pCSParse), pCSUser);

    CSParse_deleteLabel(pCSParse);
    return ret;
}
