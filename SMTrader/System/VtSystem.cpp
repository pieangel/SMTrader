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
		// ���⼭ û���� �����Ѵ�.
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
		// ���⼭ û���� �����Ѵ�.
		if (LiqudAll())
			return true;
		else
			return false;
	}

	return false;
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
	if (_SysTargetType == TargetType::RealAccount || _SysTargetType == TargetType::SubAccount) { // �ǰ��³� ���� ���� �� ��
		if (_Account) {
			VtOrderManager* orderMgr = orderMgrSelector->FindAddOrderManager(_Account->AccountNo);
			HdOrderRequest request;
			request.Price = price;
			request.Position = position;
			request.Amount = _OrderAmount;
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

				HdOrderRequest request;
				request.Price = price;
				request.Position = position;
				request.Amount = _OrderAmount * subAcnt->SeungSu;
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

	LOG_F(INFO, _T("PutOrder : openQty = %d"), posi->OpenQty);
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
		if (posi->Position == VtPositionType::None)
			return false;
		if (_LiqPriceType == VtPriceType::Market) // ���尡
			PutOrder(posi, 0, true);
		else // ������
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
		for (auto it = fundAcntList.begin(); it != fundAcntList.end(); ++it) { // ���� ���� ����� ���鼭 �� ��������� �ܰ� û���� �ش�.
			VtAccount* subAcnt = *it;
			VtPosition* posi = subAcnt->FindPosition(_Symbol->ShortCode);
			if (posi->Position == VtPositionType::None)
				continue;
			if (_LiqPriceType == VtPriceType::Market) // ���尡
				PutOrder(posi, 0, true);
			else // ������
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
	// �Ϻ� ������ �߰�
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

/*
/// <summary>
/// ���� ���� ������ ���� �ε���
/// </summary>
int _EntryBarIndex = 0;
/// <summary>
/// ATR�� û�� ������ �ð�. ATRû���� �� �ð� ���ķ� û�갡���ϴ�.
/// </summary>
VtTime _ATRTime;
/// <summary>
/// ATR û���� �������� ���� : ���϶��� ATR�� û���� �����ϴ�.
/// </summary>
bool _EnableATRLiq = false;
/// <summary>
/// û�꿡 ������ ATR ��
/// </summary>
int _ATR = 0;

double _ATRMulti = 2.0;
double _BandMulti = 0.25;
double _FilterMulti = 3.0;
int _LastEntryTime = 0;
*/

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

	// �ý��� �Ű������� �׷캰�� �����Ѵ�.
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
	if (_Symbol && _LastEntryTime != 0) {

		// ���� ������ �ð������� �����´�.
		std::string dataKey = VtChartDataManager::MakeChartDataKey(_Symbol->ShortCode, VtChartType::MIN, _Cycle);
		std::vector<double>& timeArray = _RefDataMap[dataKey]->GetDataArray(_T("time"));
		std::vector<double>& closeArray = _RefDataMap[dataKey]->GetDataArray(_T("close"));
		std::vector<double>& highArray = _RefDataMap[dataKey]->GetDataArray(_T("high"));
		std::vector<double>& lowArray = _RefDataMap[dataKey]->GetDataArray(_T("low"));

		std::vector<double>::iterator itt = std::find(timeArray.begin(), timeArray.end(), _LastEntryTime);
		// ���� �ֱٿ� ������ ���� ���� ���� �ε����� ã��
		int index = std::distance(timeArray.begin(), itt++);
		// ���� �ֱٿ� ������ ���� ���������� ����������� �� �߿��� �ְ�, �������� ã�´�.
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

		// ���� ������ �ð������� �����´�.
		std::string dataKey = VtChartDataManager::MakeChartDataKey(_Symbol->ShortCode, VtChartType::MIN, _Cycle);
		std::vector<double>& timeArray = _RefDataMap[dataKey]->GetDataArray(_T("time"));
		std::vector<double>& closeArray = _RefDataMap[dataKey]->GetDataArray(_T("close"));
		std::vector<double>& highArray = _RefDataMap[dataKey]->GetDataArray(_T("high"));
		std::vector<double>& lowArray = _RefDataMap[dataKey]->GetDataArray(_T("low"));

		std::vector<double>::iterator itt = std::find(timeArray.begin(), timeArray.end(), _LastEntryTime);
		// ���� �ֱٿ� ������ ���� ���� ���� �ε����� ã��
		int index = std::distance(timeArray.begin(), itt++);
		// ���� �ֱٿ� ������ ���� ���������� ����������� �� �߿��� �ְ�, �������� ã�´�.
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

			// ���� ������ �ð������� �����´�.
			std::string dataKey = VtChartDataManager::MakeChartDataKey(_Symbol->ShortCode, VtChartType::MIN, _Cycle);
			std::vector<double>& timeArray = _RefDataMap[dataKey]->GetDataArray(_T("time"));
			std::vector<double>& closeArray = _RefDataMap[dataKey]->GetDataArray(_T("close"));
			std::vector<double>& highArray = _RefDataMap[dataKey]->GetDataArray(_T("high"));
			std::vector<double>& lowArray = _RefDataMap[dataKey]->GetDataArray(_T("low"));

			std::vector<double>::iterator itt = std::find(timeArray.begin(), timeArray.end(), _LastEntryTime);
			// ���� �ֱٿ� ������ ���� ���� ���� �ε����� ã��
			int index = std::distance(timeArray.begin(), itt++);
			// ���� �ֱٿ� ������ ���� ���������� ����������� �� �߿��� �ְ�, �������� ã�´�.
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

			// ���� ������ �ð������� �����´�.
			std::string dataKey = VtChartDataManager::MakeChartDataKey(_Symbol->ShortCode, VtChartType::MIN, _Cycle);
			std::vector<double>& timeArray = _RefDataMap[dataKey]->GetDataArray(_T("time"));
			std::vector<double>& closeArray = _RefDataMap[dataKey]->GetDataArray(_T("close"));
			std::vector<double>& highArray = _RefDataMap[dataKey]->GetDataArray(_T("high"));
			std::vector<double>& lowArray = _RefDataMap[dataKey]->GetDataArray(_T("low"));

			std::vector<double>::iterator itt = std::find(timeArray.begin(), timeArray.end(), _LastEntryTime);
			// ã�� ���ϸ� ������ ��ȯ�Ѵ�.
			if (itt == std::end(timeArray))
				return false;
			// ���� �ֱٿ� ������ ���� ���� ���� �ε����� ã��
			int index = std::distance(timeArray.begin(), itt++);
			// ���� �ֱٿ� ������ ���� ���������� ����������� �� �߿��� �ְ�, �������� ã�´�.
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
	
}

void VtSystem::SetDataMap(VtChartData* chartData)
{
	DateTime(chartData->DateTime.data());
	Date(chartData->Date.data());
	Time(chartData->Time.data());
}

void VtSystem::ReadExtraArgs()
{

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
	if (!_Enable && _LiqByStop) { // �ý��� ��Ȱ��ȭ�� �ܰ� û�� ���� Ȯ��
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
