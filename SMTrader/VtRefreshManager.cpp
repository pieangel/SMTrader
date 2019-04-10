#include "stdafx.h"
#include "VtRefreshManager.h"
#include "VtOrderCenterWndHd.h"

VtRefreshManager::VtRefreshManager()
{
}


VtRefreshManager::~VtRefreshManager()
{
	_TaskQueue.SetReady(true);
	_TaskQueue.Push(-1);
	if (_TaskThread.joinable()) 
		_TaskThread.join();
}

void VtRefreshManager::ProcessTask()
{
	while (true) {
		int taskArg = _TaskQueue.GetLatestNClear();
		if (taskArg == -1) {
			break;
		}
		UpdateScreen();
	}
}

void VtRefreshManager::StartTaskThread()
{
	_TaskThread = std::thread(&VtRefreshManager::ProcessTask, this);
}

void VtRefreshManager::UpdateScreen()
{
	if (_CenterWnd) {
		_TaskQueue.SetReady(false);
		_CenterWnd->SendMessage(WM_REFRESH_GRID, 0, 0);
		_TaskQueue.SetReady(true);
	}
}

void VtRefreshManager::SetReady(bool flag)
{
	_TaskQueue.SetReady(flag);
}

void VtRefreshManager::AddTask(int type)
{
	_TaskQueue.Push(type);
}
