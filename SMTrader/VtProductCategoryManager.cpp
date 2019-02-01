#include "stdafx.h"
#include "VtProductCategoryManager.h"
#include "VtProductCategory.h"
#include "VtProductSection.h"
#include "VtSymbol.h"
#include "HdScheduler.h"
#include "VtProductSubSection.h"

void VtProductCategoryManager::Begin()
{
	InitHdCategoryNSection();
	InitMainSections();
}

void VtProductCategoryManager::End()
{
	for (auto i = CategoryList.begin(); i != CategoryList.end(); ++i)
	{
		delete *i;
	}
}

VtProductCategoryManager::VtProductCategoryManager()
{
	Begin();
}


VtProductCategoryManager::~VtProductCategoryManager()
{
	End();
}

void VtProductCategoryManager::InitCategory()
{
	VtProductSection* section = nullptr;
	VtProductCategory* cat = nullptr;
	cat = new VtProductCategory();
	cat->Name = _T("통화");

	section = new VtProductSection();
	section->Code = _T("6A");
	section->Name = _T("호주달러");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("6B");
	section->Name = _T("영국파운드");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("6C");
	section->Name = _T("캐나다달러");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("6E");
	section->Name = _T("유로통화");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("6J");
	section->Name = _T("일본엔");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(std::move(section));

	section = new VtProductSection();
	section->Code = _T("6L");
	section->Name = _T("브라질헤알");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("6M");
	section->Name = _T("멕시코페소");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("6N");
	section->Name = _T("뉴질랜드달러");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("6S");
	section->Name = _T("스위스프랑");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("E7");
	section->Name = _T("미니유로");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("J7");
	section->Name = _T("미니엔");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("M6A");
	section->Name = _T("ⓜ호주달러");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("M6B");
	section->Name = _T("ⓜ영국파운드");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("M6E");
	section->Name = _T("ⓜ유로통화");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("KRW");
	section->Name = _T("한국원화");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("RMB");
	section->Name = _T("중국위안");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	CategoryList.push_back(cat);

	cat = new VtProductCategory();
	cat->Name = _T("지수");

	section = new VtProductSection();
	section->Code = _T("ES");
	section->Name = _T("미니S&P");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("NQ");
	section->Name = _T("미니나스닥");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("YM");
	section->Name = _T("미니다우($5)");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("CN");
	section->Name = _T("차이나A50");
	section->ExPos = _T("SGX");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("CH");
	section->Name = _T("MSCI 차이나인덱스");
	section->ExPos = _T("SGX");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("IN");
	section->Name = _T("CNX Nifty");
	section->ExPos = _T("SGX");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("NKD");
	section->Name = _T("Nikkei 달러");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("PL");
	section->Name = _T("Platinum");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("TW");
	section->Name = _T("MSCI 타이완");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	CategoryList.push_back(cat);

	cat = new VtProductCategory();
	cat->Name = _T("에너지");

	section = new VtProductSection();
	section->Code = _T("QG");
	section->Name = _T("미니가스");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("QM");
	section->Name = _T("미니원유");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("CL");
	section->Name = _T("WTI원유");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("NG");
	section->Name = _T("천연가스");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("RB");
	section->Name = _T("가솔린");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("HO");
	section->Name = _T("난방유");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("BZ");
	section->Name = _T("Brent크루드오일");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	CategoryList.push_back(cat);

	cat = new VtProductCategory();
	cat->Name = _T("채권");

	section = new VtProductSection();
	section->Code = _T("ZB");
	section->Name = _T("미국채30년");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("ZF");
	section->Name = _T("미국채5년물");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("ZN");
	section->Name = _T("미국채 10년");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("ZT");
	section->Name = _T("미국채2년");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("GE");
	section->Name = _T("유로통화선물");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	CategoryList.push_back(cat);

	cat = new VtProductCategory();
	cat->Name = _T("금속");

	section = new VtProductSection();
	section->Code = _T("GC");
	section->Name = _T("금(골드)");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("SI");
	section->Name = _T("은(실버)");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("HG");
	section->Name = _T("구리");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("MGC");
	section->Name = _T("ⓜ금");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("QC");
	section->Name = _T("미니구리");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("QO");
	section->Name = _T("미니금");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	CategoryList.push_back(cat);

	cat = new VtProductCategory();
	cat->Name = _T("농산물");

	section = new VtProductSection();
	section->Code = _T("ZC");
	section->Name = _T("옥수수");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("ZS");
	section->Name = _T("대두");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("ZW");
	section->Name = _T("밀");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);


	section = new VtProductSection();
	section->Code = _T("ZF");
	section->Name = _T("U.S T-Notes 5년");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);


	section = new VtProductSection();
	section->Code = _T("ZL");
	section->Name = _T("콩기름");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("ZM");
	section->Name = _T("Soybean Meal");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("ZN");
	section->Name = _T("U.S T-Notes 10 Yr");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("ZT");
	section->Name = _T("U.S T-Notes 2 Yr");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("XC");
	section->Name = _T("미니옥수수");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("XK");
	section->Name = _T("미니대두");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("XW");
	section->Name = _T("미니밀");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	CategoryList.push_back(cat);
}

