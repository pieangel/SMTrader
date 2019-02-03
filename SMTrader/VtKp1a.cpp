#include "stdafx.h"
#include "VtKp1a.h"
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


VtKp1a::VtKp1a()
{
	InitArgs();
}


VtKp1a::VtKp1a(VtSystemType type)
	:VtSystem(type)
{
	InitArgs();
}

VtKp1a::VtKp1a(VtSystemType systemType, std::string name)
	: VtSystem(systemType, name)
{
	_Cycle = 5;
	InitArgs();
}

VtKp1a::~VtKp1a()
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
void VtKp1a::SetDataSrc()
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
		data = AddDataSource(symCode, VtChartType::MIN, 5);
		data->RequestChartData();
		// �ŵ�ȣ���Ѽ���
		std::string code = symCode + (_T("SHTQ"));
		AddDataSource(code, VtChartType::MIN, 5);
		// �ż�ȣ���Ѽ���
		code = symCode + (_T("BHTQ"));
		AddDataSource(code, VtChartType::MIN, 5);
		// �ŵ�ȣ���ѰǼ�
		code = symCode + (_T("SHTC"));
		AddDataSource(code, VtChartType::MIN, 5);
		// �ż�ȣ���ѰǼ�
		code = symCode + (_T("BHTC"));
		AddDataSource(code, VtChartType::MIN, 5);
	}
}

