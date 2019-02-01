#include "stdafx.h"
#include "VtSystemGroup.h"
#include "System/VtSystemManager.h"
#include "System/VtSystem.h"
VtSystemGroup::VtSystemGroup()
{
}


VtSystemGroup::~VtSystemGroup()
{
}

VtSystem* VtSystemGroup::AddSystem(VtSystemType sysType)
{
	VtSystemManager* sysMgr = VtSystemManager::GetInstance();
	VtSystem* sys = sysMgr->CreateSystem(sysType);
	_SystemMap[sys->Name()] = sys;
	_SystemList.push_back(sys);

	return sys;
}

void VtSystemGroup::ClearSystems()
{
	_SystemMap.clear();
	_SystemList.clear();
}

void VtSystemGroup::Save(simple::file_ostream<same_endian_type>& ss)
{
	size_t count = _SystemList.size();
	ss << _Name; // 그룹 이름
	ss << (int)_Type; // 그룹 타입
	ss << count; // 그룹에 속한 시스템 갯수
	// 시스템 타입만 먼저 저장한다.
	for (auto it = _SystemList.begin(); it != _SystemList.end(); ++it) {
		VtSystem* sys = *it;
		ss << (int)sys->SystemType();
	}

	// 각 시스템을 저장한다.
	for (auto it = _SystemList.begin(); it != _SystemList.end(); ++it) {
		VtSystem* sys = *it;
		sys->Save(ss);
	}
}

void VtSystemGroup::Load(simple::file_istream<same_endian_type>& ss)
{
	ss >> _Name;
	ss >> _Type;
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
		VtSystem* sys = AddSystem(*it);
		sys->Load(ss);
	}
}
