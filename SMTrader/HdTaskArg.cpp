#include "stdafx.h"
#include "HdTaskArg.h"


void HdTaskArg::AddArg(std::string name, std::string value)
{
	ArgMap[name] = value;
}

std::string HdTaskArg::GetArg(std::string name)
{
	std::string value;
	value = _T("");
	auto it = ArgMap.find(name);
	if (it != ArgMap.end())
	{
		return ArgMap[name];
	}
	else
	{
		return value;
	}
}

