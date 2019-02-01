// HdAccountPLDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "HdAccountPLDlg.h"
#include "afxdialogex.h"
#include "VtAccountManager.h"
#include "HdWindowManager.h"
#include "VtAccount.h"
#include "VtGlobal.h"
#include "Resource.h"
#include "Poco/Delegate.h"
using Poco::Delegate;
// HdAccountPLDlg dialog

IMPLEMENT_DYNAMIC(HdAccountPLDlg, CDialogEx)

HdAccountPLDlg::HdAccountPLDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MINI_JANGO, pParent)
{
	_Account = nullptr;
}

HdAccountPLDlg::~HdAccountPLDlg()
{
	HdWindowManager* wndMgr = HdWindowManager::GetInstance();
	_WindowEvent -= delegate(wndMgr, &HdWindowManager::OnWindowEvent);
}

void HdAccountPLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ACCOUNT, _ComboAccount);
}


BEGIN_MESSAGE_MAP(HdAccountPLDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(IDC_COMBO_ACCOUNT, &HdAccountPLDlg::OnCbnSelchangeComboAccount)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// HdAccountPLDlg message handlers


BOOL HdAccountPLDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	this->SetIcon(hIcon, FALSE);

	// TODO:  Add extra initialization here
	InitAccount();
	_ProductGrid.AcntPLDlg(this);
	_AccountGrid.AcntPLDlg(this);
	_AccountGrid.AttachGrid(this, IDC_STATIC_MINI_TOP);
	_ProductGrid.AttachGrid(this, IDC_STATIC_MINI_SUB);
	((CButton*)GetDlgItem(IDC_CHECK_THOUSAND))->SetCheck(BST_CHECKED);

	HdWindowManager* wndMgr = HdWindowManager::GetInstance();
	_WindowEvent += delegate(wndMgr, &HdWindowManager::OnWindowEvent);

	HdWindowEventArgs arg;
	arg.pWnd = this;
	arg.wndType = HdWindowType::MiniJangoWindow;
	arg.eventType = HdWindowEventType::Created;
	FireWindowEvent(std::move(arg));
	CRect rcWnd;
	GetWindowRect(rcWnd);
	SetWindowPos(nullptr, 0, 0, 210, rcWnd.Height(), SWP_NOMOVE);
	_ComboAccount.SetDroppedWidth(150);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void HdAccountPLDlg::InitAccount()
{
	VtGlobal* global = VtGlobal::GetInstance();
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	int selAcnt = 0;
	std::string acntName;
	for (auto it = acntMgr->AccountMap.begin(); it != acntMgr->AccountMap.end(); ++it)
	{
		VtAccount* acnt = it->second;
		acntName = acnt->AccountNo;
		acntName.append(_T(" "));
		acntName.append(acnt->AccountName);
		int index = _ComboAccount.AddString(acntName.c_str());
		_ComboAccount.SetItemDataPtr(index, acnt);
		if (index == 0)
		{
			selAcnt = index;
		}

		if (acnt->AccountLevel() == 0)
		{
			std::vector<VtAccount*>& subAcntList = acnt->GetSubAccountList();
			for (auto it = subAcntList.begin(); it != subAcntList.end(); ++it)
			{
				VtAccount* subAcnt = *it;
				acntName = subAcnt->AccountNo;
				acntName.append(_T(" "));
				acntName.append(subAcnt->AccountName);
				index = _ComboAccount.AddString(acntName.c_str());
				_ComboAccount.SetItemDataPtr(index, subAcnt);
			}
		}
	}

	if (selAcnt != -1)
	{
		_ComboAccount.SetCurSel(selAcnt);
		_Account = (VtAccount*)_ComboAccount.GetItemDataPtr(selAcnt);
		_Account->GetAccountInfoNFee(1);
	}
}


int HdAccountPLDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}


void HdAccountPLDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnClose();

	HdWindowEventArgs arg;
	arg.pWnd = this;
	arg.wndType = HdWindowType::MiniJangoWindow;
	arg.eventType = HdWindowEventType::Closed;
	FireWindowEvent(std::move(arg));
}

void HdAccountPLDlg::OnReceiveQuote(VtSymbol* sym)
{
	if (!sym)
		return;
	_AccountGrid.OnReceiveQuote(sym);
	_ProductGrid.OnReceiveQuote(sym);
}


void HdAccountPLDlg::OnOrderFilledHd(VtOrder* order)
{
	_AccountGrid.OnOrderFilledHd(order);
	_ProductGrid.ClearValues();
	_ProductGrid.OnOrderFilledHd(order);
}

void HdAccountPLDlg::OnSymbolMaster(VtSymbol* sym)
{
	if (!sym)
		return;

	if (!_Account)
		return;

	_Account->CalcOpenPL(sym);
	_Account->SumOpenPL();
	_AccountGrid.OnReceiveQuote(sym);
	_ProductGrid.OnReceiveQuote(sym);
}

void HdAccountPLDlg::OnReceiveAccountInfo()
{
	_AccountGrid.InitGrid();
	_ProductGrid.InitGrid();
}

void HdAccountPLDlg::OnCbnSelchangeComboAccount()
{
	// TODO: Add your control notification handler code here
	int curSel = _ComboAccount.GetCurSel();
	if (curSel != -1)
	{
		_AccountGrid.ClearValues();
		_ProductGrid.ClearValues();
		VtAccount* acnt = (VtAccount*)_ComboAccount.GetItemDataPtr(curSel);
		acnt->GetAccountInfoNFee(1);
		_Account = acnt;
	}
}


void HdAccountPLDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (_ProductGrid.GetSafeHwnd())
		_ProductGrid.MoveWindow(0, 112, cx, cy - 20);
}
