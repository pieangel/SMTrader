#include "afx.h"
#include "SmProductYearMonth.h"
#include "../VtSymbol.h"


SmProductYearMonth::SmProductYearMonth()
{
}


SmProductYearMonth::~SmProductYearMonth()
{
}

std::vector<VtSymbol*> SmProductYearMonth::SymbolList()
{
	std::vector<VtSymbol*> vector;
	for (auto it = _SymbolMap.begin(); it != _SymbolMap.end(); ++it) {
		vector.push_back(it->second);
	}

	return vector;
}

void SmProductYearMonth::AddSymbol(VtSymbol* symbol)
{
	if (!symbol)
		return;
	_SymbolMap[symbol->ShortCode] = symbol;
}

VtSymbol* SmProductYearMonth::GetFirstSymbol()
{
	if (_SymbolMap.size() == 0)
		return nullptr;
	return _SymbolMap.begin()->second;
}
