#pragma once
#include <map>
#include "Global/TemplateSingleton.h"

class VtChartFrm;
class VtChartManager : public TemplateSingleton<VtChartManager>
{
public:
	VtChartManager();
	~VtChartManager();
	std::map<int, VtChartFrm*> ChartMap;
	void AddChartFrm(VtChartFrm* chart);
	VtChartFrm* FindChartFrm(int id);
	VtChartFrm* ActiveChartFrm() const { return _ActiveChartFrm; }
	void ActiveChartFrm(VtChartFrm* val) { _ActiveChartFrm = val; }
private:
	VtChartFrm* _ActiveChartFrm = nullptr;
};

