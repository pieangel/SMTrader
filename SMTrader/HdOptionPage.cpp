// HdOptionPage.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "HdOptionPage.h"
#include "afxdialogex.h"
#include "HdSymbolSelecter.h"
#include "VtGlobal.h"

// HdOptionPage dialog
extern TApplicationFont g_Font;

IMPLEMENT_DYNAMIC(HdOptionPage, CDialogEx)

HdOptionPage::HdOptionPage(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_OPT_PAGE, pParent)
{
	_SymSelecter = nullptr;
}

HdOptionPage::~HdOptionPage()
{
}

void HdOptionPage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_OPT_MARKET, _ComboOptionMarket);
	DDX_Control(pDX, IDC_COMBO_OPT_MONTH, _ComboOptionMonth);
}


BEGIN_MESSAGE_MAP(HdOptionPage, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_OPT_MARKET, &HdOptionPage::OnCbnSelchangeComboOptMarket)
	ON_CBN_SELCHANGE(IDC_COMBO_OPT_MONTH, &HdOptionPage::OnCbnSelchangeComboOptMonth)
END_MESSAGE_MAP()


// HdOptionPage message handlers


BOOL HdOptionPage::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	::EnumChildWindows(m_hWnd, ::SetChildFont, (LPARAM)g_Font.GetFont());

	_OptionGrid.OptionPage(this);
	// TODO:  Add extra initialization here
	_OptionGrid.AttachGrid(this, IDC_STATIC_OPT_GRID);
	_OptionGrid.SymSelecter(_SymSelecter);
	//if (_SymSelecter && _SymSelecter->SelTab == 1)
	//	_OptionGrid.GetSymbolMasterLately();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void HdOptionPage::OnCbnSelchangeComboOptMarket()
{
	// TODO: Add your control notification handler code here
	_OptionGrid.SetProductSection();
	_OptionGrid.SetYearMonth();
	//if (_SymSelecter && _SymSelecter->SelTab == 1)
		_OptionGrid.GetSymbolMasterLately();
	_OptionGrid.InitGrid();
}


void HdOptionPage::OnCbnSelchangeComboOptMonth()
{
	// TODO: Add your control notification handler code here
	//if (_SymSelecter && _SymSelecter->SelTab == 1)
		_OptionGrid.GetSymbolMasterLately();
	_OptionGrid.InitGrid();
}

void HdOptionPage::GetSymbolMaster()
{
	_OptionGrid.GetSymbolMasterLately();
}

void HdOptionPage::SetConfigDlg(VtUsdStrategyConfigDlg* ConfigDlg)
{
	_OptionGrid.SetConfigDlg(ConfigDlg);
}

void HdOptionPage::SetAddConSigDlg(VtAddConnectSignalDlg* conSigDlg)
{
	_OptionGrid.SetAddConSigDlg(conSigDlg);
}

void HdOptionPage::SetSigConGrid(VtSignalConnectionGrid* sigConGrid)
{
	_OptionGrid.SetSigConGrid(sigConGrid);
}

void HdOptionPage::SetChartTimeToolBar(VtChartTimeToolBar* timeToolBar)
{
	_OptionGrid.SetChartTimeToolBar(timeToolBar);
}
