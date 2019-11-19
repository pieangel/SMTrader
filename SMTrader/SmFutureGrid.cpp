#include "stdafx.h"
#include "SmFutureGrid.h"
#include "VtProductCategoryManager.h"
#include "VtProductSection.h"
#include "VtProductSubSection.h"
#include "VtSymbol.h"
#include "VtOrderConfigManager.h"
#include "Poco/NumberFormatter.h"
#include "VtHdClient.h"
#include "VtChartDataRequest.h"
#include "VtOrderCenterWndHd.h"
#include "VtPosition.h"
#include "VtAccount.h"
#include "VtOrderManager.h"
#include "VtOrderManagerSelector.h"
#include "VtProductOrderManagerSelector.h"
#include "VtProductOrderManager.h"
#include "VtFund.h"
#include "VtSymbolManager.h"
#include "VtRealtimeRegisterManager.h"
#include "VtChartDataManager.h"
#include "VtChartData.h"
#include "VtGlobal.h"
#include "Format/format.h"
#include "XFormatNumber.h"
#include "VtChartDataCollector.h"
#include "SmOrderPanel.h"

SmFutureGrid::SmFutureGrid()
{
}


SmFutureGrid::~SmFutureGrid()
{
}

BEGIN_MESSAGE_MAP(SmFutureGrid, CGridCtrl)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void SmFutureGrid::Init()
{
	_CellHeight = 20;
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));

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
				if (j == 2)
					pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				else // 텍스트 정렬
					pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				// 셀 배경 색 설정
				pCell->SetBackClr(RGB(255, 255, 255));
				// 셀 글자색 설정
				pCell->SetTextClr(RGB(0, 0, 0));

				InvalidateCellRect(i, j);
			}
		}
	}

	SetFont(&_defFont);

	SetColTitle();
}

void SmFutureGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	CCellID cell = GetCellFromPt(point);
	CGridCellBase* pCell = GetCell(cell.row, cell.col);
	VtSymbol* sym = (VtSymbol*)pCell->GetData();
	if (sym) {
		if (sym->Quote.intClose == 0)
			sym->GetSymbolMaster();
		if (_OrderConfigMgr) {
			_OrderConfigMgr->Symbol(sym);

			if (_OrderConfigMgr->_HdCenterWnd) {
				_OrderConfigMgr->_HdCenterWnd->ChangeSymbol(sym);
			}
		}
	}
}

void SmFutureGrid::SetColTitle()
{
	int colWidth[3] = { 65, 50, 55 };

	for (int i = 0; i < _ColCount; i++) {
		SetColumnWidth(i, colWidth[i]);
	}

	LPCTSTR title[5] = { "지수선물", "미니선물", "코닥선물", "국체선물", "달러선물" };
	for (int j = 0; j < _RowCount; ++j) {
		QuickSetBackColor(j, 0, VtGlobal::GridTitleBackColor);
		QuickSetText(j, 0, title[j]);
		InvalidateCellRect(j, 0);
	}
}


void SmFutureGrid::InitGrid()
{
	if (!_OrderConfigMgr)
		return;

	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	int i = 0;
	CUGCell cell;
	for (auto it = prdtCatMgr->MainFutureVector.begin(); it != prdtCatMgr->MainFutureVector.end(); ++it) {
		std::string secName = *it;
		VtProductSection* section = prdtCatMgr->FindProductSection(secName);
		if (section) {
			QuickSetText(i, 1,section->Code.c_str());
			if (section->SubSectionVector.size() > 0) {
				VtProductSubSection* subSection = section->SubSectionVector.front();
				if (subSection->_SymbolVector.size() > 0) {
					VtSymbol* sym = subSection->_SymbolVector.front();
					GetSymbolMaster(sym);
					sym->RecentMonth(true);
					sym->RequestedSymbolMaster(true);
					if (_Mode == 0) {
						ShowRemain(sym);
					}
					else if (_Mode == 1) {
						ShowCurrent(sym, i);
					}
					else {
						QuickSetText(i, 2, _T("0"));
						QuickSetBackColor(i, 2, RGB(255, 255, 255));
					}

					CGridCellBase* pCell = GetCell(i, 0);
					pCell->SetBackClr(VtGlobal::GridTitleBackColor);
					pCell->SetTextClr(VtGlobal::GridTitleTextColor);
					pCell->SetText(section->UserDefinedName.c_str());
					// Set the symbol object for the current cell.
					pCell->SetData((LPARAM)sym);
					InvalidateCellRect(i, 0);

					pCell = GetCell(i, 1);
					pCell->SetData((LPARAM)sym);

					pCell = GetCell(i, 2);
					pCell->SetData((LPARAM)sym);

					SymbolMap[sym->ShortCode] = i;

					_FutureCurrentValueRowMap[sym->ShortCode] = std::make_tuple(i, 2, sym);
				}
			}
			i++;
		}
	}
}

