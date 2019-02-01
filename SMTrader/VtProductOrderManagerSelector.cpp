#include "stdafx.h"
#include "VtProductOrderManagerSelector.h"
#include "VtProductOrderManager.h"
#include "VtSymbolManager.h"
#include "VtSymbolMaster.h"


VtProductOrderManagerSelector::VtProductOrderManagerSelector()
{
}


VtProductOrderManagerSelector::~VtProductOrderManagerSelector()
{
	for (auto it = _OrderManagerMap.begin(); it != _OrderManagerMap.end(); ++it)
	{
		delete it->second;
	}
}

VtProductOrderManager* VtProductOrderManagerSelector::FindAdd(std::string symbolCode)
{
	auto it = _OrderManagerMap.find(symbolCode);
	if (it != _OrderManagerMap.end())
	{
		VtProductOrderManager* orderMgr = it->second;
		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		VtSymbolMaster* symMaster = symMgr->FindSymbolMasterByFullCode(symbolCode);
		orderMgr->SymbolMaster(symMaster);
		return orderMgr;
	}
	else
	{
		VtProductOrderManager* orderMgr = new VtProductOrderManager();
		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		VtSymbolMaster* symMaster = symMgr->FindSymbolMasterByFullCode(symbolCode);
		orderMgr->ProductCode(symbolCode);
		orderMgr->SymbolMaster(symMaster);
		_OrderManagerMap[symbolCode] = orderMgr;
		return orderMgr;
	}
}

VtProductOrderManager* VtProductOrderManagerSelector::Find(std::string symbolCode)
{
	auto it = _OrderManagerMap.find(symbolCode);
	if (it != _OrderManagerMap.end())
	{
		VtProductOrderManager* orderMgr = it->second;
		return orderMgr;
	}

	return nullptr;
}

void VtProductOrderManagerSelector::ClearAllAccepted()
{
	for (auto it = _OrderManagerMap.begin(); it != _OrderManagerMap.end(); ++it)
	{
		VtProductOrderManager* prdtOrderMgr = it->second;
		prdtOrderMgr->ClearAccepted();
	}
}

void VtProductOrderManagerSelector::ClearAllFilled()
{
	for (auto it = _OrderManagerMap.begin(); it != _OrderManagerMap.end(); ++it)
	{
		VtProductOrderManager* prdtOrderMgr = it->second;
		prdtOrderMgr->ClearFilled();
	}
}

void VtProductOrderManagerSelector::ClearAccepted(VtProductOrderManager* prdtOrderMgr)
{
	if (!prdtOrderMgr)
		return;
	prdtOrderMgr->ClearAccepted();
}

void VtProductOrderManagerSelector::ClearFilled(VtProductOrderManager* prdtOrderMgr)
{
	if (!prdtOrderMgr)
		return;
	prdtOrderMgr->ClearFilled();
}

bool VtProductOrderManagerSelector::GetInit()
{
	for (auto it = _OrderManagerMap.begin(); it != _OrderManagerMap.end(); ++it)
	{
		VtProductOrderManager* prdtOrderMgr = it->second;
		if (prdtOrderMgr->Init())
			return true;
	}

	return false;
}

void VtProductOrderManagerSelector::Save(simple::file_ostream<same_endian_type>& ss)
{
	// Save the count of the product order manager.
	ss << (int)_OrderManagerMap.size();

	// Save the product order manager each.
	for (auto it = _OrderManagerMap.begin(); it != _OrderManagerMap.end(); ++it)
	{
		VtProductOrderManager* prdtOrderMgr = it->second;
		ss << prdtOrderMgr->ProductCode();
		prdtOrderMgr->Save(ss);
	}
}

void VtProductOrderManagerSelector::Load(simple::file_istream<same_endian_type>& ss)
{
	int count = 0;
	ss >> count;
	std::string productCode;
	for (int i = 0; i < count; ++i)
	{
		ss >> productCode;
		VtProductOrderManager* prdtOrderMgr = FindAdd(productCode);
		prdtOrderMgr->Load(ss);
	}
}
