// VtOrderLeftHd.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SMTrader.h"
#include "VtOrderLeftWndHd.h"
#include "VtOrderWndHd.h"
#include "afxdialogex.h"
#include "VtOrderConfigManager.h"
#include "VtHdClient.h"
#include <string>

#include "VtProductCategoryManager.h"
#include "VtProductSection.h"
#include "VtProductSubSection.h"
#include "VtSymbol.h"
#include "VtGlobal.h"
#include "Poco/NumberFormatter.h"
using Poco::NumberFormatter;

extern TApplicationFont g_Font;

VtOrderLeftWndHd::VtOrderLeftWndHd()
{
	//_OrderConfigMgr = new VtOrderConfigManager();
	//_OrderConfigMgr->SetDefaultAccount();
	//_OrderConfigMgr->SetDefaultSymbol();
	_OrderConfigMgr = nullptr;
	_FutureSymbolMode = 1;
	_Mode = 1;

	_EventSeq = 0;
	//_DefaultWidth = VtGlobal::GetHorWidthByScaleFactor(174);
	_DefaultHeight = VtGlobal::GetVerHeightByScaleFactor(774);
}

// VtOrderLeftHd dialog

IMPLEMENT_DYNAMIC(VtOrderLeftWndHd, CDialog)
/*
VtOrderLeftWndHd::VtOrderLeftWndHd(CWnd* pParent )
	: CDialogEx(IDD_ORDER_LEFT_HD, pParent)
{

}
*/
VtOrderLeftWndHd::~VtOrderLeftWndHd()
{
	/*
	if (_OrderConfigMgr)
	{
		delete _OrderConfigMgr;
		_OrderConfigMgr = nullptr;
	}
	*/
}

int VtOrderLeftWndHd::CRHGetDialogID()
{
	return (IDD_ORDER_LEFT_HD);
}

void VtOrderLeftWndHd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PRODUCT, _ComboProduct);
	DDX_Control(pDX, IDC_COMBO_OPTION, _ComboOptionMonth);
	DDX_Control(pDX, IDC_CUSTOM_OPTION, _SymbolOptionGrid);
	DDX_Control(pDX, IDC_CUSTOM_FUTURE, _SymbolFutureGrid);
	DDX_Control(pDX, IDC_CUSTOM_PL, _ProfitLossGrid);
	DDX_Control(pDX, IDC_CUSTOM_ASSET, _AssetGrid);
}


BEGIN_MESSAGE_MAP(VtOrderLeftWndHd, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_PRODUCT, &VtOrderLeftWndHd::OnCbnSelchangeComboProduct)
	ON_CBN_SELCHANGE(IDC_COMBO_OPTION, &VtOrderLeftWndHd::OnCbnSelchangeComboOption)
	//ON_BN_CLICKED(IDC_BUTTON2, &VtOrderLeftWndHd::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_RADIO_BALANCE, &VtOrderLeftWndHd::OnBnClickedRadioBalance)
	ON_BN_CLICKED(IDC_RADIO_CURRENT, &VtOrderLeftWndHd::OnBnClickedRadioCurrent)
	ON_BN_CLICKED(IDC_RADIO_EXPECT, &VtOrderLeftWndHd::OnBnClickedRadioExpect)
END_MESSAGE_MAP()


// VtOrderLeftHd message handlers


