#include "stdafx.h"
#include "VtSystemArgGroup.h"


VtSystemArgGroup::VtSystemArgGroup()
{
}


VtSystemArgGroup::~VtSystemArgGroup()
{
}

void VtSystemArgGroup::AddSystemArg(std::string name, VtSystemArg arg)
{
	arg.Name = name;
	_ArgMap.push_back(arg);
}

VtSystemArg* VtSystemArgGroup::GetSystemArg(std::string name)
{
	for (auto it = _ArgMap.begin(); it != _ArgMap.end(); ++it) {
		VtSystemArg& arg = *it;
		if (arg.Name.compare(name) == 0) {
			return &*it;
		}
	}

	return nullptr;
}

void VtSystemArgGroup::Save(simple::file_ostream<same_endian_type>& ss)
{
	ss << _Name;
	ss << _ArgMap.size();
	for (auto it = _ArgMap.begin(); it != _ArgMap.end(); ++it) {
		VtSystemArg& arg = *it;
		ss << arg.bValue;
		ss << arg.Desc;
		ss << arg.dValue;
		ss << arg.Enable;
		ss << arg.iValue;
		ss << arg.Name;
		ss << arg.sValue;
		ss << arg.Type;
	}
}

void VtSystemArgGroup::Load(simple::file_istream<same_endian_type>& ss)
{
	ss >> _Name;
	size_t argCount = 0;
	ss >> argCount;
	if (argCount == 0)
		return;
	for (size_t i = 0; i < argCount; ++i) {
		VtSystemArg arg;
		ss >> arg.bValue;
		ss >> arg.Desc;
		ss >> arg.dValue;
		ss >> arg.Enable;
		ss >> arg.iValue;
		ss >> arg.Name;
		ss >> arg.sValue;
		ss >> arg.Type;
		AddSystemArg(arg.Name, arg);
	}
}

void VtSystemArgGroup::SaveToXml(pugi::xml_node& node_argument_group)
{
	pugi::xml_node argument_group_child = node_argument_group.append_child("name");
	argument_group_child.append_child(pugi::node_pcdata).set_value(_Name.c_str());
	
	if (_ArgMap.size() > 0) {
		pugi::xml_node argument_list = node_argument_group.append_child("argument_list");
		for (auto it = _ArgMap.begin(); it != _ArgMap.end(); ++it) {
			VtSystemArg& arg = *it;
			pugi::xml_node argument_child = argument_list.append_child("argument");
			argument_child.append_attribute("bvalue") = arg.bValue;
			argument_child.append_attribute("desc") = arg.Desc.c_str();
			argument_child.append_attribute("dvalue") = arg.dValue;
			argument_child.append_attribute("enable") = arg.Enable;
			argument_child.append_attribute("ivalue") = arg.iValue;
			argument_child.append_attribute("name") = arg.Name.c_str();
			argument_child.append_attribute("svalue") = arg.sValue.c_str();
			argument_child.append_attribute("type") = (int)arg.Type;
		}
	}
}

void VtSystemArgGroup::LoadFromXml(pugi::xml_node& node_argument_group)
{
	_Name = node_argument_group.child_value("name");
	pugi::xml_node argument_list_node = node_argument_group.child("argument_list");
	if (argument_list_node) {
		for (pugi::xml_node argument_node = argument_list_node.child("argument"); argument_node; argument_node = argument_node.next_sibling("argument")) {
			VtSystemArg arg;
			arg.bValue = argument_node.attribute("bvalue").as_bool();
			arg.Desc = argument_node.attribute("desc").as_string();
			arg.dValue = argument_node.attribute("dvalue").as_double();
			arg.Enable = argument_node.attribute("enable").as_bool();
			arg.iValue = argument_node.attribute("ivalue").as_int();
			arg.Name = argument_node.attribute("name").as_string();
			arg.sValue = argument_node.attribute("svalue").as_string();
			arg.Type = (VtParamType)argument_node.attribute("type").as_int();
			AddSystemArg(arg.Name, arg);
		}
	}
}
