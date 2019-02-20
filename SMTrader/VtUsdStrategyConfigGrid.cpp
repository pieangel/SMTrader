#include "stdafx.h"
#include "VtUsdStrategyConfigGrid.h"


VtUsdStrategyConfigGrid::VtUsdStrategyConfigGrid()
{
	m_nButtonIndex = 0;
	_Type = 0;
}


VtUsdStrategyConfigGrid::~VtUsdStrategyConfigGrid()
{
}

void VtUsdStrategyConfigGrid::OnSetup()
{
	SetUGMode(UGMode::OrderNormal);

	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
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

int VtUsdStrategyConfigGrid::OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow)
{
	return 0;
}

int VtUsdStrategyConfigGrid::OnCellTypeNotify(long ID, int col, long row, long msg, long param)
{
	if (ID == UGCT_CHECKBOX) {
		return OnCheckbox(ID, col, row, msg, param);
	}

	if (ID == m_nButtonIndex) {
		return OnPushButton(ID, col, row, msg, param);
	}

	return 0;
}

int VtUsdStrategyConfigGrid::OnCheckbox(long ID, int col, long row, long msg, long param)
{
	return 1;
}

int VtUsdStrategyConfigGrid::OnPushButton(long ID, int col, long row, long msg, long param)
{
	return 0;
}

void VtUsdStrategyConfigGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{
	CUGCell cell;
	GetCellIndirect(col, row, &cell);

	StartEdit();
}

void VtUsdStrategyConfigGrid::OnSetCell(int col, long row, CUGCell *cell)
{
	if (col != 3 || row == -1)
		return;


	//CString tempStr('*', cell->GetTextLength());
	//cell->SetText(tempStr);
}

int VtUsdStrategyConfigGrid::OnEditStart(int col, long row, CWnd **edit)
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
		m_myCUGEdit.Create(dwStyle, CRect(0, 0, 0, 0), this, CONFIG_EDIT);
		m_myCUGEdit.m_ctrl = this;

		*edit = &m_myCUGEdit;
	}

	return TRUE;
}

int VtUsdStrategyConfigGrid::OnEditVerify(int col, long row, CWnd *edit, UINT *vcKey)
{
	return TRUE;
}

int VtUsdStrategyConfigGrid::OnEditFinish(int col, long row, CWnd *edit, LPCTSTR string, BOOL cancelFlag)
{
	return TRUE;
}

int VtUsdStrategyConfigGrid::OnEditContinue(int oldcol, long oldrow, int* newcol, long* newrow)
{
	return FALSE;
}

void VtUsdStrategyConfigGrid::SetRowTitle()
{
	JoinCells(0, 0, 0, 3);
	JoinCells(5, 0, 5, 3);
	QuickSetBackColor(0, 0, RGB(218, 226, 245));
	QuickSetBackColor(5, 0, RGB(252, 226, 228));
	_ColWidthVec.push_back(30);
	_ColWidthVec.push_back(20);
	_ColWidthVec.push_back(30);
	_ColWidthVec.push_back(57);
	_ColWidthVec.push_back(35);

	_ColWidthVec.push_back(30);
	_ColWidthVec.push_back(20);
	_ColWidthVec.push_back(30);
	_ColWidthVec.push_back(57);
	_ColWidthVec.push_back(35);

	for (int i = 0; i < _ColCount; ++i) {
		SetColWidth(i, _ColWidthVec[i]);
	}

	for (int i = 0; i < 4; ++i) {
		QuickSetLabelText(1, i, _T(""));
		QuickSetCellType(1, i, UGCT_CHECKBOX);
		QuickSetCellTypeEx(1, i, UGCT_CHECKBOXFLAT | UGCT_CHECKBOXCHECKMARK);

		QuickSetLabelText(6, i, _T(""));
		QuickSetCellType(6, i, UGCT_CHECKBOX);
		QuickSetCellTypeEx(6, i, UGCT_CHECKBOXFLAT | UGCT_CHECKBOXCHECKMARK);
	}

	QuickSetCellTypeEx(0, 0, UGCT_NORMALMULTILINE);
	QuickSetText(0, 0, _T("매수\n청산"));
	QuickSetCellTypeEx(5, 0, UGCT_NORMALMULTILINE);
	QuickSetText(5, 0, _T("매도\n청산"));

	ResizeWindow();
}

void VtUsdStrategyConfigGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtUsdStrategyConfigGrid::SetArg(VtSystemArgGroup& argGrp)
{
	if (argGrp.Name().compare(_T("매수청산")) == 0) {
		std::vector<VtSystemArg>& argVec = argGrp.GetArgVec();
		for (size_t i = 0; i < argVec.size(); ++i) {
			VtSystemArg& arg = argVec[i];
			CUGCell cell;
			GetCell(1, i, &cell);
			if (arg.Enable) {
				cell.SetNumber(1);
			}
			else {
				cell.SetNumber(0);
			}
			SetCell(1, i, &cell);
			QuickRedrawCell(1, i);
			QuickSetText(2, i, arg.sValue.c_str());
			QuickRedrawCell(2, i);
			QuickSetText(3, i, arg.Name.c_str());
			QuickRedrawCell(3, i);
			_ArgMap[arg.Name] = std::make_pair(4, i);
		}
	}
	else if (argGrp.Name().compare(_T("매도청산")) == 0) {
		std::vector<VtSystemArg>& argVec = argGrp.GetArgVec();
		for (size_t i = 0; i < argVec.size(); ++i) {
			VtSystemArg& arg = argVec[i];
			CUGCell cell;
			GetCell(6, i, &cell);
			if (arg.Enable) {
				cell.SetNumber(1);
			}
			else {
				cell.SetNumber(0);
			}
			SetCell(6, i, &cell);
			QuickRedrawCell(6, i);
			QuickSetText(7, i, arg.sValue.c_str());
			QuickRedrawCell(7, i);
			QuickSetText(8, i, arg.Name.c_str());
			QuickRedrawCell(8, i);
			_ArgMap[arg.Name] = std::make_pair(9, i);
		}
	}
}

void VtUsdStrategyConfigGrid::RefreshRealTimeValue(std::string argName, CString value)
{
	auto it = _ArgMap.find(argName);
	if (it != _ArgMap.end()) {
		std::pair<int, int> pos = it->second;
		QuickSetText(std::get<0>(pos), std::get<1>(pos), value);
		QuickRedrawCell(std::get<0>(pos), std::get<1>(pos));
	}
}

void VtUsdStrategyConfigGrid::ClearArgMap()
{
	_ArgMap.clear();
}

void VtUsdStrategyConfigGrid::ResizeWindow()
{
	CRect rcWnd, rcClient;
	GetWindowRect(rcWnd);
	GetClientRect(rcClient);

	int totalHeight = _HeadHeight * 4;
	int totalWidth = 0;
	for (size_t i = 0; i < _ColWidthVec.size(); ++i) {
		totalWidth += _ColWidthVec[i];
	}
	SetWindowPos(nullptr, 0, 0, totalWidth, totalHeight, SWP_NOMOVE | SWP_NOZORDER);
}
