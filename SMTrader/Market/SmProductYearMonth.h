#pragma once
#include <string>
#include <vector>
#include <map>
class VtSymbol;
class SmProductYearMonth
{
public:
	SmProductYearMonth();
	~SmProductYearMonth();
	std::string ProductCode;
	std::string YearMonthCode;
	std::vector<VtSymbol*> SymbolList();
	void AddSymbol(VtSymbol* symbol);
private:
	std::map<std::string, VtSymbol*> _SymbolMap;
};

