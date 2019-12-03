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
	// 차트데이터를 이미 받았음을 표시한다.
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
	
	// 대기시간, 사이클
	std::pair<int, int> GetCycleByTimeDif();
	
	std::string SymbolCode() const { return _SymbolCode; }
	void SymbolCode(std::string val) { _SymbolCode = val; }
	SmChartType ChartType() const { return _ChartType; }
	void ChartType(SmChartType val) { _ChartType = val; }
	int Cycle() const { return _Cycle; }
	void Cycle(int val) { _Cycle = val; }
	// 타이머가 불릴때 호출 되는 함수
	/// <summary>
	/// 타이머가 불릴 때 사용자 목록을 참조하여 차트 데이터를 조작하여 클라이언트에게 전송한다.
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