BOOL VtOrderLeftWndHd::OnInitDialog()
{
	CRHGenericChildDialog::OnInitDialog();
	::EnumChildWindows(m_hWnd, ::SetChildFont, (LPARAM)g_Font.GetFont());
	_SymbolOptionGrid.LeftWnd(this);
	_SymbolOptionGrid.OrderConfigMgr(_OrderConfigMgr);

	_SymbolFutureGrid.OrderConfigMgr(_OrderConfigMgr);
	_SymbolFutureGrid.Mode(_FutureSymbolMode);
	// TODO:  Add extra initialization here
	//_SymbolFutureGrid.AttachGrid(this, IDC_SYMBOL_FUTURE);
	_ProfitLossGrid.OrderConfigMgr(_OrderConfigMgr);
	//_ProfitLossGrid.AttachGrid(this, IDC_PROFIT_LOSS);
	_AssetGrid.OrderConfigMgr(_OrderConfigMgr);
	//_AssetGrid.AttachGrid(this, IDC_ASSET);
	if (_OrderConfigMgr->Type() == 0) {
		_AssetGrid.ShowWindow(SW_SHOW);
	}
	else {
		_AssetGrid.ShowWindow(SW_HIDE);
	}
	//_SymbolOptionGrid.AttachGrid(this, IDC_SYMBOL_OPTION);
	_SymbolOptionGrid.Init();
	_SymbolFutureGrid.Init();
	_ProfitLossGrid.Init();
	_AssetGrid.Init();
	if (_FutureSymbolMode == 0)
	{
		((CButton*)GetDlgItem(IDC_RADIO_BALANCE))->SetCheck(BST_CHECKED);
	}
	else if (_FutureSymbolMode == 1)
	{
		((CButton*)GetDlgItem(IDC_RADIO_CURRENT))->SetCheck(BST_CHECKED);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO_EXPECT))->SetCheck(BST_CHECKED);
	}

	CRect rcRect;
	_ComboProduct.GetClientRect(rcRect);

	_ComboProduct.GetWindowRect(&rcRect);
	ScreenToClient(&rcRect);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void VtOrderLeftWndHd::OnReceiveQuote(VtSymbol* sym)
{
	_SymbolFutureGrid.OnReceiveQuote(sym);
	_SymbolOptionGrid.OnReceiveQuote(sym);
	_ProfitLossGrid.OnReceiveQuote(sym);
}

void VtOrderLeftWndHd::OnOutstanding()
{
	_ProfitLossGrid.OnOutstanding();
}

void VtOrderLeftWndHd::OnExpected(VtSymbol* sym)
{
	_SymbolFutureGrid.OnExpected(sym);
	_SymbolOptionGrid.OnExpected(sym);
}

void VtOrderLeftWndHd::BlockEvent()
{
	_SymbolFutureGrid.UnregisterAllCallback();
	_SymbolOptionGrid.UnregisterAllCallback();
}

void VtOrderLeftWndHd::Begin()
{

}

void VtOrderLeftWndHd::End()
{

}


void VtOrderLeftWndHd::OnCbnSelchangeComboProduct()
{
	// TODO: Add your control notification handler code here
	_SymbolOptionGrid.Mode(_Mode);
	_SymbolOptionGrid.SetProductSection();
	_SymbolOptionGrid.SetYearMonth();
	_SymbolOptionGrid.InitGrid();
	if (_Mode == 1)
		_SymbolOptionGrid.GetSymbolMaster();
}


void VtOrderLeftWndHd::OnCbnSelchangeComboOption()
{
	// TODO: Add your control notification handler code here
	_SymbolOptionGrid.Mode(_Mode);
	_SymbolOptionGrid.InitGrid();
	if (_Mode == 1)
		_SymbolOptionGrid.GetSymbolMaster();
}


void VtOrderLeftWndHd::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	VtHdClient* client = VtHdClient::GetInstance();
	std::string fileName = _T("product.cod");
	client->GetMasterFile(fileName);
}


void VtOrderLeftWndHd::OnBnClickedRadioBalance()
{
	_Mode = 0;
	_SymbolFutureGrid.Mode(0);
	_SymbolFutureGrid.InitGrid();
	_SymbolOptionGrid.Mode(0);
	_SymbolOptionGrid.RefreshMode();
}


void VtOrderLeftWndHd::OnBnClickedRadioCurrent()
{
	// TODO: Add your control notification handler code here
	_Mode = 1;
	_SymbolFutureGrid.Mode(1);
	_SymbolFutureGrid.InitGrid();
	_SymbolOptionGrid.Mode(1);
	_SymbolOptionGrid.RefreshMode();
	_SymbolOptionGrid.GetSymbolMaster();
	_SymbolOptionGrid.SetCurrent2();
}


