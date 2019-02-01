#include "stdafx.h"
#include "VtChartManager.h"
#include "VtChartFrm.h"

VtChartManager::VtChartManager()
{
}


VtChartManager::~VtChartManager()
{
}

void VtChartManager::AddChartFrm(VtChartFrm* chart)
{
	if (!chart)
		return;

	ChartMap[chart->Id()] = chart;
}

VtChartFrm* VtChartManager::FindChartFrm(int id)
{
	auto it = ChartMap.find(id);
	if (it != ChartMap.end())
	{
		return it->second;
	}

	return nullptr;
}
