#include "stdafx.h"
#include "VtAccount.h"
#include "VtPosition.h"
#include "VtProductOrderManagerSelector.h"
#include "VtProductOrderManager.h"
#include "VtOrder.h"
#include "VtSymbolManager.h"
#include "VtSymbol.h"
#include "HdTaskArg.h"
#include "HdScheduler.h"
#include "VtOrderManager.h"
#include "VtOrderManagerSelector.h"
#include "VtRealtimeRegisterManager.h"
#include "VtSubAccountManager.h"
#include "VtFund.h"
#include "VtFundManager.h"
#include "VtTotalOrderManager.h"

VtPosition* VtAccount::FindAdd(std::string symbolCode)
{
	if (symbolCode.length() == 0)
		return nullptr;

	VtRealtimeRegisterManager* realtimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	realtimeRegiMgr->RegisterProduct(symbolCode);

	auto it = PositionMap.find(symbolCode);
	if (it != PositionMap.end())
	{
		return it->second;
	}
	else
	{
		VtPosition* position = new VtPosition();
		position->AccountNo = AccountNo;
		position->FullCode = symbolCode;
		position->ShortCode = symbolCode;
		PositionMap[symbolCode] = position;
		return position;
	}
}

VtPosition* VtAccount::FindAdd(VtOrder* order)
{
	if (!order)
		return nullptr;
	auto it = PositionMap.find(order->fullCode);
	if (it != PositionMap.end())
	{
		return it->second;
	}
	else
	{
		VtPosition* position = new VtPosition();
		position->AccountNo = AccountNo;
		position->FullCode = order->fullCode;
		position->Position = VtPositionType::None;
		PositionMap[order->fullCode] = position;
		return position;
	}
}

void VtAccount::RemovePosition(std::string symbolCode)
{
	if (symbolCode.length() == 0)
		return;

	auto it = PositionMap.find(symbolCode);
	if (it != PositionMap.end())
	{
		VtPosition* posi = it->second;
		posi->OpenQty = 0;
	}
}

void VtAccount::ResetPosition(std::string symCode)
{
	if (symCode.length() == 0)
		return;

	auto it = PositionMap.find(symCode);
	if (it != PositionMap.end())
	{
		VtPosition* posi = it->second;
		posi->OpenQty = 0;
		posi->AvgPrice = 0.0;
		posi->Position = VtPositionType::None;
		posi->OpenProfitLoss = 0.0;
	}
}

void VtAccount::Save(simple::file_ostream<same_endian_type>& ss)
{
	ss << ParentAccountNo << AccountName << AccountNo << Password << SeungSu << Ratio << _AccountLevel << _Prime;
	if (_AccountLevel == 0)
	{
		ss << (int)_SubAccountList.size();
		for (auto it = _SubAccountList.begin(); it != _SubAccountList.end(); ++it)
		{
			VtAccount* subAcnt = *it;
			subAcnt->Save(ss);
		}
	}
}

void VtAccount::Load(simple::file_istream<same_endian_type>& ss)
{
	VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
	ss >> ParentAccountNo >> AccountName >> AccountNo >> Password >> SeungSu >> Ratio >> _AccountLevel >> _Prime;
	if (_AccountLevel == 0)
	{
		int count = 0;
		ss >> count;
		if (count > 0)
		{
			for (int i = 0; i < count; ++i)
			{
				VtAccount* acnt = new VtAccount();
				acnt->Load(ss);
				if (acnt->ParentAccountNo.compare(AccountNo) == 0)
				{
					acnt->ParentAccount(this);
				}
				_SubAccountList.push_back(acnt);
				subAcntMgr->AddAccount(acnt);
			}
		}
		else
		{
			std::string acntNo = AccountNo;
			acntNo.append(_T("_1"));
			VtAccount* subAcnt = CreateSubAccount(acntNo, AccountName, true);
			subAcnt->ParentAccount(this);
			subAcntMgr->AddAccount(subAcnt);
		}
	}
}

