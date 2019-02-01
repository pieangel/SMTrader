#pragma once
#include <string>
#include <list>
#include "VtProductSection.h"
#include "HdTaskArg.h"
class VtProductCategory
{
public:
	VtProductCategory();
	~VtProductCategory();
	std::string Name;
	std::list<VtProductSection*> SectionList;
	void AddSection(VtProductSection* section);
	void GetSymbolCode();
	void GetSymbolCode(std::vector<std::pair<std::string, HdTaskArg>>& argList);
	void GetRecentMonthSymbolMaster();
	void GetRecentMonthSymbolMaster(std::vector<std::pair<std::string, HdTaskArg>>& argList);
	void Begin();
	void End();
};

