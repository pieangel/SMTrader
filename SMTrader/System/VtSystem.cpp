#include "stdafx.h"
#include "VtSystem.h"
#include "../VtChartData.h"
#include "VtSignal.h"
#include "chartdir.h"
#include "../VtSystemArg.h"
#include "../VtChartDataCollector.h"
#include "../VtChartDataManager.h"
#include "../VtGlobal.h"
#include "../VtPosition.h"
#include "../VtAccount.h"
#include "../VtAccountManager.h"
#include "../VtFund.h"
#include "../Global/VtDefine.h"
#include "../VtOrder.h"
#include "../VtSubAccountManager.h"
#include "../VtOrderManagerSelector.h"
#include "../VtOrderManager.h"
#include "../VtSymbol.h"
#include "../VtCutManager.h"
#include "../VtRealtimeRegisterManager.h"
#include "../VtFundManager.h"
#include "../VtSymbolManager.h"
#include "../VtProductCategoryManager.h"
#include "../VtUsdStrategyConfigDlg.h"


VtSystem::VtSystem()
{
	_Running = true;
	_EntryPrice = 0.0;
	_LastSignalType = VtPositionType::None;
	_OldSignal = VtPositionType::None;
	_AppliedData = false;

	_EntranceStartTime.hour = 9;
	_EntranceStartTime.min = 5;
	_EntranceStartTime.sec = 0;

	_EntranceEndTime.hour = 15;
	_EntranceEndTime.min = 20;
	_EntranceEndTime.sec = 0;

	_LiqTime.hour = 15;
	_LiqTime.min = 29;
	_LiqTime.sec = 0;
}


VtSystem::VtSystem(VtSystemType systemType)
{
	_SystemType = systemType;
	_EntryPrice = 0.0;
	_Running = true;
	_AppliedData = false;

	_EntranceStartTime.hour = 9;
	_EntranceStartTime.min = 5;
	_EntranceStartTime.sec = 0;

	_EntranceEndTime.hour = 15;
	_EntranceEndTime.min = 20;
	_EntranceEndTime.sec = 0;

	_LiqTime.hour = 15;
	_LiqTime.min = 29;
	_LiqTime.sec = 0;
}

VtSystem::~VtSystem()
{
	ClearSignal();
}

void VtSystem::AddDataSource(std::string name, double* src)
{
	if (!src)
		return;

	DataMap[name] = src;
}


void VtSystem::AddDataSource(std::string symCode, std::string dataName, std::vector<double>& data)
{
	DataMap[dataName] = data.data();
}

VtChartData* VtSystem::AddDataSource(std::string symCode, VtChartType type, int cycle)
{
	// 여기서 실시간 차트 데이터 수집을 등록해 준다.
	VtChartDataCollector* chartDataCollector = VtChartDataCollector::GetInstance();
	VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
	VtChartData* chartData = chartDataMgr->FindNAdd(symCode, type, cycle);
	if (chartData) {
		_RefDataMap[chartData->Key()] = chartData;
		chartDataCollector->RegisterChartData(chartData);
	}

	return chartData;
}

void VtSystem::OnTimer()
{

}

bool VtSystem::IsEnterableByTime()
{
	VtTime time = VtGlobal::GetLocalTime();
	if (time.hour < _EntranceStartTime.hour)
		return false;
	if (time.hour == _EntranceStartTime.hour && time.min < _EntranceStartTime.min)
		return false;
	if (time.hour > _EntranceEndTime.hour)
		return false;
	if (time.hour == _EntranceEndTime.hour && time.min > _EntranceEndTime.min)
		return false;
	return true;
}

bool VtSystem::LiqByEndTime()
{
	if (_CurPosition == VtPositionType::None)
		return false;

	VtTime time = VtGlobal::GetLocalTime();
	if (time.hour >= _LiqTime.hour && time.min >= _LiqTime.min && time.sec >= _LiqTime.sec) {
		// 여기서 청산을 진행한다.
		if (LiqudAll())
			return true;
		else
			return false;
	}

	return false;
}

bool VtSystem::LiqByEndTime(int index)
{
	if (_CurPosition == VtPositionType::None || !_Symbol)
		return false;

	std::string dataKey = VtChartDataManager::MakeChartDataKey(_Symbol->ShortCode, VtChartType::MIN, _Cycle);
	std::vector<double>& timeArray = _RefDataMap[dataKey]->GetDataArray(_T("time"));
	if (timeArray.size() == 0 || index < 0 || index >= timeArray.size())
		return false;

	VtTime time = VtGlobal::GetTime(timeArray[index]);
	if (time.hour >= _LiqTime.hour && time.min >= _LiqTime.min && time.sec >= _LiqTime.sec) {
		// 여기서 청산을 진행한다.
		if (LiqudAll())
			return true;
		else
			return false;
	}

	return false;
}

bool VtSystem::CheckEntranceByBandForBuy()
{
	if (!_Symbol)
		return false;

	std::string code = _Symbol->ShortCode;
	std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::DAY, 1);
	std::vector<double>& highArray = _RefDataMap[dataKey]->GetDataArray(_T("high"));
	std::vector<double>& lowArray = _RefDataMap[dataKey]->GetDataArray(_T("low"));

	double preDayHigh = highArray[highArray.size() - 2];
	double preDayLow = lowArray[lowArray.size() - 2];
	double _PreHL = preDayHigh - preDayLow;
	// 전날 변동폭이 클때는 진입하지 않는다.
	if (_FilterMulti > _PreHL)
		return false;
	double _Band = _PreHL * _BandMulti;
	if (_Symbol->Quote.close > _Symbol->Quote.open + _Band)
		return true;
	else
		return false;
}

bool VtSystem::CheckEntranceByBandForBuy(size_t index)
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
	double _PreHL = preDayHigh - preDayLow;
	// 전날 변동폭이 클때는 진입하지 않는다.
	if (_FilterMulti > _PreHL)
		return false;
	double _Band = _PreHL * _BandMulti;
	if (_Symbol->Quote.close > _Symbol->Quote.open + _Band)
		return true;
	else
		return false;
}

bool VtSystem::CheckEntranceByBandForSell()
{
	if (!_Symbol)
		return false;

	std::string code = _Symbol->ShortCode;
	std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::DAY, 1);
	std::vector<double>& highArray = _RefDataMap[dataKey]->GetDataArray(_T("high"));
	std::vector<double>& lowArray = _RefDataMap[dataKey]->GetDataArray(_T("low"));

	double preDayHigh = highArray[highArray.size() - 2];
	double preDayLow = lowArray[lowArray.size() - 2];
	double _PreHL = preDayHigh - preDayLow;
	// 전날 변동폭이 클때는 진입하지 않는다.
	if (_FilterMulti > _PreHL)
		return false;
	double _Band = _PreHL * _BandMulti;
	if (_Symbol->Quote.close < _Symbol->Quote.open - _Band)
		return true;
	else
		return false;
}

bool VtSystem::CheckEntranceByBandForSell(size_t index)
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
	double _PreHL = preDayHigh - preDayLow;
	// 전날 변동폭이 클때는 진입하지 않는다.
	if (_FilterMulti > _PreHL)
		return false;
	double _Band = _PreHL * _BandMulti;
	if (_Symbol->Quote.close < _Symbol->Quote.open - _Band)
		return true;
	else
		return false;
}

