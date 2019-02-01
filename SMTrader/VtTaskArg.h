#pragma once
#include <string>
#include "Global/VtDefine.h"
enum class VtTaskType
{
	/// <summary>
	/// 계좌에 대한 실제 체결 - 전부, 접수, 체결
	/// </summary>
	VtTaskOrderFilled, // 실체결
	/// <summary>
	/// 계좌에 대한 종목별 실제 잔고
	/// </summary>
	VtTaskAccountRemain, // 실잔고
	/// <summary>
	/// 계좌에 대한 모든 주문 체결 내역과 상태
	/// </summary>
	VtTaskAccountFilled, // 계좌별 주문 체결 현황
	/// <summary>
	/// 계좌에 대한 예탁자산 및 증거금
	/// </summary>
	VtTaskAccountDeposit, // 예탁자산 및 증거금
	/// <summary>
	/// 종목에 대한 종합 정보
	/// </summary>
	VtTaskSymbolMaster, // 종목 마스터
	/// <summary>
	/// 종목에 대한 현재 시세
	/// </summary>
	VtTaskSymbolQuote, // 종목 시세
	/// <summary>
	/// 종목에 대한 현재 호가 - 5호가
	/// </summary>
	VtTaskSymbolHoga, // 종목 호가
	/// <summary>
	/// 종목에 대한 틱시세
	/// </summary>
	VtTaskSymbolTickQuote, // 종목 체결내역
	/// <summary>
	/// 종목에 대한 차트 데이터
	/// </summary>
	VtTaskChartData,  // 차트 데이터

	HdAcceptedHistory,
	HdFilledHistory,
	HdOutstandingHistory,
	HdOutstanding,
	HdCmeAcceptedHistory,
	HdCmeFilledHistory,
	HdCmeOutstandingHistory,
	HdCmeOutstanding,
	HdCmeAsset,
	HdCmePureAsset,
	HdAsset,
	HdDeposit,
	HdDailyProfitLoss,
	HdFilledHistoryTable,
	HdAccountProfitLoss,
	HdSymbolCode,
	HdTradableCodeTable,
	HdApiCustomerProfitLoss,
	HdChartData,
	HdCurrentQuote,
	HdDailyQuote,
	HdTickQuote,
	HdSecondQutoe,
	HdSymbolMaster,
	HdStockFutureSymbolMaster,
	HdIndustryMaster,
};

class VtTaskArg
{
public:
	VtTaskArg(VtTaskType type)
	{
		Type = type;
	}
	VtTaskArg();
	virtual ~VtTaskArg();
public:
	VtTaskType Type;
};

/// <summary>
/// 종목별 실체결 목록
/// </summary>
class VtTaskOrderFilledArg : public VtTaskArg
{
public:
	VtTaskOrderFilledArg() { };
	VtTaskOrderFilledArg(VtTaskType type) : VtTaskArg(type) { };
	virtual ~VtTaskOrderFilledArg() { };
	std::string Account;
	std::string Pass;
	int Divide; // 0 : All, 1 : Filled, 2 : Accepted.
	std::string BaseDate;
	int OrderBy; // 1 : 정순, 2 : 역순
};

/// <summary>
/// 종목별 실제 잔고
/// </summary>
class VtTaskAccountRemainArg : public VtTaskArg
{
public:
	VtTaskAccountRemainArg() { };
	VtTaskAccountRemainArg(VtTaskType type) : VtTaskArg(type) { };
	virtual ~VtTaskAccountRemainArg() { };
	std::string Account;
	std::string Pass;
};

/// <summary>
/// 계좌별 주문 체결 현황
/// </summary>
class VtTaskAccountFilledArg : public VtTaskArg
{
public:
	VtTaskAccountFilledArg() { };
	VtTaskAccountFilledArg(VtTaskType type) : VtTaskArg(type) { };
	virtual ~VtTaskAccountFilledArg() { };
	std::string Account;
	std::string Pass;
	int Divide; // 0.전체 1.체결 2.미체결 3.전량정정 4.전량취소 5.주문거부 6.체결 + 미체결
	std::string BaseDate;
	int OrderBy;  //조회순서(1.정순 2.역순)
	int Count; 
};

/// <summary>
/// 계좌 예탁금 및 상황
/// </summary>
class VtTaskAccountDepositArg : public VtTaskArg
{
public:
	VtTaskAccountDepositArg() { };
	VtTaskAccountDepositArg(VtTaskType type) : VtTaskArg(type) { };
	virtual ~VtTaskAccountDepositArg() { };
	std::string Account;
	std::string Pass;
	std::string Currency; // "USD"
};

/// <summary>
/// 종목 마스터
/// </summary>
class VtTaskSymbolMasterArg : public VtTaskArg // 종목 마스터
{
public:
	VtTaskSymbolMasterArg() { };
	VtTaskSymbolMasterArg(VtTaskType type) : VtTaskArg(type) { };
	virtual ~VtTaskSymbolMasterArg() { };
	std::string FullCode;
};

/// <summary>
/// 종목 시세
/// </summary>
class VtTaskSymbolQuoteArg : public VtTaskArg // 종목 시세
{
public:
	VtTaskSymbolQuoteArg() { };
	VtTaskSymbolQuoteArg(VtTaskType type) : VtTaskArg(type) { };
	virtual ~VtTaskSymbolQuoteArg() { };
	std::string FullCode;
};

/// <summary>
/// 종목 호가
/// </summary>
class VtTaskSymbolHogaArg : public VtTaskArg // 종목 호가
{
public:
	VtTaskSymbolHogaArg() { };
	VtTaskSymbolHogaArg(VtTaskType type) : VtTaskArg(type) { };
	virtual ~VtTaskSymbolHogaArg() { };
	std::string FullCode;
};

/// <summary>
/// 종목 틱 시세 
/// </summary>
class VtTaskSymbolTickQuoteArg : public VtTaskArg // 종목 체결내역
{
public:
	VtTaskSymbolTickQuoteArg() { };
	VtTaskSymbolTickQuoteArg(VtTaskType type) : VtTaskArg(type) { };
	virtual ~VtTaskSymbolTickQuoteArg() { };
	std::string FullCode;
};

/// <summary>
/// 차트 데이터
/// </summary>
class VtTaskChartDataArg : public VtTaskArg // 차트 데이터
{
public:
	VtTaskChartDataArg() { };
	VtTaskChartDataArg(VtTaskType type) : VtTaskArg(type) { };
	virtual ~VtTaskChartDataArg() { };
	VtChartType Type;
	std::string FullCode;
	std::string BaseDate;
	int Count;
	int Cycle; // tick, 분에서 모으는 단위	
	int Linked; // 연결선물구분 Y/N(일봉)			
	int JungBonDiv; // 1.전산장 2.본장 (분/틱봉)	
};