#include "stdafx.h"
#include "VtUsComp.h"
#include "System/VtSignal.h"
#include "VtChartData.h"
#include "VtChartDataManager.h"
#include "VtProductCategoryManager.h"
#include "VtSymbol.h"
#include "VtChartDataCollector.h"
#include <algorithm>
#include "VtRealtimeRegisterManager.h"
#include "VtUsdStrategyConfigDlg.h"
#include "VtPosition.h"
#include "VtAccount.h"
#include "VtFund.h"
#include "VtCutManager.h"

VtUsComp::VtUsComp()
	:VtSystem()
{
	InitArgs();
}


VtUsComp::VtUsComp(VtSystemType type)
	:VtSystem(type)
{
	InitArgs();
}

VtUsComp::VtUsComp(VtSystemType systemType, std::string name)
	: VtSystem(systemType, name)
{
	InitArgs();
}

VtUsComp::~VtUsComp()
{
	VtRealtimeRegisterManager* realRegiMgr = VtRealtimeRegisterManager::GetInstance();
	for (auto it = _DataSrcSymbolVec.begin(); it != _DataSrcSymbolVec.end(); ++it) {
		realRegiMgr->UnregisterProduct(*it);
	}
}

/// <summary>
/// �ý��ۿ� �ʿ��� �����͸� ����� �ش�. 
/// �׸��� ���� ��Ͽ��� �ǽð����� �����͸� ������ �� �ֵ��� 
/// ��Ʈ ������ �÷��Ϳ� ��û�� �Ѵ�.
/// </summary>
void VtUsComp::SetDataSrc()
{
	
}