bool VtSystem::CheckEntranceByOpenForBuy()
{
	if (!_Symbol)
		return false;

	return  _Symbol->Quote.close > _Symbol->Quote.open ? true : false;
}

bool VtSystem::CheckEntranceByOpenForBuy(size_t index)
{
	if (!_Symbol || index < 0 || index >= ChartDataSize)
		return false;

	std::string code = _Symbol->ShortCode;
	std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::DAY, 1);
	std::vector<double>& dayDateArray = _RefDataMap[dataKey]->GetDataArray(_T("date"));
	std::vector<double>& openArray = _RefDataMap[dataKey]->GetDataArray(_T("open"));
	std::vector<double>& closeArray = _RefDataMap[dataKey]->GetDataArray(_T("close"));

	dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
	std::vector<double>& maindateArray = _RefDataMap[dataKey]->GetDataArray(_T("date"));
	int curDayIndex = FindDateIndex(maindateArray[index], dayDateArray);

	dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::DAY, 1);
	if (openArray.size() == 0 || closeArray.size() == 0)
		return false;

	return  closeArray[curDayIndex] > openArray[curDayIndex] ? true : false;
}

bool VtSystem::CheckEntranceByOpenForSell()
{
	if (!_Symbol)
		return false;

	return  _Symbol->Quote.close < _Symbol->Quote.open ? true : false;
}

bool VtSystem::CheckEntranceByOpenForSell(size_t index)
{
	if (!_Symbol || index < 0 || index >= ChartDataSize)
		return false;

	std::string code = _Symbol->ShortCode;
	std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::DAY, 1);
	std::vector<double>& dayDateArray = _RefDataMap[dataKey]->GetDataArray(_T("date"));
	std::vector<double>& openArray = _RefDataMap[dataKey]->GetDataArray(_T("open"));
	std::vector<double>& closeArray = _RefDataMap[dataKey]->GetDataArray(_T("close"));

	dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
	std::vector<double>& maindateArray = _RefDataMap[dataKey]->GetDataArray(_T("date"));
	int curDayIndex = FindDateIndex(maindateArray[index], dayDateArray);

	dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::DAY, 1);
	if (openArray.size() == 0 || closeArray.size() == 0)
		return false;

	return  closeArray[curDayIndex] < openArray[curDayIndex] ? true : false;
}

bool VtSystem::CheckEntranceForBuyForKospi()
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

