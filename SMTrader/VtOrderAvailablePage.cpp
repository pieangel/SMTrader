// VtOrderAvailablePage.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtOrderAvailablePage.h"
#include "afxdialogex.h"


// VtOrderAvailablePage dialog

IMPLEMENT_DYNAMIC(VtOrderAvailablePage, CDialogEx)

VtOrderAvailablePage::VtOrderAvailablePage(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ORDER_AVAILABLE, pParent)
{

}

VtOrderAvailablePage::~VtOrderAvailablePage()
{
}

void VtOrderAvailablePage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(VtOrderAvailablePage, CDialogEx)
END_MESSAGE_MAP()


// VtOrderAvailablePage message handlers
