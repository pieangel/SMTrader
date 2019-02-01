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
	// ���⼭ �ܰ� ������ �ش�.
	VtPosition* oldPosi = acnt->FindPosition(curPosi.ShortCode);

	double curClose = sym->Quote.intClose / std::pow(10, sym->IntDecimal);
	// �ƹ��͵� ���� ���
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
		if (curPosi.Position == VtPositionType::Sell) { //ü������� �ŵ�	
			if (oldPosi->OpenQty > 0) { // ���������� �ż�			
				if (oldPosi->OpenQty >= curPosi.OpenQty) { //���������� ū���
					oldPosi->OpenQty = oldPosi->OpenQty - curPosi.OpenQty;
					oldPosi->TradePL += double(-curPosi.OpenQty * (oldPosi->AvgPrice - curPosi.AvgPrice)*sym->seungsu);
					if (oldPosi->OpenQty == 0) {
						oldPosi->AvgPrice = 0;
						oldPosi->OpenProfitLoss = 0;
					}
				}
				else { //ü������� ū ���
					oldPosi->TradePL += double(oldPosi->OpenQty * (curPosi.AvgPrice - oldPosi->AvgPrice)*sym->seungsu);
					oldPosi->AvgPrice = curPosi.AvgPrice;
					oldPosi->OpenQty = oldPosi->OpenQty - curPosi.OpenQty;
					oldPosi->OpenProfitLoss = oldPosi->OpenQty * (curClose - oldPosi->AvgPrice)*sym->seungsu;
				}
			}
			else { // ���������� �ŵ� ( �ŵ�/�ŵ� �ΰ�� ���� ����)
				oldPosi->AvgPrice = double((oldPosi->OpenQty * oldPosi->AvgPrice - curPosi.OpenQty* curPosi.AvgPrice) / (oldPosi->OpenQty - curPosi.OpenQty));
				oldPosi->OpenQty = oldPosi->OpenQty - curPosi.OpenQty;
				oldPosi->OpenProfitLoss = oldPosi->OpenQty * (curClose - oldPosi->AvgPrice)*sym->seungsu;
			}
		}
		else { //ü������� �ż�
			if (oldPosi->OpenQty >= 0) { // ���������� �ż�
				oldPosi->AvgPrice = double((oldPosi->OpenQty * oldPosi->AvgPrice + curPosi.OpenQty * curPosi.AvgPrice) / (oldPosi->OpenQty + curPosi.OpenQty));
				oldPosi->OpenQty = oldPosi->OpenQty + curPosi.OpenQty;
				oldPosi->OpenProfitLoss = oldPosi->OpenQty * (curClose - oldPosi->AvgPrice)*sym->seungsu;
			}
			else { //���������� �ŵ�
				if (abs(oldPosi->OpenQty) >= curPosi.OpenQty) { //���������� ū���
					oldPosi->OpenQty = oldPosi->OpenQty + curPosi.OpenQty;
					oldPosi->TradePL += double(curPosi.OpenQty * (oldPosi->AvgPrice - curPosi.AvgPrice)*sym->seungsu);
					if (oldPosi->OpenQty == 0) {
						oldPosi->AvgPrice = 0;
						oldPosi->OpenProfitLoss = 0;
					}
				}
				else { //ü������� ū ���				
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

// �ֹ� ü��� ������Ʈ �ϴ� �Լ� - ���� ���ڿ��� �Ǿ��־� �ε��Ҽ��� �ٲپ�� ��.
// ��� �ֹ��� �ܰ�� ���� ���� �ִ� �ܰ��� ������ �޴´�.
// ���� �ֹ��� �ܰ�� ������ �ܰ�� ���� �������� �ܰ� �ȴ�.
// �ܰ� 0�� �ƴ� �ֹ��� �ܰ� ��Ͽ� ���� �ǰ� û�� ����� �ȴ�.
void VtProductOrderManager::OnFilled(VtOrder* order)
{
	if (!order)
		return;
	// ���� �ֹ� ��Ͽ��� ������ �ش�.
	RemoveAcceptedOrder(order->orderNo);

	// �̹� ���� �ֹ��� �ƹ� ó���� ���� �ʴ´�.
	if (order->state == VtOrderState::Settled)
		return;

	VtAccount* acnt = nullptr;
	if (order->type == 0) { // �Ϲ� ���� �ֹ��� ���
		VtAccountManager* acntMgr = VtAccountManager::GetInstance();
		acnt = acntMgr->FindAccount(order->AccountNo);
	} else { // ���� ���� Ȥ�� �ݵ� �ֹ��� ���
		VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
		acnt = subAcntMgr->FindAccount(order->AccountNo);
	}
	if (!acnt)
		return;
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindHdSymbol(order->shortCode);
	if (!sym)
		return;
	// ���¿��� ���� �������� �����´�.
	VtPosition* posi = acnt->FindPosition(order->shortCode);

	double curClose = sym->Quote.intClose / std::pow(10, sym->IntDecimal);
	// �ֹ� �����ǿ� ���� ��ȣ ����
	int buho = order->orderPosition == VtPositionType::Buy ? 1 : -1;
	if (!posi) { // �������� ���� ���
		posi = acnt->CreatePosition(order);
		posi->Position = order->orderPosition;
		posi->AvgPrice = order->filledPrice;
		posi->OpenQty = buho *order->filledQty;
		posi->OpenProfitLoss = posi->OpenQty * (curClose - posi->AvgPrice)*sym->seungsu;
		// �ܰ� ������ �־� �ش�. �ŵ��� ����, �ż��� ���, ������ ������ 0
		order->remainQty = buho *order->filledQty;
		// �ܰ� �ֹ��� �ִ´�.
		AddToRemain(order);
		return;
	} else  { // �������� �ִ� ���
		RegisterOrderPosition(order, posi);
		if (order->orderPosition == VtPositionType::Sell) { //ü������� �ŵ�	
			if (posi->OpenQty > 0) { // ���������� �ż�			
				if (posi->OpenQty >= order->filledQty) { //���������� ũ�ų� ���� ���
					posi->OpenQty = posi->OpenQty - order->filledQty;
					posi->TradePL += double(-order->filledQty * (posi->AvgPrice - order->filledPrice)*sym->seungsu);
					// ���� �ֹ��� ��� �ȴ�.
					order->state = VtOrderState::Settled;
					order->remainQty = 0;
					if (posi->OpenQty == 0) { // �ż� ������ û��
						posi->AvgPrice = 0;
						posi->OpenProfitLoss = 0;
					}
					if ((order->RequestType == 1 || order->RequestType == 2) && 
						 order->LiqReqOrderNo > 0) { // �����̳� ������ ����  ������ û�� �ֹ��̴�.
						// ������ �ֹ��� ���� �ش�.
						RemoveByOrderNo(order->LiqReqOrderNo);
					}
					else { // ������ û�� �ֹ��� �ƴ� ���� �Ϲ����� ó���� �Ѵ�.
						// �� ��� ���� �ܰ� ����� ����� ��� �Ѵ�.
						AdjustRemainQ(-1 * order->filledQty);
					}
				} else { //ü������� ū ���
					posi->TradePL += double(posi->OpenQty * (order->filledPrice - posi->AvgPrice)*sym->seungsu);
					posi->AvgPrice = order->filledPrice;
					posi->OpenQty = posi->OpenQty - order->filledQty;
					posi->OpenProfitLoss = posi->OpenQty * (curClose - posi->AvgPrice)*sym->seungsu;
					// ���������� ���ǰ� ���� ������ �ܰ� �ȴ�. - ���⼭�� �ܰ� �ŵ� �������� �ȴ�.
					order->remainQty = posi->OpenQty - order->filledQty;
					// �̰�� ���� �ܰ� ����� ��� �������.
					RemoveAllRemain();
				}
			} else { // ���������� �ŵ� ( ���������̸ŵ�/ü������̸ŵ� �� ���)
				posi->AvgPrice = double((posi->OpenQty * posi->AvgPrice - order->filledQty* order->filledPrice) / (posi->OpenQty - order->filledQty));
				posi->OpenQty = posi->OpenQty - order->filledQty;
				posi->OpenProfitLoss = posi->OpenQty * (curClose - posi->AvgPrice)*sym->seungsu;
				// �̰�� �������� �����Ƿ� ���� ���� �ʴ´�.
				// �ܰ� ������ �־� �ش�. �ŵ��� ����, �ż��� ���, ������ ������ 0
				order->remainQty = buho *order->filledQty;
			}
		} else { //ü������� �ż�
			if (posi->OpenQty >= 0) { // ���������� �ż�/ü������̸ż� �� ���
				posi->AvgPrice = double((posi->OpenQty * posi->AvgPrice + order->filledQty * order->filledPrice) / (posi->OpenQty + order->filledQty));
				posi->OpenQty = posi->OpenQty + order->filledQty;
				posi->OpenProfitLoss = posi->OpenQty * (curClose - posi->AvgPrice)*sym->seungsu;
				// �̰�� �������� �����Ƿ� ���� ���� �ʴ´�.
				// �ܰ� ������ �־� �ش�. �ŵ��� ����, �ż��� ���, ������ ������ 0
				order->remainQty = buho *order->filledQty;
			} else { //���������� �ŵ�
				if (abs(posi->OpenQty) >= order->filledQty) { //���������� ū���
					posi->OpenQty = posi->OpenQty + order->filledQty;
					posi->TradePL += double(order->filledQty * (posi->AvgPrice - order->filledPrice)*sym->seungsu);
					order->state = VtOrderState::Settled;
					// ���� �ֹ��� ���ǰ� �������� ����Ѵ�.
					order->remainQty = 0;
					if (posi->OpenQty == 0) { // �ŵ� ������ û��
						posi->AvgPrice = 0;
						posi->OpenProfitLoss = 0;
					}

					if ((order->RequestType == 1 || order->RequestType == 2) &&
						order->LiqReqOrderNo > 0) { // �����̳� ������ ����  ������ û�� �ֹ��̴�.
													// ������ �ֹ��� ���� �ش�.
						RemoveByOrderNo(order->LiqReqOrderNo);
					} else { // ������ û�� �ֹ��� �ƴ� ���
						// �� ��� ���� �ܰ� ����� ��� ���� ��� �Ѵ�.
						RemoveAllRemain();
					}
				} else { //ü������� ū ���				
					posi->TradePL += double(posi->OpenQty * (order->filledPrice - posi->AvgPrice)*sym->seungsu);
					posi->AvgPrice = order->filledPrice;
					posi->OpenQty = posi->OpenQty + order->filledQty;
					posi->OpenProfitLoss = posi->OpenQty * (curClose - posi->AvgPrice)*sym->seungsu;
					// �̰�� ������ �ŵ� ������ �ܰ� ������ ���ǰ� ���� �ֹ��� �ܰ�� �ż� �������� �ȴ�.
					order->remainQty = posi->OpenQty + order->filledQty;
					// �� ��� �ܰ� ����� ������ ��� �Ѵ�.
					AdjustRemainQ(order->filledQty);
				}
			}
		}
	}

	// �ܰ� ������ ���� �������� �ٽ� ������ �ش�.
	if (posi->OpenQty > 0) { // �ż� ������ ���� : �����
		posi->Position = VtPositionType::Buy;
	} else if (posi->OpenQty < 0) { // �ŵ� ������ ���� : ������
		posi->Position = VtPositionType::Sell;
	} else { // ��� ������ ��� 0��
		posi->Position = VtPositionType::None;
		// ���̻� �ܰ� �����Ƿ� �ܰ�ǥ�� �ֹ��� ��� ������ �ش�.
		RemoveAllRemain();
	}
	
	// �ֹ� ��ü �ܰ� ���� �ܰ� ��Ͽ� �߰��� ������ �ش�.
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

//�ŵ��ܰ��� û���̸� - (û�갡�� - ��հ�)*û�����
//�ż��ܰ��� û���̸�(û�갡�� - ��հ�)*û�����
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

	// ��ǲ �������� ���Ѵ�.
	VtPositionType inputPos = newRemainQty > 0 ? VtPositionType::Buy : VtPositionType::Sell;
	int rem = newRemainQty;
	for (auto it = RemainMap.begin(); it != RemainMap.end();) {
		VtOrder* order = it->second;
		// ���� �ܰ�� ��ǲ �ܰ� ���Ѵ�.
		rem = order->remainQty + rem;
		// ���ο� �������� ���Ѵ�.
		VtPositionType newPos = rem == 0 ? VtPositionType::None : (rem > 0 ? VtPositionType::Buy : VtPositionType::Sell);
		if (newPos == inputPos || newPos == VtPositionType::None) { // �̰��� �ܰ� ��Ͽ��� ������ ��� �Ѵ�.
			RemainMap.erase(it++);
		} else { // ���� �ܰ� �˻��Ѵ�.
			++it;
		}
		// �������� �ٲ�� ������ Ż���Ѵ�. - �������� �ٲ���ٴ� ���� ��Ⱑ �����ٴ� ���� �ǹ��Ѵ�.
		if (inputPos != newPos) {
			// ���ο� �ܰ� ���� �ֹ��� �־� �ش�.
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
	// ������ �ֹ��� �������� �� �ֹ��� ���Ͽ� ������ �ֹ���ȣ�� ���� ���� �߰��Ѵ�.
	// �� �� �������� ������ �ֹ� ����� �̹� ������ �ִ�.
	for (int i = 0; i < acptCount; ++i)
	{
		;
	}
}
