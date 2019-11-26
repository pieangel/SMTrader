#include "stdafx.h"
#include "VtOrderPanelGrid.h"
#include "VtSymbolManager.h"
#include "VtQuoteManager.h"
#include "VtQuote.h"
#include "VtSymbol.h"
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
#include "VtOrderCenterWndHd.h"
#include "VtHdClient.h"
#include "VtOrderWndHd.h"
#include "VtOrderConfigManager.h"
#include "VtFundOrderManager.h"
#include "VtFund.h"
#include "VtSubAccountManager.h"
#include "Format/format.h"
#include "VtCutManager.h"
#include "VtGlobal.h"
#include "XFormatNumber.h"
#include <algorithm> 


using Poco::trim;
using Poco::NumberFormatter;


VtOrderPanelGrid::VtOrderPanelGrid()
{
	_StartRowForValue = 1;
	m_bDrawingOrder = false;
	_InitHoga = false;
	_Close = 0.0;
	_Low = 0.0;
	_High = 0.0;
	_InitHoga = false;
	_InitQuote = false;
	_InitOrder = false;
	_MouseIn = true;
	_StopOrderMgr = new VtStopOrderManager();
	_OrderConfigMgr = nullptr;
	//_Symbol = nullptr;
	//_Account = nullptr;
	_CenterWnd = nullptr;

	_CellHeight = 18;
	_StopWidth = 48;
	_OrderWidth = 60;
	_OrderCountWidth = 45;
	_OrderQuantityWidth = 45;
	_ShowStopCol = true;
	_ShowOrderCol = true;
	_ShowOrderCountCol = true;
	_ShowOrderQuantityCol = true;
	_OldOrderWidth = _OrderWidth;
	_OpenRow = 0;
	_OldRowCount = 0;
	_OrderPositionRow = -2;
	_HeadHeight = 18;
	_ShowingQuoteHoga = false;
	_ShowingCells = false;
	_Test = 0;
	_OrderPos.clear();
	_OldPositionPos.Col = -2;
	_OldPositionPos.Row = -2;
	_GridColMap[VtOrderGridCol::STOP] = 48;
	_GridColMap[VtOrderGridCol::ORDER] = 60;
	_GridColMap[VtOrderGridCol::COUNT] = 45;
	_GridColMap[VtOrderGridCol::QUANTITY] = 45;
	_GridColMap[VtOrderGridCol::CENTER] = 80;
	_CutMgr = new VtCutManager();

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

}


VtOrderPanelGrid::~VtOrderPanelGrid()
{
	if (_StopOrderMgr)
	{
		_StopOrderMgr->RemoveAllHd();
		delete _StopOrderMgr;
		_StopOrderMgr = nullptr;
	}

	if (_CutMgr)
	{
		delete _CutMgr;
		_CutMgr = nullptr;
	}
}

void VtOrderPanelGrid::OnSetup()
{
	m_Pen.CreatePen(PS_SOLID, 1, RGB(190, 190, 190));
	m_ClosePen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	_RowCount = GetMaxRow();
	m_nSeperateIndex =  AddCellType(&m_SeperateText);
	m_OrderCellTypeIndex = AddCellType(&m_OrderCellType);
	m_medBoldFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	//m_SeperateText.SetLabelFont(8, 100, _T("굴림"), RGB(255, 0, 0));
	//Push Button cell type
	m_nButtonIndex = AddCellType(&m_button);
	m_RemainCellIndex = AddCellType(&m_RemainCell);

	SetUGMode(UGMode::OrderNormal);
	//SetVScrollMode(UG_SCROLLJOYSTICK);

	_defFont.CreateFont(12, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_titleFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_CenterFont.CreateFont(11, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	_CursorFont.CreateFont(10, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("굴림"));
	SetDoubleBufferMode(TRUE);
	SetDefColWidth(80);

	SetNumberRows(_RowCount);
	SetNumberCols(_ColCount);
	SetMargin(0);

	
	for (int yIndex = 0; yIndex < _RowCount; yIndex++) {
		if (_OrderConfigMgr)
			SetRowHeight(yIndex, _CellHeight);
		else
			SetRowHeight(yIndex, 18);
		for (int xIndex = 0; xIndex < _ColCount; xIndex++) {
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER );
			QuickSetBorder(xIndex, yIndex, UG_BDR_BTHIN );
			QuickSetBorderColor(xIndex, yIndex, &m_Pen);
		}
	}
	

	SetDefFont(&_defFont);
	SetSH_Width(1);
	SetColTitle(false);
	SetVS_Width(0);
	SetHS_Height(0);

	ShowUpperCtrls();
	EnableMenu(TRUE);

	//SetTimer(RefreshTimer, 1, NULL);
	_LastAddedTime = high_resolution_clock::now();
	_HogaTime = high_resolution_clock::now();
	_QuoteTime = high_resolution_clock::now();
}

void VtOrderPanelGrid::PutOrder(CString price, VtPositionType posi)
{
	if (!_OrderConfigMgr->Account() || !_OrderConfigMgr->Symbol() || price.GetLength() == 0)
		return;

	VtOrderDirectRequest request;
	request.orderPrice = price;
	request.accountNo = _OrderConfigMgr->Account()->AccountNo;
	request.shortCode = _OrderConfigMgr->Symbol()->ShortCode;
	request.orderPosition = posi;
	request.amount = _OrderConfigMgr->OrderAmount;
	request.priceType = VtPriceType::Price;
	request.orderType = VtOrderType::New;
	request.tradeType = VtTradeType::Price;
	request.password = _OrderConfigMgr->Account()->Password;
	request.productDecimal = _OrderConfigMgr->Master()->decimal;
	_OrderConfigMgr->OrderMgr()->DirectPutOrder(std::move(request));
}

void VtOrderPanelGrid::PutOrder(double price, VtPositionType posi)
{
	if (!_OrderConfigMgr->Account() || !_OrderConfigMgr->Symbol())
		return;

	VtOrderRequest request;
	request.orderPrice = price;
	request.accountNo = _OrderConfigMgr->Account()->AccountNo;
	request.shortCode = _OrderConfigMgr->Symbol()->ShortCode;
	request.orderPosition = posi;
	request.amount = _OrderConfigMgr->OrderAmount;
	request.priceType = VtPriceType::Price;
	request.orderType = VtOrderType::New;
	request.tradeType = VtTradeType::Price;
	request.password = _OrderConfigMgr->Account()->Password;
	request.productDecimal = _OrderConfigMgr->Master()->decimal;
	_OrderConfigMgr->OrderMgr()->PutOrder(std::move(request));
}

void VtOrderPanelGrid::PutOrder(int price, VtPositionType position, VtPriceType priceType)
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
			} else { // 서브 계좌 일 때
				VtAccount* parentAcnt = acnt->ParentAccount();
				if (parentAcnt) { // 본계좌의 정보를 넣어 준다.
					request.AccountNo = parentAcnt->AccountNo;
					request.Password = parentAcnt->Password;
				}
			}
			request.SymbolCode = _CenterWnd->Symbol()->ShortCode;
			request.FillCondition = _CenterWnd->FillCondition();
			request.PriceType = priceType;

			request.RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
			request.SourceId = (long)this;
			if (acnt->AccountLevel() == 0) { // 실계좌일 때
				request.SubAccountNo = _T("");
				request.FundName = _T("");
			} else { // 서브 계좌 일 때
				request.SubAccountNo = acnt->AccountNo;
				request.FundName = _T("");
			}

			request.AccountLevel = _OrderConfigMgr->Account()->AccountLevel();
			request.orderType = VtOrderType::New;
			_OrderConfigMgr->OrderMgr()->PutOrder(std::move(request));
		}
	} else { // 펀드 주문 일 때
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
				request.Price = price;
				request.Position = position;
				request.Amount = _CenterWnd->OrderAmount() * subAcnt->SeungSu;
				if (parentAcnt) { // 부모 계좌의 정보를 넣어 준다.
					request.AccountNo = parentAcnt->AccountNo;
					request.Password = parentAcnt->Password;
				} else
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

void VtOrderPanelGrid::PutOrder(VtPosition* posi, int price, bool liqud)
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
	if (acnt->AccountLevel() == 0) { // 본계좌 일 때
		request.AccountNo = acnt->AccountNo;
		request.Password = acnt->Password;
	} else { // 서브 계좌 일 때
		VtAccount* parentAcnt = acnt->ParentAccount();
		if (parentAcnt) { // 부모 계좌가 있는지 확인 한다.
			request.AccountNo = parentAcnt->AccountNo;
			request.Password = parentAcnt->Password;
		} else
			return;
	}
	request.SymbolCode = _CenterWnd->Symbol()->ShortCode;
	request.FillCondition = _CenterWnd->FillCondition();

	if (liqud ) { // 시장가 청산인 경우
		request.Price = 0;
		request.PriceType = VtPriceType::Market;
	} else { // 지정가 청산인 경우
		request.PriceType = VtPriceType::Price;
		request.Price = price;
	}
	
	if (posi->Position == VtPositionType::Buy) { // 매수 청산인 경우
		request.Position = VtPositionType::Sell;
	} else if (posi->Position == VtPositionType::Sell) { // 매도 청산인 경우
		request.Position = VtPositionType::Buy;
	}

	request.RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
	request.SourceId = (long)this;

	if (acnt->AccountLevel() == 0) { // 본계좌 인 경우
		request.SubAccountNo = _T("");
		request.FundName = _T("");
	} else { // 서브 계좌인 경우
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

/*
void VtOrderPanelGrid::CancelOrderHd(VtOrder* order)
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !order || !_OrderConfigMgr || !_OrderConfigMgr->OrderMgr())
		return;
	
	VtSymbol* sym = _CenterWnd->Symbol();
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	VtAccount* acnt = acntMgr->FindAccount(order->accountNo);
	if (!acnt)
		return;

	if (_OrderConfigMgr->Type() == 0)
	{
		HdOrderRequest request;
		request.Amount = std::abs(order->amount);
		if (acnt->AccountLevel() == 0)
		{
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
			else
				return;
		}
		request.SymbolCode = order->shortCode;
		request.FillCondition = _CenterWnd->FillCondition();

		request.PriceType = order->priceType;
		request.orderType = VtOrderType::Cancel;
		request.RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
		request.SourceId = (long)this;
		request.SubAccountNo = _T("");
		request.Position = order->orderPosition;

		_OrderConfigMgr->OrderMgr()->PutOrder(std::move(request));
	}
	else
	{
		HdOrderRequest request;
		request.Amount = std::abs(order->amount);
		request.AccountNo = order->accountNo;
		request.Password = _OrderConfigMgr->Account()->Password;
		request.SymbolCode = order->shortCode;
		request.FillCondition = _CenterWnd->FillCondition();

		request.PriceType = order->priceType;
		request.orderType = VtOrderType::Cancel;
		request.RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
		request.SourceId = (long)this;
		request.Position = order->orderPosition;
		if (acnt->AccountLevel() == 0)
		{
			request.SubAccountNo = _T("");
			request.FundName = _T("");
		}
		else
		{
			request.SubAccountNo = acnt->AccountNo;
			if (acnt->Fund())
				request.FundName = acnt->Fund()->Name;
			else
				request.FundName = _T("");
		}

		_OrderConfigMgr->OrderMgr()->PutOrder(std::move(request));
	}
}
*/

bool VtOrderPanelGrid::CheckProfitLoss(VtSymbol* sym, CheckType type)
{
	if (!sym || !_CutMgr || !_OrderConfigMgr)
		return false;
	if (_OrderConfigMgr->Type() == 0) { // 계좌 주문 일 때
		VtAccount* acnt = _OrderConfigMgr->Account();
		VtPosition* posi = acnt->FindPosition(sym->ShortCode);
		// 포지션이 없거나, 포지션 잔고가 0일 때는 확인하지 않는다.
		if (!posi || posi->OpenQty == 0)
			return false;

		bool result = false;
		if (type == CheckType::Loss) { // 손실 확인
			if (!posi->LossCutSet) // 이미 손절 스탑이 나가있는 경우가 아닐 경우만 확인
				result = VtCutManager::CheckCutLoss(posi, sym, _CutMgr->CutLoss());
			else // 이미 손절 스탑이 나가 있는 경우
				return false;
		} else { // 익절 확인 
			if (!posi->ProfitCutSet) // 이미 익절 스탑이 나가있는 경우가 아닐 때만 확인
				result = VtCutManager::CheckCutProfit(posi, sym, _CutMgr->CutProfit());
			else // 이미 익절 스탑이 나가있는 경우
				return false;
		}
		if (result) { // 조건에 부합할 때 청산 주문을 낸다.
			PutOrder(posi, 0, true);
			posi->OpenQty = 0;
			return true;
		}
	} else { // 편드 주문일 때
		VtFund* fund = _OrderConfigMgr->Fund();
		std::vector<VtPosition*> posVec = fund->GetPositionList(sym->ShortCode);
		for (auto it = posVec.begin(); it != posVec.end(); ++it) { // 현재 펀드에 모든 포지션을 가져온다.
			VtPosition* posi = *it;
			bool result = false;
			if (type == CheckType::Loss) { // 손실 확인
				if (!posi->LossCutSet) // 손실 스탑이 나가 있지 않은 경우만 확인
					result = VtCutManager::CheckCutLoss(posi, sym, _CutMgr->CutLoss());
				else // 이미 손실 스탑이 나가 있는 경우
					return false;
			} else { // 익절 확인
				if (!posi->ProfitCutSet) // 익절 스탑이 나가 있지 않은 경우 확인
					result = VtCutManager::CheckCutProfit(posi, sym, _CutMgr->CutProfit());
				else // 이미 익절 스탑이 나가 있는 경우
					return false;
			}
			if (result) {
				PutOrder(posi, 0, true);
				posi->OpenQty = 0;
				return true;
			}
		}
	}

	return false;
}

/*
void VtOrderPanelGrid::CheckProfitLoss(VtSymbol* sym)
{
	if (!sym || !_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr)
		return;
	VtAccount* acnt = _OrderConfigMgr->Account();
	VtPosition* posi = acnt->FindPosition(sym->ShortCode);
	if (!posi)
		return;

	bool result = acnt->CheckCutLoss(posi, sym, _CenterWnd->CutLoss());
	if (result)
	{
		PutOrder(posi);
		return;
	}

	result = acnt->CheckCutProfit(posi, sym, _CenterWnd->CutProfit());
	if (result)
	{
		PutOrder(posi);
		return;
	}
}
*/
/*
void VtOrderPanelGrid::CheckCutLoss(VtSymbol* sym)
{
	if (!sym || !_CenterWnd || !_OrderConfigMgr)
		return;
	if (_OrderConfigMgr->Type() == 0)
	{
		VtAccount* acnt = _OrderConfigMgr->Account();
		VtPosition* posi = acnt->FindPosition(sym->ShortCode);
		if (!posi)
			return;
		if (posi->OpenQty == 0)
			return;

		bool result = acnt->CheckCutLoss(posi, sym, _CenterWnd->CutLoss());
		if (result)
		{
			PutOrder(posi);
			acnt->RemovePosition(posi->ShortCode);
			return;
		}
	}
	else
	{
		VtFund* fund = _OrderConfigMgr->Fund();
		std::vector<VtAccount*>& acntVec = fund->GetFundAccountVector();
		for (auto it = acntVec.begin(); it != acntVec.end(); ++it)
		{
			VtAccount* subAcnt = *it;
			VtPosition* posi = subAcnt->FindPosition(sym->ShortCode);
			if (!posi)
				return;
			if (posi->OpenQty == 0)
				return;

			bool result = subAcnt->CheckCutLoss(posi, sym, _CenterWnd->CutLoss());
			if (result)
			{
				PutOrder(posi);
				subAcnt->RemovePosition(posi->ShortCode);
				return;
			}
		}
	}
}

void VtOrderPanelGrid::CheckCutProfit(VtSymbol* sym)
{
	if (!sym || !_CenterWnd || !_OrderConfigMgr)
		return;
	if (_OrderConfigMgr->Type() == 0)
	{
		VtAccount* acnt = _OrderConfigMgr->Account();
		VtPosition* posi = acnt->FindPosition(sym->ShortCode);
		if (!posi)
			return;
		if (posi->OpenQty == 0)
			return;

		bool result = acnt->CheckCutProfit(posi, sym, _CenterWnd->CutProfit());
		if (result)
		{
			PutOrder(posi);
			acnt->RemovePosition(posi->ShortCode);
			return;
		}
	}
	else
	{
		VtFund* fund = _OrderConfigMgr->Fund();
		std::vector<VtAccount*>& acntVec = fund->GetFundAccountVector();
		for (auto it = acntVec.begin(); it != acntVec.end(); ++it)
		{
			VtAccount* subAcnt = *it;
			VtPosition* posi = subAcnt->FindPosition(sym->ShortCode);
			if (!posi)
				return;
			if (posi->OpenQty == 0)
				return;

			bool result = subAcnt->CheckCutProfit(posi, sym, _CenterWnd->CutLoss());
			if (result)
			{
				PutOrder(posi);
				subAcnt->RemovePosition(posi->ShortCode);
				return;
			}
		}
	}
}
*/
void VtOrderPanelGrid::ShowOrderAreaInGrid(bool flag)
{
	_ShowOrderCol = flag;
	if (flag)
	{
		SetColWidth(1, _OrderWidth);
		SetColWidth(7, _OrderWidth);
	}
	else
	{
		SetColWidth(1, 0);
		SetColWidth(7, 0);
	}
	RedrawAll();
	int gridWidth = GetMaxWidth();
	int gridHeight = GetMaxHeight();
	SetWindowPos(NULL, 0, 0, gridWidth, gridHeight, SWP_NOMOVE | SWP_NOZORDER);
}

void VtOrderPanelGrid::ShowStopAreaInGrid(bool flag)
{
	_ShowStopCol = flag;
	if (flag) {
		_StopWidth = _GridColMap[VtOrderGridCol::STOP];
		SetColWidth(0, _StopWidth);
		SetColWidth(8, _StopWidth);
		RedrawAll();
	} else {
		_StopWidth = 0;
		SetColWidth(0, _StopWidth);
		SetColWidth(8, _StopWidth);
		RedrawAll();
	}
	int gridWidth = GetMaxWidth();
	int gridHeight = GetMaxHeight();
	SetWindowPos(NULL, 0, 0, gridWidth, gridHeight, SWP_NOMOVE | SWP_NOZORDER);
}

void VtOrderPanelGrid::ShowOrderCountInGrid(bool flag)
{
	_ShowOrderCountCol = flag;
	if (flag)
	{
		_OrderCountWidth = _GridColMap[VtOrderGridCol::COUNT];
		SetColWidth(2, _OrderCountWidth);
		SetColWidth(6, _OrderCountWidth);
		RedrawAll();
	}
	else
	{
		_OrderCountWidth = 0;
		SetColWidth(2, _OrderCountWidth);
		SetColWidth(6, _OrderCountWidth);
		RedrawAll();
	}
	int gridWidth = GetMaxWidth();
	int gridHeight = GetMaxHeight();
	SetWindowPos(NULL, 0, 0, gridWidth, gridHeight, SWP_NOMOVE | SWP_NOZORDER);
}

void VtOrderPanelGrid::SetOrderCellHeight(int value)
{
	_CellHeight = value;
	for (int yIndex = 1; yIndex < _RowCount; yIndex++)
	{
		SetRowHeight(yIndex, _CellHeight);
	}
	int width = GetMaxWidth();
	CRect rcWnd;
	GetWindowRect(rcWnd);
	MoveWindow(rcWnd.left, rcWnd.top, width, rcWnd.Height());
	RedrawAll();
}

void VtOrderPanelGrid::SetOrderCellWidth(int value)
{
	_OrderWidth = value;
	SetColWidth(1, _OrderWidth);
	SetColWidth(7, _OrderWidth);
	int width = GetMaxWidth();
	CRect rcWnd;
	GetWindowRect(rcWnd);
	MoveWindow(rcWnd.left, rcWnd.top, width, rcWnd.Height());
	RedrawAll();
}

void VtOrderPanelGrid::SetShowPLConfigWnd(bool flag)
{

}

void VtOrderPanelGrid::LiqudAll(VtPriceType priceType, int price)
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
	} else {
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
	// 모든 접수된 주문 취소
	//CancelAllAccepted();
	// 화면 갱신
	CellPosMap refreshMap;
	ClearOldStopOrders(refreshMap);
	RefreshCells(refreshMap);
}

VtOrderRequest* VtOrderPanelGrid::AddNormalStopOrder(double price, VtPositionType posi)
{
	if (!_StopOrderMgr)
		return nullptr;
	/*
	VtOrderRequest* request = new VtOrderRequest();
	
	request->accountNo = _OrderConfigMgr->Account()->AccountNo;
	request->shortCode = _OrderConfigMgr->Symbol()->ShortCode;
	request->orderPosition = posi;
	request->amount = _OrderConfigMgr->StopVal;
	request->priceType = VtPriceType::Price;
	request->orderType = VtOrderType::New;
	request->tradeType = VtTradeType::Price;
	request->password = _OrderConfigMgr->Account()->Password;
	request->productDecimal = _OrderConfigMgr->Master()->decimal;
	request->slip = _OrderConfigMgr->StopVal;
	request->requestID = _StopOrderMgr->GetOrderID();
	request->orderPrice = price;
	_StopOrderMgr->AddOrder(request);

	return request;
	*/


	return nullptr;
}

VtOrderRequest* VtOrderPanelGrid::AddAutoStopOrder(double price, VtPositionType posi)
{
	if (!_StopOrderMgr)
		return nullptr;
	/*
	VtOrderRequest* request = new VtOrderRequest();
	request->orderPrice = price;
	request->accountNo = _OrderConfigMgr->Account()->AccountNo;
	request->shortCode = _OrderConfigMgr->Symbol()->ShortCode;
	request->orderPosition = posi;
	request->orderType = VtOrderType::New;
	request->password = _OrderConfigMgr->Account()->Password;
	request->productDecimal = _OrderConfigMgr->Master()->decimal;
	request->requestID = _StopOrderMgr->GetOrderID();
	request->slip = _CenterWnd->StopVal();
	return request;
	*/
	return nullptr;
}

void VtOrderPanelGrid::AddNormalStopOrderHd(int price, VtPositionType posi)
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr)
		return ;

	if (_OrderConfigMgr->Type() == 0) { // 계좌 주문
		if (!_OrderConfigMgr->Account())
			return;
		VtAccount* acnt = _OrderConfigMgr->Account();
		HdOrderRequest* request = new HdOrderRequest();
		if (acnt->AccountLevel() == 0) { // 본계좌인 경우
			request->Type = 0;
			request->AccountNo = acnt->AccountNo;
			request->Password = acnt->Password;
		} else { // 서브계좌 인 경우
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

	} else { // 펀드 주문
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
}

// 체결 익절이 동시에 나가 있을 경우 - 한쪽이 체결되면 한쪽은 지워준다.
void VtOrderPanelGrid::AddStopOrderForFilled(VtOrder* order, int mode)
{
	if (!_CenterWnd || !_CutMgr)
		return;

	_CutMgr->AddStopOrderForFilled(_CenterWnd->Symbol(), order);
}

void VtOrderPanelGrid::ChangeOrder(VtOrder* order, CString newValue)
{
	if (!order)
		return;
}

void VtOrderPanelGrid::ChangeOrder(VtOrder* order, int newPrice)
{
	if (!order)
		return;

	if (!_CenterWnd || !_OrderConfigMgr || !_OrderConfigMgr->OrderMgr())
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
		}
		request.SymbolCode = order->shortCode;
		request.FillCondition = _CenterWnd->FillCondition();
		request.PriceType = _CenterWnd->PriceType();
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

void VtOrderPanelGrid::CancelOrder(VtOrder* order)
{
	if (!order || !_OrderConfigMgr || !_OrderConfigMgr->OrderMgr())
		return;

	if (!_CenterWnd)
		return;

	if (_OrderConfigMgr->Type() == 0)
	{
		if (!_OrderConfigMgr->Account())
			return;

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
		}
		request.SymbolCode = order->shortCode;
		request.FillCondition = _CenterWnd->FillCondition();
		request.PriceType = _CenterWnd->PriceType();
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
			request.RequestType = order->RequestType;
			request.Price = order->intOrderPrice;
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
			request.orderType = VtOrderType::Cancel;

			_OrderConfigMgr->OrderMgr()->CancelOrder(std::move(request));
		}
	}
}

void VtOrderPanelGrid::OnOrderReceived(VtOrder* order)
{
	if (!order)
		return;


}

void VtOrderPanelGrid::OnAccepted(VtOrder* order)
{
	if (!order)
		return;

	ClearOldOrders();

	SetOrderInfo();

	ShowOrderInfo();

	ShowTotalOrder();

}

void VtOrderPanelGrid::OnFilled(VtOrder* order)
{
	if (_ShowingCells)
		return;
	_ShowingCells = true;


	SetAutoStopOnFilled(order);

	ClearOldOrders();

	SetOrderInfo();

	ShowOrderInfo();

	ShowTotalOrder();

	_ShowingCells = false;
}

void VtOrderPanelGrid::OnConfirmModify(VtOrder* order)
{
	ClearOldOrders();

	SetOrderInfo();

	ShowOrderInfo();

	ShowTotalOrder();
}

void VtOrderPanelGrid::OnConfirmCancel(VtOrder* order)
{
	ClearOldOrders();

	SetOrderInfo();

	ShowOrderInfo();

	ShowTotalOrder();
}


void VtOrderPanelGrid::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{
	if (row < _StartRowForValue || row >= _EndRowForValue)
		return;

	if (col == CenterCol - 3) // For Sell
	{
		CUGCell cell;
		GetCell(CenterCol, row, &cell);
		long price = cell.LongValue();
		PutOrder(price, VtPositionType::Sell, VtPriceType::Price);
	}
	else if (col == CenterCol + 3) // For Buy
	{
		CUGCell cell;
		GetCell(CenterCol, row, &cell);
		long price = cell.LongValue();
		PutOrder(price, VtPositionType::Buy, VtPriceType::Price);
	}
	else if (col == CenterCol + 4)
	{
		CUGCell cell;
		GetCell(CenterCol, row, &cell);
		int val = cell.LongValue();
		AddNormalStopOrderHd(val, VtPositionType::Buy);
		RefreshStopOrder();
	}
	else if (col == CenterCol - 4)
	{
		CUGCell cell;
		GetCell(CenterCol, row, &cell);
		int val = cell.LongValue();
		AddNormalStopOrderHd(val, VtPositionType::Sell);
		RefreshStopOrder();
	}
}