bool VtSystem::CheckEntranceForBuyForKospi(size_t index)
{
	if (index < 0 || index >= ChartDataSize)
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

bool VtSystem::CheckEntranceForSellForKospi()
{
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

bool VtSystem::CheckEntranceForSellForKospi(size_t index)
{
	if (index < 0 || index >= ChartDataSize)
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

bool VtSystem::CheckLiqForSellForKospi()
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

bool VtSystem::CheckLiqForSellForKospi(size_t index)
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

bool VtSystem::CheckLiqForBuyForKospi()
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

bool VtSystem::CheckLiqForBuyForKospi(size_t index)
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


bool VtSystem::CheckEntranceForBuyForKosdaq()
{
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
				if (arg.Name.compare(_T("Qbc>Qac")) == 0) {
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
				else if (arg.Name.compare(_T("Qbs>Qas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Qbs.back()*param > Qas.back()) {
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

bool VtSystem::CheckEntranceForSellForKosdaq()
{
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
				if (arg.Name.compare(_T("Qac>Qbc")) == 0) {
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
				else if (arg.Name.compare(_T("Qas>Qbs")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Qas.back()*param > Qbs.back()) {
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

bool VtSystem::CheckLiqForBuyForKosdaq()
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
				if (arg.Name.compare(_T("Qac>Qbc")) == 0) {
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
				else if (arg.Name.compare(_T("Qas>Qbs")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Qas.back()*param > Qbs.back()) {
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

bool VtSystem::CheckEntranceForBuyForUsd()
{
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
				if (arg.Name.compare(_T("Uac>Ubc")) == 0) {
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
				else if (arg.Name.compare(_T("Uas>Ubs")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("175F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Ubs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Uas.back()*param > Ubs.back()) {
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
				else if (arg.Name.compare(_T("Kbs>Kas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Kbs.back()*param > Kas.back()) {
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

bool VtSystem::CheckEntranceForSellForUsd()
{
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
				if (arg.Name.compare(_T("Ubc>Uac")) == 0) {
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
				else if (arg.Name.compare(_T("Ubs>Uas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("175F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Ubs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Ubs.back()*param > Uas.back()) {
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
				else if (arg.Name.compare(_T("Kas>Kbs")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Kas.back()*param - Kbs.back()) {
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

bool VtSystem::CheckLiqForBuyForUsd()
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
				if (arg.Name.compare(_T("Ubc>Uac")) == 0) {
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
				else if (arg.Name.compare(_T("Ubs>Uas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("175F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Ubs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Ubs.back()*param > Uas.back()) {
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
				else if (arg.Name.compare(_T("Kas>Kbs")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Kas.back()*param - Kbs.back()) {
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


bool VtSystem::CheckLiqForSellForUsd()
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
				if (arg.Name.compare(_T("Uac>Ubc")) == 0) {
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
				else if (arg.Name.compare(_T("Uas>Ubs")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("175F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Ubs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Uas.back()*param > Ubs.back()) {
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
				else if (arg.Name.compare(_T("Kbs>Kas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Kbs.back()*param > Kas.back()) {
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


bool VtSystem::CheckEntranceForBuyForUsd(size_t index)
{
	if (index < 0 || index >= ChartDataSize)
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
				if (arg.Name.compare(_T("Uac>Ubc")) == 0) {
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
				else if (arg.Name.compare(_T("Uas>Ubs")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("175F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Uas.size() == 0)
						continue;;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Ubs = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Ubs.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Uas[index] / Ubs[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(0, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Uas[index] * param > Ubs[index]) {
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
				else if (arg.Name.compare(_T("Kbs>Kas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kas.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kbs.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Kas[index] / Kbs[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(0, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Kbs[index] * param > Kas[index]) {
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

bool VtSystem::CheckEntranceForSellForUsd(size_t index)
{
	if (index < 0 || index >= ChartDataSize)
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
				if (arg.Name.compare(_T("Ubc>Uac")) == 0) {
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
				else if (arg.Name.compare(_T("Ubs>Uas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("175F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Uas.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Ubs = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Ubs.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Uas[index] / Ubs[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(0, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Ubs[index] * param > Uas[index]) {
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
				else if (arg.Name.compare(_T("Kas>Kbs")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kas.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kbs.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Kbs[index] / Kas[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(0, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Kas[index] * param - Kbs[index]) {
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

bool VtSystem::CheckLiqForSellForUsd(size_t index)
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
				if (arg.Name.compare(_T("Uac>Ubc")) == 0) {
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
				else if (arg.Name.compare(_T("Uas>Ubs")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("175F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Uas.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Ubs = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Ubs.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Ubs[index] / Uas[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(1, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Uas[index] * param > Ubs[index]) {
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
				else if (arg.Name.compare(_T("Kbs>Kas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kas.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kbs.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Kas[index] / Kbs[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(1, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Kbs[index] * param > Kas[index]) {
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

bool VtSystem::CheckLiqForBuyForUsd(size_t index)
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
				if (arg.Name.compare(_T("Ubc>Uac")) == 0) {
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
				else if (arg.Name.compare(_T("Ubs>Uas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("175F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Uas = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Uas.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Ubs = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Ubs.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Uas[index] / Ubs[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(1, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Ubs[index] * param > Uas[index]) {
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
				else if (arg.Name.compare(_T("Kas>Kbs")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("101F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTC"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kas = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kas.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTC"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Kbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Kbs.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Kbs[index] / Kas[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(1, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Kas[index] * param - Kbs[index]) {
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


bool VtSystem::CheckLiqForSellForKosdaq()
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
				if (arg.Name.compare(_T("Qbc>Qac")) == 0) {
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
				else if (arg.Name.compare(_T("Qbs>Qas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qas = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));

					double param = std::stod(arg.sValue);

					if (Qbs.back()*param > Qas.back()) {
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


bool VtSystem::CheckEntranceForBuyForKosdaq(size_t index)
{
	if (index < 0 || index >= ChartDataSize)
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
				if (arg.Name.compare(_T("Qbc>Qac")) == 0) {
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
				else if (arg.Name.compare(_T("Qbs>Qas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qas = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Qas.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Qbs.size() == 0)
						continue;
					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Qas[index] / Qbs[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(0, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Qbs[index] * param > Qas[index]) {
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

bool VtSystem::CheckEntranceForSellForKosdaq(size_t index)
{
	if (index < 0 || index >= ChartDataSize)
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
				if (arg.Name.compare(_T("Qac>Qbc")) == 0) {
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
				else if (arg.Name.compare(_T("Qas>Qbs")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qas = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Qas.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Qbs.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Qbs[index] / Qas[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(0, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Qas[index] * param > Qbs[index]) {
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

bool VtSystem::CheckLiqForSellForKosdaq(size_t index)
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
				if (arg.Name.compare(_T("Qbc>Qac")) == 0) {
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
				else if (arg.Name.compare(_T("Qbs>Qas")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qas = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Qas.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Qbs.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Qas[index] / Qbs[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(1, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Qbs[index] * param > Qas[index]) {
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

bool VtSystem::CheckLiqForBuyForKosdaq(size_t index)
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
				if (arg.Name.compare(_T("Qac>Qbc")) == 0) {
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
				else if (arg.Name.compare(_T("Qas>Qbs")) == 0) {
					VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(_T("106F"));
					// 매도 호가 총수량
					std::string code = sym->ShortCode + (_T("SHTQ"));
					std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qas = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Qas.size() == 0)
						continue;

					// 매수 호가 총수량
					code = sym->ShortCode + (_T("BHTQ"));
					dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
					std::vector<double>& Qbs = _RefDataMap[dataKey]->GetDataArray(_T("close"));
					if (Qbs.size() == 0)
						continue;

					if (_ShowRealtime && _UsdCfgDlg) {
						//_UsdCfgDlg->OnHogaCount(lastUac, lastUbc);
						double ratio = Qbs[index] / Qas[index];
						CString value;
						value.Format(_T("%.2f"), ratio);
						_UsdCfgDlg->RefreshRealTimeValue(1, arg.Name, value);
					}

					double param = std::stod(arg.sValue);

					if (Qas[index] * param > Qbs[index]) {
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

int VtSystem::FindDateIndex(double date, std::vector<double>& dateArray)
{
	for (int i = dateArray.size() - 1; i >= 0; --i) {
		if (date == dateArray[i])
			return i;
	}

	return -1;
}

void VtSystem::PutOrder(int price, VtPositionType position, VtPriceType priceType )
{
	if (!_Symbol)
		return;
	LOG_F(INFO, _T("PutOrder : price = %d"), price);

	VtOrderManagerSelector* orderMgrSelector = VtOrderManagerSelector::GetInstance();
	if (_SysTargetType == TargetType::RealAccount || _SysTargetType == TargetType::SubAccount) { // 실계좌나 서브 계좌 일 때
		if (_Account) {
			VtOrderManager* orderMgr = orderMgrSelector->FindAddOrderManager(_Account->AccountNo);
			HdOrderRequest request;
			request.Price = price;
			request.Position = position;
			request.Amount = _OrderAmount;
			if (_Account->AccountLevel() == 0) { // 실계좌 일 때
				request.AccountNo = _Account->AccountNo;
				request.Password = _Account->Password;
			} else { // 서브 계좌 일 때
				VtAccount* parentAcnt = _Account->ParentAccount();
				if (parentAcnt) { // 본계좌의 정보를 넣어 준다.
					request.AccountNo = parentAcnt->AccountNo;
					request.Password = parentAcnt->Password;
				}
			}
			request.SymbolCode = _Symbol->ShortCode;
			request.FillCondition = _FillCondition;
			request.PriceType = _PriceType;

			request.RequestId = orderMgr->GetOrderRequestID();
			request.SourceId = (long)this;
			if (_Account->AccountLevel() == 0) { // 실계좌일 때
				request.SubAccountNo = _T("");
				request.FundName = _T("");
			} else { // 서브 계좌 일 때
				request.SubAccountNo = _Account->AccountNo;
				request.FundName = _T("");
			}

			request.AccountLevel = _Account->AccountLevel();
			request.orderType = VtOrderType::New;
			orderMgr->PutOrder(std::move(request));
		}
	}
	else { // 펀드 주문 일 때
		if (_Fund) {
			std::vector<VtAccount*>& fundAcntList = _Fund->GetFundAccountVector();
			for (auto it = fundAcntList.begin(); it != fundAcntList.end(); ++it) { // 펀드 계좌를 돌면서 주문을 내준다.
				VtAccount* subAcnt = *it;
				VtAccount* parentAcnt = subAcnt->ParentAccount();
				VtOrderManager* orderMgr = orderMgrSelector->FindAddOrderManager(subAcnt->AccountNo);

				HdOrderRequest request;
				request.Price = price;
				request.Position = position;
				request.Amount = _OrderAmount * subAcnt->SeungSu;
				if (parentAcnt) { // 부모 계좌의 정보를 넣어 준다.
					request.AccountNo = parentAcnt->AccountNo;
					request.Password = parentAcnt->Password;
				}
				else
					continue;
				request.SymbolCode = _Symbol->ShortCode;
				request.FillCondition = _FillCondition;
				request.PriceType = priceType;

				request.RequestId = orderMgr->GetOrderRequestID();
				request.SourceId = (long)this;
				request.SubAccountNo = subAcnt->AccountNo;
				request.FundName = _Fund->Name;
				request.AccountLevel = 1;
				request.orderType = VtOrderType::New;
				orderMgr->PutOrder(std::move(request));
			}
		}
	}
}

void VtSystem::PutOrder(VtPosition* posi, int price, bool liqud /*= false*/)
{
	if (!posi || posi->OpenQty == 0 || !_Symbol)
		return;

	LOG_F(INFO, _T("PutOrder : openQty = %d"), posi->OpenQty);
	VtAccount* acnt = nullptr;
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	acnt = acntMgr->FindAccount(posi->AccountNo);
	if (!acnt) { // 본계좌에 없을 경우 서브 계좌를 찾아 본다.
		VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
		acnt = subAcntMgr->FindAccount(posi->AccountNo);
	}

	if (!acnt) // 계좌가 없는 경우 아무일도 하지 않음.
		return;
	VtOrderManagerSelector* orderMgrSelector = VtOrderManagerSelector::GetInstance();
	VtOrderManager* orderMgr = orderMgrSelector->FindAddOrderManager(acnt->AccountNo);

	HdOrderRequest request;
	request.Amount = std::abs(posi->OpenQty);
	if (acnt->AccountLevel() == 0) { // 본계좌 일 때
		request.AccountNo = acnt->AccountNo;
		request.Password = acnt->Password;
	}
	else { // 서브 계좌 일 때
		VtAccount* parentAcnt = acnt->ParentAccount();
		if (parentAcnt) { // 부모 계좌가 있는지 확인 한다.
			request.AccountNo = parentAcnt->AccountNo;
			request.Password = parentAcnt->Password;
		}
		else
			return;
	}
	request.SymbolCode = _Symbol->ShortCode;
	request.FillCondition = _FillCondition;

	if (liqud) { // 시장가 청산인 경우
		request.Price = 0;
		request.PriceType = VtPriceType::Market;
	}
	else { // 지정가 청산인 경우
		request.PriceType = VtPriceType::Price;
		request.Price = price;
	}

	if (posi->Position == VtPositionType::Buy) { // 매수 청산인 경우
		request.Position = VtPositionType::Sell;
	}
	else if (posi->Position == VtPositionType::Sell) { // 매도 청산인 경우
		request.Position = VtPositionType::Buy;
	}

	request.RequestId = orderMgr->GetOrderRequestID();
	request.SourceId = (long)this;

	if (acnt->AccountLevel() == 0) { // 본계좌 인 경우
		request.SubAccountNo = _T("");
		request.FundName = _T("");
	}
	else { // 서브 계좌인 경우
		request.SubAccountNo = acnt->AccountNo;
		if (acnt->Fund())
			request.FundName = acnt->Fund()->Name;
		else
			request.FundName = _T("");
	}

	request.orderType = VtOrderType::New;
	// 정산 주문이냐 청산
	// 0 : 일반 , 1 : 익절, 2 : 손절- 청산 주문일 때는 익절 손절이 활성화 되어 있어도 처리하지 않는다.
	request.RequestType = 1;

	orderMgr->PutOrder(std::move(request));
}

bool VtSystem::LiqudAll()
{
	if (!_Symbol)
		return false;

	if (_SysTargetType == TargetType::RealAccount || _SysTargetType == TargetType::SubAccount) { // 계좌 주문일 때
		if (!_Account)
			return false;
		VtPosition* posi = _Account->FindPosition(_Symbol->ShortCode);
		if (posi->Position == VtPositionType::None)
			return false;
		if (_LiqPriceType == VtPriceType::Market) // 시장가
			PutOrder(posi, 0, true);
		else // 지정가
 			PutOrder(posi, static_cast<int>(posi->AvgPrice), false);

		_ProfitLoss = 0.0;
		_MaxProfit = 0.0;

		posi->Position = VtPositionType::None;
		_CurPosition = VtPositionType::None;
		return true;
	} else {
		if (!_Fund)
			return false;
		std::vector<VtAccount*>& fundAcntList = _Fund->GetFundAccountVector();
		for (auto it = fundAcntList.begin(); it != fundAcntList.end(); ++it) { // 서브 계좌 목록을 돌면서 각 서브계좌의 잔고를 청산해 준다.
			VtAccount* subAcnt = *it;
			VtPosition* posi = subAcnt->FindPosition(_Symbol->ShortCode);
			if (posi->Position == VtPositionType::None)
				continue;
			if (_LiqPriceType == VtPriceType::Market) // 시장가
				PutOrder(posi, 0, true);
			else // 지정가
				PutOrder(posi, static_cast<int>(posi->AvgPrice), false);
			posi->Position = VtPositionType::None;
		}

		_ProfitLoss = 0.0;
		_MaxProfit = 0.0;
		_CurPosition = VtPositionType::None;
		return true;
	}
}

void VtSystem::Symbol(VtSymbol* val)
{
	_Symbol = val;
	// 일별 데이터 추가
	VtChartData* data = AddDataSource(_Symbol->ShortCode, VtChartType::DAY, 1);
	data->RequestChartData();
}

void VtSystem::SetDataSrc()
{
	
}

void VtSystem::SetDataSrc(VtSymbol* sym, VtChartType type, int cycle)
{
	VtRealtimeRegisterManager* realRegiMgr = VtRealtimeRegisterManager::GetInstance();
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

/*
/// <summary>
/// 당일 진입 가능한 봉의 인덱스
/// </summary>
int _EntryBarIndex = 0;
/// <summary>
/// ATR로 청산 가능한 시간. ATR청산은 이 시간 이후로 청산가능하다.
/// </summary>
VtTime _ATRTime;
/// <summary>
/// ATR 청산이 가능한지 여부 : 참일때만 ATR로 청산이 가능하다.
/// </summary>
bool _EnableATRLiq = false;
/// <summary>
/// 청산에 적용할 ATR 값
/// </summary>
int _ATR = 0;

double _ATRMulti = 2.0;
double _BandMulti = 0.25;
double _FilterMulti = 3.0;
int _LastEntryTime = 0;
*/

void VtSystem::Save(simple::file_ostream<same_endian_type>& ss)
{
	ss << _Name; // 시스템 이름
	ss << (int)_SysTargetType; // 시스템 타입 : 실계좌, 서브계좌, 펀드
	ss << _SysTargetName; // 시스템 대상 이름 : 계좌는 계좌번호, 펀드는 펀드명
	ss << _EntranceStartTime.hour; // 시스템 진입 시작시간
	ss << _EntranceStartTime.min;
	ss << _EntranceStartTime.sec;
	ss << _EntranceEndTime.hour; // 시스템 진입 종료시간
	ss << _EntranceEndTime.min;
	ss << _EntranceEndTime.sec;
	ss << _LiqTime.hour; // 시스템 청산 시간
	ss << _LiqTime.min;
	ss << _LiqTime.sec;
	ss << _EnableTrailStop; // 트레일 스탑 설정
	ss << _EnableLossCut; // 로스컷 설정
	ss << _EnableTargetCut; // 목표컷 설정
	ss << _TrailStop.MinProfit; // 트레일 스탑 최소 이익
	ss << _TrailStop.TrailingPercent; // 트레일 스탑 퍼센트
	ss << _LossCut; // 로스컷 설정 값
	ss << _TargetProfit; // 목표컷 설정값
	ss << (int)_LossCutType; // 로스컷 타입
	ss << (int)_TargetProfitType; // 목표컷 타입
	ss << _MaxEntrance; // 하루 최대 진입 회수
	ss << (int)_LiqPriceType; // 청산 가격 타입
	ss << _OrderAmount; // 진입 주문 수량
	ss << (int)_FillCondition; // 진입 주문 체결 조건
	ss << (int)_PriceType; // 진입 가격 타입
	ss << _SymbolCode; // 시스템 대상 종목 코드
	ss << _Cycle; // 시스템 실행 주기
	ss << _EntryBarIndex;
	ss << _ATRTime.hour << _ATRTime.min << _ATRTime.sec;
	ss << _ATR;
	ss << _ATRMulti;
	ss << _BandMulti;
	ss << _FilterMulti;

	// 시스템 매개변수를 그룹별로 저장한다.
	ss << _ArgGroupMap.size();
	for (auto it = _ArgGroupMap.begin(); it != _ArgGroupMap.end(); ++it) {
		VtSystemArgGroup& grp = *it;
		grp.Save(ss);
	}
}

/*
ss << _EntryBarIndex;
ss << _ATRTime.hour << _ATRTime.min << _ATRTime.sec;
ss << _ATR;
ss << _ATRMulti;
ss << _BandMulti;
ss << _FilterMulti;
*/
void VtSystem::Load(simple::file_istream<same_endian_type>& ss)
{
	ss >> _Name; // 시스템 이름
	ss >> _SysTargetType; // 시스템 타입 : 실계좌, 서브계좌, 펀드
	ss >> _SysTargetName; // 시스템 대상 이름 : 계좌는 계좌번호, 펀드는 펀드명
	ss >> _EntranceStartTime.hour; // 시스템 진입 시작시간
	ss >> _EntranceStartTime.min;
	ss >> _EntranceStartTime.sec;
	ss >> _EntranceEndTime.hour; // 시스템 진입 종료시간
	ss >> _EntranceEndTime.min;
	ss >> _EntranceEndTime.sec;
	ss >> _LiqTime.hour; // 시스템 청산 시간
	ss >> _LiqTime.min;
	ss >> _LiqTime.sec;
	ss >> _EnableTrailStop; // 트레일 스탑 설정
	ss >> _EnableLossCut; // 로스컷 설정
	ss >> _EnableTargetCut; // 목표컷 설정
	ss >> _TrailStop.MinProfit; // 트레일 스탑 최소 이익
	ss >> _TrailStop.TrailingPercent; // 트레일 스탑 퍼센트
	ss >> _LossCut; // 로스컷 설정 값
	ss >> _TargetProfit; // 목표컷 설정값
	ss >> _LossCutType; // 로스컷 타입
	ss >> _TargetProfitType; // 목표컷 타입
	ss >> _MaxEntrance; // 하루 최대 진입 회수
	ss >> _LiqPriceType; // 청산 가격 타입
	ss >> _OrderAmount; // 진입 주문 수량
	ss >> _FillCondition; // 진입 주문 체결 조건
	ss >> _PriceType; // 진입 가격 타입
	ss >> _SymbolCode; // 시스템 대상 종목 코드
	ss >> _Cycle; // 시스템 실행 주기
	ss >> _EntryBarIndex;
	ss >> _ATRTime.hour >> _ATRTime.min >> _ATRTime.sec;
	ss >> _ATR;
	ss >> _ATRMulti;
	ss >> _BandMulti;
	ss >> _FilterMulti;

	if (_SysTargetType == TargetType::RealAccount) {
		VtAccountManager* acntMgr = VtAccountManager::GetInstance();
		_Account = acntMgr->FindAccount(_SysTargetName);
	}
	else if (_SysTargetType == TargetType::SubAccount) {
		VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
		_Account = subAcntMgr->FindAccount(_SysTargetName);
	}
	else if (_SysTargetType == TargetType::Fund) {
		VtFundManager* fundMgr = VtFundManager::GetInstance();
		_Fund = fundMgr->FindFund(_SysTargetName);
	}

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	_Symbol = symMgr->FindSymbol(_SymbolCode);

	int argGrpCount = 0;
	ss >> argGrpCount;
	if (argGrpCount == 0)
		return;
	// 기존의 매개변수 그룹과 변수들을 모두 제거해 준다.
	_ArgGroupMap.clear();
	for (int i = 0; i < argGrpCount; ++i) {
		VtSystemArgGroup argGrp;
		argGrp.Load(ss);
		AddSystemArgGroup(argGrp.Name(), argGrp);
	}

	_ArgsLoaded = true;
}

bool VtSystem::CheckAtrLiqForBuy()
{
	if (_Symbol && _LastEntryTime != 0) {

		// 현재 종목의 시고저종을 가져온다.
		std::string dataKey = VtChartDataManager::MakeChartDataKey(_Symbol->ShortCode, VtChartType::MIN, _Cycle);
		std::vector<double>& timeArray = _RefDataMap[dataKey]->GetDataArray(_T("time"));
		std::vector<double>& closeArray = _RefDataMap[dataKey]->GetDataArray(_T("close"));
		std::vector<double>& highArray = _RefDataMap[dataKey]->GetDataArray(_T("high"));
		std::vector<double>& lowArray = _RefDataMap[dataKey]->GetDataArray(_T("low"));

		std::vector<double>::iterator itt = std::find(timeArray.begin(), timeArray.end(), _LastEntryTime);
		// 가장 최근에 진입한 봉의 다음 봉의 인덱스를 찾음
		int index = std::distance(timeArray.begin(), itt++);
		// 가장 최근에 진입한 봉의 다음봉부터 현재봉까지의 값 중에서 최고, 최저값을 찾는다.
		auto minMaxIndex = std::minmax_element(closeArray.begin() + index, closeArray.end());
		double maxClose = closeArray[std::distance(closeArray.begin(), minMaxIndex.second)];
		double atr = GetAtr(closeArray.size() - 1, _ATR, highArray.data(), lowArray.data(), closeArray.data());
		if (closeArray.back() < maxClose - atr * _ATRMulti) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

bool VtSystem::CheckAtrLiqForSell()
{
	if (_Symbol && _EnableATRLiq && _LastEntryTime != 0) {

		// 현재 종목의 시고저종을 가져온다.
		std::string dataKey = VtChartDataManager::MakeChartDataKey(_Symbol->ShortCode, VtChartType::MIN, _Cycle);
		std::vector<double>& timeArray = _RefDataMap[dataKey]->GetDataArray(_T("time"));
		std::vector<double>& closeArray = _RefDataMap[dataKey]->GetDataArray(_T("close"));
		std::vector<double>& highArray = _RefDataMap[dataKey]->GetDataArray(_T("high"));
		std::vector<double>& lowArray = _RefDataMap[dataKey]->GetDataArray(_T("low"));

		std::vector<double>::iterator itt = std::find(timeArray.begin(), timeArray.end(), _LastEntryTime);
		// 가장 최근에 진입한 봉의 다음 봉의 인덱스를 찾음
		int index = std::distance(timeArray.begin(), itt++);
		// 가장 최근에 진입한 봉의 다음봉부터 현재봉까지의 값 중에서 최고, 최저값을 찾는다.
		auto minMaxIndex = std::minmax_element(closeArray.begin() + index, closeArray.end());
		double minClose = closeArray[std::distance(closeArray.begin(), minMaxIndex.first)];
		double atr = GetAtr(closeArray.size() - 1, _ATR, highArray.data(), lowArray.data(), closeArray.data());
		if (closeArray.back() > minClose + atr * _ATRMulti) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

bool VtSystem::CheckAtrLiq()
{
	VtTime time = VtGlobal::GetLocalTime();
	if (time.hour >= _ATRTime.hour && time.min >= _ATRTime.min) {
		if (_Symbol && _LastEntryTime != 0) {

			// 현재 종목의 시고저종을 가져온다.
			std::string dataKey = VtChartDataManager::MakeChartDataKey(_Symbol->ShortCode, VtChartType::MIN, _Cycle);
			std::vector<double>& timeArray = _RefDataMap[dataKey]->GetDataArray(_T("time"));
			std::vector<double>& closeArray = _RefDataMap[dataKey]->GetDataArray(_T("close"));
			std::vector<double>& highArray = _RefDataMap[dataKey]->GetDataArray(_T("high"));
			std::vector<double>& lowArray = _RefDataMap[dataKey]->GetDataArray(_T("low"));

			std::vector<double>::iterator itt = std::find(timeArray.begin(), timeArray.end(), _LastEntryTime);
			// 가장 최근에 진입한 봉의 다음 봉의 인덱스를 찾음
			int index = std::distance(timeArray.begin(), itt++);
			// 가장 최근에 진입한 봉의 다음봉부터 현재봉까지의 값 중에서 최고, 최저값을 찾는다.
			auto minMaxIndex = std::minmax_element(closeArray.begin() + index, closeArray.end());
			double atr = GetAtr(closeArray.size() - 1, _ATR, highArray.data(), lowArray.data(), closeArray.data());
			if (_CurPosition == VtPositionType::Buy) {
				double maxClose = closeArray[std::distance(closeArray.begin(), minMaxIndex.second)];
				if (closeArray.back() < maxClose - atr * _ATRMulti) {
					return true;
				}
				else {
					return false;
				}
			}
			else if (_CurPosition == VtPositionType::Sell) {
				double minClose = closeArray[std::distance(closeArray.begin(), minMaxIndex.first)];
				if (closeArray.back() > minClose + atr * _ATRMulti) {
					return true;
				}
				else {
					return false;
				}
			}
			else
				return false;
		}
		else {
			return false;
		}
	}
	else
		return false;
}

bool VtSystem::CheckAtrLiq(int index)
{
	VtTime time = VtGlobal::GetLocalTime();
	if (time.hour >= _ATRTime.hour && time.min >= _ATRTime.min) {
		if (_Symbol && _LastEntryTime != 0) {

			// 현재 종목의 시고저종을 가져온다.
			std::string dataKey = VtChartDataManager::MakeChartDataKey(_Symbol->ShortCode, VtChartType::MIN, _Cycle);
			std::vector<double>& timeArray = _RefDataMap[dataKey]->GetDataArray(_T("time"));
			std::vector<double>& closeArray = _RefDataMap[dataKey]->GetDataArray(_T("close"));
			std::vector<double>& highArray = _RefDataMap[dataKey]->GetDataArray(_T("high"));
			std::vector<double>& lowArray = _RefDataMap[dataKey]->GetDataArray(_T("low"));

			std::vector<double>::iterator itt = std::find(timeArray.begin(), timeArray.end(), _LastEntryTime);
			// 찾지 못하면 거짓을 반환한다.
			if (itt == std::end(timeArray))
				return false;
			// 가장 최근에 진입한 봉의 다음 봉의 인덱스를 찾음
			int index = std::distance(timeArray.begin(), itt++);
			// 가장 최근에 진입한 봉의 다음봉부터 현재봉까지의 값 중에서 최고, 최저값을 찾는다.
			auto minMaxIndex = std::minmax_element(closeArray.begin() + index, closeArray.end());
			double atr = GetAtr(index, _ATR, highArray.data(), lowArray.data(), closeArray.data());
			if (_CurPosition == VtPositionType::Buy) {
				double maxClose = closeArray[std::distance(closeArray.begin(), minMaxIndex.second)];
				if (closeArray[index] < maxClose - atr * _ATRMulti) {
					return true;
				}
				else {
					return false;
				}
			}
			else if (_CurPosition == VtPositionType::Sell) {
				double minClose = closeArray[std::distance(closeArray.begin(), minMaxIndex.first)];
				if (closeArray[index] > minClose + atr * _ATRMulti) {
					return true;
				}
				else {
					return false;
				}
			}
			else
				return false;
		}
		else {
			return false;
		}
	}
	else
		return false;
}

int VtSystem::GetDailyIndex(int index)
{
	if (!_Symbol)
		return -1;

	std::string dataKey = VtChartDataManager::MakeChartDataKey(_Symbol->ShortCode, VtChartType::MIN, _Cycle);
	std::vector<double>& dateArray = _RefDataMap[dataKey]->GetDataArray(_T("date"));
	if (dateArray.size() == 0 || index < 0 || index >= dateArray.size())
		return -1;
	if (dateArray.size() == 1 || index == 0)
		return 0;
	int dateIndex = 0;
	for (size_t i = index; i >= 0; --i) {
		double pre = dateArray[i - 1];
		double cur = dateArray[i];
		if (pre != cur)
			break;
		dateIndex++;
	}

	return dateIndex;
}

int VtSystem::GetDailyIndex()
{
	if (!_Symbol)
		return -1;

	std::string dataKey = VtChartDataManager::MakeChartDataKey(_Symbol->ShortCode, VtChartType::MIN, _Cycle);
	std::vector<double>& dateArray = _RefDataMap[dataKey]->GetDataArray(_T("date"));
	if (dateArray.size() == 0)
		return -1;
	if (dateArray.size() == 1)
		return 0;
	int dateIndex = 0;
	for (size_t i = dateArray.size() - 1; i >= 0; --i) {
		double pre = dateArray[i - 1];
		double cur = dateArray[i];
		if (pre != cur)
			break;
		dateIndex++;
	}

	return dateIndex;
}

void VtSystem::AddSystemArg(std::string groupName, VtSystemArg arg)
{
	VtSystemArgGroup* argGrp = FindArgGroup(groupName);
	if (argGrp) {
		argGrp->AddSystemArg(arg.Name, arg);
	} else {
		VtSystemArgGroup group;
		group.Name(groupName);
		group.AddSystemArg(arg.Name, arg);
		_ArgGroupMap.push_back(group);
	}
}

void VtSystem::AddSystemArgGroup(std::string groupName, VtSystemArgGroup grp)
{
	VtSystemArgGroup* argGrp = FindArgGroup(groupName);
	if (!argGrp) {
		_ArgGroupMap.push_back(grp);
	}
}

bool VtSystem::CheckLossCut(int index)
{
	if (!_EnableLossCut || !_Symbol)
		return false;

	if (_SysTargetType == TargetType::RealAccount || _SysTargetType == TargetType::SubAccount) {
		if (!_Account)
			return false;
		VtPosition* posi = _Account->FindPosition(_Symbol->ShortCode);
		if (!posi || posi->OpenQty == 0)
			return false;
		if (_TargetProfitType == ValueType::Tick) { // 틱으로 청산
			if (VtCutManager::CheckCutLoss(posi, _Symbol, (int)_LossCut) && LiqudAll()) {
				return true;
			}
		}
		else { // 퍼센트로 청산
			if (VtCutManager::CheckCutLossByPercent(posi, _Symbol, _LossCut) && LiqudAll()) {
				return true;
			}
		}
	}
	else {
		if (!_Fund)
			return false;
		int count = 0;
		VtPosition posi = _Fund->GetPosition(_Symbol->ShortCode, count);
		if (count == 0 || posi.OpenQty == 0) {
			return false;
		}

		if (_TargetProfitType == ValueType::Tick) { // 틱으로 청산
			if (VtCutManager::CheckCutLoss(&posi, _Symbol, (int)_LossCut * _Fund->GetTotalSeungSu()) && LiqudAll()) {
				return true;
			}
		}
		else { // 퍼센트로 청산
			if (VtCutManager::CheckCutLossByPercent(&posi, _Symbol, _LossCut) && LiqudAll()) {
				return true;
			}
		}
	}

	return false;
}

bool VtSystem::CheckProfitCut(int index)
{
	if (!_EnableLossCut || !_Symbol)
		return false;

	if (_SysTargetType == TargetType::RealAccount || _SysTargetType == TargetType::SubAccount) {
		if (!_Account)
			return false;
		VtPosition* posi = _Account->FindPosition(_Symbol->ShortCode);
		if (!posi || posi->OpenQty == 0)
			return false;
		if (_TargetProfitType == ValueType::Tick) { // 틱으로 청산
			if (VtCutManager::CheckCutProfit(posi, _Symbol, (int)_TargetProfit) && LiqudAll()) {
				return true;
			}
		}
		else { // 퍼센트로 청산
			if (VtCutManager::CheckCutProfitByPercent(posi, _Symbol, _TargetProfit) && LiqudAll()) {
				return true;
			}
		}
	}
	else {
		if (!_Fund)
			return false;
		int count = 0;
		VtPosition posi = _Fund->GetPosition(_Symbol->ShortCode, count);
		if (count == 0 || posi.OpenQty == 0) {
			return false;
		}

		if (_TargetProfitType == ValueType::Tick) { // 틱으로 청산
			if (VtCutManager::CheckCutProfit(&posi, _Symbol, (int)_TargetProfit * _Fund->GetTotalSeungSu()) && LiqudAll()) {
				return true;
			}
		}
		else { // 퍼센트로 청산
			if (VtCutManager::CheckCutProfitByPercent(&posi, _Symbol, _TargetProfit) && LiqudAll()) {
				return true;
			}
		}
	}

	return false;
}

bool VtSystem::CheckTrailStop(int index)
{
	if (!_EnableTrailStop || _CurPosition == VtPositionType::None)
		return false;

	// 최대이익이 트레일 스탑 최소이익에 도달하지 않았으면 아무일도 하지 않는다.
	if (_MaxProfit < _TrailStop.MinProfit)
		return false;

	// 최대이익에서 손실가능한 %를 구한다. 예를들면 30%면 최대이익의 30% 금액을 구한다.
	double lossProfit = _MaxProfit * _TrailStop.TrailingPercent / 100.0;
	// 현재이익이 최대이익에서 손실 가능한 금액을 뺀 것보다 작다면 모두 청산을 시도한다.
	// 이부분이 문제임 - 잘못하면 진입이 되지 않음
	if (_ProfitLoss < (_MaxProfit - lossProfit)) {
		if (LiqudAll()) {
			return true;
		}
		else
			return false;
	}
	return false;
}

void VtSystem::UpdateSystem(int index)
{
	if (_SysTargetType == TargetType::RealAccount || _SysTargetType == TargetType::SubAccount) {
		if (!_Account)
			return;

		VtPosition* posi = _Account->FindPosition(_Symbol->ShortCode);
		if (posi)
			_ProfitLoss = posi->OpenProfitLoss;
	}
	else {
		if (!_Fund)
			return ;

		int count = 0;
		VtPosition posi = _Fund->GetPosition(_Symbol->ShortCode, count);
		if (count == 0) {
			return;
		}
		_ProfitLoss = posi.OpenProfitLoss;
	}

	// 최대 이익을 갱신한다.
	if (_ProfitLoss > _MaxProfit) {
		_MaxProfit = _ProfitLoss;
	}
}

VtSystemArgGroup* VtSystem::GetArgGroup(std::string grpName)
{
	for (auto it = _ArgGroupMap.begin(); it != _ArgGroupMap.end(); ++it) {
		VtSystemArgGroup& argGrp = *it;
		if (argGrp.Name().compare(grpName) == 0) {
			return &argGrp;
		}
	}

	return nullptr;
}

VtSystemArgGroup* VtSystem::FindArgGroup(std::string grpName)
{
	for (auto it = _ArgGroupMap.begin(); it != _ArgGroupMap.end(); ++it) {
		VtSystemArgGroup& argGrp = *it;
		if (argGrp.Name().compare(grpName) == 0) {
			return &argGrp;
		}
	}

	return nullptr;
}

void VtSystem::CreateSignal(int startIndex, int endIndex)
{

}

VtPositionType VtSystem::UpdateSignal(int index)
{
	return VtPositionType::None;
}

VtPositionType VtSystem::UpdateSignal(int start, int end)
{
	return VtPositionType::None;
}

double VtSystem::GetMA(int index, double* data, TA_MAType type, int period)
{
	int startIdx = index, endIdx = index;
	int outBegIdx = 0, outNbElement = 0;
	double temp[2] = { 0.0, 0.0 };
	int retCode = TA_MA(startIdx,
		endIdx,
		data,
		period,
		type,
		&outBegIdx,
		&outNbElement,
		temp);

	return temp[0];
}

void VtSystem::GetStochastic(int index, double* high, double* low, double* close, int fastK, int slowK, int slowD, double& outSlowK, double& outSlowD)
{
	int startIdx = index, endIdx = index;
	int outBegIdx = 0, outNbElement = 0;
	double aSlowK[2] = { 0.0, 0.0 };
	double aSlowD[2] = { 0.0, 0.0 };
	int retCode = TA_STOCH(startIdx,
		endIdx,
		high,
		low,
		close,
		fastK,
		slowK,
		TA_MAType::TA_MAType_EMA,
		slowD,
		TA_MAType::TA_MAType_EMA,
		&outBegIdx,
		&outNbElement,
		aSlowK,
		aSlowD);

	outSlowK = aSlowK[0];
	outSlowD = aSlowD[0];
}

void VtSystem::GetMacd(int index, double* data, int shortPeriod, int longPeriod, int signalPeriod, double& outMacd, double& outMacdSignal, double& outMacdHist)
{
	int startIdx = index, endIdx = index;
	int outBegIdx = 0, outNbElement = 0;
	double aMacd[2] = { 0.0, 0.0 };
	double aMacdSignal[2] = { 0.0, 0.0 };
	double aMacdHist[2] = { 0.0, 0.0 };
	int retCode = TA_MACD(startIdx,
		endIdx,
		data,
		shortPeriod,
		longPeriod,
		signalPeriod,
		&outBegIdx,
		&outNbElement,
		aMacd,
		aMacdSignal,
		aMacdHist);

	outMacd = aMacd[0];
	outMacdSignal = aMacdSignal[0];
	outMacdHist = aMacdHist[0];
}

void VtSystem::GetHLChannelValue(int index, int period, double* high, double* low, double& highest, double& lowest)
{
	highest = Chart::NoValue;
	lowest = Chart::NoValue;

	if (index < period) {
		highest = Chart::NoValue;
	} else {
		highest = high[index - period];
		for (int k = index - period; k <= index - 1; ++k) {
			if (high[k] > highest)
				highest = high[k];
		}
	}

	if (index < period) {
		lowest = Chart::NoValue;
	} else {
		lowest = low[index - period];
		for (int k = index - period; k <= index - 1; ++k) {
			if (low[k] < lowest)
				lowest = low[k];
		}
	}
}

double VtSystem::GetHighest(int index, int period, double* data)
{
	double highest = Chart::NoValue;

	if (index < period) {
		highest = Chart::NoValue;
	} else {
		highest = data[index - period];
		for (int k = index - period; k <= index - 1; ++k) {
			if (data[k] > highest)
				highest = data[k];
		}
	}

	return highest;
}

double VtSystem::GetLowest(int index, int period, double* data)
{
	double lowest = Chart::NoValue;

	if (index < period) {
		lowest = Chart::NoValue;
	} else {
		lowest = data[index - period];
		for (int k = index - period; k <= index - 1; ++k)
		{
			if (data[k] < lowest)
				lowest = data[k];
		}
	}

	return lowest;
}

double VtSystem::GetAtr(int index, int period, double* high, double* low, double* close)
{
	int startIdx = index, endIdx = index;
	int outBegIdx = 0, outNbElement = 0;
	double temp[2] = { 0.0, 0.0 };
	int retCode = TA_ATR(
		startIdx,
		endIdx,
		high,
		low,
		close,
		period,
		&outBegIdx,
		&outNbElement,
		temp);

	return temp[0];
}

void VtSystem::ClearSignal()
{
	for (auto it = SignalVector.begin(); it != SignalVector.end(); ++it) {
		delete *it;
	}

	SignalVector.clear();
}

void VtSystem::ReloadSystem(int startIndex, int endIndex)
{

}

void VtSystem::InitArgs()
{
	
}

void VtSystem::SetDataMap(VtChartData* chartData)
{
	DateTime(chartData->DateTime.data());
	Date(chartData->Date.data());
	Time(chartData->Time.data());
}

void VtSystem::ReadExtraArgs()
{
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
					_EnableByBand = true;
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
				else if (arg.Name.compare(_T("c>o")) == 0) {
					_EnableByOpen = true;
				}
			} 
			else {
				if (arg.Name.compare(_T("ATRMulti")) == 0) {
					_ATRMulti = std::stod(arg.sValue);
					_EnableATRLiq = false;
				}
				else if (arg.Name.compare(_T("BandMulti")) == 0) {
					_BandMulti = std::stod(arg.sValue);
					_EnableByBand = false;
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
				else if (arg.Name.compare(_T("c>o")) == 0) {
					_EnableByOpen = false;
				}
			}
		}
	}
}

void VtSystem::ResetSignal()
{
	ClearSignal();
	_AppliedData = false;
	_Running = false;
}

// void VtSystem::TargetSymbol(std::string val)
// {
// 	_TargetSymbol = val;
// 	_DataSrcSymbolVec.push_back(val);
// }

void VtSystem::SetExtraTargetSymbol(std::string symCode)
{
	_DataSrcSymbolVec.push_back(symCode);
}

void VtSystem::Enable(bool val)
{
	_Enable = val;
	if (!_Enable && _LiqByStop) { // 시스템 비활성화시 잔고 청산 여부 확인
		if (_SysTargetType == TargetType::RealAccount || _SysTargetType == TargetType::SubAccount) {
			if (!_Account)
				return;

			VtPosition* posi = _Account->FindPosition(_Symbol->ShortCode);
			if (posi && std::abs(posi->OpenQty) > 0) {
				int res = AfxMessageBox(_T("잔고가 남아있습니다! 청산하시겠습니까?"), MB_YESNO);
				if (res == IDYES)
					LiqudAll();
			}
		}
		else {
			if (!_Fund)
				return;

			int count = 0;
			VtPosition posi = _Fund->GetPosition(_Symbol->ShortCode, count);
			if (count == 0) {
				return;
			}
			if (std::abs(posi.OpenQty) > 0) {
				int res = AfxMessageBox(_T("잔고가 남아있습니다! 청산하시겠습니까?"), MB_YESNO);
				if (res == IDYES)
					LiqudAll();
			}
		}
	}
}

double VtSystem::GetDailyHigh(int index, double* datetime, double* high, int backLen)
{
	int startIndex = 0, endIndex = 0;
	if (datetime == nullptr || high == nullptr)
		return -1.0;

	startIndex = FindDayStartIndex(index, datetime, backLen + 1);
	endIndex = FindDayStartIndex(index, datetime, backLen);

	if (startIndex < 0 || endIndex < 0)
		return -1.0;

	double highest = high[startIndex];
	for (int i = startIndex; i < endIndex; ++i)
	{
		if (high[i] > highest)
			highest = high[i];
	}

	return highest;
}

double VtSystem::GetDailyLow(int index, double* datetime, double* low, int backLen)
{
	int startIndex = 0, endIndex = 0;
	if (datetime == nullptr || low == nullptr)
		return 0.0;
	startIndex = FindDayStartIndex(index, datetime, backLen + 1);
	endIndex = FindDayStartIndex(index, datetime, backLen);

	if (startIndex < 0 || endIndex < 0)
		return -1.0;

	CString msg;
	msg.Format(_T("startIndex = %d, endIndex = %d\n"), startIndex, endIndex);
	TRACE(msg);

	double lowest = low[startIndex];
	for (int i = startIndex; i < endIndex; ++i) {
		if (low[i] < lowest)
			lowest = low[i];
	}

	return lowest;
}

int VtSystem::FindDayStartIndex(int index, double* datetime, int backLen)
{
	double newDate = Chart::getChartYMD(datetime[index]);
	int count = 0;
	for (int i = index; i >= 0; --i) {
		double oldDate = Chart::getChartYMD(datetime[i]);
		if (oldDate != newDate) {
			newDate = oldDate;
			count++;
		}
		if (backLen == count)
			return i;
	}

	return -1;
}

double VtSystem::GetDailyHeight(int index, double* datetime, double* high, double* low, int backLen)
{
	if (datetime == nullptr || high == nullptr || low == nullptr)
		return -1.0;

	double highest = GetDailyHigh(index, datetime, high, backLen);
	double lowest = GetDailyLow(index, datetime, low, backLen);

	if (highest < 0 || lowest < 0)
		return -1.0;

	return (highest - lowest);
}



double VtSystem::GetOpen(int index, double* datetime, VtTime* time, int hour, double* open)
{
	double openValue = 0.0;

	if (datetime == nullptr || time == nullptr)
		return openValue;

	int startIndex = FindDayStartIndex(index, datetime, 1);
	for (int i = startIndex + 1; i <= index; ++i) {
		VtTime curTime = time[i];
		if (hour == curTime.hour) {
			openValue = open[i];
			break;
		}
	}

	return openValue;
}

int VtSystem::EntriesToday(double datetime)
{
	double givenDate = Chart::getChartYMD(datetime);
	int count = 0;
	for (auto it = SignalVector.rbegin(); it != SignalVector.rend(); ++it) {
		VtSignal* sig = *it;
		double sigDate = Chart::getChartYMD(sig->DateTime());
		if ((sig->SignalType() == VtPositionType::Buy || sig->SignalType() == VtPositionType::Sell) && 
			givenDate == sigDate)
			count++;
		if (sigDate < givenDate)
			break;
	}

	return count;
}

void VtSystem::SaveToXml(pugi::xml_node& node)
{
	CString msg;
	std::string value;

	msg.Format(_T("%d"), _SystemType);
	value = (LPCTSTR)msg;
	pugi::xml_node system_type = node.append_child("system_type");
	system_type.append_child(pugi::node_pcdata).set_value(value.c_str());

	pugi::xml_node system_name = node.append_child("system_name");
	value = _Name;
	system_name.append_child(pugi::node_pcdata).set_value(value.c_str());

	pugi::xml_node system_custom_name = node.append_child("system_custom_name");
	value = _CustomName;
	system_custom_name.append_child(pugi::node_pcdata).set_value(value.c_str());

}

void VtSystem::LoadFromXml(pugi::xml_node& node)
{
	std::string system_type = node.child_value("system_type");

	_SystemType = (VtSystemType)std::stoi(system_type);
	_Name = node.child_value("system_name");

	_CustomName = node.child_value("system_custom_name");

}
