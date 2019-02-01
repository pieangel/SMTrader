#include "stdafx.h"
#include "VtDailyHeightSystem.h"
#include "VtSignal.h"
#include "../VtChartData.h"
VtDailyHeightSystem::VtDailyHeightSystem()
{
	Dividance = 4;
	StartHour = 9;
	AtrPeriod = 20;
	AtrMul = 3;

	BarsSinceE = 24;
	ExitPercent = 1;
	EntryToday = 2;
	BackLen = 1;
	_Name = _T("DailyHeightSystem");
	_CustomName = _T("Daily Height시스템");
	InitArgs();
}


VtDailyHeightSystem::VtDailyHeightSystem(VtSystemType systemType)
	:VtSystem(systemType)
{
	Dividance = 8;
	StartHour = 9;
	AtrPeriod = 20;
	AtrMul = 3;

	BarsSinceE = 24;
	ExitPercent = 1;
	EntryToday = 2;
	BackLen = 1;
	_Name = _T("DailyHeightSystem");
	_CustomName = _T("Daily Height시스템");
	InitArgs();
}

VtDailyHeightSystem::VtDailyHeightSystem(VtSystemType systemType, std::string name)
	:VtSystem(systemType, name)
{
	Dividance = 8;
	StartHour = 9;
	AtrPeriod = 20;
	AtrMul = 3;

	BarsSinceE = 24;
	ExitPercent = 1;
	EntryToday = 2;
	BackLen = 1;
	_Name = _T("DailyHeightSystem");
	_CustomName = _T("Daily Height시스템");
	InitArgs();
}

VtDailyHeightSystem::~VtDailyHeightSystem()
{
}

void VtDailyHeightSystem::InitArgs()
{
	VtSystem::InitArgs();

	VtSystemArg arg;

	arg.Name = _T("Dividance");
	arg.Type = VtParamType::INT;
	arg.iValue = Dividance;
	arg.Desc = _T("Dividance를 설정 합니다.");
	AddSystemArg(_T("나눔값 설정"), arg);

	arg.Name = _T("StartHour");
	arg.Type = VtParamType::INT;
	arg.iValue = StartHour;
	arg.Desc = _T("StartHour를 설정 합니다.");
	AddSystemArg(_T("시작값 설정"), arg);

	arg.Name = _T("AtrPeriod");
	arg.Type = VtParamType::INT;
	arg.iValue = AtrPeriod;
	arg.Desc = _T("ATRPeriod를 설정 합니다.");
	AddSystemArg(_T("청산요소 설정"), arg);

	arg.Name = _T("AtrMulti");
	arg.Type = VtParamType::INT;
	arg.iValue = AtrMul;
	arg.Desc = _T("ATRMulti를 설정 합니다.");
	AddSystemArg(_T("청산요소 설정"), arg);

	arg.Name = _T("BarsSinceE");
	arg.Type = VtParamType::INT;
	arg.iValue = BarsSinceE;
	arg.Desc = _T("BarsSinceE를 설정 합니다.");
	AddSystemArg(_T("진입요소 설정"), arg);

	arg.Name = _T("ExitPercent");
	arg.Type = VtParamType::INT;
	arg.iValue = ExitPercent;
	arg.Desc = _T("ExitPercent를 설정 합니다.");
	AddSystemArg(_T("청산요소 설정"), arg);

	arg.Name = _T("EntryToday");
	arg.Type = VtParamType::INT;
	arg.iValue = EntryToday;
	arg.Desc = _T("EntryToday를 설정 합니다.");
	AddSystemArg(_T("진입요소 설정"), arg);
}

void VtDailyHeightSystem::CreateSignal(int startIndex, int endIndex)
{
	if (!_DateTime)
		return;
	VtPositionType sigType, oldSigType = VtPositionType::None;
	double* closeArray = DataMap[_T("close")];
	if (!closeArray)
		return;

	for (int i = 0; i <= endIndex; i++)
	{
		sigType = UpdateSignal(i);
		if (sigType != oldSigType && sigType != VtPositionType::None)
		{
			VtSignal* signal = new VtSignal();
			signal->Date(_Date[i]);
			signal->Time(_Time[i]);
			signal->DateTime(_DateTime[i]);
			signal->SignalType(sigType);
			signal->Price(closeArray[i]);
			SignalVector.push_back(signal);
			oldSigType = sigType;
		}
	}
}

