#include "stdafx.h"
#include "VtKp2s.h"

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


VtKp2s::VtKp2s()
	:VtSystem()
{
	InitArgs();
}


VtKp2s::VtKp2s(VtSystemType type)
	:VtSystem(type)
{
	InitArgs();
}

VtKp2s::VtKp2s(VtSystemType systemType, std::string name)
	: VtSystem(systemType, name)
{
	InitArgs();
}

VtKp2s::~VtKp2s()
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
void VtKp2s::SetDataSrc()
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

void VtKp2s::InitArgs()
{
	// 이미 매개변수가 로딩 됐다면 더이사 읽어 들이지 않는다.
	if (_ArgsLoaded)
		return;

	VtSystem::InitArgs();

	_Cycle = 5;

	_EntranceStartTime.hour = 9;
	_EntranceStartTime.min = 0;
	_EntranceStartTime.sec = 0;

	_EntranceEndTime.hour = 14;
	_EntranceEndTime.min = 30;
	_EntranceEndTime.sec = 0;

	_LiqTime.hour = 15;
	_LiqTime.min = 14;
	_LiqTime.sec = 30;

	_MaxEntrance = 1;

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
	arg.sValue = _T("1500");
	arg.Enable = true;
	arg.Desc = _T("Kbs-Kas 값을 설정 합니다.");
	AddSystemArg(_T("매수진입"), arg);

	arg.Name = _T("Kbc>Kac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac 값을 설정 합니다.");
	AddSystemArg(_T("매수진입"), arg);

	arg.Name = _T("Qbc>Qac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Qbc>Qac 값을 설정 합니다.");
	AddSystemArg(_T("매수진입"), arg);

	arg.Name = _T("Uac>Ubc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Uac-Ubc 값을 설정 합니다.");
	AddSystemArg(_T("매수진입"), arg);

	arg.Name = _T("Kas-Kbs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1500");
	arg.Enable = true;
	arg.Desc = _T("Kas-Kbs 값을 설정 합니다.");
	AddSystemArg(_T("매도진입"), arg);

	arg.Name = _T("Kac>Kbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac 값을 설정 합니다.");
	AddSystemArg(_T("매도진입"), arg);

	arg.Name = _T("Qac>Qbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Qac>Qbc 값을 설정 합니다.");
	AddSystemArg(_T("매도진입"), arg);

	arg.Name = _T("Ubc>Uac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Ubc>Uac 값을 설정 합니다.");
	AddSystemArg(_T("매도진입"), arg);

	arg.Name = _T("Kas-Kbs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1500");
	arg.Enable = true;
	arg.Desc = _T("Kas-Kbs 값을 설정 합니다.");
	AddSystemArg(_T("매수청산"), arg);

	arg.Name = _T("Kac>Kbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac 값을 설정 합니다.");
	AddSystemArg(_T("매수청산"), arg);

	arg.Name = _T("Qac>Qbc");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Qac>Qbc 값을 설정 합니다.");
	AddSystemArg(_T("매수청산"), arg);

	arg.Name = _T("Ubc>Uac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Ubc>Uac 값을 설정 합니다.");
	AddSystemArg(_T("매수청산"), arg);

	arg.Name = _T("Kbs-Kas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1500");
	arg.Enable = true;
	arg.Desc = _T("Kbs-Kas 값을 설정 합니다.");
	AddSystemArg(_T("매도청산"), arg);

	arg.Name = _T("Kbc>Kac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Kbc>Kac 값을 설정 합니다.");
	AddSystemArg(_T("매도청산"), arg);

	arg.Name = _T("Qbc>Qac");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Qbc>Qac 값을 설정 합니다.");
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
	arg.Enable = true;
	arg.Desc = _T("ATRMulti 값을 설정 합니다.");
	AddSystemArg(_T("기타변수"), arg);

	arg.Name = _T("BandMulti");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.25");
	arg.Enable = true;
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
}

void VtKp2s::CreateSignal(int startIndex, int endIndex)
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
VtPositionType VtKp2s::UpdateSignal(int index)
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

	// 예상 매수 진입 포지션을 알아본다.
	if (CheckEntranceForBuy(index)) {
		_ExpPosition = VtPositionType::Buy;
	}

	// 예상 매도 진입 포지션을 알아본다.
	if (CheckEntranceForSell(index)) {
		_ExpPosition = VtPositionType::Sell;
	}

	return _ExpPosition;
}

