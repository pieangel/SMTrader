#include "stdafx.h"
#include "VtQuoteManager.h"
#include "VtKrClient.h"
#include "VtQuote.h"
#include "VtTickQuote.h"
#include "VtQuoteEvent.h"
#include "Poco/Delegate.h"
#include "VtOrderDialogManager.h"
#include "VtWindowManager.h"
#include "VtOrderCenterWnd.h"
#include "VtOrderLeftWnd.h"

using Poco::Delegate;

VtQuoteManager::VtQuoteManager()
{
	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	_RealtimeQuoteReceivedEvent += delegate(orderDlgMgr, &VtOrderDialogManager::OnRealtimeQuoteReceived);
}


VtQuoteManager::~VtQuoteManager()
{
	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	_RealtimeQuoteReceivedEvent -= delegate(orderDlgMgr, &VtOrderDialogManager::OnRealtimeQuoteReceived);

	for (auto it = RealtimeQuoteMap.begin(); it != RealtimeQuoteMap.end(); ++it)
	{
		delete it->second;
	}

	for (auto it = TickQuoteMap.begin(); it != TickQuoteMap.end(); ++it)
	{
		delete it->second;
	}

	for (auto it = QuoteMap.begin(); it != QuoteMap.end(); ++it)
	{
		delete it->second;
	}
}

void VtQuoteManager::AddRealtimeQuote(std::string fullCode, VtRealtimeQuote* realQuote)
{
	if (!realQuote)
		return;

	RealtimeQuoteMap[fullCode] = realQuote;
}

void VtQuoteManager::RemoveRealtimeQuote(std::string fullCode)
{
	auto it = RealtimeQuoteMap.find(fullCode);
	if (it != RealtimeQuoteMap.end())
	{
		VtRealtimeQuote* quote = it->second;
		delete quote;
		RealtimeQuoteMap.erase(it);
	}
}

void VtQuoteManager::ReceiveRealtimeQuote(std::string fullCode, LPCTSTR packet)
{
	VtRealtimeQuote* quote = FindAddRealQuote(fullCode);
	if (!quote)
		return;

	memcpy(quote, packet, sizeof(VtRealtimeQuote));

	VtRealtimeQuoteEventArgs arg;
	arg.pQuote = quote;
	FireRealtimeQuoteReceived(std::move(arg));

	VtWindowManager* winMgr = VtWindowManager::GetInstance();
	CVtOrderCenterWnd* wnd = (CVtOrderCenterWnd*)winMgr->FindWindow(_T("OrderCenterWnd"));
	if (wnd)
	{
		wnd->OnReceiveRealtimeQuote(quote);
	}
}

void VtQuoteManager::ReceiveRealtimeQuote(VtQuote* quote)
{
	if (!quote)
		return;
	VtOrderDialogManager* orderWndMgr = VtOrderDialogManager::GetInstance();
	orderWndMgr->OnReceiveRealtimeQuote(quote);
}

VtRealtimeQuote* VtQuoteManager::FindRealtimeQuote(std::string fullCode)
{
	auto it = RealtimeQuoteMap.find(fullCode);
	if (it != RealtimeQuoteMap.end())
	{
		return it->second;
	}
	else
		return nullptr;
}

VtRealtimeQuote* VtQuoteManager::FindAddRealQuote(std::string fullCode)
{
	auto it = RealtimeQuoteMap.find(fullCode);
	if (it != RealtimeQuoteMap.end())
	{
		return it->second;
	}
	else
	{
		VtRealtimeQuote* quote = new VtRealtimeQuote();
		RealtimeQuoteMap[fullCode] = quote;
		return quote;
	}
}

void VtQuoteManager::AddTickQuote(std::string fullCode, VtTickQuote* realQuote)
{
	if (!realQuote)
		return;

	TickQuoteMap[fullCode] = realQuote;
}

void VtQuoteManager::RemoveTickQuote(std::string fullCode)
{
	auto it = TickQuoteMap.find(fullCode);
	if (it != TickQuoteMap.end())
	{
		VtTickQuote* quote = it->second;
		delete quote;
		TickQuoteMap.erase(it);
	}
}

void VtQuoteManager::ReceiveTickQuote(std::string fullCode, LPCTSTR packet)
{
	VtTickQuote* currentQuote = FindAddTickQuote(fullCode);
	if (!currentQuote)
		return;

	memcpy(currentQuote, packet, sizeof(VtTickQuote));
}

VtTickQuote* VtQuoteManager::FindTickQuote(std::string fullCode)
{
	auto it = TickQuoteMap.find(fullCode);
	if (it != TickQuoteMap.end())
	{
		return it->second;
	}
	else
		return nullptr;
}

VtTickQuote* VtQuoteManager::FindAddTickQuote(std::string fullCode)
{
	auto it = TickQuoteMap.find(fullCode);
	if (it != TickQuoteMap.end())
	{
		return it->second;
	}
	else
	{
		VtTickQuote* quote = new VtTickQuote();
		quote->FullCode = fullCode;
		TickQuoteMap[fullCode] = quote;
		return quote;
	}
}

void VtQuoteManager::AddQuote(std::string fullCode, VtQuote* realQuote)
{
	if (!realQuote)
		return;

	QuoteMap[fullCode] = realQuote;
}

void VtQuoteManager::RemoveQuote(std::string fullCode)
{
	auto it = QuoteMap.find(fullCode);
	if (it != QuoteMap.end())
	{
		VtQuote* quote = it->second;
		delete quote;
		QuoteMap.erase(it);
	}
}

void VtQuoteManager::ReceiveQuote(std::string fullCode, LPCTSTR packet)
{
	VtQuote* quote = FindAddQuote(fullCode);
	if (!quote)
		return;
	
	memcpy(quote, packet, sizeof(VtQuote));

	
	VtWindowManager* winMgr = VtWindowManager::GetInstance();
	CVtOrderCenterWnd* wnd = (CVtOrderCenterWnd*)winMgr->FindWindow(_T("OrderCenterWnd"));
	if (wnd)
	{
		wnd->InitQuote(quote);
	}
}

void VtQuoteManager::ReceiveQuote(VtQuote* quote)
{
	if (!quote)
		return;
	VtOrderDialogManager* orderWndMgr = VtOrderDialogManager::GetInstance();
	orderWndMgr->OnReceiveQuote(quote);
}

VtQuote* VtQuoteManager::FindQuote(std::string fullCode)
{
	auto it = QuoteMap.find(fullCode);
	if (it != QuoteMap.end())
	{
		return it->second;
	}
	else
		return nullptr;
}

VtQuote* VtQuoteManager::FindAddQuote(std::string fullCode)
{
	auto it = QuoteMap.find(fullCode);
	if (it != QuoteMap.end())
	{
		return it->second;
	}
	else
	{
		VtQuote* quote = new VtQuote();
		quote->fullCode = fullCode;
		QuoteMap[fullCode] = quote;
		return quote;
	}
}
