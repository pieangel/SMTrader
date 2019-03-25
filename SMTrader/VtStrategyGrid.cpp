#include "stdafx.h"
#include "VtStrategyGrid.h"
#include <string>
#include "VtSystemGroupManager.h"
#include "VtSystemGroup.h"
#include "System/VtSystem.h"
#include "System/VtSystemManager.h"
#include "resource.h"
#include "VtStrategyToolWnd.h"
#include "VtSystemProperty.h"
#include "VtUsdStrategyConfigDlg.h"
#include "VtStrategyWndManager.h"

VtStrategyGrid::VtStrategyGrid()
{
	_ToolWnd = nullptr;
}


VtStrategyGrid::~VtStrategyGrid()
{
}

void VtStrategyGrid::OnSetup()
{
	SetUGMode(UGMode::OrderNormal);

	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_SetBitmap.LoadBitmap(MAKEINTRESOURCE(IDB_BITMAP_TOOL2));
	m_Pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

	_ColWidthMap.push_back(95);
	_ColWidthMap.push_back(24);

	m_nButtonIndex = AddCellType(&m_button);
	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);
	SetMargin(0);

	CUGCell cell;
	for (int yIndex = 0; yIndex < _RowCount; yIndex++) {
		SetRowHeight(yIndex, _HeadHeight);
		for (int xIndex = 0; xIndex < _ColCount; xIndex++) {
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		}
	}

	SetColWidth(0, _ColWidthMap[0]);
	SetColWidth(1, _ColWidthMap[1]);

	SetDefFont(&_defFont);
	SetSH_Width(0);
	SetTH_Height(0);
	SetVS_Width(0);
	SetHS_Height(0);

	EnableMenu(TRUE);

	InitGrid();

	ResizeWindow();
}

int VtStrategyGrid::OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow)
{
	return 0;
}

int VtStrategyGrid::OnCellTypeNotify(long ID, int col, long row, long msg, long param)
{
	if (ID == UGCT_CHECKBOX) {
		return OnCheckbox(ID, col, row, msg, param);
	}

	if (ID == m_nButtonIndex) {
		return OnPushButton(ID, col, row, msg, param);
	}

	return 0;
}

int VtStrategyGrid::OnCheckbox(long ID, int col, long row, long msg, long param)
{
	VtSystem* sys = _SystemMap[row];
	if (sys) {
		CUGCell cell;
		GetCell(col, row, &cell);
		double num = cell.GetNumber();
		if (num == 1.0) {
			sys->Enable(true);
			sys->ShowRealtime(true);
			sys->SetDataSrc();
			sys->RegisterRealtimeAccountEvent();
		}
		else {
			sys->Enable(false);
			sys->ShowRealtime(false);
		}
		UpdateDialog(sys);
	}
	return 1;
}

int VtStrategyGrid::OnPushButton(long ID, int col, long row, long msg, long param)
{
	if (msg == UGCT_BUTTONDOWN) {
		auto it = _CatMap.find(row);
		if (it != _CatMap.end()) {
			auto& rowItem = it->second;
			auto& expanded = rowItem.first;
			int rowHeight = expanded ? 0 : _HeadHeight;
			expanded = !expanded;
			auto& rlist = rowItem.second;
			for (auto itr = rlist.begin(); itr != rlist.end(); ++itr) {
				SetRowHeight(*itr, rowHeight);
				_HeightMap[*itr] = rowHeight;
			}
			RedrawAll();
			ResizeWindow();
		}
		else {
			auto it = _SystemMap.find(row);
			if (it != _SystemMap.end()) {
				VtSystem* sys = it->second;
				VtUsdStrategyConfigDlg* dlg = new VtUsdStrategyConfigDlg();
				dlg->System(sys);
				dlg->Create(IDD_SYSTEM_USD, this);
				dlg->StGrid(this);
				dlg->ShowWindow(SW_SHOW);
				_DlgMap[dlg] = dlg;
			}
		}
	}
	return 1;
}

void VtStrategyGrid::SetRowTitle()
{
	
}

void VtStrategyGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtStrategyGrid::RemoveDlg(VtUsdStrategyConfigDlg* dlg)
{
	auto it = _DlgMap.find(dlg);
	if (it != _DlgMap.end())
		_DlgMap.erase(it);
}

