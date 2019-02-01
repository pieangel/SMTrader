// VtChartCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtChartCtrl.h"
#include "afxdialogex.h"
#include "Resource.h"

// VtChartCtrl dialog

IMPLEMENT_DYNAMIC(VtChartCtrl, CDialogEx)

VtChartCtrl::VtChartCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CHART, pParent)
{

}

VtChartCtrl::~VtChartCtrl()
{
}

void VtChartCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(VtChartCtrl, CDialogEx)
END_MESSAGE_MAP()


// VtChartCtrl message handlers
