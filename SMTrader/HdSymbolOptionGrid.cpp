#include "stdafx.h"
#include "HdSymbolOptionGrid.h"
#include "VtProductCategoryManager.h"
#include "VtProductSection.h"
#include "VtProductSubSection.h"
#include "VtSymbol.h"
#include "VtOrderConfigManager.h"
#include "Poco/NumberFormatter.h"
#include "VtOrderLeftWndHd.h"
#include "VtOptionMonthSection.h"
#include "VtSymbolManager.h"
#include "VtOrderCenterWndHd.h"
#include "VtPosition.h"
#include "VtAccount.h"
#include "VtOrder.h"
#include "VtOrderManager.h"
#include "VtOrderManagerSelector.h"
#include "VtProductOrderManagerSelector.h"
#include "VtProductOrderManager.h"
#include "VtFund.h"
#include "VtRealtimeRegisterManager.h"
#include "VtGlobal.h"
#include "SmOrderPanel.h"
#include "VtOrderWndHd.h"
using Poco::NumberFormatter;

HdSymbolOptionGrid::HdSymbolOptionGrid()
{
	_LeftWnd = nullptr;
	_CurPrdtSec = nullptr;
	_OrderConfigMgr = nullptr;
	_EqualCol = -2;
	_EqualRow = -2;
	_Mode = 1;
}


HdSymbolOptionGrid::~HdSymbolOptionGrid()
{
}

