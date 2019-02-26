#pragma once
#include "../Global/VtDefine.h"
#include "../Global/TemplateSingleton.h"
#include <string>
#include <functional>
#include <vector>
#include <map>
#include "../cpptime.h"
#include "../VtSystemDef.h"
class VtSystem;
class VtChartData;
class VtUsdStrategyConfigDlg;
class VtSymbol;
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
	void OnRegularTimer();
	std::map<std::string, CppTime::timer_id> _TimerMap;
	void InitDataSources();
	void UpdateRealtimeArgs(VtChartData* chartData);
	void AddSystemDialog(VtUsdStrategyConfigDlg* dlg);
	void RemoveSystemDialog(VtUsdStrategyConfigDlg* dlg);
	double Kbs = 0.0;
	double Kas = 0.0;
	double Kbc = 0.0;
	double Kac = 0.0;
	double Qbs = 0.0;
	double Qas = 0.0;
	double Qbc = 0.0;
	double Qac = 0.0;
	double Ubs = 0.0;
	double Uas = 0.0;
	double Ubc = 0.0;
	double Uac = 0.0;
private:
	VtChartData*  AddDataSource(std::string symCode, VtChartType type, int cycle);
	void InitDataSource(int cycle);
	void RemoveTimer(VtSystem* sys);
	std::map<std::string, VtSystem*> _SystemMap;
	std::vector<VtSystem*> _SystemVector;
	std::hash<std::string> _HashMaker;
	bool _CreatedSystemList;
	CppTime::Timer _Timer;
	std::map<std::string, VtSymbol*> _ArgMap;
	std::map<VtUsdStrategyConfigDlg*, VtUsdStrategyConfigDlg*> _SysDlgMap;
};