VtPositionType VtKp2s::UpdateSignal(int start, int end)
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

void VtKp2s::SetDataMap(VtChartData* chartData)
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

void VtKp2s::SaveToXml(pugi::xml_node& node)
{
	VtSystem::SaveToXml(node);

	CString msg;
	std::string value;


}

void VtKp2s::LoadFromXml(pugi::xml_node& node)
{
	VtSystem::LoadFromXml(node);


}

void VtKp2s::OnTimer()
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


	if (!_Symbol)
		return;

	// 시스템 변수를 읽어 온다.
	ReadExtraArgs();

	int curTime = VtChartDataCollector::GetLocalTime();

	if (_CurPosition == VtPositionType::None) {

		if (CheckEntranceForBuy()) {
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
		if (CheckEntranceForSell()) {
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

void VtKp2s::UpdateSystem(int index)
{
	VtSystem::UpdateSystem(index);
	if (_ShowRealtime && _UsdCfgDlg) {
		_UsdCfgDlg->OnRealTimeEvent();
	}
}

bool VtKp2s::CheckEntranceForBuy()
{
	// 시가 필터 적용
	if (!CheckEntranceByOpenForBuy())
		return false;
	// 밴드에 의한 조건을 먼저 확인한다.
	if (!CheckEntranceByBandForBuy())
		return false;

	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	std::vector<bool> argCond;
	// 매수 진입
	VtSystemArgGroup* argGrp = GetArgGroup(_T("매수진입"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("Kbs-Kas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Kbs.back() - Kas.back() > param) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kbc>Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Kbc.back()*param > Kac.back()) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Qbc>Qac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Qbc.back()*param > Qac.back()) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Uac>Ubc")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("175F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Ubc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Uac.back()*param > Ubc.back()) {
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

bool VtKp2s::CheckEntranceForSell()
{
	// 시가 필터 적용
	if (!CheckEntranceByOpenForSell())
		return false;
	// 밴드에 의한 조건을 먼저 확인한다.
	if (!CheckEntranceByBandForSell())
		return false;

	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	std::vector<bool> argCond;
	// 매수 진입
	VtSystemArgGroup* argGrp = GetArgGroup(_T("매도진입"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("Kas-Kbs")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Kas.back() - Kbs.back() > param) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kac>Kbc")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Kac.back()*param - Kbc.back()) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Qac>Qbc")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Qac.back()*param > Qbc.back()) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Ubc>Uac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("175F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Ubc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Ubc.back()*param > Uac.back()) {
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

void VtKp2s::ReadExtraArgs()
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	std::vector<bool> argCond;
	// 매수 청산
	VtSystemArgGroup* argGrp = GetArgGroup(_T("기타변수"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("ATRMulti")) == 0) {
					_ATRMulti = std::stod(arg.sValue);
					_EnableATRLiq = true;
				}
				else if (arg.Name.compare(_T("BandMulti")) == 0) {
					_BandMulti = std::stod(arg.sValue);
				}
				else if (arg.Name.compare(_T("FilterMulti")) == 0) {
					_FilterMulti = std::stod(arg.sValue);
				}
				else if (arg.Name.compare(_T("ATRTime")) == 0) {
					std::string src = arg.sValue;
					std::string hour, min;
					size_t pos = src.find(':', 0);
					hour = src.substr(0, pos);
					min = src.substr(pos + 1, src.length() - pos);
					_ATRTime.hour = std::stoi(hour);
					_ATRTime.min = std::stoi(min);
					_ATRTime.sec = 0;
					_ATRTime.mil = 0;
				}
				else if (arg.Name.compare(_T("ATR")) == 0) {
					_ATR = std::stoi(arg.sValue);
				}
				else if (arg.Name.compare(_T("EntryBarIndex")) == 0) {
					_EntryBarIndex = std::stoi(arg.sValue);
				}
			}
		}
	}
}

bool VtKp2s::CheckLiqForBuy()
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
				if (arg.Name.compare(_T("Kas-Kbs")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Kas.back() - Kbs.back() > param) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kac>Kbc")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Kac.back()*param - Kbc.back()) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Qac>Qbc")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Qac.back()*param > Qbc.back()) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Ubc>Uac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("175F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Ubc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Ubc.back()*param > Uac.back()) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
			}
		}
	}

	if (argCond.size() > 0) {
		bool preCond = true;
		// 하나의 조건이라도 거짓이면 거짓
		auto it = std::find(argCond.begin(), argCond.end(), false);
		if (it != argCond.end())
			preCond = false;
		return preCond && CheckAtrLiqForBuy() ? true : false;
	}
	else {  // ATR 단독 청산 조건
		return CheckAtrLiqForBuy();
	}
}