void VtUsComp::InitArgs()
{
	// �̹� �Ű������� �ε� �ƴٸ� ���̻� �о� ������ �ʴ´�.
	if (_ArgsLoaded)
		return;

	VtSystem::InitArgs();

	_Cycle = 5;

	_EntranceStartTime.hour = 9;
	_EntranceStartTime.min = 5;
	_EntranceStartTime.sec = 0;

	_EntranceEndTime.hour = 15;
	_EntranceEndTime.min = 0;
	_EntranceEndTime.sec = 0;

	_LiqTime.hour = 15;
	_LiqTime.min = 29;
	_LiqTime.sec = 0;

	_MaxEntrance = 3;

	_EntryBarIndex = 0;
	_ATRTime.hour = 14;
	_ATRTime.min = 30;
	_ATRTime.sec = 0;
	_ATR = 20;

	_ATRMulti = 2.0;
	_BandMulti = 0.333;
	_FilterMulti = 3.0;

	VtSystemArg arg;

	arg.Name = _T("Ubc>Uac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.6");
	arg.Enable = true;
	arg.Desc = _T("Ubc>Uac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);

	arg.Name = _T("Ubs>Uas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
	arg.Enable = true;
	arg.Desc = _T("Ubs>Uas ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);


	arg.Name = _T("Kac>Kbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kac>Kbc ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);

	arg.Name = _T("Kas>Kbs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kas>Kbs ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);

	arg.Name = _T("Uac>Ubc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.6");
	arg.Enable = true;
	arg.Desc = _T("Uac-Ubc ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("Uas>Ubs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
	arg.Enable = true;
	arg.Desc = _T("Uas-Ubs ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("Kbc>Kac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("Kbs>Kas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbs>Kas ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("Uac>Ubc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = true;
	arg.Desc = _T("Uac-Ubc ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�û��"), arg);

	arg.Name = _T("Uas>Ubs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Uas-Ubs ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�û��"), arg);


	arg.Name = _T("Kbc>Kac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�û��"), arg);

	arg.Name = _T("Kbs>Kas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbs>Kas ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�û��"), arg);


	arg.Name = _T("Ubc>Uac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = true;
	arg.Desc = _T("Ubc>Uac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�û��"), arg);

	arg.Name = _T("Ubs>Uas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Ubs>Uas ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�û��"), arg);

	arg.Name = _T("Kac>Kbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�û��"), arg);

	arg.Name = _T("Kas>Kbs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbs>Kas ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�û��"), arg);

	arg.Name = _T("ATR");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("20");
	arg.Enable = false;
	arg.Desc = _T("ATR ���� ���� �մϴ�.");
	AddSystemArg(_T("��Ÿ����"), arg);

	arg.Name = _T("ATR Time");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("9:00");
	arg.Enable = false;
	arg.Desc = _T("ATR Time���� ���� �մϴ�.");
	AddSystemArg(_T("��Ÿ����"), arg);

	arg.Name = _T("ATRMulti");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("2");
	arg.Enable = false;
	arg.Desc = _T("ATRMulti ���� ���� �մϴ�.");
	AddSystemArg(_T("��Ÿ����"), arg);

	arg.Name = _T("BandMulti");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.25");
	arg.Enable = false;
	arg.Desc = _T("BandMulti ���� ���� �մϴ�.");
	AddSystemArg(_T("��Ÿ����"), arg);

	arg.Name = _T("FilterMulti");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("3");
	arg.Enable = false;
	arg.Desc = _T("FilterMulti ���� ���� �մϴ�.");
	AddSystemArg(_T("��Ÿ����"), arg);

	arg.Name = _T("EntryBarIndex");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("EntryBarIndex ���� ���� �մϴ�.");
	AddSystemArg(_T("��Ÿ����"), arg);

	arg.Name = _T("c>o,c<o");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("c>o,c<o ���� ���� �մϴ�.");
	AddSystemArg(_T("��Ÿ����"), arg);
}

void VtUsComp::CreateSignal(int startIndex, int endIndex)
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

VtPositionType VtUsComp::UpdateSignal(int start, int end)
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

void VtUsComp::SetDataMap(VtChartData* chartData)
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

void VtUsComp::SaveToXml(pugi::xml_node& node)
{
	VtSystem::SaveToXml(node);

	CString msg;
	std::string value;


}

void VtUsComp::LoadFromXml(pugi::xml_node& node)
{
	VtSystem::LoadFromXml(node);


}

/// <summary>
/// �ǽð� üũ �Լ�
/// ���⼭ ����, ����, Ʈ���ϸ�
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
VtPositionType VtUsComp::UpdateSignal(int index)
{
	// �ý��� ������Ʈ
	UpdateSystem(index);

	_ExpPosition = VtPositionType::None;

	// û�� �ð��� ���� û�� Ȯ��
	if (LiqByEndTime(index)) {
		_CurPosition = VtPositionType::None;
		return _ExpPosition;
	}

	// ���� Ȯ��
	if (CheckLossCut(index)) {
		LOG_F(INFO, _T("��������"));
		_CurPosition = VtPositionType::None;
	}
	// ��ǥ���� Ȯ��
	if (CheckProfitCut(index)) {
		LOG_F(INFO, _T("��������"));
		_CurPosition = VtPositionType::None;
	}
	// Ʈ���ϸ� ��ž Ȯ��
	if (CheckTrailStop(index)) {
		LOG_F(INFO, _T("Ʈ���Ͻ�ž����"));
		_CurPosition = VtPositionType::None;
	}

	return _ExpPosition;
}

void VtUsComp::OnTimer()
{
	if (!_Enable || !_Symbol)
		return;
	// û�� �ð��� ���� û�� - ���Ǿ��� ������ û���Ѵ�.
	if (_CurPosition != VtPositionType::None) {
		if (LiqByEndTime()) {
			_CurPosition = VtPositionType::None;
			return;
		}
	}
	// �����ǿ� ���� û��
	// �ż��� �� û�� ���� Ȯ��
	if (_CurPosition == VtPositionType::Buy) {
		if (CheckLiqForBuy() && LiqudAll()) {
			LOG_F(INFO, _T("�ż�û�꼺��"));
			_CurPosition = VtPositionType::None;
		}
	}

	// �ŵ��� �� û�� ���� Ȯ��
	if (_CurPosition == VtPositionType::Sell) {
		if (CheckLiqForSell() && LiqudAll()) {
			LOG_F(INFO, _T("�ŵ�û�꼺��"));
			_CurPosition = VtPositionType::None;
		}
	}

	

	if (_CurPosition == VtPositionType::None) {
		// ���� �ִ� �ŷ�ȸ���� ���� ����
		if (_EntryToday >= _MaxEntrance) { // ���� �ִ� �ŷ� ȸ���� �����ߴٸ� �������� �ʴ´�.
			return;
		}

		// �ð��� ���� ���� ����
		if (!IsEnterableByTime())
			return;

		// ���ϸ� �ε����� ���� ����
		if (_EnableBarIndex) {
			if (!CheckBarIndex())
				return;
		}
		// ������ ���� ����
		if (_EnableFilterMulti) {
			if (!CheckFilterMulti())
				return;
		}
		int curTime = VtChartDataCollector::GetLocalTime();
		if (CheckEntranceForBuy()) {
			LOG_F(INFO, _T("�ż����Լ���"));
			// ������ ����
			_CurPosition = VtPositionType::Buy;
			// ���⼭ �ֹ��� ����. - �ϴ� ���尡�� ����.
			PutOrder(0, _CurPosition, VtPriceType::Market);
			if (_Symbol) // ���� �ֱ��� ���԰��� ����� ���´�.
				_LatestEntPrice = _Symbol->Quote.intClose;
			int curHourMin = VtChartDataCollector::GetHourMin(curTime, _Cycle);
			// ���� �ֱ� ��ȣ�� �߻��� �ð��� ������ �д�.
			_LastEntryTime = curHourMin * 100;
			// ����ȸ���� �÷��ش�.
			_EntryToday++;
		}

		// �ŵ� ���� ���� Ȯ��
		if (CheckEntranceForSell()) {
			LOG_F(INFO, _T("�ŵ����Լ���"));
			// ������ ����
			_CurPosition = VtPositionType::Sell;
			// ���⼭ �ֹ��� ����.
			PutOrder(0, _CurPosition, VtPriceType::Market);
			if (_Symbol) // ���� �ֱ��� ���԰��� ����� ���´�.
				_LatestEntPrice = _Symbol->Quote.intClose;
			int curHourMin = VtChartDataCollector::GetHourMin(curTime, _Cycle);
			// ���� �ֱ� ��ȣ�� �߻��� �ð��� ������ �д�.
			_LastEntryTime = curHourMin * 100;
			// ����ȸ���� �÷��ش�.
			_EntryToday++;
		}
	}
}

void VtUsComp::UpdateSystem(int index)
{
	VtSystem::UpdateSystem(index);
}

void VtUsComp::ReadExtraArgs()
{
	VtSystem::ReadExtraArgs();
}

void VtUsComp::ReloadSystem(int startIndex, int endIndex)
{
	ClearSignal();
	CreateSignal(startIndex, endIndex);
}

bool VtUsComp::CheckEntranceForBuy()
{
	std::vector<bool> argCond;

	argCond.push_back(CheckCondition(_T("�ż�����")));

	if (_EnableByBand) {
		// ��忡 ���� ������ ���� Ȯ���Ѵ�.
		argCond.push_back(CheckEntranceByBandForBuy());
	}


	if (argCond.size() == 0)
		return false;

	// �ϳ��� �����̶� �����̸� ��ȣ ����. ��ΰ� ���̸� �ż� ��ȯ
	auto it = std::find(argCond.begin(), argCond.end(), false);
	if (it != argCond.end())
		return false;
	else
		return true;
}

bool VtUsComp::CheckEntranceForBuy(size_t index)
{
	std::vector<bool> argCond;

	argCond.push_back(CheckCondition(_T("�ż�����"), index));

	if (_EnableByBand) {
		// ��忡 ���� ������ ���� Ȯ���Ѵ�.
		argCond.push_back(CheckEntranceByBandForBuy(index));
	}


	if (argCond.size() == 0)
		return false;

	// �ϳ��� �����̶� �����̸� ��ȣ ����. ��ΰ� ���̸� �ż� ��ȯ
	auto it = std::find(argCond.begin(), argCond.end(), false);
	if (it != argCond.end())
		return false;
	else
		return true;
}

bool VtUsComp::CheckEntranceForSell()
{
	std::vector<bool> argCond;

	argCond.push_back(CheckCondition(_T("�ŵ�����")));

	if (_EnableByBand) {
		// ��忡 ���� ������ ���� Ȯ���Ѵ�.
		argCond.push_back(CheckEntranceByBandForSell());
	}


	if (argCond.size() == 0)
		return false;

	// �ϳ��� �����̶� �����̸� ��ȣ ����. ��ΰ� ���̸� �ż� ��ȯ
	auto it = std::find(argCond.begin(), argCond.end(), false);
	if (it != argCond.end())
		return false;
	else
		return true;
}

bool VtUsComp::CheckEntranceForSell(size_t index)
{
	std::vector<bool> argCond;

	argCond.push_back(CheckCondition(_T("�ŵ�����"), index));

	if (_EnableByBand) {
		// ��忡 ���� ������ ���� Ȯ���Ѵ�.
		argCond.push_back(CheckEntranceByBandForBuy(index));
	}


	if (argCond.size() == 0)
		return false;

	// �ϳ��� �����̶� �����̸� ��ȣ ����. ��ΰ� ���̸� �ż� ��ȯ
	auto it = std::find(argCond.begin(), argCond.end(), false);
	if (it != argCond.end())
		return false;
	else
		return true;
}

bool VtUsComp::CheckLiqForSell()
{
	std::vector<bool> argCond;

	argCond.push_back(CheckCondition(_T("�ŵ�û��")));

	if (_EnableATRLiq) {
		argCond.push_back(CheckAtrLiqForSell());
	}


	if (argCond.size() == 0)
		return false;

	auto it = std::find(argCond.begin(), argCond.end(), false);
	if (it != argCond.end())
		return false;
	else
		return true;
}

bool VtUsComp::CheckLiqForSell(size_t index)
{
	std::vector<bool> argCond;

	argCond.push_back(CheckCondition(_T("�ŵ�û��"), index));

	if (_EnableATRLiq) {
		argCond.push_back(CheckAtrLiqForSell(index));
	}


	if (argCond.size() == 0)
		return false;

	auto it = std::find(argCond.begin(), argCond.end(), false);
	if (it != argCond.end())
		return false;
	else
		return true;
}

bool VtUsComp::CheckLiqForBuy()
{
	std::vector<bool> argCond;

	argCond.push_back(CheckCondition(_T("�ż�û��")));

	if (_EnableATRLiq) {
		argCond.push_back(CheckAtrLiqForBuy());
	}


	if (argCond.size() == 0)
		return false;

	auto it = std::find(argCond.begin(), argCond.end(), false);
	if (it != argCond.end())
		return false;
	else
		return true;
}

bool VtUsComp::CheckLiqForBuy(size_t index)
{
	std::vector<bool> argCond;

	argCond.push_back(CheckCondition(_T("�ż�û��"), index));

	if (_EnableATRLiq) {
		argCond.push_back(CheckAtrLiqForBuy(index));
	}


	if (argCond.size() == 0)
		return false;

	auto it = std::find(argCond.begin(), argCond.end(), false);
	if (it != argCond.end())
		return false;
	else
		return true;
}
