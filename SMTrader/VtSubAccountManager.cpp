#include "stdafx.h"
#include "VtSubAccountManager.h"
#include "VtAccount.h"

VtSubAccountManager::VtSubAccountManager()
{
}


VtSubAccountManager::~VtSubAccountManager()
{
}

VtAccount* VtSubAccountManager::FindAccount(std::string acntNo)
{
	auto it = AccountMap.find(acntNo);
	if (it != AccountMap.end())
		return it->second;
	else
		return nullptr;
}

VtAccount* VtSubAccountManager::FindAddAccount(VtAccount* acnt)
{
	if (!acnt)
		return nullptr;

	auto it = AccountMap.find(acnt->AccountNo);
	if (it != AccountMap.end())
		return it->second;
	else {
		AccountMap[acnt->AccountNo] = acnt;
	}

	return acnt;
}

void VtSubAccountManager::AddAccount(VtAccount* acnt)
{
	AccountMap[acnt->AccountNo] = acnt;
}

void VtSubAccountManager::RemoveAccount(std::string acntNo)
{
	auto it = AccountMap.find(acntNo);
	if (it != AccountMap.end())
		AccountMap.erase(it);
}
