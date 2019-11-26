#include "stdafx.h"
#include "VtFundManager.h"
#include "VtFund.h"
#include <msgpack.hpp>
#include "VtAccount.h"
#include "VtAccountManager.h"
#include "VtFundOrderManagerSelecter.h"
#include "VtFundOrderManager.h"
#include <vector>

VtFundManager::VtFundManager()
{
}


VtFundManager::~VtFundManager()
{
	for (auto it = _FundList.begin(); it != _FundList.end(); ++it)
	{
		delete it->second;
	}

	_FundList.clear();
}

void VtFundManager::CreateDefaultFund()
{
	AddFund(_T("Fund1"));
}

VtFund* VtFundManager::FindFund(std::string fundName)
{
	auto it = _FundList.find(fundName);
	if (it != _FundList.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

VtFund* VtFundManager::AddFund(std::string fundName)
{
	if (fundName.length() == 0)
		return nullptr;
	VtFund* fund = new VtFund();
	fund->Name = fundName;
	_FundList[fundName] = fund;

	return fund;
 }

void VtFundManager::DeleteFund(std::string fundName)
{
	auto it = _FundList.find(fundName);
	if (it != _FundList.end())
	{
		VtFund* fund = it->second;
		VtFundOrderManagerSelecter* fundOrderMgrSelector = VtFundOrderManagerSelecter::GetInstance();
		VtFundOrderManager* orderMgr = fundOrderMgrSelector->FindOrderManager(fund->Name);
		if (orderMgr)
		{
			orderMgr->CurFund(nullptr);
		}
		_FundList.erase(it);
		delete fund;
	}
}

void VtFundManager::Save(std::stringstream& ss)
{
	// Save the count of the fund list.
	msgpack::pack(ss, (int)_FundList.size());
	for (auto it = _FundList.begin(); it != _FundList.end(); ++it)
	{
		VtFund* fund = it->second;
		fund->Save(ss);
	}
}

void VtFundManager::Save(msgpack::sbuffer& ss)
{

}

void VtFundManager::Save(simple::file_ostream<same_endian_type>& ss)
{
	ss << (int)_FundList.size();
	for (auto it = _FundList.begin(); it != _FundList.end(); ++it)
	{
		VtFund* fund = it->second;
		fund->Save(ss);
	}
}

void VtFundManager::Load(std::stringstream& ss)
{
	// Get the count of the fund list;
	size_t offset = 0;
	auto oh = msgpack::unpack(ss.str().data(), ss.str().size(), offset);
	auto obj = oh.get();
	int count = 0;
	obj.convert(count);
	for (int i = 0; i < count; i++)
	{
		;
	}
}

void VtFundManager::Load(simple::file_istream<same_endian_type>& ss)
{
	int count = 0;
	ss >> count;

	if (count > 0)
	{
		for (int i = 0; i < count; i++)
		{
			VtFund* fund = new VtFund();
			fund->Load(ss);
			_FundList[fund->Name] = fund;
		}
	}
}

void VtFundManager::AddToUsedAccountMap(VtAccount* acnt)
{
	if (!acnt)
		return;

	_UsedAccountMap[acnt->AccountNo] = acnt;
}

void VtFundManager::RemoveFromUsedAccountMap(std::string acntNo)
{
	auto it = _UsedAccountMap.find(acntNo);
	if (it != _UsedAccountMap.end())
	{
		_UsedAccountMap.erase(it);
	}
}

void VtFundManager::AddToUnUsedAccountMap(VtAccount* acnt)
{
	if (!acnt)
		return;

	_UnusedAccountMap[acnt->AccountNo] = acnt;
}

void VtFundManager::RemoveFromUnUsedAccountMap(std::string acntNo)
{
	auto it = _UnusedAccountMap.find(acntNo);
	if (it != _UnusedAccountMap.end())
	{
		_UnusedAccountMap.erase(it);
	}
}

void VtFundManager::DivideAccountFromFundList()
{
	ClearUsedAccountList();
	ClearUnusedAccountList();

	if (_FundList.size() == 0)
	{
		VtAccountManager* acntMgr = VtAccountManager::GetInstance();
		for (auto it = acntMgr->AccountMap.begin(); it != acntMgr->AccountMap.end(); ++it)
		{
			VtAccount* acnt = it->second;
			std::vector<VtAccount*>& acntList = acnt->GetSubAccountList();
			for (auto it = acntList.begin(); it != acntList.end(); ++it)
			{
				VtAccount* subAcnt = *it;
				AddToUnUsedAccountMap(subAcnt);
			}
		}
	}
	else
	{
		for (auto it = _FundList.begin(); it != _FundList.end(); ++it)
		{
			VtFund* fund = it->second;
			std::vector<VtAccount*>& acntVec = fund->GetFundAccountVector();
			for (auto ait = acntVec.begin(); ait != acntVec.end(); ++ait)
			{
				VtAccount* subAcnt = *ait;
				AddToUsedAccountMap(subAcnt);
			}
		}

		VtAccountManager* acntMgr = VtAccountManager::GetInstance();
		for (auto it = acntMgr->AccountMap.begin(); it != acntMgr->AccountMap.end(); ++it)
		{
			VtAccount* acnt = it->second;
			std::vector<VtAccount*>& acntList = acnt->GetSubAccountList();
			for (auto it = acntList.begin(); it != acntList.end(); ++it)
			{
				VtAccount* subAcnt = *it;
				VtAccount* found = FindFromUsedAccountList(subAcnt->AccountNo);
				if (!found)
					AddToUnUsedAccountMap(subAcnt);
			}
		}
	}
}

void VtFundManager::ClearUsedAccountList()
{
	_UsedAccountMap.clear();
}

void VtFundManager::ClearUnusedAccountList()
{
	_UnusedAccountMap.clear();
}

VtAccount* VtFundManager::FindFromUnusedAccountList(std::string acntNo)
{
	auto it = _UnusedAccountMap.find(acntNo);
	if (it != _UnusedAccountMap.end())
		return it->second;
	else
		return nullptr;
}

VtAccount* VtFundManager::FindFromUsedAccountList(std::string acntNo)
{
	auto it = _UsedAccountMap.find(acntNo);
	if (it != _UsedAccountMap.end())
		return it->second;
	else
		return nullptr;
}

void VtFundManager::SaveToXml(pugi::xml_node& node_fund_list)
{
	for (auto it = _FundList.begin(); it != _FundList.end(); ++it) {
		VtFund* fund = it->second;
		pugi::xml_node node_fund = node_fund_list.append_child("fund");
		fund->SaveToXml(node_fund);
	}
}

void VtFundManager::LoadFromXml(pugi::xml_node& node_fund_list)
{
	for (pugi::xml_node fund_node = node_fund_list.child("fund"); fund_node; fund_node = fund_node.next_sibling("fund")) {
		VtFund* fund = new VtFund();
		fund->LoadFromXml(fund_node);
		_FundList[fund->Name] = fund;
	}
}
