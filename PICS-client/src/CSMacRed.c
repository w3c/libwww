#include "wwwsys.h"
#include "HTUtils.h"
#include "HTString.h"
#include "CSParse.h"
#include "CSMR.h"
#include "CSMRSt.h"

#define GetCSMachRead(A) ((A)->target.pCSMachRead)
#define SETNEXTSTATE(target, subState) \
  pCSParse->pTargetObject = target; \
  pCSParse->currentSubState = subState;


struct CSMachRead_s {
    CSMachReadData_t * pCSMachReadData;

    MachRead_category_t * pCurrentMachRead_category;
    MachRead_enum_t * pCurrentMachRead_enum;
    BOOL inDefault;

    MachReadTargetCallback_t * pMachReadTargetCallback;
    MRErrorHandler_t * pMRErrorHandler;
};

PRIVATE TargetObject_t MachRead_targetObject;
PRIVATE TargetObject_t Version_targetObject;
PRIVATE TargetObject_t System_targetObject;
PRIVATE TargetObject_t Service_targetObject;
PRIVATE TargetObject_t Icon_targetObject;
PRIVATE TargetObject_t Name_targetObject;
PRIVATE TargetObject_t Description_targetObject;
PRIVATE TargetObject_t Default_targetObject;
PRIVATE TargetObject_t Min_targetObject;
PRIVATE TargetObject_t Max_targetObject;
PRIVATE TargetObject_t Multi_targetObject;
PRIVATE TargetObject_t Unord_targetObject;
PRIVATE TargetObject_t Integer_targetObject;
PRIVATE TargetObject_t Labeled_targetObject;
PRIVATE TargetObject_t Category_targetObject;
PRIVATE TargetObject_t Transmit_as_targetObject;
PRIVATE TargetObject_t Enum_targetObject;
PRIVATE TargetObject_t Value_targetObject;
PRIVATE Prep_t postValueState;
PRIVATE Prep_t Category_next;
PRIVATE Prep_t Multi_setTrue;
PRIVATE Prep_t Unord_setTrue;
PRIVATE Prep_t Integer_setTrue;
PRIVATE Prep_t Labeled_setTrue;
PRIVATE Check_t Version_get;
PRIVATE Check_t System_get;
PRIVATE Check_t Service_get;
PRIVATE Check_t Icon_get;
PRIVATE Check_t Name_get;
PRIVATE Check_t Description_get;
PRIVATE Check_t Min_get;
PRIVATE Check_t Max_get;
PRIVATE Check_t Multi_get;
PRIVATE Check_t Unord_get;
PRIVATE Check_t Integer_get;
PRIVATE Check_t Labeled_get;
PRIVATE Check_t Transmit_as_get;
PRIVATE Check_t Value_get;
PRIVATE Open_t MachRead_open;
PRIVATE Open_t Category_open;
PRIVATE Open_t Default_open;
PRIVATE Open_t Enum_open;
PRIVATE Close_t MachRead_close;
PRIVATE Close_t Category_close;
PRIVATE Close_t Default_close;
PRIVATE Close_t Enum_close;
PRIVATE Destroy_t MachRead_destroy;
PRIVATE Destroy_t Category_destroy;
PRIVATE Destroy_t Default_destroy;
PRIVATE Destroy_t Enum_destroy;

PRIVATE TargetChangeCallback_t targetChangeCallback;
PRIVATE ParseErrorHandler_t parseErrorHandler;

/* CSParse_doc states */
/* M A C H I N E   R E A D A B L E   P A R S E R   S T A T E S */
/* This contains all the states in the BNF for PICS service descriptions.
 * See User/Parsing.html for details.
 */