void VtOrderPanelGrid::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (_OrderConfigMgr && _OrderConfigMgr->_HdOrderWnd) {
		; // _OrderConfigMgr->_HdOrderWnd->SetActiveCenterWnd(_CenterWnd);
	}
	SetFocus();

	CUGCell srcCell;
	CUGCell targetCell;
	CUGCell centerCell;
	CString amount;
	CString price;
	int intPrice;

	bool enterOrderArea = false;
	if (updn == TRUE) {
		if (row >= _StartRowForValue) {
			if (col == CenterCol - 3) {
				GetCell(CenterCol - 3, row, &srcCell);
				enterOrderArea = true;
				SetSelectedRect(col, row);
			} else if (col == CenterCol - 4) {
				GetCell(CenterCol - 4, row, &srcCell);
				enterOrderArea = true;
				SetSelectedRect(col, row);
			} else if (col == CenterCol + 3) {
				GetCell(CenterCol + 3, row, &srcCell);
				enterOrderArea = true;
				SetSelectedRect(col, row);
			} else if (col == CenterCol + 4) {
				GetCell(CenterCol + 4, row, &srcCell);
				enterOrderArea = true;
				SetSelectedRect(col, row);
			}

			if (enterOrderArea) {
				if (srcCell.OrderMap.size() > 0) {
					m_Prev.x = point->x;
					m_Prev.y = point->y;
					m_bDrawingOrder = true;
					m_DragStartCol = col;
					m_DragStartRow = row;
				} else if (srcCell.StopOrderListHd.size() > 0) {
					m_Prev.x = point->x;
					m_Prev.y = point->y;
					m_bDrawingOrder = true;
					m_DragStartCol = col;
					m_DragStartRow = row;
				}
			}

			enterOrderArea = false;
		}
	} else {
		if (m_bDrawingOrder) {
			m_bDrawingOrder = false;

			// We don't accept the same position as before.
			if (m_DragStartCol == col &&
				m_DragStartRow == row)
				return;

			if (col == CenterCol - 3 && m_DragStartCol == CenterCol - 3) {
				GetCell(CenterCol - 3, row, &targetCell);
				GetCell(m_DragStartCol, m_DragStartRow, &srcCell);
				if (srcCell.OrderMap.size() > 0) {
					GetCell(CenterCol, row, &centerCell);
					intPrice = centerCell.LongValue();
					for (auto it = srcCell.OrderMap.begin(); it != srcCell.OrderMap.end(); ++it) {
						VtOrder* order = it->second;
						ChangeOrder(order, intPrice);
					}

					RemoveOrderFromCell(m_DragStartCol, m_DragStartRow);
				}
			} else if (col == CenterCol + 3 && m_DragStartCol == CenterCol + 3) {
				GetCell(CenterCol + 3, row, &targetCell);
				GetCell(m_DragStartCol, m_DragStartRow, &srcCell);
				if (srcCell.OrderMap.size() > 0) {
					GetCell(CenterCol, row, &centerCell);
					intPrice = centerCell.LongValue();

					for (auto it = srcCell.OrderMap.begin(); it != srcCell.OrderMap.end(); ++it) {
						VtOrder* order = it->second;
						ChangeOrder(order, intPrice);
					}

					RemoveOrderFromCell(m_DragStartCol, m_DragStartRow);
				}
			} else if (col == CenterCol - 4 && m_DragStartCol == CenterCol - 4) {
				GetCell(CenterCol - 4, row, &targetCell);
				GetCell(m_DragStartCol, m_DragStartRow, &srcCell);
				if (srcCell.StopOrderListHd.size() > 0) {
					GetCell(CenterCol, row, &centerCell);
					intPrice = centerCell.LongValue();
					for (int i = 0; i < srcCell.StopOrderListHd.size(); i++) {
						HdOrderRequest* order = srcCell.StopOrderListHd[i];
						order->Price = intPrice;
					}
					RefreshStopOrder();
				}
			} else if (col == CenterCol + 4 && m_DragStartCol == CenterCol + 4) {
				GetCell(CenterCol + 4, row, &targetCell);
				GetCell(m_DragStartCol, m_DragStartRow, &srcCell);
				if (srcCell.StopOrderListHd.size() > 0)
				{
					GetCell(CenterCol, row, &centerCell);
					intPrice = centerCell.LongValue();
					for (int i = 0; i < srcCell.StopOrderListHd.size(); i++)
					{
						HdOrderRequest* order = srcCell.StopOrderListHd[i];
						order->Price = intPrice;
					}
					
					RefreshStopOrder();
				}
			} else {
				GetCell(m_DragStartCol, m_DragStartRow, &srcCell);
				if (srcCell.OrderMap.size() > 0) {
					for (auto it = srcCell.OrderMap.begin(); it != srcCell.OrderMap.end(); ++it) {
						VtOrder* order = it->second;
						CancelOrder(order);
						RemoveOrderFromCell(m_DragStartCol, m_DragStartRow);
					}

				} else if (srcCell.StopOrderListHd.size() > 0) {
					for (auto it = srcCell.StopOrderListHd.begin(); it != srcCell.StopOrderListHd.end(); ++it) {
						HdOrderRequest* order = *it;
						_StopOrderMgr->RemoveOrderHd(order->RequestId);
					}

					RefreshStopOrder();
				}
			}

			if (m_DragStartRow < row) {
				for (int i = m_DragStartRow; i <= row; i++) {
					QuickRedrawCell(m_DragStartCol, i);
				}
			} else {
				for (int i = row; i <= m_DragStartRow; i++) {
					QuickRedrawCell(m_DragStartCol, i);
				}
			}
		} else {
			if (_OrderConfigMgr && _OrderConfigMgr->UseOneClickOrder) {
				if (row < _StartRowForValue || row >= _EndRowForValue)
					return;

				if (col == CenterCol - 3) {
					CUGCell cell;
					GetCell(CenterCol, row, &cell);
					long price = cell.LongValue();
					PutOrder(price, VtPositionType::Sell, VtPriceType::Price);
				} else if (col == CenterCol + 3) {
					CUGCell cell;
					GetCell(CenterCol, row, &cell);
					long price = cell.LongValue();
					PutOrder(price, VtPositionType::Buy, VtPriceType::Price);
				} else if (col == CenterCol + 4) {
					CUGCell cell;
					GetCell(CenterCol, row, &cell);
					int val = cell.LongValue();
					AddNormalStopOrderHd(val, VtPositionType::Buy);
					RefreshStopOrder();
				} else if (col == CenterCol - 4) {
					CUGCell cell;
					GetCell(CenterCol, row, &cell);
					int val = cell.LongValue();
					AddNormalStopOrderHd(val, VtPositionType::Sell);
					RefreshStopOrder();
				}
			}
		}
	}
}

void VtOrderPanelGrid::OnRClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	//ShowOrderMenu(point);
	if (updn == TRUE) {
		if (_OrderConfigMgr) {
			if (_OrderConfigMgr->RButtonCancel == VtRButtonCancelOrder::CancelOnMousePosition) {
				if (col == CenterCol - 3 || col == CenterCol + 3) {
					CUGCell srcCell;
					GetCell(col, row, &srcCell);
					if (srcCell.OrderMap.size() > 0) {
						for (auto it = srcCell.OrderMap.begin(); it != srcCell.OrderMap.end(); ++it) {
							VtOrder* order = it->second;
							CancelOrder(order);
						}
					}
				}
			} else {
				std::vector<VtOrder*> acptOrderList = _OrderConfigMgr->OrderMgr()->GetAcceptedOrders(_CenterWnd->Symbol()->ShortCode);
				auto it = acptOrderList.rbegin();
				CancelOrder(*it);
			}
		}

		if (col == CenterCol - 4 || col == CenterCol + 4) {
			CUGCell srcCell;
			GetCell(col, row, &srcCell);
			if (srcCell.StopOrderListHd.size() > 0) {
				for (auto it = srcCell.StopOrderListHd.begin(); it != srcCell.StopOrderListHd.end(); ++it) {
					HdOrderRequest* order = *it;
					if (order->RequestType == 0)
						_StopOrderMgr->RemoveOrderHd(order->RequestId);
					else {
						_CutMgr->RemoveOrderHd(order->RequestId, true);
					}
				}

				RefreshStopOrder();
			}
		}
	}
}

int VtOrderPanelGrid::OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow)
{
	return 0;
}

/*
void VtOrderPanelGrid::OnDrawFocusRect(CDC *dc, RECT *rect)
{
	if (_MMRow >= _EndRowForValue || _MMRow < _StartRowForValue)
	{
		_SelectedRow = -1;
		_SelectedCol = -1;
		return;
	}
	if (!(_MMCol == CenterCol - 3 || _MMCol == CenterCol + 3 || _MMCol == CenterCol - 4 || _MMCol == CenterCol + 4))
	{
		_SelectedRow = -1;
		_SelectedCol = -1;
		return;
	}
	CPen pen;
	CPen* oldPen = nullptr;
	pen.CreatePen(PS_SOLID, 1, RGB(0, 102, 51));
	dc->SelectObject(pen);
	
	RECT rectCenter = *rect;

	rectCenter.top++;
	rectCenter.left++;
	rectCenter.bottom -= 2;
	rectCenter.right -= 2;

	dc->MoveTo(rectCenter.left, rectCenter.top);
	dc->LineTo(rectCenter.right, rectCenter.top);
	dc->LineTo(rectCenter.right, rectCenter.bottom);
	dc->LineTo(rectCenter.left, rectCenter.bottom);
	dc->LineTo(rectCenter.left, rectCenter.top);

	pen.DeleteObject();

	_SelectedRow = _MMRow;
	_SelectedCol = _MMCol;
}

*/
void VtOrderPanelGrid::OnKeyDown(UINT *vcKey, BOOL processed)
{
	/*
	for (int i = 0; i < 10000; ++i) {
		if (_CenterWnd)
			OnReceiveHoga(_CenterWnd->Symbol());
	}
	*/
	if (*vcKey == VK_UP)
		MoveCurrentRow(UG_PAGEUP);

	else if (*vcKey == VK_DOWN)
		MoveCurrentRow(UG_PAGEDOWN);

	else if (*vcKey == VK_LEFT)
		MoveCurrentCol(UG_PAGEUP);

	else if (*vcKey == VK_RIGHT)
		MoveCurrentCol(UG_PAGEDOWN);
 	else if (*vcKey == VK_SPACE) {
		if (_MouseIn) {
			if (_OrderConfigMgr->UseSpaceBarOrder) {
				if (_OrderConfigMgr->SpaceBarorderType == VtSpaceByOrderType::ByMouseClick) {
					OrderByMouseClick();
				} else if (_OrderConfigMgr->SpaceBarorderType == VtSpaceByOrderType::ByMousePositon) {
					OrderByMousePosition();
				}
			} else {
				int newCenter = FindCenterRow();
				SetCenterRow(newCenter);
				ChangeCenter(_CloseRow, newCenter);
			}
		}
	}
}

void VtOrderPanelGrid::OnGetCell(int col, long row, CUGCell *cell)
{

}

/*
void VtOrderPanelGrid::OnDrawSelectedRect(CDC *dc, RECT *rect)
{
	CPen pen;
	CPen* oldPen = nullptr;
	pen.CreatePen(PS_SOLID, 1, RGB(0, 102, 51));
	dc->SelectObject(pen);

	RECT rectCenter;

	GetCellRect(_SelectedCol, _SelectedRow, &rectCenter);

	rectCenter.top++;
	rectCenter.left++;
	rectCenter.bottom -= 2;
	rectCenter.right -= 2;

	dc->MoveTo(rectCenter.left, rectCenter.top);
	dc->LineTo(rectCenter.right, rectCenter.top);
	dc->LineTo(rectCenter.right, rectCenter.bottom);
	dc->LineTo(rectCenter.left, rectCenter.bottom);
	dc->LineTo(rectCenter.left, rectCenter.top);

	pen.DeleteObject();

}
*/
void VtOrderPanelGrid::SetColTitle(bool init)
{
	const int ColCount = _ColCount;
	CUGCell cell;
	LPCTSTR title[9] = { "STOP", "주문", "건수", "잔량", "정렬", "잔량", "건수", "주문", "STOP" };
	int colWidth[9] = { 
		_GridColMap[VtOrderGridCol::STOP], 
		_GridColMap[VtOrderGridCol::ORDER], 
		_GridColMap[VtOrderGridCol::COUNT],
		_GridColMap[VtOrderGridCol::QUANTITY], 
		_GridColMap[VtOrderGridCol::CENTER], 
		_GridColMap[VtOrderGridCol::QUANTITY],
		_GridColMap[VtOrderGridCol::COUNT],
		_GridColMap[VtOrderGridCol::ORDER],
		_GridColMap[VtOrderGridCol::STOP]
	};

	SetRowHeight(-1, _HeadHeight);

	for (int i = 0; i < _ColCount; i++)
	{
		if (i == CenterCol)
			continue;
		if (!init)
			SetColWidth(i, colWidth[i]);
		QuickSetText(i, -1, title[i]);
		GetCell(i, -1, &cell);
		cell.SetBackColor(VtGlobal::GridTitleBackColor);
		cell.SetTextColor(VtGlobal::GridTitleTextColor);
		SetCell(i, -1, &cell);
		QuickSetAlignment(i, -1, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		QuickSetFont(i, -1, &_defFont);
		QuickRedrawCell(i, -1);
	}
}

void VtOrderPanelGrid::QuickRedrawCell(int col, long row)
{
	CRect rect;
	GetCellRect(col, row, rect);
	m_CUGGrid->m_drawHint.AddHint(col, row, col, row);

	if (GetCurrentRow() != row || GetCurrentCol() != col)
		TempDisableFocusRect();

	m_CUGGrid->PaintDrawHintsNow(rect);
}

void VtOrderPanelGrid::ClearSelectedCell()
{
	//RedrawCell(_SelectedCol, _SelectedRow);
	//_SelectedRow = -1;
	//_SelectedCol = -1;

}

void VtOrderPanelGrid::SetSelectedRect(int col, int row)
{
	
	if (col == _OldSelectedCol && row == _OldSelectedRow)
		return;
	SetSelect(col, row);
	ShowSelect(col, row);
}

void VtOrderPanelGrid::OnGridWheel(int oldCenter, int newCenter)
{
	if (oldCenter == newCenter)
		return;
	ChangeCenter(oldCenter, newCenter);
	
	DrawMovingRect(_MMCol, _MMRow);

}

void VtOrderPanelGrid::SetOrderInfo()
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr || !_OrderConfigMgr->OrderMgr())
		return;

	VtSymbol* sym = _CenterWnd->Symbol();

	_OrderPos.clear();

	std::vector<VtOrder*> acptOrderList = _OrderConfigMgr->OrderMgr()->GetAcceptedOrders(sym->ShortCode);

	for (auto it = acptOrderList.begin(); it != acptOrderList.end(); ++it)
	{
		VtOrder* order = *it;
		if (order->amount == 0)
			continue;

		int row = FindRowFromCenterValue(order->intOrderPrice);

		VtCellPos pos;
		CUGCell cell;
		if (row >= _StartRowForValue && row <= _EndRowForValue)
		{
			if (order->orderPosition == VtPositionType::Sell)
			{
				pos.Col = CenterCol - 3;
				GetCell(CenterCol - 3, row, &cell);
				cell.OrderType(2);
				cell.OrderMap[order->orderNo] = order;
				//cell.OrderCount(cell.OrderMap.size());
				cell.SetCellType(m_OrderCellTypeIndex);
				SetCell(CenterCol - 3, row, &cell);
			}
			else
			{
				pos.Col = CenterCol + 3;
				GetCell(CenterCol + 3, row, &cell);
				cell.OrderType(1);
				cell.OrderMap[order->orderNo] = order;
				//cell.OrderCount(cell.OrderMap.size());
				cell.SetCellType(m_OrderCellTypeIndex);
				SetCell(CenterCol + 3, row, &cell);
			}
			pos.Row = row;
			int cellOrderCnt = 0;
			for (auto it = cell.OrderMap.begin(); it != cell.OrderMap.end(); ++it)
				cellOrderCnt += it->second->amount;

			QuickSetNumber(pos.Col, pos.Row, cellOrderCnt);
			_OrderPos.insert(pos);
			//_CurPos.insert(pos);
		}
	}
}

void VtOrderPanelGrid::SetOrderInfo(CellPosMap& refreshMap)
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr || !_OrderConfigMgr->OrderMgr())
		return;

	VtSymbol* sym = _CenterWnd->Symbol();

	_OrderPos.clear();

	std::vector<VtOrder*> acptOrderList = _OrderConfigMgr->OrderMgr()->GetAcceptedOrders(sym->ShortCode);

	for (auto it = acptOrderList.begin(); it != acptOrderList.end(); ++it) {
		VtOrder* order = *it;
		if (order->amount == 0)
			continue;

		int row = FindRowFromCenterValue(sym, order->intOrderPrice);

		VtCellPos pos;
		CUGCell cell;
		if (row >= _StartRowForValue && row <= _EndRowForValue) {
			if (order->orderPosition == VtPositionType::Sell) {
				pos.Col = CenterCol - 3;
				GetCell(CenterCol - 3, row, &cell);
				cell.OrderType(2);
				cell.OrderMap[order->orderNo] = order;
				cell.SetCellType(m_OrderCellTypeIndex);
				SetCell(CenterCol - 3, row, &cell);
			} else if (order->orderPosition == VtPositionType::Buy) {
				pos.Col = CenterCol + 3;
				GetCell(CenterCol + 3, row, &cell);
				cell.OrderType(1);
				cell.OrderMap[order->orderNo] = order;
				cell.SetCellType(m_OrderCellTypeIndex);
				SetCell(CenterCol + 3, row, &cell);
			}
			pos.Row = row;
			int cellOrderCnt = 0;
			for (auto it = cell.OrderMap.begin(); it != cell.OrderMap.end(); ++it)
				cellOrderCnt += it->second->amount;

			QuickSetNumber(pos.Col, pos.Row, cellOrderCnt);
			_OrderPos.insert(pos);
			refreshMap[pos] = pos;
		}
	}
}

void VtOrderPanelGrid::ShowOrderInfo()
{
// 	OrderPosUnion.clear();
// 	std::set_union(_OrderPos.begin(), _OrderPos.end(), _OldOrderPos.begin(), _OldOrderPos.end(), std::inserter(OrderPosUnion, OrderPosUnion.begin()));
// 	for (auto it = OrderPosUnion.begin(); it != OrderPosUnion.end(); ++it)
// 	{
// 		VtCellPos pos = *it;
// 		QuickRedrawCell(pos.Col, pos.Row);
// 	}
// 
// 	_OldOrderPos.clear();
// 	for (auto it = _OrderPos.begin(); it != _OrderPos.end(); ++it)
// 	{
// 		VtCellPos pos = *it;
// 		_OldOrderPos.insert(pos);
// 	}
}

void VtOrderPanelGrid::ShowOrderInfo(CellPosMap& refreshMap)
{
// 	RefreshCells(refreshMap);
// 
// 	_OldOrderPos.clear();
// 	for (auto it = _OrderPos.begin(); it != _OrderPos.end(); ++it)
// 	{
// 		_OldOrderPos.insert(*it);
// 	}
}

void VtOrderPanelGrid::SetStopOrderInfo()
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;

	VtSymbol* sym = _CenterWnd->Symbol();

	_StopOrderPos.clear();
	int buy = 0, sell = 0;
	
	for (auto it = _StopOrderMgr->StopOrderMapHd.begin(); it != _StopOrderMgr->StopOrderMapHd.end(); ++it)
	{
		HdOrderRequest* order = it->second;
		if (order->Position == VtPositionType::Buy)
			buy++;
		else if (order->Position == VtPositionType::Sell)
			sell++;

		//int diff = VtSymbolManager::GetTickDiff(firstValue, order->Price, sym);
		int row = FindRowFromCenterValue(order->Price);  //_StartRowForValue + diff;

		if (row < _StartRowForValue)
			continue;
		if (row > _EndRowForValue)
			continue;

		VtCellPos pos;
		CUGCell cell;
		VtArrowPos arrowPos;
		RECT rcCell1, rcCell2;
		CString text;
		VtCellPos pos1;
		
		if (order->Position == VtPositionType::Sell)
		{
			pos.Col = CenterCol - 4;
			GetCell(CenterCol - 4, row, &cell);
			cell.OrderType(4);
			cell.OrderSlip(order->slip);
			cell.StopOrderListHd.push_back(order);
			cell.SetCellType(m_OrderCellTypeIndex);
			SetCell(CenterCol - 4, row, &cell);
			GetCellRect(CenterCol - 4, row, &rcCell1);
			GetCellRect(CenterCol - 3, row, &rcCell2);

			arrowPos.sx = rcCell1.left + (rcCell1.right - rcCell1.left) / 2;
			arrowPos.sy = rcCell1.top + (rcCell1.bottom - rcCell1.top) / 2;
			arrowPos.ex = rcCell2.left + (rcCell2.right - rcCell2.left) / 2;
			arrowPos.ey = arrowPos.sy + (rcCell2.bottom - rcCell2.top) * cell.OrderSlip();
			arrowPos.type = 2;
			arrowPos.col = CenterCol - 4;
			arrowPos.row = row;
			arrowPos.slip = cell.OrderSlip();
			arrowPos.startRow = _StartRowForValue;
			arrowPos.endRow = _EndRowForValue - 1;
			AddArrowPos(std::move(arrowPos));

			if (cell.OrderSlip() > 0)
			{
				//text.Format(_T("%d(+%d)"), cell.StopOrderList.size(), cell.OrderSlip());
				for (int i = row; i <= row + cell.OrderSlip(); i++)
				{
					if (i >= _StartRowForValue && i <= _EndRowForValue)
					{
						pos1.Row = i;
						pos1.Col = CenterCol - 3;
						_StopOrderPos.insert(pos1);
						//_CurPos.insert(pos1);

						pos1.Row = i;
						pos1.Col = CenterCol - 4;
						_StopOrderPos.insert(pos1);
						//_CurPos.insert(pos1);
					}
				}
			}
			else if (cell.OrderSlip() < 0)
			{
				//text.Format(_T("%d(-%d)"), cell.StopOrderList.size(), cell.OrderSlip());
				for (int i = row; i >= row - cell.OrderSlip(); i--)
				{
					if (i >= _StartRowForValue && i <= _EndRowForValue)
					{

						pos1.Row = i;
						pos1.Col = CenterCol - 3;
						_StopOrderPos.insert(pos1);
						//_CurPos.insert(pos1);

						pos1.Row = i;
						pos1.Col = CenterCol - 4;
						_StopOrderPos.insert(pos1);
						//_CurPos.insert(pos1);
					}
				}
			}
			else
			{
				//text.Format(_T("%d(%d)"), cell.StopOrderList.size(), cell.OrderSlip());
				if (row >= _StartRowForValue && row <= _EndRowForValue)
				{
					pos1.Row = row;
					pos1.Col = CenterCol - 3;
					_StopOrderPos.insert(pos1);
					//_CurPos.insert(pos1);

					pos1.Row = row;
					pos1.Col = CenterCol - 4;
					_StopOrderPos.insert(pos1);
					//_CurPos.insert(pos1);
				}
			}
			text.Format(_T("%d(%d)"), cell.StopOrderListHd.size(), cell.OrderSlip());
		}
		else
		{
			pos.Col = CenterCol + 4;
			GetCell(CenterCol + 4, row, &cell);
			cell.OrderType(3);
			cell.OrderSlip(order->slip);
			cell.StopOrderListHd.push_back(order);
			cell.SetCellType(m_OrderCellTypeIndex);
			SetCell(CenterCol + 4, row, &cell);

			GetCellRect(CenterCol + 4, row, &rcCell1);
			GetCellRect(CenterCol + 3, row, &rcCell2);

			arrowPos.sx = rcCell1.left + (rcCell1.right - rcCell1.left) / 2;
			arrowPos.sy = rcCell1.top + (rcCell1.bottom - rcCell1.top) / 2;
			arrowPos.ex = rcCell2.left + (rcCell2.right - rcCell2.left) / 2;
			arrowPos.ey = arrowPos.sy - (rcCell2.bottom - rcCell2.top) * cell.OrderSlip();
			arrowPos.type = 1;
			arrowPos.col = CenterCol + 4;
			arrowPos.row = row;
			arrowPos.slip = cell.OrderSlip();
			arrowPos.startRow = _StartRowForValue;
			arrowPos.endRow = _EndRowForValue - 1;
			AddArrowPos(std::move(arrowPos));

			if (cell.OrderSlip() > 0)
			{
				//text.Format(_T("%d(+%d)"), cell.StopOrderList.size(), cell.OrderSlip());
				for (int i = row; i >= row - cell.OrderSlip(); i--)
				{
					if (i >= _StartRowForValue && i <= _EndRowForValue)
					{
						pos1.Row = i;
						pos1.Col = CenterCol + 3;
						_StopOrderPos.insert(pos1);
						//_CurPos.insert(pos1);
						pos1.Row = i;
						pos1.Col = CenterCol + 4;
						_StopOrderPos.insert(pos1);
						//_CurPos.insert(pos1);
					}
				}
			}
			else if (cell.OrderSlip() < 0)
			{
				//text.Format(_T("%d(-%d)"), cell.StopOrderList.size(), cell.OrderSlip());
				for (int i = row; i <= row - cell.OrderSlip(); i++)
				{
					if (i >= _StartRowForValue && i <= _EndRowForValue)
					{
						pos1.Row = i;
						pos1.Col = CenterCol + 3;
						_StopOrderPos.insert(pos1);
						//_CurPos.insert(pos1);

						pos1.Row = i;
						pos1.Col = CenterCol + 4;
						_StopOrderPos.insert(pos1);
						//_CurPos.insert(pos1);
					}
				}
			}
			else
			{
				//text.Format(_T("%d(%d)"), cell.StopOrderList.size(), cell.OrderSlip());
				if (row >= _StartRowForValue && row <= _EndRowForValue)
				{
					pos1.Row = row;
					pos1.Col = CenterCol + 3;
					_StopOrderPos.insert(pos1);
					//_CurPos.insert(pos1);

					pos1.Row = row;
					pos1.Col = CenterCol + 4;
					_StopOrderPos.insert(pos1);
					//_CurPos.insert(pos1);
				}
			}
			text.Format(_T("%d(%d)"), cell.StopOrderListHd.size(), cell.OrderSlip());
		}
		pos.Row = row;
		//_CurPos.insert(pos);
		QuickSetText(pos.Col, pos.Row, text);
	}

	QuickSetNumber(CenterCol - 4, _StartRowForValue - 1, sell);
	QuickSetNumber(CenterCol + 4, _StartRowForValue - 1, buy);
}

void VtOrderPanelGrid::SetStopOrderInfo(CellPosMap& refreshMap)
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;

	VtSymbol* sym = _CenterWnd->Symbol();

	_StopOrderPos.clear();
	int buy = 0, sell = 0;

	for (auto it = _StopOrderMgr->StopOrderMapHd.begin(); it != _StopOrderMgr->StopOrderMapHd.end(); ++it)
	{
		HdOrderRequest* order = it->second;
		if (order->Position == VtPositionType::Buy)
		{
			buy += order->Amount;
		}
		else if (order->Position == VtPositionType::Sell)
		{
			sell += order->Amount;
		}

		int row = FindRowFromCenterValue(order->Price);  

		if (row < _StartRowForValue)
			continue;
		if (row > _EndRowForValue)
			continue;

		SetStopOrder(refreshMap, order);
	}

	std::map<int, HdOrderRequest*>& profitLossMap = _CutMgr->GetStopOrderMap();
	for (auto it = profitLossMap.begin(); it != profitLossMap.end(); ++it)
	{
		HdOrderRequest* order = it->second;
		if (order->Position == VtPositionType::Buy)
		{
			buy += order->Amount;
		}
		else if (order->Position == VtPositionType::Sell)
		{
			sell += order->Amount;
		}

		int row = FindRowFromCenterValue(order->Price);

		if (row < _StartRowForValue)
			continue;
		if (row > _EndRowForValue)
			continue;

		SetStopOrder(refreshMap, order);
	}

	VtCellPos pos;
	QuickSetNumber(CenterCol - 4, _StartRowForValue - 1, sell);
	pos.Col = CenterCol - 4;
	pos.Row = _StartRowForValue - 1;
	refreshMap[pos] = pos;
	QuickSetNumber(CenterCol + 4, _StartRowForValue - 1, buy);
	pos.Col = CenterCol + 4;
	pos.Row = _StartRowForValue - 1;
	refreshMap[pos] = pos;
}

void VtOrderPanelGrid::ShowStopOrderInfo()
{
// 	StopOrderPosUnion.clear();
// 	std::set_union(_StopOrderPos.begin(), _StopOrderPos.end(), _OldStopOrderPos.begin(), _OldStopOrderPos.end(), std::inserter(StopOrderPosUnion, StopOrderPosUnion.begin()));
// 	for (auto it = StopOrderPosUnion.begin(); it != StopOrderPosUnion.end(); ++it)
// 	{
// 		VtCellPos pos = *it;
// 		QuickRedrawCell(pos.Col, pos.Row);
// 	}
// 
// 	_OldStopOrderPos.clear();
// 	for (auto it = _StopOrderPos.begin(); it != _StopOrderPos.end(); ++it)
// 	{
// 		VtCellPos pos = *it;
// 		_OldStopOrderPos.insert(pos);
// 	}
// 
// 	QuickRedrawCell(CenterCol - 4, _StartRowForValue - 1);
// 	QuickRedrawCell(CenterCol + 4, _StartRowForValue - 1);
}

void VtOrderPanelGrid::ClearOldStopOrders()
{
	CUGCell cell;
	for (auto it = ArrowPosVector.begin(); it != ArrowPosVector.end(); ++it)
	{
		VtArrowPos pos = *it;
		GetCell(pos.col, pos.row, &cell);
		cell.OrderType(0);
		cell.StopOrderListHd.clear();
		cell.OrderCount(0);
		SetCell(pos.col, pos.row, &cell);
		QuickSetCellType(pos.col, pos.row, UGCT_NORMAL);
		ClearText(pos.col, pos.row);
		VtCellPos cellPos;
		cellPos.Col = pos.col;
		cellPos.Row = pos.row;
		//_OldCellPos.insert(cellPos);
	}
	ClearArrowPos();
}

void VtOrderPanelGrid::ClearOldStopOrders(CellPosMap& refreshMap)
{
	CUGCell cell;
	for (auto it = ArrowPosVector.begin(); it != ArrowPosVector.end(); ++it)
	{
		VtArrowPos pos = *it;
		GetCell(pos.col, pos.row, &cell);
		cell.OrderType(0);
		cell.StopOrderListHd.clear();
		cell.OrderCount(0);
		SetCell(pos.col, pos.row, &cell);
		QuickSetCellType(pos.col, pos.row, UGCT_NORMAL);
		ClearText(pos.col, pos.row);
		VtCellPos cellPos;
		cellPos.Col = pos.col;
		cellPos.Row = pos.row;
		//_OldCellPos.insert(cellPos);
		refreshMap[cellPos] = cellPos;
	}
	ClearArrowPos();

	for (auto it = _StopOrderPos.begin(); it != _StopOrderPos.end(); ++it)
	{
		refreshMap[*it] = *it;
	}
}