void VtProductCategoryManager::InitHdCategoryNSection()
{
	VtProductSection* section = nullptr;
	VtProductCategory* cat = nullptr;
	cat = new VtProductCategory();
	cat->Name = _T("일반");

	section = new VtProductSection();
	section->Code = _T("101F");
	section->Name = _T("KOSPI 선물");
	section->UserDefinedName = _T("지수선물");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	section->AddSubSection(_T("101"), _T("코스피 선물"));
	//section->AddSubSection(_T("401"), _T("코스피 복합선물"));
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("101O");
	section->Name = _T("KOSPI 옵션");
	section->UserDefinedName = _T("지수옵션");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	section->AddSubSection(_T("201"), _T("코스피 콜옵션"));
	section->AddSubSection(_T("301"), _T("코스피 풋선물"));
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("105F");
	section->Name = _T("MKOSPI 선물");
	section->UserDefinedName = _T("미니선물");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	section->AddSubSection(_T("105"), _T("미니코스피 선물"));
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("105O");
	section->Name = _T(" MKOSPI 옵션");
	section->UserDefinedName = _T("미니옵션");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	section->AddSubSection(_T("205"), _T("미니코스피 콜옵션"));
	section->AddSubSection(_T("305"), _T("미니코스피 콜옵션"));
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("106F");
	section->Name = _T("KOSDAQ 선물");
	section->UserDefinedName = _T("코닥선물");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	section->AddSubSection(_T("106"), _T("코스닥 선물"));
	//section->AddSubSection(_T("406"), _T("코스닥 복합선물"));
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("106O");
	section->Name = _T("KOSDAQ 옵션");
	section->UserDefinedName = _T("코닥옵션");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	section->AddSubSection(_T("206"), _T("코스닥 콜옵션"));
	section->AddSubSection(_T("306"), _T("코스닥 풋옵션"));
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("107F");
	section->Name = _T("EST 선물");
	section->UserDefinedName = _T("유로선물");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	section->AddSubSection(_T("107"), _T("EST 선물"));
	//section->AddSubSection(_T("407"), _T("EST 복합선물"));
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("108F");
	section->Name = _T("KRX300 선물");
	section->UserDefinedName = _T("코삼선물");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	section->AddSubSection(_T("108"), _T("KRX300 선물"));
	//section->AddSubSection(_T("408"), _T("KRX300 복합선물"));
	SectionMap[section->Code] = section;

	/*
	section = new VtProductSection();
	section->Code = _T("165F");
	section->Name = _T("BM3 선물");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("166F");
	section->Name = _T("BM5 선물");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);
	SectionMap[section->Code] = section;
	*/
	section = new VtProductSection();
	section->Code = _T("167F");
	section->Name = _T("BMA 선물");
	section->UserDefinedName = _T("국채선물");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	section->AddSubSection(_T("167"), _T("10년국채 선물"));
	//section->AddSubSection(_T("467"), _T("10년국채 복합선물"));
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("175F");
	section->Name = _T("USD 선물");
	section->UserDefinedName = _T("달러선물");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);


	section->ProductCategoryManager(this);
	section->AddSubSection(_T("175"), _T("원달러 선물"));
	//section->AddSubSection(_T("475"), _T("원달러 복합선물"));
	SectionMap[section->Code] = section;
	CategoryList.push_back(cat);

	cat = new VtProductCategory();
	cat->Name = _T("주식선물");

	section = new VtProductSection();
	section->Code = _T("111F");
	section->Name = _T("삼성전자 선물");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("112F");
	section->Name = _T("SK텔레콤 선물");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("113F");
	section->Name = _T("POSCO 선물");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("114F");
	section->Name = _T("KT 선물");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("115F");
	section->Name = _T("한국전력 선물");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("116F");
	section->Name = _T("현대차 선물");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	SectionMap[section->Code] = section;
	CategoryList.push_back(cat);
}

