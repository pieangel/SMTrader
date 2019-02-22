#pragma once
#include <vector>
#include <map>
#include <string>
#include "../Global/VtDefine.h"
#include "ta_func.h"
#include "../Xml/pugixml.hpp"
#include "../VtSystemArg.h"
#include "../VtSystemArgGroup.h"
#include "../Drawing/VtUniqueID.h"
#include "../SimpleBinStream.h"
#include "../VtSystemDef.h"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;
class VtSignal;
class VtChartData;
class VtFund;
class VtAccount;
class VtSymbol;
struct VtPosition;

class VtUsdStrategyConfigDlg;
class VtSystem : public VtUniqueID
{
public:
	VtSystem(VtSystemType systemType);
	VtSystem(VtSystemType systemType, std::string name) {
		_SystemType = systemType;
		_Name = name;
	}
	VtSystem();
	virtual ~VtSystem();
	
	std::map<std::string, double*> DataMap;
	std::vector<VtSignal*> SignalVector;
	VtSystemType SystemType() const { return _SystemType; }
	void SystemType(VtSystemType val) { _SystemType = val; }
	void AddDataSource(std::string name, double* src);
	void AddDataSource(std::string symCode, std::string dataName, std::vector<double>& data);
	double* DateTime() const { return _DateTime; }
	void DateTime(double* val) { _DateTime = val; }
	double GetMA(int index, double* data, TA_MAType type, int period);
	void GetStochastic(int index, double* high, double* low, double* close, int fastK, int slowK, int slowD, double& outSlowK, double& outSlowD);
	void GetMacd(int index, double* data, int shortPeriod, int longPeriod, int signalPeriod, double& outMacd, double& outMacdSignal, double& outMacdHist);
	void GetHLChannelValue(int index, int period, double* high, double* low, double& highest, double& lowest);
	double GetHighest(int index, int period, double* data);
	double GetLowest(int index, int period, double* data);
	double GetAtr(int index, int period, double* high, double* low, double* close);
	std::string Name() const { return _Name; }
	void Name(std::string val) { _Name = val; }
	void ClearSignal();
	std::string CustomName() const { return _CustomName; }
	void CustomName(std::string val) { _CustomName = val; }
	bool Running() const { return _Running; }
	void Running(bool val) { _Running = val; }
	double GetDailyHigh(int index, double* datetime, double* high, int backLen);
	double GetDailyLow(int index, double* datetime, double* low, int backLen);
	int FindDayStartIndex(int index, double* datetime, int backLen);
	double GetDailyHeight(int index, double* datetime, double* high, double* low, int backLen);
	double GetOpen(int index, double* datetime, VtTime* time, int hour, double* open);

	VtDate* Date() const { return _Date; }
	void Date(VtDate* val) { _Date = val; }
	VtTime* Time() const { return _Time; }
	void Time(VtTime* val) { _Time = val; }
	int EntriesToday(double datetime);
	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
	VtPositionType LastSignalType() const { return _LastSignalType; }
	void LastSignalType(VtPositionType val) { _LastSignalType = val; }
	VtPositionType OldSignal() const { return _OldSignal; }
	void OldSignal(VtPositionType val) { _OldSignal = val; }
	std::vector<VtSystemArgGroup>& GetArgGroupVec()
	{
		return _ArgGroupMap;
	}

	VtSystemArgGroup* GetArgGroup(std::string grpName);

	VtSystemArgGroup* FindArgGroup(std::string grpName);
	virtual void CreateSignal(int startIndex, int endIndex);
	virtual VtPositionType UpdateSignal(int index);
	virtual VtPositionType UpdateSignal(int start, int end);
	virtual void ReloadSystem(int startIndex, int endIndex);
	virtual void InitArgs();
	virtual void SetDataMap(VtChartData* chartData);
	virtual void ReadExtraArgs();
	bool CheckLossCut(int index);
	bool CheckProfitCut(int index);
	bool CheckTrailStop(int index);
	virtual void UpdateSystem(int index);