void VtOrderLeftWndHd::OnBnClickedRadioExpect()
{
	// TODO: Add your control notification handler code here
	_Mode = 2;
	_SymbolFutureGrid.Mode(2);
	_SymbolFutureGrid.InitGrid();
	_SymbolOptionGrid.Mode(2);
	_SymbolOptionGrid.RefreshMode();
}

void VtOrderLeftWndHd::OnSymbolMaster(VtSymbol* sym)
{
	if (_Mode != 1)
		return;

	_SymbolOptionGrid.OnSymbolMaster(sym);
}

void VtOrderLeftWndHd::OnRemain(VtPosition* posi)
{
	CString msg;
	msg.Format(_T("OnRemain : EventSeq = %d\n"), _EventSeq++);
	//TRACE(msg);

	//_SymbolOptionGrid.SetRemain(posi);
	//_SymbolFutureGrid.SetRemain(posi);
}

void VtOrderLeftWndHd::OnOrderAccepted(VtOrder* order)
{
	OnOrderEvent(order);
}

void VtOrderLeftWndHd::OnOrderUnfilled(VtOrder* order)
{
	OnOrderEvent(order);
}

void VtOrderLeftWndHd::OnOrderFilled(VtOrder* order)
{
	OnOrderEvent(order);
}

void VtOrderLeftWndHd::OnReceiveAccountInfo()
{
	_AssetGrid.OnReceiveAccountInfo();
	_ProfitLossGrid.OnReceiveAccountInfo();
}

void VtOrderLeftWndHd::RefreshProfitLoss()
{
	_ProfitLossGrid.InitGrid();
}

void VtOrderLeftWndHd::RefreshAsset()
{
	if (_AssetGrid.GetSafeHwnd())
		_AssetGrid.InitGrid();
}

void VtOrderLeftWndHd::OnResizeWnd()
{
	CRect rcWnd;
	if (GetSafeHwnd()) {
		if (!_OrderConfigMgr->_HdCenterWnd->GetSafeHwnd())
			return;
		_OrderConfigMgr->_HdOrderWnd->GetWindowRect(&rcWnd);
		CRect rcGrid;
		_SymbolOptionGrid.GetWindowRect(rcGrid);
		_SymbolOptionGrid.SetWindowPos(nullptr, 0, 0, rcGrid.Width(), rcWnd.Height() - 313, SWP_NOMOVE);
		_SymbolOptionGrid.InitGrid(rcWnd.Height() - 313);
		_SymbolOptionGrid.GetSymbolMaster();
		_SymbolFutureGrid.InitGrid();
		_ProfitLossGrid.InitGrid();
		_AssetGrid.InitGrid();
	}
}

void VtOrderLeftWndHd::OnAccountChanged()
{
	//_SymbolFutureGrid.InitGrid();
	//_SymbolOptionGrid.InitGrid();
	CRect rcWnd;
	if (GetSafeHwnd()) {
		if (!_OrderConfigMgr->_HdCenterWnd->GetSafeHwnd())
			return;
		_OrderConfigMgr->_HdOrderWnd->GetWindowRect(&rcWnd);
		CRect rcGrid;
		_SymbolOptionGrid.GetWindowRect(rcGrid);
		_SymbolOptionGrid.SetWindowPos(nullptr, 0, 0, rcGrid.Width(), rcWnd.Height() - 313, SWP_NOMOVE);
		_SymbolOptionGrid.InitGrid(rcWnd.Height() - 313);
		_SymbolOptionGrid.GetSymbolMaster();
		_SymbolFutureGrid.InitGrid();
		_ProfitLossGrid.InitGrid();
		_AssetGrid.InitGrid();
	}
}

void VtOrderLeftWndHd::OnOrderEvent(VtOrder* order)
{
	if (!order || !_OrderConfigMgr)
		return;
	std::string code = order->shortCode.substr(0, 1);
	if ((code.compare(_T("2")) == 0) || (code.compare(_T("3")) == 0)) {
		_SymbolOptionGrid.SetRemain(order);
	} else {
		_SymbolFutureGrid.SetRemain(order);
	}
}

