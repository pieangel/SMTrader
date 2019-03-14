#include "stdafx.h"
#include "VtChartDataManager.h"
#include "Global/VtDefine.h"
#include "VtChartData.h"
#include <chrono>
#include <random>
#include <iostream>
#include <memory>
#include <functional>
#include <list>
#include "VtSymbol.h"
#include "VtChartDataCollector.h"
#include "VtGlobal.h"

using namespace std::chrono;
VtChartDataManager::VtChartDataManager()
{
}


VtChartDataManager::~VtChartDataManager()
{
	for (auto it = _ChartData.begin(); it != _ChartData.end(); ++it)
	{
		delete it->second;
	}
}

void VtChartDataManager::AddChartData(std::string symCode, VtChartData* data)
{
	if (!data)
		return;
	auto it = _SymbolChartMap.find(symCode);
	if (it != _SymbolChartMap.end())
	{
		std::vector<VtChartData*>& list = it->second;
		list.push_back(data);
	}
	else
	{
		std::vector<VtChartData*> list;
		list.push_back(data);
		_SymbolChartMap[symCode] = list;
	}
}

VtChartData* VtChartDataManager::Find(std::string key)
{
	auto it = _ChartData.find(key);
	if (it != _ChartData.end())
		return (*it).second;
	else
		return nullptr;
}

VtChartData* VtChartDataManager::FindNAdd(std::string symCode, std::string cycleType, std::string cycle)
{
	std::string key = symCode;
	key.append(":");
	key.append(cycleType);
	key.append(":");
	key.append(cycle);
	VtChartData* pData = Find(key);
	if (!pData)
	{
		pData = new VtChartData();
		pData->SymbolCode(symCode);
		VtChartType type = VtChartType::MIN;
		if (cycleType.compare(_T("1")) == 0)
			type = VtChartType::DAY;
		else if (cycleType.compare(_T("2")) == 0)
			type = VtChartType::WEEK;
		else if (cycleType.compare(_T("3")) == 0)
			type = VtChartType::MONTH;
		else if (cycleType.compare(_T("4")) == 0)
			type = VtChartType::HOUR;
		else if (cycleType.compare(_T("5")) == 0)
			type = VtChartType::MIN;
		else if (cycleType.compare(_T("6")) == 0)
			type = VtChartType::TICK;
		int summary = std::stoi(cycle);

		pData->ChartType(type);
		pData->Cycle(summary);
		pData->Key(key);
		_ChartData[key] = pData;

		AddChartData(symCode, pData);
		return pData;
	}
	else
	{
		return pData;
	}
}

VtChartData* VtChartDataManager::FindNAdd(std::string symCode, VtChartType type, int cycle)
{
	std::string key = symCode;
	key.append(":");
	key.append(std::to_string((int)type));
	key.append(":");
	key.append(std::to_string(cycle));
	VtChartData* pData = Find(key);
	if (!pData)
	{
		pData = new VtChartData();
		pData->SymbolCode(symCode);
		pData->ChartType(type);
		pData->Cycle(cycle);
		pData->Key(key);
		_ChartData[key] = pData;
		AddChartData(symCode, pData);
		return pData;
	}
	else
	{
		return pData;
	}
}

VtChartData* VtChartDataManager::Add(VtChartDataRequest req)
{
	std::string key = req.productCode;
	key.append(":");
	key.append(std::to_string((int)req.chartType));
	key.append(":");
	key.append(std::to_string(req.cycle));
	VtChartData* pData = Find(key);
	if (!pData)
	{
		pData = new VtChartData();
		pData->SymbolCode(req.productCode);
		pData->ChartType(req.chartType);
		pData->Cycle(req.cycle);
		pData->Key(key);
		pData->keyvalue = req.keyvalue;
		pData->Domestic(req.domestic);
		pData->MainChartType(req.mainChartType);
		pData->ColorIndex(req.colorIndex);
		pData->Initial(req.initial);
		pData->Decimal(req.decimal);
		_ChartData[key] = pData;
		// For the chart data update event.
		AddChartData(req.productCode, pData);
		return pData;
	}
	else
	{
		return pData;
	}
}

std::pair<bool, VtChartData*> VtChartDataManager::Add(std::string symCode, VtChartType type, int cycle)
{
	std::string key = symCode;
	key.append(":");
	key.append(std::to_string((int)type));
	key.append(":");
	key.append(std::to_string(cycle));
	VtChartData* pData = Find(key);
	if (!pData) {
		pData = new VtChartData();
		pData->SymbolCode(symCode);
		pData->ChartType(type);
		pData->Cycle(cycle);
		pData->Key(key);
		_ChartData[key] = pData;
		AddChartData(symCode, pData);
		return std::make_pair(true, pData);
	} else {
		return std::make_pair(false, pData);
	}
}

