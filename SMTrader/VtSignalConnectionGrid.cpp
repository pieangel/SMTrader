#include "stdafx.h"
#include "VtSignalConnectionGrid.h"
#include "VtGlobal.h"


VtSignalConnectionGrid::VtSignalConnectionGrid()
{
}


VtSignalConnectionGrid::~VtSignalConnectionGrid()
{
}

void VtSignalConnectionGrid::OnSetup()
{
	_defFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);

	//Ellipsis Button cell type
	m_nEllipsisIndex = AddCellType(&m_ellipsis);
	//Spin Button cell type
	m_nSpinIndex = AddCellType(&m_spin);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

	CUGCell cell;
	for (int yIndex = 0; yIndex < _RowCount; yIndex++) {
		for (int xIndex = 0; xIndex < _ColCount; xIndex++) {
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);
			if (xIndex == 0) {
				QuickSetLabelText(xIndex, yIndex, _T(""));
				QuickSetCellType(xIndex, yIndex, UGCT_CHECKBOX);
				QuickSetCellTypeEx(xIndex, yIndex, UGCT_CHECKBOXFLAT | UGCT_CHECKBOXCHECKMARK);
			}
			else if (xIndex == 1 || xIndex == 2) {
				GetCell(xIndex, yIndex, &cell);
				xIndex == 1 ? cell.SetText(_T("계좌선택")) : cell.SetText(_T("종목선택"));
				cell.SetCellType(m_nEllipsisIndex);
				cell.SetCellTypeEx(UGCT_NORMALELLIPSIS);
				xIndex == 1 ? cell.SetParam(ELLIPSISBUTTON_CLICK_ACNT) : cell.SetParam(ELLIPSISBUTTON_CLICK_PRDT);
				SetCell(xIndex, yIndex, &cell);
			}
			else if (xIndex == 3) {
				GetCell(xIndex, yIndex, &cell);
				cell.SetText("신호선택");
				cell.SetCellType(UGCT_DROPLIST);
				cell.SetCellTypeEx(UGCT_DROPLISTHIDEBUTTON);
				cell.SetReadOnly(FALSE);
				cell.SetLabelText("신호1\n신호2\n신호3\n");
				SetCell(xIndex, yIndex, &cell);
			}
			else if (xIndex == 4) {
				GetCell(xIndex, yIndex, &cell);
				cell.SetText("1");
				cell.SetCellType(m_nSpinIndex);
				cell.SetParam(SPIN_TYPE_SEUNGSU);
				SetCell(xIndex, yIndex, &cell);
			}
		}
	}

	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetVS_Width(0);
	SetHS_Height(0);
	SetColTitle();
	SetVScrollMode(UG_SCROLLNORMAL);
	InitGrid();
}

void VtSignalConnectionGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{
	if (col == 0 && row == 0)
		RedrawAll();
}

int VtSignalConnectionGrid::OnCellTypeNotify(long ID, int col, long row, long msg, long param) {

	if (ID == UGCT_DROPLIST) {
		return OnDropList(ID, col, row, msg, param);
	}
	if (ID == UGCT_CHECKBOX) {
		return OnCheckbox(ID, col, row, msg, param);
	}
	if (ID == m_nEllipsisIndex) {
		return OnEllipsisButton(ID, col, row, msg, param);
	}
	if (ID == m_nSpinIndex) {
		return OnSpinButton(ID, col, row, msg, param);
	}
	return TRUE;
}


void VtSignalConnectionGrid::SetColTitle()
{
	CUGCell cell;
	LPCTSTR title[5] = { "실행", "계좌번호", "종목", "신호", "승수" };
	int colWidth[5] = { 25, 80, 110, 50, 80 };


	for (int i = 0; i < _ColCount; i++) {
		SetColWidth(i, colWidth[i]);
		GetCell(i, -1, &cell);
		cell.SetText(title[i]);
		cell.SetBackColor(VtGlobal::GridTitleBackColor);
		cell.SetTextColor(VtGlobal::GridTitleTextColor);
		cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
		cell.SetFont(&_titleFont);
		SetCell(i, -1, &cell);
		QuickRedrawCell(i, -1);
	}
}

void VtSignalConnectionGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtSignalConnectionGrid::InitGrid()
{
	
}

void VtSignalConnectionGrid::ClearCells()
{
	CUGCell cell;
	for (int i = 0; i < _RowCount; i++) {
		for (int j = 0; j < _ColCount; j++) {
			QuickSetText(j, i, _T(""));
			QuickSetBackColor(j, i, RGB(255, 255, 255));
			QuickRedrawCell(j, i);
			GetCell(j, i, &cell);
			cell.Tag(nullptr);
			SetCell(j, i, &cell);
		}
	}
}

int VtSignalConnectionGrid::OnDropList(long ID, int col, long row, long msg, long param)
{
	return 1;
}

int VtSignalConnectionGrid::OnCheckbox(long ID, int col, long row, long msg, long param)
{
	return 1;
}

int VtSignalConnectionGrid::OnEllipsisButton(long ID, int col, long row, long msg, long param)
{
	CUGCell cell;
	GetCell(col, row, &cell);
	int nCellTypeIndex = cell.GetCellType();
	int nParam = cell.GetParam();

	if (msg == UGCT_ELLIPSISBUTTONCLICK) {
		if (nParam == ELLIPSISBUTTON_CLICK_ACNT) {
			MessageBox("The button was clicked", "Cell Type Notification");
		}
		else if (nParam == ELLIPSISBUTTON_CLICK_PRDT) {
			MessageBox("The button was clicked", "Cell Type Notification");
		}
		else {
			MessageBox("The button was clicked", "Cell Type Notification");
		}
	}

	return TRUE;
}

int VtSignalConnectionGrid::OnSpinButton(long ID, int col, long row, long msg, long param)
{
	long num;
	CUGCell cell;
	GetCell(col, row, &cell);
	cell.GetNumber(&num);

	if (msg == UGCT_SPINBUTTONUP) {
		num++;
	}
	if (msg == UGCT_SPINBUTTONDOWN) {
		num--;
		num == 0 ? num = 1 : num = num;
	}
	CString str;
	str.Format("%ld", num);
	cell.SetText(str);
	SetCell(col, row, &cell);
	return 1;
}
