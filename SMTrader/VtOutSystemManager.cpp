#include "stdafx.h"
#include "VtOutSystemManager.h"
#include "System/VtSystem.h"
#include <algorithm>

VtPriceType VtOutSystemManager::PriceType = VtPriceType::Price;
int VtOutSystemManager::OrderTick = 5;

void VtOutSystemManager::SaveToXml(pugi::xml_node& external_system_list_node)
{
	external_system_list_node.append_attribute("price_type") = (int)VtOutSystemManager::PriceType;
	external_system_list_node.append_attribute("order_tick") = VtOutSystemManager::OrderTick;
	
	if (_SystemVec.size() > 0) {
		pugi::xml_node system_list_child = external_system_list_node.append_child("system_list");
		for (auto it = _SystemVec.begin(); it != _SystemVec.end(); ++it) {
			SharedSystem sys = *it;
			pugi::xml_node node_system = system_list_child.append_child("system");
			sys->SaveToXml(node_system);
		}
	}
}

void VtOutSystemManager::LoadFromXml(pugi::xml_node& node)
{

}

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
	ss << (int)VtOutSystemManager::PriceType;
	ss << VtOutSystemManager::OrderTick;

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
	ss >> VtOutSystemManager::PriceType;
	ss >> VtOutSystemManager::OrderTick;

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
