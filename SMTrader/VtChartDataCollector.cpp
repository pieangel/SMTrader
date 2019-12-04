#include "stdafx.h"
#include "VtChartDataCollector.h"
#include "VtSymbolManager.h"
#include "VtSymbol.h"
#include "VtChartDataManager.h"
#include "VtChartData.h"
#include <time.h>
#include <chrono>
#include <random>
#include <iostream>
#include <memory>
#include <functional>
#include <list>
#include "VtRealtimeRegisterManager.h"
#include "System/VtSystemManager.h"
#include "VtGlobal.h"


using namespace std::chrono;
VtChartDataCollector::VtChartDataCollector()
{
	_TimerIdMap.clear();
	VtTime curTime = VtGlobal::GetLocalTime();
	int waitTime = 60 - curTime.sec;
	_TimerId = _Timer.add(seconds(waitTime - 1), [this](CppTime::timer_id) { OnTimer(); }, seconds(60));
}


VtChartDataCollector::~VtChartDataCollector()
{
	_Timer.remove(_TimerId);
}

void VtChartDataCollector::OnData(std::string code, double value)
{

}

void VtChartDataCollector::OnData(std::string code, int value)
{
	VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
	VtChartData* chartData = chartDataMgr->FindNAdd(code, VtChartType::MIN, 1);
}

void VtChartDataCollector::OnSingleData(std::string code, std::string time, int value)
{
	VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
	VtChartData* chartData = chartDataMgr->FindNAdd(code, VtChartType::MIN, 1);
	SetChartData(chartData, std::stoi(time), value);
}

VtChartData* VtChartDataCollector::FindAddChartData(std::string symCode)
{
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindSymbol(symCode);
	if (!sym) {
		AddSymbol(symCode);
	}

	VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
	std::pair<bool, VtChartData*> it = chartDataMgr->Add(symCode, VtChartType::MIN, 1);
	if (std::get<0>(it)) {
		VtChartData* chartData = std::get<1>(it);
	}
	return std::get<1>(it);
}

void VtChartDataCollector::OnTimer(int timerTime, VtChartData* chartData)
{
	if (!chartData)
		return;

	int lastIndex = ChartDataSize - 1;
	std::vector<double>& timeData = chartData->GetDataArray(_T("time"));
	std::vector<double>& openData = chartData->GetDataArray(_T("open"));
	std::vector<double>& highData = chartData->GetDataArray(_T("high"));
	std::vector<double>& lowData = chartData->GetDataArray(_T("low"));
	std::vector<double>& closeData = chartData->GetDataArray(_T("close"));

	int curTime = timerTime;
	int oldTime = (int)timeData[lastIndex];
	int curHourMin = VtChartDataCollector::GetHourMin(curTime);
	int oldHourMin = VtChartDataCollector::GetHourMin(oldTime);
	if (curHourMin != oldHourMin) {
		VtChartData::ShiftLeft(timeData, 1);
		VtChartData::ShiftLeft(openData, 1);
		VtChartData::ShiftLeft(highData, 1);
		VtChartData::ShiftLeft(lowData, 1);
		VtChartData::ShiftLeft(closeData, 1);

		timeData[lastIndex] = curHourMin * 100.0;
		openData[lastIndex] = openData[lastIndex - 1];
		highData[lastIndex] = highData[lastIndex - 1];
		lowData[lastIndex] = lowData[lastIndex - 1];
		closeData[lastIndex] = closeData[lastIndex - 1];
	}
}

void VtChartDataCollector::OnTimer()
{
	for (auto it = _ChartMap.begin(); it != _ChartMap.end(); ++it) {
		VtChartData* data = it->second;
		OnTimerEvent(data);
	}
}

