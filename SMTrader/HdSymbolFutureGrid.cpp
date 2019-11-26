#include "stdafx.h"
#include "HdSymbolFutureGrid.h"
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

using Poco::NumberFormatter;
HdSymbolFutureGrid::HdSymbolFutureGrid()
{
	_Mode = 1;
	_SellColor = RGB(218, 226, 245);
	_BuyColor = RGB(252, 226, 228);
	_DefaultColor = RGB(255, 255, 255);
	_OrderConfigMgr = nullptr;
}


HdSymbolFutureGrid::~HdSymbolFutureGrid()
{
}


void HdSymbolFutureGrid::OnSetup()
{
	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("±¼¸²"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("±¼¸²"));

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

	QuickSetAlignment(0, 0, UG_ALIGNCENTER | UG_ALIGNVCENTER);
	for (int i = 0; i < _ColCount; i++) {
		QuickSetAlignment(i, 0, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	}

	SetDefFont(&_defFont);
	

	SetColTitle();

	InitGrid();
}

void HdSymbolFutureGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

void HdSymbolFutureGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	int i = 0;
	CUGCell cell;
	GetCell(col, row, &cell);
	VtSymbol* sym = (VtSymbol*)cell.Tag();
	if (sym) {
		if (sym->Quote.intClose == 0)
			sym->GetSymbolMaster();
		if (_OrderConfigMgr) {
			_OrderConfigMgr->Symbol(sym);

			VtChartDataRequest req;
			
			req.chartType = VtChartType::MIN;
			req.productCode = _T("HSIX18");  //sym->ShortCode;
			//req.productCode = sym->ShortCode;
			req.count = 9999;
			req.cycle = 1;
			req.next = 0;
			req.reqKey = 0;
			req.seq = 0;
			req.domestic = false;
			//req.domestic = true;
			req.keyvalue = _T("");
			
			
			VtHdClient* client = VtHdClient::GetInstance();
			//client->GetChartData(std::move(req));

			if (_OrderConfigMgr->_HdCenterWnd) {
				_OrderConfigMgr->_HdCenterWnd->ChangeSymbol(sym);
			}

			VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
			VtChartData* chartData = chartDataMgr->Add(req);
			//chartDataMgr->AddChartDataRequest(10, chartData);
			//chartData->RequestChartData();
			VtChartDataCollector* collector = VtChartDataCollector::GetInstance();
			//collector->RegisterProduct(sym);
		}
	}
}

int HdSymbolFutureGrid::OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow)
{
	return 0;
}

void HdSymbolFutureGrid::SetColTitle()
{
	const int ColCount = _ColCount;
	CUGCell cell;
	LPCTSTR title[3] = { "Á¾¸ñ", "ÄÚµå", "°ª" };
	int colWidth[3] = { 65, 50, 55 };


	for (int i = 0; i < _ColCount; i++) {
		SetColWidth(i, colWidth[i]);
		QuickSetText(i, -1, title[i]);
		GetCell(i, -1, &cell);
		cell.SetBackColor(VtGlobal::GridTitleBackColor);
		cell.SetTextColor(VtGlobal::GridTitleTextColor);
		SetCell(i, -1, &cell);
		QuickSetFont(i, -1, &_titleFont);
		QuickSetAlignment(i, -1, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		RedrawCell(i, -1);
	}
}

void HdSymbolFutureGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void HdSymbolFutureGrid::Begin()
{

}

void HdSymbolFutureGrid::End()
{

}

void HdSymbolFutureGrid::InitGrid()
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
			QuickSetText(1, i, section->Code.c_str());
			QuickSetAlignment(1, i, UG_ALIGNCENTER | UG_ALIGNVCENTER);
			QuickSetAlignment(2, i, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
			if (section->SubSectionVector.size() > 0) {
				VtProductSubSection* subSection = section->SubSectionVector.front();
				if (subSection->_SymbolVector.size() > 0) {
					VtSymbol* sym = subSection->_SymbolVector.front();
					GetSymbolMaster(sym);
					sym->RecentMonth(true);
					sym->RequestedSymbolMaster(true);
					if (_Mode == 0) {
						ShowRemain(sym);
					} else if (_Mode == 1) {
						ShowCurrent(sym, i);
					} else {
						QuickSetText(2, i, _T("0"));
						QuickSetBackColor(2, i, RGB(255, 255, 255));
					}

					GetCell(0, i, &cell);
					cell.SetBackColor(VtGlobal::GridTitleBackColor);
					cell.SetTextColor(VtGlobal::GridTitleTextColor);
					cell.SetText(section->UserDefinedName.c_str());
					cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
					// Set the symbol object for the current cell.
					cell.Tag((void*)sym);
					SetCell(0, i, &cell);

					GetCell(1, i, &cell);
					cell.Tag((void*)sym);
					SetCell(1, i, &cell);

					GetCell(2, i, &cell);
					cell.Tag((void*)sym);
					SetCell(2, i, &cell);

					SymbolMap[sym->ShortCode] = i;

					_FutureCurrentValueRowMap[sym->ShortCode] = std::make_tuple(2, i, sym);
				}
			}

			QuickRedrawCell(0, i);
			QuickRedrawCell(1, i);
			QuickRedrawCell(2, i);
			i++;
		}
	}
}

