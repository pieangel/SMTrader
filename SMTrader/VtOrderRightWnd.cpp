// VtOrderRightWnd.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtOrderRightWnd.h"
#include "afxdialogex.h"
#include "VtOrderConfigManager.h"


// CVtOrderRightWnd dialog

IMPLEMENT_DYNAMIC(CVtOrderRightWnd, CDialog)



CVtOrderRightWnd::CVtOrderRightWnd()
{
	ParentOrderWnd(nullptr);
	_DefaultWidth = 184;
}

CVtOrderRightWnd::~CVtOrderRightWnd()
{
	
}

int CVtOrderRightWnd::CRHGetDialogID()
{
	return (IDD_ORDER_RIGHT);
}

void CVtOrderRightWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_CONFIG, _TabCtrl);
}


BEGIN_MESSAGE_MAP(CVtOrderRightWnd, CDialog)
	ON_WM_CLOSE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CONFIG, &CVtOrderRightWnd::OnTcnSelchangeTabConfig)
END_MESSAGE_MAP()


// CVtOrderRightWnd message handlers


BOOL CVtOrderRightWnd::OnInitDialog()
{
	CRHGenericChildDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	_AccountRemainGrid.AttachGrid(this, IDC_STATIC_ACCOUNT_REMAIN);
	_SymMasterGrid.AttachGrid(this, IDC_STATIC_SYMBOL_MASTER);

	InitTabCtrl();
	InitAccountInfo();
	InitMasterInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CVtOrderRightWnd::OnReceivedSymbolMaster(VtSymbolMaster* symMaster)
{
	_SymMasterGrid.OnReceivedSymbolMaster(symMaster);
}

void CVtOrderRightWnd::OnReceivedSymbolMaster(VtRealtimeSymbolMaster* symMaster)
{
	_SymMasterGrid.OnReceivedSymbolMaster(symMaster);
}

void CVtOrderRightWnd::OnReceiveRealtimeSymbolMaster(VtSymbolMaster* symMaster)
{
	_SymMasterGrid.OnReceiveRealtimeSymbolMaster(symMaster);
}

void CVtOrderRightWnd::OnReceiveAccountDeposit(VtAccount* acnt)
{
	_AccountRemainGrid.OnReceiveAccountDeposit(acnt);
}

void CVtOrderRightWnd::SetSymbol(VtSymbol* sym)
{
	if (!sym)
		return;
	_SymMasterGrid.InitSymMaster(sym);
}


void CVtOrderRightWnd::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	CRHGenericChildDialog::OnClose();
}

void CVtOrderRightWnd::InitTabCtrl()
{
	_TabCtrl.InsertItem(0, _T("청산"));
	_TabCtrl.InsertItem(1, _T("설정"));


	CRect rect;
	_TabCtrl.GetClientRect(rect);

	_Page1.Create(IDD_ORDER_SETTLE, &_TabCtrl);
	_Page1.SetWindowPos(nullptr, 3, 25, rect.Width() - 6, rect.Height() - 30, SWP_NOZORDER);
	_CurrentPage = &_Page1;

	_Page2.Create(IDD_ORDER_CONFIG, &_TabCtrl);
	_Page2.SetWindowPos(nullptr, 3, 25, rect.Width() - 6, rect.Height() - 30, SWP_NOZORDER);

	_CurrentPage->ShowWindow(SW_SHOW);

}


void CVtOrderRightWnd::InitAccountInfo()
{
	if (!_OrderConfigMgr || !_OrderConfigMgr->Account())
		return;
	VtAccount* acnt = _OrderConfigMgr->Account();
	_AccountRemainGrid.OnReceiveAccountDeposit(acnt);
}

void CVtOrderRightWnd::InitMasterInfo()
{
	if (!_OrderConfigMgr || !_OrderConfigMgr->Master())
		return;
	VtSymbolMaster* master = _OrderConfigMgr->Master();
	_SymMasterGrid.OnReceivedSymbolMaster(master);
}

void CVtOrderRightWnd::OnTcnSelchangeTabConfig(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (_CurrentPage != nullptr)
	{
		_CurrentPage->ShowWindow(SW_HIDE);
		_CurrentPage = nullptr;
	}

	int curIndex = _TabCtrl.GetCurSel();
	switch (curIndex)
	{
	case 0:
		_Page1.ShowWindow(SW_SHOW);
		_CurrentPage = &_Page1;
		break;
	case 1:
		_Page2.ShowWindow(SW_SHOW);
		_CurrentPage = &_Page2;
		break;
	}
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CVtOrderRightWnd::Reset()
{
	_SymMasterGrid.ResetSymbol();
}

