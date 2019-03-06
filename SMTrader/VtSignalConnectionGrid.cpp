#include "stdafx.h"
#include "VtSignalConnectionGrid.h"
#include "VtGlobal.h"
#include "VtOutSignalDefManager.h"
#include "VtAccount.h"
#include "VtFund.h"
#include "VtAccountFundSelector.h"
#include "HdSymbolSelecter.h"
#include "System/VtSystem.h"
#include "VtSymbol.h"
#include "VtOutSignalDef.h"
#include "VtOutSystemOrderManager.h"
#include "VtTotalSignalGrid.h"
#include "VtOutSignalDefManager.h"
#include "VtSymbol.h"

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

	VtOutSignalDefManager* outSigDefMgr = VtOutSignalDefManager::GetInstance();
	CUGCell cell;
	for (int yIndex = 0; yIndex < _RowCount; yIndex++) {
		for (int xIndex = 0; xIndex < _ColCount; xIndex++) {
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);
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

void VtSignalConnectionGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (_ClickedRow == row)
		return;

	if (_ClickedRow >= 0) {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, _ClickedRow, RGB(255, 255, 255));
			QuickRedrawCell(i, _ClickedRow);
		}
	}
	for (int i = 0; i < _ColCount; ++i) {
		QuickSetBackColor(i, row, _ClickedColor);
		QuickRedrawCell(i, row);
	}
	_ClickedRow = row;
}

void VtSignalConnectionGrid::OnRClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (_ClickedRow == row)
		return;

	if (_ClickedRow >= 0) {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, _ClickedRow, RGB(255, 255, 255));
			QuickRedrawCell(i, _ClickedRow);
		}
	}
	for (int i = 0; i < _ColCount; ++i) {
		QuickSetBackColor(i, row, _ClickedColor);
		QuickRedrawCell(i, row);
	}
	_ClickedRow = row;
}

void VtSignalConnectionGrid::OnMouseMove(int col, long row, POINT *point, UINT nFlags, BOOL processed /*= 0*/)
{
	if (_OldSelRow == row)
		return;

	if (_OldSelRow != _ClickedRow && _OldSelRow >= 0) {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, _OldSelRow, RGB(255, 255, 255));
			QuickRedrawCell(i, _OldSelRow);
		}
	}

	if (row != _ClickedRow) {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, row, _SelColor);
			QuickRedrawCell(i, row);
		}
	}
	else {
		for (int i = 0; i < _ColCount; ++i) {
			QuickSetBackColor(i, row, _ClickedColor);
			QuickRedrawCell(i, row);
		}
	}

	_OldSelRow = row;
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


void VtSignalConnectionGrid::OnMouseLeaveFromMainGrid()
{
	if (_OldSelRow == _ClickedRow)
		return;

	for (int i = 0; i < _ColCount; ++i) {
		QuickSetBackColor(i, _OldSelRow, RGB(255, 255, 255));
		QuickRedrawCell(i, _OldSelRow);
	}

	_OldSelRow = -2;
}

void VtSignalConnectionGrid::SetColTitle()
{
	CUGCell cell;
	LPCTSTR title[5] = { "실행", "계좌번호", "종목", "신호", "승수" };
	int colWidth[5] = { 25, 98, 90, 80, 60 };


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
	_SystemMap.clear();
	int yIndex = 0;
	CUGCell cell;
	VtOutSignalDefManager* outSigDefMgr = VtOutSignalDefManager::GetInstance();
	VtOutSystemManager* outSysMgr = VtOutSystemManager::GetInstance();
	SharedSystemVec& outSysMap = outSysMgr->GetSysMap();
	for (auto it = outSysMap.begin(); it != outSysMap.end(); ++it) {
		SharedSystem sys = *it;
		for (int xIndex = 0; xIndex < _ColCount; ++xIndex) {
			if (xIndex == 0) {
				GetCell(xIndex, yIndex, &cell);
				sys->Enable() ? cell.SetNumber(1.0) : cell.SetNumber(0.0);
				cell.SetLabelText(_T(""));
				cell.SetCellType(UGCT_CHECKBOX);
				cell.SetCellTypeEx(UGCT_CHECKBOXFLAT | UGCT_CHECKBOXCHECKMARK);
				SetCell(xIndex, yIndex, &cell);
			}
			else if (xIndex == 1) {
				GetCell(xIndex, yIndex, &cell);
				if (sys->SysTargetType() == TargetType::RealAccount || sys->SysTargetType() == TargetType::SubAccount) {
					if (sys->Account()) cell.SetText(sys->Account()->AccountNo.c_str());
				}
				else {
					if (sys->Fund()) cell.SetText(sys->Fund()->Name.c_str());
				}
				cell.SetCellType(m_nEllipsisIndex);
				cell.SetCellTypeEx(UGCT_NORMALELLIPSIS);
				cell.SetParam(ELLIPSISBUTTON_CLICK_ACNT);
				SetCell(xIndex, yIndex, &cell);
			}
			else if (xIndex == 2) {
				GetCell(xIndex, yIndex, &cell);
				if (sys->Symbol()) cell.SetText(sys->Symbol()->ShortCode.c_str());
				cell.SetCellType(m_nEllipsisIndex);
				cell.SetCellTypeEx(UGCT_NORMALELLIPSIS);
				cell.SetParam(ELLIPSISBUTTON_CLICK_PRDT);
				SetCell(xIndex, yIndex, &cell);
			}
			else if (xIndex == 3) {
				GetCell(xIndex, yIndex, &cell);
				if (sys->OutSignal())  cell.SetText(sys->OutSignal()->SignalName.c_str());
				cell.SetCellType(UGCT_DROPLIST);
				cell.SetCellTypeEx(UGCT_DROPLISTHIDEBUTTON);
				cell.SetReadOnly(FALSE);
				cell.SetLabelText(outSigDefMgr->GetOutSigDefString().c_str());
				SetCell(xIndex, yIndex, &cell);
			}
			else if (xIndex == 4) {
				GetCell(xIndex, yIndex, &cell);
				cell.SetNumber(sys->SeungSu());
				cell.SetCellType(m_nSpinIndex);
				cell.SetParam(SPIN_TYPE_SEUNGSU);
				SetCell(xIndex, yIndex, &cell);
			}
			QuickRedrawCell(xIndex, yIndex);
		}
		_SystemMap[yIndex] = sys;
		yIndex++;
	}
	_OccupiedRowCount = yIndex;
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
			cell.SetCellType(UGCT_NORMAL);
			SetCell(j, i, &cell);
		}
	}
}

