#include "stdafx.h"
#include "VtOrderQueueManager.h"
#include "VtOrderManager.h"
#include "VtHdClient.h"

VtOrderQueueManager::VtOrderQueueManager()
{
	_ready = true;
	StartThread();
}


VtOrderQueueManager::~VtOrderQueueManager()
{
	ClearOrders();
	HdOrderRequest req;
	req.RequestId = -100;
	_ready = true;
	_OrderQueue.push(std::move(req));
	if (_OrderThread.joinable()) 
		_OrderThread.join();
}

void VtOrderQueueManager::StartThread()
{
	_OrderThread = std::thread(&VtOrderQueueManager::ProcessOrder, this);
}

void VtOrderQueueManager::ProcessOrder()
{
	while (true)
	{
		if (!_ready) { continue; }
		_ready = false;
		HdOrderRequest req = _OrderQueue.pop();
		if (req.RequestId == -100)
		{
			break;
		}
		_pendingOrderReq[req.RequestId] = req;
		SendOrderRequest(req);
	}
}

void VtOrderQueueManager::SendOrderRequest(HdOrderRequest req)
{
	VtHdClient* client = VtHdClient::GetInstance();
	client->SendOrder(req);
}

void VtOrderQueueManager::RemoveOrderRequest(int reqId)
{
	auto it = _pendingOrderReq.find(reqId);
	if (it != _pendingOrderReq.end())
	{
		_pendingOrderReq.erase(it);
	}

	_ready = true;
}

void VtOrderQueueManager::AddOrderRequest(HdOrderRequest&& req)
{
	_OrderQueue.push(req);
}

void VtOrderQueueManager::ClearOrders()
{
	_OrderQueue.Clear();
}
