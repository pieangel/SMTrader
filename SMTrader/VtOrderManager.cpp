#include "stdafx.h"
#include "VtOrderManager.h"
#include "VtKrClient.h"
#include "VtOrder.h"
#include "VtAccount.h"
#include "VtChartData.h"
#include "VtChartManager.h"
#include "VtChartFrm.h"
#include "VtSymbolManager.h"
#include "VtSymbol.h"
#include "VtQuoteManager.h"
#include "VtQuote.h"
#include "VtSymbolMaster.h"
#include "Poco/NumberParser.h"
#include "Poco/Delegate.h"
#include "VtWindowManager.h"
//#include "PreOrderWnd.h"
#include "MainFrm.h"
//#include "AcceptedOrderWnd.h"
#include "VtPosition.h"
#include "VtAccountManager.h"
#include "VtChartManager.h"
#include "VtChartFrm.h"
#include "VtOrderCenterWnd.h"
#include "VtProductOrderManagerSelector.h"
#include "VtProductOrderManager.h"
#include "VtOrderDialogManager.h"
#include "VtHdClient.h"
#include "VtFundManager.h"
#include "VtFund.h"
#include "VtOrderQueueManager.h"
#include "VtTotalOrderManager.h"

using Poco::NumberParser;
using Poco::Delegate;

void VtOrderManager::RequestFilledOrder(VtAccount* acnt)
{
	VtKrClient* client = VtKrClient::GetInstance();
	client->RequestFilledOrder(acnt->AccountNo);
	_CurAccount = nullptr;
}

void VtOrderManager::RequestFilledOrder(std::string acntNo)
{
	VtKrClient* client = VtKrClient::GetInstance();
	client->RequestFilledOrder(acntNo);
}

void VtOrderManager::RequestFilledOrder(std::string acntNo, int Range, int OrderBy)
{
	VtKrClient* client = VtKrClient::GetInstance();
	client->RequestFilledOrder(acntNo, Range, OrderBy);
}

void VtOrderManager::RequestOrderList(VtTaskAccountFilledArg* arg)
{
	if (!arg) return;


}

void VtOrderManager::ReceiveFilledOrder()
{
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtProductOrderManager* prdtOrderMgr = nullptr;
	for (auto it = OrderMap.begin(); it != OrderMap.end(); ++it)
	{
		VtOrder* order = it->second;
		if (order->state == VtOrderState::Accepted)
		{
			//AcceptedList.push_back(order);
			AcceptedMap[order->orderNo] = order;
			prdtOrderMgr = FindAddProductOrderManager(order->fullCode);

			if (prdtOrderMgr)
			{
				prdtOrderMgr->AddAccepted(order);
			}
		}
	}

	//symMgr->UpdateSymbolMaster();
	//AddAccepted(order);
	VtWindowManager* wndMgr = VtWindowManager::GetInstance();
	//CAcceptedOrderWnd* wnd = (CAcceptedOrderWnd*)wndMgr->FindWindow(_T("Accepted"));
	//wnd->RefreshOrders();
}

VtOrderManager::VtOrderManager()
{
	_ProductOrderManagerSelector = new VtProductOrderManagerSelector();
	VtWindowManager* wndMgr = VtWindowManager::GetInstance();
	//CPreOrderWnd* wnd = (CPreOrderWnd*)wndMgr->FindWindow(_T("PreOrder"));
	//_PreOrderCreatedEvent += delegate(wnd, &CPreOrderWnd::OnPreOrderReceived);
	_CurAccount = nullptr;
}


VtOrderManager::~VtOrderManager()
{
	if (_ProductOrderManagerSelector)
	{
		delete _ProductOrderManagerSelector;
		_ProductOrderManagerSelector = nullptr;
	}

	VtWindowManager* wndMgr = VtWindowManager::GetInstance();
	//CPreOrderWnd* wnd = (CPreOrderWnd*)wndMgr->FindWindow(_T("PreOrder"));
	//_PreOrderCreatedEvent -= delegate(wnd, &CPreOrderWnd::OnPreOrderReceived);

	ClearOrderMap();

	for (auto it = PreOrderList.begin(); it != PreOrderList.end(); ++it)
	{
		delete *it;
	}

	for (auto it = FilledMap.begin(); it != FilledMap.end(); ++it)
	{
		delete it->second;
	}

	ClearFilledList();
}

void VtOrderManager::ClearOrderMap()
{
	for (auto it = OrderMap.begin(); it != OrderMap.end(); ++it)
	{
		delete it->second;
	}

	OrderMap.clear();
}

void VtOrderManager::ClearFilledList()
{
	for (auto it = FilledList.begin(); it != FilledList.end(); ++it)
	{
		delete *it;
	}
}


void VtOrderManager::ClearAcceptedList()
{
	AcceptedMap.clear();

	if (_ProductOrderManagerSelector)
		_ProductOrderManagerSelector->ClearAllAccepted();
}

void VtOrderManager::Buy(int amount)
{
	if (!_CurAccount)
		return;

	VtOrderRequest request;
	request.amount = amount;
	VtAccount* acnt = _CurAccount;
	if (acnt)
	{
		request.accountNo = acnt->AccountNo;
		request.password = acnt->Password;
	}
	else
		return;
	VtChartManager* chartMgr = VtChartManager::GetInstance();
	VtChartFrm* chartFrm = chartMgr->ActiveChartFrm();
	if (chartFrm)
	{
		request.chartID = chartFrm->Id();
		VtChartData* chartData = chartFrm->GetChartData();
		if (chartData)
		{
			VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
			VtSymbol* sym = symMgr->FindSymbolByFullCode(chartData->SymbolCode());
			VtSymbolMaster* symMaster = symMgr->FindSymbolMasterByFullCode(sym->FullCode);
			request.shortCode = sym->ShortCode;
			request.productDecimal = symMaster->decimal;
			VtQuoteManager* quoteMgr = VtQuoteManager::GetInstance();
			VtRealtimeQuote* quote = quoteMgr->FindRealtimeQuote(sym->FullCode);
			double closePrice = 0.0;
			if (quote)
			{
				std::string close;
				close.assign(quote->ClosePrice, sizeof(quote->ClosePrice));
				if (NumberParser::tryParseFloat(close, closePrice))
				{
					request.orderPrice = closePrice;
				}
				else
					request.orderPrice = closePrice;
			}
			else
				request.orderPrice = 0.00;
		}
		else
			return;
	}
	else
		return;
	request.requestID = GetOrderRequestID();
	request.orderType = VtOrderType::New;
	request.orderPosition = VtPositionType::Buy;
	request.priceType = VtPriceType::Market;
	request.tradeType = VtTradeType::Market;
	request.oriOrderNo = 0;

	PutOrder(std::move(request));
}

