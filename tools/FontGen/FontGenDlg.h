// FontGenDlg.h : header file
//

#if !defined(AFX_FONTGENDLG_H__9E5802E2_A7DA_44BF_AF7D_9E0D963F213D__INCLUDED_)
#define AFX_FONTGENDLG_H__9E5802E2_A7DA_44BF_AF7D_9E0D963F213D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CFontGenDlg dialog

class CFontGenDlg : public CDialog
{
// Construction
public:
	CFontGenDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFontGenDlg)
	enum { IDD = IDD_FONTGEN_DIALOG };
	CButton	m_btnOk;
	CComboBox	m_cbSize;
	CString	m_strSize;
	CString	m_strFont;
	CString	m_strFilename;
	UINT	m_nStartAngle;
	UINT	m_nStepAngle;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontGenDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFontGenDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBrowse();
	afx_msg void OnSelect();
	virtual void OnOK();
	afx_msg void OnRotated();
	afx_msg void OnChangeFilename();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FONTGENDLG_H__9E5802E2_A7DA_44BF_AF7D_9E0D963F213D__INCLUDED_)
