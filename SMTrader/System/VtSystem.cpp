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
#include "../VtSystemDef.h"
#include "../VtTotalOrderManager.h"
#include "../VtOutSignalDef.h"
#include "../VtOutSignalDefManager.h"
#include "../Util/VtTime.h"
#include "../VtScheduler.h"

VtSystem::VtSystem()
{
	_Name = _T("NoTitle");
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
	_Name = _T("NoTitle");
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
	// ���⼭ �ǽð� ��Ʈ ������ ������ ����� �ش�.
	VtChartDataCollector* chartDataCollector = VtChartDataCollector::GetInstance();
	VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
	VtChartData* chartData = chartDataMgr->FindNAdd(symCode, type, cycle);
	if (chartData) {
		_RefDataMap[chartData->Key()] = chartData;
		chartDataCollector->RegisterChartData(chartData);
	}

	return chartData;
}

void VtSystem::Fund(VtFund* val)
{
	if (val) {
		_Fund = val;
		_SysTargetType = TargetType::Fund;
		_SysTargetName = _Fund->Name;
	}
}

void VtSystem::OnTimer()
{
	
}

void VtSystem::OnRegularTimer()
{
	if (_AllSettled)
		return;
	if (LiqByEndTime()) {
		// ���⼭ ���� ������ ���¸� ������ �߰��� �ֹ��� ������ �ʰ� �Ѵ�.
		_CurPosition = VtPositionType::None;
		_LastEntryDailyIndex = -1;
		LOG_F(INFO, _T("û��ð��� ���� û�꼺��"));
		_AllSettled = true;
		return;
	}
}

bool VtSystem::IsEnterableByTime()
{
	int curTime = VtGlobal::GetTime(VtGlobal::GetLocalTime());
	int startTime = VtGlobal::GetTime(_EntranceStartTime);
	int endTime = VtGlobal::GetTime(_EntranceEndTime);
	if (curTime < startTime || curTime > endTime)
		return false;
	else
		return true;
}

bool VtSystem::LiqByEndTime()
{
	int curTime = VtGlobal::GetTime(VtGlobal::GetLocalTime());
	int liqTime = VtGlobal::GetTime(_LiqTime);
	LOG_F(INFO, _T("û��ð��� ���� û��õ� : ���� ������ = %d, ���� �ð� = %d, û�� �ð� = %d"), _CurPosition, curTime, liqTime);
	// ���⼭ û���� �����Ѵ�.
	if (LiqudAll())
		return true;
	else
		return false;
}

bool VtSystem::LiqByEndTime(int index)
{
	if (!_Symbol)
		return false;

	std::string dataKey = VtChartDataManager::MakeChartDataKey(_Symbol->ShortCode, VtChartType::MIN, _Cycle);
	VtChartData* chartData = _RefDataMap[dataKey];
	if (!chartData)
		return false;

	std::vector<double>& timeArray = chartData->GetDataArray(_T("time"));
	if (timeArray.size() == 0 || index < 0 || index >= timeArray.size())
		return false;

	VtTime time = VtGlobal::GetTime(int(timeArray[index]));
	int curTime = VtGlobal::GetTime(time);
	int liqTime = VtGlobal::GetTime(_LiqTime);
	if (curTime >= liqTime) {
		// ���⼭ û���� �����Ѵ�.
		if (LiqudAll())
			return true;
		else
			return false;
	}

	return false;
}

bool VtSystem::CheckFilterMulti()
{
	if (!_Symbol)
		return false;

	std::string code = _Symbol->ShortCode;
	std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::DAY, 1);
	VtChartData* chartData = _RefDataMap[dataKey];
	if (!chartData)
		return false;
	std::vector<double>& highArray = chartData->GetDataArray(_T("high"));
	std::vector<double>& lowArray = chartData->GetDataArray(_T("low"));

	double preDayHigh = highArray[highArray.size() - 2];
	double preDayLow = lowArray[lowArray.size() - 2];
	double _PreHL = preDayHigh - preDayLow;
	// ���� �������� Ŭ���� �������� �ʴ´�.
	if (_FilterMulti < _PreHL)
		return false;
	else
		return true;
}

bool VtSystem::CheckFilterMulti(int index)
{
	if (!_Symbol || index < 0 || index >= ChartDataSize)
		return false;

	std::string code = _Symbol->ShortCode;
	std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::DAY, 1);
	VtChartData* chartData = _RefDataMap[dataKey];
	if (!chartData)
		return false;

	std::vector<double>& dayDateArray = chartData->GetDataArray(_T("date"));
	std::vector<double>& highArray = chartData->GetDataArray(_T("high"));
	std::vector<double>& lowArray = chartData->GetDataArray(_T("low"));

	dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
	chartData = _RefDataMap[dataKey];
	if (!chartData)
		return false;
	std::vector<double>& maindateArray = chartData->GetDataArray(_T("date"));
	int curDayIndex = FindDateIndex(maindateArray[index], dayDateArray);
	if (curDayIndex <= 0)
		return false;

	double preDayHigh = highArray[curDayIndex - 1];
	double preDayLow = lowArray[curDayIndex - 1];
	double _PreHL = preDayHigh - preDayLow;
	// ���� �������� Ŭ���� �������� �ʴ´�.
	if (_FilterMulti < _PreHL)
		return false;
	else
		return true;
}

bool VtSystem::CheckBarIndex()
{
	if (GetDailyIndex() + 1 <= _EntryBarIndex)
		return false;
	else
		return true;
}

bool VtSystem::CheckBarIndex(int index)
{
	if (GetDailyIndex(index) + 1 <= _EntryBarIndex)
		return false;
	else
		return true;
}

bool VtSystem::CheckEntranceByBandForBuy()
{
	if (!_Symbol)
		return false;

	std::string code = _Symbol->ShortCode;
	std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::DAY, 1);
	VtChartData* chartData = _RefDataMap[dataKey];
	if (!chartData)
		return false;
	std::vector<double>& highArray = chartData->GetDataArray(_T("high"));
	std::vector<double>& lowArray = chartData->GetDataArray(_T("low"));

	double preDayHigh = highArray[highArray.size() - 2];
	double preDayLow = lowArray[lowArray.size() - 2];
	double _PreHL = preDayHigh - preDayLow;
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
	VtChartData* chartData = _RefDataMap[dataKey];
	if (!chartData)
		return false;

	std::vector<double>& dayDateArray = chartData->GetDataArray(_T("date"));
	std::vector<double>& highArray = chartData->GetDataArray(_T("high"));
	std::vector<double>& lowArray = chartData->GetDataArray(_T("low"));

	dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
	chartData = _RefDataMap[dataKey];
	if (!chartData)
		return false;
	std::vector<double>& maindateArray = chartData->GetDataArray(_T("date"));
	int curDayIndex = FindDateIndex(maindateArray[index], dayDateArray);
	if (curDayIndex <= 0)
		return false;

	double preDayHigh = highArray[curDayIndex - 1];
	double preDayLow = lowArray[curDayIndex - 1];
	double _PreHL = preDayHigh - preDayLow;
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
	VtChartData* chartData = _RefDataMap[dataKey];
	if (!chartData)
		return false;
	std::vector<double>& highArray = chartData->GetDataArray(_T("high"));
	std::vector<double>& lowArray = chartData->GetDataArray(_T("low"));

	double preDayHigh = highArray[highArray.size() - 2];
	double preDayLow = lowArray[lowArray.size() - 2];
	double _PreHL = preDayHigh - preDayLow;
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
	VtChartData* chartData = _RefDataMap[dataKey];
	if (!chartData)
		return false;
	std::vector<double>& dayDateArray = chartData->GetDataArray(_T("date"));
	std::vector<double>& highArray = chartData->GetDataArray(_T("high"));
	std::vector<double>& lowArray = chartData->GetDataArray(_T("low"));

	dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
	chartData = _RefDataMap[dataKey];
	if (!chartData)
		return false;
	std::vector<double>& maindateArray = chartData->GetDataArray(_T("date"));
	int curDayIndex = FindDateIndex(maindateArray[index], dayDateArray);
	if (curDayIndex <= 0)
		return false;

	double preDayHigh = highArray[curDayIndex - 1];
	double preDayLow = lowArray[curDayIndex - 1];
	double _PreHL = preDayHigh - preDayLow;
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
	VtChartData* chartData = _RefDataMap[dataKey];
	if (!chartData)
		return false;
	std::vector<double>& dayDateArray = chartData->GetDataArray(_T("date"));
	std::vector<double>& openArray = chartData->GetDataArray(_T("open"));
	std::vector<double>& closeArray = chartData->GetDataArray(_T("close"));

	dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
	chartData = _RefDataMap[dataKey];
	if (!chartData)
		return false;
	std::vector<double>& maindateArray = chartData->GetDataArray(_T("date"));
	int curDayIndex = FindDateIndex(maindateArray[index], dayDateArray);
	if (curDayIndex <= 0)
		return false;
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
	VtChartData* chartData = _RefDataMap[dataKey];
	if (!chartData)
		return false;
	std::vector<double>& dayDateArray = chartData->GetDataArray(_T("date"));
	std::vector<double>& openArray = chartData->GetDataArray(_T("open"));
	std::vector<double>& closeArray = chartData->GetDataArray(_T("close"));

	dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
	chartData = _RefDataMap[dataKey];
	if (!chartData)
		return false;
	std::vector<double>& maindateArray = chartData->GetDataArray(_T("date"));
	int curDayIndex = FindDateIndex(maindateArray[index], dayDateArray);
	if (curDayIndex <= 0)
		return false;

	if (openArray.size() == 0 || closeArray.size() == 0)
		return false;

	return  closeArray[curDayIndex] < openArray[curDayIndex] ? true : false;
}

