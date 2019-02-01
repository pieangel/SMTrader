#pragma once
#include "../Global/VtDefine.h"
#include "../Global/TemplateSingleton.h"
#include <string>
#include <functional>
#include <vector>
#include <map>
#include "../cpptime.h"
class VtSystem;
class VtChartData;
class VtSystemManager : public TemplateSingleton<VtSystemManager>
{
public:
	VtSystemManager();
	~VtSystemManager();

	VtSystem* CreateSystem(VtSystemType systemType);
	void AddSystem(std::string name, VtSystem* sys)
	{
		_SystemMap[name] = sys;
	}

	void RemoveSystem(std::string name);

	std::map<std::string, VtSystem*>& GetSystemMap()
	{
		return _SystemMap;
	}

	std::vector<VtSystem*>& GetSystemVector()
	{
		return _SystemVector;
	}

	VtSystem* GetSystem(std::string sysName);
	void CreateBundleSystem();
	bool CreatedSystemList() const { return _CreatedSystemList; }
	void CreatedSystemList(bool val) { _CreatedSystemList = val; }
	void SetDataMap(VtChartData* chartData, VtSystem* system);
	void UpdateSystem(int index);
	void OnTimer();
	std::map<std::string, CppTime::timer_id> _TimerMap;
private:
	void RemoveTimer(VtSystem* sys);
	std::map<std::string, VtSystem*> _SystemMap;
	std::vector<VtSystem*> _SystemVector;
	std::hash<std::string> _HashMaker;
	bool _CreatedSystemList;
	CppTime::Timer _Timer;
};

