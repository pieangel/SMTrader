#pragma once
#include "VtSystem.h"
#include "../Xml/pugixml.hpp"
class VtChartData;
class VtChannelJumpSystem :
	public VtSystem
{
public:
	virtual ~VtChannelJumpSystem();
	VtChannelJumpSystem();
	VtChannelJumpSystem(VtSystemType systemType, std::string name);
	VtChannelJumpSystem(VtSystemType type);

	virtual void InitArgs();
	virtual void CreateSignal(int startIndex, int endIndex);
	virtual VtPositionType UpdateSignal(int index);
	virtual void ReloadSystem(int startIndex, int endIndex);
	virtual VtPositionType UpdateSignal(int start, int end);
	virtual void SetDataMap(VtChartData* chartData);
	int ChannelPeriod;
	int AtrPeriod;
	int AtrMul;
	int MaPeriod;
	int BarsSinceE;

	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
	
private:
	int _UpCrossCount = 0;
	int _DownCrossCount = 0;
};

