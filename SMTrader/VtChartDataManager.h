#pragma once
#include "Global/TemplateSingleton.h"
#include <string>
#include <map>
#include "Global/VtDefine.h"
#include "cpptime.h"
#include "VtChartDataRequest.h"

class VtChartData;
class VtSymbol;
class VtChartDataManager : public TemplateSingleton<VtChartDataManager>
{
public:
	VtChartDataManager();
	~VtChartDataManager();
private:
	std::map<std::string, std::vector<VtChartData*>> _SymbolChartMap;
	std::map<std::string, VtChartData*> _ChartData;
	std::map<std::string, CppTime::timer_id> _RequestMap;
	void AddChartData(std::string symCode, VtChartData* data);
public:
	VtChartData* Find(std::string key);
	VtChartData* FindNAdd(std::string symCode, std::string cycleType, std::string cycle);
	VtChartData* FindNAdd(std::string symCode, VtChartType type, int cycle);
	std::pair<bool, VtChartData*> Add(std::string symCode, VtChartType type, int cycle);
	VtChartData* Add(VtChartDataRequest req);
	int GetChartDataSize()
	{
		return ChartDataSize;
	}
	CppTime::Timer& GetTimer()
	{
		return _Timer;
	}
	void AddChartDataRequest(int startAfter, int interval, VtChartData* chartData);
	void AddChartDataRequest(int interval, VtChartData* chartData);
	void RemoveChartDataRequest(VtChartData* chartData);
	void OnReceiveQuoteHd(VtSymbol* sym);
	void OnReceiveChartData(VtChartData* data);
	void OnReceiveFirstChartData(VtChartData* data);

	void OnTimer();

	static std::string MakeChartDataKey(std::string exchange, std::string code, VtChartType chartType, int cycle);
	static std::string MakeChartDataKey(VtChartDataRequest req);
	static std::string MakeChartDataKey(std::string code, VtChartType chartType, int cycle);

	void FindDataBorderQueue(VtChartData* data, double* timeInfoArray);
private:
	CppTime::Timer _Timer;
};

