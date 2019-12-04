#include "stdafx.h"
#include "SmChartData.h"
#include "../Log/loguru.hpp"
#include "../VtHdClient.h"
#include "../Util/SmUtil.h"
#include "../VtStringUtil.h"
#include "../VtHdClient.h"
#include "chartdir.h"
#include "../VtSymbolManager.h"
#include "../VtSymbol.h"

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



std::vector<double> SmChartData::GetOpenVector()
{
	VtSymbol* symbol = VtSymbolManager::GetInstance()->FindHdSymbol(_SymbolCode);
	std::vector<double> v;
	if (!symbol)
		return v;
	for (auto it = _DataMap.begin(); it != _DataMap.end(); ++it) {
		SmChartDataItem& data = it->second;
		v.push_back(data.o / std::pow(10, symbol->Decimal));
	}
	return v;
}

std::vector<double> SmChartData::GetCloseVector()
{
	VtSymbol* symbol = VtSymbolManager::GetInstance()->FindHdSymbol(_SymbolCode);
	std::vector<double> v;
	if (!symbol)
		return v;
	for (auto it = _DataMap.begin(); it != _DataMap.end(); ++it) {
		SmChartDataItem& data = it->second;
		v.push_back(data.c / std::pow(10, symbol->Decimal));
	}
	return v;
}

std::vector<double> SmChartData::GetHighVector()
{
	VtSymbol* symbol = VtSymbolManager::GetInstance()->FindHdSymbol(_SymbolCode);
	std::vector<double> v;
	if (!symbol)
		return v;
	for (auto it = _DataMap.begin(); it != _DataMap.end(); ++it) {
		SmChartDataItem& data = it->second;
		v.push_back(data.h / std::pow(10, symbol->Decimal));
	}
	return v;
}

std::vector<double> SmChartData::GetLowVector()
{
	VtSymbol* symbol = VtSymbolManager::GetInstance()->FindHdSymbol(_SymbolCode);
	std::vector<double> v;
	if (!symbol)
		return v;
	for (auto it = _DataMap.begin(); it != _DataMap.end(); ++it) {
		SmChartDataItem& data = it->second;
		v.push_back(data.l / std::pow(10, symbol->Decimal));
	}
	return v;
}

std::vector<double> SmChartData::GetDateTimeVector()
{
	std::vector<double> v;
	for (auto it = _DataMap.begin(); it != _DataMap.end(); ++it) {
		SmChartDataItem& data = it->second;
		std::vector<int> dt = SmUtil::GetTime(data.date_time);
		double date_time = Chart::chartTime(dt[0], dt[1], dt[2], dt[3], dt[4], dt[5]);
		v.push_back(date_time);
	}
	return v;
}

std::vector<double> SmChartData::GetVolumeVector()
{
	std::vector<double> v;
	for (auto it = _DataMap.begin(); it != _DataMap.end(); ++it) {
		SmChartDataItem& data = it->second;
		v.push_back(data.v);
	}
	return v;
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

void SmChartData::Received(bool val)
{
	_Received = val;
	VtSymbol* symbol = VtSymbolManager::GetInstance()->FindHdSymbol(_SymbolCode);
	auto it = _DataMap.rbegin();
	SmChartDataItem& data = it->second;
	symbol->Quote.intClose = data.c;
	symbol->Quote.close = data.c / std::pow(10, symbol->Decimal);
}
