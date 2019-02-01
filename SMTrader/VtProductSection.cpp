#include "stdafx.h"
#include "VtProductSection.h"
#include "VtProductCategoryManager.h"
#include "VtProductSubSection.h"

VtProductSection::VtProductSection()
{
	Begin();
}


VtProductSection::~VtProductSection()
{
	End();
}

void VtProductSection::AddSymbol(VtSymbol* symbol)
{
	if (symbol == nullptr)
		return;

	SymbolList.push_back(symbol);
}

void VtProductSection::Begin()
{
	Visible = true;
	_ProductCategoryManager = nullptr;
}

void VtProductSection::End()
{
	for (auto it = SubSectionVector.begin(); it != SubSectionVector.end(); ++it)
	{
		delete *it;
	}
}

void VtProductSection::AddSubSection()
{
	VtProductCategoryManager* prdtCatMgr = _ProductCategoryManager;
	if (!prdtCatMgr)
		return;

	VtProductSubSection* subSec = new VtProductSubSection();
	subSec->Code = _T("101");
	subSec->Name = _T("코스피200 선물");
	SubSectionVector.push_back(subSec);
	prdtCatMgr->AddProductSubSection(subSec);

	subSec->Code = _T("401");
	subSec->Name = _T("코스피200 복합선물");
	SubSectionVector.push_back(subSec);
	prdtCatMgr->AddProductSubSection(subSec);
}

void VtProductSection::AddSubSection(std::string code, std::string name)
{
	VtProductCategoryManager* prdtCatMgr = _ProductCategoryManager;
	if (!prdtCatMgr)
		return;

	VtProductSubSection* subSec = new VtProductSubSection();
	subSec->Code = code;
	subSec->Name = name;
	SubSectionVector.push_back(subSec);
	prdtCatMgr->AddProductSubSection(subSec);
}
