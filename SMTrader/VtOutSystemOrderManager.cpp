#include "stdafx.h"
#include "VtOutSystemOrderManager.h"
#include "System/VtSystem.h"
#include <algorithm>
#include "VtOutSignalDef.h"

VtOutSystemOrderManager::VtOutSystemOrderManager()
{
}


VtOutSystemOrderManager::~VtOutSystemOrderManager()
{
}

void VtOutSystemOrderManager::AddSystem(SharedSystem sys)
{
	if (!sys)
		return;

	_SystemVec.push_back(sys);
	AddSignalOrder(sys);
}

void VtOutSystemOrderManager::RemoveSystem(SharedSystem sys)
{
	if (!sys) return;

	auto it = std::find_if(_SystemVec.begin(), _SystemVec.end(), [&](const SharedSystem& x) { return x->Id() == sys->Id(); });
	if (it != _SystemVec.end()) {
		RemoveSignalOrder(sys);
		_SystemVec.erase(it);
	}
}

void VtOutSystemOrderManager::AddSignalOrder(SharedSystem sys)
{
	if (!sys || !sys->OutSignal()) return;

	auto it = _SignalOrderMap.find(sys->OutSignal()->SignalName);
	if (it != _SignalOrderMap.end()) {
		SharedSystemMap& orderMap = it->second;
		orderMap[sys->Id()] = sys;
	}
	else {
		SharedSystemMap orderMap;
		orderMap[sys->Id()] = sys;
		_SignalOrderMap[sys->OutSignal()->SignalName] = orderMap;
	}
}

void VtOutSystemOrderManager::RemoveSignalOrder(SharedSystem sys)
{
	if (!sys || !sys->OutSignal()) return;

	auto it = _SignalOrderMap.find(sys->OutSignal()->SignalName);
	if (it != _SignalOrderMap.end()) {
		SharedSystemMap& orderMap = it->second;
		auto iit = orderMap.find(sys->Id());
		if (iit != orderMap.end()) {
			orderMap.erase(iit);
		}
	}
}
