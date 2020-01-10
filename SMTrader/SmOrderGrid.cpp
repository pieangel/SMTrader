#include "stdafx.h"
#include "SmOrderGrid.h"
#include "Global/VtDefine.h"
#include "VtGlobal.h"
#include "VtOrderCenterWndHd.h"
#include "VtSymbol.h"
#include "VtOrderPanelGrid.h"
#include "VtSymbolManager.h"
#include "VtQuoteManager.h"
#include "VtQuote.h"
#include "VtSymbolMaster.h"
#include "Poco/String.h"
#include "VtHogaManager.h"
#include "VtHoga.h"
#include "VtCellLabel.h"
#include "VtAccountManager.h"
#include "VtOrder.h"
#include "VtOrderManager.h"
#include "VtOrderManagerSelector.h"
#include "VtAccount.h"
#include "VtOrderManagerSelector.h"
#include "VtProductOrderManagerSelector.h"
#include "VtProductOrderManager.h"
#include "Poco/NumberFormatter.h"
#include <chrono>
#include <algorithm>
#include <vector>
#include <iterator>
#include <map>
#include "resource.h"
#include "VtStopOrderManager.h"
#include "VtPosition.h"
#include "VtHdClient.h"
#include "VtOrderWndHd.h"
#include "VtOrderConfigManager.h"
#include "VtFundOrderManager.h"
#include "VtFund.h"
#include "VtSubAccountManager.h"
#include "Format/format.h"
#include "VtCutManager.h"
#include "XFormatNumber.h"
#include <algorithm> 
#include <functional>
#include "SmCallbackManager.h"
#include "VtSymbol.h"
#include "VtFundOrderManager.h"
#include "Log/loguru.hpp"
#include "SmOrderPanelOut.h"

using Poco::trim;
using Poco::NumberFormatter;
using namespace std;
using namespace std::placeholders;

constexpr int Round(float x) { return static_cast<int>(x + 0.5f); }

SmOrderGrid::SmOrderGrid()
{
	BuyColor.push_back(RGB(253, 173, 176));
	BuyColor.push_back(RGB(255, 196, 199));
	BuyColor.push_back(RGB(255, 214, 212));
	BuyColor.push_back(RGB(255, 224, 225));
	BuyColor.push_back(RGB(255, 232, 232));

	SellColor.push_back(RGB(159, 214, 255));
	SellColor.push_back(RGB(185, 224, 255));
	SellColor.push_back(RGB(204, 230, 250));
	SellColor.push_back(RGB(221, 243, 255));
	SellColor.push_back(RGB(230, 247, 255));

	_CellHeight = 20;

	m_bMouseTracking = FALSE;

	_OldIndexRow = -1;
	_OldClickedCell.row = -1;
	_OldClickedCell.col = -1;
	_OldMMCell.row = -1;
	_OldMMCell.col = -1;
	_OldMovingCellCenter.row = -1;
	_OldMovingCellCenter.col = -1;
	_OldMovingCellSide.row = -1;
	_OldMovingCellSide.col = -1;

	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_Init = false;

	_OldPositionCell.row = -1;
	_OldPositionCell.col = -1;

	_GridColMap[SmOrderGridCol::STOP] = 48;
	_GridColMap[SmOrderGridCol::ORDER] = 60;
	_GridColMap[SmOrderGridCol::COUNT] = 45;
	_GridColMap[SmOrderGridCol::QUANTITY] = 45;
	_GridColMap[SmOrderGridCol::CENTER] = 80;

	_HeadHeight = 20;

	_CutMgr = new VtCutManager();

	_StopOrderMgr = new VtStopOrderManager();
}


SmOrderGrid::~SmOrderGrid()
{
	if (_StopOrderMgr) {
		_StopOrderMgr->RemoveAllHd();
		delete _StopOrderMgr;
		_StopOrderMgr = nullptr;
	}

	if (_CutMgr) {
		delete _CutMgr;
		_CutMgr = nullptr;
	}
}

void SmOrderGrid::OnSymbolMaster(VtSymbol* sym)
{
	if (!sym)
		return;
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;
	VtSymbol* symbol = _CenterWnd->Symbol();
	if (symbol->ShortCode.compare(sym->ShortCode) != 0) {
		return;
	}

	ResetByCenterRow();
}

void SmOrderGrid::SetAutoStopOnFilled(VtOrder* order)
{
	if (!order)
		return;
	_CutMgr->AddStopOrderForFilled(_CenterWnd->Symbol(), order);
}

void SmOrderGrid::UnregisterAllCallback()
{
	UnregisterHogaCallback();
	UnregisterOrderCallback();
	UnregisterQuoteCallback();
}

void SmOrderGrid::RegisterQuoteCallback()
{
	SmCallbackManager::GetInstance()->SubscribeQuoteCallback((long)this, std::bind(&SmOrderGrid::OnQuoteEvent, this, _1));
}

void SmOrderGrid::UnregisterQuoteCallback()
{
	SmCallbackManager::GetInstance()->UnsubscribeQuoteCallback((long)this);
}

void SmOrderGrid::OnQuoteEvent(VtSymbol* symbol)
{
	if (!_Init || !symbol)
		return;
	
	if (!_CenterWnd || !_CutMgr || !_CenterWnd->Symbol()) {
		return;
	}

	if (_CenterWnd->Symbol()->ShortCode.compare(symbol->ShortCode) != 0)
		return;

	// 잔고에 대하여 익절과 손절을 확인한다.
	// 익절, 손절 스탑 주문이 종가에 닿았는지 검사한다.
	CheckProfitLossTouchHd(_CenterWnd->Symbol()->Quote.intClose);
	std::set<std::pair<int, int>> refreshSet;
	// 스탑 주문이 종가에 닿았는지 검사한다.
	if (CheckStopTouchedHd(_CenterWnd->Symbol()->Quote.intClose)) {
		// 스탑주문을 다시 그려준다.
		ClearOldStopOrders(refreshSet);
		SetStopOrderInfo(refreshSet);
		CalcPosStopOrders(refreshSet);
	}

	ClearQuotes(refreshSet);
	// 중앙 고정일때는 종가 행을 새로 찾아서 매칭해준다.
	if (_CenterWnd->FixedCenter()) {
		_IndexRow = FindIndexRow();
		SetCenterValue(_CenterWnd->Symbol(), refreshSet);
	}
	SetQuoteColor(_CenterWnd->Symbol(), refreshSet);

	RefreshCells(refreshSet);
}

void SmOrderGrid::RegisterHogaCallback()
{
	SmCallbackManager::GetInstance()->SubscribeHogaCallback((long)this, std::bind(&SmOrderGrid::OnHogaEvent, this, _1));
}

void SmOrderGrid::UnregisterHogaCallback()
{
	SmCallbackManager::GetInstance()->UnsubscribeHogaCallback((long)this);
}

void SmOrderGrid::OnHogaEvent(VtSymbol* symbol)
{
	if (!_Init || !symbol)
		return;

	if (!_CenterWnd || !_CutMgr || !_CenterWnd->Symbol()) {
		return;
	}

	if (_CenterWnd->Symbol()->ShortCode.compare(symbol->ShortCode) != 0)
		return;

	std::set<std::pair<int, int>> refreshSet;
	ClearHogas(refreshSet);
	SetHogaInfo(symbol, refreshSet);
	RefreshCells(refreshSet);
	Invalidate();
}

void SmOrderGrid::RegisterOrderallback()
{
	SmCallbackManager::GetInstance()->SubscribeOrderCallback((long)this, std::bind(&SmOrderGrid::OnOrderEvent, this, _1));
}

void SmOrderGrid::UnregisterOrderCallback()
{
	SmCallbackManager::GetInstance()->UnsubscribeOrderCallback((long)this);
}

void SmOrderGrid::OnOrderEvent(VtOrder* order)
{
	if (!_Init || !order || !_OrderConfigMgr)
		return;

	if (!_CenterWnd || !_CutMgr || !_CenterWnd->Symbol()) {
		return;
	}
	std::set<std::pair<int, int>> refreshSet;
	ClearOldOrders(refreshSet);
	SetOrderInfo(refreshSet);
	ClearPositionInfo(refreshSet);
	SetPositionInfo(refreshSet);
	RefreshCells(refreshSet);
	if (order->Type == -1 || order->Type == 0) {
		if (!_OrderConfigMgr->Account())
			return;
		// 심볼과 계좌가 같지 않으면 진행하지 않는다.
		if (_CenterWnd->Symbol()->ShortCode.compare(order->shortCode) != 0 ||
			_OrderConfigMgr->Account()->AccountNo.compare(order->AccountNo) != 0)
			return;
	}
	else if (order->Type == 1) {
		if (!_OrderConfigMgr->Account())
			return;
		// 심볼과 계좌가 같지 않으면 진행하지 않는다.
		if (_CenterWnd->Symbol()->ShortCode.compare(order->shortCode) != 0 ||
			_OrderConfigMgr->Account()->AccountNo.compare(order->SubAccountNo) != 0)
			return;
	}
	else if (order->Type == 2) {
		if (!_OrderConfigMgr->Fund())
			return;
		// 심볼과 계좌가 같지 않으면 진행하지 않는다.
		if (_CenterWnd->Symbol()->ShortCode.compare(order->shortCode) != 0 ||
			_OrderConfigMgr->Fund()->Name.compare(order->FundName) != 0)
			return;
	}
	else
		return;

	if (order->state == VtOrderState::Filled) {
		refreshSet.clear();
		_CutMgr->AddStopOrderForFilled(_CenterWnd->Symbol(), order);
		ClearOldStopOrders(refreshSet);
		SetStopOrderInfo(refreshSet);
		CalcPosStopOrders(refreshSet);
		RefreshCells(refreshSet);
	}
}

void SmOrderGrid::Init()
{
	UnregisterButtons();
	_RowCount = GetMaxValueRowCount() + 1;
	_IndexRow = FindIndexRow();
	_EndRowForValue = _RowCount - 3;
	SetRowCount(_RowCount);
	SetColumnCount(_ColCount);
	SetFixedRowCount(_FixedRow);
	SetFixedColumnCount(_FixedCol);
	SetColTitle(_Init);
	EnableScrollBars(SB_VERT, FALSE);
	EnableScrollBars(SB_HORZ, FALSE);
	SetFixedColumnSelection(FALSE);
	SetFixedRowSelection(FALSE);
	EnableColumnHide();
	SetCompareFunction(CGridCtrl::pfnCellNumericCompare);
	SetDoubleBuffering(TRUE);
	EnableSelection(FALSE);
	SetEditable(FALSE);
	EnableTitleTips(FALSE);
	SetColumnResize(FALSE);
	SetRowResize(FALSE);
	AllowReorderColumn(false);

	for (int i = 1; i < m_nRows; ++i) {
		SetRowHeight(i, _CellHeight);
	}

	RegisterButtons();
	
	SetFont(&_defFont);
	// 주문영역 색칠하기
	SetOrderAreaColor();
	// 중앙에 값 설정하기 - 종목에 따라 값을 설정한다.
	SetCenterValue();

	// 콜백 함수 등록하기
	RegisterQuoteCallback();
	RegisterHogaCallback();
	RegisterOrderallback();

	_Init = true;
}


void SmOrderGrid::RegisterButtons()
{
	// 정렬버튼 등록
	RegisterButton(1, 0, CenterCol, GetSysColor(COLOR_BTNFACE), RGB(0, 0, 0), "정렬");
	MergeCells(1, CenterCol - 2, 1, CenterCol - 1);
	MergeCells(_EndRowForValue + 2, CenterCol - 2, _EndRowForValue + 2, CenterCol - 1);
	// 시장가 매도
	std::string title;
	if (_CenterWnd && _CenterWnd->ShowOrderCountArea())
		title = "시장가매도";
	else
		title = "시장가";
	RegisterButton(2, 1, CenterCol - 2, RGB(19, 137, 255), RGB(255, 255, 255), title.c_str());
	CGridCellBase* pCell = GetCell(1, CenterCol);
	if (pCell) {
		pCell->SetText("시장가");
		// 텍스트 정렬
		pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		// 셀 배경 색 설정
		pCell->SetBackClr(GetSysColor(COLOR_BTNFACE));
	}
	MergeCells(1, CenterCol + 1, 1, CenterCol + 2);
	MergeCells(_EndRowForValue + 2, CenterCol + 1, _EndRowForValue + 2, CenterCol + 2);
	// 시장가 매수 
	if (_CenterWnd && _CenterWnd->ShowOrderCountArea())
		title = "시장가매수";
	else
		title = "시장가";
	RegisterButton(3, 1, CenterCol + 1, RGB(240, 51, 58), RGB(255, 255, 255), title.c_str());
	// 매도스탑 취소 
	RegisterButton(4, _EndRowForValue + 2, CenterCol - 4, RGB(190, 190, 245), "ST취소");
	// 매도주문 취소 
	RegisterButton(5, _EndRowForValue + 2, CenterCol - 3, RGB(190, 190, 245), "일괄취소");
	// 모든주문 취소
	RegisterButton(6, _EndRowForValue + 2, CenterCol, RGB(190, 190, 245), "전체취소");
	// 매수주문취소
	RegisterButton(7, _EndRowForValue + 2, CenterCol + 3, RGB(252, 190, 190), "일괄취소");
	// 매수스탑 취소
	RegisterButton(8, _EndRowForValue + 2, CenterCol + 4, RGB(252, 190, 190), "ST취소");
}

