// HdOrderDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "HdOrderDialog.h"
#include "afxdialogex.h"


// HdOrderDialog dialog

IMPLEMENT_DYNAMIC(HdOrderDialog, CDialogEx)

HdOrderDialog::HdOrderDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ORDER_TOTAL, pParent)
{

}

HdOrderDialog::~HdOrderDialog()
{
}

void HdOrderDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(HdOrderDialog, CDialogEx)
END_MESSAGE_MAP()


// HdOrderDialog message handlers


BOOL HdOrderDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	_OrderGrid.AttachGrid(this, IDC_STATIC_ORDER);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
