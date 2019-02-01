#include "stdafx.h"
#include "VtRealtimeRegisterManager.h"
#include "VtKrClient.h"
#include "VtHogaManager.h"
#include "VtQuoteManager.h"
#include "VtHoga.h"
#include "VtQuote.h"
#include "VtHdClient.h"

VtRealtimeRegisterManager::VtRealtimeRegisterManager()
{
}


VtRealtimeRegisterManager::~VtRealtimeRegisterManager()
{
}

void VtRealtimeRegisterManager::AddAccount(std::string acnt)
{
	AccountMap[acnt] = acnt;
}

void VtRealtimeRegisterManager::AddProduct(std::string prdt)
{
	ProductMap[prdt] = prdt;
}

void VtRealtimeRegisterManager::RemoveAccount(std::string acnt)
{
	auto it = AccountMap.find(acnt);
	if (it != AccountMap.end())
	{
		AccountMap.erase(it);
	}
}

void VtRealtimeRegisterManager::RemoveProduct(std::string prdt)
{
	auto it = ProductMap.find(prdt);
	if (it != ProductMap.end())
	{
		ProductMap.erase(it);
	}
}

void VtRealtimeRegisterManager::RegisterRecentProduct(std::string prdt)
{
	RecentProductMap[prdt] = prdt;
}

void VtRealtimeRegisterManager::UnregisterRecentProduct(std::string prdt)
{
	auto it = RecentProductMap.find(prdt);
	if (it != RecentProductMap.end())
	{
		RecentProductMap.erase(it);
	}
}

void VtRealtimeRegisterManager::RegisterAccount(std::string acnt)
{
	auto it = _AccountMap.find(acnt);
	if (it != _AccountMap.end())
	{
		VtRegisterItem item = _AccountMap[acnt];
		item.RefCount++;
		_AccountMap[acnt] = item;
	}
	else
	{
		VtHdClient* client = VtHdClient::GetInstance();
		CString userId = client->GetUserId();
		if (userId.GetLength() > 0)
		{
			client->RegisterAccount(acnt.c_str());
			VtRegisterItem item;
			item.Name = acnt;
			item.RefCount = 1;
			_AccountMap[acnt] = item;
		}
	}
}

void VtRealtimeRegisterManager::RegisterProduct(std::string prdt)
{
	auto it = _ProductMap.find(prdt);
	if (it != _ProductMap.end())
	{
		VtRegisterItem item = _ProductMap[prdt];
		item.RefCount++;
		_ProductMap[prdt] = item;
	}
	else
	{
		VtHdClient* client = VtHdClient::GetInstance();
		client->RegisterProduct(prdt.c_str());
		client->RegisterExpected(prdt.c_str());
		VtRegisterItem item;
		item.Name = prdt;
		item.RefCount = 1;
		_ProductMap[prdt] = item;
	}
}

void VtRealtimeRegisterManager::UnregisterAccount(std::string acnt)
{
	auto it = _AccountMap.find(acnt);
	if (it != _AccountMap.end())
	{
		VtRegisterItem item = _AccountMap[acnt];
		item.RefCount--;
		if (item.RefCount == 0)
		{
			VtHdClient* client = VtHdClient::GetInstance();
			CString userId = client->GetUserId();
			if (userId.GetLength() > 0)
			{
				client->UnregisterAccount(acnt.c_str());
				_AccountMap.erase(it);
			}
		}
	}
}

void VtRealtimeRegisterManager::UnregisterProduct(std::string prdt)
{
	auto it = _ProductMap.find(prdt);
	if (it != _ProductMap.end())
	{
		VtRegisterItem item = _ProductMap[prdt];
		item.RefCount--;
		if (item.RefCount == 0)
		{
			VtHdClient* client = VtHdClient::GetInstance();
			client->UnregisterProduct(prdt.c_str());
			client->UnregisterExpected(prdt.c_str());
			_ProductMap.erase(it);
		}
	}
}

void VtRealtimeRegisterManager::UnRegisterAll()
{
	VtKrClient* client = VtKrClient::GetInstance();
	for (auto it = AccountMap.begin(); it != AccountMap.end(); ++it)
	{
		std::string acntNo = it->second;
		client->UnRegisterAccountAutoUpdate(acntNo);
	}

	AccountMap.clear();

	for (auto it = ProductMap.begin(); it != ProductMap.end(); ++it)
	{
		std::string prdt = it->second;
		client->UnReisterProductAutoUpdate(prdt);
	}

	ProductMap.clear();
}

void VtRealtimeRegisterManager::RegisterCurrent()
{
	VtHdClient* client = VtHdClient::GetInstance();
	client->RegisterCurrent();
}

void VtRealtimeRegisterManager::UnregisterCurrent()
{
	VtHdClient* client = VtHdClient::GetInstance();
	client->UnregisterCurrent();
}