bool VtSystem::CheckByArg(ArgNameType argName, VtSymbol* sym, double param, bool log)
{
	if (!sym)
		return false;

	switch (argName)
	{
	case ArgNameType::KbsMiKas:
		if (log) LOG_F(INFO, _T("Kbs-Kas : param = %.0f, value = %d"), param, sym->Hoga.TotBuyQty - sym->Hoga.TotSellQty);
		if (sym->Hoga.TotBuyQty - sym->Hoga.TotSellQty > param) {
			return true;
		}
		else {
			return false;
		}
		break;
	case ArgNameType::KbcGtKac:
		if (log) LOG_F(INFO, _T("Kbc>Kac : param = %.2f, first = %.2f > last = %d"), param, sym->Hoga.TotBuyNo*param, sym->Hoga.TotSellNo);
		if (sym->Hoga.TotBuyNo*param > sym->Hoga.TotSellNo) {
			return true;
		}
		else {
			return false;
		}
		break;
	case ArgNameType::QbcGtQac:
		if (log) LOG_F(INFO, _T("Qbc>Qac : param = %.2f, first = %.2f > last = %d"), param, sym->Hoga.TotBuyNo*param, sym->Hoga.TotSellNo);
		if (sym->Hoga.TotBuyNo*param > sym->Hoga.TotSellNo) {
			return true;
		}
		else {
			return false;
		}
		break;
	case ArgNameType::UacGtUbc:
		if (log) LOG_F(INFO, _T("Uac>Ubc : param = %.2f, first = %.2f > last = %d"), param, sym->Hoga.TotSellNo*param, sym->Hoga.TotBuyNo);
		if (sym->Hoga.TotSellNo*param > sym->Hoga.TotBuyNo) {
			return true;
		}
		else {
			return false;
		}
		break;
	case ArgNameType::KasMiKbs:
		if (log) LOG_F(INFO, _T("Kas-Kbs : param = %.0f, value = %d"), param, sym->Hoga.TotSellQty - sym->Hoga.TotBuyQty);
		if (sym->Hoga.TotSellQty - sym->Hoga.TotBuyQty > param) {
			return true;
		}
		else {
			return false;
		}
		break;
	case ArgNameType::KacGtKbc:
		if (log) LOG_F(INFO, _T("Kac>Kbc : param = %.2f, first = %.2f > last = %d"), param, sym->Hoga.TotSellNo*param, sym->Hoga.TotBuyNo);
		if (sym->Hoga.TotSellNo*param > sym->Hoga.TotBuyNo) {
			return true;
		}
		else {
			return false;
		}
		break;
	case ArgNameType::QacGtQbc:
		if (log) LOG_F(INFO, _T("Qac>Qbc : param = %.2f, first = %.2f > last = %d"), param, sym->Hoga.TotSellNo*param, sym->Hoga.TotBuyNo);
		if (sym->Hoga.TotSellNo*param > sym->Hoga.TotBuyNo) {
			return true;
		}
		else {
			return false;
		}
		break;
	case ArgNameType::UbcGtUac:
		if (log) LOG_F(INFO, _T("Ubc>Uac : param = %.2f, first = %.2f > last = %d"), param, sym->Hoga.TotBuyNo*param, sym->Hoga.TotSellNo);
		if (sym->Hoga.TotBuyNo*param > sym->Hoga.TotSellNo) {
			return true;
		}
		else {
			return false;
		}
		break;
	case ArgNameType::QbsGtQas:
		if (log) LOG_F(INFO, _T("Qbs>Qas : param = %.2f, first = %.2f > last = %d"), param, sym->Hoga.TotBuyQty*param, sym->Hoga.TotSellQty);
		if (sym->Hoga.TotBuyQty*param > sym->Hoga.TotSellQty) {
			return true;
		}
		else {
			return false;
		}
		break;
	case ArgNameType::QasGtQbs:
		if (log) LOG_F(INFO, _T("Qas>Qbs : param = %.2f, first = %.2f > last = %d"), param, sym->Hoga.TotSellQty*param, sym->Hoga.TotBuyQty);
		if (sym->Hoga.TotSellQty*param > sym->Hoga.TotBuyQty) {
			return true;
		}
		else {
			return false;
		}
		break;
	case ArgNameType::UbsGtUas:
		if (log) LOG_F(INFO, _T("Ubs>Uas : param = %.2f, first = %.2f > last = %d"), param, sym->Hoga.TotBuyQty*param, sym->Hoga.TotSellQty);
		if (sym->Hoga.TotBuyQty*param > sym->Hoga.TotSellQty) {
			return true;
		}
		else {
			return false;
		}
		break;
	case ArgNameType::UasGtUbs:
		if (log) LOG_F(INFO, _T("Uas>Ubs : param = %.2f, first = %.2f > last = %d"), param, sym->Hoga.TotSellQty*param, sym->Hoga.TotBuyQty);
		if (sym->Hoga.TotSellQty*param > sym->Hoga.TotBuyQty) {
			return true;
		}
		else {
			return false;
		}
		break;
	case ArgNameType::KasGtKbs:
		if (log) LOG_F(INFO, _T("Kas>Kbs : param = %.2f, first = %.2f > last = %d"), param, sym->Hoga.TotSellQty*param, sym->Hoga.TotBuyQty);
		if (sym->Hoga.TotSellQty*param > sym->Hoga.TotBuyQty) {
			return true;
		}
		else {
			return false;
		}
		break;
	case ArgNameType::KbsGtKas:
		if (log) LOG_F(INFO, _T("Kbs>Kas : param = %.2f, first = %.2f > last = %d"), param, sym->Hoga.TotBuyQty*param, sym->Hoga.TotSellQty);
		if (sym->Hoga.TotBuyQty*param > sym->Hoga.TotSellQty) {
			return true;
		}
		else {
			return false;
		}
		break;
	default:
		break;
	}

	return false;
}

