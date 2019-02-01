#pragma once
#include <string>
#include <vector>
class VtSymbol;
class VtOptionMonthSection
{
public:
	VtOptionMonthSection();
	~VtOptionMonthSection();
	std::string Code() const { return _Code; }
	void Code(std::string val) { _Code = val; }
	std::vector<VtSymbol*> _SymbolVector;
	bool RecentMonth() const { return _RecentMonth; }
	void RecentMonth(bool val) { _RecentMonth = val; }
	bool SymbolMasterRequested() const { return _SymbolMasterRequested; }
	void SymbolMasterRequested(bool val) { _SymbolMasterRequested = val; }
private:
	std::string _Code;
	bool _RecentMonth = false;
	bool _SymbolMasterRequested = false;
};

