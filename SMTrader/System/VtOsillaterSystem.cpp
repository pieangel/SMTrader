#include "stdafx.h"
#include "VtOsillaterSystem.h"
#include "VtSignal.h"
#include "../VtChartData.h"
VtOsillaterSystem::VtOsillaterSystem()
{
	ShortPeriod = 12;
	LongPeriod = 26;
	SignalPeriod = 9;

	FastK = 12;
	SlowK = 5;
	SlowD = 5;

	AtrPeriod = 20;
	AtrMul = 3;
	BarsSinceE = 3;

	ExitMA = 20;
	_Name = _T("OsillaterSystem");
	_CustomName = _T("VRobo 1");
	InitArgs();
}


VtOsillaterSystem::VtOsillaterSystem(VtSystemType systemType)
	:VtSystem(systemType)
{
	ShortPeriod = 12;
	LongPeriod = 26;
	SignalPeriod = 9;

	FastK = 12;
	SlowK = 5;
	SlowD = 5;

	AtrPeriod = 20;
	AtrMul = 3;
	BarsSinceE = 3;

	ExitMA = 20;
	_Name = _T("OsillaterSystem");
	_CustomName = _T("VRobo 1");
	InitArgs();
}

VtOsillaterSystem::VtOsillaterSystem(VtSystemType systemType, std::string name)
	: VtSystem(systemType, name)
{
	ShortPeriod = 12;
	LongPeriod = 26;
	SignalPeriod = 9;

	FastK = 12;
	SlowK = 5;
	SlowD = 5;

	AtrPeriod = 20;
	AtrMul = 3;
	BarsSinceE = 3;

	ExitMA = 20;
	_Name = _T("OsillaterSystem");
	_CustomName = _T("VRobo 1");
	InitArgs();
}

void VtOsillaterSystem::InitArgs()
{
	VtSystem::InitArgs();

	VtSystemArg arg;

	arg.Name = _T("FastK");
	arg.Type = VtParamType::INT;
	arg.iValue = FastK;
	arg.Desc = _T("FastK를 설정 합니다.");
	AddSystemArg(_T("스토캐스틱값 설정"), arg);

	arg.Name = _T("SlowK");
	arg.Type = VtParamType::INT;
	arg.iValue = SlowK;
	arg.Desc = _T("SlowK를 설정 합니다.");
	AddSystemArg(_T("스토캐스틱값 설정"), arg);

	arg.Name = _T("SlowD");
	arg.Type = VtParamType::INT;
	arg.iValue = SlowD;
	arg.Desc = _T("SlowD를 설정 합니다.");
	AddSystemArg(_T("스토캐스틱값 설정"), arg);

	arg.Name = _T("ShortPeriod");
	arg.Type = VtParamType::INT;
	arg.iValue = ShortPeriod;
	arg.Desc = _T("단기평균값을 설정 합니다.");
	AddSystemArg(_T("평균값 설정"), arg);

	arg.Name = _T("LongPeriod");
	arg.Type = VtParamType::INT;
	arg.iValue = LongPeriod;
	arg.Desc = _T("장기평균값을 설정 합니다.");
	AddSystemArg(_T("평균값 설정"), arg);

	arg.Name = _T("SignalPeriod");
	arg.Type = VtParamType::INT;
	arg.iValue = SignalPeriod;
	arg.Desc = _T("SignalPeriod를 설정 합니다.");
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

	arg.Name = _T("ExitMA");
	arg.Type = VtParamType::INT;
	arg.iValue = ExitMA;
	arg.Desc = _T("ExitMA를 설정 합니다.");
	AddSystemArg(_T("청산요소 설정"), arg);

	arg.Name = _T("BarsSinceE");
	arg.Type = VtParamType::INT;
	arg.iValue = BarsSinceE;
	arg.Desc = _T("BarsSinceE를 설정 합니다.");
	AddSystemArg(_T("진입요소 설정"), arg);
}

void VtOsillaterSystem::CreateSignal(int startIndex, int endIndex)
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

