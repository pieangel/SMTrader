#pragma once
#include "Global/TemplateSingleton.h"
#include <map>
#include <vector>
#include "HdWindowEvent.h"

class VtChartContainer;
class VtChartContainerManager : public TemplateSingleton<VtChartContainerManager>
{
public:
	VtChartContainerManager();
	~VtChartContainerManager();
	void OnWindowEvent(HdWindowEventArgs& arg);
private:
	std::map<CWnd*, CWnd*> _WindowMap;
	void AddWindow(HdWindowType wndType, CWnd* wnd);
	void RemoveWindow(CWnd* wnd);
};

