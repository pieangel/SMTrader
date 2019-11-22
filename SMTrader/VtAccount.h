#pragma once
#include "Global/VtDefine.h"
#include <string>
#include <map>
#include "HdTaskArg.h"
#include "VtPosition.h"
#include "SimpleBinStream.h"
#include "Xml/pugixml.hpp"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;

// One account belongs to only one fund.
struct VtOrder;
class VtSymbol;
class VtFund;
class VtAccount
{
public:
	VtAccount();
	~VtAccount();
	std::string ParentAccountNo;
	std::string AccountNo; //	12	string	the number of the account.
	std::string AccountName; //	40	string	the name of the account.
	std::string Password = _T("");
	double Ratio;
	int SeungSu;

	std::map<std::string, VtPosition*>& GetPositionMap()
	{
		return PositionMap;
	}

	std::map<std::string, VtPosition*> PositionMap;
	int AccountLevel() const { return _AccountLevel; }
	void AccountLevel(int val) { _AccountLevel = val; }
	bool Prime() const { return _Prime; }
	void Prime(bool val) { _Prime = val; }
	VtAccount* ParentAccount() const { return _ParentAccount; }
	void ParentAccount(VtAccount* val) { _ParentAccount = val; }
	VtFund* Fund() const { return _Fund; }
	void Fund(VtFund* val) { _Fund = val; }
	bool Enable() const { return _Enable; }
	void Enable(bool val) { _Enable = val; }
public:
	bool hasValidPassword();
	VtPosition* FindAdd(std::string symbolCode);
	VtPosition* FindPosition(std::string symbolCode);
	VtPosition* FindAdd(VtOrder* order);
	void RemovePosition(std::string symbolCode);
	void ResetPosition(std::string symCode);
	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
	void SaveToXml(pugi::xml_node& node);
	void LoadFromXml(pugi::xml_node& node);
public:
	double InAmt = 0; //	15	double	�Աݱݾ�
	double OutAmt = 0; //	15	double	��ݱݾ�
	double Deposit = 0; //	15	double	��Ź�Ѿ�
	double OpenDeposit = 0;	//15	double	�򰡿�Ź�Ѿ�
	double OrdableAmt = 0; //	15	double	�ֹ����ɱݾ�
	double OpenPL = 0; //	15	double	�򰡼���
	double TradePL = 0; //	15	double	����(�Ÿ�)���� : û�� ����
	double TotalPL = 0; // �Ѽ���
	double Fee = 0; //	15	double	������
	double etc = 0;//	15	double
	double PurePL = 0; // ������

	double	Open_pos_mgn = 0;	/* �̰������ű�													*/
	double	Ord_mgn = 0;	/* �ֹ����ű�													*/
	double	Trst_mgn = 0;	/* ��Ź���ű�													*/
	double	Mnt_mgn = 0;	/* �������ű�													*/
	double	With_psbl_amt = 0;	/* ���Ⱑ�ɱݾ�													*/
	double	krw_with_psbl_amt = 0;	/* ��ȭ���Ⱑ�ɱݾ�												*/
	double	Add_mgn = 0;	/* �߰����ű�													*/
	double	Han_psbl_amt = 0;	/* ȯ�����ݾ�													*/
	double  TempFee = 0; // ������ �ջ��� ���� �ӽ� �����
	double  TempTradePL = 0; // �ӽ� ������
	double  TempOpenPL = 0;
	double  TempPurePL = 0;
	std::string	Crc_cd;
	int Type = 0;