void VtKp1a::InitArgs()
{
	VtSystem::InitArgs();

	_EntranceStartTime.hour = 9;
	_EntranceStartTime.min = 0;
	_EntranceStartTime.sec = 0;

	_EntranceEndTime.hour = 14;
	_EntranceEndTime.min = 30;
	_EntranceEndTime.sec = 0;

	_LiqTime.hour = 15;
	_LiqTime.min = 34;
	_LiqTime.sec = 30;

	_MaxEntrance = 1;

	VtSystemArg arg;

	arg.Name = _T("Kbs-Kas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1500");
	arg.Enable = true;
	arg.Desc = _T("Kbs-Kas ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);

	arg.Name = _T("Kbc>Kac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = true;
	arg.Desc = _T("Kbc>Kac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);

	arg.Name = _T("Qbc-Qac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = true;
	arg.Desc = _T("Qbc-Qac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);

	arg.Name = _T("Uac-Ubc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = true;
	arg.Desc = _T("Uac-Ubc ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);

	arg.Name = _T("Kas-Kbs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1500");
	arg.Enable = true;
	arg.Desc = _T("Kas-Kbs ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("Kac>Kbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = true;
	arg.Desc = _T("Kbc>Kac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("Qac-Qbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = true;
	arg.Desc = _T("Qac-Qbc ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("Ubc-Uac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = true;
	arg.Desc = _T("Ubc-Uac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("ATR");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("20");
	arg.Enable = true;
	arg.Desc = _T("ATR ���� ���� �մϴ�.");
	AddSystemArg(_T("��Ÿ����"), arg);

	arg.Name = _T("ATR Time");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("9:30");
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

}

void VtKp1a::CreateSignal(int startIndex, int endIndex)
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

/// <summary>
/// �ǽð� üũ �Լ�
/// ���⼭ ����, ����, Ʈ���ϸ�
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
VtPositionType VtKp1a::UpdateSignal(int index)
{
	// �ý��� ������Ʈ
	UpdateSystem(index);

	_ExpPosition = VtPositionType::None;

	if (LiqByEndTime()) {
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

	_ExpPosition = CheckEntranceForBuy(index);

	_ExpPosition = CheckEntranceForSell(index);

	return _ExpPosition;
}

VtPositionType VtKp1a::UpdateSignal(int start, int end)
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

void VtKp1a::SetDataMap(VtChartData* chartData)
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

void VtKp1a::SaveToXml(pugi::xml_node& node)
{
	VtSystem::SaveToXml(node);

	CString msg;
	std::string value;


}

void VtKp1a::LoadFromXml(pugi::xml_node& node)
{
	VtSystem::LoadFromXml(node);


}

void VtKp1a::OnTimer()
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

	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
	std::string code = sym->ShortCode;
	std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::DAY, 1);
	std::vector<double>& highArray = _RefDataMap[dataKey]->GetDataArray(_T("high"));
	std::vector<double>& lowArray = _RefDataMap[dataKey]->GetDataArray(_T("low"));

	double preDayHigh = highArray[highArray.size() - 2];
	double preDayLow = lowArray[lowArray.size() - 2];
	_PreHL = preDayHigh - preDayLow;
	_Band = _PreHL * _BandMulti;

	int curTime = VtChartDataCollector::GetLocalTime();

	if (_CurPosition == VtPositionType::None) {
		_LongFilter = _PreHL < _FilterMulti;
		_ShortFilter = _PreHL < _FilterMulti;

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

void VtKp1a::UpdateSystem(int index)
{
	VtSystem::UpdateSystem(index);
	if (_ShowRealtime && _UsdCfgDlg) {
		_UsdCfgDlg->OnRealTimeEvent();
	}
}

bool VtKp1a::CheckEntranceForBuy()
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	std::vector<bool> argCond;
	// �ż� ����
	VtSystemArgGroup* argGrp = GetArgGroup(_T("�ż�����"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("Kbs-Kas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// �ŵ� ȣ�� �Ѽ���
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 5);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 5);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Kbs.back() - Kas.back() > param) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
					break;
				}
				else if (arg.Name.compare(_T("Kbc-Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// �ŵ� ȣ�� �Ѽ���
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 5);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 5);
					std::vector<double>& Kbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Kbc.back() - Kac.back() > param) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
					break;
				}
				else if (arg.Name.compare(_T("Qbc-Qac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("105F"));
					// �ŵ� ȣ�� �Ѽ���
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 5);
					std::vector<double>& Qac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 5);
					std::vector<double>& Qbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Qbc.back() - Qac.back() > param) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
					break;
				}
				else if (arg.Name.compare(_T("Uac-Ubc")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("1175F"));
					// �ŵ� ȣ�� �Ѽ���
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 5);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 5);
					std::vector<double>& Ubc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Uac.back() - Ubc.back() > param) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
					break;
				}
			}
		}
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

bool VtKp1a::CheckEntranceForSell()
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	std::vector<bool> argCond;
	// �ż� ����
	VtSystemArgGroup* argGrp = GetArgGroup(_T("�ŵ�����"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("Kas-Kbs")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// �ŵ� ȣ�� �Ѽ���
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 5);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 5);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Kas.back() - Kbs.back() > param) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
					break;
				}
				else if (arg.Name.compare(_T("Kac-Kbc")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// �ŵ� ȣ�� �Ѽ���
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 5);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 5);
					std::vector<double>& Kbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Kac.back() - Kbc.back() > param) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
					break;
				}
				else if (arg.Name.compare(_T("Qac-Qbc")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("105F"));
					// �ŵ� ȣ�� �Ѽ���
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 5);
					std::vector<double>& Qac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 5);
					std::vector<double>& Qbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Qac.back() - Qbc.back() > param) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
					break;
				}
				else if (arg.Name.compare(_T("Ubc-Uac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("1175F"));
					// �ŵ� ȣ�� �Ѽ���
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 5);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 5);
					std::vector<double>& Ubc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Uac.back() - Ubc.back() > param) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
					break;
				}
			}
		}
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

void VtKp1a::ReadExtraArgs()
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	std::vector<bool> argCond;
	// �ż� û��
	VtSystemArgGroup* argGrp = GetArgGroup(_T("��Ÿ����"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("ATRMulti")) == 0) {
					_ATRMulti = std::stod(arg.sValue);
				}
				else if (arg.Name.compare(_T("BandMulti")) == 0) {
					_BandMulti = std::stod(arg.sValue);
				}
				else if (arg.Name.compare(_T("FilterMulti")) == 0) {
					_FilterMulti = std::stod(arg.sValue);
				}
			}
		}
	}
}

