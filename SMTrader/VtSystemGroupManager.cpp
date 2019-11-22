#include "stdafx.h"
#include "VtSystemGroupManager.h"
#include "System/VtSystemManager.h"
#include "System/VtSystem.h"
#include "Global/VtDefine.h"

VtSystemGroupManager::VtSystemGroupManager()
{
	//InitSystemGroup();
}


VtSystemGroupManager::~VtSystemGroupManager()
{
}

VtSystemGroup* VtSystemGroupManager::GetSystemGroup(std::string catName)
{
	for (auto it = _SystemGroupVec.begin(); it != _SystemGroupVec.end(); ++it) {
		VtSystemGroup& sg = *it;
		if (sg.Name().compare(catName) == 0) {
			return &*it;
		}
	}

	return nullptr;
}

VtSystemGroup* VtSystemGroupManager::GetSystemGroup(VtSystemGroupType sysGroupType)
{
	for (auto it = _SystemGroupVec.begin(); it != _SystemGroupVec.end(); ++it) {
		VtSystemGroup& sg = *it;
		if (sg.Type() == sysGroupType) {
			return &*it;
		}
	}

	return nullptr;
}

void VtSystemGroupManager::Save(simple::file_ostream<same_endian_type>& ss)
{
	size_t count = _SystemGroupVec.size();
	ss << count; // 그룹 갯수
	for (auto it = _SystemGroupVec.begin(); it != _SystemGroupVec.end(); ++it) {
		VtSystemGroup& sysGrp = *it;
		sysGrp.Save(ss);
	}
}

void VtSystemGroupManager::Load(simple::file_istream<same_endian_type>& ss)
{
	size_t count = 0;
	ss >> count;
	for (size_t i = 0; i < count; ++i) {
		VtSystemGroup sysGrp;
		sysGrp.Load(ss);
		_SystemGroupVec.push_back(sysGrp);
	}
}

void VtSystemGroupManager::InitSystemGroup()
{
	VtSystemGroup sysGrp;
	
	sysGrp.Name(_T("주가지수선물"));
	sysGrp.Type(VtSystemGroupType::KOSPI200F);
	sysGrp.AddSystem(VtSystemType::KP_1A);
	sysGrp.AddSystem(VtSystemType::KP_2S);
	sysGrp.AddSystem(VtSystemType::KP_3S);
	sysGrp.AddSystem(VtSystemType::KP_4B);
	sysGrp.AddSystem(VtSystemType::KP_5IN1);
	sysGrp.AddSystem(VtSystemType::KP_6IN2);
	_SystemGroupVec.push_back(sysGrp);

	sysGrp.ClearSystems();
	sysGrp.Type(VtSystemGroupType::KOSPI200O);
	sysGrp.Name(_T("주가지수옵션"));
	sysGrp.AddSystem(VtSystemType::KO_3S);
	sysGrp.AddSystem(VtSystemType::KO_4B);
	_SystemGroupVec.push_back(sysGrp);

	sysGrp.ClearSystems();
	sysGrp.Name(_T("코스닥150선물"));
	sysGrp.Type(VtSystemGroupType::KOSDAQ150F);
	sysGrp.AddSystem(VtSystemType::Q_IN1);
	sysGrp.AddSystem(VtSystemType::Q_IN2);
	sysGrp.AddSystem(VtSystemType::Q_IN3);
	sysGrp.AddSystem(VtSystemType::Q_IN4);
	sysGrp.AddSystem(VtSystemType::Q_H2);
	sysGrp.AddSystem(VtSystemType::Q_F2);
	sysGrp.AddSystem(VtSystemType::Q_CH);
	_SystemGroupVec.push_back(sysGrp);

	sysGrp.ClearSystems();
	sysGrp.Type(VtSystemGroupType::USDF);
	sysGrp.Name(_T("원달러선물"));
	
	sysGrp.AddSystem(VtSystemType::TR_US1);
	sysGrp.AddSystem(VtSystemType::TR_US2);
	sysGrp.AddSystem(VtSystemType::TR_US3);
	sysGrp.AddSystem(VtSystemType::US_IN1);
	sysGrp.AddSystem(VtSystemType::US_IN2);
	sysGrp.AddSystem(VtSystemType::US_IN3);
	sysGrp.AddSystem(VtSystemType::US_I2);
	sysGrp.AddSystem(VtSystemType::US_Comp);
	sysGrp.AddSystem(VtSystemType::US_H2);
	
	_SystemGroupVec.push_back(sysGrp);
}

void VtSystemGroupManager::SaveToXml(pugi::xml_node& node_system_group_list)
{
	for (auto it = _SystemGroupVec.begin(); it != _SystemGroupVec.end(); ++it) {
		VtSystemGroup& sysGrp = *it;
		pugi::xml_node node_system_group = node_system_group_list.append_child("system_group");
		sysGrp.SaveToXml(node_system_group);
	}
}

void VtSystemGroupManager::LoadFromXml(pugi::xml_node& node)
{

}