void HdSymbolOptionGrid::OnSetup()
{
	m_Pen.CreatePen(PS_SOLID, 1, RGB(190, 190, 190));
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

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
	SetTH_Height(22);
	LockColumns(0);
	LockColumns(2);

	QuickSetAlignment(0, 0, UG_ALIGNCENTER | UG_ALIGNVCENTER);
	for (int i = 0; i < _RowCount; i++)
	{
		SetRowHeight(i, DefaultCellHeight);
		QuickSetAlignment(0, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		QuickSetAlignment(1, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		QuickSetAlignment(2, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		QuickSetBackColor(0, i, RGB(252, 226, 228));
		QuickSetBackColor(2, i, RGB(218, 226, 245));
		QuickSetBorder(0, i, UG_BDR_BTHIN);
		QuickSetBorderColor(0, i, &m_Pen);
		QuickSetBorder(1, i, UG_BDR_BTHIN);
		QuickSetBorderColor(1, i, &m_Pen);
		QuickSetBorder(2, i, UG_BDR_BTHIN);
		QuickSetBorderColor(2, i, &m_Pen);
	}

	SetDefFont(&_defFont);


	SetColTitle();

	InitSymbol();
	InitYearMonth();
	InitGrid();
	//GetSymbolMaster();
}

void HdSymbolOptionGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

void HdSymbolOptionGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (updn == TRUE)
	{
		CUGCell cell;
		GetCell(col, row, &cell);
		VtSymbol* sym = (VtSymbol*)cell.Tag();
		if (sym)
		{
			sym->GetSymbolMaster();
			if (_OrderConfigMgr->_HdCenterWnd)
			{
				_OrderConfigMgr->Symbol(sym);
				_OrderConfigMgr->_HdCenterWnd->ChangeSymbol(sym);
			}
		}
	}
}

void HdSymbolOptionGrid::SetColTitle()
{
	CUGCell cell;
	LPCTSTR title[3] = { "Call", "행사가", "Put" };
	int colWidth[9] = { 60, 50, 60 };

	for (int i = 0; i < _ColCount; i++)
	{
		SetColWidth(i, colWidth[i]);
		QuickSetText(i, -1, title[i]);
		GetCell(i, -1, &cell);
		if (i == 0) {
			cell.SetTextColor(RGB(255, 255, 255));
			cell.SetBackColor(RGB(240, 51, 58));
		}
		else if (i == 2) {
			cell.SetTextColor(RGB(255, 255, 255));
			cell.SetBackColor(RGB(19, 137, 255));
		}
		else {
			cell.SetBackColor(VtGlobal::GridTitleBackColor);
			cell.SetTextColor(VtGlobal::GridTitleTextColor);
		}
		SetCell(i, -1, &cell);
		QuickSetFont(i, -1, &_titleFont);
		RedrawCell(i, -1);
	}
}

void HdSymbolOptionGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void HdSymbolOptionGrid::Begin()
{

}

void HdSymbolOptionGrid::End()
{

}

void HdSymbolOptionGrid::InitGrid()
{
	if (!_LeftWnd)
		return;
	if (!_CurPrdtSec)
		return;

	ResetRemainCells();

	QuickSetBackColor(_EqualCol, _EqualRow, RGB(255, 255, 255));

	int selMon = _LeftWnd->_ComboOptionMonth.GetCurSel();
	if (selMon == -1 || _CurPrdtSec->SubSectionVector.size() == 0)
		return;

	int eIndex = 0;
	int delta = 0;
	int minVal = 1000000;

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	CString curYearMon;
	CUGCell cell, centerCell;
	_LeftWnd->_ComboOptionMonth.GetLBText(selMon, curYearMon);
	VtProductSubSection* callSec = _CurPrdtSec->SubSectionVector[0];
	VtOptionMonthSection* opSec = callSec->FindOptionMap((LPCTSTR)curYearMon);
	if (opSec) {
		int i = 0;
		for (auto it = opSec->_SymbolVector.begin(); it != opSec->_SymbolVector.end(); ++it) {
			VtSymbol* sym = *it;
			GetCell(0, i, &cell);
			cell.Tag(sym);
			SetCell(0, i, &cell);
			QuickRedrawCell(0, i);

			_CallSymbolRowMap[sym->ShortCode] = std::make_tuple(0, i, sym);

			GetCell(1, i, &centerCell);
			std::string centerVal = sym->ShortCode.substr(5, 3);
			char centerTip = sym->ShortCode.at(7);
			int intCenter = std::stoi(centerVal) * static_cast<int>(std::pow(10, sym->Decimal));
			if (centerTip == '2' || centerTip == '7')
				intCenter += 50;
			delta = std::abs(symMgr->Kospi200Current - intCenter);
			if (delta < minVal) {
				minVal = delta;
				eIndex = i;
			}

			centerCell.SetNumberDecimals(sym->Decimal);
			centerCell.SetNumber(intCenter / std::pow(10, sym->Decimal));
			SetCell(1, i, &centerCell);

			QuickRedrawCell(1, i);
			i++;
		}

		SetEqualRow(eIndex);
		QuickSetBackColor(1, eIndex, RGB(255, 255, 0));
		QuickRedrawCell(1, eIndex);

		_EqualCol = 1;
		_EqualRow = eIndex;
	}

	VtProductSubSection* putSec = _CurPrdtSec->SubSectionVector[1];
	opSec = putSec->FindOptionMap((LPCTSTR)curYearMon);
	if (opSec) {
		int i = 0;
		for (auto it = opSec->_SymbolVector.begin(); it != opSec->_SymbolVector.end(); ++it) {
			VtSymbol* sym = *it;
			GetCell(2, i, &cell);
			cell.Tag(sym);
			SetCell(2, i, &cell);
			QuickRedrawCell(2, i);
			_PutSymbolRowMap[sym->ShortCode] = std::make_tuple(2, i, sym);
			i++;
		}
	}

	if (_Mode == 0)
		SetRemain();
	else if (_Mode == 1)
		SetCurrent();
	else
		SetExpected();
}

void HdSymbolOptionGrid::InitSymbol()
{
	if (!_LeftWnd)
		return;

	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	for (auto it = prdtCatMgr->MainOptionVector.begin(); it != prdtCatMgr->MainOptionVector.end(); ++it)
	{
		std::string secName = *it;
		VtProductSection* section = prdtCatMgr->FindProductSection(secName);
		if (section)
		{
			int index = _LeftWnd->_ComboProduct.AddString(section->Name.c_str());
			_LeftWnd->_ComboProduct.SetItemDataPtr(index, section);
		}
	}

	_LeftWnd->_ComboProduct.SetCurSel(0);
	_CurPrdtSec = (VtProductSection*)_LeftWnd->_ComboProduct.GetItemDataPtr(0);
}

void HdSymbolOptionGrid::InitYearMonth()
{
	if (!_LeftWnd)
		return;

	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	for (auto it = prdtCatMgr->MainOptionVector.begin(); it != prdtCatMgr->MainOptionVector.end(); ++it)
	{
		std::string secName = *it;
		VtProductSection* section = prdtCatMgr->FindProductSection(secName);
		if (section)
		{
			_CurPrdtSec = section;
			SetYearMonth();
			break;
		}
	}

	_LeftWnd->_ComboOptionMonth.SetCurSel(0);
}

void HdSymbolOptionGrid::SetYearMonth()
{
	if (!_CurPrdtSec)
		return;

	_LeftWnd->_ComboOptionMonth.ResetContent();
	VtProductSubSection* subSection = _CurPrdtSec->SubSectionVector.front();
	for (auto it = subSection->_OptionMap.begin(); it != subSection->_OptionMap.end(); ++it)
	{
		std::string code = it->first;

		CString yearMon(code.c_str());
		int index = _LeftWnd->_ComboOptionMonth.AddString(yearMon);
		//_LeftWnd->_ComboOption.SetItemDataPtr(index, code.c_str());
	}
	_LeftWnd->_ComboOptionMonth.SetCurSel(0);
}

void HdSymbolOptionGrid::SetProductSection()
{
	if (!_LeftWnd)
		return;
	int selPrdt = _LeftWnd->_ComboProduct.GetCurSel();
	if (selPrdt == -1)
		return;
	_CurPrdtSec = (VtProductSection*)_LeftWnd->_ComboProduct.GetItemDataPtr(selPrdt);
}

void HdSymbolOptionGrid::GetSymbolMaster()
{
	if (_EqualRow >= 0) {
		GetSymbolMasterByCenter();
		return;
	}
	if (!_CurPrdtSec)
		return;

	int selMon = _LeftWnd->_ComboOptionMonth.GetCurSel();
	if (selMon == -1 || _CurPrdtSec->SubSectionVector.size() == 0)
		return;
	CString curYearMon;
	_LeftWnd->_ComboOptionMonth.GetLBText(selMon, curYearMon);
	VtProductSubSection* callSec = _CurPrdtSec->SubSectionVector[0];
	VtOptionMonthSection* opSec = callSec->FindOptionMap((LPCTSTR)curYearMon);
	if (opSec)
	{
		for (auto it = opSec->_SymbolVector.begin(); it != opSec->_SymbolVector.end(); ++it)
		{
			VtSymbol* sym = *it;
			if (sym->Quote.intClose == 0)
				sym->GetSymbolMaster();
		}
	}
	VtProductSubSection* putSec = _CurPrdtSec->SubSectionVector[1];
	opSec = putSec->FindOptionMap((LPCTSTR)curYearMon);
	if (opSec)
	{
		for (auto it = opSec->_SymbolVector.begin(); it != opSec->_SymbolVector.end(); ++it)
		{
			VtSymbol* sym = *it;
			if (sym->Quote.intClose == 0)
				sym->GetSymbolMaster();
		}
	}
}

void HdSymbolOptionGrid::GetSymbolMasterByCenter()
{
	int centerRow = _EqualRow;

	if (!_CurPrdtSec)
		return;

	int selMon = _LeftWnd->_ComboOptionMonth.GetCurSel();
	if (selMon == -1 || _CurPrdtSec->SubSectionVector.size() == 0)
		return;
	CString curYearMon;
	_LeftWnd->_ComboOptionMonth.GetLBText(selMon, curYearMon);
	VtProductSubSection* callSec = _CurPrdtSec->SubSectionVector[0];
	VtOptionMonthSection* opSec = callSec->FindOptionMap((LPCTSTR)curYearMon);
	if (opSec->RecentMonth() && opSec->SymbolMasterRequested())
		return;
	VtRealtimeRegisterManager* realTimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	if (selMon == 0) {
		opSec->RecentMonth(true);
		opSec->SymbolMasterRequested(true);
		for (auto it = opSec->_SymbolVector.begin(); it != opSec->_SymbolVector.end(); ++it) {
			VtSymbol* sym = *it;
			realTimeRegiMgr->RegisterProduct(sym->ShortCode);
			sym->RecentMonth(true);
		}
	}
	
	VtProductSubSection* putSec = _CurPrdtSec->SubSectionVector[1];
	VtOptionMonthSection* opSec2 = putSec->FindOptionMap((LPCTSTR)curYearMon);
	if (selMon == 0) {
		opSec2->RecentMonth(true);
		opSec2->SymbolMasterRequested(true);
		for (auto it = opSec2->_SymbolVector.begin(); it != opSec2->_SymbolVector.end(); ++it) {
			VtSymbol* sym = *it;
			realTimeRegiMgr->RegisterProduct(sym->ShortCode);
			sym->RecentMonth(true);
		}
	}
	
	if (opSec && opSec2) {
		int startIndex = 0;
		int endIndex = opSec->_SymbolVector.size() - 1;
		int addNum = 0;
		int curIndex = _EqualRow;
		bool upRange = false, downRange = false;
		while (1) {
			if (addNum % 2 == 0)
				curIndex = curIndex + addNum;
			else
				curIndex = curIndex - addNum;

			if (curIndex < startIndex)
				upRange = true;
			if (curIndex > endIndex)
				downRange = true;
			
			if (curIndex >= startIndex && curIndex <= endIndex) {
				VtSymbol* sym = opSec->_SymbolVector[curIndex];
				if (sym->Quote.intClose == 0)
					sym->GetSymbolMaster();
				sym = opSec2->_SymbolVector[curIndex];
				if (sym->Quote.intClose == 0)
					sym->GetSymbolMaster();
			}

			if (upRange && downRange)
				break;
			
			addNum++;
		}
	}
}

void HdSymbolOptionGrid::SetRemain()
{
	if (_Mode != 0 || !_OrderConfigMgr)
		return;

	for (auto it = _CallSymbolRowMap.begin(); it != _CallSymbolRowMap.end(); ++it) {
		auto pos = it->second;
		VtSymbol* sym = std::get<2>(pos);

		if (_OrderConfigMgr->Type() == 0) {
			VtAccount* acnt = _OrderConfigMgr->Account();
			if (!acnt)
				return;

			VtOrderManagerSelector* orderMgrSeledter = VtOrderManagerSelector::GetInstance();
			VtOrderManager* orderMgr = orderMgrSeledter->FindOrderManager(acnt->AccountNo);
			if (!orderMgr)
				return;
			VtProductOrderManager* prdtOrderMgr = orderMgr->GetProductOrderManager(sym->ShortCode);
			VtPosition* posi = acnt->FindPosition(sym->ShortCode);
			if (prdtOrderMgr)
				ShowPosition(prdtOrderMgr->Init(), prdtOrderMgr->GetAcceptedOrderCount(), posi, sym->ShortCode);
			else
				ShowPosition(false, 0, nullptr, sym->ShortCode);
		} else {
			VtFund* fund = _OrderConfigMgr->Fund();
			if (!fund)
				return;

			std::tuple<bool, int> result = fund->GetInitAndAcceptedCount(sym->ShortCode);
			int count = 0;
			VtPosition posi = fund->GetPosition(sym->ShortCode, count);
			if (count != 0)
				ShowPosition(std::get<0>(result), std::get<1>(result), &posi, sym->ShortCode);
			else
				ShowPosition(std::get<0>(result), std::get<1>(result), nullptr, sym->ShortCode);
		}
	}

	for (auto it = _PutSymbolRowMap.begin(); it != _PutSymbolRowMap.end(); ++it) {
		auto pos = it->second;
		VtSymbol* sym = std::get<2>(pos);

		if (_OrderConfigMgr->Type() == 0) {
			VtAccount* acnt = _OrderConfigMgr->Account();
			if (!acnt)
				return;

			VtOrderManagerSelector* orderMgrSeledter = VtOrderManagerSelector::GetInstance();
			VtOrderManager* orderMgr = orderMgrSeledter->FindOrderManager(acnt->AccountNo);
			if (!orderMgr)
				return;
			VtProductOrderManager* prdtOrderMgr = orderMgr->GetProductOrderManager(sym->ShortCode);
			VtPosition* posi = acnt->FindPosition(sym->ShortCode);
			if (prdtOrderMgr)
				ShowPosition(prdtOrderMgr->Init(), prdtOrderMgr->GetAcceptedOrderCount(), posi, sym->ShortCode);
			else
				ShowPosition(false, 0, nullptr, sym->ShortCode);
		} else {
			VtFund* fund = _OrderConfigMgr->Fund();
			if (!fund)
				return;

			std::tuple<bool, int> result = fund->GetInitAndAcceptedCount(sym->ShortCode);
			int count = 0;
			VtPosition posi = fund->GetPosition(sym->ShortCode, count);
			if (count != 0)
				ShowPosition(std::get<0>(result), std::get<1>(result), &posi, sym->ShortCode);
			else
				ShowPosition(std::get<0>(result), std::get<1>(result), nullptr, sym->ShortCode);
		}
	}
}

void HdSymbolOptionGrid::SetRemain(VtPosition* posi)
{
	if (!posi)
		return;

	if (_Mode != 0)
		return;

	if (!_OrderConfigMgr || !_OrderConfigMgr->Account())
		return;

	CUGCell cell;
	auto it = _CallSymbolRowMap.find(posi->ShortCode);
	if (it != _CallSymbolRowMap.end())
	{
		auto pos = _CallSymbolRowMap[posi->ShortCode];


		VtAccount* acnt = _OrderConfigMgr->Account();
		VtCellPos cellPos;

		if (std::abs(posi->OpenQty) > 0)
		{
			QuickSetNumber(std::get<0>(pos), std::get<1>(pos), posi->OpenQty);
			if (acnt->GetAcceptedOrderCount(posi->ShortCode) > 0)
				QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(212, 186, 188));
			else
				QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
		}
		else
		{
			QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
			QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
		}
		cellPos.Col = std::get<0>(pos);
		cellPos.Row = std::get<1>(pos);
		_RemainPos.insert(cellPos);
		QuickRedrawCell(std::get<0>(pos), std::get<1>(pos));
	}

	it = _PutSymbolRowMap.find(posi->ShortCode);
	if (it != _PutSymbolRowMap.end())
	{
		auto pos = _PutSymbolRowMap[posi->ShortCode];

		VtAccount* acnt = _OrderConfigMgr->Account();
		VtCellPos cellPos;

		if (std::abs(posi->OpenQty) > 0)
		{
			QuickSetNumber(std::get<0>(pos), std::get<1>(pos), posi->OpenQty);
			if (acnt->GetAcceptedOrderCount(posi->ShortCode) > 0)
				QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(178, 186, 205));
			else
				QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
		}
		else
		{
			QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
			QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
		}
		cellPos.Col = std::get<0>(pos);
		cellPos.Row = std::get<1>(pos);
		_RemainPos.insert(cellPos);
		QuickRedrawCell(std::get<0>(pos), std::get<1>(pos));
	}
}

