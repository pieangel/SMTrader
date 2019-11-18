#include "stdafx.h"
#include "SmOptionGrid.h"
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
using Poco::NumberFormatter;

SmOptionGrid::SmOptionGrid()
{
}


SmOptionGrid::~SmOptionGrid()
{
}

void SmOptionGrid::UnregisterAllCallback()
{

}

void SmOptionGrid::RegisterQuoteCallback()
{

}

void SmOptionGrid::OnQuoteEvent(VtSymbol* symbol)
{

}

BEGIN_MESSAGE_MAP(SmOptionGrid, CGridCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void SmOptionGrid::Init()
{
	_EqualCell.row = -1;
	_EqualCell.col = -1;
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("����"));

	SetRowCount(_RowCount);
	SetColumnCount(_ColCount);
	SetFixedRowCount(0);
	SetFixedColumnCount(0);
	SetColTitle();
	EnableScrollBars(SB_VERT, FALSE);
	EnableScrollBars(SB_HORZ, FALSE);
	SetFixedColumnSelection(FALSE);
	SetFixedRowSelection(FALSE);
	SetCompareFunction(CGridCtrl::pfnCellNumericCompare);
	SetDoubleBuffering(TRUE);
	EnableSelection(FALSE);
	SetEditable(FALSE);
	EnableTitleTips(FALSE);
	SetColumnResize(FALSE);
	SetRowResize(FALSE);
	AllowReorderColumn(false);

	for (int i = 0; i < _RowCount; i++) {
		SetRowHeight(i, _CellHeight);
		for (int j = 0; j < 3; ++j) {
			CGridCellBase* pCell = GetCell(i, j);
			if (pCell) {
				// �ؽ�Ʈ ����
				pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				// �� ��� �� ����
				pCell->SetBackClr(RGB(255, 255, 255));
				// �� ���ڻ� ����
				pCell->SetTextClr(RGB(0, 0, 0));

				InvalidateCellRect(i, j);
			}
		}
	}

	SetFont(&_defFont);
	RegisterQuoteCallback();

	SetColTitle();

	InitSymbol();
	InitYearMonth();
}

void SmOptionGrid::OnLButtonDown(UINT nFlags, CPoint point)
{

}

void SmOptionGrid::OnRButtonDown(UINT nFlags, CPoint point)
{

}

void SmOptionGrid::SetColTitle()
{
	LPCTSTR title[3] = { "Call", "��簡", "Put" };
	int colWidth[9] = { 60, 50, 60 };

	for (int i = 0; i < _ColCount; i++)
	{
		SetColumnWidth(i, colWidth[i]);
		CGridCellBase* pCell = GetCell(0, i);
		pCell->SetText(title[i]);
		if (i == 0) {
			pCell->SetTextClr(RGB(255, 255, 255));
			pCell->SetBackClr(RGB(240, 51, 58));
		}
		else if (i == 2) {
			pCell->SetTextClr(RGB(255, 255, 255));
			pCell->SetBackClr(RGB(19, 137, 255));
		}
		else {
			pCell->SetTextClr(VtGlobal::GridTitleBackColor);
			pCell->SetBackClr(VtGlobal::GridTitleTextColor);
		}
		InvalidateCellRect(0, i);
	}
}

