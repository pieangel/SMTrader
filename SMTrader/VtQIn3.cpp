#include "stdafx.h"
#include "VtQIn3.h"
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

VtQIn3::VtQIn3()
	:VtSystem()
{
	InitArgs();
}


VtQIn3::VtQIn3(VtSystemType type)
	:VtSystem(type)
{
	InitArgs();
}

VtQIn3::VtQIn3(VtSystemType systemType, std::string name)
	: VtSystem(systemType, name)
{
	InitArgs();
}

VtQIn3::~VtQIn3()
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
void VtQIn3::SetDataSrc()
{
	VtSystem::SetDataSrc();
}

void VtQIn3::InitArgs()
{
	// �̹� �Ű������� �ε� �ƴٸ� ���̻� �о� ������ �ʴ´�.
	if (_ArgsLoaded)
		return;


	_Cycle = 1;

	_EntranceStartTime.hour = 9;
	_EntranceStartTime.min = 50;
	_EntranceStartTime.sec = 0;

	_EntranceEndTime.hour = 15;
	_EntranceEndTime.min = 15;
	_EntranceEndTime.sec = 0;

	_LiqTime.hour = 15;
	_LiqTime.min = 24;
	_LiqTime.sec = 30;

	_MaxEntrance = 2;

	_EntryBarIndex = 0;
	_ATRTime.hour = 14;
	_ATRTime.min = 30;
	_ATRTime.sec = 0;
	_ATR = 20;

	_ATRMulti = 2.0;
	_BandMulti = 0.333;
	_FilterMulti = 3.0;

	VtSystemArg arg;

	arg.Name = _T("Qbc>Qac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.9");
	arg.Enable = true;
	arg.Desc = _T("Qbc>Qac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);

	arg.Name = _T("Qbs>Qas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.6");
	arg.Enable = true;
	arg.Desc = _T("Qbs>Qas ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);

	arg.Name = _T("Kbc>Kac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);

	arg.Name = _T("Uac>Ubc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Uac-Ubc ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);

	arg.Name = _T("Qac>Qbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.9");
	arg.Enable = true;
	arg.Desc = _T("Qac>Qbc ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("Qas>Qbs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.6");
	arg.Enable = true;
	arg.Desc = _T("Qas>Qbs ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("Kac>Kbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("Ubc>Uac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Ubc>Uac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("Qac>Qbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.8");
	arg.Enable = true;
	arg.Desc = _T("Qac>Qbc ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�û��"), arg);

	arg.Name = _T("Qas>Qbs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Qas>Qbs ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�û��"), arg);

	arg.Name = _T("Kac>Kbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�û��"), arg);

	arg.Name = _T("Ubc>Uac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Ubc>Uac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�û��"), arg);

	arg.Name = _T("Qbc>Qac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.8");
	arg.Enable = true;
	arg.Desc = _T("Qbc>Qac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�û��"), arg);

	arg.Name = _T("Qbs>Qas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Qbs>Qas ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�û��"), arg);

	arg.Name = _T("Kbc>Kac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�û��"), arg);

	arg.Name = _T("Uac>Ubc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Uac-Ubc ���� ���� �մϴ�.");
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
	arg.Enable = true;
	arg.Desc = _T("c>o,c<o ���� ���� �մϴ�.");
	AddSystemArg(_T("��Ÿ����"), arg);
	VtSystem::InitArgs();
}

void VtQIn3::CreateSignal(int startIndex, int endIndex)
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

VtPositionType VtQIn3::UpdateSignal(int start, int end)
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

void VtQIn3::SetDataMap(VtChartData* chartData)
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

void VtQIn3::SaveToXml(pugi::xml_node& node)
{
	VtSystem::SaveToXml(node);

	CString msg;
	std::string value;


}

void VtQIn3::LoadFromXml(pugi::xml_node& node)
{
	VtSystem::LoadFromXml(node);


}

/// <summary>
/// �ǽð� üũ �Լ�
/// ���⼭ ����, ����, Ʈ���ϸ�
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
VtPositionType VtQIn3::UpdateSignal(int index)
{
	// �ý��� ������Ʈ
	UpdateSystem(index);


	// ���� Ȯ��
	if (_CurPosition != VtPositionType::None && CheckLossCut(index)) {
		LOG_F(INFO, _T("��������"));
		_CurPosition = VtPositionType::None;
	}
	// ��ǥ���� Ȯ��
	if (_CurPosition != VtPositionType::None && CheckProfitCut(index)) {
		LOG_F(INFO, _T("��������"));
		_CurPosition = VtPositionType::None;
	}
	// Ʈ���ϸ� ��ž Ȯ��
	if (_CurPosition != VtPositionType::None && CheckTrailStop(index)) {
		LOG_F(INFO, _T("Ʈ���Ͻ�ž����"));
		_CurPosition = VtPositionType::None;
	}
	_ExpPosition = VtPositionType::None;

	// ���⼭ �����ȣ�� �˾ƺ���.

	return _ExpPosition;
}

