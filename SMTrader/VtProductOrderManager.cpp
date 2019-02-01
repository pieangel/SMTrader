#include "stdafx.h"
#include "VtProductOrderManager.h"
#include "VtOrder.h"
#include "VtSymbolManager.h"
#include "VtSymbolMaster.h"
#include "Poco/NumberFormatter.h"
#include "VtPosition.h"
#include "VtQuoteManager.h"
#include "VtQuote.h"
#include "VtOrderManager.h"
#include "VtAccountManager.h"
#include "VtAccount.h"
#include "VtSymbol.h"
#include "VtSubAccountManager.h"
#include "VtTotalOrderManager.h"

using Poco::NumberFormatter;

VtProductOrderManager::VtProductOrderManager()
{
	_Position = nullptr;
	_SymbolMaster = nullptr;
	_Account = nullptr;
	_OrderMgr = nullptr;
	_Init = false;
}


VtProductOrderManager::~VtProductOrderManager()
{
}

void VtProductOrderManager::ClearAccepted()
{
	AcceptedMap.clear();
}

void VtProductOrderManager::ClearFilled()
{
	FilledList.clear();
}

void VtProductOrderManager::OnRemain(VtPosition&& curPosi)
{
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	VtAccount* acnt = acntMgr->FindAccount(curPosi.AccountNo);
	if (!acnt)
		return;
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindHdSymbol(curPosi.ShortCode);
	if (!sym)
		return;
	// 여기서 잔고를 갱신해 준다.
	VtPosition* oldPosi = acnt->FindPosition(curPosi.ShortCode);

	double curClose = sym->Quote.intClose / std::pow(10, sym->IntDecimal);
	// 아무것도 없는 경우
	if (!oldPosi)
	{
		oldPosi = acnt->CopyPosition(curPosi);
		oldPosi->Position = curPosi.Position;
		oldPosi->AvgPrice = curPosi.AvgPrice;
		int buho = 1;
		if (curPosi.Position == VtPositionType::Sell)
			buho = -1;
		oldPosi->OpenQty = buho *curPosi.OpenQty;
		oldPosi->OpenProfitLoss = oldPosi->OpenQty * (curClose - oldPosi->AvgPrice)*sym->seungsu;
		return;
	}
	else {
		if (curPosi.Position == VtPositionType::Sell) { //체결수량이 매도	
			if (oldPosi->OpenQty > 0) { // 보유수량이 매수			
				if (oldPosi->OpenQty >= curPosi.OpenQty) { //보유수량이 큰경우
					oldPosi->OpenQty = oldPosi->OpenQty - curPosi.OpenQty;
					oldPosi->TradePL += double(-curPosi.OpenQty * (oldPosi->AvgPrice - curPosi.AvgPrice)*sym->seungsu);
					if (oldPosi->OpenQty == 0) {
						oldPosi->AvgPrice = 0;
						oldPosi->OpenProfitLoss = 0;
					}
				}
				else { //체결수량이 큰 경우
					oldPosi->TradePL += double(oldPosi->OpenQty * (curPosi.AvgPrice - oldPosi->AvgPrice)*sym->seungsu);
					oldPosi->AvgPrice = curPosi.AvgPrice;
					oldPosi->OpenQty = oldPosi->OpenQty - curPosi.OpenQty;
					oldPosi->OpenProfitLoss = oldPosi->OpenQty * (curClose - oldPosi->AvgPrice)*sym->seungsu;
				}
			}
			else { // 보유수량이 매도 ( 매도/매도 인경우 로직 동일)
				oldPosi->AvgPrice = double((oldPosi->OpenQty * oldPosi->AvgPrice - curPosi.OpenQty* curPosi.AvgPrice) / (oldPosi->OpenQty - curPosi.OpenQty));
				oldPosi->OpenQty = oldPosi->OpenQty - curPosi.OpenQty;
				oldPosi->OpenProfitLoss = oldPosi->OpenQty * (curClose - oldPosi->AvgPrice)*sym->seungsu;
			}
		}
		else { //체결수량이 매수
			if (oldPosi->OpenQty >= 0) { // 보유수량이 매수
				oldPosi->AvgPrice = double((oldPosi->OpenQty * oldPosi->AvgPrice + curPosi.OpenQty * curPosi.AvgPrice) / (oldPosi->OpenQty + curPosi.OpenQty));
				oldPosi->OpenQty = oldPosi->OpenQty + curPosi.OpenQty;
				oldPosi->OpenProfitLoss = oldPosi->OpenQty * (curClose - oldPosi->AvgPrice)*sym->seungsu;
			}
			else { //보유수량이 매도
				if (abs(oldPosi->OpenQty) >= curPosi.OpenQty) { //보유수량이 큰경우
					oldPosi->OpenQty = oldPosi->OpenQty + curPosi.OpenQty;
					oldPosi->TradePL += double(curPosi.OpenQty * (oldPosi->AvgPrice - curPosi.AvgPrice)*sym->seungsu);
					if (oldPosi->OpenQty == 0) {
						oldPosi->AvgPrice = 0;
						oldPosi->OpenProfitLoss = 0;
					}
				}
				else { //체결수량이 큰 경우				
					oldPosi->TradePL += double(oldPosi->OpenQty * (curPosi.AvgPrice - oldPosi->AvgPrice)*sym->seungsu);
					oldPosi->AvgPrice = curPosi.AvgPrice;
					oldPosi->OpenQty = oldPosi->OpenQty + curPosi.OpenQty;
					oldPosi->OpenProfitLoss = oldPosi->OpenQty * (curClose - oldPosi->AvgPrice)*sym->seungsu;
				}
			}
		}
	}


	if (oldPosi->OpenQty > 0)
	{
		oldPosi->Position = VtPositionType::Buy;
	}
	else if (oldPosi->OpenQty < 0)
	{
		oldPosi->Position = VtPositionType::Sell;
	}
	else
		oldPosi->Position = VtPositionType::None;
}

