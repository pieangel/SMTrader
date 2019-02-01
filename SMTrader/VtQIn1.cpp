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
/// 시스템에 필요한 데이터를 등록해 준다. 
/// 그리고 최초 등록에서 실시간으로 데이터를 축적할 수 있도록 
/// 차트 데이터 컬렉터에 요청을 한다.
/// </summary>
void VtQIn1::SetDataSrc()
{
	VtRealtimeRegisterManager* realRegiMgr = VtRealtimeRegisterManager::GetInstance();
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	// Kospi200 총호가 수량과 건수
	VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
	if (sym) {
		std::string symCode = sym->ShortCode;
		_DataSrcSymbolVec.push_back(symCode);
		realRegiMgr->RegisterProduct(symCode);
		AddDataSource(symCode, VtChartType::MIN, 1);
		// 매도호가총수량
		std::string code = symCode + (_T("SHTQ"));
		AddDataSource(code, VtChartType::MIN, 1);
		// 매수호가총수량
		code = symCode + (_T("BHTQ"));
		AddDataSource(code, VtChartType::MIN, 1);
		// 매도호가총건수
		code = symCode + (_T("SHTC"));
		AddDataSource(code, VtChartType::MIN, 1);
		// 매수호가총건수
		code = symCode + (_T("BHTC"));
		AddDataSource(code, VtChartType::MIN, 1);
	}

	// Kospi200 총호가 수량과 건수
	sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
	if (sym) {
		std::string symCode = sym->ShortCode;
		_DataSrcSymbolVec.push_back(symCode);
		realRegiMgr->RegisterProduct(symCode);
		AddDataSource(symCode, VtChartType::MIN, 1);
		// 매도호가총수량
		std::string code = symCode + (_T("SHTQ"));
		AddDataSource(code, VtChartType::MIN, 1);
		// 매수호가총수량
		code = symCode + (_T("BHTQ"));
		AddDataSource(code, VtChartType::MIN, 1);
		// 매도호가총건수
		code = symCode + (_T("SHTC"));
		AddDataSource(code, VtChartType::MIN, 1);
		// 매수호가총건수
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
	arg.Desc = _T("Qbc>Qac 값을 설정 합니다.");
	AddSystemArg(_T("매수진입"), arg);

	arg.Name = _T("Qbs>Qas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
	arg.Enable = true;
	arg.Desc = _T("Qbs>Qas 값을 설정 합니다.");
	AddSystemArg(_T("매수진입"), arg);

	arg.Name = _T("Kbc>Kac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.6");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac 값을 설정 합니다.");
	AddSystemArg(_T("매수진입"), arg);

	arg.Name = _T("Kbs>Kas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
	arg.Enable = false;
	arg.Desc = _T("Kbs>Kas 값을 설정 합니다.");
	AddSystemArg(_T("매수진입"), arg);

	arg.Name = _T("Qac>Qbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.6");
	arg.Enable = true;
	arg.Desc = _T("Qac>Qbc 값을 설정 합니다.");
	AddSystemArg(_T("매도진입"), arg);

	arg.Name = _T("Qas>Qbs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
	arg.Enable = true;
	arg.Desc = _T("Qas>Qbs 값을 설정 합니다.");
	AddSystemArg(_T("매도진입"), arg);

	arg.Name = _T("Kac>Kbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.6");
	arg.Enable = false;
	arg.Desc = _T("Kac>Kbc 값을 설정 합니다.");
	AddSystemArg(_T("매도진입"), arg);

	arg.Name = _T("Kas>Kbs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
	arg.Enable = false;
	arg.Desc = _T("Kas>Kbs 값을 설정 합니다.");
	AddSystemArg(_T("매도진입"), arg);


	arg.Name = _T("Qbc>Qac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = true;
	arg.Desc = _T("Qbc>Qac 값을 설정 합니다.");
	AddSystemArg(_T("매도청산"), arg);

	arg.Name = _T("Qbs>Qas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Ubs>Uas 값을 설정 합니다.");
	AddSystemArg(_T("매도청산"), arg);

	arg.Name = _T("Kbc>Kac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac 값을 설정 합니다.");
	AddSystemArg(_T("매도청산"), arg);

	arg.Name = _T("Kbs>Kas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbs>Kas 값을 설정 합니다.");
	AddSystemArg(_T("매도청산"), arg);

	arg.Name = _T("Qac>Qbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = true;
	arg.Desc = _T("Qac>Qbc 값을 설정 합니다.");
	AddSystemArg(_T("매수청산"), arg);

	arg.Name = _T("Qas>Qbs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Qas>Qbs 값을 설정 합니다.");
	AddSystemArg(_T("매수청산"), arg);

	arg.Name = _T("Kac>Kbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kac>Kbc 값을 설정 합니다.");
	AddSystemArg(_T("매수청산"), arg);

	arg.Name = _T("Kas>Kbs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kas>Kbs 값을 설정 합니다.");
	AddSystemArg(_T("매수청산"), arg);


	arg.Name = _T("변수이름1");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
	arg.Enable = false;
	arg.Desc = _T("변수이름1 값을 설정 합니다.");
	AddSystemArg(_T("기타변수"), arg);

	arg.Name = _T("변수이름2");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
	arg.Enable = false;
	arg.Desc = _T("변수이름2 값을 설정 합니다.");
	AddSystemArg(_T("기타변수"), arg);

	arg.Name = _T("변수이름3");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
	arg.Enable = false;
	arg.Desc = _T("변수이름3 값을 설정 합니다.");
	AddSystemArg(_T("기타변수"), arg);

	arg.Name = _T("변수이름4");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
	arg.Enable = false;
	arg.Desc = _T("변수이름4 값을 설정 합니다.");
	AddSystemArg(_T("기타변수"), arg);
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
/// 실시간 체크 함수
/// 여기서 손절, 익절, 트레일링
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
VtPositionType VtQIn1::UpdateSignal(int index)
{
	// 시스템 업데이트
	UpdateSystem(index);

	_ExpPosition = VtPositionType::None;

	if (LiqByEndTime()) {
		_CurPosition = VtPositionType::None;
		return _ExpPosition;
	}

	// 손절 확인
	if (CheckLossCut(index)) {
		LOG_F(INFO, _T("손절성공"));
		_CurPosition = VtPositionType::None;
	}
	// 목표이익 확인
	if (CheckProfitCut(index)) {
		LOG_F(INFO, _T("익절성공"));
		_CurPosition = VtPositionType::None;
	}
	// 트레일링 스탑 확인
	if (CheckTrailStop(index)) {
		LOG_F(INFO, _T("트레일스탑성공"));
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

	// 청산 시간에 따른 청산 - 조건없이 무조건 청산한다.
	if (_CurPosition == VtPositionType::Buy || _CurPosition == VtPositionType::Sell) {
		if (LiqByEndTime()) {
			_CurPosition = VtPositionType::None;
			return;
		}
	}
	// 포지션에 따른 청산
	// 매수일 때 청산 조건 확인
	if (_CurPosition == VtPositionType::Buy) {
		if (CheckLiqForBuy() && LiqudAll()) {
			LOG_F(INFO, _T("매수청산성공"));
			_CurPosition = VtPositionType::None;
		}
	}

	// 매도일 때 청산 조건 확인
	if (_CurPosition == VtPositionType::Sell) {
		if (CheckLiqForSell() && LiqudAll()) {
			LOG_F(INFO, _T("매도청산성공"));
			_CurPosition = VtPositionType::None;
		}
	}

	// 일일 최대 거래회수에 의한 통제
	if (_EntryToday >= _MaxEntrance) { // 일일 최대 거래 회수에 도달했다면 진입하지 않는다.
		return;
	}

	// 시간에 따른 진입 통제
	if (!IsEnterableByTime())
		return;

	// 매수 진입 조건 확인
	if (_CurPosition == VtPositionType::None) {
		if (CheckEntranceForBuy()) {
			LOG_F(INFO, _T("매수진입성공"));
			// 포지션 설정
			_CurPosition = VtPositionType::Buy;
			// 여기서 주문을 낸다. - 일단 시장가로 낸다.
			PutOrder(0, _CurPosition, VtPriceType::Market);
			if (_Symbol) // 가장 최근의 진입가를 기록해 놓는다.
				_LatestEntPrice = _Symbol->Quote.intClose;
			// 진입회수를 올려준다.
			_EntryToday++;
		}
	}

	// 매도 진입 조건 확인
	if (_CurPosition == VtPositionType::None) {
		if (CheckEntranceForSell()) {
			LOG_F(INFO, _T("매도진입성공"));
			// 포지션 설정
			_CurPosition = VtPositionType::Sell;
			// 여기서 주문을 낸다.
			PutOrder(0, _CurPosition, VtPriceType::Market);
			if (_Symbol) // 가장 최근의 진입가를 기록해 놓는다.
				_LatestEntPrice = _Symbol->Quote.intClose;
			// 진입회수를 올려준다.
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
	// 매수 진입
	VtSystemArgGroup* argGrp = GetArgGroup(_T("매수진입"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("Ubc>Uac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총건수
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총건수
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
						LOG_F(INFO, _T("매수진입 ubc : %.0f, uac : %.0f, param : %.2f, uac/ubc : %.2f"), lastUbc, lastUac, param, lastUac / lastUbc);
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Ubs>Uas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
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
						LOG_F(INFO, _T("매수진입 ubs : %.0f, uas : %.0f, param : %.2f, uas/ubs : %.2f"), lastUbs, lastUas, param, lastUas / lastUbs);
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kbc>Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총건수
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총건수
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

					// 매수 호가 총수량
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

	// 하나의 조건이라도 거짓이면 신호 없음. 모두가 참이면 매수 반환
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
	// 매수 진입
	VtSystemArgGroup* argGrp = GetArgGroup(_T("매도진입"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("Uac>Ubc")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총건수
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총건수
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
						LOG_F(INFO, _T("매도진입 ubc : %.0f, uac : %.0f, param : %.2f, ubc/uac : %.2f"), lastUbc, lastUac, param, lastUbc / lastUac);
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Uas>Ubs")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
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
						LOG_F(INFO, _T("매도진입 ubs : %.0f, uas : %.0f, param : %.2f, uas/ubs : %.2f"), lastUbs, lastUas, param, lastUbs / lastUas);
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kbc>Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총건수
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총건수
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

					// 매수 호가 총수량
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

	// 하나의 조건이라도 거짓이면 신호 없음. 모두가 참이면 매수 반환
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
	// 매수 청산
	VtSystemArgGroup* argGrp = GetArgGroup(_T("매수청산"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("Ubc<Uac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총건수
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총건수
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
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
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
					// 매도 호가 총건수
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총건수
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

					// 매수 호가 총수량
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

	// 하나의 조건이라도 거짓이면 신호 없음. 모두가 참이면 매수 반환
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
	// 매도 청산
	VtSystemArgGroup* argGrp = GetArgGroup(_T("매도청산"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("Ubc>Uac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총건수
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총건수
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
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
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
					// 매도 호가 총건수
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총건수
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

					// 매수 호가 총수량
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

	// 하나의 조건이라도 거짓이면 신호 없음. 모두가 참이면 매수 반환
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
	// 매수 진입
	VtSystemArgGroup* argGrp = GetArgGroup(_T("매수진입"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("Ubc>Uac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총건수
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총건수
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
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
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
					// 매도 호가 총건수
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총건수
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
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
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

	// 하나의 조건이라도 거짓이면 신호 없음. 모두가 참이면 매수 반환
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
	// 매수 진입
	VtSystemArgGroup* argGrp = GetArgGroup(_T("매도진입"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("Uac>Ubc")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총건수
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총건수
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
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
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
					// 매도 호가 총건수
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총건수
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
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
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

	// 하나의 조건이라도 거짓이면 신호 없음. 모두가 참이면 매도 반환
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
	// 매수 진입
	VtSystemArgGroup* argGrp = GetArgGroup(_T("매도청산"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("Ubc>Uac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총건수
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총건수
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
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
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
					// 매도 호가 총건수
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총건수
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
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
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

	// 하나의 조건이라도 거짓이면 신호 없음. 모두가 참이면 매수 반환
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
	// 매수 진입
	VtSystemArgGroup* argGrp = GetArgGroup(_T("매수청산"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("Ubc<Uac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총건수
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총건수
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
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
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
					// 매도 호가 총건수
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총건수
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
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, 1);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
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

	// 하나의 조건이라도 거짓이면 신호 없음. 모두가 참이면 매도 반환
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


