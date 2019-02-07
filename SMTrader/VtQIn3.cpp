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
/// 시스템에 필요한 데이터를 등록해 준다. 
/// 그리고 최초 등록에서 실시간으로 데이터를 축적할 수 있도록 
/// 차트 데이터 컬렉터에 요청을 한다.
/// </summary>
void VtQIn3::SetDataSrc()
{
	VtRealtimeRegisterManager* realRegiMgr = VtRealtimeRegisterManager::GetInstance();
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	// Kospi200 총호가 수량과 건수
	VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
	if (sym) {
		std::string symCode = sym->ShortCode;
		_DataSrcSymbolVec.push_back(symCode);
		realRegiMgr->RegisterProduct(symCode);
		// 일별 데이터 추가
		VtChartData* data = AddDataSource(symCode, VtChartType::DAY, 1);
		data->RequestChartData();
		// 5분봉 데이터 추가
		data = AddDataSource(symCode, VtChartType::MIN, _Cycle);
		data->RequestChartData();
		// 매도호가총수량
		std::string code = symCode + (_T("SHTQ"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
		// 매수호가총수량
		code = symCode + (_T("BHTQ"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
		// 매도호가총건수
		code = symCode + (_T("SHTC"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
		// 매수호가총건수
		code = symCode + (_T("BHTC"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
	}

	sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
	if (sym) {
		std::string symCode = sym->ShortCode;
		_DataSrcSymbolVec.push_back(symCode);
		// 실시간 데이터 등록
		realRegiMgr->RegisterProduct(symCode);
		// 주기 데이터 추가
		VtChartData* data = AddDataSource(symCode, VtChartType::MIN, _Cycle);
		data->RequestChartData();
		// 매도호가총수량
		std::string code = symCode + (_T("SHTQ"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
		// 매수호가총수량
		code = symCode + (_T("BHTQ"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
		// 매도호가총건수
		code = symCode + (_T("SHTC"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
		// 매수호가총건수
		code = symCode + (_T("BHTC"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
	}

	sym = prdtCatMgr->GetRecentFutureSymbol(_T("175F"));
	if (sym) {
		std::string symCode = sym->ShortCode;
		_DataSrcSymbolVec.push_back(symCode);
		// 실시간 데이터 등록
		realRegiMgr->RegisterProduct(symCode);
		// 주기 데이터 추가
		VtChartData* data = AddDataSource(symCode, VtChartType::MIN, _Cycle);
		data->RequestChartData();
		// 매도호가총수량
		std::string code = symCode + (_T("SHTQ"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
		// 매수호가총수량
		code = symCode + (_T("BHTQ"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
		// 매도호가총건수
		code = symCode + (_T("SHTC"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
		// 매수호가총건수
		code = symCode + (_T("BHTC"));
		AddDataSource(code, VtChartType::MIN, _Cycle);
	}
}

void VtQIn3::InitArgs()
{
	// 이미 매개변수가 로딩 됐다면 더이사 읽어 들이지 않는다.
	if (_ArgsLoaded)
		return;

	VtSystem::InitArgs();

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
	arg.sValue = _T("0.7");
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
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac 값을 설정 합니다.");
	AddSystemArg(_T("매수진입"), arg);

	arg.Name = _T("Uac>Ubc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Uac-Ubc 값을 설정 합니다.");
	AddSystemArg(_T("매수진입"), arg);

	arg.Name = _T("Qac>Qbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.7");
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
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac 값을 설정 합니다.");
	AddSystemArg(_T("매도진입"), arg);

	arg.Name = _T("Ubc>Uac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Ubc>Uac 값을 설정 합니다.");
	AddSystemArg(_T("매도진입"), arg);

	arg.Name = _T("Qac>Qbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.9");
	arg.Enable = true;
	arg.Desc = _T("Qac>Qbc 값을 설정 합니다.");
	AddSystemArg(_T("매수청산"), arg);

	arg.Name = _T("Qas>Qbs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.9");
	arg.Enable = true;
	arg.Desc = _T("Qas>Qbs 값을 설정 합니다.");
	AddSystemArg(_T("매수청산"), arg);

	arg.Name = _T("Kac>Kbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac 값을 설정 합니다.");
	AddSystemArg(_T("매수청산"), arg);

	arg.Name = _T("Ubc>Uac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Ubc>Uac 값을 설정 합니다.");
	AddSystemArg(_T("매수청산"), arg);

	arg.Name = _T("Qbc>Qac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.9");
	arg.Enable = true;
	arg.Desc = _T("Qbc>Qac 값을 설정 합니다.");
	AddSystemArg(_T("매도청산"), arg);

	arg.Name = _T("Qbs>Qas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.9");
	arg.Enable = true;
	arg.Desc = _T("Qbs>Qas 값을 설정 합니다.");
	AddSystemArg(_T("매도청산"), arg);

	arg.Name = _T("Kbc>Kac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac 값을 설정 합니다.");
	AddSystemArg(_T("매도청산"), arg);

	arg.Name = _T("Uac>Ubc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Uac-Ubc 값을 설정 합니다.");
	AddSystemArg(_T("매도청산"), arg);

	arg.Name = _T("ATR");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("20");
	arg.Enable = true;
	arg.Desc = _T("ATR 값을 설정 합니다.");
	AddSystemArg(_T("기타변수"), arg);

	arg.Name = _T("ATR Time");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("9:00");
	arg.Enable = true;
	arg.Desc = _T("ATR Time값을 설정 합니다.");
	AddSystemArg(_T("기타변수"), arg);

	arg.Name = _T("ATRMulti");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("2");
	arg.Enable = false;
	arg.Desc = _T("ATRMulti 값을 설정 합니다.");
	AddSystemArg(_T("기타변수"), arg);

	arg.Name = _T("BandMulti");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.25");
	arg.Enable = false;
	arg.Desc = _T("BandMulti 값을 설정 합니다.");
	AddSystemArg(_T("기타변수"), arg);

	arg.Name = _T("FilterMulti");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("3");
	arg.Enable = true;
	arg.Desc = _T("FilterMulti 값을 설정 합니다.");
	AddSystemArg(_T("기타변수"), arg);

	arg.Name = _T("EntryBarIndex");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = true;
	arg.Desc = _T("EntryBarIndex 값을 설정 합니다.");
	AddSystemArg(_T("기타변수"), arg);

	arg.Name = _T("c>o");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("c>o 값을 설정 합니다.");
	AddSystemArg(_T("기타변수"), arg);
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

/// <summary>
/// 실시간 체크 함수
/// 여기서 손절, 익절, 트레일링
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
VtPositionType VtQIn3::UpdateSignal(int index)
{
	// 시스템 업데이트
	UpdateSystem(index);

	_ExpPosition = VtPositionType::None;

	// 청산 시간에 의한 청산 확인
	if (LiqByEndTime(index)) {
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

	CheckLiqForBuyForKosdaq(index);
	CheckLiqForSellForKosdaq(index);

	// 예상 매수 진입 포지션을 알아본다.
	if (CheckEntranceForBuyForKosdaq(index)) {
		_ExpPosition = VtPositionType::Buy;
	}

	// 예상 매도 진입 포지션을 알아본다.
	if (CheckEntranceForSellForKosdaq(index)) {
		_ExpPosition = VtPositionType::Sell;
	}

	return _ExpPosition;
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

void VtQIn3::OnTimer()
{
	if (!_Enable)
		return;
	// 청산 시간에 따른 청산 - 조건없이 무조건 청산한다.
	if (_CurPosition != VtPositionType::None) {
		if (LiqByEndTime()) {
			_CurPosition = VtPositionType::None;
			return;
		}
	}
	// 포지션에 따른 청산
	// 매수일 때 청산 조건 확인
	if (_CurPosition == VtPositionType::Buy) {
		if (CheckLiqForBuyForKosdaq() && LiqudAll()) {
			LOG_F(INFO, _T("매수청산성공"));
			_CurPosition = VtPositionType::None;
		}
	}

	// 매도일 때 청산 조건 확인
	if (_CurPosition == VtPositionType::Sell) {
		if (CheckLiqForSellForKosdaq() && LiqudAll()) {
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


	if (!_Symbol)
		return;

	// 시스템 변수를 읽어 온다.
	ReadExtraArgs();

	int curTime = VtChartDataCollector::GetLocalTime();

	if (_CurPosition == VtPositionType::None) {

		if (CheckEntranceForBuyForKosdaq()) {
			LOG_F(INFO, _T("매수진입성공"));
			// 포지션 설정
			_CurPosition = VtPositionType::Buy;
			// 여기서 주문을 낸다. - 일단 시장가로 낸다.
			PutOrder(0, _CurPosition, VtPriceType::Market);
			if (_Symbol) // 가장 최근의 진입가를 기록해 놓는다.
				_LatestEntPrice = _Symbol->Quote.intClose;
			int curHourMin = VtChartDataCollector::GetHourMin(curTime, _Cycle);
			// 가장 최근 신호가 발생한 시간을 저장해 둔다.
			_LastEntryTime = curHourMin * 100;
			// 진입회수를 올려준다.
			_EntryToday++;
		}

		// 매도 진입 조건 확인
		if (CheckEntranceForSellForKosdaq()) {
			LOG_F(INFO, _T("매도진입성공"));
			// 포지션 설정
			_CurPosition = VtPositionType::Sell;
			// 여기서 주문을 낸다.
			PutOrder(0, _CurPosition, VtPriceType::Market);
			if (_Symbol) // 가장 최근의 진입가를 기록해 놓는다.
				_LatestEntPrice = _Symbol->Quote.intClose;
			int curHourMin = VtChartDataCollector::GetHourMin(curTime, _Cycle);
			// 가장 최근 신호가 발생한 시간을 저장해 둔다.
			_LastEntryTime = curHourMin * 100;
			// 진입회수를 올려준다.
			_EntryToday++;
		}
	}
}

void VtQIn3::UpdateSystem(int index)
{
	VtSystem::UpdateSystem(index);
	if (_ShowRealtime && _UsdCfgDlg) {
		_UsdCfgDlg->OnRealTimeEvent();
	}
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


bool VtQIn3::CheckEntranceForBuyForKosdaq()
{
	return VtSystem::CheckEntranceForBuyForKosdaq();
}

bool VtQIn3::CheckEntranceForBuyForKosdaq(size_t index)
{
	return VtSystem::CheckEntranceForBuyForKosdaq(index);
}

bool VtQIn3::CheckEntranceForSellForKosdaq()
{
	return VtSystem::CheckEntranceForSellForKosdaq();
}

bool VtQIn3::CheckEntranceForSellForKosdaq(size_t index)
{
	return VtSystem::CheckEntranceForSellForKosdaq(index);
}

bool VtQIn3::CheckLiqForSellForKosdaq()
{
	return VtSystem::CheckLiqForSellForKosdaq();
}

bool VtQIn3::CheckLiqForSellForKosdaq(size_t index)
{
	return VtSystem::CheckLiqForSellForKosdaq(index);
}

bool VtQIn3::CheckLiqForBuyForKosdaq()
{
	return VtSystem::CheckLiqForBuyForKosdaq();
}

bool VtQIn3::CheckLiqForBuyForKosdaq(size_t index)
{
	return VtSystem::CheckLiqForBuyForKosdaq(index);
}


