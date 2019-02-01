#include "stdafx.h"
#include "VtWindowManager.h"


VtWindowManager::VtWindowManager()
{
}


VtWindowManager::~VtWindowManager()
{
}

void VtWindowManager::AddWindow(std::string windowName, CWnd* wnd)
{
	if (!wnd) return;

	WindowMap[windowName] = wnd;
}

CWnd* VtWindowManager::FindWindow(std::string windowName)
{
	auto it = WindowMap.find(windowName);
	if (it != WindowMap.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

void VtWindowManager::RemoveWindow(std::string windowName)
{
	auto it = WindowMap.find(windowName);
	if (it != WindowMap.end())
	{
		WindowMap.erase(it);
	}
}