void VtOrderManager::Sell(int amount)
{
	if (!_CurAccount)
		return;
	VtOrderRequest request;
	request.amount = amount;
	VtAccount* acnt = _CurAccount;
	if (acnt)
	{
		request.accountNo = acnt->AccountNo;
		request.password = acnt->Password;
	}
	else
		return;

	VtChartManager* chartMgr = VtChartManager::GetInstance();
	VtChartFrm* chartFrm = chartMgr->ActiveChartFrm();
	if (chartFrm)
	{
		request.chartID = chartFrm->Id();
		VtChartData* chartData = chartFrm->GetChartData();
		if (chartData)
		{
			VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
			VtSymbol* sym = symMgr->FindSymbolByFullCode(chartData->SymbolCode());
			VtSymbolMaster* symMaster = symMgr->FindSymbolMasterByFullCode(sym->FullCode);
			request.shortCode = sym->ShortCode;
			request.productDecimal = symMaster->decimal;
			VtQuoteManager* quoteMgr = VtQuoteManager::GetInstance();
			VtRealtimeQuote* quote = quoteMgr->FindRealtimeQuote(sym->FullCode);
			double closePrice = 0.0;
			if (quote)
			{
				std::string close;
				close.assign(quote->ClosePrice, sizeof(quote->ClosePrice));
				if (NumberParser::tryParseFloat(close, closePrice))
				{
					request.orderPrice = closePrice;
				}
				else
					request.orderPrice = closePrice;
			}
			else
				request.orderPrice = 0.00;
		}
		else
			return;
	}
	else
		return;

	request.requestID = GetOrderRequestID();
	request.orderType = VtOrderType::New;
	request.orderPosition = VtPositionType::Sell;
	request.priceType = VtPriceType::Market;
	request.tradeType = VtTradeType::Market;
	request.oriOrderNo = 0;
	PutOrder(std::move(request));
}

void VtOrderManager::PutOrder(VtOrderRequest&& request)
{
	if (!_CurAccount)
		return;

	VtOrder* order = new VtOrder();
	order->requestID = request.requestID;
	order->chartID = request.chartID;
	order->shortCode = request.shortCode;
	order->amount = request.amount;
	order->orderType = request.orderType;
	order->orderPosition = request.orderPosition;
	order->priceType = request.priceType;
	order->orderPrice = ROUNDING(request.orderPrice, 2);

	AddPreOrder(order);
	//AddPreOrderToChart(order);
	SendPreOrderEvent(order);
	VtKrClient* client = VtKrClient::GetInstance();
	client->PutOrder(std::move(request));
}

void VtOrderManager::PutOrder(int chartId, VtOrderCmd cmd, int qty, double price, VtSymbolMaster* symMstr)
{
	if (!_CurAccount)
		return;
	if (!symMstr)
		return;

	VtOrderRequest request;
	request.amount = qty;
	VtAccount* acnt = _CurAccount;
	if (acnt)
	{
		request.accountNo = acnt->AccountNo;
		request.password = acnt->Password;
	}
	else
		return;

	request.shortCode = symMstr->ShortCode;
	request.productDecimal = symMstr->decimal;
	request.chartID = chartId;
	request.orderPrice = price;
	request.requestID = GetOrderRequestID();
	request.orderType = VtOrderType::New;
	request.oriOrderNo = 0;

	switch (cmd)
	{
	case VtOrderCmd::PreBuy:
		request.orderPosition = VtPositionType::Buy;
		request.priceType = VtPriceType::Market;
		request.tradeType = VtTradeType::Market;
		break;
	case VtOrderCmd::PreSell:
		request.orderPosition = VtPositionType::Sell;
		request.priceType = VtPriceType::Market;
		request.tradeType = VtTradeType::Market;
		break;
	case VtOrderCmd::BuyAtPrice:
		request.orderPosition = VtPositionType::Buy;
		request.priceType = VtPriceType::Price;
		request.tradeType = VtTradeType::Price;
		break;
	case VtOrderCmd::SellAtPrice:
		request.orderPosition = VtPositionType::Sell;
		request.priceType = VtPriceType::Price;
		request.tradeType = VtTradeType::Price;
		break;
	case VtOrderCmd::BuyAtMarket:
		request.orderPosition = VtPositionType::Buy;
		request.priceType = VtPriceType::Market;
		request.tradeType = VtTradeType::Market;
		break;
	case VtOrderCmd::SellAtMarket:
		request.orderPosition = VtPositionType::Sell;
		request.priceType = VtPriceType::Market;
		request.tradeType = VtTradeType::Market;
		break;
	}

	PutOrder(std::move(request));
}

void VtOrderManager::PutOrder(HdOrderRequest&& request)
{
	VtHdClient* client = VtHdClient::GetInstance();
	client->PutOrder(std::move(request));
	//VtOrderQueueManager* orderQMgr = VtOrderQueueManager::GetInstance();
	//orderQMgr->AddOrderRequest(std::move(request));
	//orderQMgr->Ready(true);
}

void VtOrderManager::ChangeOrder(VtOrder* oldOrder, double newValue)
{
	if (!_CurAccount)
		return;

	if (!oldOrder) return;
	VtOrderRequest request;
	FillRequest(std::move(request), oldOrder);
	request.requestID = GetOrderRequestID();
	request.orderType = VtOrderType::Change;
	request.orderPrice = newValue;
	request.amount = oldOrder->amount;
	request.oriOrderNo = oldOrder->orderNo;
	VtAccount* acnt = _CurAccount;
	if (acnt)
	{
		request.accountNo = acnt->AccountNo;
		request.password = acnt->Password;
	}
	else
		return;

	VtOrder* order = new VtOrder();
	order->requestID = request.requestID;
	order->chartID = request.chartID;
	order->shortCode = request.shortCode;
	order->amount = request.amount;
	order->orderType = request.orderType;
	order->orderPosition = request.orderPosition;
	order->priceType = request.priceType;
	order->orderPrice = ROUNDING(request.orderPrice, 2);
	order->oriOrderNo = request.oriOrderNo;

	AddPreOrder(order);
	VtKrClient* client = VtKrClient::GetInstance();
	client->ChangeOrder(std::move(request));
}

