#include "stdafx.h"
#include "VtConfigGrid.h"
#include <numeric>
#include "VtGlobal.h"
#include "VtOrderCenterWndHd.h"
#include "SmOrderPanel.h"
VtConfigGrid::VtConfigGrid()
{
}


VtConfigGrid::~VtConfigGrid()
{
}

void VtConfigGrid::OnSetup()
{
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	m_ClosePen.CreatePen(PS_SOLID, 1, RGB(175, 25, 192));
	SetUGMode(UGMode::Normal);
	SetVScrollMode(UG_SCROLLJOYSTICK);

	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);
	//_RowCount = GetMaxRow();
	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

	SetSH_Width(0);
	SetHS_Height(0);
	SetVS_Width(0);
	SetTH_Height(0);

	//Spin Button cell type
	m_nSpinIndex = AddCellType(&m_spin);

	for (int i = 0; i < _ColCount; i++) {
		QuickSetAlignment(i, 0, UG_ALIGNCENTER | UG_ALIGNVCENTER);
	}

	SetDefFont(&_defFont);


	SetRowTitle();

}

void VtConfigGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

void VtConfigGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (updn == TRUE) { // for clicked down only
		if (!_CenterWnd)
			return ;
		if (col == 4 && row == 0)
			_CenterWnd->OnBnClickedBtnLiq();
	}
}

int VtConfigGrid::OnCellTypeNotify(long ID, int col, long row, long msg, long param)
{
	if (ID == UGCT_CHECKBOX) {
		return OnCheckbox(ID, col, row, msg, param);
	}
	
	if (ID == m_nSpinIndex) {
		return OnSpinButton(ID, col, row, msg, param);
	}

// 	if (ID == m_nButtonIndex) {
// 		return OnPushButton(ID, col, row, msg, param);
// 	}
	
	return TRUE;
}

int VtConfigGrid::OnCheckbox(long ID, int col, long row, long msg, long param)
{
	if (!_CenterWnd)
		return FALSE;

	if (ID == UGCT_CHECKBOX) {
		CUGCell cell;
		GetCell(col, row, &cell);
		VtChartData* data = (VtChartData*)cell.Tag();
		double num = cell.GetNumber();
		if (num == 0.0){ // 체크박스 해제 - 호가 고정 안함
			_CenterWnd->FixedCenter(false);
		} else { // 체크박스 설정 - 호가 고정 함
			_CenterWnd->FixedCenter(true);
		}
	}
	return TRUE;
}

int VtConfigGrid::OnSpinButton(long ID, int col, long row, long msg, long param)
{
	if (!_CenterWnd)
		return FALSE;

	CUGCell cell;
	GetCell(col, row, &cell);
	int nCellTypeIndex = cell.GetCellType();
	int nParam = cell.GetParam();

	if (nParam == SPIN_TYPE1) {
		long num;
		cell.GetNumber(&num);

		if (msg == UGCT_SPINBUTTONUP) {
			num++;
		}
		if (msg == UGCT_SPINBUTTONDOWN) {
			num--;
			if (num < 0)
				num = 0;
		}
		CString str;
		str.Format("%ld", num);
		cell.SetText(str);
		SetCell(col, row, &cell);
		_CenterWnd->StopVal(num);
	}
	return TRUE;
}

int VtConfigGrid::OnPushButton(long ID, int col, long row, long msg, long param)
{
	if (!_CenterWnd)
		return FALSE;
	_CenterWnd->OnBnClickedBtnLiq();
	return TRUE;
}

void VtConfigGrid::OnMouseLeaveFromMainGrid()
{
	RestoreButtonCell();
}

void VtConfigGrid::OnMouseMove(int col, long row, POINT *point, UINT nFlags, BOOL processed /*= 0*/)
{
	if (col == 4 && row == 0) {
		CUGCell cell;
		GetCell(col, row, &cell);
		cell.SetTextColor(RGB(0, 0, 0));
		cell.SetBackColor(VtGlobal::SelCellBackColor);
		SetCell(col, row, &cell);
		QuickRedrawCell(col, row);
		_OldBtnCol = col;
		_OldBtnRow = row;
	}
	else {
		RestoreButtonCell();
	}
}

