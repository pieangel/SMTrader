#pragma once
#include <string>
#include "VtQuote.h"
#include "VtHoga.h"
class VtSymbol
{
public:
	VtSymbol();
	~VtSymbol();
	std::string FullCode;
	std::string ShortCode;
	std::string Name;
	std::string ShortName;
	std::string EngName;
	std::string BriefName;
	std::string Initial;

	std::string Decimal;
	std::string TickSize;
	std::string TickValue;
	std::string UserDefinedName;
	int intTickSize;
	int IntDecimal;
	int SymbolDate;
	int ValueWidth;
	int intTickValue;
	int intValueWidth;
	int seungsu = 250000;
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

