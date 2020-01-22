// VtOrderLogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtOrderLogDlg.h"
#include "afxdialogex.h"
#include "VtStringUtil.h"

// VtOrderLogDlg dialog

IMPLEMENT_DYNAMIC(VtOrderLogDlg, CDialogEx)

VtOrderLogDlg::VtOrderLogDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ORDER_LOG, pParent)
{

}

VtOrderLogDlg::~VtOrderLogDlg()
{
}

void VtOrderLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(VtOrderLogDlg, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// VtOrderLogDlg message handlers


BOOL VtOrderLogDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	_SysLogGrid.Mode(1);
	_SysLogGrid.RowCount(100);
	_SysLogGrid.AttachGrid(this, IDC_STATIC_GRID);

	Resize();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void VtOrderLogDlg::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();
	delete this;
}

void VtOrderLogDlg::UpdateOrderLog(std::vector<std::pair<std::string, std::string>> log_vec)
{
	/*
	for (int i = 0; i < 20; i++) {
		std::string now_time = VtStringUtil::NowTime();
		std::string msg = "test message";
		log_vec.push_back(std::make_pair(now_time, msg));
	}
	*/
	_SysLogGrid.UpdateOrderLog(log_vec);
}

void VtOrderLogDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	this->Resize();
}

void VtOrderLogDlg::Resize()
{
	if (!GetSafeHwnd() || !_SysLogGrid.GetSafeHwnd())
		return;

	CRect rcClient;
	GetClientRect(rcClient);
	_SysLogGrid.Resize(rcClient);
	_SysLogGrid.MoveWindow(rcClient);
}