	bool AppliedData() const { return _AppliedData; }
	void AppliedData(bool val) { _AppliedData = val; }
	void ResetSignal();
	std::string SysTargetName() const { return _SysTargetName; }
	void SysTargetName(std::string val) { _SysTargetName = val; }
	TargetType SysTargetType() const { return _SysTargetType; }
	void SysTargetType(TargetType val) { _SysTargetType = val; }
	double ProfitLoss() const { return _ProfitLoss; }
	void ProfitLoss(double val) { _ProfitLoss = val; }
	bool EnableTrailStop() const { return _EnableTrailStop; }
	void EnableTrailStop(bool val) { _EnableTrailStop = val; }
	bool EnableLossCut() const { return _EnableLossCut; }
	void EnableLossCut(bool val) { _EnableLossCut = val; }
	bool EnableTargetCut() const { return _EnableTargetCut; }
	void EnableTargetCut(bool val) { _EnableTargetCut = val; }
	TrailingStop& TrailStop() { return _TrailStop; }
	double LossCut() const { return _LossCut; }
	void LossCut(double val) { _LossCut = val; }
	ValueType LossCutType() const { return _LossCutType; }
	void LossCutType(ValueType val) { _LossCutType = val; }
	double TargetProfit() const { return _TargetProfit; }
	void TargetProfit(double val) { _TargetProfit = val; }
	ValueType TargetProfitType() const { return _TargetProfitType; }
	void TargetProfitType(ValueType val) { _TargetProfitType = val; }
	int MaxEntrance() const { return _MaxEntrance; }
	void MaxEntrance(int val) { _MaxEntrance = val; }
	//std::string TargetSymbol() const { return _TargetSymbol; }
	//void TargetSymbol(std::string val);
	VtTime& EntranceStartTime() { return _EntranceStartTime; }
	VtTime& EntranceEndTime() { return _EntranceEndTime; }
	VtTime& LiqTime() { return _LiqTime; }
	// �� �ý����� �����µ� �ʿ��� �߰� �����͸� ����� �ش�.
	// ��ϵǴ� ���� ��Ʈ ������ ������ ���۵ȴ�.
	void SetExtraTargetSymbol(std::string symCode);
	bool Enable() const { return _Enable; }
	void Enable(bool val);
	VtAccount* Account() const { return _Account; }
	void Account(VtAccount* val) { _Account = val; }
	VtFund* Fund() const { return _Fund; }
	void Fund(VtFund* val) { _Fund = val; }
	virtual void OnTimer();
	/// <summary>
	/// �ð��� ���� ���� ���� Ȯ��
	/// ������ �ð� ���� ���� ������ ���� ��ȯ, �׷��� ������ ������ ��ȯ�Ѵ�.
	/// </summary>
	/// <returns>���԰����ϸ� ��, ���԰������� ������ ������ ��ȯ</returns>
	bool IsEnterableByTime();
	/// <summary>
	/// �ý��� ���� �ð��� ���� ������ û�� �Լ�
	/// </summary>
	bool LiqByEndTime();
	bool LiqByEndTime(int index);
	bool CheckFilterMulti();
	bool CheckFilterMulti(int index);
	bool CheckBarIndex();
	bool CheckBarIndex(int index);
	bool CheckEntranceByBandForBuy();
	bool CheckEntranceByBandForSell();
	bool CheckEntranceByOpenForBuy();
	bool CheckEntranceByOpenForSell();

	bool CheckEntranceByBandForBuy(size_t index);
	bool CheckEntranceByBandForSell(size_t index);
	bool CheckEntranceByOpenForBuy(size_t index);
	bool CheckEntranceByOpenForSell(size_t index);

	bool CheckByArg(ArgNameType argName, VtSymbol* sym, double param, bool log = true);
	bool CheckByArg(ArgNameType argName, VtSymbol* sym, double param, int index);
	bool CheckCondition(std::string argGrpName);
	bool CheckCondition(std::string argGrpName, int index);
	virtual bool CheckEntranceForBuy();
	virtual bool CheckEntranceForSell();
	virtual bool CheckLiqForSell();
	virtual bool CheckLiqForBuy();

	virtual bool CheckEntranceForBuy(size_t index);
	virtual bool CheckEntranceForSell(size_t index);
	virtual bool CheckLiqForSell(size_t index);
	virtual bool CheckLiqForBuy(size_t index);
	
