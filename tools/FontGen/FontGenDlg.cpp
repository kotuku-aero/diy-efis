// FontGenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FontGen.h"
#include "FontGenDlg.h"

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
// CFontGenDlg dialog

CFontGenDlg::CFontGenDlg(CWnd* pParent /*=NULL*/)
: CDialog(CFontGenDlg::IDD, pParent)
	{
	//{{AFX_DATA_INIT(CFontGenDlg)
	m_strSize = _T("");
	m_strFont = _T("");
	m_strFilename = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	}

void CFontGenDlg::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFontGenDlg)
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDC_FONTSIZE, m_cbSize);
	DDX_CBString(pDX, IDC_FONTSIZE, m_strSize);
	DDX_Text(pDX, IDC_FONT, m_strFont);
	DDX_Text(pDX, IDC_FILENAME, m_strFilename);
	//}}AFX_DATA_MAP

	m_btnOk.EnableWindow(!m_strFilename.IsEmpty());
	}

BEGIN_MESSAGE_MAP(CFontGenDlg, CDialog)
	//{{AFX_MSG_MAP(CFontGenDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_SELECT, OnSelect)
	ON_BN_CLICKED(IDC_ROTATED, OnRotated)
	ON_EN_CHANGE(IDC_FILENAME, OnChangeFilename)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontGenDlg message handlers

static LPCTSTR szFont = _T("Font");
static LPCTSTR szSize = _T("Font Size");
static LPCTSTR szFilename = _T("Filename");

static LPCTSTR szParams = _T("Params");

BOOL CFontGenDlg::OnInitDialog()
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

	m_strSize = AfxGetApp()->GetProfileString(szParams, szSize, _T("9"));
	m_strFont = AfxGetApp()->GetProfileString(szParams, szFont, _T("Arial"));
	m_strFilename = AfxGetApp()->GetProfileString(szParams, szFilename, _T(""));

	UpdateData(FALSE);

	m_cbSize.AddString(_T("9"));
	m_cbSize.AddString(_T("10"));
	m_cbSize.AddString(_T("11"));
	m_cbSize.AddString(_T("12"));

	return TRUE;  // return TRUE  unless you set the focus to a control
	}

void CFontGenDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFontGenDlg::OnPaint() 
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
HCURSOR CFontGenDlg::OnQueryDragIcon()
	{
	return (HCURSOR) m_hIcon;
	}

void CFontGenDlg::OnBrowse() 
	{
	CFileDialog dlg(TRUE);

	UpdateData();
	TCHAR szFilename[MAX_PATH];
	_tcscpy(szFilename, m_strFilename);
	dlg.m_ofn.lpstrFile = szFilename;

	if(dlg.DoModal())
		{
		m_strFilename = dlg.GetPathName();
		UpdateData(FALSE);
		}
	}

void CFontGenDlg::OnSelect() 
	{
	CFontDialog dlg;
	UpdateData();

	if(!m_strFont.IsEmpty())
		{
		_tcsncpy(dlg.m_cf.lpLogFont->lfFaceName, m_strFont, 31);
		dlg.m_cf.lpLogFont->lfFaceName[31] = 0;
		}

	if(!m_strSize.IsEmpty())
		dlg.m_cf.iPointSize = atoi(m_strSize) * 10;

	if(dlg.DoModal() == IDOK)
		{
		m_strFont = dlg.m_cf.lpLogFont->lfFaceName;
		m_strSize.Format(_T("%d"), dlg.m_cf.iPointSize / 10);

		UpdateData(FALSE);
		}
	}

#define BASE_CHARACTER ' '
#define LAST_CHARACTER 127
#define NUM_CHARACTERS ((LAST_CHARACTER +1) - BASE_CHARACTER)

