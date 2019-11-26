#include "stdafx.h"
#include "VtChartData.h"
#include "Poco/Format.h"
#include "VtChartDataRequest.h"
#include "VtHdClient.h"
#include "VtChartFrm.h"
#include "System/VtSystem.h"
#include "VtChartDataManager.h"
#include "VtChartWindow.h"
#include "VtSymbol.h"
#include "VtGlobal.h"

using Poco::format;

VtChartData::VtChartData()
{
	_CurIndex = 0;
	_CyclicRequestCount = 4;
	_FirstRequestCount = ChartDataSize;
	_FilledCount = 0;
	_Domestic = true;
	_Decimal = 2;
	_Exchange = _T("CME");
	Reset();
	InitChartData();
}


VtChartData::~VtChartData()
{
	TRACE(_T("VtChartData destroyed!"));
}

void VtChartData::Reset()
{
	_CurIndex = 0;
	memset(Id.data(), 0x00, sizeof(Id));
	memset(High.data(), 0x00, sizeof(High));
	memset(Low.data(), 0x00, sizeof(Low));
	memset(Open.data(), 0x00, sizeof(Open));
	memset(Close.data(), 0x00, sizeof(Close));
	memset(Volume.data(), 0x00, sizeof(Volume));
	memset(Date.data(), 0x00, sizeof(Date));
	memset(Time.data(), 0x00, sizeof(Time));
	memset(TimeInfo.data(), 0x00, sizeof(Time));
	memset(DateTime.data(), 0x00, sizeof(DateTime));
}

void VtChartData::ShiftLeft(int count)
{
	if (count < 0) return;
	std::memmove(Id.data(), Id.data() + count, sizeof(Id[0]) * (ChartDataSize - count));
	std::memmove(High.data(), High.data() + count, sizeof(High[0]) * (ChartDataSize - count));
	std::memmove(Low.data(), Low.data() + count, sizeof(Low[0]) * (ChartDataSize - count));
	std::memmove(Open.data(), Open.data() + count, sizeof(Open[0]) * (ChartDataSize - count));
	std::memmove(Close.data(), Close.data() + count, sizeof(Close[0]) * (ChartDataSize - count));
	std::memmove(Volume.data(), Volume.data()+ count, sizeof(Volume[0]) * (ChartDataSize - count));
	std::memmove(Date.data(), Date.data() + count, sizeof(Date[0]) * (ChartDataSize - count));
	std::memmove(Time.data(), Time.data() + count, sizeof(Time[0]) * (ChartDataSize - count));
	std::memmove(DateTime.data(), DateTime.data() + count, sizeof(DateTime[0]) * (ChartDataSize - count));
	std::memmove(TimeInfo.data(), TimeInfo.data() + count, sizeof(TimeInfo[0]) * (ChartDataSize - count));
}

void VtChartData::ShiftLeft(std::vector<double>& src, int count)
{
	if (count <= 0) return;
	std::memmove(src.data(), src.data() + count, sizeof(src[0]) * (ChartDataSize - count));
}

void VtChartData::ShiftIdLeft(int count)
{
	if (count <= 0) return;
	std::memmove(Id.data(), Id.data() + count, sizeof(Id[0]) * (ChartDataSize - count));
}

int VtChartData::FindIndex(VtDate date, VtTime time)
{
	for (int i = ChartDataSize - 1; i >= 0; --i)
	{
		if (Date[i].year == date.year &&
			Date[i].month == date.month &&
			Date[i].day == date.day &&
			Time[i].hour == time.hour &&
			Time[i].min == time.min &&
			Time[i].sec == time.sec &&
			Time[i].mil == time.mil)
			return i;
	}

	return -1;
}

VtDate VtChartData::GetDate(char* src)
{
	VtDate date;
	char buff[16];
	memset(buff, 0x00, sizeof(buff));
	memcpy(buff, src, 4);
	date.year = std::atoi(buff);
	memset(buff, 0x00, sizeof(buff));
	memcpy(buff, src + 4, 2);
	date.month = std::atoi(buff);
	memset(buff, 0x00, sizeof(buff));
	memcpy(buff, src + 6, 2);
	date.day = std::atoi(buff);
	return date;
}

