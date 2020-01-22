#include "stdafx.h"
#include "SmOrderManager.h"
#include "VtOrder.h"

SmOrderManager::SmOrderManager()
{
}


SmOrderManager::~SmOrderManager()
{
}

void SmOrderManager::AddOrder(VtOrder* order)
{
	if (!order)
		return;
	_TotalOrderMap[(order->orderNo)] = order;
}

void SmOrderManager::AddAcceptedOrder(VtOrder* order)
{
	if (!order)
		return;
	_AcceptedOrderMap[(order->orderNo)] = order;
}

void SmOrderManager::AddFilledOrder(VtOrder* order)
{
	if (!order)
		return;
	_FilledOrderMap[(order->orderNo)] = order;
}
