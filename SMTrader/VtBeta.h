#pragma once
// Future for us dollars
#include "System/VtSystem.h"
#include "Xml/pugixml.hpp"
class VtChartData;
/// <summary>
/// ����, ������ ƽ�� ���� ƽ���� %�� ���԰��ݿ� �Է°��� ������ 100
/// Ʈ���ϸ���ž�� �ִ����(���� ����)���� �и��� %�� û����
/// </summary>
class VtBeta : public VtSystem
{
public:
	VtBeta();
	VtBeta(VtSystemType type);
	VtBeta(VtSystemType systemType, std::string name);
	virtual ~VtBeta();

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
	VtPositionType CheckEntranceForBuy(size_t index);
	VtPositionType CheckEntranceForSell(size_t index);
	VtPositionType CheckLiqForSell(size_t index);
	VtPositionType CheckLiqForBuy(size_t index);

private:
	double _ATRMulti = 2.0;
	double _BandMulti = 0.25;
	double _FilterMulti = 3.0;

	double _PreHL = 0.0;
	double _Band = 0.0;
	bool _LongTrigger = false;
	bool _ShortTrigger = false;
	bool _xLongTrigger = false;
	bool _xShortTrigger = false;
	bool _LongFilter = false;
	bool _ShortFilter = false;
	int _LastEntryTime = 0;
};