bool VtKp2s::CheckEntranceByBandForBuy()
{
	if (!_Symbol)
		return false;

	std::string code = _Symbol->ShortCode;
	std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::DAY, 1);
	std::vector<double>& highArray = _RefDataMap[dataKey]->GetDataArray(_T("high"));
	std::vector<double>& lowArray = _RefDataMap[dataKey]->GetDataArray(_T("low"));

	double preDayHigh = highArray[highArray.size() - 2];
	double preDayLow = lowArray[lowArray.size() - 2];
	_PreHL = preDayHigh - preDayLow;
	// 전날 변동폭이 클때는 진입하지 않는다.
	if (_FilterMulti > _PreHL)
		return false;
	_Band = _PreHL * _BandMulti;
	if (_Symbol->Quote.close > _Symbol->Quote.open + _Band)
		return true;
	else
		return false;
}

bool VtKp2s::CheckEntranceByBandForBuy(size_t index)
{
	if (!_Symbol || index < 0 || index >= ChartDataSize)
		return false;

	std::string code = _Symbol->ShortCode;
	std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::DAY, 1);
	std::vector<double>& dayDateArray = _RefDataMap[dataKey]->GetDataArray(_T("date"));
	std::vector<double>& highArray = _RefDataMap[dataKey]->GetDataArray(_T("high"));
	std::vector<double>& lowArray = _RefDataMap[dataKey]->GetDataArray(_T("low"));

	dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
	std::vector<double>& maindateArray = _RefDataMap[dataKey]->GetDataArray(_T("date"));
	int curDayIndex = FindDateIndex(maindateArray[index], dayDateArray);
	if (curDayIndex == 0)
		return false;

	double preDayHigh = highArray[curDayIndex - 1];
	double preDayLow = lowArray[curDayIndex - 1];
	_PreHL = preDayHigh - preDayLow;
	// 전날 변동폭이 클때는 진입하지 않는다.
	if (_FilterMulti > _PreHL)
		return false;
	_Band = _PreHL * _BandMulti;
	if (_Symbol->Quote.close > _Symbol->Quote.open + _Band)
		return true;
	else
		return false;
}

bool VtKp2s::CheckEntranceByBandForSell()
{
	if (!_Symbol)
		return false;

	std::string code = _Symbol->ShortCode;
	std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::DAY, 1);
	std::vector<double>& highArray = _RefDataMap[dataKey]->GetDataArray(_T("high"));
	std::vector<double>& lowArray = _RefDataMap[dataKey]->GetDataArray(_T("low"));

	double preDayHigh = highArray[highArray.size() - 2];
	double preDayLow = lowArray[lowArray.size() - 2];
	_PreHL = preDayHigh - preDayLow;
	// 전날 변동폭이 클때는 진입하지 않는다.
	if (_FilterMulti > _PreHL)
		return false;
	_Band = _PreHL * _BandMulti;
	if (_Symbol->Quote.close < _Symbol->Quote.open - _Band)
		return true;
	else
		return false;
}

bool VtKp2s::CheckEntranceByBandForSell(size_t index)
{
	if (!_Symbol || index < 0 || index >= ChartDataSize)
		return false;

	std::string code = _Symbol->ShortCode;
	std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::DAY, 1);
	std::vector<double>& dayDateArray = _RefDataMap[dataKey]->GetDataArray(_T("date"));
	std::vector<double>& highArray = _RefDataMap[dataKey]->GetDataArray(_T("high"));
	std::vector<double>& lowArray = _RefDataMap[dataKey]->GetDataArray(_T("low"));

	dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
	std::vector<double>& maindateArray = _RefDataMap[dataKey]->GetDataArray(_T("date"));
	int curDayIndex = FindDateIndex(maindateArray[index], dayDateArray);
	if (curDayIndex == 0)
		return false;

	double preDayHigh = highArray[curDayIndex - 1];
	double preDayLow = lowArray[curDayIndex - 1];
	_PreHL = preDayHigh - preDayLow;
	// 전날 변동폭이 클때는 진입하지 않는다.
	if (_FilterMulti > _PreHL)
		return false;
	_Band = _PreHL * _BandMulti;
	if (_Symbol->Quote.close < _Symbol->Quote.open - _Band)
		return true;
	else
		return false;
}