void VtProductOrderManager::AddAccepted(VtOrder* order)
{
	if (!order) return;

	AcceptedMap[order->orderNo] = order;
}

void VtProductOrderManager::OrderArrived(VtOrder* order)
{

}

void VtProductOrderManager::OnAccepted(VtOrder* order)
{
	if (!order)
		return;

	AcceptedMap[order->orderNo] = order;
}

// 주문 체결시 업데이트 하는 함수 - 현재 문자열로 되어있어 부동소수로 바꾸어야 함.
// 모든 주문의 잔고는 현재 남아 있는 잔고의 영향을 받는다.
// 현재 주문의 잔고는 기존의 잔고와 상쇄된 나머지가 잔고가 된다.
// 잔고가 0이 아닌 주문만 잔고 목록에 들어가게 되고 청산 대상이 된다.
void VtProductOrderManager::OnFilled(VtOrder* order)
{
	if (!order)
		return;
	// 접수 주문 목록에서 제거해 준다.
	RemoveAcceptedOrder(order->orderNo);

	// 이미 상쇄된 주문은 아무 처리도 하지 않는다.
	if (order->state == VtOrderState::Settled)
		return;

	VtAccount* acnt = nullptr;
	if (order->type == 0) { // 일반 계좌 주문일 경우
		VtAccountManager* acntMgr = VtAccountManager::GetInstance();
		acnt = acntMgr->FindAccount(order->AccountNo);
	} else { // 서브 계좌 혹은 펀드 주문일 경우
		VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
		acnt = subAcntMgr->FindAccount(order->AccountNo);
	}
	if (!acnt)
		return;
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindHdSymbol(order->shortCode);
	if (!sym)
		return;
	// 계좌에서 현재 포지션을 가져온다.
	VtPosition* posi = acnt->FindPosition(order->shortCode);

	double curClose = sym->Quote.intClose / std::pow(10, sym->IntDecimal);
	// 주문 포지션에 따른 부호 결정
	int buho = order->orderPosition == VtPositionType::Buy ? 1 : -1;
	if (!posi) { // 포지션이 없는 경우
		posi = acnt->CreatePosition(order);
		posi->Position = order->orderPosition;
		posi->AvgPrice = order->filledPrice;
		posi->OpenQty = buho *order->filledQty;
		posi->OpenProfitLoss = posi->OpenQty * (curClose - posi->AvgPrice)*sym->seungsu;
		// 잔고 수량에 넣어 준다. 매도는 음수, 매수는 양수, 포지션 없으면 0
		order->remainQty = buho *order->filledQty;
		// 잔고 주문에 넣는다.
		AddToRemain(order);
		return;
	} else  { // 포지션이 있는 경우
		RegisterOrderPosition(order, posi);
		if (order->orderPosition == VtPositionType::Sell) { //체결수량이 매도	
			if (posi->OpenQty > 0) { // 보유수량이 매수			
				if (posi->OpenQty >= order->filledQty) { //보유수량이 크거나 같은 경우
					posi->OpenQty = posi->OpenQty - order->filledQty;
					posi->TradePL += double(-order->filledQty * (posi->AvgPrice - order->filledPrice)*sym->seungsu);
					// 들어온 주문은 상쇄 된다.
					order->state = VtOrderState::Settled;
					order->remainQty = 0;
					if (posi->OpenQty == 0) { // 매수 포지션 청산
						posi->AvgPrice = 0;
						posi->OpenProfitLoss = 0;
					}
					if ((order->RequestType == 1 || order->RequestType == 2) && 
						 order->LiqReqOrderNo > 0) { // 손절이나 익절로 인한  지정된 청산 주문이다.
						// 지정된 주문을 없애 준다.
						RemoveByOrderNo(order->LiqReqOrderNo);
					}
					else { // 지정된 청산 주문이 아닐 경우는 일반적인 처리를 한다.
						// 이 경우 기존 잔고 목록을 상쇄해 줘야 한다.
						AdjustRemainQ(-1 * order->filledQty);
					}
				} else { //체결수량이 큰 경우
					posi->TradePL += double(posi->OpenQty * (order->filledPrice - posi->AvgPrice)*sym->seungsu);
					posi->AvgPrice = order->filledPrice;
					posi->OpenQty = posi->OpenQty - order->filledQty;
					posi->OpenProfitLoss = posi->OpenQty * (curClose - posi->AvgPrice)*sym->seungsu;
					// 보유수량과 상쇄되고 남은 갯수가 잔고가 된다. - 여기서는 잔고가 매도 포지션이 된다.
					order->remainQty = posi->OpenQty - order->filledQty;
					// 이경우 기존 잔고 목록은 모두 사라진다.
					RemoveAllRemain();
				}
			} else { // 보유수량이 매도 ( 보유수량이매도/체결수량이매도 인 경우)
				posi->AvgPrice = double((posi->OpenQty * posi->AvgPrice - order->filledQty* order->filledPrice) / (posi->OpenQty - order->filledQty));
				posi->OpenQty = posi->OpenQty - order->filledQty;
				posi->OpenProfitLoss = posi->OpenQty * (curClose - posi->AvgPrice)*sym->seungsu;
				// 이경우 포지션이 같으므로 더해 주지 않는다.
				// 잔고 수량에 넣어 준다. 매도는 음수, 매수는 양수, 포지션 없으면 0
				order->remainQty = buho *order->filledQty;
			}
		} else { //체결수량이 매수
			if (posi->OpenQty >= 0) { // 보유수량이 매수/체결수량이매수 인 경우
				posi->AvgPrice = double((posi->OpenQty * posi->AvgPrice + order->filledQty * order->filledPrice) / (posi->OpenQty + order->filledQty));
				posi->OpenQty = posi->OpenQty + order->filledQty;
				posi->OpenProfitLoss = posi->OpenQty * (curClose - posi->AvgPrice)*sym->seungsu;
				// 이경우 포지션이 같으므로 더해 주지 않는다.
				// 잔고 수량에 넣어 준다. 매도는 음수, 매수는 양수, 포지션 없으면 0
				order->remainQty = buho *order->filledQty;
			} else { //보유수량이 매도
				if (abs(posi->OpenQty) >= order->filledQty) { //보유수량이 큰경우
					posi->OpenQty = posi->OpenQty + order->filledQty;
					posi->TradePL += double(order->filledQty * (posi->AvgPrice - order->filledPrice)*sym->seungsu);
					order->state = VtOrderState::Settled;
					// 들어온 주문은 상쇄되고 포지션을 상실한다.
					order->remainQty = 0;
					if (posi->OpenQty == 0) { // 매도 포지션 청산
						posi->AvgPrice = 0;
						posi->OpenProfitLoss = 0;
					}

					if ((order->RequestType == 1 || order->RequestType == 2) &&
						order->LiqReqOrderNo > 0) { // 손절이나 익절로 인한  지정된 청산 주문이다.
													// 지정된 주문을 없애 준다.
						RemoveByOrderNo(order->LiqReqOrderNo);
					} else { // 지정된 청산 주문이 아닐 경우
						// 이 경우 기존 잔고 목록을 모두 없애 줘야 한다.
						RemoveAllRemain();
					}
				} else { //체결수량이 큰 경우				
					posi->TradePL += double(posi->OpenQty * (order->filledPrice - posi->AvgPrice)*sym->seungsu);
					posi->AvgPrice = order->filledPrice;
					posi->OpenQty = posi->OpenQty + order->filledQty;
					posi->OpenProfitLoss = posi->OpenQty * (curClose - posi->AvgPrice)*sym->seungsu;
					// 이경우 기존의 매도 포지션 잔고 갯수가 상쇄되고 남은 주문의 잔고는 매수 포지션이 된다.
					order->remainQty = posi->OpenQty + order->filledQty;
					// 이 경우 잔고 목록을 상쇄시켜 줘야 한다.
					AdjustRemainQ(order->filledQty);
				}
			}
		}
	}

	// 잔고 수량에 따라 포지션을 다시 결정해 준다.
	if (posi->OpenQty > 0) { // 매수 포지션 설정 : 양수임
		posi->Position = VtPositionType::Buy;
	} else if (posi->OpenQty < 0) { // 매도 포지션 설정 : 음수임
		posi->Position = VtPositionType::Sell;
	} else { // 모든 포지션 상실 0임
		posi->Position = VtPositionType::None;
		// 더이상 잔고가 없으므로 잔고표시 주문을 모두 제거해 준다.
		RemoveAllRemain();
	}
	
	// 주문 객체 잔고에 따라 잔고 목록에 추가를 결정해 준다.
	if (std::abs(order->remainQty) > 0) {
		RemainMap[order->orderNo] = order;
	}
}