void HdSymbolOptionGrid::SetRemain(VtOrder* order)
{
	if (!order || _Mode != 0 || !_OrderConfigMgr)
		return;

	if (_OrderConfigMgr->Type() == 0) {
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (!acnt)
			return;

		VtOrderManagerSelector* orderMgrSeledter = VtOrderManagerSelector::GetInstance();
		VtOrderManager* orderMgr = orderMgrSeledter->FindOrderManager(acnt->AccountNo);
		if (!orderMgr)
			return;
		VtProductOrderManager* prdtOrderMgr = orderMgr->GetProductOrderManager(order->shortCode);
		if (!prdtOrderMgr)
			return;
		VtPosition* posi = acnt->FindPosition(order->shortCode);
		ShowPosition(prdtOrderMgr->Init(), prdtOrderMgr->GetAcceptedOrderCount(), posi, order->shortCode);
	} else {
		VtFund* fund = _OrderConfigMgr->Fund();
		if (!fund)
			return;

		std::tuple<bool, int> result = fund->GetInitAndAcceptedCount(order->shortCode);
		int count = 0;
		VtPosition posi = fund->GetPosition(order->shortCode, count);
		if (count != 0)
			ShowPosition(std::get<0>(result), std::get<1>(result), &posi, order->shortCode);
		else
			ShowPosition(std::get<0>(result), std::get<1>(result), nullptr, order->shortCode);
	}
}