void VtProductCategoryManager::InitFavoriteSections()
{
	std::string sectionName;
	sectionName = _T("101F");
	FavoriteList.push_back(sectionName);

	sectionName = _T("101O");
	FavoriteList.push_back(sectionName);

	sectionName = _T("105F");
	FavoriteList.push_back(sectionName);

	sectionName = _T("105O");
	FavoriteList.push_back(sectionName);

	sectionName = _T("106F");
	FavoriteList.push_back(sectionName);

	sectionName = _T("106O");
	FavoriteList.push_back(sectionName);

	//sectionName = _T("107F");
	//FavoriteList.push_back(sectionName);

	//sectionName = _T("108F");
	//FavoriteList.push_back(sectionName);

	sectionName = _T("167F");
	FavoriteList.push_back(sectionName);

	sectionName = _T("175F");
	FavoriteList.push_back(sectionName);
}

void VtProductCategoryManager::InitMainSections()
{
	std::string sectionName;
	sectionName = _T("101F");
	MainFutureVector.push_back(sectionName);

	sectionName = _T("101O");
	MainOptionVector.push_back(sectionName);

	sectionName = _T("105F");
	MainFutureVector.push_back(sectionName);

	sectionName = _T("105O");
	MainOptionVector.push_back(sectionName);

	sectionName = _T("106F");
	MainFutureVector.push_back(sectionName);

	sectionName = _T("106O");
	MainOptionVector.push_back(sectionName);

	//sectionName = _T("107F");
	//MainFutureVector.push_back(sectionName);

	//sectionName = _T("108F");
	//MainFutureVector.push_back(sectionName);

	sectionName = _T("167F");
	MainFutureVector.push_back(sectionName);

	sectionName = _T("175F");
	MainFutureVector.push_back(sectionName);
}

void VtProductCategoryManager::AddSymbol(VtSymbol* symbol)
{
	if (symbol == nullptr)
		return;
	int len = symbol->ShortCode.length();
	if (len == 0)
		return;

	std::string sectionCode;
	if (len == 4)
		sectionCode = symbol->ShortCode.substr(0, 2);
	else
		sectionCode = symbol->ShortCode.substr(0, 3);

	auto it = SectionMap.find(sectionCode);
	if (it == SectionMap.end())
	{
		return;
	}
	try
	{
		VtProductSection* section = SectionMap[sectionCode];
		section->AddSymbol(symbol);
	}
	catch (std::exception* e)
	{
		std::string errorMsg = e->what();
	}
}

