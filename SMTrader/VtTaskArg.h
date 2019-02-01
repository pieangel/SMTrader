#pragma once
#include <string>
#include "Global/VtDefine.h"
enum class VtTaskType
{
	/// <summary>
	/// ���¿� ���� ���� ü�� - ����, ����, ü��
	/// </summary>
	VtTaskOrderFilled, // ��ü��
	/// <summary>
	/// ���¿� ���� ���� ���� �ܰ�
	/// </summary>
	VtTaskAccountRemain, // ���ܰ�
	/// <summary>
	/// ���¿� ���� ��� �ֹ� ü�� ������ ����
	/// </summary>
	VtTaskAccountFilled, // ���º� �ֹ� ü�� ��Ȳ
	/// <summary>
	/// ���¿� ���� ��Ź�ڻ� �� ���ű�
	/// </summary>
	VtTaskAccountDeposit, // ��Ź�ڻ� �� ���ű�
	/// <summary>
	/// ���� ���� ���� ����
	/// </summary>
	VtTaskSymbolMaster, // ���� ������
	/// <summary>
	/// ���� ���� ���� �ü�
	/// </summary>
	VtTaskSymbolQuote, // ���� �ü�
	/// <summary>
	/// ���� ���� ���� ȣ�� - 5ȣ��
	/// </summary>
	VtTaskSymbolHoga, // ���� ȣ��
	/// <summary>
	/// ���� ���� ƽ�ü�
	/// </summary>
	VtTaskSymbolTickQuote, // ���� ü�᳻��
	/// <summary>
	/// ���� ���� ��Ʈ ������
	/// </summary>
	VtTaskChartData,  // ��Ʈ ������

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
/// ���� ��ü�� ���
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
	int OrderBy; // 1 : ����, 2 : ����
};

/// <summary>
/// ���� ���� �ܰ�
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
/// ���º� �ֹ� ü�� ��Ȳ
/// </summary>
class VtTaskAccountFilledArg : public VtTaskArg
{
public:
	VtTaskAccountFilledArg() { };
	VtTaskAccountFilledArg(VtTaskType type) : VtTaskArg(type) { };
	virtual ~VtTaskAccountFilledArg() { };
	std::string Account;
	std::string Pass;
	int Divide; // 0.��ü 1.ü�� 2.��ü�� 3.�������� 4.������� 5.�ֹ��ź� 6.ü�� + ��ü��
	std::string BaseDate;
	int OrderBy;  //��ȸ����(1.���� 2.����)
	int Count; 
};

/// <summary>
/// ���� ��Ź�� �� ��Ȳ
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
/// ���� ������
/// </summary>
class VtTaskSymbolMasterArg : public VtTaskArg // ���� ������
{
public:
	VtTaskSymbolMasterArg() { };
	VtTaskSymbolMasterArg(VtTaskType type) : VtTaskArg(type) { };
	virtual ~VtTaskSymbolMasterArg() { };
	std::string FullCode;
};

/// <summary>
/// ���� �ü�
/// </summary>
class VtTaskSymbolQuoteArg : public VtTaskArg // ���� �ü�
{
public:
	VtTaskSymbolQuoteArg() { };
	VtTaskSymbolQuoteArg(VtTaskType type) : VtTaskArg(type) { };
	virtual ~VtTaskSymbolQuoteArg() { };
	std::string FullCode;
};

/// <summary>
/// ���� ȣ��
/// </summary>
class VtTaskSymbolHogaArg : public VtTaskArg // ���� ȣ��
{
public:
	VtTaskSymbolHogaArg() { };
	VtTaskSymbolHogaArg(VtTaskType type) : VtTaskArg(type) { };
	virtual ~VtTaskSymbolHogaArg() { };
	std::string FullCode;
};

/// <summary>
/// ���� ƽ �ü� 
/// </summary>
class VtTaskSymbolTickQuoteArg : public VtTaskArg // ���� ü�᳻��
{
public:
	VtTaskSymbolTickQuoteArg() { };
	VtTaskSymbolTickQuoteArg(VtTaskType type) : VtTaskArg(type) { };
	virtual ~VtTaskSymbolTickQuoteArg() { };
	std::string FullCode;
};

/// <summary>
/// ��Ʈ ������
/// </summary>
class VtTaskChartDataArg : public VtTaskArg // ��Ʈ ������
{
public:
	VtTaskChartDataArg() { };
	VtTaskChartDataArg(VtTaskType type) : VtTaskArg(type) { };
	virtual ~VtTaskChartDataArg() { };
	VtChartType Type;
	std::string FullCode;
	std::string BaseDate;
	int Count;
	int Cycle; // tick, �п��� ������ ����	
	int Linked; // ���ἱ������ Y/N(�Ϻ�)			
	int JungBonDiv; // 1.������ 2.���� (��/ƽ��)	
};