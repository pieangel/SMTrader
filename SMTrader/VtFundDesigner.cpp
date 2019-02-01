// VtFundDesigner.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtFundDesigner.h"
#include "afxdialogex.h"
#include "VtSaveManager.h"
#include "HdWindowManager.h"
#include "Poco/Delegate.h"
#include "VtGlobal.h"
using Poco::Delegate;

// VtFundDesigner dialog
extern TApplicationFont g_Font;
IMPLEMENT_DYNAMIC(VtFundDesigner, CDialogEx)

VtFundDesigner::VtFundDesigner(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FUND_DESIGNER, pParent)
{
	_CurrentPage = nullptr;
}

VtFundDesigner::~VtFundDesigner()
{
	HdWindowManager* wndMgr = HdWindowManager::GetInstance();
	_WindowEvent -= delegate(wndMgr, &HdWindowManager::OnWindowEvent);
}

void VtFundDesigner::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_FD, _TabCtrl);
}


void VtFundDesigner::InitTabCtrl()
{
	if (_TabCtrl.GetSafeHwnd())
	{
		CRect rcWnd;
		GetWindowRect(rcWnd);
		ScreenToClient(rcWnd);
		// Tab 컨트롤은 전체 클라이언트 영역을 처리해야 합니다.
		_TabCtrl.SetWindowPos(NULL, -1, -1, rcWnd.Width(), rcWnd.Height(), SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	}

	_TabCtrl.InsertItem(0, _T("서브계좌"));
	_TabCtrl.InsertItem(1, _T("다계좌"));

	CRect rect;
	_TabCtrl.GetClientRect(rect);

	_SubAcntPage.Create(IDD_FUND_SUBACCOUNTPAGE, &_TabCtrl);
	_SubAcntPage.FundDesigner(this);
	_SubAcntPage.SetWindowPos(nullptr, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);
	_SubAcntPage.ShowWindow(SW_HIDE);

	_MultiAcntPage.Create(IDD_FUND_ACCOUNTPAGE, &_TabCtrl);
	_MultiAcntPage.SetWindowPos(nullptr, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);
	_MultiAcntPage.ShowWindow(SW_HIDE);

	if (SelTab == 0)
	{
		_CurrentPage = &_SubAcntPage;
	}
	else
	{
		_CurrentPage = &_MultiAcntPage;
	}

	_TabCtrl.SetCurSel(SelTab);
	_CurrentPage->ShowWindow(SW_SHOW);
}

BEGIN_MESSAGE_MAP(VtFundDesigner, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_FD, &VtFundDesigner::OnTcnSelchangeTabFund)
	ON_WM_SIZE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// VtFundDesigner message handlers


BOOL VtFundDesigner::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	this->SetIcon(hIcon, FALSE);

	::EnumChildWindows(m_hWnd, ::SetChildFont, (LPARAM)g_Font.GetFont());

	InitTabCtrl();
	// TODO:  Add extra initialization here

	HdWindowManager* wndMgr = HdWindowManager::GetInstance();
	_WindowEvent += delegate(wndMgr, &HdWindowManager::OnWindowEvent);

	HdWindowEventArgs arg;
	arg.pWnd = this;
	arg.wndType = HdWindowType::FundDesigner;
	arg.eventType = HdWindowEventType::Created;
	FireWindowEvent(std::move(arg));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void VtFundDesigner::OnTcnSelchangeTabFund(NMHDR *pNMHDR, LRESULT *pResult)
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
		_SubAcntPage.ShowWindow(SW_SHOW);
		_CurrentPage = &_SubAcntPage;
		SelTab = 0;
		break;
	case 1:
		_MultiAcntPage.ShowWindow(SW_SHOW);
		_CurrentPage = &_MultiAcntPage;
		SelTab = 1;
		break;
	}
	*pResult = 0;
}


void VtFundDesigner::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (_TabCtrl.GetSafeHwnd())
	{
		// Tab 컨트롤은 전체 클라이언트 영역을 처리해야 합니다.
		_TabCtrl.SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

		CRect rect;
		_TabCtrl.GetClientRect(rect);

		_SubAcntPage.SetWindowPos(nullptr, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);

		_MultiAcntPage.SetWindowPos(nullptr, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);
	}
}

void VtFundDesigner::OnClose()
{
	VtSaveManager* saveMgr = VtSaveManager::GetInstance();
	saveMgr->SaveFundList(_T("fundlist.dat"));

	CDialogEx::OnClose();

	HdWindowEventArgs arg;
	arg.pWnd = this;
	arg.wndType = HdWindowType::FundDesigner;
	arg.eventType = HdWindowEventType::Closed;
	FireWindowEvent(std::move(arg));
}