void VtProductOrderManager::OnConfirmModify(VtOrder* order)
{
	AcceptedMap[order->orderNo] = order;
	RemoveAcceptedOrder(order->oriOrderNo);
}

void VtProductOrderManager::OnConfirmCancel(VtOrder* order)
{
	RemoveAcceptedOrder(order->oriOrderNo);
}


void VtProductOrderManager::PutOrder(int id, VtOrderCmd cmd, int Qty, double price, VtSymbolMaster* symMstr)
{

}

void VtProductOrderManager::ChangeOrder(double newValue, VtOrder* order)
{
	
}

void VtProductOrderManager::CancelOrder(VtOrder* order)
{
	if (!order)
		return;

}

void VtProductOrderManager::Position(VtPosition* val)
{
	_Position = val;
	//VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	//_SymbolMaster = symMgr->FindSymbolMasterByFullCode(_Position->FullCode);
}

void VtProductOrderManager::CancelAllBuyAccepted()
{
	if (!_OrderMgr)
		return;

	for (auto it = AcceptedMap.begin(); it != AcceptedMap.end(); it++)
	{
		VtOrder* order = it->second;
		if (order->orderPosition == VtPositionType::Buy)
		{
			_OrderMgr->CancelOrder(order);
		}
	}
}

void VtProductOrderManager::CancelAllSellAccepted()
{
	if (!_OrderMgr)
		return;

	for (auto it = AcceptedMap.begin(); it != AcceptedMap.end(); it++)
	{
		VtOrder* order = it->second;
		if (order->orderPosition == VtPositionType::Sell)
		{
			_OrderMgr->CancelOrder(order);
		}
	}
}

