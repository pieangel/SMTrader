#pragma once
#include "VtSystem.h"
#include "../Xml/pugixml.hpp"
class VtChartData;
class VtMaSystem :
	public VtSystem
{
public:
	VtMaSystem(VtSystemType systemType);
	VtMaSystem(VtSystemType systemType, std::string name);
	VtMaSystem();
	virtual void InitArgs();
	virtual ~VtMaSystem();
	virtual void CreateSignal(int startIndex, int endIndex);
	virtual VtPositionType UpdateSignal(int index);
	virtual void ReloadSystem(int startIndex, int endIndex);
	virtual VtPositionType UpdateSignal(int start, int end);
	virtual void SetDataMap(VtChartData* chartData);
	int ShortPeriod;
	int LongPeriod;
	int BigPeriod;

	int AtrPeriod;
	int AtrMul;
	int BarsSinceE;

	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
};

