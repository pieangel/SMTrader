#include "stdafx.h"
#include "VtFundMap.h"
#include "VtFundAccount.h"
#include "VtAccount.h"
#include <msgpack.hpp>
#include "VtSubAccountManager.h"
#include "VtFund.h"
VtFundMap::VtFundMap()
{
}


VtFundMap::~VtFundMap()
{
	_Fund = nullptr;
}

VtAccount* VtFundMap::AddAccount(VtAccount* acnt)
{
	if (!acnt)
		return nullptr;

	_AccountVector.push_back(acnt);
	return nullptr;
}

void VtFundMap::RemoveAccount(std::string acntNo)
{
	for (auto it = _AccountVector.begin(); it != _AccountVector.end(); ++it)
	{
		VtAccount* acnt = *it;
		if (acnt->AccountNo.compare(acntNo) == 0)
		{
			delete acnt;
			_AccountVector.erase(it);
			break;
		}
	}
}

VtAccount* VtFundMap::FindAccount(std::string acntNo)
{
	for (auto it = _AccountVector.begin(); it != _AccountVector.end(); ++it)
	{
		VtAccount* acnt = *it;
		if (acnt->AccountNo.compare(acntNo) == 0)
		{
			return acnt;
		}
	}

	return nullptr;
}

void VtFundMap::ClearAllFundAccounts()
{
 	_AccountVector.clear();
}

void VtFundMap::Save(std::stringstream& ss)
{
	// Save the count of the fund account list;
	msgpack::pack(ss, (int)_AccountVector.size());
	for (auto it = _AccountVector.begin(); it != _AccountVector.end(); ++it)
	{
		VtAccount* fundAcnt = *it;
		//fundAcnt->Save(ss);
	}
}

void VtFundMap::Save(msgpack::sbuffer& ss)
{
	// Save the count of the fund account list;
	msgpack::pack(ss, (int)_AccountVector.size());
	for (auto it = _AccountVector.begin(); it != _AccountVector.end(); ++it)
	{
		VtAccount* fundAcnt = *it;
		//fundAcnt->Save(ss);
	}
}

void VtFundMap::Save(simple::file_ostream<same_endian_type>& ss)
{
	ss << (int)_AccountVector.size();
	for (auto it = _AccountVector.begin(); it != _AccountVector.end(); ++it)
	{
		// 서브 계좌 정보 전체를 저장한다.
		VtAccount* fundAcnt = *it;
		//fundAcnt->Save(ss);
		ss << fundAcnt->AccountNo;
	}
}

void VtFundMap::Load(simple::file_istream<same_endian_type>& ss)
{
	int count;
	ss >> count;
	VtAccount* acnt = nullptr;
	for (int i = 0; i < count; i++)
	{
		std::string acntNo;
		ss >> acntNo;
		VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
		VtAccount* subAcnt = subAcntMgr->FindAccount(acntNo);
		if (subAcnt)
		{
			AddAccount(subAcnt);
			subAcnt->Fund(_Fund);
			if (subAcnt->ParentAccount())
			{
				subAcnt->ParentAccount()->AddToFundMap(_Fund->Name, subAcnt);
			}
		}
	}
}
