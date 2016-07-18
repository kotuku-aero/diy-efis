// BitmapGenDlg.h : header file
//

#if !defined(AFX_BITMAPGENDLG_H__D1A020F0_E00A_47D9_A2F1_07C02EE639DE__INCLUDED_)
#define AFX_BITMAPGENDLG_H__D1A020F0_E00A_47D9_A2F1_07C02EE639DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CBitmapGenDlg dialog

class CBitmapGenDlg : public CDialog
{
// Construction
public:
	CBitmapGenDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBitmapGenDlg)
	enum { IDD = IDD_BITMAPGEN_DIALOG };
	CString	m_strBitmapFile;
	CString	m_strSource;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitmapGenDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CBitmapGenDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBrowseBitmap();
	afx_msg void OnBrowseSource();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITMAPGENDLG_H__D1A020F0_E00A_47D9_A2F1_07C02EE639DE__INCLUDED_)
