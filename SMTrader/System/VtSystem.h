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
	// 이 시스템을 돌리는데 필요한 추가 데이터를 등록해 준다.
	// 등록되는 순간 차트 데이터 수집이 시작된다.
	void SetExtraTargetSymbol(std::string symCode);
	bool Enable() const { return _Enable; }
	void Enable(bool val);
	VtAccount* Account() const { return _Account; }
	void Account(VtAccount* val) { _Account = val; }
	VtFund* Fund() const { return _Fund; }
	void Fund(VtFund* val) { _Fund = val; }
	virtual void OnTimer();
	/// <summary>
	/// 시간에 따른 진입 여부 확인
	/// 정해진 시간 범위 내에 있으면 참을 반환, 그렇지 않으면 거짓을 반환한다.
	/// </summary>
	/// <returns>진입가능하면 참, 진입가능하지 않으면 거짓을 반환</returns>
	bool IsEnterableByTime();
	/// <summary>
	/// 시스템 종료 시간에 따른 포지션 청산 함수
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
	/// 시작봉이 이루어졌음을 확인한다.
	/// 시작봉이 이루어졌으면 참을 아니면 거짓을 반환한다.
	/// </summary>
	/// <returns></returns>
	bool CheckEntranceBar();
	/// <summary>
	/// 현재 매개변수의 조건을 만족하는지 돌려준다.
	/// </summary>
	bool GetCondition(std::string argName, std::string param);
