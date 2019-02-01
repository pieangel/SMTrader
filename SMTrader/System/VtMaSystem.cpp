#include "stdafx.h"
#include "VtMaSystem.h"
#include "VtSignal.h"
#include "../VtSystemArgGroup.h"
#include "../VtChartData.h"

VtMaSystem::VtMaSystem()
{
	ShortPeriod = 5;
	LongPeriod = 20;
	BigPeriod = 60;
	AtrMul = 3;
	AtrPeriod = 20;
	BarsSinceE = 24;
	_CustomName = _T("Trend Flollower1");
	_Name = _T("MaSystem");
	InitArgs();
}


VtMaSystem::VtMaSystem(VtSystemType systemType)
	:VtSystem(systemType)
{
	ShortPeriod = 5;
	LongPeriod = 20;
	BigPeriod = 60;
	AtrMul = 3;
	AtrPeriod = 20;
	BarsSinceE = 24;
	_Name = _T("MaSystem");
	_CustomName = _T("Trend Flollower2");
	InitArgs();
}

VtMaSystem::VtMaSystem(VtSystemType systemType, std::string name)
	: VtSystem(systemType, name)
{
	ShortPeriod = 5;
	LongPeriod = 20;
	BigPeriod = 60;
	AtrMul = 3;
	AtrPeriod = 20;
	BarsSinceE = 24;
	_Name = _T("MaSystem");
	_CustomName = _T("Trend Flollower2");
	InitArgs();
}

void VtMaSystem::InitArgs()
{
	VtSystem::InitArgs();

	VtSystemArg arg;
	
	arg.Name = _T("ShortPeriod");
	arg.Type = VtParamType::INT;
	arg.iValue = ShortPeriod;
	arg.Desc = _T("�ܱ���հ��� ���� �մϴ�.");
	AddSystemArg(_T("��հ� ����"), arg);

	arg.Name = _T("LongPeriod");
	arg.Type = VtParamType::INT;
	arg.iValue = LongPeriod;
	arg.Desc = _T("�����հ��� ���� �մϴ�.");
	AddSystemArg(_T("��հ� ����"), arg);

	arg.Name = _T("BigPeriod");
	arg.Type = VtParamType::INT;
	arg.iValue = BigPeriod;
	arg.Desc = _T("����հ��� ���� �մϴ�.");
	AddSystemArg(_T("��հ� ����"), arg);

	arg.Name = _T("AtrPeriod");
	arg.Type = VtParamType::INT;
	arg.iValue = AtrPeriod;
	arg.Desc = _T("ATRPeriod�� ���� �մϴ�.");
	AddSystemArg(_T("û���� ����"), arg);

	arg.Name = _T("AtrMulti");
	arg.Type = VtParamType::INT;
	arg.iValue = AtrMul;
	arg.Desc = _T("ATRMulti�� ���� �մϴ�.");
	AddSystemArg(_T("û���� ����"), arg);

	arg.Name = _T("BarsSinceE");
	arg.Type = VtParamType::INT;
	arg.iValue = BarsSinceE;
	arg.Desc = _T("BarsSinceE�� ���� �մϴ�.");
	AddSystemArg(_T("���Կ�� ����"), arg);
}

VtMaSystem::~VtMaSystem()
{
}

void VtMaSystem::CreateSignal(int startIndex, int endIndex)
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

VtPositionType VtMaSystem::UpdateSignal(int index)
{
	double ma5 = 0, ma20 = 0, ma60 = 0, close = 0.0;
	double* closeArray = DataMap[_T("close")];
	double* highArray = DataMap[_T("high")];
	double* lowArray = DataMap[_T("low")];
	double* datetime = _DateTime;
	if (!closeArray || !highArray || !lowArray || !datetime)
		return VtPositionType::None;
	close = closeArray[index];
	if (index + 1 < BigPeriod)
		return VtPositionType::None;

	int entryCount = EntriesToday(datetime[index]);

	CString msg;
	msg.Format(_T("entryCount = %d\n"), entryCount);
	//TRACE(msg);


	ma5 = GetMA(index, closeArray, TA_MAType::TA_MAType_SMA, ShortPeriod);
	ma20 = GetMA(index, closeArray, TA_MAType::TA_MAType_SMA, LongPeriod);
	ma60 = GetMA(index, closeArray, TA_MAType::TA_MAType_SMA, BigPeriod);

	if (close > ma5 && ma5 > ma20 && ma20 > ma60)
	{
		_BarSinceEntry = 0;
		_LastSignalType = VtPositionType::Buy;
		return VtPositionType::Buy;
	}
	if (close < ma5 && ma5 < ma20 && ma20 < ma60)
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

VtPositionType VtMaSystem::UpdateSignal(int start, int end)
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

void VtMaSystem::SetDataMap(VtChartData* chartData)
{
	VtSystem::SetDataMap(chartData);

	if (!_AppliedData)
	{
		//DateTime(chartData->DateTime.data());
		AddDataSource(_T("high"), chartData->High.data());
		AddDataSource(_T("low"), chartData->Low.data());
		AddDataSource(_T("close"), chartData->Close.data());
		CreateSignal(0, chartData->GetDataCount() - 1);
		_AppliedData = true;
		_Running = true;
	}
}

void VtMaSystem::SaveToXml(pugi::xml_node& node)
{
	VtSystem::SaveToXml(node);

	/*
	int ShortPeriod;
	int LongPeriod;
	int BigPeriod;

	int AtrPeriod;
	int AtrMul;
	int BarsSinceE;*/

	CString msg;
	std::string value;

	msg.Format(_T("%d"), ShortPeriod);
	value = (LPCTSTR)msg;
	pugi::xml_node short_period = node.append_child("short_period");
	short_period.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), LongPeriod);
	value = (LPCTSTR)msg;
	pugi::xml_node long_period = node.append_child("long_period");
	long_period.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), BigPeriod);
	value = (LPCTSTR)msg;
	pugi::xml_node big_period = node.append_child("big_period");
	big_period.append_child(pugi::node_pcdata).set_value(value.c_str());


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

void VtMaSystem::LoadFromXml(pugi::xml_node& node)
{
	VtSystem::LoadFromXml(node);

	std::string short_period = node.child_value("short_period");
	ShortPeriod = std::stoi(short_period);

	std::string long_period = node.child_value("long_period");
	LongPeriod = std::stoi(long_period);

	std::string big_period = node.child_value("big_period");
	BigPeriod = std::stoi(big_period);

	std::string atr_period = node.child_value("atr_period");
	AtrPeriod = std::stoi(atr_period);

	std::string atr_mul = node.child_value("atr_mul");
	AtrMul = std::stoi(atr_mul);

	std::string barsince_e = node.child_value("barsince_e");
	BarsSinceE = std::stoi(barsince_e);
}

void VtMaSystem::ReloadSystem(int startIndex, int endIndex)
{
	ClearSignal();
	CreateSignal(startIndex, endIndex);
}
