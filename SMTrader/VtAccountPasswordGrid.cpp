#include "stdafx.h"
#include "VtAccountPasswordGrid.h"
#include "VtAccountManager.h"
#include "VtAccount.h"
#include "VtGlobal.h"
#include "VtAccountManager.h"

#define USE_PASSWORD		1003
#define USE_NORMAL          1004
#define FLEXEDIT_ID			320001
#define COXMASK_ID			320002
#define COXNUMBER_ID		320003
#define COXCURRENCY_ID		320004

VtAccountPasswordGrid::VtAccountPasswordGrid()
{
	_ShowPassword = false;
	_EdittingRow = -2;
}


VtAccountPasswordGrid::~VtAccountPasswordGrid()
{
}

void VtAccountPasswordGrid::OnSetup()
{
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

	SetUGMode(UGMode::Normal);
	SetVScrollMode(UG_SCROLLJOYSTICK);

	SetDoubleBufferMode(TRUE);

	SetDefColWidth(80);
	//_RowCount = GetMaxRow();
	SetHS_Height(0);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);

	CUGCell cell;
	QuickSetAlignment(0, 0, UG_ALIGNCENTER | UG_ALIGNVCENTER);
	for (int i = 0; i < _RowCount; i++) {
		QuickSetAlignment(0, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		QuickSetAlignment(1, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		QuickSetAlignment(2, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		QuickSetAlignment(3, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);

		GetCell(3, i, &cell);
		cell.SetReadOnly(FALSE);
		cell.SetText("");
		cell.SetParam(USE_PASSWORD);
		SetCell(3, i, &cell);
	}

	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetVS_Width(0);
	SetColTitle();
	InitGrid();
}

void VtAccountPasswordGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{
	//CUGCell cell;
	//GetCellIndirect(col, row, &cell);

	//StartEdit();
}

void VtAccountPasswordGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	CUGCell cell;
	GetCellIndirect(col, row, &cell);
	if (updn == FALSE) {
		StartEdit();
	}
}

void VtAccountPasswordGrid::OnSetCell(int col, long row, CUGCell *cell)
{
	if (col != 3 || row == -1)
		return;

	
	if (!_ShowPassword) {
		CString tempStr('*', cell->GetTextLength());
		cell->SetText(tempStr);
	}
	
}

int VtAccountPasswordGrid::OnCellTypeNotify(long ID, int col, long row, long msg, long param)
{
	if (ID == UGCT_CHECKBOX) {
		return OnCheckbox(ID, col, row, msg, param);
	}
	return TRUE;
}

int VtAccountPasswordGrid::OnCheckbox(long ID, int col, long row, long msg, long param)
{
	CUGCell cell;
	if (row == -1) {
		GetCell(0, -1, &cell);
		double num = cell.GetNumber();
		for (size_t i = 0; i < _CellAcntMap.size(); ++i) {
			GetCell(0, i, &cell);
			VtAccount* acnt = (VtAccount*)cell.Tag();
			cell.SetNumber(num);
			num == 1.0 ? acnt->Enable(true) : acnt->Enable(false);
			SetCell(0, i, &cell);
			QuickRedrawCell(0, i);
		}
		return 1;
	}
	GetCell(col, row, &cell);
	VtAccount* acnt = (VtAccount*)cell.Tag();
	double num = cell.GetNumber();
	if (num == 1.0) {
		if (acnt)
			acnt->Enable(true);
	}
	else {
		if (acnt)
			acnt->Enable(false);
	}
	return 1;
}

int VtAccountPasswordGrid::OnEditStart(int col, long row, CWnd **edit)
{
	_EdittingRow = row;
	CUGCell cell;
	GetCellIndirect(col, row, &cell);
	DWORD dwStyle = 0;

	if (!_ShowPassword) {
		dwStyle = ES_PASSWORD;
	}
	
	if (cell.GetAlignment() & UG_ALIGNRIGHT)
		dwStyle |= ES_RIGHT;
	else if (cell.GetAlignment() & UG_ALIGNCENTER)
		dwStyle |= ES_CENTER;

	if (dwStyle != 0) {
		if (m_myCUGEdit.GetSafeHwnd())
			m_myCUGEdit.DestroyWindow();

		dwStyle |= WS_CHILD | WS_VISIBLE;
		// create the edit control with specified style
		m_myCUGEdit.Create(dwStyle, CRect(0, 0, 0, 0), this, FLEXEDIT_ID);
		m_myCUGEdit.m_ctrl = this;

		*edit = &m_myCUGEdit;
	}

	return TRUE;
}

int VtAccountPasswordGrid::OnEditVerify(int col, long row, CWnd *edit, UINT *vcKey)
{
	return TRUE;
}

int VtAccountPasswordGrid::OnEditFinish(int col, long row, CWnd *edit, LPCTSTR string, BOOL cancelFlag)
{
	if (_EdittingRow != row)
		return FALSE;

	CString tempStr(string);
	CUGCell cell;
	GetCell(col, row, &cell);
	
	if (_ShowPassword) {
		cell.SetText(tempStr);
	} else {
		CString tempStr2('*', tempStr.GetLength());
		cell.SetText(tempStr2);
	}
	SetCell(col, row, &cell);
	RedrawCell(col, row);
	
	CUGCell indexCell;
	GetCell(0, row, &indexCell);
	VtAccount* acnt = (VtAccount*)indexCell.Tag();
	if (acnt) {
		acnt->TempPassword = (LPCTSTR)tempStr;
	}

	std::pair<VtAccount*, std::string>& item = _CellAcntMap[std::make_pair(0, row)];
	if (item.first) {
		std::string& pwd = item.second;
		pwd = (LPCTSTR)tempStr;
	}
	return TRUE;
}

int VtAccountPasswordGrid::OnEditContinue(int oldcol, long oldrow, int* newcol, long* newrow)
{
	return FALSE;
}

void VtAccountPasswordGrid::SetColTitle()
{
	const int ColCount = _ColCount;
	CUGCell cell;
	LPCTSTR title[4] = { "", "계좌명", "계좌번호", "비밀번호" };
	int colWidth[4] = { 25, 125, 130, 53 };


	for (int i = 0; i < _ColCount; i++) {
		SetColWidth(i, colWidth[i]);
		QuickSetText(i, -1, title[i]);
		GetCell(i, -1, &cell);
		cell.SetBackColor(RGB(70, 130, 180));
		cell.SetTextColor(RGB(255, 255, 255));
		SetCell(i, -1, &cell);
		QuickSetFont(i, -1, &_titleFont);
		QuickSetAlignment(i, -1, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		RedrawCell(i, -1);
	}

	QuickSetNumber(0, -1, 1.0);
	QuickSetCellType(0, -1, UGCT_CHECKBOX);
	QuickSetCellTypeEx(0, -1, UGCT_CHECKBOXFLAT | UGCT_CHECKBOXCHECKMARK);

}

void VtAccountPasswordGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtAccountPasswordGrid::InitGrid()
{
	if (_FromServer) {
		VtGlobal* global = VtGlobal::GetInstance();
		std::map<std::string, VtAccount*>& acntMap = global->AcntList;
		InitGrid(acntMap);
	}
	else {
		VtAccountManager* acntMgr = VtAccountManager::GetInstance();
		std::map<std::string, VtAccount*>& acntMap = acntMgr->GetAccountMap();
		InitGrid(acntMap);
	}
}

void VtAccountPasswordGrid::InitGrid(std::map<std::string, VtAccount*>& acntMap)
{
	CUGCell cell;
	int i = 0;
	for (auto it = acntMap.begin(); it != acntMap.end(); ++it) {
		VtAccount* acnt = it->second;
		acnt->TempPassword = acnt->Password;

		GetCell(0, i, &cell);
		cell.Tag(acnt);
		acnt->Enable() ? cell.SetNumber(1.0) : cell.SetNumber(0.0);
		cell.SetCellType(UGCT_CHECKBOX);
		cell.SetCellTypeEx(UGCT_CHECKBOXFLAT | UGCT_CHECKBOXCHECKMARK);
		SetCell(0, i, &cell);
		_CellAcntMap[std::make_pair(0, i)] = std::make_pair(acnt, acnt->Password);
		QuickSetText(1, i, acnt->AccountName.c_str());
		QuickSetText(2, i, acnt->AccountNo.c_str());
		GetCell(3, i, &cell);
		if (_ShowPassword) {
			cell.SetText(acnt->Password.c_str());
		}
		else {
			CString tempStr('*', acnt->Password.length());
			cell.SetText(tempStr);
		}
		SetCell(3, i, &cell);
		QuickRedrawCell(0, i);
		QuickRedrawCell(1, i);
		QuickRedrawCell(2, i);
		QuickRedrawCell(3, i);
		i++;
	}
}

void VtAccountPasswordGrid::ShowPassword(bool show)
{
	_ShowPassword = show;
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	int i = 0;
	CUGCell cell;
	for (auto it = acntMgr->AccountMap.begin(); it != acntMgr->AccountMap.end(); ++it) {
		VtAccount* acnt = it->second;
		GetCell(3, i, &cell);
		std::pair<VtAccount*, std::string> item = _CellAcntMap[std::make_pair(0, i)];
		if (item.first) {
			if (show) {
				//cell.SetText(acnt->Password.c_str());
				cell.SetText(item.second.c_str());
			}
			else {
				//CString tempStr('*', acnt->Password.length());
				CString tempStr('*', item.second.length());
				cell.SetText(tempStr);
			}
		}
		SetCell(3, i, &cell);
		QuickRedrawCell(3, i);
		i++;
	}
}

void VtAccountPasswordGrid::SavePassword()
{
	for (auto it = _CellAcntMap.begin(); it != _CellAcntMap.end(); ++it) {
		std::pair<VtAccount*, std::string>& item = it->second;
		// 계좌가 있고 활성화 되어 있을 때만 비번을 저장한다.
		if (item.first && item.first->Enable()) {
			item.first->Password = item.second;
		}
	}
}