void HdSymbolOptionGrid::SetRemain(VtSymbol* sym)
{
	if (!sym || !_OrderConfigMgr)
		return;

	if (_OrderConfigMgr->Type() == 0) {
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (!acnt)
			return;

		VtOrderManagerSelector* orderMgrSeledter = VtOrderManagerSelector::GetInstance();
		VtOrderManager* orderMgr = orderMgrSeledter->FindOrderManager(acnt->AccountNo);
		if (!orderMgr)
			return;
		VtProductOrderManager* prdtOrderMgr = orderMgr->GetProductOrderManager(sym->ShortCode);
		if (!prdtOrderMgr)
			return;
		VtPosition* posi = acnt->FindPosition(sym->ShortCode);
		ShowPosition(prdtOrderMgr->Init(), prdtOrderMgr->GetAcceptedOrderCount(), posi, sym->ShortCode);
	}
	else {
		VtFund* fund = _OrderConfigMgr->Fund();
		if (!fund)
			return;

		std::tuple<bool, int> result = fund->GetInitAndAcceptedCount(sym->ShortCode);
		int count = 0;
		VtPosition posi = fund->GetPosition(sym->ShortCode, count);
		if (count != 0)
			ShowPosition(std::get<0>(result), std::get<1>(result), &posi, sym->ShortCode);
		else
			ShowPosition(std::get<0>(result), std::get<1>(result), nullptr, sym->ShortCode);
	}
}

