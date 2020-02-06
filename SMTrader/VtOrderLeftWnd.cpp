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
	_DefaultWidth = 162;
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
	DDX_Control(pDX, IDC_CUSTOM_TICK, _TickQuoteGrid);
	DDX_Control(pDX, IDC_CUSTOM_ACCOUNT, _TotalGrid);
	DDX_Control(pDX, IDC_BTN_CANCEL_ALL, _BtnCancellAll);
	DDX_Control(pDX, IDC_BTN_CANCEL_SEL, _BtnCancelSel);
	DDX_Control(pDX, IDC_BTN_LIQ_ALL, _BtnLiqAll);
	DDX_Control(pDX, IDC_BTN_LIQ_SEL, _BtnLiqSel);
}


void CVtOrderLeftWnd::OnReceiveRealtimeQuote(VtQuote* quote)
{
	//_TickQuoteGrid.OnReceiveRealtimeQuote(quote);
}

BEGIN_MESSAGE_MAP(CVtOrderLeftWnd, CDialog)
	ON_BN_CLICKED(IDC_BTN_CANCEL_SEL, &CVtOrderLeftWnd::OnBnClickedBtnCancelSel)
	ON_BN_CLICKED(IDC_BTN_CANCEL_ALL, &CVtOrderLeftWnd::OnBnClickedBtnCancelAll)
	ON_BN_CLICKED(IDC_BTN_LIQ_SEL, &CVtOrderLeftWnd::OnBnClickedBtnLiqSel)
	ON_BN_CLICKED(IDC_BTN_LIQ_ALL, &CVtOrderLeftWnd::OnBnClickedBtnLiqAll)
	ON_STN_CLICKED(IDC_STATIC_ACCEPTED, &CVtOrderLeftWnd::OnStnClickedStaticAccepted)
END_MESSAGE_MAP()


// CVtOrderLeftWnd message handlers


void CVtOrderLeftWnd::SetRealtickSymbol(VtSymbol* symbol)
{
	if (!symbol)
		return;
	_TickQuoteGrid.Symbol(symbol);
}

void CVtOrderLeftWnd::InitGridInfo()
{
	InitAcceptedList();
	InitRemainList();
	InitRealtimeQuote();
	InitAccountInfo();
}

void CVtOrderLeftWnd::OrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;

	_OrderConfigMgr = val;
	_TickQuoteGrid.SetOrderConfigMgr(val);
	_AcceptGrid.SetOrderConfigMgr(val);
	_RemainGrid.SetOrderConfigMgr(val);
	_TotalGrid.OrderConfigMgr(val);

	_AcceptGrid.OrderLeftWnd(this);
	_RemainGrid.OrderLeftWnd(this);
}

BOOL CVtOrderLeftWnd::OnInitDialog()
{
	CRHGenericChildDialog::OnInitDialog();

	_TickQuoteGrid.Init();
	_AcceptGrid.AttachGrid(this, IDC_STATIC_ACCEPTED);
	_RemainGrid.AttachGrid(this, IDC_STATIC_REMAIN_LIST);
	_TotalGrid.Init();
	

	std::vector<CShadeButtonST*> _btnVec;
	_btnVec.push_back(&_BtnCancellAll);
	_btnVec.push_back(&_BtnCancelSel);
	_btnVec.push_back(&_BtnLiqAll);
	_btnVec.push_back(&_BtnLiqSel);

	for (auto it = _btnVec.begin(); it != _btnVec.end(); ++it) {
		CShadeButtonST* btn = *it;
		btn->SetShade(SHS_HSHADE, 8, 20, 5, RGB(55, 55, 255));
		btn->DrawFlatFocus(TRUE);
	}
	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CVtOrderLeftWnd::InitRealtimeQuote()
{
// 	if (_TickQuoteGrid.GetSafeHwnd()) {
// 		// 틱그리드 초기화
// 		_TickQuoteGrid.Init();
// 	}
}

void CVtOrderLeftWnd::InitRemainList()
{
	if (!_OrderConfigMgr || !_OrderConfigMgr->Account())
		return;
	if (_RemainGrid.GetSafeHwnd()) {
		VtAccount* acnt = _OrderConfigMgr->Account();
		_RemainGrid.SetRemainList();
	}
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
	if (_TotalGrid.GetSafeHwnd()) {
		VtAccount* acnt = _OrderConfigMgr->Account();
		_TotalGrid.InitGrid();
	}
}

void CVtOrderLeftWnd::RefreshRealtimeQuote()
{
	
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
// 	if (GetSafeHwnd())
// 		_TotalGrid.OnReceiveAccountDeposit(acnt);
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


void CVtOrderLeftWnd::OnStnClickedStaticAccepted()
{
	// TODO: Add your control notification handler code here
}