void VtOrderPanelGrid::SetCloseLine()
{
	_CloseLinePos.clear();
	for (int i = 0; i < _ColCount; ++i)
	{
		if (_CloseLineRow >= _StartRowForValue && _CloseLineRow < _EndRowForValue)
		{
			QuickSetBorder(i, _CloseLineRow, UG_BDR_BTHIN);
			QuickSetBorderColor(i, _CloseLineRow, &m_ClosePen);
			//QuickRedrawCell(i, _CloseLineRow);
			VtCellPos pos;
			pos.Col = i;
			pos.Row = _CloseLineRow;
			_CloseLinePos.insert(pos);
			//_CurPos.insert(pos);
		}
	}
}

void VtOrderPanelGrid::SetCloseLine(CellPosMap& refreshMap)
{
	_CloseLinePos.clear();
	for (int i = 0; i < _ColCount; ++i)
	{
		if (_CloseLineRow >= _StartRowForValue && _CloseLineRow < _EndRowForValue)
		{
			QuickSetBorder(i, _CloseLineRow, UG_BDR_BTHIN);
			QuickSetBorderColor(i, _CloseLineRow, &m_ClosePen);
			//QuickRedrawCell(i, _CloseLineRow);
			VtCellPos pos;
			pos.Col = i;
			pos.Row = _CloseLineRow;
			_CloseLinePos.insert(pos);
			refreshMap[pos] = pos;
		}
	}
}

void VtOrderPanelGrid::ClearTotalInfoCells()
{
	CUGCell cell;

	int col = 0, row = 0;
	col = CenterCol - 4;
	row = _EndRowForValue + 1;
	QuickSetText(col, row, _T(""));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);

	col = CenterCol - 3;
	row = _EndRowForValue + 1;
	QuickSetText(col, row, _T(""));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);

	col = CenterCol + 4;
	row = _EndRowForValue + 1;
	QuickSetText(col, row, _T(""));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);

	col = CenterCol + 3;
	row = _EndRowForValue + 1;
	QuickSetText(col, row, _T(""));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);

	col = CenterCol;
	row = _EndRowForValue;
	QuickSetText(col, row, _T(""));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);

	col = CenterCol - 1; 
	row = _EndRowForValue;
	QuickSetText(col, row, _T(""));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);


	col = CenterCol + 1;
	row = _EndRowForValue;
	QuickSetText(col, row, _T(""));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);

	col = CenterCol - 2;
	row = _EndRowForValue;
	QuickSetText(col, row, _T(""));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);


	col = CenterCol + 2;
	row = _EndRowForValue;
	QuickSetText(col, row, _T(""));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);

	col = CenterCol;
	row = _EndRowForValue + 1;
	QuickSetText(col, row, _T(""));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);

	UnJoinCells(CenterCol - 2, _EndRowForValue + 1);
	UnJoinCells(CenterCol + 1, _EndRowForValue + 1);
}

void VtOrderPanelGrid::ClearTotalHogaInfo()
{
	CUGCell cell;

	int col = 0, row = 0;
	col = CenterCol - 4;
	row = _EndRowForValue + 1;
	QuickSetText(col, row, _T(""));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);

	col = CenterCol - 3;
	row = _EndRowForValue + 1;
	QuickSetText(col, row, _T(""));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);

	col = CenterCol + 4;
	row = _EndRowForValue + 1;
	QuickSetText(col, row, _T(""));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);

	col = CenterCol + 3;
	row = _EndRowForValue + 1;
	QuickSetText(col, row, _T(""));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);

	col = CenterCol;
	row = _EndRowForValue;
	QuickSetText(col, row, _T(""));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);

	col = CenterCol - 1;
	row = _EndRowForValue;
	QuickSetText(col, row, _T(""));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);


	col = CenterCol + 1;
	row = _EndRowForValue;
	QuickSetText(col, row, _T(""));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);

	col = CenterCol - 2;
	row = _EndRowForValue;
	QuickSetText(col, row, _T(""));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);


	col = CenterCol + 2;
	row = _EndRowForValue;
	QuickSetText(col, row, _T(""));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);

	
	col = CenterCol;
	row = _EndRowForValue;
	QuickSetText(col, row, _T(""));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);
	
}

void VtOrderPanelGrid::SetPositionInfo()
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr)
		return;
	VtSymbol* sym = _CenterWnd->Symbol();
	if (_OrderConfigMgr->Type() == 0)
	{
		VtAccount* acnt = _OrderConfigMgr->Account();
		VtPosition* posi = acnt->FindPosition(sym->ShortCode);
		ShowPosition(posi, sym);
	}
	else
	{
		VtFund* fund = _OrderConfigMgr->Fund();
		int count = 0;
		VtPosition posi = fund->GetPosition(sym->ShortCode, count);
		if (count == 0)
			ShowPosition(nullptr, sym);
		else
			ShowPosition(&posi, sym);
	}
}

void VtOrderPanelGrid::SetPositionInfo(CellPosMap& refreshMap)
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr)
		return;
	VtSymbol* sym = _CenterWnd->Symbol();
	if (_OrderConfigMgr->Type() == 0)
	{
		VtAccount* acnt = _OrderConfigMgr->Account();
		if (!acnt)
			return;
		VtPosition* posi = acnt->FindPosition(sym->ShortCode);
		ShowPosition(refreshMap, posi, sym);
	}
	else
	{
		VtFund* fund = _OrderConfigMgr->Fund();
		if (!fund)
			return;
		int count = 0;
		VtPosition posi = fund->GetPosition(sym->ShortCode, count);
		if (count == 0)
			ShowPosition(refreshMap, nullptr, sym);
		else
			ShowPosition(refreshMap, &posi, sym);
	}
}

void VtOrderPanelGrid::RefreshGrid()
{
	int count;
	int *packets;
	if ((count = _RefreshBuf.get(&packets)) <= 0) {
		return;
	}

	for (int i = 0; i < count; i++) {
		RefreshInfo2(packets[i]);
	}
}

void VtOrderPanelGrid::RefreshOrderPosition()
{
	CellPosMap refreshMap;
	ClearOldPosition(refreshMap);
	SetPositionInfo(refreshMap);
	RefreshCells(refreshMap);
}

void VtOrderPanelGrid::ResizeGrid(int height)
{
	CellPosMap refreshMap;

	ClearLowerCtrls(refreshMap);
	ClearTotalHoga(refreshMap);
	ClearOldValues(refreshMap);

	CUGCell cell;
	GetCell(3, 0, &cell);


	_RowCount = GetMaxRow();
	SetNumberRows(_RowCount);
	_EndRowForValue = _RowCount - 2;

	for (int yIndex = 0; yIndex < _RowCount; yIndex++) {
		SetRowHeight(yIndex, _CellHeight);
		for (int xIndex = 0; xIndex < _ColCount; xIndex++) {
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);
			QuickSetBorder(xIndex, yIndex, UG_BDR_BTHIN);
			QuickSetBorderColor(xIndex, yIndex, &m_Pen);
		}
	}
	
	InitButtonCol();
	SetOrderArea();
	SetCenterValue(refreshMap);
	SetQuoteInfo(refreshMap);
	SetPositionInfo(refreshMap);
	SetHogaInfo(refreshMap);
	SetStopOrderInfo(refreshMap);
	SetOrderInfo(refreshMap);
	ShowOrderInfo(refreshMap);
	SetCloseLine(refreshMap);

	SetColTitle(true);

	ShowTotalHoga(refreshMap);
	ShowTotalOrder(refreshMap);
	ShowTotalStopOrder(refreshMap);
	ShowUpperCtrls(refreshMap);
	ShowLowerCtrls(refreshMap);

	RefreshCells(refreshMap);

	_OldRowCount = _RowCount;
}

void VtOrderPanelGrid::InitInfo()
{
	CellPosMap refreshMap;
	ClearTotalHoga(refreshMap);
	ClearTotalOrder(refreshMap);
	ClearOldValues(refreshMap);

	_RowCount = GetMaxRow();
	_OldRowCount = _RowCount;
	_EndRowForValue = _RowCount - 2;
	SetNumberRows(_RowCount);

	for (int yIndex = 0; yIndex < _RowCount; yIndex++) {
		SetRowHeight(yIndex, _CellHeight);
		for (int xIndex = 0; xIndex < _ColCount; xIndex++) {
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);
			QuickSetBorder(xIndex, yIndex, UG_BDR_BTHIN);
			QuickSetBorderColor(xIndex, yIndex, &m_Pen);
		}
	}

	InitButtonCol();
	_CloseRow = FindCenterRow();
	SetCenterRow(_CloseRow);

	SetCenterValue(refreshMap);
	SetQuoteInfo(refreshMap);
	SetPositionInfo(refreshMap);
	SetHogaInfo(refreshMap);
	SetOrderInfo(refreshMap);
	SetStopOrderInfo(refreshMap);
	SetCloseLine(refreshMap);

	SetLowerCtrls(refreshMap);

	SetOrderArea();

	ShowTotalHoga(refreshMap);
	ShowTotalOrder(refreshMap);
	ShowTotalStopOrder(refreshMap);
	ShowLowerCtrls(refreshMap);

	RefreshCells(refreshMap);
}

void VtOrderPanelGrid::InitQuote(VtQuote* quote)
{
	if (!quote || _InitQuote)
		return;

	_Close = quote->close;
	_High = quote->high;
	_Low = quote->low;

	SetQuoteInfo();

	for (auto it = _QuotePos.begin(); it != _QuotePos.end(); ++it)
	{
		VtCellPos pos = *it;
		//_OldQuotePos.insert(pos);
	}

	ShowQuoteInfo();

	InitOrder();

	//InitHogaFromQuote(quote);

	_InitQuote = true;
}

void VtOrderPanelGrid::InitHoga(VtHoga* hoga)
{
	if (!hoga || _InitHoga)
		return;

	//InitHogaFromQuote(_Quote);
	SetHogaInfo(hoga);

	for (auto it = _HogaPos.begin(); it != _HogaPos.end(); ++it)
	{
		VtCellPos pos = *it;
		//_OldHogaPos.insert(pos);
	}

	ShowHogaInfo();

	ShowTotolHogas(hoga);

	_InitHoga = true;
}

void VtOrderPanelGrid::InitOrder()
{
	if (_InitOrder)
		return;
	ClearOrders();

	SetOrderInfo();

	for (auto it = _OrderPos.begin(); it != _OrderPos.end(); ++it)
	{
		VtCellPos pos = *it;
		//_OldOrderPos.insert(pos);
	}

	ShowOrderInfo();

	ShowTotalOrder();

	_InitOrder = true;
}

void VtOrderPanelGrid::InitAllInfo()
{
	/*
	ClearOldValues(_CloseRow, 0);
	ClearHogas();

	_CloseRow = FindCenterRow();
	_HighRow = 0;
	_LowRow = 0;

	_OldCloseRow = 0;
	_OldHighRow = 0;
	_OldLowRow = 0;

	_Close = 0.0;
	_Low = 0.0;
	_High = 0.0;

	RedrawCell(_OldSelectedCol, _OldSelectedRow);

	SetQuoteInfo();
	ShowQuoteInfo();
	SetHogaInfo(_OrderConfigMgr->Hoga());
	ShowHogaInfo();
	SetOrderInfo();
	ShowOrderInfo();
	SetStopOrderInfo();
	ShowStopOrderInfo();

	_SelectedCol = -1;
	_SelectedRow = -1;
	SetClickRect(nullptr);

	_InitHoga = false;
	_InitQuote = false;
	_InitOrder = false;
	RedrawAll();
	*/
}

void VtOrderPanelGrid::OnReceiveRealtimeQuote(VtRealtimeQuote* quote)
{
	if (!quote)
		return;
}

void VtOrderPanelGrid::OnReceiveRealtimeQuote(VtQuote* quote)
{
	if (!quote)
		return;
	CheckStopTouched(quote->close);
	if (_Close == quote->close)
		return;
	
	ClearOldQuotes();

	int diff = VtSymbolManager::GetTickDiff(_Close, quote->close, _OrderConfigMgr->Master());

	_CloseRow = _CloseRow + diff;

	_Close = quote->close;
	_High = quote->high;
	_Low = quote->low;

	//ShowQuoteInfo(_CloseRow, true);
	SetQuoteInfo();

	ShowQuoteInfo();
}

void VtOrderPanelGrid::OnReceiveRealtimeHoga(VtRealtimeHoga* hoga)
{
	if (!hoga)
		return;

}

void VtOrderPanelGrid::OnReceiveRealtimeHoga(VtHoga* hoga)
{
	if (!hoga || !_OrderConfigMgr->Master())
		return;

	ClearOldHogas();

	SetHogaInfo(hoga);

	ShowHogaInfo();

	ShowTotolHogas(hoga);
}


void VtOrderPanelGrid::InitHogaFromQuote(VtQuote* quote)
{
	if (!quote)
		return;

	VtHogaManager* hogaMgr = VtHogaManager::GetInstance();
	VtHoga* hoga = hogaMgr->FindAddHoga(quote->fullCode);
	if (!hoga)
		return;
	
	for (int i = 0; i < 5; i++)
	{
		hoga->Ary[i].BuyPrice = VtSymbolManager::GetNextValue(_Close, _OrderConfigMgr->Master(), i + 1);
		hoga->Ary[i].SellPrice = VtSymbolManager::GetNextValue(_Close, _OrderConfigMgr->Master(), i - 5);
	}

	SetHogaInfo(hoga);
	
	for (auto it = _HogaPos.begin(); it != _HogaPos.end(); ++it)
	{
		VtCellPos pos = *it;
		//_OldHogaPos.insert(pos);
	}
}

void VtOrderPanelGrid::ClearOldOrders()
{
// 	CUGCell cell;
// 	for (auto it = _OldOrderPos.begin(); it != _OldOrderPos.end(); ++it)
// 	{
// 		VtCellPos pos = *it;
// 		GetCell(pos.Col, pos.Row, &cell);
// 		cell.OrderType(0);
// 		cell.OrderList.clear();
// 		cell.OrderMap.clear();
// 		cell.OrderCount(0);
// 		cell.SetCellType(UGCT_NORMAL);
// 		cell.SetText(_T(""));
// 		SetCell(pos.Col, pos.Row, &cell);
// 		_OldCellPos.insert(pos);
// 	}
}

void VtOrderPanelGrid::ClearOldOrders(CellPosMap& refreshMap)
{
	if (!_CenterWnd || !_CenterWnd->GetSafeHwnd())
		return;

	CUGCell cell;
	for (auto it = _OrderPos.begin(); it != _OrderPos.end(); ++it)
	{
		VtCellPos pos = *it;
		GetCell(pos.Col, pos.Row, &cell);
		cell.OrderType(0);
		cell.OrderList.clear();
		cell.OrderMap.clear();
		cell.OrderCount(0);
		cell.SetCellType(UGCT_NORMAL);
		cell.SetText(_T(""));
		SetCell(pos.Col, pos.Row, &cell);
		refreshMap[pos] = pos;
	}
}

void VtOrderPanelGrid::ClearText(int col, int row)
{
	QuickSetText(col, row, "");
}

void VtOrderPanelGrid::ClearOldValues()
{
	ClearOldQuotes();
	ClearOldHogas();
	ClearOldOrders();
	ClearOldStopOrders();

	ClearOldCloseLine();
}


void VtOrderPanelGrid::ClearOldValues(CellPosMap& refreshMap)
{
	ClearOldQuotes(refreshMap);
	ClearOldHogas(refreshMap);
	ClearOldOrders(refreshMap);
	ClearOldStopOrders(refreshMap);

	ClearOldCloseLine(refreshMap);
}

void VtOrderPanelGrid::ClearOldHogas()
{
// 	for (auto it = _OldHogaPos.begin(); it != _OldHogaPos.end(); ++it)
// 	{
// 		VtCellPos pos = *it;
// 		ClearText(pos.Col, pos.Row);
// 		QuickSetBackColor(pos.Col, pos.Row, RGB(255, 255, 255));
// 		_OldCellPos.insert(pos);
// 	}
}

void VtOrderPanelGrid::ClearOldHogas(CellPosMap& refreshMap)
{
	for (auto it = _HogaPos.begin(); it != _HogaPos.end(); ++it)
	{
		VtCellPos pos = *it;
		ClearText(pos.Col, pos.Row);
		QuickSetBackColor(pos.Col, pos.Row, RGB(255, 255, 255));
		refreshMap[pos] = pos;
	}
}

void VtOrderPanelGrid::ClearHogas()
{
	for (auto it = _HogaPos.begin(); it != _HogaPos.end(); ++it)
	{
		VtCellPos pos = *it;
		ClearText(pos.Col, pos.Row);
		QuickSetBackColor(pos.Col, pos.Row, RGB(255, 255, 255));
	}
}

void VtOrderPanelGrid::ClearOrders()
{
	CUGCell cell;
	for (auto it = _OrderPos.begin(); it != _OrderPos.end(); ++it)
	{
		VtCellPos pos = *it;
		GetCell(pos.Col, pos.Row, &cell);
		cell.OrderType(0);
		cell.OrderList.clear();
		cell.OrderMap.clear();
		SetCell(pos.Col, pos.Row, &cell);
		QuickSetCellType(pos.Col, pos.Row, UGCT_NORMAL);
		ClearText(pos.Col, pos.Row);
	}
}

void VtOrderPanelGrid::ClearOldQuotes()
{
// 	CUGCell cell;
// 	for (auto it = _OldQuotePos.begin(); it != _OldQuotePos.end(); ++it)
// 	{
// 		VtCellPos pos = *it;
// 		GetCell(pos.Col, pos.Row, &cell);
// 		cell.SetLabelText( _T(""));
// 		cell.SetCellType(UGCT_NORMAL);
// 		cell.SetBackColor(RGB(255, 255, 255));
// 		cell.SetTextColor(RGB(0, 0, 0));
// 		cell.OrderType(0);
// 		SetCell(pos.Col, pos.Row, &cell);
// 		//_OldCellPos.insert(pos);
// 	}
}

void VtOrderPanelGrid::ClearOldQuotes(CellPosMap& refreshMap)
{
	CUGCell cell;
	for (auto it = _QuotePos.begin(); it != _QuotePos.end(); ++it)
	{
		VtCellPos pos = *it;
		if (pos.Row >= _EndRowForValue) {
			continue;
		}
		GetCell(pos.Col, pos.Row, &cell);
		cell.SetLabelText(_T(""));
		cell.SetCellType(UGCT_NORMAL);
		cell.SetBackColor(RGB(255, 255, 255));
		cell.SetTextColor(RGB(0, 0, 0));
		cell.OrderType(0);
		SetCell(pos.Col, pos.Row, &cell);
		
		refreshMap[pos] = pos;
	}
}

void VtOrderPanelGrid::ClearOldPosition()
{
	CUGCell cell;
	VtCellPos pos;
	pos.Col = CenterCol;
	pos.Row = _OrderPositionRow;
	GetCell(pos.Col, pos.Row, &cell);
	//QuickSetBorder(pos.Col, pos.Row, UG_BDR_LTHIN);
	//QuickSetBorderColor(pos.Col, pos.Row, &m_Pen);
	cell.SetLabelText(_T(""));
	cell.SetCellType(UGCT_NORMAL);
	cell.SetBackColor(RGB(255, 255, 255));
	cell.SetTextColor(RGB(0, 0, 0));
	cell.OrderType(0);
	SetCell(pos.Col, pos.Row, &cell);
}

void VtOrderPanelGrid::ClearOldPosition(CellPosMap& refreshMap)
{
	CUGCell cell;
	VtCellPos pos;
	pos.Col = CenterCol;
	pos.Row = _OrderPositionRow;
	GetCell(pos.Col, pos.Row, &cell);
	//QuickSetBorder(pos.Col, pos.Row, UG_BDR_LTHIN);
	//QuickSetBorderColor(pos.Col, pos.Row, &m_Pen);
	//cell.SetLabelText(_T(""));
	cell.SetCellType(UGCT_NORMAL);
	//cell.SetBackColor(RGB(255, 255, 255));
	//cell.SetTextColor(RGB(0, 0, 0));
	cell.OrderType(0);
	SetCell(pos.Col, pos.Row, &cell);
	refreshMap[pos] = pos;
	refreshMap[_OldPositionPos] = _OldPositionPos;

	//LOG_F(INFO, _T("ClearOldPosition : _OrderPositionRow = %d"), _OrderPositionRow);
}

void VtOrderPanelGrid::ClearOldCloseLine()
{
	CUGCell cell;
	for (auto it = _CloseLinePos.begin(); it != _CloseLinePos.end(); ++it) {
		VtCellPos pos = *it;
		GetCell(pos.Col, pos.Row, &cell);
		cell.SetBorder(UG_BDR_BTHIN);
		cell.SetBorderColor(&m_Pen);
		SetCell(pos.Col, pos.Row, &cell);
		//_OldCellPos.insert(pos);
	}
}

void VtOrderPanelGrid::ClearOldCloseLine(CellPosMap& refreshMap)
{
	CUGCell cell;
	for (auto it = _CloseLinePos.begin(); it != _CloseLinePos.end(); ++it) {
		VtCellPos pos = *it;
		GetCell(pos.Col, pos.Row, &cell);
		cell.SetBorder(UG_BDR_BTHIN);
		cell.SetBorderColor(&m_Pen);
		SetCell(pos.Col, pos.Row, &cell);
		refreshMap[pos] = pos;
	}
}

void VtOrderPanelGrid::ShowProductRemain()
{

}


void VtOrderPanelGrid::DrawMovingRect(int col, long row)
{
	if (_OldMMCol == _MMCol && _OldMMRow == _MMRow)
		return;

	if (row >= _EndRowForValue || row < _StartRowForValue)
	{
		ClearOldMovingRect();
		_InOrderArea = false;
		return;
	}

	_InOrderArea = true;

	ClearOldMovingRect();

	CUGCell cell;
	if (col == CenterCol - 3)
	{
		GetCell(CenterCol - 3, row, &cell);
		cell.Focused(true);
		SetCell(CenterCol - 3, row, &cell);
		GetCell(CenterCol, row, &cell);
		cell.Focused(true);
		SetCell(CenterCol, row, &cell);

		QuickRedrawCell(CenterCol -3, row);
		QuickRedrawCell(CenterCol, row);
	}
	else if (col == CenterCol + 3)
	{
		GetCell(CenterCol + 3, row, &cell);
		cell.Focused(true);
		SetCell(CenterCol + 3, row, &cell);
		GetCell(CenterCol, row, &cell);
		cell.Focused(true);
		SetCell(CenterCol, row, &cell);
		QuickRedrawCell(CenterCol + 3, row);
		QuickRedrawCell(CenterCol, row);
	}
	else if (col == CenterCol - 4)
	{
		GetCell(CenterCol - 4, row, &cell);
		cell.Focused(true);
		SetCell(CenterCol - 4, row, &cell);
		GetCell(CenterCol, row, &cell);
		cell.Focused(true);
		SetCell(CenterCol, row, &cell);
		QuickRedrawCell(CenterCol - 4, row);
		QuickRedrawCell(CenterCol, row);
	}
	else if (col == CenterCol + 4)
	{
		GetCell(CenterCol + 4, row, &cell);
		cell.Focused(true);
		SetCell(CenterCol + 4, row, &cell);
		GetCell(CenterCol, row, &cell);
		cell.Focused(true);
		SetCell(CenterCol, row, &cell);
		QuickRedrawCell(CenterCol + 4, row);
		QuickRedrawCell(CenterCol, row);
	}
	else if (col == CenterCol)
	{
		GetCell(CenterCol, row, &cell);
		cell.Focused(true);
		SetCell(CenterCol, row, &cell);
		QuickRedrawCell(CenterCol, row);
	}
	
	_OldMMCol = col;
	_OldMMRow = row;
}

void VtOrderPanelGrid::DrawSelectedRect(int col, long row)
{
	if (row >= _EndRowForValue || row < _StartRowForValue) {
		_SelectedRow = -1;
		_SelectedCol = -1;
		return;
	}
	if (!(col == CenterCol - 3 || col == CenterCol + 3 || col == CenterCol - 4 || col == CenterCol + 4)) {
		_SelectedRow = -1;
		_SelectedCol = -1;
		return;
	}

	CDC *dc = m_CUGGrid->GetDC();

	CPen pen;
	CPen* oldPen = nullptr;
	pen.CreatePen(PS_SOLID, 1, RGB(0, 102, 51));
	dc->SelectObject(pen);

	RECT rectCenter;

	GetCellRect(col, row, &rectCenter);

	rectCenter.top++;
	rectCenter.left++;
	rectCenter.bottom -= 2;
	rectCenter.right -= 2;

	dc->MoveTo(rectCenter.left, rectCenter.top);
	dc->LineTo(rectCenter.right, rectCenter.top);
	dc->LineTo(rectCenter.right, rectCenter.bottom);
	dc->LineTo(rectCenter.left, rectCenter.bottom);
	dc->LineTo(rectCenter.left, rectCenter.top);

	pen.DeleteObject();

	_SelectedRow = _MMRow;
	_SelectedCol = _MMCol;

	m_CUGGrid->ReleaseDC(dc);
}

void VtOrderPanelGrid::ClearOldMovingRect()
{
	if (_InOrderArea)
	{
		CUGCell cell;
		GetCell(_OldMMCol, _OldMMRow, &cell);
		cell.Focused(false);
		SetCell(_OldMMCol, _OldMMRow, &cell);
		GetCell(CenterCol, _OldMMRow, &cell);
		cell.Focused(false);
		SetCell(CenterCol, _OldMMRow, &cell);
		QuickRedrawCell(_OldMMCol, _OldMMRow);
		QuickRedrawCell(CenterCol, _OldMMRow);
	}
}

void VtOrderPanelGrid::ChangeCenter(int oldCenter, int newCenter)
{
	CellPosMap refreshMap;
	ClearOldValues(refreshMap);

	int centerRow = newCenter;
	_CloseRow = centerRow;

	SetCenterValue(refreshMap);
	SetQuoteInfo(refreshMap);
	SetPositionInfo(refreshMap);
	SetHogaInfo(refreshMap);
	SetOrderInfo(refreshMap);
	SetStopOrderInfo(refreshMap);
	SetCloseLine(refreshMap);

	ShowTotalHoga(refreshMap);
	ShowLowerCtrls(refreshMap);
	RefreshCells(refreshMap);
	//ShowUnionCells();

	int diff = 0; 
	if (_CenterWnd && _CenterWnd->Symbol()) {
		VtSymbol* sym = _CenterWnd->Symbol();
		diff = VtSymbolManager::GetTickDiff(sym->Quote.intClose, _SelectedValInt, sym);
		_SelectedRow = _CloseRow + diff;
		ShowSelect(_SelectedCol, _SelectedRow);
	}
	
}

void VtOrderPanelGrid::OnMouseMove(int col, long row, POINT *point, UINT nFlags, BOOL processed /*= 0*/)
{
	HitTestButtonCol(col, row);
	_MouseIn = true;
	_MMCol = col;
	_MMRow = row;
	if (m_bDrawingOrder) {
		DrawMovingOrder(col, row, point);
	}
	else
		DrawMovingRect(col, row);
}

void VtOrderPanelGrid::OnKillFocus(int section, CWnd *pNewWnd)
{
	int i = 0;
}

void VtOrderPanelGrid::OnMouseLeaveFromMainGrid()
{
	if (!m_bDrawingOrder) {
		ClearSelectedCell();
		CUGCell cell;
		GetCell(_OldMMCol, _OldMMRow, &cell);
		cell.Focused(false);
		SetCell(_OldMMCol, _OldMMRow, &cell);
		GetCell(CenterCol, _OldMMRow, &cell);
		cell.Focused(false);
		SetCell(CenterCol, _OldMMRow, &cell);
		QuickRedrawCell(_OldMMCol, _OldMMRow);
		QuickRedrawCell(CenterCol, _OldMMRow);
	}

	RestoreButtonCell();
	_MouseIn = false;
}

void VtOrderPanelGrid::ShowPosition(VtPosition* posi, VtSymbol* sym)
{
	if (!sym)
		return;

	VtCellPos pos;
	if (!posi || posi->OpenQty == 0 || posi->Position == VtPositionType::None)
	{
		QuickSetCellType(CenterCol, _OrderPositionRow, UGCT_NORMAL);
		pos.Col = CenterCol;
		pos.Row = _OrderPositionRow;
		_QuotePos.insert(pos);
		//_CurPos.insert(pos);
		return;
	}

	if (RowToValueMap.size() == 0)
		return;

	auto it = RowToValueMap.begin();
	int smallVal = it->first;
	auto itr = RowToValueMap.rbegin();
	int bigVal = itr->first;


	int intAvg = static_cast<int>(ROUNDING(posi->AvgPrice * 100.0, sym->Decimal));

	if (intAvg > bigVal || intAvg < smallVal)
		return;


	int delta = 999999;
	int eRow = -1;
	for (auto it = RowToValueMap.begin(); it != RowToValueMap.end(); ++it)
	{
		int key = it->first;
		int row = it->second;
		int curDelta = std::abs(key - intAvg);
		if (curDelta < delta)
		{
			eRow = row;
			delta = curDelta;
		}
	}

	CUGCell cell;
	GetCell(CenterCol, eRow, &cell);
	if (posi->Position == VtPositionType::Buy)
		cell.OrderType(1);
	else
		cell.OrderType(2);
	cell.SetCellType(m_RemainCellIndex);
	SetCell(CenterCol, eRow, &cell);
	QuickRedrawCell(CenterCol, eRow);
	pos.Col = CenterCol;
	pos.Row = eRow;
	_OrderPositionRow = pos.Row;
	//_QuotePos.insert(pos);
	//_CurPos.insert(pos);
}

