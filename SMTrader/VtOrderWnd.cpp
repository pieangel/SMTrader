// VtOrderWnd.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtOrderWnd.h"
#include "afxdialogex.h"
#include "VtQuote.h"
#include "VtWindowManager.h"
#include "MainFrm.h"
#include "VtOrderDialogManager.h"
#include "VtSymbolManager.h"
#include "VtSymbol.h"
#include "VtQuote.h"
#include "VtHoga.h"
#include "VtAccount.h"
#include "VtSymbolMaster.h"
#include "VtSymbolSelector.h"
#include "Poco/Delegate.h"
#include "DeferPos.h"
using Poco::Delegate;

// CVtOrderWnd dialog

IMPLEMENT_DYNAMIC(CVtOrderWnd, CDialog)

CVtOrderWnd::CVtOrderWnd(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_ORDER_WND, pParent)
{
	//_OrderMgr = nullptr;
	_CenterWnd.LeftWnd(&_LeftWnd);
	_CenterWnd.RightWnd(&_RightWnd);

	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	_OrderWindowEvent += delegate(orderDlgMgr, &VtOrderDialogManager::OnOrderWindowEventReceived);

	VtOrderWindowEventArgs arg;
	arg.pOrderWnd = this;
	arg.type = VtOrderWindowEventType::Created;
	FireOrderWindowEvent(std::move(arg));

	_CenterWnd.ParentOrderWnd(this);
	_LeftWnd.ParentOrderWnd(this);
	_RightWnd.ParentOrderWnd(this);

	_CenterWnd.SetOrderConfigMgr(&OrderConfigMgr);
	_RightWnd.SetOrderConfigMgr(&OrderConfigMgr);
	_LeftWnd.SetOrderConfigMgr(&OrderConfigMgr);

	OrderConfigMgr.orderWnd = this;
	OrderConfigMgr.leftWnd = &_LeftWnd;
	OrderConfigMgr.centerWnd = &_CenterWnd;
	OrderConfigMgr.rightWnd = &_RightWnd;
}

CVtOrderWnd::~CVtOrderWnd()
{
	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	_OrderWindowEvent -= delegate(orderDlgMgr, &VtOrderDialogManager::OnOrderWindowEventReceived);
}


void CVtOrderWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_LEFT, _leftCtrl);
	DDX_Control(pDX, IDC_STATIC_CENTER, _CenterCtrl);
	DDX_Control(pDX, IDC_STATIC_RIGHT, _RigthCtrl);
}


BEGIN_MESSAGE_MAP(CVtOrderWnd, CDialog)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CVtOrderWnd message handlers


BOOL CVtOrderWnd::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  Add extra initialization here
	VtWindowManager* winMgr = VtWindowManager::GetInstance();


	CRect rect;   // size of this rect doesn't matter

	_CenterCtrl.GetWindowRect(&rect);
	ScreenToClient(&rect);
	_CenterWnd.CRHCreateGenericChildDialog(this, &rect, 1, NULL);

	_leftCtrl.GetWindowRect(&rect);
	ScreenToClient(&rect);
	_LeftWnd.CRHCreateGenericChildDialog(this, &rect, 0, NULL);

	_RigthCtrl.GetWindowRect(&rect);
	ScreenToClient(&rect);
	_RightWnd.CRHCreateGenericChildDialog(this, &rect, 2, NULL);

	winMgr->AddWindow(_T("OrderRightWnd"), &_RightWnd);
	winMgr->AddWindow(_T("OrderLeftWnd"), &_LeftWnd);
	winMgr->AddWindow(_T("OrderCenterWnd"), &_CenterWnd);

	CRect rcLeft, rcCenter, rcRight;
	_LeftWnd.GetWindowRect(rcLeft);
	_CenterWnd.GetWindowRect(rcCenter);
	_RightWnd.GetWindowRect(rcRight);

	OrderConfigMgr.leftWidth = rcLeft.Width();
	OrderConfigMgr.centerWidth = rcCenter.Width();
	OrderConfigMgr.rightWidth = rcRight.Width();


	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CVtOrderWnd::OnRealtimeQuoteReceived(VtRealtimeQuote* realtimeQuote)
{
	if (!realtimeQuote)
		return;
}