VtDate VtChartData::GetDate(CString src)
{
	VtDate date;
	date.year = _ttoi(src.Mid(0, 4));
	date.month = _ttoi(src.Mid(4, 2));
	date.day = _ttoi(src.Mid(6, 2));
	return date;
}

VtTime VtChartData::GetTime(char* src)
{
	VtTime time;
	char buff[16];
	memset(buff, 0x00, sizeof(buff));
	memcpy(buff, src, 2);
	time.hour = std::atoi(buff);
	memset(buff, 0x00, sizeof(buff));
	memcpy(buff, src + 3, 2);
	time.min = std::atoi(buff);
	memset(buff, 0x00, sizeof(buff));
	memcpy(buff, src + 6, 2);
	time.sec = std::atoi(buff);
	time.mil = 0;

	return time;
}

VtTime VtChartData::GetTimeForAbroad(CString src)
{
	VtTime time;
	time.hour = _ttoi(src.Mid(0, 2));
	time.min = _ttoi(src.Mid(2, 2));
	time.sec = _ttoi(src.Mid(4, 2));
	time.mil = _ttoi(src.Mid(6, 4));
	return time;
}

std::pair<VtDate, VtTime> VtChartData::GetDateTime(CString datetime)
{
	VtDate date;
	date.year = _ttoi(datetime.Mid(0, 4));
	date.month = _ttoi(datetime.Mid(4, 2));
	date.day = _ttoi(datetime.Mid(6, 2));
	VtTime time;
	time.hour = _ttoi(datetime.Mid(8, 2));
	time.min = _ttoi(datetime.Mid(10, 2));

	return std::make_pair(date, time);
}

std::string VtChartData::MakeID(std::string fullCode, VtChartType type, int cycle)
{
	std::string id = fullCode;
	id.append(std::to_string((int)type));
	std::string strCycle = format("%3d", cycle);
	id.append(strCycle);
	return id;
}

void VtChartData::OnTimer()
{
	RequestChartData();
}

std::string VtChartData::GetChartDataKey()
{
	std::string key = _SymbolCode;
	key.append(":");
	key.append(std::to_string((int)_ChartType));
	key.append(":");
	key.append(std::to_string(_Cycle));
	return key;
}

void VtChartData::RequestChartData()
{
	VtChartDataRequest req;

	req.msgid = "0000000000";
	req.cycle = _Cycle;
	req.count = _FilledCount < ChartDataSize ? _FirstRequestCount : _CyclicRequestCount;
	req.chartType = _ChartType;
	req.productCode = _SymbolCode;
	req.next = 0;
	req.reqKey = 0;
	req.seq = 0;
	req.domestic = _Domestic;
	req.dataKey = GetChartDataKey();

	VtHdClient* client = VtHdClient::GetInstance();
	if (_Domestic)
		client->GetChartData(std::move(req));
	else
		client->AbGetChartData(std::move(req));
}

void VtChartData::AddDataByRecent(int rcvIndex, ChartDataSet& data)
{
	int insertIndex = ChartDataSize - 1 - rcvIndex;
	if (insertIndex >= 0)
	{
		High[insertIndex] = data.High;
		Low[insertIndex] = data.Low;
		Open[insertIndex] = data.Open;
		Close[insertIndex] = data.Close;
		Volume[insertIndex] = data.Volume;
		TimeInfo[insertIndex] = data.TimeInfo;
	}
}

void VtChartData::AdjustData(int foundIndex)
{
	if (foundIndex == 0)
		return;
	ShiftLeft(foundIndex);
	
}

