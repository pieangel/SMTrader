#include "stdafx.h"
#include "VtChannelCrossSystem.h"
#include "VtSignal.h"
#include "../VtChartData.h"
VtChannelCrossSystem::VtChannelCrossSystem()
{
	ChannelPeriod = 24;
	AtrPeriod = 20;
	AtrMul = 3;
	BarsSinceE = 24;
	_CustomName = _T("Trend Flollower2");
	_Name = _T("ChannelCrossSystem");
	InitArgs();
}


VtChannelCrossSystem::VtChannelCrossSystem(VtSystemType type)
	:VtSystem(type)
{
	ChannelPeriod = 24;
	AtrPeriod = 20;
	AtrMul = 3;
	BarsSinceE = 24;
	_Name = _T("ChannelCrossSystem");
	_CustomName = _T("Trend Flollower2");
	InitArgs();
}

VtChannelCrossSystem::VtChannelCrossSystem(VtSystemType systemType, std::string name)
	:VtSystem(systemType, name)
{
	ChannelPeriod = 24;
	AtrPeriod = 20;
	AtrMul = 3;
	BarsSinceE = 24;
	_Name = _T("ChannelCrossSystem");
	_CustomName = _T("Trend Flollower2");
	InitArgs();
}

VtChannelCrossSystem::~VtChannelCrossSystem()
{
}

void VtChannelCrossSystem::InitArgs()
{
	VtSystem::InitArgs();

	VtSystemArg arg;

	arg.Name = _T("ChannelPeriod");
	arg.Type = VtParamType::INT;
	arg.iValue = ChannelPeriod;
	arg.Desc = _T("채널값을 설정 합니다.");
	AddSystemArg(_T("평균값 설정"), arg);

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
}

void VtChannelCrossSystem::CreateSignal(int startIndex, int endIndex)
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
			signal->DateTime(_DateTime[i]);
			signal->Date(_Date[i]);
			signal->Time(_Time[i]);
			signal->SignalType(sigType);
			signal->Price(closeArray[i]);
			SignalVector.push_back(signal);
			oldSigType = sigType;
		}
	}
}

VtPositionType VtChannelCrossSystem::UpdateSignal(int index)
{
	double close = 0.0, highest = 0.0, lowest = 0.0, ma20 = 0.0;
	double* closeArray = DataMap[_T("close")];
	double* highArray = DataMap[_T("high")];
	double* lowArray = DataMap[_T("low")];
	if (!closeArray || !highArray || !lowArray)
		return VtPositionType::None;
	close = closeArray[index];
	if (index + 1 < ChannelPeriod)
		return VtPositionType::None;

	GetHLChannelValue(index, ChannelPeriod, highArray, lowArray, highest, lowest);

	if (close > highest)
	{
		_BarSinceEntry = 0;
		_LastSignalType = VtPositionType::Buy;
		return VtPositionType::Buy;
	}
	if (close < lowest)
	{
		_BarSinceEntry = 0;
		_LastSignalType = VtPositionType::ExitBuy;
		return VtPositionType::ExitBuy;
	}

	_BarSinceEntry++;
	if (_BarSinceEntry >= BarsSinceE)
	{
		if (_LastSignalType == VtPositionType::Buy)
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

		if (_LastSignalType == VtPositionType::Sell)
		{
			double lastLowest = GetLowest(index, _BarSinceEntry, lowArray);
			double atr = GetAtr(index, AtrPeriod, highArray, lowArray, closeArray);
			if (close > (lastLowest - atr * AtrMul))
			{
				_BarSinceEntry = -1;
				_LastSignalType = VtPositionType::ExitSell;
				return VtPositionType::ExitSell;
			}
		}
	}

	return VtPositionType::None;
}

VtPositionType VtChannelCrossSystem::UpdateSignal(int start, int end)
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

void VtChannelCrossSystem::SetDataMap(VtChartData* chartData)
{
	VtSystem::SetDataMap(chartData);

	if (!_AppliedData)
	{
		//DateTime(chartData->DateTime.data());
		AddDataSource(_T("close"), chartData->Close.data());
		AddDataSource(_T("high"), chartData->High.data());
		AddDataSource(_T("low"), chartData->Low.data());
		AddDataSource(_T("timestamp"), chartData->DateTime.data());
		CreateSignal(0, chartData->GetDataCount() - 1);
		_AppliedData = true;
		_Running = true;
	}
}

void VtChannelCrossSystem::SaveToXml(pugi::xml_node& node)
{
	VtSystem::SaveToXml(node);

	/*
		int ChannelPeriod;
	int AtrPeriod;
	int AtrMul;
	int BarsSinceE;*/

	CString msg;
	std::string value;

	msg.Format(_T("%d"), ChannelPeriod);
	value = (LPCTSTR)msg;
	pugi::xml_node channel_period = node.append_child("channel_period");
	channel_period.append_child(pugi::node_pcdata).set_value(value.c_str());

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
}

void VtChannelCrossSystem::LoadFromXml(pugi::xml_node& node)
{
	VtSystem::LoadFromXml(node);

	std::string channel_period = node.child_value("channel_period");
	ChannelPeriod = std::stoi(channel_period);

	std::string atr_period = node.child_value("atr_period");
	AtrPeriod = std::stoi(atr_period);

	std::string atr_mul = node.child_value("atr_mul");
	AtrMul = std::stoi(atr_mul);

	std::string barsince_e = node.child_value("barsince_e");
	BarsSinceE = std::stoi(barsince_e);
}

void VtChannelCrossSystem::ReloadSystem(int startIndex, int endIndex)
{
	ClearSignal();
	CreateSignal(startIndex, endIndex);
}
