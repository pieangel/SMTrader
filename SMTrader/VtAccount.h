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
	double InAmt = 0; //	15	double	입금금액
	double OutAmt = 0; //	15	double	출금금액
	double Deposit = 0; //	15	double	예탁총액
	double OpenDeposit = 0;	//15	double	평가예탁총액
	double OrdableAmt = 0; //	15	double	주문가능금액
	double OpenPL = 0; //	15	double	평가손익
	double TradePL = 0; //	15	double	실현(매매)손익 : 청산 손익
	double TotalPL = 0; // 총손익
	double Fee = 0; //	15	double	수수료
	double etc = 0;//	15	double
	double PurePL = 0; // 순손익

	double	Open_pos_mgn = 0;	/* 미결제증거금													*/
	double	Ord_mgn = 0;	/* 주문증거금													*/
	double	Trst_mgn = 0;	/* 위탁증거금													*/
	double	Mnt_mgn = 0;	/* 유지증거금													*/
	double	With_psbl_amt = 0;	/* 인출가능금액													*/
	double	krw_with_psbl_amt = 0;	/* 원화인출가능금액												*/
	double	Add_mgn = 0;	/* 추가증거금													*/
	double	Han_psbl_amt = 0;	/* 환전대기금액													*/
	double  TempFee = 0; // 수수료 합산을 위한 임시 저장소
	double  TempTradePL = 0; // 임시 순손익
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


	// 주어진 종목에 대하여 접수된 주문의 개수를 가져온다.
	// 반환값이 -1인 것은 한번도 접수 주문이 들어온 적이 없다는 것이다.
	int GetAcceptedOrderCount(std::string symCode);
	// 주어진 종목에 대하여 잔고를 가져온다.
	// 주문이 나갔었지만 청산후에 0이 되었을 때는 0,
	// 현재 잔고가 남아 있다면 남아 있는 잔고를 반환한다. (매수나 매도를 고려하지 않고 잔고 개수만 반환한다.)
	std::pair<bool, int> GetRemainCount(std::string symCode);

private:
	bool _Enable = true;
	int _AccountLevel = 0; // 0 : 본계좌, 1 : 서브계좌

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
	// 계좌에 관련된 펀드 목록들. 각 계좌가 서브계좌를 가지고 서브 계좌는 펀드에 속하게된다.
	// 서브계좌가 펀드에 속하게되면 서브계좌의 본계좌의 펀드 목록에 펀드 이름이 추가되게 된다.
	// 주문에 변화가 일어났을 때 이 계좌에 속한 펀드에 평가손익과 실현속익을 계산하기 위하여 필요하다.
	std::map<std::string, FundAccountMap*> _FundMap;
	FundAccountMap* FindAccountMap(std::string fundName);
	// 계좌가 서브 계좌일 경우에 서브 계좌가 속한 펀드 객체
	// 널일 경우는 서브계좌가 펀드에 속해 있지 않은 단독 계좌임을 나타낸다.
	VtFund* _Fund;

};

