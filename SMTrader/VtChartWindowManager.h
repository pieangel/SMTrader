#pragma once
#include "Global/TemplateSingleton.h"
#include <map>
#include "Poco/BasicEvent.h"
#include "VtChartDataEvent.h"

using Poco::BasicEvent;

class VtChartEventDispatcher;
class VtChartData;


class VtChartWindow;

struct VtChartWindowEventDispatcher
{
	std::map<VtChartWindow*, VtChartWindow*> WndMap;
};


class CWnd;
class VtChartWindowManager : public TemplateSingleton<VtChartWindowManager>
{
public:
	VtChartWindowManager();
	~VtChartWindowManager();
	
	
	/// <summary>
	/// 새로운 윈도우를 생성하고 추가한다.
	/// </summary>
	/// <param name="parentWnd"></param>
	/// <returns></returns>
	VtChartWindow* CreateChartWindow(CWnd* parentWnd);
	/// <summary>
	/// 차트 윈도우를 이벤트 목록에 추가한다.
	/// </summary>
	/// <param name="key"></param>
	/// <param name="wnd"></param>
	void AddChartWindow(std::string key, VtChartWindow* wnd);
	/// <summary>
	/// 이벤트 목록에서 윈도우가 이벤트를 받지 않게 해준다.
	/// </summary>
	/// <param name="key"></param>
	/// <param name="wnd"></param>
	void RemoveChartWindow(std::string key, VtChartWindow* wnd);
	/// <summary>
	/// 윈도우에 새로운 키를 할당하여 새로운 이벤트를 받게 한다.
	/// </summary>
	void ChangeChartWindow(std::string newKey, VtChartWindow* wnd);	
	
	/// <summary>
	/// 전체 윈도우에서 주어진 윈도우를 제거하는 함수
	/// </summary>
	/// <param name="wnd"></param>
	void RemoveChartWindow(VtChartWindow* wnd);
	/// <summary>
	/// 전체 윈도우 목록
	/// </summary>
	std::map<VtChartWindow*, VtChartWindow*> TotalWndMap;
	
	/// <summary>
	/// 차트 데이터가 도착했을 때 호출되는 함수
	/// </summary>
	/// <param name="data"></param>
	void OnReceiveChartData(VtChartData* data);
	/// <summary>
	/// 차트 데이터가 도착했을 때 호출되는 함수
	/// </summary>
	/// <param name="data"></param>
	void OnUpdateChartData(VtChartData* data, int count);
	/// <summary>
	/// 메인 윈도우 이벤트 처리 맵
	/// </summary>
	std::map<std::string, VtChartWindowEventDispatcher*> MainChartMap;
	/// <summary>
	/// 참조 윈도우 이벤트 처리 맵
	/// </summary>
	std::map<std::string, VtChartWindowEventDispatcher*> ReferenceMap;

	void AddRefChartWindow(std::string key, VtChartWindow* wnd);
	void RemoveRefChartWindow(std::string key, VtChartWindow* wnd);
	void ChangeRefChartWindow(std::string oldKey, std::string newKey, VtChartWindow* wnd);
	/// <summary>
	/// 실시간 데이터를 받을 윈도우 맵
	/// </summary>
	std::map<std::string, VtChartWindowEventDispatcher*> RealtimeMap;

	void AddRealtimeChartWindow(std::string oldKey, std::string key, VtChartWindow* wnd);
	void AddRealtimeChartWindow(std::string key, VtChartWindow* wnd);
	void RemoveRealtimeChartWindow(std::string key, VtChartWindow* wnd);

	void OnReceiveRealtimeData(std::string exc, std::string shortCode, double krwClose, double usdClose = 0);

public:
	BasicEvent<VtChartDataEventArgs> _ChartDataReceivedEvent;
	void FireChartDataReceived(VtChartDataEventArgs&& arg)
	{
		_ChartDataReceivedEvent(this, arg);
	}
};

