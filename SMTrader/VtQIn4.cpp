#include "stdafx.h"
#include "VtQIn4.h"
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

VtQIn4::VtQIn4()
	:VtSystem()
{
	InitArgs();
}


VtQIn4::VtQIn4(VtSystemType type)
	:VtSystem(type)
{
	InitArgs();
}

VtQIn4::VtQIn4(VtSystemType systemType, std::string name)
	: VtSystem(systemType, name)
{
	InitArgs();
}

VtQIn4::~VtQIn4()
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
void VtQIn4::SetDataSrc()
{
	VtSystem::SetDataSrc();
}

void VtQIn4::InitArgs()
{
	// 이미 매개변수가 로딩 됐다면 더이사 읽어 들이지 않는다.
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
	_LiqTime.min = 29;
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
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Qbc>Qac 값을 설정 합니다.");
	AddSystemArg(_T("매수진입"), arg);

	arg.Name = _T("Qbs>Qas");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.6");
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
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("Qac>Qbc 값을 설정 합니다.");
	AddSystemArg(_T("매도진입"), arg);

	arg.Name = _T("Qas>Qbs");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("0.6");
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
	arg.sValue = _T("1");
	arg.Enable = false;
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
	arg.sValue = _T("1");
	arg.Enable = false;
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
	arg.Enable = false;
	arg.Desc = _T("ATR 값을 설정 합니다.");
	AddSystemArg(_T("기타변수"), arg);

	arg.Name = _T("ATR Time");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("9:00");
	arg.Enable = false;
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
	arg.Enable = false;
	arg.Desc = _T("FilterMulti 값을 설정 합니다.");
	AddSystemArg(_T("기타변수"), arg);

	arg.Name = _T("EntryBarIndex");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = false;
	arg.Desc = _T("EntryBarIndex 값을 설정 합니다.");
	AddSystemArg(_T("기타변수"), arg);

	arg.Name = _T("c>o,c<o");
	arg.Type = VtParamType::STRING;
	arg.sValue = _T("1");
	arg.Enable = true;
	arg.Desc = _T("c>o,c<o 값을 설정 합니다.");
	AddSystemArg(_T("기타변수"), arg);
	VtSystem::InitArgs();
}

void VtQIn4::CreateSignal(int startIndex, int endIndex)
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

VtPositionType VtQIn4::UpdateSignal(int start, int end)
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

void VtQIn4::SetDataMap(VtChartData* chartData)
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

void VtQIn4::SaveToXml(pugi::xml_node& node)
{
	VtSystem::SaveToXml(node);

	CString msg;
	std::string value;


}

void VtQIn4::LoadFromXml(pugi::xml_node& node)
{
	VtSystem::LoadFromXml(node);


}

/// <summary>
/// 실시간 체크 함수
/// 여기서 손절, 익절, 트레일링
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
VtPositionType VtQIn4::UpdateSignal(int index)
{
	// 시스템 업데이트
	UpdateSystem(index);

	
	// 손절 확인
	if (_CurPosition != VtPositionType::None && CheckLossCut(index)) {
		LOG_F(INFO, _T("손절성공"));
		_CurPosition = VtPositionType::None;
	}
	// 목표이익 확인
	if (_CurPosition != VtPositionType::None && CheckProfitCut(index)) {
		LOG_F(INFO, _T("익절성공"));
		_CurPosition = VtPositionType::None;
	}
	// 트레일링 스탑 확인
	if (_CurPosition != VtPositionType::None && CheckTrailStop(index)) {
		LOG_F(INFO, _T("트레일스탑성공"));
		_CurPosition = VtPositionType::None;
	}
	_ExpPosition = VtPositionType::None;

	// 여기서 예상신호를 알아본다.


	return _ExpPosition;
}