void SmOptionGrid::InitGrid()
{
	if (!_LeftWnd)
		return;
	if (!_CurPrdtSec)
		return;
	CGridCellBase* pCell = nullptr;
	ResetRemainCells();

	pCell = GetCell(_EqualCol, _EqualRow);
	if (pCell)
		pCell->SetBackClr(RGB(255, 255, 255));

	int selMon = _LeftWnd->_ComboOption.GetCurSel();
	if (selMon == -1 || _CurPrdtSec->SubSectionVector.size() == 0)
		return;

	int eIndex = 0;
	int delta = 0;
	int minVal = 1000000;

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	CString curYearMon;
	_LeftWnd->_ComboOption.GetLBText(selMon, curYearMon);
	VtProductSubSection* callSec = _CurPrdtSec->SubSectionVector[0];
	VtOptionMonthSection* opSec = callSec->FindOptionMap((LPCTSTR)curYearMon);
	if (opSec) {
		int i = 0;
		for (auto it = opSec->_SymbolVector.begin(); it != opSec->_SymbolVector.end(); ++it) {
			VtSymbol* sym = *it;
			// �� ���� �ɺ��� �Ҵ��� �ش�.
			pCell = GetCell(i, 0);
			pCell->SetData((LPARAM)sym);
			_CallSymbolRowMap[sym->ShortCode] = std::make_tuple(i, 0, sym);

			pCell = GetCell(i, 1);
			std::string centerVal = sym->ShortCode.substr(5, 3);
			char centerTip = sym->ShortCode.at(7);
			int intCenter = std::stoi(centerVal) * static_cast<int>(std::pow(10, sym->IntDecimal));
			if (centerTip == '2' || centerTip == '7')
				intCenter += 50;
			delta = std::abs(symMgr->Kospi200Current - intCenter);
			if (delta < minVal) {
				minVal = delta;
				eIndex = i;
			}

			// �߾Ӱ��� �����Ѵ�.
			std::string strVal = NumberFormatter::format(intCenter / std::pow(10, sym->IntDecimal), sym->IntDecimal);
			pCell->SetText(strVal.c_str());
			InvalidateCellRect(i, 1);
			i++;
		}

		// �ֱٿ��� ����
		SetEqualRow(eIndex);
		pCell = GetCell(eIndex, i);
		pCell->SetBackClr(RGB(255, 255, 0));
		InvalidateCellRect(eIndex, i);
		_EqualCol = 1;
		_EqualRow = eIndex;
	}

	VtProductSubSection* putSec = _CurPrdtSec->SubSectionVector[1];
	opSec = putSec->FindOptionMap((LPCTSTR)curYearMon);
	if (opSec) {
		int i = 0;
		for (auto it = opSec->_SymbolVector.begin(); it != opSec->_SymbolVector.end(); ++it) {
			// ǲ���� �ɺ��� �����Ѵ�.
			VtSymbol* sym = *it;
			pCell = GetCell(i, 2);
			pCell->SetData((LPARAM)sym);
			_PutSymbolRowMap[sym->ShortCode] = std::make_tuple(i, 2, sym);
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

void SmOptionGrid::InitGrid(int height)
{
	std::pair<int, int> start_max = FindValueStartRow(height);
	if (start_max.first == 0 && start_max.second == 0)
		return;
	_RowCount = start_max.second;
	SetRowCount(_RowCount);

	for (int i = 1; i < _RowCount; i++) {
		SetRowHeight(i, _CellHeight);
		for (int j = 0; j < 3; ++j) {
			CGridCellBase* pCell = GetCell(i, j);
			if (pCell) {
				// �ؽ�Ʈ ����
				pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				InvalidateCellRect(i, j);
			}
		}
	}

	MakeSymbolRowMap(start_max.first, start_max.second);

	if (_Mode == 0)
		SetRemain2();
	else if (_Mode == 1)
		SetCurrent2();
	else
		SetExpected2();
}

void SmOptionGrid::InitSymbol()
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

void SmOptionGrid::InitYearMonth()
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

	_LeftWnd->_ComboOption.SetCurSel(0);
}

void SmOptionGrid::SetYearMonth()
{
	if (!_CurPrdtSec)
		return;

	_LeftWnd->_ComboOption.ResetContent();
	VtProductSubSection* subSection = _CurPrdtSec->SubSectionVector.front();
	for (auto it = subSection->_OptionMap.begin(); it != subSection->_OptionMap.end(); ++it)
	{
		std::string code = it->first;

		CString yearMon(code.c_str());
		int index = _LeftWnd->_ComboOption.AddString(yearMon);
		//_LeftWnd->_ComboOption.SetItemDataPtr(index, code.c_str());
	}
	_LeftWnd->_ComboOption.SetCurSel(0);
}

void SmOptionGrid::SetProductSection()
{
	if (!_LeftWnd)
		return;
	int selPrdt = _LeftWnd->_ComboProduct.GetCurSel();
	if (selPrdt == -1)
		return;
	_CurPrdtSec = (VtProductSection*)_LeftWnd->_ComboProduct.GetItemDataPtr(selPrdt);
}

void SmOptionGrid::GetSymbolMaster()
{
	if (_EqualRow >= 0) {
		GetSymbolMasterByCenter();
		return;
	}
	if (!_CurPrdtSec)
		return;

	int selMon = _LeftWnd->_ComboOption.GetCurSel();
	if (selMon == -1 || _CurPrdtSec->SubSectionVector.size() == 0)
		return;
	CString curYearMon;
	_LeftWnd->_ComboOption.GetLBText(selMon, curYearMon);
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

void SmOptionGrid::GetSymbolMasterByCenter()
{
	int centerRow = _EqualRow;

	if (!_CurPrdtSec)
		return;

	int selMon = _LeftWnd->_ComboOption.GetCurSel();
	if (selMon == -1 || _CurPrdtSec->SubSectionVector.size() == 0)
		return;
	CString curYearMon;
	_LeftWnd->_ComboOption.GetLBText(selMon, curYearMon);
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

std::pair<int, int> SmOptionGrid::FindValueStartRow(int height)
{
	int selMon = _LeftWnd->_ComboOption.GetCurSel();
	if (selMon == -1 || _CurPrdtSec->SubSectionVector.size() == 0)
		return std::make_pair(0, 0);

	int eIndex = 0;
	int delta = 0;
	int minVal = 1000000;
	int intCenter = 0;
	int eCenter = 0;
	int startRow = 0;
	int max_row = height / DefaultCellHeight;;
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	CString curYearMon;
	_LeftWnd->_ComboOption.GetLBText(selMon, curYearMon);
	VtProductSubSection* callSec = _CurPrdtSec->SubSectionVector[0];
	VtOptionMonthSection* opSec = callSec->FindOptionMap((LPCTSTR)curYearMon);
	if (opSec) {
		for (int i = 0; i < opSec->_SymbolVector.size(); ++i) {
			VtSymbol* sym = opSec->_SymbolVector[i];
			std::string centerVal = sym->ShortCode.substr(5, 3);
			char centerTip = sym->ShortCode.at(7);
			intCenter = std::stoi(centerVal) * static_cast<int>(std::pow(10, sym->IntDecimal));
			if (centerTip == '2' || centerTip == '7')
				intCenter += 50;
			delta = std::abs(symMgr->Kospi200Current - intCenter);
			if (delta < minVal) {
				minVal = delta;
				eCenter = intCenter;
				eIndex = i;
			}
		}
		
		_EqualSymbol = opSec->_SymbolVector[eIndex];
		int half = (int)(max_row / 2);
		startRow = eIndex - half;
	}

	return std::make_pair(startRow, max_row);
}

void SmOptionGrid::ResetRemainCells()
{
	for (auto it = _RemainPos.begin(); it != _RemainPos.end(); ++it) {
		VtCellPos pos = *it;
		if (pos.Col == 0)
			QuickSetBackColor(pos.Row, pos.Col, RGB(252, 226, 228));
		else
			QuickSetBackColor(pos.Row, pos.Col, RGB(218, 226, 245));

		QuickSetText(pos.Row, pos.Col, _T(""));
	}
}

void SmOptionGrid::ShowPosition(bool init, int acptCnt, VtPosition* posi, std::string symCode)
{
	CUGCell cell;
	auto it = _CallSymbolRowMap.find(symCode);
	if (it != _CallSymbolRowMap.end()) {
		auto pos = _CallSymbolRowMap[symCode];

		VtCellPos cellPos;

		if (!posi) {
			if (acptCnt > 0) { // ������ ���� ����Ȯ�� �ֹ��� ���� �� - ȸ�� ��濡 0
				QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
				QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(212, 186, 188));
			}
			else { // ���� Ȯ�� �ֹ��� ���� ��
				if (init) { // �ѹ��̶� �ֹ��� �������� �����濡 0
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
				}
				else { // �ѹ��� �ֹ��� �� �������� �ż� ����
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T(""));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(252, 226, 228));
				}
			}
		}
		else { // �������� �ִ� ��� - �ּ� �ѹ��̶� �ֹ��� ���������� �����Ѵ�. - ������ ����� 
			if (std::abs(posi->OpenQty) > 0) { // �ܰ� �ִ� ���
				QuickSetNumber(std::get<0>(pos), std::get<1>(pos), posi->OpenQty);
				if (acptCnt > 0) // ���� Ȯ�� �ֹ��� �ִ� ���  ȸ����濡 �ܰ� ����
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(212, 186, 188));
				else // ���� Ȯ�� �ֹ��� ���� ���� ��� ��濡 �ܰ� ����
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
			}
			else { // �ܰ� ���� ���
				if (acptCnt > 0) { // �ѹ��̶� �ֹ��� ������� ȸ����濡 0
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(212, 186, 188));
				}
				else { // �ܰ�, ����Ȯ�� �� �� ���� �� ��� ��濡 0
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
				}
			}
		}

		cellPos.Col = std::get<0>(pos);
		cellPos.Row = std::get<1>(pos);
		_RemainPos.insert(cellPos);
		InvalidateCellRect(std::get<0>(pos), std::get<1>(pos));
	}

	it = _PutSymbolRowMap.find(symCode);
	if (it != _PutSymbolRowMap.end()) {
		auto pos = _PutSymbolRowMap[symCode];

		VtCellPos cellPos;

		if (!posi) { // �������� ���� ��� - �ֹ��� �������� ���� ���
			if (acptCnt > 0) { // ����Ȯ�ε� �ֹ��� ���� ��� - 0�� ȸ�� ���
				QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
				QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(178, 186, 205));
			}
			else {
				if (init) { // �ܰ� ���� �ѹ��̶� �ֹ��� ������� - 0�� ��� ���
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
				}
				else { // �ܰ� ���� �ѹ��� �ֹ��� ������ ���� ��� - ���ھ��� �ŵ� ����
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T(""));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(218, 226, 245));
				}
			}
		}
		else { // �������� �ִ� ��� - �ֹ��� �������� �ִ� ��� - �ѹ��̶� �ֹ��� ������
			if (std::abs(posi->OpenQty) > 0) { // �ܰ� �ִ� ���
				QuickSetNumber(std::get<0>(pos), std::get<1>(pos), posi->OpenQty);
				if (acptCnt > 0) // ����Ȯ���� �ִ� ��� ȸ�� ���
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(178, 186, 205));
				else // �ܰ� ���� ���� ��� ���
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
			}
			else { // �ܰ� ���� ���
				if (acptCnt > 0) { // ����Ȯ���� �ִ� ��� - ȸ�� ��濡 0
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(178, 186, 205));
				}
				else { // �ܰ�, ����Ȯ�� ���� �� ��� ��濡 0
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
				}
			}
		}
		cellPos.Col = std::get<0>(pos);
		cellPos.Row = std::get<1>(pos);
		_RemainPos.insert(cellPos);
		InvalidateCellRect(std::get<0>(pos), std::get<1>(pos));
	}
}