void VtOrderManager::ChangeOrder(HdOrderRequest&& request)
{
	VtHdClient* client = VtHdClient::GetInstance();
	client->ChangeOrder(std::move(request));
	//VtOrderQueueManager* orderQMgr = VtOrderQueueManager::GetInstance();
	//orderQMgr->AddOrderRequest(std::move(request));
	//orderQMgr->Ready(true);
}

void VtOrderManager::CancelOrder(VtOrder* oldOrder)
{
	if (!_CurAccount)
		return;
	if (!oldOrder) return;

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	int decimal = symMgr->GetSymbolDecimal(oldOrder->shortCode);

	VtOrderRequest request;
	FillRequest(std::move(request), oldOrder);
	request.requestID = GetOrderRequestID();
	request.orderType = VtOrderType::Cancel;
	request.orderPrice = oldOrder->orderPrice;
	request.amount = oldOrder->amount;
	request.oriOrderNo = oldOrder->orderNo;
	request.productDecimal = decimal;
	VtAccount* acnt = _CurAccount;
	if (acnt)
	{
		request.accountNo = acnt->AccountNo;
		request.password = acnt->Password;
	}
	else
		return;

	VtOrder* order = new VtOrder();
	order->requestID = request.requestID;
	order->chartID = request.chartID;
	order->shortCode = request.shortCode;
	order->amount = request.amount;
	order->orderType = request.orderType;
	order->orderPosition = request.orderPosition;
	order->priceType = request.priceType;
	order->orderPrice = ROUNDING(request.orderPrice, 2);
	order->oriOrderNo = request.oriOrderNo;

	AddPreOrder(order);
	VtKrClient* client = VtKrClient::GetInstance();
	client->CancelOrder(std::move(request));
}

void VtOrderManager::CancelOrder(HdOrderRequest&& request)
{
	VtHdClient* client = VtHdClient::GetInstance();
	client->CancelOrder(std::move(request));
	//VtOrderQueueManager* orderQMgr = VtOrderQueueManager::GetInstance();
	//orderQMgr->AddOrderRequest(std::move(request));
	//orderQMgr->Ready(true);
}

std::vector<VtOrder*> VtOrderManager::GetAcceptedOrders(std::string symbolCode)
{
	std::vector<VtOrder*> acptList;
	VtProductOrderManager* prdtOrderMgr = _ProductOrderManagerSelector->Find(symbolCode);
	if (!prdtOrderMgr)
		return acptList;
	
	for (auto it = prdtOrderMgr->AcceptedMap.begin(); it != prdtOrderMgr->AcceptedMap.end(); ++it)
	{
		acptList.push_back(it->second);
	}

	return acptList;
}

void VtOrderManager::GetAcceptedOrders(std::string symbolCode, std::vector<VtOrder*>& orderVector)
{
	VtProductOrderManager* prdtOrderMgr = _ProductOrderManagerSelector->Find(symbolCode);
	if (!prdtOrderMgr)
		return;

	for (auto it = prdtOrderMgr->AcceptedMap.begin(); it != prdtOrderMgr->AcceptedMap.end(); ++it)
	{
		orderVector.push_back(it->second);
	}
}

void VtOrderManager::AddOrder(VtOrder* order)
{
	if (!order)
		return;
	OrderMap[order->orderNo] = order;
}

void VtOrderManager::AddPreOrder(VtOrder* order)
{
	PreOrderList.push_back(order);
}

void VtOrderManager::AddAccepted(VtOrder* order)
{
	//AcceptedList.push_back(order);
	if (!order)
		return;

	AcceptedMap[order->orderNo] = order;

	VtTotalOrderManager* totalOrderMgr = VtTotalOrderManager::GetInstance();
	totalOrderMgr->AddAcceptedOrder(order);
}

void VtOrderManager::AddFilled(VtOrder* order)
{
	auto it = FilledMap.find(order->shortCode);
	if (it != FilledMap.end())
	{
		std::list<VtOrder*>* filledList = it->second;
		HandleFilledOrder(filledList, order);
	}
	else
	{
		std::list<VtOrder*>* filledList = new std::list<VtOrder *>();
		filledList->push_back(order);
		FilledMap[order->shortCode] = filledList;
	}
}


VtOrder* VtOrderManager::RemovePreOrder(int orderRequestID)
{
	VtOrder* order = nullptr;
	for (auto it = PreOrderList.begin(); it != PreOrderList.end(); ++it)
	{
		order = *it;
		if (order->requestID == orderRequestID)
		{
			PreOrderList.erase(it);
			break;
		}
	}

	return order;
}

void VtOrderManager::RemoveAccepted(int orderNo)
{
	for (auto it = AcceptedMap.begin(); it != AcceptedMap.end(); ++it) {
		VtOrder* order = it->second;
		if (order->orderNo == orderNo) {
			AcceptedMap.erase(it);
			break;
		}
	}

	VtTotalOrderManager* totalOrderMgr = VtTotalOrderManager::GetInstance();
	totalOrderMgr->RemoveAcceptedOrder(orderNo);
}

void VtOrderManager::RemoveFilled(int orderNo)
{
	for (auto it = FilledList.begin(); it != FilledList.end(); ++it) {
		VtOrder* order = *it;
		if (order->orderNo == orderNo) {
			FilledList.erase(it);
			break;
		}
	}
}

VtOrder* VtOrderManager::FindPreOrderByRequestID(int requestID)
{
	for (auto it = PreOrderList.begin(); it != PreOrderList.end(); ++it)
	{
		VtOrder* order = *it;
		if (order->requestID == requestID)
		{
			return order;
		}
	}

	return nullptr;
}

VtOrder* VtOrderManager::FindAccepted(int orderNo)
{
	auto it = AcceptedMap.find(orderNo);
	if (it != AcceptedMap.end())
		return it->second;
	else
		return nullptr;
}

