#pragma once
#include "VtSystem.h"
#include "../Xml/pugixml.hpp"
class VtChartData;
class VtMacdSystem :
	public VtSystem
{
public:
	VtMacdSystem(VtSystemType systemType);
	VtMacdSystem(VtSystemType systemType, std::string name);
	VtMacdSystem();
	virtual void InitArgs();
	virtual ~VtMacdSystem();
	virtual void CreateSignal(int startIndex, int endIndex);
	virtual VtPositionType UpdateSignal(int index);
	virtual void ReloadSystem(int startIndex, int endIndex);
	virtual VtPositionType UpdateSignal(int start, int end);
	virtual void SetDataMap(VtChartData* chartData);
	int ShortPeriod;
	int LongPeriod;
	int SignalPeriod;
	int BarsSinceE;
	int ExitMA;

	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
};

