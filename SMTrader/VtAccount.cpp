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
#include "VtHdClient.h"
#include "cryptor.hpp"
#include "VtAccountManager.h"
#include "VtGlobal.h"

bool VtAccount::hasValidPassword()
{
	std::string pwd = Password;
	if (!pwd.empty()) {
		return true;
	}
	else {
		return false;
	}
}

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
	auto enc_ParentAccountNo = cryptor::encrypt(ParentAccountNo);
	auto enc_AccountName = cryptor::encrypt(AccountName);
	auto enc_AccountNo = cryptor::encrypt(AccountNo);
	auto enc_Password = cryptor::encrypt(Password);

	ss << enc_ParentAccountNo << enc_AccountName << enc_AccountNo << enc_Password << SeungSu << Ratio << _AccountLevel << _Prime;
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
	std::string dec_ParentAccountNo, dec_AccountName, dec_AccountNo, dec_Password;
	ss >> dec_ParentAccountNo;
	ss >> dec_AccountName;
	ss >> dec_AccountNo;
	ss >> dec_Password;

	ParentAccountNo = cryptor::decrypt(dec_ParentAccountNo);
	AccountName = cryptor::decrypt(dec_AccountName);
	AccountNo = cryptor::decrypt(dec_AccountNo);
	Password = cryptor::decrypt(dec_Password);

	ss >> SeungSu >> Ratio >> _AccountLevel >> _Prime;
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
			CreateDefaultSubAccount();
		}
	}
}

void VtAccount::SaveToXml(pugi::xml_node& node_account)
{
	auto enc_ParentAccountNo = cryptor::encrypt(ParentAccountNo);
	auto enc_AccountName = cryptor::encrypt(AccountName);
	auto enc_AccountNo = cryptor::encrypt(AccountNo);
	auto enc_Password = cryptor::encrypt(Password);

	pugi::xml_node account_child = node_account.append_child("parent_account_no");
	account_child.append_child(pugi::node_pcdata).set_value(enc_ParentAccountNo.c_str());
	account_child = node_account.append_child("account_name");
	account_child.append_child(pugi::node_pcdata).set_value(enc_AccountName.c_str());
	account_child = node_account.append_child("account_no");
	account_child.append_child(pugi::node_pcdata).set_value(enc_AccountNo.c_str());
	account_child = node_account.append_child("password");
	account_child.append_child(pugi::node_pcdata).set_value(enc_Password.c_str());
	account_child = node_account.append_child("seungsu");
	account_child.append_child(pugi::node_pcdata).set_value(std::to_string(SeungSu).c_str());
	account_child = node_account.append_child("ratio");
	account_child.append_child(pugi::node_pcdata).set_value(std::to_string(Ratio).c_str());
	account_child = node_account.append_child("account_level");
	account_child.append_child(pugi::node_pcdata).set_value(std::to_string(_AccountLevel).c_str());
	account_child = node_account.append_child("prime");
	account_child.append_child(pugi::node_pcdata).set_value(std::to_string(_Prime).c_str());
	account_child = node_account.append_child("type");
	// 국내, 해외, FX 구분자
	if (_AccountLevel == 1) {
		account_child.append_child(pugi::node_pcdata).set_value(std::to_string(_ParentAccount->Type).c_str());
	}
	else {
		account_child.append_child(pugi::node_pcdata).set_value(std::to_string(Type).c_str());
	}
	if (_SubAccountList.size() > 0) {
		account_child = node_account.append_child("sub_account_list");
		for (auto it = _SubAccountList.begin(); it != _SubAccountList.end(); ++it) {
			VtAccount* subAcnt = *it;
			pugi::xml_node sub_account = account_child.append_child("sub_account");
			subAcnt->SaveToXml(sub_account);
		}
	}
}