void VtChartData::CopyData(int rcvCnt, int foundIndex)
{
	if (rcvCnt == 0)
		return;
	int totalCnt = _FilledCount + rcvCnt;
	if (totalCnt <= ChartDataSize) {
		if (_FilledCount != 0)
			ShiftLeft(rcvCnt);

		int dstIndex = ChartDataSize - rcvCnt;
		int startId = _FilledCount <= 0 ? 0 : Id[ChartDataSize - 1];
		for (int i = 0; i < rcvCnt; i++) {
			Id[dstIndex + i] = startId + i;
			High[dstIndex + i] = InputChartData.High[i];
			Low[dstIndex + i] = InputChartData.Low[i];
			Open[dstIndex + i] = InputChartData.Open[i];
			Close[dstIndex + i] = InputChartData.Close[i];
			Volume[dstIndex + i] = InputChartData.Volume[i];
			TimeInfo[dstIndex + i] = InputChartData.TimeInfo[i];
			Date[dstIndex + i] = InputChartData.Date[i];
			Time[dstIndex + i] = InputChartData.Time[i];
			DateTime[dstIndex + i] = InputChartData.DateTime[i];

			std::vector<double>& highArray = _DataCol[_T("high")];
			highArray.data()[dstIndex + i] = High[dstIndex + i];
			std::vector<double>& lowArray = _DataCol[_T("low")];
			lowArray.data()[dstIndex + i] = Low[dstIndex + i];
			std::vector<double>& openArray = _DataCol[_T("open")];
			openArray.data()[dstIndex + i] = Open[dstIndex + i];
			std::vector<double>& closeArray = _DataCol[_T("close")];
			closeArray.data()[dstIndex + i] = Close[dstIndex + i];
			std::vector<double>& volumeArray = _DataCol[_T("volume")];
			volumeArray.data()[dstIndex + i] = Volume[dstIndex + i];
			std::vector<double>& dateArray = _DataCol[_T("date")];
			dateArray.data()[dstIndex + i] = VtGlobal::GetDate(Date[dstIndex + i]);
			std::vector<double>& timeArray = _DataCol[_T("time")];
			timeArray.data()[dstIndex + i] = VtGlobal::GetTime(Time[dstIndex + i]);
		}
	}
	else {
		int shiftCount = 0;
		int srcIndex = 0;
		if (foundIndex == rcvCnt) {
			shiftCount = rcvCnt;
		}
		else {
			shiftCount = rcvCnt - (foundIndex + 1);
			srcIndex = foundIndex + 1;
		}
		if (shiftCount != 0)
			ShiftLeft(shiftCount);
		else
			return;
		int dstIndex = _FilledCount - shiftCount;
		int startId = _FilledCount <= 0 ? 0 : (Id[_FilledCount - 1] + 1);
		for (int i = srcIndex, j = 0; i < rcvCnt; i++, j++) {
			Id[dstIndex + j] = startId + j;
			High[dstIndex + j] = InputChartData.High[i];
			Low[dstIndex + j] = InputChartData.Low[i];
			Open[dstIndex + j] = InputChartData.Open[i];
			Close[dstIndex + j] = InputChartData.Close[i];
			Volume[dstIndex + j] = InputChartData.Volume[i];
			TimeInfo[dstIndex + j] = InputChartData.TimeInfo[i];
			Date[dstIndex + j] = InputChartData.Date[i];
			Time[dstIndex + j] = InputChartData.Time[i];
			DateTime[dstIndex + j] = InputChartData.DateTime[i];
		}
	}
	_FilledCount = (totalCnt > ChartDataSize) ? ChartDataSize : totalCnt;
}

void VtChartData::OnReceiveQuoteHd(VtSymbol* sym)
{
	if (!sym)
		return;
	if (_Filled) {
		double close = sym->Quote.intClose/std::pow(10, sym->Decimal);
		_RealTimeClose = close;
		VtChartDataItem item = GetChartData(ChartDataSize - 1);
		item.Close = close;
		SetChartData(ChartDataSize - 1, item);
	}
	// broad cast to all the chart windows using this data.
	for (auto it = _ChartMap.begin(); it != _ChartMap.end(); ++it) {
		VtChartWindow* wnd = it->second;
		wnd->OnReceiveQuoteHd(sym);
	}
}

