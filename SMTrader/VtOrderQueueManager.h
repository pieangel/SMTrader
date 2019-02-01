#pragma once
#include "Global/TemplateSingleton.h"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>
#include "VtOrder.h"
#include "HdTaskEventArg.h"
#include "VtCorrentQueue.h"
#include "HdTaskArg.h"

class VtOrderQueueManager : public TemplateSingleton<VtOrderQueueManager>
{
public:
	VtOrderQueueManager();
	~VtOrderQueueManager();


	HdOrderRequest& Peek()
	{
		return _queue.front();
	}
	void StartThread();
	void ProcessOrder();
	void SendOrderRequest(HdOrderRequest req);
	void RemoveOrderRequest(int reqId);
	void AddOrderRequest(HdOrderRequest&& req);
	bool Ready() const { return _ready; }
	void Ready(bool val) { _ready = val; }
	void ClearOrders();
private:

	VtQueue<HdOrderRequest> _OrderQueue;

	std::thread _OrderThread;
	std::queue<HdOrderRequest> _queue;
	bool _ready;
	// Key : order request no , value : HdOrderRequest.
	std::map<int, HdOrderRequest> _pendingOrderReq;
	// key : order no, value : HdOrderRequest.
	std::map<int, HdOrderRequest> _requestMap;
};

