// FontRotateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FontRotate.h"
#include "FontRotateDlg.h"

#include "PreviewWnd.h"

#include <math.h>

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
// CFontRotateDlg dialog

static LPCTSTR szYPixels = "Y Pixels";
static LPCTSTR szXPixels = "X Pixels";
static LPCTSTR szFontSize = "Font Size";
static LPCTSTR szFontName = "Font Name";
static LPCTSTR szOutputPath = "Output";
static LPCTSTR szText = _T("Text");

CFontRotateDlg::CFontRotateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFontRotateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFontRotateDlg)
	m_nYPixels = AfxGetApp()->GetProfileInt(_T(""), szYPixels, 31);
	m_nXPixels = AfxGetApp()->GetProfileInt(_T(""), szXPixels, 31);
	m_strOutput = AfxGetApp()->GetProfileString(_T(""), szOutputPath, "");
	m_nFontSize = AfxGetApp()->GetProfileInt(_T(""), szFontSize, 9);
	m_strFontName = AfxGetApp()->GetProfileString(_T(""), szFontName, "Arial");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFontRotateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFontRotateDlg)
	DDX_Control(pDX, IDC_PREVIEW, m_wndPreview);
	DDX_Control(pDX, IDOK, m_wndOK);
	DDX_Control(pDX, IDC_YPIXELS, m_wndYPixels);
	DDX_Control(pDX, IDC_XPIXELS, m_wndXPixels);
	DDX_Control(pDX, IDC_SELECT_FONT, m_wndSelectFont);
	DDX_Control(pDX, IDC_PROGRESS, m_wndProgress);
	DDX_Control(pDX, IDC_OUTPUT, m_wndOutput);
	DDX_Control(pDX, IDC_BROWSE_FILE, m_wndBrowseFile);
	DDX_Control(pDX, IDC_ADD, m_wndAdd);
	DDX_Control(pDX, IDC_FONTNAME, m_wndFontName);
	DDX_Control(pDX, IDC_FONT_SIZE_SPIN, m_wndFontSizeSpin);
	DDX_Control(pDX, IDC_FONT_SIZE, m_wndFontSize);
	DDX_Control(pDX, IDC_CELLS, m_wndCells);
	DDX_Control(pDX, IDC_DELETE, m_wndDelete);
	DDX_Control(pDX, IDC_DOWN, m_wndDn);
	DDX_Control(pDX, IDC_UP, m_wndUp);
	DDX_Control(pDX, IDC_YPIXELS_SPIN, m_wndYPixelsSpin);
	DDX_Control(pDX, IDC_XPIXELS_SPIN, m_wndXPixelsSpin);
	DDX_Text(pDX, IDC_YPIXELS, m_nYPixels);
	DDV_MinMaxUInt(pDX, m_nYPixels, 8, 64);
	DDX_Text(pDX, IDC_XPIXELS, m_nXPixels);
	DDV_MinMaxUInt(pDX, m_nXPixels, 8, 64);
	DDX_Text(pDX, IDC_OUTPUT, m_strOutput);
	DDX_Text(pDX, IDC_FONT_SIZE, m_nFontSize);
	DDV_MinMaxUInt(pDX, m_nFontSize, 8, 36);
	DDX_Text(pDX, IDC_FONTNAME, m_strFontName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFontRotateDlg, CDialog)
	//{{AFX_MSG_MAP(CFontRotateDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SELECT_FONT, OnSelectFont)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_BN_CLICKED(IDC_BROWSE_FILE, OnBrowseFile)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CELLS, OnItemchangedCells)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_CELLS, OnBeginlabeleditCells)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_CELLS, OnEndlabeleditCells)
	ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontRotateDlg message handlers