bool VtKp2s::CheckEntranceByOpenForBuy()
{
	if (!_Symbol)
		return false;

	std::string code = _Symbol->ShortCode;
	std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
	std::vector<double>& openArray = _RefDataMap[dataKey]->GetDataArray(_T("open"));
	std::vector<double>& closeArray = _RefDataMap[dataKey]->GetDataArray(_T("close"));
	return  closeArray.back() > openArray.back() ? true : false;
}

bool VtKp2s::CheckEntranceByOpenForBuy(size_t index)
{
	if (!_Symbol || index < 0 || index >= ChartDataSize)
		return false;

	std::string code = _Symbol->ShortCode;
	std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
	std::vector<double>& openArray = _RefDataMap[dataKey]->GetDataArray(_T("open"));
	std::vector<double>& closeArray = _RefDataMap[dataKey]->GetDataArray(_T("close"));
	if (openArray.size() == 0 || closeArray.size() == 0)
		return false;

	return  closeArray[index] > openArray[index] ? true : false;
}

bool VtKp2s::CheckEntranceByOpenForSell()
{
	if (!_Symbol)
		return false;

	std::string code = _Symbol->ShortCode;
	std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
	std::vector<double>& openArray = _RefDataMap[dataKey]->GetDataArray(_T("open"));
	std::vector<double>& closeArray = _RefDataMap[dataKey]->GetDataArray(_T("close"));
	return  closeArray.back() < openArray.back() ? true : false;
}

bool VtKp2s::CheckEntranceByOpenForSell(size_t index)
{
	if (!_Symbol || index < 0 || index >= ChartDataSize)
		return false;

	std::string code = _Symbol->ShortCode;
	std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
	std::vector<double>& openArray = _RefDataMap[dataKey]->GetDataArray(_T("open"));
	std::vector<double>& closeArray = _RefDataMap[dataKey]->GetDataArray(_T("close"));
	if (openArray.size() == 0 || closeArray.size() == 0)
		return false;

	return  closeArray[index] < openArray[index] ? true : false;
}

bool VtKp2s::CheckLiqForSell()
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
				if (arg.Name.compare(_T("Kbs-Kas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Kbs.back() - Kas.back() > param) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kbc>Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Kbc.back()*param > Kac.back()) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Qbc>Qac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Qbc.back()*param > Qac.back()) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Uac>Ubc")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("175F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Ubc = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Uac.back()*param > Ubc.back()) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
			}
		}
	}

	if (argCond.size() > 0) {
		bool preCond = true;
		// 하나의 조건이라도 거짓이면 거짓
		auto it = std::find(argCond.begin(), argCond.end(), false);
		if (it != argCond.end())
			preCond = false;
		return preCond && CheckAtrLiqForSell() ? true : false;
	}
	else {  // ATR 단독 청산 조건
		return CheckAtrLiqForSell();
	}
}