void VtProductOrderManager::CancelAllAccepted()
{
	if (!_OrderMgr)
		return;

	for (auto it = AcceptedMap.begin(); it != AcceptedMap.end(); it++)
	{
		VtOrder* order = it->second;
		_OrderMgr->CancelOrder(order);
	}
}

VtOrder* VtProductOrderManager::FindAccepted(int orderNo)
{
	auto it = AcceptedMap.find(orderNo);
	if (it != AcceptedMap.end())
		return it->second;
	else
		return nullptr;
}

void VtProductOrderManager::RemoveAcceptedOrder(int oldOrderNo)
{
	auto it = AcceptedMap.find(oldOrderNo);
	if (it != AcceptedMap.end())
	{
		AcceptedMap.erase(it);
	}
}

void VtProductOrderManager::OnReceiveQuoteHd(VtSymbol* sym)
{
	if (!sym)
		return;
}

int VtProductOrderManager::GetAcceptedOrderCount()
{
	return AcceptedMap.size();
}

void VtProductOrderManager::AddToRemain(VtOrder* order)
{
	if (!order || order->remainQty == 0)
		return;

	RemainMap[order->orderNo] = order;
}

void VtProductOrderManager::RemoveFromRemain(int orderNo)
{
	auto it = RemainMap.find(orderNo);
	if (it != RemainMap.end()) {
		RemainMap.erase(it);
	}
}

