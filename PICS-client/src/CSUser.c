#include "wwwsys.h"
#include "HTUtils.h"
#include "HTString.h"
#include "HTEscape.h"
#include "HTParse.h"
#include "CSParse.h"
#include "CSUser.h"
#include "CSUserSt.h"

#define GetCSUser(A) ((A)->target.pCSUser)

struct CSUser_s {
    CSUserData_t * pCSUserData;

    UserService_t * pCurrentUserService;
    UserServiceRating_t * pCurrentUserServiceRating;
    Range_t * pCurrentRange;

    int currentLabelNumber;

    UserTargetCallback_t * pUserTargetCallback;
    UserErrorHandler_t * pUserErrorHandler;
};

PRIVATE TargetObject_t User_targetObject;
PRIVATE TargetObject_t UserService_targetObject;
PRIVATE TargetObject_t UserRatingList_targetObject;
PRIVATE TargetObject_t UserRating_targetObject;
PRIVATE TargetObject_t UserRange_targetObject;
PRIVATE Prep_t User_setTarget;
PRIVATE Prep_t clearToken;
PRIVATE Check_t getProxy;
PRIVATE Check_t getValue;
PRIVATE Check_t UserService_getId;
PRIVATE Check_t UserRating_getId;
PRIVATE Check_t UserRange_get;
PRIVATE Open_t User_open;
PRIVATE Open_t UserService_open;
PRIVATE Open_t UserRating_open;
PRIVATE Open_t UserRatingList_open;
PRIVATE Open_t UserRange_open;
PRIVATE Close_t User_close;
PRIVATE Close_t UserService_close;
PRIVATE Close_t UserRatingList_close;
PRIVATE Close_t UserRating_close;
PRIVATE Close_t UserRange_close;
PRIVATE Destroy_t User_destroy;
PRIVATE Destroy_t UserService_destroy;
PRIVATE Destroy_t UserRatingList_destroy;
PRIVATE Destroy_t UserRating_destroy;
PRIVATE Destroy_t UserRange_destroy;

PRIVATE TargetChangeCallback_t targetChangeCallback;
PRIVATE ParseErrorHandler_t parseErrorHandler;

/* CSParse_doc states */
/* U S E R   P A R S E R   S T A T E S */
/* This parses the example User profile for the LineMode browser.
 * See User/Parsing.html for details.
 */
