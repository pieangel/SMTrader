#include "stdafx.h"
#include "VtChartCtrlManager.h"
#include "VtChartWindow.h"
#include "Resource.h"
VtChartCtrlManager::VtChartCtrlManager()
{
}


VtChartCtrlManager::~VtChartCtrlManager()
{
	for (auto it = ChartCtrlMap.begin(); it != ChartCtrlMap.end(); ++it)
	{
		delete it->second;
	}
}

void VtChartCtrlManager::AddChartCtrl(VtChartWindow* chartCtrl)
{
	if (!chartCtrl)
		return;

	ChartCtrlMap[(int)chartCtrl] = chartCtrl;
}

VtChartWindow* VtChartCtrlManager::GetChartCtrl(CWnd* parentWnd)
{
	VtChartWindow* chartCtrl = new VtChartWindow();
	chartCtrl->Create(IDD_DIALOG_CHART, parentWnd);
	AddChartCtrl(chartCtrl);
	return chartCtrl;
}

void VtChartCtrlManager::DeleteChartCtrl(VtChartWindow* chartCtrl)
{
	if (!chartCtrl)
		return;

	auto it = ChartCtrlMap.find((int)chartCtrl);
	if (it != ChartCtrlMap.end())
	{
		VtChartWindow* ctrl = it->second;
		ChartCtrlMap.erase(it);
		delete ctrl;
	}
}
