#include "stdafx.h"
#include "VtKp5In1.h"

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

template < typename T>
std::pair<bool, int > findInVector(const std::vector<T>  & vecOfElements, const T  & element)
{
	std::pair<bool, int > result;

	// Find given element in vector
	auto it = std::find(vecOfElements.begin(), vecOfElements.end(), element);

	if (it != vecOfElements.end())
	{
		result.second = distance(vecOfElements.begin(), it);
		result.first = true;
	}
	else
	{
		result.first = false;
		result.second = -1;
	}

	return result;
}


VtKp5In1::VtKp5In1()
	:VtSystem()
{
	InitArgs();
}


VtKp5In1::VtKp5In1(VtSystemType type)
	:VtSystem(type)
{
	InitArgs();
}

VtKp5In1::VtKp5In1(VtSystemType systemType, std::string name)
	: VtSystem(systemType, name)
{
	InitArgs();
}

VtKp5In1::~VtKp5In1()
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
void VtKp5In1::SetDataSrc()
{
	VtRealtimeRegisterManager* realRegiMgr = VtRealtimeRegisterManager::GetInstance();
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	// Kospi200 ��ȣ�� ������ �Ǽ�
	VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
	if (sym) {
		std::string symCode = sym->ShortCode;
		_DataSrcSymbolVec.push_back(symCode);
		realRegiMgr->RegisterProduct(symCode);
		// �Ϻ� ������ �߰�
		VtChartData* data = AddDataSource(symCode, VtChartType::DAY, 1);
		data->RequestChartData();
		// 5�к� ������ �߰�
		data = AddDataSource(symCode, VtChartType::MIN, _Cycle);
		data->RequestChartData();
		// �ŵ�ȣ���Ѽ���
		std::string code = symCode + (_T("SHTQ"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
		// �ż�ȣ���Ѽ���
		code = symCode + (_T("BHTQ"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
		// �ŵ�ȣ���ѰǼ�
		code = symCode + (_T("SHTC"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
		// �ż�ȣ���ѰǼ�
		code = symCode + (_T("BHTC"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
	}

	sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
	if (sym) {
		std::string symCode = sym->ShortCode;
		_DataSrcSymbolVec.push_back(symCode);
		// �ǽð� ������ ���
		realRegiMgr->RegisterProduct(symCode);
		// �ֱ� ������ �߰�
		VtChartData* data = AddDataSource(symCode, VtChartType::MIN, _Cycle);
		data->RequestChartData();
		// �ŵ�ȣ���Ѽ���
		std::string code = symCode + (_T("SHTQ"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
		// �ż�ȣ���Ѽ���
		code = symCode + (_T("BHTQ"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
		// �ŵ�ȣ���ѰǼ�
		code = symCode + (_T("SHTC"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
		// �ż�ȣ���ѰǼ�
		code = symCode + (_T("BHTC"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
	}

	sym = prdtCatMgr->GetRecentFutureSymbol(_T("175F"));
	if (sym) {
		std::string symCode = sym->ShortCode;
		_DataSrcSymbolVec.push_back(symCode);
		// �ǽð� ������ ���
		realRegiMgr->RegisterProduct(symCode);
		// �ֱ� ������ �߰�
		VtChartData* data = AddDataSource(symCode, VtChartType::MIN, _Cycle);
		data->RequestChartData();
		// �ŵ�ȣ���Ѽ���
		std::string code = symCode + (_T("SHTQ"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
		// �ż�ȣ���Ѽ���
		code = symCode + (_T("BHTQ"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
		// �ŵ�ȣ���ѰǼ�
		code = symCode + (_T("SHTC"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
		// �ż�ȣ���ѰǼ�
		code = symCode + (_T("BHTC"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
	}
}

void VtKp5In1::InitArgs()
{
	// �̹� �Ű������� �ε� �ƴٸ� ���̻� �о� ������ �ʴ´�.
	if (_ArgsLoaded)
		return;

	VtSystem::InitArgs();

	_Cycle = 1;

	_EntranceStartTime.hour = 9;
	_EntranceStartTime.min = 0;
	_EntranceStartTime.sec = 0;

	_EntranceEndTime.hour = 15;
	_EntranceEndTime.min = 0;
	_EntranceEndTime.sec = 0;

	_LiqTime.hour = 15;
	_LiqTime.min = 24;
	_LiqTime.sec = 30;

	_MaxEntrance = 2;

	_EntryBarIndex = 0;
	_ATRTime.hour = 9;
	_ATRTime.min = 0;
	_ATRTime.sec = 0;
	_ATR = 20;

	_ATRMulti = 2.0;
	_BandMulti = 0.25;
	_FilterMulti = 3.0;

	VtSystemArg arg;

	arg.Name = _T("Kbs-Kas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = true;
	arg.Desc = _T("Kbs-Kas ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);

	arg.Name = _T("Kbc>Kac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);

	arg.Name = _T("Qbc>Qac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Qbc>Qac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);

	arg.Name = _T("Uac>Ubc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Uac-Ubc ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);

	arg.Name = _T("Kas-Kbs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = true;
	arg.Desc = _T("Kas-Kbs ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("Kac>Kbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("Qac>Qbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Qac>Qbc ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("Ubc>Uac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Ubc>Uac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("Kas-Kbs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = true;
	arg.Desc = _T("Kas-Kbs ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�û��"), arg);

	arg.Name = _T("Kac>Kbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.9");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�û��"), arg);

	arg.Name = _T("Qac>Qbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Qac>Qbc ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�û��"), arg);

	arg.Name = _T("Ubc>Uac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Ubc>Uac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�û��"), arg);

	arg.Name = _T("Kbs-Kas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = true;
	arg.Desc = _T("Kbs-Kas ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�û��"), arg);

	arg.Name = _T("Kbc>Kac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.9");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�û��"), arg);

	arg.Name = _T("Qbc>Qac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Qbc>Qac ���� ���� �մϴ�.");
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
	arg.Enable = true;
	arg.Desc = _T("ATR ���� ���� �մϴ�.");
	AddSystemArg(_T("��Ÿ����"), arg);

	arg.Name = _T("ATR Time");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("9:00");
	arg.Enable = true;
	arg.Desc = _T("ATR Time���� ���� �մϴ�.");
	AddSystemArg(_T("��Ÿ����"), arg);

	arg.Name = _T("ATRMulti");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("2");
	arg.Enable = true;
	arg.Desc = _T("ATRMulti ���� ���� �մϴ�.");
	AddSystemArg(_T("��Ÿ����"), arg);

	arg.Name = _T("BandMulti");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.25");
	arg.Enable = true;
	arg.Desc = _T("BandMulti ���� ���� �մϴ�.");
	AddSystemArg(_T("��Ÿ����"), arg);

	arg.Name = _T("FilterMulti");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("3");
	arg.Enable = true;
	arg.Desc = _T("FilterMulti ���� ���� �մϴ�.");
	AddSystemArg(_T("��Ÿ����"), arg);

	arg.Name = _T("EntryBarIndex");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = true;
	arg.Desc = _T("EntryBarIndex ���� ���� �մϴ�.");
	AddSystemArg(_T("��Ÿ����"), arg);

	arg.Name = _T("c>o");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("c>o ���� ���� �մϴ�.");
	AddSystemArg(_T("��Ÿ����"), arg);
}

void VtKp5In1::CreateSignal(int startIndex, int endIndex)
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


VtPositionType VtKp5In1::UpdateSignal(int start, int end)
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

void VtKp5In1::SetDataMap(VtChartData* chartData)
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

void VtKp5In1::SaveToXml(pugi::xml_node& node)
{
	VtSystem::SaveToXml(node);

	CString msg;
	std::string value;


}

void VtKp5In1::LoadFromXml(pugi::xml_node& node)
{
	VtSystem::LoadFromXml(node);


}

/// <summary>
/// �ǽð� üũ �Լ�
/// ���⼭ ����, ����, Ʈ���ϸ�
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
VtPositionType VtKp5In1::UpdateSignal(int index)
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

	CheckLiqForBuyForKospi(index);
	CheckLiqForSellForKospi(index);

	// ���� �ż� ���� �������� �˾ƺ���.
	if (CheckEntranceForBuyForKospi(index)) {
		_ExpPosition = VtPositionType::Buy;
	}

	// ���� �ŵ� ���� �������� �˾ƺ���.
	if (CheckEntranceForSellForKospi(index)) {
		_ExpPosition = VtPositionType::Sell;
	}

	return _ExpPosition;
}

void VtKp5In1::OnTimer()
{
	if (!_Enable)
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
		if (CheckLiqForBuyForKospi() && LiqudAll()) {
			LOG_F(INFO, _T("�ż�û�꼺��"));
			_CurPosition = VtPositionType::None;
		}
	}

	// �ŵ��� �� û�� ���� Ȯ��
	if (_CurPosition == VtPositionType::Sell) {
		if (CheckLiqForSellForKospi() && LiqudAll()) {
			LOG_F(INFO, _T("�ŵ�û�꼺��"));
			_CurPosition = VtPositionType::None;
		}
	}

	// ���� �ִ� �ŷ�ȸ���� ���� ����
	if (_EntryToday >= _MaxEntrance) { // ���� �ִ� �ŷ� ȸ���� �����ߴٸ� �������� �ʴ´�.
		return;
	}

	// �ð��� ���� ���� ����
	if (!IsEnterableByTime())
		return;


	if (!_Symbol)
		return;

	// �ý��� ������ �о� �´�.
	ReadExtraArgs();

	int curTime = VtChartDataCollector::GetLocalTime();

	if (_CurPosition == VtPositionType::None) {

		if (CheckEntranceForBuyForKospi()) {
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
		if (CheckEntranceForSellForKospi()) {
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

void VtKp5In1::UpdateSystem(int index)
{
	VtSystem::UpdateSystem(index);
	if (_ShowRealtime && _UsdCfgDlg) {
		_UsdCfgDlg->OnRealTimeEvent();
	}
}

void VtKp5In1::ReadExtraArgs()
{
	VtSystem::ReadExtraArgs();
}

bool VtKp5In1::CheckEntranceForBuyForKospi()
{
	std::vector<bool> argCond;

	argCond.push_back(VtSystem::CheckEntranceForBuyForKospi());


	if (argCond.size() == 0)
		return false;

	// �ϳ��� �����̶� �����̸� ��ȣ ����. ��ΰ� ���̸� �ż� ��ȯ
	auto it = std::find(argCond.begin(), argCond.end(), false);
	if (it != argCond.end())
		return false;
	else
		return true;
}

bool VtKp5In1::CheckEntranceForBuyForKospi(size_t index)
{
	std::vector<bool> argCond;

	argCond.push_back(VtSystem::CheckEntranceForBuyForKospi(index));

	if (argCond.size() == 0)
		return false;

	// �ϳ��� �����̶� �����̸� ��ȣ ����. ��ΰ� ���̸� �ż� ��ȯ
	auto it = std::find(argCond.begin(), argCond.end(), false);
	if (it != argCond.end())
		return false;
	else
		return true;
}

bool VtKp5In1::CheckEntranceForSellForKospi()
{
	std::vector<bool> argCond;

	argCond.push_back(VtSystem::CheckEntranceForSellForKospi());


	if (argCond.size() == 0)
		return false;

	// �ϳ��� �����̶� �����̸� ��ȣ ����. ��ΰ� ���̸� �ż� ��ȯ
	auto it = std::find(argCond.begin(), argCond.end(), false);
	if (it != argCond.end())
		return false;
	else
		return true;
}

bool VtKp5In1::CheckEntranceForSellForKospi(size_t index)
{
	std::vector<bool> argCond;

	argCond.push_back(VtSystem::CheckEntranceForSellForKospi(index));


	if (argCond.size() == 0)
		return false;

	// �ϳ��� �����̶� �����̸� ��ȣ ����. ��ΰ� ���̸� �ż� ��ȯ
	auto it = std::find(argCond.begin(), argCond.end(), false);
	if (it != argCond.end())
		return false;
	else
		return true;
}

bool VtKp5In1::CheckLiqForSellForKospi()
{
	return VtSystem::CheckLiqForSellForKospi();
}

bool VtKp5In1::CheckLiqForSellForKospi(size_t index)
{
	return VtSystem::CheckLiqForSellForKospi(index);
}

bool VtKp5In1::CheckLiqForBuyForKospi()
{
	return VtSystem::CheckLiqForBuyForKospi();
}

bool VtKp5In1::CheckLiqForBuyForKospi(size_t index)
{
	return VtSystem::CheckLiqForBuyForKospi(index);
}

void VtKp5In1::ReloadSystem(int startIndex, int endIndex)
{
	ClearSignal();
	CreateSignal(startIndex, endIndex);
}

