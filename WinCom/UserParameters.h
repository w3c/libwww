// UserParameters.h: interface for the CUserParameters class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UserParameters_H__B7CDBADF_C4DC_11D1_93E1_080009DCA30B__INCLUDED_)
#define AFX_UserParameters_H__B7CDBADF_C4DC_11D1_93E1_080009DCA30B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WWWLib.h"

extern HTAlertCallback UserConfirm;
extern HTAlertCallback UserPrompt;
extern HTAlertCallback UserPassword;
extern HTAlertCallback UserNameAndPassword;
extern HTAlertCallback UserProgress;
extern HTAlertCallback UserPrint;

class CUserParameters : public CObject  
{
public:
	CUserParameters(HTRequest * request, HTAlertOpcode op,
                        int msgnum, const char * dfault, void * input,
                        HTAlertPar * reply);
	virtual ~CUserParameters();

    HTRequest * request;
    HTAlertOpcode op;
    int msgnum;
    const char * dfault;
    void * input;
    HTAlertPar * reply;
    int type;
    char * buf;
    size_t bufLen;
    WPARAM * pWParam;
};

#endif // !defined(AFX_UserParameters_H__B7CDBADF_C4DC_11D1_93E1_080009DCA30B__INCLUDED_)
