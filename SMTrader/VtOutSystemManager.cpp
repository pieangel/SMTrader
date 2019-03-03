#include "stdafx.h"
#include "VtOutSystemManager.h"
#include "System/VtSystem.h"
#include <algorithm>

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