void VtOrderPanelGrid::ShowPosition(CellPosMap& refreshMap, VtPosition* posi, VtSymbol* sym)
{
	if (!sym)
		return;

	VtCellPos pos;
	if (!posi || posi->OpenQty == 0 || posi->Position == VtPositionType::None)
	{
		QuickSetCellType(CenterCol, _OrderPositionRow, UGCT_NORMAL);
		pos.Col = CenterCol;
		pos.Row = _OrderPositionRow;
		refreshMap[pos] = pos;
		_OldPositionPos = pos;
		return;
	}

	int eRow = FindNearestCellRow(posi);
	if (eRow == -1)
		return;

	CUGCell cell;
	GetCell(CenterCol, eRow, &cell);
	if (posi->Position == VtPositionType::Buy)
		cell.OrderType(1);
	else if (posi->Position == VtPositionType::Sell)
		cell.OrderType(2);
	else
		cell.OrderType(0);
	cell.SetCellType(m_RemainCellIndex);
	SetCell(CenterCol, eRow, &cell);

	pos.Col = CenterCol;
	pos.Row = eRow;
	if (pos.Row != _OrderPositionRow)
		_OrderPositionRow = pos.Row;
	refreshMap[pos] = pos;
	_OldPositionPos = pos;
}

int VtOrderPanelGrid::OnCellTypeNotify(long ID, int col, long row, long msg, long param)
{
	
	if (ID == UGCT_DROPLIST) {
		return OnDropList(ID, col, row, msg, param);
	}
	if (ID == UGCT_CHECKBOX) {
		return OnCheckbox(ID, col, row, msg, param);
	}
	
	if (ID == m_nButtonIndex) {
		return OnPushButton(ID, col, row, msg, param);
	}
	else
		return -1;
	/*
	if (ID == m_nEllipsisIndex) {
		return OnEllipsisButton(ID, col, row, msg, param);
	}
	if (ID == m_nSpinIndex) {
		return OnSpinButton(ID, col, row, msg, param);
	}
	if (ID == m_nSliderIndex) {
		return OnSlider(ID, col, row, msg, param);
	}
	*/
}

int VtOrderPanelGrid::OnDropList(long ID, int col, long row, long msg, long param)
{
	return 1;
}

int VtOrderPanelGrid::OnCheckbox(long ID, int col, long row, long msg, long param)
{
	return 1;
}

int VtOrderPanelGrid::OnDateTime(long ID, int col, long row, long msg, long param)
{
	return 1;
}

int VtOrderPanelGrid::OnPushButton(long ID, int col, long row, long msg, long param)
{
	CUGCell cell;
	GetCell(col, row, &cell);
	int nCellTypeIndex = cell.GetCellType();
	int nParam = cell.GetParam();

// 	if (msg == UGCT_BUTTONCLICK) {
// 		if (nParam == BUTTON_CLICK3) {
// 			MessageBox("The button pressed", "Cell Type Notification");
// 		}
// 	}

	if (msg == UGCT_BUTTONDOWN)
	{
		if (nParam == BUTTON_SET_CENTER) 
		{
			//MessageBox("The button pressed", "Cell Type Notification");
			if (row == -1)
			{
				ResetByCenterRow();
			}
		}
		else if (nParam == BUTTON_BUY_ATMARKET)
		{
			BuyAtMarket();
		}
		else if (nParam == BUTTON_SELL_ATMARKET)
		{
			SellAtMarket();
		}
		else if (nParam == BUTTON_CANCEL_SELL_ALLSTOP)
		{
			CancelAllSellStop();
		}
		else if (nParam == BUTTON_CANCEL_BUY_ALLSTOP)
		{
			CancelAllBuyStop();
		}
		else if (nParam == BUTTON_CANCEL_BUY_ALL_ACCEPTED)
		{
			CancelAllBuyAccepted();
		}
		else if (nParam == BUTTON_CANCEL_SELL_ALL_ACCEPTED)
		{
			CancelAllSellAccepted();
		}
		else if (nParam == BUTTON_CANCEL_ALL_ACCEPTED)
		{
			CancelAllAccepted();
		}
	}

	return TRUE;
}

int VtOrderPanelGrid::OnEllipsisButton(long ID, int col, long row, long msg, long param)
{
	return 1;
}

int VtOrderPanelGrid::OnSpinButton(long ID, int col, long row, long msg, long param)
{
	return 1;
}

int VtOrderPanelGrid::OnSlider(long ID, int col, long row, long msg, long param)
{
	return 1;
}

int VtOrderPanelGrid::FindCenterRow()
{
	int count = GetMaxRow();

	return (count - 2) / 2;
}

int VtOrderPanelGrid::GetMaxRow()
{
	RECT rect;
	GetClientRect(&rect);


	double pureHeight = rect.bottom - rect.top - _HeadHeight;

	int count = (int)(pureHeight / _CellHeight);

	return count;
}

void VtOrderPanelGrid::SetOrderArea()
{
	for (int i = 0; i < _RowCount + 1; i++)
	{
		QuickSetBackColor(CenterCol - 3, i, RGB(218, 226, 245));
		QuickSetBackColor(CenterCol + 3, i, RGB(252, 226, 228));
		//QuickSetBackColor(CenterCol - 4, i, RGB(218, 226, 245));
		//QuickSetBackColor(CenterCol + 4, i, RGB(252, 226, 228));
	}
}

void VtOrderPanelGrid::SetOrderArea(int height, int width)
{
	_CellHeight = height;
	_GridColMap[VtOrderGridCol::ORDER] = width;
	/*
	CellPosMap refreshMap;
	ClearTotalHoga(refreshMap);
	ClearLowerCtrls(refreshMap);
	ClearOldValues(refreshMap);

	_CellHeight = height;
	_RowCount = GetMaxRow();
	SetNumberRows(_RowCount);

	_CloseRow = FindCenterRow();
	SetCenterRow(_CloseRow);
	_EndRowForValue = _RowCount - 2;

	for (int yIndex = 1; yIndex < _RowCount; yIndex++) {
		SetRowHeight(yIndex, _CellHeight);
		for (int xIndex = 0; xIndex < _ColCount; xIndex++) {
			QuickSetAlignment(xIndex, yIndex, UG_ALIGNCENTER | UG_ALIGNVCENTER);
			QuickSetBorder(xIndex, yIndex, UG_BDR_BTHIN);
			QuickSetBorderColor(xIndex, yIndex, &m_Pen);
		}
	}

	//_OrderWidth = width;
	SetColWidth(1, _GridColMap[VtOrderGridCol::ORDER]);
	SetColWidth(7, _GridColMap[VtOrderGridCol::ORDER]);

// 	int gridWidth = GetMaxWidth();
// 	WINDOWPLACEMENT wp = {};
// 	wp.length = sizeof(WINDOWPLACEMENT);
// 	this->GetWindowPlacement(&wp);
// 	SetWindowPos(NULL, 0, 0, gridWidth, wp.rcNormalPosition.bottom - wp.rcNormalPosition.top, SWP_NOMOVE | SWP_NOZORDER);

	SetColTitle(true);
	SetOrderArea();

	SetCenterValue(refreshMap);
	SetQuoteInfo(refreshMap);
	SetPositionInfo(refreshMap);
	SetHogaInfo(refreshMap);
	SetStopOrderInfo(refreshMap);
	SetOrderInfo(refreshMap);
	SetCloseLine(refreshMap);
	ShowTotalHoga(refreshMap);
	ShowLowerCtrls(refreshMap);
	
	//RefreshCells(refreshMap);
	RedrawAll();
	_OldRowCount = _RowCount;
	*/
}

void VtOrderPanelGrid::SetOrderArea(int endRow)
{
	for (int i = 0; i < _RowCount; i++) {
		if (i <= endRow) {
			QuickSetBackColor(CenterCol - 3, i, RGB(218, 226, 245));
			QuickSetBackColor(CenterCol + 3, i, RGB(252, 226, 228));
		} else {
			QuickSetBackColor(CenterCol - 3, i, RGB(255, 255, 255));
			QuickSetBackColor(CenterCol + 3, i, RGB(255, 255, 255));
		}
	}
}

void VtOrderPanelGrid::ShowCloseLine()
{
	for (int i = 0; i < _ColCount; ++i) {
		if (_CloseLineRow >= _StartRowForValue && _CloseLineRow < _EndRowForValue) {
			QuickSetBorder(i, _CloseLineRow, UG_BDR_BTHIN);
			QuickSetBorderColor(i, _CloseLineRow, &m_ClosePen);
			QuickRedrawCell(i, _CloseLineRow);
		}
	}

	_OldCloseLineRow = _CloseLineRow;
}

int VtOrderPanelGrid::GetMaxWidth()
{
	int total = 80;
	if (_ShowStopCol)
		total += 2 * _StopWidth;
	if (_ShowOrderCol)
		total += 2 * _OrderWidth;
	if (_ShowOrderCountCol)
		total += 2 * _OrderCountWidth;
	if (_ShowOrderQuantityCol)
		total += 2 * _OrderQuantityWidth;
	return total + 6;
}

int VtOrderPanelGrid::GetMaxHeight()
{
	int total = _HeadHeight;
	total += _CellHeight * _RowCount;

	return total;
}

void VtOrderPanelGrid::ApplyProfitLossForPosition()
{
	if (!_CutMgr || !_CenterWnd || !_CenterWnd->Symbol())
		return;
	// 모든 익절,손절 스탑 취소
	_CutMgr->RemoveAllHd();
	_CutMgr->MakePositionStopByRemain(_CenterWnd->Symbol());
	RefreshStopOrder();
}

int VtOrderPanelGrid::ShowHideOrderGrid(std::vector<bool>& colOptions)
{
	for (int i = 0; i < colOptions.size(); ++i) {
		if (i == 0) {
			if (colOptions[i]) { // for order column
				SetColWidth(1, _GridColMap[VtOrderGridCol::ORDER]);
				SetColWidth(7, _GridColMap[VtOrderGridCol::ORDER]);
			} else {
				SetColWidth(1, 0);
				SetColWidth(7, 0);
			}
		}
		else if (i == 1) { // for stop column
			if (colOptions[1]) {
				SetColWidth(0, _GridColMap[VtOrderGridCol::STOP]);
				SetColWidth(8, _GridColMap[VtOrderGridCol::STOP]);
			} else {
				SetColWidth(0, 0);
				SetColWidth(8, 0);
			}
		}
		else if (i == 2) { // for order count column
			if (colOptions[2]) {
				SetColWidth(2, _GridColMap[VtOrderGridCol::COUNT]);
				SetColWidth(6, _GridColMap[VtOrderGridCol::COUNT]);
			} else {
				SetColWidth(2, 1);
				SetColWidth(6, 1);
			}
		}
	}

	return 0;
}

int VtOrderPanelGrid::FindNearestCellPos(VtPosition* posi)
{
	if (!posi || RowToValueMap.size() == 0)
		return -1;

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindSymbol(posi->ShortCode);
	if (!sym)
		return -1;

	auto it = RowToValueMap.begin();
	int smallVal = it->first;
	auto itr = RowToValueMap.rbegin();
	int bigVal = itr->first;


	int intAvg = static_cast<int>(ROUNDING(posi->AvgPrice * std::pow(10, sym->Decimal), sym->Decimal));

	if (intAvg > bigVal || intAvg < smallVal)
		return -1;


	int delta = 999999;
	int eRow = -1;
	for (auto it = RowToValueMap.begin(); it != RowToValueMap.end(); ++it)
	{
		int key = it->first;
		int row = it->second;
		int curDelta = std::abs(key - intAvg);
		if (curDelta < delta)
		{
			eRow = row;
			delta = curDelta;
		}
	}

	return eRow;
}

int VtOrderPanelGrid::FindNearestCellRow(VtPosition* posi)
{
	if (!posi || RowToValueMap.size() == 0)
		return -1;

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindSymbol(posi->ShortCode);
	if (!sym)
		return -1;

	int intAvg = static_cast<int>(ROUNDING(posi->AvgPrice * std::pow(10, sym->Decimal), sym->Decimal));
	intAvg = intAvg - intAvg % sym->intTickSize;
	
	auto it = RowToValueMap.find(intAvg);
	if (it != RowToValueMap.end())
		return it->second;
	else
		return -1;
}

void VtOrderPanelGrid::PutOrderByRemain(int price)
{

}

void VtOrderPanelGrid::RefreshInfo()
{
	CString msg;
	// Get starting timepoint 
	auto start = high_resolution_clock::now();

	CellPosMap refreshMap;
	ClearOldCloseLine(refreshMap);
	ClearOldHogas(refreshMap);
	ClearTotalHoga(refreshMap);
	ClearOldOrders(refreshMap);
	SetHogaInfo(refreshMap);
	SetCloseLine(refreshMap);
	SetOrderInfo(refreshMap);
	ShowTotalOrder(refreshMap);
	ShowTotalHoga(refreshMap);
	RefreshCells(refreshMap);

	// Get ending timepoint 
	auto stop = high_resolution_clock::now();

	// Get duration. Substart timepoints to  
	// get durarion. To cast it to proper unit 
	// use duration cast method 
	auto duration = duration_cast<microseconds>(stop - start);

	msg.Format(_T("RefreshInfo type :: elapsed time :: microsecond = %d\n"), duration.count());
	//TRACE(msg);
}

void VtOrderPanelGrid::RefreshInfo(int type)
{
	CString msg;
	
	switch (type)
	{
	case 1:
		UpdateFilled();
		break;
	case 2:
		UpdateAccepted();
		break;
	case 3:
		UpdateUnfilled();
		break;
	case 4:
		UpdateQuote();
		break;
	case 5:
		UpdateHoga();
		break;
	default:
		break;
	}
}

void VtOrderPanelGrid::RefreshInfo2(int type)
{
	switch (type)
	{
	case 1:
		UpdateFilled();
		break;
	case 2:
		UpdateAccepted();
		break;
	case 3:
		UpdateUnfilled();
		break;
	case 4:
		UpdateQuote();
		break;
	case 5:
		UpdateHoga();
		break;
	default:
		break;
	}
}

void VtOrderPanelGrid::UpdateHoga()
{
	CellPosMap refreshMap;
	ClearOldCloseLine(refreshMap);
	ClearOldHogas(refreshMap);
	ClearTotalHoga(refreshMap);
	ClearOldOrders(refreshMap);
	SetHogaInfo(refreshMap);
	SetCloseLine(refreshMap);
	SetOrderInfo(refreshMap);
	ShowTotalOrder(refreshMap);
	ShowTotalHoga(refreshMap);
	RefreshCells(refreshMap);
}

void VtOrderPanelGrid::UpdateQuote()
{
	CellPosMap refreshMap;
	ClearOldQuotes(refreshMap);
	ClearOldOrders(refreshMap);

	if (_CenterWnd->FixedCenter())
		ShowCenterValueByFixed(refreshMap);
	else
		RefreshCenterValue(refreshMap);

	SetQuoteInfo(refreshMap);
	SetPositionInfo(refreshMap);
	SetOrderInfo(refreshMap);

	ShowTotalOrder(refreshMap);
	ShowTotalHoga(refreshMap);

	RefreshCells(refreshMap);
}

void VtOrderPanelGrid::UpdateAccepted()
{
	CellPosMap refreshMap;
	ClearOldOrders(refreshMap);
	SetOrderInfo(refreshMap);
	ShowTotalOrder(refreshMap);
	RefreshCells(refreshMap);
}

void VtOrderPanelGrid::UpdateFilled()
{
	CellPosMap refreshMap;

	ClearOldOrders(refreshMap);
	SetOrderInfo(refreshMap);

	ClearOldPosition(refreshMap);
	SetPositionInfo(refreshMap);
	ShowTotalOrder(refreshMap);
	ClearOldStopOrders(refreshMap);
	SetStopOrderInfo(refreshMap);
	RefreshCells(refreshMap);
}

void VtOrderPanelGrid::UpdateUnfilled()
{
	CellPosMap refreshMap;
	ClearOldOrders(refreshMap);
	SetOrderInfo(refreshMap);
	ShowTotalOrder(refreshMap);
	RefreshCells(refreshMap);
}

void VtOrderPanelGrid::StartTimer()
{
	SetTimer(RefreshTimer, _TimerInternal, NULL);
}

void VtOrderPanelGrid::CommitRefresh(int type)
{
	// 화면 갱신 타이머가 동작 중일때
	if (!_IsRefreshTimerWorking) {
		for(int i = 0; i < _MaxRefreshQSize; ++i)
			_RefreshQ.push(type);
		_IsRefreshTimerWorking = true;
		StartTimer();
	}
}

void VtOrderPanelGrid::SetHogaInfo(VtHoga* hoga)
{
	if (!hoga)
		return;
	VtCellPos cellPos;
	_HogaPos.clear();
	for (int i = 0; i < 5; i++)
	{
		int diff = VtSymbolManager::GetTickDiff(_Close, hoga->Ary[i].BuyPrice, _OrderConfigMgr->Master());
		int pos = _CloseRow + diff;
		if (pos >= _StartRowForValue && pos < _EndRowForValue)
		{
			QuickSetNumber(CenterCol + 2, pos, hoga->Ary[i].BuyNo);
			QuickSetNumber(CenterCol + 1, pos, hoga->Ary[i].BuyQty);
			QuickSetBackColor(CenterCol + 2, pos, RGB(255, 196, 196));
			QuickSetBackColor(CenterCol + 1, pos, RGB(255, 196, 196));
			cellPos.Col = CenterCol + 2;
			cellPos.Row = pos;
			_HogaPos.insert(cellPos);
			cellPos.Col = CenterCol + 1;
			cellPos.Row = pos;
			_HogaPos.insert(cellPos);
		}
		diff = VtSymbolManager::GetTickDiff(_Close, hoga->Ary[i].SellPrice, _OrderConfigMgr->Master());
		pos = _CloseRow + diff;
		if (pos >= _StartRowForValue && pos < _EndRowForValue)
		{
			QuickSetNumber(CenterCol - 2, pos, hoga->Ary[i].SellNo);
			QuickSetNumber(CenterCol - 1, pos, hoga->Ary[i].SellQty);
			QuickSetBackColor(CenterCol - 2, pos, RGB(196, 196, 255));
			QuickSetBackColor(CenterCol - 1, pos, RGB(196, 196, 255));
			cellPos.Col = CenterCol - 2;
			cellPos.Row = pos;
			_HogaPos.insert(cellPos);
			cellPos.Col = CenterCol - 1;
			cellPos.Row = pos;
			_HogaPos.insert(cellPos);
		}
	}
}

void VtOrderPanelGrid::SetHogaInfo()
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;
	VtSymbol* sym = _CenterWnd->Symbol();
	VtCellPos cellPos;
	_HogaPos.clear();
	for (int i = 0; i < 5; i++)
	{
		//int diff = VtSymbolManager::GetTickDiff(sym->Quote.intClose, sym->Hoga.Ary[i].IntBuyPrice, sym);
		//int pos = _CloseRow + diff;
		int pos = FindRowFromCenterValue(sym->Hoga.Ary[i].IntBuyPrice);
		if (pos >= _StartRowForValue && pos < _EndRowForValue)
		{
			QuickSetNumber(CenterCol + 2, pos, sym->Hoga.Ary[i].BuyNo);
			QuickSetNumber(CenterCol + 1, pos, sym->Hoga.Ary[i].BuyQty);
			QuickSetBackColor(CenterCol + 2, pos, RGB(255, 196, 196));
			QuickSetBackColor(CenterCol + 1, pos, RGB(255, 196, 196));
			cellPos.Col = CenterCol + 2;
			cellPos.Row = pos;
			_HogaPos.insert(cellPos);
			//_CurPos.insert(cellPos);
			cellPos.Col = CenterCol + 1;
			cellPos.Row = pos;
			_HogaPos.insert(cellPos);
			//_CurPos.insert(cellPos);
		}
		//diff = VtSymbolManager::GetTickDiff(sym->Quote.intClose, sym->Hoga.Ary[i].IntSellPrice, sym);
		//pos = _CloseRow + diff;
		pos = FindRowFromCenterValue(sym->Hoga.Ary[i].IntSellPrice);
		if (i == 0)
		{
			_CloseLineRow = pos;
		}
		if (pos >= _StartRowForValue && pos < _EndRowForValue)
		{
			QuickSetNumber(CenterCol - 2, pos, sym->Hoga.Ary[i].SellNo);
			QuickSetNumber(CenterCol - 1, pos, sym->Hoga.Ary[i].SellQty);
			QuickSetBackColor(CenterCol - 2, pos, RGB(196, 196, 255));
			QuickSetBackColor(CenterCol - 1, pos, RGB(196, 196, 255));
			cellPos.Col = CenterCol - 2;
			cellPos.Row = pos;
			_HogaPos.insert(cellPos);
			//_CurPos.insert(cellPos);
			cellPos.Col = CenterCol - 1;
			cellPos.Row = pos;
			_HogaPos.insert(cellPos);
			//_CurPos.insert(cellPos);
		}
	}
}

void VtOrderPanelGrid::SetHogaInfo(CellPosMap& refreshMap)
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;
	VtSymbol* sym = _CenterWnd->Symbol();
	VtCellPos cellPos;
	_HogaPos.clear();
	for (int i = 0; i < 5; i++)
	{
		//int diff = VtSymbolManager::GetTickDiff(sym->Quote.intClose, sym->Hoga.Ary[i].IntBuyPrice, sym);
		//int pos = _CloseRow + diff;
		int pos = FindRowFromCenterValue(sym->Hoga.Ary[i].IntBuyPrice);
		if (pos >= _StartRowForValue && pos < _EndRowForValue)
		{
			QuickSetNumber(CenterCol + 2, pos, sym->Hoga.Ary[i].BuyNo);
			QuickSetNumber(CenterCol + 1, pos, sym->Hoga.Ary[i].BuyQty);
			//QuickSetBackColor(CenterCol + 2, pos, RGB(255, 196, 196));
			//QuickSetBackColor(CenterCol + 1, pos, RGB(255, 196, 196));
			QuickSetBackColor(CenterCol + 2, pos, BuyColor[i]);
			QuickSetBackColor(CenterCol + 1, pos, BuyColor[i]);
			cellPos.Col = CenterCol + 2;
			cellPos.Row = pos;
			_HogaPos.insert(cellPos);
			//_CurPos.insert(cellPos);
			refreshMap[cellPos] = cellPos;
			cellPos.Col = CenterCol + 1;
			cellPos.Row = pos;
			_HogaPos.insert(cellPos);
			refreshMap[cellPos] = cellPos;
			//_CurPos.insert(cellPos);
		}
		//diff = VtSymbolManager::GetTickDiff(sym->Quote.intClose, sym->Hoga.Ary[i].IntSellPrice, sym);
		//pos = _CloseRow + diff;
		pos = FindRowFromCenterValue(sym->Hoga.Ary[i].IntSellPrice);
		if (i == 0)
		{
			_CloseLineRow = pos;
		}
		if (pos >= _StartRowForValue && pos < _EndRowForValue)
		{
			QuickSetNumber(CenterCol - 2, pos, sym->Hoga.Ary[i].SellNo);
			QuickSetNumber(CenterCol - 1, pos, sym->Hoga.Ary[i].SellQty);
			//QuickSetBackColor(CenterCol - 2, pos, RGB(196, 196, 255));
			//QuickSetBackColor(CenterCol - 1, pos, RGB(196, 196, 255));
			QuickSetBackColor(CenterCol - 2, pos, SellColor[i]);
			QuickSetBackColor(CenterCol - 1, pos, SellColor[i]);
			cellPos.Col = CenterCol - 2;
			cellPos.Row = pos;
			_HogaPos.insert(cellPos);
			refreshMap[cellPos] = cellPos;
			//_CurPos.insert(cellPos);
			cellPos.Col = CenterCol - 1;
			cellPos.Row = pos;
			_HogaPos.insert(cellPos);
			refreshMap[cellPos] = cellPos;
			//_CurPos.insert(cellPos);
		}
	}
}

void VtOrderPanelGrid::ShowHogaInfo()
{
// 	HogaPosUnion.clear();
// 	std::set_union(_HogaPos.begin(), _HogaPos.end(), _OldHogaPos.begin(), _OldHogaPos.end(), std::inserter(HogaPosUnion, HogaPosUnion.begin()));
// 	for (auto it = HogaPosUnion.begin(); it != HogaPosUnion.end(); ++it)
// 	{
// 		VtCellPos pos = *it;
// 		QuickRedrawCell(pos.Col, pos.Row);
// 	}
// 
// 	_OldHogaPos.clear();
// 	for (auto it = _HogaPos.begin(); it != _HogaPos.end(); ++it)
// 	{
// 		VtCellPos pos = *it;
// 		_OldHogaPos.insert(pos);
// 	}

	//ShowTotalHoga();
}

void VtOrderPanelGrid::ShowHogaInfo(CellPosMap& refreshMap)
{
// 	RefreshCells(refreshMap);
// 	_OldHogaPos.clear();
// 	for (auto it = _HogaPos.begin(); it != _HogaPos.end(); ++it)
// 	{
// 		_OldHogaPos.insert(*it);
// 	}
}

void VtOrderPanelGrid::RefreshCells(CellPosMap& refreshMap)
{
	for (auto it = refreshMap.begin(); it != refreshMap.end(); ++it)
	{
		QuickRedrawCell(it->second.Col, it->second.Row);
	}
}

void VtOrderPanelGrid::ShowCenterValue(double close, int closeRow)
{
	
}

void VtOrderPanelGrid::SetCenterValue()
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;
	VtSymbol* sym = _CenterWnd->Symbol();
	
	std::string code = sym->ShortCode.substr(0, 1);
	if ((code.compare(_T("2")) == 0) || (code.compare(_T("3")) == 0)) {
		ShowCenterValueForOption();
		return;
	}
	
	CUGCell cell;
	// Set the close value on the center of the center grid
	int startValue = sym->Quote.intClose + (sym->intTickSize * (_CloseRow - _StartRowForValue));
	
	RowToValueMap.clear();
	for (int i = _StartRowForValue; i < _EndRowForValue; i++) {
		GetCell(CenterCol, i, &cell);
		int curValue = startValue - sym->intTickSize * (i - 1);
		//cell.SetNumberDecimals(sym->IntDecimal);
		//cell.SetNumber(curValue / std::pow(10, sym->IntDecimal));
		//std::string strVal = NumberFormatter::format(curValue / std::pow(10, sym->IntDecimal), sym->IntDecimal);
		std::string strVal = fmt::format("{:.{}f}", curValue / std::pow(10, sym->Decimal), sym->Decimal);
		cell.SetText(strVal.c_str());
		cell.LongValue(curValue);
		SetCell(CenterCol, i, &cell);
		//if (i < rangeStart && i > rangeEnd)
		//	QuickRedrawCell(CenterCol, i);
		RowToValueMap[curValue] = i;

		VtCellPos cellPos;
		cellPos.Col = CenterCol;
		cellPos.Row = i;
		//_CurPos.insert(cellPos);
	}
}

void VtOrderPanelGrid::SetCenterValue(CellPosMap& refreshMap)
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;
	VtSymbol* sym = _CenterWnd->Symbol();

	std::string code = sym->ShortCode.substr(0, 1);
	if ((code.compare(_T("2")) == 0) || (code.compare(_T("3")) == 0)) {
		ShowCenterValueForOption(refreshMap);
		return;
	}

	CUGCell cell;
	// Set the close value on the center of the center grid
	int startValue = sym->Quote.intClose + (sym->intTickSize * (_CloseRow - _StartRowForValue));

	RowToValueMap.clear();
	for (int i = _StartRowForValue; i < _EndRowForValue; i++) {
		GetCell(CenterCol, i, &cell);
		int curValue = startValue - sym->intTickSize * (i - 1);
		std::string strVal = fmt::format("{:.{}f}", curValue / std::pow(10, sym->Decimal), sym->Decimal);
		CString thVal = XFormatNumber(strVal.c_str(), sym->Decimal);
		cell.SetText(thVal);
		cell.LongValue(curValue);
		SetCell(CenterCol, i, &cell);
		RowToValueMap[curValue] = i;

		VtCellPos cellPos;
		cellPos.Col = CenterCol;
		cellPos.Row = i;
		refreshMap[cellPos] = cellPos;
	}
}