int HdSymbolOptionGrid::FindValueStartRow(int height)
{
	int selMon = _LeftWnd->_ComboOptionMonth.GetCurSel();
	if (selMon == -1 || _CurPrdtSec->SubSectionVector.size() == 0)
		return 0;

	int eIndex = 0;
	int delta = 0;
	int minVal = 1000000;

	int startRow = 0;

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	CString curYearMon;
	_LeftWnd->_ComboOptionMonth.GetLBText(selMon, curYearMon);
	VtProductSubSection* callSec = _CurPrdtSec->SubSectionVector[0];
	VtOptionMonthSection* opSec = callSec->FindOptionMap((LPCTSTR)curYearMon);
	if (opSec) {
		for (int i = 0; i < opSec->_SymbolVector.size(); ++i) {
			VtSymbol* sym = opSec->_SymbolVector[i];
			std::string centerVal = sym->ShortCode.substr(5, 3);
			char centerTip = sym->ShortCode.at(7);
			int intCenter = std::stoi(centerVal) * static_cast<int>(std::pow(10, sym->Decimal));
			if (centerTip == '2' || centerTip == '7')
				intCenter += 50;
			delta = std::abs(symMgr->Kospi200Current - intCenter);
			if (delta < minVal) {
				minVal = delta;
				eIndex = i;
			}
			i++;
		}
		int max_row = height / DefaultCellHeight;;
		int half = (int)(max_row / 2);
		startRow = eIndex - half;
	}

	return startRow;
}

