#include "stdafx.h"
#include "VtCutManager.h"
#include "VtAccount.h"
#include "VtOrder.h"
#include "VtPosition.h"
#include "VtFund.h"
#include "VtSymbol.h"
#include "VtOrderConfigManager.h"
#include "VtOrderManager.h"
#include "VtSymbolManager.h"
#include "VtAccountManager.h"
#include "VtSubAccountManager.h"
#include "VtFundOrderManager.h"

VtCutManager::VtCutManager()
{
	_OrderConfigMgr = nullptr;
	_CutProfit = 2;
	_CutLoss = 2;
}


VtCutManager::~VtCutManager()
{
}

void VtCutManager::AddOrderHd(HdOrderRequest* req)
{
	if (!req)
		return;
	StopOrderMapHd[req->RequestId] = req;
}

HdOrderRequest* VtCutManager::FindOrderHd(int reqID)
{
	auto it = StopOrderMapHd.find(reqID);
	if (it != StopOrderMapHd.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

void VtCutManager::RemoveOrderHd(int reqID, bool resetProfitLoss)
{
	auto it = StopOrderMapHd.find(reqID);
	if (it != StopOrderMapHd.end()) {
		HdOrderRequest* order = it->second;
		StopOrderMapHd.erase(it);
		delete order;
	}
}

void VtCutManager::RemoveAllHd()
{
	for (auto it = StopOrderMapHd.begin(); it != StopOrderMapHd.end(); it++) {
		HdOrderRequest* order = it->second;
		delete order;
	}
	StopOrderMapHd.clear();
}

void VtCutManager::RemoveBuyAllHd()
{
	for (auto it = StopOrderMapHd.begin(); it != StopOrderMapHd.end(); ) {
		HdOrderRequest* order = it->second;
		if (order->Position == VtPositionType::Buy) {
			it = StopOrderMapHd.erase(it);
			delete order;
		}
		else
			it++;
	}
}

void VtCutManager::RemoveSellAllHd()
{
	for (auto it = StopOrderMapHd.begin(); it != StopOrderMapHd.end(); ) {
		HdOrderRequest* order = it->second;
		if (order->Position == VtPositionType::Sell) {
			it = StopOrderMapHd.erase(it);
			delete order;
		}
		else
			it++;
	}
}

void VtCutManager::MakePositionStop(int mode, VtSymbol* sym)
{
	if (!sym || !_OrderConfigMgr)
		return;
	if (_OrderConfigMgr->Type() == 0) {
		VtAccount* acnt = _OrderConfigMgr->Account();
		VtPosition* posi = acnt->FindPosition(sym->ShortCode);
		if (posi && std::abs(posi->OpenQty) > 0) {
			if (mode == 0 || mode == 1)
				MakePositionStopOrderRequest(mode, posi, sym, acnt, nullptr);
			else {
				MakePairStopOrderRequest(posi, sym, acnt, nullptr);
			}
		}
	} else {
		VtFund* fund = _OrderConfigMgr->Fund();
		int count = 0;
		VtPosition posi = fund->GetPosition(sym->ShortCode, count);
		if (count != 0) {
			MakeAutoStopList(mode, &posi, sym, fund);
		}
	}
}

void VtCutManager::MakeAutoStopList(int mode, VtPosition* totalPosi, VtSymbol* sym, VtFund* fund)
{
	if (!totalPosi || !sym || !fund )
		return;

	std::vector<VtPosition*> posiVec = fund->GetPositionList(sym->ShortCode);
	for (auto it = posiVec.begin(); it != posiVec.end(); ++it) {
		VtPosition* curPosi = *it;
		VtAccount* parentAcnt = fund->GetParentAccount(curPosi->SubAccountNo);
		if (!parentAcnt)
			continue;
		if (std::abs(curPosi->OpenQty) > 0) {
			if (mode == 0 || mode == 1)
				MakePositionStopOrderRequest(mode, curPosi, sym, parentAcnt, fund);
			else {
				MakePairStopOrderRequest(curPosi, sym, parentAcnt, fund);
			}
		}
	}
}

HdOrderRequest* VtCutManager::MakePositionStopOrderRequest(int mode, VtPosition* posi, VtSymbol* sym, VtAccount* acnt, VtFund* fund)
{
	if (!posi || !sym || !acnt || !_OrderConfigMgr->OrderMgr())
		return nullptr;

	if (!(posi->Position == VtPositionType::Buy || posi->Position == VtPositionType::Sell))
		return nullptr;

	int intAvg = static_cast<int>(ROUNDING(posi->AvgPrice * std::pow(10, sym->Decimal), sym->Decimal));
	// 적절한 값을 셀에서 찾기 위하여 나머지를 빼준다.
	intAvg = intAvg - intAvg % sym->intTickSize;

	HdOrderRequest* request = nullptr;
	std::string keyAccountNo = fund ? posi->SubAccountNo : acnt->AccountNo;
	auto it = CutInfoMap.find(std::make_tuple(keyAccountNo, sym->ShortCode, mode));
	if (it != CutInfoMap.end()) { // 이미 손절이나 익절 스탑이 있는 경우
		request = it->second;
	}
	else { // 익절, 손절 스탑이 전혀 없는 경우
		request = new HdOrderRequest();
		// 새로운 주문요청번호를 생성하여 대입한다.
		request->RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
		// 펀드일 경우 2, 아닐 경우 0
		request->Type = fund ? 2 : 0;
		CutInfoMap[std::make_tuple(keyAccountNo, sym->ShortCode, mode)] = request;
	}

	if (posi->Position == VtPositionType::Buy) { // 매수 포지션일 때
		if (mode == 0) { // 익절일 때
			request->Price = intAvg + sym->intTickSize * _CutProfit;
			request->RequestType = 1;
			posi->ProfitCutSet = true;
		}
		else { // 손절 일 때
			request->Price = intAvg - sym->intTickSize * _CutLoss;
			request->RequestType = 2;
			posi->LossCutSet = true;
		}
		request->Position = VtPositionType::Sell;
	} else if (posi->Position == VtPositionType::Sell) { // 매도 포지션일 때
		if (mode == 0) { // 익절일 때
			request->Price = intAvg - sym->intTickSize * _CutProfit;
			request->RequestType = 1;
			posi->ProfitCutSet = true;
		} else { // 손절 일 때
			request->Price = intAvg + sym->intTickSize * _CutLoss;
			request->RequestType = 2;
			posi->LossCutSet = true;
		}
		request->Position = VtPositionType::Buy;
	}

	if (_OrderType == 0)
		request->PriceType = VtPriceType::Market;
	else
		request->PriceType = VtPriceType::Price;

	request->Amount = std::abs(posi->OpenQty);
	request->AccountNo = acnt->AccountNo;
	request->Password = acnt->Password;
	request->SymbolCode = sym->ShortCode;
	request->FillCondition = _FillCondition;
	request->slip = _OrderTypeSlip;
	request->SourceId = (long)this;
	request->SubAccountNo = posi->SubAccountNo;
	request->FundName = posi->FundName;
	request->SrcPosition = posi;
	AddOrderHd(request);
	return request;
}

HdOrderRequest* VtCutManager::MakePositionStopOrderRequest(int mode, VtOrder* order, VtSymbol* sym, VtAccount* acnt, VtFund* fund)
{
	if (!order || !sym || !acnt || !_OrderConfigMgr->OrderMgr())
		return nullptr;

	if (!(order->orderPosition == VtPositionType::Buy || order->orderPosition == VtPositionType::Sell))
		return nullptr;

	int filledPrice = order->intFilledPrice;
	
	HdOrderRequest* request = nullptr;
	std::string keyAccountNo = order->Type == 0 ? order->AccountNo : order->SubAccountNo;
	auto it = CutInfoMap.find(std::make_tuple(keyAccountNo, sym->ShortCode, mode));
	if (it != CutInfoMap.end()) { // 이미 손절이나 익절 스탑이 있는 경우
		request = it->second;
	} else { // 익절, 손절 스탑이 전혀 없는 경우
		request = new HdOrderRequest();
		// 새로운 주문요청번호를 생성하여 대입한다.
		request->RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
		request->Type = order->Type;
		CutInfoMap[std::make_tuple(keyAccountNo, sym->ShortCode, mode)] = request;
	}

	if (order->orderPosition == VtPositionType::Buy) { // 매수 포지션일 때
		if (mode == 0) { // 익절일 때
			request->Price = filledPrice + sym->intTickSize * _CutProfit;
			request->RequestType = 1;
			// 익절 감시대상에서 제외
			order->EnableProfitCut = false;
		} else { // 손절 일 때
			request->Price = filledPrice - sym->intTickSize * _CutLoss;
			request->RequestType = 2;
			// 손절 감시대상에서 제외
			order->EnableLossCut = false;
		}
		request->Position = VtPositionType::Sell;
	}
	else if (order->orderPosition == VtPositionType::Sell) { // 매도 포지션일 때
		if (mode == 0) { // 익절일 때
			request->Price = filledPrice - sym->intTickSize * _CutProfit;
			request->RequestType = 1;
			// 익절 감시대상에서 제외
			order->EnableProfitCut = false;
		}
		else { // 손절 일 때
			request->Price = filledPrice + sym->intTickSize * _CutLoss;
			request->RequestType = 2;
			// 손절 감시대상에서 제외
			order->EnableLossCut = false;
		}
		request->Position = VtPositionType::Buy;
	}

	if (_OrderType == 0)
		request->PriceType = VtPriceType::Market;
	else
		request->PriceType = VtPriceType::Price;

	request->Amount = std::abs(order->remainQty);
	request->AccountNo = acnt->AccountNo;
	request->Password = acnt->Password;
	request->SymbolCode = sym->ShortCode;
	request->FillCondition = _FillCondition;
	request->slip = _OrderTypeSlip;
	request->SourceId = (long)this;
	request->SubAccountNo = order->SubAccountNo;
	request->FundName = order->FundName;
	// 청산 대상 주문번호를 넣어 준다.
	// 주문이 나가 체결되었을 경우 이 번호를 찾아 잔고 목록에서 지워준다.
	request->LiqReqOrderNo = order->orderNo;
	AddOrderHd(request);
	return request;
}

HdOrderRequest* VtCutManager::MakePositionStopOrderRequest(int mode, VtOrder* order, VtSymbol* sym, VtAccount* acnt)
{
	if (!order || !sym || !acnt || !_OrderConfigMgr->OrderMgr())
		return nullptr;

	if (!(order->orderPosition == VtPositionType::Buy || order->orderPosition == VtPositionType::Sell))
		return nullptr;
	// 익절로 청산, 손절로 청산 주문이거나, 주문 잔고가 없을 때는 수행하지 않는다.
	if (order->RequestType == 1 || order->RequestType == 2 || order->remainQty == 0)
		return nullptr;

	int filledPrice = order->intFilledPrice;

	HdOrderRequest* request = new HdOrderRequest();
	// 새로운 주문요청번호를 생성하여 대입한다.
	request->RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
	// 주문타입 본계좌 0, 서브계좌 1, 펀드 2.
	request->Type = order->Type;

	if (order->orderPosition == VtPositionType::Buy) { // 매수 포지션일 때
		if (mode == 0) { // 익절일 때
			request->RequestType = 1;
			// 익절 감시대상에서 제외
			order->EnableProfitCut = false;
		} else { // 손절 일 때
			request->RequestType = 2;
			// 손절 감시대상에서 제외
			order->EnableLossCut = false;
		}
		request->Position = VtPositionType::Sell;
	} else if (order->orderPosition == VtPositionType::Sell) { // 매도 포지션일 때
		if (mode == 0) { // 익절일 때
			request->RequestType = 1;
			// 익절 감시대상에서 제외
			order->EnableProfitCut = false;
		} else { // 손절 일 때
			request->RequestType = 2;
			// 손절 감시대상에서 제외
			order->EnableLossCut = false;
		}
		request->Position = VtPositionType::Buy;
	}

	if (_OrderType == 0)
		request->PriceType = VtPriceType::Market;
	else
		request->PriceType = VtPriceType::Price;

	request->Amount = std::abs(order->remainQty);
	request->AccountNo = acnt->AccountNo;
	request->Password = acnt->Password;
	request->SymbolCode = sym->ShortCode;
	request->FillCondition = _FillCondition;
	request->slip = _OrderTypeSlip;
	request->SourceId = (long)this;
	request->SubAccountNo = order->SubAccountNo;
	request->FundName = order->FundName;
	// 청산 대상 주문번호를 넣어 준다.
	// 주문이 나가 체결되었을 경우 이 번호를 찾아 잔고 목록에서 지워준다.
	request->LiqReqOrderNo = order->orderNo;
	AddOrderHd(request);
	// 평균값을 계산하여 스탑 위치를 다시 설정한다.
	RecalPosByAvg(sym);
	return request;
}

void VtCutManager::MakePairStopOrderRequest(VtPosition* posi, VtSymbol* sym, VtAccount* acnt, VtFund* fund)
{
	HdOrderRequest* profitReq = MakePositionStopOrderRequest(0, posi, sym, acnt, fund);
	HdOrderRequest* lossReq = MakePositionStopOrderRequest(1, posi, sym, acnt, fund);
	if (profitReq && lossReq) {
		profitReq->Paired = true;
		profitReq->PairedReq = lossReq;
		lossReq->Paired = true;
		lossReq->PairedReq = profitReq;
	}
}

void VtCutManager::MakePairStopOrderRequest(VtOrder* order, VtSymbol* sym, VtAccount* acnt)
{
	HdOrderRequest* profitReq = MakePositionStopOrderRequest(0, order, sym, acnt);
	HdOrderRequest* lossReq = MakePositionStopOrderRequest(1, order, sym, acnt);
	if (profitReq && lossReq) {
		profitReq->Paired = true;
		profitReq->PairedReq = lossReq;
		lossReq->Paired = true;
		lossReq->PairedReq = profitReq;
	}
}

void VtCutManager::AddStopOrderForFilled(VtSymbol* sym, VtOrder* order)
{
	if (!_OrderConfigMgr || !order || (!_EnableCutProfit && !_EnableCutLoss))
		return;
	// 모드를 확인한다.
	int mode = _EnableCutProfit && _EnableCutLoss ? 2 : (_EnableCutProfit ? 0 : 1);

	VtAccount* acnt = nullptr;
	if (_OrderConfigMgr->Type() == 0) {  // 실계좌나 서브계좌 주문일때
		acnt = _OrderConfigMgr->Account();
		if (acnt->AccountLevel() == 0) { //  실계좌 주문일 때
			if (order->Type != 0) // 주문타입이 실계좌 아니면 불이행
				return;
		} else {
			if (order->Type != 1) // 주문 타입이 서브계좌 아니면 불이행
				return;
			acnt = acnt->ParentAccount();
		}
	} else { // 펀드주문일 때
		if (order->Type != 2) // 주문 타입이 펀드 타입이 아니면 불 이행
			return;
		acnt = _OrderConfigMgr->Fund()->GetParentAccount(order->SubAccountNo);
	}

	if (!acnt)
		return;
	if (mode == 0 || mode == 1) // 익절만 혹은 손절만 설정되어 있는 경우
		MakePositionStopOrderRequest(mode, order, sym, acnt);
	else { // 익절, 손절 동시에 설정되어 있는 경우
		MakePairStopOrderRequest(order, sym, acnt);
	}
}

void VtCutManager::MakePositionStopByRemain(int mode, VtSymbol* sym)
{
	if (!sym || !_OrderConfigMgr || !_OrderConfigMgr->OrderMgr())
		return;
	std::map<int, VtOrder*> remainMap = _OrderConfigMgr->OrderMgr()->GetTotalRemain(sym->ShortCode);
	// 잔고 목록이 비어 있으면 수행하지 않는다.
	if (remainMap.size() == 0)
		return;
	VtAccount* acnt = nullptr;
	if (_OrderConfigMgr->Type() == 0) {  // 계좌 주문일때
		acnt = _OrderConfigMgr->Account();
		if (!acnt) // 계좌가 없으면 탈출
			return;

		for (auto it = remainMap.begin(); it != remainMap.end(); ++it) {
			VtOrder* order = it->second;
			if (mode == 0 || mode == 1) { // 손절만 혹은 익절만 설정되어 있을 경우
				MakePositionStopOrderRequest(mode, order, sym, acnt);
			} else { // 손절, 익절 동시에 설정되어 있을 경우
				MakePairStopOrderRequest(order, sym, acnt);
			}
		}
	} else { // 펀드 주문일 경우
		VtFund* fund = _OrderConfigMgr->Fund();
		for (auto it = remainMap.begin(); it != remainMap.end(); ++it) {
			VtOrder* order = it->second;
			// 부모 계좌를 가져온다.
			acnt = fund->GetParentAccount(order->SubAccountNo);
			if (!acnt)
				continue;

			if (mode == 0 || mode == 1) { // 손절만 혹은 익절만 설정되어 있을 경우
				MakePositionStopOrderRequest(mode, order, sym, acnt);
			} else { // 손절, 익절 동시에 설정되어 있을 경우
				MakePairStopOrderRequest(order, sym, acnt);
			}
		}
	}
}

// 손절, 익절이 걸려 있지 않을 때 수동 설정하는 함수
void VtCutManager::MakePositionStopByRemain(VtSymbol* sym)
{
	if (!sym || !_OrderConfigMgr || !_OrderConfigMgr->OrderMgr())
		return;
	std::map<int, VtOrder*> remainMap = _OrderConfigMgr->OrderMgr()->GetTotalRemain(sym->ShortCode);
	// 잔고 목록이 비어 있으면 수행하지 않는다.
	if (remainMap.size() == 0)
		return;
	// 모드를 확인한다.
	int mode = _EnableCutProfit && _EnableCutLoss ? 2 : (_EnableCutProfit ? 0 : 1);
	VtAccount* acnt = nullptr;
	if (_OrderConfigMgr->Type() == 0) {  // 실계좌 주문일때 서브 계좌 주문일 때
		acnt = _OrderConfigMgr->Account();
		if (acnt->AccountLevel() != 0) { // 서브 계좌일 경우 부모 계좌를 가져 온다.
			acnt = acnt->ParentAccount();
		}
		if (!acnt) // 계좌가 없으면 탈출
			return;

		for (auto it = remainMap.begin(); it != remainMap.end(); ++it) {
			VtOrder* order = it->second;
			if (mode == 0 || mode == 1) { // 손절만 혹은 익절만 설정되어 있을 경우
				MakePositionStopOrderRequest(mode, order, sym, acnt);
			} else { // 손절, 익절 동시에 설정되어 있을 경우
				MakePairStopOrderRequest(order, sym, acnt);
			}
		}
	}
	else { // 펀드 주문일 경우
		VtFund* fund = _OrderConfigMgr->Fund();
		for (auto it = remainMap.begin(); it != remainMap.end(); ++it) {
			VtOrder* order = it->second;
			// 부모 계좌를 가져온다.
			acnt = fund->GetParentAccount(order->SubAccountNo);
			if (!acnt)
				continue;

			if (mode == 0 || mode == 1) { // 손절만 혹은 익절만 설정되어 있을 경우
				MakePositionStopOrderRequest(mode, order, sym, acnt);
			} else { // 손절, 익절 동시에 설정되어 있을 경우
				MakePairStopOrderRequest(order, sym, acnt);
			}
		}
	}
}

void VtCutManager::UpdateCutInfo()
{
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = nullptr;
	for (auto it = StopOrderMapHd.begin(); it != StopOrderMapHd.end(); it++)
	{
		HdOrderRequest* order = it->second;
		sym = symMgr->FindSymbol(order->SymbolCode);
		if (!sym)
			continue;
	}
}

void VtCutManager::ApplyProfitLossForPosition(VtSymbol* sym)
{

}

HdOrderRequest* VtCutManager::FindStop(int reqId)
{
	auto it = StopOrderMapHd.find(reqId);
	if (it != StopOrderMapHd.end())
		return it->second;
	else
		return nullptr;
}

void VtCutManager::ResetFlag(VtOrder* order)
{
	if (!order)
		return;

// 	if (order->RequestType == 1)
// 		_CutProfitSet = false;
// 	if (order->RequestType == 2)
// 		_CutLossSet = false;
}

bool VtCutManager::CheckCutLoss(VtPosition* posi, VtSymbol* sym, int tickCount)
{
	if (!sym)
		return false;
	if (!posi || posi->OpenQty == 0)
		return false;
	// 틱크기 * 틱수 * 승수 = 실제 수익
	int target = sym->intTickSize * tickCount * sym->Seungsu;

	target = -1 * target;
	if (posi->OpenProfitLoss * std::pow(10, sym->Decimal) <= (double)target)
		return true;
	else
		return false;
}

bool VtCutManager::CheckCutLoss(VtOrder* order, VtSymbol* sym, int cutTick)
{
	if (!_EnableCutLoss || !sym || !order || order->EnableLossCut)
		return false;

	// 체결된 값을 넣어 준다.
	int targetValue = order->intFilledPrice;
	if (order->orderPosition == VtPositionType::Buy) { // 잔고가 매수일 때
		// 손실 확인 이므로 체결된 값에서 틱크기만큼 빼준다.
		targetValue -= cutTick * sym->intTickSize;
		// 지정된 값보다 작거나 같으면 터치했다고 보고 컷신호 발사
		if (sym->Quote.intClose <= targetValue) {
			// 여기서 청산 주문을 내준다.
			PutOrder(order, sym, 2, VtPriceType::Price);
			// 이미 청산 명렬이 내려졌으므로 더이상 손절 확인을 하지 않게 한다.
			order->EnableLossCut = false;
			return true;
		}
	} else if (order->orderPosition == VtPositionType::Sell) { // 잔고가 매도일 때
		// 손실 확인 이므로 체결된 값에서 틱크기만큼 더해준다.
		targetValue += cutTick * sym->intTickSize;
		// 지정된 값보다 크거나 같으면 터치했다고 보고 컷신호 발사
		if (sym->Quote.intClose >= targetValue) {
			// 여기서 청산 주문을 내준다.
			PutOrder(order, sym, 2, VtPriceType::Price);
			// 이미 청산 명렬이 내려졌으므로 더이상 손절 확인을 하지 않게 한다.
			order->EnableLossCut = false;
			return true;
		}
	}
	return false;
}

bool VtCutManager::CheckCutProfit(VtPosition* posi, VtSymbol* sym, int size)
{
	if (!sym)
		return false;

	if (!posi)
		return false;
	// 틱크기 * 틱수 * 승수 = 실제 수익
	int target = sym->intTickSize * size * sym->Seungsu;

	if (posi->OpenProfitLoss * std::pow(10, sym->Decimal) >= (double)target)
		return true;
	else
		return false;
}

bool VtCutManager::CheckCutProfit(VtOrder* order, VtSymbol* sym, int cutTick)
{
	if (!_EnableCutProfit || !sym || !order || order->EnableProfitCut)
		return false;

	// 체결된 값을 넣어 준다.
	int targetValue = order->intFilledPrice;
	if (order->orderPosition == VtPositionType::Buy) { // 잔고가 매수일 때
		// 이익 확인 이므로 체결된 값에서 틱크기만큼 더해준다.
		targetValue += cutTick * sym->intTickSize;
		// 종가가 지정된 값보다 크거나 같으면 터치했다고 보고 컷신호 발사
		if (sym->Quote.intClose >= targetValue) {
			// 여기서 청산 주문을 내준다.
			PutOrder(order, sym, 1, VtPriceType::Price);
			// 이미 청산 명렬이 내려졌으므로 더이상 익절 확인을 하지 않게 한다.
			order->EnableProfitCut = false;
			return true;
		}
	}
	else if (order->orderPosition == VtPositionType::Sell) { // 잔고가 매도일 때
		// 이익 확인 이므로 체결된 값에서 틱크기만큼 빼준다.
		targetValue -= cutTick * sym->intTickSize;
		// 지정된 값보다 작거나 같으면 터치했다고 보고 컷신호 발사
		if (sym->Quote.intClose <= targetValue) {
			// 여기서 청산 주문을 내준다.
			PutOrder(order, sym, 1, VtPriceType::Price);
			// 이미 청산 명렬이 내려졌으므로 더이상 익절 확인을 하지 않게 한다.
			order->EnableProfitCut = false;
			return true;
		}
	}
	return false;
}

bool VtCutManager::CheckCutLossByPercent(VtPosition* posi, VtSymbol* sym, double percent)
{
	double avg = posi->AvgPrice;
	double dif = avg * percent / 100.0;
	double curPrice = sym->Quote.intClose / std::pow(10, sym->Decimal);
	if (posi->Position == VtPositionType::Buy) {
		if (curPrice <= avg - dif) {
			return true;
		}
	}
	else if (posi->Position == VtPositionType::Sell) {
		if (curPrice >= avg + dif) {
			return true;
		}
	}
	return false;
}

bool VtCutManager::CheckCutProfitByPercent(VtPosition* posi, VtSymbol* sym, double percent)
{
	double avg = posi->AvgPrice;
	double dif = avg * percent / 100.0;
	double curPrice = sym->Quote.intClose / std::pow(10, sym->Decimal);
	if (posi->Position == VtPositionType::Buy) {
		if (curPrice >= avg + dif) {
			return true;
		}
	}
	else if (posi->Position == VtPositionType::Sell) {
		if (curPrice <= avg - dif) {
			return true;
		}
	}
	return false;
}

bool VtCutManager::CheckProfitLoss(VtSymbol* sym)
{
	if (!sym)
		return false;
	// 잔고 주문 목록을 가져온다.
	std::map<int, VtOrder*> remainMap = _OrderConfigMgr->OrderMgr()->GetTotalRemain(sym->ShortCode);
	// 잔고가 없으면 처리하지 않는다.
	if (remainMap.size() == 0)
		return false;
	// 잔고 목록을 돌면서 손실과 이익을 확인한다.
	for (auto it = remainMap.begin(); it != remainMap.end(); ++it) { 
		VtOrder* order = it->second;
		if (!CheckCutLoss(order, sym, _CutLoss)) { // 손실확인이 실패하면 익절확인을 한다.
			CheckCutProfit(order, sym, _CutProfit); // 익절 확인을 한다.
		}
	}

	return false;
}

void VtCutManager::LiqudAll(VtSymbol* sym, VtPriceType priceType, int price)
{
	if (!sym)
		return;

	if (_OrderConfigMgr->Type() == 0) {
		auto remain = GetTotalRemain(sym, _OrderConfigMgr->OrderMgr());
		if (remain.first == 0)
			return;
		int totalRemain = remain.first;
		VtOrder* order = remain.second;

		LiqudOrder(sym, order, totalRemain, priceType, price);
	} else {
		VtFundOrderManager* fundOrderMgr = (VtFundOrderManager*)_OrderConfigMgr->OrderMgr();
		std::map<std::string, VtOrderManager*>& orderMgrMap = fundOrderMgr->GetOrderManagerMap();
		for (auto it = orderMgrMap.begin(); it != orderMgrMap.end(); ++it) {
			VtOrderManager* orderMgr = it->second;
			auto remain = GetTotalRemain(sym, orderMgr);
			if (remain.first == 0)
				continue;
			int totalRemain = remain.first;
			VtOrder* order = remain.second;

			LiqudOrder(sym, order, totalRemain, priceType, price);
		}
	}

	
}

std::pair<int, VtOrder*> VtCutManager::GetTotalRemain(VtSymbol* sym, VtOrderManager* orderMgr)
{
	if (!sym || !orderMgr)
		return std::make_pair(0, nullptr);

	std::map<int, VtOrder*> remainMap = orderMgr->GetTotalRemain(sym->ShortCode);
	// 잔고 목록이 비어 있으면 수행하지 않는다.
	if (remainMap.size() == 0)
		return std::make_pair(0, nullptr);;

	int total = 0;
	VtOrder* topOrder = nullptr;
	for (auto it = remainMap.begin(); it != remainMap.end(); ++it) {
		topOrder = it->second;
		total += it->second->remainQty;
	}

	return std::make_pair(total, topOrder);;
}

int VtCutManager::GetAvg(VtSymbol* sym)
{
	if (!sym)
		return -1;

	std::map<int, VtOrder*> remainMap = _OrderConfigMgr->OrderMgr()->GetTotalRemain(sym->ShortCode);
	// 잔고 목록이 비어 있으면 수행하지 않는다.
	if (remainMap.size() == 0)
		return -1;

	int total = 0;
	for (auto it = remainMap.begin(); it != remainMap.end(); ++it) {
		total += it->second->intFilledPrice;
	}

	int avg = total / remainMap.size();
	avg = avg - avg % sym->intTickSize;
	return avg;
}

void VtCutManager::RecalPosByAvg(VtSymbol* sym)
{
	if (!sym)
		return;
	int avg = GetAvg(sym);
	for (auto it = StopOrderMapHd.begin(); it != StopOrderMapHd.end(); ++it) {
		HdOrderRequest* request = it->second;
		if (request->RequestType == 1) { // 익절일 때
			request->Price = avg + sym->intTickSize * _CutProfit;
		} else if (request->RequestType == 2) { // 손절 일 때
			request->Price = avg - sym->intTickSize * _CutLoss;
		}
	}
}

VtPosition* VtCutManager::FindPositionByOrder(VtOrder* order)
{
	if (!order)
		return nullptr;
	if (_OrderConfigMgr->Type() == 0) { // 계좌 주문일 때
		if (!_OrderConfigMgr->Account())
			return nullptr;

		VtAccount* acnt = _OrderConfigMgr->Account();
		return acnt->FindPosition(order->shortCode);
	} else { // 펀드 주문일 때
		if (!_OrderConfigMgr->Fund())
			return nullptr;
		VtFund* fund = _OrderConfigMgr->Fund();
		return fund->FindPosition(order->SubAccountNo, order->shortCode);
	}
}

void VtCutManager::RemoveCutInfo(HdOrderRequest* req, bool resetProfitLoss)
{
	if (!req)
		return;
	auto keyAccount = req->Type == 0 ? req->AccountNo : req->SubAccountNo;
	auto it = CutInfoMap.find(std::make_tuple(keyAccount, req->SymbolCode, req->RequestType == 1 ? 0 : 1));
	if (it != CutInfoMap.end()) { // 익절, 손절 정보 객체가 목록에 있는 경우
		CutInfoMap.erase(it);
	}
}

void VtCutManager::PutOrder(VtOrder* order, VtSymbol* sym, int reqType, VtPriceType priceType)
{
	if (!order || !sym || !_OrderConfigMgr || order->remainQty == 0)
		return;

	VtAccount* acnt = nullptr;
	if (order->Type == 0) { // 실계좌 주문
		VtAccountManager* acntMgr = VtAccountManager::GetInstance();
		acnt = acntMgr->FindAccount(order->AccountNo);
	} else { // 서브계좌 혹은 펀드 주문일때
		VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
		acnt = subAcntMgr->FindAccount(order->SubAccountNo);
	}

	if (!acnt) // 계좌가 없는 경우 아무일도 하지 않음.
		return;

	HdOrderRequest request;
	request.Amount = std::abs(order->remainQty);
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
	request.SymbolCode = order->shortCode;
	request.FillCondition = _FillCondition;

	request.PriceType = priceType;

	if (order->orderPosition == VtPositionType::Buy) { // 매수 청산인 경우
		request.Price = VtSymbolManager::GetNextValue(sym->Quote.intClose,_OrderTypeSlip, sym);
	} else { // 매도 청산인 경우
		request.Price = VtSymbolManager::GetNextValue(sym->Quote.intClose, -1 * _OrderTypeSlip, sym);
	}

	if (order->orderPosition == VtPositionType::Buy) { // 매수 청산인 경우
		request.Position = VtPositionType::Sell;
	}
	else if (order->orderPosition == VtPositionType::Sell) { // 매도 청산인 경우
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
	request.RequestType = reqType;
	// 청산 주문 번호를 넣어 준다.
	request.LiqReqOrderNo = order->orderNo;

	_OrderConfigMgr->OrderMgr()->PutOrder(std::move(request));
}

void VtCutManager::LiqudOrder(VtSymbol* sym, VtOrder* order, int totalRemain, VtPriceType priceType, int price)
{
	VtAccount* acnt = nullptr;
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	if (order->Type == 0)
		acnt = acntMgr->FindAccount(order->AccountNo);
	else { // 본계좌에 없을 경우 서브 계좌를 찾아 본다.
		VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
		acnt = subAcntMgr->FindAccount(order->SubAccountNo);
		acnt = acnt->ParentAccount();
	}

	if (!acnt) // 계좌가 없는 경우 아무일도 하지 않음.
		return;

	HdOrderRequest request;
	request.Amount = std::abs(totalRemain);
	request.AccountNo = acnt->AccountNo;
	request.Password = acnt->Password;

	request.SymbolCode = order->shortCode;
	request.FillCondition = _FillCondition;

	request.PriceType = priceType;

	if (totalRemain > 0) { // 매수 청산인 경우
		if (priceType == VtPriceType::Price)
			request.Price = VtSymbolManager::GetNextValue(price, -1 *_OrderTypeSlip, sym);
		else
			request.Price = 0;
		request.Position = VtPositionType::Sell;
	}
	else { // 매도 청산인 경우
		if (priceType == VtPriceType::Price)
			request.Price = VtSymbolManager::GetNextValue(price, _OrderTypeSlip, sym);
		else
			request.Price = 0;

		request.Position = VtPositionType::Buy;
	}

	request.RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
	request.SourceId = (long)this;

	request.SubAccountNo = order->SubAccountNo;
	request.FundName = order->FundName;

	request.orderType = VtOrderType::New;
	// 정산 주문이냐 청산
	// 0 : 일반 , 1 : 익절, 2 : 손절- 청산 주문일 때는 익절 손절이 활성화 되어 있어도 처리하지 않는다.
	request.RequestType = 1;

	_OrderConfigMgr->OrderMgr()->PutOrder(std::move(request));
}

