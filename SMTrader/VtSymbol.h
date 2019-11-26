#pragma once
#include <string>
#include "VtQuote.h"
#include "VtHoga.h"
class VtSymbol
{
public:
	VtSymbol();
	~VtSymbol();

	int Index;
	int NearMonth;
	std::string ProductCode;
	std::string MarketName;
	double CtrUnit;
	std::string LastDate;
	// 풀코드
	std::string FullCode;
	// 짧은 코드
	std::string ShortCode;
	// 종목명
	std::string Name;
	std::string ShortName;
	// 종목명 - 영문
	std::string EngName;
	std::string BriefName;
	std::string Initial;

	// 틱크기
	double TickSize;
	std::string UserDefinedName;
	// 틱크기 - 정수로 바꾼것
	int intTickSize;
	// 소수점 자릿수 - 정수
	int Decimal;

	int SymbolDate;
	// 틱가치
	double TickValue;
	// 거래 승수
	int Seungsu = 250000;
	int ExpireYear;
	int ExpireMonth;
	int ComToPrev;
	int UpdownRate;


	VtHoga Hoga;
	VtQuote Quote;

	void Begin();
	void End();

	bool RecentMonth() const { return _RecentMonth; }
	void RecentMonth(bool val) { _RecentMonth = val; }
	bool RequestedSymbolMaster() const { return _RequestedSymbolMaster; }
	void RequestedSymbolMaster(bool val) { _RequestedSymbolMaster = val; }
public:
	void GetSymbolMaster();
private:
	bool _RecentMonth = false;
	bool _RequestedSymbolMaster = false;
};

struct CompareSymbol {
	bool operator()(VtSymbol * lhs, VtSymbol * rhs) { return lhs->SymbolDate < rhs->SymbolDate; }
};

