#include "stdafx.h"
#include "VtStopOrderManager.h"
#include "VtSymbolManager.h"


VtStopOrderManager::VtStopOrderManager()
{
}


VtStopOrderManager::~VtStopOrderManager()
{
	RemoveAll();
	RemoveAllHd();
}

void VtStopOrderManager::AddOrder(VtOrderRequest* req)
{
	if (!req)
		return;
	StopOrderMap[req->requestID] = req;
}

VtOrderRequest* VtStopOrderManager::FindOrder(int reqID)
{
	auto it = StopOrderMap.find(reqID);
	if (it != StopOrderMap.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

void VtStopOrderManager::RemoveOrder(int reqID)
{
	auto it = StopOrderMap.find(reqID);
	if (it != StopOrderMap.end())
	{
		VtOrderRequest* order = it->second;
		StopOrderMap.erase(it);
		delete order;
	}
}

void VtStopOrderManager::RemoveAll()
{
	for (auto it = StopOrderMapHd.begin(); it != StopOrderMapHd.end(); it++)
	{
		HdOrderRequest* order = it->second;
		delete order;
	}
	StopOrderMap.clear();
}

void VtStopOrderManager::RemoveBuyAll()
{
	for (auto it = StopOrderMapHd.begin(); it != StopOrderMapHd.end(); )
	{
		HdOrderRequest* order = it->second;
		if (order->Position == VtPositionType::Buy)
		{
			it = StopOrderMapHd.erase(it);
			delete order;
		}
		else
			it++;
	}
}

void VtStopOrderManager::RemoveSellAll()
{
	for (auto it = StopOrderMapHd.begin(); it != StopOrderMapHd.end(); )
	{
		HdOrderRequest* order = it->second;
		if (order->Position == VtPositionType::Sell)
		{
			it = StopOrderMapHd.erase(it);
			delete order;
		}
		else
			it++;
	}
}

void VtStopOrderManager::AddOrderHd(HdOrderRequest* req)
{
	if (!req)
		return;
	StopOrderMapHd[req->RequestId] = req;
}

HdOrderRequest* VtStopOrderManager::FindOrderHd(int reqID)
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

void VtStopOrderManager::RemoveOrderHd(int reqID)
{
	auto it = StopOrderMapHd.find(reqID);
	if (it != StopOrderMapHd.end())
	{
		HdOrderRequest* order = it->second;
		StopOrderMapHd.erase(it);
		delete order;
	}
}

void VtStopOrderManager::RemoveAllHd()
{
	for (auto it = StopOrderMapHd.begin(); it != StopOrderMapHd.end(); it++)
	{
		HdOrderRequest* order = it->second;
		delete order;
	}
	StopOrderMapHd.clear();
}

void VtStopOrderManager::RemoveBuyAllHd()
{
	for (auto it = StopOrderMapHd.begin(); it != StopOrderMapHd.end(); )
	{
		HdOrderRequest* order = it->second;
		if (order->Position == VtPositionType::Buy)
		{
			it = StopOrderMapHd.erase(it);
			delete order;
		}
		else
			it++;
	}
}

void VtStopOrderManager::RemoveSellAllHd()
{
	for (auto it = StopOrderMapHd.begin(); it != StopOrderMapHd.end(); )
	{
		HdOrderRequest* order = it->second;
		if (order->Position == VtPositionType::Sell)
		{
			it = StopOrderMapHd.erase(it);
			delete order;
		}
		else
			it++;
	}
}
