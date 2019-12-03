#pragma once
#include <string>
#include "SmChartDefine.h"
#include <vector>
#include <set>
#include <list>
#include <map>
class VtSymbol;
class SmChartData
{
private:
	// ��Ʈ�����͸� �̹� �޾����� ǥ���Ѵ�.
	bool _Received = false;
	std::string _SymbolCode;
	SmChartType _ChartType = SmChartType::MIN;
	int _Cycle = 0;
	void GetChartDataFromDB();
	size_t _DataQueueSize = SMChartDataSize;
	size_t _CycleDataSize = 3;
	std::multimap<std::string, SmChartDataItem> _DataMap;
public:
	void GetChartDataFromServer();
	void GetCyclicDataFromServer();
	std::vector<double> GetOpenVector();
	std::vector<double> GetCloseVector();
	std::vector<double> GetHighVector();
	std::vector<double> GetLowVector();
	std::vector<double> GetDateTimeVector();
	std::vector<double> GetVolumeVector();
	void UpdateLastValue(int close);
	std::multimap<std::string, SmChartDataItem>& GetDataMap() {
		return _DataMap;
	}
	size_t GetDataCount() {
		return _DataMap.size();
	}
	void AddData(SmChartDataItem& data_item);
	static std::string MakeDataKey(std::string symbol_code, int chart_type, int cycle) {
		std::string key = symbol_code;
		key.append(":");
		key.append(std::to_string(chart_type));
		key.append(":");
		key.append(std::to_string(cycle));
		return key;
	}
	
	// ���ð�, ����Ŭ
	std::pair<int, int> GetCycleByTimeDif();
	
	std::string SymbolCode() const { return _SymbolCode; }
	void SymbolCode(std::string val) { _SymbolCode = val; }
	SmChartType ChartType() const { return _ChartType; }
	void ChartType(SmChartType val) { _ChartType = val; }
	int Cycle() const { return _Cycle; }
	void Cycle(int val) { _Cycle = val; }
	// Ÿ�̸Ӱ� �Ҹ��� ȣ�� �Ǵ� �Լ�
	/// <summary>
	/// Ÿ�̸Ӱ� �Ҹ� �� ����� ����� �����Ͽ� ��Ʈ �����͸� �����Ͽ� Ŭ���̾�Ʈ���� �����Ѵ�.
	/// </summary>
	void OnTimer();
	std::string GetDataKey() {
		std::string key = _SymbolCode;
		key.append(":");
		key.append(std::to_string((int)_ChartType));
		key.append(":");
		key.append(std::to_string(_Cycle));
		return key;
	}
	int DataQueueSize() const { return _DataQueueSize; }
	void DataQueueSize(int val) { _DataQueueSize = val; }
	size_t CycleDataSize() const { return _CycleDataSize; }
	void CycleDataSize(size_t val) { _CycleDataSize = val; }
	bool Received() const { return _Received; }
	void Received(bool val) { _Received = val; }
};

