#pragma once
#include "Global/TemplateSingleton.h"
#include <string>
#include "cpptime.h"
#include <vector>
#include <map>
#include "Global/VtDefine.h"
class VtChartData;
class VtSymbol;
class VtChartDataCollector : public TemplateSingleton<VtChartDataCollector>
{
public:
	VtChartDataCollector();
	~VtChartDataCollector();
	void OnData(std::string code, double value);
	void OnData(std::string code, int value);
	void OnSingleData(std::string code, std::string time, int value);
	VtChartData* FindAddChartData(std::string symCode);
	void OnTimer(int timerTime, VtChartData* chartData);
	void OnTimerEvent(VtChartData* chartData);
	void OnTimer();
	static int GetLocalDate();
	static int GetLocalTime();
	static int GetLocalTime(VtTime givenTime, int cycle);
	static int GetLocalTime(int cycle);
	static int GetHourMin(int curTime);
	static int GetHourMin(int curTime, int cycle);
	void SetChartData(VtChartData* chartData, int time, double value);
	void OnReceiveData(std::string symCode, int time, double value);
	void RegisterChartData(std::string symCode);
	void RegisterChartData(VtChartData* chartData);
	void RegisterProduct(VtSymbol* sym);
private:
	void AddSymbol(std::string code);
	CppTime::Timer _Timer;
	std::map<CppTime::timer_id, CppTime::timer_id> _TimerIdMap;
	CppTime::timer_id _TimerId;
	std::map<std::string, VtChartData*> _ChartMap;
};

