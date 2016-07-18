// FontRotateDlg.h : header file
//

#if !defined(AFX_FONTROTATEDLG_H__FB6DE93A_BEE1_4499_B789_8449AEADAA28__INCLUDED_)
#define AFX_FONTROTATEDLG_H__FB6DE93A_BEE1_4499_B789_8449AEADAA28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CFontRotateDlg dialog

class CFontRotateDlg : public CDialog
{
// Construction
public:
	CFontRotateDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFontRotateDlg)
	enum { IDD = IDD_FONTROTATE_DIALOG };
	CButton	m_wndPreview;
	CButton	m_wndOK;
	CEdit	m_wndYPixels;
	CEdit	m_wndXPixels;
	CButton	m_wndSelectFont;
	CProgressCtrl	m_wndProgress;
	CEdit	m_wndOutput;
	CButton	m_wndBrowseFile;
	CButton	m_wndAdd;
	CEdit	m_wndFontName;
	CSpinButtonCtrl	m_wndFontSizeSpin;
	CEdit	m_wndFontSize;
	CListCtrl	m_wndCells;
	CButton	m_wndDelete;
	CButton	m_wndDn;
	CButton	m_wndUp;
	CSpinButtonCtrl	m_wndYPixelsSpin;
	CSpinButtonCtrl	m_wndXPixelsSpin;
	UINT	m_nYPixels;
	UINT	m_nXPixels;
	CString	m_strOutput;
	UINT	m_nFontSize;
	CString	m_strFontName;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontRotateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFontRotateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelectFont();
	afx_msg void OnDown();
	afx_msg void OnBrowseFile();
	afx_msg void OnAdd();
	afx_msg void OnItemchangedCells(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelete();
	afx_msg void OnUp();
	afx_msg void OnBeginlabeleditCells(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditCells(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnPreview();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void EnableControls(BOOL);
	void PumpMessages();
	BOOL GenerateBitmap(BOOL bReEnable);

	CDC m_imageDC;
	CBitmap m_imageBitmap;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FONTROTATEDLG_H__FB6DE93A_BEE1_4499_B789_8449AEADAA28__INCLUDED_)