void SmOrderGrid::UnregisterButtons()
{
	for (int id = 1; id <= 9; ++id) {
		UnregisterButton(id);
	}

	RestoreCells(1, CenterCol - 2, 1, CenterCol - 1);
	RestoreCells(_EndRowForValue + 2, CenterCol - 2, _EndRowForValue + 2, CenterCol - 1);

	RestoreCells(1, CenterCol + 1, 1, CenterCol + 2);
	RestoreCells(_EndRowForValue + 2, CenterCol + 1, _EndRowForValue + 2, CenterCol + 2);
}


void SmOrderGrid::BuyAtMarket()
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr)
		return;
	PutOrder(0, VtPositionType::Buy, VtPriceType::Market);
}

void SmOrderGrid::SellAtMarket()
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr)
		return;
	PutOrder(0, VtPositionType::Sell, VtPriceType::Market);
}

void SmOrderGrid::CancelAllSellStop()
{
	_StopOrderMgr->RemoveSellAll();
	_CutMgr->RemoveSellAllHd();
	RefreshStopOrder();
}

void SmOrderGrid::CancelAllBuyStop()
{
	_StopOrderMgr->RemoveBuyAll();
	_CutMgr->RemoveBuyAllHd();
	RefreshStopOrder();
}

void SmOrderGrid::CancelAllSellAccepted()
{
	if (!_CenterWnd)
		return;

	std::vector<VtOrder*> acptOrderList = _OrderConfigMgr->OrderMgr()->GetAcceptedOrders(_CenterWnd->Symbol()->ShortCode);

	for (auto it = acptOrderList.begin(); it != acptOrderList.end(); ++it) {
		VtOrder* order = *it;
		if (order->orderPosition == VtPositionType::Sell)
			CancelOrder(order);
	}
}

void SmOrderGrid::CancelAllBuyAccepted()
{
	if (!_CenterWnd)
		return;

	std::vector<VtOrder*> acptOrderList = _OrderConfigMgr->OrderMgr()->GetAcceptedOrders(_CenterWnd->Symbol()->ShortCode);

	for (auto it = acptOrderList.begin(); it != acptOrderList.end(); ++it) {
		VtOrder* order = *it;
		if (order->orderPosition == VtPositionType::Buy)
			CancelOrder(order);
	}
}

void SmOrderGrid::CancelAllAccepted()
{
	if (!_CenterWnd)
		return;

	std::vector<VtOrder*> acptOrderList = _OrderConfigMgr->OrderMgr()->GetAcceptedOrders(_CenterWnd->Symbol()->ShortCode);

	for (auto it = acptOrderList.begin(); it != acptOrderList.end(); ++it) {
		VtOrder* order = *it;
		CancelOrder(order);
	}
}

void SmOrderGrid::RefreshStopOrder()
{
	std::set<std::pair<int, int>> refreshSet;
	ClearOldStopOrders(refreshSet);
	SetStopOrderInfo(refreshSet);
	CalcPosStopOrders(refreshSet);
	RefreshCells(refreshSet);
}

void SmOrderGrid::CheckProfitLossTouchHd(int intClose)
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_StopOrderMgr)
		return;

	VtSymbol* sym = _CenterWnd->Symbol();

	for (auto it = _StopOrderMgr->StopOrderMapHd.begin(), next_it = it; it != _StopOrderMgr->StopOrderMapHd.end(); it = next_it) {
		HdOrderRequest* stop = it->second;
		++next_it;

		if (stop->Price == intClose) {
			if (stop->Position == VtPositionType::Buy) {
				stop->Price = VtSymbolManager::GetNextValue(stop->Price, stop->slip, sym);
			}
			else {
				stop->Price = VtSymbolManager::GetNextValue(stop->Price, -stop->slip, sym);
			}
			_OrderConfigMgr->OrderMgr()->PutOrder(std::move(*stop));
			// 가격이 닿은 스탑 주문은 목록에서 없애 준다.
			_StopOrderMgr->StopOrderMapHd.erase(it);
		}
	}
}

bool SmOrderGrid::CheckStopTouchedHd(int intClose)
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_CutMgr)
		return false;
	std::vector<int> removeList;
	VtSymbol* sym = _CenterWnd->Symbol();
	std::map<int, HdOrderRequest*>& profitLossMap = _CutMgr->GetStopOrderMap();
	for (auto it = profitLossMap.begin(); it != profitLossMap.end(); ++it) {
		HdOrderRequest* stop = it->second;
		//LOG_F(INFO, _T("CheckProfitLossTouchHd : 손절익절가격 %d"), stop->Price);
		if (stop->Price == intClose) { // 종가와 일치하는 경우
			if (stop->Position == VtPositionType::Buy) { // 매수 포지션인 경우
				stop->Price = VtSymbolManager::GetNextValue(stop->Price, stop->slip, sym);
			}
			else { // 매도 포지션인 경우
				stop->Price = VtSymbolManager::GetNextValue(stop->Price, -stop->slip, sym);
			}

			removeList.push_back(stop->RequestId);  // 삭제 목록에 저장한다.

			if (stop->Paired && stop->PairedReq) { // 익절, 손절 쌍으로 연결되어 있는 경우 한쪽이 사라지면 다른 한쪽도 삭제 목록에 넣어준다.
				removeList.push_back(stop->PairedReq->RequestId);
			}
			_OrderConfigMgr->OrderMgr()->PutOrder(std::move(*stop));
		}
	}
	bool result = false;
	for (auto it = removeList.begin(); it != removeList.end(); ++it) { // 위에서 등록된 삭제 목록들을 목록에서 지워준다.
		_CutMgr->RemoveOrderHd(*it, false);
		result = true;
	}

	return result;
}

void SmOrderGrid::SetColTitle(bool init)
{
	const int ColCount = _ColCount;
	LPCTSTR title[9] = { "STOP", "주문", "건수", "잔량", "정렬", "잔량", "건수", "주문", "STOP" };
	int colWidth[9] = {
		_GridColMap[SmOrderGridCol::STOP],
		_GridColMap[SmOrderGridCol::ORDER],
		_GridColMap[SmOrderGridCol::COUNT],
		_GridColMap[SmOrderGridCol::QUANTITY],
		_GridColMap[SmOrderGridCol::CENTER],
		_GridColMap[SmOrderGridCol::QUANTITY],
		_GridColMap[SmOrderGridCol::COUNT],
		_GridColMap[SmOrderGridCol::ORDER],
		_GridColMap[SmOrderGridCol::STOP]
	};

	SetRowHeight(0, _HeadHeight);

	for (int i = 0; i < _ColCount; i++) {
		if (!init)
			SetColumnWidth(i, colWidth[i]);

		CGridCellBase* pCell = GetCell(0, i);
		pCell->SetText(title[i]);
		// 텍스트 정렬
		pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		// 셀 배경 색 설정
		pCell->SetBackClr(VtGlobal::GridTitleBackColor);
		// 셀 글자색 설정
		pCell->SetTextClr(VtGlobal::GridTitleTextColor);
		
		InvalidateCellRect(0, i);
	}
}

void SmOrderGrid::SetCenterValue(const VtSymbol* symbol, std::set<std::pair<int, int>>& refreshSet)
{
	if (!symbol)
		return;

	std::string code = symbol->ShortCode.substr(0, 1);
	if ((code.compare(_T("2")) == 0) || (code.compare(_T("3")) == 0)) {
		SetCenterValueForOption(symbol, refreshSet);
	}
	else
	{
		SetCenterValueForFuture(symbol, refreshSet);
	}
}

void SmOrderGrid::SetCenterValue()
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;
	VtSymbol* symbol = _CenterWnd->Symbol();
	if (symbol) {
		std::set<std::pair<int, int>> refreshSet;
		SetCenterValue(symbol, refreshSet);
		ClearQuotes(refreshSet);
		SetQuoteColor(symbol, refreshSet);

		ClearHogas(refreshSet);
		SetHogaInfo(symbol, refreshSet);

		ClearOldOrders(refreshSet);
		SetOrderInfo(refreshSet);
		
		ClearOldStopOrders(refreshSet);
		SetStopOrderInfo(refreshSet);
		CalcPosStopOrders(refreshSet);

		ClearPositionInfo(refreshSet);
		SetPositionInfo(refreshSet);

		RefreshCells(refreshSet);
	}
}

void SmOrderGrid::SetCenterValueByFixed(const VtSymbol* symbol, std::set<std::pair<int, int>>& refreshSet)
{
	if (!symbol)
		return;

	std::string code = symbol->ShortCode.substr(0, 1);
	if ((code.compare(_T("2")) == 0) || (code.compare(_T("3")) == 0)){
		SetCenterValueByFixedForOption(symbol, refreshSet);
	}
	else {
		SetCenterValueByFixedForFuture(symbol, refreshSet);
	}
}

void SmOrderGrid::ClearQuotes(std::set<std::pair<int, int>>& refreshSet)
{
	for (auto it = _QuotePos.begin(); it != _QuotePos.end(); ++it) {
		std::pair<int, int> pos = *it;
		if (pos.first < _StartRowForValue ||
			pos.first > _EndRowForValue)
			continue;
		refreshSet.insert(std::make_pair(pos.first, pos.second));
		CGridCellBase* pCell = GetCell(pos.first, pos.second);
		if (pCell) {
			pCell->SetTextClr(RGB(0, 0, 0));
			pCell->SetLabel("");
		}
	}
}

void SmOrderGrid::SetCenterValueForFuture(const VtSymbol* sym, std::set<std::pair<int, int>>& refreshSet)
{
	if (!sym)
		return;

	// Set the close value on the center of the center grid
	int startValue = sym->Quote.intClose + (sym->intTickSize * (_IndexRow - _StartRowForValue));

	ValueToRowMap.clear();
	RowToValueMap.clear();
	for (int i = _StartRowForValue; i <= _EndRowForValue; i++) {
		int curValue = startValue - sym->intTickSize * (i - 1);
		CGridCellBase* pCell = GetCell(i, CenterCol);
		std::string strVal = fmt::format("{:.{}f}", curValue / std::pow(10, sym->Decimal), sym->Decimal);
		pCell->SetText(strVal.c_str());
		pCell->SetLongValue(curValue);
		pCell->SetTextClr(RGB(0, 0, 0));
		// 텍스트 정렬
		pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//RedrawCell(i, CenterCol);
		refreshSet.insert(std::make_pair(i, CenterCol));
		ValueToRowMap[curValue] = i;
		RowToValueMap[i] = curValue;
	}
}

void SmOrderGrid::SetCenterValueForOption(const VtSymbol* sym, std::set<std::pair<int, int>>& refreshSet)
{
	if (!sym)
		return;

	int endValue = sym->Quote.intClose;
	int endRow = _EndRowForValue - 1;
	int zeroRow = _EndRowForValue;
	if (_IndexRow < endRow) {
		for (int r = _IndexRow; r < endRow; ++r) {
			// 0.01 밑으로 안나오게 함
			if (endValue == 1 || endValue == 0) {
				zeroRow = r;
				break;
			}
			if (endValue <= 1000)
				endValue -= sym->intTickSize;
			else
				endValue -= 5;
		}

		if (zeroRow < endRow) {
			_IndexRow = endRow - (zeroRow - _IndexRow) + 1;
		}
	}

	int startValue = sym->Quote.intClose;
	if (_IndexRow > _StartRowForValue) {
		for (int r = _IndexRow; r > _StartRowForValue; --r) {
			if (startValue < 1000)
				startValue += sym->intTickSize;
			else
				startValue += 5;
		}
	}
	else if (_IndexRow <= _StartRowForValue) {
		for (int r = _IndexRow; r < _StartRowForValue; ++r) {
			if (startValue <= 1000)
				startValue -= sym->intTickSize;
			else
				startValue -= 5;
		}
	}
	ValueToRowMap.clear();
	RowToValueMap.clear();
	int curVal = startValue;
	for (int i = _StartRowForValue; i <= _EndRowForValue; ++i) {
		CGridCellBase* pCell = GetCell(i, CenterCol);
		ValueToRowMap[curVal] = i;
		RowToValueMap[i] = curVal;
		std::string strVal = NumberFormatter::format(curVal / std::pow(10, sym->Decimal), sym->Decimal);
		pCell->SetText(strVal.c_str());
		pCell->SetLongValue(curVal);
		pCell->SetTextClr(RGB(0, 0, 0));
		// 텍스트 정렬
		pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		if (curVal > 1000)
			curVal = curVal - 5;
		else
			curVal = curVal - sym->intTickSize;
		//RedrawCell(i, CenterCol);
		refreshSet.insert(std::make_pair(i, CenterCol));
	}
}

