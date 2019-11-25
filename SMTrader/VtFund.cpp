#include "stdafx.h"
#include "VtFund.h"
#include "VtAccount.h"
#include "VtFundAccount.h"
#include <msgpack.hpp>
#include <algorithm>
#include "VtOrderManagerSelector.h"
#include "VtOrderManager.h"
#include "VtProductOrderManager.h"
#include "VtSubAccountManager.h"
VtFund::VtFund()
{
	_FundMap.Fund(this);
}


VtFund::~VtFund()
{
}

VtAccount* VtFund::CreateFundAccount(VtAccount* acnt)
{
	VtAccount* fundAcnt = new VtAccount();
	fundAcnt->AccountNo = acnt->AccountNo;
	fundAcnt->AccountName = acnt->AccountName;

	return fundAcnt;
}

VtAccount* VtFund::AddFundAccount(VtAccount* fundAcnt)
{
	if (!fundAcnt)
		return nullptr;

	_FundMap.AddAccount(fundAcnt);

	return fundAcnt;
}

void VtFund::RemoveFundAccount(VtAccount* fundAcnt)
{
	if (!fundAcnt)
		return;

	VtAccount* found = _FundMap.FindAccount(fundAcnt->AccountNo);
	if (!found)
		return;

	_FundMap.RemoveAccount(found->AccountNo);

	delete found;
}

void VtFund::ClearAccounts()
{
	_FundMap.ClearAllFundAccounts();
}

void VtFund::Save(std::stringstream& ss)
{
	msgpack::pack(ss, Name);
	_FundMap.Save(ss);
}

void VtFund::Save(msgpack::sbuffer& ss)
{
	msgpack::pack(ss, Name);
	_FundMap.Save(ss);
}

void VtFund::Save(simple::file_ostream<same_endian_type>& ss)
{
	ss << Name;
	_FundMap.Save(ss);
}

void VtFund::Load(simple::file_istream<same_endian_type>& ss)
{
	ss >> Name;
	_FundMap.Fund(this);
	_FundMap.Load(ss);
}

void VtFund::GetFundAccountList(std::set<VtAccount*>& acntSet)
{
	std::vector<VtAccount*>& acntVector = _FundMap.GetAccountVector();
	std::copy(acntVector.begin(), acntVector.end(), inserter(acntSet, acntSet.begin()));
}

std::set<VtAccount*> VtFund::GetParentAccountSet()
{
	std::set<VtAccount*> parentAcntSet;
	std::vector<VtAccount*>& subAcntVector = _FundMap.GetAccountVector();
	for (auto it = subAcntVector.begin(); it != subAcntVector.end(); ++it)
	{
		VtAccount* subAcnt = *it;
		VtAccount* parentAcnt = subAcnt->ParentAccount();
		parentAcntSet.insert(parentAcnt);
	}

	return parentAcntSet;
}

std::pair<bool, int> VtFund::GetRemainCount(std::string symCode)
{
	int remainCnt = 0;
	int count = 0;

	std::vector<VtAccount*>& subAcntVector = _FundMap.GetAccountVector();
	for (auto it = subAcntVector.begin(); it != subAcntVector.end(); ++it) {
		VtAccount* subAcnt = *it;
		VtPosition* subAcntPosi = subAcnt->FindPosition(symCode);
		if (subAcntPosi) {
			remainCnt += subAcntPosi->OpenQty;
			count++;
		}
	}

	if (count == 0) {
		return std::make_pair(false, 0);
	} else {
		return std::make_pair(true, remainCnt);
	}
}

VtPosition VtFund::GetPosition(std::string symCode, int& count)
{
	VtPosition posi;
	posi.ShortCode = symCode;
	posi.FullCode = symCode;
	posi.AvgPrice = 0.0;
	posi.OpenQty = 0;
	posi.OpenProfitLoss = 0.0;
	count = 0;

	std::vector<VtAccount*>& subAcntVector = _FundMap.GetAccountVector();
	for (auto it = subAcntVector.begin(); it != subAcntVector.end(); ++it)
	{
		VtAccount* subAcnt = *it;
		VtPosition* subAcntPosi = subAcnt->FindPosition(symCode);
		if (subAcntPosi)
		{
			if (subAcntPosi->Position != VtPositionType::None)
				posi.Position = subAcntPosi->Position;
			posi.OpenQty += subAcntPosi->OpenQty;
			posi.AvgPrice += subAcntPosi->AvgPrice * subAcntPosi->OpenQty;
			posi.CurPrice = subAcntPosi->CurPrice;
			posi.OpenProfitLoss += subAcntPosi->OpenProfitLoss;
			count++;
		}
	}

	if (posi.OpenQty != 0)
		posi.AvgPrice = std::abs(posi.AvgPrice) / std::abs(posi.OpenQty);

	return posi;
}