void SmOptionGrid::ShowExpected(VtSymbol* sym)
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

void SmOptionGrid::ShowExpected(int col, int row, VtSymbol* sym)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		int curValue = sym->Quote.intExpected;
		std::string strVal = NumberFormatter::format(curValue / std::pow(10, sym->IntDecimal), sym->IntDecimal);
		pCell->SetText(strVal.c_str());
		InvalidateCellRect(row, col);
	}
}

void SmOptionGrid::ShowCurrent(VtSymbol* sym)
{
	if (!sym)
		return;
	CUGCell cell;
	auto it = _SymbolRowMap.find(sym->ShortCode);
	if (it != _SymbolRowMap.end())
	{
		auto pos = _SymbolRowMap[sym->ShortCode];
		ShowCurrent(std::get<0>(pos), std::get<1>(pos), std::get<2>(pos));
	}
}

void SmOptionGrid::ShowCurrent(int row, int col, VtSymbol* sym)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		int curValue = sym->Quote.intClose;
		std::string strVal = NumberFormatter::format(curValue / std::pow(10, sym->IntDecimal), sym->IntDecimal);
		pCell->SetText(strVal.c_str());
		InvalidateCellRect(row, col);
	}
}

void SmOptionGrid::SetRemain()
{
	if (_Mode != 0 || !_OrderConfigMgr)
		return;

	for (auto it = _SymbolRowMap.begin(); it != _SymbolRowMap.end(); ++it) {
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
}

void SmOptionGrid::SetRemain(VtPosition* posi)
{
	if (!posi)
		return;

	if (_Mode != 0)
		return;

	if (!_OrderConfigMgr || !_OrderConfigMgr->Account())
		return;

	CUGCell cell;
	auto it = _SymbolRowMap.find(posi->ShortCode);
	if (it != _SymbolRowMap.end())
	{
		auto pos = _SymbolRowMap[posi->ShortCode];


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
		InvalidateCellRect(std::get<0>(pos), std::get<1>(pos));
	}
}

void SmOptionGrid::SetRemain(VtOrder* order)
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
	}
	else {
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

void SmOptionGrid::SetRemain(VtSymbol* sym)
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

void SmOptionGrid::SetRemain2()
{
	if (_Mode != 0 || !_OrderConfigMgr)
		return;

	for (auto it = _SymbolRowMap.begin(); it != _SymbolRowMap.end(); ++it) {
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
}

void SmOptionGrid::SetCurrent2()
{
	if (_Mode != 1)
		return;

	for (auto it = _SymbolRowMap.begin(); it != _SymbolRowMap.end(); ++it)
	{
		auto pos = it->second;
		VtSymbol* sym = std::get<2>(pos);
		ShowCurrent(sym);
	}
}

void SmOptionGrid::SetExpected2()
{
	if (!_OrderConfigMgr || !_OrderConfigMgr->Account())
		return;

	for (auto it = _SymbolRowMap.begin(); it != _SymbolRowMap.end(); ++it)
	{
		auto pos = it->second;
		QuickSetText(std::get<0>(pos), std::get<1>(pos), _T(""));
		InvalidateCellRect(std::get<0>(pos), std::get<1>(pos));
	}
}

void SmOptionGrid::QuickSetBackColor(int row, int col, COLORREF color)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetBackClr(color);
	}
}

