#pragma once
#include "stdafx.h"
#include <string>
#include <map>
#include <vector>
#include "Global/VtDefine.h"
																											  /// <summary>
enum class HdTaskType
{
	HdNoTask,
	HdAcceptedHistory, // ����Ȯ�� ����
	HdFilledHistory, // ü�᳻��
	HdOutstandingHistory, // �ܰ���
	HdOutstanding, // �����ܰ�
	HdCmeAcceptedHistory, // cme ����Ȯ�� ����
	HdCmeFilledHistory, // cme ü�᳻��
	HdCmeOutstandingHistory, // cme �ܰ���
	HdCmeOutstanding, // cme �ܰ�
	HdCmeAsset, // cme �ڻ�
	HdCmePureAsset, // cme ���ڻ�
	HdAsset, // �ڻ�
	HdDeposit, // ���� ����
	HdDailyProfitLoss, // ���� ����
	HdFilledHistoryTable, // ü�� ����
	HdAccountProfitLoss, // ���º� ����
	HdSymbolCode, // �ɺ� �ڵ�
	HdTradableCodeTable, // �ŷ����� ����ǥ
	HdApiCustomerProfitLoss, // �� ���� ��Ȳ
	HdChartData, // ��Ʈ ������
	HdCurrentQuote, // ���� �ü�
	HdDailyQuote, // �Ϻ� �ü�
	HdTickQuote, // ƽ �ü�
	HdSecondQutoe, // ��� �ü�
	HdSymbolMaster, // �ɺ� ������
	HdStockFutureSymbolMaster, // ���Ǽ��� �ɺ� ������
	HdIndustryMaster, // ����� ������
	HdTaskComplete, // �۾� �Ϸ�
	HdRecentMonthSymbolMaser, // �ֱٿ� �ɺ� ������
	HdOrderNew, // �ű��ֹ�
	HdOrderChange, // ���� �ֹ�
	HdOrderCancel, // ��� �ֹ�
	HdAccountFeeInfoStep1, // ���� ����
	HdAccountFeeInfoStep2 // ���� ����
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
};
