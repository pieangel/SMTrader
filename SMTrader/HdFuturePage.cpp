// HdFuturePage.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "HdFuturePage.h"
#include "afxdialogex.h"
#include "HdSymbolSelecter.h"
#include "VtGlobal.h"

// HdFuturePage dialog
extern TApplicationFont g_Font;

IMPLEMENT_DYNAMIC(HdFuturePage, CDialogEx)

HdFuturePage::HdFuturePage(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FUT_PAGE, pParent)
{
	_SymSelecter = nullptr;
}

HdFuturePage::~HdFuturePage()
{
}

void HdFuturePage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FUT_MARKET, _ComboFutureMarket);
}


BEGIN_MESSAGE_MAP(HdFuturePage, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_FUT_MARKET, &HdFuturePage::OnCbnSelchangeComboFutMarket)
END_MESSAGE_MAP()


// HdFuturePage message handlers


BOOL HdFuturePage::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	::EnumChildWindows(m_hWnd, ::SetChildFont, (LPARAM)g_Font.GetFont());
	// TODO:  Add extra initialization here
	_FutureGrid.FuturePage(this);
	_FutureGrid.AttachGrid(this, IDC_STATIC_FUT_GRID);
	_FutureGrid.SymSelecter(_SymSelecter);
	//if (_SymSelecter && _SymSelecter->SelTab == 0)
	//	_FutureGrid.GetSymbolMaster();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void HdFuturePage::OnCbnSelchangeComboFutMarket()
{
	// TODO: Add your control notification handler code here
	_FutureGrid.SetProductSection();
	_FutureGrid.InitGrid();
	if (_SymSelecter && _SymSelecter->SelTab == 0)
		_FutureGrid.GetSymbolMaster();
}

void HdFuturePage::GetSymbolMaster()
{
	_FutureGrid.GetSymbolMaster();
}

void HdFuturePage::SetConfigDlg(VtUsdStrategyConfigDlg* ConfigDlg)
{
	_FutureGrid.SetConfigDlg(ConfigDlg);
}

void HdFuturePage::SetAddConSigDlg(VtAddConnectSignalDlg* conSigDlg)
{
	_FutureGrid.SetAddConSigDlg(conSigDlg);
}

void HdFuturePage::SetSigConGrid(VtSignalConnectionGrid* sigConGrid)
{
	_FutureGrid.SetSigConGrid(sigConGrid);
}

void HdFuturePage::SetChartTimeToolBar(VtChartTimeToolBar* timeToolBar)
{
	_FutureGrid.SetChartTimeToolBar(timeToolBar);
}
