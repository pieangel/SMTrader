#pragma once
#include "Global/TemplateSingleton.h"
#include "VtOutSysDef.h"
class VtOutSystemOrderManager : public TemplateSingleton<VtOutSystemOrderManager>
{
public:
	VtOutSystemOrderManager();
	~VtOutSystemOrderManager();
	void AddSystem(SharedSystem sys);
	void RemoveSystem(int sysId);
	SharedSystemVec& GetSysVec()
	{
		return _SystemVec;
	}
private:
	SharedSystemVec _SystemVec;
};

