#include "stdafx.h"
#include "VtFundOrderManager.h"
#include "VtFund.h"
#include "VtAccount.h"
#include "VtAccountManager.h"
#include "VtOrderManager.h"
#include "VtOrderManagerSelector.h"

VtFundOrderManager::VtFundOrderManager()
{
	_CurFund = nullptr;
}


VtFundOrderManager::~VtFundOrderManager()
{
	int i = 0;
}

void VtFundOrderManager::CurFund(VtFund* val)
{
	_CurFund = val;
	MakeOrderManagers();
}

std::vector<VtOrder*> VtFundOrderManager::GetAcceptedOrders(std::string symbolCode)
{
	std::vector<VtOrder*> acptList;

	// 각각의 펀드 계좌에 대한 주문 관리자가 만들어져 있다고 가정한다.
	// 각각의 펀드 계좌 주문 관리자는 주문이 나가기전에 미리 만들어져야 한다.
	for (auto it = _OrderMgrMap.begin(); it != _OrderMgrMap.end(); ++it) {
		VtOrderManager* orderMgr = it->second;
		orderMgr->GetAcceptedOrders(symbolCode, acptList);
	}

	return acptList;
}

std::vector<VtAccount*> VtFundOrderManager::GetParentAccountList()
{
	std::vector<VtAccount*> acntList;

	if (!_CurFund)
		return acntList;

	std::vector<VtAccount*>& fundAcntList = _CurFund->GetFundAccountVector();
	for (auto it = fundAcntList.begin(); it != fundAcntList.end(); ++it)
	{
		VtAccount* fundAcnt = *it;
		if (fundAcnt->ParentAccount())
		{
			acntList.push_back(fundAcnt->ParentAccount());
		}
	}

	return acntList;
}

void VtFundOrderManager::UpdateOrderManagers()
{
	if (!_CurFund)
		return;

	_OrderMgrMap.clear();
	MakeOrderManagers();
}

std::map<int, VtOrder*> VtFundOrderManager::GetTotalRemain(std::string symbolCode)
{
	std::map<int, VtOrder*> remainMap;
	// 각각의 펀드 계좌에 대한 주문 관리자가 만들어져 있다고 가정한다.
	// 각각의 펀드 계좌 주문 관리자는 주문이 나가기전에 미리 만들어져야 한다.
	for (auto it = _OrderMgrMap.begin(); it != _OrderMgrMap.end(); ++it) {
		VtOrderManager* orderMgr = it->second;
		orderMgr->GetRemainMap(symbolCode, remainMap);
	}

	return remainMap;
}

void VtFundOrderManager::ClearRemainOrderMap(std::string symbolCode)
{
	for (auto it = _OrderMgrMap.begin(); it != _OrderMgrMap.end(); ++it) {
		VtOrderManager* orderMgr = it->second;
		orderMgr->ClearRemainOrderMap(symbolCode);
	}
}

void VtFundOrderManager::MakeOrderManagers()
{
	if (!_CurFund)
		return;

	VtOrderManagerSelector* orderMgrSelecter = VtOrderManagerSelector::GetInstance();
	std::vector<VtAccount*>& acntVector = _CurFund->GetFundAccountVector();
	for (auto it = acntVector.begin(); it != acntVector.end(); ++it)
	{
		VtAccount* acnt = *it;
		VtOrderManager* orderMgr = orderMgrSelecter->FindAddOrderManager(acnt->AccountNo);
		_OrderMgrMap[acnt->AccountNo] = orderMgr;
	}
}