void CVtOrderWnd::OnClose()
{
	if (OrderConfigMgr.Symbol() && OrderConfigMgr.Account())
	{
		VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
		orderDlgMgr->RemoveSymbolReceiveWindow(OrderConfigMgr.Symbol()->FullCode, this);
		orderDlgMgr->RemoveAccountReceiveWindow(OrderConfigMgr.Account()->AccountNo, this);
	}
	CDialog::OnClose();
	VtOrderWindowEventArgs arg;
	arg.pOrderWnd = this;
	arg.type = VtOrderWindowEventType::Closed;
	FireOrderWindowEvent(std::move(arg));
}


VtSymbol* CVtOrderWnd::FindSymbol(std::string fullCode)
{
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindSymbolByFullCode(fullCode);
	return sym;
}

void CVtOrderWnd::OnReceiveRealtimeHoga(VtHoga* hoga)
{
	_CenterWnd.OnReceiveRealtimeHoga(hoga);
}

void CVtOrderWnd::OnReceiveHoga(VtHoga* hoga)
{
	VtSymbol* sym = FindSymbol(hoga->FullCode);
	if (sym != OrderConfigMgr.Symbol())
		return;

	OrderConfigMgr.Hoga(hoga);

	_CenterWnd.InitHoga(hoga);
}

void CVtOrderWnd::OnOrderReceived(VtOrder* order)
{
	if (!_IsOpen)
		return;

	_CenterWnd.OnOrderReceived(order);
}

void CVtOrderWnd::OnAccepted(VtOrder* order)
{
	if (!_IsOpen)
		return;
	_CenterWnd.OnAccepted(order);
	_LeftWnd.RefreshAcceptedList();
}

void CVtOrderWnd::OnFilled(VtOrder* order)
{
	_CenterWnd.OnFilled(order);
	_LeftWnd.RefreshAcceptedList();
	_LeftWnd.RefreshRemainList();
}

void CVtOrderWnd::OnConfirmModify(VtOrder* order)
{
	_CenterWnd.OnConfirmModify(order);
	_LeftWnd.RefreshAcceptedList();
}

void CVtOrderWnd::OnConfirmCancel(VtOrder* order)
{
	_CenterWnd.OnConfirmCancel(order);
	_LeftWnd.RefreshAcceptedList();
}

void CVtOrderWnd::OnReceiveRealtimeQuote(VtQuote* quote)
{
	_CenterWnd.OnReceiveRealtimeQuote(quote);
	_LeftWnd.OnReceiveRealtimeQuote(quote);
}

void CVtOrderWnd::OnReceiveQuote(VtQuote* quote)
{
	VtSymbol* sym = FindSymbol(quote->fullCode);
	if (sym != OrderConfigMgr.Symbol())
		return;
	OrderConfigMgr.Quote(quote);
	_CenterWnd.InitQuote(quote);
}

void CVtOrderWnd::OnReceiveSymbolMaster(VtSymbolMaster* master)
{
	VtSymbol* sym = FindSymbol(master->FullCode);
	if (sym != OrderConfigMgr.Symbol())
		return;
	OrderConfigMgr.Master(master);

	_RightWnd.OnReceivedSymbolMaster(master);
}

void CVtOrderWnd::OnReceiveRealtimeSymbolMaster(VtSymbolMaster* master)
{
	_RightWnd.OnReceiveRealtimeSymbolMaster(master);
}

void CVtOrderWnd::OnSymbolClicked(VtSymbol* symbol, int src, VtSymbolSelector* srdWnd)
{
	if (!symbol)
		return;

	OrderConfigMgr.Symbol(symbol);

	_LeftWnd.ClearRealtimeTickQuoteGrid();
}

void CVtOrderWnd::OnReceiveAccountDeposit(VtAccount* acnt)
{
	_RightWnd.OnReceiveAccountDeposit(acnt);
	_LeftWnd.OnReceiveAccountDeposit(acnt);
}

void CVtOrderWnd::ResetSymbol(VtSymbol* sym)
{
	if (OrderConfigMgr.Symbol() == sym)
		return;

	_CenterWnd.ResetSymbol(sym);
	_LeftWnd.ClearRealtimeTickQuoteGrid();
	_LeftWnd.RefreshRealtimeQuote();
}

void CVtOrderWnd::SetHoga(VtHoga* hoga)
{
	OrderConfigMgr.Hoga(hoga);
}

void CVtOrderWnd::SetQuote(VtQuote* quote)
{
	OrderConfigMgr.Quote(quote);
}

void CVtOrderWnd::SetMaster(VtSymbolMaster* master)
{
	OrderConfigMgr.Master(master);
}