PRIVATE StateToken_t User_stateTokens[] = { 
    /* A: open or close
       B: red open
       C: read value 
       D: read proxy */
     {           "start", SubState_N,    Punct_ALL, 0,                  0, 0, 0, &User_targetObject, SubState_A, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     {            "open", SubState_A, Punct_LPAREN, 0,                  0, 0, 0, &User_targetObject, SubState_B, 0, 0},
     {             "end", SubState_A, Punct_RPAREN, 0,                  0, 0, 0, &User_targetObject, SubState_A, Command_CLOSE, 0},
     {         "version", SubState_B,  Punct_WHITE, 0,          "version", 0, 0, &User_targetObject, SubState_C, 0, &User_setTarget},
     {       "user-name", SubState_B,  Punct_WHITE, 0,        "user-name", 0, 0, &User_targetObject, SubState_C, 0, &User_setTarget},
     {        "password", SubState_B,  Punct_WHITE, 0,         "password", 0, 0, &User_targetObject, SubState_C, 0, &User_setTarget},
     {      "super-user", SubState_B,  Punct_WHITE, 0,       "super-user", 0, 0, &User_targetObject, SubState_C, 0, &User_setTarget},
     {"minimum-services", SubState_B,  Punct_WHITE, 0, "minimum-services", 0, 0, &User_targetObject, SubState_C, 0, &User_setTarget},
     { "missing-service", SubState_B,  Punct_WHITE, 0,  "missing-service", 0, 0, &User_targetObject, SubState_C, 0, &User_setTarget},
     {   "missing-scale", SubState_B,  Punct_WHITE, 0,    "missing-scale", 0, 0, &User_targetObject, SubState_C, 0, &User_setTarget},
     {   "observe-dates", SubState_B,  Punct_WHITE, 0,    "observe-dates", 0, 0, &User_targetObject, SubState_C, 0, &User_setTarget},
     {          "bureau", SubState_B,  Punct_WHITE,0,"bureau","label-bureau", 0, &User_targetObject, SubState_C, 0, &User_setTarget},
     {           "proxy", SubState_B,  Punct_WHITE, 0,            "proxy", 0, 0, &User_targetObject, SubState_D, 0, 0},
     {  "rating-service", SubState_B,  Punct_WHITE, 0,   "rating-service", 0, 0, &UserService_targetObject, SubState_N, 0, 0},
     {       "get value", SubState_C, Punct_RPAREN, &getValue,          0, 0, 0, &User_targetObject, SubState_A, 0, 0},
     {       "get proxy", SubState_D, Punct_RPAREN, &getProxy,          0, 0, 0, &User_targetObject, SubState_A, 0, 0}
    };
PRIVATE TargetObject_t User_targetObject = {"User", &User_open, &User_close, &User_destroy, User_stateTokens, raysize(User_stateTokens), CSUserTC_USER};

PRIVATE StateToken_t UserService_stateTokens[] = { 
    /* A: fresh UserService
       B: red id
       C: read value 
       D: need paren before next parm */
     {          "start", SubState_N,    Punct_ALL, 0,                  0, 0, 0, &UserService_targetObject, SubState_A, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     {  "rating system", SubState_A, Punct_LPAREN, &UserService_getId, 0, 0, 0, &UserService_targetObject, SubState_B, 0, 0},
     {        "service", SubState_B,  Punct_WHITE, 0,   "rating-service", 0, 0, &UserService_targetObject, SubState_C, 0, &User_setTarget},
     {"missing-service", SubState_B,  Punct_WHITE, 0,  "missing-service", 0, 0, &UserService_targetObject, SubState_C, 0, &User_setTarget},
     {  "missing-scale", SubState_B,  Punct_WHITE, 0,    "missing-scale", 0, 0, &UserService_targetObject, SubState_C, 0, &User_setTarget},
     {  "observe-dates", SubState_B,  Punct_WHITE, 0,    "observe-dates", 0, 0, &UserService_targetObject, SubState_C, 0, &User_setTarget},
     {        "ratings", SubState_B, Punct_LPAREN|Punct_RPAREN,0,"ratings",0,0, &UserRatingList_targetObject, SubState_N, Command_CHAIN, &clearToken},
     {      "get value", SubState_C, Punct_RPAREN, &getValue,          0, 0, 0, &UserService_targetObject, SubState_D, 0, 0},
     {            "end", SubState_D, Punct_RPAREN, 0,                  0, 0, 0, &User_targetObject, SubState_A, Command_CLOSE, 0},
     {      "need open", SubState_D, Punct_LPAREN, 0,                  0, 0, 0, &UserService_targetObject, SubState_B, 0, 0}
    };
PRIVATE TargetObject_t UserService_targetObject = {"UserService", &UserService_open, &UserService_close, &UserService_destroy, UserService_stateTokens, raysize(UserService_stateTokens), CSUserTC_SERVICE};

PRIVATE StateToken_t UserRatingList_stateTokens[] = { 
    /* A: single state list, expects open or close paren */
     {          "start", SubState_N,    Punct_ALL, 0, 0, 0, 0, &UserRatingList_targetObject, SubState_A, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     {   "start rating", SubState_A, Punct_LPAREN, 0, 0, 0, 0,     &UserRating_targetObject, SubState_N, 0, 0},
     {"end rating list", SubState_A, Punct_RPAREN, 0, 0, 0, 0,    &UserService_targetObject, SubState_D, Command_CLOSE, 0}
    };
PRIVATE TargetObject_t UserRatingList_targetObject = {"UserRatingList", &UserRatingList_open, &UserRatingList_close, &UserRatingList_destroy, UserRatingList_stateTokens, raysize(UserRatingList_stateTokens), CSUserTC_RLIST};

PRIVATE StateToken_t UserRating_stateTokens[] = { 
    /* A: fresh UserRating
       B: red id
       C: get value
       D: need paren before next parm */
     {          "start", SubState_N,    Punct_ALL, 0,                 0, 0, 0, &UserRating_targetObject, SubState_A, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     {       "range id", SubState_A, Punct_LPAREN, &UserRating_getId, 0, 0, 0, &UserRating_targetObject, SubState_B, 0, 0},
     {  "missing-scale", SubState_B,  Punct_WHITE, 0,   "missing-scale", 0, 0, &UserRating_targetObject, SubState_C, 0, &User_setTarget},
     {  "observe-dates", SubState_B,  Punct_WHITE, 0,   "observe-dates", 0, 0, &UserRating_targetObject, SubState_C, 0, &User_setTarget},
     {         "ranges", SubState_B,  Punct_WHITE, 0,    "range", "ranges", 0, &UserRange_targetObject, SubState_N, 0, 0},
     {      "get value", SubState_C, Punct_RPAREN, &getValue,         0, 0, 0, &UserRating_targetObject, SubState_D, 0, 0},
     {     "rating end", SubState_D, Punct_RPAREN, 0,                 0, 0, 0, &UserRatingList_targetObject, SubState_A, Command_CLOSE, 0},
     {      "need open", SubState_D, Punct_LPAREN, 0,                 0, 0, 0, &UserRating_targetObject, SubState_B, 0, 0}
    };
PRIVATE TargetObject_t UserRating_targetObject = {"UserRating", &UserRating_open, &UserRating_close, &UserRating_destroy, UserRating_stateTokens, raysize(UserRating_stateTokens), CSUserTC_RATING};

PRIVATE StateToken_t UserRange_stateTokens[] = { 
    /* A: read ranges */
     {      "start", SubState_N,    Punct_ALL,              0, 0, 0, 0, &UserRange_targetObject, SubState_A, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     {      "value", SubState_A,  Punct_WHITE, &UserRange_get, 0, 0, 0, &UserRange_targetObject, SubState_A, 0, 0},
     {  "value end", SubState_A, Punct_RPAREN, &UserRange_get, 0, 0, 0, &UserRating_targetObject, SubState_D, Command_CLOSE, 0}
    };
PRIVATE TargetObject_t UserRange_targetObject = {"UserRange", &UserRange_open, &UserRange_close, &UserRange_destroy, UserRange_stateTokens, raysize(UserRange_stateTokens), CSUserTC_RANGE};

/* CSParse_doc end */
/* S T A T E   A S S O C I A T I O N - associate a CSUser with the label list data 
   The label list data is kept around until all states referencing it are destroyed */
typedef struct {
    CSUser_t * pCSUser;
    CSUserData_t * pCSUserData;
    } CSUserAssoc_t;

PRIVATE HTList * CSUserAssocs = 0;

PRIVATE void CSUserAssoc_add(CSUser_t * pCSUser, CSUserData_t * pCSUserData)
{
    CSUserAssoc_t * pElement;
    if ((pElement = (CSUserAssoc_t *) HT_CALLOC(1, sizeof(CSUserAssoc_t))) == NULL)
        HT_OUTOFMEM("CSUserAssoc_t");
    pElement->pCSUser = pCSUser;
    pElement->pCSUserData = pCSUserData;
    if (!CSUserAssocs)
        CSUserAssocs = HTList_new();
    HTList_appendObject(CSUserAssocs, (void *)pElement);
}

PRIVATE CSUserAssoc_t * CSUserAssoc_findByData(CSUserData_t * pCSUserData)
{
    HTList * assocs = CSUserAssocs;
    CSUserAssoc_t * pElement;
    while ((pElement = (CSUserAssoc_t *) HTList_nextObject(assocs)))
        if (pElement->pCSUserData == pCSUserData)
            return pElement;
    return 0;
}

PRIVATE CSUserAssoc_t * CSUserAssoc_findByState(CSUser_t * pCSUser)
{
    HTList * assocs = CSUserAssocs;
    CSUserAssoc_t * pElement;
    while ((pElement = (CSUserAssoc_t *) HTList_nextObject(assocs)))
        if (pElement->pCSUser == pCSUser)
            return pElement;
    return 0;
}

PRIVATE void CSUserAssoc_removeByState(CSUser_t * pCSUser)
{
    CSUserAssoc_t * pElement = CSUserAssoc_findByState(pCSUser);
    if (!pElement)
        return;
    HTList_removeObject(CSUserAssocs, (void *)pElement);
    HT_FREE(pElement);
}

/* C O N S T R U C T O R S / D E S T R U C T O R S */
PRIVATE UserServiceRating_t * UserServiceRating_new(void)
{
    UserServiceRating_t * me;
    if ((me = (UserServiceRating_t *) HT_CALLOC(1, sizeof(UserServiceRating_t))) == NULL)
        HT_OUTOFMEM("UserServiceRating_t");
    me->ranges = HTList_new();
    return me;
}

PUBLIC void UserServiceRating_free(UserServiceRating_t * me)
{
    Range_t * pRange;
    while ((pRange = (Range_t *) HTList_removeLastObject(me->ranges)))
        HT_FREE(pRange);
    HTList_delete(me->ranges);
    SVal_clear(&me->identifier);
    BVal_clear(&me->missing_scale);
    BVal_clear(&me->observe_dates);
    HT_FREE(me);
}

PRIVATE UserService_t * UserService_new(void)
{
    UserService_t * me;
	if ((me = (UserService_t *) HT_CALLOC(1, sizeof(UserService_t))) == NULL)
	    HT_OUTOFMEM("UserService_t");
    return me;
}

PUBLIC void UserService_free(UserService_t * me)
{
    UserServiceRating_t * pUserServiceRating;
    while ((pUserServiceRating = (UserServiceRating_t *) HTList_removeLastObject(me->userServiceRatings)))
        UserServiceRating_free(pUserServiceRating);
    HTList_delete(me->userServiceRatings);
    FVal_clear(&me->version);
    SVal_clear(&me->rating_system);
    SVal_clear(&me->rating_service);
    BVal_clear(&me->missing_service);
    BVal_clear(&me->missing_scale);
    BVal_clear(&me->observe_dates);
    HT_FREE(me);
}

PUBLIC CSUserData_t * CSUserData_new(void)
{
    CSUserData_t * me;
    if ((me = (CSUserData_t *) HT_CALLOC(1, sizeof(CSUserData_t))) == NULL)
        HT_OUTOFMEM("CSUserData_t");
    me->userServices = HTList_new();
    me->proxies = HTList_new();
    return me;
}

PUBLIC void CSUserData_free(CSUserData_t * me)
{
    UserService_t * pUserService;
    char * proxy;
    if (CSUserAssoc_findByData(me))
        return;
    while ((pUserService = (UserService_t *) HTList_removeLastObject(me->userServices)))
        UserService_free(pUserService);
    HTList_free(me->userServices);
    while ((proxy = (char *) HTList_removeLastObject(me->proxies)))
        HT_FREE(proxy);
    HTList_free(me->proxies);
    FVal_clear(&me->version);
    SVal_clear(&me->user_name);
    SVal_clear(&me->password);
    BVal_clear(&me->super_user);
    FVal_clear(&me->minimum_services);
    BVal_clear(&me->missing_service);
    BVal_clear(&me->missing_scale);
    BVal_clear(&me->observe_dates);
    SVal_clear(&me->bureau);
    HT_FREE(me);
}

PUBLIC CSUser_t * CSUser_new(CSUserData_t * pCSUserData)
{
    CSUser_t * me;
    if ((me = (CSUser_t *) HT_CALLOC(1, sizeof(CSUser_t))) == NULL)
        HT_OUTOFMEM("CSUser_t");
    me->pCSUserData = pCSUserData;
    CSUserAssoc_add(me, pCSUserData);
    return me;
}

PUBLIC CSUser_t * CSUser_copy(CSUser_t * old)
{
    CSUser_t * me;
    me = CSUser_new(old->pCSUserData);
    memcpy(me, old, sizeof(CSUser_t));
    return me;
}

PUBLIC void CSUser_free(CSUser_t * me)
{
    CSUserData_t * pCSUserData = me->pCSUserData;
    CSUserAssoc_removeByState(me);
    HT_FREE(me);
    CSUserData_free(pCSUserData);
}

PUBLIC char * CSUser_name(CSUser_t * pCSUser) {return SVal_value(&pCSUser->pCSUserData->user_name);}
PUBLIC BOOL CSUser_checkPassword(CSUser_t * pCSUser, char * password) {return strcasecomp(SVal_value(&pCSUser->pCSUserData->password), password) ? NO : YES;}
PUBLIC char * CSUser_bureau(CSUser_t * pCSUser) {return SVal_value(&pCSUser->pCSUserData->bureau);}
PUBLIC CSUserData_t * CSUser_getCSUserData(CSUser_t * me) {return me->pCSUserData;}
PUBLIC UserService_t * CSUser_getUserService(CSUser_t * pCSUser) {return pCSUser->pCurrentUserService;}
PUBLIC UserServiceRating_t * CSUser_getUserServiceRating(CSUser_t * pCSUser) {return pCSUser->pCurrentUserServiceRating;}
PUBLIC Range_t * CSUser_getUserRatingRange(CSUser_t * pCSUser) {return pCSUser->pCurrentRange;}
PUBLIC char * CSUser_getRatingStr(CSUser_t * pCSUser)
{
    HTChunk * pChunk;
    HTList * ranges;
    Range_t * curRange;
    int count = 0;
    pChunk = HTChunk_new(20);
    ranges = CSUser_getUserServiceRating(pCSUser)->ranges;
    while ((curRange = (Range_t *) HTList_nextObject(ranges))) {
        char * ptr;
	count++;
	ptr = Range_toStr(curRange);
	if (count > 1)
	    HTChunk_puts(pChunk, " ");
	HTChunk_puts(pChunk, ptr);
	HT_FREE(ptr);
    }
    return HTChunk_toCString(pChunk);
}

PUBLIC CSParse_t * CSParse_newUser()
{
    CSParse_t * me = CSParse_new();
    me->pParseContext->engineOf = &CSParse_targetParser;
    me->pParseContext->pTargetChangeCallback = &targetChangeCallback;
    me->pParseContext->pParseErrorHandler = &parseErrorHandler;
    me->target.pCSUser = CSUser_new(CSUserData_new());
    me->pTargetObject = &User_targetObject;
    me->currentSubState = SubState_N;
    return me;
}

PUBLIC CSUser_t * CSParse_getUser(CSParse_t * me)
{
    return (me->target.pCSUser);
}

PUBLIC BOOL CSParse_deleteUser(CSParse_t * pCSParse)
{
    CSUser_t * me = GetCSUser(pCSParse);
    CSUserData_free(CSUser_getCSUserData(me));
    CSUser_free(me);
    return YES;
}

/* P A R S I N G   H A N D L E R S */
PRIVATE StateRet_t targetChangeCallback(CSParse_t * pCSParse, TargetObject_t * pTargetObject, CSParseTC_t target, BOOL closed, void * pVoid)
{

    CSUser_t * pCSUser = GetCSUser(pCSParse);
    if (pCSUser->pUserTargetCallback)
	return (*pCSUser->pUserTargetCallback)(pCSUser, pCSParse, (CSUserTC_t)target, closed, pVoid);
    return StateRet_OK;
}

PRIVATE StateRet_t parseErrorHandler(CSParse_t * pCSParse, const char * token, char demark, StateRet_t errorCode)
{
	CSUser_t * pCSUser = GetCSUser(pCSParse);
	if (pCSUser->pUserErrorHandler)
		return (*pCSUser->pUserErrorHandler)(pCSUser, pCSParse, token, demark, errorCode);
  return errorCode;
}

/* CSParse_doc methods */
/* P A R S I N G   S T A T E   F U N C T I O N S */
PRIVATE StateRet_t clearToken(CSParse_t * pCSParse, char * token, char demark)
{
    HTChunk_clear(pCSParse->token);
    return StateRet_OK;
}

PRIVATE StateRet_t getProxy(CSParse_t * pCSParse, StateToken_t * pStateToken, char * token, char demark)
{
    CSUser_t * pCSUser = GetCSUser(pCSParse);
    ParseContext_t * pParseContext = pCSParse->pParseContext;
    char * proxy = 0;
    if (!token || !pParseContext->observedQuotes)
        return StateRet_WARN_NO_MATCH;
    if (Punct_badDemark(pStateToken->validPunctuation, demark))
        return StateRet_WARN_BAD_PUNCT; /* let CSParse_targetParser get the punctuation error */
    StrAllocCopy(proxy, token);
    HTList_appendObject(pCSUser->pCSUserData->proxies, (void *)proxy);
    return StateRet_OK;
}

#define READY_BVAL(holder, pointer) \
    pCSParse->pParseContext->valTarget.pTargetBVal = &CSUser_get##holder(pCSUser)->pointer; pCSParse->pParseContext->valType = ValType_BVAL;
#define READY_FVAL(holder, pointer) \
    pCSParse->pParseContext->valTarget.pTargetFVal = &CSUser_get##holder(pCSUser)->pointer; pCSParse->pParseContext->valType = ValType_FVAL;
#define READY_DVAL(holder, pointer) \
    pCSParse->pParseContext->valTarget.pTargetDVal = &CSUser_get##holder(pCSUser)->pointer; pCSParse->pParseContext->valType = ValType_DVAL;
#define READY_SVAL(holder, pointer) \
    pCSParse->pParseContext->valTarget.pTargetSVal = &CSUser_get##holder(pCSUser)->pointer; pCSParse->pParseContext->valType = ValType_SVAL;

PRIVATE StateRet_t User_setTarget(CSParse_t * pCSParse, char * token, char demark)
{
    CSUser_t * pCSUser = GetCSUser(pCSParse);
    int caseNumber;
    pCSParse->pParseContext->valType = ValType_SVAL;

    if (pCSParse->pTargetObject == &User_targetObject) {
        caseNumber = pCSParse->pStateToken-User_stateTokens;
        switch (caseNumber) {
            case 3: READY_FVAL(CSUserData, version); break;
            case 4: READY_SVAL(CSUserData, user_name); break;
            case 5: READY_SVAL(CSUserData, password); break;
            case 6: READY_BVAL(CSUserData, super_user); break;
            case 7: READY_FVAL(CSUserData, minimum_services); break;
            case 8: READY_BVAL(CSUserData, missing_service); break;
            case 9: READY_BVAL(CSUserData, missing_scale); break;
            case 10: READY_BVAL(CSUserData, observe_dates); break;
            case 11: READY_SVAL(CSUserData, bureau); break;
        }
    } else if (pCSParse->pTargetObject == &UserService_targetObject) {
        caseNumber = pCSParse->pStateToken-UserService_stateTokens;
        switch (caseNumber) {
            case 2: READY_SVAL(UserService, rating_service); break;
            case 3: READY_BVAL(UserService, missing_service); break;
            case 4: READY_BVAL(UserService, missing_scale); break;
            case 5: READY_BVAL(UserService, observe_dates); break;
        }
    } else if (pCSParse->pTargetObject == &UserRating_targetObject) {
        caseNumber = pCSParse->pStateToken-UserRating_stateTokens;
        switch (caseNumber) {
            case 2: READY_BVAL(UserServiceRating, missing_scale); break;
            case 3: READY_BVAL(UserServiceRating, observe_dates); break;
        }
    }
    return StateRet_OK;
}

PRIVATE StateRet_t getValue(CSParse_t * pCSParse, StateToken_t * pStateToken, char * token, char demark)
{
    switch (pCSParse->pParseContext->valType) {
        case ValType_BVAL:
            BVal_readVal(pCSParse->pParseContext->valTarget.pTargetBVal, token);
            pCSParse->pParseContext->valType = ValType_NONE;
            break;
        case ValType_FVAL:
            FVal_readVal(pCSParse->pParseContext->valTarget.pTargetFVal, token);
            pCSParse->pParseContext->valType = ValType_NONE;
            break;
        case ValType_SVAL:
            SVal_readVal(pCSParse->pParseContext->valTarget.pTargetSVal, token);
            pCSParse->pParseContext->valType = ValType_NONE;
            break;
        case ValType_DVAL:
            DVal_readVal(pCSParse->pParseContext->valTarget.pTargetDVal, token);
            pCSParse->pParseContext->valType = ValType_NONE;
            break;
	default:
	    break;
    }
    return StateRet_OK;
}

PRIVATE StateRet_t User_open(CSParse_t * pCSParse, char * token, char demark)
{
    return StateRet_OK;
}

PRIVATE StateRet_t User_close(CSParse_t * pCSParse, char * token, char demark)
{
    return StateRet_DONE;
}

PRIVATE void User_destroy(CSParse_t * pCSParse)
{
/*    CSUser_t * pCSUser = GetCSUser(pCSParse); */
}

PRIVATE StateRet_t UserService_open(CSParse_t * pCSParse, char * token, char demark)
{
    CSUser_t * pCSUser = GetCSUser(pCSParse);
    pCSUser->pCurrentUserService = UserService_new();
    HTList_appendObject(pCSUser->pCSUserData->userServices, (void *)pCSUser->pCurrentUserService);
    return StateRet_OK;
}

PRIVATE StateRet_t UserService_getId(CSParse_t * pCSParse, StateToken_t * pStateToken, char * token, char demark)
{
    CSUser_t * pCSUser = GetCSUser(pCSParse);
    ParseContext_t * pParseContext = pCSParse->pParseContext;
    if (!token || !pParseContext->observedQuotes)
        return StateRet_WARN_NO_MATCH;
    if (Punct_badDemark(pStateToken->validPunctuation, demark))
        return StateRet_WARN_BAD_PUNCT; /* let CSParse_targetParser get the punctuation error */
    SVal_readVal(&pCSUser->pCurrentUserService->rating_service, token);
    return StateRet_OK;
}

PRIVATE StateRet_t UserService_close(CSParse_t * pCSParse, char * token, char demark)
{
    CSUser_t * pCSUser = GetCSUser(pCSParse);
    pCSUser->pCurrentUserService = 0;
    return StateRet_OK;
}

PRIVATE void UserService_destroy(CSParse_t * pCSParse)
{
    CSUser_t * pCSUser = GetCSUser(pCSParse);
    HTList_removeObject(pCSUser->pCSUserData->userServices, (void *)pCSUser->pCurrentUserService);
    UserService_free(pCSUser->pCurrentUserService);
    pCSUser->pCurrentUserService = 0;
}

PRIVATE StateRet_t UserRatingList_open(CSParse_t * pCSParse, char * token, char demark)
{
    CSUser_t * pCSUser = GetCSUser(pCSParse);
    pCSUser->pCurrentUserService->userServiceRatings = HTList_new();
    return StateRet_OK;
}

PRIVATE StateRet_t UserRatingList_close(CSParse_t * pCSParse, char * token, char demark)
{
    return StateRet_OK;
}

PRIVATE void UserRatingList_destroy(CSParse_t * pCSParse)
{
    CSUser_t * pCSUser = GetCSUser(pCSParse);
    UserService_t * pUserService = pCSUser->pCurrentUserService;
    UserServiceRating_t * pUserServiceRating;
    while ((pUserServiceRating = (UserServiceRating_t *) HTList_removeLastObject(pUserService->userServiceRatings)))
        UserServiceRating_free(pUserServiceRating);
    HTList_delete(pUserService->userServiceRatings);
    pUserService->userServiceRatings = 0;
}

PRIVATE StateRet_t UserRating_open(CSParse_t * pCSParse, char * token, char demark)
{
    CSUser_t * pCSUser = GetCSUser(pCSParse);
    pCSUser->pCurrentUserServiceRating = UserServiceRating_new();
    HTList_appendObject(pCSUser->pCurrentUserService->userServiceRatings, (void *)pCSUser->pCurrentUserServiceRating);
    return StateRet_OK;
}

PRIVATE StateRet_t UserRating_getId(CSParse_t * pCSParse, StateToken_t * pStateToken, char * token, char demark)
{
    CSUser_t * pCSUser = GetCSUser(pCSParse);
    ParseContext_t * pParseContext = pCSParse->pParseContext;
    if (!token || !pParseContext->observedQuotes)
        return StateRet_WARN_NO_MATCH;
    if (Punct_badDemark(pStateToken->validPunctuation, demark))
        return StateRet_WARN_BAD_PUNCT; /* let CSParse_targetParser get the punctuation error */
    SVal_readVal(&pCSUser->pCurrentUserServiceRating->identifier, token);
    return StateRet_OK;
}

PRIVATE StateRet_t UserRating_close(CSParse_t * pCSParse, char * token, char demark)
{
    CSUser_t * pCSUser = GetCSUser(pCSParse);
    pCSUser->pCurrentUserServiceRating = 0;
    return StateRet_OK;
}

PRIVATE void UserRating_destroy(CSParse_t * pCSParse)
{
    CSUser_t * pCSUser = GetCSUser(pCSParse);
    HTList_removeObject(pCSUser->pCurrentUserService->userServiceRatings, (void *)pCSUser->pCurrentUserServiceRating);
    UserServiceRating_free(pCSUser->pCurrentUserServiceRating);
    pCSUser->pCurrentUserServiceRating = 0;
}

PRIVATE StateRet_t UserRange_open(CSParse_t * pCSParse, char * token, char demark)
{
    return StateRet_OK;
}

PRIVATE StateRet_t UserRange_get(CSParse_t * pCSParse, StateToken_t * pStateToken, char * token, char demark)
{
    CSUser_t * pCSUser = GetCSUser(pCSParse);
    UserServiceRating_t * pUserServiceRating = pCSUser->pCurrentUserServiceRating;
    Range_t * me;
    char * ptr, * backPtr;
    if (!token)
        return StateRet_WARN_NO_MATCH;
    if (Punct_badDemark(pStateToken->validPunctuation, demark))
        return StateRet_WARN_BAD_PUNCT;
	if ((me = (Range_t *) HT_CALLOC(1, sizeof(Range_t))) == NULL)
	    HT_OUTOFMEM("Range_t");
/*    me = Range_new(); */
    HTList_appendObject(pUserServiceRating->ranges, (void *)me);
    backPtr = ptr = token;
    while (*ptr) {
        if (*ptr == ':') {
            *ptr = 0;
            ptr++;
            break;
        }
        ptr++;
    }
    FVal_readVal(&me->min, backPtr);
    if (*ptr)
        FVal_readVal(&me->max, ptr);
    return StateRet_OK;
}

PRIVATE StateRet_t UserRange_close(CSParse_t * pCSParse, char * token, char demark)
{
    return StateRet_OK;
}

PRIVATE void UserRange_destroy(CSParse_t * pCSParse)
{
}

/* CSParse_doc end */
/* I T E R A T O R S - scan through the CSUser data structures for <identifier>
 */
PUBLIC CSError_t CSUser_iterateServices(CSUser_t * pCSUser, CSUser_callback_t * pIteratorCB, State_Parms_t * pParms, const char * identifier, void * pVoid)
{
    HTList * userServices;
    CSError_t ret = CSError_OK;
    int count = 0;
    if (!pIteratorCB ||
        !pCSUser ||
        !pCSUser->pCSUserData->userServices)
        return CSError_BAD_PARAM;
    userServices = pCSUser->pCSUserData->userServices;
	while (ret == CSError_OK && (pCSUser->pCurrentUserService = (UserService_t *) HTList_nextObject(userServices)))
        if (!identifier || !strcasecomp(SVal_value(&pCSUser->pCurrentUserService->rating_service), identifier)) {
            ret = (*pIteratorCB)(pCSUser, pParms, identifier, pVoid);
            count++;
        }
    if (!count)
        return CSError_SERVICE_MISSING;
    return ret;
}

PUBLIC CSError_t CSUser_iterateServiceRatings(CSUser_t * pCSUser, CSUser_callback_t * pIteratorCB, State_Parms_t * pParms, const char * identifier, void * pVoid)
{
    HTList * userServiceRatings;
    CSError_t ret = CSError_OK;
    int count = 0;
    if (!pIteratorCB ||
        !pCSUser ||
        !pCSUser->pCurrentUserService ||
        !pCSUser->pCurrentUserService->userServiceRatings)
        return CSError_BAD_PARAM;
    userServiceRatings = pCSUser->pCurrentUserService->userServiceRatings;
	while (ret == CSError_OK && (pCSUser->pCurrentUserServiceRating = (UserServiceRating_t *) HTList_nextObject(userServiceRatings)))
        if (!identifier || !strcasecomp(SVal_value(&pCSUser->pCurrentUserServiceRating->identifier), identifier)) {
            ret = (*pIteratorCB)(pCSUser, pParms, identifier, pVoid);
            count++;
        }
    if (!count)
        return CSError_RATING_MISSING;
    return ret;
}

/* L A B E L   R E Q U E S T   G E N E R A T O R S */
#define LCURLYSTR "{"
#define RCURLYSTR "}"

PRIVATE char * Completenesses[] = {"minimal", "short", "full", "signed"}; /* keep in synch with CSCompleteness_t */

PRIVATE char * Options[] = {"signed", "normal", "tree", "generic+tree"}; /* keep in synch with CSOption_t */

PUBLIC char * CSUser_acceptLabels(CSUser_t * pCSUser,
				  CSCompleteness_t completeness)
{
    HTChunk * pChunk = HTChunk_new(0x40);
    HTList * userServices = CSUser_getCSUserData(pCSUser)->userServices;
    UserService_t * pUserService;
    int serviceNo = 0; /* just needed to space out URLs */

    HTChunk_puts(pChunk, 
		 "Protocol-Request: "LCURLYSTR"PICS-1.1 "LCURLYSTR"params ");
    HTChunk_puts(pChunk, Completenesses[completeness]);
    HTChunk_puts(pChunk, " "LCURLYSTR"services ");
    while ((pUserService = (UserService_t *) HTList_nextObject(userServices))){
        if (serviceNo)
            HTChunk_puts(pChunk, " ");
        HTChunk_puts(pChunk, "\"");
        HTChunk_puts(pChunk, SVal_value(&pUserService->rating_service));
        HTChunk_puts(pChunk, "\"");
        serviceNo++;
	}
    HTChunk_puts(pChunk, RCURLYSTR RCURLYSTR RCURLYSTR"\r\n");
    return HTChunk_toCString(pChunk);
}

PUBLIC char * CSUser_getLabels(CSUser_t * pCSUser, char * url, 
			       CSOption_t option, 
			       CSCompleteness_t completeness)
{
    HTChunk * pChunk = HTChunk_new(0x40);
    HTList * userServices = CSUser_getCSUserData(pCSUser)->userServices;
    UserService_t * pUserService;
    int serviceNo = 0; /* just needed to space out URLs */
    char * escapedURL;
    char * bureau;

    if (!(bureau = CSUser_bureau(pCSUser)))
        return 0;
/*    HTChunk_puts(pChunk, "GET "); */
/*
    escapedURL = HTParse(bureau, 0, PARSE_PATH|PARSE_ANCHOR|PARSE_PUNCTUATION);
    HTChunk_puts(pChunk, escapedURL);
    HT_FREE(escapedURL);
*/
    HTChunk_puts(pChunk, bureau);
    HTChunk_puts(pChunk, "?opt=");
    HTChunk_puts(pChunk, Options[option]);
    HTChunk_puts(pChunk, "&format=");
    HTChunk_puts(pChunk, Completenesses[completeness]);
    HTChunk_puts(pChunk, "&u=");
    escapedURL = HTEscape(url, URL_XPALPHAS);
    HTChunk_puts(pChunk, escapedURL);
    HT_FREE(escapedURL);
    while ((pUserService = (UserService_t *) HTList_nextObject(userServices))){
        HTChunk_puts(pChunk, "&s=");
/*        HTChunk_puts(pChunk, "\""); */
	escapedURL = HTEscape(SVal_value(&pUserService->rating_service), 
			      URL_XPALPHAS);
        HTChunk_puts(pChunk, escapedURL);
	HT_FREE(escapedURL);
/*        HTChunk_puts(pChunk, "\""); */
        serviceNo++;
	}
/*    HTChunk_puts(pChunk, " HTTP/1.0\r\n"); */
    return HTChunk_toCString(pChunk);
}

