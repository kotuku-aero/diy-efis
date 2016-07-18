// BitmapGen.h : main header file for the BITMAPGEN application
//

#if !defined(AFX_BITMAPGEN_H__3BA5A90B_D8B9_4145_8CD5_AEA935F54406__INCLUDED_)
#define AFX_BITMAPGEN_H__3BA5A90B_D8B9_4145_8CD5_AEA935F54406__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CBitmapGenApp:
// See BitmapGen.cpp for the implementation of this class
//

class CBitmapGenApp : public CWinApp
{
public:
	CBitmapGenApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitmapGenApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBitmapGenApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITMAPGEN_H__3BA5A90B_D8B9_4145_8CD5_AEA935F54406__INCLUDED_)
