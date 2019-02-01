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
	/// ���ο� �����츦 �����ϰ� �߰��Ѵ�.
	/// </summary>
	/// <param name="parentWnd"></param>
	/// <returns></returns>
	VtChartWindow* CreateChartWindow(CWnd* parentWnd);
	/// <summary>
	/// ��Ʈ �����츦 �̺�Ʈ ��Ͽ� �߰��Ѵ�.
	/// </summary>
	/// <param name="key"></param>
	/// <param name="wnd"></param>
	void AddChartWindow(std::string key, VtChartWindow* wnd);
	/// <summary>
	/// �̺�Ʈ ��Ͽ��� �����찡 �̺�Ʈ�� ���� �ʰ� ���ش�.
	/// </summary>
	/// <param name="key"></param>
	/// <param name="wnd"></param>
	void RemoveChartWindow(std::string key, VtChartWindow* wnd);
	/// <summary>
	/// �����쿡 ���ο� Ű�� �Ҵ��Ͽ� ���ο� �̺�Ʈ�� �ް� �Ѵ�.
	/// </summary>
	void ChangeChartWindow(std::string newKey, VtChartWindow* wnd);	
	
	/// <summary>
	/// ��ü �����쿡�� �־��� �����츦 �����ϴ� �Լ�
	/// </summary>
	/// <param name="wnd"></param>
	void RemoveChartWindow(VtChartWindow* wnd);
	/// <summary>
	/// ��ü ������ ���
	/// </summary>
	std::map<VtChartWindow*, VtChartWindow*> TotalWndMap;
	
	/// <summary>
	/// ��Ʈ �����Ͱ� �������� �� ȣ��Ǵ� �Լ�
	/// </summary>
	/// <param name="data"></param>
	void OnReceiveChartData(VtChartData* data);
	/// <summary>
	/// ��Ʈ �����Ͱ� �������� �� ȣ��Ǵ� �Լ�
	/// </summary>
	/// <param name="data"></param>
	void OnUpdateChartData(VtChartData* data, int count);
	/// <summary>
	/// ���� ������ �̺�Ʈ ó�� ��
	/// </summary>
	std::map<std::string, VtChartWindowEventDispatcher*> MainChartMap;
	/// <summary>
	/// ���� ������ �̺�Ʈ ó�� ��
	/// </summary>
	std::map<std::string, VtChartWindowEventDispatcher*> ReferenceMap;

	void AddRefChartWindow(std::string key, VtChartWindow* wnd);
	void RemoveRefChartWindow(std::string key, VtChartWindow* wnd);
	void ChangeRefChartWindow(std::string oldKey, std::string newKey, VtChartWindow* wnd);
	/// <summary>
	/// �ǽð� �����͸� ���� ������ ��
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

