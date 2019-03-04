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
	ON_BN_CLICKED(IDC_BTN_DEL_CONNECT, &VtAutoSignalManagerDialog::OnBnClickedBtnDelConnect)
	ON_BN_CLICKED(IDC_BTN_DEL_SIGNAL, &VtAutoSignalManagerDialog::OnBnClickedBtnDelSignal)
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// VtAutoSignalManagerDialog message handlers


BOOL VtAutoSignalManagerDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	_TotalSigGrid.AttachGrid(this, IDC_STATIC_TOTAL_SIGNAL);
	_ConnectGrid.AttachGrid(this, IDC_STATIC_SIGNAL_CONNECTION);
	_DefineGrid.AttachGrid(this, IDC_STATIC_SIGNAL_DEFINITION);
	_ConnectGrid.TotalGrid(&_TotalSigGrid);
	SetTimer(RefTimer, 100, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void VtAutoSignalManagerDialog::RefreshOrder()
{
	_TotalSigGrid.RefreshOrders();
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


void VtAutoSignalManagerDialog::OnBnClickedBtnDelConnect()
{
	_ConnectGrid.RemoveSystem();
	_ConnectGrid.Refresh();
}


void VtAutoSignalManagerDialog::OnBnClickedBtnDelSignal()
{
	// TODO: Add your control notification handler code here
}


void VtAutoSignalManagerDialog::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == RefTimer) {
		RefreshOrder();
	}
	CDialogEx::OnTimer(nIDEvent);
}


void VtAutoSignalManagerDialog::OnClose()
{
	KillTimer(RefTimer);
	CDialogEx::OnClose();
}
