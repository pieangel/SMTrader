#include "stdafx.h"
#include "SmCallbackManager.h"


SmCallbackManager::SmCallbackManager()
{
}


SmCallbackManager::~SmCallbackManager()
{
}

void SmCallbackManager::UnsubscribeQuoteCallback(long id)
{
	auto it = _QuoteMap.find(id);
	if (it != _QuoteMap.end()) {
		_QuoteMap.erase(it);
	}
}

void SmCallbackManager::OnQuoteEvent( VtSymbol* symbol)
{
	if (!symbol)
		return;
	// 맵에 있는 모든 함수를 호출한다.
	for (auto it = _QuoteMap.begin(); it != _QuoteMap.end(); ++it) {
		it->second(symbol);
	}
}

void SmCallbackManager::OnHogaEvent( VtSymbol* symbol)
{
	if (!symbol)
		return;
	// 맵에 있는 모든 함수를 호출한다.
	for (auto it = _HogaMap.begin(); it != _HogaMap.end(); ++it) {
		it->second(symbol);
	}
}

void SmCallbackManager::UnsubscribeHogaCallback(long id)
{
	auto it = _HogaMap.find(id);
	if (it != _HogaMap.end()) {
		_HogaMap.erase(it);
	}
}

void SmCallbackManager::UnsubscribeOrderCallback(long id)
{
	auto it = _OrderMap.find(id);
	if (it != _OrderMap.end()) {
		_OrderMap.erase(it);
	}
}

void SmCallbackManager::OnOrderEvent( VtOrder* order)
{
	if (!order)
		return;
	// 맵에 있는 모든 함수를 호출한다.
	for (auto it = _OrderMap.begin(); it != _OrderMap.end(); ++it) {
		it->second(order);
	}
}
