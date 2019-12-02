#include "stdafx.h"
#include "SmChartData.h"
#include "../Log/loguru.hpp"
#include "../VtHdClient.h"
#include "../Util/SmUtil.h"
#include "../VtStringUtil.h"
#include "../VtHdClient.h"

void SmChartData::GetChartDataFromDB()
{
	
}

void SmChartData::GetChartDataFromServer()
{
	SmChartDataRequest req;
	req.reqType = SmChartDataReqestType::FIRST;
	req.symbolCode = _SymbolCode;
	req.chartType = _ChartType;
	req.cycle = _Cycle;
	req.count = _DataQueueSize;
	req.next = 0;
	VtHdClient* client = VtHdClient::GetInstance();
	client->GetChartData(req);
}

void SmChartData::GetCyclicDataFromServer()
{
	SmChartDataRequest req;
	req.reqType = SmChartDataReqestType::CYCLE;
	req.symbolCode = _SymbolCode;
	req.chartType = _ChartType;
	req.cycle = _Cycle;
	req.count = _CycleDataSize;
	req.next = 0;
	VtHdClient* client = VtHdClient::GetInstance();
	client->GetChartData(req);
}



void SmChartData::UpdateLastValue(int c)
{
	if (_DataMap.size() == 0)
		return;

	SmChartDataItem& lastData = _DataMap.rbegin()->second;

	int h = lastData.h;
	int l = lastData.l;
	if (c > h) {
		lastData.h = c;
	}
	if (c < l) {
		lastData.l = l;
	}
	lastData.c = c;
}

void SmChartData::AddData(SmChartDataItem& data_item)
{
	auto it = _DataMap.find(data_item.date_time);
	if (it != _DataMap.end()) {
		SmChartDataItem& data = it->second;
		data.c = data_item.c;
		data.o = data_item.o;
		data.h = data_item.h;
		data.l = data_item.l;
		data.v = data_item.v;
		return;
	}
	_DataMap.insert(std::make_pair(data_item.date_time, data_item));
	size_t count = _DataMap.size();
	if (count > _DataQueueSize) {
		// 큐의 크기를 넘어서면 맨 과거 데이터를 제거해 준다.
		auto it = _DataMap.begin();
		_DataMap.erase(it);
	}
}

std::pair<int, int> SmChartData::GetCycleByTimeDif()
{
	return std::make_pair(0,0);
}

void SmChartData::OnTimer()
{
	GetCyclicDataFromServer();
}
