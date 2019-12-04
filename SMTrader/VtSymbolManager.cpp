#include "stdafx.h"
#include "VtSymbolManager.h"
#include "VtSymbol.h"
#include "VtSymbolMaster.h"
#include "VtKrClient.h"
#include "VtTaskManager.h"
#include <algorithm>
#include "VtWindowManager.h"
#include "Poco/Delegate.h"
#include "MainFrm.h"
#include "VtOrderRightWnd.h"
#include "VtOrderDialogManager.h"
#include "Poco/NumberFormatter.h"
#include "HdScheduler.h"
#include "HdProductInfo.h"
#include "ZmConfigManager.h"
using Poco::NumberFormatter;

VtSymbolManager::VtSymbolManager()
{
	Begin();
}


VtSymbolManager::~VtSymbolManager()
{
	End();
}


void VtSymbolManager::AddSymbolMasterToSet(std::string fullCode)
{
	SymbolMasterUpdateSet.insert(fullCode);
}

void VtSymbolManager::RequestSymbolMasterBatch()
{
	HdScheduler* scheduler = HdScheduler::GetInstance();
	scheduler->SymbolCount(SymbolMap.size());
	for (auto it = SymbolMap.begin(); it != SymbolMap.end(); ++it)
	{
		VtSymbol* sym = it->second;
		HdTaskArg arg;
		arg.Type = HdTaskType::HdSymbolMaster;
		arg.AddArg(_T("SymbolCode"), sym->ShortCode);
		scheduler->AddTask(std::move(arg));
	}
}

void VtSymbolManager::RequestSymbolBundleInfo(std::string fullCode)
{
	VtTaskManager* taskMgr = VtTaskManager::GetInstance();
	taskMgr->DoBundleTaskForSymbol(fullCode);
}

void VtSymbolManager::RequestSymbolMaster(std::string fullCode)
{
	VtKrClient* client = VtKrClient::GetInstance();
	client->RequestSymbolMaster(fullCode);
}

void VtSymbolManager::RequestSymbolQuote(std::string fullCode)
{
	VtKrClient* client = VtKrClient::GetInstance();
	client->RequestSymbolQuote(fullCode);
}

void VtSymbolManager::RequestSymbolHoga(std::string fullCode)
{
	VtKrClient* client = VtKrClient::GetInstance();
	client->RequestSymbolHoga(fullCode);
}

void VtSymbolManager::RequestSymbolTickQuote(std::string fullCode)
{
	VtKrClient* client = VtKrClient::GetInstance();
	client->RequestSymbolTickQuote(fullCode);
}
// 
// void VtSymbolManager::AddSymbol(VtSymbol* symbol)
// {
// 	if (symbol == nullptr)
// 		return;
// 	FullCodeMap[symbol->ShortCode] = symbol->FullCode;
// 	ShortCodeMap[symbol->FullCode] = symbol->ShortCode;
// 	SymbolMap[symbol->ShortCode] = symbol;
// }

void VtSymbolManager::AddHdSymbol(VtSymbol* symbol)
{
	FullCodeMap[symbol->ShortCode] = symbol->FullCode;
	ShortCodeMap[symbol->FullCode] = symbol->ShortCode;
	SymbolMap[symbol->ShortCode] = symbol;
}

VtSymbol* VtSymbolManager::FindAddSymbol(std::string symbol_code)
{
	VtSymbol* symbol = FindHdSymbol(symbol_code);
	if (!symbol) {
		symbol = new VtSymbol();
		symbol->ShortCode = symbol_code;
		AddHdSymbol(symbol);
	}

	return symbol;
}

VtSymbol* VtSymbolManager::FindSymbol(std::string shortCode)
{
	auto it = SymbolMap.find(shortCode);
	if (it != SymbolMap.end())
		return it->second;
	else
		return nullptr;
}

VtSymbol* VtSymbolManager::FindHdSymbol(std::string shortCode)
{
	auto it = SymbolMap.find(shortCode);
	if (it != SymbolMap.end())
		return it->second;
	else
		return nullptr;
}

VtSymbol* VtSymbolManager::FindSymbolByFullCode(std::string code)
{
	std::string shortCode = ShortCodeMap[code];
	return FindSymbol(shortCode);
}

void VtSymbolManager::AddSymbolMaster(VtSymbolMaster* symbol)
{
	SymbolMasterMap[symbol->FullCode] = symbol;
}

VtRealtimeSymbolMaster* VtSymbolManager::FindRealtimeSymbolMasterByFullCode(std::string code)
{
	auto it = RealtimeSymbolMasterMap.find(code);
	if (it != RealtimeSymbolMasterMap.end())
		return it->second;
	else
	{
		VtRealtimeSymbolMaster* master = new VtRealtimeSymbolMaster();
		master->FullCode = code;
		RealtimeSymbolMasterMap[code] = master;
		return master;
	}
}

