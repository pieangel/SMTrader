#include "stdafx.h"
#include "VtFundOrderManagerSelecter.h"
#include "VtFundOrderManager.h"
#include "VtFund.h"
#include "VtFundManager.h"

VtFundOrderManagerSelecter::VtFundOrderManagerSelecter()
{
	ActiveOrderManager(nullptr);
}


VtFundOrderManagerSelecter::~VtFundOrderManagerSelecter()
{
	for (auto it = _OrderManagerMap.begin(); it != _OrderManagerMap.end(); ++it) {
		delete it->second;
	}
}

VtFundOrderManager* VtFundOrderManagerSelecter::FindAddOrderManager(std::string fundName)
{
	VtFundManager* fundMgr = VtFundManager::GetInstance();
	auto it = _OrderManagerMap.find(fundName);
	if (it == _OrderManagerMap.end()) {
		VtFundOrderManager* orderMgr = new VtFundOrderManager();
		orderMgr->CurFund(fundMgr->FindFund(fundName));
		_OrderManagerMap[fundName] = orderMgr;
		orderMgr->FundName = fundName;
		return orderMgr;
	} else {
		return it->second;
	}
}

void VtFundOrderManagerSelecter::AddOrderManager(VtFund* fund)
{
	VtFundOrderManager* orderMgr = new VtFundOrderManager();
	orderMgr->CurFund(fund);
	orderMgr->FundName = fund->Name;
	_OrderManagerMap[fund->Name] = orderMgr;
}

void VtFundOrderManagerSelecter::SetActiveOrderManager(VtFund* fund)
{
	if (!fund) return;

	_ActiveOrderManager = FindOrderManager(fund->Name);
}

VtFundOrderManager* VtFundOrderManagerSelecter::FindOrderManager(std::string fundName)
{
	return FindAddOrderManager(fundName);
}