PRIVATE StateToken_t MachRead_stateTokens[] = { 
    /* A: 1st we need a version
       B: then a system
       C: and a service
       D: an open paren
       E: could be icondef - category
       F: back from category so open or close paren
       G: open paren so it is a category */
     {           "enter", SubState_N,    Punct_ALL, 0,             0, 0, 0, &MachRead_targetObject, SubState_A, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     {"need version set", SubState_A, Punct_LPAREN, 0,             0, 0, 0, &Version_targetObject, SubState_N, 0, 0},
     { "need system set", SubState_B, Punct_LPAREN, 0,             0, 0, 0, &System_targetObject, SubState_N, 0, 0},
     {"need service set", SubState_C, Punct_LPAREN, 0,             0, 0, 0, &Service_targetObject, SubState_N, 0, 0},
     {"need options set", SubState_D, Punct_LPAREN, 0,             0, 0, 0, &MachRead_targetObject, SubState_E, 0, 0},
     {            "icon", SubState_E,  Punct_WHITE, 0,        "icon", 0, 0, &Icon_targetObject, SubState_N, 0, 0},
     {            "name", SubState_E,  Punct_WHITE, 0,        "name", 0, 0, &Name_targetObject, SubState_N, 0, 0},
     {     "description", SubState_E,  Punct_WHITE, 0, "description", 0, 0, &Description_targetObject, SubState_N, 0, 0},
     {         "default", SubState_E, Punct_LPAREN, 0,     "default", 0, 0, &Default_targetObject, SubState_N, 0, 0},
     {        "category", SubState_E, Punct_LPAREN, 0,    "category", 0, 0, &Category_targetObject, SubState_N, 0, 0},
     {   "open category", SubState_F, Punct_LPAREN, 0,             0, 0, 0, &MachRead_targetObject, SubState_G, 0, 0},
     {             "end", SubState_F, Punct_RPAREN, 0,             0, 0, 0, &MachRead_targetObject, SubState_N, Command_CLOSE, 0},
     {        "category", SubState_G, Punct_LPAREN, 0,    "category", 0, 0, &Category_targetObject, SubState_N, 0, 0}
    };
PRIVATE TargetObject_t MachRead_targetObject = {"MachRead", &MachRead_open, &MachRead_close, &MachRead_destroy, MachRead_stateTokens, raysize(MachRead_stateTokens), CSMRTC_MACHREAD};

PRIVATE StateToken_t Version_stateTokens[] = { 
    /* A: fresh Version
       B: read value */
     {"enter", SubState_N,    Punct_ALL, 0,              0, 0, 0, &Version_targetObject, SubState_A, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     { "text", SubState_A,  Punct_WHITE, 0, "PICS-version", 0, 0, &Version_targetObject, SubState_B, 0, 0},
     {"value", SubState_B, Punct_RPAREN, &Version_get,   0, 0, 0, &MachRead_targetObject, SubState_B, Command_CLOSE, 0}
    };
PRIVATE TargetObject_t Version_targetObject = {"Version", 0, 0, 0, Version_stateTokens, raysize(Version_stateTokens), CSMRTC_VERSION};

PRIVATE StateToken_t System_stateTokens[] = { 
    /* A: fresh System
       B: read value */
     {"enter", SubState_N,    Punct_ALL, 0,               0, 0, 0, &System_targetObject, SubState_A, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     { "text", SubState_A,  Punct_WHITE, 0, "rating-system", 0, 0, &System_targetObject, SubState_B, 0, 0},
     {"value", SubState_B, Punct_RPAREN, &System_get,     0, 0, 0, &MachRead_targetObject, SubState_C, Command_CLOSE, 0}
    };
PRIVATE TargetObject_t System_targetObject = {"System", 0, 0, 0, System_stateTokens, raysize(System_stateTokens), CSMRTC_SYSTEM};

PRIVATE StateToken_t Service_stateTokens[] = { 
    /* A: fresh Service
       B: read value */
     {"enter", SubState_N,    Punct_ALL, 0,                0, 0, 0, &Service_targetObject, SubState_A, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     { "text", SubState_A,  Punct_WHITE, 0, "rating-service", 0, 0, &Service_targetObject, SubState_B, 0, 0},
     {"value", SubState_B, Punct_RPAREN, &Service_get,     0, 0, 0, &MachRead_targetObject, SubState_D, Command_CLOSE, 0}
    };
PRIVATE TargetObject_t Service_targetObject = {"Service", 0, 0, 0, Service_stateTokens, raysize(Service_stateTokens), CSMRTC_SERVICE};

PRIVATE StateToken_t Icon_stateTokens[] = { 
    /* A: read value */
     {"enter", SubState_N,    Punct_ALL, 0,          0, 0, 0, &Icon_targetObject, SubState_A, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     {"value", SubState_A, Punct_RPAREN, &Icon_get, 0, 0, 0, 0, SubState_X, Command_CLOSE, &postValueState}
    };
PRIVATE TargetObject_t Icon_targetObject = {"Icon", 0, 0, 0, Icon_stateTokens, raysize(Icon_stateTokens), CSMRTC_ICON};

PRIVATE StateToken_t Name_stateTokens[] = { 
    /* A: read value */
     {"enter", SubState_N,    Punct_ALL, 0,          0, 0, 0, &Name_targetObject, SubState_A, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     {"value", SubState_A, Punct_RPAREN, &Name_get, 0, 0, 0, 0, SubState_X, Command_CLOSE, &postValueState}
    };
PRIVATE TargetObject_t Name_targetObject = {"Name", 0, 0, 0, Name_stateTokens, raysize(Name_stateTokens), CSMSRC_NAME};

PRIVATE StateToken_t Value_stateTokens[] = { 
    /* A: read value */
     {"enter", SubState_N,    Punct_ALL, 0,          0, 0, 0, &Value_targetObject, SubState_A, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     {"value", SubState_A, Punct_RPAREN, &Value_get, 0, 0, 0,  &Enum_targetObject, SubState_G, Command_CLOSE, 0}
    };
PRIVATE TargetObject_t Value_targetObject = {"Name", 0, 0, 0, Value_stateTokens, raysize(Value_stateTokens), CSMSRC_VALUE};

PRIVATE StateToken_t Description_stateTokens[] = { 
    /* A: read value */
     {"enter", SubState_N,    Punct_ALL, 0,          0, 0, 0, &Description_targetObject, SubState_A, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     {"value", SubState_A, Punct_RPAREN, &Description_get, 0, 0, 0, 0, SubState_X, Command_CLOSE, &postValueState}
    };
PRIVATE TargetObject_t Description_targetObject = {"Description", 0, 0, 0, Description_stateTokens, raysize(Description_stateTokens), CSMSRC_DESC};

PRIVATE StateToken_t Default_stateTokens[] = { 
    /* A: need an open
	   B: need option */
     {  "enter", SubState_N, Punct_ALL, 0,          0, 0, 0, &Default_targetObject, SubState_B, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     {   "open", SubState_A,             Punct_LPAREN, 0,            0, 0, 0, &Default_targetObject, SubState_B, 0, 0},
     {  "close", SubState_A,             Punct_RPAREN, 0,            0, 0, 0, &MachRead_targetObject, SubState_D, Command_CLOSE, 0},
     {    "min", SubState_B,              Punct_WHITE, 0,        "min", 0, 0, &Min_targetObject, SubState_N, 0, 0},
     {    "max", SubState_B,              Punct_WHITE, 0,        "max", 0, 0, &Max_targetObject, SubState_N, 0, 0},
     {  "multi", SubState_B, Punct_WHITE|Punct_RPAREN, 0, "multivalue", 0, 0, &Multi_targetObject, SubState_N, Command_CHAIN, 0},
     {  "unord", SubState_B, Punct_WHITE|Punct_RPAREN, 0,  "unordered", 0, 0, &Unord_targetObject, SubState_N, Command_CHAIN, 0},
     {"integer", SubState_B, Punct_WHITE|Punct_RPAREN, 0,    "integer", 0, 0, &Integer_targetObject, SubState_N, Command_CHAIN, 0},
     {"labeled", SubState_B, Punct_WHITE|Punct_RPAREN, 0, "label-only", 0, 0, &Labeled_targetObject, SubState_N, Command_CHAIN, 0},
    };
PRIVATE TargetObject_t Default_targetObject = {"Default", &Default_open, &Default_close, &Default_destroy, Default_stateTokens, raysize(Default_stateTokens), CSMRTC_DEF};

PRIVATE StateToken_t Min_stateTokens[] = { 
    /* A: read value */
     {"enter", SubState_N,    Punct_ALL,        0, 0, 0, 0, &Min_targetObject, SubState_A, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     {"value", SubState_A, Punct_RPAREN, &Min_get, 0, 0, 0, 0, SubState_X, Command_CLOSE, &postValueState}
    };
PRIVATE TargetObject_t Min_targetObject = {"Min", 0, 0, 0, Min_stateTokens, raysize(Min_stateTokens), CSMRTC_MIN};

PRIVATE StateToken_t Max_stateTokens[] = { 
    /* A: read value */
     {"enter", SubState_N,    Punct_ALL,        0, 0, 0, 0, &Max_targetObject, SubState_A, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     {"value", SubState_A, Punct_RPAREN, &Max_get, 0, 0, 0, 0, SubState_X, Command_CLOSE, &postValueState}
    };
PRIVATE TargetObject_t Max_targetObject = {"Max", 0, 0, 0, Max_stateTokens, raysize(Max_stateTokens), CSMRTC_MAX};

PRIVATE StateToken_t Multi_stateTokens[] = { 
    /* A: chained text with optional close paren
	   B: value */
     { "enter", SubState_N,    Punct_ALL, 0,            0, 0, 0, &Multi_targetObject, SubState_A, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     {"binary", SubState_A,  Punct_WHITE, 0, "multivalue", 0, 0, &Multi_targetObject, SubState_B, 0, 0},
     { "unary", SubState_A, Punct_RPAREN, 0, "multivalue", 0, 0, 0, SubState_X, Command_CLOSE, &Multi_setTrue},
     { "value", SubState_B, Punct_RPAREN, &Multi_get,   0, 0, 0, 0, SubState_X, Command_CLOSE, &postValueState}
    };
PRIVATE TargetObject_t Multi_targetObject = {"Multi", 0, 0, 0, Multi_stateTokens, raysize(Multi_stateTokens), CSMRTC_MULTI};

PRIVATE StateToken_t Unord_stateTokens[] = { 
    /* A: chained text with optional close paren
	   B: value */
     { "enter", SubState_N,    Punct_ALL, 0,           0, 0, 0, &Unord_targetObject, SubState_A, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     {"binary", SubState_A,  Punct_WHITE, 0, "unordered", 0, 0, &Unord_targetObject, SubState_B, 0, 0},
     { "unary", SubState_A, Punct_RPAREN, 0, "unordered", 0, 0, 0, SubState_X, Command_CLOSE, &Unord_setTrue},
     { "value", SubState_B, Punct_RPAREN, &Unord_get,  0, 0, 0, 0, SubState_X, Command_CLOSE, &postValueState}
    };
PRIVATE TargetObject_t Unord_targetObject = {"Unord", 0, 0, 0, Unord_stateTokens, raysize(Unord_stateTokens), CSMRTC_UNORD};

PRIVATE StateToken_t Integer_stateTokens[] = { 
    /* A: chained text with optional close paren
	   B: value */
     { "enter", SubState_N,    Punct_ALL,    0,       0, 0, 0, &Integer_targetObject, SubState_A, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     {"binary", SubState_A,  Punct_WHITE,    0, "integer", 0, 0, &Integer_targetObject, SubState_B, 0, 0},
     { "unary", SubState_A, Punct_RPAREN,    0, "integer", 0, 0, 0, SubState_X, Command_CLOSE, &Integer_setTrue},
     { "value", SubState_B, Punct_RPAREN, &Integer_get, 0, 0, 0, 0, SubState_X, Command_CLOSE, &postValueState}
    };
PRIVATE TargetObject_t Integer_targetObject = {"Integer", 0, 0, 0, Integer_stateTokens, raysize(Integer_stateTokens), CSMRTC_INT};

PRIVATE StateToken_t Labeled_stateTokens[] = { 
    /* A: chained text with optional close paren
	   B: value */
     { "enter", SubState_N,    Punct_ALL,    0,       0, 0, 0, &Labeled_targetObject, SubState_A, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     {"binary", SubState_A,  Punct_WHITE, 0, "label-only", 0, 0, &Labeled_targetObject, SubState_B, 0, 0},
     { "unary", SubState_A, Punct_RPAREN, 0, "label-only", 0, 0, 0, SubState_X, Command_CLOSE, &Labeled_setTrue},
     { "value", SubState_B, Punct_RPAREN, &Labeled_get, 0, 0, 0, 0, SubState_X, Command_CLOSE, &postValueState}
    };
PRIVATE TargetObject_t Labeled_targetObject = {"Labeled", 0, 0, 0, Labeled_stateTokens, raysize(Labeled_stateTokens), CSMRTC_LABL};

PRIVATE StateToken_t Category_stateTokens[] = { 
    /* A: 1st we need a transmit-as
       B: an open paren
       C: could be icondef - enum, category
       D: back from enum so need open or close
       E: enum or category
       F: back from category so need open or close
       G: category */
     {           "enter", SubState_N,       Punct_ALL, 0,       0, 0, 0, &Transmit_as_targetObject, SubState_N, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     {"need options set", SubState_B,    Punct_LPAREN, 0,             0, 0, 0, &Category_targetObject, SubState_C, 0, 0},
     {             "end", SubState_B,    Punct_RPAREN, 0,             0, 0, 0, 0, SubState_X, Command_CLOSE, &Category_next},
     {            "icon", SubState_C,     Punct_WHITE, 0,        "icon", 0, 0, &Icon_targetObject, SubState_N, 0, 0},
     {            "name", SubState_C,     Punct_WHITE, 0,        "name", 0, 0, &Name_targetObject, SubState_N, 0, 0},
     {     "description", SubState_C,     Punct_WHITE, 0, "description", 0, 0, &Description_targetObject, SubState_N, 0, 0},
     {             "min", SubState_C,     Punct_WHITE, 0,         "min", 0, 0, &Min_targetObject, SubState_N, 0, 0},
     {             "max", SubState_C,     Punct_WHITE, 0,         "max", 0, 0, &Max_targetObject, SubState_N, 0, 0},
     {  "multi", SubState_C, Punct_WHITE|Punct_RPAREN, 0,  "multivalue", 0, 0, &Multi_targetObject, SubState_N, Command_CHAIN, 0},
     {  "unord", SubState_C, Punct_WHITE|Punct_RPAREN, 0,   "unordered", 0, 0, &Unord_targetObject, SubState_N, Command_CHAIN, 0},
     {"integer", SubState_C, Punct_WHITE|Punct_RPAREN, 0,     "integer", 0, 0, &Integer_targetObject, SubState_N, Command_CHAIN, 0},
     {"labeled", SubState_C, Punct_WHITE|Punct_RPAREN, 0,  "label-only", 0, 0, &Labeled_targetObject, SubState_N, Command_CHAIN, 0},
     {            "enum", SubState_C,    Punct_LPAREN, 0,       "label", 0, 0, &Enum_targetObject, SubState_N, 0, 0},
     {        "category", SubState_C,    Punct_LPAREN, 0,    "category", 0, 0, &Category_targetObject, SubState_N, 0, 0},
     {"open enum or cat", SubState_D,    Punct_LPAREN, 0,             0, 0, 0, &Category_targetObject, SubState_E, 0, 0},
     {             "end", SubState_D,    Punct_RPAREN, 0,             0, 0, 0, 0, SubState_X, Command_CLOSE, &Category_next},
     {            "enum", SubState_E,    Punct_LPAREN, 0,       "label", 0, 0, &Enum_targetObject, SubState_N, 0, 0},
     {        "category", SubState_E,    Punct_LPAREN, 0,    "category", 0, 0, &Category_targetObject, SubState_N, 0, 0},
     {             "end", SubState_E,    Punct_RPAREN, 0,             0, 0, 0, 0, SubState_X, Command_CLOSE, &Category_next},
     {     "open or cat", SubState_F,    Punct_LPAREN, 0,             0, 0, 0, &Category_targetObject, SubState_G, 0, 0},
     {             "end", SubState_F,    Punct_RPAREN, 0,             0, 0, 0, 0, SubState_X, Command_CLOSE, &Category_next},
     {        "category", SubState_G,    Punct_LPAREN, 0,    "category", 0, 0, &Category_targetObject, SubState_N, 0, 0},

     {     "to MachRead", SubState_H,       Punct_ALL, 0,             0, 0, 0, &MachRead_targetObject, SubState_F, Command_CHAIN, 0}
    };
PRIVATE TargetObject_t Category_targetObject = {"Category", &Category_open, &Category_close, &Category_destroy, Category_stateTokens, raysize(Category_stateTokens), CSMRTC_CAT};

PRIVATE StateToken_t Transmit_as_stateTokens[] = { 
    /* A: fresh Service
       B: read value */
     {"enter", SubState_N,    Punct_ALL,    0,             0, 0, 0, &Transmit_as_targetObject, SubState_A, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     { "text", SubState_A,  Punct_WHITE,    0, "transmit-as", 0, 0, &Transmit_as_targetObject, SubState_B, 0, 0},
     {"value", SubState_B, Punct_RPAREN, &Transmit_as_get, 0, 0, 0, &Category_targetObject, SubState_B, Command_CLOSE, 0}
    };
PRIVATE TargetObject_t Transmit_as_targetObject = {"Transmit_as", 0, 0, 0, Transmit_as_stateTokens, raysize(Transmit_as_stateTokens), CSMRTC_TRANS};

PRIVATE StateToken_t Enum_stateTokens[] = { 
    /* A: 'label'(
	   B: 'name' -> C
	   C: (
	   D: 'description' -> E
	   D: 'value' -> G
	   E: (
	   F: 'value' -> G
	   G: ( - expect an icon
	   G: ) close
	   H: 'icon' -> I
	   I: close */
     {      "enter", SubState_N,    Punct_ALL, 0,             0, 0, 0, &Enum_targetObject, SubState_B, Command_MATCHANY|Command_OPEN|Command_CHAIN, 0},
     {       "name", SubState_B,  Punct_WHITE, 0,        "name", 0, 0, &Name_targetObject, SubState_N, 0, 0},
     {       "open", SubState_C, Punct_LPAREN, 0,             0, 0, 0, &Enum_targetObject, SubState_D, 0, 0},
     {"description", SubState_D,  Punct_WHITE, 0, "description", 0, 0, &Description_targetObject, SubState_N, 0, 0},
     {      "value", SubState_D,  Punct_WHITE, 0,       "value", 0, 0, &Value_targetObject, SubState_N, 0, 0},
     {       "open", SubState_E, Punct_LPAREN, 0,             0, 0, 0, &Enum_targetObject, SubState_F, 0, 0},
     {      "value", SubState_F,  Punct_WHITE, 0,       "value", 0, 0, &Value_targetObject, SubState_N, 0, 0},
     {       "open", SubState_G, Punct_LPAREN, 0,             0, 0, 0, &Enum_targetObject, SubState_H, 0, 0},
     {      "close", SubState_G, Punct_RPAREN, 0,             0, 0, 0, &Category_targetObject, SubState_B, Command_CLOSE, 0},
     {       "icon", SubState_H,  Punct_WHITE, 0,        "icon", 0, 0, &Icon_targetObject, SubState_N, 0, 0},
     {      "close", SubState_I, Punct_RPAREN, 0,             0, 0, 0, &Category_targetObject, SubState_B, Command_CLOSE, 0}
    };
PRIVATE TargetObject_t Enum_targetObject = {"Enum", &Enum_open, &Enum_close, &Enum_destroy, Enum_stateTokens, raysize(Enum_stateTokens), CSMRTC_ENUM};

/* CSParse_doc end */
/* S T A T E   A S S O C I A T I O N - associate a CSMachRead with the label list data 
   The label list data is kept around until all states referencing it are destroyed */
typedef struct {
    CSMachRead_t * pCSMachRead;
    CSMachReadData_t * pCSMachReadData;
    } CSMachReadAssoc_t;

PRIVATE HTList * CSMachReadAssocs = 0;

PRIVATE void CSMachReadAssoc_add(CSMachRead_t * pCSMachRead, CSMachReadData_t * pCSMachReadData)
{
    CSMachReadAssoc_t * pElement;
    if ((pElement = (CSMachReadAssoc_t *) HT_CALLOC(1, sizeof(CSMachReadAssoc_t))) == NULL)
        HT_OUTOFMEM("CSMachReadAssoc_t");
    pElement->pCSMachRead = pCSMachRead;
    pElement->pCSMachReadData = pCSMachReadData;
    if (!CSMachReadAssocs)
        CSMachReadAssocs = HTList_new();
    HTList_appendObject(CSMachReadAssocs, (void *)pElement);
}

PRIVATE CSMachReadAssoc_t * CSMachReadAssoc_findByData(CSMachReadData_t * pCSMachReadData)
{
    HTList * assocs = CSMachReadAssocs;
    CSMachReadAssoc_t * pElement;
    while ((pElement = (CSMachReadAssoc_t *) HTList_nextObject(assocs)))
        if (pElement->pCSMachReadData == pCSMachReadData)
            return pElement;
    return 0;
}

PRIVATE CSMachReadAssoc_t * CSMachReadAssoc_findByState(CSMachRead_t * pCSMachRead)
{
    HTList * assocs = CSMachReadAssocs;
    CSMachReadAssoc_t * pElement;
    while ((pElement = (CSMachReadAssoc_t *) HTList_nextObject(assocs)))
        if (pElement->pCSMachRead == pCSMachRead)
            return pElement;
    return 0;
}

PRIVATE void CSMachReadAssoc_removeByState(CSMachRead_t * pCSMachRead)
{
    CSMachReadAssoc_t * pElement = CSMachReadAssoc_findByState(pCSMachRead);
    if (!pElement)
        return;
    HTList_removeObject(CSMachReadAssocs, (void *)pElement);
    HT_FREE(pElement);
}

/* C O N S T R U C T O R S / D E S T R U C T O R S */
PRIVATE MachRead_enum_t * MachRead_enum_new(void)
{
	MachRead_enum_t * me;
	if ((me = (MachRead_enum_t *) HT_CALLOC(1, sizeof(MachRead_enum_t))) == NULL)
	    HT_OUTOFMEM("MachRead_enum_t");
    return me;
}

PUBLIC void MachRead_enum_free(MachRead_enum_t * me)
{
    SVal_clear(&me->name);
    SVal_clear(&me->description);
    FVal_clear(&me->value);
    SVal_clear(&me->icon);
}

PRIVATE MachRead_category_t * MachRead_category_new(void)
{
    MachRead_category_t * me;
    if ((me = (MachRead_category_t *) HT_CALLOC(1, sizeof(MachRead_category_t))) == NULL)
        HT_OUTOFMEM("MachRead_category_t");
    return me;
}

PUBLIC void MachRead_category_free(MachRead_category_t * me)
{
    MachRead_enum_t * pMachRead_enum;
    MachRead_category_t * pMachRead_category;
    while ((pMachRead_enum = (MachRead_enum_t *) HTList_removeLastObject(me->machRead_enums)))
        MachRead_enum_free(pMachRead_enum);
    while ((pMachRead_category = (MachRead_category_t *) HTList_removeLastObject(me->machRead_categories)))
        MachRead_category_free(pMachRead_category);
    HTList_delete(me->machRead_enums);
    HTList_delete(me->machRead_categories);
    SVal_clear(&me->transmit);
    SVal_clear(&me->icon);
    SVal_clear(&me->name);
    SVal_clear(&me->description);
    FVal_clear(&me->min);
    FVal_clear(&me->max);
    BVal_clear(&me->multi);
    BVal_clear(&me->unord);
    BVal_clear(&me->integer);
    BVal_clear(&me->labeled);
}

PUBLIC CSMachReadData_t * CSMachReadData_new(void)
{
    CSMachReadData_t * me;
    if ((me = (CSMachReadData_t *) HT_CALLOC(1, sizeof(CSMachReadData_t))) == NULL)
        HT_OUTOFMEM("CSMachReadData_t");
    me->machRead_categories = HTList_new();
    return me;
}

PUBLIC void CSMachReadData_free(CSMachReadData_t * me)
{
    MachRead_category_t * pMachRead_category;
    if (CSMachReadAssoc_findByData(me))
        return;
    while ((pMachRead_category = (MachRead_category_t *) HTList_removeLastObject(me->machRead_categories)))
        MachRead_category_free(pMachRead_category);
    FVal_clear(&me->version);
    SVal_clear(&me->system);
    SVal_clear(&me->service);
    SVal_clear(&me->icon);
    SVal_clear(&me->name);
    SVal_clear(&me->description);
    FVal_clear(&me->min);
    FVal_clear(&me->max);
    BVal_clear(&me->multi);
    BVal_clear(&me->unord);
    BVal_clear(&me->integer);
    BVal_clear(&me->labeled);
    HT_FREE(me);
}

PUBLIC CSMachRead_t * CSMachRead_new(CSMachReadData_t * pCSMachReadData, MachReadTargetCallback_t * pMachReadTargetCallback, 
								   MRErrorHandler_t * pMRErrorHandler)
{
    CSMachRead_t * me;
    if ((me = (CSMachRead_t *) HT_CALLOC(1, sizeof(CSMachRead_t))) == NULL)
        HT_OUTOFMEM("CSMachRead_t");
    me->pCSMachReadData = pCSMachReadData;
    me->pMachReadTargetCallback = pMachReadTargetCallback;
    me->pMRErrorHandler = pMRErrorHandler;
    CSMachReadAssoc_add(me, pCSMachReadData);
    return me;
}

PUBLIC CSMachRead_t * CSMachRead_copy(CSMachRead_t * old)
{
    CSMachRead_t * me;
    me = CSMachRead_new(old->pCSMachReadData, old->pMachReadTargetCallback, 
			old->pMRErrorHandler);
    memcpy(me, old, sizeof(CSMachRead_t));
    return me;
}

PUBLIC void CSMachRead_free(CSMachRead_t * me)
{
    CSMachReadData_t * pCSMachReadData = me->pCSMachReadData;
    CSMachReadAssoc_removeByState(me);
    HT_FREE(me);
    CSMachReadData_free(pCSMachReadData);
}

PUBLIC char * CSMachRead_getSystem(CSMachRead_t * me) {return SVal_value(&me->pCSMachReadData->system);}
PUBLIC char * CSMachRead_getService(CSMachRead_t * me) {return SVal_value(&me->pCSMachReadData->service);}
PUBLIC CSMachReadData_t * CSMachRead_getCSMachReadData(CSMachRead_t * me) {return me->pCSMachReadData;}
PUBLIC MachRead_category_t * CSMachRead_getMachReadCategory(CSMachRead_t * pCSMachRead) {return pCSMachRead->pCurrentMachRead_category;}
PUBLIC MachRead_enum_t * CSMachRead_getMachReadEnum(CSMachRead_t * pCSMachRead) {return pCSMachRead->pCurrentMachRead_enum;}

PUBLIC CSParse_t * CSParse_newMachRead(MachReadTargetCallback_t * pMachReadTargetCallback, 
				       MRErrorHandler_t * pMRErrorHandler)
{
    CSParse_t * me = CSParse_new();
    me->pParseContext->engineOf = &CSParse_targetParser;
    me->pParseContext->pTargetChangeCallback = &targetChangeCallback;
    me->pParseContext->pParseErrorHandler = &parseErrorHandler;
    me->target.pCSMachRead = CSMachRead_new(CSMachReadData_new(), pMachReadTargetCallback, pMRErrorHandler);
    me->pTargetObject = &MachRead_targetObject;
    me->currentSubState = SubState_N;
    return me;
}

PUBLIC CSMachRead_t * CSParse_getMachRead(CSParse_t * me)
{
    return (me->target.pCSMachRead);
}

PUBLIC BOOL CSParse_deleteMachRead(CSParse_t * pCSParse)
{
    CSMachRead_t * me = GetCSMachRead(pCSParse);
    CSMachReadData_free(CSMachRead_getCSMachReadData(me));
    CSMachRead_free(me);
    return YES;
}

/* P A R S I N G   H A N D L E R S */
PRIVATE StateRet_t targetChangeCallback(CSParse_t * pCSParse, TargetObject_t * pTargetObject, CSParseTC_t target, BOOL closed, void * pVoid)
{

    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    if (pCSMachRead->pMachReadTargetCallback)
       	return (*pCSMachRead->pMachReadTargetCallback)(pCSMachRead, pCSParse, (CSMRTC_t)target, closed, pVoid);
    return StateRet_OK;
}

PRIVATE StateRet_t parseErrorHandler(CSParse_t * pCSParse, const char * token, char demark, StateRet_t errorCode)
{
	CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
	if (pCSMachRead->pMRErrorHandler)
		return (*pCSMachRead->pMRErrorHandler)(pCSMachRead, pCSParse, token, demark, errorCode);
  return errorCode;
}

/* CSParse_doc methods */
/* P A R S I N G   S T A T E   F U N C T I O N S */
PRIVATE StateRet_t postValueState(CSParse_t * pCSParse, char * token, char demark)
{
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);

    if (pCSMachRead->inDefault) {
        SETNEXTSTATE(&Default_targetObject, SubState_A);
	return StateRet_OK;
    }
    if (pCSMachRead->pCurrentMachRead_enum) {
	if (SVal_initialized(&pCSMachRead->pCurrentMachRead_enum->icon)) {
	    SETNEXTSTATE(&Enum_targetObject, SubState_I);
	    return StateRet_OK;
	}
	if (FVal_initialized(&pCSMachRead->pCurrentMachRead_enum->value)) {
	    SETNEXTSTATE(&Enum_targetObject, SubState_G);
	    return StateRet_OK;
	}
	if (SVal_initialized(&pCSMachRead->pCurrentMachRead_enum->description)) {
	    SETNEXTSTATE(&Enum_targetObject, SubState_E);
	    return StateRet_OK;
	}
        SETNEXTSTATE(&Enum_targetObject, SubState_C);
	return StateRet_OK;
      }
    if (pCSMachRead->pCurrentMachRead_category){
	SETNEXTSTATE(&Category_targetObject, SubState_B);
	return StateRet_OK;
    }
    SETNEXTSTATE(&MachRead_targetObject, SubState_D);
    return StateRet_OK;
}

PRIVATE StateRet_t Category_next(CSParse_t * pCSParse, char * token, char demark)
{
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    if (pCSMachRead->pCurrentMachRead_category) {
        SETNEXTSTATE(&Category_targetObject, SubState_F);
	return StateRet_OK;
    }
    SETNEXTSTATE(&Category_targetObject, SubState_H);
    return StateRet_OK;
}

PRIVATE StateRet_t Multi_setTrue(CSParse_t * pCSParse, char * token, char demark)
{
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    if (pCSMachRead->pCurrentMachRead_category) {
        BVal_set(&pCSMachRead->pCurrentMachRead_category->multi, YES);
    } else {
        BVal_set(&pCSMachRead->pCSMachReadData->multi, YES);
    }
    return postValueState(pCSParse, token, demark);
}

PRIVATE StateRet_t Unord_setTrue(CSParse_t * pCSParse, char * token, char demark)
{
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    if (pCSMachRead->pCurrentMachRead_category) {
        BVal_set(&pCSMachRead->pCurrentMachRead_category->unord, YES);
    } else {
        BVal_set(&pCSMachRead->pCSMachReadData->unord, YES);
    }
    return postValueState(pCSParse, token, demark);
}

PRIVATE StateRet_t Integer_setTrue(CSParse_t * pCSParse, char * token, char demark)
{
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
	if (pCSMachRead->pCurrentMachRead_category) {
		BVal_set(&pCSMachRead->pCurrentMachRead_category->integer, YES);
	} else {
		BVal_set(&pCSMachRead->pCSMachReadData->integer, YES);
	}
	return postValueState(pCSParse, token, demark);
}

PRIVATE StateRet_t Labeled_setTrue(CSParse_t * pCSParse, char * token, char demark)
{
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
	if (pCSMachRead->pCurrentMachRead_category) {
		BVal_set(&pCSMachRead->pCurrentMachRead_category->labeled, YES);
	} else {
		BVal_set(&pCSMachRead->pCSMachReadData->labeled, YES);
	}
	return postValueState(pCSParse, token, demark);
}

PRIVATE StateRet_t Version_get(CSParse_t * pCSParse, StateToken_t * pStateToken, char * token, char demark)
{
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
	if (!FVal_readVal(&pCSMachRead->pCSMachReadData->version, token))
		return StateRet_WARN_NO_MATCH;
	return StateRet_OK;
}

PRIVATE StateRet_t System_get(CSParse_t * pCSParse, StateToken_t * pStateToken, char * token, char demark)
{
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
	if (!SVal_readVal(&pCSMachRead->pCSMachReadData->system, token))
		return StateRet_WARN_NO_MATCH;
	return StateRet_OK;
}

PRIVATE StateRet_t Service_get(CSParse_t * pCSParse, StateToken_t * pStateToken, char * token, char demark)
{
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
	if (!SVal_readVal(&pCSMachRead->pCSMachReadData->service, token))
		return StateRet_WARN_NO_MATCH;
	return StateRet_OK;
}

PRIVATE StateRet_t Icon_get(CSParse_t * pCSParse, StateToken_t * pStateToken, char * token, char demark)
{
	SVal_t * pSVal;
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    if (pCSMachRead->pCurrentMachRead_enum)
		pSVal = &pCSMachRead->pCurrentMachRead_enum->icon;
    else if (pCSMachRead->pCurrentMachRead_category)
		pSVal = &pCSMachRead->pCurrentMachRead_category->icon;
	else
		pSVal = &pCSMachRead->pCSMachReadData->icon;
	if (!SVal_readVal(pSVal, token))
		return StateRet_WARN_NO_MATCH;
	return StateRet_OK;
}

PRIVATE StateRet_t Name_get(CSParse_t * pCSParse, StateToken_t * pStateToken, char * token, char demark)
{
	SVal_t * pSVal;
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    if (pCSMachRead->pCurrentMachRead_enum)
		pSVal = &pCSMachRead->pCurrentMachRead_enum->name;
    else if (pCSMachRead->pCurrentMachRead_category)
		pSVal = &pCSMachRead->pCurrentMachRead_category->name;
	else
		pSVal = &pCSMachRead->pCSMachReadData->name;
	if (!SVal_readVal(pSVal, token))
		return StateRet_WARN_NO_MATCH;
	return StateRet_OK;
}

PRIVATE StateRet_t Description_get(CSParse_t * pCSParse, StateToken_t * pStateToken, char * token, char demark)
{
	SVal_t * pSVal;
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    if (pCSMachRead->pCurrentMachRead_enum)
		pSVal = &pCSMachRead->pCurrentMachRead_enum->description;
    else if (pCSMachRead->pCurrentMachRead_category)
		pSVal = &pCSMachRead->pCurrentMachRead_category->description;
	else
		pSVal = &pCSMachRead->pCSMachReadData->description;
	if (!SVal_readVal(pSVal, token))
		return StateRet_WARN_NO_MATCH;
	return StateRet_OK;
}

PRIVATE StateRet_t Min_get(CSParse_t * pCSParse, StateToken_t * pStateToken, char * token, char demark)
{
	FVal_t * pFVal;
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    if (pCSMachRead->pCurrentMachRead_category)
		pFVal = &pCSMachRead->pCurrentMachRead_category->min;
	else
		pFVal = &pCSMachRead->pCSMachReadData->min;
	if (!FVal_readVal(pFVal, token))
		return StateRet_WARN_NO_MATCH;
	return StateRet_OK;
}

PRIVATE StateRet_t Max_get(CSParse_t * pCSParse, StateToken_t * pStateToken, char * token, char demark)
{
	FVal_t * pFVal;
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    if (pCSMachRead->pCurrentMachRead_category)
		pFVal = &pCSMachRead->pCurrentMachRead_category->max;
	else
		pFVal = &pCSMachRead->pCSMachReadData->max;
	if (!FVal_readVal(pFVal, token))
		return StateRet_WARN_NO_MATCH;
	return StateRet_OK;
}

PRIVATE StateRet_t Multi_get(CSParse_t * pCSParse, StateToken_t * pStateToken, char * token, char demark)
{
	BVal_t * pBVal;
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    if (pCSMachRead->pCurrentMachRead_category)
		pBVal = &pCSMachRead->pCurrentMachRead_category->multi;
	else
		pBVal = &pCSMachRead->pCSMachReadData->multi;
	if (!BVal_readVal(pBVal, token))
		return StateRet_WARN_NO_MATCH;
	return StateRet_OK;
}

PRIVATE StateRet_t Unord_get(CSParse_t * pCSParse, StateToken_t * pStateToken, char * token, char demark)
{
	BVal_t * pBVal;
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    if (pCSMachRead->pCurrentMachRead_category)
		pBVal = &pCSMachRead->pCurrentMachRead_category->unord;
	else
		pBVal = &pCSMachRead->pCSMachReadData->unord;
	if (!BVal_readVal(pBVal, token))
		return StateRet_WARN_NO_MATCH;
	return StateRet_OK;
}

PRIVATE StateRet_t Integer_get(CSParse_t * pCSParse, StateToken_t * pStateToken, char * token, char demark)
{
	BVal_t * pBVal;
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    if (pCSMachRead->pCurrentMachRead_category)
		pBVal = &pCSMachRead->pCurrentMachRead_category->integer;
	else
		pBVal = &pCSMachRead->pCSMachReadData->integer;
	if (!BVal_readVal(pBVal, token))
		return StateRet_WARN_NO_MATCH;
	return StateRet_OK;
}


PRIVATE StateRet_t Labeled_get(CSParse_t * pCSParse, StateToken_t * pStateToken, char * token, char demark)
{
    BVal_t * pBVal;
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    if (pCSMachRead->pCurrentMachRead_category)
        pBVal = &pCSMachRead->pCurrentMachRead_category->labeled;
    else
        pBVal = &pCSMachRead->pCSMachReadData->labeled;
    if (!BVal_readVal(pBVal, token))
        return StateRet_WARN_NO_MATCH;
    return StateRet_OK;
}

PRIVATE StateRet_t Transmit_as_get(CSParse_t * pCSParse, StateToken_t * pStateToken, char * token, char demark)
{
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    if (!SVal_readVal(&pCSMachRead->pCurrentMachRead_category->transmit, token))
        return StateRet_WARN_NO_MATCH;
    return StateRet_OK;
}

PRIVATE StateRet_t Value_get(CSParse_t * pCSParse, StateToken_t * pStateToken, char * token, char demark)
{
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    if (!FVal_readVal(&pCSMachRead->pCurrentMachRead_enum->value, token))
        return StateRet_WARN_NO_MATCH;
    return StateRet_OK;
}

PRIVATE StateRet_t MachRead_open(CSParse_t * pCSParse, char * token, char demark)
{
    return StateRet_OK;
}

PRIVATE StateRet_t Category_open(CSParse_t * pCSParse, char * token, char demark)
{
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    MachRead_category_t * me = MachRead_category_new();
    me->pParent = pCSMachRead->pCurrentMachRead_category;
    pCSMachRead->pCurrentMachRead_category = me;
    HTList_appendObject(pCSMachRead->pCSMachReadData->machRead_categories, (void *)me);
    return StateRet_OK;
}

PRIVATE StateRet_t Default_open(CSParse_t * pCSParse, char * token, char demark)
{
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    pCSMachRead->inDefault = YES;
    return StateRet_OK;
}

PRIVATE StateRet_t Enum_open(CSParse_t * pCSParse, char * token, char demark)
{
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    MachRead_enum_t * me = MachRead_enum_new();
    pCSMachRead->pCurrentMachRead_enum = me;
    if (!pCSMachRead->pCurrentMachRead_category->machRead_enums)
        pCSMachRead->pCurrentMachRead_category->machRead_enums = HTList_new();
    HTList_appendObject(pCSMachRead->pCurrentMachRead_category->machRead_enums, (void *)me);
    return StateRet_OK;
}

PRIVATE StateRet_t MachRead_close(CSParse_t * pCSParse, char * token, char demark)
{
	return StateRet_DONE;
}

PRIVATE StateRet_t Category_close(CSParse_t * pCSParse, char * token, char demark)
{
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    pCSMachRead->pCurrentMachRead_category = pCSMachRead->pCurrentMachRead_category->pParent;
    return StateRet_OK;
}

PRIVATE StateRet_t Default_close(CSParse_t * pCSParse, char * token, char demark)
{
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    pCSMachRead->inDefault = NO;
    return StateRet_OK;
}

PRIVATE StateRet_t Enum_close(CSParse_t * pCSParse, char * token, char demark)
{
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    pCSMachRead->pCurrentMachRead_enum = 0;
    return StateRet_OK;
}

PRIVATE void MachRead_destroy(CSParse_t * pCSParse)
{
}

PRIVATE void Category_destroy(CSParse_t * pCSParse)
{
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    HTList_removeObject(pCSMachRead->pCurrentMachRead_category->machRead_categories, (void *)pCSMachRead->pCurrentMachRead_category);
    MachRead_category_free(pCSMachRead->pCurrentMachRead_category);
    pCSMachRead->pCurrentMachRead_category = 0;
}

PRIVATE void Default_destroy(CSParse_t * pCSParse)
{
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    pCSMachRead->inDefault = NO;
}

PRIVATE void Enum_destroy(CSParse_t * pCSParse)
{
    CSMachRead_t * pCSMachRead = GetCSMachRead(pCSParse);
    HTList_removeObject(pCSMachRead->pCurrentMachRead_category->machRead_enums, (void *)pCSMachRead->pCurrentMachRead_enum);
	if (!HTList_count(pCSMachRead->pCurrentMachRead_category->machRead_enums)) {
        HTList_delete(pCSMachRead->pCurrentMachRead_category->machRead_enums);
        pCSMachRead->pCurrentMachRead_category->machRead_enums = 0;
    }
    MachRead_enum_free(pCSMachRead->pCurrentMachRead_enum);
    pCSMachRead->pCurrentMachRead_enum = 0;
}

/* CSParse_doc end */
/* I T E R A T O R S - scan through the CSMachRead data structures for <identifier>
 */
PUBLIC CSError_t CSMachRead_iterateCategories(CSMachRead_t * pCSMachRead, CSMachRead_callback_t * pIteratorCB, State_Parms_t * pParms, const char * identifier, void * pVoid)
{
    HTList * machRead_categories;
    CSError_t ret = CSError_OK;
    int count = 0;
    if (!pIteratorCB ||
        !pCSMachRead ||
        !pCSMachRead->pCSMachReadData->machRead_categories)
        return CSError_BAD_PARAM;
    machRead_categories = pCSMachRead->pCSMachReadData->machRead_categories;
	while ((pCSMachRead->pCurrentMachRead_category = (MachRead_category_t *) HTList_nextObject(machRead_categories)) && ret == CSError_OK)
        if (!identifier || !strcasecomp(SVal_value(&pCSMachRead->pCurrentMachRead_category->transmit), identifier)) {
            ret = (*pIteratorCB)(pCSMachRead, pParms, identifier, pVoid) != CSError_OK;
            count++;
        }
    if (!count)
        return CSError_CATEGORY_MISSING;
    return ret;
}

PUBLIC CSError_t CSMachRead_iterateEnums(CSMachRead_t * pCSMachRead, CSMachRead_callback_t * pIteratorCB, State_Parms_t * pParms, const char * identifier, void * pVoid)
{
    HTList * machRead_enums;
    CSError_t ret = CSError_OK;
    int count = 0;
    if (!pIteratorCB ||
        !pCSMachRead ||
        !pCSMachRead->pCurrentMachRead_category ||
        !pCSMachRead->pCurrentMachRead_category->machRead_enums)
        return CSError_BAD_PARAM;
    machRead_enums = pCSMachRead->pCurrentMachRead_category->machRead_enums;
	while ((pCSMachRead->pCurrentMachRead_enum = (MachRead_enum_t *) HTList_nextObject(machRead_enums)) && ret == CSError_OK)
        if (!identifier || !strcasecomp(SVal_value(&pCSMachRead->pCurrentMachRead_enum->name), identifier)) {
            ret = (*pIteratorCB)(pCSMachRead, pParms, identifier, pVoid) != CSError_OK;
            count++;
        }
    if (!count)
        return CSError_ENUM_MISSING;
    return ret;
}