void HdSymbolOptionGrid::OnExpected(VtSymbol* sym)
{
	if (_Mode == 0) {
		SetRemain(sym);
	}
	else if (_Mode == 2) {
		ShowExpected(sym);
	}
}

void HdSymbolOptionGrid::SetCurrent()
{
	if (_Mode != 1)
		return;

	for (auto it = _CallSymbolRowMap.begin(); it != _CallSymbolRowMap.end(); ++it)
	{
		auto pos = it->second;
		VtSymbol* sym = std::get<2>(pos);
		ShowCurrent(sym);
	}

	for (auto it = _PutSymbolRowMap.begin(); it != _PutSymbolRowMap.end(); ++it)
	{
		auto pos = it->second;
		VtSymbol* sym = std::get<2>(pos);
		ShowCurrent(sym);
	}
}

void HdSymbolOptionGrid::SetExpected()
{
	if (!_OrderConfigMgr || !_OrderConfigMgr->Account())
		return;

	for (auto it = _CallSymbolRowMap.begin(); it != _CallSymbolRowMap.end(); ++it)
	{
		auto pos = it->second;
		QuickSetText(std::get<0>(pos), std::get<1>(pos), _T(""));
		QuickRedrawCell(std::get<0>(pos), std::get<1>(pos));
	}

	for (auto it = _PutSymbolRowMap.begin(); it != _PutSymbolRowMap.end(); ++it)
	{
		auto pos = it->second;
		QuickSetText(std::get<0>(pos), std::get<1>(pos), _T(""));
		QuickRedrawCell(std::get<0>(pos), std::get<1>(pos));
	}
}

