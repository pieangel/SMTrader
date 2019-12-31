#include "stdafx.h"
#include "VtProductCategory.h"
#include "HdTaskArg.h"
#include "HdScheduler.h"
#include "VtSymbol.h"
#include "VtProductSubSection.h"
#include "VtGlobal.h"
#include "resource.h"
#include "VtProgressDlg.h"

VtProductCategory::VtProductCategory()
{
}


VtProductCategory::~VtProductCategory()
{
	End();
}

void VtProductCategory::AddSection(VtProductSection* section)
{
	SectionList.push_back(section);
}

void VtProductCategory::GetSymbolCode()
{
	//VtGlobal* global = VtGlobal::GetInstance();
	//global->CloseWaitDialog();


	//CString msg;
	//msg.LoadString(IDS_BUSYTEXT_DATA);
	//global->OpenWaitDialog(msg);
	
	//VtProgressDlg dlg;
	//dlg.DoModal();

	HdScheduler* scheduler = HdScheduler::GetInstance();
	for (auto i = SectionList.begin(); i != SectionList.end(); ++i)
	{
		VtProductSection* prdtSec = *i;
		HdTaskArg arg;
		arg.Type = HdTaskType::HdSymbolCode;
		arg.AddArg(_T("Category"), prdtSec->Code);
		scheduler->AddTask(std::move(arg));
	}
}

void VtProductCategory::GetSymbolCode(std::vector<std::pair<std::string, HdTaskArg>>& argList)
{
	HdScheduler* scheduler = HdScheduler::GetInstance();
	for (auto i = SectionList.begin(); i != SectionList.end(); ++i)
	{
		VtProductSection* prdtSec = *i;
		HdTaskArg arg;
		arg.Type = HdTaskType::HdSymbolCode;
		arg.AddArg(_T("Category"), prdtSec->Code);
		//scheduler->AddTask(std::move(arg));
		argList.push_back(std::make_pair(prdtSec->Name, arg));
	}
}

void VtProductCategory::GetRecentMonthSymbolMaster()
{
	HdScheduler* scheduler = HdScheduler::GetInstance();
	int r = 0;
	for (auto i = SectionList.begin(); i != SectionList.end(); ++i) {
		VtProductSection* prdtSec = *i;
		for (auto it = prdtSec->SubSectionVector.begin(); it != prdtSec->SubSectionVector.end(); ++it) {
			VtProductSubSection* subSection = *it;
			if (subSection->_SymbolVector.size() > 0) {
				scheduler->AddRequest(HdTaskType::HdSymbolMaster, r++);
			}
		}
	}
	r = 0;
	for (auto i = SectionList.begin(); i != SectionList.end(); ++i) {
		VtProductSection* prdtSec = *i;
		for (auto it = prdtSec->SubSectionVector.begin(); it != prdtSec->SubSectionVector.end(); ++it) {
			VtProductSubSection* subSection = *it;
			if (subSection->_SymbolVector.size() > 0) {
				
				VtSymbol* sym = subSection->_SymbolVector.front();
				//VtSymbol* sym = subSection->_SymbolVector[1];
				HdTaskArg arg;
				arg.Type = HdTaskType::HdSymbolMaster;
				arg.AddArg(_T("SymbolCode"), sym->ShortCode);
				scheduler->AddTask(std::move(arg));
				Sleep(VtGlobal::ServerSleepTime);

				LOG_F(INFO, _T("section = %s, req count = %d"), subSection->Name.c_str(), r++);
				
				/*
				for (auto iit = subSection->_SymbolVector.begin(); iit != subSection->_SymbolVector.end(); ++iit)
				{
					VtSymbol* sym = *iit; 
					HdTaskArg arg;
					arg.Type = HdTaskType::HdSymbolMaster;
					arg.AddArg(_T("SymbolCode"), sym->ShortCode);
					scheduler->AddTask(std::move(arg));
					Sleep(ServerSleepTime);
				}
				*/
			}
		}
	}
}

void VtProductCategory::GetRecentMonthSymbolMaster(std::vector<std::pair<std::string, HdTaskArg>>& argList)
{
	HdScheduler* scheduler = HdScheduler::GetInstance();
	int r = 0;
	for (auto i = SectionList.begin(); i != SectionList.end(); ++i) {
		VtProductSection* prdtSec = *i;
		for (auto it = prdtSec->SubSectionVector.begin(); it != prdtSec->SubSectionVector.end(); ++it) {
			VtProductSubSection* subSection = *it;
			if (subSection->_SymbolVector.size() > 0) {
				scheduler->AddRequest(HdTaskType::HdSymbolMaster, r++);
			}
		}
	}
	r = 0;
	for (auto i = SectionList.begin(); i != SectionList.end(); ++i) {
		VtProductSection* prdtSec = *i;
		for (auto it = prdtSec->SubSectionVector.begin(); it != prdtSec->SubSectionVector.end(); ++it) {
			VtProductSubSection* subSection = *it;
			if (subSection->_SymbolVector.size() > 0) {
				VtSymbol* sym = subSection->_SymbolVector.front();
				HdTaskArg arg;
				arg.Type = HdTaskType::HdSymbolMaster;
				arg.AddArg(_T("SymbolCode"), sym->ShortCode);
				argList.push_back(std::make_pair(subSection->Name, arg));

				LOG_F(INFO, _T("section = %s, req count = %d"), subSection->Name.c_str(), r++);
			}
		}
	}
}

void VtProductCategory::Begin()
{
	
}

void VtProductCategory::End()
{
	for (auto i = SectionList.begin(); i != SectionList.end(); ++i)
	{
		delete *i;
	}
}

