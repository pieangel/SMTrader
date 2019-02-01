#pragma once
#include "VtSystem.h"
#include "../Xml/pugixml.hpp"
class VtChartData;
class VtOsillaterSystem :
	public VtSystem
{
public:
	VtOsillaterSystem();
	virtual ~VtOsillaterSystem();

	VtOsillaterSystem(VtSystemType systemType);
	VtOsillaterSystem(VtSystemType systemType, std::string name);

	virtual void InitArgs();
	virtual void CreateSignal(int startIndex, int endIndex);
	virtual VtPositionType UpdateSignal(int index);
	virtual void ReloadSystem(int startIndex, int endIndex);
	virtual VtPositionType UpdateSignal(int start, int end);
	virtual void SetDataMap(VtChartData* chartData);
	int ShortPeriod;
	int LongPeriod;
	int SignalPeriod;

	int FastK;
	int SlowK;
	int SlowD;

	int AtrPeriod;
	int AtrMul;
	int BarsSinceE;

	int ExitMA;

	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
};

