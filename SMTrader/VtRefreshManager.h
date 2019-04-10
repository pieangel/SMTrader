#pragma once
#include "Global/TemplateSingleton.h"
#include <list>
#include <thread>
#include <map>
#include <queue>
#include "BlockingQueue.h"
#define WM_REFRESH_GRID (WM_USER + 10002)
class VtOrderCenterWndHd;
class VtRefreshManager : public TemplateSingleton<VtRefreshManager>
{
public:
	VtRefreshManager();
	~VtRefreshManager();
	std::thread _TaskThread;
	// int : task type
	BlockingQueue<int> _TaskQueue;
	void ProcessTask();
	void StartTaskThread();
	void UpdateScreen();
	void SetReady(bool flag);
	void AddTask(int type);
	VtOrderCenterWndHd* CenterWnd() const { return _CenterWnd; }
	void CenterWnd(VtOrderCenterWndHd* val) { _CenterWnd = val; }
private:
	VtOrderCenterWndHd* _CenterWnd = nullptr;
};