void SmOrderGrid::SetCenterValueByFixedForFuture(const VtSymbol* sym, std::set<std::pair<int, int>>& refreshSet)
{
	if (!sym)
		return;

	_IndexRow = FindIndexRow();
	// Set the close value on the center of the center grid
	int startValue = sym->Quote.intClose + (sym->intTickSize * (_IndexRow - _StartRowForValue));

	int lowRow = FindRowFromCenterValue(sym, sym->Quote.intLow);
	int highRow = FindRowFromCenterValue(sym, sym->Quote.intHigh);
	int rangeStart = std::max(highRow - 1, _StartRowForValue);
	int rangeEnd = std::min(lowRow + 1, _EndRowForValue);
	ValueToRowMap.clear();
	RowToValueMap.clear();
	for (int i = _StartRowForValue; i <= _EndRowForValue; i++) {
		int curValue = startValue - sym->intTickSize * (i - 1);
		CGridCellBase* pCell = GetCell(i, CenterCol);
		std::string strVal = NumberFormatter::format(curValue / std::pow(10, sym->Decimal), sym->Decimal);
		pCell->SetText(strVal.c_str());
		pCell->SetLongValue(curValue);
		pCell->SetTextClr(RGB(0, 0, 0));
		// 텍스트 정렬
		pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//RedrawCell(i, CenterCol);
		refreshSet.insert(std::make_pair(i, CenterCol));
		ValueToRowMap[curValue] = i;
		RowToValueMap[i] = curValue;
	}
}

void SmOrderGrid::SetCenterValueByFixedForOption(const VtSymbol* sym, std::set<std::pair<int, int>>& refreshSet)
{
	if (!sym)
		return;

	_IndexRow = FindIndexRow();
	_IndexRow = -1 * _IndexRow;

	// 호가가 바뀌는 행 - 옵션은 10.0부터 호가가 0.05로 그 이하는 0.01로 계산이 된다.
	int Row10 = 0;
	if (1000 <= sym->Quote.intClose) {
		int delta = sym->Quote.intClose - 1000;
		int deltaRow = delta / 5;
		Row10 = _IndexRow - deltaRow;
	}
	else {
		int delta = 1000 - sym->Quote.intClose;
		int deltaRow = delta / sym->intTickSize;
		Row10 = _IndexRow + deltaRow;
	}

	int startValue = 0;
	if (_StartRowForValue >= Row10) {
		startValue = (_StartRowForValue - Row10) * 5 + 1000;
	}
	else {
		startValue = 1000 - (Row10 - _StartRowForValue) * sym->intTickSize;
	}

	ValueToRowMap.clear();
	RowToValueMap.clear();
	int curVal = startValue;
	for (int i = _StartRowForValue; i <= _EndRowForValue; ++i) {
		ValueToRowMap[curVal] = i;
		RowToValueMap[i] = curVal;
		CGridCellBase* pCell = GetCell(i, CenterCol);
		std::string strVal = NumberFormatter::format(curVal / std::pow(10, sym->Decimal), sym->Decimal);
		pCell->SetText(strVal.c_str());
		pCell->SetLongValue(curVal);
		pCell->SetTextClr(RGB(0, 0, 0));
		// 텍스트 정렬
		pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//RedrawCell(i, CenterCol);
		refreshSet.insert(std::make_pair(i, CenterCol));
		if (curVal > 1000)
			curVal = curVal - 5;
		else
			curVal = curVal - sym->intTickSize;
	}
}

int SmOrderGrid::FindIndexRow()
{
	int count = GetMaxValueRowCount();

	return ((int)(count / 2) + 1);
}

int SmOrderGrid::GetMaxValueRowCount()
{
	RECT rect;
	GetClientRect(&rect);


	double pureHeight = rect.bottom - rect.top - _HeadHeight;

	int count = (int)(pureHeight / _CellHeight);

	return count;
}


void SmOrderGrid::SetHogaInfo(const VtSymbol* sym, std::set<std::pair<int, int>>& refreshSet)
{
	if (!sym)
		return;
	_HogaPos.clear();
	for (int i = 0; i < 5; i++) {
		int pos = FindRowFromCenterValue(sym->Hoga.Ary[i].IntBuyPrice);
		if (pos >= _StartRowForValue && pos <= _EndRowForValue) {
			CGridCellBase* pCell = GetCell(pos, CenterCol + 2);
			std::string strVal = fmt::format("{}", sym->Hoga.Ary[i].BuyNo);
			pCell->SetText(strVal.c_str());
			pCell->SetBackClr(BuyColor[i]);
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			_HogaPos.insert(std::make_pair(pos, CenterCol + 2));
			refreshSet.insert(std::make_pair(pos, CenterCol + 2));

			pCell = GetCell(pos, CenterCol + 1);
			strVal = fmt::format("{}", sym->Hoga.Ary[i].BuyQty);
			pCell->SetText(strVal.c_str());
			pCell->SetBackClr(BuyColor[i]);
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			_HogaPos.insert(std::make_pair(pos, CenterCol + 1));
			refreshSet.insert(std::make_pair(pos, CenterCol + 1));
		}

		pos = FindRowFromCenterValue(sym->Hoga.Ary[i].IntSellPrice);
		if (i == 0) {
			_CloseLineRow = pos;
			for (int k = 0; k < m_nCols; ++k) {
				CGridCellBase* pCell = GetCell(pos, k);
				if (pCell)
					pCell->Style(1);
			}
		}
		if (pos >= _StartRowForValue && pos <= _EndRowForValue) {
			CGridCellBase* pCell = GetCell(pos, CenterCol - 2);
			std::string strVal = fmt::format("{}", sym->Hoga.Ary[i].SellNo);
			pCell->SetText(strVal.c_str());
			pCell->SetBackClr(SellColor[i]);
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			_HogaPos.insert(std::make_pair(pos, CenterCol - 2));
			refreshSet.insert(std::make_pair(pos, CenterCol - 2));

			pCell = GetCell(pos, CenterCol - 1);
			strVal = fmt::format("{}", sym->Hoga.Ary[i].SellQty);
			pCell->SetText(strVal.c_str());
			pCell->SetBackClr(SellColor[i]);
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			_HogaPos.insert(std::make_pair(pos, CenterCol - 1));
			refreshSet.insert(std::make_pair(pos, CenterCol - 1));
		}
	}

	// 총호가 건수, 수량 설정
	{
		CGridCellBase* pCell = GetCell(_EndRowForValue + 1, CenterCol - 2);
		std::string strVal = fmt::format("{}", sym->Hoga.TotSellNo);
		if (pCell) {
			pCell->SetText(strVal.c_str());
			//pCell->SetBackClr(SellColor[0]);
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			_HogaPos.insert(std::make_pair(_EndRowForValue + 1, CenterCol - 2));
			refreshSet.insert(std::make_pair(_EndRowForValue + 1, CenterCol - 2));
		}

		pCell = GetCell(_EndRowForValue + 1, CenterCol - 1);
		if (pCell) {
			strVal = fmt::format("{}", sym->Hoga.TotSellQty);
			pCell->SetText(strVal.c_str());
			//pCell->SetBackClr(SellColor[0]);
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			_HogaPos.insert(std::make_pair(_EndRowForValue + 1, CenterCol - 1));
			refreshSet.insert(std::make_pair(_EndRowForValue + 1, CenterCol - 1));
		}

		pCell = GetCell(_EndRowForValue + 1, CenterCol + 1);
		if (pCell) {
			strVal = fmt::format("{}", sym->Hoga.TotBuyQty);
			pCell->SetText(strVal.c_str());
			//pCell->SetBackClr(BuyColor[0]);
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			_HogaPos.insert(std::make_pair(_EndRowForValue + 1, CenterCol + 1));
			refreshSet.insert(std::make_pair(_EndRowForValue + 1, CenterCol + 1));
		}

		pCell = GetCell(_EndRowForValue + 1, CenterCol + 2);
		if (pCell) {
			strVal = fmt::format("{}", sym->Hoga.TotBuyNo);
			pCell->SetText(strVal.c_str());
			//pCell->SetBackClr(BuyColor[0]);
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			_HogaPos.insert(std::make_pair(_EndRowForValue + 1, CenterCol + 2));
			refreshSet.insert(std::make_pair(_EndRowForValue + 1, CenterCol + 2));
		}

		int dif = sym->Hoga.TotBuyQty - sym->Hoga.TotSellQty;

		pCell = GetCell(_EndRowForValue + 1, CenterCol);
		if (pCell) {
			strVal = fmt::format("{}", dif);
			pCell->SetText(strVal.c_str());
			//pCell->SetBackClr(BuyColor[0]);
			pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			if (dif > 0)
			{
				pCell->SetTextClr(RGB(255, 0, 0));
			}
			else
			{
				pCell->SetTextClr(RGB(0, 0, 255));
			}
			_HogaPos.insert(std::make_pair(_EndRowForValue + 1, CenterCol + 2));
			refreshSet.insert(std::make_pair(_EndRowForValue + 1, CenterCol + 2));
		}
	}
}

int SmOrderGrid::FindRowFromCenterValue(int value)
{
	auto it = ValueToRowMap.find(value);
	if (it != ValueToRowMap.end()) {
		return it->second;
	}
	else {
		return -2;
	}
}

int SmOrderGrid::FindRowFromCenterValue(const VtSymbol* sym, int value)
{
	if (!sym || ValueToRowMap.size() == 0)
		return 0;
	auto it = ValueToRowMap.find(value);
	if (it != ValueToRowMap.end()) { // 값이 보이는 범위 안에 있을 때
		return it->second;
	}
	else { // 값이 보이는 범위 밖에 있을 때
		auto itr = ValueToRowMap.rbegin();
		int bigVal = itr->first;
		int bigRow = itr->second;
		int thousandRow = 0;
		// 가격이 10인 행을 찾는 과정 - 10이상이면 승수가 변한다.
		if (bigVal >= 1000) { // 최상위 값이 10보다 이상일 경우
			int delta = bigVal - 1000;
			int deltaRow = delta / sym->intTickSize;
			thousandRow = bigRow + deltaRow;
		}
		else { // 최상위 값이 10미만인 경우
			int delta = 1000 - bigVal;
			thousandRow = bigRow - delta;
		}

		if (value >= 1000) { // 가격이 10 이상인 있는 경우 - 종목의 틱크기 만큼 변함
			int delta = value - 1000;
			int deltaRow = delta / sym->intTickSize;
			return thousandRow - deltaRow;
		}
		else { // 가격이 10 미만인 경우 - 종목에 관계없이 1씩 변함
			int delta = 1000 - value;
			return thousandRow + delta;
		}
	}
}