std::vector<VtPosition*> VtFund::GetPositionList(std::string symCode)
{
	std::vector<VtPosition*> posiMap;
	if (symCode.length() == 0)
		return posiMap;
	std::vector<VtAccount*>& subAcntVector = _FundMap.GetAccountVector();
	for (auto it = subAcntVector.begin(); it != subAcntVector.end(); ++it) {
		VtAccount* subAcnt = *it;
		VtPosition* posi = subAcnt->FindPosition(symCode);
		posi->SubAccountNo = subAcnt->AccountNo;
		// 포지션에서 잔고가 있는 것들만 가져온다.
		if (posi && std::abs(posi->OpenQty) > 0)
			posiMap.push_back(posi);
	}

	return posiMap;
}

void VtFund::ResetPosition(std::string symCode)
{
	std::vector<VtAccount*>& subAcntVector = _FundMap.GetAccountVector();
	for (auto it = subAcntVector.begin(); it != subAcntVector.end(); ++it) {
		VtAccount* subAcnt = *it;
		VtPosition* posi = subAcnt->FindPosition(symCode);
		if (!posi)
			continue;
		posi->SubAccountNo = subAcnt->AccountNo;
		// 포지션에서 잔고가 있는 것들만 가져온다.
		if (posi && std::abs(posi->OpenQty) > 0) {
			posi->Position = VtPositionType::None;
			posi->OpenQty = 0;
			posi->OpenProfitLoss = 0.0;
			posi->AvgPrice = 0.0;
		}
			
	}
}

VtPosition* VtFund::FindPosition(std::string subAcntNo, std::string symCode)
{
	VtAccount* subAcnt = _FundMap.FindAccount(subAcntNo);
	if (!subAcnt)
		return nullptr;
	return subAcnt->FindPosition(symCode);
}

/*
std::vector<VtPosition> VtFund::GetPositionBySymbol(std::string symCode)
{
	std::vector<VtPosition> posiVec;

	return posiVec;
}
*/
std::vector<VtPosition*> VtFund::GetPositionListBySubAccount()
{
	std::vector<VtPosition*> posiVec;
	std::vector<VtAccount*>& subAcntVector = _FundMap.GetAccountVector();
	for (auto it = subAcntVector.begin(); it != subAcntVector.end(); ++it)
	{
		VtAccount* subAcnt = *it;
		for (auto itn = subAcnt->PositionMap.begin(); itn != subAcnt->PositionMap.end(); ++itn)
		{
			VtPosition* posi = itn->second;
			posiVec.push_back(posi);
		}
	}

	return posiVec;
}


std::map<std::string, std::shared_ptr<VtPosition>> VtFund::GetPositionListByProduct()
{
	std::map<std::string, std::shared_ptr<VtPosition>> posiMap;
	std::vector<VtAccount*>& subAcntVector = _FundMap.GetAccountVector();
	for (auto it = subAcntVector.begin(); it != subAcntVector.end(); ++it)
	{
		VtAccount* subAcnt = *it;
		for (auto itn = subAcnt->PositionMap.begin(); itn != subAcnt->PositionMap.end(); ++itn)
		{
			VtPosition* posi = itn->second;
			std::shared_ptr<VtPosition> posiPtr;
			auto itp = posiMap.find(posi->ShortCode);
			if (itp != posiMap.end())
			{
				posiPtr = itp->second;
				posiPtr->OpenQty += posi->OpenQty;
				posiPtr->AvgPrice += posi->AvgPrice * posi->OpenQty;
				posiPtr->OpenProfitLoss += posi->OpenProfitLoss;
			}
			else
			{
				//posiPtr.reset(new VtPosition());
				posiPtr = std::make_shared<VtPosition>();
				posiMap[posi->ShortCode] = posiPtr;
				posiPtr->ShortCode = posi->ShortCode;
				posiPtr->Position = posi->Position;
				posiPtr->AvgPrice = posi->AvgPrice * posi->OpenQty;
				posiPtr->OpenProfitLoss = posi->OpenProfitLoss;
				posiPtr->AccountNo = posi->AccountNo;
				posiPtr->OpenQty = posi->OpenQty;
				posiPtr->Fee = posi->Fee;
				posiPtr->CurPrice = posi->CurPrice;
				posiPtr->intCurPrice = posi->intCurPrice;
			}
		}
	}

	for (auto it = posiMap.begin(); it != posiMap.end(); ++it)
	{
		std::shared_ptr<VtPosition> posi = it->second;
		posi->AvgPrice = posi->OpenQty == 0 ? 0 : posi->AvgPrice / std::abs(posi->OpenQty);
	}

	return posiMap;
}

