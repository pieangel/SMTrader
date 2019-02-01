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