void SmOrderGrid::SetQuoteColor(const VtSymbol* sym, std::set<std::pair<int, int>>& refreshSet)
{
	if (!sym)
		return;

	if (ValueToRowMap.size() == 0)
		return;

	CUGCell cell;

	int lowRow = FindRowFromCenterValue(sym, sym->Quote.intLow);
	int highRow = FindRowFromCenterValue(sym, sym->Quote.intHigh);
	int closeRow = FindRowFromCenterValue(sym, sym->Quote.intClose);
	int openRow = FindRowFromCenterValue(sym, sym->Quote.intOpen);
	int preCloseRow = FindRowFromCenterValue(sym, sym->Quote.intPreClose);

	if (sym->Quote.intClose > sym->Quote.intOpen) { // 양봉
		for (auto it = ValueToRowMap.rbegin(); it != ValueToRowMap.rend(); ++it) {
			if (it->second < highRow) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(242, 242, 242));
			}
			else if (it->second < closeRow) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(255, 255, 255));
			}
			else if (it->second <= openRow) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(252, 226, 228));
			}
			else if (it->second < lowRow + 1) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(255, 255, 255));
			}
			else {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(242, 242, 242));
			}
		}

	}
	else if (sym->Quote.intClose < sym->Quote.intOpen) { // 음봉
		for (auto it = ValueToRowMap.rbegin(); it != ValueToRowMap.rend(); ++it) {
			if (it->second < highRow) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(242, 242, 242));
			}
			else if (it->second < openRow) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(255, 255, 255));
			}
			else if (it->second <= closeRow) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(218, 226, 245));
			}
			else if (it->second < lowRow + 1) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(255, 255, 255));
			}
			else {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(242, 242, 242));
			}
		}
	}
	else { // 도지
		for (auto it = ValueToRowMap.rbegin(); it != ValueToRowMap.rend(); ++it) {
			if (it->second < highRow) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(242, 242, 242));
			}
			else if (it->second < closeRow) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(255, 255, 255));
			}
			else if (it->second <= openRow) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(252, 226, 228));
			}
			else if (it->second < lowRow + 1) {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(255, 255, 255));
			}
			else {
				CGridCellBase* pCell = GetCell(it->second, CenterCol);
				pCell->SetBackClr(RGB(242, 242, 242));
			}
		}
	}

	// 종가 색상 입히기
	CGridCellBase* pCell = GetCell(closeRow, CenterCol);
	if (pCell && closeRow >= _StartRowForValue && closeRow <= _EndRowForValue) {
		pCell->SetBackClr(RGB(255, 255, 0));
		_QuotePos.insert(std::make_pair(closeRow, CenterCol));
	}
	// 고가 텍스트 색상 설정
	SetSiseLabel(highRow, "H", RGB(255, 0, 0));
	
	// 저가 텍스트 색상 설정
	SetSiseLabel(lowRow, "L", RGB(0, 0, 255));

	// 시작가 레이블 설정
	SetSiseLabel(openRow, "O", RGB(0, 0, 0));
	
	// 이전 종가 레이블 설정
	SetSiseLabel(preCloseRow, "C", RGB(0, 0, 0));
	
	if (_CenterWnd->FixedCenter()) {
		for (auto it = ValueToRowMap.begin(); it != ValueToRowMap.end(); ++it) {
			refreshSet.insert(std::make_pair(it->second, CenterCol));
		}
	}
	else {
		int min_row = std::min(closeRow, _OldIndexRow);
		int max_row = std::max(closeRow, _OldIndexRow);
		if (min_row < _StartRowForValue)
			min_row = _StartRowForValue;
		if (max_row > _EndRowForValue)
			max_row = _EndRowForValue;
		_OldIndexRow = closeRow;
		// 표시 범위를 벗어날을 때는 갱신하지 않는다.
		if (max_row < _StartRowForValue || min_row > _EndRowForValue)
			return;
		// 표시 범위 안에 있는 것만 업데이트 한다.
		for (int i = min_row; i <= max_row; ++i) {
			refreshSet.insert(std::make_pair(i, CenterCol));
		}
	}
}

void SmOrderGrid::SetSiseCellBackColor(int minRow, int maxRow, int start, int end, COLORREF color)
{
	if (end < minRow || start > maxRow) {
		return;
	}

	start = max(minRow, start);
	end = min(maxRow, end);

	for (int i = start; i < end; ++i) {
		CGridCellBase* pCell = GetCell(i, CenterCol);
		pCell->SetBackClr(color);
	}
}

void SmOrderGrid::SetSiseLabel(int row, LPCTSTR text, COLORREF textColor)
{
	if (row < _StartRowForValue || row > _EndRowForValue)
		return;
	CGridCellBase* pCell = GetCell(row, CenterCol);
	if (pCell) {
		pCell->SetLabel(text);
		pCell->SetTextClr(textColor);
		_QuotePos.insert(std::make_pair(row, CenterCol));
	}
}

void SmOrderGrid::ClearHogas(std::set<std::pair<int, int>>& refreshSet)
{
	// 호가 표시된 기존 셀을 초기화 시킨다.
	for (auto it = _HogaPos.begin(); it != _HogaPos.end(); ++it){
		std::pair<int, int> pos = *it;
		CCellID cell(pos.first, pos.second);
		if (cell.IsValid() == TRUE) {
			CGridCellBase* pCell = GetCell(cell.row, cell.col);
			if (pCell) {
				pCell->SetText("");
				pCell->SetBackClr(RGB(255, 255, 255));
				refreshSet.insert(std::make_pair(pos.first, pos.second));
			}
		}
	}

	// 호가 라인을 다시 정상으로 되돌려 놓는다.
	for (int k = 0; k < m_nCols; ++k) {
		CCellID cell(_CloseLineRow, k);
		if (cell.IsValid() == TRUE) {
			CGridCellBase* pCell = GetCell(_CloseLineRow, k);
			if (pCell)
				pCell->Style(0);
		}
	}
}

void SmOrderGrid::SetOrderInfo(std::set<std::pair<int, int>>& refreshSet)
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr || !_OrderConfigMgr->OrderMgr())
		return;

	VtSymbol* sym = _CenterWnd->Symbol();

	_OrderPos.clear();

	std::vector<VtOrder*> acptOrderList = _OrderConfigMgr->OrderMgr()->GetAcceptedOrders(sym->ShortCode);

	int buy_count = 0, sell_count = 0;
	CGridCellBase* pCell = nullptr;
	std::string strVal;
	for (auto it = acptOrderList.begin(); it != acptOrderList.end(); ++it) {
		VtOrder* order = *it;
		if (order->amount == 0)
			continue;
		order->orderPosition == VtPositionType::Buy ? buy_count += order->amount : sell_count += order->amount;
		int row = FindRowFromCenterValue(sym, order->intOrderPrice);
		if (row >= _StartRowForValue && row <= _EndRowForValue) {
			if (order->orderPosition == VtPositionType::Sell) {
				pCell = GetCell(row, CenterCol - 3);
				pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->AddOrder(order);
				_OrderPos.insert(std::make_pair(row, CenterCol - 3));
				refreshSet.insert(std::make_pair(row, CenterCol - 3));
			}
			else if (order->orderPosition == VtPositionType::Buy) {
				pCell = GetCell(row, CenterCol + 3);
				pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				pCell->AddOrder(order);
				_OrderPos.insert(std::make_pair(row, CenterCol + 3));
				refreshSet.insert(std::make_pair(row, CenterCol + 3));
			}
			// 주문 갯수 설정
			strVal = fmt::format("{}", pCell->GetOrderCount());
			pCell->SetText(strVal.c_str());	
		}
	}

	// 전체 주문 갯수 설정
	pCell = GetCell(1, CenterCol + 3);
	if (pCell) {
		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		_OrderPos.insert(std::make_pair(1, CenterCol + 3));
		refreshSet.insert(std::make_pair(1, CenterCol + 3));
		strVal = fmt::format("{}", buy_count);
		pCell->SetText(strVal.c_str());
	}

	pCell = GetCell(1, CenterCol - 3);
	if (pCell) {
		pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		_OrderPos.insert(std::make_pair(1, CenterCol - 3));
		refreshSet.insert(std::make_pair(1, CenterCol - 3));
		strVal = fmt::format("{}", sell_count);
		pCell->SetText(strVal.c_str());
	}
}

void SmOrderGrid::ClearOldOrders(std::set<std::pair<int, int>>& refreshSet)
{
	for (auto it = _OrderPos.begin(); it != _OrderPos.end(); ++it) {
		std::pair<int, int> pos = *it;
		CGridCellBase* pCell = GetCell(pos.first, pos.second);
		if (pCell) {
			pCell->SetText("");
			pCell->ClearOrders();
			refreshSet.insert(std::make_pair(pos.first, pos.second));
		}
	}
}

void SmOrderGrid::SetStopOrderInfo(std::set<std::pair<int, int>>& refreshSet)
{
	_StopOrderPos.clear();
	CGridCellBase* pCell = nullptr;
	std::string strVal;
	int buy_count = 0, sell_count = 0;
	// 일반 스탑주문을 표시해 준다.
	for (auto it = _StopOrderMgr->StopOrderMapHd.begin(); it != _StopOrderMgr->StopOrderMapHd.end(); ++it) {
		HdOrderRequest* order = it->second;
		order->Position == VtPositionType::Buy ? buy_count += order->Amount : sell_count += order->Amount;
		int row = FindRowFromCenterValue(order->Price);
		if (row >= _StartRowForValue && row <= _EndRowForValue) {
			if (order->Position == VtPositionType::Sell) {
				pCell = GetCell(row, CenterCol - 4);
				pCell->AddStopOrder(order);
				pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				_StopOrderPos.insert(std::make_pair(row, CenterCol - 4));
				refreshSet.insert(std::make_pair(row, CenterCol - 4));
			}
			else if (order->Position == VtPositionType::Buy) {
				pCell = GetCell(row, CenterCol + 4);
				pCell->AddStopOrder(order);
				pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				_StopOrderPos.insert(std::make_pair(row, CenterCol + 4));
				refreshSet.insert(std::make_pair(row, CenterCol + 4));
			}
			// 스탑 주문 갯수 설정
			strVal = fmt::format("{}({})", pCell->GetStopOrderCount(), _StopOrderMgr->Slip());
			pCell->SetText(strVal.c_str());
		}
	}

	// 자동 손절로 인한 스탑주문은 따로 추가해 준다.
	std::map<int, HdOrderRequest*>& profitLossMap = _CutMgr->GetStopOrderMap();
	for (auto it = profitLossMap.begin(); it != profitLossMap.end(); ++it) {
		HdOrderRequest* order = it->second;
		order->Position == VtPositionType::Buy ? buy_count += order->Amount : sell_count += order->Amount;
		int row = FindRowFromCenterValue(order->Price);
		if (row >= _StartRowForValue && row <= _EndRowForValue) {
			if (order->Position == VtPositionType::Sell) {
				pCell = GetCell(row, CenterCol - 4);
				pCell->AddStopOrder(order);
				pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				_StopOrderPos.insert(std::make_pair(row, CenterCol - 4));
				refreshSet.insert(std::make_pair(row, CenterCol - 4));
			}
			else if (order->Position == VtPositionType::Buy) {
				pCell = GetCell(row, CenterCol + 4);
				pCell->AddStopOrder(order);
				pCell->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				_StopOrderPos.insert(std::make_pair(row, CenterCol + 4));
				refreshSet.insert(std::make_pair(row, CenterCol + 4));
			}
			// 스탑 주문 갯수 설정
			strVal = fmt::format("{}({})", pCell->GetStopOrderCount(), _StopOrderMgr->Slip());
			pCell->SetText(strVal.c_str());
		}
	}

	// 전체 주문 갯수 설정
	pCell = GetCell(1, CenterCol + 4);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	_StopOrderPos.insert(std::make_pair(1, CenterCol + 4));
	refreshSet.insert(std::make_pair(1, CenterCol + 4));
	strVal = fmt::format("{}", buy_count);
	pCell->SetText(strVal.c_str());

	pCell = GetCell(1, CenterCol - 4);
	pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	_StopOrderPos.insert(std::make_pair(1, CenterCol - 4));
	refreshSet.insert(std::make_pair(1, CenterCol - 4));
	strVal = fmt::format("{}", sell_count);
	pCell->SetText(strVal.c_str());
}

void SmOrderGrid::ClearOldStopOrders(std::set<std::pair<int, int>>& refreshSet)
{
	for (auto it = _StopOrderPos.begin(); it != _StopOrderPos.end(); ++it) {
		std::pair<int, int> pos = *it;
		CGridCellBase* pCell = GetCell(pos.first, pos.second);
		pCell->SetText("");
		pCell->ClearStopOrders();
		refreshSet.insert(std::make_pair(pos.first, pos.second));
	}
}

void SmOrderGrid::CalcPosStopOrders(std::set<std::pair<int, int>>& refreshSet)
{
	ClearStopOrderVectors();
	for (auto it = _StopOrderMgr->StopOrderMapHd.begin(); it != _StopOrderMgr->StopOrderMapHd.end(); ++it) {
		HdOrderRequest* order = it->second;
		int order_row = FindRowFromCenterValue(order->Price);
		CCellID order_cell, slip_cell;
		order_cell.row = order_row;
		if (order->Position == VtPositionType::Buy) {
			order_cell.col = CenterCol + 4;
			slip_cell.col = CenterCol + 3;
			order->slip > 0 ? slip_cell.row = order_row - order->slip : slip_cell.row = order_row + order->slip;
		}
		else if (order->Position == VtPositionType::Sell) {
			order_cell.col = CenterCol - 4;
			slip_cell.col = CenterCol - 3;
			order->slip > 0 ? slip_cell.row = order_row + order->slip : slip_cell.row = order_row - order->slip;
		}

		if (order_cell.row < _StartRowForValue || order_cell.row > _EndRowForValue ||
			slip_cell.row  < _StartRowForValue || slip_cell.row > _EndRowForValue)
			continue;
	
		PutStopOrderVector(std::make_pair(order_cell, slip_cell));

		int min_row = min(order_cell.row, slip_cell.row);
		int max_row = max(order_cell.row, slip_cell.row);
		int min_col = min(order_cell.col, slip_cell.col);
		int max_col = max(order_cell.col, slip_cell.col);
		for (int r = min_row; r <= max_row; ++r) {
			for (int c = min_col; c <= max_col; ++c) {
				refreshSet.insert(std::make_pair(r, c));
			}
		}
	}

	std::map<int, HdOrderRequest*>& profitLossMap = _CutMgr->GetStopOrderMap();
	for (auto it = profitLossMap.begin(); it != profitLossMap.end(); ++it) {
		HdOrderRequest* order = it->second;
		int order_row = FindRowFromCenterValue(order->Price);
		CCellID order_cell, slip_cell;
		order_cell.row = order_row;
		if (order->Position == VtPositionType::Buy) {
			order_cell.col = CenterCol + 4;
			slip_cell.col = CenterCol + 3;
			order->slip > 0 ? slip_cell.row = order_row - order->slip : slip_cell.row = order_row + order->slip;
		}
		else if (order->Position == VtPositionType::Sell) {
			order_cell.col = CenterCol - 4;
			slip_cell.col = CenterCol - 3;
			order->slip > 0 ? slip_cell.row = order_row + order->slip : slip_cell.row = order_row - order->slip;
		}

		if (order_cell.row < _StartRowForValue || order_cell.row > _EndRowForValue ||
			slip_cell.row  < _StartRowForValue || slip_cell.row > _EndRowForValue)
			continue;

		PutStopOrderVector(std::make_pair(order_cell, slip_cell));

		int min_row = min(order_cell.row, slip_cell.row);
		int max_row = max(order_cell.row, slip_cell.row);
		int min_col = min(order_cell.col, slip_cell.col);
		int max_col = max(order_cell.col, slip_cell.col);
		for (int r = min_row; r <= max_row; ++r) {
			for (int c = min_col; c <= max_col; ++c) {
				refreshSet.insert(std::make_pair(r, c));
			}
		}
	}

	Invalidate();
}

