#include "stdafx.h"
#include "SmSystemPositionManager.h"


SmSystemPositionManager::SmSystemPositionManager()
{
}


SmSystemPositionManager::~SmSystemPositionManager()
{
}

void SmSystemPositionManager::AddPosition(std::tuple<std::string, std::string, std::string> key, VtPosition* position)
{
	if (!position)
		return;
	_PositionMap[key] = position;
}

VtPosition* SmSystemPositionManager::FindPosition(std::tuple<std::string, std::string, std::string> key)
{
	auto it = _PositionMap.find(key);
	if (it != _PositionMap.end()) {
		return it->second;
	}
	else {
		return nullptr;
	}
}