void SmOptionGrid::QuickSetText(int row, int col, LPCTSTR text)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetText(text);
	}
}

void SmOptionGrid::QuickSetNumber(int row, int col, int number)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		CString text;
		text.Format("%d", nullptr);
		pCell->SetText(text);
	}
}

void SmOptionGrid::QuickSetTextColor(int row, int col, COLORREF color)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetTextClr(color);
	}
}

void SmOptionGrid::SetEqualRow(int equalRow)
{
	int maxRow = GetMaxRow();

	int centerRow = maxRow / 2;

	//SetTopRow(equalRow - centerRow);
}

int SmOptionGrid::GetMaxRow()
{
	int rowHeight = DefaultCellHeight;

	if (_OrderConfigMgr)
		rowHeight = _OrderConfigMgr->OrderCellHeight;


	RECT rect;
	GetClientRect(&rect);

	int count = (rect.bottom - rect.top) / rowHeight;

	return count;
}

void SmOptionGrid::MakeSymbolRowMap(int start_index, int max_row)
{
	int selMon = _LeftWnd->_ComboOption.GetCurSel();
	if (selMon == -1 || _CurPrdtSec->SubSectionVector.size() == 0)
		return;

	int eIndex = 0;
	int delta = 0;
	int minVal = 1000000;

	if (_EqualCell.IsValid()) {
		CGridCellBase* pCell = GetCell(_EqualCell.row, _EqualCell.col);
		if (pCell) {
			pCell->SetBackClr(RGB(255, 255, 255));
		}
	}

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	CString curYearMon;
	_LeftWnd->_ComboOption.GetLBText(selMon, curYearMon);
	VtProductSubSection* callSec = _CurPrdtSec->SubSectionVector[0];
	VtProductSubSection* putSec = _CurPrdtSec->SubSectionVector[1];
	VtOptionMonthSection* callMonthSec = callSec->FindOptionMap((LPCTSTR)curYearMon);
	VtOptionMonthSection* putMonthSec = putSec->FindOptionMap((LPCTSTR)curYearMon);
	if (callMonthSec && putMonthSec) {
		for (int i = start_index, j = 1; i < start_index + max_row; ++i, ++j) {
			VtSymbol* call_sym = callMonthSec->_SymbolVector[i];
			VtSymbol* put_sym = putMonthSec->_SymbolVector[i];
			_SymbolRowMap[call_sym->ShortCode] = std::make_tuple(j, 0, call_sym);
			_SymbolRowMap[put_sym->ShortCode] = std::make_tuple(j, 2, put_sym);

			std::string centerVal = call_sym->ShortCode.substr(5, 3);
			char centerTip = call_sym->ShortCode.at(7);
			int intCenter = std::stoi(centerVal) * static_cast<int>(std::pow(10, call_sym->IntDecimal));
			if (centerTip == '2' || centerTip == '7')
				intCenter += 50;
			
			// �߾Ӱ��� �����Ѵ�.
			std::string strVal = NumberFormatter::format(intCenter / std::pow(10, call_sym->IntDecimal), call_sym->IntDecimal);
			CGridCellBase* pCell = GetCell(j, 1);
			if (pCell) {
				if (call_sym == _EqualSymbol) {
					pCell->SetBackClr(RGB(255, 255, 0));
					_EqualCell.row = j;
					_EqualCell.col = 1;
				}
				pCell->SetText(strVal.c_str());
				InvalidateCellRect(j, 1);
			}
			pCell = GetCell(j, 0);
			if (pCell) {
				pCell->SetBackClr(RGB(252, 226, 228));
				InvalidateCellRect(j, 0);
			}
			pCell = GetCell(j, 2);
			if (pCell) {
				pCell->SetBackClr(RGB(218, 226, 245));
				InvalidateCellRect(j, 2);
			}
		}
	}
}