void VtConfigGrid::RestoreButtonCell()
{
	if (_OldBtnCol == 4 && _OldBtnRow == 0) {
		CUGCell cell;
		GetCell(_OldBtnCol, _OldBtnRow, &cell);
		cell.SetTextColor(RGB(255, 255, 255));
		cell.SetBackColor(RGB(22, 159, 79));
		SetCell(_OldBtnCol, _OldBtnRow, &cell);
		QuickRedrawCell(_OldBtnCol, _OldBtnRow);
	}
}

void VtConfigGrid::SetRowTitle()
{
	for (int i = 0; i < _ColCount; ++i) {
		QuickSetAlignment(i, 0, UG_ALIGNCENTER | UG_ALIGNVCENTER);
	}
	_ColWidths.push_back(57); // 매도 가능
	_ColWidths.push_back(26); // 매도 가능 입력
	_ColWidths.push_back(57); // 매수 가능
	_ColWidths.push_back(26); // 매수가능 입력
	_ColWidths.push_back(57); // Liq all

	_ColWidths.push_back(40); // Stop
	_ColWidths.push_back(35); // Stop input
	_ColWidths.push_back(67); // check label
	SetColWidth(0, _ColWidths[0]); // 매도 가능
	SetColWidth(1, _ColWidths[1]); // 매도 가능 입력
	SetColWidth(2, _ColWidths[2]); // 매수 가능
	SetColWidth(3, _ColWidths[3]); // 매수 가능 입력
	SetColWidth(4, _ColWidths[4]); // 모두청산 버튼
	SetColWidth(5, _ColWidths[5]); // Stop
	SetColWidth(6, _ColWidths[6]); // Stop 입력
	SetColWidth(7, _ColWidths[7]); // 호가고정 체크

	QuickSetText(0, 0, _T("매도가능"));
	QuickSetText(2, 0, _T("매수가능"));
	QuickSetText(4, 0, _T("모두청산"));
	QuickSetText(5, 0, _T("STOP")); 
	QuickSetText(7, 0, _T("호가고정"));

	CUGCell cell;
	GetCell(6, 0, &cell);
	cell.SetText("1");
	cell.SetCellType(m_nSpinIndex);
	cell.SetParam(SPIN_TYPE1);
	SetCell(6, 0, &cell);

	QuickSetLabelText(7, 0, _T("호가고정"));
	QuickSetCellType(7, 0, UGCT_CHECKBOX);

	QuickSetCellTypeEx(7, 0, UGCT_CHECKBOXFLAT | UGCT_CHECKBOXCHECKMARK);

	//Push Button cell type
	m_nButtonIndex = AddCellType(&m_button);

	GetCell(4, 0, &cell);
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetTextColor(RGB(0, 0, 128));
	cell.SetBackColor(GetSysColor(COLOR_BTNFACE));
	SetCell(4, 0, &cell);

	
	QuickSetBackColor(0, 0, RGB(218, 226, 245)); // 매도가능
	QuickSetBackColor(2, 0, RGB(252, 226, 228)); // 매수가능
	//QuickSetBackColor(7, 0, RGB(175, 25, 192)); // 호가고정
	//QuickSetTextColor(7, 0, RGB(255, 255, 255)); // 호가고정
	QuickSetBackColor(4, 0, RGB(22, 159, 79)); // 모두청산
	QuickSetTextColor(4, 0, RGB(255, 255, 255)); // 모두청산

	for (int i = 0; i < _ColCount; ++i) {
		QuickRedrawCell(0, i);
	}

	CRect rcWnd;
	GetWindowRect(rcWnd);
	SetWindowPos(nullptr, 0, 0, std::accumulate(_ColWidths.begin(), _ColWidths.end(), 0), 21, SWP_NOMOVE);
}

void VtConfigGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}
