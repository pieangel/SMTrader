// VtFDSubAcntPage.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtFDSubAcntPage.h"
#include "afxdialogex.h"
#include "VtAccount.h"
#include "VtSubAccountManager.h"
#include "VtSaveManager.h"
#include "VtFundDesigner.h"
#include "VtGlobal.h"
#include "VtFundManager.h"
// VtFDSubAcntPage dialog

extern TApplicationFont g_Font;

IMPLEMENT_DYNAMIC(VtFDSubAcntPage, CDialogEx)

VtFDSubAcntPage::VtFDSubAcntPage(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FUND_SUBACCOUNTPAGE, pParent)
{
	_SelAccount = nullptr;
	_FundDesigner = nullptr;
}

VtFDSubAcntPage::~VtFDSubAcntPage()
{
}

void VtFDSubAcntPage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SUB_ACNT_NAME, _EditSubAcntName);
	DDX_Control(pDX, IDC_EDIT_SUBACNT_CODE, _EditSubAcntCode);
	DDX_Control(pDX, IDC_BTN_MOD, _BtnMod);
	DDX_Control(pDX, IDC_BTN_CREATE, _BtnCreate);
}


BEGIN_MESSAGE_MAP(VtFDSubAcntPage, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_MOD, &VtFDSubAcntPage::OnBnClickedBtnMod)
	ON_BN_CLICKED(IDC_BTN_CREATE, &VtFDSubAcntPage::OnBnClickedBtnCreate)
	ON_BN_CLICKED(IDC_BTN_SAVE, &VtFDSubAcntPage::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &VtFDSubAcntPage::OnBnClickedBtnClose)
	ON_BN_CLICKED(IDC_BTN_OK, &VtFDSubAcntPage::OnBnClickedBtnOk)
	ON_BN_CLICKED(IDC_BTN_DEL, &VtFDSubAcntPage::OnBnClickedBtnDel)
END_MESSAGE_MAP()


// VtFDSubAcntPage message handlers


BOOL VtFDSubAcntPage::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	::EnumChildWindows(m_hWnd, ::SetChildFont, (LPARAM)g_Font.GetFont());

	// TODO:  Add extra initialization here
	_AcntGrid.AttachGrid(this, IDC_STATIC_SUBACNT_SOURCE);
	_SubAcntGrid.AttachGrid(this, IDC_STATIC_SUBACNT_TARGET);

	_AcntGrid.InitGrid();
	_AcntGrid.SubAcntPage(this);
	_SubAcntGrid.SubAcntPage(this);
	_SelAccount = _AcntGrid.GetSelectedAccount();
	_SubAcntGrid.InitGrid(_SelAccount);
	SelSubAccount(_SubAcntGrid.GetSelectedAccount());
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void VtFDSubAcntPage::OnBnClickedBtnMod()
{
	CString strAcntNo, strAcntName;
	_EditSubAcntCode.GetWindowText(strAcntNo);
	_EditSubAcntName.GetWindowText(strAcntName);

	if (_SelAccount) {
		VtAccount* subAcnt = _SubAcntGrid.GetSelectedAccount();
		if (!subAcnt)
			return;

		VtAccount*  anotherSubAcnt = _SelAccount->FindSubAccount((LPCTSTR)strAcntNo, subAcnt->AccountNo);
		if (anotherSubAcnt) {
			AfxMessageBox(_T("동일한 이름의 서브 계좌가 이미 있습니다. 다른 이름을 사용하십시오.!"));
			return;
		}
		subAcnt->AccountNo = (LPCTSTR)strAcntNo;
		subAcnt->AccountName = (LPCTSTR)strAcntName;
		_SubAcntGrid.InitGrid(_SelAccount);
	}
}


