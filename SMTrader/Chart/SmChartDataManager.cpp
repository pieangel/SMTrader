#include "stdafx.h"
#include "SmChartDataManager.h"
#include "SmChartData.h"
#include "../Util/SmUtil.h"
#include <chrono>
#include "../VtSymbolManager.h"
#include "../VtSymbol.h"
#include "../SmCallbackManager.h"
#include <functional>

using namespace std::chrono;


SmChartDataManager::SmChartDataManager()
{

}

SmChartDataManager::~SmChartDataManager()
{
	for (auto it = _ChartDataMap.begin(); it != _ChartDataMap.end(); ++it) {
		delete it->second;
	}
}

void SmChartDataManager::AddChartData(SmChartData* chart_data)
{
	if (!chart_data)
		return;

	auto it = _ChartDataMap.find(chart_data->GetDataKey());
	if (it == _ChartDataMap.end()) {
		_ChartDataMap[chart_data->GetDataKey()] = chart_data;
		VtSymbol* symbol = VtSymbolManager::GetInstance()->FindHdSymbol(chart_data->SymbolCode());
		// 심볼에 차트 데이터를 추가해 준다. 
		// 종가 업데이트를 위해서 꼭 필요하다.
		if (symbol) {
			symbol->addChartData(chart_data->GetDataKey(), chart_data);
		}
	}
}

SmChartData* SmChartDataManager::AddChartData(SmChartDataRequest data_req)
{
	SmChartData* chartData = FindChartData(data_req.GetDataKey());
	if (!chartData) {
		chartData = new SmChartData();
		chartData->SymbolCode(data_req.symbolCode);
		chartData->ChartType(data_req.chartType);
		chartData->Cycle(data_req.cycle);
		_ChartDataMap[data_req.GetDataKey()] = chartData;
		VtSymbol* symbol = VtSymbolManager::GetInstance()->FindHdSymbol(data_req.symbolCode);
		// 심볼에 차트 데이터를 추가해 준다. 
		// 종가 업데이트를 위해서 꼭 필요하다.
		if (symbol) {
			symbol->addChartData(data_req.GetDataKey(), chartData);
		}
	}

	return chartData;
}

SmChartData* SmChartDataManager::AddChartData(SmChartDataItem data_item)
{
	SmChartData* chartData = FindChartData(data_item.GetDataKey());
	if (!chartData) {
		chartData = new SmChartData();
		chartData->SymbolCode(data_item.symbolCode);
		chartData->ChartType(data_item.chartType);
		chartData->Cycle(data_item.cycle);
		_ChartDataMap[data_item.GetDataKey()] = chartData;

		VtSymbol* symbol = VtSymbolManager::GetInstance()->FindHdSymbol(data_item.symbolCode);
		// 심볼에 차트 데이터를 추가해 준다. 
		// 종가 업데이트를 위해서 꼭 필요하다.
		if (symbol) {
			symbol->addChartData(data_item.GetDataKey(), chartData);
		}
	}

	return chartData;
}

SmChartData* SmChartDataManager::AddChartData(std::string symbol_code, int chart_type, int cycle)
{
	std::string data_key = SmChartData::MakeDataKey(symbol_code, chart_type, cycle);
	SmChartData* chartData = FindChartData(data_key);
	if (!chartData) {
		chartData = new SmChartData();
		chartData->SymbolCode(symbol_code);
		chartData->ChartType((SmChartType)chart_type);
		chartData->Cycle(cycle);
		_ChartDataMap[data_key] = chartData;
		VtSymbol* symbol = VtSymbolManager::GetInstance()->FindHdSymbol(symbol_code);
		// 심볼에 차트 데이터를 추가해 준다. 
		// 종가 업데이트를 위해서 꼭 필요하다.
		if (symbol) {
			symbol->addChartData(data_key, chartData);
		}
	}

	return chartData;
}

void SmChartDataManager::RegisterTimer(SmChartData* chartData)
{
	if (!chartData)
		return;

	auto it = _CycleDataReqTimerMap.find(chartData->GetDataKey());
	// 이미 등록되어 있으면 등록하지 않는다.
	if (it != _CycleDataReqTimerMap.end())
		return;

	// 주기 데이터는 분 데이터만 처리한다. 
	if (chartData->ChartType() != SmChartType::MIN)
		return;
	// 대기시간 
	int waitTime = 0;
	int cycle_time = chartData->Cycle() * 60;
	// 주기가 60분을 넘으면 60분마다 갱신한다.
	if (chartData->Cycle() > 60) {
		cycle_time = 60 * 60;
	}
	else {
		std::vector<int> date_time = SmUtil::GetLocalDateTime();
		int cur_min = date_time[4];
		int total_seconds = cur_min * 60 + date_time[5];
		int mod_seconds = total_seconds % (chartData->Cycle() * 60);
		int wait_seconds = chartData->Cycle() * 60 - mod_seconds;
		waitTime = wait_seconds;
	}

	// 주기를 초로 환산해서 대입한다.
	// Add to the timer.
	auto id = _Timer.add(seconds(waitTime), std::bind(&SmChartData::OnTimer, chartData), seconds(cycle_time));
	// Add to the request map.
	_CycleDataReqTimerMap[chartData->GetDataKey()] = id;
}

SmChartData* SmChartDataManager::FindChartData(std::string data_key)
{
	auto it = _ChartDataMap.find(data_key);
	if (it != _ChartDataMap.end()) {
		return it->second;
	}

	return nullptr;
}

