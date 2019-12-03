#pragma once
#include <vector>
class SmChartDataSource
{
public:
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

