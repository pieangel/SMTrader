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
#include "../VtBeta.h"
#include <chrono>
#include "../VtAlpha.h"
#include "../VtQIn1.h"
#include "../VtQIn2.h"
#include "../VtQIn3.h"
#include "../VtQIn4.h"

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
	case VtSystemType::Alpha:
		system = new VtAlpha(VtSystemType::Alpha, _T("Alpha"));
		break;
	case VtSystemType::Beta:
		system = new VtBeta(VtSystemType::Beta, _T("Beta"));
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