void VtOrderPanelGrid::SetUpperCtrls()
{
	CUGCell cell;
	GetCell(CenterCol, -1, &cell);
	cell.SetText("정렬");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 128));
	cell.SetBackColor(GetSysColor(COLOR_BTNFACE));
	cell.SetParam(BUTTON_SET_CENTER);
	cell.SetBorderColor(RGB(0, 0, 0));
	SetCell(CenterCol, -1, &cell);


	GetCell(CenterCol, 0, &cell);
	cell.SetText("시장가");
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	cell.SetBackColor(GetSysColor(COLOR_BTNFACE));
	SetCell(CenterCol, 0, &cell);


	JoinCells(2, 0, 3, 0);
	GetCell(2, 0, &cell);
	cell.SetText("시장가매도");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	cell.SetBackColor(RGB(0, 67, 178));
	cell.SetParam(BUTTON_SELL_ATMARKET);
	SetCell(2, 0, &cell);
	JoinCells(5, 0, 6, 0);
	GetCell(5, 0, &cell);
	cell.SetText("시장가매수");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	cell.SetBackColor(RGB(255, 83, 75));
	cell.SetParam(BUTTON_BUY_ATMARKET);
	SetCell(5, 0, &cell);

	QuickSetBackColor(CenterCol - 1, 0, RGB(218, 226, 245));
	QuickSetBackColor(CenterCol - 2, 0, RGB(218, 226, 245));
	QuickSetBackColor(CenterCol - 4, 0, RGB(218, 226, 245));
	QuickSetBackColor(CenterCol + 1, 0, RGB(252, 226, 228));
	QuickSetBackColor(CenterCol + 2, 0, RGB(252, 226, 228));
	QuickSetBackColor(CenterCol + 4, 0, RGB(252, 226, 228));
}

