#include "stdafx.h"
#include "VtHogaManager.h"
#include "VtHoga.h"
#include "VtKrClient.h"
#include "VtWindowManager.h"
#include "SmOrderPanelOut.h"
#include "VtOrderDialogManager.h"

VtHogaManager::VtHogaManager()
{
}


VtHogaManager::~VtHogaManager()
{
	for (auto it = RealtimeHogaMap.begin(); it != RealtimeHogaMap.end(); ++it)
	{
		delete it->second;
	}

	for (auto it = HogaMap.begin(); it != HogaMap.end(); ++it)
	{
		delete it->second;
	}
}

void VtHogaManager::AddRealtimeHoga(std::string fullCode, VtRealtimeHoga* realtimeHoga)
{
	if (!realtimeHoga)
		return;

	RealtimeHogaMap[fullCode] = realtimeHoga;
}

void VtHogaManager::ReceiveRealtimeHoga(std::string fullCode, LPCTSTR packet)
{
// 	VtRealtimeHoga* hoga = FindAddRealtimeHoga(fullCode);
// 	VtWindowManager* winMgr = VtWindowManager::GetInstance();
// 	CVtOrderCenterWnd* wnd = (CVtOrderCenterWnd*)winMgr->FindWindow(_T("OrderCenterWnd"));
// 	if (wnd)
// 	{
// 		wnd->OnReceiveRealtimeHoga(hoga);
// 	}
}

void VtHogaManager::ReceiveRealtimeHoga(VtRealtimeHoga* hoga)
{
// 	VtWindowManager* winMgr = VtWindowManager::GetInstance();
// 	CVtOrderCenterWnd* wnd = (CVtOrderCenterWnd*)winMgr->FindWindow(_T("OrderCenterWnd"));
// 	if (wnd)
// 	{
// 		wnd->OnReceiveRealtimeHoga(hoga);
// 	}
}

void VtHogaManager::ReceiveRealtimeHoga(VtHoga* hoga)
{
	VtOrderDialogManager* orderWndMgr = VtOrderDialogManager::GetInstance();
	orderWndMgr->OnReceiveRealtimeHoga(hoga);
}

void VtHogaManager::RemoveRealtimeHoga(std::string fullCode)
{
	auto it = RealtimeHogaMap.find(fullCode);
	if (it != RealtimeHogaMap.end())
	{
		VtRealtimeHoga* hoga = it->second;
		delete hoga;
		RealtimeHogaMap.erase(it);
	}
}

VtRealtimeHoga* VtHogaManager::FindAddRealtimeHoga(std::string fullCode)
{
	auto it = RealtimeHogaMap.find(fullCode);
	if (it != RealtimeHogaMap.end())
	{
		return it->second;
	}
	else
	{
		VtRealtimeHoga* hoga = new VtRealtimeHoga();
		RealtimeHogaMap[fullCode] = hoga;
		return hoga;
	}
}

void VtHogaManager::AddHoga(std::string fullCode, VtHoga* realtimeHoga)
{
	if (!realtimeHoga)
		return;

	HogaMap[fullCode] = realtimeHoga;
}

void VtHogaManager::ReceiveHoga(std::string fullCode, LPCTSTR packet)
{
	VtHoga* hoga = FindAddHoga(fullCode);
	//memcpy(hoga, packet, sizeof(VtHoga));
	VtOrderDialogManager* orderWndMgr = VtOrderDialogManager::GetInstance();
	orderWndMgr->OnReceiveHoga(hoga);
}

void VtHogaManager::ReceiveHoga(VtHoga* hoga)
{
	VtOrderDialogManager* orderWndMgr = VtOrderDialogManager::GetInstance();
	orderWndMgr->OnReceiveHoga(hoga);
}

void VtHogaManager::RemoveHoga(std::string fullCode)
{
	auto it = HogaMap.find(fullCode);
	if (it != HogaMap.end())
	{
		VtHoga* hoga = it->second;
		delete hoga;
		HogaMap.erase(it);
	}
}

VtHoga* VtHogaManager::FindAddHoga(std::string fullCode)
{
	auto it = HogaMap.find(fullCode);
	if (it != HogaMap.end())
	{
		return it->second;
	}
	else
	{
		VtHoga* hoga = new VtHoga();
		hoga->FullCode = fullCode;
		HogaMap[fullCode] = hoga;
		return hoga;
	}
}

VtHoga* VtHogaManager::FindHoga(std::string fullCode)
{
	auto it = HogaMap.find(fullCode);
	if (it != HogaMap.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

void VtHogaManager::RequestHoga(std::string fullCode)
{
	VtKrClient* client = VtKrClient::GetInstance();
	client->RequestSymbolHoga(fullCode);
}
