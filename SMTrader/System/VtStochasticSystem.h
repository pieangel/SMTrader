#pragma once
#include "VtSystem.h"
#include "../Xml/pugixml.hpp"
class VtChartData;
class VtStochasticSystem :
	public VtSystem
{
public:
	VtStochasticSystem(VtSystemType systemType);
	VtStochasticSystem(VtSystemType systemType, std::string name);
	VtStochasticSystem();
	virtual ~VtStochasticSystem();

	virtual void InitArgs();

	virtual void CreateSignal(int startIndex, int endIndex);
	virtual VtPositionType UpdateSignal(int index);
	virtual void ReloadSystem(int startIndex, int endIndex);
	virtual VtPositionType UpdateSignal(int start, int end);
	virtual void SetDataMap(VtChartData* chartData);
	int FastK;
	int SlowK;
	int SlowD;
	int BarsSinceE;
	int ExitMA;

	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
};