void VtOrderPanelGrid::SetLowerCtrls()
{
	CUGCell cell;

	JoinCells(CenterCol - 2, _EndRowForValue + 1, CenterCol - 1, _EndRowForValue + 1);
	JoinCells(CenterCol + 1, _EndRowForValue + 1, CenterCol + 2, _EndRowForValue + 1);

	GetCell(CenterCol - 4, _EndRowForValue + 1, &cell);
	cell.SetText("ST취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	cell.SetBackColor(RGB(218, 226, 245));
	cell.SetParam(BUTTON_CANCEL_SELL_ALLSTOP);
	SetCell(CenterCol - 4, _EndRowForValue + 1, &cell);

	GetCell(CenterCol - 3, _EndRowForValue + 1, &cell);
	cell.SetText("일괄취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	cell.SetBackColor(RGB(218, 226, 245));
	cell.SetParam(BUTTON_CANCEL_SELL_ALL_ACCEPTED);
	SetCell(CenterCol - 3, _EndRowForValue + 1, &cell);

	GetCell(CenterCol + 4, _EndRowForValue + 1, &cell);
	cell.SetText("ST취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	cell.SetBackColor(RGB(252, 226, 228));
	cell.SetParam(BUTTON_CANCEL_BUY_ALLSTOP);
	SetCell(CenterCol + 4, _EndRowForValue + 1, &cell);

	GetCell(CenterCol + 3, _EndRowForValue + 1, &cell);
	cell.SetText("일괄취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	cell.SetBackColor(RGB(252, 226, 228));
	cell.SetParam(BUTTON_CANCEL_BUY_ALL_ACCEPTED);
	SetCell(CenterCol + 3, _EndRowForValue + 1, &cell);

	GetCell(CenterCol, _EndRowForValue + 1, &cell);
	cell.SetText("전체취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	cell.SetBackColor(RGB(218, 226, 245));
	cell.SetParam(BUTTON_CANCEL_ALL_ACCEPTED);
	SetCell(CenterCol, _EndRowForValue + 1, &cell);
}

void VtOrderPanelGrid::SetLowerCtrls(CellPosMap& refreshMap)
{
	VtCellPos pos;
	CUGCell cell;

	JoinCells(CenterCol - 2, _EndRowForValue + 1, CenterCol - 1, _EndRowForValue + 1);
	JoinCells(CenterCol + 1, _EndRowForValue + 1, CenterCol + 2, _EndRowForValue + 1);

	GetCell(CenterCol - 4, _EndRowForValue + 1, &cell);
	cell.SetText("ST취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	cell.SetBackColor(RGB(218, 226, 245));
	cell.SetParam(BUTTON_CANCEL_SELL_ALLSTOP);
	SetCell(CenterCol - 4, _EndRowForValue + 1, &cell);
	pos.Col = CenterCol - 4;
	pos.Row = _EndRowForValue + 1;
	refreshMap[pos] = pos;

	GetCell(CenterCol - 3, _EndRowForValue + 1, &cell);
	cell.SetText("일괄취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	cell.SetBackColor(RGB(218, 226, 245));
	cell.SetParam(BUTTON_CANCEL_SELL_ALL_ACCEPTED);
	SetCell(CenterCol - 3, _EndRowForValue + 1, &cell);
	pos.Col = CenterCol - 3;
	pos.Row = _EndRowForValue + 1;
	refreshMap[pos] = pos;

	GetCell(CenterCol + 4, _EndRowForValue + 1, &cell);
	cell.SetText("ST취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	cell.SetBackColor(RGB(252, 226, 228));
	cell.SetParam(BUTTON_CANCEL_BUY_ALLSTOP);
	SetCell(CenterCol + 4, _EndRowForValue + 1, &cell);
	pos.Col = CenterCol + 4;
	pos.Row = _EndRowForValue + 1;
	refreshMap[pos] = pos;

	GetCell(CenterCol + 3, _EndRowForValue + 1, &cell);
	cell.SetText("일괄취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	cell.SetBackColor(RGB(252, 226, 228));
	cell.SetParam(BUTTON_CANCEL_BUY_ALL_ACCEPTED);
	SetCell(CenterCol + 3, _EndRowForValue + 1, &cell);
	pos.Col = CenterCol + 3;
	pos.Row = _EndRowForValue + 1;
	refreshMap[pos] = pos;

	GetCell(CenterCol, _EndRowForValue + 1, &cell);
	cell.SetText("전체취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	cell.SetBackColor(RGB(218, 226, 245));
	cell.SetParam(BUTTON_CANCEL_ALL_ACCEPTED);
	SetCell(CenterCol, _EndRowForValue + 1, &cell);
	pos.Col = CenterCol;
	pos.Row = _EndRowForValue + 1;
	refreshMap[pos] = pos;
}

void VtOrderPanelGrid::ShowLowerCtrls()
{
	VtCellPos cellPos;
	_LowerCtrlPos.clear();

	CUGCell cell;

	JoinCells(CenterCol - 2, _EndRowForValue + 1, CenterCol - 1, _EndRowForValue + 1);
	QuickSetBackColor(CenterCol - 2, _EndRowForValue + 1, RGB(255, 255, 255));
	QuickRedrawCell(CenterCol - 2, _EndRowForValue + 1);
	JoinCells(CenterCol + 1, _EndRowForValue + 1, CenterCol + 2, _EndRowForValue + 1);
	QuickSetBackColor(CenterCol + 1, _EndRowForValue + 1, RGB(255, 255, 255));
	QuickRedrawCell(CenterCol + 1, _EndRowForValue + 1);
	
	GetCell(CenterCol - 4, _EndRowForValue + 1, &cell);
	cell.SetText("ST취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	//cell.SetBackColor(RGB(218, 226, 245));
	cell.SetBackColor(RGB(200, 200, 245));
	cell.SetParam(BUTTON_CANCEL_SELL_ALLSTOP);
	SetCell(CenterCol - 4, _EndRowForValue + 1, &cell);
	QuickRedrawCell(CenterCol - 4, _EndRowForValue + 1);
	cellPos.Col = CenterCol - 4;
	cellPos.Row = _EndRowForValue + 1;
	_LowerCtrlPos.insert(cellPos);

	GetCell(CenterCol - 3, _EndRowForValue + 1, &cell);
	cell.SetText("일괄취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	//cell.SetBackColor(RGB(218, 226, 245));
	cell.SetBackColor(RGB(200, 200, 245));
	cell.SetParam(BUTTON_CANCEL_SELL_ALL_ACCEPTED);
	SetCell(CenterCol - 3, _EndRowForValue + 1, &cell);
	QuickRedrawCell(CenterCol - 3, _EndRowForValue + 1);
	cellPos.Col = CenterCol - 3;
	cellPos.Row = _EndRowForValue + 1;
	_LowerCtrlPos.insert(cellPos);

	GetCell(CenterCol + 4, _EndRowForValue + 1, &cell);
	cell.SetText("ST취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	//cell.SetBackColor(RGB(252, 226, 228));
	cell.SetBackColor(RGB(252, 200, 200));
	cell.SetParam(BUTTON_CANCEL_BUY_ALLSTOP);
	SetCell(CenterCol + 4, _EndRowForValue + 1, &cell);
	QuickRedrawCell(CenterCol + 4, _EndRowForValue + 1);
	cellPos.Col = CenterCol + 4;
	cellPos.Row = _EndRowForValue + 1;
	_LowerCtrlPos.insert(cellPos);

	GetCell(CenterCol + 3, _EndRowForValue + 1, &cell);
	cell.SetText("일괄취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	//cell.SetBackColor(RGB(252, 226, 228));
	cell.SetBackColor(RGB(252, 200, 200));
	cell.SetParam(BUTTON_CANCEL_BUY_ALL_ACCEPTED);
	SetCell(CenterCol + 3, _EndRowForValue + 1, &cell);
	QuickRedrawCell(CenterCol + 3, _EndRowForValue + 1);
	cellPos.Col = CenterCol + 3;
	cellPos.Row = _EndRowForValue + 1;
	_LowerCtrlPos.insert(cellPos);

	GetCell(CenterCol, _EndRowForValue + 1, &cell);
	cell.SetText("전체취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	cell.SetBackColor(RGB(218, 226, 245));
	cell.SetParam(BUTTON_CANCEL_ALL_ACCEPTED);
	SetCell(CenterCol, _EndRowForValue + 1, &cell);
	QuickRedrawCell(CenterCol, _EndRowForValue + 1);

	cellPos.Col = CenterCol;
	cellPos.Row = _EndRowForValue + 1;
	_LowerCtrlPos.insert(cellPos);
}

void VtOrderPanelGrid::ShowLowerCtrls(bool include)
{
	VtCellPos cellPos;
	_LowerCtrlPos.clear();

	CUGCell cell;

	JoinCells(CenterCol - 2, _EndRowForValue + 1, CenterCol - 1, _EndRowForValue + 1);
	QuickSetBackColor(CenterCol - 2, _EndRowForValue + 1, RGB(255, 255, 255));
	cellPos.Col = CenterCol - 2;
	cellPos.Row = _EndRowForValue + 1;
// 	if (include)
// 		_CurPos.insert(cellPos);
// 	else
		QuickRedrawCell(CenterCol - 2, _EndRowForValue + 1);
	JoinCells(CenterCol + 1, _EndRowForValue + 1, CenterCol + 2, _EndRowForValue + 1);
	QuickSetBackColor(CenterCol + 1, _EndRowForValue + 1, RGB(255, 255, 255));
	cellPos.Col = CenterCol + 1;
	cellPos.Row = _EndRowForValue + 1;
// 	if (include)
// 		_CurPos.insert(cellPos);
// 	else
		QuickRedrawCell(CenterCol + 1, _EndRowForValue + 1);

	GetCell(CenterCol - 4, _EndRowForValue + 1, &cell);
	cell.SetText("ST취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	cell.SetBackColor(RGB(218, 226, 245));
	cell.SetParam(BUTTON_CANCEL_SELL_ALLSTOP);
	SetCell(CenterCol - 4, _EndRowForValue + 1, &cell);
	cellPos.Col = CenterCol - 4;
	cellPos.Row = _EndRowForValue + 1;
	_LowerCtrlPos.insert(cellPos);
// 	if (include)
// 		_CurPos.insert(cellPos);
// 	else
		QuickRedrawCell(CenterCol - 4, _EndRowForValue + 1);

	GetCell(CenterCol - 3, _EndRowForValue + 1, &cell);
	cell.SetText("일괄취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	cell.SetBackColor(RGB(218, 226, 245));
	cell.SetParam(BUTTON_CANCEL_SELL_ALL_ACCEPTED);
	SetCell(CenterCol - 3, _EndRowForValue + 1, &cell);
	cellPos.Col = CenterCol - 3;
	cellPos.Row = _EndRowForValue + 1;
	_LowerCtrlPos.insert(cellPos);
// 	if (include)
// 		_CurPos.insert(cellPos);
// 	else
		QuickRedrawCell(CenterCol - 3, _EndRowForValue + 1);


	GetCell(CenterCol + 4, _EndRowForValue + 1, &cell);
	cell.SetText("ST취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	cell.SetBackColor(RGB(252, 226, 228));
	cell.SetParam(BUTTON_CANCEL_BUY_ALLSTOP);
	SetCell(CenterCol + 4, _EndRowForValue + 1, &cell);
	cellPos.Col = CenterCol + 4;
	cellPos.Row = _EndRowForValue + 1;
	_LowerCtrlPos.insert(cellPos);
// 	if (include)
// 		_CurPos.insert(cellPos);
// 	else
		QuickRedrawCell(CenterCol + 4, _EndRowForValue + 1);


	GetCell(CenterCol + 3, _EndRowForValue + 1, &cell);
	cell.SetText("일괄취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	cell.SetBackColor(RGB(252, 226, 228));
	cell.SetParam(BUTTON_CANCEL_BUY_ALL_ACCEPTED);
	SetCell(CenterCol + 3, _EndRowForValue + 1, &cell);
	cellPos.Col = CenterCol + 3;
	cellPos.Row = _EndRowForValue + 1;
	_LowerCtrlPos.insert(cellPos);
// 	if (include)
// 		_CurPos.insert(cellPos);
// 	else
		QuickRedrawCell(CenterCol + 3, _EndRowForValue + 1);


	GetCell(CenterCol, _EndRowForValue + 1, &cell);
	cell.SetText("전체취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	cell.SetBackColor(RGB(218, 226, 245));
	cell.SetParam(BUTTON_CANCEL_ALL_ACCEPTED);
	SetCell(CenterCol, _EndRowForValue + 1, &cell);

	cellPos.Col = CenterCol;
	cellPos.Row = _EndRowForValue + 1;
	_LowerCtrlPos.insert(cellPos);
// 	if (include)
// 		_CurPos.insert(cellPos);
// 	else
		QuickRedrawCell(CenterCol, _EndRowForValue + 1);

}

void VtOrderPanelGrid::ShowLowerCtrls(CellPosMap& refreshMap)
{
	VtCellPos cellPos;
	_LowerCtrlPos.clear();

	CUGCell cell;

	JoinCells(CenterCol - 2, _EndRowForValue + 1, CenterCol - 1, _EndRowForValue + 1);
	QuickSetBackColor(CenterCol - 2, _EndRowForValue + 1, RGB(255, 255, 255));
	cellPos.Col = CenterCol - 2;
	cellPos.Row = _EndRowForValue + 1;
	refreshMap[cellPos] = cellPos;
	JoinCells(CenterCol + 1, _EndRowForValue + 1, CenterCol + 2, _EndRowForValue + 1);
	QuickSetBackColor(CenterCol + 1, _EndRowForValue + 1, RGB(255, 255, 255));
	cellPos.Col = CenterCol + 1;
	cellPos.Row = _EndRowForValue + 1;
	refreshMap[cellPos] = cellPos;

	GetCell(CenterCol - 4, _EndRowForValue + 1, &cell);
	cell.SetText("ST취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	//cell.SetBackColor(RGB(218, 226, 245));
	cell.SetBackColor(RGB(190, 190, 245));
	cell.SetParam(BUTTON_CANCEL_SELL_ALLSTOP);
	SetCell(CenterCol - 4, _EndRowForValue + 1, &cell);
	cellPos.Col = CenterCol - 4;
	cellPos.Row = _EndRowForValue + 1;
	_LowerCtrlPos.insert(cellPos);
	refreshMap[cellPos] = cellPos;

	GetCell(CenterCol - 3, _EndRowForValue + 1, &cell);
	cell.SetText("일괄취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	//cell.SetBackColor(RGB(218, 226, 245));
	cell.SetBackColor(RGB(190, 190, 245));
	cell.SetParam(BUTTON_CANCEL_SELL_ALL_ACCEPTED);
	SetCell(CenterCol - 3, _EndRowForValue + 1, &cell);
	cellPos.Col = CenterCol - 3;
	cellPos.Row = _EndRowForValue + 1;
	_LowerCtrlPos.insert(cellPos);
	refreshMap[cellPos] = cellPos;


	GetCell(CenterCol + 4, _EndRowForValue + 1, &cell);
	cell.SetText("ST취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	//cell.SetBackColor(RGB(252, 226, 228));
	cell.SetBackColor(RGB(252, 190, 190));
	cell.SetParam(BUTTON_CANCEL_BUY_ALLSTOP);
	SetCell(CenterCol + 4, _EndRowForValue + 1, &cell);
	cellPos.Col = CenterCol + 4;
	cellPos.Row = _EndRowForValue + 1;
	_LowerCtrlPos.insert(cellPos);
	refreshMap[cellPos] = cellPos;


	GetCell(CenterCol + 3, _EndRowForValue + 1, &cell);
	cell.SetText("일괄취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	//cell.SetBackColor(RGB(252, 226, 228));
	cell.SetBackColor(RGB(252, 190, 190));
	cell.SetParam(BUTTON_CANCEL_BUY_ALL_ACCEPTED);
	SetCell(CenterCol + 3, _EndRowForValue + 1, &cell);
	cellPos.Col = CenterCol + 3;
	cellPos.Row = _EndRowForValue + 1;
	_LowerCtrlPos.insert(cellPos);
	refreshMap[cellPos] = cellPos;


	GetCell(CenterCol, _EndRowForValue + 1, &cell);
	cell.SetText("전체취소");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	//cell.SetBackColor(RGB(218, 226, 245));
	cell.SetBackColor(RGB(190, 190, 245));
	cell.SetParam(BUTTON_CANCEL_ALL_ACCEPTED);
	SetCell(CenterCol, _EndRowForValue + 1, &cell);

	cellPos.Col = CenterCol;
	cellPos.Row = _EndRowForValue + 1;
	_LowerCtrlPos.insert(cellPos);
	refreshMap[cellPos] = cellPos;
}

void VtOrderPanelGrid::ShowUpperCtrls()
{
	if (!_CenterWnd)
		return;

	CUGCell cell;
	GetCell(CenterCol, -1, &cell);
	cell.SetText("정렬");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 128));
	cell.SetBackColor(GetSysColor(COLOR_BTNFACE));
	cell.SetParam(BUTTON_SET_CENTER);
	cell.SetBorderColor(RGB(0, 0, 0));
	SetCell(CenterCol, -1, &cell);
	QuickRedrawCell(CenterCol, -1);


	GetCell(CenterCol, 0, &cell);
	cell.SetText("시장가");
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	cell.SetBackColor(GetSysColor(COLOR_BTNFACE));
	SetCell(CenterCol, 0, &cell);
	QuickRedrawCell(CenterCol, 0);


	JoinCells(2, 0, 3, 0);
	GetCell(2, 0, &cell);
	if (_CenterWnd->ShowOrderCountArea()) {
		cell.SetText("시장가매도");
	} else {
		cell.SetText("시장가");
	}
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(255, 255, 255));
	cell.SetBackColor(RGB(19, 137, 255));
	cell.SetParam(BUTTON_SELL_ATMARKET);
	SetCell(2, 0, &cell);
	QuickRedrawCell(2, 0);

	JoinCells(5, 0, 6, 0);
	GetCell(5, 0, &cell);
	//cell.SetText("시장가매수");
	if (_CenterWnd->ShowOrderCountArea()) {
		cell.SetText("시장가매수");
	}
	else {
		cell.SetText("시장가");
	}
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(255, 255, 255));
	cell.SetBackColor(RGB(240, 51, 58));
	cell.SetParam(BUTTON_BUY_ATMARKET);
	SetCell(5, 0, &cell);
	QuickRedrawCell(5, 0);
}

void VtOrderPanelGrid::ShowUpperCtrls(CellPosMap& refreshMap)
{
	VtCellPos pos;
	CUGCell cell;
	GetCell(CenterCol, -1, &cell);
	cell.SetText("정렬");
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 128));
	cell.SetBackColor(GetSysColor(COLOR_BTNFACE));
	cell.SetParam(BUTTON_SET_CENTER);
	cell.SetBorderColor(RGB(0, 0, 0));
	SetCell(CenterCol, -1, &cell);
	//QuickRedrawCell(CenterCol, -1);
	pos.Col = CenterCol;
	pos.Row = -1;
	refreshMap[pos] = pos;


	GetCell(CenterCol, 0, &cell);
	cell.SetText("시장가");
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(0, 0, 0));
	cell.SetBackColor(GetSysColor(COLOR_BTNFACE));
	SetCell(CenterCol, 0, &cell);
	//QuickRedrawCell(CenterCol, 0);
	pos.Col = CenterCol;
	pos.Row = 0;
	refreshMap[pos] = pos;

	JoinCells(2, 0, 3, 0);
	GetCell(2, 0, &cell);
	//cell.SetText("시장가매도");
	if (_CenterWnd->ShowOrderCountArea()) {
		cell.SetText("시장가매도");
	}
	else {
		cell.SetText("시장가");
	}
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(255, 255, 255));
	cell.SetBackColor(RGB(19, 137, 255));
	cell.SetParam(BUTTON_SELL_ATMARKET);
	SetCell(2, 0, &cell);
	//QuickRedrawCell(2, 0);
	pos.Col = 2;
	pos.Row = 0;
	refreshMap[pos] = pos;

	JoinCells(5, 0, 6, 0);
	GetCell(5, 0, &cell);
	//cell.SetText("시장가매수");
	if (_CenterWnd->ShowOrderCountArea()) {
		cell.SetText("시장가매수");
	}
	else {
		cell.SetText("시장가");
	}
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	cell.SetFont(&m_medBoldFont);
	cell.SetTextColor(RGB(255, 255, 255));
	cell.SetBackColor(RGB(240, 51, 58));
	cell.SetParam(BUTTON_BUY_ATMARKET);
	SetCell(5, 0, &cell);
	//QuickRedrawCell(5, 0);
	pos.Col = 5;
	pos.Row = 0;
	refreshMap[pos] = pos;
}

void VtOrderPanelGrid::SetTotalHoga()
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;

	VtSymbol* sym = _CenterWnd->Symbol();

	CUGCell cell;
	GetCell(CenterCol - 1, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	SetCell(CenterCol - 1, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol - 1, _EndRowForValue, sym->Hoga.TotSellQty);
	QuickRedrawCell(CenterCol - 1, _EndRowForValue);

	GetCell(CenterCol - 2, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	SetCell(CenterCol - 2, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol - 2, _EndRowForValue, sym->Hoga.TotSellNo);
	QuickRedrawCell(CenterCol - 2, _EndRowForValue);

	GetCell(CenterCol + 1, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	SetCell(CenterCol + 1, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol + 1, _EndRowForValue, sym->Hoga.TotBuyQty);
	QuickRedrawCell(CenterCol + 1, _EndRowForValue);

	GetCell(CenterCol + 2, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	SetCell(CenterCol + 2, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol + 2, _EndRowForValue, sym->Hoga.TotBuyNo);
	QuickRedrawCell(CenterCol + 2, _EndRowForValue);

	int dif = sym->Hoga.TotBuyQty - sym->Hoga.TotSellQty;

	GetCell(CenterCol, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	SetCell(CenterCol, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol, _EndRowForValue, dif);
	if (dif > 0)
	{
		QuickSetTextColor(CenterCol, _EndRowForValue, RGB(255, 0, 0));
	}
	else
	{
		QuickSetTextColor(CenterCol, _EndRowForValue, RGB(0, 0, 255));
	}
}

void VtOrderPanelGrid::AddOrderToCell(VtOrder* order)
{
	if (!order)
		return;

	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;
	VtSymbol* sym = _CenterWnd->Symbol();

	CUGCell cell;
	VtCellPos pos;

	int row = FindRowFromCenterValue(order->intOrderPrice);
	if (row >= _StartRowForValue && row <= _EndRowForValue)
	{
		if (order->orderPosition == VtPositionType::Sell)
		{
			pos.Col = CenterCol - 3;
			GetCell(CenterCol - 3, row, &cell);
			cell.OrderType(2);
			cell.OrderMap[order->orderNo] = order;
			//cell.OrderCount(cell.OrderMap.size());
			cell.SetCellType(m_OrderCellTypeIndex);
			SetCell(CenterCol - 3, row, &cell);
		}
		else
		{
			pos.Col = CenterCol + 3;
			GetCell(CenterCol + 3, row, &cell);
			cell.OrderType(1);
			cell.OrderMap[order->orderNo] = order;
			//cell.OrderCount(cell.OrderMap.size());
			cell.SetCellType(m_OrderCellTypeIndex);
			SetCell(CenterCol + 3, row, &cell);
		}
		pos.Row = row;

		int cellOrderCnt = 0;
		for (auto it = cell.OrderMap.begin(); it != cell.OrderMap.end(); ++it)
			cellOrderCnt += it->second->amount;

		QuickSetNumber(pos.Col, pos.Row, cellOrderCnt);
		_OrderPos.insert(pos);
		QuickRedrawCell(pos.Col, pos.Row);
	}
}

void VtOrderPanelGrid::AddOrderToCell(VtOrder* order, bool include)
{
	if (!order)
		return;

	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;
	VtSymbol* sym = _CenterWnd->Symbol();

	CUGCell cell;
	VtCellPos pos;

	int row = FindRowFromCenterValue(order->intOrderPrice);
	if (row >= _StartRowForValue && row <= _EndRowForValue)
	{
		if (order->orderPosition == VtPositionType::Sell)
		{
			pos.Col = CenterCol - 3;
			GetCell(CenterCol - 3, row, &cell);
			cell.OrderType(2);
			//cell.OrderList.push_back(order);
			cell.OrderMap[order->orderNo] = order;
			//cell.OrderCount(cell.OrderMap.size());
			cell.SetCellType(m_OrderCellTypeIndex);
			SetCell(CenterCol - 3, row, &cell);
		}
		else
		{
			pos.Col = CenterCol + 3;
			GetCell(CenterCol + 3, row, &cell);
			cell.OrderType(1);
			cell.OrderMap[order->orderNo] = order;
			//cell.OrderCount(cell.OrderMap.size());
			cell.SetCellType(m_OrderCellTypeIndex);
			SetCell(CenterCol + 3, row, &cell);
		}
		pos.Row = row;

		int cellOrderCnt = 0;
		for (auto it = cell.OrderMap.begin(); it != cell.OrderMap.end(); ++it)
			cellOrderCnt += it->second->amount;

		QuickSetNumber(pos.Col, pos.Row, cellOrderCnt);
		_OrderPos.insert(pos);
// 		if (include)
// 			_CurPos.insert(pos);
// 		else
			QuickRedrawCell(pos.Col, pos.Row);
	}
}

void VtOrderPanelGrid::AddOrderToCell(CellPosMap& refreshMap, VtOrder* order)
{
	if (!order || !_CenterWnd || !_CenterWnd->Symbol())
		return;
	VtSymbol* sym = _CenterWnd->Symbol();

	CUGCell cell;
	VtCellPos pos;

	int row = FindRowFromCenterValue(order->intOrderPrice);
	if (row >= _StartRowForValue && row <= _EndRowForValue)
	{
		if (order->orderPosition == VtPositionType::Sell)
		{
			pos.Col = CenterCol - 3;
			GetCell(CenterCol - 3, row, &cell);
			cell.OrderType(2);
			cell.OrderMap[order->orderNo] = order;
			cell.SetCellType(m_OrderCellTypeIndex);
			SetCell(CenterCol - 3, row, &cell);
		}
		else if (order->orderPosition == VtPositionType::Buy)
		{
			pos.Col = CenterCol + 3;
			GetCell(CenterCol + 3, row, &cell);
			cell.OrderType(1);
			cell.OrderMap[order->orderNo] = order;
			cell.SetCellType(m_OrderCellTypeIndex);
			SetCell(CenterCol + 3, row, &cell);
		}
		pos.Row = row;

		int cellOrderCnt = 0;
		for (auto it = cell.OrderMap.begin(); it != cell.OrderMap.end(); ++it)
			cellOrderCnt += it->second->amount;

		QuickSetNumber(pos.Col, pos.Row, cellOrderCnt);
		_OrderPos.insert(pos);
		refreshMap[pos] = pos;
	}
}

VtCellPos VtOrderPanelGrid::FindOrderPos(VtOrder* order)
{
	VtCellPos pos;
	pos.Col = -2;
	pos.Row = -2;

	int row = FindRowFromCenterValue(order->intOrderPrice);
	if (order->orderPosition == VtPositionType::Sell) {
		pos.Col = CenterCol - 3;
	} else {
		pos.Col = CenterCol + 3;
	}
	pos.Row = row;
	return pos;
}

void VtOrderPanelGrid::RemoveOrderFromCell(int col, int row)
{
	CUGCell cell;
	GetCell(col, row, &cell);
	cell.OrderType(0);
	cell.OrderList.clear();
	cell.OrderMap.clear();
	cell.OrderCount(0);
	cell.SetCellType(UGCT_NORMAL);
	cell.SetText(_T(""));
	SetCell(col, row, &cell);
}

void VtOrderPanelGrid::ShowUnionCells()
{
// 	_UnionPos.clear();
// 	std::set_union(_CurPos.begin(), _CurPos.end(), _OldCellPos.begin(), _OldCellPos.end(), std::inserter(_UnionPos, _UnionPos.begin()));
// 	for (auto it = _UnionPos.begin(); it != _UnionPos.end(); ++it)
// 	{
// 		VtCellPos pos = *it;
// 		QuickRedrawCell(pos.Col, pos.Row);
// 	}
}

void VtOrderPanelGrid::ClearTotalHoga()
{
	for (auto it = _TotalHogaPos.begin(); it != _TotalHogaPos.end(); ++it)
	{
		VtCellPos pos = *it;
		int col = pos.Col;
		int row = pos.Row;
		QuickSetText(col, row, _T(""));
		QuickSetFont(col, row, &_defFont);
		QuickSetCellType(col, row, UGCT_NORMAL);
		QuickSetBackColor(col, row, RGB(255, 255, 255));
		QuickSetTextColor(col, row, RGB(0, 0, 0));
		QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);

		//_OldCellPos.insert(pos);
	}
}

void VtOrderPanelGrid::ClearTotalHoga(CellPosMap& refreshMap)
{
	for (auto it = _TotalHogaPos.begin(); it != _TotalHogaPos.end(); ++it)
	{
		VtCellPos pos = *it;
		int col = pos.Col;
		int row = pos.Row;
		QuickSetText(col, row, _T(""));
		QuickSetFont(col, row, &_defFont);
		QuickSetCellType(col, row, UGCT_NORMAL);
		QuickSetBackColor(col, row, RGB(255, 255, 255));
		QuickSetTextColor(col, row, RGB(0, 0, 0));
		QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		
		refreshMap[pos] = pos;
	}
}

void VtOrderPanelGrid::ClearTotalOrder()
{
	int col = 0, row = 0;
	col = CenterCol - 3;
	row = 0;
	QuickSetText(col, row, _T("0"));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNRIGHT | UG_ALIGNVCENTER);

	VtCellPos pos;
	pos.Col = col;
	pos.Row = row;
	//_OldCellPos.insert(pos);

	col = CenterCol + 3;
	row = 0;
	QuickSetText(col, row, _T("0"));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	pos.Col = col;
	pos.Row = row;
	//_OldCellPos.insert(pos);
}

void VtOrderPanelGrid::ClearTotalOrder(CellPosMap& refreshMap)
{
	int col = 0, row = 0;
	col = CenterCol - 3;
	row = 0;
	QuickSetText(col, row, _T("0"));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNRIGHT | UG_ALIGNVCENTER);

	VtCellPos pos;
	pos.Col = col;
	pos.Row = row;
	refreshMap[pos] = pos;

	col = CenterCol + 3;
	row = 0;
	QuickSetText(col, row, _T("0"));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetFont(col, row, &_defFont);
	QuickSetCellType(col, row, UGCT_NORMAL);
	QuickSetBackColor(col, row, RGB(255, 255, 255));
	QuickSetTextColor(col, row, RGB(0, 0, 0));
	QuickSetAlignment(col, row, UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	pos.Col = col;
	pos.Row = row;
	//_OldCellPos.insert(pos);
	refreshMap[pos] = pos;
}

void VtOrderPanelGrid::ClearLowerCtrls()
{
	int oldEndRow = _EndRowForValue + 1;
	for (auto it = _LowerCtrlPos.begin(); it != _LowerCtrlPos.end(); ++it)
	{
		VtCellPos pos = *it;
		int col = pos.Col;
		int row = pos.Row;
		oldEndRow = row;
		QuickSetText(col, row, _T(""));
		QuickSetTextColor(col, row, RGB(0, 0, 0));
		QuickSetFont(col, row, &_defFont);
		QuickSetCellType(col, row, UGCT_NORMAL);
		if (col == CenterCol - 3)
			QuickSetBackColor(col, row, RGB(218, 226, 245));
		else if (col == CenterCol + 3)
			QuickSetBackColor(col, row, RGB(252, 226, 228));
		else
			QuickSetBackColor(col, row, RGB(255, 255, 255));

		QuickSetTextColor(col, row, RGB(0, 0, 0));
		QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		//_OldCellPos.insert(pos);
	}

	VtCellPos cellPos;
	UnJoinCells(CenterCol - 2, oldEndRow);
	UnJoinCells(CenterCol + 1, oldEndRow);
	cellPos.Col = CenterCol - 1;
	cellPos.Row = oldEndRow;
	//_OldCellPos.insert(cellPos);

	cellPos.Col = CenterCol - 2;
	cellPos.Row = oldEndRow;
	//_OldCellPos.insert(cellPos);

	cellPos.Col = CenterCol + 1;
	cellPos.Row = oldEndRow;
	//_OldCellPos.insert(cellPos);

	cellPos.Col = CenterCol + 2;
	cellPos.Row = oldEndRow;
	//_OldCellPos.insert(cellPos);
}

void VtOrderPanelGrid::ClearLowerCtrls(CellPosMap& refreshMap)
{
	int oldEndRow = _EndRowForValue + 1;
	for (auto it = _LowerCtrlPos.begin(); it != _LowerCtrlPos.end(); ++it) {
		VtCellPos pos = *it;
		int col = pos.Col;
		int row = pos.Row;
		oldEndRow = row;
		QuickSetText(col, row, _T(""));
		QuickSetTextColor(col, row, RGB(0, 0, 0));
		QuickSetFont(col, row, &_defFont);
		QuickSetCellType(col, row, UGCT_NORMAL);
		if (col == CenterCol - 3)
			QuickSetBackColor(col, row, RGB(218, 226, 245));
		else if (col == CenterCol + 3)
			QuickSetBackColor(col, row, RGB(252, 226, 228));
		else
			QuickSetBackColor(col, row, RGB(255, 255, 255));

		QuickSetTextColor(col, row, RGB(0, 0, 0));
		QuickSetAlignment(col, row, UG_ALIGNCENTER | UG_ALIGNVCENTER);
		refreshMap[pos] = pos;
	}

	VtCellPos cellPos;
	UnJoinCells(CenterCol - 2, oldEndRow);
	UnJoinCells(CenterCol + 1, oldEndRow);
	cellPos.Col = CenterCol - 1;
	cellPos.Row = oldEndRow;
	refreshMap[cellPos] = cellPos;

	cellPos.Col = CenterCol - 2;
	cellPos.Row = oldEndRow;
	refreshMap[cellPos] = cellPos;

	cellPos.Col = CenterCol + 1;
	cellPos.Row = oldEndRow;
	refreshMap[cellPos] = cellPos;

	cellPos.Col = CenterCol + 2;
	cellPos.Row = oldEndRow;
	refreshMap[cellPos] = cellPos;
}

void VtOrderPanelGrid::InitButtonCol()
{
	_ButtonCol.clear();
	// 시장가 매도
	_ButtonCol[std::make_pair(2, 0)] = std::make_pair(RGB(255, 255, 255), RGB(19, 137, 255));
	_ButtonCol[std::make_pair(3, 0)] = std::make_pair(RGB(255, 255, 255), RGB(19, 137, 255));
	// 시장가 매도
	_ButtonCol[std::make_pair(5, 0)] = std::make_pair(RGB(255, 255, 255), RGB(240, 51, 58));
	// 매도 스탑 취소
	_ButtonCol[std::make_pair(CenterCol - 4, _EndRowForValue + 1)] = std::make_pair(RGB(0, 0, 0), RGB(190, 190, 245));
	// 매도 주문 취소
	_ButtonCol[std::make_pair(CenterCol - 3, _EndRowForValue + 1)] = std::make_pair(RGB(0, 0, 0), RGB(190, 190, 245));
	// 매수 스탑 취소
	_ButtonCol[std::make_pair(CenterCol + 4, _EndRowForValue + 1)] = std::make_pair(RGB(0, 0, 0), RGB(252, 190, 190));
	// 매수 주문 취소
	_ButtonCol[std::make_pair(CenterCol + 3, _EndRowForValue + 1)] = std::make_pair(RGB(0, 0, 0), RGB(252, 190, 190));
	// 전체 주문 취소
	_ButtonCol[std::make_pair(CenterCol, _EndRowForValue + 1)] = std::make_pair(RGB(0, 0, 0), RGB(190, 190, 245));
}

void VtOrderPanelGrid::ResetByCenterRow()
{
	int newCenter = FindCenterRow();
	SetCenterRow(newCenter);
	ChangeCenter(_CloseRow, newCenter);
}

void VtOrderPanelGrid::HitTestButtonCol(int col, int row)
{
	CUGCell cell;
	auto key = std::make_pair(col, row);
	auto it = _ButtonCol.find(key);
	if (it != _ButtonCol.end()) { // for found
		RestoreButtonCell();
		GetCell(col, row, &cell);
		cell.SetTextColor(RGB(0, 0, 0));
		cell.SetBackColor(VtGlobal::SelCellBackColor);
		SetCell(col, row, &cell);
		QuickRedrawCell(col, row);
		_OldBtnCol = col;
		_OldBtnRow = row;
	} else { // for not found
		// Restore the original text color and background color.
		RestoreButtonCell();
	}
	
}

void VtOrderPanelGrid::RestoreButtonCell()
{
	CUGCell cell;
	auto it = _ButtonCol.find(std::make_pair(_OldBtnCol, _OldBtnRow));
	if (it != _ButtonCol.end()) {
		GetCell(std::get<0>(it->first), std::get<1>(it->first), &cell);
		cell.SetTextColor(std::get<0>(it->second));
		cell.SetBackColor(std::get<1>(it->second));
		SetCell(std::get<0>(it->first), std::get<1>(it->first), &cell);
		QuickRedrawCell(std::get<0>(it->first), std::get<1>(it->first));
	}
}

void VtOrderPanelGrid::SetSelect(int col, int row)
{
	_SelectedCol = col;
	_SelectedRow = row;

	if (row >= _StartRowForValue && row <= _EndRowForValue) {
		CUGCell cell;
		GetCell(CenterCol, _SelectedRow, &cell);
		_SelectedVal = cell.LongValue();
	}
}

void VtOrderPanelGrid::ShowSelect(int col, int row)
{
	RedrawCell(_OldSelectedCol, _OldSelectedRow);
	if (row >= _StartRowForValue && row <= _EndRowForValue) {
		GetCellRect(col, row, &rcSelected);
		SetClickRect(&rcSelected);
		RedrawCell(col, row);
	}
	else {
		SetClickRect(nullptr);
		if (_OldSelectedRow != -1) {
			RedrawCell(_OldSelectedCol, _OldSelectedRow);
		}
	}

	_OldSelectedCol = _SelectedCol;
	_OldSelectedRow = _SelectedRow;
}

void VtOrderPanelGrid::RefreshStopOrder()
{
	CellPosMap refreshMap;
	ClearOldStopOrders(refreshMap);
	SetStopOrderInfo(refreshMap);
	RefreshCells(refreshMap);
}

void VtOrderPanelGrid::ShowTotolHogas(VtHoga* hoga)
{
	if (!hoga)
		return;

	CUGCell cell;
	GetCell(CenterCol - 1, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	SetCell(CenterCol - 1, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol - 1, _EndRowForValue, hoga->TotSellQty);
	QuickRedrawCell(CenterCol - 1, _EndRowForValue);

	GetCell(CenterCol - 2, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	SetCell(CenterCol - 2, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol - 2, _EndRowForValue, hoga->TotSellNo);
	QuickRedrawCell(CenterCol - 2, _EndRowForValue);

	GetCell(CenterCol + 1, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	SetCell(CenterCol + 1, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol + 1, _EndRowForValue, hoga->TotBuyQty);
	QuickRedrawCell(CenterCol + 1, _EndRowForValue);

	GetCell(CenterCol + 2, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	SetCell(CenterCol + 2, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol + 2, _EndRowForValue, hoga->TotBuyNo);
	QuickRedrawCell(CenterCol + 2, _EndRowForValue);

	int dif = hoga->TotBuyQty - hoga->TotSellQty;

	GetCell(CenterCol, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	SetCell(CenterCol, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol, _EndRowForValue, dif);
	if (dif > 0)
	{
		QuickSetTextColor(CenterCol, _EndRowForValue, RGB(255, 0, 0));
	}
	else 
	{
		QuickSetTextColor(CenterCol, _EndRowForValue, RGB(0, 0, 255));
	}

	QuickRedrawCell(CenterCol, _EndRowForValue);

	ShowTotalStopOrder();
}

void VtOrderPanelGrid::ShowTotalOrder()
{
	if (!_CenterWnd)
		return;

	//VtProductOrderManager* prdtOrderMgr = _CenterWnd->PrdtOrderMgr();

	std::vector<VtOrder*> acptOrderList = _OrderConfigMgr->OrderMgr()->GetAcceptedOrders(_CenterWnd->Symbol()->ShortCode);

	int buy = 0, sell = 0;
	for (auto it = acptOrderList.begin(); it != acptOrderList.end(); ++it)
	{
		VtOrder* order = *it;
		if (order->orderPosition == VtPositionType::Sell)
			sell += order->amount;
		else
			buy += order->amount;
	}

	QuickSetNumber(CenterCol - 3, 0, sell);
	CUGCell cell;
	GetCell(CenterCol - 3, 0, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	cell.SetBackColor(RGB(218, 226, 245));
	SetCell(CenterCol - 3, 0, &cell);
	//QuickRedrawCell(CenterCol - 3, 0);
	VtCellPos cellPos;
	cellPos.Col = CenterCol - 3;
	cellPos.Row = 0;
	_OrderPos.insert(cellPos);

	QuickSetNumber(CenterCol + 3, 0, buy);
	GetCell(CenterCol + 3, 0, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	cell.SetBackColor(RGB(252, 226, 228));
	SetCell(CenterCol + 3, 0, &cell);
	//QuickRedrawCell(CenterCol + 3, 0);
	cellPos.Col = CenterCol + 3;
	cellPos.Row = 0;
	_OrderPos.insert(cellPos);
}

void VtOrderPanelGrid::ShowTotalOrder(bool include)
{
	if (!_CenterWnd || !_OrderConfigMgr || !_OrderConfigMgr->OrderMgr() )
		return;

	//VtProductOrderManager* prdtOrderMgr = _CenterWnd->PrdtOrderMgr();

	std::vector<VtOrder*> acptOrderList = _OrderConfigMgr->OrderMgr()->GetAcceptedOrders(_CenterWnd->Symbol()->ShortCode);

	int buy = 0, sell = 0;
	for (auto it = acptOrderList.begin(); it != acptOrderList.end(); ++it)
	{
		VtOrder* order = *it;
		if (order->orderPosition == VtPositionType::Sell)
			sell += order->amount;
		else
			buy += order->amount;
	}

	QuickSetNumber(CenterCol - 3, 0, sell);
	CUGCell cell;
	GetCell(CenterCol - 3, 0, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	cell.SetBackColor(RGB(218, 226, 245));
	SetCell(CenterCol - 3, 0, &cell);
	VtCellPos cellPos;
	cellPos.Col = CenterCol - 3;
	cellPos.Row = 0;
// 	if (include)
// 		_CurPos.insert(cellPos);
// 	else
// 		QuickRedrawCell(CenterCol - 3, 0);

	_OrderPos.insert(cellPos);

	QuickSetNumber(CenterCol + 3, 0, buy);
	GetCell(CenterCol + 3, 0, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	cell.SetBackColor(RGB(252, 226, 228));
	SetCell(CenterCol + 3, 0, &cell);
	
	cellPos.Col = CenterCol + 3;
	cellPos.Row = 0;
// 	if (include)
// 		_CurPos.insert(cellPos);
// 	else
// 		QuickRedrawCell(CenterCol + 3, 0);

	_OrderPos.insert(cellPos);
}

void VtOrderPanelGrid::ShowTotalOrder(CellPosMap& refreshMap)
{
	if (!_CenterWnd || !_OrderConfigMgr->OrderMgr())
		return;

	std::vector<VtOrder*> acptOrderList = _OrderConfigMgr->OrderMgr()->GetAcceptedOrders(_CenterWnd->Symbol()->ShortCode);

	int buy = 0, sell = 0;
	for (auto it = acptOrderList.begin(); it != acptOrderList.end(); ++it) {
		VtOrder* order = *it;
		if (order->orderPosition == VtPositionType::Sell)
			sell += order->amount;
		else
			buy += order->amount;
	}

	QuickSetNumber(CenterCol - 3, 0, sell);
	CUGCell cell;
	GetCell(CenterCol - 3, 0, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	cell.SetBackColor(RGB(218, 226, 245));
	SetCell(CenterCol - 3, 0, &cell);
	VtCellPos cellPos;
	cellPos.Col = CenterCol - 3;
	cellPos.Row = 0;
	refreshMap[cellPos] = cellPos;

	QuickSetNumber(CenterCol + 3, 0, buy);
	GetCell(CenterCol + 3, 0, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	cell.SetBackColor(RGB(252, 226, 228));
	SetCell(CenterCol + 3, 0, &cell);
	cellPos.Col = CenterCol + 3;
	cellPos.Row = 0;
	refreshMap[cellPos] = cellPos;
}

void VtOrderPanelGrid::ShowTotalStopOrder()
{
	int buy = 0, sell = 0;
	for (auto it = _StopOrderMgr->StopOrderMapHd.begin(); it != _StopOrderMgr->StopOrderMapHd.end(); ++it) {
		HdOrderRequest* order = it->second;
		if (order->Position == VtPositionType::Sell)
			sell++;
		else
			buy++;
	}

	QuickSetNumber(CenterCol - 4, 0, sell);
	CUGCell cell;
	GetCell(CenterCol - 4, 0, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	cell.SetBackColor(RGB(218, 226, 245));
	SetCell(CenterCol - 4, 0, &cell);
	QuickRedrawCell(CenterCol - 4, 0);
	QuickSetNumber(CenterCol + 4, 0, buy);
	GetCell(CenterCol + 4, 0, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	cell.SetBackColor(RGB(252, 226, 228));
	SetCell(CenterCol + 4, 0, &cell);
	QuickRedrawCell(CenterCol + 4, 0);
}

void VtOrderPanelGrid::ShowTotalStopOrder(bool include)
{
	int buy = 0, sell = 0;
	for (auto it = _StopOrderMgr->StopOrderMapHd.begin(); it != _StopOrderMgr->StopOrderMapHd.end(); ++it) {
		HdOrderRequest* order = it->second;
		if (order->Position == VtPositionType::Sell)
			sell++;
		else
			buy++;
	}

	VtCellPos cellPos;

	QuickSetNumber(CenterCol - 4, 0, sell);
	CUGCell cell;
	GetCell(CenterCol - 4, 0, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	cell.SetBackColor(RGB(218, 226, 245));
	SetCell(CenterCol - 4, 0, &cell);
	cellPos.Col = CenterCol - 4;
	cellPos.Row = 0;
// 	if (include)
// 		_CurPos.insert(cellPos);
// 	else
		QuickRedrawCell(CenterCol - 4, 0);
	QuickSetNumber(CenterCol + 4, 0, buy);
	GetCell(CenterCol + 4, 0, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	cell.SetBackColor(RGB(252, 226, 228));
	SetCell(CenterCol + 4, 0, &cell);
	cellPos.Col = CenterCol + 4;
	cellPos.Row = 0;
// 	if (include)
// 		_CurPos.insert(cellPos);
// 	else
		QuickRedrawCell(CenterCol + 4, 0);
}

void VtOrderPanelGrid::ShowTotalStopOrder(CellPosMap& refreshMap)
{
	int buy = 0, sell = 0;
	for (auto it = _StopOrderMgr->StopOrderMapHd.begin(); it != _StopOrderMgr->StopOrderMapHd.end(); ++it)
	{
		HdOrderRequest* order = it->second;
		if (order->Position == VtPositionType::Sell)
		{
			//sell++;
			sell += order->Amount;
		}
		else
		{
			buy += order->Amount;
		}
	}

	VtCellPos cellPos;

	QuickSetNumber(CenterCol - 4, 0, sell);
	CUGCell cell;
	GetCell(CenterCol - 4, 0, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	cell.SetBackColor(RGB(218, 226, 245));
	SetCell(CenterCol - 4, 0, &cell);
	cellPos.Col = CenterCol - 4;
	cellPos.Row = 0;
	refreshMap[cellPos] = cellPos;
	QuickSetNumber(CenterCol + 4, 0, buy);
	GetCell(CenterCol + 4, 0, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	cell.SetBackColor(RGB(252, 226, 228));
	SetCell(CenterCol + 4, 0, &cell);
	cellPos.Col = CenterCol + 4;
	cellPos.Row = 0;
	refreshMap[cellPos] = cellPos;
}

void VtOrderPanelGrid::SetQuoteInfo()
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;

	if (RowToValueMap.size() == 0)
		return;

	VtSymbol* sym = _CenterWnd->Symbol();

	CUGCell cell;
	VtCellPos pos;

	_LowRow = FindRowFromCenterValue(sym, sym->Quote.intLow);//_StartRowForValue + diff;
	_HighRow = FindRowFromCenterValue(sym, sym->Quote.intHigh);//_StartRowForValue + diff;
	_CloseRow = FindRowFromCenterValue(sym, sym->Quote.intClose); // _StartRowForValue + diff;
	_OpenRow = FindRowFromCenterValue(sym, sym->Quote.intOpen);
	_PreCloseRow = FindRowFromCenterValue(sym, sym->Quote.intPreClose);

	auto it = RowToValueMap.begin();
	int maxRow = it->second;
	maxRow++;
	auto itr = RowToValueMap.rbegin();
	int minRow = itr->second;
	// 양봉
	if (sym->Quote.intClose > sym->Quote.intOpen)
	{
		int start = _StartRowForValue;
		int end = _HighRow;
		SetCellBackColor(minRow, maxRow, start, end, RGB(242, 242, 242));

		start = _HighRow + 1;
		end = _CloseRow;
		SetCellBackColor(minRow, maxRow, start, end, RGB(255, 255, 255));

		start = _CloseRow;
		end = _OpenRow + 1;
		SetCellBackColor(minRow, maxRow, start, end, RGB(252, 226, 228));

		start = _OpenRow + 1;
		end = _LowRow;
		SetCellBackColor(minRow, maxRow, start, end, RGB(255, 255, 255));

		start = _LowRow + 1;
		end = _EndRowForValue + 1;
		SetCellBackColor(minRow, maxRow, start, end, RGB(242, 242, 242));
	}
	else if (sym->Quote.intClose < sym->Quote.intOpen) // 음봉
	{
		int start = _StartRowForValue;
		int end = _HighRow;
		SetCellBackColor(minRow, maxRow, start, end, RGB(242, 242, 242));

		start = _HighRow + 1;
		end = _OpenRow;
		SetCellBackColor(minRow, maxRow, start, end, RGB(255, 255, 255));

		start = _OpenRow;
		end = _CloseRow + 1;
		SetCellBackColor(minRow, maxRow, start, end, RGB(218, 226, 245));

		start = _CloseRow + 1;
		end = _LowRow;
		SetCellBackColor(minRow, maxRow, start, end, RGB(255, 255, 255));

		start = _LowRow + 1;
		end = _EndRowForValue + 1;
		SetCellBackColor(minRow, maxRow, start, end, RGB(242, 242, 242));
	}
	else
	{
		int start = _StartRowForValue;
		int end = _HighRow;
		SetCellBackColor(minRow, maxRow, start, end, RGB(242, 242, 242));

		start = _HighRow + 1;
		end = _OpenRow;
		SetCellBackColor(minRow, maxRow, start, end, RGB(255, 255, 255));

		start = _OpenRow + 1;
		end = _LowRow;
		SetCellBackColor(minRow, maxRow, start, end, RGB(255, 255, 255));

		start = _LowRow + 1;
		end = _EndRowForValue + 1;
		SetCellBackColor(minRow, maxRow, start, end, RGB(242, 242, 242));
	}
	
	if (_LowRow >= _StartRowForValue && _LowRow < _EndRowForValue)
	{
		if (_LowRow != _CloseRow)
		{
			GetCell(CenterCol, _LowRow, &cell);
			cell.SetCellType(m_nSeperateIndex);
			cell.OrderType(3);
			cell.SetLabelText("L");
			cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
			//cell.SetBackColor(RGB(255, 200, 255));
			cell.SetTextColor(RGB(0, 0, 255));
			SetCell(CenterCol, _LowRow, &cell);
			
			std::string strVal = NumberFormatter::format(sym->Quote.intLow / std::pow(10, sym->Decimal), sym->Decimal);
			cell.SetText(strVal.c_str());
			pos.Col = CenterCol;
			pos.Row = _LowRow;

			_QuotePos.insert(pos);
		}
	}

	if (_HighRow >= _StartRowForValue && _HighRow < _EndRowForValue)
	{
		if (_HighRow != _CloseRow)
		{
			GetCell(CenterCol, _HighRow, &cell);
			cell.SetCellType(m_nSeperateIndex);
			cell.OrderType(2);
			cell.SetLabelText("H");
			cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
			//cell.SetBackColor(RGB(255, 0, 0));
			cell.SetTextColor(RGB(255, 0, 0));
			
			std::string strVal = NumberFormatter::format(sym->Quote.intHigh / std::pow(10, sym->Decimal), sym->Decimal);
			cell.SetText(strVal.c_str());
			SetCell(CenterCol, _HighRow, &cell);
			pos.Col = CenterCol;
			pos.Row = _HighRow;


			_QuotePos.insert(pos);
		}
	}
	
	if (_CloseRow >= _StartRowForValue && _CloseRow < _EndRowForValue)
	{
		GetCell(CenterCol, _CloseRow, &cell);
		cell.SetCellType(m_nSeperateIndex);
		//cell.SetLabelText("C");
		cell.OrderType(1);
		cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
		cell.SetBackColor(RGB(255, 255, 0));
		cell.SetTextColor(RGB(0, 0, 0));
		//cell.SetBorder(UG_BDR_BTHICK);
		//cell.SetBorderColor(&m_ClosePen);
		SetCell(CenterCol, _CloseRow, &cell);

		std::string strVal = NumberFormatter::format(sym->Quote.intClose / std::pow(10, sym->Decimal), sym->Decimal);
		cell.SetText(strVal.c_str());
		pos.Col = CenterCol;
		pos.Row = _CloseRow;
		_OldCloseRow = _CloseRow;

		_QuotePos.insert(pos);
	}

	if (_PreCloseRow >= _StartRowForValue && _PreCloseRow < _EndRowForValue)
	{
		GetCell(CenterCol, _PreCloseRow, &cell);
		cell.OrderType(1);
		cell.SetCellType(m_nSeperateIndex);
		cell.SetLabelText("C");
		cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
		cell.SetTextColor(RGB(0, 0, 0));

		//cell.SetBorder(UG_BDR_BTHICK);
		//cell.SetBorderColor(&m_ClosePen);
		SetCell(CenterCol, _PreCloseRow, &cell);

		std::string strVal = NumberFormatter::format(sym->Quote.intPreClose / std::pow(10, sym->Decimal), sym->Decimal);
		cell.SetText(strVal.c_str());
		pos.Col = CenterCol;
		pos.Row = _PreCloseRow;

		_QuotePos.insert(pos);
	}

	if (_OpenRow >= _StartRowForValue && _OpenRow < _EndRowForValue)
	{
		GetCell(CenterCol, _OpenRow, &cell);
		cell.OrderType(1);
		cell.SetCellType(m_nSeperateIndex);
		cell.SetLabelText("O");
		cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
		//cell.SetBackColor(RGB(0, 255, 255));
		cell.SetTextColor(RGB(0, 0, 0));
		SetCell(CenterCol, _OpenRow, &cell);

		std::string strVal = NumberFormatter::format(sym->Quote.intOpen / std::pow(10, sym->Decimal), sym->Decimal);
		cell.SetText(strVal.c_str());
		pos.Col = CenterCol;
		pos.Row = _OpenRow;

		_QuotePos.insert(pos);
	}
}


void VtOrderPanelGrid::SetQuoteInfo(CellPosMap& refreshMap)
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;

	if (RowToValueMap.size() == 0)
		return;

	VtSymbol* sym = _CenterWnd->Symbol();

	CUGCell cell;
	VtCellPos pos;

	_LowRow = FindRowFromCenterValue(sym, sym->Quote.intLow);//_StartRowForValue + diff;
	_HighRow = FindRowFromCenterValue(sym, sym->Quote.intHigh);//_StartRowForValue + diff;
	_CloseRow = FindRowFromCenterValue(sym, sym->Quote.intClose); // _StartRowForValue + diff;
	_OpenRow = FindRowFromCenterValue(sym, sym->Quote.intOpen);
	_PreCloseRow = FindRowFromCenterValue(sym, sym->Quote.intPreClose);

	auto it = RowToValueMap.begin();
	int maxRow = it->second;
	maxRow++;
	auto itr = RowToValueMap.rbegin();
	int minRow = itr->second;
	if (sym->Quote.intClose > sym->Quote.intOpen) { // 양봉
		int start = _StartRowForValue;
		int end = _HighRow;
		SetCellBackColor(minRow, maxRow, start, end, RGB(242, 242, 242));

		start = _HighRow + 1;
		end = _CloseRow;
		SetCellBackColor(minRow, maxRow, start, end, RGB(255, 255, 255));

		start = _CloseRow;
		end = _OpenRow + 1;
		SetCellBackColor(minRow, maxRow, start, end, RGB(252, 226, 228));

		start = _OpenRow + 1;
		end = _LowRow;
		SetCellBackColor(minRow, maxRow, start, end, RGB(255, 255, 255));

		start = _LowRow + 1;
		end = _EndRowForValue + 1;
		SetCellBackColor(minRow, maxRow, start, end, RGB(242, 242, 242));
	} else if (sym->Quote.intClose < sym->Quote.intOpen) { // 음봉
		int start = _StartRowForValue;
		int end = _HighRow;
		SetCellBackColor(minRow, maxRow, start, end, RGB(242, 242, 242));

		start = _HighRow + 1;
		end = _OpenRow;
		SetCellBackColor(minRow, maxRow, start, end, RGB(255, 255, 255));

		start = _OpenRow;
		end = _CloseRow + 1;
		SetCellBackColor(minRow, maxRow, start, end, RGB(218, 226, 245));

		start = _CloseRow + 1;
		end = _LowRow;
		SetCellBackColor(minRow, maxRow, start, end, RGB(255, 255, 255));

		start = _LowRow + 1;
		end = _EndRowForValue + 1;
		SetCellBackColor(minRow, maxRow, start, end, RGB(242, 242, 242));
	} else { // 도지
		int start = _StartRowForValue;
		int end = _HighRow;
		SetCellBackColor(minRow, maxRow, start, end, RGB(242, 242, 242));

		start = _HighRow + 1;
		end = _OpenRow;
		SetCellBackColor(minRow, maxRow, start, end, RGB(255, 255, 255));

		start = _OpenRow + 1;
		end = _LowRow;
		SetCellBackColor(minRow, maxRow, start, end, RGB(255, 255, 255));

		start = _LowRow + 1;
		end = _EndRowForValue + 1;
		SetCellBackColor(minRow, maxRow, start, end, RGB(242, 242, 242));
	}

	if (_LowRow >= _StartRowForValue && _LowRow < _EndRowForValue) {
		if (_LowRow != _CloseRow) {
			GetCell(CenterCol, _LowRow, &cell);
			cell.SetCellType(m_nSeperateIndex);
			cell.OrderType(3);
			cell.SetLabelText("L");
			cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
			//cell.SetBackColor(RGB(255, 200, 255));
			cell.SetTextColor(RGB(0, 0, 255));
			SetCell(CenterCol, _LowRow, &cell);

			std::string strVal = NumberFormatter::format(sym->Quote.intLow / std::pow(10, sym->Decimal), sym->Decimal);
			cell.SetText(strVal.c_str());
			pos.Col = CenterCol;
			pos.Row = _LowRow;

			_QuotePos.insert(pos);
			refreshMap[pos] = pos;
		}
	}

	if (_HighRow >= _StartRowForValue && _HighRow < _EndRowForValue) {
		if (_HighRow != _CloseRow) {
			GetCell(CenterCol, _HighRow, &cell);
			cell.SetCellType(m_nSeperateIndex);
			cell.OrderType(2);
			cell.SetLabelText("H");
			cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
			//cell.SetBackColor(RGB(255, 0, 0));
			cell.SetTextColor(RGB(255, 0, 0));

			std::string strVal = NumberFormatter::format(sym->Quote.intHigh / std::pow(10, sym->Decimal), sym->Decimal);
			cell.SetText(strVal.c_str());
			SetCell(CenterCol, _HighRow, &cell);
			pos.Col = CenterCol;
			pos.Row = _HighRow;


			_QuotePos.insert(pos);
			refreshMap[pos] = pos;
		}
	}

	if (_CloseRow >= _StartRowForValue && _CloseRow < _EndRowForValue) {
		GetCell(CenterCol, _CloseRow, &cell);
		cell.SetCellType(m_nSeperateIndex);
		//cell.SetLabelText("C");
		cell.OrderType(1);
		cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
		cell.SetBackColor(RGB(255, 255, 0));
		cell.SetTextColor(RGB(0, 0, 0));
		//cell.SetBorder(UG_BDR_BTHICK);
		//cell.SetBorderColor(&m_ClosePen);
		SetCell(CenterCol, _CloseRow, &cell);

		std::string strVal = NumberFormatter::format(sym->Quote.intClose / std::pow(10, sym->Decimal), sym->Decimal);
		cell.SetText(strVal.c_str());
		pos.Col = CenterCol;
		pos.Row = _CloseRow;
		_OldCloseRow = _CloseRow;

		_QuotePos.insert(pos);
		refreshMap[pos] = pos;
	}

	if (_PreCloseRow >= _StartRowForValue && _PreCloseRow < _EndRowForValue) {
		GetCell(CenterCol, _PreCloseRow, &cell);
		cell.OrderType(1);
		cell.SetCellType(m_nSeperateIndex);
		cell.SetLabelText("C");
		cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
		cell.SetTextColor(RGB(0, 0, 0));

		//cell.SetBorder(UG_BDR_BTHICK);
		//cell.SetBorderColor(&m_ClosePen);
		SetCell(CenterCol, _PreCloseRow, &cell);

		std::string strVal = NumberFormatter::format(sym->Quote.intPreClose / std::pow(10, sym->Decimal), sym->Decimal);
		cell.SetText(strVal.c_str());
		pos.Col = CenterCol;
		pos.Row = _PreCloseRow;

		_QuotePos.insert(pos);
		refreshMap[pos] = pos;
	}

	if (_OpenRow >= _StartRowForValue && _OpenRow < _EndRowForValue) {
		GetCell(CenterCol, _OpenRow, &cell);
		cell.OrderType(1);
		cell.SetCellType(m_nSeperateIndex);
		cell.SetLabelText("O");
		cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
		//cell.SetBackColor(RGB(0, 255, 255));
		cell.SetTextColor(RGB(0, 0, 0));
		SetCell(CenterCol, _OpenRow, &cell);

		std::string strVal = NumberFormatter::format(sym->Quote.intOpen / std::pow(10, sym->Decimal), sym->Decimal);
		cell.SetText(strVal.c_str());
		pos.Col = CenterCol;
		pos.Row = _OpenRow;

		_QuotePos.insert(pos);
		refreshMap[pos] = pos;
	}
}

void VtOrderPanelGrid::ShowQuoteInfo()
{
// 	for (int i = _StartRowForValue; i < _EndRowForValue; i++)
// 	{
// 		QuickRedrawCell(CenterCol, i);
// 	}
// 
// 	_OldQuotePos.clear();
// 	for (auto it = _QuotePos.begin(); it != _QuotePos.end(); ++it)
// 	{
// 		VtCellPos pos = *it;
// 		_OldQuotePos.insert(pos);
// 	}
}


void VtOrderPanelGrid::ShowQuoteInfo(CellPosMap& refreshMap)
{
// 	for (int i = _StartRowForValue; i < _EndRowForValue; i++)
// 	{
// 		VtCellPos pos;
// 		pos.Col = CenterCol;
// 		pos.Row = i;
// 		refreshMap[pos] = pos;
// 	}
// 
// 	RefreshCells(refreshMap);
// 
// 	_OldQuotePos.clear();
// 	for (auto it = _QuotePos.begin(); it != _QuotePos.end(); ++it)
// 	{
// 		VtCellPos pos = *it;
// 		_OldQuotePos.insert(pos);
// 	}
}

void VtOrderPanelGrid::DrawOrderLine(int a_Col, long a_Row)
{
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	CDC* dc = m_CUGGrid->GetDC();
	dc->SelectObject(pen);
	RECT rcCell;
	GetCellRect(a_Col, a_Row, &rcCell);
	rcCell.right -= 2;
	rcCell.bottom -= 2;

	//CPoint pt1, pt2;
	if (a_Col < CenterCol)
	{
		//dc->MoveTo(rcCell.left, rcCell.top + (rcCell.bottom - rcCell.top) / 2);
		//dc->LineTo(rcCell.left + (rcCell.right - rcCell.left) / 2, rcCell.top + (rcCell.bottom - rcCell.top) / 2);
		//CPoint pt1(rcCell.left, rcCell.top + (rcCell.bottom - rcCell.top) / 2);
		//CPoint pt2(rcCell.right, rcCell.top + (rcCell.bottom - rcCell.top) / 2);
		CPoint pt1(rcCell.left, rcCell.top);
		CPoint pt2(rcCell.left, rcCell.bottom + 200);
		//DrawArrow(dc, pt1, pt2);
		DrawArrow(1, nullptr, dc, pt1, pt2, 6, 6);
	}
	else
	{
		//dc->MoveTo(rcCell.right, rcCell.top + (rcCell.bottom - rcCell.top) / 2);
		//dc->LineTo(rcCell.left + (rcCell.right - rcCell.left) / 2, rcCell.top + (rcCell.bottom - rcCell.top) / 2);
		CPoint pt1(rcCell.left, rcCell.bottom);
		CPoint pt2(rcCell.left, rcCell.top + 200);
		//DrawArrow(dc, pt1, pt2);
		DrawArrow(1, nullptr, dc, pt1, pt2, 6, 6);
	}
	m_CUGGrid->ReleaseDC(dc);
	pen.DeleteObject();
}

void VtOrderPanelGrid::SetCellBackColor(int minRow, int maxRow, int start, int end, COLORREF color)
{
	VtCellPos pos;
	if (end < minRow || start > maxRow) {
		return;
	}

	start = max(minRow, start);
	end = min(maxRow, end);
	
	for (int i = start; i < end; ++i) {
		QuickSetBackColor(CenterCol, i, color);
		pos.Col = CenterCol;
		pos.Row = i;
		if (i == _EndRowForValue + 1) {
			int k = 0;
		}
		_QuotePos.insert(pos);
	}
}

constexpr int Round(float x) { return static_cast<int>(x + 0.5f); }


void VtOrderPanelGrid::DrawArrow(int direction, POINT *point, CDC* pdc, POINT p0, POINT p1, int head_length, int head_width)
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
	if (direction == 1)
	{
		pdc->MoveTo(p0);
		pdc->LineTo(p1);
	}
	else
	{
		POINT p3;
		pdc->MoveTo(p0);
		pdc->LineTo(p1);
		p3.y = p1.y;
		p3.x = point->x;
		pdc->MoveTo(p1);
		pdc->LineTo(p3);
		p0 = p1;
		p1 = p3;
		//msg = _T("취소");
	}
	

	const float dx = static_cast<float>(p1.x - p0.x);
	const float dy = static_cast<float>(p1.y - p0.y);
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
	/*
	RECT rc;
	rc.left = p1.x;
	rc.top = p1.y;
	rc.right = rc.left + 44;
	rc.bottom = rc.top + 22;
	CFont* oldFont = pdc->SelectObject(&_CursorFont);
	pdc->DrawText(msg, &rc, DT_CENTER | DT_VCENTER | DT_WORDBREAK);
	pdc->SelectObject(oldFont);
	*/
	pdc->SelectObject(&OrigBrush);
}

void VtOrderPanelGrid::DrawMovingOrder(int col, int row, POINT *point)
{
	RedrawCell(_OldMMCol, _OldMMRow);
	RECT rcPrev, rcCur;
	GetCellRect(m_DragStartCol, m_DragStartRow, &rcPrev);
	GetCellRect(m_DragStartCol, row, &rcCur);
	CPoint pt1(rcPrev.left + (rcPrev.right - rcPrev.left) / 2, rcPrev.top + (rcPrev.bottom - rcPrev.top) / 2);
	CPoint pt2(rcCur.left + (rcCur.right - rcCur.left) / 2, rcCur.top + (rcCur.bottom - rcCur.top) / 2);
	CDC *dc = m_CUGGrid->GetDC();
	if (col > m_DragStartCol)
	{
		DrawArrow(2, point, dc, pt1, pt2, 6, 6);
	}
	else if (col < m_DragStartCol)
	{
		DrawArrow(3, point, dc, pt1, pt2, 6, 6);
	}
	else
	{
		DrawArrow(1, point, dc, pt1, pt2, 6, 6);
	}
	m_CUGGrid->ReleaseDC(dc);

}

void VtOrderPanelGrid::ShowOrderMenu(POINT* point)
{
	/*
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_ORDER));
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	POINT pt;
	pt.x = point->x;
	pt.y = point->y + 22;
	ClientToScreen(&pt);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
	*/
}

int VtOrderPanelGrid::FindRowFromCenterValue(int value)
{
	auto it = RowToValueMap.find(value);
	if (it != RowToValueMap.end())
	{
		return it->second;
	}
	else
	{
		return -2;
	}
}

/*
int VtOrderPanelGrid::FindRowFromCenterValue(VtSymbol* sym, int value)
{
	if (!sym)
		return 0;
	auto it = RowToValueMap.find(value);
	if (it != RowToValueMap.end())
	{
		return it->second;
	}
	else
	{
		auto itr = RowToValueMap.rbegin();
		int bigVal = itr->first;
		int bigRow = itr->second;

		int delta = bigVal - value;
		int rowDiff = delta / sym->intTickSize;
		return bigRow + rowDiff;
	}
}
*/


int VtOrderPanelGrid::FindRowFromCenterValue(VtSymbol* sym, int value)
{
	if (!sym)
		return 0;
	auto it = RowToValueMap.find(value);
	if (it != RowToValueMap.end()) { // 값이 보이는 범위 안에 있을 때
		return it->second;
	} else { // 값이 보이는 범위 밖에 있을 때
		auto itr = RowToValueMap.rbegin();
		int bigVal = itr->first;
		int bigRow = itr->second;
		int thousandRow = 0;
		// 가격이 10인 행을 찾는 과정 - 10이상이면 승수가 변한다.
		if (bigVal >= 1000) { // 최상위 값이 10보다 이상일 경우
			int delta = bigVal - 1000;
			int deltaRow = delta / sym->intTickSize;
			thousandRow = bigRow + deltaRow;
		} else { // 최상위 값이 10미만인 경우
			int delta = 1000 - bigVal;
			thousandRow = bigRow - delta;
		}

		if (value >= 1000) { // 가격이 10 이상인 있는 경우 - 종목의 틱크기 만큼 변함
			int delta = value - 1000;
			int deltaRow = delta / sym->intTickSize;
			return thousandRow - deltaRow;
		} else { // 가격이 10 미만인 경우 - 종목에 관계없이 1씩 변함
			int delta = 1000 - value;
			return thousandRow + delta;
		}
	}
}

void VtOrderPanelGrid::ClearAllStopOrders()
{
	if (_StopOrderMgr)
		_StopOrderMgr->RemoveAllHd();
}

int VtOrderPanelGrid::GetGridWidth(std::vector<bool>& colOptions)
{
	int totalWidth = _GridColMap[VtOrderGridCol::CENTER] + _GridColMap[VtOrderGridCol::QUANTITY] * 2;
	for (int i = 0; i < colOptions.size(); ++i) {
		if (i == 0) { // for order
			totalWidth += colOptions[i] ? _GridColMap[VtOrderGridCol::ORDER] * 2 : 0;
		}
		else if (i == 1) { // for stop
			totalWidth += colOptions[i] ? _GridColMap[VtOrderGridCol::STOP] * 2 : 0;
		}
		else if (i == 2) { // for count and quantity.
			totalWidth += colOptions[i] ? _GridColMap[VtOrderGridCol::COUNT] * 2 : 0;
		}
	}

	return totalWidth;
}

int VtOrderPanelGrid::GetGridHeight()
{
	int total = _HeadHeight;
	total += _CellHeight * _RowCount;

	return total;
}

void VtOrderPanelGrid::BuyAtMarket()
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr)
		return;
	PutOrder(0, VtPositionType::Buy, VtPriceType::Market);
}

void VtOrderPanelGrid::SellAtMarket()
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_OrderConfigMgr)
		return;
	PutOrder(0, VtPositionType::Sell, VtPriceType::Market);
}

void VtOrderPanelGrid::CancelAllSellStop()
{
	_StopOrderMgr->RemoveSellAll();
	_CutMgr->RemoveSellAllHd();
	RefreshStopOrder();
}

void VtOrderPanelGrid::CancelAllBuyStop()
{
	_StopOrderMgr->RemoveBuyAll();
	_CutMgr->RemoveBuyAllHd();
	RefreshStopOrder();
}

void VtOrderPanelGrid::CancelAllSellAccepted()
{
	//_OrderConfigMgr->PrdtOrderMgr()->CancelAllSellAccepted();
	if (!_CenterWnd)
		return;

	//VtProductOrderManager* prdtOrderMgr = _CenterWnd->PrdtOrderMgr();
	std::vector<VtOrder*> acptOrderList = _OrderConfigMgr->OrderMgr()->GetAcceptedOrders(_CenterWnd->Symbol()->ShortCode);

	for (auto it = acptOrderList.begin(); it != acptOrderList.end(); ++it)
	{
		VtOrder* order = *it;
		if (order->orderPosition == VtPositionType::Sell)
			CancelOrder(order);
	}
}

void VtOrderPanelGrid::CancelAllBuyAccepted()
{
	//_OrderConfigMgr->PrdtOrderMgr()->CancelAllBuyAccepted();
	if (!_CenterWnd)
		return;

	//VtProductOrderManager* prdtOrderMgr = _CenterWnd->PrdtOrderMgr();
	std::vector<VtOrder*> acptOrderList = _OrderConfigMgr->OrderMgr()->GetAcceptedOrders(_CenterWnd->Symbol()->ShortCode);

	for (auto it = acptOrderList.begin(); it != acptOrderList.end(); ++it)
	{
		VtOrder* order = *it;
		if (order->orderPosition == VtPositionType::Buy)
			CancelOrder(order);
	}
}

void VtOrderPanelGrid::CancelAllAccepted()
{
	if (!_CenterWnd)
		return;

	//VtProductOrderManager* prdtOrderMgr = _CenterWnd->PrdtOrderMgr();
	std::vector<VtOrder*> acptOrderList = _OrderConfigMgr->OrderMgr()->GetAcceptedOrders(_CenterWnd->Symbol()->ShortCode);

	for (auto it = acptOrderList.begin(); it != acptOrderList.end(); ++it)
	{
		VtOrder* order = *it;
		CancelOrder(order);
	}
}

void VtOrderPanelGrid::SetAutoStopOnFilled(VtOrder* order)
{
	if (!order)
		return;

	int diff = VtSymbolManager::GetTickDiff(_Close, order->filledPrice, _OrderConfigMgr->Master());
	CString profitVal;
	CString losscutVal;
	VtOrderRequest* profitStop = nullptr;
	VtOrderRequest* losscutStop = nullptr;
	if (_OrderConfigMgr->UseAutoStopProfit)
	{
		if (order->orderPosition == VtPositionType::Buy)
			profitVal = QuickGetText(CenterCol, _CloseRow + diff - _OrderConfigMgr->AutoStopProfitValue);
		else
			profitVal = QuickGetText(CenterCol, _CloseRow + diff + _OrderConfigMgr->AutoStopProfitValue);
		profitStop = AddAutoStopOrder(_ttof(profitVal), order->orderPosition);
		profitStop->stopType = 0;
		profitStop->slip = _OrderConfigMgr->AutoStopPriceSlip;
		profitStop->priceType = _OrderConfigMgr->AutoStopPriceType;
		if (_OrderConfigMgr->AutoStopPriceType == VtPriceType::Price)
		{
			profitStop->tradeType = VtTradeType::Price;
		}
		else
		{
			profitStop->tradeType = VtTradeType::Market;
		}
		profitStop->amount = order->filledQty;
		_StopOrderMgr->AddOrder(profitStop);
	}

	if (_OrderConfigMgr->UseAutoStopLosscut)
	{
		if (order->orderPosition == VtPositionType::Buy)
			losscutVal = QuickGetText(CenterCol, _CloseRow + diff + _OrderConfigMgr->AutoStopLosscutValue);
		else
			losscutVal = QuickGetText(CenterCol, _CloseRow + diff - _OrderConfigMgr->AutoStopLosscutValue);

		if (order->orderPosition == VtPositionType::Buy)
			losscutStop = AddAutoStopOrder(_ttof(losscutVal), VtPositionType::Sell);
		else
			losscutStop = AddAutoStopOrder(_ttof(losscutVal), VtPositionType::Buy);
		losscutStop->stopType = 0;
		losscutStop->slip = _OrderConfigMgr->AutoStopPriceSlip;
		losscutStop->priceType = _OrderConfigMgr->AutoStopPriceType;
		if (_OrderConfigMgr->AutoStopPriceType == VtPriceType::Price)
		{
			losscutStop->tradeType = VtTradeType::Price;
		}
		else
		{
			losscutStop->tradeType = VtTradeType::Market;
		}
		losscutStop->amount = order->filledQty;

		_StopOrderMgr->AddOrder(losscutStop);
	}

	RefreshStopOrder();

	// 상대방의 주문요구번호
	if (_OrderConfigMgr->UseAutoStopLosscut && 
		_OrderConfigMgr->UseAutoStopLosscut && 
		losscutStop && profitStop)
	{
		profitStop->counterReqId = losscutStop->requestID;
		losscutStop->counterReqId = profitStop->requestID;
		profitStop->stopType = 1;
		losscutStop->stopType = 1;
	}
}

void VtOrderPanelGrid::CheckStopTouched(double close)
{
	if (_StopOrderMgr && _OrderConfigMgr->Master())
	{
		for (auto it = _StopOrderMgr->StopOrderMap.begin(); it != _StopOrderMgr->StopOrderMap.end(); ++it)
		{
			VtOrderRequest* stop = it->second;
			int diff = VtSymbolManager::GetTickDiff(close, stop->orderPrice, _OrderConfigMgr->Master());
			if (diff == 0)
			{
				if (stop->orderPosition == VtPositionType::Buy)
				{
					stop->orderPrice = VtSymbolManager::GetNextValue(stop->orderPrice, _OrderConfigMgr->Master(), stop->slip);
				}
				else
				{
					stop->orderPrice = VtSymbolManager::GetNextValue(stop->orderPrice, _OrderConfigMgr->Master(), -stop->slip);
				}

				_OrderConfigMgr->OrderMgr()->PutOrder(std::move(*stop));
				_StopOrderMgr->FinishedOrderList.push_back(stop);
				if (stop->stopType == 1)
				{
					VtOrderRequest* counterOrder = _StopOrderMgr->FindOrder(stop->counterReqId);
					if (counterOrder)
						_StopOrderMgr->FinishedOrderList.push_back(counterOrder);
				}
			}
		}
		for (auto it = _StopOrderMgr->FinishedOrderList.begin(); it != _StopOrderMgr->FinishedOrderList.end(); ++it)
		{
			VtOrderRequest* req = *it;
			_StopOrderMgr->RemoveOrder(req->requestID);
		}

		_StopOrderMgr->FinishedOrderList.clear();

		RefreshStopOrder();
	}
}

void VtOrderPanelGrid::CheckStopTouchedHd(int intClose)
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_StopOrderMgr)
		return;

	VtSymbol* sym = _CenterWnd->Symbol();

	for (auto it = _StopOrderMgr->StopOrderMapHd.begin(), next_it = it; it != _StopOrderMgr->StopOrderMapHd.end(); it = next_it)
	{
		HdOrderRequest* stop = it->second;
		++next_it;

		if (stop->Price == intClose)
		{
			if (stop->Position == VtPositionType::Buy)
			{
				stop->Price = VtSymbolManager::GetNextValue(stop->Price, stop->slip, sym);
			}
			else
			{
				stop->Price = VtSymbolManager::GetNextValue(stop->Price, -stop->slip, sym);
			}
			_OrderConfigMgr->OrderMgr()->PutOrder(std::move(*stop));
			// 가격이 닿은 스탑 주문은 목록에서 없애 준다.
			_StopOrderMgr->StopOrderMapHd.erase(it);
		}
	}
}

void VtOrderPanelGrid::CheckProfitLossTouchHd(int intClose)
{
	if (!_CenterWnd || !_CenterWnd->Symbol() || !_CutMgr)
		return;
	std::vector<int> removeList;
	VtSymbol* sym = _CenterWnd->Symbol();
	std::map<int, HdOrderRequest*>& profitLossMap = _CutMgr->GetStopOrderMap();
	for (auto it = profitLossMap.begin(); it != profitLossMap.end(); ++it) {
		HdOrderRequest* stop = it->second;
		//LOG_F(INFO, _T("CheckProfitLossTouchHd : 손절익절가격 %d"), stop->Price);
		if (stop->Price == intClose) { // 종가와 일치하는 경우
			if (stop->Position == VtPositionType::Buy) { // 매수 포지션인 경우
				stop->Price = VtSymbolManager::GetNextValue(stop->Price, stop->slip, sym);
			} else { // 매도 포지션인 경우
				stop->Price = VtSymbolManager::GetNextValue(stop->Price, -stop->slip, sym);
			}

			removeList.push_back(stop->RequestId);  // 삭제 목록에 저장한다.

			if (stop->Paired && stop->PairedReq) { // 익절, 손절 쌍으로 연결되어 있는 경우 한쪽이 사라지면 다른 한쪽도 삭제 목록에 넣어준다.
				removeList.push_back(stop->PairedReq->RequestId);
			}
			_OrderConfigMgr->OrderMgr()->PutOrder(std::move(*stop));
		}
	}

	for (auto it = removeList.begin(); it != removeList.end(); ++it) { // 위에서 등록된 삭제 목록들을 목록에서 지워준다.
		_CutMgr->RemoveOrderHd(*it, false);
	}
}


void VtOrderPanelGrid::SetStopOrder(CellPosMap& refreshMap, HdOrderRequest* order)
{
	int row = FindRowFromCenterValue(order->Price); 

	VtCellPos pos;
	CUGCell cell;
	VtArrowPos arrowPos;
	RECT rcCell1, rcCell2;
	CString text;
	VtCellPos pos1;

	if (order->Position == VtPositionType::Sell)
	{
		pos.Col = CenterCol - 4;
		GetCell(CenterCol - 4, row, &cell);
		cell.OrderType(4);
		cell.OrderSlip(order->slip);
		cell.StopOrderListHd.push_back(order);
		cell.SetCellType(m_OrderCellTypeIndex);
		SetCell(CenterCol - 4, row, &cell);
		GetCellRect(CenterCol - 4, row, &rcCell1);
		GetCellRect(CenterCol - 3, row, &rcCell2);

		arrowPos.sx = rcCell1.left + (rcCell1.right - rcCell1.left) / 2;
		arrowPos.sy = rcCell1.top + (rcCell1.bottom - rcCell1.top) / 2;
		arrowPos.ex = rcCell2.left + (rcCell2.right - rcCell2.left) / 2;
		arrowPos.ey = arrowPos.sy + (rcCell2.bottom - rcCell2.top) * cell.OrderSlip();
		arrowPos.type = 2;
		arrowPos.col = CenterCol - 4;
		arrowPos.row = row;
		arrowPos.slip = cell.OrderSlip();
		arrowPos.startRow = _StartRowForValue;
		arrowPos.endRow = _EndRowForValue - 1;
		AddArrowPos(std::move(arrowPos));

		if (cell.OrderSlip() > 0)
		{
			for (int i = row; i <= row + cell.OrderSlip(); i++)
			{
				if (i >= _StartRowForValue && i <= _EndRowForValue)
				{
					pos1.Row = i;
					pos1.Col = CenterCol - 3;
					_StopOrderPos.insert(pos1);
					refreshMap[pos1] = pos1;

					pos1.Row = i;
					pos1.Col = CenterCol - 4;
					_StopOrderPos.insert(pos1);
					refreshMap[pos1] = pos1;
				}
			}
		}
		else if (cell.OrderSlip() < 0)
		{
			for (int i = row; i >= row - cell.OrderSlip(); i--)
			{
				if (i >= _StartRowForValue && i <= _EndRowForValue)
				{

					pos1.Row = i;
					pos1.Col = CenterCol - 3;
					_StopOrderPos.insert(pos1);
					refreshMap[pos1] = pos1;

					pos1.Row = i;
					pos1.Col = CenterCol - 4;
					_StopOrderPos.insert(pos1);
					refreshMap[pos1] = pos1;
				}
			}
		}
		else
		{
			if (row >= _StartRowForValue && row <= _EndRowForValue)
			{
				pos1.Row = row;
				pos1.Col = CenterCol - 3;
				_StopOrderPos.insert(pos1);
				refreshMap[pos1] = pos1;

				pos1.Row = row;
				pos1.Col = CenterCol - 4;
				_StopOrderPos.insert(pos1);
				refreshMap[pos1] = pos1;
			}
		}
		int cellOrderCnt = 0;
		for (auto it = cell.StopOrderListHd.begin(); it != cell.StopOrderListHd.end(); ++it)
			cellOrderCnt += (*it)->Amount;
		text.Format(_T("%d(%d)"), cellOrderCnt, cell.OrderSlip());
	}
	else
	{
		pos.Col = CenterCol + 4;
		GetCell(CenterCol + 4, row, &cell);
		cell.OrderType(3);
		cell.OrderSlip(order->slip);
		cell.StopOrderListHd.push_back(order);
		cell.SetCellType(m_OrderCellTypeIndex);
		SetCell(CenterCol + 4, row, &cell);

		GetCellRect(CenterCol + 4, row, &rcCell1);
		GetCellRect(CenterCol + 3, row, &rcCell2);

		arrowPos.sx = rcCell1.left + (rcCell1.right - rcCell1.left) / 2;
		arrowPos.sy = rcCell1.top + (rcCell1.bottom - rcCell1.top) / 2;
		arrowPos.ex = rcCell2.left + (rcCell2.right - rcCell2.left) / 2;
		arrowPos.ey = arrowPos.sy - (rcCell2.bottom - rcCell2.top) * cell.OrderSlip();
		arrowPos.type = 1;
		arrowPos.col = CenterCol + 4;
		arrowPos.row = row;
		arrowPos.slip = cell.OrderSlip();
		arrowPos.startRow = _StartRowForValue;
		arrowPos.endRow = _EndRowForValue - 1;
		AddArrowPos(std::move(arrowPos));

		if (cell.OrderSlip() > 0)
		{
			for (int i = row; i >= row - cell.OrderSlip(); i--)
			{
				if (i >= _StartRowForValue && i <= _EndRowForValue)
				{
					pos1.Row = i;
					pos1.Col = CenterCol + 3;
					_StopOrderPos.insert(pos1);
					refreshMap[pos1] = pos1;
					pos1.Row = i;
					pos1.Col = CenterCol + 4;
					_StopOrderPos.insert(pos1);
					refreshMap[pos1] = pos1;
				}
			}
		}
		else if (cell.OrderSlip() < 0)
		{
			for (int i = row; i <= row - cell.OrderSlip(); i++)
			{
				if (i >= _StartRowForValue && i <= _EndRowForValue)
				{
					pos1.Row = i;
					pos1.Col = CenterCol + 3;
					_StopOrderPos.insert(pos1);
					refreshMap[pos1] = pos1;

					pos1.Row = i;
					pos1.Col = CenterCol + 4;
					_StopOrderPos.insert(pos1);
					refreshMap[pos1] = pos1;
				}
			}
		}
		else
		{
			if (row >= _StartRowForValue && row <= _EndRowForValue)
			{
				pos1.Row = row;
				pos1.Col = CenterCol + 3;
				_StopOrderPos.insert(pos1);
				refreshMap[pos1] = pos1;

				pos1.Row = row;
				pos1.Col = CenterCol + 4;
				_StopOrderPos.insert(pos1);
				refreshMap[pos1] = pos1;
			}
		}

		int cellOrderCnt = 0;
		for (auto it = cell.StopOrderListHd.begin(); it != cell.StopOrderListHd.end(); ++it)
			cellOrderCnt += (*it)->Amount;

		text.Format(_T("%d(%d)"), cellOrderCnt, cell.OrderSlip());
	}
	pos.Row = row;
	refreshMap[pos1] = pos1;
	QuickSetText(pos.Col, pos.Row, text);
}

void VtOrderPanelGrid::OrderByMouseClick()
{
	if (GetClickRect()) {
		if (_SelectedCol == CenterCol - 3) {
			PutOrder(_SelectedVal, VtPositionType::Sell, VtPriceType::Price);
		} else if (_SelectedCol == CenterCol + 3) {
			PutOrder(_SelectedVal, VtPositionType::Buy, VtPriceType::Price);
		} else if (_SelectedCol == CenterCol - 4) {
			AddNormalStopOrderHd(_SelectedVal, VtPositionType::Sell);
			RefreshStopOrder();
		} else if (_SelectedCol == CenterCol + 4) {
			AddNormalStopOrderHd(_SelectedVal, VtPositionType::Buy);
			RefreshStopOrder();
		}
	}
}

void VtOrderPanelGrid::OrderByMousePosition()
{
	if (_MouseIn)
	{
		CUGCell cell;
		GetCell(CenterCol, _MMRow, &cell);
		int curVal = cell.LongValue();
		if (_MMCol == CenterCol - 3)
		{
			PutOrder(curVal, VtPositionType::Sell, VtPriceType::Price);
		}
		else if (_MMCol == CenterCol + 3)
		{
			PutOrder(curVal, VtPositionType::Buy, VtPriceType::Price);
		}
		else if (_MMCol == CenterCol - 4)
		{
			AddNormalStopOrderHd(curVal, VtPositionType::Sell);
			RefreshStopOrder();
		}
		else if (_MMCol == CenterCol + 4)
		{
			AddNormalStopOrderHd(curVal, VtPositionType::Buy);
			RefreshStopOrder();
		}
	}
}

void VtOrderPanelGrid::ShowTotalHoga()
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;

	VtSymbol* sym = _CenterWnd->Symbol();

	_TotalHogaPos.clear();
	VtCellPos cellPos;

	CUGCell cell;
	GetCell(CenterCol - 1, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	SetCell(CenterCol - 1, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol - 1, _EndRowForValue, sym->Hoga.TotSellQty);
	QuickRedrawCell(CenterCol - 1, _EndRowForValue);
	cellPos.Col = CenterCol - 1;
	cellPos.Row = _EndRowForValue;
	_TotalHogaPos.insert(cellPos);

	GetCell(CenterCol - 2, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	SetCell(CenterCol - 2, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol - 2, _EndRowForValue, sym->Hoga.TotSellNo);
	QuickRedrawCell(CenterCol - 2, _EndRowForValue);
	cellPos.Col = CenterCol - 2;
	cellPos.Row = _EndRowForValue;
	_TotalHogaPos.insert(cellPos);

	GetCell(CenterCol + 1, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	SetCell(CenterCol + 1, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol + 1, _EndRowForValue, sym->Hoga.TotBuyQty);
	QuickRedrawCell(CenterCol + 1, _EndRowForValue);
	cellPos.Col = CenterCol + 1;
	cellPos.Row = _EndRowForValue;
	_TotalHogaPos.insert(cellPos);

	GetCell(CenterCol + 2, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	SetCell(CenterCol + 2, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol + 2, _EndRowForValue, sym->Hoga.TotBuyNo);
	QuickRedrawCell(CenterCol + 2, _EndRowForValue);
	cellPos.Col = CenterCol + 2;
	cellPos.Row = _EndRowForValue;
	_TotalHogaPos.insert(cellPos);

	int dif = sym->Hoga.TotBuyQty - sym->Hoga.TotSellQty;

	GetCell(CenterCol, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	SetCell(CenterCol, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol, _EndRowForValue, dif);
	cellPos.Col = CenterCol;
	cellPos.Row = _EndRowForValue;
	_TotalHogaPos.insert(cellPos);

	if (dif > 0)
	{
		QuickSetTextColor(CenterCol, _EndRowForValue, RGB(255, 0, 0));
		QuickRedrawCell(CenterCol, _EndRowForValue);
	}
	else
	{
		QuickSetTextColor(CenterCol, _EndRowForValue, RGB(0, 0, 255));
		QuickRedrawCell(CenterCol, _EndRowForValue);
	}
}

void VtOrderPanelGrid::ShowTotalHoga(bool include)
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;

	VtSymbol* sym = _CenterWnd->Symbol();

	_TotalHogaPos.clear();
	VtCellPos cellPos;

	CUGCell cell;
	GetCell(CenterCol - 1, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	SetCell(CenterCol - 1, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol - 1, _EndRowForValue, sym->Hoga.TotSellQty);
	cellPos.Col = CenterCol - 1;
	cellPos.Row = _EndRowForValue;
	_TotalHogaPos.insert(cellPos);
// 	if (include)
// 	{
// 		_CurPos.insert(cellPos);
// 	}
// 	else
		QuickRedrawCell(CenterCol - 1, _EndRowForValue);

	GetCell(CenterCol - 2, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	SetCell(CenterCol - 2, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol - 2, _EndRowForValue, sym->Hoga.TotSellNo);
	
	cellPos.Col = CenterCol - 2;
	cellPos.Row = _EndRowForValue;
	_TotalHogaPos.insert(cellPos);
// 	if (include)
// 	{
// 		_CurPos.insert(cellPos);
// 	}
// 	else
		QuickRedrawCell(CenterCol - 2, _EndRowForValue);

	GetCell(CenterCol + 1, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	SetCell(CenterCol + 1, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol + 1, _EndRowForValue, sym->Hoga.TotBuyQty);
	
	cellPos.Col = CenterCol + 1;
	cellPos.Row = _EndRowForValue;
	_TotalHogaPos.insert(cellPos);
// 	if (include)
// 	{
// 		_CurPos.insert(cellPos);
// 	}
// 	else
		QuickRedrawCell(CenterCol + 1, _EndRowForValue);

	GetCell(CenterCol + 2, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	SetCell(CenterCol + 2, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol + 2, _EndRowForValue, sym->Hoga.TotBuyNo);
	
	cellPos.Col = CenterCol + 2;
	cellPos.Row = _EndRowForValue;
	_TotalHogaPos.insert(cellPos);
// 	if (include)
// 	{
// 		_CurPos.insert(cellPos);
// 	}
// 	else
		QuickRedrawCell(CenterCol + 2, _EndRowForValue);

	int dif = sym->Hoga.TotBuyQty - sym->Hoga.TotSellQty;

	GetCell(CenterCol, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	SetCell(CenterCol, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol, _EndRowForValue, dif);
	cellPos.Col = CenterCol;
	cellPos.Row = _EndRowForValue;
	_TotalHogaPos.insert(cellPos);
	
	if (dif > 0) {
		QuickSetTextColor(CenterCol, _EndRowForValue, RGB(255, 0, 0));
	} else {
		QuickSetTextColor(CenterCol, _EndRowForValue, RGB(0, 0, 255));
	}

// 	if (include)
// 	{
// 		_CurPos.insert(cellPos);
// 	}
// 	else
		QuickRedrawCell(CenterCol, _EndRowForValue);
}

void VtOrderPanelGrid::ShowTotalHoga(CellPosMap& refreshMap)
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;

	VtSymbol* sym = _CenterWnd->Symbol();

	_TotalHogaPos.clear();
	VtCellPos cellPos;

	CUGCell cell;
	GetCell(CenterCol - 1, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	SetCell(CenterCol - 1, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol - 1, _EndRowForValue, sym->Hoga.TotSellQty);
	//QuickRedrawCell(CenterCol - 1, _EndRowForValue);
	cellPos.Col = CenterCol - 1;
	cellPos.Row = _EndRowForValue;
	_TotalHogaPos.insert(cellPos);
	refreshMap[cellPos] = cellPos;

	GetCell(CenterCol - 2, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	SetCell(CenterCol - 2, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol - 2, _EndRowForValue, sym->Hoga.TotSellNo);
	//QuickRedrawCell(CenterCol - 2, _EndRowForValue);
	cellPos.Col = CenterCol - 2;
	cellPos.Row = _EndRowForValue;
	_TotalHogaPos.insert(cellPos);
	refreshMap[cellPos] = cellPos;

	GetCell(CenterCol + 1, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	SetCell(CenterCol + 1, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol + 1, _EndRowForValue, sym->Hoga.TotBuyQty);
	//QuickRedrawCell(CenterCol + 1, _EndRowForValue);
	cellPos.Col = CenterCol + 1;
	cellPos.Row = _EndRowForValue;
	_TotalHogaPos.insert(cellPos);
	refreshMap[cellPos] = cellPos;

	GetCell(CenterCol + 2, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNRIGHT | UG_ALIGNVCENTER);
	SetCell(CenterCol + 2, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol + 2, _EndRowForValue, sym->Hoga.TotBuyNo);
	//QuickRedrawCell(CenterCol + 2, _EndRowForValue);
	cellPos.Col = CenterCol + 2;
	cellPos.Row = _EndRowForValue;
	_TotalHogaPos.insert(cellPos);
	refreshMap[cellPos] = cellPos;

	int dif = sym->Hoga.TotBuyQty - sym->Hoga.TotSellQty;

	GetCell(CenterCol, _EndRowForValue, &cell);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	SetCell(CenterCol, _EndRowForValue, &cell);
	QuickSetNumber(CenterCol, _EndRowForValue, dif);
	cellPos.Col = CenterCol;
	cellPos.Row = _EndRowForValue;
	_TotalHogaPos.insert(cellPos);
	refreshMap[cellPos] = cellPos;

	if (dif > 0)
	{
		QuickSetTextColor(CenterCol, _EndRowForValue, RGB(255, 0, 0));
		//QuickRedrawCell(CenterCol, _EndRowForValue);
	}
	else
	{
		QuickSetTextColor(CenterCol, _EndRowForValue, RGB(0, 0, 255));
		//QuickRedrawCell(CenterCol, _EndRowForValue);
	}
}

void VtOrderPanelGrid::ShowCenterValueForOption()
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;

	VtSymbol* sym = _CenterWnd->Symbol();

	CUGCell cell;



	int endValue = sym->Quote.intClose;
	int endRow = _EndRowForValue - 1;
	int zeroRow = _EndRowForValue;
	if (_CloseRow < endRow) {
		for (int r = _CloseRow; r < endRow; ++r) {
			if (endValue == 0) {
				zeroRow = r;
				break;
			}
			if (endValue <= 1000)
				endValue -= sym->intTickSize;
			else
				endValue -= 5;
		}

		if (zeroRow < endRow) {
			_CloseRow = endRow - (zeroRow - _CloseRow);
			SetCenterRow(_CloseRow);
		}
	}

	int startValue = sym->Quote.intClose;
	if (_CloseRow > _StartRowForValue) {
		for (int r = _CloseRow; r > _StartRowForValue; --r) {
			if (startValue < 1000)
				startValue += sym->intTickSize;
			else
				startValue += 5;
		}
	}
	else if (_CloseRow <= _StartRowForValue) {
		for (int r = _CloseRow; r < _StartRowForValue; ++r) {
			if (startValue <= 1000)
				startValue -= sym->intTickSize;
			else
				startValue -= 5;
		}
	}
	RowToValueMap.clear();
	int curVal = startValue;
	for (int i = _StartRowForValue; i < _EndRowForValue; ++i) {
		GetCell(CenterCol, i, &cell);
		std::string strVal = NumberFormatter::format(curVal / std::pow(10, sym->Decimal), sym->Decimal);
		cell.SetText(strVal.c_str());
		cell.LongValue(curVal);
		RowToValueMap[curVal] = i;
		SetCell(CenterCol, i, &cell);
		if (curVal > 1000)
			curVal = curVal - 5;
		else
			curVal = curVal - sym->intTickSize;
	}
}

void VtOrderPanelGrid::ShowCenterValueForOption(CellPosMap& refreshMap)
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;

	VtSymbol* sym = _CenterWnd->Symbol();

	CUGCell cell;



	int endValue = sym->Quote.intClose;
	int endRow = _EndRowForValue - 1;
	int zeroRow = _EndRowForValue;
	if (_CloseRow < endRow) {
		for (int r = _CloseRow; r < endRow; ++r) {
			if (endValue == 0) {
				zeroRow = r;
				break;
			}
			if (endValue <= 1000)
				endValue -= sym->intTickSize;
			else
				endValue -= 5;
		}

		if (zeroRow < endRow) {
			_CloseRow = endRow - (zeroRow - _CloseRow);
			SetCenterRow(_CloseRow);
		}
	}

	int startValue = sym->Quote.intClose;
	if (_CloseRow > _StartRowForValue) {
		for (int r = _CloseRow; r > _StartRowForValue; --r) {
			if (startValue < 1000)
				startValue += sym->intTickSize;
			else
				startValue += 5;
		}
	} else if (_CloseRow <= _StartRowForValue) {
		for (int r = _CloseRow; r < _StartRowForValue; ++r) {
			if (startValue <= 1000)
				startValue -= sym->intTickSize;
			else
				startValue -= 5;
		}
	}
	RowToValueMap.clear();
	int curVal = startValue;
	for (int i = _StartRowForValue; i < _EndRowForValue; ++i) {
		GetCell(CenterCol, i, &cell);
		std::string strVal = NumberFormatter::format(curVal / std::pow(10, sym->Decimal), sym->Decimal);
		cell.SetText(strVal.c_str());
		cell.LongValue(curVal);
		RowToValueMap[curVal] = i;
		SetCell(CenterCol, i, &cell);
		if (curVal > 1000)
			curVal = curVal - 5;
		else
			curVal = curVal - sym->intTickSize;
		VtCellPos pos;
		pos.Col = CenterCol;
		pos.Row = i;
		refreshMap[pos] = pos;
	}
}

void VtOrderPanelGrid::ShowCenterValueForOptionFixed()
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;

	_CloseRow = FindCenterRow();
	_CloseRow = -1 * _CloseRow;

	VtSymbol* sym = _CenterWnd->Symbol();

	CUGCell cell;

	int Row10 = 0;
	if (1000 <= sym->Quote.intClose) {
		int delta = sym->Quote.intClose - 1000;
		int deltaRow = delta / 5;
		Row10 = _CloseRow - deltaRow;
	} else {
		int delta = 1000 - sym->Quote.intClose;
		int deltaRow = delta / sym->intTickSize;
		Row10 = _CloseRow + deltaRow;
	}

	int startValue = 0;
	if (_StartRowForValue >= Row10) {
		startValue = (_StartRowForValue - Row10) * 5 + 1000;
	} else {
		startValue = 1000 - (Row10 - _StartRowForValue) * sym->intTickSize;
	}

	RowToValueMap.clear();
	int curVal = startValue;
	for (int i = _StartRowForValue; i < _EndRowForValue; ++i) {
		GetCell(CenterCol, i, &cell);
		std::string strVal = NumberFormatter::format(curVal / std::pow(10, sym->Decimal), sym->Decimal);
		cell.SetText(strVal.c_str());
		cell.LongValue(curVal);
		RowToValueMap[curVal] = i;
		SetCell(CenterCol, i, &cell);
		if (curVal > 1000)
			curVal = curVal - 5;
		else
			curVal = curVal - sym->intTickSize;
	}
}

void VtOrderPanelGrid::RefreshCenterValue()
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;
	VtSymbol* sym = _CenterWnd->Symbol();

	CUGCell cell;
	CUGCell firstCell;
	GetCell(CenterCol, _StartRowForValue, &firstCell);
	// Set the close value on the center of the center grid
	int startValue = firstCell.LongValue();

	int rangeStart = std::max(_HighRow - 1, _StartRowForValue);
	int rangeEnd = std::min(_LowRow + 1, _EndRowForValue);

	for (int i = _StartRowForValue; i < _EndRowForValue; i++) {
		GetCell(CenterCol, i, &cell);
		int curValue = startValue - sym->intTickSize * (i - 1);
		//cell.SetNumberDecimals(sym->IntDecimal);
		//cell.SetNumber(curValue / std::pow(10, sym->IntDecimal));
		std::string strVal = NumberFormatter::format(curValue / std::pow(10, sym->Decimal), sym->Decimal);
		cell.SetText(strVal.c_str());
		cell.LongValue(curValue);
		SetCell(CenterCol, i, &cell);
		//if (i < rangeStart && i > rangeEnd)
		//	QuickRedrawCell(CenterCol, i);
	}
}

void VtOrderPanelGrid::RefreshCenterValue(CellPosMap& refreshMap)
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;
	VtSymbol* sym = _CenterWnd->Symbol();

	CUGCell cell;
	CUGCell firstCell;
	GetCell(CenterCol, _StartRowForValue, &firstCell);
	// Set the close value on the center of the center grid
	int startValue = firstCell.LongValue();

	int rangeStart = std::max(_HighRow - 1, _StartRowForValue);
	int rangeEnd = std::min(_LowRow + 1, _EndRowForValue);

	for (int i = _StartRowForValue; i < _EndRowForValue; i++) {
		GetCell(CenterCol, i, &cell);
		int curValue = startValue - sym->intTickSize * (i - 1);
		std::string strVal = NumberFormatter::format(curValue / std::pow(10, sym->Decimal), sym->Decimal);
		CString thVal = XFormatNumber(strVal.c_str(), sym->Decimal);
		cell.SetText(thVal);
		cell.LongValue(curValue);
		SetCell(CenterCol, i, &cell);
		VtCellPos pos;
		pos.Col = CenterCol;
		pos.Row = i;
		refreshMap[pos] = pos;
	}
}

void VtOrderPanelGrid::ShowCenterValueByFixed()
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;
	VtSymbol* sym = _CenterWnd->Symbol();

	std::string code = sym->ShortCode.substr(0, 1);
	if ((code.compare(_T("2")) == 0) || (code.compare(_T("3")) == 0)) {
		ShowCenterValueForOptionFixed();
		return;
	}

	CUGCell cell;
	_CloseRow = FindCenterRow();
	// Set the close value on the center of the center grid
	int startValue = sym->Quote.intClose + (sym->intTickSize * (_CloseRow - _StartRowForValue));

	int rangeStart = std::max(_HighRow - 1, _StartRowForValue);
	int rangeEnd = std::min(_LowRow + 1, _EndRowForValue);
	RowToValueMap.clear();
	for (int i = _StartRowForValue; i < _EndRowForValue; i++) {
		GetCell(CenterCol, i, &cell);
		int curValue = startValue - sym->intTickSize * (i - 1);
		//cell.SetNumberDecimals(sym->IntDecimal);
		//cell.SetNumber(curValue / std::pow(10, sym->IntDecimal));
		std::string strVal = NumberFormatter::format(curValue / std::pow(10, sym->Decimal), sym->Decimal);
		cell.SetText(strVal.c_str());
		cell.LongValue(curValue);
		SetCell(CenterCol, i, &cell);
		//if (i < rangeStart && i > rangeEnd)
		//	QuickRedrawCell(CenterCol, i);
		RowToValueMap[curValue] = i;
	}
}

void VtOrderPanelGrid::ShowCenterValueByFixed(CellPosMap& refreshMap)
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;
	VtSymbol* sym = _CenterWnd->Symbol();

	std::string code = sym->ShortCode.substr(0, 1);
	if ((code.compare(_T("2")) == 0) || (code.compare(_T("3")) == 0))
	{
		ShowCenterValueForOptionFixed();
		return;
	}

	CUGCell cell;
	_CloseRow = FindCenterRow();
	// Set the close value on the center of the center grid
	int startValue = sym->Quote.intClose + (sym->intTickSize * (_CloseRow - _StartRowForValue));

	int rangeStart = std::max(_HighRow - 1, _StartRowForValue);
	int rangeEnd = std::min(_LowRow + 1, _EndRowForValue);
	RowToValueMap.clear();
	for (int i = _StartRowForValue; i < _EndRowForValue; i++)
	{
		GetCell(CenterCol, i, &cell);
		int curValue = startValue - sym->intTickSize * (i - 1);
		//cell.SetNumberDecimals(sym->IntDecimal);
		//cell.SetNumber(curValue / std::pow(10, sym->IntDecimal));
		std::string strVal = NumberFormatter::format(curValue / std::pow(10, sym->Decimal), sym->Decimal);
		cell.SetText(strVal.c_str());
		cell.LongValue(curValue);
		SetCell(CenterCol, i, &cell);
		//if (i < rangeStart && i > rangeEnd)
		//	QuickRedrawCell(CenterCol, i);
		RowToValueMap[curValue] = i;

		VtCellPos pos;
		pos.Col = CenterCol;
		pos.Row = i;
		refreshMap[pos] = pos;
	}
}

void VtOrderPanelGrid::OnReceiveHoga(VtSymbol* sym)
{
	auto start = high_resolution_clock::now();

	auto duration = duration_cast<microseconds>(start - _HogaTime);

	_HogaTime = start;

	if (_CenterWnd->UseHogaSiseFilter() && duration.count() < DelayTime) {
		CommitRefresh(5);
	}
	else {
		RefreshInfo(5);
	}
}

void VtOrderPanelGrid::OnReceiveHoga()
{
	ClearOldHogas();
	SetHogaInfo();
	ShowHogaInfo();
}

void VtOrderPanelGrid::OnReceiveQuote(VtSymbol* sym)
{
	if (!sym || !_CenterWnd || !_CutMgr) {
		_ShowingCells = false;
		return;
	}
	// 잔고에 대하여 익절과 손절을 확인한다.
	//_CutMgr->CheckProfitLoss(sym);
	// 익절, 손절 스탑 주문이 종가에 닿았는지 검사한다.
	CheckProfitLossTouchHd(sym->Quote.intClose);
	// 스탑 주문이 종가에 닿았는지 검사한다.
	CheckStopTouchedHd(sym->Quote.intClose);

	auto start = high_resolution_clock::now();

	auto duration = duration_cast<microseconds>(start - _QuoteTime);

	_QuoteTime = start;

	if (_CenterWnd->UseHogaSiseFilter() && duration.count() < DelayTime) {
		CommitRefresh(4);
	}
	else {
		RefreshInfo(4);
	}
}

void VtOrderPanelGrid::OnReceiveQuote()
{
	ClearOldQuotes();
	SetCenterValue();
	SetQuoteInfo();
	ShowQuoteInfo();
}

void VtOrderPanelGrid::OnSymbolMaster(VtSymbol* sym)
{
	if (!sym || !_CenterWnd)
		return;

	_CloseRow = FindCenterRow();

	CellPosMap refreshMap;

	ClearOldQuotes(refreshMap);
	ClearOldHogas(refreshMap);
	ClearOldOrders(refreshMap);
	ClearOldStopOrders(refreshMap);
	ClearOldCloseLine(refreshMap);

	SetCenterValue(refreshMap);
	SetQuoteInfo(refreshMap);
	SetPositionInfo(refreshMap);

	SetHogaInfo(refreshMap);

	SetOrderInfo(refreshMap);

	SetStopOrderInfo(refreshMap);
	SetCloseLine(refreshMap);

	SetOrderArea();

	ShowLowerCtrls(refreshMap);

	ShowTotalHoga(refreshMap);
	ShowTotalOrder(refreshMap);
	ShowTotalStopOrder(refreshMap);

	RefreshCells(refreshMap);
}

void VtOrderPanelGrid::OnOrderReceivedHd(VtOrder* order)
{

}

void VtOrderPanelGrid::OnOrderAcceptedHd(VtOrder* order)
{
	RefreshInfo(2);
}

void VtOrderPanelGrid::OnOrderUnfilledHd(VtOrder* order)
{
	RefreshInfo(3);
}

// 익절과 손절은 체결시 스탑주문으로만 나간다.
// 이미 나가 있는 주문에 대해서는 버튼을 눌러 스탑을 걸어야 한다.
// 익절, 손절을 체크했더라도 명시적으로 스탑을 누르지 않으면 익절 손절은 적용되지 않는다.
void VtOrderPanelGrid::OnOrderFilledHd(VtOrder* order)
{
	if (!order)
		return;
 	_CutMgr->AddStopOrderForFilled(_CenterWnd->Symbol(), order);
	RefreshInfo(1);
}

void VtOrderPanelGrid::SetOrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
	_CutMgr->OrderConfigMgr(val);
}

BEGIN_MESSAGE_MAP(VtOrderPanelGrid, VtGrid)
	ON_WM_TIMER()
END_MESSAGE_MAP()


void VtOrderPanelGrid::OnTimer(UINT_PTR nIDEvent)
{
	if (!_CenterWnd || !_CenterWnd->Symbol())
		return;
	if (nIDEvent == RefreshTimer) {
		if (_RefreshQ.size() == 0) {
			_IsRefreshTimerWorking = false;
			StopTimer();
		}
		else {
			_RefreshQ.pop();
			RefreshInfo();
		}
	}
	VtGrid::OnTimer(nIDEvent);
}

void VtOrderPanelGrid::StopTimer()
{
	KillTimer(RefreshTimer);
}

void VtOrderPanelGrid::AddRefresh(int data)
{
	_RefreshBuf.put(data);
	//SetTimer(RefreshTimer, 1, NULL);
}
