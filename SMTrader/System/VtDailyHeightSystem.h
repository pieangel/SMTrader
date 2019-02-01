#pragma once
#include "VtSystem.h"
#include "../Xml/pugixml.hpp"
class VtChartData;
class VtDailyHeightSystem :
	public VtSystem
{
public:

	VtDailyHeightSystem(VtSystemType systemType);
	VtDailyHeightSystem();
	VtDailyHeightSystem(VtSystemType systemType, std::string name);
	virtual ~VtDailyHeightSystem();

	virtual void InitArgs();
	virtual void CreateSignal(int startIndex, int endIndex);
	virtual VtPositionType UpdateSignal(int index);
	virtual void ReloadSystem(int startIndex, int endIndex);
	virtual VtPositionType UpdateSignal(int start, int end);
	virtual void SetDataMap(VtChartData* chartData);

	int Dividance;
	int StartHour;
	int AtrPeriod;
	int AtrMul;

	int BarsSinceE;
	int EntryToday;
	int ExitPercent;
	int BackLen;

	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
};

