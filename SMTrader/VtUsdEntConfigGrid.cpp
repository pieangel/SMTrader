#include "stdafx.h"
#include "VtUsdEntConfigGrid.h"



VtUsdEntConfigGrid::VtUsdEntConfigGrid()
{
	m_nButtonIndex = 0;
	_Type = 0;
}


VtUsdEntConfigGrid::~VtUsdEntConfigGrid()
{
}

void VtUsdEntConfigGrid::OnSetup()
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

int VtUsdEntConfigGrid::OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow)
{
	return 0;
}

int VtUsdEntConfigGrid::OnCellTypeNotify(long ID, int col, long row, long msg, long param)
{
	if (ID == UGCT_CHECKBOX) {
		return OnCheckbox(ID, col, row, msg, param);
	}

	if (ID == m_nButtonIndex) {
		return OnPushButton(ID, col, row, msg, param);
	}

	return 0;
}

int VtUsdEntConfigGrid::OnCheckbox(long ID, int col, long row, long msg, long param)
{
	return 1;
}

int VtUsdEntConfigGrid::OnPushButton(long ID, int col, long row, long msg, long param)
{
	return 0;
}

void VtUsdEntConfigGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{
	CUGCell cell;
	GetCellIndirect(col, row, &cell);

	StartEdit();
}

void VtUsdEntConfigGrid::OnSetCell(int col, long row, CUGCell *cell)
{
	if (col != 3 || row == -1)
		return;


	//CString tempStr('*', cell->GetTextLength());
	//cell->SetText(tempStr);
}

int VtUsdEntConfigGrid::OnEditStart(int col, long row, CWnd **edit)
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
		m_myCUGEdit.Create(dwStyle, CRect(0, 0, 0, 0), this, USD_CONFIG_EDIT);
		m_myCUGEdit.m_ctrl = this;

		*edit = &m_myCUGEdit;
	}

	return TRUE;
}

int VtUsdEntConfigGrid::OnEditVerify(int col, long row, CWnd *edit, UINT *vcKey)
{
	return TRUE;
}

int VtUsdEntConfigGrid::OnEditFinish(int col, long row, CWnd *edit, LPCTSTR string, BOOL cancelFlag)
{
	return TRUE;
}

int VtUsdEntConfigGrid::OnEditContinue(int oldcol, long oldrow, int* newcol, long* newrow)
{
	return FALSE;
}

void VtUsdEntConfigGrid::SetRowTitle()
{
	JoinCells(0, 0, 0, 3);
	JoinCells(5, 0, 5, 3);
	QuickSetBackColor(0, 0, RGB(252, 226, 228));
	QuickSetBackColor(5, 0, RGB(218, 226, 245));
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
	QuickSetText(0, 0, _T("매수\n진입"));
	QuickSetCellTypeEx(5, 0, UGCT_NORMALMULTILINE);
	QuickSetText(5, 0, _T("매도\n진입"));

	ResizeWindow();
}

void VtUsdEntConfigGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtUsdEntConfigGrid::SetArg(VtSystemArgGroup& argGrp)
{
	if (argGrp.Name().compare(_T("매수진입")) == 0) {
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
	else if (argGrp.Name().compare(_T("매도진입")) == 0) {
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

void VtUsdEntConfigGrid::SetHogaCountBuyEntRatio(CString value)
{
	CUGCell cell;
	GetCell(4, 0, &cell);
	cell.SetText(value);
	SetCell(4, 0, &cell);
	RedrawCell(4, 0);
}

void VtUsdEntConfigGrid::SetHogaCountSellEntRatio(CString value)
{
	CUGCell cell;
	GetCell(9, 0, &cell);
	cell.SetText(value);
	SetCell(9, 0, &cell);
	RedrawCell(9, 0);
}

void VtUsdEntConfigGrid::SetHogaCountEntRatio(double uac, double ubc)
{
	double ratio = uac / ubc;
	CString value;
	value.Format(_T("%.2f"), ratio);
	SetHogaCountBuyEntRatio(value);
	ratio = ubc / uac;
	value.Format(_T("%.2f"), ratio);
	SetHogaCountSellEntRatio(value);
}

void VtUsdEntConfigGrid::SetHogaQtyEntRatio(double uas, double ubs)
{
	double ratio = uas / ubs;
	CString value;
	value.Format(_T("%.2f"), ratio);
	SetHogaQtyBuyEntRatio(value);
	ratio = ubs / uas;
	value.Format(_T("%.2f"), ratio);
	SetHogaQtySellEntRatio(value);
}

void VtUsdEntConfigGrid::RefreshRealTimeValue(std::string argName, CString value)
{
	auto it = _ArgMap.find(argName);
	if (it != _ArgMap.end()) {
		std::pair<int, int> pos = it->second;
		QuickSetText(std::get<0>(pos), std::get<1>(pos), value);
		QuickRedrawCell(std::get<0>(pos), std::get<1>(pos));
	}
}

void VtUsdEntConfigGrid::ClearArgMap()
{
	_ArgMap.clear();
}

void VtUsdEntConfigGrid::SetHogaQtyBuyEntRatio(CString value)
{
	CUGCell cell;
	GetCell(4, 1, &cell);
	cell.SetText(value);
	SetCell(4, 1, &cell);
	RedrawCell(4, 1);
}


void VtUsdEntConfigGrid::SetHogaQtySellEntRatio(CString value)
{
	CUGCell cell;
	GetCell(9, 1, &cell);
	cell.SetText(value);
	SetCell(9, 1, &cell);
	RedrawCell(9, 1);
}

void VtUsdEntConfigGrid::ResizeWindow()
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