bool VtKp1a::CheckLiqForBuy()
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));

	std::string dataKey = VtChartDataManager::MakeChartDataKey(sym->ShortCode, VtChartType::MIN, _Cycle);
	std::vector<double>& timeArray = _RefDataMap[dataKey]->GetDataArray(_T("time"));
	std::vector<double>& closeArray = _RefDataMap[dataKey]->GetDataArray(_T("close"));
	std::vector<double>& highArray = _RefDataMap[dataKey]->GetDataArray(_T("high"));
	std::vector<double>& lowArray = _RefDataMap[dataKey]->GetDataArray(_T("low"));

	std::vector<double>::iterator it = std::find(timeArray.begin(), timeArray.end(), _LastEntryTime);
	int index = std::distance(timeArray.begin(), it);
	auto minMaxIndex = std::minmax_element(closeArray.begin() + index, closeArray.end());
	double maxClose = closeArray[std::distance(closeArray.begin(), minMaxIndex.second)];
	double atr20 = GetAtr(closeArray.size() - 1, 20, highArray.data(), lowArray.data(), closeArray.data());
	if (closeArray.back() < maxClose - atr20 * _ATRMulti) {
		return true;
	}
	else {
		return false;
	}
}

bool VtKp1a::CheckLiqForSell()
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));

	std::string dataKey = VtChartDataManager::MakeChartDataKey(sym->ShortCode, VtChartType::MIN, _Cycle);
	std::vector<double>& timeArray = _RefDataMap[dataKey]->GetDataArray(_T("time"));
	std::vector<double>& closeArray = _RefDataMap[dataKey]->GetDataArray(_T("close"));
	std::vector<double>& highArray = _RefDataMap[dataKey]->GetDataArray(_T("high"));
	std::vector<double>& lowArray = _RefDataMap[dataKey]->GetDataArray(_T("low"));

	std::vector<double>::iterator it = std::find(timeArray.begin(), timeArray.end(), _LastEntryTime);
	int index = std::distance(timeArray.begin(), it);
	auto minMaxIndex = std::minmax_element(closeArray.begin() + index, closeArray.end());
	double minClose = closeArray[std::distance(closeArray.begin(), minMaxIndex.first)];
	double atr20 = GetAtr(closeArray.size() - 1, 20, highArray.data(), lowArray.data(), closeArray.data());
	if (closeArray.back() > minClose + atr20 * _ATRMulti) {
		return true;
	}
	else {
		return false;
	}
}


VtPositionType VtKp1a::CheckEntranceForBuy(size_t index)
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	std::vector<bool> argCond;
	// �ż� ����
	VtSystemArgGroup* argGrp = GetArgGroup(_T("�ż�����"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("Kbs-Kas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// �ŵ� ȣ�� �Ѽ���
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 5);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 5);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					if (index < 0 || index >= Kas.size() || index >= Kbs.size()) {
						argCond.push_back(false);
						continue;
					}

					double param = std::stod(arg.sValue);

					if (_ShowRealtime && _UsdCfgDlg) {
						double ratio = Kbs[index] - Kas[index];
						CString value;
						value.Format(_T("%.0f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(arg.Name, value);
					}

					if (Kbs[index] - Kas[index] > param) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
					break;
				}
			}
		}
	}

	if (argCond.size() == 0)
		return VtPositionType::None;

	// �ϳ��� �����̶� �����̸� ��ȣ ����. ��ΰ� ���̸� �ż� ��ȯ
	auto it = std::find(argCond.begin(), argCond.end(), false);
	if (it != argCond.end())
		return VtPositionType::None;
	else
		return VtPositionType::Buy;
}

