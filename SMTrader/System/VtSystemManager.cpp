#include "stdafx.h"
#include "VtSystemManager.h"
#include "VtSystem.h"
#include "VtMaSystem.h"
#include "VtStochasticSystem.h"
#include "VtMacdSystem.h"
#include "VtChannelCrossSystem.h"
#include "../VtGlobal.h"
#include "VtChannelJumpSystem.h"
#include "VtOsillaterSystem.h"
#include "VtDailyHeightSystem.h"
#include "../VtChartData.h"
#include "../VtTrUs1.h"
#include "../VtTrUs2.h"
#include "../VtTrUs3.h"
#include "../VtUsIn1.h"
#include "../VtUsIn2.h"
#include "../VtUsIn3.h"
#include "../VtUsI2.h"
#include "../VtUsComp.h"
#include "../VtUsH2.h"
#include <chrono>
#include "../VtQIn1.h"
#include "../VtQIn2.h"
#include "../VtQIn3.h"
#include "../VtQIn4.h"
#include "../VtKp1a.h"
#include "../VtKp2s.h"
#include "../VtKp3s.h"
#include "../VtKp4b.h"
#include "../VtKp5In1.h"
#include "../VtKp6In2.h"
#include "../VtQH2.h"
#include "../VtQF2.h"
#include "../VtProductCategoryManager.h"
#include "../VtRealtimeRegisterManager.h"
#include "../VtSymbol.h"
#include "../VtChartData.h"
#include "../VtChartDataManager.h"
#include "../VtProductCategoryManager.h"
#include "../VtRealtimeRegisterManager.h"
#include "../VtChartDataCollector.h"
#include "../VtKo3s.h"
#include "../VtKo4b.h"
using namespace std::chrono;
VtSystemManager::VtSystemManager()
{
	_CreatedSystemList = false;
}


VtSystemManager::~VtSystemManager()
{
	for (auto it = _SystemVector.begin(); it != _SystemVector.end(); ++it) {
		delete *it;
	}

	_SystemMap.clear();
}

