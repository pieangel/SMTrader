#pragma once
#include "Global/VtDefine.h"
struct VtChartDataRequest
{
	VtMainChartType mainChartType = VtMainChartType::CandleStick;
	std::string msgid;
	std::string productCode;
	VtChartType chartType;
	int cycle; 
	int count;
	std::string currency;
	std::string exchange;
	int next;
	int reqKey;
	// 갭보정 0 : 사용안함, 1 : 사용함
	int seq;
	std::string dataKey;
	VtChartRequestType type;
	int timerDuration;
	bool requestAll = true;
	int refCount;
	std::string keyvalue;
	bool domestic;
	int colorIndex = 0;
	std::string initial;
	int decimal = 2;
};

struct VtRealtimeRequest
{
	std::string exc;
	std::string code;
	std::string currency;
	int type = 0;
};