void VtProductCategoryManager::GetSymbolCode(std::string catName)
{
	HdScheduler* scheduler = HdScheduler::GetInstance();

	for (auto it = CategoryList.begin(); it != CategoryList.end(); ++it) {
		VtProductCategory* prdtCat = *it;
		if (prdtCat->Name.compare(catName) == 0) {
			// 스케줄러에 심볼 갯수를 등록해 주고 다 되면 다음 작업을 진행하게 만든다.
			for (int i = 0; i < prdtCat->SectionList.size(); ++i) {
				scheduler->AddRequest(HdTaskType::HdSymbolCode, i);
			}
			prdtCat->GetSymbolCode();
			break;
		}
	}
}

void VtProductCategoryManager::GetSymbolCode(std::vector<std::pair<std::string, HdTaskArg>>& argVec, std::string catName)
{
	HdScheduler* scheduler = HdScheduler::GetInstance();

	for (auto it = CategoryList.begin(); it != CategoryList.end(); ++it) {
		VtProductCategory* prdtCat = *it;
		if (prdtCat->Name.compare(catName) == 0) {
			// 스케줄러에 심볼 갯수를 등록해 주고 다 되면 다음 작업을 진행하게 만든다.
			for (int i = 0; i < prdtCat->SectionList.size(); ++i) {
				scheduler->AddRequest(HdTaskType::HdSymbolCode, i);
			}
			prdtCat->GetSymbolCode(argVec);
			break;
		}
	}
}

void VtProductCategoryManager::GetRecentMonthSymbolMasterByCategory(std::string catName)
{
	HdScheduler* scheduler = HdScheduler::GetInstance();

	for (auto it = CategoryList.begin(); it != CategoryList.end(); ++it) {
		VtProductCategory* prdtCat = *it;
		if (prdtCat->Name.compare(catName) == 0) {
			// 스케줄러에 섹션 개수를 등록해 주고 섹션에 최근월 상품 정보를 가져오게 한다.
			scheduler->SubSectionCount(SubSectionMap.size());
			prdtCat->GetRecentMonthSymbolMaster();
			break;
		}
	}
}

void VtProductCategoryManager::GetRecentMonthSymbolMasterByCategory(std::vector<std::pair<std::string, HdTaskArg>>& argVec, std::string catName)
{
	HdScheduler* scheduler = HdScheduler::GetInstance();
	for (auto it = CategoryList.begin(); it != CategoryList.end(); ++it) {
		VtProductCategory* prdtCat = *it;
		if (prdtCat->Name.compare(catName) == 0) {
			// 스케줄러에 섹션 개수를 등록해 주고 섹션에 최근월 상품 정보를 가져오게 한다.
			scheduler->SubSectionCount(SubSectionMap.size());
			prdtCat->GetRecentMonthSymbolMaster(argVec);
			break;
		}
	}
}

void VtProductCategoryManager::AddProductSubSection(VtProductSubSection* subSection)
{
	if (!subSection)
		return;

	SubSectionMap[subSection->Code] = subSection;
}

VtProductSubSection* VtProductCategoryManager::FindProductSubSection(std::string code)
{
	auto it = SubSectionMap.find(code);
	if (it != SubSectionMap.end()) {
		VtProductSubSection* subSection = it->second;

		return subSection;
	} else 
		return nullptr;
}

VtProductSection* VtProductCategoryManager::FindProductSection(std::string code)
{
	auto it = SectionMap.find(code);
	if (it != SectionMap.end()) {
		return it->second;
	}

	return nullptr;
}

VtSymbol* VtProductCategoryManager::GetRecentFutureSymbol(std::string secName)
{
	VtProductSection* section = FindProductSection(secName);
	if (section) {
		if (section->SubSectionVector.size() > 0) {
			VtProductSubSection* subSection = section->SubSectionVector.front();
			if (subSection->_SymbolVector.size() > 0) {
				VtSymbol* sym = subSection->_SymbolVector.front();
				return sym;
			}
		}
	}

	return nullptr;
}

