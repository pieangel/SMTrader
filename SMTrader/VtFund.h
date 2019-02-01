#pragma once
#include <map>
#include <string>
#include "VtFundMap.h"
#include <sstream>
#include <msgpack.hpp>
#include "SimpleBinStream.h"
#include <set>
#include <vector>
#include "VtPosition.h"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;

class VtAccount;
class VtFundAccount;
class VtFund
{
public:
	VtFund();
	~VtFund();
	std::string Name;
	VtFundMap& GetFundMap()
	{
		return _FundMap;
	}

	std::vector<VtAccount*>& GetFundAccountVector()
	{
		return _FundMap.GetAccountVector();
	}

	VtAccount* CreateFundAccount(VtAccount* acnt);
	VtAccount* AddFundAccount(VtAccount* fundAcnt);
	void RemoveFundAccount(VtAccount* fundAcnt);
	void ClearAccounts();
	void Save(std::stringstream& ss);
	void Save(msgpack::sbuffer& ss);
	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
	void GetFundAccountList(std::set<VtAccount*>& acntSet);
	std::set<VtAccount*> GetParentAccountSet();
	std::pair<bool, int> GetRemainCount(std::string symCode);
	VtPosition GetPosition(std::string symCode, int& count);
	std::vector<VtPosition*> GetPositionList(std::string symCode);
	void ResetPosition(std::string symCode);
	VtPosition* FindPosition(std::string subAcntNo, std::string symCode);
	//std::vector<VtPosition> GetPositionBySymbol(std::string symCode);
	std::vector<VtPosition*> GetPositionListBySubAccount();
	std::map<std::string, std::shared_ptr<VtPosition>> GetPositionListByProduct();
	double OpenPL = 0; //	15	double	평가손익
	double TradePL = 0; //	15	double	실현(매매)손익 : 청산 손익
	double TotalPL = 0; // 총손익
	double TotalFee = 0; // 펀드 총 수수료
	void SumOpenPL();
	void AddSymbol(std::string symCode);
	std::map<std::string, std::string> SymbolList() const { return _SymbolList; }
	std::tuple<bool, int> GetInitAndAcceptedCount();
	std::tuple<bool, int> GetInitAndAcceptedCount(std::string symCode);
	VtAccount* GetParentAccount(std::string subAcntNo);
	VtAccount* FindAccount(std::string subAcntNo);
	/// <summary>
	/// 각 서브 계좌의 승수를 합한 값을 반환한다.
	/// </summary>
	/// <returns></returns>
	int GetTotalSeungSu();
private:
	VtFundMap _FundMap;
	std::map<std::string, std::string> _SymbolList;
};

