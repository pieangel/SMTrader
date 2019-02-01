#include "stdafx.h"
#include "VtOrderConfigManager.h"
#include "VtOrderManager.h"
#include "VtOrderManagerSelector.h"
#include "VtAccount.h"
#include "VtSymbol.h"
#include "VtProductOrderManagerSelector.h"
#include "VtProductOrderManager.h"
#include "VtProductCategoryManager.h"
#include "VtProductCategory.h"
#include "VtProductSection.h"
#include "VtProductCategoryManager.h"
#include "VtProductSection.h"
#include "VtProductSubSection.h"
#include <string>
#include "VtOrderDialogManager.h"
#include "VtRealtimeRegisterManager.h"
#include "VtGlobal.h"
#include "VtAccountManager.h"
#include "VtFundOrderManager.h"
#include "VtFundOrderManagerSelecter.h"
#include "VtFund.h"

VtOrderConfigManager::VtOrderConfigManager()
{
	_Fund = nullptr;
	_Account = nullptr;
	_Symbol = nullptr;
	_Hoga = nullptr;
	_Quote = nullptr;
	_Master = nullptr;
	_Position = nullptr;
	_OrderMgr = nullptr;
	_OldAccount = nullptr;
	_OldFund = nullptr;
}


VtOrderConfigManager::~VtOrderConfigManager()
{
}

void VtOrderConfigManager::Account(VtAccount* val)
{
	_OldAccount = _Account;
	_Account = val;
	VtOrderManagerSelector* orderMgrSelector = VtOrderManagerSelector::GetInstance();
	_OrderMgr = orderMgrSelector->FindAddOrderManager(_Account->AccountNo);
}

void VtOrderConfigManager::Symbol(VtSymbol* val)
{
	_Symbol = val;
	if (_OrderMgr)
	{
		_PrdtOrderMgr = _OrderMgr->FindAddProductOrderManager(_Symbol->FullCode);
		_PrdtOrderMgr->OrderMgr(_OrderMgr);
		_PrdtOrderMgr->Account(_Account);
	}
}

void VtOrderConfigManager::RegisterRealtimeSymbol()
{
	if (!_Symbol || !_HdOrderWnd)
		return;

	VtOrderDialogManager* oldDlgMgr = VtOrderDialogManager::GetInstance();
	oldDlgMgr->AddSymbolWindow(_Symbol->ShortCode, _HdOrderWnd);

	VtRealtimeRegisterManager* realtimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	realtimeRegiMgr->RegisterProduct(_Symbol->ShortCode);
}

void VtOrderConfigManager::UnregisterRealtimeSymbol()
{
	if (!_Symbol || !_HdOrderWnd)
		return;
	VtRealtimeRegisterManager* realtimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	realtimeRegiMgr->UnregisterProduct(_Symbol->ShortCode);

	VtOrderDialogManager* oldDlgMgr = VtOrderDialogManager::GetInstance();
	oldDlgMgr->RemoveSymbolWindow(_Symbol->ShortCode, _HdOrderWnd);
}

void VtOrderConfigManager::Fund(VtFund* val)
{
	if (!val)
		return;

	_OldFund = _Fund;
	_Fund = val;

	VtFundOrderManagerSelecter* orderMgrSelector = VtFundOrderManagerSelecter::GetInstance();
	_OrderMgr = orderMgrSelector->FindAddOrderManager(_Fund->Name);
	((VtFundOrderManager*)_OrderMgr)->UpdateOrderManagers();
}

void VtOrderConfigManager::Reset()
{
	_Account = nullptr;
	_Symbol = nullptr;
	_Quote = nullptr;
	_Master = nullptr;
	_Hoga = nullptr;
	_Position = nullptr;
}

void VtOrderConfigManager::SetDefaultSymbol()
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	std::string secName = prdtCatMgr->MainFutureVector.front();
	VtProductSection* section = prdtCatMgr->FindProductSection(secName);
	if (section)
	{

		if (section->SubSectionVector.size() > 0)
		{
			VtProductSubSection* subSection = section->SubSectionVector.front();
			if (subSection->_SymbolVector.size() > 0)
			{
				VtSymbol* sym = subSection->_SymbolVector.front();
				_Symbol = sym;
			}
		}
	}
}

void VtOrderConfigManager::SetDefaultAccount()
{
	VtGlobal* global = VtGlobal::GetInstance();
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	int i = 0;
	for (auto it = acntMgr->AccountMap.begin(); it != acntMgr->AccountMap.end(); ++it) {
		VtAccount* acnt = it->second;
		if (i == 0) {
			_Account = acnt;
			_OldAccount = _Account;
			break;
		}
	}
}