BOOL CFontRotateDlg::OnInitDialog()
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
	
	// bind the controls.
	UpdateData(FALSE);

	m_wndFontSizeSpin.SetRange(8, 36);
	m_wndXPixelsSpin.SetRange(8, 64);
	m_wndYPixelsSpin.SetRange(8, 64);

	CRect rect;
	m_wndCells.GetClientRect(rect);

	m_wndCells.InsertColumn(0, _T("Details"), LVCFMT_LEFT, rect.Width());

	CString strText = AfxGetApp()->GetProfileString(_T(""), szText, _T(""));

	while(!strText.IsEmpty())
	{
		int pos = strText.Find('\n');
		CString txt;
		if(pos == -1)
		{
			txt = strText;
			strText.Empty();
		}
		else
		{
			txt = strText.Left(pos);
			strText = strText.Mid(pos + 1);
		}

		m_wndCells.InsertItem(m_wndCells.GetItemCount(), txt);
	}	
	
	CWindowDC myDc(this);
	m_imageDC.CreateCompatibleDC(&myDc);

	SetGraphicsMode(m_imageDC, GM_ADVANCED);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFontRotateDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFontRotateDlg::OnPaint() 
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
HCURSOR CFontRotateDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFontRotateDlg::OnSelectFont() 
{
	UpdateData();

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));

	CWindowDC dc(this);

	lf.lfHeight = -MulDiv(m_nFontSize, GetDeviceCaps(dc, LOGPIXELSY), 72);
	strncpy(lf.lfFaceName, LPCTSTR(m_strFontName), sizeof(lf.lfFaceName));

	CFontDialog dlg(&lf);

	if(dlg.DoModal()== IDOK)
	{
		m_nFontSize = -MulDiv(lf.lfHeight, 72, GetDeviceCaps(dc, LOGPIXELSY));
		m_strFontName = lf.lfFaceName;
	}

	UpdateData(FALSE);
}

void CFontRotateDlg::OnDown() 
{
	// TODO: Add your control notification handler code here
	
}

void CFontRotateDlg::OnBrowseFile() 
{
	UpdateData();
	CFileDialog dlg(FALSE, ".cpp", m_strOutput,
									OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
									"CPP Files (*.cpp)|*.cpp|Header Files (*.h)|*.h|All Files (*.*)|*.*||");

	if(dlg.DoModal())
		m_strOutput = dlg.GetPathName();
	
	UpdateData(FALSE);
}

void CFontRotateDlg::OnAdd() 
{
	int nItem = m_wndCells.InsertItem(m_wndCells.GetItemCount(), _T("New Item"));
	m_wndCells.EditLabel(nItem);
}