VtSystem* VtSystemManager::CreateSystem(VtSystemType systemType)
{
	VtSystem* system = nullptr;

	switch (systemType)
	{
	case VtSystemType::MaSystem:
		system = new VtMaSystem(VtSystemType::MaSystem, _T("MaSystem"));
		break;
	case VtSystemType::StoSystem:
		system = new VtStochasticSystem(VtSystemType::StoSystem, _T("StochasticSystem"));
		break;
	case VtSystemType::MacdSystem:
		system = new VtMacdSystem(VtSystemType::MacdSystem, _T("MacdSystem"));
		break;
	case VtSystemType::ChannelCross:
		system = new VtChannelCrossSystem(VtSystemType::ChannelCross, _T("ChannelCrossSystem"));
		break;
	case VtSystemType::ChannelJump:
		system = new VtChannelJumpSystem(VtSystemType::ChannelJump, _T("ChannelJumpSystem"));
		break;
	case VtSystemType::Osillator:
		system = new VtOsillaterSystem(VtSystemType::Osillator, _T("OsillaterSystem"));
		break;
	case VtSystemType::DailyHeight:
		system = new VtDailyHeightSystem(VtSystemType::DailyHeight, _T("DailyHeight"));
		break;
	case VtSystemType::KP_1A:
		system = new VtKp1a(VtSystemType::KP_1A, _T("KP_1A"));
		break;
	case VtSystemType::KP_2S:
		system = new VtKp2s(VtSystemType::KP_2S, _T("KP_2S"));
		break;
	case VtSystemType::KP_3S:
		system = new VtKp3s(VtSystemType::KP_3S, _T("KP_3S"));
		break;
	case VtSystemType::KP_4B:
		system = new VtKp4b(VtSystemType::KP_4B, _T("KP_4B"));
		break;
	case VtSystemType::KP_5IN1:
		system = new VtKp5In1(VtSystemType::KP_5IN1, _T("KP_5IN1"));
		break;
	case VtSystemType::KP_6IN2:
		system = new VtKp6In2(VtSystemType::KP_6IN2, _T("KP_6IN2"));
		break;
	case VtSystemType::KO_3S:
		system = new VtKo3s(VtSystemType::KO_3S, _T("KO_3S"));
		break;
	case VtSystemType::KO_4B:
		system = new VtKo4b(VtSystemType::KO_4B, _T("KO_4B"));
		break;
	case VtSystemType::Q_IN1:
		system = new VtQIn1(VtSystemType::Q_IN1, _T("Q_IN1"));
		break;
	case VtSystemType::Q_IN2:
		system = new VtQIn2(VtSystemType::Q_IN2, _T("Q_IN2"));
		break;
	case VtSystemType::Q_IN3:
		system = new VtQIn3(VtSystemType::Q_IN3, _T("Q_IN3"));
		break;
	case VtSystemType::Q_IN4:
		system = new VtQIn4(VtSystemType::Q_IN4, _T("Q_IN4"));
		break;
	case VtSystemType::Q_H2:
		system = new VtQH2(VtSystemType::Q_H2, _T("Q_H2"));
		break;
	case VtSystemType::Q_F2:
		system = new VtQF2(VtSystemType::Q_F2, _T("Q_F2"));
		break;
	case VtSystemType::TR_US1:
		system = new VtTrUs1(VtSystemType::TR_US1, _T("TR_US1"));
		break;
	case VtSystemType::TR_US2:
		system = new VtTrUs2(VtSystemType::TR_US2, _T("TR_US2"));
		break;
	case VtSystemType::TR_US3:
		system = new VtTrUs3(VtSystemType::TR_US3, _T("TR_US3"));
		break;
	case VtSystemType::US_IN1:
		system = new VtUsIn1(VtSystemType::US_IN1, _T("US_IN1"));
		break;
	case VtSystemType::US_IN2:
		system = new VtUsIn2(VtSystemType::US_IN2, _T("US_IN2"));
		break;
	case VtSystemType::US_IN3:
		system = new VtUsIn3(VtSystemType::US_IN3, _T("US_IN3"));
		break;
	case VtSystemType::US_I2:
		system = new VtUsI2(VtSystemType::US_I2, _T("US_I2"));
		break;
	case VtSystemType::US_Comp:
		system = new VtUsComp(VtSystemType::US_Comp, _T("US_Comp"));
		break;
	case VtSystemType::US_H2:
		system = new VtUsH2(VtSystemType::US_H2, _T("US_H2"));
		break;
	default:
		break;
	}

	_SystemMap[system->Name()] = system;
	_SystemVector.push_back(system);

	VtTime curTime = VtGlobal::GetLocalTime();
	int minMod = curTime.min % system->Cycle();
	int waitTime = system->Cycle() * 60 - (minMod * 60 + curTime.sec);
	// Add to the timer.
	auto id = _Timer.add(seconds(waitTime), std::bind(&VtSystem::OnTimer, system), seconds(system->Cycle() * 60));
	// Add to the request map.
	_TimerMap[system->Name()] = id;

	return system;
}

void VtSystemManager::RemoveSystem(std::string name)
{
	auto it = _SystemMap.find(name);
	if (it != _SystemMap.end())
	{
		delete it->second;
		_SystemMap.erase(it);
	}
}

VtSystem* VtSystemManager::GetSystem(std::string sysName)
{
	auto it = _SystemMap.find(sysName);
	if (it != _SystemMap.end())
	{
		return it->second;
	}

	return nullptr;
}

void VtSystemManager::CreateBundleSystem()
{
	if (_CreatedSystemList)
		return;

	VtSystem* sys = CreateSystem(VtSystemType::MaSystem);

	sys = CreateSystem(VtSystemType::MacdSystem);

	sys = CreateSystem(VtSystemType::ChannelCross);

	sys = CreateSystem(VtSystemType::ChannelJump);

	sys = CreateSystem(VtSystemType::DailyHeight);

	sys = CreateSystem(VtSystemType::Osillator);

	sys = CreateSystem(VtSystemType::StoSystem);

	_CreatedSystemList = true;
}

