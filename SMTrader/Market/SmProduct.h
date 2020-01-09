#pragma once
#include <string>
#include <vector>
#include <map>
class SmProductYearMonth;
class VtSymbol;
class SmProduct
{
public:
	SmProduct();
	~SmProduct();
	std::string Code() const { return _Code; }
	void Code(std::string val) { _Code = val; }
	std::string NameKr() const { return _NameKr; }
	void NameKr(std::string val) { _NameKr = val; }
	std::string Name() const { return _Name; }
	void Name(std::string val) { _Name = val; }
	std::string Exchange() const { return _Exchange; }
	void Exchange(std::string val) { _Exchange = val; }
	std::string ExchangeCode() const { return _ExchangeCode; }
	void ExchangeCode(std::string val) { _ExchangeCode = val; }
	std::string ExchangeIndex() const { return _ExchangeIndex; }
	void ExchangeIndex(std::string val) { _ExchangeIndex = val; }
	std::string MarketName() const { return _MarketName; }
	void MarketName(std::string val) { _MarketName = val; }
	std::string MarketCode() const { return _MarketCode; }
	void MarketCode(std::string val) { _MarketCode = val; }
	VtSymbol* AddSymbol(std::string symCode);
	VtSymbol* AddSymbol(std::string symCode, std::string name);
	std::vector<VtSymbol*>& GetSymbolList() {
		return _SymbolList;
	}
	VtSymbol* GetRecentMonthSymbol();
	VtSymbol* GetNextMonthSymbol();
	SmProductYearMonth* GetRecentYearMonth();
	SmProductYearMonth* GetNextYearMonth();
	std::map<std::string, SmProductYearMonth*>& GetYearMonthMap() {
		return _YearMonthMap;
	}

	SmProductYearMonth* GetYearMonth(std::string year_month);
	void MakeYearMonth();
private:
	// 품목코드
	std::string _Code;
	// 품목 한글 이름
	std::string _NameKr;
	// 품목 영문 이름
	std::string _Name;
	// 거래소 이름
	std::string _Exchange;
	/// <summary>
	/// 거래소 코드
	/// 1:CME, 2:CBOT, 3:SGX, 4:LIFFE, 5:EUREX, 6:ICE, 7:OSE, 8:LME
	/// </summary>
	std::string _ExchangeCode;
	// 거래소 인덱스 코드
	std::string _ExchangeIndex;
	// 품목이 속한 시장 이름
	std::string _MarketName;
	/// <summary>
	/// 품목이 속한 시장 구분 코드
	/// 101 : Currency(통화)                  
	/// 102 : Index(지수)                     
	/// 103 : Fixed Income(금리)              
	/// 104 : Energy(에너지)                  
	/// 105 : Metal(금속)                     
	/// 106 : Agricultural(농산물)            
	/// 107 : Dairy & Beef(축산)              
	/// </summary>
	std::string _MarketCode;
	std::vector<VtSymbol*> _SymbolList;
	std::map<std::string, SmProductYearMonth*> _YearMonthMap;
	void AddToYearMonth(VtSymbol* symbol);
	void AddToYearMonth(std::string symbol_code, VtSymbol* symbol);
	void AddToYearMonth(std::string symbol_code, std::string name, VtSymbol* symbol);
	std::map<std::string, std::string> _DomesticYearTable;
	std::map<std::string, std::string> _DomesticMonthTable;
	std::map<std::string, std::string> _AbroadMonthTable;
	int dow(int y, int m, int d);
};

