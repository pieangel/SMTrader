#include "stdafx.h"
#include "../../UGrid/UGCtrl.h"
#include "UGCTOrder.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/***************************************************
CUGLabeledType - Constructor
Initialize all member data
****************************************************/
CUGCTOrder::CUGCTOrder() {

	m_canOverLap = FALSE;

	SetLabelFont(10, 300, _T("±¼¸²"), RGB(255, 0, 0));
}

/***************************************************
~CUGLabeledType - Destructor
Clean up all allocated resources
****************************************************/
CUGCTOrder::~CUGCTOrder() {
}

/***************************************************
GetName  - overloaded CUGCellType::GetName
Returns a readable name for the cell type.
Returned value is used to help end-users
to see what cell type are available.

**See CUGCellType::GetName for more details
about this function

Params:
<none>
Return:
cell type name
****************************************************/
LPCTSTR CUGCTOrder::GetName()
{
	return _T("Order");
}



constexpr int Round(float x) { return static_cast<int>(x + 0.5f); }


void CUGCTOrder::DrawOrderArrow(int orderType, CDC* pdc, POINT p0, POINT p1, int head_length, int head_width)
{
	CPen pen;
	CBrush brush1;   // Must initialize!
	CPen* pOldPen = NULL;
	CBrush* pTempBrush = NULL;
	CBrush OrigBrush;

	if (orderType == 2 || orderType == 4)
	{
		brush1.CreateSolidBrush(RGB(0, 0, 255));   // Blue brush.
		pen.CreatePen(0, 1, RGB(0, 0, 255));
		pTempBrush = (CBrush*)pdc->SelectObject(&brush1);
		pOldPen = pdc->SelectObject(&pen);
		// Save original brush.
		OrigBrush.FromHandle((HBRUSH)pTempBrush);
	}
	else if (orderType == 1 || orderType == 3)
	{
		brush1.CreateSolidBrush(RGB(255, 0, 0));   // Blue brush.
		pen.CreatePen(0, 1, RGB(255, 0, 0));
		pTempBrush = (CBrush*)pdc->SelectObject(&brush1);
		pOldPen = pdc->SelectObject(&pen);
		// Save original brush.
		OrigBrush.FromHandle((HBRUSH)pTempBrush);
	}


	pdc->MoveTo(p0);
	pdc->LineTo(p1);

	const float dx = static_cast<float>(p1.x - p0.x);
	const float dy = static_cast<float>(p1.y - p0.y);
	const auto length = std::sqrt(dx*dx + dy*dy);
	if (head_length < 1 || length < head_length) return;

	// ux,uy is a unit vector parallel to the line.
	const auto ux = dx / length;
	const auto uy = dy / length;

	// vx,vy is a unit vector perpendicular to ux,uy
	const auto vx = -uy;
	const auto vy = ux;

	const auto half_width = 0.5f * head_width;

	const POINT arrow[3] =
	{ p1,
		POINT{ Round(p1.x - head_length*ux + half_width*vx),
		Round(p1.y - head_length*uy + half_width*vy) },
		POINT{ Round(p1.x - head_length*ux - half_width*vx),
		Round(p1.y - head_length*uy - half_width*vy) }
	};
	pdc->Polygon(arrow, 3);


	pdc->SelectObject(pOldPen);

	// Reselect original brush into device context.
	pdc->SelectObject(&OrigBrush);
}


/***************************************************
GetUGID  - overloaded CUGCellType::GetUGID
Returns a GUID for the cell type, this number
is unique for each cell type and never changes.
This number can be used to find the cell types
added to this instance of the Ultimate Grid.

**See CUGCellType::GetUGID for more details
about this function

Params:
<none>
Returns:
UGID (which is actually a GUID)
****************************************************/
LPCUGID CUGCTOrder::GetUGID()
{
	// {97431CB8-2142-49A8-B166-CE87460A5DD2}
	static const UGID ugid =
	{ 0x46bb579b, 0x3300, 0x4045,{ 0x9a, 0x27, 0x97, 0x85, 0x68, 0x65, 0xb3, 0x32 } };

	return &ugid;
}

/***************************************************
GetEditArea - overloaded CUGCellType::GetEditArea

**See CUGCellType::GetEditArea for more details
about this function
****************************************************/
int CUGCTOrder::GetEditArea(RECT *rect) {

	//rect->top += m_fontHeight;

	if (m_ctrl->m_editInProgress)
		m_ctrl->RedrawCell(m_ctrl->m_editCol, m_ctrl->m_editRow);

	return UG_SUCCESS;
}

