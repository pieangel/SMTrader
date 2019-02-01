// VtOrderLeftWnd.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtOrderLeftWnd.h"
#include "afxdialogex.h"
#include "VtQuote.h"
#include "resource.h"
#include "VtOrderConfigManager.h"
#include "VtQuoteManager.h"
#include "VtSymbol.h"


// CVtOrderLeftWnd dialog

IMPLEMENT_DYNAMIC(CVtOrderLeftWnd, CDialog)

/*
CVtOrderLeftWnd::CVtOrderLeftWnd(CWnd* pParent )
	: CDialogEx(IDD_ORDER_LEFT, pParent)
{

}
*/
CVtOrderLeftWnd::CVtOrderLeftWnd()
{
	_ParentOrderWnd = nullptr;
}

CVtOrderLeftWnd::~CVtOrderLeftWnd()
{
}

int CVtOrderLeftWnd::CRHGetDialogID()
{
	return (IDD_ORDER_LEFT);
}

void CVtOrderLeftWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


void CVtOrderLeftWnd::OnReceiveRealtimeQuote(VtQuote* quote)
{
	_TickQuoteGrid.OnReceiveRealtimeQuote(quote);
}

BEGIN_MESSAGE_MAP(CVtOrderLeftWnd, CDialog)
	ON_BN_CLICKED(IDC_BTN_CANCEL_SEL, &CVtOrderLeftWnd::OnBnClickedBtnCancelSel)
	ON_BN_CLICKED(IDC_BTN_CANCEL_ALL, &CVtOrderLeftWnd::OnBnClickedBtnCancelAll)
	ON_BN_CLICKED(IDC_BTN_LIQ_SEL, &CVtOrderLeftWnd::OnBnClickedBtnLiqSel)
	ON_BN_CLICKED(IDC_BTN_LIQ_ALL, &CVtOrderLeftWnd::OnBnClickedBtnLiqAll)
END_MESSAGE_MAP()


// CVtOrderLeftWnd message handlers


BOOL CVtOrderLeftWnd::OnInitDialog()
{
	CRHGenericChildDialog::OnInitDialog();

	_TickQuoteGrid.AttachGrid(this, IDC_STATIC_REAL_TICKQUOTE);
	_AcceptGrid.AttachGrid(this, IDC_STATIC_ACCEPTED);
	_RemainGrid.AttachGrid(this, IDC_STATIC_REMAIN_LIST);
	_TotalGrid.AttachGrid(this, IDC_STATIC_REMAIN_TOTAL);
	
	InitAcceptedList();
	InitRemainList();
	InitRealtimeQuote();
	InitAccountInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CVtOrderLeftWnd::SetOrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
	_TickQuoteGrid.SetOrderConfigMgr(val);
	_AcceptGrid.SetOrderConfigMgr(val);
	_RemainGrid.SetOrderConfigMgr(val);
	_TotalGrid.SetOrderConfigMgr(val);

	_AcceptGrid.OrderLeftWnd(this);
	_RemainGrid.OrderLeftWnd(this);
}

void CVtOrderLeftWnd::InitRealtimeQuote()
{
	if (!_OrderConfigMgr || !_OrderConfigMgr->Symbol())
		return;
	VtQuoteManager* quoteMgr = VtQuoteManager::GetInstance();
	VtQuote* quote = quoteMgr->FindQuote(_OrderConfigMgr->Symbol()->FullCode);
	if (quote)
		_TickQuoteGrid.OnReceiveRealtimeQuote(quote);
}

void CVtOrderLeftWnd::InitRemainList()
{
	if (!_OrderConfigMgr || !_OrderConfigMgr->Account())
		return;
	VtAccount* acnt = _OrderConfigMgr->Account();
	_RemainGrid.SetRemainList();
}

void CVtOrderLeftWnd::InitAcceptedList()
{
	if (!_OrderConfigMgr || !_OrderConfigMgr->Account())
		return;
	VtAccount* acnt = _OrderConfigMgr->Account();
	_AcceptGrid.SetAcptOrderList();
}

void CVtOrderLeftWnd::InitAccountInfo()
{
	if (!_OrderConfigMgr || !_OrderConfigMgr->Account())
		return;
	VtAccount* acnt = _OrderConfigMgr->Account();
	_TotalGrid.OnReceiveAccountDeposit(acnt);
}

void CVtOrderLeftWnd::RefreshRealtimeQuote()
{
	if (GetSafeHwnd())
		_TickQuoteGrid.SetQuote();
}

void CVtOrderLeftWnd::RefreshRemainList()
{
	if (GetSafeHwnd())
		_RemainGrid.SetRemainList();
}

void CVtOrderLeftWnd::RefreshAcceptedList()
{
	if (GetSafeHwnd())
		_AcceptGrid.SetAcptOrderList();
}

void CVtOrderLeftWnd::ClearRealtimeTickQuoteGrid()
{
	if (GetSafeHwnd())
		_TickQuoteGrid.ClearText();
}

void CVtOrderLeftWnd::OnReceiveAccountDeposit(VtAccount* acnt)
{
	if (GetSafeHwnd())
		_TotalGrid.OnReceiveAccountDeposit(acnt);
}


void CVtOrderLeftWnd::OnBnClickedBtnCancelSel()
{
	// TODO: Add your control notification handler code here
	_AcceptGrid.CancelOrderSelected();
}


void CVtOrderLeftWnd::OnBnClickedBtnCancelAll()
{
	// TODO: Add your control notification handler code here
	_AcceptGrid.CancelOrderAll();
}


void CVtOrderLeftWnd::OnBnClickedBtnLiqSel()
{
	// TODO: Add your control notification handler code here
	_RemainGrid.LiqSelectedPos();
}


void CVtOrderLeftWnd::OnBnClickedBtnLiqAll()
{
	// TODO: Add your control notification handler code here
	_RemainGrid.LiqAll();
}

void CVtOrderLeftWnd::OnOutstanding()
{
	
}