void VtStrategyGrid::InitGrid()
{
	VtSystemGroupManager* straCatMgr = VtSystemGroupManager::GetInstance();
	std::vector<VtSystemGroup>& catList = straCatMgr->GetSystemGroupVec();
	std::vector<COLORREF> _CatColor;
	_CatColor.push_back(RGB(19, 137, 255));
	_CatColor.push_back(RGB(240, 51, 58));
	_CatColor.push_back(RGB(22, 159, 79));
	_CatColor.push_back(RGB(193, 11, 232));
	int i = 0, c = 0;
	for (auto it = catList.begin(); it != catList.end(); ++it) {
		VtSystemGroup& cat = *it;
		JoinCells(0, i, 1, i);
		//std::string catTitle = cat.Name().substr(2, cat.Name().length() - 2);
		QuickSetText(0, i, cat.Name().c_str());
		QuickSetTextColor(0, i, RGB(255, 255, 255));
		QuickSetBackColor(0, i, _CatColor[c++]);
		QuickSetCellType(0, i, m_nButtonIndex);
		std::vector<int> rowList;
		_CatMap[i] = std::make_pair(true, rowList);
		_HeightMap[i] = _HeadHeight;
		auto& catItem = _CatMap[i];
		i++;
		std::vector<VtSystem*>& sysList = cat.GetSystemList();
		auto& rlist = catItem.second;
		for (auto its = sysList.begin(); its != sysList.end(); ++its) {
			VtSystem* sys = *its;
			rlist.push_back(i);
			QuickSetLabelText(0, i, sys->Name().c_str());
			QuickSetCellType(0, i, UGCT_CHECKBOX);
			QuickSetCellTypeEx(0, i, UGCT_CHECKBOXFLAT | UGCT_CHECKBOXCHECKMARK);
			_SystemMap[i] = sys;
			_SystemObjectMap[sys->Name()] = std::make_pair(i, sys);
			QuickSetCellType(1, i, m_nButtonIndex);
			QuickSetBitmap(1, i, &_SetBitmap);
			_HeightMap[i] = _HeadHeight;
			i++;
		}
	}
}

void VtStrategyGrid::ResizeWindow()
{
	CRect rcWnd, rcClient;
	GetWindowRect(rcWnd);
	GetClientRect(rcClient);
	int totalHeight = rcWnd.Height() - rcClient.Height();
	for (auto it = _HeightMap.begin(); it != _HeightMap.end(); ++it) {
		totalHeight += it->second;
	}
	int totalWidth = 0;
	for (int i = 0; i < _ColWidthMap.size(); ++i) {
		totalWidth += _ColWidthMap[i];
	}
	if (_ToolWnd)
		_ToolWnd->SetWindowPos(nullptr, 0, 0, totalWidth + 6, totalHeight + 38, SWP_NOMOVE | SWP_NOZORDER);
	SetWindowPos(nullptr, 0, 0, totalWidth, totalHeight, SWP_NOMOVE | SWP_NOZORDER);
	
}

void VtStrategyGrid::DisableAllSystems()
{
	for (auto it = _SystemMap.begin(); it != _SystemMap.end(); ++it) {
		VtSystem* sys = it->second;
		if (sys) {
			sys->ShowRealtime(false);
			sys->Enable(false);
		}
	}
}

void VtStrategyGrid::UpdateSystem(int row, bool enable)
{
	auto it = _SystemMap.find(row);
	if (it != _SystemMap.end()) {
		VtSystem* sys = it->second;
		//sys->Enable(enable);
		//enable ? sys->ShowRealtime(true) : sys->ShowRealtime(false);
		sys->Enable(enable);
		CUGCell cell;
		if (enable) {
			sys->ShowRealtime(true);
			GetCell(0, row, &cell);
			cell.SetNumber(1.0);
			SetCell(0, row, &cell);
		}
		else {
			sys->ShowRealtime(false);
			GetCell(0, row, &cell);
			cell.SetNumber(0.0);
			SetCell(0, row, &cell);
		}

		QuickRedrawCell(0, row);
	}
}

void VtStrategyGrid::UpdateSystem(VtSystem* sys, bool enable)
{
	if (!sys)
		return;
	auto it = _SystemObjectMap.find(sys->Name());
	if (it != _SystemObjectMap.end()) {
		std::pair<int, VtSystem*> item = it->second;
		std::get<1>(item)->Enable(enable);
		CUGCell cell;
		if (enable) {
			sys->ShowRealtime(true);
			GetCell(0, std::get<0>(item), &cell);
			cell.SetNumber(1.0);
			SetCell(0, std::get<0>(item), &cell);
		}
		else {
			sys->ShowRealtime(false);
			GetCell(0, std::get<0>(item), &cell);
			cell.SetNumber(0.0);
			SetCell(0, std::get<0>(item), &cell);
		}

		QuickRedrawCell(0, std::get<0>(item));
	}
}

void VtStrategyGrid::UpdateDialog(VtSystem* sys)
{
	if (!sys)
		return;
// 	for (auto it = _DlgMap.begin(); it != _DlgMap.end(); ++it) {
// 		it->second->UpdateRunCheck(sys);
// 	}
	VtStrategyWndManager* stgWndMgr = VtStrategyWndManager::GetInstance();
	stgWndMgr->UpdateDialog(sys);
}

void VtStrategyGrid::UpdatePosition()
{
	for (auto it = _SystemObjectMap.begin(); it != _SystemObjectMap.end(); ++it) {
		auto item = it->second;
		int row = item.first;
		VtSystem* sys = item.second;

		if (sys->PositionState.Find(_T("매수")) >= 0) {
			QuickSetTextColor(0, row, RGB(255, 255, 255));
			QuickSetBackColor(0, row, RGB(240, 51, 58));
		}
		else if (sys->PositionState.Find(_T("매도")) >= 0) {
			QuickSetTextColor(0, row, RGB(255, 255, 255));
			QuickSetBackColor(0, row, RGB(19, 137, 255));
		}
		else {
			QuickSetTextColor(0, row, RGB(0, 0, 0));
			QuickSetBackColor(0, row, RGB(255, 255, 255));
		}
		QuickRedrawCell(0, row);
	}
}
