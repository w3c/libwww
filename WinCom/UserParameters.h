/*
**	@(#) $Id$
**	
**	W3C Web Commander can be found at "http://www.w3.org/WinCom/"
**	
**	Copyright © 1995-1998 World Wide Web Consortium, (Massachusetts
**	Institute of Technology, Institut National de Recherche en
**	Informatique et en Automatique, Keio University). All Rights
**	Reserved. This program is distributed under the W3C's Software
**	Intellectual Property License. This program is distributed in the hope
**	that it will be useful, but WITHOUT ANY WARRANTY; without even the
**	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
**	PURPOSE. See W3C License http://www.w3.org/Consortium/Legal/ for more
**	details.
*/

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
    DECLARE_DYNCREATE(CUserParameters)
	
// Construction
public:
    CUserParameters(HTRequest * request, HTAlertOpcode op,
    	int msgnum, const char * dfault, void * input,
	HTAlertPar * reply);
    CUserParameters();
    ~CUserParameters();


    HTRequest *	    m_request;
    HTAlertOpcode   m_op;
    int		    m_msgnum;
    const char *    m_default;
    void *	    m_input;
    HTAlertPar *    m_reply;
    int		    m_type;
    char *	    m_buf;
    size_t	    m_bufLen;
    WPARAM *	    m_pWParam;
};

#endif // !defined(AFX_UserParameters_H__B7CDBADF_C4DC_11D1_93E1_080009DCA30B__INCLUDED_)
