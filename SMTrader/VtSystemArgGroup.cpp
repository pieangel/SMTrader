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
