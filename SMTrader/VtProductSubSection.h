#pragma once
#include <string>
#include <vector>
#include <map>
class VtSymbol;
class VtOptionMonthSection;
class VtProductSubSection
{
public:
	VtProductSubSection();
	~VtProductSubSection();

	std::string Name;
	std::string Code;
	void AddSymbol(VtSymbol* sym);
	VtOptionMonthSection* FindOptionMap(std::string code);
	std::map<std::string, VtOptionMonthSection*> _OptionMap;
	std::vector<VtSymbol*> _SymbolVector;
};