void CFontRotateDlg::OnItemchangedCells(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CFontRotateDlg::OnDelete() 
{
	int nItem = m_wndCells.GetSelectionMark();
	if(nItem != -1)
		m_wndCells.DeleteItem(nItem);
}

void CFontRotateDlg::OnUp() 
{
	// TODO: Add your control notification handler code here
	
}

static const double PI = 3.1415926535897932384626433832795;
inline double DegreesToRadians(double d)
{
	return d * (PI / 180);
}

void CFontRotateDlg::OnBeginlabeleditCells(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	
	*pResult = FALSE;
}

void CFontRotateDlg::OnEndlabeleditCells(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	
	*pResult = TRUE;
}

void CFontRotateDlg::OnOK() 
{
	UINT nRows = m_wndCells.GetItemCount();
	m_wndProgress.SetRange32(0, (nRows * 360));
	if(!GenerateBitmap(FALSE))
		return;

	AfxGetApp()->WriteProfileInt(_T(""), szYPixels, m_nYPixels);
	AfxGetApp()->WriteProfileInt(_T(""), szXPixels, m_nXPixels);
	AfxGetApp()->WriteProfileString(_T(""), szOutputPath, m_strOutput);
	AfxGetApp()->WriteProfileInt(_T(""), szFontSize, m_nFontSize);
	AfxGetApp()->WriteProfileString(_T(""), szFontName, m_strFontName);

	CString str;
	for(UINT item = 0; item < nRows; item++)
	{
		CString txt = m_wndCells.GetItemText(item, 0);
		str += txt;
		if(item < (nRows -1))
			str += _T("\n");
	}

	AfxGetApp()->WriteProfileString(_T(""), szText, str);

	// we have a bitmap, now we can dump the file
	CStdioFile os(m_strOutput, CFile::modeCreate | CFile::modeWrite);
	os.WriteString(_T("#include <vFont.h>\n\n"));

	os.WriteString("// Rotated font bitmap\n");
	str.Format("// Font Face : %s\n", LPCTSTR(m_strFontName));
	os.WriteString(str);
	str.Format("// Font Size : %d\n", m_nFontSize);
	os.WriteString(str);
	str.Format("// Cell X    : %d\n", m_nXPixels);
	os.WriteString(str);
	str.Format("// Cell Y    : %d\n", m_nYPixels);
	os.WriteString(str);
	os.WriteString("// Text Rows\n");

	for(UINT row = 0; row < nRows; row++)
	{
		CString txt = m_wndCells.GetItemText(row, 0);
		str.Format("// Row %2.2d : %s\n", row, LPCTSTR(txt));
		os.WriteString(str);
	}

	os.WriteString("// Bitmap has 360 degrees of rotation\n");

	// the cell sizes are fixed so we do not have to calculate
	// each one, we only need to know the char width in bytes
	UINT nCharColumns = (((m_nXPixels - 1) | 7) +1) >> 3;

	os.WriteString("static const short font_widths[] = {\n"); 

	UINT nOffset = 0;
	for(UINT ch = 0; ch < nRows; ch++)
		{
		str.Format(_T("\t%d, %d,\n"), m_nXPixels, nOffset);
		os.WriteString(str);
		nOffset += m_nYPixels * nCharColumns;
		}

	os.WriteString(_T("};\n"));
	os.WriteString(_T("\n"));

	for(int rotn = 0; rotn < 360; rotn++)
		{
		// write a new font for each angle
		str.Format("static const unsigned char font_bitmap_%d[] = {\n\t", rotn);
		os.WriteString(str);

		UINT byte = 0;

		for(UINT ch = 0; ch < nRows; ch++)
			{
			for(UINT col = 0; col < nCharColumns; col++)
				{
				for(UINT row = 0; row < m_nYPixels; row++)
					{
					BYTE b = 0;
					UINT pel;
					UINT bit = 0;
					for(pel = (col << 3); bit < 8 && pel < m_nXPixels; pel++, bit++)
						{
						b <<= 1;
						COLORREF cr = m_imageDC.GetPixel(pel + (rotn * m_nXPixels),
																						 row + (ch * m_nYPixels));

						if(cr != 0)
							b |= 1;
						}

					// shift the pel's
					while(bit < 8)
						{
						b <<= 1;
						bit++;
						}

					str.Format("0x%02.2x, ", b);
					os.WriteString(str);

					byte++;

					if((byte & 7)== 0)
						os.WriteString(_T("\n\t"));
					}
				}
			}

		os.WriteString(_T("\n};\n\n"));

		str.Format(_T("// %s %d font rotated %d degrees\n")
							 _T("static const font_t font_%d = {\n")
							 _T("\tsizeof(font_t),\n")
							 _T("\t%d,\n")
							 _T("\t%d,\n")
							 _T("\t%d,\n")
							 _T("\t0,\n")
							 _T("\tfont_bitmap_%d,\n")
							 _T("\tfont_widths,\n")
							 _T("};\n"),
							 LPCTSTR(m_strFontName),
							 m_nFontSize,
							 rotn,
							 rotn,
							 m_nYPixels,
							 1,
							 nRows + 1,
							 rotn
							 );
		os.WriteString(str);	// height of the bitmap
		}

	// now write the array
	os.WriteString("\n// font array\n"
								 "const font_t *<<name>>_fonts[] = {\n");

	for(UINT i = 0; i < 360; i++)
		{
		str.Format("\t&font_%d,\n", i);
		os.WriteString(str);
		}

	os.WriteString("\t};\n");

	os.Close();



	CDialog::OnOK();
}

BOOL CFontRotateDlg::GenerateBitmap(BOOL bReEnable)
{
	if(!UpdateData())
		return FALSE;

	// we generate n lines with 359 degrees of rotation
	int nRows = m_wndCells.GetItemCount();


	AfxGetApp()->WriteProfileInt(_T(""), szYPixels, m_nYPixels);
	AfxGetApp()->WriteProfileInt(_T(""), szXPixels, m_nXPixels);
	AfxGetApp()->WriteProfileString(_T(""), szOutputPath, m_strOutput);
	AfxGetApp()->WriteProfileInt(_T(""), szFontSize, m_nFontSize);
	AfxGetApp()->WriteProfileString(_T(""), szFontName, m_strFontName);

	CString str;
	for(int item = 0; item < nRows; item++)
		{
		CString txt = m_wndCells.GetItemText(item, 0);
		str += txt;
		if(item < (nRows -1))
			str += _T("\n");
		}

	AfxGetApp()->WriteProfileString(_T(""), szText, str);

	if(nRows == 0)
		{
		AfxMessageBox(_T("Please enter at least 1 text row"));
		return FALSE;
		}

	if(m_strOutput.IsEmpty())
		{
		AfxMessageBox(_T("Please enter a source file name"));
		return FALSE;
		}

	EnableControls(FALSE);

	m_wndProgress.ModifyStyle(0, WS_VISIBLE);

	UpdateWindow();

	CSize imageSize(360 * m_nXPixels, nRows * m_nYPixels);
	m_imageBitmap.DeleteObject();
	m_imageBitmap.CreateCompatibleBitmap(&m_imageDC, imageSize.cx, imageSize.cy);
	m_imageBitmap.SetBitmapDimension(imageSize.cx, imageSize.cy);

	m_imageDC.SelectObject(&m_imageBitmap);

	int nMapMode = m_imageDC.SetMapMode(MM_TEXT);

	CFont font;
	font.CreatePointFont(0-(m_nFontSize * 10), m_strFontName);

	CFont *pOldFont = m_imageDC.SelectObject(&font);
	COLORREF crText = m_imageDC.SetTextColor(RGB(255, 255, 255));
	COLORREF crBackground = m_imageDC.SetBkColor(RGB(0, 0, 0));

	// the printing cell is the last cell
	// we do not need row 1 (only the first cell)

	int nPos = 0;

	int row;
	for(row = 0; row < nRows; row++)
		{
		CString txt = m_wndCells.GetItemText(row, 0);
		for(int cell = 0; cell < 360; cell++)
			{
			CSize cellSize = m_imageDC.GetTextExtent(txt);

			CRect rectCell(cell * m_nXPixels, row * m_nYPixels, (cell * m_nXPixels) + m_nXPixels, (row * m_nYPixels) + m_nYPixels);
			m_imageDC.FillRect(rectCell, CBrush::FromHandle(HBRUSH(GetStockObject(BLACK_BRUSH))));

			double rads = DegreesToRadians(cell);
			// we need to set a transform on the GDI
			XFORM xform;
			xform.eM11 = (FLOAT)cos(rads);
			xform.eM22 = xform.eM11;
			xform.eM12 = (FLOAT)sin(rads);
			xform.eM21 = 0 - xform.eM12;
			xform.eDx = FLOAT((m_nXPixels >> 1) + rectCell.left);
			xform.eDy = FLOAT((m_nYPixels >> 1) + rectCell.top);

			if(!SetWorldTransform(m_imageDC, &xform))
				{
				LPVOID lpMsgBuf;
				FormatMessage( 
					FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					FORMAT_MESSAGE_FROM_SYSTEM | 
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					GetLastError(),
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
					(LPTSTR) &lpMsgBuf,
					0,
					NULL 
					);
				// Process any inserts in lpMsgBuf.
				// ...
				// Display the string.
				AfxMessageBox((LPCTSTR)lpMsgBuf,  MB_OK | MB_ICONSTOP );
				// Free the buffer.
				LocalFree( lpMsgBuf );

				return FALSE;
				}

			m_imageDC.TextOut(-(cellSize.cx >> 1),  -(cellSize.cy >> 1), txt);

			ModifyWorldTransform(m_imageDC, NULL, MWT_IDENTITY);

			m_wndProgress.SetPos(++nPos);
			UpdateWindow();

			PumpMessages();
			}
		}

	// select the old files
	m_imageDC.SelectObject(pOldFont);
	m_imageDC.SetTextColor(crText);
	m_imageDC.SetBkColor(crBackground);

	if(bReEnable)
		{
		m_wndProgress.ModifyStyle(WS_VISIBLE, 0);

		EnableControls(TRUE);
		}

	return TRUE;
}

void CFontRotateDlg::EnableControls(BOOL bEnable)
{
	m_wndAdd.EnableWindow(bEnable);
	m_wndBrowseFile.EnableWindow(bEnable);
	m_wndCells.EnableWindow(bEnable);
	m_wndDelete.EnableWindow(bEnable);
	m_wndDn.EnableWindow(bEnable);
	m_wndFontName.EnableWindow(bEnable);
	m_wndFontSize.EnableWindow(bEnable);
	m_wndFontSizeSpin.EnableWindow(bEnable);
	m_wndOK.EnableWindow(bEnable);
	m_wndOutput.EnableWindow(bEnable);
	m_wndPreview.EnableWindow(bEnable);
	m_wndSelectFont.EnableWindow(bEnable);
	m_wndUp.EnableWindow(bEnable);
	m_wndXPixels.EnableWindow(bEnable);
	m_wndXPixelsSpin.EnableWindow(bEnable);
	m_wndYPixels.EnableWindow(bEnable);
	m_wndYPixelsSpin.EnableWindow(bEnable);
}

void CFontRotateDlg::PumpMessages()
{
	MSG msg;
	while(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

void CFontRotateDlg::OnPreview() 
{
	if(!GenerateBitmap(FALSE))
		return;

	CPreviewWnd dlg;

	dlg.m_pDC = &m_imageDC;
	dlg.m_pBitmap = &m_imageBitmap;

	dlg.DoModal();

	EnableControls(TRUE);
}
