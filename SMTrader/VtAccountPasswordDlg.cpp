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
	// TODO: Add your control notification handler code here
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
