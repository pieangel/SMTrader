#pragma once
// Future for us dollars
#include "System/VtSystem.h"
#include "Xml/pugixml.hpp"
class VtChartData;
/// <summary>
/// ����, ������ ƽ�� ���� ƽ���� %�� ���԰��ݿ� �Է°��� ������ 100
/// Ʈ���ϸ���ž�� �ִ����(���� ����)���� �и��� %�� û����
/// </summary>
class VtUsComp : public VtSystem
{
public:
	VtUsComp();
	VtUsComp(VtSystemType type);
	VtUsComp(VtSystemType systemType, std::string name);
	virtual ~VtUsComp();

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

	virtual bool CheckEntranceForBuy();
	virtual bool CheckEntranceForSell();
	virtual bool CheckLiqForSell();
	virtual bool CheckLiqForBuy();

	virtual bool CheckEntranceForBuy(size_t index);
	virtual bool CheckEntranceForSell(size_t index);
	virtual bool CheckLiqForSell(size_t index);
	virtual bool CheckLiqForBuy(size_t index);
};