void HdSymbolOptionGrid::SetEqualRow(int equalRow)
{
	int maxRow = GetMaxRow();

	int centerRow = maxRow / 2;

	SetTopRow(equalRow - centerRow);
}

int HdSymbolOptionGrid::GetMaxRow()
{
	int rowHeight = DefaultCellHeight;

	if (_OrderConfigMgr)
		rowHeight = _OrderConfigMgr->OrderCellHeight;


	RECT rect;
	GetClientRect(&rect);

	if (_OrderConfigMgr->_HdCenterWnd->GetSafeHwnd())
		_OrderConfigMgr->_HdOrderWnd->GetWindowRect(&rect);

	int count = (rect.bottom - rect.top) / rowHeight;

	return count - 2;
}

void HdSymbolOptionGrid::OnSymbolMaster(VtSymbol* sym)
{
	ShowCurrent(sym);
}

void HdSymbolOptionGrid::ResetRemainCells()
{
	for (auto it = _RemainPos.begin(); it != _RemainPos.end(); ++it) {
		VtCellPos pos = *it;
		if (pos.Col == 0)
			QuickSetBackColor(pos.Col, pos.Row, RGB(252, 226, 228));
		else
			QuickSetBackColor(pos.Col, pos.Row, RGB(218, 226, 245));

		QuickSetText(pos.Col, pos.Row, _T(""));
	}
}

void HdSymbolOptionGrid::ShowPosition(bool init, int acptCnt, VtPosition* posi, std::string symCode)
{
	CUGCell cell;
	auto it = _CallSymbolRowMap.find(symCode);
	if (it != _CallSymbolRowMap.end()) {
		auto pos = _CallSymbolRowMap[symCode];

		VtCellPos cellPos;

		if (!posi) {
			if (acptCnt > 0) { // 포지션 없고 접수확인 주문만 있을 때 - 회색 배경에 0
				QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
				QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(212, 186, 188));
			} else { // 접수 확인 주문이 없을 때
				if (init) { // 한번이라도 주문이 나갔으면 흰색배경에 0
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
				} else { // 한번도 주문이 안 나갔으면 매수 배경색
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T(""));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(252, 226, 228));
				}
			}
		} else { // 포지션이 있는 경우 - 최소 한번이라도 주문이 나간것으로 간주한다. - 이전에 나갔어도 
			if (std::abs(posi->OpenQty) > 0) { // 잔고가 있는 경우
				QuickSetNumber(std::get<0>(pos), std::get<1>(pos), posi->OpenQty);
				if (acptCnt > 0) // 접수 확인 주문이 있는 경우  회색배경에 잔고 갯수
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(212, 186, 188));
				else // 접수 확인 주문이 없는 경우는 흰색 배경에 잔고 갯수
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
			} else { // 잔고가 없는 경우
				if (acptCnt > 0) { // 한번이라도 주문이 나간경우 회색배경에 0
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(212, 186, 188));
				} else { // 잔고, 접수확인 둘 다 없을 때 흰색 배경에 0
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
				}
			}
		}

		cellPos.Col = std::get<0>(pos);
		cellPos.Row = std::get<1>(pos);
		_RemainPos.insert(cellPos);
		QuickRedrawCell(std::get<0>(pos), std::get<1>(pos));
	}

	it = _PutSymbolRowMap.find(symCode);
	if (it != _PutSymbolRowMap.end()) {
		auto pos = _PutSymbolRowMap[symCode];

		VtCellPos cellPos;

		if (!posi) { // 포지션이 없는 경우 - 주문이 나간적이 없을 경우
			if (acptCnt > 0) { // 접수확인된 주문이 있을 경우 - 0에 회색 배경
				QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
				QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(178, 186, 205));
			} else {
				if (init) { // 잔고 없이 한번이라도 주문이 나간경우 - 0에 흰색 배경
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
				} else { // 잔고도 없고 한번도 주문이 나가지 않은 경우 - 숫자없이 매도 배경색
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T(""));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(218, 226, 245));
				}
			}
		} else { // 포지션이 있는 경우 - 주문이 나간적이 있는 경우 - 한번이라도 주문이 나갔다
			if (std::abs(posi->OpenQty) > 0) { // 잔고가 있는 경우
				QuickSetNumber(std::get<0>(pos), std::get<1>(pos), posi->OpenQty);
				if (acptCnt > 0) // 접수확인이 있는 경우 회색 배경
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(178, 186, 205));
				else // 잔고만 있을 때는 흰색 배경
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
			} else { // 잔고가 없는 경우
				if (acptCnt > 0) { // 접수확인이 있는 경우 - 회색 배경에 0
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(178, 186, 205));
				} else { // 잔고, 접수확인 없을 때 흰색 배경에 0
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
				}
			}
		}
		cellPos.Col = std::get<0>(pos);
		cellPos.Row = std::get<1>(pos);
		_RemainPos.insert(cellPos);
		QuickRedrawCell(std::get<0>(pos), std::get<1>(pos));
	}
}