/***************************************************
OnDraw - overloaded CUGCellType::OnDraw
This function will adjust the font size used for
drawing the text for the given cell based on the
length of the text, in an attempt to display the
entire text string

**See CUGCellType::OnDraw for more details
about this function
****************************************************/
void CUGCTOrder::OnDraw(CDC *dc, RECT *rect, int col, long row,
	CUGCell *cell, int selected, int current)
{
	if (!m_drawThemesSet)
		m_useThemes = cell->UseThemes();


	//draw the cells border using the default drawing routine
	DrawBorder(dc, rect, rect, cell);

	CRect tempRect(rect);
	
	DrawBackground(dc, rect, cell->GetBackColor(), row, col, cell, current > 0, selected > 0);

	if (UGXPThemes::IsThemed())
	{
		dc->SetBkMode(TRANSPARENT);
	}


	//check the selected and current states
	//if (selected || (current && m_ctrl->m_GI->m_currentCellMode & 2))
	//	dc->SetTextColor(cell->GetHTextColor());
	//else
	dc->SetTextColor(cell->GetTextColor());

	tempRect.CopyRect(rect);
	
	RECT rectout;
	CopyRect(&rectout, rect);
	CPoint pt1;
	CPoint pt2;
	if (cell->OrderType() == 1 || cell->OrderType() == 3) // for stop buy
	{
		if (cell->OrderType() == 1)
		{
			tempRect.left = tempRect.left + tempRect.Width() / 2;
			//draw the text in using the default drawing routine
			DrawText(dc, tempRect, 0, col, row, cell, selected, current);

			rectout.right -= (rectout.right - rectout.left) / 2;
			pt1.x = rectout.right;
			pt1.y = rectout.top + (rectout.bottom - rectout.top) / 2;
			pt2.x = rectout.left;
			pt2.y = rectout.top + (rectout.bottom - rectout.top) / 2;

			DrawOrderArrow(cell->OrderType(), dc, pt1, pt2, 6, 6);
		}
		else
		{
			DrawText(dc, rect, 0, col, row, cell, selected, current);
		}
	}
	else if (cell->OrderType() == 2 || cell->OrderType() == 4) // for stop sell
	{
		if (cell->OrderType() == 2)
		{
			tempRect.right = tempRect.right - tempRect.Width() / 2;
			//draw the text in using the default drawing routine
			DrawText(dc, tempRect, 0, col, row, cell, selected, current);

			rectout.left = rectout.left + (rectout.right - rectout.left) / 2;
			pt1.x = rectout.left;
			pt1.y = rectout.top + (rectout.bottom - rectout.top) / 2;
			pt2.x = rectout.right;
			pt2.y = rectout.top + (rectout.bottom - rectout.top) / 2;

			DrawOrderArrow(cell->OrderType(), dc, pt1, pt2, 6, 6);
		}
		else
		{
			DrawText(dc, rect, 0, col, row, cell, selected, current);
		}
	}
}

/***************************************************
SetLabelFont
Sets up the font that will be used to draw the
cells label
Params
height - height of the font in pixels
weight - weight of the font (0-1000)
name   - typeface name of the font
color  - color of the font
Return
UG_SUCCESS - success
UG_ERROR - error
****************************************************/
int CUGCTOrder::SetLabelFont(int height, int weight, LPCTSTR name, COLORREF color) {

	m_fontHeight = height;
	m_labelColor = color;

	if (m_labelFont.GetSafeHandle() != NULL)
		m_labelFont.DeleteObject();

	if (m_labelFont.CreateFont(m_fontHeight, 0, 0, 0, weight, 0, 0, 0, 0, 0, 0, 0, 0, name) != 0)
		return UG_SUCCESS;
	return UG_ERROR;
}

/****************************************************
GetBestSize
Returns the best (nominal) size for a cell using
this cell type, with the given cell properties.
Params:
dc		- device context to use to calc the size on
size	- return the best size in this param
cell	- pointer to a cell object to use for the calc.
Return:
<none>
*****************************************************/
void CUGCTOrder::GetBestSize(CDC *dc, CSize *size, CUGCell *cell)
{
	// get the size required by the main text
	CUGCellType::GetBestSize(dc, size, cell);
	// calculate the width required by the label text
	CRect rect(0, 0, 0, 0);
	CFont* oldFont = dc->SelectObject(&m_labelFont);
	dc->DrawText(cell->GetLabelText(), rect, DT_CALCRECT | DT_SINGLELINE);
	dc->SelectObject(oldFont);

	if (size->cx < rect.Width())
		size->cx = rect.Width();
}

