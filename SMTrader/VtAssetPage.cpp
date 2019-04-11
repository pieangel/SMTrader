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

void VtAssetPage::SetAccount(std::string acntNo)
{
	auto it = _ComboMap.find(acntNo);
	if (it != _ComboMap.end()) {
		_ComboAccount.SetCurSel(it->second.first);
		_Account = it->second.second;
	}
}

void VtAssetPage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ACCOUNT, _ComboAccount);
	DDX_Control(pDX, IDC_BTN_COMMIT, _BtnCommit);
	DDX_Control(pDX, IDC_BTN_SEARCH, _BtnSearch);
	DDX_Control(pDX, IDC_EDIT_PWD, _EditPwd);
}


BEGIN_MESSAGE_MAP(VtAssetPage, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SEARCH, &VtAssetPage::OnBnClickedBtnSearch)
	ON_CBN_SELCHANGE(IDC_COMBO_ACCOUNT, &VtAssetPage::OnCbnSelchangeComboAccount)
	ON_BN_CLICKED(IDC_BTN_COMMIT, &VtAssetPage::OnBnClickedBtnCommit)
END_MESSAGE_MAP()


// VtAssetPage message handlers


void VtAssetPage::InitAccount()
{
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
		_ComboMap[acnt->AccountNo] = std::make_pair(index, acnt);
		_ComboAccount.SetItemDataPtr(index, acnt);
		if (index == 0)
		{
			selAcnt = index;
		}
	}

	if (selAcnt != -1) {
		_ComboAccount.SetCurSel(selAcnt);
		_Account = (VtAccount*)_ComboAccount.GetItemDataPtr(selAcnt);
		if (_Account->hasValidPassword()) {
			_Account->GetAccountProfitLossDirect();
			CString pwd;
			pwd.Format(_T("%s"), _Account->Password.c_str());
			_EditPwd.SetWindowText(pwd);
		}
	}

	_ComboAccount.SetDroppedWidth(210);
}

BOOL VtAssetPage::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	_BtnCommit.SetShade(SHS_HSHADE, 8, 20, 5, RGB(55, 55, 255));
	_BtnCommit.DrawFlatFocus(TRUE);

	_BtnSearch.SetShade(SHS_HSHADE, 8, 20, 5, RGB(55, 55, 255));
	_BtnSearch.DrawFlatFocus(TRUE);

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
	if (_Account) {
		if (_Account->hasValidPassword())
			_Account->GetAccountProfitLossDirect();
	}
}


void VtAssetPage::OnCbnSelchangeComboAccount()
{
	// TODO: Add your control notification handler code here
	int curSel = _ComboAccount.GetCurSel();
	if (curSel != -1)
	{
		VtAccount* acnt = (VtAccount*)_ComboAccount.GetItemDataPtr(curSel);
		if (acnt->hasValidPassword())
			acnt->GetAccountProfitLossDirect();
		_Account = acnt;
	}
}

void VtAssetPage::OnReceiveAccountInfo()
{
	_AssetGrid.InitGrid();
}


void VtAssetPage::OnBnClickedBtnCommit()
{
	if (_Account && _Account->hasValidPassword())
		_Account->GetAsset();
}
