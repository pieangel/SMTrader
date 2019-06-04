#include "stdafx.h"
#include "VtScheduler.h"
#include "System/VtSystemManager.h"
#include "System/VtSystem.h"
#include "Scheduler/Scheduler.h"
#include <chrono>
#include <iomanip>
VtScheduler::VtScheduler()
{
	_Scheduler = new Bosma::Scheduler(12);
}

VtScheduler::~VtScheduler()
{
	if (_Scheduler) {
		delete _Scheduler;
		_Scheduler = nullptr;
	}
}

void VtScheduler::RepeatSymbolService()
{
	std::chrono::milliseconds milSec(200);
	_Scheduler->every(milSec, [this]() { this->SendRealtimeSymbol(); });
}

void OnSystemLiq(std::string sys_name, std::chrono::system_clock::time_point tp) {
	VtScheduler* sch = VtScheduler::GetInstance();
	sch->ExecSystemLiq(sys_name, tp);
}

void VtScheduler::RegisterSystemLiq(VtSystem* sys, std::string liq_time)
{
	if (!sys)
		return;

	sys->GetDailyIndexByChart();

	std::chrono::system_clock::time_point liq_timepoint = _Scheduler->get_timepoint(liq_time);
	std::chrono::system_clock::time_point tp = _Scheduler->at(liq_time, OnSystemLiq, sys->Name(), liq_timepoint);
	if (tp == liq_timepoint) {
		CString msg;
		msg.Format("register :: sys_name = %s, tp = %ld\n", sys->Name().c_str(), liq_timepoint);
		TRACE(msg);

		sys->LiqTimepoint = tp;
	}
}

void VtScheduler::SendRealtimeSymbol()
{
	
}

void VtScheduler::ExecSystemLiq(std::string sys_name, std::chrono::system_clock::time_point tp)
{
	VtSystemManager* sysMgr = VtSystemManager::GetInstance();
	VtSystem* sys = sysMgr->GetSystem(sys_name);
	if (sys) {
		std::string liq_time = sys->GetLiqTime();
		std::chrono::system_clock::time_point liq_timepoint = _Scheduler->get_timepoint(liq_time);
		CString msg;
		msg.Format("exec ::sys_name = %s, tp = %ld, liq_tp = %ld\n", sys->Name().c_str(), tp, liq_timepoint);
		TRACE(msg);
		if (tp == sys->LiqTimepoint) {
			sys->OnRegularTimer();
		}
	}
}
