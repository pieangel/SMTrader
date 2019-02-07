#pragma once

// Future for us dollars
#include "System/VtSystem.h"
#include "Xml/pugixml.hpp"
class VtChartData;
/// <summary>
/// 손절, 익절은 틱은 원래 틱수로 %는 진입가격에 입력가격 나누기 100
/// 트레일링스탑은 최대수익(실제 돈임)에서 밀리는 %에 청산함
/// </summary>
class VtKp4b : public VtSystem
{
public:
	VtKp4b();
	VtKp4b(VtSystemType type);
	VtKp4b(VtSystemType systemType, std::string name);
	virtual ~VtKp4b();

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

	virtual bool CheckEntranceForBuyForKospi();
	virtual bool CheckEntranceForSellForKospi();
	virtual bool CheckLiqForSellForKospi();
	virtual bool CheckLiqForBuyForKospi();

	virtual bool CheckEntranceForBuyForKospi(size_t index);
	virtual bool CheckEntranceForSellForKospi(size_t index);
	virtual bool CheckLiqForSellForKospi(size_t index);
	virtual bool CheckLiqForBuyForKospi(size_t index);
};
