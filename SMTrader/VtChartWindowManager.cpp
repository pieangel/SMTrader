#include "stdafx.h"
//#include "VtChartCrtl.h"
#include "VtChartData.h"
#include "VtChartWindowManager.h"
#include "Poco/Delegate.h"
#include "MainFrm.h"
#include "VtChartWindow.h"
#include "Resource.h"
//#include "VtChartDataRequestManager.h"

using Poco::Delegate;

VtChartWindowManager::VtChartWindowManager()
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	//_ChartDataReceivedEvent += delegate(pMainFrm, &CMainFrame::OnChartDataReceived);
}


VtChartWindowManager::~VtChartWindowManager()
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	//_ChartDataReceivedEvent -= delegate(pMainFrm, &CMainFrame::OnChartDataReceived);

	for (auto it = MainChartMap.begin(); it != MainChartMap.end(); ++it)
	{
		delete it->second;
	}

	for (auto it = ReferenceMap.begin(); it != ReferenceMap.end(); ++it)
	{
		delete it->second;
	}
}



VtChartWindow* VtChartWindowManager::CreateChartWindow(CWnd* parentWnd)
{
	VtChartWindow* chartCtrl = new VtChartWindow();
	chartCtrl->Create(IDD_DIALOG_CHART, parentWnd);
	TotalWndMap[chartCtrl] = chartCtrl;
	return chartCtrl;
}

void VtChartWindowManager::AddChartWindow(std::string key, VtChartWindow* wnd)
{
	if (!wnd)
		return;

	wnd->ChartDataKey(key);

	auto it = MainChartMap.find(key);
	if (it != MainChartMap.end())
	{
		VtChartWindowEventDispatcher* sdp = it->second;
		sdp->WndMap[wnd] = wnd;
	}
	else
	{
		VtChartWindowEventDispatcher* sdp = new VtChartWindowEventDispatcher();
		sdp->WndMap[wnd] = wnd;
		MainChartMap[key] = sdp;
	}
}


void VtChartWindowManager::RemoveChartWindow(std::string key, VtChartWindow* wnd)
{
	auto it = MainChartMap.find(key);
	if (it != MainChartMap.end())
	{
		VtChartWindowEventDispatcher* sdp = it->second;
		auto innerIt = sdp->WndMap.find(wnd);
		if (innerIt != sdp->WndMap.end())
		{
			sdp->WndMap.erase(wnd);
		}
	}
}

void VtChartWindowManager::RemoveChartWindow(VtChartWindow* wnd)
{
	if (!wnd)
		return;
	auto it = TotalWndMap.find(wnd);
	{
		VtChartWindow* delWnd = it->second;
		TotalWndMap.erase(it);
		delete delWnd;
	}
}


void VtChartWindowManager::ChangeChartWindow(std::string newKey, VtChartWindow* wnd)
{
	if (!wnd)
		return;
	RemoveChartWindow(wnd->ChartDataKey(), wnd);
	AddChartWindow(newKey, wnd);
}


void VtChartWindowManager::OnReceiveChartData(VtChartData* data)
{
	if (!data)
		return;
	if (data->SymbolCode().length() == 0)
		return;
	if (data->RequestType() == VtChartRequestType::HISTORY)
	{
		auto it = MainChartMap.find(data->Key());
		if (it != MainChartMap.end())
		{
			VtChartWindowEventDispatcher* Dispatcher = it->second;
			for (auto inner = Dispatcher->WndMap.begin(); inner != Dispatcher->WndMap.end(); ++inner)
			{
				VtChartWindow* wnd = inner->second;
				//wnd->OnReceiveChartData(data);
				if (wnd->GetSafeHwnd())
					wnd->SendMessage(WM_CHARTDATA_RECEIVED, (WPARAM)data, 0);
			}
		}
	}
	else if (data->RequestType() == VtChartRequestType::REF)
	{
		auto it = ReferenceMap.find(data->Key());
		if (it != ReferenceMap.end())
		{
			VtChartWindowEventDispatcher* Dispatcher = it->second;
			for (auto inner = Dispatcher->WndMap.begin(); inner != Dispatcher->WndMap.end(); ++inner)
			{
				VtChartWindow* wnd = inner->second;
				//wnd->OnReceiveRefData(data);
				if (wnd->GetSafeHwnd())
					wnd->SendMessage(WM_CHARTDATA_RECEIVED, (WPARAM)data, 1);
			}
		}
	}
}

