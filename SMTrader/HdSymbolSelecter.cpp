// HdSymbolSelecter.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "HdSymbolSelecter.h"
#include "afxdialogex.h"
#include "VtGlobal.h"

// HdSymbolSelecter dialog
extern TApplicationFont g_Font;

IMPLEMENT_DYNAMIC(HdSymbolSelecter, CDialogEx)

HdSymbolSelecter::HdSymbolSelecter(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SYMBOL_SELECTER_HD, pParent)
{
	_CurrentPage = nullptr;
	_FutPage.SymSelecter(this);
	_OptPage.SymSelecter(this);
	SelTab = -1;
	_OrderConfigMgr = nullptr;
}

HdSymbolSelecter::~HdSymbolSelecter()
{
}

void HdSymbolSelecter::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_FUTOPT, _TabCtrl);
}


BEGIN_MESSAGE_MAP(HdSymbolSelecter, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_FUTOPT, &HdSymbolSelecter::OnTcnSelchangeTabFutopt)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// HdSymbolSelecter message handlers


BOOL HdSymbolSelecter::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	::EnumChildWindows(m_hWnd, ::SetChildFont, (LPARAM)g_Font.GetFont());
	// TODO:  Add extra initialization here
	InitTabCtrl();
	_FutPage.GetSymbolMaster();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void HdSymbolSelecter::InitTabCtrl()
{
	
	_TabCtrl.InsertItem(0, _T("선물"));
	_TabCtrl.InsertItem(1, _T("옵션"));

	CRect rect;
	_TabCtrl.GetClientRect(rect);

	_FutPage.Create(IDD_FUT_PAGE, &_TabCtrl);
	_FutPage.SetWindowPos(nullptr, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);
	_CurrentPage = &_FutPage;

	_OptPage.Create(IDD_OPT_PAGE, &_TabCtrl);
	_OptPage.SetWindowPos(nullptr, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);
	_OptPage.ShowWindow(SW_HIDE);
	
	_CurrentPage->ShowWindow(SW_SHOW);
	SelTab = 0;
}


void HdSymbolSelecter::OnTcnSelchangeTabFutopt(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	if (_CurrentPage != nullptr)
	{
		_CurrentPage->ShowWindow(SW_HIDE);
		_CurrentPage = nullptr;
	}

	int curIndex = _TabCtrl.GetCurSel();
	switch (curIndex)
	{
	case 0:
		_FutPage.ShowWindow(SW_SHOW);
		_FutPage.GetSymbolMaster();
		_CurrentPage = &_FutPage;
		SelTab = 0;
		break;
	case 1:
		_OptPage.ShowWindow(SW_SHOW);
		_OptPage.GetSymbolMaster();
		_CurrentPage = &_OptPage;
		SelTab = 1;
		break;
	}

	*pResult = 0;
}


void HdSymbolSelecter::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (_TabCtrl.GetSafeHwnd())
	{
		// Tab 컨트롤은 전체 클라이언트 영역을 처리해야 합니다.
		_TabCtrl.SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void HdSymbolSelecter::SetConfigDlg(VtUsdStrategyConfigDlg* ConfigDlg)
{
	_FutPage.SetConfigDlg(ConfigDlg);
	_OptPage.SetConfigDlg(ConfigDlg);
}

void HdSymbolSelecter::SetAddConSigDlg(VtAddConnectSignalDlg* conSigDlg)
{
	_FutPage.SetAddConSigDlg(conSigDlg);
	_OptPage.SetAddConSigDlg(conSigDlg);
}
