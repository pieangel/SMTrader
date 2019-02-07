#pragma once

// Future for us dollars
#include "System/VtSystem.h"
#include "Xml/pugixml.hpp"
class VtChartData;
/// <summary>
/// ����, ������ ƽ�� ���� ƽ���� %�� ���԰��ݿ� �Է°��� ������ 100
/// Ʈ���ϸ���ž�� �ִ����(���� ����)���� �и��� %�� û����
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