void VtAccount::LoadFromXml(pugi::xml_node& account_node)
{
	_AccountLevel = std::stoi(account_node.child_value("account_level"));
	std::string dec_ParentAccountNo, dec_AccountName, dec_AccountNo, dec_Password;

	dec_ParentAccountNo = account_node.child_value("parent_account_no");
	dec_AccountName = account_node.child_value("account_name");
	dec_AccountNo = account_node.child_value("account_no");
	dec_Password = account_node.child_value("password");

	ParentAccountNo = cryptor::decrypt(dec_ParentAccountNo);
	AccountName = cryptor::decrypt(dec_AccountName);
	AccountNo = cryptor::decrypt(dec_AccountNo);
	Password = cryptor::decrypt(dec_Password);

	SeungSu = std::stoi(account_node.child_value("seungsu"));
	Ratio = std::stod(account_node.child_value("ratio"));
	std::stoi(account_node.child_value("prime")) == 0 ? _Prime = false : _Prime = true;
	if (_AccountLevel == 0) {
		VtAccountManager::GetInstance()->AddAccount(this);
	}
	else {
		_ParentAccount = VtAccountManager::GetInstance()->FindAccount(ParentAccountNo);
	}

	pugi::xml_node account_type_node = account_node.child("type");
	if (account_type_node) {
		Type = std::stoi(account_node.child_value("type"));
	}

	pugi::xml_node sub_account_list_node = account_node.child("sub_account_list");
	// 서브 계좌가 있는 경우
	if (sub_account_list_node) {
		for (pugi::xml_node sub_account_node = sub_account_list_node.child("sub_account"); sub_account_node; sub_account_node = sub_account_node.next_sibling("sub_account")) {
			VtAccount* sub_account = new VtAccount();
			sub_account->LoadFromXml(sub_account_node);
			_SubAccountList.push_back(sub_account);
			VtSubAccountManager::GetInstance()->FindAddAccount(sub_account);
		}
	}
	else {
		// 서브 계좌가 없지만 최상위 레벨 계좌이면 기본 서브 계좌를 만들어 준다.
		if (_AccountLevel == 0)
			CreateDefaultSubAccount();
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
	for (auto it = PositionMap.begin(); it != PositionMap.end(); ++it) {
		VtPosition* posi = it->second;
		OpenPL += posi->OpenProfitLoss;
		TradePL += posi->TradePL;
	}

	TotalPL = OpenPL + TradePL + Fee;
}

void VtAccount::CalcOpenPL(VtSymbol* sym)
{
	if (sym) {
		double curClose = sym->Quote.intClose / std::pow(10, sym->Decimal);
		VtPosition* posi = FindPosition(sym->ShortCode);
		if (posi)
			posi->OpenProfitLoss = posi->OpenQty * (curClose - posi->AvgPrice)*sym->Seungsu;
	}
}

void VtAccount::SumOpenPL(VtSymbol* sym, VtPosition* posi)
{
	if (sym && posi) {
		double curClose = sym->Quote.intClose / std::pow(10, sym->Decimal);
		posi->OpenProfitLoss = posi->OpenQty * (curClose - posi->AvgPrice)*sym->Seungsu;
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
	position->AvgPrice = order->intFilledPrice / std::pow(10, sym->Decimal);
	position->CurPrice = order->intFilledPrice / std::pow(10, sym->Decimal);
	position->SubAccountNo = order->SubAccountNo;
	position->FundName = order->FundName;
	position->orderType = order->Type;
	PositionMap[order->shortCode] = position;

	VtTotalOrderManager* totalOrderMgr = VtTotalOrderManager::GetInstance();
	if (order->Type == 0) {
		totalOrderMgr->AddPosition(order->Type, position->AccountNo, position->ShortCode, position);
	}
	else if (order->Type == 1) {
		totalOrderMgr->AddPosition(order->Type, position->SubAccountNo, position->ShortCode, position);
	}
	else if (order->Type == 2) {
		totalOrderMgr->AddPosition(order->Type, position->SubAccountNo, position->ShortCode, position);
	}

	if (order->Type == 1) {
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
	Sleep(VtGlobal::ServerSleepTime);
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
	Sleep(VtGlobal::ServerSleepTime);
	HdScheduler* scheduler = HdScheduler::GetInstance();
	HdTaskArg arg;
	arg.Type = HdTaskType::HdAcceptedHistory;
	arg.AddArg(_T("AccountNo"), AccountNo);
	arg.AddArg(_T("Password"), Password);
	scheduler->AddTask(std::move(arg));
}

void VtAccount::GetAcceptedHistory(std::vector<std::pair<std::string, HdTaskArg>>& argList)
{
	Sleep(VtGlobal::ServerSleepTime);
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

void VtAccount::GetAccountProfitLossDirect()
{
	VtHdClient* client = VtHdClient::GetInstance();
	client->GetApiCustomerProfitLoss(AccountNo, Password);
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

	double target = sym->intTickSize * size * sym->Seungsu;
	//target = target / std::pow(10, sym->IntDecimal);

	target = -1 * target;
	if (posi->OpenProfitLoss * std::pow(10, sym->Decimal) <= target)
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

	double target = sym->intTickSize * size * sym->Seungsu;
	//target = target / std::pow(10, sym->IntDecimal);

	target = -1 * target;
	if (posi->OpenProfitLoss * std::pow(10, sym->Decimal) <= target)
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

	double target = sym->intTickSize * size * sym->Seungsu;
	//target = target / std::pow(10, sym->IntDecimal);

	if (posi->OpenProfitLoss * std::pow(10, sym->Decimal) >= target)
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

	double target = sym->intTickSize * size * sym->Seungsu;
	//target = target / std::pow(10, sym->IntDecimal);

	if (posi->OpenProfitLoss * std::pow(10, sym->Decimal) >= target)
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

void VtAccount::CreateDefaultSubAccount()
{
	VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
	std::string acntNo = AccountNo;
	acntNo.append(_T("_1"));
	VtAccount* legSubAcnt = FindSubAccount(acntNo);
	if (!legSubAcnt) {
		VtAccount* subAcnt = CreateSubAccount(acntNo, AccountName, true);
		subAcnt->ParentAccount(this);
		subAcntMgr->FindAddAccount(subAcnt);
	}
	else {
		legSubAcnt->Prime(true);
	}
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