protected:
	int FindDateIndex(double date, std::vector<double>& dateArray);
	void PutOrder(int price, VtPositionType position, VtPriceType priceType = VtPriceType::Price);
	void PutOrder(VtPosition* posi, int price, bool liqud = false);
	bool LiqudAll();
	VtPositionType _CurPosition = VtPositionType::None;
	// 종가 예상 포지션
	VtPositionType _ExpPosition = VtPositionType::None;
	/// <summary>
	/// 시스템 활성화 여부
	/// </summary>
	bool _Enable = false;
	// 데이터 수집대상이 되는 심볼 코드의 모임
	// 이 시스템을 돌리는 데 필요한 데이터 수집이 필요한 심볼들의 모임
	std::vector<std::string> _DataSrcSymbolVec;
	VtChartData*  AddDataSource(std::string symCode, VtChartType type, int cycle);
	// key : 종목이름, 데이터 이름(예) 101P3000, close, 
	std::map<std::string, VtChartData*> _RefDataMap;
	// 시스템 매개변수는 group 별로 분류되며, group 밑에 실제 매개변수들이 맵 형태로 저장되어 있다.
	// 시스템 매개 변수는 그룹이름과 매개변수를 주면 그룹 밑으로 들어가게 된다.
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
	/// 사용자 정의 시스템 이름
	/// </summary>
	std::string _CustomName;
	bool _Running;

	double _EntryPrice;

	bool _AppliedData;
	/// <summary>
	/// 실시간 메시지 보여주기 여부
	/// </summary>
	bool _ShowRealtime = false;
	/// <summary>
	/// 시스템 운영시 도달한 최고 이익 가격
	/// </summary>
	double _MaxProfit = 0.0;

	/// <summary>
	/// 마지막으로 진입한 가격
	/// </summary>
	int _LatestEntPrice = 0;
	/// <summary>
	///  시스템 손일
	/// </summary>
	double _ProfitLoss = 0.0;
	/// <summary>
	/// 시스템이 동작한 후에 진입한 횟수
	/// </summary>
	int _EntryToday = 0;
	/// <summary>
	/// 시스템 정지시 잔고 청산
	/// </summary>
	bool _LiqByStop = false;

	/// <summary>
	/// 시스템 타입
	/// </summary>
	VtSystemType _SystemType = VtSystemType::None;
	/// <summary>
	/// 시스템 이름
	/// </summary>
	std::string _Name;
	/// <summary>
	/// 시스템 대상 이름 : 계좌일 경우 계좌번호, 펀드일 경우 펀드이름
	/// </summary>
	std::string _SysTargetName;
	/// <summary>
	/// 시스템 대상 계좌 객체
	/// </summary>
	VtAccount* _Account = nullptr;
	/// <summary>
	/// 시스템 대상 펀드 객체
	/// </summary>
	VtFund* _Fund = nullptr;
	/// <summary>
	/// 대상 타입
	/// </summary>
	TargetType _SysTargetType = TargetType::RealAccount;
	/// <summary>
	/// 진입 시작 시간
	/// </summary>
	VtTime _EntranceStartTime;
	/// <summary>
	/// 진입 종료 시간
	/// </summary>
	VtTime _EntranceEndTime;
	/// <summary>
	/// 청산 시간
	/// </summary>
	VtTime _LiqTime;
	/// <summary>
	/// 트레일링 스탑 활성화 여부
	/// </summary>
	bool _EnableTrailStop = false;
	/// <summary>
	/// 손절 설정 여부
	/// </summary>
	bool _EnableLossCut = false;
	/// <summary>
	/// 목표 설정 여부
	/// </summary>
	bool _EnableTargetCut = false;
	/// <summary>
	/// 트레일 스탑
	/// </summary>
	TrailingStop _TrailStop;
	/// <summary>
	/// 손절 값
	/// </summary>
	double _LossCut = 5;
	/// <summary>
	/// 손절 타입
	/// </summary>
	ValueType _LossCutType = ValueType::Tick;
	/// <summary>
	/// 목표이익
	/// </summary>
	double _TargetProfit = 5;
	/// <summary>
	/// 목표이익 타입
	/// </summary>
	ValueType _TargetProfitType = ValueType::Tick;
	/// <summary>
	/// 하루 최대 진입 횟수
	/// </summary>
	int _MaxEntrance = 0;
	/// <summary>
	/// 청산 타입
	/// </summary>
	VtPriceType _LiqPriceType = VtPriceType::Market;
	/// <summary>
	/// 주문 수량
	/// </summary>
	int _OrderAmount = 1;
	/// <summary>
	/// 체결 조건
	/// </summary>
	VtFilledCondition _FillCondition = VtFilledCondition::Fas;
	/// <summary>
	/// 진입가격 타입
	/// </summary>
	VtPriceType _PriceType = VtPriceType::Market;
	/// <summary>
	/// 시스템 주요 심볼
	/// </summary>
	VtSymbol* _Symbol = nullptr;
	/// <summary>
	/// 시스템 수행 주기 - 분단위
	/// </summary>
	int _Cycle = 1;
	/// <summary>
	/// 심볼 코드
	/// </summary>
	std::string _SymbolCode = _T("");
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
	bool _ArgsLoaded = false;
	bool _EnableByOpen = false;
	bool _EnableByBand = false;
	/// <summary>
	/// 매수 진입 조건 매개변수 데이터 소스
	/// </summary>
	std::map<std::string, ArgDataSource> _BuyEntArg;
	/// <summary>
	/// 매도 진입 조건 매개변수 데이터 소스
	/// </summary>
	std::map<std::string, ArgDataSource> _SellEntArg;
	/// <summary>
	/// 매수 청산 조건 매개변수 데이터 소스
	/// </summary>
	std::map<std::string, ArgDataSource> _BuyLiqArg;
	/// <summary>
	/// 매도 청산 조건 매개변수 데이터 소스
	/// </summary>
	std::map<std::string, ArgDataSource> _SellLiqArg;
	/// <summary>
	/// 시스템 그룹 타입
	/// </summary>
	VtSystemGroupType _SystemGroup;
	std::map<std::string, ArgNameType> _ArgTypeMap;
	void InitArgType();
	std::map<std::string, std::map<std::string, ArgDataSource>*> _CondGroupMap;
	/// <summary>
	/// 마지막으로 진입한 데일리 인덱스
	/// </summary>
	int _LastEntryDailyIndex = -1;
	bool _EnableBarIndex = false;
	bool _EnableFilterMulti = false;
};

