#pragma once
#include "Global/TemplateSingleton.h"
#include <chrono>
namespace Bosma
{
	class Scheduler;
};
class VtSystem;
class VtScheduler : public TemplateSingleton<VtScheduler>
{
public:
	VtScheduler();
	~VtScheduler();
	void RepeatSymbolService();
	void RegisterSystemLiq(VtSystem* sym, std::string liq_time);
	void ExecSystemLiq(std::string sys_name, std::chrono::system_clock::time_point tp);
private:
	Bosma::Scheduler* _Scheduler = nullptr;
	void SendRealtimeSymbol();
};