void SmOrderGrid::RefreshCells(std::set<std::pair<int, int>>& refreshSet)
{
	for (auto it = refreshSet.begin(); it != refreshSet.end(); ++it) {
		std::pair<int, int> pos = *it;
		CGridCellBase* pCell = GetCell(pos.first, pos.second);
		InvalidateCellRect(pos.first, pos.second);
	}
}

void SmOrderGrid::PutOrder(int price, VtPositionType position, VtPriceType priceType /*= VtPriceType::Price*/)
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr)
		return;

	if (_OrderConfigMgr->Type() == 0) { // 실계좌나 서브 계좌 일 때
		if (_OrderConfigMgr->Account()) {
			VtAccount* acnt = _OrderConfigMgr->Account();

			HdOrderRequest request;
			request.Price = price;
			request.Position = position;
			request.Amount = _CenterWnd->OrderAmount();
			if (acnt->AccountLevel() == 0) { // 실계좌 일 때
				request.AccountNo = acnt->AccountNo;
				request.Password = acnt->Password;
				request.Type = 0;
			}
			else { // 서브 계좌 일 때
				VtAccount* parentAcnt = acnt->ParentAccount();
				if (parentAcnt) { // 본계좌의 정보를 넣어 준다.
					request.AccountNo = parentAcnt->AccountNo;
					request.Password = parentAcnt->Password;
				}
				request.Type = 1;
			}
			request.SymbolCode = _CenterWnd->Symbol()->ShortCode;
			request.FillCondition = _CenterWnd->FillCondition();
			request.PriceType = priceType;

			request.RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
			request.SourceId = (long)this;
			if (acnt->AccountLevel() == 0) { // 실계좌일 때
				request.SubAccountNo = _T("");
				request.FundName = _T("");
			}
			else { // 서브 계좌 일 때
				request.SubAccountNo = acnt->AccountNo;
				request.FundName = _T("");
			}

			request.AccountLevel = _OrderConfigMgr->Account()->AccountLevel();
			request.orderType = VtOrderType::New;
			_OrderConfigMgr->OrderMgr()->PutOrder(std::move(request));
		}
	}
	else { // 펀드 주문 일 때
		if (_CenterWnd->OrderByRemain()) { // 체결된 잔고가 설정되어 있을 때는 이 청산 루틴을 탄다.
			LiqudAll(VtPriceType::Price, price);
			_CenterWnd->ResetRemainFund();
			return;
		}

		if (_OrderConfigMgr->Fund()) {
			VtOrderManagerSelector* orderMgrSelector = VtOrderManagerSelector::GetInstance();
			std::vector<VtAccount*>& fundAcntList = _OrderConfigMgr->Fund()->GetFundAccountVector();
			for (auto it = fundAcntList.begin(); it != fundAcntList.end(); ++it) { // 펀드 계좌를 돌면서 주문을 내준다.
				VtAccount* subAcnt = *it;
				VtAccount* parentAcnt = subAcnt->ParentAccount();
				VtOrderManager* orderMgr = orderMgrSelector->FindAddOrderManager(subAcnt->AccountNo);

				HdOrderRequest request;
				request.Type = 2;
				request.Price = price;
				request.Position = position;
				request.Amount = _CenterWnd->OrderAmount() * subAcnt->SeungSu;
				if (parentAcnt) { // 부모 계좌의 정보를 넣어 준다.
					request.AccountNo = parentAcnt->AccountNo;
					request.Password = parentAcnt->Password;
				}
				else
					continue;
				request.SymbolCode = _CenterWnd->Symbol()->ShortCode;
				request.FillCondition = _CenterWnd->FillCondition();
				request.PriceType = priceType;

				request.RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
				request.SourceId = (long)this;
				request.SubAccountNo = subAcnt->AccountNo;
				request.FundName = _OrderConfigMgr->Fund()->Name;
				request.AccountLevel = _OrderConfigMgr->Account()->AccountLevel();
				request.orderType = VtOrderType::New;
				orderMgr->PutOrder(std::move(request));
			}
		}
	}
}

void SmOrderGrid::PutOrder(VtPosition* posi, int price, bool liqud /*= false*/)
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !posi || !_OrderConfigMgr)
		return;
	if (posi->OpenQty == 0)
		return;

	VtSymbol* sym = _CenterWnd->Symbol();
	VtAccount* acnt = nullptr;
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	acnt = acntMgr->FindAccount(posi->AccountNo);
	if (!acnt) { // 본계좌에 없을 경우 서브 계좌를 찾아 본다.
		VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
		acnt = subAcntMgr->FindAccount(posi->AccountNo);
	}

	if (!acnt) // 계좌가 없는 경우 아무일도 하지 않음.
		return;

	HdOrderRequest request;
	request.Amount = std::abs(posi->OpenQty);
	if (_OrderConfigMgr->Type() == 0) { // 본계좌나 서브 계좌일때
		if (acnt->AccountLevel() == 0) { // 본계좌 일 때
			request.AccountNo = acnt->AccountNo;
			request.Password = acnt->Password;
			request.Type = 0;
		}
		else { // 서브 계좌 일 때
			VtAccount* parentAcnt = acnt->ParentAccount();
			if (parentAcnt) { // 부모 계좌가 있는지 확인 한다.
				request.AccountNo = parentAcnt->AccountNo;
				request.Password = parentAcnt->Password;
			}
			request.Type = 1;
		}
	}
	else { // 펀드 주문일때
		VtAccount* parentAcnt = acnt->ParentAccount();
		if (parentAcnt) { // 부모 계좌가 있는지 확인 한다.
			request.AccountNo = parentAcnt->AccountNo;
			request.Password = parentAcnt->Password;
		}
		request.Type = 2;
	}
	
	request.SymbolCode = _CenterWnd->Symbol()->ShortCode;
	request.FillCondition = _CenterWnd->FillCondition();

	if (liqud) { // 시장가 청산인 경우
		request.Price = 0;
		request.PriceType = VtPriceType::Market;
	}
	else { // 지정가 청산인 경우
		request.PriceType = VtPriceType::Price;
		request.Price = price;
	}

	if (posi->Position == VtPositionType::Buy) { // 매수 청산인 경우
		request.Position = VtPositionType::Sell;
	}
	else if (posi->Position == VtPositionType::Sell) { // 매도 청산인 경우
		request.Position = VtPositionType::Buy;
	}

	request.RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
	request.SourceId = (long)this;

	if (acnt->AccountLevel() == 0) { // 본계좌 인 경우
		request.SubAccountNo = _T("");
		request.FundName = _T("");
	}
	else { // 서브 계좌인 경우
		request.SubAccountNo = acnt->AccountNo;
		if (acnt->Fund())
			request.FundName = acnt->Fund()->Name;
		else
			request.FundName = _T("");
	}

	request.orderType = VtOrderType::New;
	// 정산 주문이냐 청산
	// 0 : 일반 , 1 : 익절, 2 : 손절- 청산 주문일 때는 익절 손절이 활성화 되어 있어도 처리하지 않는다.
	request.RequestType = 1;

	_OrderConfigMgr->OrderMgr()->PutOrder(std::move(request));
}

void SmOrderGrid::RefreshOrderPosition()
{
	std::set<std::pair<int, int>> refreshSet;
	ClearOldOrders(refreshSet);
	SetOrderInfo(refreshSet);
	RefreshCells(refreshSet);

	//LOG_F(INFO, _T("주문그리드 갱신"));
}

void SmOrderGrid::SetOrderArea(int height, int width)
{
	_CellHeight = height;
	_GridColMap[SmOrderGridCol::ORDER] = width;
}