VtOrder* VtOrderManager::FindFilled(int orderNo)
{
	for (auto it = FilledList.begin(); it != FilledList.end(); ++it)
	{
		VtOrder* order = *it;
		if (order->orderNo == orderNo)
		{
			return order;
		}
	}

	return nullptr;
}

VtOrder* VtOrderManager::FindOrder(int orderNo)
{
	auto it = OrderMap.find(orderNo);
	if (it != OrderMap.end())
	{
		return it->second;
	}

	return nullptr;
}

VtOrder* VtOrderManager::CloneOrder(VtOrder* src)
{
	VtOrder* dst = new VtOrder();
	dst->AccountNo = src->AccountNo;
	dst->amount = src->amount;
	dst->chartID = src->chartID;
	dst->filledPrice = src->filledPrice;
	dst->filledQty = src->filledQty;
	dst->orderPrice = src->orderPrice;
	dst->orderNo = src->orderNo;
	dst->orderType = src->orderType;
	dst->oriOrderNo = src->oriOrderNo;
	dst->orderPosition = src->orderPosition;
	dst->priceType = src->priceType;
	dst->shortCode = src->shortCode;
	dst->unacceptedQty = src->unacceptedQty;
	dst->requestID = src->requestID;
	dst->state = src->state;
	//dst->stopPrice = src->stopPrice;
	dst->tradeCondition = src->tradeCondition;
	dst->tradeDate = src->tradeDate;
	dst->TradeNo = src->TradeNo;
	dst->tradeTime = src->tradeTime;
	dst->tradeType = src->tradeType;
	dst->SubAccountNo = src->SubAccountNo;
	dst->FundName = src->FundName;
	dst->intOrderPrice = src->intOrderPrice;
	dst->intFilledPrice = src->intFilledPrice;
	return dst;
}

void VtOrderManager::CopyOrder(VtOrder* src, VtOrder* dst)
{
	if (!src || !dst)
		return;

	dst->AccountNo = src->AccountNo;
	dst->amount = src->amount;
	dst->chartID = src->chartID;
	dst->filledPrice = src->filledPrice;
	dst->filledQty = src->filledQty;
	dst->orderPrice = src->orderPrice;
	dst->orderNo = src->orderNo;
	dst->orderType = src->orderType;
	dst->oriOrderNo = src->oriOrderNo;
	dst->orderPosition = src->orderPosition;
	dst->priceType = src->priceType;
	dst->shortCode = src->shortCode;
	dst->unacceptedQty = src->unacceptedQty;
	dst->requestID = src->requestID;
	dst->state = src->state;
	//dst->stopPrice = src->stopPrice;
	dst->tradeCondition = src->tradeCondition;
	dst->tradeDate = src->tradeDate;
	dst->TradeNo = src->TradeNo;
	dst->tradeTime = src->tradeTime;
	dst->tradeType = src->tradeType;
	dst->SubAccountNo = src->SubAccountNo;
	dst->FundName = src->FundName;
	dst->intOrderPrice = src->intOrderPrice;
	dst->intFilledPrice = src->intFilledPrice;
}

void VtOrderManager::FillRequest(VtOrderRequest&& request, VtOrder* order)
{
	if (!order) return;

	request.accountNo = order->AccountNo;
	request.chartID = order->chartID;
	request.amount = order->amount;
	request.orderPrice = order->orderPrice;
	request.orderType = order->orderType;
	request.oriOrderNo = order->oriOrderNo;
	request.orderPosition = order->orderPosition;
	request.priceType = order->priceType;
	request.shortCode = order->shortCode;
	request.productDecimal = order->productDecimal;
	request.requestID = order->requestID;
	request.tradeType = order->tradeType;
}

void VtOrderManager::FillRequest(VtOrderDirectRequest&& request, VtOrder* order)
{
	if (!order) return;

	request.accountNo = order->AccountNo;
	request.chartID = order->chartID;
	request.amount = order->amount;
	request.orderPrice = std::to_string(order->orderPrice);
	request.orderType = order->orderType;
	request.oriOrderNo = order->oriOrderNo;
	request.orderPosition = order->orderPosition;
	request.priceType = order->priceType;
	request.shortCode = order->shortCode;
	request.productDecimal = order->productDecimal;
	request.requestID = order->requestID;
	request.tradeType = order->tradeType;
}

void VtOrderManager::RemoveOriginalOrder(int oldOrderNo)
{
	auto it = AcceptedMap.find(oldOrderNo);
	if (it != AcceptedMap.end())
	{
		AcceptedMap.erase(it);
	}
}

void VtOrderManager::SetOrderInfo(VtOrder* order, VtRealtimeOrder&& real)
{

}

void VtOrderManager::AddAcceptedFromList(VtOrder* order)
{
	if (!order)
		return;

	AcceptedMap[order->orderNo] = order;
}

void VtOrderManager::HandleFilledOrder(std::list<VtOrder*>* filledList, VtOrder* curOrder)
{
	if (!curOrder || !filledList)
		return;
	// 체결 목록에 아무것도 없는 경우는 그냥 추가하고 나간다.
	if (filledList->size() == 0)
	{
		filledList->push_back(curOrder);
		return;
	}


	//std::list<VtOrder *>::iterator it = filledList.begin();
	//std::list<VtOrder *>::iterator end = filledList.end();

	
	for(auto it = filledList->begin(); it != filledList->end();)
	{
		VtOrder* order = *it;
		// 남아 있는 주문과 같은 종류의 주문일 경우 리스트에 추가하고 나간다.
		if (curOrder->orderPosition == order->orderPosition)
		{
			filledList->push_back(curOrder);
			break;
		}
		else
		{
			// 주문 수량이 같을 경우 서로 상쇄하고 루틴을 빠져 나간다.
			if (curOrder->amount == order->amount)
			{
				filledList->erase(it);
				break;
			}
			else if (curOrder->amount > order->amount) // 현재 들어온 주문 수량이 더 큰 경우
			{
				// 현재 들어온 주문의 수량으로 기존의 주문으로 상쇄하고 루틴을 계속한다.
				curOrder->amount = curOrder->amount - order->amount;
				// 주문이 상쇄 되었으므로 주문 목록에서 없애 버린다.
				it = filledList->erase(it);
			}
			else if (curOrder->amount < order->amount)
			{
				// 기존 주문을 상쇄하고 빠져 나간다.
				order->amount = order->amount - curOrder->amount;
				break;
			}
		}
	}
}

