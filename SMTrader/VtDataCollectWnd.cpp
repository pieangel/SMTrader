// VtDataCollectWnd.cpp : implementation file
//

#include "stdafx.h"
#include "VtDataCollectWnd.h"
#include "afxdialogex.h"
#include "resource.h"
#include <string>

// VtDataCollectWnd dialog

IMPLEMENT_DYNAMIC(VtDataCollectWnd, CDialogEx)


VtDataCollectWnd::VtDataCollectWnd(CWnd* pParent )
	: CDialogEx(IDD_DATA_COL, pParent)
{
	_ChartData = nullptr;
}

VtDataCollectWnd::~VtDataCollectWnd()
{
}

void VtDataCollectWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(VtDataCollectWnd, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_START, &VtDataCollectWnd::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_END, &VtDataCollectWnd::OnBnClickedBtnEnd)
END_MESSAGE_MAP()


// VtDataCollectWnd message handlers


void VtDataCollectWnd::OnTimer(UINT_PTR nIDEvent)
{
	/* initialize random seed: */
	srand((unsigned int)time(0));

	/* generate secret number between 1 and 10: */
	int iSecret = rand() % 256;

	int localTime = VtChartDataCollector::GetLocalTime();
	if (nIDEvent == RegularTimerID) {
		_DataCollector.SetChartData(_ChartData, localTime, iSecret);
	}
	else if (nIDEvent == MinTimerID) {
		_DataCollector.OnTimer(localTime, _ChartData);
	}
	CDialogEx::OnTimer(nIDEvent);
}


BOOL VtDataCollectWnd::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	_ChartData = _DataCollector.FindAddChartData(_T("Áö¼öÃÑÈ£°¡"));
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void VtDataCollectWnd::OnBnClickedBtnStart()
{
	// TODO: Add your control notification handler code here
	SetTimer(RegularTimerID, 100, nullptr);
	SetTimer(MinTimerID, 1000, nullptr);
}


void VtDataCollectWnd::OnBnClickedBtnEnd()
{
	KillTimer(RegularTimerID);
	KillTimer(MinTimerID);
}