void SmFutureGrid::ShowPosition(bool init, int acptCnt, VtPosition* posi, std::string symCode)
{
	auto it = _FutureCurrentValueRowMap.find(symCode);
	if (it != _FutureCurrentValueRowMap.end()) {
		auto pos = _FutureCurrentValueRowMap[symCode];

		if (!posi) {
			if (acptCnt > 0) {
				QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
				QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(186, 186, 186));
			}
			else {
				if (init) {
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
				}
				else {
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T(""));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
				}
			}
		}
		else {
			if (std::abs(posi->OpenQty) > 0) {
				CString strVal;
				strVal.Format(_T("%d"), posi->OpenQty);
				QuickSetText(std::get<0>(pos), std::get<1>(pos), strVal);
				if (acptCnt > 0)
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(186, 186, 186));
				else
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
			}
			else {
				if (acptCnt > 0) {
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(186, 186, 186));
				}
				else {
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
				}
			}
		}

		QuickSetTextColor(std::get<0>(pos), std::get<1>(pos), RGB(0, 0, 0));
		InvalidateCellRect(std::get<0>(pos), std::get<1>(pos));
	}
}

void SmFutureGrid::GetSymbolMaster(VtSymbol* sym)
{
	if (!sym)
		return;
	if (sym->RecentMonth() && sym->RequestedSymbolMaster())
		return;
	sym->GetSymbolMaster();
	VtRealtimeRegisterManager* realTimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	realTimeRegiMgr->RegisterProduct(sym->ShortCode);
}

void SmFutureGrid::QuickSetBackColor(int row, int col, COLORREF color)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetBackClr(color);
	}
}

void SmFutureGrid::QuickSetText(int row, int col, LPCTSTR text)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetText(text);
	}
}

void SmFutureGrid::QuickSetNumber(int row, int col, int number)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		CString text;
		text.Format("%d", number);
		pCell->SetText(text);
	}
}

void SmFutureGrid::QuickSetTextColor(int row, int col, COLORREF color)
{
	CGridCellBase* pCell = GetCell(row, col);
	if (pCell) {
		pCell->SetTextClr(color);
	}
}

void SmFutureGrid::OnReceiveQuote(VtSymbol* sym)
{
	if (!sym)
		return;

	if (_Mode == 0) {
		ShowRemain(sym);
	}
	else if (_Mode == 1) {
		CUGCell cell;
		auto it = _FutureCurrentValueRowMap.find(sym->ShortCode);
		if (it != _FutureCurrentValueRowMap.end()) {
			auto pos = _FutureCurrentValueRowMap[sym->ShortCode];
			ShowCurrent(sym, std::get<1>(pos));
		}
	}
}

void SmFutureGrid::OnSymbolMaster(VtSymbol* sym)
{
	if (!sym)
		return;
	if (_Mode != 1)
		return;

	CUGCell cell;
	auto it = _FutureCurrentValueRowMap.find(sym->ShortCode);
	if (it != _FutureCurrentValueRowMap.end()) {
		auto pos = _FutureCurrentValueRowMap[sym->ShortCode];
		ShowCurrent(sym, std::get<1>(pos));
	}
}