void VtOrderManager::UpdateAccountRemain(VtOrder* order)
{
	if (!order)
		return;
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	acntMgr->UpdateAccountRemain(order);
}

void VtOrderManager::AddPreOrderToChart(VtOrder* order)
{
	if (!order) return;

	VtChartManager* chartMgr = VtChartManager::GetInstance();
	VtChartFrm* chartFrm = chartMgr->FindChartFrm(order->chartID);
	if (chartFrm)
		chartFrm->AddPreOrder(order);
}

void VtOrderManager::AddAcceptedOrderToChart(VtOrder* order)
{
	if (!order) return;

	VtChartManager* chartMgr = VtChartManager::GetInstance();
	VtChartFrm* chartFrm = chartMgr->FindChartFrm(order->chartID);
	chartFrm->OnAccepted(order);
}

void VtOrderManager::AddFilledOrderToChart(VtOrder* order)
{
	if (!order) return;

	VtChartManager* chartMgr = VtChartManager::GetInstance();
	VtChartFrm* chartFrm = chartMgr->FindChartFrm(order->chartID);
	chartFrm->OnFilled(order);
}

void VtOrderManager::DirectPutOrder(VtOrderDirectRequest&& request)
{
	if (!_CurAccount)
		return;

	VtOrder* order = new VtOrder();
	request.chartID = 0;
	request.requestID = GetOrderRequestID();

	order->requestID = request.requestID;
	order->chartID = request.chartID;
	order->shortCode = request.shortCode;
	order->amount = request.amount;
	order->orderType = request.orderType;
	order->orderPosition = request.orderPosition;
	order->priceType = request.priceType;
	order->orderPrice = std::stof(request.orderPrice);

	AddPreOrder(order);
	//AddPreOrderToChart(order);
	SendPreOrderEvent(order);
	VtKrClient* client = VtKrClient::GetInstance();
	client->PutOrder(std::move(request));
}

void VtOrderManager::DirectChangeOrder(VtOrder* oldOrder, std::string newValue)
{
	if (!oldOrder)
		return;

	if (!_CurAccount)
		return;

	if (!oldOrder) return;
	VtOrderDirectRequest request;
	FillRequest(std::move(request), oldOrder);
	request.requestID = GetOrderRequestID();
	request.orderType = VtOrderType::Change;
	request.orderPrice = newValue;
	request.amount = oldOrder->amount;
	request.oriOrderNo = oldOrder->orderNo;
	VtAccount* acnt = _CurAccount;
	if (acnt)
	{
		request.accountNo = acnt->AccountNo;
		request.password = acnt->Password;
	}
	else
		return;

	VtOrder* order = new VtOrder();
	order->requestID = request.requestID;
	order->chartID = request.chartID;
	order->shortCode = request.shortCode;
	order->amount = request.amount;
	order->orderType = request.orderType;
	order->orderPosition = request.orderPosition;
	order->priceType = request.priceType;
	order->orderPrice = std::stof(request.orderPrice);
	order->oriOrderNo = request.oriOrderNo;

	AddPreOrder(order);
	VtKrClient* client = VtKrClient::GetInstance();
	client->ChangeOrder(std::move(request));

}

void VtOrderManager::DirectCancelOrder(VtOrder* oldOrder)
{
	if (!_CurAccount)
		return;
	if (!oldOrder) return;
	VtOrderDirectRequest request;
	FillRequest(std::move(request), oldOrder);
	request.requestID = GetOrderRequestID();
	request.orderType = VtOrderType::Cancel;
	request.orderPrice = std::to_string(oldOrder->orderPrice);
	request.amount = oldOrder->amount;
	request.oriOrderNo = oldOrder->orderNo;
	VtAccount* acnt = _CurAccount;
	if (acnt)
	{
		request.accountNo = acnt->AccountNo;
		request.password = acnt->Password;
	}
	else
		return;

	VtOrder* order = new VtOrder();
	order->requestID = request.requestID;
	order->chartID = request.chartID;
	order->shortCode = request.shortCode;
	order->amount = request.amount;
	order->orderType = request.orderType;
	order->orderPosition = request.orderPosition;
	order->priceType = request.priceType;
	order->orderPrice = std::stof(request.orderPrice);
	order->oriOrderNo = request.oriOrderNo;

	AddPreOrder(order);
	VtKrClient* client = VtKrClient::GetInstance();
	client->CancelOrder(std::move(request));

}

void VtOrderManager::SendPreOrderEvent(VtOrder* pOrder)
{
	VtOrderEventArgs arg;
	arg.pOrder = pOrder;
	OnPreOrderCreated(std::move(arg));
}

void VtOrderManager::OnOrderReceived(VtOrder* rcvdOrder)
{
	if (!rcvdOrder)
		return;
	rcvdOrder->state = VtOrderState::OrderReceived;
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	rcvdOrder->fullCode = symMgr->FullCodeMap[rcvdOrder->shortCode];
	// 역전되어 오는 경우에는 이미 주문이 들어가 있으므로 주문목록에서 먼저 찾아본다.
	VtOrder* foundOrder = FindOrder(rcvdOrder->orderNo);
	if (foundOrder)
	{
		delete rcvdOrder;
		rcvdOrder = nullptr;
		rcvdOrder = foundOrder;
	}


	OrderMap[rcvdOrder->orderNo] = rcvdOrder;

	VtWindowManager* wndMgr = VtWindowManager::GetInstance();
	//CPreOrderWnd* wnd = (CPreOrderWnd*)wndMgr->FindWindow(_T("PreOrder"));
	//wnd->RefreshOrders();

	AddPreOrderToChart(rcvdOrder);

	VtOrderDialogManager* orderWndMgr = VtOrderDialogManager::GetInstance();
	orderWndMgr->OnOrderReceived(rcvdOrder);
}

void VtOrderManager::OnError(std::string errorMsg)
{
	VtWindowManager* wndMgr = VtWindowManager::GetInstance();
	//CPreOrderWnd* wnd = (CPreOrderWnd*)wndMgr->FindWindow(_T("PreOrder"));
	//wnd->ResetOrders();
	CMainFrame* mainWnd = (CMainFrame*)wndMgr->FindWindow(_T("MainFrame"));
	//mainWnd->SetStatusBarText(errorMsg);
}