VtPositionType VtKp1a::CheckEntranceForSell(size_t index)
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	std::vector<bool> argCond;
	// �ż� ����
	VtSystemArgGroup* argGrp = GetArgGroup(_T("�ŵ�����"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("Kas-Kbs")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// �ŵ� ȣ�� �Ѽ���
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 5);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 5);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					if (index < 0 || index >= Kas.size() || index >= Kbs.size()) {
						argCond.push_back(false);
						continue;
					}

					if (_ShowRealtime && _UsdCfgDlg) {
						double ratio = Kas[index] - Kbs[index];
						CString value;
						value.Format(_T("%.0f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Kas[index] - Kbs[index] > param) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}

					break;
				}
			}
		}
	}

	if (argCond.size() == 0)
		return VtPositionType::None;

	// �ϳ��� �����̶� �����̸� ��ȣ ����. ��ΰ� ���̸� �ż� ��ȯ
	auto it = std::find(argCond.begin(), argCond.end(), false);
	if (it != argCond.end())
		return VtPositionType::None;
	else
		return VtPositionType::Sell;
}

VtPositionType VtKp1a::CheckLiqForSell(size_t index)
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));

	std::string dataKey = VtChartDataManager::MakeChartDataKey(sym->ShortCode, VtChartType::MIN, _Cycle);
	std::vector<double>& timeArray = _RefDataMap[dataKey]->GetDataArray(_T("time"));
	std::vector<double>& closeArray = _RefDataMap[dataKey]->GetDataArray(_T("close"));
	std::vector<double>& highArray = _RefDataMap[dataKey]->GetDataArray(_T("high"));
	std::vector<double>& lowArray = _RefDataMap[dataKey]->GetDataArray(_T("low"));

	std::vector<double>::iterator it = std::find(timeArray.begin(), timeArray.end(), _LastEntryTime);
	int lastEntryIndex = std::distance(timeArray.begin(), it);
	auto minMaxIndex = std::minmax_element(closeArray.begin() + lastEntryIndex, closeArray.end());
	double minClose = closeArray[std::distance(closeArray.begin(), minMaxIndex.first)];
	double atr20 = GetAtr(index, 20, highArray.data(), lowArray.data(), closeArray.data());
	if (closeArray.back() > minClose + atr20 * _ATRMulti) {
		return VtPositionType::Buy;
	}
	else {
		return VtPositionType::None;
	}
}

VtPositionType VtKp1a::CheckLiqForBuy(size_t index)
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));

	std::string dataKey = VtChartDataManager::MakeChartDataKey(sym->ShortCode, VtChartType::MIN, _Cycle);
	std::vector<double>& timeArray = _RefDataMap[dataKey]->GetDataArray(_T("time"));
	std::vector<double>& closeArray = _RefDataMap[dataKey]->GetDataArray(_T("close"));
	std::vector<double>& highArray = _RefDataMap[dataKey]->GetDataArray(_T("high"));
	std::vector<double>& lowArray = _RefDataMap[dataKey]->GetDataArray(_T("low"));

	std::vector<double>::iterator it = std::find(timeArray.begin(), timeArray.end(), _LastEntryTime);
	int lastEntryIndex = std::distance(timeArray.begin(), it);
	auto minMaxIndex = std::minmax_element(closeArray.begin() + lastEntryIndex, closeArray.end());
	double maxClose = closeArray[std::distance(closeArray.begin(), minMaxIndex.second)];
	double atr20 = GetAtr(index, 20, highArray.data(), lowArray.data(), closeArray.data());
	if (closeArray.back() < maxClose - atr20 * _ATRMulti) {
		return VtPositionType::Sell;
	}
	else {
		return VtPositionType::None;
	}
}

void VtKp1a::ReloadSystem(int startIndex, int endIndex)
{
	ClearSignal();
	CreateSignal(startIndex, endIndex);
}