void VtChartDataCollector::OnTimerEvent(VtChartData* chartData)
{
	if (!chartData)
		return;

	int lastIndex = ChartDataSize - 1;
	std::vector<double>& timeData = chartData->GetDataArray(_T("time"));
	int curTime = GetLocalTime();
	int openTime = VtChartDataCollector::GetLocalTime(VtGlobal::OpenTime, chartData->Cycle());
	int closeTime = VtChartDataCollector::GetLocalTime(VtGlobal::CloseTime, chartData->Cycle());
	// 영업시간에 따른 통제
	if (curTime < openTime || curTime > closeTime)
		return;
	int oldTime = (int)timeData.back();
	int curHourMin = VtChartDataCollector::GetHourMin(curTime, chartData->Cycle());
	int oldHourMin = VtChartDataCollector::GetHourMin(oldTime, chartData->Cycle());
	if (oldTime == 0) {
		return;
	} 

	if (curHourMin > oldHourMin) {
		std::vector<double>& timeData = chartData->GetDataArray(_T("time"));
		std::vector<double>& openData = chartData->GetDataArray(_T("open"));
		std::vector<double>& highData = chartData->GetDataArray(_T("high"));
		std::vector<double>& lowData = chartData->GetDataArray(_T("low"));
		std::vector<double>& closeData = chartData->GetDataArray(_T("close"));
		std::vector<double>& dateData = chartData->GetDataArray(_T("date"));
		std::vector<double>& datetimeData = chartData->GetDataArray(_T("datetime"));
		std::vector<double>& volume = chartData->GetDataArray(_T("volume"));

		VtChartData::ShiftLeft(dateData, 1);
		VtChartData::ShiftLeft(datetimeData, 1);
		VtChartData::ShiftLeft(volume, 1);
		VtChartData::ShiftLeft(timeData, 1);
		VtChartData::ShiftLeft(openData, 1);
		VtChartData::ShiftLeft(highData, 1);
		VtChartData::ShiftLeft(lowData, 1);
		VtChartData::ShiftLeft(closeData, 1);
		chartData->ShiftIdLeft(1);

		dateData[lastIndex] = GetLocalDate();
		timeData[lastIndex] = curHourMin * 100.0;
		openData[lastIndex] = closeData[lastIndex - 1];
		highData[lastIndex] = closeData[lastIndex - 1];
		lowData[lastIndex] = closeData[lastIndex - 1];
		closeData[lastIndex] = closeData[lastIndex - 1];

		//LOG_F(INFO, _T("ChartData : symcode : %s, time = %d,  o = %d, h = %d, l = %d, c = %d"), chartData->SymbolCode().c_str(), (int)timeData[lastIndex],(int)openData[lastIndex], (int)highData[lastIndex], (int)lowData[lastIndex], (int)closeData[lastIndex]);
	}
}

void VtChartDataCollector::AddSymbol(std::string code)
{
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindAddSymbol(code);
	sym->ShortCode = code;
	//symMgr->AddHdSymbol(sym);
}

void VtChartDataCollector::SetChartData(VtChartData* chartData, int time, double value)
{
	if (!chartData)
		return;
	int lastIndex = ChartDataSize - 1;
	std::vector<double>& timeData = chartData->GetDataArray(_T("time"));
	std::vector<double>& openData = chartData->GetDataArray(_T("open"));
	std::vector<double>& highData = chartData->GetDataArray(_T("high"));
	std::vector<double>& lowData = chartData->GetDataArray(_T("low"));
	std::vector<double>& closeData = chartData->GetDataArray(_T("close"));
	std::vector<double>& dateData = chartData->GetDataArray(_T("date"));
	std::vector<double>& datetimeData = chartData->GetDataArray(_T("datetime"));
	std::vector<double>& volume = chartData->GetDataArray(_T("volume"));

	int curTime = time;
	int oldTime = (int)timeData[lastIndex];
	int curHourMin = VtChartDataCollector::GetHourMin(curTime, chartData->Cycle());
	int oldHourMin = VtChartDataCollector::GetHourMin(oldTime, chartData->Cycle());
	int oldDate = (int)dateData[lastIndex];
	int curDate = VtChartDataCollector::GetLocalDate();
	//int temp = curHourMin * 100;

	if (chartData->ChartType() == VtChartType::MIN) {
		if (oldTime == 0) { // 이전 시간이 비어 있는 경우
			// 최신데이터를 대입해 준다.
			dateData[lastIndex] = curDate;
			timeData[lastIndex] = curHourMin * 100.0;
			openData[lastIndex] = value;
			highData[lastIndex] = value;
			lowData[lastIndex] = value;
			closeData[lastIndex] = value;
		}
		else { // 이전 시간이 비어 있지 않은 경우
			// 날짜가 다를 경우
			if (oldDate != curDate) {
				// 모든 데이터를 한칸 왼쪽으로 밀어 준다.
				VtChartData::ShiftLeft(dateData, 1);
				VtChartData::ShiftLeft(datetimeData, 1);
				VtChartData::ShiftLeft(volume, 1);
				VtChartData::ShiftLeft(timeData, 1);
				VtChartData::ShiftLeft(openData, 1);
				VtChartData::ShiftLeft(highData, 1);
				VtChartData::ShiftLeft(lowData, 1);
				VtChartData::ShiftLeft(closeData, 1);
				chartData->ShiftIdLeft(1);

				// 최신데이터를 대입해 준다.
				dateData[lastIndex] = curDate;
				timeData[lastIndex] = curHourMin * 100.0;
				openData[lastIndex] = value;
				highData[lastIndex] = value;
				lowData[lastIndex] = value;
				closeData[lastIndex] = value;
			} 
			else {
				if (curHourMin > oldHourMin) { // 분이 바뀐경우
					VtChartData::ShiftLeft(dateData, 1);
					VtChartData::ShiftLeft(datetimeData, 1);
					VtChartData::ShiftLeft(volume, 1);
					VtChartData::ShiftLeft(timeData, 1);
					VtChartData::ShiftLeft(openData, 1);
					VtChartData::ShiftLeft(highData, 1);
					VtChartData::ShiftLeft(lowData, 1);
					VtChartData::ShiftLeft(closeData, 1);
					chartData->ShiftIdLeft(1);

					// 최신데이터를 대입해 준다.
					dateData[lastIndex] = curDate;
					timeData[lastIndex] = curHourMin * 100.0;
					openData[lastIndex] = value;
					highData[lastIndex] = value;
					lowData[lastIndex] = value;
					closeData[lastIndex] = value;
				}
				else { // 분이 바뀌지 않은 경우
					if (value > highData[lastIndex])
						highData[lastIndex] = value;
					// 저가가 바뀌었다면 갱신
					if (value < lowData[lastIndex])
						lowData[lastIndex] = (double)value;
					// 현재가(종가) 갱신
					closeData[lastIndex] = (double)value;
				}
			}
		}
	}
	else if (chartData->ChartType() != VtChartType::TICK) {
		// 고가가 바뀌었다면 갱신
		if (value > highData[lastIndex])
			highData[lastIndex] = value;
		// 저가가 바뀌었다면 갱신
		if (value < lowData[lastIndex])
			lowData[lastIndex] = (double)value;
		// 현재가(종가) 갱신
		closeData[lastIndex] = (double)value;
	}

	VtSystemManager* sysMgr = VtSystemManager::GetInstance();
	sysMgr->UpdateRealtimeArgs(chartData);
	sysMgr->UpdateSystem(lastIndex);
}

