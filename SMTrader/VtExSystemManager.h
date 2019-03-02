#pragma once
#include "Global/TemplateSingleton.h"
#include <map>

class VtSystem;
class VtExSystemManager : public TemplateSingleton<VtExSystemManager>
{
public:
	void AddSystem(VtSystem* sys);
	VtExSystemManager();
	~VtExSystemManager();
private:
	std::map<int, VtSystem*> _SystemMap;
};