bool VtKp2s::CheckEntranceForBuy(size_t index)
{
	if (index < 0 || index >= ChartDataSize)
		return false;
	// 시가 필터 적용
	if (!CheckEntranceByOpenForBuy(index))
		return false;

	// 밴드에 의한 조건을 먼저 확인한다.
	if (!CheckEntranceByBandForBuy(index))
		return false;

	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	std::vector<bool> argCond;
	// 매수 진입
	VtSystemArgGroup* argGrp = GetArgGroup(_T("매수진입"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("Kbs-Kas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kas.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kbs.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Kbs[index] - Kas[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(0, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Kbs[index] - Kas[index] > param) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kbc>Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kac.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kbc.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Kac[index] / Kbc[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(0, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Kbc[index] * param > Kac[index]) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Qbc>Qac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qac = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Qac.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Qbc.size() == 0)
						continue;
					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Qac[index] / Qbc[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(0, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Qbc[index] * param > Qac[index]) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Uac>Ubc")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("175F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Uac.size() == 0)
						continue;;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Ubc = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Ubc.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Uac[index] / Ubc[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(0, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Uac[index] * param > Ubc[index]) {
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

bool VtKp2s::CheckEntranceForSell(size_t index)
{
	if (index < 0 || index >= ChartDataSize)
		return false;
	// 시가 필터 적용
	if (!CheckEntranceByOpenForSell(index))
		return false;
	// 밴드에 의한 조건을 먼저 확인한다.
	if (!CheckEntranceByBandForSell(index))
		return false;

	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	std::vector<bool> argCond;
	// 매수 진입
	VtSystemArgGroup* argGrp = GetArgGroup(_T("매도진입"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("Kas-Kbs")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kas.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kbs.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Kas[index] - Kbs[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(0, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Kas[index] - Kbs[index] > param) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kac>Kbc")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kac.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kbc.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Kbc[index] / Kac[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(0, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Kac[index] * param - Kbc[index]) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Qac>Qbc")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qac = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Qac.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Qbc.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Qbc[index] / Qac[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(0, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Qac[index] * param > Qbc[index]) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Ubc>Uac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("175F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Uac.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Ubc = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Ubc.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Uac[index] / Ubc[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(0, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Ubc[index] * param > Uac[index]) {
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

bool VtKp2s::CheckLiqForSell(size_t index)
{
	if (index < 0 || index >= ChartDataSize)
		return false;

	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	std::vector<bool> argCond;
	// 매수 진입
	VtSystemArgGroup* argGrp = GetArgGroup(_T("매도청산"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("Kbs-Kas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kas.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kbs.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Kbs[index] - Kas[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(1, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Kbs[index] - Kas[index] > param) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kbc>Kac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kac.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kbc.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Kac[index] / Kbc[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(1, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Kbc[index] * param > Kac[index]) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Qbc>Qac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qac = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Qac.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Qbc.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Qac[index] / Qbc[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(1, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Qbc[index] * param > Qac[index]) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Uac>Ubc")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("175F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Uac.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Ubc = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Ubc.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Ubc[index] / Uac[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(1, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Uac[index] * param > Ubc[index]) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
			}
		}
	}

	if (argCond.size() > 0) {
		bool preCond = true;
		// 하나의 조건이라도 거짓이면 거짓
		auto it = std::find(argCond.begin(), argCond.end(), false);
		if (it != argCond.end())
			preCond = false;
		return preCond && CheckAtrLiqForSell() ? true : false;
	}
	else {  // ATR 단독 청산 조건
		return CheckAtrLiqForSell();
	}
}

bool VtKp2s::CheckLiqForBuy(size_t index)
{
	if (index < 0 || index >= ChartDataSize)
		return false;

	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	std::vector<bool> argCond;
	// 매수 진입
	VtSystemArgGroup* argGrp = GetArgGroup(_T("매수청산"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				if (arg.Name.compare(_T("Kas-Kbs")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kas.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kbs.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Kas[index] - Kbs[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(1, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Kas[index] - Kbs[index] > param) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Kac>Kbc")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kac = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kac.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kbc.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Kbc[index] / Kac[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(1, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Kac[index] * param - Kbc[index]) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Qac>Qbc")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qac = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Qac.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qbc = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Qbc.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Qbc[index] / Qac[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(1, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Qac[index] * param > Qbc[index]) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
				else if (arg.Name.compare(_T("Ubc>Uac")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("175F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Uac = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Uac.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Ubc = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Ubc.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Uac[index] / Ubc[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(1, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Ubc[index] * param > Uac[index]) {
						argCond.push_back(true);
					}
					else {
						argCond.push_back(false);
					}
				}
			}
		}
	}

	if (argCond.size() > 0) {
		bool preCond = true;
		// 하나의 조건이라도 거짓이면 거짓
		auto it = std::find(argCond.begin(), argCond.end(), false);
		if (it != argCond.end())
			preCond = false;
		return preCond && CheckAtrLiqForBuy() ? true : false;
	}
	else {  // ATR 단독 청산 조건
		return CheckAtrLiqForBuy();
	}
}

void VtKp2s::ReloadSystem(int startIndex, int endIndex)
{
	ClearSignal();
	CreateSignal(startIndex, endIndex);
}