void VtQIn4::OnTimer()
{
	if (!_Enable || !_Symbol)
		return;
	// 진입 시작시간 0분에 나오는 신호는 무시한다.
	if (!CheckEntranceBar())
		return;
	// 청산 시간에 따른 청산 - 조건없이 무조건 청산한다.

	// 포지션에 따른 청산
	// 매수일 때 청산 조건 확인
	if (_CurPosition == VtPositionType::Buy) {
		if (CheckLiqForBuy() && LiqudAll()) {
			LOG_F(INFO, _T("매수청산성공"));
			_CurPosition = VtPositionType::None;
			_LastEntryDailyIndex = -1;
		}
	}

	// 매도일 때 청산 조건 확인
	if (_CurPosition == VtPositionType::Sell) {
		if (CheckLiqForSell() && LiqudAll()) {
			LOG_F(INFO, _T("매도청산성공"));
			_CurPosition = VtPositionType::None;
			_LastEntryDailyIndex = -1;
		}
	}

	if (_CurPosition == VtPositionType::None) {
		// 일일 최대 거래회수에 의한 통제
		if (_EntryToday >= _MaxEntrance) { // 일일 최대 거래 회수에 도달했다면 진입하지 않는다.
			return;
		}

		// 시간에 따른 진입 통제
		if (!IsEnterableByTime())
			return;

		// 데일리 인덱스에 의한 통제
		if (_EnableBarIndex) {
			if (!CheckBarIndex())
				return;
		}
		// 진폭에 의한 통제
		if (_EnableFilterMulti) {
			if (!CheckFilterMulti())
				return;
		}

		int curTime = VtChartDataCollector::GetLocalTime();
		if (CheckEntranceForBuy()) {
			LOG_F(INFO, _T("매수진입성공"));
			// 포지션 설정
			_CurPosition = VtPositionType::Buy;
			// 여기서 주문을 낸다.
			if (!PutEntranceOrder(_CurPosition)) {
				_CurPosition = VtPositionType::None;
				return;
			}
			int curHourMin = VtChartDataCollector::GetHourMin(curTime, _Cycle);
			// 가장 최근 신호가 발생한 시간을 저장해 둔다.
			_LastEntryTime = curHourMin * 100;
			_LastEntryDailyIndex = GetDailyIndex();
			// 진입회수를 올려준다.
			_EntryToday++;
		}

		// 매도 진입 조건 확인
		if (CheckEntranceForSell()) {
			LOG_F(INFO, _T("매도진입성공"));
			// 포지션 설정
			_CurPosition = VtPositionType::Sell;
			// 여기서 주문을 낸다.
			if (!PutEntranceOrder(_CurPosition)) {
				_CurPosition = VtPositionType::None;
				return;
			}
			int curHourMin = VtChartDataCollector::GetHourMin(curTime, _Cycle);
			// 가장 최근 신호가 발생한 시간을 저장해 둔다.
			_LastEntryTime = curHourMin * 100;
			_LastEntryDailyIndex = GetDailyIndex();
			// 진입회수를 올려준다.
			_EntryToday++;
		}
	}
}

void VtQIn4::UpdateSystem(int index)
{
	VtSystem::UpdateSystem(index);
}

void VtQIn4::ReadExtraArgs()
{
	VtSystem::ReadExtraArgs();
}

void VtQIn4::ReloadSystem(int startIndex, int endIndex)
{
	ClearSignal();
	CreateSignal(startIndex, endIndex);
}

bool VtQIn4::CheckEntranceForBuy()
{
	std::vector<bool> argCond;

	argCond.push_back(CheckCondition(_T("매수진입")));

	if (_EnableByBand) {
		// 밴드에 의한 조건을 먼저 확인한다.
		argCond.push_back(CheckEntranceByBandForBuy());
	}

	if (_EnableByOpen) {
		argCond.push_back(CheckEntranceByOpenForBuy());
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

bool VtQIn4::CheckEntranceForBuy(size_t index)
{
	std::vector<bool> argCond;

	argCond.push_back(CheckCondition(_T("매수진입"), index));

	if (_EnableByBand) {
		// 밴드에 의한 조건을 먼저 확인한다.
		argCond.push_back(CheckEntranceByBandForBuy(index));
	}

	if (_EnableByOpen) {
		argCond.push_back(CheckEntranceByOpenForBuy(index));
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

bool VtQIn4::CheckEntranceForSell()
{
	std::vector<bool> argCond;

	argCond.push_back(CheckCondition(_T("매도진입")));

	if (_EnableByBand) {
		// 밴드에 의한 조건을 먼저 확인한다.
		argCond.push_back(CheckEntranceByBandForSell());
	}

	if (_EnableByOpen) {
		argCond.push_back(CheckEntranceByOpenForSell());
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

bool VtQIn4::CheckEntranceForSell(size_t index)
{
	std::vector<bool> argCond;

	argCond.push_back(CheckCondition(_T("매도진입"), index));

	if (_EnableByBand) {
		// 밴드에 의한 조건을 먼저 확인한다.
		argCond.push_back(CheckEntranceByBandForBuy(index));
	}

	if (_EnableByOpen) {
		argCond.push_back(CheckEntranceByOpenForSell(index));
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

bool VtQIn4::CheckLiqForSell()
{
	std::vector<bool> argCond;

	argCond.push_back(CheckCondition(_T("매도청산")));

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

bool VtQIn4::CheckLiqForSell(size_t index)
{
	std::vector<bool> argCond;

	argCond.push_back(CheckCondition(_T("매도청산"), index));

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

bool VtQIn4::CheckLiqForBuy()
{
	std::vector<bool> argCond;

	argCond.push_back(CheckCondition(_T("매수청산")));

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

bool VtQIn4::CheckLiqForBuy(size_t index)
{
	std::vector<bool> argCond;

	argCond.push_back(CheckCondition(_T("매수청산"), index));

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
