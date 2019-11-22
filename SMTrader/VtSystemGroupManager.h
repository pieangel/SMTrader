#pragma once
#include "Global/TemplateSingleton.h"
#include <map>
#include <string>
#include <vector>
#include "VtSystemGroup.h"
#include "VtSystemDef.h"
#include "SimpleBinStream.h"
#include "Xml/pugixml.hpp"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;

class VtSystemGroupManager : public TemplateSingleton<VtSystemGroupManager>
{
public:
	VtSystemGroupManager();
	~VtSystemGroupManager();
	std::vector<VtSystemGroup>& GetSystemGroupVec() {
		return _SystemGroupVec;
	}
	VtSystemGroup* GetSystemGroup(std::string catName);
	VtSystemGroup* GetSystemGroup(VtSystemGroupType sysGroupType);

	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
	
	void InitSystemGroup();

	void SaveToXml(pugi::xml_node& node);
	void LoadFromXml(pugi::xml_node& node);
private:
	std::vector<VtSystemGroup> _SystemGroupVec;
};