VtPositionType VtDailyHeightSystem::UpdateSignal(int index)
{
	double close = 0.0, ma20 = 0.0;
	double* closeArray = DataMap[_T("close")];
	double* highArray = DataMap[_T("high")];
	double* lowArray = DataMap[_T("low")];
	double* openArray = DataMap[_T("open")];
	double* datetime = DataMap[_T("timestamp")];
	VtTime* time = _Time;
	VtDate* date = _Date;

	if (!closeArray || !highArray || !lowArray || !time || !datetime)
		return VtPositionType::None;

	int entryCount = EntriesToday(datetime[index]);


	

	close = closeArray[index];
	
	double preDayHeight = GetDailyHeight(index, datetime, highArray, lowArray, BackLen);
	if (preDayHeight < 0)
		return VtPositionType::None;

	double stepVal = preDayHeight / (double)Dividance;
	double openVal = GetOpen(index, datetime, time, StartHour, openArray);


	CString msg;
	msg.Format(_T("preDayHeight = %.2f, stepVal = %.2f, openVal = %.2f\n"), preDayHeight, stepVal, openVal);
	TRACE(msg);

	if (_LastSignalType == VtPositionType::Buy || _LastSignalType == VtPositionType::Sell)
		_BarSinceEntry++;

	/*
	// 손절
	if (_BarSinceEntry > 0)
	{
		double threshold = (_EntryPrice * (double)ExitPercent) / 100.0;;
		if (close < threshold)
		{
			_BarSinceEntry = 0;
			_LastSignalType = VtPositionType::ExitBuy;
			return VtPositionType::ExitBuy;
		}


		if (close < (openVal - stepVal))
		{
			_BarSinceEntry = 0;
			_LastSignalType = VtPositionType::ExitBuy;
			return VtPositionType::ExitBuy;
		}
	}
	*/
	// 청산
	if (_BarSinceEntry >= BarsSinceE && _LastSignalType == VtPositionType::Buy)
	{
		double lastHightest = GetHighest(index, _BarSinceEntry, highArray);
		double atr = GetAtr(index, AtrPeriod, highArray, lowArray, closeArray);
		if (close < (lastHightest - atr * AtrMul))
		{
			_BarSinceEntry = -1;
			_LastSignalType = VtPositionType::ExitBuy;
			return VtPositionType::ExitBuy;
		}
	}
	
	// 신호 발생
	if (openVal > 0.0 && close > (openVal + stepVal) && entryCount < EntryToday)
	{
		_BarSinceEntry = 0;
		_EntryPrice = close;
		_LastSignalType = VtPositionType::Buy;
		return VtPositionType::Buy;
	}

	return VtPositionType::None;
}

VtPositionType VtDailyHeightSystem::UpdateSignal(int start, int end)
{
	VtPositionType sigType = VtPositionType::None;
	VtPositionType oldSigType = _LastSignalType;
	double* closeArray = DataMap[_T("close")];
	if (!closeArray)
		return sigType;
	for (int i = start; i <= end; i++)
	{
		sigType = UpdateSignal(i);
		if (sigType != oldSigType && sigType != VtPositionType::None)
		{
			VtSignal* signal = new VtSignal();
			signal->DateTime(_DateTime[i]);
			signal->SignalType(sigType);
			signal->Price(closeArray[i]);
			SignalVector.push_back(signal);
			oldSigType = sigType;
		}
	}

	return sigType;
}

void VtDailyHeightSystem::SetDataMap(VtChartData* chartData)
{
	VtSystem::SetDataMap(chartData);

	if (!_AppliedData)
	{
		//DateTime(chartData->DateTime.data());
		Time(chartData->Time.data());
		Date(chartData->Date.data());
		AddDataSource(_T("close"), chartData->Close.data());
		AddDataSource(_T("high"), chartData->High.data());
		AddDataSource(_T("low"), chartData->Low.data());
		AddDataSource(_T("open"), chartData->Low.data());
		AddDataSource(_T("timestamp"), chartData->DateTime.data());
		CreateSignal(0, chartData->GetDataCount() - 1);
		_AppliedData = true;
		_Running = true;
	}
}

void VtDailyHeightSystem::SaveToXml(pugi::xml_node& node)
{
	VtSystem::SaveToXml(node);
	/*
	int Dividance;
	int StartHour;
	int AtrPeriod;
	int AtrMul;

	int BarsSinceE;
	int EntryToday;
	int ExitPercent;
	int BackLen;*/


	CString msg;
	std::string value;

	msg.Format(_T("%d"), Dividance);
	value = (LPCTSTR)msg;
	pugi::xml_node dividance = node.append_child("dividance");
	dividance.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), StartHour);
	value = (LPCTSTR)msg;
	pugi::xml_node start_hour = node.append_child("start_hour");
	start_hour.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), AtrPeriod);
	value = (LPCTSTR)msg;
	pugi::xml_node atr_period = node.append_child("atr_period");
	atr_period.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), AtrMul);
	value = (LPCTSTR)msg;
	pugi::xml_node atr_mul = node.append_child("atr_mul");
	atr_mul.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), BarsSinceE);
	value = (LPCTSTR)msg;
	pugi::xml_node barsince_e = node.append_child("barsince_e");
	barsince_e.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), ExitPercent);
	value = (LPCTSTR)msg;
	pugi::xml_node exit_percent = node.append_child("exit_percent");
	exit_percent.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), BackLen);
	value = (LPCTSTR)msg;
	pugi::xml_node backlen = node.append_child("backlen");
	backlen.append_child(pugi::node_pcdata).set_value(value.c_str());
}

void VtDailyHeightSystem::LoadFromXml(pugi::xml_node & node)
{
	VtSystem::LoadFromXml(node);

	std::string dividance = node.child_value("dividance");
	Dividance = std::stoi(dividance);

	std::string start_hour = node.child_value("start_hour");
	StartHour = std::stoi(start_hour);

	std::string atr_period = node.child_value("atr_period");
	AtrPeriod = std::stoi(atr_period);

	std::string atr_mul = node.child_value("atr_mul");
	AtrMul = std::stoi(atr_mul);

	std::string barsince_e = node.child_value("barsince_e");
	BarsSinceE = std::stoi(barsince_e);

	std::string exit_percent = node.child_value("exit_percent");
	ExitPercent = std::stoi(exit_percent);

	std::string backlen = node.child_value("backlen");
	BackLen = std::stoi(backlen);

}

void VtDailyHeightSystem::ReloadSystem(int startIndex, int endIndex)
{
	ClearSignal();
	CreateSignal(startIndex, endIndex);
}
