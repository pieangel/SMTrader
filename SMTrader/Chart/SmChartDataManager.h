#pragma once
#include <string>
#include <map>
#include "../Global/TemplateSingleton.h"
#include "SmChartDefine.h"
#include "../cpptime.h"
class SmChartData;
class SmChartDataManager : public TemplateSingleton<SmChartDataManager>
{
public:
	SmChartDataManager();
	~SmChartDataManager();
	SmChartData* AddChartData(SmChartDataRequest data_req);
	void AddChartData(SmChartData* chart_data);
	SmChartData* FindChartData(std::string data_key);
	SmChartData* AddChartData(SmChartDataItem data_item);
	SmChartData* AddChartData(std::string symbol_code, int chart_type, int cycle);
	void RegisterTimer(SmChartData* chartData);
private:
	std::map<std::string, SmChartData*> _ChartDataMap;
	CppTime::Timer _Timer;
	std::map<std::string, CppTime::timer_id> _CycleDataReqTimerMap;
};

