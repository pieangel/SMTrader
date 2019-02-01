#include "stdafx.h"
#include "../../UGrid/UGCtrl.h"
#include "UGCTSeperateText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/***************************************************
CUGLabeledType - Constructor
Initialize all member data
****************************************************/
CUGCTSeperateText::CUGCTSeperateText() {

	m_canOverLap = FALSE;

	SetLabelFont(12, 700, _T("±¼¸²"), RGB(0, 0, 0));
}

/***************************************************
~CUGLabeledType - Destructor
Clean up all allocated resources
****************************************************/
CUGCTSeperateText::~CUGCTSeperateText() {
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
LPCTSTR CUGCTSeperateText::GetName()
{
	return _T("Seperated");
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
LPCUGID CUGCTSeperateText::GetUGID()
{
	// {97431CB8-2142-49A8-B166-CE87460A5DD2}
	static const UGID ugid =
	//{ 0x10f07b47, 0xdcde, 0x11d5,{ 0x9b, 0x3a, 0x0, 0x50, 0xba, 0xd4, 0x4b, 0xcd } };
	{ 0x97431cb8, 0x2142, 0x49a8,{ 0xb1, 0x66, 0xce, 0x87, 0x46, 0xa, 0x5d, 0xd2 } };
	return &ugid;
}

/***************************************************
GetEditArea - overloaded CUGCellType::GetEditArea

**See CUGCellType::GetEditArea for more details
about this function
****************************************************/
int CUGCTSeperateText::GetEditArea(RECT *rect) {

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
void CUGCTSeperateText::OnDraw(CDC *dc, RECT *rect, int col, long row,
	CUGCell *cell, int selected, int current)
{
	if (!m_drawThemesSet)
		m_useThemes = cell->UseThemes();


	//draw the cells border using the default drawing routine
	DrawBorder(dc, rect, rect, cell);

	CRect tempRect(rect);
	//tempRect.bottom = tempRect.top + m_fontHeight;

	//check the selected and current states
	//if (selected || (current && m_ctrl->m_GI->m_currentCellMode & 2))
	//	DrawBackground(dc, rect, cell->GetHBackColor(), row, col, cell, current > 0, selected > 0);
	//else
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
	//tempRect.top += m_fontHeight;
	tempRect.right = rect->right;
	//draw the text in using the default drawing routine
	DrawText(dc, tempRect, 0, col, row, cell, selected, current);

	// Draw the label text
	tempRect.CopyRect(rect);
	tempRect.top += 3;
	tempRect.left = rect->right - 10;
	tempRect.bottom = rect->bottom - 1;
	
	if (cell->OrderType() == 2)
		m_labelColor = RGB(255, 0, 0);
	else if (cell->OrderType() == 1)
		m_labelColor = RGB(0, 0, 0);
	else
		m_labelColor = RGB(0, 0, 255);
	
	dc->SetTextColor(m_labelColor);
	CFont* oldFont = dc->SelectObject(&m_labelFont);
	//dc->ExtTextOut(tempRect.right - 30, tempRect.top, ETO_CLIPPED, tempRect, cell->GetLabelText(), NULL);
	//DrawText(dc, tempRect, 0, col, row, cell, selected, current);
	dc->DrawText(cell->GetLabelText(), tempRect, DT_LEFT | DT_BOTTOM);
	dc->SelectObject(oldFont);

	dc->SetBkMode(OPAQUE);

	if (cell->Focused()) {
		DrawFocusedRect(dc, rect);
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
int CUGCTSeperateText::SetLabelFont(int height, int weight, LPCTSTR name, COLORREF color) {

	m_fontHeight = height;
	m_labelColor = color;

	if (m_labelFont.GetSafeHandle() != NULL)
		m_labelFont.DeleteObject();

	if (m_labelFont.CreateFont(m_fontHeight, 0, 0, 0, weight, 0, 0, 0, 0, 0, 0, 0, 0, name) != 0)
		return UG_SUCCESS;
	return UG_ERROR;
}

void CUGCTSeperateText::SetLabelColor(COLORREF color)
{
	m_labelColor = color;
}

void CUGCTSeperateText::DrawFocusedRect(CDC *dc, RECT *rect)
{
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	CPen* oldPen = nullptr;
	oldPen = (CPen*)dc->SelectObject(&pen);

	RECT rectSide(*rect);
	rectSide.top++;
	rectSide.left++;
	rectSide.bottom -= 2;
	rectSide.right -= 2;

	dc->MoveTo(rectSide.left, rectSide.top);
	dc->LineTo(rectSide.right, rectSide.top);
	dc->LineTo(rectSide.right, rectSide.bottom);
	dc->LineTo(rectSide.left, rectSide.bottom);
	dc->LineTo(rectSide.left, rectSide.top);

	dc->SelectObject(oldPen);
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
void CUGCTSeperateText::GetBestSize(CDC *dc, CSize *size, CUGCell *cell)
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