void VtSystemManager::SetDataMap(VtChartData* chartData, VtSystem* system)
{
	if (!chartData || !system || system->SystemType() == VtSystemType::None)
		return;

	switch (system->SystemType())
	{
	case VtSystemType::MaSystem:
	{
		system->DateTime(chartData->DateTime.data());
		system->AddDataSource(_T("high"), chartData->High.data());
		system->AddDataSource(_T("low"), chartData->Low.data());
		system->AddDataSource(_T("close"), chartData->Close.data());
		system->CreateSignal(0, chartData->GetDataCount() - 1);
	}
	break;
	case VtSystemType::StoSystem:
	{
		system->DateTime(chartData->DateTime.data());
		system->AddDataSource(_T("high"), chartData->Close.data());
		system->AddDataSource(_T("low"), chartData->Low.data());
		system->AddDataSource(_T("close"), chartData->Close.data());
		system->CreateSignal(0, chartData->GetDataCount() - 1);
	}
	break;
	case VtSystemType::MacdSystem:
	{
		system->DateTime(chartData->DateTime.data());
		system->AddDataSource(_T("close"), chartData->Close.data());
		system->CreateSignal(0, chartData->GetDataCount() - 1);
	}
	break;
	case VtSystemType::ChannelCross:
	{
		system->DateTime(chartData->DateTime.data());
		system->AddDataSource(_T("close"), chartData->Close.data());
		system->AddDataSource(_T("high"), chartData->High.data());
		system->AddDataSource(_T("low"), chartData->Low.data());
		system->AddDataSource(_T("timestamp"), chartData->DateTime.data());
		system->CreateSignal(0, chartData->GetDataCount() - 1);
	}
	break;
	case VtSystemType::ChannelJump:
	{
		system->DateTime(chartData->DateTime.data());
		system->AddDataSource(_T("close"), chartData->Close.data());
		system->AddDataSource(_T("high"), chartData->High.data());
		system->AddDataSource(_T("low"), chartData->Low.data());
		system->AddDataSource(_T("timestamp"), chartData->DateTime.data());
		system->CreateSignal(0, chartData->GetDataCount() - 1);
	}
	break;

	case VtSystemType::Osillator:
	{
		system->DateTime(chartData->DateTime.data());
		system->AddDataSource(_T("close"), chartData->Close.data());
		system->AddDataSource(_T("high"), chartData->High.data());
		system->AddDataSource(_T("low"), chartData->Low.data());
		system->AddDataSource(_T("timestamp"), chartData->DateTime.data());
		system->CreateSignal(0, chartData->GetDataCount() - 1);
	}
	break;

	case VtSystemType::DailyHeight:
	{
		system->DateTime(chartData->DateTime.data());
		system->Time(chartData->Time.data());
		system->Date(chartData->Date.data());
		system->AddDataSource(_T("close"), chartData->Close.data());
		system->AddDataSource(_T("high"), chartData->High.data());
		system->AddDataSource(_T("low"), chartData->Low.data());
		system->AddDataSource(_T("open"), chartData->Low.data());
		system->AddDataSource(_T("timestamp"), chartData->DateTime.data());
		system->CreateSignal(0, chartData->GetDataCount() - 1);
	}
	break;

	default:
		break;
	}
}

void VtSystemManager::UpdateSystem(int index)
{
	for (auto it = _SystemVector.begin(); it != _SystemVector.end(); ++it) {
		VtSystem* sys = *it;
		if (sys->Enable()) {
			sys->UpdateSignal(index);
		}
	}
}

void VtSystemManager::OnTimer()
{
	for (auto it = _SystemVector.begin(); it != _SystemVector.end(); ++it) {
		VtSystem* sys = *it;
		if (sys->Enable()) {
			sys->OnTimer();
		}
	}
}


void VtSystemManager::OnRegularTimer()
{
	for (auto it = _SystemVector.begin(); it != _SystemVector.end(); ++it) {
		VtSystem* sys = *it;
		if (sys->Enable()) {
			sys->OnRegularTimer();
		}
	}
}

VtChartData* VtSystemManager::AddDataSource(std::string symCode, VtChartType type, int cycle)
{
	// 여기서 실시간 차트 데이터 수집을 등록해 준다.
	VtChartDataCollector* chartDataCollector = VtChartDataCollector::GetInstance();
	VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
	VtChartData* chartData = chartDataMgr->FindNAdd(symCode, type, cycle);
	if (chartData) {
		chartDataCollector->RegisterChartData(chartData);
	}

	return chartData;
}

void VtSystemManager::RemoveTimer(VtSystem* sys)
{
	if (!sys)
		return;
	auto it = _TimerMap.find(sys->Name());
	if (it != _TimerMap.end()) {
		auto id = it->second;
		_Timer.remove(id);
	}
}

void VtSystemManager::InitDataSources()
{
	InitDataSource(1);
	InitDataSource(5);
}

