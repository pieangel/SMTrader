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
	// ������ ���� ������ ã�� ���Ͽ� �������� ���ش�.
	intAvg = intAvg - intAvg % sym->intTickSize;

	HdOrderRequest* request = nullptr;
	std::string keyAccountNo = fund ? posi->SubAccountNo : acnt->AccountNo;
	auto it = CutInfoMap.find(std::make_tuple(keyAccountNo, sym->ShortCode, mode));
	if (it != CutInfoMap.end()) { // �̹� �����̳� ���� ��ž�� �ִ� ���
		request = it->second;
	}
	else { // ����, ���� ��ž�� ���� ���� ���
		request = new HdOrderRequest();
		// ���ο� �ֹ���û��ȣ�� �����Ͽ� �����Ѵ�.
		request->RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
		// �ݵ��� ��� 2, �ƴ� ��� 0
		request->Type = fund ? 2 : 0;
		CutInfoMap[std::make_tuple(keyAccountNo, sym->ShortCode, mode)] = request;
	}

	if (posi->Position == VtPositionType::Buy) { // �ż� �������� ��
		if (mode == 0) { // ������ ��
			request->Price = intAvg + sym->intTickSize * _CutProfit;
			request->RequestType = 1;
			posi->ProfitCutSet = true;
		}
		else { // ���� �� ��
			request->Price = intAvg - sym->intTickSize * _CutLoss;
			request->RequestType = 2;
			posi->LossCutSet = true;
		}
		request->Position = VtPositionType::Sell;
	} else if (posi->Position == VtPositionType::Sell) { // �ŵ� �������� ��
		if (mode == 0) { // ������ ��
			request->Price = intAvg - sym->intTickSize * _CutProfit;
			request->RequestType = 1;
			posi->ProfitCutSet = true;
		} else { // ���� �� ��
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
	if (it != CutInfoMap.end()) { // �̹� �����̳� ���� ��ž�� �ִ� ���
		request = it->second;
	} else { // ����, ���� ��ž�� ���� ���� ���
		request = new HdOrderRequest();
		// ���ο� �ֹ���û��ȣ�� �����Ͽ� �����Ѵ�.
		request->RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
		request->Type = order->Type;
		CutInfoMap[std::make_tuple(keyAccountNo, sym->ShortCode, mode)] = request;
	}

	if (order->orderPosition == VtPositionType::Buy) { // �ż� �������� ��
		if (mode == 0) { // ������ ��
			request->Price = filledPrice + sym->intTickSize * _CutProfit;
			request->RequestType = 1;
			// ���� ���ô�󿡼� ����
			order->EnableProfitCut = false;
		} else { // ���� �� ��
			request->Price = filledPrice - sym->intTickSize * _CutLoss;
			request->RequestType = 2;
			// ���� ���ô�󿡼� ����
			order->EnableLossCut = false;
		}
		request->Position = VtPositionType::Sell;
	}
	else if (order->orderPosition == VtPositionType::Sell) { // �ŵ� �������� ��
		if (mode == 0) { // ������ ��
			request->Price = filledPrice - sym->intTickSize * _CutProfit;
			request->RequestType = 1;
			// ���� ���ô�󿡼� ����
			order->EnableProfitCut = false;
		}
		else { // ���� �� ��
			request->Price = filledPrice + sym->intTickSize * _CutLoss;
			request->RequestType = 2;
			// ���� ���ô�󿡼� ����
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
	// û�� ��� �ֹ���ȣ�� �־� �ش�.
	// �ֹ��� ���� ü��Ǿ��� ��� �� ��ȣ�� ã�� �ܰ� ��Ͽ��� �����ش�.
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
	// ������ û��, ������ û�� �ֹ��̰ų�, �ֹ� �ܰ� ���� ���� �������� �ʴ´�.
	if (order->RequestType == 1 || order->RequestType == 2 || order->remainQty == 0)
		return nullptr;

	int filledPrice = order->intFilledPrice;

	HdOrderRequest* request = new HdOrderRequest();
	// ���ο� �ֹ���û��ȣ�� �����Ͽ� �����Ѵ�.
	request->RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
	// �ֹ�Ÿ�� ������ 0, ������� 1, �ݵ� 2.
	request->Type = order->Type;

	if (order->orderPosition == VtPositionType::Buy) { // �ż� �������� ��
		if (mode == 0) { // ������ ��
			request->RequestType = 1;
			// ���� ���ô�󿡼� ����
			order->EnableProfitCut = false;
		} else { // ���� �� ��
			request->RequestType = 2;
			// ���� ���ô�󿡼� ����
			order->EnableLossCut = false;
		}
		request->Position = VtPositionType::Sell;
	} else if (order->orderPosition == VtPositionType::Sell) { // �ŵ� �������� ��
		if (mode == 0) { // ������ ��
			request->RequestType = 1;
			// ���� ���ô�󿡼� ����
			order->EnableProfitCut = false;
		} else { // ���� �� ��
			request->RequestType = 2;
			// ���� ���ô�󿡼� ����
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
	// û�� ��� �ֹ���ȣ�� �־� �ش�.
	// �ֹ��� ���� ü��Ǿ��� ��� �� ��ȣ�� ã�� �ܰ� ��Ͽ��� �����ش�.
	request->LiqReqOrderNo = order->orderNo;
	AddOrderHd(request);
	// ��հ��� ����Ͽ� ��ž ��ġ�� �ٽ� �����Ѵ�.
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
	// ��带 Ȯ���Ѵ�.
	int mode = _EnableCutProfit && _EnableCutLoss ? 2 : (_EnableCutProfit ? 0 : 1);

	VtAccount* acnt = nullptr;
	if (_OrderConfigMgr->Type() == 0) {  // �ǰ��³� ������� �ֹ��϶�
		acnt = _OrderConfigMgr->Account();
		if (acnt->AccountLevel() == 0) { //  �ǰ��� �ֹ��� ��
			if (order->Type != 0) // �ֹ�Ÿ���� �ǰ��� �ƴϸ� ������
				return;
		} else {
			if (order->Type != 1) // �ֹ� Ÿ���� ������� �ƴϸ� ������
				return;
			acnt = acnt->ParentAccount();
		}
	} else { // �ݵ��ֹ��� ��
		if (order->Type != 2) // �ֹ� Ÿ���� �ݵ� Ÿ���� �ƴϸ� �� ����
			return;
		acnt = _OrderConfigMgr->Fund()->GetParentAccount(order->SubAccountNo);
	}

	if (!acnt)
		return;
	if (mode == 0 || mode == 1) // ������ Ȥ�� ������ �����Ǿ� �ִ� ���
		MakePositionStopOrderRequest(mode, order, sym, acnt);
	else { // ����, ���� ���ÿ� �����Ǿ� �ִ� ���
		MakePairStopOrderRequest(order, sym, acnt);
	}
}

void VtCutManager::MakePositionStopByRemain(int mode, VtSymbol* sym)
{
	if (!sym || !_OrderConfigMgr || !_OrderConfigMgr->OrderMgr())
		return;
	std::map<int, VtOrder*> remainMap = _OrderConfigMgr->OrderMgr()->GetTotalRemain(sym->ShortCode);
	// �ܰ� ����� ��� ������ �������� �ʴ´�.
	if (remainMap.size() == 0)
		return;
	VtAccount* acnt = nullptr;
	if (_OrderConfigMgr->Type() == 0) {  // ���� �ֹ��϶�
		acnt = _OrderConfigMgr->Account();
		if (!acnt) // ���°� ������ Ż��
			return;

		for (auto it = remainMap.begin(); it != remainMap.end(); ++it) {
			VtOrder* order = it->second;
			if (mode == 0 || mode == 1) { // ������ Ȥ�� ������ �����Ǿ� ���� ���
				MakePositionStopOrderRequest(mode, order, sym, acnt);
			} else { // ����, ���� ���ÿ� �����Ǿ� ���� ���
				MakePairStopOrderRequest(order, sym, acnt);
			}
		}
	} else { // �ݵ� �ֹ��� ���
		VtFund* fund = _OrderConfigMgr->Fund();
		for (auto it = remainMap.begin(); it != remainMap.end(); ++it) {
			VtOrder* order = it->second;
			// �θ� ���¸� �����´�.
			acnt = fund->GetParentAccount(order->SubAccountNo);
			if (!acnt)
				continue;

			if (mode == 0 || mode == 1) { // ������ Ȥ�� ������ �����Ǿ� ���� ���
				MakePositionStopOrderRequest(mode, order, sym, acnt);
			} else { // ����, ���� ���ÿ� �����Ǿ� ���� ���
				MakePairStopOrderRequest(order, sym, acnt);
			}
		}
	}
}

// ����, ������ �ɷ� ���� ���� �� ���� �����ϴ� �Լ�
void VtCutManager::MakePositionStopByRemain(VtSymbol* sym)
{
	if (!sym || !_OrderConfigMgr || !_OrderConfigMgr->OrderMgr())
		return;
	std::map<int, VtOrder*> remainMap = _OrderConfigMgr->OrderMgr()->GetTotalRemain(sym->ShortCode);
	// �ܰ� ����� ��� ������ �������� �ʴ´�.
	if (remainMap.size() == 0)
		return;
	// ��带 Ȯ���Ѵ�.
	int mode = _EnableCutProfit && _EnableCutLoss ? 2 : (_EnableCutProfit ? 0 : 1);
	VtAccount* acnt = nullptr;
	if (_OrderConfigMgr->Type() == 0) {  // �ǰ��� �ֹ��϶� ���� ���� �ֹ��� ��
		acnt = _OrderConfigMgr->Account();
		if (acnt->AccountLevel() != 0) { // ���� ������ ��� �θ� ���¸� ���� �´�.
			acnt = acnt->ParentAccount();
		}
		if (!acnt) // ���°� ������ Ż��
			return;

		for (auto it = remainMap.begin(); it != remainMap.end(); ++it) {
			VtOrder* order = it->second;
			if (mode == 0 || mode == 1) { // ������ Ȥ�� ������ �����Ǿ� ���� ���
				MakePositionStopOrderRequest(mode, order, sym, acnt);
			} else { // ����, ���� ���ÿ� �����Ǿ� ���� ���
				MakePairStopOrderRequest(order, sym, acnt);
			}
		}
	}
	else { // �ݵ� �ֹ��� ���
		VtFund* fund = _OrderConfigMgr->Fund();
		for (auto it = remainMap.begin(); it != remainMap.end(); ++it) {
			VtOrder* order = it->second;
			// �θ� ���¸� �����´�.
			acnt = fund->GetParentAccount(order->SubAccountNo);
			if (!acnt)
				continue;

			if (mode == 0 || mode == 1) { // ������ Ȥ�� ������ �����Ǿ� ���� ���
				MakePositionStopOrderRequest(mode, order, sym, acnt);
			} else { // ����, ���� ���ÿ� �����Ǿ� ���� ���
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
	// ƽũ�� * ƽ�� * �¼� = ���� ����
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

	// ü��� ���� �־� �ش�.
	int targetValue = order->intFilledPrice;
	if (order->orderPosition == VtPositionType::Buy) { // �ܰ� �ż��� ��
		// �ս� Ȯ�� �̹Ƿ� ü��� ������ ƽũ�⸸ŭ ���ش�.
		targetValue -= cutTick * sym->intTickSize;
		// ������ ������ �۰ų� ������ ��ġ�ߴٰ� ���� �ƽ�ȣ �߻�
		if (sym->Quote.intClose <= targetValue) {
			// ���⼭ û�� �ֹ��� ���ش�.
			PutOrder(order, sym, 2, VtPriceType::Price);
			// �̹� û�� ����� ���������Ƿ� ���̻� ���� Ȯ���� ���� �ʰ� �Ѵ�.
			order->EnableLossCut = false;
			return true;
		}
	} else if (order->orderPosition == VtPositionType::Sell) { // �ܰ� �ŵ��� ��
		// �ս� Ȯ�� �̹Ƿ� ü��� ������ ƽũ�⸸ŭ �����ش�.
		targetValue += cutTick * sym->intTickSize;
		// ������ ������ ũ�ų� ������ ��ġ�ߴٰ� ���� �ƽ�ȣ �߻�
		if (sym->Quote.intClose >= targetValue) {
			// ���⼭ û�� �ֹ��� ���ش�.
			PutOrder(order, sym, 2, VtPriceType::Price);
			// �̹� û�� ����� ���������Ƿ� ���̻� ���� Ȯ���� ���� �ʰ� �Ѵ�.
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
	// ƽũ�� * ƽ�� * �¼� = ���� ����
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

	// ü��� ���� �־� �ش�.
	int targetValue = order->intFilledPrice;
	if (order->orderPosition == VtPositionType::Buy) { // �ܰ� �ż��� ��
		// ���� Ȯ�� �̹Ƿ� ü��� ������ ƽũ�⸸ŭ �����ش�.
		targetValue += cutTick * sym->intTickSize;
		// ������ ������ ������ ũ�ų� ������ ��ġ�ߴٰ� ���� �ƽ�ȣ �߻�
		if (sym->Quote.intClose >= targetValue) {
			// ���⼭ û�� �ֹ��� ���ش�.
			PutOrder(order, sym, 1, VtPriceType::Price);
			// �̹� û�� ����� ���������Ƿ� ���̻� ���� Ȯ���� ���� �ʰ� �Ѵ�.
			order->EnableProfitCut = false;
			return true;
		}
	}
	else if (order->orderPosition == VtPositionType::Sell) { // �ܰ� �ŵ��� ��
		// ���� Ȯ�� �̹Ƿ� ü��� ������ ƽũ�⸸ŭ ���ش�.
		targetValue -= cutTick * sym->intTickSize;
		// ������ ������ �۰ų� ������ ��ġ�ߴٰ� ���� �ƽ�ȣ �߻�
		if (sym->Quote.intClose <= targetValue) {
			// ���⼭ û�� �ֹ��� ���ش�.
			PutOrder(order, sym, 1, VtPriceType::Price);
			// �̹� û�� ����� ���������Ƿ� ���̻� ���� Ȯ���� ���� �ʰ� �Ѵ�.
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
	// �ܰ� �ֹ� ����� �����´�.
	std::map<int, VtOrder*> remainMap = _OrderConfigMgr->OrderMgr()->GetTotalRemain(sym->ShortCode);
	// �ܰ� ������ ó������ �ʴ´�.
	if (remainMap.size() == 0)
		return false;
	// �ܰ� ����� ���鼭 �սǰ� ������ Ȯ���Ѵ�.
	for (auto it = remainMap.begin(); it != remainMap.end(); ++it) { 
		VtOrder* order = it->second;
		if (!CheckCutLoss(order, sym, _CutLoss)) { // �ս�Ȯ���� �����ϸ� ����Ȯ���� �Ѵ�.
			CheckCutProfit(order, sym, _CutProfit); // ���� Ȯ���� �Ѵ�.
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
	// �ܰ� ����� ��� ������ �������� �ʴ´�.
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
	// �ܰ� ����� ��� ������ �������� �ʴ´�.
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
		if (request->RequestType == 1) { // ������ ��
			request->Price = avg + sym->intTickSize * _CutProfit;
		} else if (request->RequestType == 2) { // ���� �� ��
			request->Price = avg - sym->intTickSize * _CutLoss;
		}
	}
}

VtPosition* VtCutManager::FindPositionByOrder(VtOrder* order)
{
	if (!order)
		return nullptr;
	if (_OrderConfigMgr->Type() == 0) { // ���� �ֹ��� ��
		if (!_OrderConfigMgr->Account())
			return nullptr;

		VtAccount* acnt = _OrderConfigMgr->Account();
		return acnt->FindPosition(order->shortCode);
	} else { // �ݵ� �ֹ��� ��
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
	if (it != CutInfoMap.end()) { // ����, ���� ���� ��ü�� ��Ͽ� �ִ� ���
		CutInfoMap.erase(it);
	}
}

void VtCutManager::PutOrder(VtOrder* order, VtSymbol* sym, int reqType, VtPriceType priceType)
{
	if (!order || !sym || !_OrderConfigMgr || order->remainQty == 0)
		return;

	VtAccount* acnt = nullptr;
	if (order->Type == 0) { // �ǰ��� �ֹ�
		VtAccountManager* acntMgr = VtAccountManager::GetInstance();
		acnt = acntMgr->FindAccount(order->AccountNo);
	} else { // ������� Ȥ�� �ݵ� �ֹ��϶�
		VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
		acnt = subAcntMgr->FindAccount(order->SubAccountNo);
	}

	if (!acnt) // ���°� ���� ��� �ƹ��ϵ� ���� ����.
		return;

	HdOrderRequest request;
	request.Amount = std::abs(order->remainQty);
	if (acnt->AccountLevel() == 0) { // ������ �� ��
		request.AccountNo = acnt->AccountNo;
		request.Password = acnt->Password;
	} else { // ���� ���� �� ��
		VtAccount* parentAcnt = acnt->ParentAccount();
		if (parentAcnt) { // �θ� ���°� �ִ��� Ȯ�� �Ѵ�.
			request.AccountNo = parentAcnt->AccountNo;
			request.Password = parentAcnt->Password;
		} else
			return;
	}
	request.SymbolCode = order->shortCode;
	request.FillCondition = _FillCondition;

	request.PriceType = priceType;

	if (order->orderPosition == VtPositionType::Buy) { // �ż� û���� ���
		request.Price = VtSymbolManager::GetNextValue(sym->Quote.intClose,_OrderTypeSlip, sym);
	} else { // �ŵ� û���� ���
		request.Price = VtSymbolManager::GetNextValue(sym->Quote.intClose, -1 * _OrderTypeSlip, sym);
	}

	if (order->orderPosition == VtPositionType::Buy) { // �ż� û���� ���
		request.Position = VtPositionType::Sell;
	}
	else if (order->orderPosition == VtPositionType::Sell) { // �ŵ� û���� ���
		request.Position = VtPositionType::Buy;
	}

	request.RequestId = _OrderConfigMgr->OrderMgr()->GetOrderRequestID();
	request.SourceId = (long)this;

	if (acnt->AccountLevel() == 0) { // ������ �� ���
		request.SubAccountNo = _T("");
		request.FundName = _T("");
	} else { // ���� ������ ���
		request.SubAccountNo = acnt->AccountNo;
		if (acnt->Fund())
			request.FundName = acnt->Fund()->Name;
		else
			request.FundName = _T("");
	}

	request.orderType = VtOrderType::New;
	// ���� �ֹ��̳� û��
	// 0 : �Ϲ� , 1 : ����, 2 : ����- û�� �ֹ��� ���� ���� ������ Ȱ��ȭ �Ǿ� �־ ó������ �ʴ´�.
	request.RequestType = reqType;
	// û�� �ֹ� ��ȣ�� �־� �ش�.
	request.LiqReqOrderNo = order->orderNo;

	_OrderConfigMgr->OrderMgr()->PutOrder(std::move(request));
}

void VtCutManager::LiqudOrder(VtSymbol* sym, VtOrder* order, int totalRemain, VtPriceType priceType, int price)
{
	VtAccount* acnt = nullptr;
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	if (order->Type == 0)
		acnt = acntMgr->FindAccount(order->AccountNo);
	else { // �����¿� ���� ��� ���� ���¸� ã�� ����.
		VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
		acnt = subAcntMgr->FindAccount(order->SubAccountNo);
		acnt = acnt->ParentAccount();
	}

	if (!acnt) // ���°� ���� ��� �ƹ��ϵ� ���� ����.
		return;

	HdOrderRequest request;
	request.Amount = std::abs(totalRemain);
	request.AccountNo = acnt->AccountNo;
	request.Password = acnt->Password;

	request.SymbolCode = order->shortCode;
	request.FillCondition = _FillCondition;

	request.PriceType = priceType;

	if (totalRemain > 0) { // �ż� û���� ���
		if (priceType == VtPriceType::Price)
			request.Price = VtSymbolManager::GetNextValue(price, -1 *_OrderTypeSlip, sym);
		else
			request.Price = 0;
		request.Position = VtPositionType::Sell;
	}
	else { // �ŵ� û���� ���
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
	// ���� �ֹ��̳� û��
	// 0 : �Ϲ� , 1 : ����, 2 : ����- û�� �ֹ��� ���� ���� ������ Ȱ��ȭ �Ǿ� �־ ó������ �ʴ´�.
	request.RequestType = 1;

	_OrderConfigMgr->OrderMgr()->PutOrder(std::move(request));
}

