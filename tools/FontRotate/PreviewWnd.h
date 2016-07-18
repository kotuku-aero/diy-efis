#if !defined(AFX_PREVIEWWND_H__82A21543_15C2_4A25_9D41_8B4B7E67EDBB__INCLUDED_)
#define AFX_PREVIEWWND_H__82A21543_15C2_4A25_9D41_8B4B7E67EDBB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreviewWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPreviewWnd dialog

class CPreviewWnd : public CDialog
{
// Construction
public:
	CPreviewWnd(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPreviewWnd)
	enum { IDD = IDD_PREVIEW };
	//}}AFX_DATA

	CDC *m_pDC;
	CBitmap *m_pBitmap;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreviewWnd)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSize m_sizeBitmap;
	// Generated message map functions
	//{{AFX_MSG(CPreviewWnd)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREVIEWWND_H__82A21543_15C2_4A25_9D41_8B4B7E67EDBB__INCLUDED_)
