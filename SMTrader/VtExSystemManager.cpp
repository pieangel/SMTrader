#include "stdafx.h"
#include "VtExSystemManager.h"
#include "System/VtSystem.h"

void VtExSystemManager::AddSystem(VtSystem* sys)
{
	if (!sys)
		return;
	_SystemMap[sys->Id()] = sys;
}

VtExSystemManager::VtExSystemManager()
{
}


VtExSystemManager::~VtExSystemManager()
{
}
