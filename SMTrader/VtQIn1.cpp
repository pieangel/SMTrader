#include "stdafx.h"
#include "VtQIn1.h"
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

VtQIn1::VtQIn1()
{
	InitArgs();
}


VtQIn1::VtQIn1(VtSystemType type)
	:VtSystem(type)
{
	InitArgs();
}

VtQIn1::VtQIn1(VtSystemType systemType, std::string name)
	: VtSystem(systemType, name)
{
	InitArgs();
}

VtQIn1::~VtQIn1()
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
void VtQIn1::SetDataSrc()
{
	VtRealtimeRegisterManager* realRegiMgr = VtRealtimeRegisterManager::GetInstance();
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	// Kospi200 ��ȣ�� ������ �Ǽ�
	VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
	if (sym) {
		std::string symCode = sym->ShortCode;
		_DataSrcSymbolVec.push_back(symCode);
		realRegiMgr->RegisterProduct(symCode);
		AddDataSource(symCode, VtChartType::MIN, 1);
		// �ŵ�ȣ���Ѽ���
		std::string code = symCode + (_T("SHTQ"));
		AddDataSource(code, VtChartType::MIN, 1);
		// �ż�ȣ���Ѽ���
		code = symCode + (_T("BHTQ"));
		AddDataSource(code, VtChartType::MIN, 1);
		// �ŵ�ȣ���ѰǼ�
		code = symCode + (_T("SHTC"));
		AddDataSource(code, VtChartType::MIN, 1);
		// �ż�ȣ���ѰǼ�
		code = symCode + (_T("BHTC"));
		AddDataSource(code, VtChartType::MIN, 1);
	}

	// Kospi200 ��ȣ�� ������ �Ǽ�
	sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
	if (sym) {
		std::string symCode = sym->ShortCode;
		_DataSrcSymbolVec.push_back(symCode);
		realRegiMgr->RegisterProduct(symCode);
		AddDataSource(symCode, VtChartType::MIN, 1);
		// �ŵ�ȣ���Ѽ���
		std::string code = symCode + (_T("SHTQ"));
		AddDataSource(code, VtChartType::MIN, 1);
		// �ż�ȣ���Ѽ���
		code = symCode + (_T("BHTQ"));
		AddDataSource(code, VtChartType::MIN, 1);
		// �ŵ�ȣ���ѰǼ�
		code = symCode + (_T("SHTC"));
		AddDataSource(code, VtChartType::MIN, 1);
		// �ż�ȣ���ѰǼ�
		code = symCode + (_T("BHTC"));
		AddDataSource(code, VtChartType::MIN, 1);
	}
}