void VtProductOrderManager::AddToLiqud(VtOrder* order)
{
	if (!order)
		return;

	LiqudMap[order->orderNo] = order;
}

void VtProductOrderManager::RemoveFromLiqud(int orderNo)
{
	auto it = LiqudMap.find(orderNo);
	if (it != LiqudMap.end()) {
		LiqudMap.erase(it);
	}
}

void VtProductOrderManager::RegisterOrderPosition(VtOrder* order, VtPosition* position)
{
	if (!order || !position) {
		return;
	}

	VtTotalOrderManager* totalOrderMgr = VtTotalOrderManager::GetInstance();
	if (order->type == 0) {
		totalOrderMgr->AddPosition(order->type, position->AccountNo, position->ShortCode, position);
	}
	else if (order->type == 1) {
		totalOrderMgr->AddPosition(order->type, position->SubAccountNo, position->ShortCode, position);
	}
	else if (order->type == 2) {
		totalOrderMgr->AddPosition(order->type, position->SubAccountNo, position->ShortCode, position);
	}
}

void VtProductOrderManager::ResetPositionValue(VtPosition* posi)
{
	if (!posi)
		return;

	posi->OpenAmt = 0.0;
	posi->OpenProfitLoss = 0.0;
	posi->OpenQty = 0;
	posi->Position = VtPositionType::None;
}

double VtProductOrderManager::CalcOpenPL(VtOrder* order, double avgPrice, int count)
{
	double result = 0.0;
	if (!order)
		return result;

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindHdSymbol(order->shortCode);
	if (!sym)
		return result;
	int intAvgPrice = (int)(avgPrice * std::pow(10, sym->IntDecimal));
	int diff = VtSymbolManager::GetTickDiff(sym->Quote.intClose, intAvgPrice, sym);
	result = abs(diff) * sym->intTickValue * count;
	return result;
}

