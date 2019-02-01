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
	ss << _Name; // �׷� �̸�
	ss << (int)_Type; // �׷� Ÿ��
	ss << count; // �׷쿡 ���� �ý��� ����
	// �ý��� Ÿ�Ը� ���� �����Ѵ�.
	for (auto it = _SystemList.begin(); it != _SystemList.end(); ++it) {
		VtSystem* sys = *it;
		ss << (int)sys->SystemType();
	}

	// �� �ý����� �����Ѵ�.
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
