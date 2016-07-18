// PreviewWnd.cpp : implementation file
//

#include "stdafx.h"
#include "FontRotate.h"
#include "PreviewWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreviewWnd dialog


CPreviewWnd::CPreviewWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CPreviewWnd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPreviewWnd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_sizeBitmap.cx = 0;
	m_sizeBitmap.cy = 0;
}


void CPreviewWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreviewWnd)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPreviewWnd, CDialog)
	//{{AFX_MSG_MAP(CPreviewWnd)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreviewWnd message handlers

BOOL CPreviewWnd::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateData(FALSE);

	ModifyStyle(0, WS_HSCROLL | WS_VSCROLL, SWP_FRAMECHANGED);
	if(m_pBitmap != NULL)
	{
		m_sizeBitmap = m_pBitmap->GetBitmapDimension();

		SetScrollRange(SB_HORZ, 0, m_sizeBitmap.cx);
		SetScrollRange(SB_VERT, 0, m_sizeBitmap.cy);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreviewWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rect;
	GetClientRect(rect);

	if(m_pDC != NULL)
		dc.BitBlt(0, 0, rect.Width(), rect.Height(),
							m_pDC, GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT), SRCCOPY);
	else
		dc.FillRect(rect, CBrush::FromHandle((HBRUSH) GetStockObject(WHITE_BRUSH)));
}

void CPreviewWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	switch(nSBCode)
		{
		case SB_LEFT :	//   Scroll to far left. 
			SetScrollPos(SB_HORZ, 0);
			break;

		case SB_ENDSCROLL : //  End scroll. 
			return;

		case SB_LINELEFT :	// Scroll left. 
			SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ) - 1);
			break;

		case SB_LINERIGHT :	// Scroll right. 
			SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ) + 1);
			break;

		case SB_PAGELEFT :	// Scroll one page left. 
			SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ) - 10);
			break;

		case SB_PAGERIGHT :	// Scroll one page right. 
			SetScrollPos(SB_HORZ, GetScrollPos(SB_HORZ) + 10);
			break;

		case SB_RIGHT :	// Scroll to far right. 
			SetScrollPos(SB_HORZ, m_sizeBitmap.cx);
			break;

		case SB_THUMBPOSITION : //   Scroll to absolute position. The current position is specified by the nPos parameter. 
		case SB_THUMBTRACK : //  Drag scroll box to specified position. The current position is specified by the nPos parameter. 
			SetScrollPos(SB_HORZ, nPos);
			break;
		}

	Invalidate();
}

void CPreviewWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	switch(nSBCode)
		{
		case SB_LEFT :	//   Scroll to far left. 
			SetScrollPos(SB_VERT, 0);
			break;

		case SB_ENDSCROLL : //  End scroll. 
			return;

		case SB_LINELEFT :	// Scroll left. 
			SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) - 1);
			break;

		case SB_LINERIGHT :	// Scroll right. 
			SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) + 1);
			break;

		case SB_PAGELEFT :	// Scroll one page left. 
			SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) - 10);
			break;

		case SB_PAGERIGHT :	// Scroll one page right. 
			SetScrollPos(SB_VERT, GetScrollPos(SB_VERT) + 10);
			break;

		case SB_RIGHT :	// Scroll to far right. 
			SetScrollPos(SB_VERT, m_sizeBitmap.cx);
			break;

		case SB_THUMBPOSITION : //   Scroll to absolute position. The current position is specified by the nPos parameter. 
		case SB_THUMBTRACK : //  Drag scroll box to specified position. The current position is specified by the nPos parameter. 
			SetScrollPos(SB_VERT, nPos);
			break;
		}

	Invalidate();	
}

BOOL CPreviewWnd::OnEraseBkgnd(CDC* pDC) 
{
	CRect rectClient;

	GetClientRect(rectClient);

	CBrush *brWhite = CBrush::FromHandle(HBRUSH(GetStockObject(WHITE_BRUSH)));

	pDC->FillRect(CRect(rectClient.left + m_sizeBitmap.cx, 0, rectClient.right, rectClient.top + m_sizeBitmap.cy), brWhite);
	pDC->FillRect(CRect(rectClient.left, rectClient.top + m_sizeBitmap.cy, rectClient.right, rectClient.bottom), brWhite);

	
	return FALSE;
}