void VtChartWindowManager::OnUpdateChartData(VtChartData* data, int count)
{
	if (!data)
		return;
	if (data->SymbolCode().length() == 0)
		return;
	if (data->RequestType() == VtChartRequestType::TIMER)
	{
		auto it = MainChartMap.find(data->Key());
		if (it != MainChartMap.end())
		{
			VtChartWindowEventDispatcher* Dispatcher = it->second;
			for (auto inner = Dispatcher->WndMap.begin(); inner != Dispatcher->WndMap.end(); ++inner)
			{
				VtChartWindow* wnd = inner->second;
				if (wnd->GetSafeHwnd())
					wnd->SendMessage(WM_CHARTDATA_UPDATED, (WPARAM)data, count);
			}
		}
	}
	else if (data->RequestType() == VtChartRequestType::HISTORY)
	{
		auto it = MainChartMap.find(data->Key());
		if (it != MainChartMap.end())
		{
			VtChartWindowEventDispatcher* Dispatcher = it->second;
			for (auto inner = Dispatcher->WndMap.begin(); inner != Dispatcher->WndMap.end(); ++inner)
			{
				VtChartWindow* wnd = inner->second;
				//wnd->OnReceiveChartData(data);
				if (wnd->GetSafeHwnd())
					wnd->SendMessage(WM_CHARTDATA_RECEIVED, (WPARAM)data, 0);
			}
		}
	}
	else if (data->RequestType() == VtChartRequestType::REF)
	{
		auto it = ReferenceMap.find(data->Key());
		if (it != ReferenceMap.end())
		{
			VtChartWindowEventDispatcher* Dispatcher = it->second;
			for (auto inner = Dispatcher->WndMap.begin(); inner != Dispatcher->WndMap.end(); ++inner)
			{
				VtChartWindow* wnd = inner->second;
				//wnd->OnReceiveRefData(data);
				if (wnd->GetSafeHwnd())
					wnd->SendMessage(WM_CHARTDATA_RECEIVED, (WPARAM)data, 1);
			}
		}
	}
}

void VtChartWindowManager::AddRefChartWindow(std::string key, VtChartWindow* wnd)
{
	if (!wnd)
		return;

	wnd->ChartDataKey(key);

	auto it = ReferenceMap.find(key);
	if (it != ReferenceMap.end())
	{
		VtChartWindowEventDispatcher* sdp = it->second;
		sdp->WndMap[wnd] = wnd;
	}
	else
	{
		VtChartWindowEventDispatcher* sdp = new VtChartWindowEventDispatcher();
		sdp->WndMap[wnd] = wnd;
		ReferenceMap[key] = sdp;
	}
}

void VtChartWindowManager::RemoveRefChartWindow(std::string key, VtChartWindow* wnd)
{
	if (!wnd)
		return;

	auto it = ReferenceMap.find(key);
	if (it != ReferenceMap.end())
	{
		VtChartWindowEventDispatcher* sdp = it->second;
		auto innerIt = sdp->WndMap.find(wnd);
		if (innerIt != sdp->WndMap.end())
		{
			sdp->WndMap.erase(wnd);
		}
	}
}

void VtChartWindowManager::ChangeRefChartWindow(std::string oldKey, std::string newKey, VtChartWindow* wnd)
{
	if (!wnd)
		return;
	RemoveRefChartWindow(oldKey, wnd);
	AddRefChartWindow(newKey, wnd);
}

void VtChartWindowManager::AddRealtimeChartWindow(std::string oldKey, std::string key, VtChartWindow* wnd)
{
	if (!wnd)
		return;
	if (oldKey.length() > 0)
	{
		if (oldKey.compare(key) == 0)
			return;
		RemoveRealtimeChartWindow(oldKey, wnd);
	}

	wnd->RealtimeDataKey(key);

	auto it = RealtimeMap.find(key);
	if (it != RealtimeMap.end())
	{
		VtChartWindowEventDispatcher* sdp = it->second;
		sdp->WndMap[wnd] = wnd;
	}
	else
	{
		VtChartWindowEventDispatcher* sdp = new VtChartWindowEventDispatcher();
		sdp->WndMap[wnd] = wnd;
		RealtimeMap[key] = sdp;
	}
}

void VtChartWindowManager::AddRealtimeChartWindow(std::string key, VtChartWindow* wnd)
{
	if (!wnd)
		return;
	
	wnd->RealtimeDataKey(key);

	auto it = RealtimeMap.find(key);
	if (it != RealtimeMap.end())
	{
		VtChartWindowEventDispatcher* sdp = it->second;
		sdp->WndMap[wnd] = wnd;
	}
	else
	{
		VtChartWindowEventDispatcher* sdp = new VtChartWindowEventDispatcher();
		sdp->WndMap[wnd] = wnd;
		RealtimeMap[key] = sdp;
	}
}

void VtChartWindowManager::RemoveRealtimeChartWindow(std::string key, VtChartWindow* wnd)
{
	if (!wnd)
		return;

	auto it = RealtimeMap.find(key);
	if (it != RealtimeMap.end())
	{
		VtChartWindowEventDispatcher* sdp = it->second;
		auto innerIt = sdp->WndMap.find(wnd);
		if (innerIt != sdp->WndMap.end())
		{
			sdp->WndMap.erase(wnd);
		}
	}
}

void VtChartWindowManager::OnReceiveRealtimeData(std::string exc, std::string shortCode, double krwClose, double usdClose /*= 0*/)
{
	std::string fullCode = exc;
	fullCode.append(shortCode);

	auto it = RealtimeMap.find(fullCode);		
	if (it != RealtimeMap.end())
	{
		VtChartWindowEventDispatcher* Dispatcher = it->second;
		for (auto inner = Dispatcher->WndMap.begin(); inner != Dispatcher->WndMap.end(); ++inner)
		{
			VtChartWindow* wnd = inner->second;
			wnd->OnReceiveRealtimeData(exc, shortCode, krwClose, usdClose);
			wnd->PostMessage(WM_CHARTDATA_RECEIVED, 0, 0);
		}
	}
}