	int EntryToday() const { return _EntryToday; }
	void EntryToday(int val) { _EntryToday = val; }
	VtSymbol* Symbol() const { return _Symbol; }
	void Symbol(VtSymbol* val);
	virtual void SetDataSrc();
	void SetDataSrc(VtSymbol* sym, VtChartType type, int cycle);
	bool LiqByStop() const { return _LiqByStop; }
	void LiqByStop(bool val) { _LiqByStop = val; }
	int OrderAmount() const { return _OrderAmount; }
	void OrderAmount(int val) { _OrderAmount = val; }
	bool ShowRealtime() const { return _ShowRealtime; }
	void ShowRealtime(bool val) { _ShowRealtime = val; }
	int LatestEntPrice() const { return _LatestEntPrice; }
	void LatestEntPrice(int val) { _LatestEntPrice = val; }
	int Cycle() const { return _Cycle; }
	void Cycle(int val) { _Cycle = val; }
	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
	std::string SymbolCode() const { return _SymbolCode; }
	void SymbolCode(std::string val) { _SymbolCode = val; }
	int EntryBarIndex() const { return _EntryBarIndex; }
	void EntryBarIndex(int val) { _EntryBarIndex = val; }
	VtTime ATRTime() const { return _ATRTime; }
	void ATRTime(VtTime val) { _ATRTime = val; }
	int ATR() const { return _ATR; }
	void ATR(int val) { _ATR = val; }
	bool CheckAtrLiqForBuy();
	bool CheckAtrLiqForSell();
	bool CheckAtrLiqForBuy(int index);
	bool CheckAtrLiqForSell(int index);
	int GetDailyIndex(int index);
	int GetDailyIndex();
	double ATRMulti() const { return _ATRMulti; }
	void ATRMulti(double val) { _ATRMulti = val; }
	double BandMulti() const { return _BandMulti; }
	void BandMulti(double val) { _BandMulti = val; }
	double FilterMulti() const { return _FilterMulti; }
	void FilterMulti(double val) { _FilterMulti = val; }
	int LastEntryTime() const { return _LastEntryTime; }
	void LastEntryTime(int val) { _LastEntryTime = val; }
	bool EnableByOpen() const { return _EnableByOpen; }
	void EnableByOpen(bool val) { _EnableByOpen = val; }
	bool EnableByBand() const { return _EnableByBand; }
	void EnableByBand(bool val) { _EnableByBand = val; }
	VtSystemGroupType SystemGroup() const { return _SystemGroup; }
	void SystemGroup(VtSystemGroupType val);
	CString PositionState;
	void SetPositionState(VtPosition* posi);
	bool EnableBarIndex() const { return _EnableBarIndex; }
	void EnableBarIndex(bool val) { _EnableBarIndex = val; }
	void InitArgsGroups();
	void CheckLiqByStop();
	/// <summary>
	/// ���ۺ��� �̷�������� Ȯ���Ѵ�.
	/// ���ۺ��� �̷�������� ���� �ƴϸ� ������ ��ȯ�Ѵ�.
	/// </summary>
	/// <returns></returns>
	bool CheckEntranceBar();
	/// <summary>
	/// ���� �Ű������� ������ �����ϴ��� �����ش�.
	/// </summary>
	bool GetCondition(std::string argName, std::string param);
protected:
	int FindDateIndex(double date, std::vector<double>& dateArray);
	void PutOrder(int price, VtPositionType position, VtPriceType priceType = VtPriceType::Price);
	void PutOrder(VtPosition* posi, int price, bool liqud = false);
	bool LiqudAll();
	VtPositionType _CurPosition = VtPositionType::None;
	// ���� ���� ������
	VtPositionType _ExpPosition = VtPositionType::None;
	/// <summary>
	/// �ý��� Ȱ��ȭ ����
	/// </summary>
	bool _Enable = false;
	// ������ ��������� �Ǵ� �ɺ� �ڵ��� ����
	// �� �ý����� ������ �� �ʿ��� ������ ������ �ʿ��� �ɺ����� ����
	std::vector<std::string> _DataSrcSymbolVec;
	VtChartData*  AddDataSource(std::string symCode, VtChartType type, int cycle);
	// key : �����̸�, ������ �̸�(��) 101P3000, close, 
	std::map<std::string, VtChartData*> _RefDataMap;
	// �ý��� �Ű������� group ���� �з��Ǹ�, group �ؿ� ���� �Ű��������� �� ���·� ����Ǿ� �ִ�.
	// �ý��� �Ű� ������ �׷��̸��� �Ű������� �ָ� �׷� ������ ���� �ȴ�.
	void AddSystemArg(std::string groupName, VtSystemArg arg);
	void AddSystemArgGroup(std::string groupName, VtSystemArgGroup argGrp);
	std::vector<VtSystemArgGroup> _ArgGroupMap;
	VtDate* _Date = nullptr;
	VtTime* _Time = nullptr;

	double* _DateTime = nullptr;
	int _BarSinceEntry = -1;
	VtPositionType _LastSignalType = VtPositionType::None;
	VtPositionType _OldSignal = VtPositionType::None;
	/// <summary>
	/// ����� ���� �ý��� �̸�
	/// </summary>
	std::string _CustomName;
	bool _Running;

	double _EntryPrice;

	bool _AppliedData;
	/// <summary>
	/// �ǽð� �޽��� �����ֱ� ����
	/// </summary>
	bool _ShowRealtime = false;
	/// <summary>
	/// �ý��� ��� ������ �ְ� ���� ����
	/// </summary>
	double _MaxProfit = 0.0;

	/// <summary>
	/// ���������� ������ ����
	/// </summary>
	int _LatestEntPrice = 0;
	/// <summary>
	///  �ý��� ����
	/// </summary>
	double _ProfitLoss = 0.0;
	/// <summary>
	/// �ý����� ������ �Ŀ� ������ Ƚ��
	/// </summary>
	int _EntryToday = 0;
	/// <summary>
	/// �ý��� ������ �ܰ� û��
	/// </summary>
	bool _LiqByStop = false;