void VtChartDataManager::AddChartDataRequest(int interval, VtChartData* chartData)
{
	if (!chartData)
		return;
	std::string dataKey = chartData->GetChartDataKey();
	// First, find the chart data key from the request map.
	auto it = _RequestMap.find(dataKey);
	if (it != _RequestMap.end())
		return;
	VtTime curTime = VtGlobal::GetLocalTime();
	int waitTime = 60 - curTime.sec;
	// Add to the timer.
	auto id = _Timer.add(seconds(waitTime), std::bind(&VtChartData::OnTimer, chartData), seconds(chartData->Cycle() * 60));
	// Add to the request map.
	_RequestMap[chartData->GetChartDataKey()] = id;
}

void VtChartDataManager::AddChartDataRequest(int startAfter, int interval, VtChartData* chartData)
{
	if (!chartData)
		return;
	std::string dataKey = chartData->GetChartDataKey();
	// First, find the chart data key from the request map.
	auto it = _RequestMap.find(dataKey);
	if (it != _RequestMap.end())
		return;

	// Add to the timer.
	auto id = _Timer.add(seconds(startAfter), std::bind(&VtChartData::OnTimer, chartData), seconds(interval));
	// Add to the request map.
	_RequestMap[chartData->GetChartDataKey()] = id;

	VtChartDataCollector* collector = VtChartDataCollector::GetInstance();
}

void VtChartDataManager::RemoveChartDataRequest(VtChartData* chartData)
{
	if (!chartData)
		return;
	auto it = _RequestMap.find(chartData->GetChartDataKey());
	if (it != _RequestMap.end())
	{
		auto id = it->second;
		// Remove the id from the timer belonged to the given chart data.
		_Timer.remove(id);
		_RequestMap.erase(it);
	}
}

void VtChartDataManager::OnReceiveQuoteHd(VtSymbol* sym)
{
	if (!sym)
		return;

	auto it = _SymbolChartMap.find(sym->ShortCode);
	if (it != _SymbolChartMap.end()) {
		std::vector<VtChartData*>& list = it->second;
		for (auto itn = list.begin(); itn != list.end(); ++itn) {
			VtChartData* data = *itn;
			data->OnReceiveQuoteHd(sym);
		}
	}
}



void VtChartDataManager::OnReceiveChartData(VtChartData* data)
{
	if (!data)
		return;
	data->OnReceiveChartData(data);
}

void VtChartDataManager::OnReceiveFirstChartData(VtChartData* data)
{
	if (!data)
		return;
	data->OnReceiveFirstChartData(data);
}

void VtChartDataManager::OnTimer()
{

}

std::string VtChartDataManager::MakeChartDataKey(std::string code, VtChartType chartType, int cycle)
{
	std::string key = code;
	key.append(":");
	key.append(std::to_string((int)chartType));
	key.append(":");
	key.append(std::to_string(cycle));
	return key;
}

std::string VtChartDataManager::MakeChartDataKey(std::string exchange, std::string code, VtChartType chartType, int cycle)
{
	std::string key = exchange;
	key.append(":");
	key.append(code);
	key.append(":");
	key.append(std::to_string((int)chartType));
	key.append(":");
	key.append(std::to_string(cycle));
	return key;
}

std::string VtChartDataManager::MakeChartDataKey(VtChartDataRequest req)
{
	std::string key = req.exchange;
	key.append(":");
	key.append(req.productCode);
	key.append(":");
	key.append(std::to_string((int)req.chartType));
	key.append(":");
	key.append(std::to_string(req.cycle));
	return key;
}

void VtChartDataManager::FindDataBorderQueue(VtChartData* data, double* timeInfoArray)
{
	if (!data || !timeInfoArray)
		return;
	VtDate latestDate = data->Date[data->GetDataCount() - 1];
	VtTime latestTime = data->Time[data->GetDataCount() - 1];
	DataBorder border;
	border.EndDate = latestDate;
	border.EndTime = latestTime;
	border.endIndex = data->GetDataCount() - 1;

	VtDate oldDate = latestDate;
	VtTime oldTime = latestTime;
	std::list<DataBorder> borderList;
	borderList.push_front(border);
	for (int i = data->GetDataCount() - 1; i >= 0; --i)
	{
		VtDate curDate = data->Date[i];
		if (curDate.year != oldDate.year || 
			curDate.month != oldDate.month || 
			curDate.day != oldDate.day)
		{
			DataBorder& curBorder = borderList.front();
			curBorder.StartDate = oldDate;
			curBorder.StartTime = oldTime;
			curBorder.startIndex = i + 1;

			DataBorder newBorder;
			newBorder.EndDate = curDate;
			newBorder.EndTime = data->Time[i];
			newBorder.endIndex = i;
			borderList.push_front(newBorder);
		}
		oldDate = curDate;
		oldTime = data->Time[i];
		if (i == 0)
		{
			DataBorder& curBorder = borderList.front();
			curBorder.StartDate = oldDate;
			curBorder.StartTime = oldTime;
			curBorder.startIndex = i;
		}
	}

	int i = 0;
}
