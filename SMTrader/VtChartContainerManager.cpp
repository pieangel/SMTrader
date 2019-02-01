#include "stdafx.h"
#include "VtChartContainerManager.h"
#include "VtChartContainer.h"

VtChartContainerManager::VtChartContainerManager()
{
}


VtChartContainerManager::~VtChartContainerManager()
{
	for (auto it = _WindowMap.begin(); it != _WindowMap.end(); ++it)
	{
		CWnd* wnd = it->second;
		if (wnd->GetSafeHwnd())
		{
			wnd->DestroyWindow();
		}
		delete wnd;
	}

	_WindowMap.clear();
}

void VtChartContainerManager::OnWindowEvent(HdWindowEventArgs& arg)
{
	if (!arg.pWnd)
		return;

	if (arg.eventType == HdWindowEventType::Created)
		AddWindow(arg.wndType, arg.pWnd);
	else if (arg.eventType == HdWindowEventType::Closed)
	{
		RemoveWindow(arg.pWnd);
		arg.pWnd->DestroyWindow();
		delete arg.pWnd;
	}
}

void VtChartContainerManager::AddWindow(HdWindowType wndType, CWnd* wnd)
{
	_WindowMap[wnd] = wnd;
}

void VtChartContainerManager::RemoveWindow(CWnd* wnd)
{
	auto it = _WindowMap.find(wnd);
	if (it != _WindowMap.end())
	{
		_WindowMap.erase(it);
	}
}