void VtSignalConnectionGrid::SetTargetAcntOrFund(std::tuple<int, VtAccount*, VtFund*>& selItem)
{
	CUGCell cell;
	GetCell(1, _ButtonRow, &cell);
	int _Type = std::get<0>(selItem);
	SharedSystem sys = _SystemMap[_ButtonRow];
	cell.LongValue(_Type);
	if (_Type == 0 || _Type == 1) {
		VtAccount* _Account = std::get<1>(selItem);
		cell.SetText(_Account->AccountNo.c_str());
		cell.Tag(_Account);
		if (sys) {
			sys->Account(_Account);
		}
	}
	else {
		VtFund* _Fund = std::get<2>(selItem);
		cell.SetText(_Fund->Name.c_str());
		cell.Tag(_Fund);
		if (sys) {
			sys->Fund(_Fund);
		}
	}
	SetCell(1, _ButtonRow, &cell);
	QuickRedrawCell(1, _ButtonRow);
}

void VtSignalConnectionGrid::SetSymbol(VtSymbol* sym)
{
	if (!sym)
		return;
	SharedSystem sys = _SystemMap[_ButtonRow];
	if (sys) {
		CUGCell cell;
		GetCell(2, _ButtonRow, &cell);
		cell.SetText(sym->ShortCode.c_str());
		cell.Tag(sym);
		SetCell(2, _ButtonRow, &cell);
		sys->Symbol(sym);
		QuickRedrawCell(2, _ButtonRow);
	}
}

void VtSignalConnectionGrid::AddSystem(SharedSystem sys)
{
	VtOutSystemManager* outSysMgr = VtOutSystemManager::GetInstance();
	VtOutSignalDefManager* outSigDefMgr = VtOutSignalDefManager::GetInstance();
	CUGCell cell;
	int yIndex = outSysMgr->GetSysMap().size();
	for (int xIndex = 0; xIndex < _ColCount; ++xIndex) {
		if (xIndex == 0) {
			GetCell(xIndex, yIndex, &cell);
			sys->Enable() ? cell.SetNumber(1.0) : cell.SetNumber(0.0);
			cell.SetLabelText(_T(""));
			cell.SetCellType(UGCT_CHECKBOX);
			cell.SetCellTypeEx(UGCT_CHECKBOXFLAT | UGCT_CHECKBOXCHECKMARK);
			SetCell(xIndex, yIndex, &cell);
		}
		else if (xIndex == 1) {
			GetCell(xIndex, yIndex, &cell);
			if (sys->SysTargetType() == TargetType::RealAccount || sys->SysTargetType() == TargetType::SubAccount) {
				if (sys->Account()) cell.SetText(sys->Account()->AccountNo.c_str());
			}
			else {
				if (sys->Fund()) cell.SetText(sys->Fund()->Name.c_str());
			}
			cell.SetCellType(m_nEllipsisIndex);
			cell.SetCellTypeEx(UGCT_NORMALELLIPSIS);
			cell.SetParam(ELLIPSISBUTTON_CLICK_ACNT);
			SetCell(xIndex, yIndex, &cell);
		}
		else if (xIndex == 2) {
			GetCell(xIndex, yIndex, &cell);
			if (sys->Symbol()) cell.SetText(sys->Symbol()->ShortCode.c_str());
			cell.SetCellType(m_nEllipsisIndex);
			cell.SetCellTypeEx(UGCT_NORMALELLIPSIS);
			cell.SetParam(ELLIPSISBUTTON_CLICK_PRDT);
			SetCell(xIndex, yIndex, &cell);
		}
		else if (xIndex == 3) {
			GetCell(xIndex, yIndex, &cell);
			if (sys->OutSignal())  cell.SetText(sys->OutSignal()->SignalName.c_str());
			cell.SetCellType(UGCT_DROPLIST);
			cell.SetCellTypeEx(UGCT_DROPLISTHIDEBUTTON);
			cell.SetReadOnly(FALSE);
			cell.SetLabelText(outSigDefMgr->GetOutSigDefString().c_str());
			SetCell(xIndex, yIndex, &cell);
		}
		else if (xIndex == 4) {
			GetCell(xIndex, yIndex, &cell);
			cell.SetNumber(sys->SeungSu());
			cell.SetCellType(m_nSpinIndex);
			cell.SetParam(SPIN_TYPE_SEUNGSU);
			SetCell(xIndex, yIndex, &cell);
		}
		QuickRedrawCell(xIndex, yIndex);
	}
	_SystemMap[yIndex] = sys;
	outSysMgr->AddSystem(sys);
	_OccupiedRowCount = yIndex;
}