VtPositionType VtOsillaterSystem::UpdateSignal(int index)
{
	double macd = 0, macdSignal = 0, macdHist = 0.0, close = 0.0;
	double slowK = 0, slowD = 0, ma20 = 0.0;

	double* closeArray = DataMap[_T("close")];
	double* highArray = DataMap[_T("high")];
	double* lowArray = DataMap[_T("low")];
	if (!closeArray || !highArray || !lowArray)
		return VtPositionType::None;
	close = closeArray[index];
	if (index + 1 < LongPeriod)
		return VtPositionType::None;
	if (index + 1 < FastK)
		return VtPositionType::None;

	GetMacd(index, closeArray, ShortPeriod, LongPeriod, SignalPeriod, macd, macdSignal, macdHist);
	GetStochastic(index, highArray, lowArray, closeArray, FastK, SlowK, SlowD, slowK, slowD);
	ma20 = GetMA(index, closeArray, TA_MAType::TA_MAType_SMA, ExitMA);

	if (macdSignal > macd && slowK > slowD)
	{
		_BarSinceEntry = 0;
		_LastSignalType = VtPositionType::Buy;
		return VtPositionType::Buy;
	}
	if (macdSignal < macd && slowK < slowD)
	{
		if (close < ma20)
		{
			_BarSinceEntry = 0;
			_LastSignalType = VtPositionType::ExitBuy;
			return VtPositionType::ExitBuy;
		}
	}

	_BarSinceEntry++;
	if (_BarSinceEntry >= BarsSinceE)
	{
		if (_LastSignalType == VtPositionType::Buy)
		{
			
			if (close < ma20)
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
			
			/*
			if (close < ma20)
			{
				_BarSinceEntry = -1;
				_LastSignalType = VtPositionType::ExitBuy;
				return VtPositionType::ExitBuy;
			}
			*/
		}

		/*
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
		*/
	}
	
	return VtPositionType::None;
}

VtPositionType VtOsillaterSystem::UpdateSignal(int start, int end)
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

void VtOsillaterSystem::SetDataMap(VtChartData* chartData)
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

void VtOsillaterSystem::SaveToXml(pugi::xml_node& node)
{
	VtSystem::SaveToXml(node);

	/*
	int ShortPeriod;
	int LongPeriod;
	int SignalPeriod;

	int FastK;
	int SlowK;
	int SlowD;

	int AtrPeriod;
	int AtrMul;
	int BarsSinceE;

	int ExitMA;*/


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

	msg.Format(_T("%d"), SignalPeriod);
	value = (LPCTSTR)msg;
	pugi::xml_node signal_period = node.append_child("signal_period");
	signal_period.append_child(pugi::node_pcdata).set_value(value.c_str());


	msg.Format(_T("%d"), FastK);
	value = (LPCTSTR)msg;
	pugi::xml_node fastk = node.append_child("fastk");
	fastk.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), SlowK);
	value = (LPCTSTR)msg;
	pugi::xml_node slowk = node.append_child("slowk");
	slowk.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), SlowD);
	value = (LPCTSTR)msg;
	pugi::xml_node slowd = node.append_child("slowd");
	slowd.append_child(pugi::node_pcdata).set_value(value.c_str());


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

	msg.Format(_T("%d"), ExitMA);
	value = (LPCTSTR)msg;
	pugi::xml_node exit_ma = node.append_child("exit_ma");
	exit_ma.append_child(pugi::node_pcdata).set_value(value.c_str());
}

void VtOsillaterSystem::LoadFromXml(pugi::xml_node& node)
{
	VtSystem::LoadFromXml(node);

	std::string short_period = node.child_value("short_period");
	ShortPeriod = std::stoi(short_period);

	std::string long_period = node.child_value("long_period");
	LongPeriod = std::stoi(long_period);

	std::string signal_period = node.child_value("signal_period");
	SignalPeriod = std::stoi(signal_period);

	std::string fastk = node.child_value("fastk");
	FastK = std::stoi(fastk);

	std::string slowk = node.child_value("slowk");
	SlowK = std::stoi(slowk);

	std::string slowd = node.child_value("slowd");
	SlowD = std::stoi(slowd);


	std::string atr_period = node.child_value("atr_period");
	AtrPeriod = std::stoi(atr_period);

	std::string atr_mul = node.child_value("atr_mul");
	AtrMul = std::stoi(atr_mul);

	std::string barsince_e = node.child_value("barsince_e");
	BarsSinceE = std::stoi(barsince_e);

	std::string exit_ma = node.child_value("exit_ma");
	ExitMA = std::stoi(exit_ma);
}

void VtOsillaterSystem::ReloadSystem(int startIndex, int endIndex)
{
	ClearSignal();
	CreateSignal(startIndex, endIndex);
}

VtOsillaterSystem::~VtOsillaterSystem()
{
	
}
