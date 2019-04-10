// VtSystemLogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtSystemLogDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"

// VtSystemLogDlg dialog

IMPLEMENT_DYNAMIC(VtSystemLogDlg, CDialogEx)

VtSystemLogDlg::VtSystemLogDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SYS_LOG, pParent)
{

}

VtSystemLogDlg::~VtSystemLogDlg()
{
}

void VtSystemLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(VtSystemLogDlg, CDialogEx)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// VtSystemLogDlg message handlers


void VtSystemLogDlg::RefreshGrid()
{
	_SysLogGrid.UpdateLog();
}

BOOL VtSystemLogDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	_SysLogGrid.AttachGrid(this, IDC_STATIC_LOG);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void VtSystemLogDlg::PostNcDestroy()
{
	delete this;
	CDialogEx::PostNcDestroy();
}


void VtSystemLogDlg::OnClose()
{
	CDialogEx::OnClose();
}