bool VtSystem::CheckByArg(ArgNameType argName, VtSymbol* sym, double param, int index)
{
	if (!sym || index < 0)
		return false;

	switch (argName)
	{
	case ArgNameType::KbsMiKas: {
		std::string code = sym->ShortCode + (_T("SHTQ"));
		std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		VtChartData* chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& sellVec= chartData->GetDataArray(_T("close"));
		if (sellVec.size() == 0 || index <= sellVec.size())
			return false;

		code = sym->ShortCode + (_T("BHTQ"));
		dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& buyVec = chartData->GetDataArray(_T("close"));
		if (buyVec.size() == 0 || index <= buyVec.size())
			return false;

		if (buyVec[index] - sellVec[index] > param) {
			return true;
		}
		else {
			return false;
		}
	}
		break;
	case ArgNameType::KbcGtKac: {
		std::string code = sym->ShortCode + (_T("SHTC"));
		std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		VtChartData* chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& sellVec = chartData->GetDataArray(_T("close"));
		if (sellVec.size() == 0 || index <= sellVec.size())
			return false;

		code = sym->ShortCode + (_T("BHTC"));
		dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& buyVec = chartData->GetDataArray(_T("close"));
		if (buyVec.size() == 0 || index <= buyVec.size())
			return false;

		if (buyVec[index]*param > sellVec[index]) {
			return true;
		}
		else {
			return false;
		}
	}
		break;
	case ArgNameType::QbcGtQac: {
		std::string code = sym->ShortCode + (_T("SHTC"));
		std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		VtChartData* chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& sellVec = chartData->GetDataArray(_T("close"));
		if (sellVec.size() == 0 || index <= sellVec.size())
			return false;

		code = sym->ShortCode + (_T("BHTC"));
		dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& buyVec = chartData->GetDataArray(_T("close"));
		if (buyVec.size() == 0 || index <= buyVec.size())
			return false;

		if (buyVec[index] *param > sellVec[index]) {
			return true;
		}
		else {
			return false;
		}
	}
		break;
	case ArgNameType::UacGtUbc: {
		std::string code = sym->ShortCode + (_T("SHTC"));
		std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		VtChartData* chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& sellVec = chartData->GetDataArray(_T("close"));
		if (sellVec.size() == 0 || index <= sellVec.size())
			return false;

		code = sym->ShortCode + (_T("BHTC"));
		dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& buyVec = chartData->GetDataArray(_T("close"));
		if (buyVec.size() == 0 || index <= buyVec.size())
			return false;

		if (sellVec[index]*param > buyVec[index]) {
			return true;
		}
		else {
			return false;
		}
	}
		break;
	case ArgNameType::KasMiKbs: {
		std::string code = sym->ShortCode + (_T("SHTQ"));
		std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		VtChartData* chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& sellVec = chartData->GetDataArray(_T("close"));
		if (sellVec.size() == 0 || index <= sellVec.size())
			return false;

		code = sym->ShortCode + (_T("BHTQ"));
		dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& buyVec = chartData->GetDataArray(_T("close"));
		if (buyVec.size() == 0 || index <= buyVec.size())
			return false;

		if (sellVec[index] - buyVec[index] > param) {
			return true;
		}
		else {
			return false;
		}
	}
		break;
	case ArgNameType::KacGtKbc: {
		std::string code = sym->ShortCode + (_T("SHTC"));
		std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		VtChartData* chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& sellVec = chartData->GetDataArray(_T("close"));
		if (sellVec.size() == 0 || index <= sellVec.size())
			return false;

		code = sym->ShortCode + (_T("BHTC"));
		dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& buyVec = chartData->GetDataArray(_T("close"));
		if (buyVec.size() == 0 || index <= buyVec.size())
			return false;

		if (sellVec[index] *param > buyVec[index]) {
			return true;
		}
		else {
			return false;
		}
	}
		break;
	case ArgNameType::QacGtQbc: {
		std::string code = sym->ShortCode + (_T("SHTC"));
		std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		VtChartData* chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& sellVec = chartData->GetDataArray(_T("close"));
		if (sellVec.size() == 0 || index <= sellVec.size())
			return false;

		code = sym->ShortCode + (_T("BHTC"));
		dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& buyVec = chartData->GetDataArray(_T("close"));
		if (buyVec.size() == 0 || index <= buyVec.size())
			return false;

		if (sellVec[index]*param > buyVec[index]) {
			return true;
		}
		else {
			return false;
		}
	}
		break;
	case ArgNameType::UbcGtUac: {
		std::string code = sym->ShortCode + (_T("SHTC"));
		std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		VtChartData* chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& sellVec = chartData->GetDataArray(_T("close"));
		if (sellVec.size() == 0 || index <= sellVec.size())
			return false;

		code = sym->ShortCode + (_T("BHTC"));
		dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& buyVec = chartData->GetDataArray(_T("close"));
		if (buyVec.size() == 0 || index <= buyVec.size())
			return false;

		if (buyVec[index]*param > sellVec[index]) {
			return true;
		}
		else {
			return false;
		}
	}
		break;
	case ArgNameType::QbsGtQas: {
		std::string code = sym->ShortCode + (_T("SHTQ"));
		std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		VtChartData* chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& sellVec = chartData->GetDataArray(_T("close"));
		if (sellVec.size() == 0 || index <= sellVec.size())
			return false;

		code = sym->ShortCode + (_T("BHTQ"));
		dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& buyVec = chartData->GetDataArray(_T("close"));
		if (buyVec.size() == 0 || index <= buyVec.size())
			return false;

		if (buyVec[index]*param > sellVec[index]) {
			return true;
		}
		else {
			return false;
		}
	}
		break;
	case ArgNameType::QasGtQbs: {
		std::string code = sym->ShortCode + (_T("SHTQ"));
		std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		VtChartData* chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& sellVec = chartData->GetDataArray(_T("close"));
		if (sellVec.size() == 0 || index <= sellVec.size())
			return false;

		code = sym->ShortCode + (_T("BHTQ"));
		dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& buyVec = chartData->GetDataArray(_T("close"));
		if (buyVec.size() == 0 || index <= buyVec.size())
			return false;

		if (sellVec[index]*param > buyVec[index]) {
			return true;
		}
		else {
			return false;
		}
	}
		break;
	case ArgNameType::UbsGtUas: {
		std::string code = sym->ShortCode + (_T("SHTQ"));
		std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		VtChartData* chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& sellVec = chartData->GetDataArray(_T("close"));
		if (sellVec.size() == 0 || index <= sellVec.size())
			return false;

		code = sym->ShortCode + (_T("BHTQ"));
		dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& buyVec = chartData->GetDataArray(_T("close"));
		if (buyVec.size() == 0 || index <= buyVec.size())
			return false;

		if (buyVec[index] *param > sellVec[index]) {
			return true;
		}
		else {
			return false;
		}
	}
		break;
	case ArgNameType::UasGtUbs: {
		std::string code = sym->ShortCode + (_T("SHTQ"));
		std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		VtChartData* chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& sellVec = chartData->GetDataArray(_T("close"));
		if (sellVec.size() == 0 || index <= sellVec.size())
			return false;

		code = sym->ShortCode + (_T("BHTQ"));
		dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& buyVec = chartData->GetDataArray(_T("close"));
		if (buyVec.size() == 0 || index <= buyVec.size())
			return false;

		if (sellVec[index]*param > buyVec[index]) {
			return true;
		}
		else {
			return false;
		}
	}
		break;
	case ArgNameType::KbsGtKas: {
		std::string code = sym->ShortCode + (_T("SHTQ"));
		std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		VtChartData* chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& sellVec = chartData->GetDataArray(_T("close"));
		if (sellVec.size() == 0 || index <= sellVec.size())
			return false;

		code = sym->ShortCode + (_T("BHTQ"));
		dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& buyVec = chartData->GetDataArray(_T("close"));
		if (buyVec.size() == 0 || index <= buyVec.size())
			return false;

		if (buyVec[index]*param > sellVec[index]) {
			return true;
		}
		else {
			return false;
		}
	}
		break;
	case ArgNameType::KasGtKbs: {
		std::string code = sym->ShortCode + (_T("SHTQ"));
		std::string dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		VtChartData* chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& sellVec = chartData->GetDataArray(_T("close"));
		if (sellVec.size() == 0 || index <= sellVec.size())
			return false;

		code = sym->ShortCode + (_T("BHTQ"));
		dataKey = VtChartDataManager::MakeChartDataKey(code, VtChartType::MIN, _Cycle);
		chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;
		std::vector<double>& buyVec = chartData->GetDataArray(_T("close"));
		if (buyVec.size() == 0 || index <= buyVec.size())
			return false;

		if (sellVec[index]*param > buyVec[index]) {
			return true;
		}
		else {
			return false;
		}
	}
		break;
	default:
		break;
	}

	return false;
}

bool VtSystem::CheckCondition(std::string argGrpName)
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	std::vector<bool> argCond;
	VtSystemArgGroup* argGrp = GetArgGroup(argGrpName);
	auto conds = _CondGroupMap[argGrpName];

	if (argGrp && conds) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				ArgDataSource dataSrc = (*conds)[arg.Name];
				VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(dataSrc.SymbolCode);
				if (!sym)
					continue;
				double param = std::stod(arg.sValue);
				ArgNameType argName = _ArgTypeMap[arg.Name];
				argCond.push_back(CheckByArg(argName, sym, param));
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

