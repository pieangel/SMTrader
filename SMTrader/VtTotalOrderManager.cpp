#include "stdafx.h"
#include "VtTotalOrderManager.h"
#include "VtOrder.h"
#include "VtPosition.h"

VtTotalOrderManager::VtTotalOrderManager()
{
}


VtTotalOrderManager::~VtTotalOrderManager()
{
}

void VtTotalOrderManager::AddOrder(VtOrder* order)
{
	if (!order)
		return;

	_OrderMap[order->orderNo] = order;
}

void VtTotalOrderManager::AddAcceptedOrder(VtOrder* order)
{
	if (!order)
		return;
	_AcceptedMap[order->orderNo] = order;
}

void VtTotalOrderManager::RemoveAcceptedOrder(VtOrder* order)
{
	if (!order)
		return;
	auto it = _AcceptedMap.find(order->orderNo);
	if (it != _AcceptedMap.end()) {
		_AcceptedMap.erase(it);
	}
}

void VtTotalOrderManager::RemoveAcceptedOrder(std::string orderNo)
{
	auto it = _AcceptedMap.find(orderNo);
	if (it != _AcceptedMap.end()) {
		_AcceptedMap.erase(it);
	}
}

void VtTotalOrderManager::AddFilledOrder(VtOrder* order)
{
	if (!order)
		return;
	_FilledMap[order->orderNo] = order;
}

void VtTotalOrderManager::AddPosition(int type, std::string acntNo, std::string symCode, VtPosition* posi)
{
	if (!posi)
		return;
	_PositionMap[std::make_tuple(type, acntNo, symCode)] = posi;
}

void VtTotalOrderManager::RemovePosition(int type, std::string acntNo, std::string symCode)
{
	auto key = std::make_tuple(type, acntNo, symCode);
	auto it = _PositionMap.find(key);
	if (it != _PositionMap.end()) {
		_PositionMap.erase(it);
	}
}

std::vector<std::string> VtTotalOrderManager::GetUnsettledList()
{
	std::vector<std::string> msgList;
	std::string msg;
	if (_AcceptedMap.size() > 0) {
		msgList.push_back(_T("���� ����Ȯ�� �ֹ� ���"));
		for (auto it = _AcceptedMap.begin(); it != _AcceptedMap.end(); ++it) {
			VtOrder* order = it->second;
			if (order->state == VtOrderState::Accepted) {
				msg = _T("���¹�ȣ : ");
				msg.append(order->AccountNo);
				msg.append(_T("�����ڵ� : "));
				msg.append(order->shortCode);
				msg.append(_T("�ֹ����� : "));
				if (order->orderPosition == VtPositionType::Buy)
					msg.append(_T("�ż�"));
				else if (order->orderPosition == VtPositionType::Sell)
					msg.append(_T("�ŵ�"));
				msg.append(std::to_string(order->amount));
				msgList.push_back(msg);
			}
		}
	}

	if (_PositionMap.size() > 0) {
		bool enter = false;
		for (auto it = _PositionMap.begin(); it != _PositionMap.end(); ++it) {
			VtPosition* posi = it->second;
			if (std::abs(posi->OpenQty) > 0) {
				if (!enter) {
					msgList.push_back(_T("���� �ܰ� ���"));
					enter = true;
				}
				msg = _T("���¹�ȣ : ");
				msg.append(posi->AccountNo);
				msg.append(_T("�����ڵ� : "));
				msg.append(posi->ShortCode);
				msg.append(_T("�ܰ���� : "));
				if (posi->OpenQty > 0) {
					msg.append(_T("�ż�"));
				}
				else if (posi->OpenQty < 0) {
					msg.append(_T("�ŵ�"));
				}
				msg.append(std::to_string(std::abs(posi->OpenQty)));
				msgList.push_back(msg);
			}
		}
	}

	return msgList;
}

size_t VtTotalOrderManager::GetUnsettledCount()
{
	size_t count = _AcceptedMap.size();
	for (auto it = _PositionMap.begin(); it != _PositionMap.end(); ++it) {
		VtPosition* posi = it->second;
		if (std::abs(posi->OpenQty) > 0) {
			count++;
		}
	}

	return count;
}