double VtProductOrderManager::CalcOpenPL(VtOrder* order, int avgPrice, int count)
{
	double result = 0.0;
	if (!order)
		return result;

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindSymbol(order->shortCode);
	if (!sym)
		return result;

	
	int diff = VtSymbolManager::GetTickDiff(sym->Quote.intClose, avgPrice, sym);
	result = abs(diff) * sym->intTickValue * count;
	return result;
}

//매도잔고의 청산이면 - (청산가격 - 평균가)*청산수량
//매수잔고의 청산이면(청산가격 - 평균가)*청산수량
double VtProductOrderManager::CalcTradePL(double avgPrice, std::string filledPrice, int count)
{
	double result = 0.0;

	std::string avg = NumberFormatter::format(avgPrice, 20, _SymbolMaster->decimal);
	int diff = VtSymbolManager::GetTickDiff(avg, filledPrice, _SymbolMaster);
	result = abs(diff) * std::stof(_SymbolMaster->Scn_Tick_val) * count;

	return result;
}

double VtProductOrderManager::CalcTradePL(double avgPrice, double filledPrice, int count)
{
	double result = 0.0;
	if (!_SymbolMaster)
		return result;
	int diff = VtSymbolManager::GetTickDiff(avgPrice, filledPrice, _SymbolMaster);
	result = abs(diff) * _SymbolMaster->tickVal * count;

	return result;
}

double VtProductOrderManager::CalcOpenPLHd(double close, VtPosition* posi, VtSymbol* sym)
{
	if (!posi || !sym)
		return 0.0;

	double result = 0;
	result = posi->OpenQty * (close - posi->AvgPrice)*sym->seungsu;

	return result;
}

void VtProductOrderManager::AdjustRemainQ(int newRemainQty)
{
	if (newRemainQty == 0)
		return;

	// 인풋 포지션을 정한다.
	VtPositionType inputPos = newRemainQty > 0 ? VtPositionType::Buy : VtPositionType::Sell;
	int rem = newRemainQty;
	for (auto it = RemainMap.begin(); it != RemainMap.end();) {
		VtOrder* order = it->second;
		// 기존 잔고와 인풋 잔고를 더한다.
		rem = order->remainQty + rem;
		// 새로운 포지션을 정한다.
		VtPositionType newPos = rem == 0 ? VtPositionType::None : (rem > 0 ? VtPositionType::Buy : VtPositionType::Sell);
		if (newPos == inputPos || newPos == VtPositionType::None) { // 이경우는 잔고 목록에서 제거해 줘야 한다.
			RemainMap.erase(it++);
		} else { // 다음 잔고를 검사한다.
			++it;
		}
		// 포지션이 바뀌면 루프를 탈출한다. - 포지션이 바뀌었다는 것은 상쇄가 끝났다는 것을 의미한다.
		if (inputPos != newPos) {
			// 새로운 잔고를 기존 주문에 넣어 준다.
			order->remainQty = rem;
			break;
		}
	}
}

void VtProductOrderManager::RemoveAllRemain()
{
	RemainMap.clear();
}

void VtProductOrderManager::RemoveByOrderNo(int orderNo)
{
	auto it = RemainMap.find(orderNo);
	if (it != RemainMap.end()) {
		RemainMap.erase(it);
	}
}

void VtProductOrderManager::Save(simple::file_ostream<same_endian_type>& ss)
{
	// Save the count of the orders accepted.
	ss << (int)AcceptedMap.size();

	for (auto it = AcceptedMap.begin(); it != AcceptedMap.end(); ++it)
	{
		VtOrder* acptOrder = it->second;
		acptOrder->Save(ss);
	}
}

void VtProductOrderManager::Load(simple::file_istream<same_endian_type>& ss)
{
	int acptCount = 0;
	ss >> acptCount;
	// 접수된 주문은 서버에서 온 주문과 비교하여 동일한 주문번호가 있을 때만 추가한다.
	// 이 때 서버에서 접수된 주문 목록을 이미 가지고 있다.
	for (int i = 0; i < acptCount; ++i)
	{
		;
	}
}