void VtFund::SumOpenPL()
{
	TradePL = 0;
	OpenPL = 0;
	TotalFee = 0;
	std::vector<VtAccount*>& subAcntVector = _FundMap.GetAccountVector();
	for (auto it = subAcntVector.begin(); it != subAcntVector.end(); ++it)
	{
		VtAccount* subAcnt = *it;
		OpenPL += subAcnt->OpenPL;
		TradePL += subAcnt->TradePL;
		TotalFee += subAcnt->Fee;
	}
	TotalPL = OpenPL + TradePL + TotalFee;
}

void VtFund::AddSymbol(std::string symCode)
{
	_SymbolList[symCode] = symCode;
}

std::tuple<bool, int> VtFund::GetInitAndAcceptedCount()
{
	bool init = false;
	int acptCnt = 0;
	VtOrderManagerSelector* orderMgrSelecter = VtOrderManagerSelector::GetInstance();
	std::vector<VtAccount*>& subAcntVector = _FundMap.GetAccountVector();
	for (auto it = subAcntVector.begin(); it != subAcntVector.end(); ++it)
	{
		VtAccount* subAcnt = *it;
		VtOrderManager* orderMgr = orderMgrSelecter->FindOrderManager(subAcnt->AccountNo);
		if (!orderMgr)
			continue;
		acptCnt += orderMgr->GetAcceptedCount();
		if (orderMgr->GetInit())
			init = true;
	}

	return std::make_tuple(init, acptCnt);
}

std::tuple<bool, int> VtFund::GetInitAndAcceptedCount(std::string symCode)
{
	bool init = false;
	int acptCnt = 0;
	VtOrderManagerSelector* orderMgrSelecter = VtOrderManagerSelector::GetInstance();
	std::vector<VtAccount*>& subAcntVector = _FundMap.GetAccountVector();
	for (auto it = subAcntVector.begin(); it != subAcntVector.end(); ++it)
	{
		VtAccount* subAcnt = *it;
		VtOrderManager* orderMgr = orderMgrSelecter->FindOrderManager(subAcnt->AccountNo);
		if (!orderMgr)
			continue;
		VtProductOrderManager* prdtOrderMgr = orderMgr->GetProductOrderManager(symCode);
		if (prdtOrderMgr)
		{
			acptCnt += prdtOrderMgr->GetAcceptedOrderCount();
			if (prdtOrderMgr->Init())
				init = true;
		}
	}

	return std::make_tuple(init, acptCnt);
}

VtAccount* VtFund::GetParentAccount(std::string subAcntNo)
{
	VtAccount* subAcnt = _FundMap.FindAccount(subAcntNo);
	if (subAcnt)
		return subAcnt->ParentAccount();
	else
		return nullptr;
}

VtAccount* VtFund::FindAccount(std::string subAcntNo)
{
	return _FundMap.FindAccount(subAcntNo);
}

int VtFund::GetTotalSeungSu()
{
	int total = 0;
	std::vector<VtAccount*>& subAcntVector = _FundMap.GetAccountVector();
	for (auto it = subAcntVector.begin(); it != subAcntVector.end(); ++it) {
		total += (*it)->SeungSu;
	}
	return total;
}

void VtFund::SaveToXml(pugi::xml_node& node_fund)
{
	pugi::xml_node fund_child = node_fund.append_child("fund_name");
	fund_child.append_child(pugi::node_pcdata).set_value(Name.c_str());
	fund_child = node_fund.append_child("fund_account_list");
	if (_FundMap.GetAccountVector().size() > 0) {
		for (auto it = _FundMap.GetAccountVector().begin(); it != _FundMap.GetAccountVector().end(); ++it) {
			VtAccount* fund_account = *it;
			pugi::xml_node node_fund_account = fund_child.append_child("fund_account");
			node_fund_account.append_child(pugi::node_pcdata).set_value(fund_account->AccountNo.c_str());
		}
	}
}

/*
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
*/

void VtFund::LoadFromXml(pugi::xml_node& node_fund)
{
	Name = node_fund.child_value("fund_name");
	pugi::xml_node fund_account_list_node = node_fund.child("fund_account_list");
	if (fund_account_list_node) {
		for (pugi::xml_node fund_account_node = fund_account_list_node.child("fund_account"); fund_account_node; fund_account_node = fund_account_node.next_sibling("fund_account")) {
			std::string fund_account_no = fund_account_node.child_value();
			VtAccount* fund_account = VtSubAccountManager::GetInstance()->FindAccount(fund_account_no);
			if (fund_account) {
				_FundMap.AddAccount(fund_account);
				if (fund_account->ParentAccount()) {
					fund_account->ParentAccount()->AddToFundMap(Name, fund_account);
				}
			}
		}
	}
}