void SmOrderGrid::AddOrderToCell(VtOrder* order)
{
	if (!order)
		return;
	if (order->amount == 0)
		return;
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;
	VtSymbol* sym = _CenterWnd->Symbol();
	CGridCellBase* pCell = nullptr;
	int row = FindRowFromCenterValue(sym, order->intOrderPrice);
	if (row >= _StartRowForValue && row <= _EndRowForValue) {
		if (order->orderPosition == VtPositionType::Sell) {
			pCell = GetCell(row, CenterCol - 3);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->AddOrder(order);
			_OrderPos.insert(std::make_pair(row, CenterCol - 3));
			// 주문 갯수 설정
			std::string strVal = fmt::format("{}", pCell->GetOrderCount());
			pCell->SetText(strVal.c_str());
			InvalidateCellRect(row, CenterCol - 3);
		}
		else if (order->orderPosition == VtPositionType::Buy) {
			pCell = GetCell(row, CenterCol + 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->AddOrder(order);
			_OrderPos.insert(std::make_pair(row, CenterCol + 3));
			// 주문 갯수 설정
			std::string strVal = fmt::format("{}", pCell->GetOrderCount());
			pCell->SetText(strVal.c_str());
			InvalidateCellRect(row, CenterCol - 3);
		}
	}
}

void SmOrderGrid::RemoveOrderFromCell(VtOrder* order)
{
	if (!order)
		return;
	if (order->amount == 0)
		return;
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;
	VtSymbol* sym = _CenterWnd->Symbol();
	CGridCellBase* pCell = nullptr;
	int row = FindRowFromCenterValue(sym, order->intOrderPrice);
	if (row >= _StartRowForValue && row <= _EndRowForValue) {
		if (order->orderPosition == VtPositionType::Sell) {
			pCell = GetCell(row, CenterCol - 3);
			pCell->SetFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->RemoveOrder(order->orderNo);
			_OrderPos.insert(std::make_pair(row, CenterCol - 3));
			// 주문 갯수 설정
			std::string strVal = fmt::format("{}", pCell->GetOrderCount());
			pCell->SetText(strVal.c_str());
			InvalidateCellRect(row, CenterCol - 3);
		}
		else if (order->orderPosition == VtPositionType::Buy) {
			pCell = GetCell(row, CenterCol + 3);
			pCell->SetFormat(DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pCell->RemoveOrder(order->orderNo);
			_OrderPos.insert(std::make_pair(row, CenterCol + 3));
			// 주문 갯수 설정
			std::string strVal = fmt::format("{}", pCell->GetOrderCount());
			pCell->SetText(strVal.c_str());
			InvalidateCellRect(row, CenterCol + 3);
		}
	}
}

void SmOrderGrid::RefreshTotalOrderCount(VtOrder* input_order)
{
	if (!input_order)
		return;

	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr || !_OrderConfigMgr->OrderMgr())
		return;

	VtSymbol* sym = _CenterWnd->Symbol();

	std::vector<VtOrder*> acptOrderList = _OrderConfigMgr->OrderMgr()->GetAcceptedOrders(sym->ShortCode);

	int buy_count = 0, sell_count = 0;
	for (auto it = acptOrderList.begin(); it != acptOrderList.end(); ++it) {
		VtOrder* order = *it;
		order->orderPosition == VtPositionType::Buy ? buy_count += order->amount : sell_count += order->amount;
	}

	CGridCellBase* pCell = nullptr;
	std::string strVal;
	if (input_order->orderPosition == VtPositionType::Buy) {
		// 전체 주문 갯수 설정
		pCell = GetCell(1, CenterCol + 3);
		if (pCell) {
			_OrderPos.insert(std::make_pair(1, CenterCol + 3));
			strVal = fmt::format("{}", buy_count);
			pCell->SetText(strVal.c_str());
			InvalidateCellRect(1, CenterCol + 3);
		}
	}

	if (input_order->orderPosition == VtPositionType::Sell) {
		pCell = GetCell(1, CenterCol - 3);
		if (pCell) {
			_OrderPos.insert(std::make_pair(1, CenterCol - 3));
			strVal = fmt::format("{}", sell_count);
			pCell->SetText(strVal.c_str());
			InvalidateCellRect(1, CenterCol - 3);
		}
	}
}

void SmOrderGrid::SetOrderAreaColor()
{
	CGridCellBase* pCell = nullptr;
	for (int i = 1; i <= _EndRowForValue + 1; ++i) {
		pCell = GetCell(i, CenterCol - 3);
		pCell->SetBackClr(RGB(218, 226, 245));
		pCell = GetCell(i, CenterCol + 3);
		pCell->SetBackClr(RGB(252, 226, 228));
	}

	pCell = GetCell(1, CenterCol - 4);
	pCell->SetBackClr(RGB(218, 226, 245));
	pCell = GetCell(1, CenterCol + 4);
	pCell->SetBackClr(RGB(252, 226, 228));
}

void SmOrderGrid::SetPositionInfo(std::set<std::pair<int, int>>& refreshSet)
{
	if (!_OrderConfigMgr)
		return;
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;
	if (_OrderConfigMgr->Type() == 0)
	{
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (!acnt)
			return;
		VtPosition* posi = acnt->FindPosition(_CenterWnd->Symbol()->ShortCode);
		ShowPosition(refreshSet, posi, _CenterWnd->Symbol());
	}
	else
	{
		VtFund* fund = _OrderConfigMgr->Fund();
		if (!fund)
			return;
		int count = 0;
		VtPosition posi = fund->GetPosition(_CenterWnd->Symbol()->ShortCode, count);
		if (count == 0)
			ShowPosition(refreshSet, nullptr, _CenterWnd->Symbol());
		else
			ShowPosition(refreshSet, &posi, _CenterWnd->Symbol());
	}
}

void SmOrderGrid::ClearPositionInfo(std::set<std::pair<int, int>>& refreshSet)
{
	if (_OldPositionCell.IsValid() == TRUE) {
		CGridCellBase* pCell = GetCell(_OldPositionCell.row, _OldPositionCell.col);
		pCell->SetPosition(0);
		refreshSet.insert(std::make_pair(_OldPositionCell.row, _OldPositionCell.col));
		_OldPositionCell.row = -1;
		_OldPositionCell.col = -1;
	}
}

void SmOrderGrid::ShowPosition(std::set<std::pair<int, int>>& refreshSet, VtPosition* posi, VtSymbol* sym)
{
	if (!sym)
		return;

	if (!posi || posi->OpenQty == 0 || posi->Position == VtPositionType::None) {
		return;
	}

	int position_row = FindPositionRow(posi);
	CCellID cell;
	cell.row = position_row;
	cell.col = CenterCol;
	if (cell.IsValid() == TRUE) {
		CGridCellBase* pCell = GetCell(cell.row, cell.col);
		posi->Position == VtPositionType::Buy ? pCell->SetPosition(1) : pCell->SetPosition(2);
		refreshSet.insert(std::make_pair(cell.row, cell.col));
		_OldPositionCell = cell;
	}
}

void SmOrderGrid::OrderBySpaceBar()
{
	if (_MouseIn) {
		if (_OrderConfigMgr->UseSpaceBarOrder) {
			if (_OrderConfigMgr->SpaceBarorderType == VtSpaceByOrderType::ByMouseClick) {
				OrderByMouseClick();
			}
			else if (_OrderConfigMgr->SpaceBarorderType == VtSpaceByOrderType::ByMousePositon) {
				OrderByMousePosition();
			}
		}
		else {
			int newCenter = FindIndexRow();
			//SetCenterRow(newCenter);
			//ChangeCenter(_CloseRow, newCenter);
		}
	}
}

// 종가라인은 변하지 않는다.
// 행 갯수만 늘어난다.
void SmOrderGrid::ResizeGrid()
{
	UnregisterButtons();
	_RowCount = GetMaxValueRowCount() + 1;
	_EndRowForValue = _RowCount - 3;
	SetRowCount(_RowCount);
	for (int i = 1; i < m_nRows; ++i) {
		SetRowHeight(i, _CellHeight);
	}
	SetOrderAreaColor();
	SetCenterValue();
	RegisterButtons();
}

void SmOrderGrid::ResizeGrid(int cellHeight, int orderAreaWidth)
{
	_CellHeight = cellHeight;
	_OrderWidth = orderAreaWidth;
	UnregisterButtons();
	_RowCount = GetMaxValueRowCount() + 1;
	_EndRowForValue = _RowCount - 3;
	SetRowCount(_RowCount);
	for (int i = 1; i < m_nRows; ++i) {
		SetRowHeight(i, _CellHeight);
	}
	_IndexRow = FindIndexRow();
	SetOrderAreaColor();
	SetCenterValue();
	RegisterButtons();
}

void SmOrderGrid::LiqudAll(VtPriceType priceType, int price /*= 0*/)
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr || !_OrderConfigMgr->OrderMgr())
		return;
	VtSymbol* sym = _CenterWnd->Symbol();
	if (_OrderConfigMgr->Type() == 0) { // 계좌 주문일 때
		VtAccount* acnt = _OrderConfigMgr->Account();
		VtPosition* posi = acnt->FindPosition(sym->ShortCode);

		if (!posi || posi->OpenQty == 0)
			return;
		if (priceType == VtPriceType::Market) // 시장가
			PutOrder(posi, 0, true);
		else // 지정가
			PutOrder(posi, price, false);
	}
	else {
		if (_OrderConfigMgr->Fund()) { // 펀드 주문일 때
			VtOrderManagerSelector* orderMgrSelector = VtOrderManagerSelector::GetInstance();
			std::vector<VtAccount*>& fundAcntList = _OrderConfigMgr->Fund()->GetFundAccountVector();
			for (auto it = fundAcntList.begin(); it != fundAcntList.end(); ++it) { // 서브 계좌 목록을 돌면서 각 서브계좌의 잔고를 청산해 준다.
				VtAccount* subAcnt = *it;
				VtPosition* posi = subAcnt->FindPosition(sym->ShortCode);

				if (!posi || posi->OpenQty == 0)
					continue;
				if (priceType == VtPriceType::Market) // 시장가
					PutOrder(posi, 0, true);
				else // 지정가
					PutOrder(posi, price, false);
			}
		}
	}

	// 모든 스탑 주문 취소
	_StopOrderMgr->RemoveAll();
	// 모든 익절,손절 스탑 취소
	_CutMgr->RemoveAllHd();
	// 스탑 주문 모두 갱신
	std::set<std::pair<int, int>> refreshSet;
	ClearOldStopOrders(refreshSet);
	SetStopOrderInfo(refreshSet);
	CalcPosStopOrders(refreshSet);
	RefreshCells(refreshSet);
}

void SmOrderGrid::OrderByMouseClick()
{
	if (_OldClickedCell.IsValid() == FALSE)
		return;
	auto it = RowToValueMap.find(_OldClickedCell.row);
	int price = it->second;

	if (_OldClickedCell.col == CenterCol - 3) {
		PutOrder(price, VtPositionType::Sell, VtPriceType::Price);
	}
	else if (_OldClickedCell.col == CenterCol + 3) {
		PutOrder(price, VtPositionType::Buy, VtPriceType::Price);
	}
	else if (_OldClickedCell.col == CenterCol - 4) {
		AddStopOrder(price, VtPositionType::Sell);
	}
	else if (_OldClickedCell.col == CenterCol + 4) {
		AddStopOrder(price, VtPositionType::Buy);
	}
}

void SmOrderGrid::OrderByMousePosition()
{
	if (_OldMMCell.IsValid() == FALSE)
		return;
	auto it = RowToValueMap.find(_OldMMCell.row);
	int price = it->second;

	if (_OldMMCell.col == CenterCol - 3)
	{
		PutOrder(price, VtPositionType::Sell, VtPriceType::Price);
	}
	else if (_OldMMCell.col == CenterCol + 3)
	{
		PutOrder(price, VtPositionType::Buy, VtPriceType::Price);
	}
	else if (_OldMMCell.col == CenterCol - 4)
	{
		AddStopOrder(price, VtPositionType::Sell);
	}
	else if (_OldMMCell.col == CenterCol + 4)
	{
		AddStopOrder(price, VtPositionType::Buy);
	}
}

void SmOrderGrid::RefreshAllValues()
{
	if (!_CenterWnd || !_CenterWnd->Symbol() )
		return;
	std::set<std::pair<int, int>> refreshSet;
	ClearQuotes(refreshSet);
	SetQuoteColor(_CenterWnd->Symbol(), refreshSet);

	ClearHogas(refreshSet);
	SetHogaInfo(_CenterWnd->Symbol(), refreshSet);

	ClearOldOrders(refreshSet);
	SetOrderInfo(refreshSet);

	ClearOldStopOrders(refreshSet);
	SetStopOrderInfo(refreshSet);
	CalcPosStopOrders(refreshSet);

	RefreshCells(refreshSet);
}

BEGIN_MESSAGE_MAP(SmOrderGrid, CGridCtrl)
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


BOOL SmOrderGrid::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	int distance = zDelta / 60;
	if (abs(zDelta) > 120)
		distance = zDelta / 120;
	else
		distance = zDelta / 40;
	_IndexRow =_IndexRow + (distance);
	InvalidateClickedCell();
	SetCenterValue();
	return CGridCtrl::OnMouseWheel(nFlags, zDelta, pt);
}


void SmOrderGrid::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CCellID cell = GetCellFromPt(point);
	if (!IsValid(cell))
	{
		//ASSERT(FALSE);
		return;
	}

	//CGridCtrl::OnLButtonDblClk(nFlags, point);

	CGridCellBase* pCell = GetCell(cell.row, cell.col);
	if (!pCell)
		return;

	//SendMessageToParent(cell.row, cell.col, NM_DBLCLK);

	if (cell.row < _StartRowForValue || cell.row >= _EndRowForValue)
		return;
	auto it = RowToValueMap.find(cell.row);
	int price = it->second;

	if (cell.col == CenterCol - 3) // For Sell
	{
		PutOrder(price, VtPositionType::Sell, VtPriceType::Price);
	}
	else if (cell.col == CenterCol + 3) // For Buy
	{
		PutOrder(price, VtPositionType::Buy, VtPriceType::Price);
	}
	else if (cell.col == CenterCol + 4)
	{
		AddStopOrder(price, VtPositionType::Buy);
	}
	else if (cell.col == CenterCol - 4)
	{
		AddStopOrder(price, VtPositionType::Sell);
	}

	CWnd::OnLButtonDblClk(nFlags, point);
}


void SmOrderGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (_OrderConfigMgr && _OrderConfigMgr->_HdOrderWnd) {
		_OrderConfigMgr->_HdOrderWnd->SetActiveCenterWnd(_CenterWnd);
	}
	CCellID cell = GetCellFromPt(point);
	CGridCellBase* pCell = GetCell(cell.row, cell.col);
	if (pCell) {
		// 클릭한 셀을 표시하고 저장한다.
		if (cell.row >= _StartRowForValue &&
			cell.row <= _EndRowForValue && (
			cell.col == CenterCol - 4 ||
			cell.col == CenterCol - 3 ||
			cell.col == CenterCol + 4 ||
			cell.col == CenterCol + 3)) {

			pCell->SetClicked(true);
			if (_OldClickedCell.IsValid() == TRUE) {
				CGridCellBase* pOldClickedCell = GetCell(_OldClickedCell.row, _OldClickedCell.col);
				if (pOldClickedCell) {
					pOldClickedCell->SetClicked(false);
					InvalidateCellRect(_OldClickedCell);
				}
			}
			InvalidateCellRect(cell.row, cell.col);
			_OldClickedCell = cell;
		}
	}
	if (IsValid(cell) == TRUE) {
		CGridCellBase* pCell = GetCell(cell.row, cell.col);
		if (pCell->GetOrderCount() > 0 || pCell->GetStopOrderCount() > 0) {
			GetCellRect(cell, _DragStartRect);
			_OrderDragStarted = true;
			OrderCellStart = cell;
		}
	}

	SetCapture();
	int nButtonID = FindButtonID(cell.row, cell.col);
	HandleButtonEvent(nButtonID);
	CGridCtrl::OnLButtonDown(nFlags, point);
}