VtSymbolMaster* VtSymbolManager::FindSymbolMasterByFullCode(std::string code)
{
	auto it = SymbolMasterMap.find(code);
	if (it != SymbolMasterMap.end())
		return it->second;
	else
		return nullptr;
}

VtSymbolMaster* VtSymbolManager::FindAddSymbolMaster(std::string fullCode)
{
	auto it = SymbolMasterMap.find(fullCode);
	if (it != SymbolMasterMap.end())
		return it->second;
	else
	{
		VtSymbolMaster* symMaster = new VtSymbolMaster();
		symMaster->FullCode = fullCode;
		SymbolMasterMap[fullCode] = symMaster;
		return symMaster;
	}
}

VtSymbolMaster* VtSymbolManager::FindSymbolMasterByShortCode(std::string code)
{
	VtSymbol* sym = FindSymbol(code);
	auto it = SymbolMasterMap.find(sym->FullCode);
	if (it != SymbolMasterMap.end())
		return it->second;
	else
		return nullptr;
}

std::string VtSymbolManager::GetShortCode(std::string fullCode)
{
	return ShortCodeMap[fullCode];
}

std::string VtSymbolManager::GetFullCode(std::string shortCode)
{
	return FullCodeMap[shortCode];
}

/// <summary>
/// 취소 주문에만 사용해야 한다.
/// </summary>
/// <param name="shortCode"></param>
/// <returns></returns>
int VtSymbolManager::GetSymbolDecimal(std::string shortCode)
{
	VtSymbolMaster* master = FindSymbolMasterByShortCode(shortCode);
	if (!master)
		return 0;
	else
		return master->decimal;
}

long VtSymbolManager::GetInt(std::string value)
{
	if (value.length() == 0 )
		return 0;
	value.erase(std::remove(value.begin(), value.end(), '.'), value.end());
	return std::stol(value);
}

int VtSymbolManager::GetTickDiff(std::string src, std::string dst, VtSymbolMaster* symMaster)
{
	if (!symMaster || src.length() == 0 || dst.length() == 0)
		return 0;

	src.erase(std::remove(src.begin(), src.end(), '.'), src.end());
	dst.erase(std::remove(dst.begin(), dst.end(), '.'), dst.end());
	std::string tick = symMaster->Scn_Tick_size;
	tick.erase(std::remove(tick.begin(), tick.end(), '.'), tick.end());

	int srcVal = std::stoi(src);
	int dstVal = std::stoi(dst);
	int tickVal = std::stoi(tick);

	int diff = dstVal - srcVal;
	int count = diff / tickVal;
	return count;
}

int VtSymbolManager::GetTickDiff(double src, double dst, VtSymbolMaster* symMaster)
{
	int diff = 0;
	if (!symMaster)
		return diff;
	/*
	double delta = ROUNDING(src, symMaster->decimal) - ROUNDING(dst, symMaster->decimal);
	delta = ROUNDING(delta, symMaster->decimal);

	int delta2 = (int)(delta * pow(10, symMaster->decimal));
	return delta2 / symMaster->TickSizeInt;
	*/
	std::string srcVal = NumberFormatter::format(src, symMaster->decimal);
	std::string dstVal = NumberFormatter::format(dst, symMaster->decimal);
	srcVal.erase(std::remove(srcVal.begin(), srcVal.end(), '.'), srcVal.end());
	dstVal.erase(std::remove(dstVal.begin(), dstVal.end(), '.'), dstVal.end());
	int intSrc = std::stoi(srcVal);
	int intDst = std::stoi(dstVal);
	return (intSrc - intDst) / symMaster->TickSizeInt;
}

int VtSymbolManager::GetTickDiff(double src, std::string dst, VtSymbolMaster* symMaster)
{
	ASSERT(symMaster);
	double dest = std::stof(dst);
	double delta = ROUNDING(src, symMaster->decimal) - ROUNDING(dest, symMaster->decimal);

	return (int)(delta / symMaster->tickSize);
}

int VtSymbolManager::GetTickDiff(int src, int dst, VtSymbol* sym)
{
	int diff = -9999;
	if (!sym || sym->intTickSize == 0)
		return diff;

	return (src - dst) / sym->intTickSize;
}

