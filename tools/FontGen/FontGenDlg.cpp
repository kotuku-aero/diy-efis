// FontGenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FontGen.h"
#include "FontGenDlg.h"
#include <compressapi.h>

#include <stdint.h>

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

static const TCHAR *defaultCharSet = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!\"#$%&'(){}*+-,./\\[]^_`:;<=>?@~| ");

/////////////////////////////////////////////////////////////////////////////
// CFontGenDlg dialog

CFontGenDlg::CFontGenDlg(CWnd* pParent /*=NULL*/)
: CDialog(CFontGenDlg::IDD, pParent)
, m_nOutputType(0)
, m_strFontSizes(_T(""))
, m_strCharSet(_T(""))
, m_strFontName(_T(""))
  {
	//{{AFX_DATA_INIT(CFontGenDlg)
	m_strSize = _T("");
	m_strFontFace = _T("");
	m_strFilename = _T("");
  m_strCharSet = defaultCharSet;
  m_nOutputType = 0;
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
  DDX_Text(pDX, IDC_FONT, m_strFontFace);
  DDX_Text(pDX, IDC_FILENAME, m_strFilename);
  DDX_Radio(pDX, IDC_C_ARRAY, m_nOutputType);
  DDV_MinMaxInt(pDX, m_nOutputType, 0, 2);
  //}}AFX_DATA_MAP

  m_btnOk.EnableWindow(!m_strFilename.IsEmpty());
  DDX_Text(pDX, IDC_CHARACTER_SET, m_strCharSet);
  DDX_Text(pDX, IDC_FONT_NAME, m_strFontName);
  DDV_MaxChars(pDX, m_strFontName, 16);
  DDX_Control(pDX, IDC_FONTSIZES, m_lbFontSizes);

  m_sizes.RemoveAll();

  int count = m_lbFontSizes.GetCount();
  for(int i = 0; i < count; i++)
    {
    CString item;
    m_lbFontSizes.GetText(i, item);
    
    m_sizes.Add(atoi(item));
    }

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
  ON_LBN_SELCHANGE(IDC_FONTSIZES, &CFontGenDlg::OnLbnSelchangeFontsizes)
  ON_BN_CLICKED(IDC_ADD, &CFontGenDlg::OnBnClickedAdd)
  ON_BN_CLICKED(IDC_REMOVE, &CFontGenDlg::OnBnClickedRemove)
  ON_CBN_SELCHANGE(IDC_FONTSIZE, &CFontGenDlg::OnCbnSelchangeFontsize)
  ON_EN_CHANGE(IDC_CHARACTER_SET, &CFontGenDlg::OnEnChangeCharacterSet)
  ON_BN_CLICKED(IDC_DEFAULT_SET, &CFontGenDlg::OnBnClickedDefaultSet)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontGenDlg message handlers

static LPCTSTR szFont = _T("Font");
static LPCTSTR szName = _T("Name");
static LPCTSTR szFilename = _T("Filename");
static LPCTSTR szItalic = _T("Italic");
static LPCTSTR szUnderline = _T("Underline");
static LPCTSTR szWeight = _T("Weight");
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

	m_strFontFace = AfxGetApp()->GetProfileString(szParams, szFont, _T("Neo Sans Arabic"));
  m_strFontName = AfxGetApp()->GetProfileString(szParams, szName, _T("Neo"));
	m_strFilename = AfxGetApp()->GetProfileString(szParams, szFilename, _T(""));
  m_bItalic = AfxGetApp()->GetProfileIntA(szParams, szItalic, 0);
  m_bUnderline = AfxGetApp()->GetProfileIntA(szParams, szUnderline, 0);
  m_nFontWeight = AfxGetApp()->GetProfileIntA(szParams, szWeight, 0);

	UpdateData(FALSE);

  m_cbSize.AddString(_T("5"));
  m_cbSize.AddString(_T("6"));
  m_cbSize.AddString(_T("7"));
  m_cbSize.AddString(_T("8"));
  m_cbSize.AddString(_T("9"));
  m_cbSize.AddString(_T("10"));
  m_cbSize.AddString(_T("11"));
  m_cbSize.AddString(_T("12"));
  m_cbSize.AddString(_T("13"));
  m_cbSize.AddString(_T("14"));
  m_cbSize.AddString(_T("15"));
  m_cbSize.AddString(_T("16"));
  m_cbSize.AddString(_T("17"));
  m_cbSize.AddString(_T("18"));
  m_cbSize.AddString(_T("19"));
  m_cbSize.AddString(_T("20"));
  m_cbSize.AddString(_T("21"));
  m_cbSize.AddString(_T("22"));
  m_cbSize.AddString(_T("23"));
  m_cbSize.AddString(_T("24"));

  m_lbFontSizes.AddString("9");
  m_lbFontSizes.AddString("12");
  m_lbFontSizes.AddString("15");
  m_lbFontSizes.AddString("18");

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
  dlg.m_cf.Flags |= CF_NOSCRIPTSEL;
  dlg.m_cf.Flags |= CF_NOSIZESEL;
  dlg.m_cf.Flags |= CF_SCALABLEONLY;
	UpdateData();

	if(!m_strFontFace.IsEmpty())
		{
		_tcsncpy(dlg.m_cf.lpLogFont->lfFaceName, m_strFontFace, 31);
		dlg.m_cf.lpLogFont->lfFaceName[31] = 0;
		}

	if(!m_strSize.IsEmpty())
		dlg.m_cf.iPointSize = atoi(m_strSize) * 10;

	if(dlg.DoModal() == IDOK)
		{
		m_strFontFace = dlg.m_cf.lpLogFont->lfFaceName;
    m_bItalic = dlg.m_cf.lpLogFont->lfItalic;
    m_bUnderline = dlg.m_cf.lpLogFont->lfUnderline;
    m_nFontWeight = dlg.m_cf.lpLogFont->lfWeight;

		UpdateData(FALSE);
		}
	}