void VtOrderManager::OnLedger(VtRealtimeOrder&& real)
{
	VtOrder* order = FindOrder(real.orderNo);
	if (!order)
		return;

	order->state = real.state;
	order->unacceptedQty = real.remainQty;
	order->tradeDate = real.tradeDate;
	order->tradeTime = real.tradeTime;
	order->fullCode = real.fullCode;
}

void VtOrderManager::OnAccepted(VtRealtimeOrder&& real)
{
	VtOrder* order = FindOrder(real.orderNo);
	// 이 경우는 외부에서 주문이 체결되어 온 경우이므로 별도로 처리해야 함.
	if (!order)
		return;

	order->state = real.state;
	order->unacceptedQty = real.remainQty;
	order->tradeDate = real.tradeDate;
	order->tradeTime = real.tradeTime;
	//order->strOrderPrice = real.strOrderPrice;
	order->fullCode = real.fullCode;
	if (order)
	{
		AddAccepted(order);

		VtProductOrderManager* prdtOrderMgr = _ProductOrderManagerSelector->FindAdd(order->fullCode);
		prdtOrderMgr->OnAccepted(order);
		
		VtWindowManager* wndMgr = VtWindowManager::GetInstance();
		//CAcceptedOrderWnd* wnd = (CAcceptedOrderWnd*)wndMgr->FindWindow(_T("Accepted"));
		//wnd->RefreshOrders();

		VtChartManager* chartMgr = VtChartManager::GetInstance();
		VtChartFrm* chartFrm = chartMgr->FindChartFrm(order->chartID);
		if (chartFrm)
			chartFrm->OnAccepted(order);

		VtOrderDialogManager* orderWndMgr = VtOrderDialogManager::GetInstance();
		orderWndMgr->OnAccepted(order);
	}
}

void VtOrderManager::OnFilled(VtRealtimeOrder&& real)
{
	VtOrder* order = FindOrder(real.orderNo);
	// 이미 나간 주문이 체결되어오는 경우 
	// 이 경우는 프로그램이 시작되기 전에 나가있는 주문이기 때문에 프로그램 시작시에 
	// 미체결 주문을 받아와서 시작해야 해결이 된다.
	if (!order)
		return;

	order->state = real.state;
	order->filledQty = real.filledAmount;
	order->filledPrice = real.filledPrice;
	order->filledMoney = real.filledMoney;
	order->tradeDate = real.tradeDate;
	order->tradeTime = real.tradeTime;
	order->TradeNo = real.tradeNo;
	order->tradeCondition = real.tradeCondition;
	order->remainQty = real.remainAmount;
	order->orderableAmount = real.orderableAmount;
	//order->strFilledPrice = real.strFilledPrice;
	if (order)
	{
		RemoveAccepted(order->orderNo);

		VtWindowManager* wndMgr = VtWindowManager::GetInstance();
		//CAcceptedOrderWnd* wnd = (CAcceptedOrderWnd*)wndMgr->FindWindow(_T("Accepted"));
		//wnd->RefreshOrders();

		AddFilled(order);

		VtAccountManager* acntMgr = VtAccountManager::GetInstance();
		VtAccount* acnt = acntMgr->FindAccount(order->AccountNo);
		// 여기서 현재 체결된 주문을 정리해준다.
		VtProductOrderManager* prdtOrderMgr = _ProductOrderManagerSelector->FindAdd(order->fullCode);
		// 여기서 잔고를 갱신해 준다.
		VtPosition* posi = acnt->FindAdd(order);
		prdtOrderMgr->Position(posi);
		//prdtOrderMgr->OnFilled(order, posi);
		// 계좌의 잔고를 갱신해 준다.
		UpdateAccountRemain(order);

		// 잔고목록 갱신
		//CRemainOrderWnd* remainWnd = (CRemainOrderWnd*)wndMgr->FindWindow(_T("Remain"));
		//remainWnd->ReceivedRemain();

		// 차트 갱신
		VtChartManager* chartMgr = VtChartManager::GetInstance();
		VtChartFrm* chartFrm = chartMgr->FindChartFrm(order->chartID);
		if (chartFrm)
			chartFrm->OnFilled(order);
		// 주문창에 알린다.
		VtOrderDialogManager* orderWndMgr = VtOrderDialogManager::GetInstance();
		orderWndMgr->OnFilled(order);
	}
}

void VtOrderManager::OnConfirmModify(VtRealtimeOrder&& real)
{
	VtOrder* order = FindOrder(real.orderNo);
	if (!order)
	{
		//OutputDebugString("order Not found!");
		HandleReverseConfirmModify(std::move(real));
		return;
	}

	order->state = real.state;
	order->amount = real.amount;
	order->unacceptedQty = real.remainQty;
	order->filledQty = real.filledAmount;
	order->filledPrice = real.filledPrice;
	order->filledMoney = real.filledMoney;
	order->tradeDate = real.tradeDate;
	order->tradeTime = real.tradeTime;
	order->TradeNo = real.tradeNo;
	order->tradeCondition = real.tradeCondition;

	order->oriOrderNo = real.oriOrderNo;
	RemoveOriginalOrder(real.oriOrderNo);
	AddAccepted(order);

	VtProductOrderManager* prdtOrderMgr = _ProductOrderManagerSelector->FindAdd(order->fullCode);
	prdtOrderMgr->OnConfirmModify(order);

	VtWindowManager* wndMgr = VtWindowManager::GetInstance();
	//CAcceptedOrderWnd* wnd = (CAcceptedOrderWnd*)wndMgr->FindWindow(_T("Accepted"));
	//wnd->RefreshOrders();

	VtChartManager* chartMgr = VtChartManager::GetInstance();
	VtChartFrm* chartFrm = chartMgr->FindChartFrm(order->chartID);
	if (chartFrm)
		chartFrm->OnConfirmModify(order);

	VtOrderDialogManager* orderWndMgr = VtOrderDialogManager::GetInstance();
	orderWndMgr->OnConfirmModify(order);
}

