// HdOrderDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "HdOrderDialog.h"
#include "afxdialogex.h"
#include "VtTotalOrderManager.h"
#include "VtOrder.h"

// HdOrderDialog dialog

IMPLEMENT_DYNAMIC(HdOrderDialog, CDialogEx)

HdOrderDialog::HdOrderDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ORDER_TOTAL, pParent)
{

}

HdOrderDialog::~HdOrderDialog()
{
}

void HdOrderDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_SHOW_CANCEL_ORDER, _CheckShowCancelOrders);
	DDX_Control(pDX, IDC_CHECK_SHOW_REAL_ACCOUNT_ONLY, _CheckShowRealAccountOnly);
	DDX_Control(pDX, IDC_COMBO_ACCOUNT, _ComboAccount);
	DDX_Control(pDX, IDC_COMBO_SYMBOL, _ComboSymbol);
	DDX_Control(pDX, IDC_EDIT_ACCOUNT_NAME, _EditAccountName);
}


BEGIN_MESSAGE_MAP(HdOrderDialog, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_ACCOUNT, &HdOrderDialog::OnCbnSelchangeComboAccount)
	ON_CBN_SELCHANGE(IDC_COMBO_SYMBOL, &HdOrderDialog::OnCbnSelchangeComboSymbol)
	ON_BN_CLICKED(IDC_CHECK_SHOW_CANCEL_ORDER, &HdOrderDialog::OnBnClickedCheckShowCancelOrder)
	ON_BN_CLICKED(IDC_CHECK_SHOW_REAL_ACCOUNT_ONLY, &HdOrderDialog::OnBnClickedCheckShowRealAccountOnly)
	ON_BN_CLICKED(IDC_MFCBUTTON_STATE, &HdOrderDialog::OnBnClickedMfcbuttonState)
END_MESSAGE_MAP()


// HdOrderDialog message handlers
std::vector<VtOrder*> HdOrderDialog::FindByAccount(std::pair<int, std::string> acnt)
{
	std::vector<VtOrder*> orderList;
	int type = std::get<0>(acnt);
	std::string acntNo = std::get<1>(acnt);
	VtTotalOrderManager* totalOrderMgr = VtTotalOrderManager::GetInstance();
	std::map<int, VtOrder*>& orderMap = totalOrderMgr->GetOrderMap();
	for (auto it = orderMap.rbegin(); it != orderMap.rend(); ++it) {
		VtOrder* order = it->second;
		if (type == 0) {
			if (order->AccountNo.compare(acntNo) == 0)
				orderList.push_back(order);
		}
		else if (type == 1) {
			if (order->SubAccountNo.compare(acntNo) == 0)
				orderList.push_back(order);
		}
		else if (type == 2) {
			if (order->FundName.compare(acntNo) == 0)
				orderList.push_back(order);
		}
	}
	return orderList;
}

std::vector<VtOrder*> HdOrderDialog::FindBySymbol(std::string symbolCode)
{
	std::vector<VtOrder*> orderList;
	VtTotalOrderManager* totalOrderMgr = VtTotalOrderManager::GetInstance();
	std::map<int, VtOrder*>& orderMap = totalOrderMgr->GetOrderMap();
	for (auto it = orderMap.rbegin(); it != orderMap.rend(); ++it) {
		VtOrder* order = it->second;
		if (order->shortCode.compare(symbolCode) == 0)
			orderList.push_back(order);
	}
	return orderList;
}

BOOL HdOrderDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	VtTotalOrderManager* totalOrderMgr = VtTotalOrderManager::GetInstance();
	std::map<int, VtOrder*>& orderMap = totalOrderMgr->GetOrderMap();
	for (auto it = orderMap.begin(); it != orderMap.end(); ++it) {
		VtOrder* order = it->second;
		_AccountSet.insert(std::make_pair(0, order->AccountNo));
		if (order->SubAccountNo.length() > 0)
			_AccountSet.insert(std::make_pair(1, order->SubAccountNo));
		if (order->FundName.length() > 0)
			_AccountSet.insert(std::make_pair(2, order->FundName));
		_SymbolSet.insert(order->shortCode);
	}
	std::string title = _T("모두");
	int index = _ComboAccount.AddString(title.c_str());
	_AccountComboMap[index] = std::make_pair(-1, title);
	for (auto it = _AccountSet.begin(); it != _AccountSet.end(); ++it) {
		index = _ComboAccount.AddString(std::get<1>(*it).c_str());
		_AccountComboMap[index] = *it;
	}
	_ComboAccount.SetCurSel(0);

	title = _T("모두");
	index = _ComboSymbol.AddString(title.c_str());
	_SymbolComboMap[index] = title;
	for (auto it = _SymbolSet.begin(); it != _SymbolSet.end(); ++it) {
		index = _ComboSymbol.AddString((*it).c_str());
		_SymbolComboMap[index] = *it;
	}
	_ComboSymbol.SetCurSel(0);

	// TODO:  Add extra initialization here
	_OrderGrid.AttachGrid(this, IDC_STATIC_ORDER);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void HdOrderDialog::OnCbnSelchangeComboAccount()
{
	int sel = _ComboAccount.GetCurSel();
	if (sel < 0)
		return;
	auto acnt = _AccountComboMap[sel];
	std::vector<VtOrder*> orderList = FindByAccount(acnt);
	VtTotalOrderManager* totalOrderMgr = VtTotalOrderManager::GetInstance();
	_OrderGrid.ClearCells(totalOrderMgr->GetOrderCount());
	_OrderGrid.InitGrid(std::move(orderList));
}


void HdOrderDialog::OnCbnSelchangeComboSymbol()
{
	int sel = _ComboSymbol.GetCurSel();
	if (sel < 0)
		return;
	auto sym = _SymbolComboMap[sel];
	std::vector<VtOrder*> orderList = FindBySymbol(sym);
	VtTotalOrderManager* totalOrderMgr = VtTotalOrderManager::GetInstance();
	_OrderGrid.ClearCells(totalOrderMgr->GetOrderCount());
	_OrderGrid.InitGrid(std::move(orderList));
}


void HdOrderDialog::OnBnClickedCheckShowCancelOrder()
{
	// TODO: Add your control notification handler code here
}


void HdOrderDialog::OnBnClickedCheckShowRealAccountOnly()
{
	// TODO: Add your control notification handler code here
}


void HdOrderDialog::OnBnClickedMfcbuttonState()
{
	// TODO: Add your control notification handler code here
}
