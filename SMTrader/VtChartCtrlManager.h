#pragma once
#include <map>
#include "Global/TemplateSingleton.h"
class VtChartCrtl;
class VtChartWindow;
class CWnd;
class VtChartCtrlManager : public TemplateSingleton<VtChartCtrlManager>
{
public:
	VtChartCtrlManager();
	~VtChartCtrlManager();

	std::map<int, VtChartWindow*> ChartCtrlMap;
	VtChartWindow* GetChartCtrl(CWnd* parentWnd);
	void DeleteChartCtrl(VtChartWindow* chartCtrl);
private:
	void AddChartCtrl(VtChartWindow* chartCtrl);
};