void VtOrderManager::OnConfirmCancel(VtRealtimeOrder&& real)
{
	VtOrder* order = FindOrder(real.orderNo);
	if (!order)
		return;

	order->state = real.state;
	order->filledQty = real.filledAmount;
	order->filledPrice = real.filledPrice;
	order->filledMoney = real.filledMoney;
	order->tradeDate = real.tradeDate;
	order->tradeTime = real.tradeTime;
	order->TradeNo = real.tradeNo;
	order->tradeCondition = real.tradeCondition;
	order->oriOrderNo = real.oriOrderNo;
	if (order)
	{
		RemoveOriginalOrder(real.oriOrderNo);

		//AddAccepted(order);

		VtProductOrderManager* prdtOrderMgr = _ProductOrderManagerSelector->FindAdd(order->fullCode);
		prdtOrderMgr->OnConfirmCancel(order);


		VtWindowManager* wndMgr = VtWindowManager::GetInstance();
		//CAcceptedOrderWnd* wnd = (CAcceptedOrderWnd*)wndMgr->FindWindow(_T("Accepted"));
		//wnd->RefreshOrders();

		VtChartManager* chartMgr = VtChartManager::GetInstance();
		VtChartFrm* chartFrm = chartMgr->FindChartFrm(order->chartID);
		if (chartFrm)
			chartFrm->OnConfirmCancel(order);

		VtOrderDialogManager* orderWndMgr = VtOrderDialogManager::GetInstance();
		orderWndMgr->OnConfirmCancel(order);
	}
}

void VtOrderManager::OnRejectNew(VtRealtimeOrder&& real)
{
	VtOrder* order = FindOrder(real.orderNo);
	if (!order)
		return;

	order->state = real.state;
	order->filledQty = real.filledAmount;
	order->filledPrice = real.filledPrice;
	order->filledMoney = real.filledMoney;
	order->tradeDate = real.tradeDate;
	order->tradeTime = real.tradeTime;
	order->TradeNo = real.tradeNo;
	order->tradeCondition = real.tradeCondition;
}

void VtOrderManager::OnRejectModify(VtRealtimeOrder&& real)
{
	VtOrder* order = FindOrder(real.orderNo);
	if (!order)
		return;

	order->state = real.state;
	order->filledQty = real.filledAmount;
	order->filledPrice = real.filledPrice;
	order->filledMoney = real.filledMoney;
	order->tradeDate = real.tradeDate;
	order->tradeTime = real.tradeTime;
	order->TradeNo = real.tradeNo;
	order->tradeCondition = real.tradeCondition;
}

void VtOrderManager::OnRejectCancel(VtRealtimeOrder&& real)
{
	VtOrder* order = FindOrder(real.orderNo);
	if (!order)
		return;

	order->state = real.state;
	order->filledQty = real.filledAmount;
	order->filledPrice = real.filledPrice;
	order->filledMoney = real.filledMoney;
	order->tradeDate = real.tradeDate;
	order->tradeTime = real.tradeTime;
	order->TradeNo = real.tradeNo;
	order->tradeCondition = real.tradeCondition;
}


void VtOrderManager::HandleReverseConfirmModify(VtRealtimeOrder&& real)
{
	VtOrder* order = FindOrder(real.oriOrderNo);
	if (!order)
		return;
	else
	{
		VtOrder* newOrder = nullptr; 
		newOrder = CloneOrder(order);
		newOrder->fullCode = real.fullCode;
		newOrder->orderPrice = real.orderedPrice;
		newOrder->orderType = real.orderType;
		newOrder->priceType = real.priceType;
		newOrder->state = real.state;
		newOrder->amount = real.amount;
		newOrder->unacceptedQty = real.remainQty;
		newOrder->filledQty = real.filledAmount;
		newOrder->filledPrice = real.filledPrice;
		newOrder->filledMoney = real.filledMoney;
		newOrder->tradeDate = real.tradeDate;
		newOrder->tradeTime = real.tradeTime;
		newOrder->TradeNo = real.tradeNo;
		newOrder->tradeCondition = real.tradeCondition;
		newOrder->orderNo = real.orderNo;
		newOrder->oriOrderNo = real.oriOrderNo;

		OrderMap[newOrder->orderNo] = newOrder;
		RemoveOriginalOrder(real.oriOrderNo);
		AddAccepted(newOrder);

		VtProductOrderManager* prdtOrderMgr = _ProductOrderManagerSelector->FindAdd(newOrder->fullCode);
		prdtOrderMgr->OnConfirmModify(newOrder);

		VtWindowManager* wndMgr = VtWindowManager::GetInstance();
		//CAcceptedOrderWnd* wnd = (CAcceptedOrderWnd*)wndMgr->FindWindow(_T("Accepted"));
		//wnd->RefreshOrders();

		VtChartManager* chartMgr = VtChartManager::GetInstance();
		VtChartFrm* chartFrm = chartMgr->FindChartFrm(newOrder->chartID);
		if (chartFrm)
			chartFrm->OnConfirmModify(newOrder);

		VtOrderDialogManager* orderWndMgr = VtOrderDialogManager::GetInstance();
		orderWndMgr->OnConfirmModify(newOrder);
	}
}


void VtOrderManager::Save(simple::file_ostream<same_endian_type>& ss)
{
	_ProductOrderManagerSelector->Save(ss);
}

void VtOrderManager::Load(simple::file_istream<same_endian_type>& ss)
{
	_ProductOrderManagerSelector->Load(ss);
}

void VtOrderManager::OnOrderReceivedHd(VtOrder* order)
{
	if (!order)
		return;
	VtOrder* exOrder = FindOrder(order->orderNo);
	if (!exOrder)
	{
		AddOrder(order);
		order->state = VtOrderState::OrderReceived;
	}
	else
		exOrder->state = VtOrderState::OrderReceived;
}