void VtChartDataCollector::OnReceiveData(std::string symCode, int time, double value)
{
	auto it = _ChartMap.find(symCode);
	if (it != _ChartMap.end()) {
		VtChartData* chartData = it->second;
		SetChartData(chartData, time, value);
	}
}

void VtChartDataCollector::RegisterChartData(std::string symCode)
{
	VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
	std::pair<bool, VtChartData*> it = chartDataMgr->Add(symCode, VtChartType::MIN, 1);
	VtChartData* chartData = std::get<1>(it);
	if (chartData)
		_ChartMap[chartData->GetChartDataKey()] = chartData;
}

void VtChartDataCollector::RegisterChartData(VtChartData* chartData)
{
	if (!chartData)
		return;
	_ChartMap[chartData->GetChartDataKey()] = chartData;
}

void VtChartDataCollector::RegisterProduct(VtSymbol* sym)
{
	if (!sym)
		return;
	std::string symCode = sym->ShortCode;

	VtRealtimeRegisterManager* realRegMgr = VtRealtimeRegisterManager::GetInstance();
	// 실시간 등록을 해준다.
	realRegMgr->RegisterProduct(symCode);

	std::string code = symCode;
	RegisterChartData(symCode);
	code = symCode + (_T("SHTQ"));
	RegisterChartData(code);
	code = symCode + (_T("BHTQ"));
	RegisterChartData(code);
	code = symCode + (_T("SHTC"));
	RegisterChartData(code);
	code = symCode + (_T("BHTC"));
	RegisterChartData(code);
}

int VtChartDataCollector::GetLocalDate()
{
	time_t now = time(0);
	tm timeinfo;
	localtime_s(&timeinfo, &now);

	int timeNo = (timeinfo.tm_year + 1900) * 10000;
	timeNo += (timeinfo.tm_mon + 1) * 100;
	timeNo += timeinfo.tm_mday;

	return timeNo;
}

int VtChartDataCollector::GetLocalTime()
{
	time_t now = time(0);
	tm timeinfo;
	localtime_s(&timeinfo, &now);

	int timeNo = timeinfo.tm_hour * 10000;
	timeNo += timeinfo.tm_min * 100;
	timeNo += timeinfo.tm_sec;

	return timeNo;
}

int VtChartDataCollector::GetLocalTime(int cycle)
{
	time_t now = time(0);
	tm timeinfo;
	localtime_s(&timeinfo, &now);

	int timeNo = timeinfo.tm_hour * 10000;
	timeNo += (timeinfo.tm_min - (timeinfo.tm_min % cycle)) * 100;
	timeNo += timeinfo.tm_sec;

	return timeNo;
}

int VtChartDataCollector::GetLocalTime(VtTime givenTime, int cycle)
{
	int timeNo = givenTime.hour * 10000;
	timeNo += (givenTime.min - (givenTime.min % cycle)) * 100;
	timeNo += givenTime.sec;

	return timeNo;
}

int VtChartDataCollector::GetHourMin(int curTime)
{
	return (int)(curTime / 100);
}

int VtChartDataCollector::GetHourMin(int curTime, int cycle)
{
	int hourMin = (int)(curTime / 100);
	return hourMin - hourMin % cycle;
}
