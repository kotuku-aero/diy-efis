// FontGen.h : main header file for the FONTGEN application
//

#if !defined(AFX_FONTGEN_H__859E6B78_4B40_4AAB_942F_6C006662173E__INCLUDED_)
#define AFX_FONTGEN_H__859E6B78_4B40_4AAB_942F_6C006662173E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFontGenApp:
// See FontGen.cpp for the implementation of this class
//

class CFontGenApp : public CWinApp
{
public:
	CFontGenApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontGenApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFontGenApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FONTGEN_H__859E6B78_4B40_4AAB_942F_6C006662173E__INCLUDED_)