std::string VtSymbolManager::GetNext(std::string src, VtSymbolMaster* symMaster, int size)
{
	std::string strNewValue = _T("");
	if (!symMaster || src.length() == 0)
		return src;

	src.erase(std::remove(src.begin(), src.end(), '.'), src.end());
	std::string tick = symMaster->Scn_Tick_size;
	tick.erase(std::remove(tick.begin(), tick.end(), '.'), tick.end());

	int srcVal = std::stoi(src);
	int tickVal = std::stoi(tick);

	int newVal = srcVal + tickVal * size;
	strNewValue = std::to_string(newVal);
	int newPos = strNewValue.length() - symMaster->decimal;
	if (newPos < 0)
	{
		strNewValue = strNewValue.insert(0, -newPos, '0');
		strNewValue = strNewValue.insert(0, 1, '.');
		strNewValue = strNewValue.insert(0, 1, '0');
	}
	else if (newPos == 0)
	{
		strNewValue = strNewValue.insert(0, 1, '.');
		strNewValue = strNewValue.insert(0, 1, '0');
	}
	else
		strNewValue = strNewValue.insert(newPos, 1, '.');
	return strNewValue;
}


double VtSymbolManager::GetNextValue(double src, VtSymbolMaster* symMaster, int size)
{
	if (!symMaster)
		return 0.0;

	return ROUNDING(src, symMaster->decimal) + symMaster->tickSize * size;
}

int VtSymbolManager::GetNextValue(int src, int size, VtSymbol* sym)
{
	if (!sym)
		return src;

	int result = src + sym->intTickSize * size;

	return result;
}

std::tuple<int, int> VtSymbolManager::GetExpireYearMonth(std::string symCode)
{
	int year = 0, month = 0;
	if (symCode.length() == 0)
		return std::make_tuple(year, month);

	char syear = symCode.at(3);
	char smonth = symCode.at(4);
	
	switch (syear)
	{
	case 'A':
		year = 2006;
		break;
	case 'B':
		year = 2007;
		break;
	case 'C':
		year = 2008;
		break;
	case 'D':
		year = 2009;
		break;
	case 'E':
		year = 2010;
		break;
	case 'F':
		year = 2011;
		break;
	case 'G':
		year = 2012;
		break;
	case 'H':
		year = 2013;
		break;
	case 'J':
		year = 2014;
		break;
	case 'K':
		year = 2015;
		break;
	case 'L':
		year = 2016;
		break;
	case 'M':
		year = 2017;
		break;
	case 'N':
		year = 2018;
		break;
	case 'P':
		year = 2019;
		break;
	case 'Q':
		year = 2020;
		break;
	case 'R':
		year = 2021;
		break;
	case 'S':
		year = 2022;
		break;
	case 'T':
		year = 2023;
		break;
	case 'V':
		year = 2024;
		break;
	case 'W':
		year = 2025;
		break;
	}

	switch (smonth)
	{
	case '1':
		month = 1;
		break;
	case '2':
		month = 2;
		break;
	case '3':
		month = 3;
		break;
	case '4':
		month = 4;
		break;
	case '5':
		month = 5;
		break;
	case '6':
		month = 6;
		break;
	case '7':
		month = 7;
		break;
	case '8':
		month = 8;
		break;
	case '9':
		month = 9;
		break;
	case 'A':
		month = 10;
		break;
	case 'B':
		month = 11;
		break;
	case 'C':
		month = 12;
		break;
	}

	return std::make_tuple(year, month);
}

std::tuple<int, int> VtSymbolManager::GetYearMonth(std::string code)
{
	if (code.length() < 2)
		return std::make_tuple(0, 0);

	char syear = code.at(0);
	char smonth = code.at(1);
	int year = 0, month = 0;

	switch (syear)
	{
	case 'A':
		year = 2006;
		break;
	case 'B':
		year = 2007;
		break;
	case 'C':
		year = 2008;
		break;
	case 'D':
		year = 2009;
		break;
	case 'E':
		year = 2010;
		break;
	case 'F':
		year = 2011;
		break;
	case 'G':
		year = 2012;
		break;
	case 'H':
		year = 2013;
		break;
	case 'J':
		year = 2014;
		break;
	case 'K':
		year = 2015;
		break;
	case 'L':
		year = 2016;
		break;
	case 'M':
		year = 2017;
		break;
	case 'N':
		year = 2018;
		break;
	case 'P':
		year = 2019;
		break;
	case 'Q':
		year = 2020;
		break;
	case 'R':
		year = 2021;
		break;
	case 'S':
		year = 2022;
		break;
	case 'T':
		year = 2023;
		break;
	case 'V':
		year = 2024;
		break;
	case 'W':
		year = 2025;
		break;
	}

	switch (smonth)
	{
	case '1':
		month = 1;
		break;
	case '2':
		month = 2;
		break;
	case '3':
		month = 3;
		break;
	case '4':
		month = 4;
		break;
	case '5':
		month = 5;
		break;
	case '6':
		month = 6;
		break;
	case '7':
		month = 7;
		break;
	case '8':
		month = 8;
		break;
	case '9':
		month = 9;
		break;
	case 'A':
		month = 10;
		break;
	case 'B':
		month = 11;
		break;
	case 'C':
		month = 12;
		break;
	}

	return std::make_tuple(year, month);
}

