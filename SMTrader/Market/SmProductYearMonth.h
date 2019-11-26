#pragma once
#include <string>
#include <vector>
class VtSymbol;
class SmProductYearMonth
{
public:
	SmProductYearMonth();
	~SmProductYearMonth();
	std::string ProductCode;
	std::string YearMonthCode;
	std::vector<VtSymbol*> SymbolList;
};