VtPosition* VtAccount::FindPosition(std::string symbolCode)
{
	if (symbolCode.length() == 0)
		return nullptr;

	auto it = PositionMap.find(symbolCode);
	if (it != PositionMap.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

void VtAccount::SumOpenPL()
{
	TradePL = 0;
	OpenPL = 0;
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	for (auto it = PositionMap.begin(); it != PositionMap.end(); ++it)
	{
		VtPosition* posi = it->second;
		OpenPL += posi->OpenProfitLoss;
		TradePL += posi->TradePL;
	}

	TotalPL = OpenPL + TradePL + Fee;
}

void VtAccount::CalcOpenPL(VtSymbol* sym)
{
	if (sym)
	{
		double curClose = sym->Quote.intClose / std::pow(10, sym->IntDecimal);
		VtPosition* posi = FindPosition(sym->ShortCode);
		if (posi)
			posi->OpenProfitLoss = posi->OpenQty * (curClose - posi->AvgPrice)*sym->seungsu;
	}
}

void VtAccount::SumOpenPL(VtSymbol* sym, VtPosition* posi)
{
	if (sym && posi)
	{
		double curClose = sym->Quote.intClose / std::pow(10, sym->IntDecimal);
		posi->OpenProfitLoss = posi->OpenQty * (curClose - posi->AvgPrice)*sym->seungsu;
	}
}

VtAccount::VtAccount()
{
	TempFee = 0;
	PurePL = 0;
	TempTradePL = 0;
	TempOpenPL = 0;
	TempPurePL = 0;
	_Prime = false;
	SeungSu = 1;
	Ratio = 0.0;
	_Fund = nullptr;
}

VtAccount::~VtAccount()
{
	for (auto it = PositionMap.begin(); it != PositionMap.end(); ++it)
	{
		delete it->second;
	}

	PositionMap.clear();

	for (auto it = _SubAccountList.begin(); it != _SubAccountList.end(); ++it)
	{
		delete *it;
	}

	_SubAccountList.clear();

	for (auto it = _FundMap.begin(); it != _FundMap.end(); ++it)
	{
		delete it->second;
	}
	_FundMap.clear();
}

VtPosition* VtAccount::CreatePosition(VtOrder* order)
{
	if (!order)
		return nullptr;
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindHdSymbol(order->shortCode);
	if (!sym)
		return nullptr;

	VtPosition* position = new VtPosition();
	position->AccountNo = order->AccountNo;
	position->ShortCode = order->shortCode;
	position->Position = order->orderPosition;
	position->OpenQty = order->filledQty;
	position->AvgPrice = order->intFilledPrice / std::pow(10, sym->IntDecimal);
	position->CurPrice = order->intFilledPrice / std::pow(10, sym->IntDecimal);
	position->SubAccountNo = order->SubAccountNo;
	position->FundName = order->FundName;
	position->orderType = order->type;
	PositionMap[order->shortCode] = position;

	VtTotalOrderManager* totalOrderMgr = VtTotalOrderManager::GetInstance();
	if (order->type == 0) {
		totalOrderMgr->AddPosition(order->type, position->AccountNo, position->ShortCode, position);
	}
	else if (order->type == 1) {
		totalOrderMgr->AddPosition(order->type, position->SubAccountNo, position->ShortCode, position);
	}
	else if (order->type == 2) {
		totalOrderMgr->AddPosition(order->type, position->SubAccountNo, position->ShortCode, position);
	}

	if (order->type == 1) {
		VtFundManager* fundMgr = VtFundManager::GetInstance();
		VtFund* fund = fundMgr->FindFund(order->FundName);
		if (fund) {
			fund->AddSymbol(order->shortCode);
		}
	}

	return position;
}

VtPosition* VtAccount::CopyPosition(VtPosition& curPosi)
{
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindHdSymbol(curPosi.ShortCode);
	if (!sym)
		return nullptr;

	VtPosition* position = new VtPosition();
	position->AccountNo = curPosi.AccountNo;
	position->ShortCode = curPosi.ShortCode;
	position->Position = curPosi.Position;
	position->OpenQty = curPosi.OpenQty;
	position->AvgPrice = curPosi.AvgPrice;
	position->CurPrice = curPosi.CurPrice;
	PositionMap[curPosi.ShortCode] = position;
	return position;
}

void VtAccount::GetFilledHistoryTable()
{
	HdScheduler* scheduler = HdScheduler::GetInstance();
	HdTaskArg arg;
	arg.Type = HdTaskType::HdFilledHistoryTable;
	arg.AddArg(_T("AccountNo"), AccountNo);
	arg.AddArg(_T("Password"), Password);
	scheduler->AddTask(std::move(arg));
}

void VtAccount::GetOutstandingHistory()
{
	HdScheduler* scheduler = HdScheduler::GetInstance();
	HdTaskArg arg;
	arg.Type = HdTaskType::HdOutstandingHistory;
	arg.AddArg(_T("AccountNo"), AccountNo);
	arg.AddArg(_T("Password"), Password);
	scheduler->AddTask(std::move(arg));
}

void VtAccount::GetOutstanding()
{
	HdScheduler* scheduler = HdScheduler::GetInstance();
	HdTaskArg arg;
	arg.Type = HdTaskType::HdOutstanding;
	arg.AddArg(_T("AccountNo"), AccountNo);
	arg.AddArg(_T("Password"), Password);
	scheduler->AddTask(std::move(arg));
}

void VtAccount::GetOutstanding(std::vector<std::pair<std::string, HdTaskArg>>& argList)
{
	HdScheduler* scheduler = HdScheduler::GetInstance();
	HdTaskArg arg;
	arg.Type = HdTaskType::HdOutstanding;
	arg.AddArg(_T("AccountNo"), AccountNo);
	arg.AddArg(_T("Password"), Password);
	//scheduler->AddTask(std::move(arg));
	argList.push_back(std::make_pair(AccountName, arg));
}

void VtAccount::GetAcceptedHistory()
{
	Sleep(400);
	HdScheduler* scheduler = HdScheduler::GetInstance();
	HdTaskArg arg;
	arg.Type = HdTaskType::HdAcceptedHistory;
	arg.AddArg(_T("AccountNo"), AccountNo);
	arg.AddArg(_T("Password"), Password);
	scheduler->AddTask(std::move(arg));
}

void VtAccount::GetAcceptedHistory(std::vector<std::pair<std::string, HdTaskArg>>& argList)
{
	Sleep(400);
	HdScheduler* scheduler = HdScheduler::GetInstance();
	HdTaskArg arg;
	arg.Type = HdTaskType::HdAcceptedHistory;
	arg.AddArg(_T("AccountNo"), AccountNo);
	arg.AddArg(_T("Password"), Password);
	//scheduler->AddTask(std::move(arg));
	argList.push_back(std::make_pair(AccountName, arg));
}

void VtAccount::GetAsset()
{
	HdScheduler* scheduler = HdScheduler::GetInstance();
	HdTaskArg arg;
	arg.Type = HdTaskType::HdAsset;
	arg.AddArg(_T("AccountNo"), AccountNo);
	arg.AddArg(_T("Password"), Password);
	scheduler->AddTask(std::move(arg));
}

void VtAccount::GetDeposit()
{
	HdScheduler* scheduler = HdScheduler::GetInstance();
	HdTaskArg arg;
	arg.Type = HdTaskType::HdDeposit;
	arg.AddArg(_T("AccountNo"), AccountNo);
	arg.AddArg(_T("Password"), Password);
	scheduler->AddTask(std::move(arg));
}

void VtAccount::GetDeposit(std::vector<std::pair<std::string, HdTaskArg>>& argList)
{
	HdScheduler* scheduler = HdScheduler::GetInstance();
	HdTaskArg arg;
	arg.Type = HdTaskType::HdDeposit;
	arg.AddArg(_T("AccountNo"), AccountNo);
	arg.AddArg(_T("Password"), Password);
	//scheduler->AddTask(std::move(arg));
	argList.push_back(std::make_pair(AccountName, arg));
}

void VtAccount::GetDailyProfitLoss()
{
	HdScheduler* scheduler = HdScheduler::GetInstance();
	HdTaskArg arg;
	arg.Type = HdTaskType::HdDailyProfitLoss;
	arg.AddArg(_T("AccountNo"), AccountNo);
	arg.AddArg(_T("Password"), Password);
	scheduler->Available(true);
	scheduler->AddTask(std::move(arg));
}

void VtAccount::GetAccountProfitLoss()
{
	HdScheduler* scheduler = HdScheduler::GetInstance();
	HdTaskArg arg;
	arg.Type = HdTaskType::HdAccountProfitLoss;
	arg.AddArg(_T("AccountNo"), AccountNo);
	arg.AddArg(_T("Password"), Password);
	scheduler->Available(true);
	scheduler->AddTask(std::move(arg));
}

void VtAccount::GetApiCustomerProfitLoss()
{
	HdScheduler* scheduler = HdScheduler::GetInstance();
	HdTaskArg arg;
	arg.Type = HdTaskType::HdApiCustomerProfitLoss;
	arg.AddArg(_T("AccountNo"), AccountNo);
	arg.AddArg(_T("Password"), Password);
	scheduler->Available(true);
	scheduler->AddTask(std::move(arg));
}



void VtAccount::GetApiCustomerProfitLoss(std::vector<std::pair<std::string, HdTaskArg>>& argList)
{
	HdScheduler* scheduler = HdScheduler::GetInstance();
	HdTaskArg arg;
	arg.Type = HdTaskType::HdApiCustomerProfitLoss;
	arg.AddArg(_T("AccountNo"), AccountNo);
	arg.AddArg(_T("Password"), Password);
	scheduler->Available(true);
	//scheduler->AddTask(std::move(arg));
	argList.push_back(std::make_pair(AccountName, arg));
}

bool VtAccount::CheckCutLoss(VtSymbol* sym, int size)
{
	if (!sym)
		return false;
	VtPosition* posi = FindPosition(sym->ShortCode);
	if (!posi)
		return false;

	double target = sym->intTickSize * size * sym->seungsu;
	//target = target / std::pow(10, sym->IntDecimal);

	target = -1 * target;
	if (posi->OpenProfitLoss * std::pow(10, sym->IntDecimal) <= target)
		return true;
	else
		return false;
}

bool VtAccount::CheckCutLoss(VtPosition* posi, VtSymbol* sym, int size)
{
	if (!sym)
		return false;
	if (!posi)
		return false;

	double target = sym->intTickSize * size * sym->seungsu;
	//target = target / std::pow(10, sym->IntDecimal);

	target = -1 * target;
	if (posi->OpenProfitLoss * std::pow(10, sym->IntDecimal) <= target)
		return true;
	else
		return false;
}

bool VtAccount::CheckCutProfit(VtSymbol* sym, int size)
{
	if (!sym)
		return false;

	VtPosition* posi = FindPosition(sym->ShortCode);
	if (!posi)
		return false;

	double target = sym->intTickSize * size * sym->seungsu;
	//target = target / std::pow(10, sym->IntDecimal);

	if (posi->OpenProfitLoss * std::pow(10, sym->IntDecimal) >= target)
		return true;
	else
		return false;
}

bool VtAccount::CheckCutProfit(VtPosition* posi, VtSymbol* sym, int size)
{
	if (!sym)
		return false;

	if (!posi)
		return false;

	double target = sym->intTickSize * size * sym->seungsu;
	//target = target / std::pow(10, sym->IntDecimal);

	if (posi->OpenProfitLoss * std::pow(10, sym->IntDecimal) >= target)
		return true;
	else
		return false;
}

bool VtAccount::CheckCutLossProfit(VtPosition* posi, VtSymbol* sym, int profitSize, int lossSize)
{
	if (!sym)
		return false;

	if (!posi)
		return false;

	return false;
}

void VtAccount::GetAccountInfoNFee(int taskType)
{
	HdScheduler* scheduler = HdScheduler::GetInstance();
	HdTaskArg arg;
	if (taskType == 1)
		arg.Type = HdTaskType::HdAccountFeeInfoStep1;
	else
		arg.Type = HdTaskType::HdAccountFeeInfoStep2;

	arg.AddArg(_T("AccountNo"), AccountNo);
	arg.AddArg(_T("Password"), Password);
	scheduler->Available(true);
	scheduler->AddTask(std::move(arg));
}

int VtAccount::GetAcceptedOrderCount(std::string symCode)
{
	VtOrderManagerSelector* orderMgrSelector = VtOrderManagerSelector::GetInstance();
	VtOrderManager* orderMgr = orderMgrSelector->FindOrderManager(AccountNo);
	if (!orderMgr)
		return -1;
	VtProductOrderManager* prdtOrderMgr = orderMgr->GetProductOrderManager(symCode);
	if (!prdtOrderMgr)
		return -1;
	if (prdtOrderMgr->Init())
		return prdtOrderMgr->GetAcceptedOrderCount();
	else
		return -1;
}

std::pair<bool, int> VtAccount::GetRemainCount(std::string symCode)
{
	VtPosition* posi = FindPosition(symCode);
	if (!posi)
		return std::make_pair(false, 0);
	else
		return std::make_pair(true, posi->OpenQty);
}

VtAccount* VtAccount::CreateSubAccount(std::string acntNo, std::string acntName, bool prime)
{
	VtAccount* acnt = new VtAccount();
	acnt->ParentAccountNo = this->AccountNo;
	acnt->AccountNo = acntNo;
	acnt->AccountName = acntName;
	acnt->_AccountLevel = 1;
	acnt->Prime(prime);
	_SubAccountList.push_back(acnt);

	return acnt;
}

void VtAccount::RemoveSubAccount(std::string acntNo)
{
	for (auto it = _SubAccountList.begin(); it != _SubAccountList.end(); ++it)
	{
		VtAccount* acnt = *it;
		if (acnt->AccountNo.compare(acntNo) == 0)
		{
			delete acnt;
			_SubAccountList.erase(it);
			break;
		}
	}
}

VtAccount* VtAccount::FindSubAccount(std::string acntNo)
{
	for (auto it = _SubAccountList.begin(); it != _SubAccountList.end(); ++it)
	{
		VtAccount* acnt = *it;
		if (acnt->AccountNo.compare(acntNo) == 0)
		{
			return acnt;
		}
	}

	return nullptr;
}

VtAccount* VtAccount::FindSubAccount(std::string acntNo, std::string myself)
{
	for (auto it = _SubAccountList.begin(); it != _SubAccountList.end(); ++it) {
		VtAccount* acnt = *it;
		if (acnt->AccountNo.compare(myself) != 0 &&
			acnt->AccountNo.compare(acntNo) == 0) {
			return acnt;
		}
	}

	return nullptr;
}

void VtAccount::AddToFundMap(std::string fundName, VtAccount* subAcnt)
{
	if (fundName.length() == 0 || !subAcnt)
		return;
	FundAccountMap* acntMap = FindAccountMap(fundName);
	if (acntMap)
	{
		acntMap->insert(std::make_pair(subAcnt->AccountNo, subAcnt));
	}
	else
	{
		acntMap = new FundAccountMap();
		acntMap->insert(std::make_pair(subAcnt->AccountNo, subAcnt));
		_FundMap[fundName] = acntMap;
	}
}

void VtAccount::RemoveFromFundMap(std::string fundName, VtAccount* subAcnt)
{
	if (fundName.length() == 0 || !subAcnt)
		return;

	FundAccountMap* acntMap = FindAccountMap(fundName);
	if (acntMap)
	{
		auto it = acntMap->find(subAcnt->AccountNo);
		if (it != acntMap->end())
		{
			acntMap->erase(it);
			if (acntMap->size() == 0)
			{
				auto itf = _FundMap.find(fundName);
				if (itf != _FundMap.end())
				{
					delete itf->second;
					_FundMap.erase(itf);
				}
			}
		}
	}
}

std::vector<std::string> VtAccount::GetFundNameList()
{
	std::vector<std::string> fundNameList;
	for (auto it = _FundMap.begin(); it != _FundMap.end(); ++it)
	{
		fundNameList.push_back(it->first);
	}

	return fundNameList;
}

VtAccount::FundAccountMap* VtAccount::FindAccountMap(std::string fundName)
{
	auto it = _FundMap.find(fundName);
	if (it != _FundMap.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}


