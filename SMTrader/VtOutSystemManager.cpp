#include "stdafx.h"
#include "VtOutSystemManager.h"
#include "System/VtSystem.h"
#include <algorithm>

void VtOutSystemManager::AddSystem(SharedSystem sys)
{
	if (!sys)
		return;

	_SystemVec.push_back(sys);
}

void VtOutSystemManager::RemoveSystem(int sysId)
{
	auto it = std::find_if(_SystemVec.begin(), _SystemVec.end(), [&](const SharedSystem& x) { return x->Id() == sysId; });
	if (it != _SystemVec.end()) {
		_SystemVec.erase(it);
	}
}

VtOutSystemManager::VtOutSystemManager()
{
}


VtOutSystemManager::~VtOutSystemManager()
{
}

void VtOutSystemManager::Save(simple::file_ostream<same_endian_type>& ss)
{
	size_t count = _SystemVec.size();
	ss << count; // 시스템 갯수
	
	for (auto it = _SystemVec.begin(); it != _SystemVec.end(); ++it) {
		SharedSystem sys = *it;
		ss << (int)sys->SystemType();
	}

	// 각 시스템을 저장한다.
	for (auto it = _SystemVec.begin(); it != _SystemVec.end(); ++it) {
		SharedSystem sys = *it;
		sys->Save(ss);
	}
}

void VtOutSystemManager::Load(simple::file_istream<same_endian_type>& ss)
{
	int count = 0;
	ss >> count;
	if (count == 0)
		return;

	std::vector<VtSystemType> systypeVec;
	VtSystemType sysType;
	for (int i = 0; i < count; ++i) {
		ss >> sysType;
		systypeVec.push_back(sysType);
	}

	for (auto it = systypeVec.begin(); it != systypeVec.end(); ++it) {
		SharedSystem sys = std::make_shared<VtSystem>(*it);
		sys->Load(ss);
		sys->ReadExtraArgs();
		sys->RegisterRealtimeAccountEvent();
		AddSystem(sys);
	}
}
