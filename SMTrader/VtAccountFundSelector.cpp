// VtAccountFundSelector.cpp : implementation file
//

#include "stdafx.h"
#include "VtAccountFundSelector.h"
#include "afxdialogex.h"
#include "resource.h"

// VtAccountFundSelector dialog

IMPLEMENT_DYNAMIC(VtAccountFundSelector, CDialogEx)

VtAccountFundSelector::VtAccountFundSelector(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ACNT_FUND_SELECTOR, pParent)
{
}

VtAccountFundSelector::~VtAccountFundSelector()
{
}

void VtAccountFundSelector::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(VtAccountFundSelector, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO_ENTIRE, &VtAccountFundSelector::OnBnClickedRadioEntire)
	ON_BN_CLICKED(IDC_RADIO_ACNT, &VtAccountFundSelector::OnBnClickedRadioAcnt)
	ON_BN_CLICKED(IDC_RADIO_FUND, &VtAccountFundSelector::OnBnClickedRadioFund)
END_MESSAGE_MAP()


// VtAccountFundSelector message handlers


BOOL VtAccountFundSelector::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	((CButton*)GetDlgItem(IDC_RADIO_ENTIRE))->SetCheck(BST_CHECKED);
	// TODO:  Add extra initialization here
	_Grid.AttachGrid(this, IDC_STATIC_GRID);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void VtAccountFundSelector::OnBnClickedRadioEntire()
{
	_Grid.Mode(0);
	_Grid.InitGrid();
}


void VtAccountFundSelector::OnBnClickedRadioAcnt()
{
	_Grid.Mode(1);
	_Grid.InitGrid();
}


void VtAccountFundSelector::OnBnClickedRadioFund()
{
	_Grid.Mode(2);
	_Grid.InitGrid();
}

void VtAccountFundSelector::SetConfigDlg(VtUsdStrategyConfigDlg* ConfigDlg)
{
	_Grid.SetConfigDlg(ConfigDlg);
}
