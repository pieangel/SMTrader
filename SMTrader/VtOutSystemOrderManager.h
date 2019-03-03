#pragma once
#include "Global/TemplateSingleton.h"
#include "VtOutSysDef.h"
#include <string>
#include <map>

class VtOutSystemOrderManager : public TemplateSingleton<VtOutSystemOrderManager>
{
public:
	VtOutSystemOrderManager();
	~VtOutSystemOrderManager();
	void AddSystem(SharedSystem sys);
	void RemoveSystem(SharedSystem sys);
	SharedSystemVec& GetSysVec()
	{
		return _SystemVec;
	}
private:
	void AddSignalOrder(SharedSystem sys);
	void RemoveSignalOrder(SharedSystem sys);
	SharedSystemVec _SystemVec;
	std::map<std::string, SharedSystemMap> _SignalOrderMap;
};

