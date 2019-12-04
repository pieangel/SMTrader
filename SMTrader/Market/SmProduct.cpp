#include "afx.h"
#include "SmProduct.h"
#include "../VtSymbol.h"
#include "../VtSymbolManager.h"
#include "SmProductYearMonth.h"
SmProduct::SmProduct()
{
	
}


SmProduct::~SmProduct()
{
	for (auto it = _YearMonthMap.begin(); it != _YearMonthMap.end(); ++it) {
		delete it->second;
	}
}

VtSymbol* SmProduct::AddSymbol(std::string symCode)
{
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindAddSymbol(symCode);
	sym->ShortCode = symCode;
	_SymbolList.push_back(sym);
	//symMgr->AddHdSymbol(sym);
	AddToYearMonth(symCode, sym);
	return sym;
}

VtSymbol* SmProduct::GetRecentMonthSymbol()
{
	if (_SymbolList.size() == 0)
		return nullptr;
	return _SymbolList[0];
}

SmProductYearMonth* SmProduct::GetRecentYearMonth()
{
	if (_YearMonthMap.size() == 0)
		return nullptr;
	else
		return _YearMonthMap.begin()->second;
}

SmProductYearMonth* SmProduct::GetNextYearMonth()
{
	if (_YearMonthMap.size() <= 1)
		return nullptr;
	else
		return (++_YearMonthMap.begin())->second;
}

void SmProduct::AddToYearMonth(std::string symbol_code, VtSymbol* symbol)
{
	if (!symbol)
		return;

	if (isdigit(symbol_code.at(2))) { // 국내 상풍
		std::string product_code = symbol_code.substr(0, 3);
		std::string year_month = symbol_code.substr(3, 2);
		auto it = _YearMonthMap.find(year_month);
		SmProductYearMonth* ym = nullptr;
		if (it == _YearMonthMap.end()) {
			ym = new SmProductYearMonth();
			ym->ProductCode = product_code;
			ym->YearMonthCode = year_month;
			_YearMonthMap[year_month] = ym;
		}
		else {
			ym = it->second;
		}
		ym->SymbolList.push_back(symbol);
	}
	else { // 해외 상품
		std::string product_code = symbol_code.substr(0, 2);
		std::string year = symbol_code.substr(symbol_code.length() - 2, 2);
		std::string month = symbol_code.substr(symbol_code.length() - 3, 1);
		std::string year_month = year + month;
		auto it = _YearMonthMap.find(year_month);
		SmProductYearMonth* ym = nullptr;
		if (it == _YearMonthMap.end()) {
			ym = new SmProductYearMonth();
			ym->ProductCode = product_code;
			ym->YearMonthCode = year_month;
			_YearMonthMap[year_month] = ym;
		}
		else {
			ym = it->second;
		}
		ym->SymbolList.push_back(symbol);
	}
}