bool VtSystem::CheckCondition(std::string argGrpName, int index)
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	std::vector<bool> argCond;
	VtSystemArgGroup* argGrp = GetArgGroup(argGrpName);
	auto conds = _CondGroupMap[argGrpName];

	if (argGrp && conds) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (auto it = argVec.begin(); it != argVec.end(); ++it) {
			VtSystemArg& arg = *it;
			if (arg.Enable) {
				ArgDataSource dataSrc = (*conds)[arg.Name];
				VtSymbol* sym = prdtCatMgr->GetRecentFutureSymbol(dataSrc.SymbolCode);
				if (!sym)
					continue;
				double param = std::stod(arg.sValue);
				ArgNameType argName = _ArgTypeMap[arg.Name];
				argCond.push_back(CheckByArg(argName, sym, param, index));
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

bool VtSystem::CheckEntranceForBuy()
{
	return false;
}

bool VtSystem::CheckEntranceForBuy(size_t index)
{
	return false;
}

bool VtSystem::CheckEntranceForSell()
{
	return false;
}

bool VtSystem::CheckEntranceForSell(size_t index)
{
	return false;
}

bool VtSystem::CheckLiqForSell()
{
	return false;
}

bool VtSystem::CheckLiqForSell(size_t index)
{
	return false;
}

bool VtSystem::CheckLiqForBuy()
{
	return false;
}

bool VtSystem::CheckLiqForBuy(size_t index)
{
	return false;
}

bool VtSystem::PutEntranceOrder(VtPositionType position)
{
	int orderPrice = GetOrderPrice(position);
	if (orderPrice < 0) return false;

	if (_Symbol)
		LOG_F(INFO, _T("PutEntranceOrder :: �ý��� �ֹ� : ���簡 =%d, �ֹ��� = %d"), _Symbol->Quote.intClose, orderPrice);
	try
	{
		PutOrder(orderPrice, position, _PriceType);
		_LatestEntPrice = _Symbol->Quote.intClose;
	}
	catch (std::exception& e)
	{
		LOG_F(INFO, _T("Error :: PutEntranceOrder msg = %s"), e.what());
		return false;
	}
	return true;
}

bool VtSystem::PutLiqOrder(VtPosition* posi)
{
	// �ý��� ��ü û��� Ÿ�̸ӿ� ���� û���� ��ĥ���� �����ϱ� ���� �ڵ�
	std::unique_lock<std::mutex> lck(_LiqMutex);

	if (!posi || _CurPosition == VtPositionType::None)
		return false;

	VtPositionType position = VtPositionType::None;
	_CurPosition == VtPositionType::Buy ? position = VtPositionType::Sell : position = VtPositionType::Buy;
	int orderPrice = GetOrderPrice(position);
	if (orderPrice < 0)
		return false;

	LOG_F(INFO, _T("û�� �ֹ� : ���� %s, �ܰ� = %d"), posi->AccountNo.c_str(), posi->OpenQty);
	VtAccount* acnt = nullptr;
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	acnt = acntMgr->FindAccount(posi->AccountNo);
	if (!acnt) { // �����¿� ���� ��� ���� ���¸� ã�� ����.
		VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
		acnt = subAcntMgr->FindAccount(posi->AccountNo);
	}

	if (!acnt) // ���°� ���� ��� �ƹ��ϵ� ���� ����.
		return false;

	try
	{
		VtOrderManagerSelector* orderMgrSelector = VtOrderManagerSelector::GetInstance();
		VtOrderManager* orderMgr = orderMgrSelector->FindAddOrderManager(acnt->AccountNo);

		HdOrderRequest request;
		request.Amount = std::abs(posi->OpenQty);
		if (acnt->AccountLevel() == 0) { // ������ �� ��
			request.AccountNo = acnt->AccountNo;
			request.Password = acnt->Password;
		}
		else { // ���� ���� �� ��
			VtAccount* parentAcnt = acnt->ParentAccount();
			if (parentAcnt) { // �θ� ���°� �ִ��� Ȯ�� �Ѵ�.
				request.AccountNo = parentAcnt->AccountNo;
				request.Password = parentAcnt->Password;
			}
			else return false;
		}
		request.SymbolCode = _Symbol->ShortCode;
		request.FillCondition = _FillCondition;

		request.PriceType = _PriceType;
		request.Price = orderPrice;

		if (posi->Position == VtPositionType::Buy) { // �ż� û���� ���
			request.Position = VtPositionType::Sell;
		}
		else if (posi->Position == VtPositionType::Sell) { // �ŵ� û���� ���
			request.Position = VtPositionType::Buy;
		}

		request.RequestId = orderMgr->GetOrderRequestID();
		request.SourceId = (long)this;

		if (acnt->AccountLevel() == 0) { // ������ �� ���
			request.SubAccountNo = _T("");
			request.FundName = _T("");
		}
		else { // ���� ������ ���
			request.SubAccountNo = acnt->AccountNo;
			if (acnt->Fund())
				request.FundName = acnt->Fund()->Name;
			else
				request.FundName = _T("");
		}

		request.orderType = VtOrderType::New;
		// ���� �ֹ��̳� û��
		// 0 : �Ϲ� , 1 : ����, 2 : ����- û�� �ֹ��� ���� ���� ������ Ȱ��ȭ �Ǿ� �־ ó������ �ʴ´�.
		request.RequestType = 1;

		orderMgr->PutOrder(std::move(request));
	}
	catch (std::exception& e)
	{
		LOG_F(INFO, _T("Error :: PutLiqOrder msg = %s"), e.what());
		return false;
	}
	return true;
}

void VtSystem::RegisterLiqToScheduler()
{
	std::string liqTime = GetLiqTime();

	VtScheduler* sch = VtScheduler::GetInstance();
	sch->RegisterSystemLiq(this, liqTime);
}

std::string VtSystem::GetLiqTime()
{
	struct tm t = VtTimeUtil::Now();
	t.tm_hour = _LiqTime.hour;
	t.tm_min = _LiqTime.min;
	t.tm_sec = _LiqTime.sec;

	std::stringstream buffer;

	buffer << std::put_time(&t, "%H:%M:%S");

	std::string liqTime = buffer.str();

	return liqTime;
}

int VtSystem::GetDailyIndexByChart()
{
	if (!_Symbol)
		return -1;

	std::string dataKey = VtChartDataManager::MakeChartDataKey(_Symbol->ShortCode, VtChartType::MIN, _Cycle);
	VtChartData* chartData = _RefDataMap[dataKey];
	if (!chartData)
		return -1;
	std::vector<double>& dateArray = chartData->GetDataArray(_T("date"));
	if (dateArray.size() == 0 || dateArray.size() == 1)
		return -1;
	int localDate = VtChartDataCollector::GetLocalDate();
	if (dateArray[dateArray.size() - 1] < localDate)
		return -1;

	int dateIndex = 0;
	for (size_t i = dateArray.size() - 1; i > 0; --i) {
		double pre = dateArray[i - 1];
		double cur = dateArray[i];
		if (pre != cur)
			break;
		dateIndex++;
	}

	return dateIndex;
}

int VtSystem::GetOrderPrice(VtPositionType position)
{
	int price = -1;
	if (!_Symbol || position == VtPositionType::None)
		return price;
	if (_PriceType == VtPriceType::Market)
		return price = 0;
	else if (_PriceType == VtPriceType::Price) {
		price = _Symbol->Quote.intClose;
		if (position == VtPositionType::Buy || position == VtPositionType::ExitSell) {
			// �ż��� ��� ��ǥ���ݺ��� ������ ƽ��ŭ ���� �ֹ��� ����.
			price = price + _Symbol->intTickSize * _OrderTick;
		}
		else if (position == VtPositionType::Sell || position == VtPositionType::ExitBuy) {
			// �ŵ��� ��� ��ǥ���ݺ��� ������ ƽ��ŭ �Ʒ��� �ֹ��� ����.
			price = price - _Symbol->intTickSize * _OrderTick;
		}
	}

	return price;
}

std::pair<int, int> VtSystem::GetOrderPrice()
{
	std::pair<int, int> result;
	if (!_Symbol) {
		return std::make_pair(0, 0);
	}

	return std::make_pair(_Symbol->Quote.intClose, 0);
}

void VtSystem::RegisterRealtimeSymbolEvent()
{
	VtRealtimeRegisterManager* realtimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	if (_Symbol) {
		realtimeRegiMgr->RegisterProduct(_Symbol->ShortCode);
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

	VtOrderManagerSelector* orderMgrSelector = VtOrderManagerSelector::GetInstance();
	if (_SysTargetType == TargetType::RealAccount || _SysTargetType == TargetType::SubAccount) { // �ǰ��³� ���� ���� �� ��
		if (_Account) {
			LOG_F(INFO, _T("�ý��� ���� �ֹ� : ���� %s, �ֹ����� = %d"), _Account->AccountNo.c_str(), _OrderAmount * _SeungSu);
			VtOrderManager* orderMgr = orderMgrSelector->FindAddOrderManager(_Account->AccountNo);
			HdOrderRequest request;
			request.Price = price;
			request.Position = position;
			request.Amount = _OrderAmount * _SeungSu;
			if (_Account->AccountLevel() == 0) { // �ǰ��� �� ��
				request.AccountNo = _Account->AccountNo;
				request.Password = _Account->Password;
			} else { // ���� ���� �� ��
				VtAccount* parentAcnt = _Account->ParentAccount();
				if (parentAcnt) { // �������� ������ �־� �ش�.
					request.AccountNo = parentAcnt->AccountNo;
					request.Password = parentAcnt->Password;
				}
			}
			request.SymbolCode = _Symbol->ShortCode;
			request.FillCondition = _FillCondition;
			request.PriceType = _PriceType;

			request.RequestId = orderMgr->GetOrderRequestID();
			request.SourceId = (long)this;
			if (_Account->AccountLevel() == 0) { // �ǰ����� ��
				request.SubAccountNo = _T("");
				request.FundName = _T("");
			} else { // ���� ���� �� ��
				request.SubAccountNo = _Account->AccountNo;
				request.FundName = _T("");
			}

			request.AccountLevel = _Account->AccountLevel();
			request.orderType = VtOrderType::New;
			orderMgr->PutOrder(std::move(request));
		}
	}
	else { // �ݵ� �ֹ� �� ��
		if (_Fund) {
			std::vector<VtAccount*>& fundAcntList = _Fund->GetFundAccountVector();
			for (auto it = fundAcntList.begin(); it != fundAcntList.end(); ++it) { // �ݵ� ���¸� ���鼭 �ֹ��� ���ش�.
				VtAccount* subAcnt = *it;
				VtAccount* parentAcnt = subAcnt->ParentAccount();
				VtOrderManager* orderMgr = orderMgrSelector->FindAddOrderManager(subAcnt->AccountNo);
				LOG_F(INFO, _T("�ý��� �ݵ� �ֹ� : �ݵ� =%s, ���� %s, �ֹ����� = %d"), _Fund->Name.c_str(), subAcnt->AccountNo.c_str(), _OrderAmount * _SeungSu);

				HdOrderRequest request;
				request.Price = price;
				request.Position = position;
				request.Amount = _OrderAmount * subAcnt->SeungSu * _SeungSu;
				if (parentAcnt) { // �θ� ������ ������ �־� �ش�.
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

	LOG_F(INFO, _T("û�� �ֹ� : ���� %s, �ܰ� = %d"), posi->AccountNo.c_str() ,posi->OpenQty);
	VtAccount* acnt = nullptr;
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	acnt = acntMgr->FindAccount(posi->AccountNo);
	if (!acnt) { // �����¿� ���� ��� ���� ���¸� ã�� ����.
		VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
		acnt = subAcntMgr->FindAccount(posi->AccountNo);
	}

	if (!acnt) // ���°� ���� ��� �ƹ��ϵ� ���� ����.
		return;
	VtOrderManagerSelector* orderMgrSelector = VtOrderManagerSelector::GetInstance();
	VtOrderManager* orderMgr = orderMgrSelector->FindAddOrderManager(acnt->AccountNo);

	HdOrderRequest request;
	request.Amount = std::abs(posi->OpenQty);
	if (acnt->AccountLevel() == 0) { // ������ �� ��
		request.AccountNo = acnt->AccountNo;
		request.Password = acnt->Password;
	}
	else { // ���� ���� �� ��
		VtAccount* parentAcnt = acnt->ParentAccount();
		if (parentAcnt) { // �θ� ���°� �ִ��� Ȯ�� �Ѵ�.
			request.AccountNo = parentAcnt->AccountNo;
			request.Password = parentAcnt->Password;
		}
		else
			return;
	}
	request.SymbolCode = _Symbol->ShortCode;
	request.FillCondition = _FillCondition;

	if (liqud) { // ���尡 û���� ���
		request.Price = 0;
		request.PriceType = VtPriceType::Market;
	}
	else { // ������ û���� ���
		request.PriceType = VtPriceType::Price;
		request.Price = price;
	}

	if (posi->Position == VtPositionType::Buy) { // �ż� û���� ���
		request.Position = VtPositionType::Sell;
	}
	else if (posi->Position == VtPositionType::Sell) { // �ŵ� û���� ���
		request.Position = VtPositionType::Buy;
	}

	request.RequestId = orderMgr->GetOrderRequestID();
	request.SourceId = (long)this;

	if (acnt->AccountLevel() == 0) { // ������ �� ���
		request.SubAccountNo = _T("");
		request.FundName = _T("");
	}
	else { // ���� ������ ���
		request.SubAccountNo = acnt->AccountNo;
		if (acnt->Fund())
			request.FundName = acnt->Fund()->Name;
		else
			request.FundName = _T("");
	}

	request.orderType = VtOrderType::New;
	// ���� �ֹ��̳� û��
	// 0 : �Ϲ� , 1 : ����, 2 : ����- û�� �ֹ��� ���� ���� ������ Ȱ��ȭ �Ǿ� �־ ó������ �ʴ´�.
	request.RequestType = 1;

	orderMgr->PutOrder(std::move(request));
}

bool VtSystem::LiqudAll()
{
	if (!_Symbol)
		return false;

	if (_SysTargetType == TargetType::RealAccount || _SysTargetType == TargetType::SubAccount) { // ���� �ֹ��� ��
		if (!_Account)
			return false;
		VtPosition* posi = _Account->FindPosition(_Symbol->ShortCode);

		if (!posi || posi->OpenQty == 0 || posi->Position == VtPositionType::None)
			return false;

		PutLiqOrder(posi);

		_ProfitLoss = 0.0;
		_MaxProfit = 0.0;

		return true;
	} else {
		if (!_Fund)
			return false;
		std::vector<VtAccount*>& fundAcntList = _Fund->GetFundAccountVector();
		for (auto it = fundAcntList.begin(); it != fundAcntList.end(); ++it) { // ���� ���� ����� ���鼭 �� ��������� �ܰ� û���� �ش�.
			VtAccount* subAcnt = *it;
			VtPosition* posi = subAcnt->FindPosition(_Symbol->ShortCode);

			if (!posi || posi->OpenQty == 0 || posi->Position == VtPositionType::None)
				continue;
			PutLiqOrder(posi);
		}

		_ProfitLoss = 0.0;
		_MaxProfit = 0.0;
		return true;
	}
}

void VtSystem::Symbol(VtSymbol* val)
{
	if (!val)
		return;

	_Symbol = val;
	_SymbolCode = _Symbol->ShortCode;
	SetDataSrc();
}

void VtSystem::SetDataSrc()
{
	if (!_Symbol)
		return;

	// �Ϻ� ������ �߰�
	VtChartData* data = AddDataSource(_Symbol->ShortCode, VtChartType::DAY, 1);
	if (!data)
		return;

	data->RequestChartData();
	data = AddDataSource(_Symbol->ShortCode, VtChartType::MIN, _Cycle);
	data->RequestChartData();
	_DataSrcSymbolVec.push_back(_Symbol->ShortCode);
	VtRealtimeRegisterManager* realRegiMgr = VtRealtimeRegisterManager::GetInstance();
	realRegiMgr->RegisterProduct(_Symbol->ShortCode);
}

void VtSystem::SetDataSrc(VtSymbol* sym, VtChartType type, int cycle)
{
	VtRealtimeRegisterManager* realRegiMgr = VtRealtimeRegisterManager::GetInstance();
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

void VtSystem::Save(simple::file_ostream<same_endian_type>& ss)
{
	ss << _Name; // �ý��� �̸�
	ss << (int)_SysTargetType; // �ý��� Ÿ�� : �ǰ���, �������, �ݵ�
	ss << _SysTargetName; // �ý��� ��� �̸� : ���´� ���¹�ȣ, �ݵ�� �ݵ��
	ss << _EntranceStartTime.hour; // �ý��� ���� ���۽ð�
	ss << _EntranceStartTime.min;
	ss << _EntranceStartTime.sec;
	ss << _EntranceEndTime.hour; // �ý��� ���� ����ð�
	ss << _EntranceEndTime.min;
	ss << _EntranceEndTime.sec;
	ss << _LiqTime.hour; // �ý��� û�� �ð�
	ss << _LiqTime.min;
	ss << _LiqTime.sec;
	ss << _EnableTrailStop; // Ʈ���� ��ž ����
	ss << _EnableLossCut; // �ν��� ����
	ss << _EnableTargetCut; // ��ǥ�� ����
	ss << _TrailStop.MinProfit; // Ʈ���� ��ž �ּ� ����
	ss << _TrailStop.TrailingPercent; // Ʈ���� ��ž �ۼ�Ʈ
	ss << _LossCut; // �ν��� ���� ��
	ss << _TargetProfit; // ��ǥ�� ������
	ss << (int)_LossCutType; // �ν��� Ÿ��
	ss << (int)_TargetProfitType; // ��ǥ�� Ÿ��
	ss << _MaxEntrance; // �Ϸ� �ִ� ���� ȸ��
	ss << (int)_LiqPriceType; // û�� ���� Ÿ��
	ss << _OrderAmount; // ���� �ֹ� ����
	ss << (int)_FillCondition; // ���� �ֹ� ü�� ����
	ss << (int)_PriceType; // ���� ���� Ÿ��
	ss << _SymbolCode; // �ý��� ��� ���� �ڵ�
	ss << _Cycle; // �ý��� ���� �ֱ�
	ss << _EntryBarIndex;
	ss << _ATRTime.hour << _ATRTime.min << _ATRTime.sec;
	ss << _ATR;
	ss << _ATRMulti;
	ss << _BandMulti;
	ss << _FilterMulti;
	ss << _OutSignalName;
	ss << _OrderTick; // ������ �ֹ� ƽ��

	// �ý��� �Ű������� �׷캰�� �����Ѵ�.
	ss << _ArgGroupMap.size();
	for (auto it = _ArgGroupMap.begin(); it != _ArgGroupMap.end(); ++it) {
		VtSystemArgGroup& grp = *it;
		grp.Save(ss);
	}
}

void VtSystem::Load(simple::file_istream<same_endian_type>& ss)
{
	ss >> _Name; // �ý��� �̸�
	ss >> _SysTargetType; // �ý��� Ÿ�� : �ǰ���, �������, �ݵ�
	ss >> _SysTargetName; // �ý��� ��� �̸� : ���´� ���¹�ȣ, �ݵ�� �ݵ��
	ss >> _EntranceStartTime.hour; // �ý��� ���� ���۽ð�
	ss >> _EntranceStartTime.min;
	ss >> _EntranceStartTime.sec;
	ss >> _EntranceEndTime.hour; // �ý��� ���� ����ð�
	ss >> _EntranceEndTime.min;
	ss >> _EntranceEndTime.sec;
	ss >> _LiqTime.hour; // �ý��� û�� �ð�
	ss >> _LiqTime.min;
	ss >> _LiqTime.sec;
	ss >> _EnableTrailStop; // Ʈ���� ��ž ����
	ss >> _EnableLossCut; // �ν��� ����
	ss >> _EnableTargetCut; // ��ǥ�� ����
	ss >> _TrailStop.MinProfit; // Ʈ���� ��ž �ּ� ����
	ss >> _TrailStop.TrailingPercent; // Ʈ���� ��ž �ۼ�Ʈ
	ss >> _LossCut; // �ν��� ���� ��
	ss >> _TargetProfit; // ��ǥ�� ������
	ss >> _LossCutType; // �ν��� Ÿ��
	ss >> _TargetProfitType; // ��ǥ�� Ÿ��
	ss >> _MaxEntrance; // �Ϸ� �ִ� ���� ȸ��
	ss >> _LiqPriceType; // û�� ���� Ÿ��
	ss >> _OrderAmount; // ���� �ֹ� ����
	ss >> _FillCondition; // ���� �ֹ� ü�� ����
	ss >> _PriceType; // ���� ���� Ÿ��
	ss >> _SymbolCode; // �ý��� ��� ���� �ڵ�
	ss >> _Cycle; // �ý��� ���� �ֱ�
	ss >> _EntryBarIndex;
	ss >> _ATRTime.hour >> _ATRTime.min >> _ATRTime.sec;
	ss >> _ATR;
	ss >> _ATRMulti;
	ss >> _BandMulti;
	ss >> _FilterMulti;
	ss >> _OutSignalName;
	ss >> _OrderTick; 

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

	VtOutSignalDefManager* outSigMgr = VtOutSignalDefManager::GetInstance();
	SharedOutSigDef outSig = outSigMgr->FindOutSigDef(_OutSignalName);
	if (outSig) _OutSignal = outSig;

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	_Symbol = symMgr->FindSymbol(_SymbolCode);

	int argGrpCount = 0;
	ss >> argGrpCount;
	if (argGrpCount == 0)
		return;
	// ������ �Ű����� �׷�� �������� ��� ������ �ش�.
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
	VtTime time = VtGlobal::GetLocalTime();
	if (!(time.hour >= _ATRTime.hour && time.min >= _ATRTime.min))
		return false;
	int curDailyIndex = GetDailyIndex();
	if (_Symbol && _LastEntryDailyIndex >= 0 && curDailyIndex > _LastEntryDailyIndex) {

		// ���� ������ �ð������� �����´�.
		std::string dataKey = VtChartDataManager::MakeChartDataKey(_Symbol->ShortCode, VtChartType::MIN, _Cycle);
		VtChartData* chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;

		std::vector<double>& closeArray = chartData->GetDataArray(_T("close"));
		std::vector<double>& highArray = chartData->GetDataArray(_T("high"));
		std::vector<double>& lowArray = chartData->GetDataArray(_T("low"));

		int dailyStartIndex = closeArray.size() - 1 - curDailyIndex;
		int startIndex = dailyStartIndex + _LastEntryDailyIndex + 1;
		int endIndex = closeArray.size() - 1;
		double maxClose = closeArray[startIndex];
		for (int i = startIndex; i <= endIndex; ++i) {
			if (closeArray[i] > maxClose) {
				maxClose = closeArray[i];
			}
		}

		double atr = GetAtr(closeArray.size() - 1, _ATR, highArray.data(), lowArray.data(), closeArray.data());
		LOG_F(INFO, _T("CheckAtrLiqForBuy : Code = %s, close = %.2f, maxClose = %.2f, atr = %.2f, _ATRMulti = %.2f"), _Symbol->ShortCode, closeArray.back(), maxClose, atr, _ATRMulti);
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

bool VtSystem::CheckAtrLiqForBuy(int index)
{
	// ���� ������ �ð������� �����´�.
	std::string dataKey = VtChartDataManager::MakeChartDataKey(_Symbol->ShortCode, VtChartType::MIN, _Cycle);
	VtChartData* chartData = _RefDataMap[dataKey];
	if (!chartData)
		return false;

	std::vector<double>& timeArray = chartData->GetDataArray(_T("time"));
	VtTime time = VtGlobal::GetTime((int)timeArray[index]);
	if (!(time.hour >= _ATRTime.hour && time.min >= _ATRTime.min))
		return false;
	int curDailyIndex = GetDailyIndex(index);
	if (_Symbol && _LastEntryDailyIndex >= 0 && curDailyIndex > _LastEntryDailyIndex) {
		std::vector<double>& closeArray = chartData->GetDataArray(_T("close"));
		std::vector<double>& highArray = chartData->GetDataArray(_T("high"));
		std::vector<double>& lowArray = chartData->GetDataArray(_T("low"));

		int dailyStartIndex = closeArray.size() - 1 - curDailyIndex;
		int startIndex = dailyStartIndex + _LastEntryDailyIndex + 1;
		int endIndex = index;
		double maxClose = closeArray[startIndex];
		for (int i = startIndex; i <= endIndex; ++i) {
			if (closeArray[i] > maxClose) {
				maxClose = closeArray[i];
			}
		}
		double atr = GetAtr(index, _ATR, highArray.data(), lowArray.data(), closeArray.data());
		if (closeArray[index] < maxClose - atr * _ATRMulti) {
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
	VtTime time = VtGlobal::GetLocalTime();
	if (!(time.hour >= _ATRTime.hour && time.min >= _ATRTime.min))
		return false;
	int curDailyIndex = GetDailyIndex();
	if (_Symbol && _LastEntryDailyIndex != 0 && curDailyIndex > _LastEntryDailyIndex) {

		// ���� ������ �ð������� �����´�.
		std::string dataKey = VtChartDataManager::MakeChartDataKey(_Symbol->ShortCode, VtChartType::MIN, _Cycle);
		VtChartData* chartData = _RefDataMap[dataKey];
		if (!chartData)
			return false;

		std::vector<double>& closeArray = chartData->GetDataArray(_T("close"));
		std::vector<double>& highArray = chartData->GetDataArray(_T("high"));
		std::vector<double>& lowArray = chartData->GetDataArray(_T("low"));

		int dailyStartIndex = closeArray.size() - 1 - curDailyIndex;
		int startIndex = dailyStartIndex + _LastEntryDailyIndex + 1;
		int endIndex = closeArray.size() - 1;
		double minClose = closeArray[startIndex];
		for (int i = startIndex; i <= endIndex; ++i) {
			if (closeArray[i] < minClose) {
				minClose = closeArray[i];
			}
		}

		double atr = GetAtr(closeArray.size() - 1, _ATR, highArray.data(), lowArray.data(), closeArray.data());
		LOG_F(INFO, _T("CheckAtrLiqForBuy : Code = %s, close = %.2f, minClose = %.2f, atr = %.2f, _ATRMulti = %.2f"), _Symbol->ShortCode, closeArray.back(), minClose, atr, _ATRMulti);
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

bool VtSystem::CheckAtrLiqForSell(int index)
{
	// ���� ������ �ð������� �����´�.
	std::string dataKey = VtChartDataManager::MakeChartDataKey(_Symbol->ShortCode, VtChartType::MIN, _Cycle);
	VtChartData* chartData = _RefDataMap[dataKey];
	if (!chartData)
		return false;

	std::vector<double>& timeArray = chartData->GetDataArray(_T("time"));
	VtTime time = VtGlobal::GetTime((int)timeArray[index]);
	if (!(time.hour >= _ATRTime.hour && time.min >= _ATRTime.min))
		return false;
	int curDailyIndex = GetDailyIndex(index);
	if (_Symbol && _LastEntryDailyIndex >= 0 && curDailyIndex > _LastEntryDailyIndex) {
		std::vector<double>& closeArray = chartData->GetDataArray(_T("close"));
		std::vector<double>& highArray = chartData->GetDataArray(_T("high"));
		std::vector<double>& lowArray = chartData->GetDataArray(_T("low"));

		int dailyStartIndex = closeArray.size() - 1 - curDailyIndex;
		int startIndex = dailyStartIndex + _LastEntryDailyIndex + 1;
		int endIndex = index;
		double minClose = closeArray[startIndex];
		for (int i = startIndex; i <= endIndex; ++i) {
			if (closeArray[i] < minClose) {
				minClose = closeArray[i];
			}
		}

		double atr = GetAtr(closeArray.size() - 1, _ATR, highArray.data(), lowArray.data(), closeArray.data());
		LOG_F(INFO, _T("CheckAtrLiqForBuy : Code = %s, close = %.2f, minClose = %.2f, atr = %.2f, _ATRMulti = %d"), _Symbol->ShortCode, closeArray.back(), minClose, atr, _ATRMulti);
		if (closeArray[index] > minClose + atr * _ATRMulti) {
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


int VtSystem::GetDailyIndex(int index)
{
	if (!_Symbol)
		return -1;

	std::string dataKey = VtChartDataManager::MakeChartDataKey(_Symbol->ShortCode, VtChartType::MIN, _Cycle);
	VtChartData* chartData = _RefDataMap[dataKey];
	if (!chartData)
		return -1;

	std::vector<double>& dateArray = chartData->GetDataArray(_T("date"));
	if (dateArray.size() == 0 || index < 0 || index >= dateArray.size())
		return -1;
	if (dateArray.size() == 1 || index == 0)
		return 0;
	int dateIndex = 0;
	for (size_t i = index; i > 0; --i) {
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
	time_t now;
	struct tm begin;
	//struct tm end;

	double seconds;

	time(&now);  /* get current time; same as: now = time(NULL)  */

	begin = *localtime(&now);
	//end = *localtime(&now);

	begin.tm_hour = VtGlobal::OpenTime.hour; 
	begin.tm_min = VtGlobal::OpenTime.min; 
	begin.tm_sec = VtGlobal::OpenTime.sec;
	//end.tm_hour = 9;
	//end.tm_min = 5;
	//end.tm_sec = 0;


	//seconds = difftime(mktime(&end), mktime(&begin));
	seconds = difftime(now, mktime(&begin));

	double se = seconds;

	int min = (int)(se / 60);

	int barCount = (int)(min / this->Cycle());
	return barCount;
}

void VtSystem::SystemGroup(VtSystemGroupType val)
{
	_SystemGroup = val;
	InitArgsGroups();
}

void VtSystem::SetPositionState(VtPosition* posi)
{
	if (!posi)
		return;
	CString str;
	if (posi->OpenQty < 0)
		str.Format(_T("�ŵ�:%4d"), std::abs(posi->OpenQty));
	else if (posi->OpenQty > 0)
		str.Format(_T("�ż�:%4d"), std::abs(posi->OpenQty));
	else
		str.Format(_T(""), 0);
	PositionState = str;
}

void VtSystem::InitArgsGroups()
{
	InitArgType();
	if (_SystemGroup == VtSystemGroupType::KOSPI200F) {
		_BuyEntArg[_T("Kbs-Kas")] = ArgDataSource{ _T("101F"), _T("SHTQ"), _T("close"), _T("BHTQ"), _T("close") };
		_BuyEntArg[_T("Kbc>Kac")] = ArgDataSource{ _T("101F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_BuyEntArg[_T("Qbc>Qac")] = ArgDataSource{ _T("106F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_BuyEntArg[_T("Uac>Ubc")] = ArgDataSource{ _T("175F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };

		_SellEntArg[_T("Kas-Kbs")] = ArgDataSource{ _T("101F"), _T("SHTQ"), _T("close"), _T("BHTQ"), _T("close") };
		_SellEntArg[_T("Kac>Kbc")] = ArgDataSource{ _T("101F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_SellEntArg[_T("Qac>Qbc")] = ArgDataSource{ _T("106F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_SellEntArg[_T("Ubc>Uac")] = ArgDataSource{ _T("175F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };

		_BuyLiqArg[_T("Kas-Kbs")] = ArgDataSource{ _T("101F"), _T("SHTQ"), _T("close"), _T("BHTQ"), _T("close") };
		_BuyLiqArg[_T("Kac>Kbc")] = ArgDataSource{ _T("101F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_BuyLiqArg[_T("Qac>Qbc")] = ArgDataSource{ _T("106F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_BuyLiqArg[_T("Ubc>Uac")] = ArgDataSource{ _T("175F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };

		_SellLiqArg[_T("Kbs-Kas")] = ArgDataSource{ _T("101F"), _T("SHTQ"), _T("close"), _T("BHTQ"), _T("close") };
		_SellLiqArg[_T("Kbc>Kac")] = ArgDataSource{ _T("101F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_SellLiqArg[_T("Qbc>Qac")] = ArgDataSource{ _T("106F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_SellLiqArg[_T("Uac>Ubc")] = ArgDataSource{ _T("175F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
	}
	else if (_SystemGroup == VtSystemGroupType::KOSDAQ150F) {
		_BuyEntArg[_T("Qbc>Qac")] = ArgDataSource{ _T("106F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_BuyEntArg[_T("Qbs>Qas")] = ArgDataSource{ _T("106F"), _T("SHTQ"), _T("close"), _T("BHTQ"), _T("close") };
		_BuyEntArg[_T("Kbc>Kac")] = ArgDataSource{ _T("101F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_BuyEntArg[_T("Uac>Ubc")] = ArgDataSource{ _T("175F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };

		_SellEntArg[_T("Qac>Qbc")] = ArgDataSource{ _T("106F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_SellEntArg[_T("Qas>Qbs")] = ArgDataSource{ _T("106F"), _T("SHTQ"), _T("close"), _T("BHTQ"), _T("close") };
		_SellEntArg[_T("Kac>Kbc")] = ArgDataSource{ _T("101F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_SellEntArg[_T("Ubc>Uac")] = ArgDataSource{ _T("175F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };

		_BuyLiqArg[_T("Qac>Qbc")] = ArgDataSource{ _T("106F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_BuyLiqArg[_T("Qas>Qbs")] = ArgDataSource{ _T("106F"), _T("SHTQ"), _T("close"), _T("BHTQ"), _T("close") };
		_BuyLiqArg[_T("Kac>Kbc")] = ArgDataSource{ _T("101F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_BuyLiqArg[_T("Ubc>Uac")] = ArgDataSource{ _T("175F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };

		_SellLiqArg[_T("Qbc>Qac")] = ArgDataSource{ _T("106F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_SellLiqArg[_T("Qbs>Qas")] = ArgDataSource{ _T("106F"), _T("SHTQ"), _T("close"), _T("BHTQ"), _T("close") };
		_SellLiqArg[_T("Kbc>Kac")] = ArgDataSource{ _T("101F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_SellLiqArg[_T("Uac>Ubc")] = ArgDataSource{ _T("175F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
	}
	else if (_SystemGroup == VtSystemGroupType::USDF) {

		_BuyEntArg[_T("Ubc>Uac")] = ArgDataSource{ _T("175F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_BuyEntArg[_T("Ubs>Uas")] = ArgDataSource{ _T("175F"), _T("SHTQ"), _T("close"), _T("BHTQ"), _T("close") };
		_BuyEntArg[_T("Kac>Kbc")] = ArgDataSource{ _T("101F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_BuyEntArg[_T("Kas>Kbs")] = ArgDataSource{ _T("101F"), _T("SHTQ"), _T("close"), _T("BHTQ"), _T("close") };

		_SellEntArg[_T("Uac>Ubc")] = ArgDataSource{ _T("175F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_SellEntArg[_T("Uas>Ubs")] = ArgDataSource{ _T("175F"), _T("SHTQ"), _T("close"), _T("BHTQ"), _T("close") };
		_SellEntArg[_T("Kbc>Kac")] = ArgDataSource{ _T("101F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_SellEntArg[_T("Kbs>Kas")] = ArgDataSource{ _T("101F"), _T("SHTQ"), _T("close"), _T("BHTQ"), _T("close") };

		_BuyLiqArg[_T("Uac>Ubc")] = ArgDataSource{ _T("175F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_BuyLiqArg[_T("Uas>Ubs")] = ArgDataSource{ _T("175F"), _T("SHTQ"), _T("close"), _T("BHTQ"), _T("close") };
		_BuyLiqArg[_T("Kbc>Kac")] = ArgDataSource{ _T("101F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_BuyLiqArg[_T("Kbs>Kas")] = ArgDataSource{ _T("101F"), _T("SHTQ"), _T("close"), _T("BHTQ"), _T("close") };

		_SellLiqArg[_T("Ubc>Uac")] = ArgDataSource{ _T("175F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_SellLiqArg[_T("Ubs>Uas")] = ArgDataSource{ _T("175F"), _T("SHTQ"), _T("close"), _T("BHTQ"), _T("close") };
		_SellLiqArg[_T("Kac>Kbc")] = ArgDataSource{ _T("101F"), _T("SHTC"), _T("close"), _T("BHTC"), _T("close") };
		_SellLiqArg[_T("Kas>Kbs")] = ArgDataSource{ _T("101F"), _T("SHTQ"), _T("close"), _T("BHTQ"), _T("close") };
	}

	_CondGroupMap[_T("�ż�����")] = &_BuyEntArg;
	_CondGroupMap[_T("�ŵ�����")] = &_SellEntArg;
	_CondGroupMap[_T("�ż�û��")] = &_BuyLiqArg;
	_CondGroupMap[_T("�ŵ�û��")] = &_SellLiqArg;
}

void VtSystem::CheckLiqByStop()
{
	if (_LiqByStop) { // �ý��� ��Ȱ��ȭ�� �ܰ� û�� ���� Ȯ��
		if (_SysTargetType == TargetType::RealAccount || _SysTargetType == TargetType::SubAccount) {
			if (!_Account)
				return;

			VtPosition* posi = _Account->FindPosition(_Symbol->ShortCode);
			if (posi && std::abs(posi->OpenQty) > 0) {
				int res = AfxMessageBox(_T("�ܰ� �����ֽ��ϴ�! û���Ͻðڽ��ϱ�?"), MB_YESNO);
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
				int res = AfxMessageBox(_T("�ܰ� �����ֽ��ϴ�! û���Ͻðڽ��ϱ�?"), MB_YESNO);
				if (res == IDYES)
					LiqudAll();
			}
		}
	}
}

bool VtSystem::CheckEntranceBar()
{
	VtTime time = VtGlobal::GetLocalTime();
	if (time.hour == _EntranceStartTime.hour && time.min == _EntranceStartTime.min)
		return false;
	else
		return true;
}

bool VtSystem::GetCondition(std::string argName, std::string par)
{
	double param = std::stod(par);
	ArgNameType arg = _ArgTypeMap[argName];
	return CheckByArg(arg, _Symbol, param, false);
}

void VtSystem::RegisterRealtimeAccountEvent()
{
	VtRealtimeRegisterManager* realtimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	if (_SysTargetType == TargetType::RealAccount || _SysTargetType == TargetType::SubAccount) {
		if (_Account) {
			if (_Account->AccountLevel() == 0) {
				realtimeRegiMgr->RegisterAccount(_Account->AccountNo);
			}
			else {
				VtAccount* parentAcnt = _Account->ParentAccount();
				if (parentAcnt) {
					realtimeRegiMgr->RegisterAccount(parentAcnt->AccountNo);
				}
			}
		}
	}
	else {
		if (_Fund) {
			std::set<VtAccount*> parendAcntSet = _Fund->GetParentAccountSet();
			for (auto it = parendAcntSet.begin(); it != parendAcntSet.end(); ++it) {
				VtAccount* parentAcnt = *it;
				realtimeRegiMgr->RegisterAccount(parentAcnt->AccountNo);
			}
		}
	}
}

void VtSystem::OutSignal(SharedOutSigDef val)
{
	if (!val)
		return;

	_OutSignal = val;
	_OutSignalName = _OutSignal->SignalName;
}

VtPosition VtSystem::GetPosition()
{
	VtPosition curPosi;
	if (_SysTargetType == TargetType::RealAccount || _SysTargetType == TargetType::SubAccount) { // ���� �ֹ��� ��
		if (!_Account || !_Symbol)
			return curPosi;
		VtPosition* posi = _Account->FindPosition(_Symbol->ShortCode);

		if (!posi || posi->OpenQty == 0 || posi->Position == VtPositionType::None)
			return curPosi;
		return *posi;
	}
	else {
		if (!_Fund || !_Symbol)
			return curPosi;
		int count = 0;
		curPosi = _Fund->GetPosition(_Symbol->ShortCode, count);
		return curPosi;
	}
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

void VtSystem::InitArgType()
{
	_ArgTypeMap[_T("Kbs-Kas")] = ArgNameType::KbsMiKas;
	_ArgTypeMap[_T("Kbc>Kac")] = ArgNameType::KbcGtKac;
	_ArgTypeMap[_T("Qbc>Qac")] = ArgNameType::QbcGtQac;
	_ArgTypeMap[_T("Uac>Ubc")] = ArgNameType::UacGtUbc;
	_ArgTypeMap[_T("Kas-Kbs")] = ArgNameType::KasMiKbs;
	_ArgTypeMap[_T("Kac>Kbc")] = ArgNameType::KacGtKbc;
	_ArgTypeMap[_T("Qac>Qbc")] = ArgNameType::QacGtQbc;
	_ArgTypeMap[_T("Ubc>Uac")] = ArgNameType::UbcGtUac;
	_ArgTypeMap[_T("Qbs>Qas")] = ArgNameType::QbsGtQas;
	_ArgTypeMap[_T("Qas>Qbs")] = ArgNameType::QasGtQbs;
	_ArgTypeMap[_T("Ubs>Uas")] = ArgNameType::UbsGtUas;
	_ArgTypeMap[_T("Uas>Ubs")] = ArgNameType::UasGtUbs;
	_ArgTypeMap[_T("Kas>Kbs")] = ArgNameType::KasGtKbs;
	_ArgTypeMap[_T("Kbs>Kas")] = ArgNameType::KbsGtKas;
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
		if (_TargetProfitType == ValueType::Tick) { // ƽ���� û��
			if (VtCutManager::CheckCutLoss(posi, _Symbol, (int)_LossCut) && LiqudAll()) {
				return true;
			}
		}
		else { // �ۼ�Ʈ�� û��
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

		if (_TargetProfitType == ValueType::Tick) { // ƽ���� û��
			if (VtCutManager::CheckCutLoss(&posi, _Symbol, (int)_LossCut * _Fund->GetTotalSeungSu()) && LiqudAll()) {
				return true;
			}
		}
		else { // �ۼ�Ʈ�� û��
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
		if (_TargetProfitType == ValueType::Tick) { // ƽ���� û��
			if (VtCutManager::CheckCutProfit(posi, _Symbol, (int)_TargetProfit) && LiqudAll()) {
				return true;
			}
		}
		else { // �ۼ�Ʈ�� û��
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

		if (_TargetProfitType == ValueType::Tick) { // ƽ���� û��
			if (VtCutManager::CheckCutProfit(&posi, _Symbol, (int)_TargetProfit * _Fund->GetTotalSeungSu()) && LiqudAll()) {
				return true;
			}
		}
		else { // �ۼ�Ʈ�� û��
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

	// �ִ������� Ʈ���� ��ž �ּ����Ϳ� �������� �ʾ����� �ƹ��ϵ� ���� �ʴ´�.
	if (_MaxProfit < _TrailStop.MinProfit)
		return false;

	// �ִ����Ϳ��� �սǰ����� %�� ���Ѵ�. ������� 30%�� �ִ������� 30% �ݾ��� ���Ѵ�.
	double lossProfit = _MaxProfit * _TrailStop.TrailingPercent / 100.0;
	// ���������� �ִ����Ϳ��� �ս� ������ �ݾ��� �� �ͺ��� �۴ٸ� ��� û���� �õ��Ѵ�.
	// �̺κ��� ������ - �߸��ϸ� ������ ���� ����
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
	if (!_Symbol)
		return;

	if (_SysTargetType == TargetType::RealAccount || _SysTargetType == TargetType::SubAccount) {
		if (!_Account)
			return;

		VtPosition* posi = _Account->FindPosition(_Symbol->ShortCode);
		if (posi) {
			_ProfitLoss = posi->OpenProfitLoss;
			SetPositionState(posi);
		}
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
		SetPositionState(&posi);
	}

	// �ִ� ������ �����Ѵ�.
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
	ReadExtraArgs();
	RegisterLiqToScheduler();
}

void VtSystem::SetDataMap(VtChartData* chartData)
{
	DateTime(chartData->DateTime.data());
	Date(chartData->Date.data());
	Time(chartData->Time.data());
}

void VtSystem::ReadExtraArgs()
{
	//InitArgsGroups();
	// �ż� û��
	VtSystemArgGroup* argGrp = GetArgGroup(_T("��Ÿ����"));
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
					_EnableFilterMulti = true;
					_FilterMulti = std::stod(arg.sValue);
				}
				else if (arg.Name.compare(_T("ATR Time")) == 0) {
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
					_EnableBarIndex = true;
				}
				else if (arg.Name.compare(_T("c>o,c<o")) == 0) {
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
					_EnableFilterMulti = false;
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
					_EnableBarIndex = false;
				}
				else if (arg.Name.compare(_T("c>o,c<o")) == 0) {
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

void VtSystem::SetExtraTargetSymbol(std::string symCode)
{
	_DataSrcSymbolVec.push_back(symCode);
}

void VtSystem::Enable(bool val)
{
	if (!val)
		CheckLiqByStop();
	_Enable = val;
}

void VtSystem::Account(VtAccount* val)
{
	if (!val)
		return;

	_Account = val;
	if (_Account->AccountLevel() == 0)
		_SysTargetType = TargetType::RealAccount;
	else
		_SysTargetType = TargetType::SubAccount;
	_SysTargetName = _Account->AccountNo;
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
	pugi::xml_node system_child = node.append_child("system_type");
	system_child.append_child(pugi::node_pcdata).set_value(value.c_str());

	system_child = node.append_child("system_name");
	value = _Name;
	system_child.append_child(pugi::node_pcdata).set_value(value.c_str());

	system_child = node.append_child("system_custom_name");
	value = _CustomName;
	system_child.append_child(pugi::node_pcdata).set_value(value.c_str());

	system_child = node.append_child("system_target_type");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string((int)_SysTargetType).c_str());

	system_child = node.append_child("system_start_hour");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_EntranceStartTime.hour).c_str());

	system_child = node.append_child("system_start_min");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_EntranceStartTime.min).c_str());

	system_child = node.append_child("system_start_sec");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_EntranceStartTime.sec).c_str());

	system_child = node.append_child("system_end_hour");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_EntranceEndTime.hour).c_str());

	system_child = node.append_child("system_end_min");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_EntranceEndTime.min).c_str());

	system_child = node.append_child("system_end_sec");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_EntranceEndTime.sec).c_str());

	system_child = node.append_child("system_liq_hour");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_LiqTime.hour).c_str());

	system_child = node.append_child("system_liq_min");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_LiqTime.min).c_str());

	system_child = node.append_child("system_liq_sec");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_LiqTime.sec).c_str());

	system_child = node.append_child("enable_trail_stop");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_EnableTrailStop).c_str());

	system_child = node.append_child("enable_loss_cut");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_EnableLossCut).c_str());

	system_child = node.append_child("enable_target_cut");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_EnableTargetCut).c_str());

	system_child = node.append_child("loss_cut_type");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string((int)_LossCutType).c_str());

	system_child = node.append_child("target_profit_type");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string((int)_TargetProfitType).c_str());

	system_child = node.append_child("trail_stop_min_profit");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_TrailStop.MinProfit).c_str());

	system_child = node.append_child("trail_stop_trail_percent");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_TrailStop.TrailingPercent).c_str());

	system_child = node.append_child("loss_cut_value");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_LossCut).c_str());

	system_child = node.append_child("target_profit");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_TargetProfit).c_str());
	
	system_child = node.append_child("max_entrance");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_MaxEntrance).c_str());
	
	system_child = node.append_child("liq_price_type");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string((int)(_LiqPriceType)).c_str());

	system_child = node.append_child("order_amount");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_OrderAmount).c_str());

	system_child = node.append_child("filled_condition");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string((int)_FillCondition).c_str());

	system_child = node.append_child("price_type");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string((int)_PriceType).c_str());

	system_child = node.append_child("symbol_code");
	system_child.append_child(pugi::node_pcdata).set_value(_SymbolCode.c_str());

	system_child = node.append_child("system_cycle");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string((int)(_Cycle)).c_str());

	system_child = node.append_child("entrybar_index");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_EntryBarIndex).c_str());

	system_child = node.append_child("atr_time_hour");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_ATRTime.hour).c_str());

	system_child = node.append_child("atr_time_min");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_ATRTime.min).c_str());

	system_child = node.append_child("atr_time_sec");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_ATRTime.sec).c_str());

	system_child = node.append_child("atr");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_ATR).c_str());

	system_child = node.append_child("atr_multi");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_ATRMulti).c_str());

	system_child = node.append_child("band_multi");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_BandMulti).c_str());

	system_child = node.append_child("filter_multi");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_FilterMulti).c_str());

	system_child = node.append_child("out_signal_name");
	system_child.append_child(pugi::node_pcdata).set_value(_OutSignalName.c_str());

	system_child = node.append_child("order_tick");
	system_child.append_child(pugi::node_pcdata).set_value(std::to_string(_OrderTick).c_str());

	if (_ArgGroupMap.size() > 0) {
		system_child = node.append_child("argument_group_list");
		for (auto it = _ArgGroupMap.begin(); it != _ArgGroupMap.end(); ++it) {
			VtSystemArgGroup& grp = *it;
			pugi::xml_node argument_group = system_child.append_child("argument_group");
			grp.SaveToXml(argument_group);
		}
	}
}

void VtSystem::LoadFromXml(pugi::xml_node& node)
{
	std::string system_type = node.child_value("system_type");

	_SystemType = (VtSystemType)std::stoi(system_type);
	_Name = node.child_value("system_name");

	_CustomName = node.child_value("system_custom_name");

}