void VtChartData::OnReceiveChartData(VtChartData* data)
{
	if (!data || data != this)
		return;
	// For the real time value.
	if (_FilledCount > 0)
		_RealTimeClose = InputChartData.Close[ChartDataSize - 1];
	for (auto it = _ChartMap.begin(); it != _ChartMap.end(); ++it) {
		VtChartWindow* wnd = it->second;
		wnd->OnReceiveChartData(this);
	}
}

void VtChartData::AddSystem(VtSystem* sys)
{
	if (!sys)
		return;
	_SystemMap[sys->Id()] = sys;
}

void VtChartData::RemoveSystem(VtSystem* sys)
{
	if (!sys)
		return;
	auto it = _SystemMap.find(sys->Id());
	if (it != _SystemMap.end())
	{
		_SystemMap.erase(it);
	}
}


void VtChartData::AddChartWindow(VtChartWindow* chartWnd)
{
	if (!chartWnd)
		return;
	_ChartMap[chartWnd->Id()] = chartWnd;
}

void VtChartData::RemoveChartWindow(VtChartWindow* chartWnd)
{
	if (!chartWnd)
		return;

	auto it = _ChartMap.find(chartWnd->Id());
	if (it != _ChartMap.end())
	{
		_ChartMap.erase(it);

		if (_ChartMap.size() == 0)
		{
			VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
			// Remove the request of chart data from the timer.
			chartDataMgr->RemoveChartDataRequest(this);
		}
	}
}

void VtChartData::RealTimeClose(double val)
{
	_RealTimeClose = val;
	Close[_DataCount - 1] = val;
}

void VtChartData::CopyPrevData(int index)
{
	if (index < _DataCount)
		return;

	for (int i = _DataCount - 1; i <= index; i++)
	{
		High[index] = High[i - 1];
		Low[index] = Low[i - 1];
		Open[index] = Open[i - 1];
		Close[index] = Close[i - 1];
		Volume[index] = Volume[i - 1];
	}
}

int VtChartData::FindIndex(double dateTime)
{
	return 0;
}

void VtChartData::OnUpdateDataCount(int oldCount, int newCount)
{
	int i = 0;
	_DataCount = newCount;
}

void VtChartData::SaveToXml(pugi::xml_node& node)
{
	CString msg;
	std::string value;

	pugi::xml_node symbol = node.append_child("symbol");
	symbol.append_child(pugi::node_pcdata).set_value(SymbolCode().c_str());

	pugi::xml_node exchange = node.append_child("exchange");
	exchange.append_child(pugi::node_pcdata).set_value(Exchange().c_str());

	msg.Format(_T("%d"), (int)ChartType());
	value = (LPCTSTR)msg;
	pugi::xml_node chart_type = node.append_child("chart_type");
	chart_type.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), Cycle());
	value = (LPCTSTR)msg;
	pugi::xml_node chart_cycle = node.append_child("chart_cycle");
	chart_cycle.append_child(pugi::node_pcdata).set_value(value.c_str());


	pugi::xml_node chart_currency = node.append_child("chart_currency");
	chart_currency.append_child(pugi::node_pcdata).set_value(Currency().c_str());
}

std::string VtChartData::LoadFromXml(pugi::xml_node& node)
{
	std::string symbol = node.child_value("symbol");
	std::string exchange = node.child_value("exchange");
	std::string chart_type = node.child_value("chart_type");

	std::string chart_cycle = node.child_value("chart_cycle");
	std::string chart_currency = node.child_value("chart_currency");

	VtChartDataRequest req;

	req.msgid = "0000000000";
	req.type = VtChartRequestType::HISTORY;
	req.cycle = std::stoi(chart_cycle);
	req.count = 1000;
	req.chartType = (VtChartType)std::stoi(chart_type);
	req.currency = chart_currency;
	req.productCode = symbol;
	req.exchange = exchange;
	req.requestAll = true;

	std::string newKey = VtChartDataManager::MakeChartDataKey(exchange, symbol, req.chartType, req.cycle);
	
	return newKey;
}