void SmOrderGrid::OnLButtonUp(UINT nFlags, CPoint point)
{
	CCellID cell = GetCellFromPt(point);
	if (IsValid(cell) == TRUE) {
		if (_OrderDragStarted) {
			Invalidate();
		}
	}
	
	if (_OrderDragStarted) {
		OrderCellEnd = cell;
		if (OrderCellStart.row == cell.row &&
			OrderCellStart.col ==cell.col) {
			OrderCellStart.row = -1;
			OrderCellStart.col = -1;
			OrderCellEnd.row = -1;
			OrderCellEnd.col = -1;
			_OrderDragStarted = false;
			return;
			}

		if (OrderCellEnd.col == CenterCol - 3 && OrderCellStart.col == CenterCol - 3) {
			ChangeOrder();
		}
		else if (OrderCellEnd.col == CenterCol + 3 && OrderCellStart.col == CenterCol + 3) {
			ChangeOrder();
		}
		else if (OrderCellEnd.col == CenterCol - 4 && OrderCellStart.col == CenterCol - 4) {
			ChangeStopOrder();
		}
		else if (OrderCellEnd.col == CenterCol + 4 && OrderCellStart.col == CenterCol + 4) {
			ChangeStopOrder();
		}
		else {
			CancelOrder();
		}

		OrderCellStart.row = -1;
		OrderCellStart.col = -1;
		OrderCellEnd.row = -1;
		OrderCellEnd.col = -1;
		_OrderDragStarted = false;
		Invalidate();
	}
	ReleaseCapture();
	CGridCtrl::OnLButtonUp(nFlags, point);
}


void SmOrderGrid::OnMove(int x, int y)
{
	CGridCtrl::OnMove(x, y);
}


void SmOrderGrid::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	//CGridCtrl::OnRButtonDown(nFlags, point);
}


void SmOrderGrid::OnRButtonUp(UINT nFlags, CPoint point)
{
	CCellID cell = GetCellFromPt(point);
	CGridCellBase* pSrcCell = GetCell(cell.row, cell.col);
	if (pSrcCell->GetOrderCount() > 0) {
		std::map<int, VtOrder*>& orderMap = pSrcCell->GetOrderMap();
		for (auto it = orderMap.begin(); it != orderMap.end(); ++it) {
			VtOrder* order = it->second;
			CancelOrder(order);
		}
		pSrcCell->ClearOrders();
	}

	if (pSrcCell->GetStopOrderCount() > 0) {
		std::map<int, HdOrderRequest*>& stopOrderMap = pSrcCell->GetStopOrderMap();
		for (auto it = stopOrderMap.begin(); it != stopOrderMap.end(); ++it) {
			_StopOrderMgr->RemoveOrderHd(it->first);
			_CutMgr->RemoveOrderHd(it->first);
		}

		pSrcCell->ClearStopOrders();
		std::set<std::pair<int, int>> refreshSet;
		ClearOldStopOrders(refreshSet);
		SetStopOrderInfo(refreshSet);
		CalcPosStopOrders(refreshSet);
		RefreshCells(refreshSet);
	}
	
	//CGridCtrl::OnRButtonUp(nFlags, point);
}

void SmOrderGrid::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_SPACE) {
		AfxMessageBox("space clicked");
	}

	CGridCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void SmOrderGrid::DrawArrow(int direction, POINT *point, CDC* pdc, POINT p0, POINT p1, int head_length, int head_width)
{
	CBrush brush1;   // Must initialize!
	brush1.CreateSolidBrush(RGB(0, 0, 255));   // Blue brush.

	CBrush* pTempBrush = NULL;
	CBrush OrigBrush;

	//CString msg;
	//msg = _T("정정");
	pTempBrush = (CBrush*)pdc->SelectObject(&brush1);
	// Save original brush.
	OrigBrush.FromHandle((HBRUSH)pTempBrush);
	POINT p2;
	float dx = 0.0;
	float dy = 0.0;
	if (direction == 1)
	{
		pdc->MoveTo(p0);
		pdc->LineTo(p1);
		dx = static_cast<float>(p1.x - p0.x);
		dy = static_cast<float>(p1.y - p0.y);
	}
	else
	{
		
		p2.y = p1.y;
		p2.x = p0.x;
		pdc->MoveTo(p0);
		pdc->LineTo(p2);

		//pdc->MoveTo(p2);
		pdc->LineTo(p1);
		//msg = _T("취소");
		dx = static_cast<float>(p1.x - p2.x);
		dy = static_cast<float>(p1.y - p2.y);
	}

	const auto length = std::sqrt(dx*dx + dy*dy);
	if (head_length < 1 || length < head_length) return;

	// ux,uy is a unit vector parallel to the line.
	const auto ux = dx / length;
	const auto uy = dy / length;

	// vx,vy is a unit vector perpendicular to ux,uy
	const auto vx = -uy;
	const auto vy = ux;

	const auto half_width = 0.5f * head_width;

	const POINT arrow[3] =
	{ p1,
		POINT{ Round(p1.x - head_length*ux + half_width*vx),
		Round(p1.y - head_length*uy + half_width*vy) },
		POINT{ Round(p1.x - head_length*ux - half_width*vx),
		Round(p1.y - head_length*uy - half_width*vy) }
	};
	pdc->Polygon(arrow, 3);
	pdc->SetBkMode(TRANSPARENT);
	pdc->SelectObject(&OrigBrush);
}


void SmOrderGrid::RedrawOrderTrackCells()
{
	int min_col = std::min(_DragStartCol, _OldMMCell.col);
	int max_col = std::max(_DragStartCol, _OldMMCell.col);
	int min_row = std::min(_DragStartRow, _OldMMCell.row);
	int max_row = std::max(_DragStartRow, _OldMMCell.row);
	for (int i = min_row; i <= max_row; ++i) {
		for (int j = min_col; j <= max_col; ++j) {
			InvalidateCellRect(i, j);
		}
	}
}

void SmOrderGrid::AddStopOrder(int price, VtPositionType posi)
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr)
		return;

	if (_OrderConfigMgr->Type() == 0) { // 계좌 주문
		if (!_OrderConfigMgr->Account())
			return;
		VtAccount* acnt = _OrderConfigMgr->Account();
		HdOrderRequest* request = new HdOrderRequest();
		if (acnt->AccountLevel() == 0) { // 본계좌인 경우
			request->Type = 0;
			request->AccountNo = acnt->AccountNo;
			request->Password = acnt->Password;
		}
		else { // 서브계좌 인 경우
			request->Type = 1;
			VtAccount* parentAcnt = acnt->ParentAccount();
			if (parentAcnt) {
				request->AccountNo = parentAcnt->AccountNo;
				request->Password = parentAcnt->Password;
			}
		}
		request->Price = price;
		request->Position = posi;
		request->Amount = _CenterWnd->OrderAmount();
		request->SymbolCode = _CenterWnd->Symbol()->ShortCode;
		request->FillCondition = _CenterWnd->FillCondition();
		request->PriceType = _CenterWnd->PriceType();
		request->slip = _CenterWnd->StopVal();
		request->RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
		request->SourceId = (long)this;
		request->SubAccountNo = _T("");
		request->FundName = _T("");
		_StopOrderMgr->AddOrderHd(request);

	}
	else { // 펀드 주문
		if (!_OrderConfigMgr->Fund())
			return;
		VtFund* fund = _OrderConfigMgr->Fund();
		std::vector<VtAccount*>& acntVec = fund->GetFundAccountVector();
		for (auto it = acntVec.begin(); it != acntVec.end(); ++it) {
			VtAccount* acnt = *it;
			// 부모 계좌가 없는 것은 주문을 낼 수 없다.
			if (!acnt->ParentAccount())
				continue;

			HdOrderRequest* request = new HdOrderRequest();
			request->Type = 2;
			request->Price = price;
			request->Position = posi;
			// 펀드는 계좌의 승수를 넣어 준다.
			request->Amount = acnt->SeungSu;
			request->AccountNo = acnt->ParentAccount()->AccountNo;
			request->Password = acnt->ParentAccount()->Password;
			request->SymbolCode = _CenterWnd->Symbol()->ShortCode;
			request->FillCondition = _CenterWnd->FillCondition();
			request->PriceType = _CenterWnd->PriceType();
			request->slip = _CenterWnd->StopVal();
			request->RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
			request->SourceId = (long)this;
			request->SubAccountNo = acnt->AccountNo;
			request->FundName = fund->Name;
			_StopOrderMgr->AddOrderHd(request);
		}
	}

	RefreshStopOrder();
}

void SmOrderGrid::InvalidateClickedCell()
{
	if (_OldClickedCell.IsValid() == TRUE) {
		CGridCellBase* pOldClickedCell = GetCell(_OldClickedCell.row, _OldClickedCell.col);
		if (pOldClickedCell) {
			pOldClickedCell->SetClicked(false);
			InvalidateCellRect(_OldClickedCell);
		}
	}
	_OldClickedCell.row = -1;
	_OldClickedCell.col = -1;
}

void SmOrderGrid::SetMovingCell(CCellID cell)
{
	if (_OldMovingCellCenter.IsValid() == TRUE) {
		CGridCellBase* pCell = GetCell(_OldMovingCellCenter.row, _OldMovingCellCenter.col);
		if (pCell) {
			pCell->SetMoving(false);
			InvalidateCellRect(_OldMovingCellCenter);
		}
	}

	if (_OldMovingCellSide.IsValid() == TRUE) {
		CGridCellBase* pCell = GetCell(_OldMovingCellSide.row, _OldMovingCellSide.col);
		if (pCell) {
			pCell->SetMoving(false);
			InvalidateCellRect(_OldMovingCellSide);
		}
	}

	if (cell.IsValid() == FALSE)
		return;

	switch (cell.col)
	{
	case CenterCol:
		_OldMovingCellCenter = cell;
		break;
	case CenterCol - 4:
		_OldMovingCellCenter.row = cell.row;
		_OldMovingCellCenter.col = CenterCol;
		_OldMovingCellSide.row = cell.row;
		_OldMovingCellSide.col = CenterCol - 4;
		break;
	case CenterCol - 3:
		_OldMovingCellCenter.row = cell.row;
		_OldMovingCellCenter.col = CenterCol;
		_OldMovingCellSide.row = cell.row;
		_OldMovingCellSide.col = CenterCol - 3;
		break;
	case CenterCol + 3:
		_OldMovingCellCenter.row = cell.row;
		_OldMovingCellCenter.col = CenterCol;
		_OldMovingCellSide.row = cell.row;
		_OldMovingCellSide.col = CenterCol + 3;
		break;
	case CenterCol + 4:
		_OldMovingCellCenter.row = cell.row;
		_OldMovingCellCenter.col = CenterCol;
		_OldMovingCellSide.row = cell.row;
		_OldMovingCellSide.col = CenterCol + 4;
			break;
	default:
		_OldMovingCellCenter.row = -1;
		_OldMovingCellCenter.col = -1;
		_OldMovingCellSide.row = -1;
		_OldMovingCellSide.col = -1;
		break;
	}

	if (_OldMovingCellCenter.row < _StartRowForValue ||
		_OldMovingCellCenter.row > _EndRowForValue) {
		_OldMovingCellCenter.row = -1;
		_OldMovingCellCenter.col = -1;
		_OldMovingCellSide.row = -1;
		_OldMovingCellSide.col = -1;
	}

	if (_OldMovingCellCenter.IsValid() == TRUE) {
		CGridCellBase* pCell = GetCell(_OldMovingCellCenter.row, _OldMovingCellCenter.col);
		if (pCell) {
			pCell->SetMoving(true);
			InvalidateCellRect(_OldMovingCellCenter);
		}
	}

	if (_OldMovingCellSide.IsValid() == TRUE) {
		CGridCellBase* pCell = GetCell(_OldMovingCellSide.row, _OldMovingCellSide.col);
		if (pCell) {
			pCell->SetMoving(true);
			InvalidateCellRect(_OldMovingCellSide);
		}
	}
}

void SmOrderGrid::HandleButtonEvent(int button_id)
{
	if (button_id == -1)
		return;
	switch (button_id)
	{
	case 1: // 정렬
		ResetByCenterRow();
		break;
	case 2: // 시장가 매도
		SellAtMarket();
		break;
	case 3: // 시장가 매수
		BuyAtMarket();
		break;
	case 4: // 매도 스탑 취소
		CancelAllSellStop();
		break;
	case 5: // 매도 주문 취소
		CancelAllSellAccepted();
		break;
	case 6: // 전체 주문 취소
		CancelAllAccepted();
		break;
	case 7: // 매수 주문 취소
		CancelAllBuyAccepted();
		break;
	case 8: // 매수 스탑 취소
		CancelAllBuyStop();
		break;
	default:
		break;
	}
}

