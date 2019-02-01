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
	cat->Name = _T("��ȭ");

	section = new VtProductSection();
	section->Code = _T("6A");
	section->Name = _T("ȣ�ִ޷�");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("6B");
	section->Name = _T("�����Ŀ��");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("6C");
	section->Name = _T("ĳ���ٴ޷�");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("6E");
	section->Name = _T("������ȭ");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("6J");
	section->Name = _T("�Ϻ���");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(std::move(section));

	section = new VtProductSection();
	section->Code = _T("6L");
	section->Name = _T("��������");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("6M");
	section->Name = _T("�߽������");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("6N");
	section->Name = _T("��������޷�");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("6S");
	section->Name = _T("����������");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("E7");
	section->Name = _T("�̴�����");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("J7");
	section->Name = _T("�̴Ͽ�");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("M6A");
	section->Name = _T("��ȣ�ִ޷�");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("M6B");
	section->Name = _T("�ٿ����Ŀ��");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("M6E");
	section->Name = _T("��������ȭ");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("KRW");
	section->Name = _T("�ѱ���ȭ");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("RMB");
	section->Name = _T("�߱�����");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	CategoryList.push_back(cat);

	cat = new VtProductCategory();
	cat->Name = _T("����");

	section = new VtProductSection();
	section->Code = _T("ES");
	section->Name = _T("�̴�S&P");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("NQ");
	section->Name = _T("�̴ϳ�����");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("YM");
	section->Name = _T("�̴ϴٿ�($5)");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("CN");
	section->Name = _T("���̳�A50");
	section->ExPos = _T("SGX");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("CH");
	section->Name = _T("MSCI ���̳��ε���");
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
	section->Name = _T("Nikkei �޷�");
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
	section->Name = _T("MSCI Ÿ�̿�");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	CategoryList.push_back(cat);

	cat = new VtProductCategory();
	cat->Name = _T("������");

	section = new VtProductSection();
	section->Code = _T("QG");
	section->Name = _T("�̴ϰ���");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("QM");
	section->Name = _T("�̴Ͽ���");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("CL");
	section->Name = _T("WTI����");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("NG");
	section->Name = _T("õ������");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("RB");
	section->Name = _T("���ָ�");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("HO");
	section->Name = _T("������");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("BZ");
	section->Name = _T("Brentũ������");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	CategoryList.push_back(cat);

	cat = new VtProductCategory();
	cat->Name = _T("ä��");

	section = new VtProductSection();
	section->Code = _T("ZB");
	section->Name = _T("�̱�ä30��");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("ZF");
	section->Name = _T("�̱�ä5�⹰");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("ZN");
	section->Name = _T("�̱�ä 10��");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("ZT");
	section->Name = _T("�̱�ä2��");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("GE");
	section->Name = _T("������ȭ����");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	CategoryList.push_back(cat);

	cat = new VtProductCategory();
	cat->Name = _T("�ݼ�");

	section = new VtProductSection();
	section->Code = _T("GC");
	section->Name = _T("��(���)");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("SI");
	section->Name = _T("��(�ǹ�)");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("HG");
	section->Name = _T("����");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("MGC");
	section->Name = _T("�ٱ�");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("QC");
	section->Name = _T("�̴ϱ���");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("QO");
	section->Name = _T("�̴ϱ�");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	CategoryList.push_back(cat);

	cat = new VtProductCategory();
	cat->Name = _T("��깰");

	section = new VtProductSection();
	section->Code = _T("ZC");
	section->Name = _T("������");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("ZS");
	section->Name = _T("���");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("ZW");
	section->Name = _T("��");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);


	section = new VtProductSection();
	section->Code = _T("ZF");
	section->Name = _T("U.S T-Notes 5��");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);


	section = new VtProductSection();
	section->Code = _T("ZL");
	section->Name = _T("��⸧");
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
	section->Name = _T("�̴Ͽ�����");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("XK");
	section->Name = _T("�̴ϴ��");
	section->ExPos = _T("CME");
	SectionMap[section->Code] = section;
	cat->AddSection(section);

	section = new VtProductSection();
	section->Code = _T("XW");
	section->Name = _T("�̴Ϲ�");
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
	cat->Name = _T("�Ϲ�");

	section = new VtProductSection();
	section->Code = _T("101F");
	section->Name = _T("KOSPI ����");
	section->UserDefinedName = _T("��������");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	section->AddSubSection(_T("101"), _T("�ڽ��� ����"));
	//section->AddSubSection(_T("401"), _T("�ڽ��� ���ռ���"));
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("101O");
	section->Name = _T("KOSPI �ɼ�");
	section->UserDefinedName = _T("�����ɼ�");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	section->AddSubSection(_T("201"), _T("�ڽ��� �ݿɼ�"));
	section->AddSubSection(_T("301"), _T("�ڽ��� ǲ����"));
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("105F");
	section->Name = _T("MKOSPI ����");
	section->UserDefinedName = _T("�̴ϼ���");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	section->AddSubSection(_T("105"), _T("�̴��ڽ��� ����"));
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("105O");
	section->Name = _T(" MKOSPI �ɼ�");
	section->UserDefinedName = _T("�̴Ͽɼ�");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	section->AddSubSection(_T("205"), _T("�̴��ڽ��� �ݿɼ�"));
	section->AddSubSection(_T("305"), _T("�̴��ڽ��� �ݿɼ�"));
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("106F");
	section->Name = _T("KOSDAQ ����");
	section->UserDefinedName = _T("�ڴڼ���");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	section->AddSubSection(_T("106"), _T("�ڽ��� ����"));
	//section->AddSubSection(_T("406"), _T("�ڽ��� ���ռ���"));
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("106O");
	section->Name = _T("KOSDAQ �ɼ�");
	section->UserDefinedName = _T("�ڴڿɼ�");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	section->AddSubSection(_T("206"), _T("�ڽ��� �ݿɼ�"));
	section->AddSubSection(_T("306"), _T("�ڽ��� ǲ�ɼ�"));
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("107F");
	section->Name = _T("EST ����");
	section->UserDefinedName = _T("���μ���");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	section->AddSubSection(_T("107"), _T("EST ����"));
	//section->AddSubSection(_T("407"), _T("EST ���ռ���"));
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("108F");
	section->Name = _T("KRX300 ����");
	section->UserDefinedName = _T("�ڻＱ��");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	section->AddSubSection(_T("108"), _T("KRX300 ����"));
	//section->AddSubSection(_T("408"), _T("KRX300 ���ռ���"));
	SectionMap[section->Code] = section;

	/*
	section = new VtProductSection();
	section->Code = _T("165F");
	section->Name = _T("BM3 ����");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("166F");
	section->Name = _T("BM5 ����");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);
	SectionMap[section->Code] = section;
	*/
	section = new VtProductSection();
	section->Code = _T("167F");
	section->Name = _T("BMA ����");
	section->UserDefinedName = _T("��ä����");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	section->AddSubSection(_T("167"), _T("10�ⱹä ����"));
	//section->AddSubSection(_T("467"), _T("10�ⱹä ���ռ���"));
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("175F");
	section->Name = _T("USD ����");
	section->UserDefinedName = _T("�޷�����");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);


	section->ProductCategoryManager(this);
	section->AddSubSection(_T("175"), _T("���޷� ����"));
	//section->AddSubSection(_T("475"), _T("���޷� ���ռ���"));
	SectionMap[section->Code] = section;
	CategoryList.push_back(cat);

	cat = new VtProductCategory();
	cat->Name = _T("�ֽļ���");

	section = new VtProductSection();
	section->Code = _T("111F");
	section->Name = _T("�Ｚ���� ����");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("112F");
	section->Name = _T("SK�ڷ��� ����");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("113F");
	section->Name = _T("POSCO ����");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("114F");
	section->Name = _T("KT ����");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("115F");
	section->Name = _T("�ѱ����� ����");
	section->ExPos = _T("SEOUL");
	cat->AddSection(section);

	section->ProductCategoryManager(this);
	SectionMap[section->Code] = section;

	section = new VtProductSection();
	section->Code = _T("116F");
	section->Name = _T("������ ����");
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
			// �����ٷ��� �ɺ� ������ ����� �ְ� �� �Ǹ� ���� �۾��� �����ϰ� �����.
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
			// �����ٷ��� �ɺ� ������ ����� �ְ� �� �Ǹ� ���� �۾��� �����ϰ� �����.
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
			// �����ٷ��� ���� ������ ����� �ְ� ���ǿ� �ֱٿ� ��ǰ ������ �������� �Ѵ�.
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
			// �����ٷ��� ���� ������ ����� �ְ� ���ǿ� �ֱٿ� ��ǰ ������ �������� �Ѵ�.
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