void SmFutureGrid::SetRemain(VtPosition* posi)
{
	if (!posi)
		return;

	if (_Mode != 0)
		return;
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindSymbol(posi->ShortCode);
	if (sym) {
		ShowRemain(sym);
	}
}

void SmFutureGrid::SetRemain(VtOrder* order)
{
	if (!order || _Mode != 0 || !_OrderConfigMgr)
		return;

	if (_OrderConfigMgr->Type() == 0) {
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (!acnt)
			return;

		VtOrderManagerSelector* orderMgrSeledter = VtOrderManagerSelector::GetInstance();
		VtOrderManager* orderMgr = orderMgrSeledter->FindAddOrderManager(acnt->AccountNo);
		VtProductOrderManager* prdtOrderMgr = orderMgr->GetProductOrderManager(order->shortCode);
		VtPosition* posi = acnt->FindPosition(order->shortCode);
		if (prdtOrderMgr)
			ShowPosition(prdtOrderMgr->Init(), prdtOrderMgr->GetAcceptedOrderCount(), posi, order->shortCode);
		else
			ShowPosition(false, 0, nullptr, order->shortCode);
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

void SmFutureGrid::ShowRemain(VtSymbol* sym)
{
	if (!sym)
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

void SmFutureGrid::ShowCurrent(VtSymbol* sym, int row)
{
	if (!sym)
		return;
	std::string val = fmt::format("{:.{}f}", sym->Quote.intClose / std::pow(10, sym->IntDecimal), sym->IntDecimal);
	CString thVal = XFormatNumber(val.c_str(), sym->IntDecimal);
	QuickSetText(row, 2, thVal);
	if (sym->Quote.intOpen == 0) {
		QuickSetTextColor(row, 2, RGB(0, 0, 0));
	}
	else {
		if (sym->Quote.intClose > sym->Quote.intOpen) {
			QuickSetTextColor(row, 2, RGB(255, 0, 0));
		}
		else if (sym->Quote.intClose < sym->Quote.intOpen) {
			QuickSetTextColor(row, 2, RGB(0, 0, 255));
		}
		else {
			QuickSetTextColor(row, 2, RGB(0, 0, 0));
		}
	}
	QuickSetBackColor(row, 2, RGB(255, 255, 255));
	InvalidateCellRect(row, 2);
}

void SmFutureGrid::OnOrderFilled(VtOrder* order)
{
	if (!_OrderConfigMgr || !order)
		return;

	if (_OrderConfigMgr->Type() == 0) {
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (!acnt)
			return;

		VtPosition* posi = acnt->FindPosition(order->shortCode);
		if (posi)
			SetRemain(posi);
	}
	else {
		VtFund* fund = _OrderConfigMgr->Fund();
		if (!fund)
			return;
		int count = 0;
		VtPosition posi = fund->GetPosition(order->shortCode, count);
		SetRemain(&posi);
	}
}

void SmFutureGrid::OnExpected(VtSymbol* sym)
{
	if (!sym)
		return;

	if (_Mode == 2) {
		CUGCell cell;
		auto it = _FutureCurrentValueRowMap.find(sym->ShortCode);
		if (it != _FutureCurrentValueRowMap.end()) {
			auto pos = _FutureCurrentValueRowMap[sym->ShortCode];
			ShowExpected(sym, std::get<1>(pos));
		}
	}
}

void SmFutureGrid::ShowExpected(VtSymbol* sym, int row)
{
	if (!sym)
		return;
	std::string val = fmt::format("{:.{}f}", sym->Quote.intExpected / std::pow(10, sym->IntDecimal), sym->IntDecimal);
	CString thVal = XFormatNumber(val.c_str(), sym->IntDecimal);
	QuickSetText(row, 2, thVal);
	QuickSetTextColor(row, 2, RGB(0, 0, 0));
	QuickSetBackColor(row, 2, RGB(255, 255, 255));
	InvalidateCellRect(row, 2);
}