void CVtOrderWnd::ExtendLeftWnd(bool flag)
{
	if (_LeftWnd.IsWindowVisible())
	{
		_LeftWnd.ShowWindow(SW_HIDE);
		RECT rcLeft;
		_LeftWnd.GetClientRect(&rcLeft);
		RECT rcCenter;
		_CenterWnd.GetClientRect(&rcCenter);
		rcCenter.left = 0;
		rcCenter.top = 0;
		_CenterWnd.MoveWindow(&rcCenter, TRUE);
	}
	else
	{
		_LeftWnd.ShowWindow(SW_SHOW);
	}
}

void CVtOrderWnd::RefreshAccount()
{
	_LeftWnd.SetOrderConfigMgr(&OrderConfigMgr);
	_RightWnd.SetOrderConfigMgr(&OrderConfigMgr);
}

void CVtOrderWnd::RefreshSymbol()
{
	_LeftWnd.ClearRealtimeTickQuoteGrid();
	_LeftWnd.SetOrderConfigMgr(&OrderConfigMgr);
	_RightWnd.SetOrderConfigMgr(&OrderConfigMgr);
}

void CVtOrderWnd::RecalcLayout()
{
	//CDeferPos dp(1);
	//dp.MoveWindow(_SymSelector->GetSafeHwnd(), dlgRect.left, dlgRect.top, dlgRect.Width(), dlgRect.Height(), TRUE);
	CRect rcLeft, rcCenter, rcRight, rcWnd;
	int width = 0, height = 0;
	GetWindowRect(rcWnd);
	bool bShowLeft = true;
	bool bShowRight = true;

	if (OrderConfigMgr.ShowLeft)
	{
		_LeftWnd.ShowWindow(SW_SHOW);
		_LeftWnd.GetWindowRect(rcLeft);
		width += OrderConfigMgr.leftWidth;
	}
	else
	{
		_LeftWnd.ShowWindow(SW_HIDE);
		bShowLeft = false;
	}

	_CenterWnd.GetWindowRect(rcCenter);
	width += OrderConfigMgr.centerWidth;
	height = rcCenter.Height();

	if (OrderConfigMgr.ShowRight)
	{
		_RightWnd.ShowWindow(SW_SHOW);
		_RightWnd.GetWindowRect(rcRight);
		width += OrderConfigMgr.rightWidth;
	}
	else
	{
		_RightWnd.ShowWindow(SW_HIDE);
		bShowRight = false;
	}

	if (!bShowLeft)
	{
		if (!bShowRight)
		{
			_CenterWnd.MoveWindow(0, 0, rcCenter.Width(), rcCenter.Height(), TRUE);
			MoveWindow(rcWnd.left, rcWnd.top, rcCenter.Width() + 13, rcCenter.Height(), TRUE);
		}
		else
		{
			_CenterWnd.MoveWindow(0, 0, rcCenter.Width(), rcCenter.Height(), TRUE);
			_RightWnd.MoveWindow(OrderConfigMgr.centerWidth, 0, rcRight.Width(), rcRight.Height(), TRUE);
			MoveWindow(rcWnd.left, rcWnd.top, OrderConfigMgr.centerWidth + OrderConfigMgr.rightWidth + 13, rcCenter.Height(), TRUE);
		}
	}
	else
	{
		if (!bShowRight)
		{
			_LeftWnd.MoveWindow(0, 0, OrderConfigMgr.leftWidth, rcCenter.Height(), TRUE);
			_CenterWnd.MoveWindow(OrderConfigMgr.leftWidth, 0, rcCenter.Width(), rcCenter.Height(), TRUE);
			MoveWindow(rcWnd.left, rcWnd.top, OrderConfigMgr.centerWidth + OrderConfigMgr.leftWidth + 13, rcCenter.Height(), TRUE);
		}
		else
		{
			_LeftWnd.MoveWindow(0, 0, OrderConfigMgr.leftWidth, rcCenter.Height(), TRUE);
			_CenterWnd.MoveWindow(OrderConfigMgr.leftWidth, 0, rcCenter.Width(), rcCenter.Height(), TRUE);
			_RightWnd.MoveWindow(OrderConfigMgr.leftWidth + OrderConfigMgr.centerWidth, 0, rcRight.Width(), rcRight.Height(), TRUE);
			MoveWindow(rcWnd.left, rcWnd.top, OrderConfigMgr.centerWidth + OrderConfigMgr.leftWidth + OrderConfigMgr.rightWidth, rcCenter.Height(), TRUE);
		}
	}
}

