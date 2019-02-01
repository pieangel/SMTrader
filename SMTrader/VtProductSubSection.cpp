#include "stdafx.h"
#include "VtProductSubSection.h"
#include "VtSymbol.h"
#include "VtOptionMonthSection.h"

VtProductSubSection::VtProductSubSection()
{
}


VtProductSubSection::~VtProductSubSection()
{
	for (auto it = _OptionMap.begin(); it != _OptionMap.end(); ++it)
	{
		delete it->second;
	}
}

void VtProductSubSection::AddSymbol(VtSymbol* sym)
{
	if (!sym)
		return;
	CString yearMon;
	yearMon.Format(_T("%04d-%02d"), sym->ExpireYear, sym->ExpireMonth);
	std::string code(yearMon, yearMon.GetLength());
	auto it = _OptionMap.find(code);
	VtOptionMonthSection* optionSec = nullptr;
	if (it == _OptionMap.end())
	{
		optionSec = new VtOptionMonthSection();
		optionSec->Code(code);
		_OptionMap[code] = optionSec;
	}
	else
	{
		optionSec = _OptionMap[code];
	}

	optionSec->_SymbolVector.push_back(sym);
	_SymbolVector.push_back(sym);
}

VtOptionMonthSection* VtProductSubSection::FindOptionMap(std::string code)
{
	auto it = _OptionMap.find(code);
	if (it != _OptionMap.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}
