#include "stdafx.h"
#include "VtChartOrderManager.h"
#include "VtOrder.h"
#include "VtOrderManagerSelector.h"
#include "VtOrderManager.h"

VtChartOrderManager::VtChartOrderManager()
{
	_Chart = nullptr;
}


VtChartOrderManager::~VtChartOrderManager()
{
}

void VtChartOrderManager::OrderArrived(VtOrder* order)
{
	if (!order) return;

	PreOrderMap[order->requestID] = order;
}

void VtChartOrderManager::AddPreOrder(VtOrder* order)
{
	if (!order) return;

	PreOrderMap[order->requestID] = order;
}

void VtChartOrderManager::OrderAccepted(VtOrder* order)
{
	if (!order) return;

	AcceptedMap[order->orderNo] = order;
}

void VtChartOrderManager::OrderFilled(VtOrder* order)
{
	if (!order) return;

	FilledMap[order->orderNo] = order;
}

void VtChartOrderManager::RemovePreOrder(int orderNo)
{
	auto it = PreOrderMap.find(orderNo);
	if (it != PreOrderMap.end())
	{
		PreOrderMap.erase(orderNo);
	}
}

void VtChartOrderManager::RemoveAccepted(int orderNo)
{
	auto it = AcceptedMap.find(orderNo);
	if (it != AcceptedMap.end())
	{
		AcceptedMap.erase(orderNo);
	}
}

void VtChartOrderManager::RemoveFilled(int orderNo)
{
	auto it = FilledMap.find(orderNo);
	if (it != FilledMap.end())
	{
		FilledMap.erase(orderNo);
	}
}

void VtChartOrderManager::PutOrder(int id, VtOrderCmd cmd, int Qty, double price, VtSymbolMaster* symMstr)
{
	VtOrderManagerSelector* orderMgrSelector = VtOrderManagerSelector::GetInstance();
	VtOrderManager* orderMgr = orderMgrSelector->ActiveOrderManager();
	orderMgr->PutOrder(id, cmd, Qty, price, symMstr);
}

void VtChartOrderManager::ChangeOrder(double newValue, VtOrder* oldOrder)
{
	if (!oldOrder) return;

	VtOrderManagerSelector* orderMgrSelector = VtOrderManagerSelector::GetInstance();
	VtOrderManager* orderMgr = orderMgrSelector->ActiveOrderManager();
	orderMgr->ChangeOrder(oldOrder, newValue);
}
