#pragma once
// Future for us dollars
#include "System/VtSystem.h"
#include "Xml/pugixml.hpp"
class VtChartData;
/// <summary>
/// 손절, 익절은 틱은 원래 틱수로 %는 진입가격에 입력가격 나누기 100
/// 트레일링스탑은 최대수익(실제 돈임)에서 밀리는 %에 청산함
/// </summary>
class VtQIn3 : public VtSystem
{
public:
	VtQIn3();
	VtQIn3(VtSystemType type);
	VtQIn3(VtSystemType systemType, std::string name);
	virtual ~VtQIn3();

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

	virtual bool CheckEntranceForBuyForKosdaq();
	virtual bool CheckEntranceForSellForKosdaq();
	virtual bool CheckLiqForSellForKosdaq();
	virtual bool CheckLiqForBuyForKosdaq();

	virtual bool CheckEntranceForBuyForKosdaq(size_t index);
	virtual bool CheckEntranceForSellForKosdaq(size_t index);
	virtual bool CheckLiqForSellForKosdaq(size_t index);
	virtual bool CheckLiqForBuyForKosdaq(size_t index);
};

