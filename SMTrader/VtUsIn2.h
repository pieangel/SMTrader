#pragma once
// Future for us dollars
#include "System/VtSystem.h"
#include "Xml/pugixml.hpp"
class VtChartData;
class VtUsIn2 : public VtSystem
{
public:
	VtUsIn2();
	VtUsIn2(VtSystemType type);
	VtUsIn2(VtSystemType systemType, std::string name);
	virtual ~VtUsIn2();

	virtual void SetDataSrc();
	virtual void InitArgs();
	virtual void CreateSignal(int startIndex, int endIndex);
	virtual VtPositionType UpdateSignal(int index);
	virtual void ReloadSystem(int startIndex, int endIndex);
	virtual VtPositionType UpdateSignal(int start, int end);
	virtual void SetDataMap(VtChartData* chartData);
	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
	virtual void OnTimer();
	virtual void UpdateSystem(int index);
	virtual void ReadExtraArgs();

	virtual bool CheckEntranceForBuyForUsd();
	virtual bool CheckEntranceForSellForUsd();
	virtual bool CheckLiqForSellForUsd();
	virtual bool CheckLiqForBuyForUsd();

	virtual bool CheckEntranceForBuyForUsd(size_t index);
	virtual bool CheckEntranceForSellForUsd(size_t index);
	virtual bool CheckLiqForSellForUsd(size_t index);
	virtual bool CheckLiqForBuyForUsd(size_t index);
};

