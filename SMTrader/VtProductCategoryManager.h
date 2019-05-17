#pragma once
#include <vector>
#include <string>
#include <map>
#include "Global/TemplateSingleton.h"
#include "HdTaskArg.h"
class VtProductCategory;
class VtProductSection;
class VtSymbol;
class VtProductSubSection;
class VtProductCategoryManager : public TemplateSingleton<VtProductCategoryManager>
{
public:
	void Begin();
	void End();
	VtProductCategoryManager();
	~VtProductCategoryManager();
	void InitCategory();
	void InitHdCategoryNSection();
	void InitFavoriteSections();
	void InitMainSections();
	std::vector<std::string> FavoriteList;
	std::vector<std::string> MainFutureVector;
	std::vector<std::string> MainOptionVector;
	std::vector<VtProductCategory*> CategoryList;
	std::map<std::string, VtProductSection*> SectionMap;
	void AddSymbol(VtSymbol* symbol);
	void GetSymbolCode(std::string catName);
	void GetSymbolCode(std::vector<std::pair<std::string,HdTaskArg>>& argVec, std::string catName);
	void GetRecentMonthSymbolMasterByCategory(std::string catName);
	void GetRecentMonthSymbolMasterByCategory(std::vector<std::pair<std::string, HdTaskArg>>& argVec, std::string catName);
	void AddProductSubSection(VtProductSubSection* subSection);
	std::map<std::string, VtProductSubSection*> SubSectionMap;
	VtProductSubSection* FindProductSubSection(std::string code);
	VtProductSection* FindProductSection(std::string code);
	/// <summary>
	/// 선물의 최근월물을 가져온다.
	/// </summary>
	/// <param name="secName">섹션 이름 : 예? 101F, 105F etc</param>
	/// <returns></returns>
	VtSymbol* GetRecentFutureSymbol(std::string secName);
	VtSymbol* GetNextFutureSymbol(std::string secName);
	std::vector<VtSymbol*> GetSectionSymbolVector(std::string secName);

};

