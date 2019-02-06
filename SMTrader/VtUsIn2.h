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

	bool CheckEntranceForBuy();
	bool CheckEntranceForSell();
	bool CheckLiqForSell();
	bool CheckLiqForBuy();
	bool CheckEntranceByBandForBuy();
	bool CheckEntranceByBandForSell();
	bool CheckEntranceByOpenForBuy();
	bool CheckEntranceByOpenForSell();

	bool CheckEntranceForBuy(size_t index);
	bool CheckEntranceForSell(size_t index);
	bool CheckLiqForSell(size_t index);
	bool CheckLiqForBuy(size_t index);

	bool CheckEntranceByBandForBuy(size_t index);
	bool CheckEntranceByBandForSell(size_t index);
	bool CheckEntranceByOpenForBuy(size_t index);
	bool CheckEntranceByOpenForSell(size_t index);

private:
	double _PreHL = 0.0;
	double _Band = 0.0;
};