	/// <summary>
	/// �ý��� Ÿ��
	/// </summary>
	VtSystemType _SystemType = VtSystemType::None;
	/// <summary>
	/// �ý��� �̸�
	/// </summary>
	std::string _Name;
	/// <summary>
	/// �ý��� ��� �̸� : ������ ��� ���¹�ȣ, �ݵ��� ��� �ݵ��̸�
	/// </summary>
	std::string _SysTargetName;
	/// <summary>
	/// �ý��� ��� ���� ��ü
	/// </summary>
	VtAccount* _Account = nullptr;
	/// <summary>
	/// �ý��� ��� �ݵ� ��ü
	/// </summary>
	VtFund* _Fund = nullptr;
	/// <summary>
	/// ��� Ÿ��
	/// </summary>
	TargetType _SysTargetType = TargetType::RealAccount;
	/// <summary>
	/// ���� ���� �ð�
	/// </summary>
	VtTime _EntranceStartTime;
	/// <summary>
	/// ���� ���� �ð�
	/// </summary>
	VtTime _EntranceEndTime;
	/// <summary>
	/// û�� �ð�
	/// </summary>
	VtTime _LiqTime;
	/// <summary>
	/// Ʈ���ϸ� ��ž Ȱ��ȭ ����
	/// </summary>
	bool _EnableTrailStop = false;
	/// <summary>
	/// ���� ���� ����
	/// </summary>
	bool _EnableLossCut = false;
	/// <summary>
	/// ��ǥ ���� ����
	/// </summary>
	bool _EnableTargetCut = false;
	/// <summary>
	/// Ʈ���� ��ž
	/// </summary>
	TrailingStop _TrailStop;
	/// <summary>
	/// ���� ��
	/// </summary>
	double _LossCut = 5;
	/// <summary>
	/// ���� Ÿ��
	/// </summary>
	ValueType _LossCutType = ValueType::Tick;
	/// <summary>
	/// ��ǥ����
	/// </summary>
	double _TargetProfit = 5;
	/// <summary>
	/// ��ǥ���� Ÿ��
	/// </summary>
	ValueType _TargetProfitType = ValueType::Tick;
	/// <summary>
	/// �Ϸ� �ִ� ���� Ƚ��
	/// </summary>
	int _MaxEntrance = 0;
	/// <summary>
	/// û�� Ÿ��
	/// </summary>
	VtPriceType _LiqPriceType = VtPriceType::Market;
	/// <summary>
	/// �ֹ� ����
	/// </summary>
	int _OrderAmount = 1;
	/// <summary>
	/// ü�� ����
	/// </summary>
	VtFilledCondition _FillCondition = VtFilledCondition::Fas;
	/// <summary>
	/// ���԰��� Ÿ��
	/// </summary>
	VtPriceType _PriceType = VtPriceType::Market;
	/// <summary>
	/// �ý��� �ֿ� �ɺ�
	/// </summary>
	VtSymbol* _Symbol = nullptr;
	/// <summary>
	/// �ý��� ���� �ֱ� - �д���
	/// </summary>
	int _Cycle = 1;
	/// <summary>
	/// �ɺ� �ڵ�
	/// </summary>
	std::string _SymbolCode = _T("");
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
	bool _ArgsLoaded = false;
	bool _EnableByOpen = false;
	bool _EnableByBand = false;
	/// <summary>
	/// �ż� ���� ���� �Ű����� ������ �ҽ�
	/// </summary>
	std::map<std::string, ArgDataSource> _BuyEntArg;
	/// <summary>
	/// �ŵ� ���� ���� �Ű����� ������ �ҽ�
	/// </summary>
	std::map<std::string, ArgDataSource> _SellEntArg;
	/// <summary>
	/// �ż� û�� ���� �Ű����� ������ �ҽ�
	/// </summary>
	std::map<std::string, ArgDataSource> _BuyLiqArg;
	/// <summary>
	/// �ŵ� û�� ���� �Ű����� ������ �ҽ�
	/// </summary>
	std::map<std::string, ArgDataSource> _SellLiqArg;
	/// <summary>
	/// �ý��� �׷� Ÿ��
	/// </summary>
	VtSystemGroupType _SystemGroup;
	std::map<std::string, ArgNameType> _ArgTypeMap;
	void InitArgType();
	std::map<std::string, std::map<std::string, ArgDataSource>*> _CondGroupMap;
	/// <summary>
	/// ���������� ������ ���ϸ� �ε���
	/// </summary>
	int _LastEntryDailyIndex = -1;
	bool _EnableBarIndex = false;
	bool _EnableFilterMulti = false;
};

