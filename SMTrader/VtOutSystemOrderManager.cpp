#include "stdafx.h"
#include "VtOutSystemOrderManager.h"
#include "System/VtSystem.h"
#include <algorithm>

VtOutSystemOrderManager::VtOutSystemOrderManager()
{
}


VtOutSystemOrderManager::~VtOutSystemOrderManager()
{
}

void VtOutSystemOrderManager::AddSystem(SharedSystem sys)
{
	if (!sys)
		return;

	_SystemVec.push_back(sys);
	
}

void VtOutSystemOrderManager::RemoveSystem(int sysId)
{
	auto it = std::find_if(_SystemVec.begin(), _SystemVec.end(), [&](const SharedSystem& x) { return x->Id() == sysId; });
	if (it != _SystemVec.end()) {
		_SystemVec.erase(it);
	}
}