void VtSystemManager::UpdateRealtimeArgs(VtChartData* chartData)
{
	if (!chartData)
		return;
	std::string symCode = chartData->SymbolCode().substr(0, 3);
	VtSymbol* sym = _ArgMap[symCode];
	if (!sym)
		return;

	if (symCode.compare(_T("101")) == 0) {
		Kbs = sym->Hoga.TotBuyQty;
		Kas = sym->Hoga.TotSellQty;
		Kbc = sym->Hoga.TotBuyNo;
		Kac = sym->Hoga.TotSellNo;
	} 
	else if (symCode.compare(_T("106")) == 0) {
		Qbs = sym->Hoga.TotBuyQty;
		Qas = sym->Hoga.TotSellQty;
		Qbc = sym->Hoga.TotBuyNo;
		Qac = sym->Hoga.TotSellNo;
	}
	else if (symCode.compare(_T("175")) == 0) {
		Ubs = sym->Hoga.TotBuyQty;
		Uas = sym->Hoga.TotSellQty;
		Ubc = sym->Hoga.TotBuyNo;
		Uac = sym->Hoga.TotSellNo;
	}
}

void VtSystemManager::AddSystemDialog(VtUsdStrategyConfigDlg* dlg)
{
	_SysDlgMap[dlg] = dlg;
}

void VtSystemManager::RemoveSystemDialog(VtUsdStrategyConfigDlg* dlg)
{
	auto it = _SysDlgMap.find(dlg);
	if (it != _SysDlgMap.end()) {
		_SysDlgMap.erase(it);
	}
}

void VtSystemManager::InitDataSource(int cycle)
{
	VtRealtimeRegisterManager* realRegiMgr = VtRealtimeRegisterManager::GetInstance();
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	// Kospi200 총호가 수량과 건수
	VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
	if (sym) {
		std::string symCode = sym->ShortCode;
		realRegiMgr->RegisterProduct(symCode);
		// 5분봉 데이터 추가
		VtChartData* data = AddDataSource(symCode, VtChartType::MIN, cycle);
		data->RequestChartData();
		// 매도호가총수량
		std::string code = symCode + (_T("SHTQ"));
		AddDataSource(code, VtChartType::MIN, cycle);
		// 매수호가총수량
		code = symCode + (_T("BHTQ"));
		AddDataSource(code, VtChartType::MIN, cycle);
		// 매도호가총건수
		code = symCode + (_T("SHTC"));
		AddDataSource(code, VtChartType::MIN, cycle);
		// 매수호가총건수
		code = symCode + (_T("BHTC"));
		AddDataSource(code, VtChartType::MIN, cycle);
		_ArgMap[_T("101")] = sym;
	}
	// 코스닥 150 선눌지수와 건수
	sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
	if (sym) {
		std::string symCode = sym->ShortCode;
		// 실시간 데이터 등록
		realRegiMgr->RegisterProduct(symCode);
		// 주기 데이터 추가
		VtChartData* data = AddDataSource(symCode, VtChartType::MIN, cycle);
		data->RequestChartData();
		// 매도호가총수량
		std::string code = symCode + (_T("SHTQ"));
		AddDataSource(code, VtChartType::MIN, cycle);
		// 매수호가총수량
		code = symCode + (_T("BHTQ"));
		AddDataSource(code, VtChartType::MIN, cycle);
		// 매도호가총건수
		code = symCode + (_T("SHTC"));
		AddDataSource(code, VtChartType::MIN, cycle);
		// 매수호가총건수
		code = symCode + (_T("BHTC"));
		AddDataSource(code, VtChartType::MIN, cycle);
		_ArgMap[_T("106")] = sym;
	}

	// 원달러 선물지수와 건수
	sym = prdtCatMgr->GetRecentFutureSymbol(_T("175F"));
	if (sym) {
		std::string symCode = sym->ShortCode;
		// 실시간 데이터 등록
		realRegiMgr->RegisterProduct(symCode);
		// 주기 데이터 추가
		VtChartData* data = AddDataSource(symCode, VtChartType::MIN, cycle);
		data->RequestChartData();
		// 매도호가총수량
		std::string code = symCode + (_T("SHTQ"));
		AddDataSource(code, VtChartType::MIN, cycle);
		// 매수호가총수량
		code = symCode + (_T("BHTQ"));
		AddDataSource(code, VtChartType::MIN, cycle);
		// 매도호가총건수
		code = symCode + (_T("SHTC"));
		AddDataSource(code, VtChartType::MIN, cycle);
		// 매수호가총건수
		code = symCode + (_T("BHTC"));
		AddDataSource(code, VtChartType::MIN, cycle);
		_ArgMap[_T("175")] = sym;
	}
}
