// VtAccountPasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtAccountPasswordDlg.h"
#include "afxdialogex.h"
#include "VtAccountManager.h"
#include "VtAccount.h"
#include "VtSaveManager.h"
#include "ZmConfigManager.h"
#include <vector>
#include <string>

// VtAccountPasswordDlg dialog

IMPLEMENT_DYNAMIC(VtAccountPasswordDlg, CDialogEx)

VtAccountPasswordDlg::VtAccountPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ACCOUNT_PWD, pParent)
{

}

VtAccountPasswordDlg::~VtAccountPasswordDlg()
{
}

void VtAccountPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_SHOW_PWD, _CheckShowPwd);
}


BEGIN_MESSAGE_MAP(VtAccountPasswordDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK_SHOW_PWD, &VtAccountPasswordDlg::OnBnClickedCheckShowPwd)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &VtAccountPasswordDlg::OnBnClickedBtnClose)
	ON_BN_CLICKED(IDC_BTN_SAVE, &VtAccountPasswordDlg::OnBnClickedBtnSave)
END_MESSAGE_MAP()


// VtAccountPasswordDlg message handlers


void VtAccountPasswordDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}


BOOL VtAccountPasswordDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	this->SetIcon(hIcon, FALSE);

	// TODO:  Add extra initialization here
	_PasswordGrid.AttachGrid(this, IDC_STATIC_ACNT_PWD);
	_CheckShowPwd.SetCheck(BST_UNCHECKED);
	CenterWindow();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void VtAccountPasswordDlg::FromServer(bool val)
{
	_PasswordGrid.FromServer(val);
}

void VtAccountPasswordDlg::OnBnClickedCheckShowPwd()
{
	// TODO: Add your control notification handler code here
	if (_CheckShowPwd.GetCheck() == BST_CHECKED)
	{
		_PasswordGrid.ShowPassword(true);
	}
	else
	{
		_PasswordGrid.ShowPassword(false);
	}
}


void VtAccountPasswordDlg::OnBnClickedBtnClose()
{
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	std::vector<std::string> empty_list = acntMgr->GetEmptyPasswordAccountList();
	if (empty_list.size() > 0) {
		CString msg;
		msg.Append(_T("아래 계좌에 비밀번호가 정상적으로 입력되지 않았습니다.\n아래 계좌는 주문이 정상적으로 이루어지지 않습니다. \n정상적인 주문을 원하면 비밀번호를 입력하십시오.\n\n"));
		for (auto it = empty_list.begin(); it != empty_list.end(); ++it) {
			msg.Append(_T("계좌번호 : "));
			msg.Append((*it).c_str());
			msg.Append(_T("\n"));
		}
		msg.Append(_T("\n이대로 진행하시겠습니까?"));
		int Ret = AfxMessageBox(msg, MB_YESNO);
		if (Ret == IDNO)
			return;
	}

	EndDialog(IDOK);
}


void VtAccountPasswordDlg::OnBnClickedBtnSave()
{
	_PasswordGrid.SavePassword();

	VtSaveManager* saveMgr = VtSaveManager::GetInstance();
	saveMgr->SaveAccountList();

	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string section = _T("ACCOUNT_INFO");
	std::string name, value;
	name = _T("savepwd");
	value = _T("true");
	configMgr->setString(section, name, value);
}