void CFontGenDlg::OnOK() 
	{
	// we export 2 files.  The header and the implementation.

	// the file(s) are exported as fontdefintions
	UpdateData();

  // HACK.
  m_sizes.Add(9);
  m_sizes.Add(12);
  m_sizes.Add(15);
  m_sizes.Add(18);

  if(GenerateFontFile())
    {
    TCHAR baseName[_MAX_PATH];

    TCHAR drive[_MAX_DRIVE];
    TCHAR path[_MAX_PATH];
    TCHAR fname[_MAX_FNAME];

    _tsplitpath(m_strFilename, drive, path, fname, NULL);
    _tmakepath(baseName, drive, path, fname, NULL);

    CString dataName = baseName;

    switch(m_nOutputType)
      {
      case 0:
        dataName += ".c";
        WriteCOutputFile(dataName);
        break;
      case 1:
        dataName += ".txt";
        WriteBase64OutputFile(dataName);
        break;
      case 2:
        dataName += ".fon";
        WriteBinaryOutputFile(dataName);
        break;
      }

    AfxGetApp()->WriteProfileString(szParams, szName, m_strFontName);
    AfxGetApp()->WriteProfileString(szParams, szFont, m_strFontFace);
    AfxGetApp()->WriteProfileInt(szParams, szItalic, m_bItalic);
    AfxGetApp()->WriteProfileInt(szParams, szUnderline, m_bUnderline);
    AfxGetApp()->WriteProfileInt(szParams, szWeight, m_nFontWeight);
    AfxGetApp()->WriteProfileString(szParams, szFilename, m_strFilename);
    }

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

class CharMap {
public:
  UINT16 start;
  UINT16 end;
  CArray<UINT16> glyphOffsets;

  CharMap()
    {
    start = 0;
    end = 0;
    }

  CharMap &operator =(const CharMap &other)
    {
    start = other.start;
    end = other.end;
    glyphOffsets.RemoveAll();
    glyphOffsets.Append(other.glyphOffsets);
    return *this;
    }
  };

class CSortCharArray : public CArray <TCHAR>
  {
  static int Compare(const void *, const void *);
public:
  void Sort()
    {
    qsort(GetData(), GetSize(), sizeof(TCHAR), Compare);
    };
  TCHAR * Search(TCHAR i)
    {
    return (TCHAR *)bsearch(&i, GetData(), GetSize(), sizeof(TCHAR), Compare);
    };
  };

int CSortCharArray::Compare(const void *arg1, const void *arg2)
  {
  TCHAR int1 = *(TCHAR *)arg1;
  TCHAR int2 = *(TCHAR *)arg2;

  if(int1 < int2)
    return -1;
  else
    if(int1 > int2)
      return +1;
  return 0;
  }

// variable length..
struct glyph_t {
  uint8_t advance;           // advance for the glyph
  uint8_t baseline;          // baseline of the bitmap.
  uint8_t offset;            // offset to first column of bitmap
  uint8_t width;             // width of the bitmap
  uint8_t height;            // height of the bitmap
  uint8_t pixels[];
  };


BOOL CFontGenDlg::GenerateFontFile()
  {

  // calc the size of the matrix
  CWindowDC sdc(this);

  CDC dc;
  dc.CreateCompatibleDC(&sdc);

  // this sets how many columns for each character
  CArray<int> charWidths;

  int nMaxWidth = 0;

  int nBytes = 0;

  // A raster font stream is defined as the following format
  //
  // uint32_t magic	                  // can be:
                                      // FONT - non compressed binary font image
                                      // CFNT - compressed binary image
  // char name[REG_NAME_MAX]          // name of the font. (16 chars)
  // uint16_t file_length;            // un-compressed file length
  // uint8_t num_fonts               // number of fixed size fonts
  // uint8_t reserved[9]
  // the following record is repeated for num_fonts
  // -- if the file type is CFNT then all that remains is compressed ---
  // uint16_t record_size;            // length of this font record.
  // uint8_t size;                   // height of the font this bitmap renders
  // uint8_t vertical_height;        // height including ascender/descender
  // uint8_t baseline;               // where logical 0 is for the font outline.
  // uint8_t num_maps                // number of character maps
  // the character maps then continue for the num_maps
  // uint8_t start_char              // first character in the character map
  // uint8_t last_char               // last character in the character map
  // uint16_t glyphs_offset[]         // offset to the glyph records (offset from start of the block)
  // each glyph is indexed based on this number
  // The glyphs then follow in the following format
  // uint8_t glyph_advance           // horizontal advance for the glyph
  // uint8_t glyph_baseline          // baseline of the bitmap, is aligned to the baseline when rendered
  // uint8_t glyph_offset;           // offset to col 0 of the glyph
  // uint8_t width                   // width of the actual glyph
  // uint8_t height                  // height of the glyph
  // uint8_t bitmap[width * height]   // alpha values of the bitmap
  // ----- End of deflated record

  UINT16 numFonts = m_sizes.GetCount();

  CSortCharArray chars;

  for(int c = 0; c < m_strCharSet.GetLength(); c++)
    chars.Add(m_strCharSet[c]);

  // sort the array
  chars.Sort();

  CArray<CharMap> charMaps;
  CharMap nextMap;
  nextMap.start = chars[0];
  nextMap.end = chars[0];

  // set the initial offset
  uint16_t glyphOffset = 8;

  for(int c = 1; c < chars.GetSize(); c++)
    {
    if(nextMap.start + c != chars[c])
      {
      // add the size of this map
      glyphOffset += 2 + ((nextMap.end - nextMap.start + 1) << 1);
      charMaps.Add(nextMap);
      nextMap.start = chars[c];
      nextMap.end = chars[c];
      }
    else
      nextMap.end = chars[c];
    }

  // add the last one
  charMaps.Add(nextMap);
  // add the size of this map
  glyphOffset += 2 + ((nextMap.end - nextMap.start + 1) << 1);

  // now adjust the offset to a 16 byte boundary
  glyphOffset = ((glyphOffset - 1) | 15) + 1;

  CArray<UINT8> fontRec;      // built font record.
  CArray<glyph_t *> glyphs;
  CArray<UINT8> outRec;       // buffer that can me compressed

  for(UINT16 fontNum = 0; fontNum < numFonts; fontNum++)
    {
    fontRec.RemoveAll();
    charMaps[0].glyphOffsets.RemoveAll();

    CFont fnt;
    fnt.CreateFont(m_sizes[fontNum], 0, 0, 0, m_nFontWeight, m_bItalic, m_bUnderline,
      0, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE | DEFAULT_PITCH,
      m_strFontFace);
    //fnt.CreatePointFont(m_sizes[fontNum] * 10, m_strFontFace);
    dc.SelectObject(fnt);

    OUTLINETEXTMETRIC otm;
    GetOutlineTextMetrics(dc, sizeof(OUTLINETEXTMETRIC), &otm);

    CSize fontBox(otm.otmTextMetrics.tmMaxCharWidth, otm.otmTextMetrics.tmHeight);

    // create a bitmap
    CBitmap bm;
    bm.CreateCompatibleBitmap(&dc, fontBox.cx, fontBox.cy);

    // white is the foreground color
    dc.SelectObject(&bm);
    dc.SetTextColor(0xFFFFFF);
    dc.SetBkColor(0);

    uint16_t currentGlyphOffset = glyphOffset;

    // build the array of variable length glyphs based on the charmaps
    int charMap = 0;
    for(int glyph = 0; glyph < chars.GetSize(); glyph++)
      {
      TCHAR ch[2] = { chars[glyph], 0 };
      CSize w = dc.GetTextExtent(ch, 1);
      if(charMaps[charMap].end < chars[glyph])
        {
        // next map
        charMap++;
        charMaps[charMap].glyphOffsets.RemoveAll();
        }

      uint16_t advance = w.cx;
      uint16_t x_offset = 0;
      uint16_t y_offset = 0;
      uint16_t stride = 0;

      uint16_t numBytes = sizeof(glyph_t);

      // store where we are
      charMaps[charMap].glyphOffsets.Add(currentGlyphOffset);

      if (ch[0] != ' ')
        {
        for (int row = 0; row < w.cy; row++)
          {
          for (int col = 0; col < w.cx; col++)
            {
            dc.SetPixel(col, row, 0);
            }
          }

        if (!dc.ExtTextOut(0, 0, 0, NULL, ch, 1, NULL))
          {
          AfxMessageBox(_T("Cannot render the bitmap"));
          return FALSE;
          }

        bool isBlank;

        // Scan the generated bitmap.  Columns that are empty
        // to the left are ignored
        for (int col = 0; col < w.cx; col++)
          {
          isBlank = true;
          for (int row = 0; row < w.cy; row++)
            {
            if (dc.GetPixel(col, row) != 0)
              {
              isBlank = false;
              break;
              }
            }

          if (!isBlank)
            break;

          x_offset++;
          }

        // Scan the generated bitmap.  Columns that are empty
        // to the right are ignored
        for (int col = w.cx; col > 0; col--)
          {
          isBlank = true;
          for (int row = 0; row < w.cy; row++)
            {
            if (dc.GetPixel(col - 1, row) != 0)
              {
              isBlank = false;
              break;
              }
            }

          if (!isBlank)
            break;

          w.cx--;
          }

        // Scan the bitmap for empty rows
        for (int row = 0; row < w.cy; row++)
          {
          isBlank = true;
          for (int col = x_offset; col < w.cx; col++)
            {
            if (dc.GetPixel(col, row) != 0)
              {
              isBlank = false;
              break;
              }
            }

          if (!isBlank)
            break;
          y_offset++;
          }

        // Scan the bitmap for empty rows
        for (int row = w.cy; row > y_offset; row--)
          {
          isBlank = true;
          for (int col = x_offset; col < advance; col++)
            {
            if (dc.GetPixel(col, row - 1) != 0)
              {
              isBlank = false;
              break;
              }
            }

          if (!isBlank)
            break;

          w.cy--;
          }

        // remove the offset from the width
        w.cy -= y_offset;
        w.cx -= x_offset;

        stride = (((w.cx - 1) | 7) + 1) >> 3;

        numBytes += stride * w.cy;
        }

      // roung the glyph to the nearest page
      numBytes = ((numBytes - 1) | 15) + 1;
      glyph_t *pGlyph = (glyph_t *)malloc(numBytes);

      // round to 16 byte boundary
      currentGlyphOffset += numBytes;

      memset(pGlyph, 0, numBytes);

      glyphs.Add(pGlyph);
      pGlyph->advance = advance;
      pGlyph->baseline = otm.otmTextMetrics.tmAscent;

      if (ch[0] != ' ')
        {
        // remove the rows at the top that are blank.
        pGlyph->baseline -= y_offset;

        pGlyph->width = w.cx;
        pGlyph->height = w.cy;

        for (int row = 0; row < w.cy; row++)
          {
          for (int col = 0; col < w.cx; col += 8)
            {
            // raster-font
            BYTE pixel = 0;
            int bit;
            for (bit = 0; bit < 8 && (col + bit) < w.cx; bit++)
              {
              pixel <<= 1;
              if (dc.GetPixel(col + bit + x_offset, row + y_offset) != 0)
                pixel |= 1;
              }

            // shift the pel's
            while (bit < 8)
              {
              pixel <<= 1;
              bit++;
              }
            pGlyph->pixels[(col >> 3) + (row * stride)] = pixel;
            }
          }
        }
      
#ifdef _DEBUG_FONT
      {
      static TCHAR buf[256];
      snprintf(buf, 256, "Character 0x%02.2x\r\n", ch[0]);
      OutputDebugString(buf);
      snprintf(buf, 256, "Offset : %d\r\n", currentGlyphOffset - numBytes);
      OutputDebugString(buf);
      snprintf(buf, 256, "-------------------\r\n");
      OutputDebugString(buf);
      for(int row = 0; row < w.cy; row++)
        {
        for(int col = 0; col < w.cx; col += 8)
          {
          snprintf(buf, 256, "0x%02.2x ", pGlyph->pixels[(col >> 3) + (row * stride)]);
          OutputDebugString(buf);
          }
        snprintf(buf, 256, "\r\n");
        OutputDebugString(buf);
        }
      snprintf(buf, 256, "-------------------\r\n");
      OutputDebugString(buf);
      }
#endif

      }

    // uint8_t size;                   // height of the font this bitmap renders
    fontRec.Add(m_sizes[fontNum]);
    // uint8_t vertical_height;        // height including ascender/descender
    fontRec.Add(fontBox.cy);
    // uint8_t baseline;               // we assume the baseline is same as the height - could be wrong
    fontRec.Add(otm.otmTextMetrics.tmAscent);
    // uint8_t num_maps                // number of character maps
    fontRec.Add(charMaps.GetSize());
    // Reserved
    fontRec.Add(0);
    fontRec.Add(0);

    int byte = 0;
    // dump the bitmaps.
    for(int n = 0; n < charMaps.GetSize(); n++)
      {
      CharMap &map = charMaps[n];

      // uint_t start_char              // first character in the character map
      fontRec.Add(map.start);
      // uint8_t last_char               // last character in the character map
      fontRec.Add(map.end);

      // uint16_t glyphs_offset           // offset to the glyph records (offset from start of the block)
      uint16_t numGlyphs = map.glyphOffsets.GetSize();
      ASSERT(numGlyphs == (map.end - map.start + 1));
      for(int i = 0; i < numGlyphs; i++)
        {
        uint16_t offset = map.glyphOffsets[i];
        fontRec.Add(offset >> 8);
        fontRec.Add(offset);
        }
      }

    // adjust to 16 byte boundary
    uint16_t pos = fontRec.GetSize();
    pos += 2;       // adjust for the 2 bytes added when the record length is added

    while ((pos & 0x0f) > 0)
      {
      fontRec.Add(0);
      pos++;
      }

    // dump the glyphs
    for(int n = 0; n < glyphs.GetSize(); n++)
      {
      glyph_t *pGlyph = glyphs[n];
      // uint8_t glyph_advance           // horizontal advance for the glyph
      fontRec.Add(pGlyph->advance);
      // uint8_t glyph_baseline          // baseline of the bitmap, is aligned to the baseline when rendered
      fontRec.Add(pGlyph->baseline);
      // uint8_t offset                  // offset to column 0
      fontRec.Add(pGlyph->offset);
      // uint8_t width                   // width of the actual glyph
      fontRec.Add(pGlyph->width);
      // uint8_t height                  // height of the glyph
      fontRec.Add(pGlyph->height);

      int recLen = 5;
      uint16_t stride = (((pGlyph->width - 1) | 7) + 1) >> 3;
      // uint8_t bitmap[width * height]   // alpha values of the bitmap
      for(int row = 0; row < pGlyph->height; row++)
        {
        for(int col = 0; col < pGlyph->width; col+=8)
          {
          fontRec.Add(pGlyph->pixels[(col >> 3) + (row * stride)]);
          recLen++;
          }
        }

      int pad;
      for (pad = recLen; pad < (((recLen - 1) | 15) + 1); pad++)
        fontRec.Add(0);     // pad to 16 byte boundary

      // free the glyph
      free(pGlyph);
      }

    glyphs.RemoveAll();

    uint16_t len = fontRec.GetSize();
    len += 2;

    // uint16_t record_size;            // length of this font record.
    outRec.Add(len >> 8);
    outRec.Add(len);

    outRec.Append(fontRec);       // uncompressed font file
    }

  if(m_nOutputType == 0)
    {
    m_fontFile.Add('F');
    m_fontFile.Add('O');
    m_fontFile.Add('N');
    m_fontFile.Add('T');
    }
  else
    {
    m_fontFile.Add('C');
    m_fontFile.Add('F');
    m_fontFile.Add('N');
    m_fontFile.Add('T');
    }

  // add the uncompressed file name.
  int i;
  for(i = 0; i < m_strFontName.GetLength() && i < 16; i++)
    {
    m_fontFile.Add(m_strFontName[i]);
    }

  while(i < 16)
    {
    m_fontFile.Add(0x00);
    i++;
    }

  uint32_t fileLength = outRec.GetSize();
  fileLength += 32;
  if (fileLength > 65535)
    {
    AfxMessageBox(_T("The generated font file exceeds the maximumm size.  Must be < 65535 bytes.  Remove pixel sizes or characters"));
    return FALSE;
    }


  m_fontFile.Add(fileLength >> 8);
  m_fontFile.Add(fileLength);

  m_fontFile.Add(numFonts);

  // reserved bytes (9)
  m_fontFile.Add(0);
  m_fontFile.Add(0);
  m_fontFile.Add(0);
  m_fontFile.Add(0);
  m_fontFile.Add(0);
  m_fontFile.Add(0);
  m_fontFile.Add(0);
  m_fontFile.Add(0);
  m_fontFile.Add(0);

  if(m_nOutputType == 0)
    m_fontFile.Append(outRec);            // binary file.
  else
    {
    // calculate the compressed size.
    COMPRESSOR_HANDLE Compressor = NULL;
    PBYTE CompressedBuffer = NULL;
    SIZE_T CompressedDataSize, CompressedBufferSize;

    BOOL Success = CreateCompressor(
      COMPRESS_ALGORITHM_XPRESS_HUFF, //  Compression Algorithm
      NULL,                           //  Optional allocation routine
      &Compressor);                   //  Handle
                                      //  Query compressed buffer size.
    Success = Compress(
      Compressor,                  //  Compressor Handle
      outRec.GetData(),        //  Input buffer, Uncompressed data
      outRec.GetSize(),         //  Uncompressed data size
      NULL,                        //  Compressed Buffer
      0,                           //  Compressed Buffer size
      &CompressedBufferSize);      //  Compressed Data size

                                    //  Allocate memory for compressed buffer.
    if(!Success)
      {
      DWORD ErrorCode = GetLastError();

      if(ErrorCode != ERROR_INSUFFICIENT_BUFFER)
        {
        AfxMessageBox(_T("Cannot compress font record"));
        return FALSE;
        }

      CompressedBuffer = (PBYTE)malloc(CompressedBufferSize);
      if(!CompressedBuffer)
        {
        AfxMessageBox(_T("Cannot allocate memory for compressed buffer.\n"));
        return FALSE;
        }
      }

    //  Call Compress() again to do real compression and output the compressed
    //  data to CompressedBuffer.
    Success = Compress(
      Compressor,             //  Compressor Handle
      outRec.GetData(),        //  Input buffer, Uncompressed data
      outRec.GetSize(),         //  Uncompressed data size
      CompressedBuffer,       //  Compressed Buffer
      CompressedBufferSize,   //  Compressed Buffer size
      &CompressedDataSize);   //  Compressed Data size

    if(!Success)
      {
      AfxMessageBox(_T("Cannot compress data: %d\n"), GetLastError());
      return FALSE;
      }

    // append the compressed record.
    for(SIZE_T n = 0; n < CompressedDataSize; n++)
      m_fontFile.Add(CompressedBuffer[n]);

    free(CompressedBuffer);
    }

  return TRUE;
  }

BOOL CFontGenDlg::WriteCOutputFile(CString &dataName)
  {
  CStdioFile data(dataName, CFile::modeCreate | CFile::modeWrite);

  data.WriteString("#include <stdint.h>\n");
  data.WriteString("/* autogenerated file.  Do not edit\n");
  data.WriteString("Font name: ");
  data.WriteString(m_strFontName);
  data.WriteString("\nCharacter set : ");
  data.WriteString(m_strCharSet);
  data.WriteString("\nPixel sizes : ");

  for (int i = 0; i < m_sizes.GetSize(); i++)
    {
    TCHAR buffer[64];
    data.WriteString(itoa(m_sizes[i], buffer, 10));
    data.WriteString(" ");
    }
  
  data.WriteString("\n*/\n\n");
  data.WriteString("const uint8_t ");
  data.WriteString(m_strFontName);
  data.WriteString("[] = {\n");

  TCHAR hexBuf[16];

  for(int bo = 0; bo < m_fontFile.GetSize(); bo++)
    {
    if((bo & 0x0f) == 0)
      data.WriteString("\n");

    _stprintf(hexBuf, _T("0x%02.2x, "), m_fontFile[bo]);
    data.WriteString(hexBuf);
    }

  data.WriteString("};\n");

  data.WriteString("const uint16_t ");
  data.WriteString(m_strFontName);
  data.WriteString("_length = ");
  _stprintf(hexBuf, _T("%d"), m_fontFile.GetSize());
  data.WriteString(hexBuf);
  data.WriteString(";\n");

  data.Close();
  return TRUE;
  }

#include <wincrypt.h>

BOOL CFontGenDlg::WriteBase64OutputFile(CString &dataName)
  {
  CStdioFile data(dataName, CFile::modeCreate | CFile::modeWrite);

  DWORD len;
  if(CryptBinaryToString(m_fontFile.GetData(), m_fontFile.GetSize(), CRYPT_STRING_BASE64, NULL, &len))
    {
    TCHAR *file = (TCHAR *) malloc(len * sizeof(TCHAR));

    if(CryptBinaryToString(m_fontFile.GetData(), m_fontFile.GetSize(), CRYPT_STRING_BASE64, file, &len))
      data.WriteString(file);

    free(file);
    }
  data.Close();
  return TRUE;
  }

BOOL CFontGenDlg::WriteBinaryOutputFile(CString &dataName)
  {
  CFile data(dataName, CFile::modeCreate | CFile::modeWrite);

  data.Write(m_fontFile.GetData(), m_fontFile.GetSize());

  data.Close();
  return TRUE;
  }

void CFontGenDlg::OnLbnSelchangeFontsizes()
  {
  // TODO: Add your control notification handler code here
  }


void CFontGenDlg::OnBnClickedAdd()
  {
  UpdateData();
  if(m_strSize.GetLength() > 0)
    {
    int count = m_lbFontSizes.GetCount();
    for(int i = 0; i < count; i++)
      {
      CString item;
      m_lbFontSizes.GetText(i, item);
      if(item == m_strSize)
        return;
      }

    m_lbFontSizes.AddString(m_strSize);
    }
  }


void CFontGenDlg::OnBnClickedRemove()
  {
  UpdateData();
  int count = m_lbFontSizes.GetSelCount();
  if(count < 1)
    return;

  CArray< int, int > arrayListSel;
  arrayListSel.SetSize(count);

  m_lbFontSizes.GetSelItems(count, arrayListSel.GetData());

  for(; count > 0; count--)
    m_lbFontSizes.DeleteString(arrayListSel[count - 1]);
  }


void CFontGenDlg::OnCbnSelchangeFontsize()
  {
  // TODO: Add your control notification handler code here
  }


void CFontGenDlg::OnEnChangeCharacterSet()
  {
  // TODO:  If this is a RICHEDIT control, the control will not
  // send this notification unless you override the CDialog::OnInitDialog()
  // function and call CRichEditCtrl().SetEventMask()
  // with the ENM_CHANGE flag ORed into the mask.

  // TODO:  Add your control notification handler code here
  }


void CFontGenDlg::OnBnClickedDefaultSet()
  {
  m_strCharSet = defaultCharSet;
  }
