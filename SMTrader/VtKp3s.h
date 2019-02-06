#pragma once

// Future for us dollars
#include "System/VtSystem.h"
#include "Xml/pugixml.hpp"
class VtChartData;
/// <summary>
/// ����, ������ ƽ�� ���� ƽ���� %�� ���԰��ݿ� �Է°��� ������ 100
/// Ʈ���ϸ���ž�� �ִ����(���� ����)���� �и��� %�� û����
/// </summary>
class VtKp3s : public VtSystem
{
public:
	VtKp3s();
	VtKp3s(VtSystemType type);
	VtKp3s(VtSystemType systemType, std::string name);
	virtual ~VtKp3s();

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
