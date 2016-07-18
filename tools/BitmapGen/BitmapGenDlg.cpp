// BitmapGenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BitmapGen.h"
#include "BitmapGenDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBitmapGenDlg dialog

static LPCTSTR szSource = _T("Bitmap File");
static LPCTSTR szDest = _T("Bitmap Destionation");

CBitmapGenDlg::CBitmapGenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBitmapGenDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBitmapGenDlg)
	m_strBitmapFile = _T("");
	m_strSource = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBitmapGenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBitmapGenDlg)
	DDX_Text(pDX, IDC_BITMAP_FILE, m_strBitmapFile);
	DDX_Text(pDX, IDC_SOURCE, m_strSource);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBitmapGenDlg, CDialog)
	//{{AFX_MSG_MAP(CBitmapGenDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BROWSE_BITMAP, OnBrowseBitmap)
	ON_BN_CLICKED(IDC_BROWSE_SOURCE, OnBrowseSource)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBitmapGenDlg message handlers

BOOL CBitmapGenDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_strBitmapFile = AfxGetApp()->GetProfileString(_T(""), szDest);
	m_strSource = AfxGetApp()->GetProfileString(_T(""), szSource);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBitmapGenDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBitmapGenDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBitmapGenDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CBitmapGenDlg::OnBrowseBitmap() 
{
	CFileDialog dlg(TRUE);

	UpdateData();
	TCHAR szFilename[MAX_PATH];
	_tcscpy(szFilename, m_strBitmapFile);
	dlg.m_ofn.lpstrFile = szFilename;

	if(dlg.DoModal())
	{
		m_strBitmapFile = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CBitmapGenDlg::OnBrowseSource() 
{
	CFileDialog dlg(TRUE);

	UpdateData();
	TCHAR szFilename[MAX_PATH];
	_tcscpy(szFilename, m_strSource);
	dlg.m_ofn.lpstrFile = szFilename;

	if(dlg.DoModal())
	{
		m_strSource = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CBitmapGenDlg::OnOK() 
{
	// the file(s) are exported as bitmap defintions
	UpdateData();

	TCHAR baseName[_MAX_PATH];

	TCHAR drive[_MAX_DRIVE];
	TCHAR path[_MAX_PATH];
	TCHAR fname[_MAX_FNAME];

	_tsplitpath(m_strBitmapFile, drive, path, fname, NULL);
	_tmakepath(baseName, drive, path, fname, NULL);

	CString dataName = baseName;
	dataName += ".cpp";

	CStdioFile data(dataName, CFile::modeCreate | CFile::modeReadWrite);

	// calc the size of the matrix
	CWindowDC sdc(this);

	CDC dc;
	dc.CreateCompatibleDC(&sdc);

	HANDLE hbm = LoadImage(NULL, m_strSource, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if(hbm == NULL)
		{
		AfxMessageBox("Cannot load the requested bitmap", MB_ICONSTOP);
		return;
		}

	CBitmap bm;
	bm.m_hObject = HBITMAP(hbm);

	BITMAP bmi;
	bm.GetBitmap(&bmi);

	dc.SelectObject(&bm);

	// emit the header
	data.WriteString("#include \"../gdi-lib/bitmap.h\"\n\n"
									 "static const color_t bitmap_bits[] = {\n");

	CString str;

	int byte = 0;
	for(int row = 0; row < bmi.bmHeight; row++)
		{
		for(int col = 0; col < bmi.bmWidth; col++)
			{
			COLORREF cr = dc.GetPixel(col, row);

			if((byte & 3) == 0)
				data.WriteString("\t");

				str.Format("0x%08.8x,", cr);
				data.WriteString(str);

				byte++;
				if((byte & 3) == 0)
					data.WriteString("\n");
				else
					data.WriteString(" ");
			}
		}

	if((byte & 3) != 0)
		data.WriteString("\n");

	data.WriteString("\t};\n\n");

	str.Format("const bitmap_t  %s_bitmap = { sizeof(bitmap_t ), 32, 0, %d, %d, bitmap_bits };\n",
						 fname, bmi.bmWidth, bmi.bmHeight);

	data.WriteString(str);
	data.Close();

	AfxGetApp()->WriteProfileString(_T(""), szDest, m_strBitmapFile);
	AfxGetApp()->WriteProfileString(_T(""), szSource, m_strSource);
	
	CDialog::OnOK();
}
