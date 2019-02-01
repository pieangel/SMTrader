// VtAssetPage.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtAssetPage.h"
#include "afxdialogex.h"
#include "VtAccountManager.h"
#include "VtAccount.h"
#include "VtGlobal.h"

// VtAssetPage dialog

IMPLEMENT_DYNAMIC(VtAssetPage, CDialogEx)

VtAssetPage::VtAssetPage(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ASSET, pParent)
{
	_Account = nullptr;
}

VtAssetPage::~VtAssetPage()
{
}

void VtAssetPage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ACCOUNT, _ComboAccount);
}


BEGIN_MESSAGE_MAP(VtAssetPage, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SEARCH, &VtAssetPage::OnBnClickedBtnSearch)
	ON_CBN_SELCHANGE(IDC_COMBO_ACCOUNT, &VtAssetPage::OnCbnSelchangeComboAccount)
END_MESSAGE_MAP()


// VtAssetPage message handlers


void VtAssetPage::InitAccount()
{
	/*
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	for (auto it = acntMgr->AccountMap.begin(); it != acntMgr->AccountMap.end(); ++it)
	{
		VtAccount* acnt = it->second;
		int index = _ComboAccount.AddString(acnt->AccountNo.c_str());
		_ComboAccount.SetItemDataPtr(index, acnt);
	}

	if (_ComboAccount.GetCount() > 2)
	{
		_ComboAccount.SetCurSel(2);
		_Account = (VtAccount*)_ComboAccount.GetItemDataPtr(2);
	}
	*/

	VtGlobal* global = VtGlobal::GetInstance();
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	int selAcnt = 0;
	std::string acntName;
	for (auto it = acntMgr->AccountMap.begin(); it != acntMgr->AccountMap.end(); ++it) {
		VtAccount* acnt = it->second;
		acntName = acnt->AccountNo.c_str();
		acntName.append(_T(" "));
		acntName.append(acnt->AccountName);
		int index = _ComboAccount.AddString(acntName.c_str());
		_ComboAccount.SetItemDataPtr(index, acnt);
		if (index == 0)
		{
			selAcnt = index;
		}
	}

	if (selAcnt != -1) {
		_ComboAccount.SetCurSel(selAcnt);
		_Account = (VtAccount*)_ComboAccount.GetItemDataPtr(selAcnt);
		_Account->GetAccountInfoNFee(1);
	}

	_ComboAccount.SetDroppedWidth(210);
}

BOOL VtAssetPage::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	InitAccount();
	_AssetGrid.AssetPage(this);
	// TODO:  Add extra initialization here
	_AssetGrid.AttachGrid(this, IDC_STATIC_ASSET);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void VtAssetPage::OnBnClickedBtnSearch()
{
	// TODO: Add your control notification handler code here
	if (_Account)
		_Account->GetAccountInfoNFee(1);
}


void VtAssetPage::OnCbnSelchangeComboAccount()
{
	// TODO: Add your control notification handler code here
	int curSel = _ComboAccount.GetCurSel();
	if (curSel != -1)
	{
		VtAccount* acnt = (VtAccount*)_ComboAccount.GetItemDataPtr(curSel);
		acnt->GetAccountInfoNFee(1);
		_Account = acnt;
	}
}

void VtAssetPage::OnReceiveAccountInfo()
{
	_AssetGrid.InitGrid();
}