void VtFDSubAcntPage::OnBnClickedBtnCreate()
{
	// TODO: Add your control notification handler code here
	CString strAcntNo, strAcntName;
	_EditSubAcntCode.GetWindowText(strAcntNo);
	_EditSubAcntName.GetWindowText(strAcntName);

	if (_SelAccount)
	{
		VtAccount* found = _SelAccount->FindSubAccount((LPCTSTR)strAcntNo);
		if (found)
		{
			AfxMessageBox(_T("동일한 이름의 서브 계좌가 이미 있습니다. 다른 이름을 사용하십시오.!"));
			return;
		}
		VtAccount* subAcnt = _SelAccount->CreateSubAccount((LPCTSTR)strAcntNo, (LPCTSTR)strAcntName, _SelAccount->GetSubAccountCount() <= 0 ? true : false);
		subAcnt->ParentAccount(_SelAccount);
		VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
		subAcntMgr->FindAddAccount(subAcnt);
		_SubAcntGrid.InitGrid(_SelAccount);
	}
}


void VtFDSubAcntPage::OnBnClickedBtnSave()
{
	VtSaveManager* saveMgr = VtSaveManager::GetInstance();
	saveMgr->SaveAccountList(_T("accountlist.dat"));
}


void VtFDSubAcntPage::OnBnClickedBtnClose()
{
	if (!_FundDesigner)
		return;

	_FundDesigner->EndDialog(IDOK);
}


void VtFDSubAcntPage::OnBnClickedBtnOk()
{
	if (!_FundDesigner)
		return;

	_FundDesigner->EndDialog(IDOK);
}

void VtFDSubAcntPage::RefreshSubAccountList(VtAccount* acnt)
{
	_SubAcntGrid.InitGrid(acnt);
}

void VtFDSubAcntPage::SelAccount(VtAccount* acnt)
{
	if (!acnt) {
		_EditSubAcntCode.SetWindowText(_T(""));
		_EditSubAcntName.SetWindowText(_T(""));
		return;
	}
	_SelAccount = acnt;
	if (acnt->GetSubAccountCount() == 0) {
		CString strAcntNo, strAcntName;
		strAcntNo = acnt->AccountNo.c_str();
		strAcntNo.Append(_T("_1"));
		strAcntName = acnt->AccountName.c_str();
		strAcntName.Append(_T("_1"));
		_EditSubAcntCode.SetWindowText(strAcntNo);
		_EditSubAcntName.SetWindowText(strAcntName);
		_SubAcntGrid.ClearCells();
		return;
	}
	_SubAcntGrid.InitGrid(acnt);
	VtAccount* subAcnt = _SubAcntGrid.GetSelectedAccount();
	if (subAcnt)
		SelSubAccount(subAcnt);
	else
		_SubAcntGrid.SetDefaultSelect();
}

void VtFDSubAcntPage::SelSubAccount(VtAccount* acnt)
{
	if (!acnt)
		return;
	_EditSubAcntCode.SetWindowText(acnt->AccountNo.c_str());
	_EditSubAcntName.SetWindowText(acnt->AccountName.c_str());
	if (acnt->Prime() && _BtnMod.GetSafeHwnd())
	{
		_BtnMod.EnableWindow(FALSE);
	}
	else
	{
		_BtnMod.EnableWindow(TRUE);
	}
}


void VtFDSubAcntPage::OnBnClickedBtnDel()
{
	if (!_SelAccount)
		return;
	if (_SelAccount->GetSubAccountCount() == 1) { // 하나일 때는 삭제하지 못한다.
		return;
	}
	VtAccount* subAcnt = _SubAcntGrid.GetSelectedAccount();
	if (!subAcnt)
		return;

	VtFundManager* fundMgr = VtFundManager::GetInstance();
	VtAccount* foundAcnt = fundMgr->FindFromUsedAccountList(subAcnt->AccountNo);
	if (foundAcnt) { 
		AfxMessageBox(_T("현재 펀드에 사용중이므로 삭제할 수 없습니다."));
		return;
	}

	_SelAccount->RemoveSubAccount(subAcnt->AccountNo);
	_SubAcntGrid.InitGrid(_SelAccount);
}