void VtSignalConnectionGrid::RemoveSystem()
{
	int row = _ClickedRow;
	auto it = _SystemMap.find(row);
	if (it != _SystemMap.end()) {
		SharedSystem sys = it->second;
		// 시스템을 정지 시킨다.
		sys->Enable(false);
		VtOutSystemOrderManager* outSysOrderMgr = VtOutSystemOrderManager::GetInstance();
		// 주문관리자에서 삭제한다.
		outSysOrderMgr->RemoveSystem(sys);
		// 주문상태 목록을 리프레쉬 한다.
		if (_TotalGrid) _TotalGrid->Refresh();

		// 시스템 목록에서 삭제한다.
		VtOutSystemManager* outSysMgr = VtOutSystemManager::GetInstance();
		outSysMgr->RemoveSystem(sys->Id());

		// 모든 셀 정보를 초기화 시킨다.
		CUGCell cell;
		for (int i = 0; i < _ColCount; ++i) {
			GetCell(i, row, &cell);
			cell.SetCellType(UGCT_NORMAL);
			cell.SetText(_T(""));
			SetCell(i, row, &cell);
			QuickRedrawCell(i, row);
		}
	}
}

void VtSignalConnectionGrid::Refresh()
{
	ClearCells();
	InitGrid();
}

int VtSignalConnectionGrid::OnDropList(long ID, int col, long row, long msg, long param)
{
	if (msg == UGCT_DROPLISTSTART) {
		CStringList* pList = (CStringList*)param;
		pList->RemoveAll();
		VtOutSignalDefManager* outSigDefMgr = VtOutSignalDefManager::GetInstance();
		OutSigDefVec& sigDefVec = outSigDefMgr->GetSignalDefVec();
		for (size_t loop = 0; loop < sigDefVec.size(); loop++) {
			SharedOutSigDef sig = sigDefVec[loop];
			pList->AddTail(sig->SignalName.c_str());
		}
	}
	if (msg == UGCT_DROPLISTSELECT) {
		VtOutSignalDefManager* outSigMgr = VtOutSignalDefManager::GetInstance();
		CString * pString = (CString*)param;
		std::string sigName = *pString;
		SharedOutSigDef sig = outSigMgr->FindOutSigDef(sigName);
		SharedSystem sys = _SystemMap[row];
		if (sig && sys) {
			sys->OutSignal(sig);
			CUGCell cell;
			GetCell(col, row, &cell);
			cell.SetText(sig->SignalName.c_str());
			QuickRedrawCell(col, row);
		}
	}
	
	return TRUE;
}

int VtSignalConnectionGrid::OnCheckbox(long ID, int col, long row, long msg, long param)
{
	VtOutSystemOrderManager* outSysOrderMgr = VtOutSystemOrderManager::GetInstance();
	SharedSystem sys = _SystemMap[row];
	if (sys) {
		CUGCell cell;
		GetCell(col, row, &cell);
		double num = cell.GetNumber();
		if (num == 1.0) {
			sys->Enable(true);
			outSysOrderMgr->AddSystem(sys);
		}
		else {
			sys->Enable(false);
			outSysOrderMgr->RemoveSystem(sys);
		}
		if (_TotalGrid) _TotalGrid->Refresh();
	}

	return TRUE;
}

int VtSignalConnectionGrid::OnEllipsisButton(long ID, int col, long row, long msg, long param)
{
	CUGCell cell;
	GetCell(col, row, &cell);
	int nCellTypeIndex = cell.GetCellType();
	int nParam = cell.GetParam();

	if (msg == UGCT_ELLIPSISBUTTONCLICK) {
		if (nParam == ELLIPSISBUTTON_CLICK_ACNT) {
			_ButtonRow = row;
			VtAccountFundSelector dlg;
			dlg.SetSignalConnectionGrid(this);
			dlg.DoModal();
		}
		else if (nParam == ELLIPSISBUTTON_CLICK_PRDT) {
			_ButtonRow = row;
			HdSymbolSelecter dlg;
			dlg.SetSigConGrid(this);
			dlg.DoModal();
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