std::string& VtSymbolManager::TrimRight(std::string& input, char c)
{
	input.erase(input.find_last_not_of(c) + 1);
	return input;
}

std::string& VtSymbolManager::TrimLeft(std::string& input, char c)
{
	input.erase(0, input.find_first_not_of(c));
	return input;
}

std::string& VtSymbolManager::TrimBoth(std::string& input, char c)
{
	return TrimLeft(TrimRight(input, c), c);
}

HdProductInfo* VtSymbolManager::AddProductInfo(std::string code)
{
	HdProductInfo* prdtInfo = FindProductInfo(code);
	if (!prdtInfo)
		prdtInfo = new HdProductInfo();
	prdtInfo->code = code;

	ProductInfoMap[code] = prdtInfo;

	return prdtInfo;
}

HdProductInfo* VtSymbolManager::FindProductInfo(std::string code)
{
	auto it = ProductInfoMap.find(code);
	if (it != ProductInfoMap.end())
		return it->second;
	else
		return nullptr;
}

void VtSymbolManager::LoadAbroadSymbols()
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string appPath = configMgr->GetAppPath();
	appPath.append(_T("\\mst\\jMCODE.cod"));

	std::ifstream input(appPath);

	for (std::string line; getline(input, line); )
	{
		std::string sub = line;
	}
}

void VtSymbolManager::OnReceivedSymbolList()
{
	VtSymbolEventArgs arg;
	arg.pSymMgr = this;
	FireCodeListReceived(std::move(arg));
}

void VtSymbolManager::UpdateSymbolMaster()
{
	VtTaskManager* taskMgr = VtTaskManager::GetInstance();
	/*taskMgr->DoBundleTaskForSymbol(fullCode);*/
	for (auto it = SymbolMasterUpdateSet.begin(); it != SymbolMasterUpdateSet.end(); ++it)
	{
		std::string fullCode = *it;
		VtSymbolMaster* symMaster = FindSymbolMasterByFullCode(fullCode);
		if (!symMaster)
		{
			VtTaskSymbolMasterArg* arg = new VtTaskSymbolMasterArg(VtTaskType::VtTaskSymbolMaster);
			arg->FullCode = fullCode;
			taskMgr->AddTask(arg);
		}
	}

	if (!taskMgr->IsExecuting())
		taskMgr->StartTask();
}

void VtSymbolManager::OnReceivedSymbolMaster(VtSymbolMaster* symMaster)
{
	if (!symMaster)
		return;
	VtOrderDialogManager* orderWndMgr = VtOrderDialogManager::GetInstance();
	orderWndMgr->OnReceiveSymbolMaster(symMaster);
}

void VtSymbolManager::OnReceivedSymbolMaster(VtRealtimeSymbolMaster* symMaster)
{
	if (!symMaster)
		return;

	VtWindowManager* wndMgr = VtWindowManager::GetInstance();

	CVtOrderRightWnd* wnd1 = (CVtOrderRightWnd*)wndMgr->FindWindow(_T("OrderRightWnd"));
	if (wnd1)
	{
		wnd1->OnReceivedSymbolMaster(symMaster);
	}
}

void VtSymbolManager::Begin()
{
	VtWindowManager* wndMgr = VtWindowManager::GetInstance();
	CMainFrame* pMainFrm = (CMainFrame*)wndMgr->FindWindow(_T("MainFrame"));
	//_SymbolListReceivedEvent += delegate(pMainFrm, &CMainFrame::OnSymbolListReceived);
}

void VtSymbolManager::End()
{
	VtWindowManager* wndMgr = VtWindowManager::GetInstance();
	CMainFrame* pMainFrm = (CMainFrame*)wndMgr->FindWindow(_T("MainFrame"));
	//_SymbolListReceivedEvent -= delegate(pMainFrm, &CMainFrame::OnSymbolListReceived);
	for (auto it = SymbolMap.begin(); it != SymbolMap.end(); ++it)
	{
		delete it->second;
	}

	for (auto it = SymbolMasterMap.begin(); it != SymbolMasterMap.end(); ++it)
	{
		delete it->second;
	}

	for (auto it = RealtimeSymbolMasterMap.begin(); it != RealtimeSymbolMasterMap.end(); ++it)
	{
		delete it->second;
	}

	for (auto it = ProductInfoMap.begin(); it != ProductInfoMap.end(); ++it)
	{
		delete it->second;
	}
}

void VtSymbolManager::OnReceivedRealtimeSymbolMaster(VtSymbolMaster* symMaster)
{
	if (!symMaster)
		return;

	VtOrderDialogManager* orderWndMgr = VtOrderDialogManager::GetInstance();
	orderWndMgr->OnReceiveRealtimeSymbolMaster(symMaster);
}
