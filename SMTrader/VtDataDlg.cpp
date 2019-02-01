// VtDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VtDataDlg.h"
#include "afxdialogex.h"
#include "resource.h"

// VtDataDlg dialog

IMPLEMENT_DYNAMIC(VtDataDlg, CDialogEx)

VtDataDlg::VtDataDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DATA_COL, pParent)
{

}

VtDataDlg::~VtDataDlg()
{
	KillTimer(RegularTimerID);
	KillTimer(MinTimerID);
}

void VtDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(VtDataDlg, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_START, &VtDataDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_END, &VtDataDlg::OnBnClickedBtnEnd)
END_MESSAGE_MAP()

void VtDataDlg::OnTimer(UINT_PTR nIDEvent)
{
	int localTime = VtChartDataCollector::GetLocalTime();
	if (nIDEvent == RegularTimerID) {
		_DataCollector.SetChartData(_ChartData, localTime, localTime);
	}
	else if (nIDEvent == MinTimerID) {
		_DataCollector.OnTimer(localTime, _ChartData);
	}
	CDialogEx::OnTimer(nIDEvent);
}


BOOL VtDataDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	_ChartData = _DataCollector.FindAddChartData(_T("Áö¼öÃÑÈ£°¡"));
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void VtDataDlg::OnBnClickedBtnStart()
{
	// TODO: Add your control notification handler code here
	SetTimer(RegularTimerID, 100, nullptr);
	SetTimer(MinTimerID, 1000, nullptr);
}


void VtDataDlg::OnBnClickedBtnEnd()
{
	KillTimer(RegularTimerID);
	KillTimer(MinTimerID);
}

