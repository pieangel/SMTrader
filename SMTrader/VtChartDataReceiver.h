#pragma once
#include "Global/VtDefine.h"
#include <string>
#include <cctype>
#include <algorithm>
#include <map>
#include <mutex>
#include <thread>
#include "Global/TemplateSingleton.h"
#include "Poco/BasicEvent.h"
#include "VtChartDataEvent.h"

using Poco::BasicEvent;

class VtChartData;
class VtChartDataReceiver : public TemplateSingleton<VtChartDataReceiver>
{
public:
	VtChartDataReceiver();
	~VtChartDataReceiver();
	void ReceivedPacket(LPCTSTR param);
private:
	std::string fullCode;
	// ������ ���� ������.
	std::string maxDataCnt;
	std::string dataCnt;
	// ������ ���� ƽ���� - �к��� �� ����, �ֺ��� �� ����, �Ϻ��� �ϰ���, ������ �� ����
	std::string tickCnt;
	std::string toDay;
	std::string preClose;
	std::string dataGb;
	std::string dayCnt;
	// �ֱ� ��
	std::string summary;
	std::string preLast;

	// Data
	VtDate date;
	VtTime time;
	std::string openPrice = _T("");
	std::string highPrice = _T("");
	std::string lowPrice = _T("");
	std::string closePrice = _T("");
	std::string volume = _T("");


	std::map<std::string, VtChartData*> _PacketMap;
	std::mutex  _lock;

public:
	BasicEvent<VtChartDataEventArgs> _ChartDataReceivedEvent;
	void FireChartDataReceived(VtChartDataEventArgs&& arg)
	{
		_ChartDataReceivedEvent(this, arg);
	}
};



