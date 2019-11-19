#pragma once
#include <map>
#include <functional>
#include "Global/TemplateSingleton.h"

class VtSymbol;
struct VtOrder;
class SmCallbackManager : public TemplateSingleton<SmCallbackManager>
{
public:
	SmCallbackManager();
	~SmCallbackManager();
	
	void SubscribeQuoteCallback(long id, std::function <void( VtSymbol* symbol)> callback) {
		_QuoteMap[id] = callback;
	}
	void UnsubscribeQuoteCallback(long id);
	void OnQuoteEvent( VtSymbol* symbol);
	void OnHogaEvent( VtSymbol* symbol);
	
	void SubscribeHogaCallback(long id, std::function <void( VtSymbol* symbol)> callback) {
		_HogaMap[id] = callback;
	}
	void UnsubscribeHogaCallback(long id);

	void SubscribeOrderCallback(long id, std::function <void( VtOrder* order)> callback) {
		_OrderMap[id] = callback;
	}
	void UnsubscribeOrderCallback(long id);

	void OnOrderEvent( VtOrder* order);

	void SubscribeMasterCallback(long id, std::function <void(VtSymbol* symbol)> callback) {
		_MasterMap[id] = callback;
	}
	void UnsubscribeMasterCallback(long id);
	void OnMasterEvent(VtSymbol* symbol);

private:
	std::map<long, std::function<void( VtSymbol* symbol)>> _QuoteMap;
	std::map<long, std::function<void( VtSymbol* symbol)>> _HogaMap;
	std::map<int, std::function<void( VtOrder* order)>> _OrderMap;
	std::map<int, std::function<void(VtSymbol* symbol)>> _MasterMap;
};