std::vector<double>& VtChartData::GetDataArray(std::string dataName)
{
	return _DataCol[dataName];
}

VtChartDataItem VtChartData::GetChartData(int index)
{
	VtChartDataItem item;
	item.Open = _DataCol[_T("open")].at(index);
	item.High = _DataCol[_T("high")].at(index);
	item.Low = _DataCol[_T("low")].at(index);
	item.Close = _DataCol[_T("close")].at(index);
	item.Volume = _DataCol[_T("volume")].at(index);
	item.DateTime = _DataCol[_T("datetime")].at(index);

	return item;
}

VtChartDataItem VtChartData::GetInputChartData(int index)
{
	VtChartDataItem item;
	item.Open = InputChartData.Open.at(index);
	item.High = InputChartData.High.at(index);
	item.Low = InputChartData.Low.at(index);
	item.Close = InputChartData.Close.at(index);
	item.Volume = InputChartData.Volume.at(index);
	item.DateTime = InputChartData.DateTime.at(index);

	return item;
}

void VtChartData::SetChartData(int index, VtChartDataItem item)
{
	_DataCol[_T("open")][index] = item.Open;
	_DataCol[_T("high")][index] = item.High;
	_DataCol[_T("low")][index] = item.Low;
	_DataCol[_T("close")][index] = item.Close;
	_DataCol[_T("volume")][index] = item.Volume;
	_DataCol[_T("datetime")][index] = item.DateTime;
}

void VtChartData::SetFirstData()
{
	if (InputDateTimeMap.size() == 0)
		return;
	// 먼저 초기화를 진행한다.
	for (int i = 0; i < ChartDataSize; ++i) {
		int index = i;
		VtChartDataItem item;
		SetChartData(index, item);
	}

	for (auto it = InputDateTimeMap.begin(); it != InputDateTimeMap.end(); ++it) {
		int index = it->second;
		VtChartDataItem item = GetInputChartData(index);
		SetChartData(index, item);
		SyncDateTimeMap[it->first] = index;
	}
}

void VtChartData::OnReceiveFirstChartData(VtChartData* data)
{
	if (!data || data != this)
		return;
	// For the real time value.
	if (_FilledCount > 0)
		_RealTimeClose = InputChartData.Close[ChartDataSize - 1];
	for (auto it = _ChartMap.begin(); it != _ChartMap.end(); ++it) {
		VtChartWindow* wnd = it->second;
		wnd->OnReceiveFirstChartData(this);
	}
}

/*
std::array<double, ChartDataSize> High;
	std::array<double, ChartDataSize> Low;
	std::array<double, ChartDataSize> Open;
	std::array<double, ChartDataSize> Close;
	std::array<double, ChartDataSize> Volume;
	std::array<double, ChartDataSize> TimeInfo;
	// Items under is for the chart display.
	std::array<VtDate, ChartDataSize> Date;
	std::array<VtTime, ChartDataSize> Time;
	std::array<double, ChartDataSize> DateTime;
*/
void VtChartData::InitChartData()
{
	std::vector<double> data(ChartDataSize);
	_DataCol[_T("open")] = data;
	_DataCol[_T("high")] = data;
	_DataCol[_T("low")] = data;
	_DataCol[_T("close")] = data;
	_DataCol[_T("volume")] = data;
	_DataCol[_T("date")] = data;
	_DataCol[_T("time")] = data;
	_DataCol[_T("datetime")] = data;
}

void VtChartData::SendMessageToChartWnd(bool real)
{
	for (auto it = _ChartMap.begin(); it != _ChartMap.end(); ++it) {
		VtChartWindow* wnd = it->second;

	}
}

