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
	_ComboAccount.SetDroppedWidth(250);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void HdAccountPLDlg::SaveToXml(pugi::xml_node& window_node)
{
	CRect rcWnd;
	GetWindowRect(rcWnd);
	pugi::xml_node window_child = window_node.append_child("window_pos");
	window_child.append_attribute("left") = rcWnd.left;
	window_child.append_attribute("top") = rcWnd.top;
	window_child.append_attribute("right") = rcWnd.right;
	window_child.append_attribute("bottom") = rcWnd.bottom;
	if (_Account) {
		window_child = window_node.append_child("account_no");
		window_child.append_child(pugi::node_pcdata).set_value(_Account->AccountNo.c_str());
	}
}

void HdAccountPLDlg::LoadFromXml(pugi::xml_node& node)
{

}

void HdAccountPLDlg::Save(simple::file_ostream<same_endian_type>& ss)
{
	CRect rcWnd;
	GetWindowRect(rcWnd);
	// 주문창 크기 및 위치 저장
	ss << rcWnd.left << rcWnd.top << rcWnd.right << rcWnd.bottom;
	if (_Account) {
		ss << true;
		ss << _Account->AccountNo;
	}
	else {
		ss << false;
	}
}

void HdAccountPLDlg::Load(simple::file_istream<same_endian_type>& ss)
{
	CRect rcWnd;
	ss >> rcWnd.left >> rcWnd.top >> rcWnd.right >> rcWnd.bottom;
	bool exist = false;
	ss >> exist;
	if (exist) {
		ss >> _DefaultAccount;
		SetDefaultAccount();
	}
	MoveWindow(rcWnd);
	ShowWindow(SW_SHOW);
}

void HdAccountPLDlg::InitAccount()
{
	_ComboAccount.ResetContent();
	VtGlobal* global = VtGlobal::GetInstance();
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	
	std::string acntName;
	std::map<std::string, int> comboMap;
	for (auto it = acntMgr->AccountMap.begin(); it != acntMgr->AccountMap.end(); ++it) {
		VtAccount* acnt = it->second;
		acntName = acnt->AccountNo;
		acntName.append(_T(" "));
		acntName.append(acnt->AccountName);
		int index = _ComboAccount.AddString(acntName.c_str());
		_ComboAccount.SetItemDataPtr(index, acnt);
		comboMap[acnt->AccountNo] = index;
		if (acnt->AccountLevel() == 0) {
			std::vector<VtAccount*>& subAcntList = acnt->GetSubAccountList();
			for (auto it = subAcntList.begin(); it != subAcntList.end(); ++it) {
				VtAccount* subAcnt = *it;
				acntName = subAcnt->AccountNo;
				acntName.append(_T(" "));
				acntName.append(subAcnt->AccountName);
				index = _ComboAccount.AddString(acntName.c_str());
				_ComboAccount.SetItemDataPtr(index, subAcnt);
				comboMap[subAcnt->AccountNo] = index;
			}
		}
	}
	
	if (comboMap.size() == 0)
		return;
	
	int selAcnt = 0;
	auto it = comboMap.find(_DefaultAccount);
	it == comboMap.end() ? selAcnt = 0 : selAcnt = it->second;
	_ComboAccount.SetCurSel(selAcnt);
	_Account = (VtAccount*)_ComboAccount.GetItemDataPtr(selAcnt);
	if (_Account->hasValidPassword())
		_Account->GetAccountInfoNFee(1);
}


void HdAccountPLDlg::Account(VtAccount* val)
{
	_Account = val;
}

void HdAccountPLDlg::SetDefaultAccount()
{
	InitAccount();
	_AccountGrid.ClearValues();
	_ProductGrid.ClearValues();

	_AccountGrid.InitGrid();
	_ProductGrid.InitGrid();
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
		if (acnt && acnt->AccountLevel() == 0 && acnt->hasValidPassword())
			acnt->GetApiCustomerProfitLoss();
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
