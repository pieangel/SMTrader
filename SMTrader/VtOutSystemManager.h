#pragma once
#include "Global/TemplateSingleton.h"
#include <map>
#include "VtOutSysDef.h"

class VtOutSystemManager : public TemplateSingleton<VtOutSystemManager>
{
public:
	void AddSystem(SharedSystem sys);
	void RemoveSystem(int sysId);
	VtOutSystemManager();
	~VtOutSystemManager();
	SharedSystemVec& GetSysMap()
	{
		return _SystemVec;
	}
private:
	SharedSystemVec _SystemVec;
};

