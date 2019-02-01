// VtOrderRightCancelHd.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtOrderRightCancelHd.h"
#include "afxdialogex.h"


// VtOrderRightCancelHd dialog

IMPLEMENT_DYNAMIC(VtOrderRightCancelHd, CDialogEx)

VtOrderRightCancelHd::VtOrderRightCancelHd(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ORDER_RIGHT_CANCEL_HD, pParent)
{
	_OrderConfigMgr = nullptr;
}

VtOrderRightCancelHd::~VtOrderRightCancelHd()
{
}

void VtOrderRightCancelHd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CANCEL, _EditCancel);
}


BEGIN_MESSAGE_MAP(VtOrderRightCancelHd, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CANCEL_LAST, &VtOrderRightCancelHd::OnBnClickedBtnCancelLast)
	ON_BN_CLICKED(IDC_CK_FROMRECENT, &VtOrderRightCancelHd::OnBnClickedCkFromrecent)
	ON_BN_CLICKED(IDC_BTN_CANCEL_SELECTED, &VtOrderRightCancelHd::OnBnClickedBtnCancelSelected)
END_MESSAGE_MAP()


// VtOrderRightCancelHd message handlers


void VtOrderRightCancelHd::OnBnClickedBtnCancelLast()
{
	// TODO: Add your control notification handler code here
}


void VtOrderRightCancelHd::OnBnClickedCkFromrecent()
{
	// TODO: Add your control notification handler code here
}


void VtOrderRightCancelHd::OnBnClickedBtnCancelSelected()
{
	// TODO: Add your control notification handler code here
}


BOOL VtOrderRightCancelHd::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