void VtOrderManager::OnOrderAcceptedHd(VtOrder* order)
{
	if (!order)
		return;
	VtOrder* exOrder = FindOrder(order->orderNo);
	VtTotalOrderManager* totalOrderMgr = VtTotalOrderManager::GetInstance();
	if (!exOrder) {
		AddOrder(order);
		totalOrderMgr->AddOrder(order);
	}
	else {
		totalOrderMgr->AddOrder(exOrder);
	}
	
	VtProductOrderManager* prdtOrderMgr = _ProductOrderManagerSelector->FindAdd(order->shortCode);
	// 한번이라도 주문이 나왔다는 것을 표시해 준다.
	prdtOrderMgr->Init(true);
	if (order->orderType == VtOrderType::Change) {
		// 주문상태를 접수로 변경
		order->state = VtOrderState::Accepted;
		// 상품별 주문 관리자의 접수 주문 목록에서 제거한다.
		prdtOrderMgr->RemoveAcceptedOrder(order->oriOrderNo);
		// 상품별 주문 관리자의 접수 주문 목록에 추가한다.
		prdtOrderMgr->AddAccepted(order);
		// 기존 주문은 제거한다.
		RemoveAccepted(order->oriOrderNo);
		// 접수 주문에 추가한다.
		AddAccepted(order);
	} else if (order->orderType == VtOrderType::Cancel) {
		// 기존 주문은 제거한다.
		RemoveAccepted(order->oriOrderNo);
		// 상품별 주문 관리자의 접수 주문 목록에서 제거한다.
		prdtOrderMgr->RemoveAcceptedOrder(order->oriOrderNo);
	} else {
		// 이 단계가 최종 접수된 상태이다.
		if (order->modifiedOrderCount == 0) {
			// 주문상태를 접수로 변경
			order->state = VtOrderState::Accepted;
			// 상품별 주문 관리자의 접수 주문 목록에 추가한다.
			prdtOrderMgr->AddAccepted(order);
			// 기존 주문은 제거한다.
			RemoveAccepted(order->oriOrderNo);
			// 접수 주문에 추가한다.
			AddAccepted(order);
		}
	}
}

void VtOrderManager::OnOrderUnfilledHd(VtOrder* order)
{
	if (!order)
		return;
	VtOrder* exOrder = FindOrder(order->orderNo);
	if (!exOrder)
		AddOrder(order);
}

void VtOrderManager::OnOrderFilledHd(VtOrder* order)
{
	if (!order)
		return;
	VtOrder* exOrder = FindOrder(order->orderNo);
	// 주문 목록에서 찾아서 없을 때는 외부 주문으로 간주하고 주문목록에 추가해 준다.
	if (!exOrder)
		AddOrder(order);

	// 개별 종목 평가손익을 계산해 준다.
	VtProductOrderManager* prdtOrderMgr = _ProductOrderManagerSelector->FindAdd(order->shortCode);
	prdtOrderMgr->OnFilled(order);

	// 접수 주문 목록에서 제거해 준다.
	RemoveAccepted(order->orderNo);

	// 여기서 계좌의 총 손익을 계산해 준다.
	CalcTotalProfitLoss(order);

}

void VtOrderManager::OnReceiveQuoteHd(VtSymbol* sym)
{
	CalcTotalProfitLoss(sym);
}

void VtOrderManager::OnRemain(VtPosition&& curPosi)
{
	VtProductOrderManager* prdtOrderMgr = _ProductOrderManagerSelector->FindAdd(curPosi.ShortCode);
	prdtOrderMgr->OnRemain(std::move(curPosi));
}

void VtOrderManager::CalcTotalProfitLoss(VtOrder* order)
{
	if (!order)
		return;

	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	VtAccount* acnt = acntMgr->FindAccount(order->AccountNo);
	if (acnt)
	{
		acnt->SumOpenPL();
	}
}

void VtOrderManager::CalcTotalProfitLoss(VtSymbol* symbol)
{
	if (!symbol)
		return;

	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	VtAccount* acnt = acntMgr->FindAccount(AccountNo);
	if (!acnt)
		return;
	acnt->CalcOpenPL(symbol);
	acnt->SumOpenPL();

	std::vector<VtAccount*>& acntList = acnt->GetSubAccountList();
	for (auto it = acntList.begin(); it != acntList.end(); ++it)
	{
		VtAccount* subAcnt = *it;
		subAcnt->CalcOpenPL(symbol);
		subAcnt->SumOpenPL();
	}

	VtFundManager* fundMgr = VtFundManager::GetInstance();
	VtFund* fund = nullptr;
	std::vector<std::string> fundNameList = acnt->GetFundNameList();
	for (auto it = fundNameList.begin(); it != fundNameList.end(); ++it)
	{
		fund = fundMgr->FindFund(*it);
		if (fund)
			fund->SumOpenPL();
	}
}

int VtOrderManager::GetAcceptedCount()
{
	return (int)AcceptedMap.size();
}

bool VtOrderManager::GetInit()
{
	return _ProductOrderManagerSelector->GetInit();
}

VtProductOrderManager* VtOrderManager::GetProductOrderManager(std::string symbolCode)
{
	if (_ProductOrderManagerSelector)
		return _ProductOrderManagerSelector->Find(symbolCode);
	else
		return nullptr;
}

VtProductOrderManager* VtOrderManager::FindAddProductOrderManager(std::string symbolCode)
{
	return _ProductOrderManagerSelector->FindAdd(symbolCode);
}

std::map<int, VtOrder*> VtOrderManager::GetTotalRemain(std::string symbolCode)
{
	VtProductOrderManager* prdtOrderMgr = FindAddProductOrderManager(symbolCode);
	return prdtOrderMgr->GetRemainMap();
}

void VtOrderManager::ClearRemainOrderMap(std::string symbolCode)
{
	VtProductOrderManager* prdtOrderMgr = FindAddProductOrderManager(symbolCode);
	prdtOrderMgr->GetRemainMap().clear();
}

int VtOrderManager::GetAvg(std::string symbolCode)
{
	std::map<int, VtOrder*> remainMap = GetTotalRemain(symbolCode);
	if (remainMap.size() == 0)
		return -1;

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindSymbol(symbolCode);
	int total = 0;
	for (auto it = remainMap.begin(); it != remainMap.end(); ++it) {
		total += it->second->intFilledPrice;
	}

	int avg = total / remainMap.size();
	avg = avg - avg % sym->intTickSize;
	return avg;
}

void VtOrderManager::GetRemainMap(std::string symbolCode, std::map<int, VtOrder*>& remainMap)
{
	VtProductOrderManager* prdtOrderMgr = _ProductOrderManagerSelector->Find(symbolCode);
	if (!prdtOrderMgr)
		return;
	std::map<int, VtOrder*> prdtRemainMap = prdtOrderMgr->GetRemainMap();
	for (auto it = prdtRemainMap.begin(); it != prdtRemainMap.end(); ++it)
	{
		remainMap[it->first] = it->second;
	}
}