void SmOptionGrid::OnSymbolMaster(VtSymbol* sym)
{
	ShowCurrent(sym);
}

void SmOptionGrid::OnOrderFilled(VtOrder* order)
{

}

void SmOptionGrid::OnReceiveQuote(VtSymbol* sym)
{

}

void SmOptionGrid::OnExpected(VtSymbol* sym)
{
	if (_Mode == 0) {
		SetRemain(sym);
	}
	else if (_Mode == 2) {
		ShowExpected(sym);
	}
}

void SmOptionGrid::SetCurrent()
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

void SmOptionGrid::SetExpected()
{
	if (!_OrderConfigMgr || !_OrderConfigMgr->Account())
		return;

	for (auto it = _CallSymbolRowMap.begin(); it != _CallSymbolRowMap.end(); ++it)
	{
		auto pos = it->second;
		QuickSetText(std::get<0>(pos), std::get<1>(pos), _T(""));
		InvalidateCellRect(std::get<0>(pos), std::get<1>(pos));
	}

	for (auto it = _PutSymbolRowMap.begin(); it != _PutSymbolRowMap.end(); ++it)
	{
		auto pos = it->second;
		QuickSetText(std::get<0>(pos), std::get<1>(pos), _T(""));
		InvalidateCellRect(std::get<0>(pos), std::get<1>(pos));
	}
}