void CFontGenDlg::OnOK() 
	{
	// we export 2 files.  The header and the implementation.

	// the file(s) are exported as fontdefintions
	UpdateData();

	TCHAR baseName[_MAX_PATH];

	TCHAR drive[_MAX_DRIVE];
	TCHAR path[_MAX_PATH];
	TCHAR fname[_MAX_FNAME];

	_tsplitpath(m_strFilename, drive, path, fname, NULL);
	_tmakepath(baseName, drive, path, fname, NULL);

	CString dataName = baseName;
	dataName += ".cpp";

	CStdioFile data(dataName, CFile::modeCreate | CFile::modeReadWrite);

	// calc the size of the matrix
	CWindowDC sdc(this);

	CDC dc;
	dc.CreateCompatibleDC(&sdc);

	CHAR szChars[NUM_CHARACTERS + 1];
	int n = 0;
	for(int c = BASE_CHARACTER; c < (LAST_CHARACTER + 1); n++, c++)
		szChars[n] = (CHAR)c;

	szChars[n] = 0;

	CFont font;
	font.CreatePointFont(0-(atoi(m_strSize) * 10), m_strFont);
	CFont *pOldFont = dc.SelectObject(&font);

	CSize size = dc.GetTextExtent(_T("88"), 2);

	// create a bitmap
	CBitmap bm;
	bm.CreateCompatibleBitmap(&dc, size.cx, size.cy);

	CBitmap *pOldBitmap = dc.SelectObject(&bm);

	// this is the width of each character in pixels
	int nWidths[NUM_CHARACTERS];

	// this is the offsets to each bitmap
	int nOffsets[NUM_CHARACTERS];

	// this sets how many columns for each character
	int nCharWidths[NUM_CHARACTERS];

	int nMaxWidth = 0;

	int nBytes = 0;

	COLORREF oldColor = dc.SetTextColor(0xFFFFFF);
	COLORREF oldBkColor = dc.SetBkColor(0);

	data.WriteString(_T("#include <vFont.h>\n\n"));

	data.WriteString(_T("static const unsigned char font_bitmap[] = {\n\t"));

	int byte = 0;
	// dump the bitmaps.
	for(n = 0; n < NUM_CHARACTERS; n++)
		{
		CSize w = dc.GetTextExtent(szChars + n, 1);
		nWidths[n] = w.cx;

		if(w.cx > nMaxWidth)
			nMaxWidth = w.cx;

		// this sets the number of byte columns this character will
		// occupy
		nCharWidths[n] = (((w.cx -1) | 7) +1) >> 3;

		nOffsets[n] = nBytes;
		nBytes += size.cy * nCharWidths[n];

		if(!dc.ExtTextOut(0, 0, ETO_OPAQUE, NULL, szChars +n, 1, NULL))
			{
			AfxMessageBox(_T("Cannot format the bitmap"));
			return;
			}

		for(int col = 0; col < nCharWidths[n]; col++)
			{
			for(int row = 0; row < size.cy; row++)
				{
				BYTE b = 0;
				int pel;
				int bit = 0;
				for(pel = (col << 3); bit < 8 && pel < nWidths[n]; pel++, bit++)
					{
					b <<= 1;
					COLORREF cr = dc.GetPixel(pel, row);

					if(cr != 0)
						b |= 1;
					}

				// shift the pel's
				while(bit < 8)
					{
					b <<= 1;
					bit++;
					}

				CString str;
				str.Format(_T("0x%02.2x, "), b);

				data.WriteString(str);
				byte ++;

				if((byte & 7)== 0)
					data.WriteString(_T("\n\t"));
				}
			}
		}

	dc.SetTextColor(oldColor);
	dc.SetBkColor(oldBkColor);

	data.WriteString(_T("\n};\n\n"));

	CString str;
	data.WriteString(_T("static const short font_widths[] = {\n")); 
	for(n = 0; n < NUM_CHARACTERS; n++)
		{
		str.Format(_T("\t%d, %d,\n"), nWidths[n], nOffsets[n]);
		data.WriteString(str);
		}

	data.WriteString(_T("};\n"));
	data.WriteString(_T("\n"));

	str.Format(_T("// %s %s font_t\n")
		_T("const font_t %s_font = {\n")
		_T("\tsizeof(font_t),\n")
		_T("\t%d,\n")
		_T("\t%d,\n")
		_T("\t%d,\n")
		_T("\t\' \',\n")
		_T("\tfont_bitmap,\n")
		_T("\tfont_widths,\n")
		_T("};\n"),
		LPCTSTR(m_strFont),
		LPCTSTR(m_strSize),
		fname,
		size.cy,
		BASE_CHARACTER,
		LAST_CHARACTER
		);
	data.WriteString(str);	// height of the bitmap

	data.Close();

	AfxGetApp()->WriteProfileString(szParams, szSize, m_strSize);
	AfxGetApp()->WriteProfileString(szParams, szFont, m_strFont);
	AfxGetApp()->WriteProfileString(szParams, szFilename, m_strFilename);

	CDialog::OnOK();
	}

void CFontGenDlg::OnRotated() 
	{
	UpdateData();	
	}

void CFontGenDlg::OnChangeFilename() 
	{
	UpdateData();
	}
