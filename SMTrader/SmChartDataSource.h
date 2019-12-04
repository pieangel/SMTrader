#pragma once
#include <vector>
#include <string>
#include "Chart/SmChartDefine.h"
class VtSymbol;
class SmChartData;
class SmChartDataSource
{
public:
	int colorIndex = 0;
	SmChartStyle chartStyle = SmChartStyle::CandleStick;
	VtSymbol* symbol = nullptr;
	SmChartData* source = nullptr;
	SmChartDataSource();
	~SmChartDataSource();
	std::vector<double> open;
	std::vector<double> high;
	std::vector<double> low;
	std::vector<double> close;
	std::vector<double> volume;
	std::vector<double> datetime;
	int GetDataCount() {
		return (int)datetime.size();
	}
};