	void SumOpenPL();
	void CalcOpenPL(VtSymbol* sym);
	void SumOpenPL(VtSymbol* sym, VtPosition* posi);
	VtPosition* CreatePosition(VtOrder* order);
	VtPosition* CopyPosition(VtPosition& curPosi);
	void GetFilledHistoryTable();
	void GetOutstandingHistory();
	void GetOutstanding();
	void GetOutstanding(std::vector<std::pair<std::string, HdTaskArg>>& argList);
	void GetAcceptedHistory();
	void GetAcceptedHistory(std::vector<std::pair<std::string, HdTaskArg>>& argList);
	void GetAsset();
	void GetDeposit();
	void GetDeposit(std::vector<std::pair<std::string, HdTaskArg>>& argList);
	void GetDailyProfitLoss();
	void GetAccountProfitLoss();
	void GetAccountProfitLossDirect();
	void GetApiCustomerProfitLoss();
	void GetApiCustomerProfitLoss(std::vector<std::pair<std::string, HdTaskArg>>& argList);
	bool CheckCutLoss(VtSymbol* sym, int size);
	bool CheckCutProfit(VtSymbol* sym, int size);
	bool CheckCutLoss(VtPosition* posi, VtSymbol* sym, int size);
	bool CheckCutProfit(VtPosition* posi, VtSymbol* sym, int size);
	bool CheckCutLossProfit(VtPosition* posi, VtSymbol* sym, int profitSize, int lossSize);
	void GetAccountInfoNFee(int taskType);


	// �־��� ���� ���Ͽ� ������ �ֹ��� ������ �����´�.
	// ��ȯ���� -1�� ���� �ѹ��� ���� �ֹ��� ���� ���� ���ٴ� ���̴�.
	int GetAcceptedOrderCount(std::string symCode);
	// �־��� ���� ���Ͽ� �ܰ� �����´�.
	// �ֹ��� ���������� û���Ŀ� 0�� �Ǿ��� ���� 0,
	// ���� �ܰ� ���� �ִٸ� ���� �ִ� �ܰ� ��ȯ�Ѵ�. (�ż��� �ŵ��� ������� �ʰ� �ܰ� ������ ��ȯ�Ѵ�.)
	std::pair<bool, int> GetRemainCount(std::string symCode);

private:
	bool _Enable = true;
	int _AccountLevel = 0; // 0 : ������, 1 : �������

	VtAccount* _ParentAccount = nullptr;
	std::vector<VtAccount*> _SubAccountList;
	bool _Prime = false;
public:
	void CreateDefaultSubAccount();
	VtAccount* CreateSubAccount(std::string acntNo, std::string acntName, bool prime = false);
	void RemoveSubAccount(std::string acntNo);
	VtAccount* FindSubAccount(std::string acntNo);
	VtAccount* FindSubAccount(std::string acntNo, std::string myself);
	int GetSubAccountCount()
	{
		return _SubAccountList.size();
	}
	std::vector<VtAccount*>& GetSubAccountList()
	{
		return _SubAccountList;
	}
	void AddToFundMap(std::string fundName, VtAccount* subAcnt);
	void RemoveFromFundMap(std::string fundName, VtAccount* subAcnt);
	std::vector<std::string> GetFundNameList();
	// For saving the temporary password.
	std::string TempPassword;
private:
	typedef std::map<std::string, VtAccount*> FundAccountMap;
	// ���¿� ���õ� �ݵ� ��ϵ�. �� ���°� ������¸� ������ ���� ���´� �ݵ忡 ���ϰԵȴ�.
	// ������°� �ݵ忡 ���ϰԵǸ� ��������� �������� �ݵ� ��Ͽ� �ݵ� �̸��� �߰��ǰ� �ȴ�.
	// �ֹ��� ��ȭ�� �Ͼ�� �� �� ���¿� ���� �ݵ忡 �򰡼��Ͱ� ���������� ����ϱ� ���Ͽ� �ʿ��ϴ�.
	std::map<std::string, FundAccountMap*> _FundMap;
	FundAccountMap* FindAccountMap(std::string fundName);
	// ���°� ���� ������ ��쿡 ���� ���°� ���� �ݵ� ��ü
	// ���� ���� ������°� �ݵ忡 ���� ���� ���� �ܵ� �������� ��Ÿ����.
	VtFund* _Fund;

};

