#include "WWWLib.h"
#include "CSLUtils.h"
#include "CSLL.h"
#include "CSLLSt.h"

/* base of all streams */
struct _HTStream {
    const HTStreamClass *	isa;
    /* ... */
};

PUBLIC BOOL ps(HTStream * pStream, const char * fmt, ...)
{
    va_list list;
    char space[257];

    va_start(list, fmt);
    vsprintf(space, fmt, list);
    va_end(list);
    return ((*pStream->isa->put_string)(pStream, space));
}

struct State_Parms_s {
    CSLabel_t * pCSLabel;
    HTStream * pStream;
    };

#define OUTPUT_OPTION_TOKEN_BVAL(str, local) \
    if (BVal_initialized(local))\
        ps(pStream, str" %s ", BVal_value(local) ? "YES" : "NO");
#define OUTPUT_OPTION_TOKEN_FVAL(str, local) \
    if (FVal_initialized(local))\
        ps(pStream, str" %.1f ", FVal_value(local));
#define OUTPUT_OPTION_TOKEN_SVAL(str, local) \
    if (SVal_initialized(local))\
        ps(pStream, str" \"%s\" ", SVal_value(local));
#define OUTPUT_OPTION_TOKEN_DVAL(str, local) \
    if (DVal_initialized(local))\
        ps(pStream, str" \"%s\" ", DVal_value(local));
PRIVATE int LabelOptions_output(LabelOptions_t * me, HTStream * pStream)
{
    HTList * comments;
    char * comment;
    OUTPUT_OPTION_TOKEN_DVAL("at", &me->at)
    OUTPUT_OPTION_TOKEN_SVAL("by", &me->by)
    OUTPUT_OPTION_TOKEN_SVAL("complete_label", &me->complete_label)
    OUTPUT_OPTION_TOKEN_SVAL("for", &me->fur)
    OUTPUT_OPTION_TOKEN_BVAL("generic", &me->generic)
    OUTPUT_OPTION_TOKEN_SVAL("MIC-md5", &me->MIC_md5)
    OUTPUT_OPTION_TOKEN_DVAL("on", &me->on)
    OUTPUT_OPTION_TOKEN_SVAL("signature-PKCS", &me->signature_PKCS)
    OUTPUT_OPTION_TOKEN_DVAL("until", &me->until)
    comments = me->comments;
    while ((comment = (char *)HTList_nextObject(comments)))
        ps(pStream, "comment \"%s\" ", comment);
    return HT_OK;
}

PRIVATE int Range_output(Range_t * pRange, HTStream * pStream)
{
    ps(pStream, "%.1f", FVal_value(&pRange->min));
    if (FVal_initialized(&pRange->max))
        ps(pStream, ":%.1f", FVal_value(&pRange->max));
    ps(pStream, " ");
    return HT_OK;
}

#define LPARENSTR "("
#define RPARENSTR ")"

PRIVATE CSLabel_callback_t CSLLOut_outputRatings;
PRIVATE CSError_t CSLLOut_outputRatings(CSLabel_t * pCSLabel, State_Parms_t * pParms, const char * identifier, void * pVoid)
{
    Range_t * pRange;
    LabelRating_t * pLabelRating = CSLabel_getLabelRating(pCSLabel);
    ps(pParms->pStream, "%s ", SVal_value(&pLabelRating->identifier));
    if (FVal_initialized(&pLabelRating->value))
        ps(pParms->pStream, "%.1f ", FVal_value(&pLabelRating->value));
    if (pLabelRating->ranges) {
        HTList * ranges = pLabelRating->ranges;
        ps(pParms->pStream, LPARENSTR);
        while ((pRange = (Range_t *) HTList_nextObject(ranges)))
            Range_output(pRange, pParms->pStream);
        ps(pParms->pStream, RPARENSTR);
    }
    return HT_OK;
}

PRIVATE CSLabel_callback_t CSLLOut_outputSingleLabel;
PRIVATE CSError_t CSLLOut_outputSingleLabel(CSLabel_t * pCSLabel, State_Parms_t * pParms, const char * identifier, void * pVoid)
{
    CSError_t ret;
    SingleLabel_t * pSingleLabel = CSLabel_getSingleLabel(pCSLabel);
    LabelOptions_output(pSingleLabel->pLabelOptions, pParms->pStream);
    ps(pParms->pStream, "r "LPARENSTR);
    ret = CSLabel_iterateLabelRatings(pParms->pCSLabel, CSLLOut_outputRatings, pParms, 0, 0);
    ps(pParms->pStream, RPARENSTR" ");
    return ret;
}

PRIVATE CSLabel_callback_t CSLLOut_outputLabel;
PRIVATE CSError_t CSLLOut_outputLabel(CSLabel_t * pCSLabel, State_Parms_t * pParms, const char * identifier, void * pVoid)
{
    CSError_t ret;
    Label_t * pLabel = CSLabel_getLabel(pCSLabel);
    if (pLabel->singleLabels)
        ps(pParms->pStream, LPARENSTR" ");
    ret =  CSLabel_iterateSingleLabels(pParms->pCSLabel, CSLLOut_outputSingleLabel, pParms, 0, 0);
    if (pLabel->singleLabels)
        ps(pParms->pStream, RPARENSTR" ");
    return ret;
}

PRIVATE CSLabel_callback_t CSLLOut_outputService;
PRIVATE CSError_t CSLLOut_outputService(CSLabel_t * pCSLabel, State_Parms_t * pParms, const char * identifier, void * pVoid)
{
    CSError_t ret;
    ServiceInfo_t * pServiceInfo = CSLabel_getServiceInfo(pCSLabel);
    if (!SVal_initialized(&pServiceInfo->rating_service)) {
        ps(pParms->pStream, "%s ", "<service error>");
	return HT_OK;
    }
    ps(pParms->pStream, "\"%s\" ", SVal_value(&pServiceInfo->rating_service));
    LabelOptions_output(pServiceInfo->pLabelOptions, pParms->pStream);
    ps(pParms->pStream, "l ");
    ret = CSLabel_iterateLabels(pParms->pCSLabel, CSLLOut_outputLabel, pParms, 0, pVoid);
    return ret;
}

PUBLIC int CSLabel_output(CSLabel_t * pCSLabel, HTStream * pStream)
{
    CSError_t ret;
    State_Parms_t parms;
    parms.pCSLabel = pCSLabel;
    parms.pStream = pStream;
    ps(parms.pStream, LPARENSTR"PICS-%.1f ", FVal_value(&CSLabel_getCSLLData(pCSLabel)->version));
	ret = CSLabel_iterateServices(parms.pCSLabel, CSLLOut_outputService, &parms, 0, 0);
    ps(parms.pStream, RPARENSTR"\n");
    return ret;
}

extern HTStream * Kwik_new (void);
extern int Kwik_free (HTStream * me);

PUBLIC void CSLabel_dump(CSLabel_t * pCSLabel)
{
    HTStream * pKwik = Kwik_new();
    CSLabel_t * pCSLabelCopy = CSLabel_copy(pCSLabel);
    CSLabel_output(pCSLabelCopy, pKwik);
    CSLabel_free(pCSLabelCopy);
    Kwik_free(pKwik);
}

