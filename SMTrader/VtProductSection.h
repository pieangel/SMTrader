#pragma once
#include <string>
#include <list>
#include <vector>
class VtSymbol;
class VtProductSubSection;
class VtProductCategoryManager;
class VtProductSection
{
public:
	VtProductSection();
	~VtProductSection();
	bool Visible;
	std::string UserDefinedName;
	std::string Name;
	std::string Code;
	std::string ExPos;
	std::list<VtSymbol*> SymbolList;
	void AddSymbol(VtSymbol* symbol);
	std::vector<VtProductSubSection*> SubSectionVector;

	void Begin();
	void End();
	void AddSubSection();
	void AddSubSection(std::string code, std::string name);

	VtProductCategoryManager* ProductCategoryManager() const { return _ProductCategoryManager; }
	void ProductCategoryManager(VtProductCategoryManager* val) { _ProductCategoryManager = val; }
private:
	VtProductCategoryManager* _ProductCategoryManager = nullptr;
};

