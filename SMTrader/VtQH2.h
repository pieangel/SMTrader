#pragma once

// Future for us dollars
#include "System/VtSystem.h"
#include "Xml/pugixml.hpp"
class VtChartData;
/// <summary>
/// 손절, 익절은 틱은 원래 틱수로 %는 진입가격에 입력가격 나누기 100
/// 트레일링스탑은 최대수익(실제 돈임)에서 밀리는 %에 청산함
/// </summary>
class VtQH2 : public VtSystem
{
public:
	VtQH2();
	VtQH2(VtSystemType type);
	VtQH2(VtSystemType systemType, std::string name);
	virtual ~VtQH2();

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

	bool CheckEntranceForBuy();
	bool CheckEntranceForSell();
	bool CheckLiqForSell();
	bool CheckLiqForBuy();
	VtPositionType CheckEntranceForBuy(size_t index);
	VtPositionType CheckEntranceForSell(size_t index);
	VtPositionType CheckLiqForSell(size_t index);
	VtPositionType CheckLiqForBuy(size_t index);
};