void SmOrderGrid::ChangeOrder(VtOrder* order, int newPrice)
{
	if (!order)
		return;

	if (!_OrderConfigMgr || !_OrderConfigMgr->OrderMgr())
		return;
	if (_OrderConfigMgr->Type() == 0)
	{
		if (!_OrderConfigMgr->Account())
			return;
		VtAccount* acnt = _OrderConfigMgr->Account();

		HdOrderRequest request;
		request.Price = newPrice;
		request.Position = order->orderPosition;
		request.Amount = order->amount;
		request.RequestType = order->RequestType;

		if (acnt->AccountLevel() == 0)
		{
			request.Type = 0;
			request.AccountNo = acnt->AccountNo;
			request.Password = acnt->Password;
		}
		else
		{
			VtAccount* parentAcnt = acnt->ParentAccount();
			if (parentAcnt)
			{
				request.AccountNo = parentAcnt->AccountNo;
				request.Password = parentAcnt->Password;
			}
			request.Type = 1;
		}
		request.SymbolCode = order->shortCode;
		request.FillCondition = VtFilledCondition::Fas;
		request.PriceType = VtPriceType::Price;
		request.OrderNo = order->orderNo;
		request.RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
		request.SourceId = (long)this;
		if (acnt->AccountLevel() == 0)
		{
			request.SubAccountNo = order->SubAccountNo;
			request.FundName = order->FundName;
		}
		else
			request.SubAccountNo = acnt->AccountNo;

		request.orderType = VtOrderType::Change;

		_OrderConfigMgr->OrderMgr()->ChangeOrder(std::move(request));
	}
	else
	{
		if (_OrderConfigMgr->Fund())
		{
			VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
			VtAccount* subAcnt = subAcntMgr->FindAccount(order->SubAccountNo);
			if (!subAcnt)
				return;
			VtAccount* parentAcnt = subAcnt->ParentAccount();
			if (!parentAcnt)
				return;

			HdOrderRequest request;
			request.Type = 2;
			request.RequestType = order->RequestType;
			request.Price = newPrice;
			request.Position = order->orderPosition;
			request.Amount = order->amount;
			request.AccountNo = parentAcnt->AccountNo;
			request.Password = parentAcnt->Password;
			request.SymbolCode = order->shortCode;
			request.FillCondition = _CenterWnd->FillCondition();
			request.PriceType = _CenterWnd->PriceType();
			request.OrderNo = order->orderNo;
			request.RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
			request.SourceId = (long)this;
			request.SubAccountNo = subAcnt->AccountNo;
			request.FundName = _OrderConfigMgr->Fund()->Name;
			request.orderType = VtOrderType::Change;

			_OrderConfigMgr->OrderMgr()->ChangeOrder(std::move(request));
		}
	}
}

void SmOrderGrid::ChangeOrder()
{
	try
	{
		CGridCellBase* pSrcCell = GetCell(OrderCellStart.row, OrderCellStart.col);
		if (pSrcCell->GetOrderCount() > 0) {
			auto it = RowToValueMap.find(OrderCellEnd.row);
			if (it != RowToValueMap.end()) {
				int price = it->second;
				std::map<int, VtOrder*> orderMap = pSrcCell->GetOrderMap();
				for (auto it = orderMap.begin(); it != orderMap.end(); ++it) {
					VtOrder* order = it->second;
					//if (order->state == VtOrderState::Accepted)
						ChangeOrder(order, price);
				}

				pSrcCell->ClearOrders();
			}
			else {
				int i = 0;
				i = i + 1;
			}
		}
	}
	catch (std::exception& e)
	{
		std::string error = e.what();
	}
}

void SmOrderGrid::ChangeStopOrder()
{
	try
	{
		CGridCellBase* pSrcCell = GetCell(OrderCellStart.row, OrderCellStart.col);
		CGridCellBase* pTgtCell = GetCell(OrderCellEnd.row, OrderCellEnd.col);
		if (pSrcCell->GetStopOrderCount() > 0) {
			auto it = RowToValueMap.find(OrderCellEnd.row);
			if (it != RowToValueMap.end()) {
				int price = it->second;
				std::map<int, HdOrderRequest*> stopOrderMap = pSrcCell->GetStopOrderMap();
				for (auto it = stopOrderMap.begin(); it != stopOrderMap.end(); ++it) {
					HdOrderRequest* order = it->second;
					order->Price = price;
				}
				std::set<std::pair<int, int>> refreshSet;
				ClearOldStopOrders(refreshSet);
				SetStopOrderInfo(refreshSet);
				CalcPosStopOrders(refreshSet);
				RefreshCells(refreshSet);
			}
		}
	}
	catch (std::exception& e)
	{
		std::string error = e.what();
	}
}

void SmOrderGrid::CancelOrder(VtOrder* order)
{
	if (!order || !_OrderConfigMgr || !_OrderConfigMgr->OrderMgr())
		return;
	try
	{

		if (_OrderConfigMgr->Type() == 0)
		{
			if (!_OrderConfigMgr->Account())
				return;
			VtAccount* acnt = _OrderConfigMgr->Account();

			HdOrderRequest request;
			request.Price = order->intOrderPrice;
			request.Position = order->orderPosition;
			request.Amount = order->amount;
			request.RequestType = order->RequestType;

			if (acnt->AccountLevel() == 0)
			{
				request.Type = 0;
				request.AccountNo = acnt->AccountNo;
				request.Password = acnt->Password;
			}
			else
			{
				VtAccount* parentAcnt = acnt->ParentAccount();
				if (parentAcnt)
				{
					request.AccountNo = parentAcnt->AccountNo;
					request.Password = parentAcnt->Password;
				}
				request.Type = 1;
			}
			request.SymbolCode = order->shortCode;
			request.FillCondition = VtFilledCondition::Fas;
			request.PriceType = VtPriceType::Price;
			request.OrderNo = order->orderNo;
			request.RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
			request.SourceId = (long)this;
			if (acnt->AccountLevel() == 0)
			{
				request.SubAccountNo = order->SubAccountNo;
				request.FundName = order->FundName;
			}
			else
				request.SubAccountNo = acnt->AccountNo;

			request.orderType = VtOrderType::Cancel;

			_OrderConfigMgr->OrderMgr()->CancelOrder(std::move(request));
		}
		else
		{
			if (_OrderConfigMgr->Fund())
			{
				VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
				VtAccount* subAcnt = subAcntMgr->FindAccount(order->SubAccountNo);
				if (!subAcnt)
					return;
				VtAccount* parentAcnt = subAcnt->ParentAccount();
				if (!parentAcnt)
					return;

				HdOrderRequest request;
				request.Type = 2;
				request.RequestType = order->RequestType;
				request.Price = order->intOrderPrice;
				request.Position = order->orderPosition;
				request.Amount = order->amount;
				request.AccountNo = parentAcnt->AccountNo;
				request.Password = parentAcnt->Password;
				request.SymbolCode = order->shortCode;
				request.FillCondition = VtFilledCondition::Fas;
				request.PriceType = VtPriceType::Price;
				request.OrderNo = order->orderNo;
				request.RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
				request.SourceId = (long)this;
				request.SubAccountNo = subAcnt->AccountNo;
				request.FundName = _OrderConfigMgr->Fund()->Name;
				request.orderType = VtOrderType::Cancel;

				_OrderConfigMgr->OrderMgr()->CancelOrder(std::move(request));
			}
		}
	}
	catch (std::exception& e) {
		std::string error = e.what();
	}
}

void SmOrderGrid::CancelOrder()
{
	try
	{
		CGridCellBase* pSrcCell = GetCell(OrderCellStart.row, OrderCellStart.col);
		if (pSrcCell->GetOrderCount() > 0) {
			std::map<int, VtOrder*> orderMap = pSrcCell->GetOrderMap();
			for (auto it = orderMap.begin(); it != orderMap.end(); ++it) {
				VtOrder* order = it->second;
				if (order->state == VtOrderState::Accepted)
					CancelOrder(order);
			}
			pSrcCell->ClearOrders();
		}

		if (pSrcCell->GetStopOrderCount() > 0) {
			std::map<int, HdOrderRequest*>& stopOrderMap = pSrcCell->GetStopOrderMap();
			for (auto it = stopOrderMap.begin(); it != stopOrderMap.end(); ++it) {
				_StopOrderMgr->RemoveOrderHd(it->first);
			}
			pSrcCell->ClearStopOrders();
			std::set<std::pair<int, int>> refreshSet;
			ClearOldStopOrders(refreshSet);
			SetStopOrderInfo(refreshSet);
			CalcPosStopOrders(refreshSet);
			RefreshCells(refreshSet);
		}
	}
	catch (std::exception& e)
	{
		std::string error = e.what();
	}
}

void SmOrderGrid::OnMouseMove(UINT nFlags, CPoint point)
{
	_MouseIn = true;

	if (!m_bMouseTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = this->m_hWnd;

		if (::_TrackMouseEvent(&tme)) {
			m_bMouseTracking = TRUE;
		}
	}

	CCellID cell = GetCellFromPt(point);
		
	SetMovingCell(cell);

	_OldMMCell = cell;
	if (_OrderDragStarted && cell.IsValid() == TRUE) {
		OrderCellEnd = cell;
	}

	if (_OrderDragStarted) {
		Invalidate();
	}

	CGridCtrl::OnMouseMove(nFlags, point);
}

LRESULT SmOrderGrid::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	_MouseIn = false;
	m_bMouseTracking = FALSE;
	CCellID cell;
	cell.row = -1;
	cell.col = -1;
	SetMovingCell(cell);
	return 1;
}


int SmOrderGrid::FindPositionRow(VtPosition* posi)
{
	if (!posi || ValueToRowMap.size() == 0)
		return -1;

	VtSymbol* sym = VtSymbolManager::GetInstance()->FindSymbol(posi->ShortCode);
	if (!sym)
		return -1;

	int intAvg = static_cast<int>(ROUNDING(posi->AvgPrice * std::pow(10, sym->Decimal), sym->Decimal));
	intAvg = intAvg - intAvg % sym->intTickSize;

	auto it = ValueToRowMap.find(intAvg);
	if (it != ValueToRowMap.end())
		return it->second;
	else
		return -1;
}

void SmOrderGrid::OnClose()
{
	int i = 0;
	i = i + 1;
	//UnregisterQuoteCallback();

	CGridCtrl::OnClose();
}


void SmOrderGrid::OnDestroy()
{
	UnregisterQuoteCallback();
	CGridCtrl::OnDestroy();

	// TODO: Add your message handler code here
}

int SmOrderGrid::ShowHideOrderGrid(std::vector<bool>& colOptions)
{
	for (int i = 0; i < colOptions.size(); ++i) {
		if (i == 0) {
			if (colOptions[i]) { // for order column
				SetColumnWidth(1, _GridColMap[SmOrderGridCol::ORDER]);
				SetColumnWidth(7, _GridColMap[SmOrderGridCol::ORDER]);
			}
			else {
				SetColumnWidth(1, 0);
				SetColumnWidth(7, 0);
			}
		}
		else if (i == 1) { // for stop column
			if (colOptions[1]) {
				SetColumnWidth(0, _GridColMap[SmOrderGridCol::STOP]);
				SetColumnWidth(8, _GridColMap[SmOrderGridCol::STOP]);
			}
			else {
				SetColumnWidth(0, 0);
				SetColumnWidth(8, 0);
			}
		}
		else if (i == 2) { // for order count column
			if (colOptions[2]) {
				SetColumnWidth(2, _GridColMap[SmOrderGridCol::COUNT]);
				SetColumnWidth(6, _GridColMap[SmOrderGridCol::COUNT]);
			}
			else {
				SetColumnWidth(2, 1);
				SetColumnWidth(6, 1);
			}
		}
	}

	return 0;
}

int SmOrderGrid::GetGridWidth(std::vector<bool>& colOptions)
{
	int totalWidth = _GridColMap[SmOrderGridCol::CENTER] + _GridColMap[SmOrderGridCol::QUANTITY] * 2;
	for (int i = 0; i < colOptions.size(); ++i) {
		if (i == 0) { // for order
			totalWidth += colOptions[i] ? _GridColMap[SmOrderGridCol::ORDER] * 2 : 0;
		}
		else if (i == 1) { // for stop
			totalWidth += colOptions[i] ? _GridColMap[SmOrderGridCol::STOP] * 2 : 0;
		}
		else if (i == 2) { // for count and quantity.
			totalWidth += colOptions[i] ? _GridColMap[SmOrderGridCol::COUNT] * 2 : 0;
		}
	}

	return totalWidth + 5;
}

int SmOrderGrid::GetGridHeight()
{
	int total = _HeadHeight;
	total += _CellHeight * _RowCount;

	return total;
}

void SmOrderGrid::ApplyProfitLossForPosition()
{
	if (!_CutMgr || !_CenterWnd || !_CenterWnd->Symbol())
		return;
	// 모든 익절,손절 스탑 취소
	_CutMgr->RemoveAllHd();
	_CutMgr->MakePositionStopByRemain(_CenterWnd->Symbol());
	std::set<std::pair<int, int>> refreshSet;
	ClearOldOrders(refreshSet);
	SetOrderInfo(refreshSet);
	ClearPositionInfo(refreshSet);
	SetPositionInfo(refreshSet);
	RefreshCells(refreshSet);
}

void SmOrderGrid::ResetByCenterRow()
{
	_IndexRow = FindIndexRow();
	SetCenterValue();
}
