#include "stdafx.h"
#include "VtOrderManagerSelector.h"
#include "VtOrderManager.h"
#include "VtAccount.h"
#include "VtAccountManager.h"
#include "VtSubAccountManager.h"

VtOrderManagerSelector::VtOrderManagerSelector()
{
	ActiveOrderManager(nullptr);
}


VtOrderManagerSelector::~VtOrderManagerSelector()
{
	for (auto it = _OrderManagerMap.begin(); it != _OrderManagerMap.end(); ++it)
	{
		VtOrderManager* orderMgr = it->second;
		delete it->second;
	}
}

VtOrderManager* VtOrderManagerSelector::FindAddOrderManager(std::string acntNo)
{
	auto it = _OrderManagerMap.find(acntNo);
	if (it == _OrderManagerMap.end()) {
		VtOrderManager* orderMgr = new VtOrderManager();
		VtAccountManager* acntMgr = VtAccountManager::GetInstance();
		VtAccount* acnt = acntMgr->FindAccount(acntNo);
		orderMgr->CurAccount(acnt);
		_OrderManagerMap[acntNo] = orderMgr;
		orderMgr->AccountNo = acntNo;
		if (acnt) // ½Ç°èÁÂ
			orderMgr->AccountLevel = acnt->AccountLevel();
		else // ¼­ºê °èÁÂ
			orderMgr->AccountLevel = 1;
		return orderMgr;
	}
	else {
		return it->second;
	}
}

VtOrderManager* VtOrderManagerSelector::CreateOrderManager(VtAccount* acnt)
{
	VtOrderManager* orderMgr = new VtOrderManager();
	orderMgr->CurAccount(acnt);
	orderMgr->AccountNo = acnt->AccountNo;
	orderMgr->AccountLevel = acnt->AccountLevel();
	_OrderManagerMap[acnt->AccountNo] = orderMgr;

	return orderMgr;
}

void VtOrderManagerSelector::SetActiveOrderManager(VtAccount* acnt)
{
	if (!acnt) return;

	_ActiveOrderManager = FindOrderManager(acnt->AccountNo);
}

VtOrderManager* VtOrderManagerSelector::FindOrderManager(std::string acntNo)
{
	auto it = _OrderManagerMap.find(acntNo);
	if (it != _OrderManagerMap.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

void VtOrderManagerSelector::Save(simple::file_ostream<same_endian_type>& ss)
{
	// Save the count of the order managers for the accounts.
	ss << (int)_OrderManagerMap.size();

	for (auto it = _OrderManagerMap.begin(); it != _OrderManagerMap.end(); ++it)
	{
		VtOrderManager* orderMgr = it->second;
		ss << orderMgr->AccountNo;
		ss << orderMgr->AccountLevel;
		orderMgr->Save(ss);
	}
}

void VtOrderManagerSelector::Load(simple::file_istream<same_endian_type>& ss)
{
	int count = 0;
	ss >> count;
	std::string accountNo;
	int accountLevel = 0;
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
	VtAccount* acnt = nullptr;
	for (int i = 0; i < count; ++i)
	{
		ss >> accountNo >> accountLevel;
		if (accountLevel == 0)
		{
			acnt = acntMgr->FindAccount(accountNo);
		}
		else
		{
			acnt = subAcntMgr->FindAccount(accountNo);
		}
		if (acnt)
		{
			VtOrderManager* orderMgr = CreateOrderManager(acnt);
			orderMgr->Load(ss);
		}
	}
}
