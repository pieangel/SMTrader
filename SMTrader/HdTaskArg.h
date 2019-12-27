#pragma once
#include "stdafx.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include "Global/VtDefine.h"
																											  /// <summary>
enum class HdTaskType
{
	HdNoTask,
	HdAcceptedHistory, // 접수확인 내역
	HdFilledHistory, // 체결내역
	HdOutstandingHistory, // 잔고내역
	HdOutstanding, // 현재잔고
	HdCmeAcceptedHistory, // cme 접수확인 내역
	HdCmeFilledHistory, // cme 체결내역
	HdCmeOutstandingHistory, // cme 잔고내역
	HdCmeOutstanding, // cme 잔고
	HdCmeAsset, // cme 자산
	HdCmePureAsset, // cme 순자산
	HdAsset, // 자산
	HdDeposit, // 계좌 정보
	HdDailyProfitLoss, // 일일 손익
	HdFilledHistoryTable, // 체결 내역
	HdAccountProfitLoss, // 계좌별 손익
	HdSymbolCode, // 심볼 코드
	HdTradableCodeTable, // 거래가능 종목표
	HdApiCustomerProfitLoss, // 고객 손익 현황
	HdChartData, // 차트 데이터
	HdCurrentQuote, // 현재 시세
	HdDailyQuote, // 일별 시세
	HdTickQuote, // 틱 시세
	HdSecondQutoe, // 장외 시세
	HdSymbolMaster, // 심볼 마스터
	HdStockFutureSymbolMaster, // 증권선물 심볼 마스터
	HdIndustryMaster, // 산업별 마스터
	HdTaskComplete, // 작업 완료
	HdRecentMonthSymbolMaser, // 최근월 심볼 마스터
	HdOrderNew, // 신규주문
	HdOrderChange, // 정정 주문
	HdOrderCancel, // 취소 주문
	HdAccountFeeInfoStep1, // 계좌 정보
	HdAccountFeeInfoStep2, // 계좌 정보
	HdSymbolFileDownload // 심볼파일 다운로드
};

enum class HdTaskState {
	HdTaskInit,
	HdSymbolFileDownloading,
	HdSymbolFileDownloaded,
	HdSymbolMasterDownloading,
	HdSymbolMasterDownloaded,
	HdDepositDownloading,
	HdDepositDownloaded,
	HdApiCustomerProfitLossDownloading,
	HdApiCustomerProfitLossDownloaded,
	HdOutstandingDownloading,
	HdOutstandingDownloaded,
	HdAcceptedHistoryDownloading,
	HdAcceptedHistoryDownloaded,
	HdDownloadComplete
};

class HdTaskArg
{
public:
	
	HdTaskArg(HdTaskType type)
	{
		Type = type;
	}
	HdTaskArg()
	{}
	virtual ~HdTaskArg()
	{}
public:
	std::string info_text;
	int RequestID = 0;
	HdTaskType Type;
	void AddArg(std::string name, std::string value);
	std::string GetArg(std::string name);
private:
	std::map<std::string, std::string> ArgMap;
};

struct HdTaskInfo{
	std::string TaskName;
	int TotalTaskCount;
	int RemainTaskCount;
	std::vector<std::pair<std::string, HdTaskArg>> argVec;
	std::map<int, std::shared_ptr<HdTaskArg>> argMap;
};