void HdSymbolOptionGrid::ShowCurrent(VtSymbol* sym)
{
	if (!sym)
		return;
	CUGCell cell;
	auto it = _CallSymbolRowMap.find(sym->ShortCode);
	if (it != _CallSymbolRowMap.end())
	{
		auto pos = _CallSymbolRowMap[sym->ShortCode];
		ShowCurrent(std::get<0>(pos), std::get<1>(pos), std::get<2>(pos));
	}

	it = _PutSymbolRowMap.find(sym->ShortCode);
	if (it != _PutSymbolRowMap.end())
	{
		auto pos = _PutSymbolRowMap[sym->ShortCode];
		ShowCurrent(std::get<0>(pos), std::get<1>(pos), std::get<2>(pos));
	}
}

void HdSymbolOptionGrid::ShowCurrent(int col, int row, VtSymbol* sym)
{
	CUGCell cell;
	GetCell(col, row, &cell);
	int curValue = sym->Quote.intClose;
	
	std::string strVal = NumberFormatter::format(curValue / std::pow(10, sym->Decimal), sym->Decimal);

	cell.SetText(strVal.c_str());
	SetCell(col, row, &cell);
	QuickRedrawCell(col, row);
}

void HdSymbolOptionGrid::ShowExpected(VtSymbol* sym)
{
	if (!sym)
		return;
	CUGCell cell;
	auto it = _CallSymbolRowMap.find(sym->ShortCode);
	if (it != _CallSymbolRowMap.end())
	{
		auto pos = _CallSymbolRowMap[sym->ShortCode];
		ShowExpected(std::get<0>(pos), std::get<1>(pos), std::get<2>(pos));
	}

	it = _PutSymbolRowMap.find(sym->ShortCode);
	if (it != _PutSymbolRowMap.end())
	{
		auto pos = _PutSymbolRowMap[sym->ShortCode];
		ShowExpected(std::get<0>(pos), std::get<1>(pos), std::get<2>(pos));
	}
}

void HdSymbolOptionGrid::ShowExpected(int col, int row, VtSymbol* sym)
{
	CUGCell cell;
	GetCell(col, row, &cell);
	int curValue = sym->Quote.intExpected;

	std::string strVal = NumberFormatter::format(curValue / std::pow(10, sym->Decimal), sym->Decimal);

	cell.SetText(strVal.c_str());
	SetCell(col, row, &cell);
	QuickRedrawCell(col, row);
}

void HdSymbolOptionGrid::OnOrderFilled(VtOrder* order)
{
	if (!_OrderConfigMgr || !order)
		return;

	if (_OrderConfigMgr->Type() == 0)
	{
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (!acnt)
			return;

		VtPosition* posi = acnt->FindPosition(order->shortCode);
		if (posi)
			SetRemain(posi);
	}
	else
	{
		VtFund* fund = _OrderConfigMgr->Fund();
		if (!fund)
			return;
		int count = 0;
		VtPosition posi = fund->GetPosition(order->shortCode, count);
		SetRemain(&posi);
	}
}

void HdSymbolOptionGrid::OnReceiveQuote(VtSymbol* sym)
{
	if (_Mode == 0) {
		SetRemain(sym);
	}
	else if (_Mode == 1) {
		ShowCurrent(sym);
	}
}