void VtQIn3::OnTimer()
{
	if (!_Enable || !_Symbol)
		return;
	// ���� ���۽ð� 0�п� ������ ��ȣ�� �����Ѵ�.
	if (!CheckEntranceBar())
		return;
	// û�� �ð��� ���� û�� - ���Ǿ��� ������ û���Ѵ�.
	if (_CurPosition != VtPositionType::None) {
		if (LiqByEndTime()) {
			_CurPosition = VtPositionType::None;
			_LastEntryDailyIndex = -1;
			return;
		}
	}
	// �����ǿ� ���� û��
	// �ż��� �� û�� ���� Ȯ��
	if (_CurPosition == VtPositionType::Buy) {
		if (CheckLiqForBuy() && LiqudAll()) {
			LOG_F(INFO, _T("�ż�û�꼺��"));
			_CurPosition = VtPositionType::None;
			_LastEntryDailyIndex = -1;
		}
	}

	// �ŵ��� �� û�� ���� Ȯ��
	if (_CurPosition == VtPositionType::Sell) {
		if (CheckLiqForSell() && LiqudAll()) {
			LOG_F(INFO, _T("�ŵ�û�꼺��"));
			_CurPosition = VtPositionType::None;
			_LastEntryDailyIndex = -1;
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
			// ���⼭ �ֹ��� ����.
			if (!PutEntranceOrder(_CurPosition)) {
				_CurPosition = VtPositionType::None;
				return;
			}
			int curHourMin = VtChartDataCollector::GetHourMin(curTime, _Cycle);
			// ���� �ֱ� ��ȣ�� �߻��� �ð��� ������ �д�.
			_LastEntryTime = curHourMin * 100;
			_LastEntryDailyIndex = GetDailyIndex();
			// ����ȸ���� �÷��ش�.
			_EntryToday++;
		}

		// �ŵ� ���� ���� Ȯ��
		if (CheckEntranceForSell()) {
			LOG_F(INFO, _T("�ŵ����Լ���"));
			// ������ ����
			_CurPosition = VtPositionType::Sell;
			// ���⼭ �ֹ��� ����.
			if (!PutEntranceOrder(_CurPosition)) {
				_CurPosition = VtPositionType::None;
				return;
			}
			int curHourMin = VtChartDataCollector::GetHourMin(curTime, _Cycle);
			// ���� �ֱ� ��ȣ�� �߻��� �ð��� ������ �д�.
			_LastEntryTime = curHourMin * 100;
			_LastEntryDailyIndex = GetDailyIndex();
			// ����ȸ���� �÷��ش�.
			_EntryToday++;
		}
	}
}

void VtQIn3::UpdateSystem(int index)
{
	VtSystem::UpdateSystem(index);
}

void VtQIn3::ReadExtraArgs()
{
	VtSystem::ReadExtraArgs();
}

void VtQIn3::ReloadSystem(int startIndex, int endIndex)
{
	ClearSignal();
	CreateSignal(startIndex, endIndex);
}

bool VtQIn3::CheckEntranceForBuy()
{
	std::vector<bool> argCond;

	argCond.push_back(CheckCondition(_T("�ż�����")));

	if (_EnableByBand) {
		// ��忡 ���� ������ ���� Ȯ���Ѵ�.
		argCond.push_back(CheckEntranceByBandForBuy());
	}

	if (_EnableByOpen) {
		argCond.push_back(CheckEntranceByOpenForBuy());
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

bool VtQIn3::CheckEntranceForBuy(size_t index)
{
	std::vector<bool> argCond;

	argCond.push_back(CheckCondition(_T("�ż�����"), index));

	if (_EnableByBand) {
		// ��忡 ���� ������ ���� Ȯ���Ѵ�.
		argCond.push_back(CheckEntranceByBandForBuy(index));
	}

	if (_EnableByOpen) {
		argCond.push_back(CheckEntranceByOpenForBuy(index));
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

bool VtQIn3::CheckEntranceForSell()
{
	std::vector<bool> argCond;

	argCond.push_back(CheckCondition(_T("�ŵ�����")));

	if (_EnableByBand) {
		// ��忡 ���� ������ ���� Ȯ���Ѵ�.
		argCond.push_back(CheckEntranceByBandForSell());
	}

	if (_EnableByOpen) {
		argCond.push_back(CheckEntranceByOpenForSell());
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

bool VtQIn3::CheckEntranceForSell(size_t index)
{
	std::vector<bool> argCond;

	argCond.push_back(CheckCondition(_T("�ŵ�����"), index));

	if (_EnableByBand) {
		// ��忡 ���� ������ ���� Ȯ���Ѵ�.
		argCond.push_back(CheckEntranceByBandForBuy(index));
	}

	if (_EnableByOpen) {
		argCond.push_back(CheckEntranceByOpenForSell(index));
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

bool VtQIn3::CheckLiqForSell()
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

bool VtQIn3::CheckLiqForSell(size_t index)
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

bool VtQIn3::CheckLiqForBuy()
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

bool VtQIn3::CheckLiqForBuy(size_t index)
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