void VtQIn1::InitArgs()
{
	VtSystem::InitArgs();

	_EntranceStartTime.hour = 9;
	_EntranceStartTime.min = 5;
	_EntranceStartTime.sec = 0;

	_EntranceEndTime.hour = 14;
	_EntranceEndTime.min = 59;
	_EntranceEndTime.sec = 0;

	_LiqTime.hour = 15;
	_LiqTime.min = 29;
	_LiqTime.sec = 0;

	_MaxEntrance = 3;

	VtSystemArg arg;

	arg.Name = _T("Qbc>Qac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.6");
	arg.Enable = true;
	arg.Desc = _T("Qbc>Qac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);

	arg.Name = _T("Qbs>Qas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
	arg.Enable = true;
	arg.Desc = _T("Qbs>Qas ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);

	arg.Name = _T("Kbc>Kac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.6");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);

	arg.Name = _T("Kbs>Kas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
	arg.Enable = false;
	arg.Desc = _T("Kbs>Kas ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�����"), arg);

	arg.Name = _T("Qac>Qbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.6");
	arg.Enable = true;
	arg.Desc = _T("Qac>Qbc ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("Qas>Qbs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
	arg.Enable = true;
	arg.Desc = _T("Qas>Qbs ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("Kac>Kbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.6");
	arg.Enable = false;
	arg.Desc = _T("Kac>Kbc ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);

	arg.Name = _T("Kas>Kbs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
	arg.Enable = false;
	arg.Desc = _T("Kas>Kbs ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�����"), arg);


	arg.Name = _T("Qbc>Qac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = true;
	arg.Desc = _T("Qbc>Qac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�û��"), arg);

	arg.Name = _T("Qbs>Qas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Ubs>Uas ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�û��"), arg);

	arg.Name = _T("Kbc>Kac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�û��"), arg);

	arg.Name = _T("Kbs>Kas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbs>Kas ���� ���� �մϴ�.");
	AddSystemArg(_T("�ŵ�û��"), arg);

	arg.Name = _T("Qac>Qbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
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
	arg.Desc = _T("Kac>Kbc ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�û��"), arg);

	arg.Name = _T("Kas>Kbs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kas>Kbs ���� ���� �մϴ�.");
	AddSystemArg(_T("�ż�û��"), arg);


	arg.Name = _T("�����̸�1");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
	arg.Enable = false;
	arg.Desc = _T("�����̸�1 ���� ���� �մϴ�.");
	AddSystemArg(_T("��Ÿ����"), arg);

	arg.Name = _T("�����̸�2");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
	arg.Enable = false;
	arg.Desc = _T("�����̸�2 ���� ���� �մϴ�.");
	AddSystemArg(_T("��Ÿ����"), arg);

	arg.Name = _T("�����̸�3");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
	arg.Enable = false;
	arg.Desc = _T("�����̸�3 ���� ���� �մϴ�.");
	AddSystemArg(_T("��Ÿ����"), arg);

	arg.Name = _T("�����̸�4");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
	arg.Enable = false;
	arg.Desc = _T("�����̸�4 ���� ���� �մϴ�.");
	AddSystemArg(_T("��Ÿ����"), arg);
}

void VtQIn1::CreateSignal(int startIndex, int endIndex)
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
VtPositionType VtQIn1::UpdateSignal(int index)
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

VtPositionType VtQIn1::UpdateSignal(int start, int end)
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

void VtQIn1::SetDataMap(VtChartData* chartData)
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

void VtQIn1::SaveToXml(pugi::xml_node& node)
{
	VtSystem::SaveToXml(node);

	CString msg;
	std::string value;


}

void VtQIn1::LoadFromXml(pugi::xml_node& node)
{
	VtSystem::LoadFromXml(node);


}

void VtQIn1::OnTimer()
{
	if (!_Enable)
		return;

	// û�� �ð��� ���� û�� - ���Ǿ��� ������ û���Ѵ�.
	if (_CurPosition == VtPositionType::Buy || _CurPosition == VtPositionType::Sell) {
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

	// �ż� ���� ���� Ȯ��
	if (_CurPosition == VtPositionType::None) {
		if (CheckEntranceForBuy()) {
			LOG_F(INFO, _T("�ż����Լ���"));
			// ������ ����
			_CurPosition = VtPositionType::Buy;
			// ���⼭ �ֹ��� ����. - �ϴ� ���尡�� ����.
			PutOrder(0, _CurPosition, VtPriceType::Market);
			if (_Symbol) // ���� �ֱ��� ���԰��� ����� ���´�.
				_LatestEntPrice = _Symbol->Quote.intClose;
			// ����ȸ���� �÷��ش�.
			_EntryToday++;
		}
	}

	// �ŵ� ���� ���� Ȯ��
	if (_CurPosition == VtPositionType::None) {
		if (CheckEntranceForSell()) {
			LOG_F(INFO, _T("�ŵ����Լ���"));
			// ������ ����
			_CurPosition = VtPositionType::Sell;
			// ���⼭ �ֹ��� ����.
			PutOrder(0, _CurPosition, VtPriceType::Market);
			if (_Symbol) // ���� �ֱ��� ���԰��� ����� ���´�.
				_LatestEntPrice = _Symbol->Quote.intClose;
			// ����ȸ���� �÷��ش�.
			_EntryToday++;
		}
	}
}

void VtQIn1::UpdateSystem(int index)
{
	VtSystem::UpdateSystem(index);
	if (_ShowRealtime && _UsdCfgDlg) {
		_UsdCfgDlg->OnRealTimeEvent();
	}
}

bool VtQIn1::CheckEntranceForBuy()
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
				if (arg.Name.compare(_T("Ubc>Uac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// �ŵ� ȣ�� �ѰǼ�
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �ѰǼ�
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Ubc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double lastUbc = Ubc.back();
					double lastUac = Uac.back();
					double param = std::stod(arg.sValue);

					if (_ShowRealtime && _UsdCfgDlg) {
						_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
					}

					if (lastUbc * param > lastUac) {
						LOG_F(INFO, _T("�ż����� ubc : %.0f, uac : %.0f, param : %.2f, uac/ubc : %.2f"), lastUbc, lastUac, param, lastUac / lastUbc);
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Ubs>Uas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// �ŵ� ȣ�� �Ѽ���
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Ubs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double lastUbs = Ubs.back();
					double lastUas = Uas.back();
					double param = std::stod(arg.sValue);

					if (_ShowRealtime && _UsdCfgDlg) {
						_UsdCfgDlg->OnHogaQty(lastUas, lastUbs);
					}

					if (lastUbs * param > lastUas) {
						LOG_F(INFO, _T("�ż����� ubs : %.0f, uas : %.0f, param : %.2f, uas/ubs : %.2f"), lastUbs, lastUas, param, lastUas / lastUbs);
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kbc>Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// �ŵ� ȣ�� �ѰǼ�
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �ѰǼ�
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double lastKbc = Kbc.back();
					double lastKac = Kac.back();
					double param = std::stod(arg.sValue);
					if (lastKbc * param > lastKac) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kbc>Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double lastKbs = Kbs.back();
					double lastKas = Kas.back();
					double param = std::stod(arg.sValue);
					if (lastKbs * param > lastKas) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
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

bool VtQIn1::CheckEntranceForSell()
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
				if (arg.Name.compare(_T("Uac>Ubc")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// �ŵ� ȣ�� �ѰǼ�
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �ѰǼ�
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Ubc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double lastUbc = Ubc.back();
					double lastUac = Uac.back();
					double param = std::stod(arg.sValue);

					if (_ShowRealtime && _UsdCfgDlg) {
						_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
					}

					if (lastUac * param > lastUbc) {
						LOG_F(INFO, _T("�ŵ����� ubc : %.0f, uac : %.0f, param : %.2f, ubc/uac : %.2f"), lastUbc, lastUac, param, lastUbc / lastUac);
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Uas>Ubs")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// �ŵ� ȣ�� �Ѽ���
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Ubs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double lastUbs = Ubs.back();
					double lastUas = Uas.back();
					double param = std::stod(arg.sValue);

					if (_ShowRealtime && _UsdCfgDlg) {
						_UsdCfgDlg->OnHogaQty(lastUas, lastUbs);
					}

					if (lastUas * param > lastUbs) {
						LOG_F(INFO, _T("�ŵ����� ubs : %.0f, uas : %.0f, param : %.2f, uas/ubs : %.2f"), lastUbs, lastUas, param, lastUbs / lastUas);
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kbc>Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// �ŵ� ȣ�� �ѰǼ�
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �ѰǼ�
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double lastKbc = Kbc.back();
					double lastKac = Kac.back();
					double param = std::stod(arg.sValue);
					if (lastKbc * param > lastKac) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kbc>Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double lastKbs = Kbs.back();
					double lastKas = Kas.back();
					double param = std::stod(arg.sValue);
					if (lastKbs * param > lastKas) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
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

bool VtQIn1::CheckLiqForBuy()
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	std::vector<bool> argCond;
	// �ż� û��
	VtSystemArgGroup* argGrp = GetArgGroup(_T("�ż�û��"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("Ubc<Uac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// �ŵ� ȣ�� �ѰǼ�
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �ѰǼ�
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Ubc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double lastUbc = Ubc.back();
					double lastUac = Uac.back();
					double param = std::stod(arg.sValue);
					if (lastUbc * param < lastUac) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Ubs>Uas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// �ŵ� ȣ�� �Ѽ���
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Ubs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double lastUbs = Ubs.back();
					double lastUas = Uas.back();
					double param = std::stod(arg.sValue);
					if (lastUbs * param > lastUas) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kbc>Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// �ŵ� ȣ�� �ѰǼ�
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �ѰǼ�
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double lastKbc = Kbc.back();
					double lastKac = Kac.back();
					double param = std::stod(arg.sValue);
					if (lastKbc * param > lastKac) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kbc>Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double lastKbs = Kbs.back();
					double lastKas = Kas.back();
					double param = std::stod(arg.sValue);
					if (lastKbs * param > lastKas) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
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
	else {
		return true;
	}
}

bool VtQIn1::CheckLiqForSell()
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	std::vector<bool> argCond;
	// �ŵ� û��
	VtSystemArgGroup* argGrp = GetArgGroup(_T("�ŵ�û��"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("Ubc>Uac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// �ŵ� ȣ�� �ѰǼ�
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �ѰǼ�
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Ubc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double lastUbc = Ubc.back();
					double lastUac = Uac.back();
					double param = std::stod(arg.sValue);
					if (lastUbc * param > lastUac) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Ubs>Uas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// �ŵ� ȣ�� �Ѽ���
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Ubs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double lastUbs = Ubs.back();
					double lastUas = Uas.back();
					double param = std::stod(arg.sValue);
					if (lastUbs * param > lastUas) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kbc>Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// �ŵ� ȣ�� �ѰǼ�
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �ѰǼ�
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double lastKbc = Kbc.back();
					double lastKac = Kac.back();
					double param = std::stod(arg.sValue);
					if (lastKbc * param > lastKac) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kbc>Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double lastKbs = Kbs.back();
					double lastKas = Kas.back();
					double param = std::stod(arg.sValue);
					if (lastKbs * param > lastKas) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
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


VtPositionType VtQIn1::CheckEntranceForBuy(size_t index)
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
				if (arg.Name.compare(_T("Ubc>Uac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// �ŵ� ȣ�� �ѰǼ�
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �ѰǼ�
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Ubc = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (index < 0 || index >= Ubc.size() || index >= Uac.size()) {
						argCond.push_back(false);
						continue;
					}

					double lastUbc = Ubc[index];
					double lastUac = Uac[index];
					double param = std::stod(arg.sValue);

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = lastUac / lastUbc;
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(arg.Name, value);
					}

					if (lastUbc * param > lastUac) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Ubs>Uas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// �ŵ� ȣ�� �Ѽ���
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Ubs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					if (index < 0 || index >= Ubs.size() || index >= Uas.size()) {
						argCond.push_back(false);
						continue;
					}

					double lastUbs = Ubs[index];
					double lastUas = Uas[index];
					double param = std::stod(arg.sValue);

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaQty(lastUas, lastUbs);
						double ratio = lastUas / lastUbs;
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(arg.Name, value);
					}

					if (lastUbs * param > lastUas) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kbc>Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// �ŵ� ȣ�� �ѰǼ�
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �ѰǼ�
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					if (index < 0 || index >= Kbc.size() || index >= Kac.size()) {
						argCond.push_back(false);
						continue;
					}

					double lastKbc = Kbc[index];
					double lastKac = Kac[index];

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaQty(lastUas, lastUbs);
						double ratio = lastKac / lastKbc;
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(arg.Name, value);
					}

					double param = std::stod(arg.sValue);
					if (lastKbc * param > lastKac) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kbc>Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// �ŵ� ȣ�� �Ѽ���
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					if (index < 0 || index >= Kbs.size() || index >= Kas.size()) {
						argCond.push_back(false);
						continue;
					}

					double lastKbs = Kbs[index];
					double lastKas = Kas[index];

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaQty(lastUas, lastUbs);
						double ratio = lastKas / lastKbs;
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(arg.Name, value);
					}


					double param = std::stod(arg.sValue);
					if (lastKbs * param > lastKas) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
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

VtPositionType VtQIn1::CheckEntranceForSell(size_t index)
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
				if (arg.Name.compare(_T("Uac>Ubc")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// �ŵ� ȣ�� �ѰǼ�
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �ѰǼ�
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Ubc = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (index < 0 || index >= Ubc.size() || index >= Uac.size()) {
						argCond.push_back(false);
						continue;
					}

					double lastUbc = Ubc[index];
					double lastUac = Uac[index];
					double param = std::stod(arg.sValue);

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaQty(lastUas, lastUbs);
						double ratio = lastUbc / lastUac;
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(arg.Name, value);
					}

					if (lastUac * param > lastUbc) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Uas>Ubs")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// �ŵ� ȣ�� �Ѽ���
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Ubs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					if (index < 0 || index >= Ubs.size() || index >= Uas.size()) {
						argCond.push_back(false);
						continue;
					}

					double lastUbs = Ubs[index];
					double lastUas = Uas[index];
					double param = std::stod(arg.sValue);

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaQty(lastUas, lastUbs);
						double ratio = lastUbs / lastUas;
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(arg.Name, value);
					}

					if (lastUas * param > lastUbs) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kbc>Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// �ŵ� ȣ�� �ѰǼ�
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �ѰǼ�
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					if (index < 0 || index >= Kbc.size() || index >= Kac.size()) {
						argCond.push_back(false);
						continue;
					}

					double lastKbc = Kbc[index];
					double lastKac = Kac[index];
					double param = std::stod(arg.sValue);

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaQty(lastUas, lastUbs);
						double ratio = lastKac / lastKbc;
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(arg.Name, value);
					}

					if (lastKbc * param > lastKac) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kbc>Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// �ŵ� ȣ�� �Ѽ���
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					if (index < 0 || index >= Kbs.size() || index >= Kas.size()) {
						argCond.push_back(false);
						continue;
					}

					double lastKbs = Kbs[index];
					double lastKas = Kas[index];
					double param = std::stod(arg.sValue);

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaQty(lastUas, lastUbs);
						double ratio = lastKbs / lastKas;
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(arg.Name, value);
					}

					if (lastKbs * param > lastKas) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
			}
		}
	}
	if (argCond.size() == 0)
		return VtPositionType::None;

	// �ϳ��� �����̶� �����̸� ��ȣ ����. ��ΰ� ���̸� �ŵ� ��ȯ
	auto it = std::find(argCond.begin(), argCond.end(), false);
	if (it != argCond.end())
		return VtPositionType::None;
	else
		return VtPositionType::Sell;
}

VtPositionType VtQIn1::CheckLiqForSell(size_t index)
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	std::vector<bool> argCond;
	// �ż� ����
	VtSystemArgGroup* argGrp = GetArgGroup(_T("�ŵ�û��"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("Ubc>Uac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// �ŵ� ȣ�� �ѰǼ�
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �ѰǼ�
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Ubc = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (index < 0 || index >= Ubc.size() || index >= Uac.size()) {
						argCond.push_back(false);
						continue;
					}

					double lastUbc = Ubc[index];
					double lastUac = Uac[index];
					double param = std::stod(arg.sValue);
					if (lastUbc * param > lastUac) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Ubs>Uas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// �ŵ� ȣ�� �Ѽ���
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Ubs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					if (index < 0 || index >= Ubs.size() || index >= Uas.size()) {
						argCond.push_back(false);
						continue;
					}

					double lastUbs = Ubs[index];
					double lastUas = Uas[index];
					double param = std::stod(arg.sValue);
					if (lastUbs * param > lastUas) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kbc>Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// �ŵ� ȣ�� �ѰǼ�
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �ѰǼ�
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					if (index < 0 || index >= Kbc.size() || index >= Kac.size()) {
						argCond.push_back(false);
						continue;
					}

					double lastKbc = Kbc[index];
					double lastKac = Kac[index];
					double param = std::stod(arg.sValue);
					if (lastKbc * param > lastKac) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kbc>Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// �ŵ� ȣ�� �Ѽ���
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					if (index < 0 || index >= Kbs.size() || index >= Kas.size()) {
						argCond.push_back(false);
						continue;
					}

					double lastKbs = Kbs[index];
					double lastKas = Kas[index];
					double param = std::stod(arg.sValue);
					if (lastKbs * param > lastKas) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
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

VtPositionType VtQIn1::CheckLiqForBuy(size_t index)
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	std::vector<bool> argCond;
	// �ż� ����
	VtSystemArgGroup* argGrp = GetArgGroup(_T("�ż�û��"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("Ubc<Uac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// �ŵ� ȣ�� �ѰǼ�
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �ѰǼ�
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Ubc = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (index < 0 || index >= Ubc.size() || index >= Uac.size()) {
						argCond.push_back(false);
						continue;
					}

					double lastUbc = Ubc[index];
					double lastUac = Uac[index];
					double param = std::stod(arg.sValue);
					if (lastUbc * param < lastUac) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Ubs>Uas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// �ŵ� ȣ�� �Ѽ���
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Ubs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					if (index < 0 || index >= Ubs.size() || index >= Uas.size()) {
						argCond.push_back(false);
						continue;
					}

					double lastUbs = Ubs[index];
					double lastUas = Uas[index];
					double param = std::stod(arg.sValue);
					if (lastUbs * param > lastUas) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kbc>Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// �ŵ� ȣ�� �ѰǼ�
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �ѰǼ�
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					if (index < 0 || index >= Kbc.size() || index >= Kac.size()) {
						argCond.push_back(false);
						continue;
					}

					double lastKbc = Kbc[index];
					double lastKac = Kac[index];
					double param = std::stod(arg.sValue);
					if (lastKbc * param > lastKac) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kbc>Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// �ŵ� ȣ�� �Ѽ���
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// �ż� ȣ�� �Ѽ���
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					if (index < 0 || index >= Kbs.size() || index >= Kas.size()) {
						argCond.push_back(false);
						continue;
					}

					double lastKbs = Kbs[index];
					double lastKas = Kas[index];
					double param = std::stod(arg.sValue);
					if (lastKbs * param > lastKas) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
			}
		}
	}

	if (argCond.size() == 0)
		return VtPositionType::None;

	// �ϳ��� �����̶� �����̸� ��ȣ ����. ��ΰ� ���̸� �ŵ� ��ȯ
	auto it = std::find(argCond.begin(), argCond.end(), false);
	if (it != argCond.end())
		return VtPositionType::None;
	else
		return VtPositionType::Sell;
}

void VtQIn1::ReloadSystem(int startIndex, int endIndex)
{
	ClearSignal();
	CreateSignal(startIndex, endIndex);
}


