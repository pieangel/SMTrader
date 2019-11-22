#pragma once
#include <string>
#include <map>
#include <vector>
#include "Global/VtDefine.h"
#include "VtSystemDef.h"
#include "SimpleBinStream.h"
#include "Xml/pugixml.hpp"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;
class VtSystem;
class VtSystemGroup
{
public:
	VtSystemGroup();
	~VtSystemGroup();

	std::string Name() const { return _Name; }
	void Name(std::string val) { _Name = val; }
	VtSystem* AddSystem(VtSystemType sysType);
	std::vector<VtSystem*>& GetSystemList() {
		return _SystemList;
	}
	void ClearSystems();
	VtSystemGroupType Type() const { return _Type; }
	void Type(VtSystemGroupType val) { _Type = val; }
	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);

	void SaveToXml(pugi::xml_node& node);
	void LoadFromXml(pugi::xml_node& node);
private:
	VtSystemGroupType _Type;
	std::string _Name;
	std::map<std::string, VtSystem*> _SystemMap;
	std::vector<VtSystem*> _SystemList;
};