void HdSymbolFutureGrid::OnReceiveQuote(VtSymbol* sym)
{
	if (!sym)
		return;

	if (_Mode == 0) {
		ShowRemain(sym);
	} else if (_Mode == 1) {
		CUGCell cell;
		auto it = _FutureCurrentValueRowMap.find(sym->ShortCode);
		if (it != _FutureCurrentValueRowMap.end()) {
			auto pos = _FutureCurrentValueRowMap[sym->ShortCode];
			ShowCurrent(sym, std::get<1>(pos));
		}
	}
}

void HdSymbolFutureGrid::OnSymbolMaster(VtSymbol* sym)
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

void HdSymbolFutureGrid::SetRemain(VtPosition* posi)
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

void HdSymbolFutureGrid::SetRemain(VtOrder* order)
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

void HdSymbolFutureGrid::ShowRemain(VtSymbol* sym)
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

void HdSymbolFutureGrid::ShowCurrent(VtSymbol* sym, int row)
{
	if (!sym)
		return;
	std::string val = fmt::format("{:.{}f}", sym->Quote.intClose / std::pow(10, sym->Decimal), sym->Decimal);
	CString thVal = XFormatNumber(val.c_str(), sym->Decimal);
	QuickSetText(2, row, thVal);
	if (sym->Quote.intOpen == 0) {
		QuickSetTextColor(2, row, RGB(0, 0, 0));
	} else {
		if (sym->Quote.intClose > sym->Quote.intOpen) {
			QuickSetTextColor(2, row, RGB(255, 0, 0));
		}
		else if (sym->Quote.intClose < sym->Quote.intOpen) {
			QuickSetTextColor(2, row, RGB(0, 0, 255));
		}
		else {
			QuickSetTextColor(2, row, RGB(0, 0, 0));
		}
	}
	QuickSetBackColor(2, row, RGB(255, 255, 255));
	QuickRedrawCell(2, row);
}

void HdSymbolFutureGrid::ShowPosition(bool init, int acptCnt, VtPosition* posi, std::string symCode)
{
	CUGCell cell;
	auto it = _FutureCurrentValueRowMap.find(symCode);
	if (it != _FutureCurrentValueRowMap.end()) {
		auto pos = _FutureCurrentValueRowMap[symCode];

		if (!posi) {
			if (acptCnt > 0) {
				QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
				QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(186, 186, 186));
			} else {
				if (init) {
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
				} else {
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T(""));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
				}
			}
		} else {
			if (std::abs(posi->OpenQty) > 0) {
				CString strVal;
				strVal.Format(_T("%d"), posi->OpenQty);
				QuickSetText(std::get<0>(pos), std::get<1>(pos), strVal);
				if (acptCnt > 0)
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(186, 186, 186));
				else
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
			} else {
				if (acptCnt > 0) {
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(186, 186, 186));
				} else {
					QuickSetText(std::get<0>(pos), std::get<1>(pos), _T("0"));
					QuickSetBackColor(std::get<0>(pos), std::get<1>(pos), RGB(255, 255, 255));
				}
			}
		}

		QuickSetTextColor(std::get<0>(pos), std::get<1>(pos), RGB(0, 0, 0));
		QuickRedrawCell(std::get<0>(pos), std::get<1>(pos));
	}
}

void HdSymbolFutureGrid::GetSymbolMaster(VtSymbol* sym)
{
	if (!sym)
		return;
	if (sym->RecentMonth() && sym->RequestedSymbolMaster())
		return;
	sym->GetSymbolMaster();
	VtRealtimeRegisterManager* realTimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	realTimeRegiMgr->RegisterProduct(sym->ShortCode);
}

void HdSymbolFutureGrid::OnOrderFilled(VtOrder* order)
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
	} else {
		VtFund* fund = _OrderConfigMgr->Fund();
		if (!fund)
			return;
		int count = 0;
		VtPosition posi = fund->GetPosition(order->shortCode, count);
		SetRemain(&posi);
	}
}

void HdSymbolFutureGrid::OnExpected(VtSymbol* sym)
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

void HdSymbolFutureGrid::ShowExpected(VtSymbol* sym, int row)
{
	if (!sym)
		return;
	std::string val = fmt::format("{:.{}f}", sym->Quote.intExpected / std::pow(10, sym->Decimal), sym->Decimal);
	CString thVal = XFormatNumber(val.c_str(), sym->Decimal);
	QuickSetText(2, row, thVal);
	QuickSetTextColor(2, row, RGB(0, 0, 0));
	QuickSetBackColor(2, row, RGB(255, 255, 255));
	QuickRedrawCell(2, row);
}

