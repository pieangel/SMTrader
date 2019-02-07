#include "stdafx.h"
#include "VtSysArgGrid.h"


VtSysArgGrid::VtSysArgGrid()
{
	m_nButtonIndex = 0;
}


VtSysArgGrid::~VtSysArgGrid()
{
}


void VtSysArgGrid::OnSetup()
{
	SetUGMode(UGMode::OrderNormal);

	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("±¼¸²"));
	_SetBitmap.LoadBitmap(MAKEINTRESOURCE(IDB_BITMAP_SET));
	m_Pen.CreatePen(PS_SOLID, 1, RGB(120, 120, 120));

	m_nButtonIndex = AddCellType(&m_button);
	SetDoubleBufferMode(TRUE);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);
	SetMargin(0);

	CUGCell cell;
	for (int yIndex = 0; yIndex < _RowCount; yIndex++) {
		SetRowHeight(yIndex, _HeadHeight);
		for (int xIndex = 0; xIndex < _ColCount; xIndex++) {
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);
			GetCell(xIndex, yIndex, &cell);
			if (yIndex == 0) {
				if (xIndex < _ColCount - 1)
					cell.SetBorder(UG_BDR_BTHIN | UG_BDR_TTHIN | UG_BDR_LTHIN);
				else
					cell.SetBorder(UG_BDR_BTHIN | UG_BDR_TTHIN | UG_BDR_LTHIN | UG_BDR_RTHIN);
			}
			else {
				if (xIndex < _ColCount - 1)
					cell.SetBorder(UG_BDR_BTHIN | UG_BDR_LTHIN);
				else
					cell.SetBorder(UG_BDR_BTHIN | UG_BDR_LTHIN | UG_BDR_RTHIN);
			}
			cell.SetBorderColor(&m_Pen);
			SetCell(xIndex, yIndex, &cell);
		}
	}

	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetTH_Height(0);
	SetVS_Width(0);
	SetHS_Height(0);

	EnableMenu(TRUE);

	SetRowTitle();
	//InitGrid();

	//ResizeWindow();
}

int VtSysArgGrid::OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow)
{
	return 0;
}

int VtSysArgGrid::OnCellTypeNotify(long ID, int col, long row, long msg, long param)
{
	if (ID == UGCT_CHECKBOX) {
		return OnCheckbox(ID, col, row, msg, param);
	}

	if (ID == m_nButtonIndex) {
		return OnPushButton(ID, col, row, msg, param);
	}

	return 0;
}

int VtSysArgGrid::OnCheckbox(long ID, int col, long row, long msg, long param)
{
	return 1;
}

int VtSysArgGrid::OnPushButton(long ID, int col, long row, long msg, long param)
{
	return 0;
}

void VtSysArgGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{
	CUGCell cell;
	GetCellIndirect(col, row, &cell);

	StartEdit();
}

void VtSysArgGrid::OnSetCell(int col, long row, CUGCell *cell)
{
	if (col != 3 || row == -1)
		return;


	//CString tempStr('*', cell->GetTextLength());
	//cell->SetText(tempStr);
}

int VtSysArgGrid::OnEditStart(int col, long row, CWnd **edit)
{
	CUGCell cell;
	GetCellIndirect(col, row, &cell);
	DWORD dwStyle = 0;


	if (cell.GetAlignment() & UG_ALIGNRIGHT)
		dwStyle |= ES_RIGHT;
	else if (cell.GetAlignment() & UG_ALIGNCENTER)
		dwStyle |= ES_CENTER;

	if (dwStyle != 0) {
		if (m_myCUGEdit.GetSafeHwnd())
			m_myCUGEdit.DestroyWindow();

		dwStyle |= WS_CHILD | WS_VISIBLE;
		// create the edit control with specified style
		m_myCUGEdit.Create(dwStyle, CRect(0, 0, 0, 0), this, ARG_EDIT);
		m_myCUGEdit.m_ctrl = this;

		*edit = &m_myCUGEdit;
	}

	return TRUE;
}

int VtSysArgGrid::OnEditVerify(int col, long row, CWnd *edit, UINT *vcKey)
{
	return TRUE;
}

int VtSysArgGrid::OnEditFinish(int col, long row, CWnd *edit, LPCTSTR string, BOOL cancelFlag)
{
	return TRUE;
}

int VtSysArgGrid::OnEditContinue(int oldcol, long oldrow, int* newcol, long* newrow)
{
	return FALSE;
}

void VtSysArgGrid::SetRowTitle()
{
	_ColWidthVec.push_back(20);
	_ColWidthVec.push_back(82);
	_ColWidthVec.push_back(70);
	_ColWidthVec.push_back(20);
	_ColWidthVec.push_back(82);
	_ColWidthVec.push_back(70);

	for (int i = 0; i < _ColCount; ++i) {
		SetColWidth(i, _ColWidthVec[i]);
	}

	for (int i = 0; i < _RowCount; ++i) {
		QuickSetLabelText(0, i, _T(""));
		QuickSetCellType(0, i, UGCT_CHECKBOX);
		QuickSetCellTypeEx(0, i, UGCT_CHECKBOXFLAT | UGCT_CHECKBOXCHECKMARK);

		QuickSetLabelText(3, i, _T(""));
		QuickSetCellType(3, i, UGCT_CHECKBOX);
		QuickSetCellTypeEx(3, i, UGCT_CHECKBOXFLAT | UGCT_CHECKBOXCHECKMARK);

		QuickSetBackColor(1, i, RGB(245, 245, 255));
		QuickSetBackColor(4, i, RGB(245, 245, 255));
	}


	ResizeWindow();
}

void VtSysArgGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();
	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtSysArgGrid::SetArg(VtSystemArgGroup& argGrp)
{
	std::vector<VtSystemArg>& argVec = argGrp.GetArgVec();
	for (size_t i = 0; i < argVec.size(); ++i) {
		VtSystemArg& arg = argVec[i];
		CUGCell cell;
		int row = (int)(i / 2);
		if (i % 2 == 0) {
			QuickSetText(1, row, arg.Name.c_str());
			QuickRedrawCell(1, row);
			QuickSetText(2, row, arg.sValue.c_str());
			QuickRedrawCell(2, row);
			GetCell(0, row, &cell);
			if (arg.Enable) {
				cell.SetNumber(1);
			}
			else {
				cell.SetNumber(0);
			}
			SetCell(0, row, &cell);
			QuickRedrawCell(0, row);
		} else {
			QuickSetText(4, row, arg.Name.c_str());
			QuickRedrawCell(4, row);
			QuickSetText(5, row, arg.sValue.c_str());
			QuickRedrawCell(5, row);
			GetCell(3, row, &cell);
			if (arg.Enable) {
				cell.SetNumber(1);
			}
			else {
				cell.SetNumber(0);
			}
			SetCell(3, row, &cell);
			QuickRedrawCell(3, row);
		}
	}
}

void VtSysArgGrid::ResizeWindow()
{
	CRect rcWnd, rcClient;
	GetWindowRect(rcWnd);
	GetClientRect(rcClient);

	int totalHeight = _HeadHeight * 4 + 4;
	int totalWidth = 0;
	for (size_t i = 0; i < _ColWidthVec.size(); ++i) {
		totalWidth += _ColWidthVec[i];
	}
	SetWindowPos(nullptr, 0, 0, totalWidth, totalHeight, SWP_NOMOVE | SWP_NOZORDER);
}

