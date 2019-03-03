// VtAutoSignalManagerDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtAutoSignalManagerDialog.h"
#include "afxdialogex.h"
#include "VtAddConnectSignalDlg.h"
#include "VtAddOutSigDefDlg.h"

// VtAutoSignalManagerDialog dialog

IMPLEMENT_DYNAMIC(VtAutoSignalManagerDialog, CDialogEx)

VtAutoSignalManagerDialog::VtAutoSignalManagerDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SYS_AUTO_CONNECT, pParent)
{

}

VtAutoSignalManagerDialog::~VtAutoSignalManagerDialog()
{
}

void VtAutoSignalManagerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


void VtAutoSignalManagerDialog::PostNcDestroy()
{
	delete this;
	CDialogEx::PostNcDestroy();
}

BEGIN_MESSAGE_MAP(VtAutoSignalManagerDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_ADD_CONNECT, &VtAutoSignalManagerDialog::OnBnClickedBtnAddConnect)
	ON_BN_CLICKED(IDC_BTN_ADD_SIGNAL, &VtAutoSignalManagerDialog::OnBnClickedBtnAddSignal)
END_MESSAGE_MAP()


// VtAutoSignalManagerDialog message handlers


BOOL VtAutoSignalManagerDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	_TotalSigGrid.AttachGrid(this, IDC_STATIC_TOTAL_SIGNAL);
	_ConnectGrid.AttachGrid(this, IDC_STATIC_SIGNAL_CONNECTION);
	_DefineGrid.AttachGrid(this, IDC_STATIC_SIGNAL_DEFINITION);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void VtAutoSignalManagerDialog::OnBnClickedBtnAddConnect()
{
	VtAddConnectSignalDlg dlg;
	dlg.SigConGrid(&_ConnectGrid);
	dlg.DoModal();
}


void VtAutoSignalManagerDialog::OnBnClickedBtnAddSignal()
{
	VtAddOutSigDefDlg dlg;
	dlg.SigDefGrid(&_DefineGrid);
	dlg.DoModal();
}
