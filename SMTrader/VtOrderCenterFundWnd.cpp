// VtOrderFundCenterWnd.cpp : implementation file
//
#include "resource.h"
#include "stdafx.h"
#include "SMTrader.h"
#include "VtOrderCenterFundWnd.h"
#include "afxdialogex.h"
#include "VtOrderDialogManager.h"
#include "VtSymbol.h"
#include "VtAccount.h"
#include "VtOrderManagerSelector.h"
#include "VtRealtimeRegisterManager.h"
#include "VtAccountManager.h"
#include "VtGlobal.h"
#include "VtOrderManager.h"
#include "VtProductOrderManager.h"
#include "VtProductOrderManagerSelector.h"
#include "VtProductCategoryManager.h"
#include "VtProductCategory.h"
#include "VtProductSection.h"
#include "VtProductSubSection.h"
#include "HdSymbolSelecter.h"
#include <Poco/NumberParser.h>
#include "VtOrderWndHd.h"
#include "VtOrderConfigDlg.h"
#include "VtOrderGridConfig.h"
#include "HdScheduler.h"
#include "VtSymbolManager.h"
#include "VtSymbol.h"
#include "VtSaveManager.h"
#include "VtFund.h"
using Poco::NumberParser;

IMPLEMENT_DYNAMIC(VtOrderCenterFundWnd, CDialog)


VtOrderCenterFundWnd::VtOrderCenterFundWnd(CWnd* pParent)
	: CDialog(IDD_ORDER_CENTER_HD, pParent)
{
	_Fund = nullptr;
	_ParentDlg = nullptr;
	_Symbol = nullptr;
	_Account = nullptr;
	_OrderAmount = 1;
	_OrderMgr = nullptr;
	_FillCondition = VtFilledCondition::Fas;
	_PriceType = VtPriceType::Price;
	_PrdtOrderMgr = nullptr;
	_FixedCenter = false;
	_EnableCutProfit = false;
	_EnableCutLoss = false;
	_CutProfit = 5;
	_CutLoss = 5;
	_OrderType = 1;
	_OrderTypeSlip = 0;
	_StopVal = 2;
	_Activated = false;
	_RealTickWnd = nullptr;
	_ConfigDlg = nullptr;
	_Init = false;
	_ShowRealTickWndMode = 0;
	x = 0;
	y = 0;
	width = 0;
	height = 0;
	_GridGap = 0;
}

VtOrderCenterFundWnd::~VtOrderCenterFundWnd()
{
	if (_ConfigDlg)
	{
		_ConfigDlg->DestroyWindow();
		delete _ConfigDlg;
		_ConfigDlg = nullptr;
	}

	ClearConfigDlg();
}

void VtOrderCenterFundWnd::OrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
	_OrderPanelGrid.OrderConfigMgr(_OrderConfigMgr);
	_ProductRemainGrid.OrderConfigMgr(_OrderConfigMgr);
}

void VtOrderCenterFundWnd::ResizeOrderGrid(int maxHeight)
{
	if (!_Init)
		return;

	_OrderPanelGrid.ResizeGrid(maxHeight);
}

void VtOrderCenterFundWnd::SetFund(VtFund* fund)
{
	if (!fund)
		return;
	_Fund = fund;
}

void VtOrderCenterFundWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PRODUCT_HD, _ComboSymbol);
	DDX_Control(pDX, IDC_STATIC_MSG, _StaticMsg);
	DDX_Control(pDX, IDC_EDIT_ORDER_AMOUNT, _EditOrderAmount);
	DDX_Control(pDX, IDC_SPIN_ORDER_AMOUNT, _ScOrderAmount);
	DDX_Control(pDX, IDC_SPIN_STOPVAL, _ScStopVal);
	DDX_Control(pDX, IDC_EDIT_STOPVAL, _EditStopVal);
	DDX_Control(pDX, IDC_BTN_AVAIL, _BtnAvail);
	DDX_Control(pDX, IDC_BTN_REMAIN, _BtnRemain);
	DDX_Control(pDX, IDC_STATIC_PRODUCT_NAME, _StaticProductName);
	DDX_Control(pDX, IDC_BTN_ORDER_AMT1, _BtnAmt1);
	DDX_Control(pDX, IDC_BTN_ORDER_AMT2, _BtnAmt2);
	DDX_Control(pDX, IDC_BTN_ORDER_AMT3, _BtnAmt3);
	DDX_Control(pDX, IDC_BTN_ORDER_AMT4, _BtnAmt4);
	DDX_Control(pDX, IDC_BTN_ORDER_AMT5, _BtnAmt5);
	DDX_Control(pDX, IDC_BTN_ORDER_AMT6, _BtnAmt6);
	DDX_Control(pDX, IDC_EDIT_AMT, _EditAmt);
	DDX_Control(pDX, IDC_STATIC_STOP, _StaticStop);
	DDX_Control(pDX, IDC_CK_FIXED_CENTER, _CheckFixedCenter);
}


BEGIN_MESSAGE_MAP(VtOrderCenterFundWnd, CDialog)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_PRODUCT_HD, &VtOrderCenterWndHd::OnCbnSelchangeComboProductHd)
	ON_BN_CLICKED(IDC_CK_FIXED_CENTER, &VtOrderCenterWndHd::OnBnClickedCkFixedCenter)
	ON_BN_CLICKED(IDC_BTN_ORDER_AMT1, &VtOrderCenterWndHd::OnBnClickedBtnOrderAmt1)
	ON_BN_CLICKED(IDC_BTN_ORDER_AMT2, &VtOrderCenterWndHd::OnBnClickedBtnOrderAmt2)
	ON_BN_CLICKED(IDC_BTN_ORDER_AMT3, &VtOrderCenterWndHd::OnBnClickedBtnOrderAmt3)
	ON_BN_CLICKED(IDC_BTN_ORDER_AMT4, &VtOrderCenterWndHd::OnBnClickedBtnOrderAmt4)
	ON_BN_CLICKED(IDC_BTN_ORDER_AMT5, &VtOrderCenterWndHd::OnBnClickedBtnOrderAmt5)
	ON_BN_CLICKED(IDC_BTN_ORDER_AMT6, &VtOrderCenterWndHd::OnBnClickedBtnOrderAmt6)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ORDER_AMOUNT, &VtOrderCenterWndHd::OnDeltaposSpinOrderAmount)
	ON_EN_CHANGE(IDC_EDIT_ORDER_AMOUNT, &VtOrderCenterWndHd::OnEnChangeEditOrderAmount)
	ON_EN_CHANGE(IDC_EDIT_STOPVAL, &VtOrderCenterWndHd::OnEnChangeEditStopval)
	ON_BN_CLICKED(IDC_BTN_LIQ, &VtOrderCenterWndHd::OnBnClickedBtnLiq)
	ON_BN_CLICKED(IDC_BTN_REMAIN, &VtOrderCenterWndHd::OnBnClickedBtnRemain)
	ON_BN_CLICKED(IDC_BTN_SEL_SYMBOL, &VtOrderCenterWndHd::OnBnClickedBtnSelSymbol)
	ON_WM_PAINT()
	//ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_EXPAND, &VtOrderCenterWndHd::OnDeltaposSpinExpand)
	ON_STN_CLICKED(IDC_STATIC_MSG, &VtOrderCenterWndHd::OnStnClickedStaticMsg)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_STATIC_FILL_PL, &VtOrderCenterWndHd::OnBnClickedStaticFillPl)
	ON_BN_CLICKED(IDC_BUTTON_SETTING, &VtOrderCenterWndHd::OnBnClickedButtonSetting)
	ON_BN_CLICKED(IDC_BUTTON_PROFIT_LOSS, &VtOrderCenterWndHd::OnBnClickedButtonProfitLoss)
	ON_STN_CLICKED(IDC_STATIC_ORDER_PANEL, &VtOrderCenterWndHd::OnStnClickedStaticOrderPanel)
	ON_NOTIFY(NM_RCLICK, IDC_BTN_ORDER_AMT1, OnRClicked)
	ON_NOTIFY(NM_RCLICK, IDC_BTN_ORDER_AMT2, OnRClicked)
	ON_NOTIFY(NM_RCLICK, IDC_BTN_ORDER_AMT3, OnRClicked)
	ON_NOTIFY(NM_RCLICK, IDC_BTN_ORDER_AMT4, OnRClicked)
	ON_NOTIFY(NM_RCLICK, IDC_BTN_ORDER_AMT5, OnRClicked)
	ON_NOTIFY(NM_RCLICK, IDC_BTN_ORDER_AMT6, OnRClicked)
	ON_STN_CLICKED(IDC_STATIC_REAL_TICK, &VtOrderCenterWndHd::OnStnClickedStaticRealTick)
END_MESSAGE_MAP()


// VtOrderCenterHd message handlers


void VtOrderCenterFundWnd::InitInfo()
{
	_OrderPanelGrid.InitInfo();
}

void VtOrderCenterFundWnd::InitAll()
{
	InitAccount();
	InitSymbol();
	InitPosition();
	RegisterSymbolWnd();
	RegisterOrderWnd();
	RegisterRealtimeSymbol();
	RegisterRealtimeAccount();
	InitInfo();
	_Init = true;
}

void VtOrderCenterFundWnd::InitByLoading(int height)
{
	InitPosition();
	RegisterSymbolWnd();
	RegisterOrderWnd();
	RegisterRealtimeSymbol();
	RegisterRealtimeAccount();
	_OrderPanelGrid.ResizeGrid(height);
	_Init = true;
}

BOOL VtOrderCenterFundWnd::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowPos(NULL, x, y, width, height, SWP_NOZORDER);

	_BtnAmt1.SetFlat(FALSE);
	_BtnAmt2.SetFlat(FALSE);
	_BtnAmt3.SetFlat(FALSE);
	_BtnAmt4.SetFlat(FALSE);
	_BtnAmt5.SetFlat(FALSE);
	_BtnAmt6.SetFlat(FALSE);

	m_brush.CreateSolidBrush(RGB(255, 255, 255));

	_ScOrderAmount.SetRange32(1, 100);
	_ScStopVal.SetRange32(0, 100);

	CString strValue;
	strValue.Format(_T("%d"), _CutLoss);

	strValue.Format(_T("%d"), _CutProfit);

	strValue.Format(_T("%d"), _OrderAmount);
	_EditOrderAmount.SetWindowText(strValue);

	strValue.Format(_T("%d"), _StopVal);
	_EditStopVal.SetWindowText(strValue);

	//_OrderPanelGrid.CenterWnd(this);
	//_ProductRemainGrid.CenterWnd(this);
	_OrderPanelGrid.AttachGrid(this, IDC_STATIC_ORDER_PANEL);
	_ProductRemainGrid.AttachGrid(this, IDC_STATIC_PRODUCT_REMAIN);
	_TickGrid.AttachGrid(this, IDC_STATIC_REAL_TICK);

	_ConfigDlg = new VtOrderConfigDlg();
	//_ConfigDlg->CenterWnd(this);
	CreateChildWindow(_ConfigDlg, IDD_CENTER_CONFIG, this);
	CRect rcConfig, rcWnd;
	GetWindowRect(rcWnd);
	_ConfigDlg->GetWindowRect(rcConfig);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void VtOrderCenterFundWnd::ClearConfigDlg()
{
	for (auto it = _ConfigDlgVector.begin(); it != _ConfigDlgVector.end(); ++it)
	{
		VtOrderGridConfig* dlg = *it;
		dlg->DestroyWindow();
		delete dlg;
	}

	_ConfigDlgVector.clear();
}

void VtOrderCenterFundWnd::SetProductGridWidth()
{
	CRect rcGrid;
	GetClientRect(rcGrid);
	int level = _ProductRemainGrid.SetColTitleForLevel(rcGrid.Width());
	RePosStopCtrl(level);
}

void VtOrderCenterFundWnd::RePosStopCtrl(int level)
{
	CRect rcGrid, rcCtrl;
	_ProductRemainGrid.GetWindowRect(rcGrid);
	ScreenToClient(rcGrid);

	_CheckFixedCenter.GetWindowRect(rcCtrl);
	ScreenToClient(rcCtrl);
	_CheckFixedCenter.SetWindowPos(nullptr, rcGrid.right + 5, rcCtrl.top, rcCtrl.Width(), rcCtrl.Height(), SWP_NOZORDER);

	CRect editRect;
	_EditStopVal.GetWindowRect(&editRect);
	ScreenToClient(&editRect);
	CWnd * spin = _EditStopVal.GetWindow(GW_HWNDNEXT);
	CRect spinRect;
	spin->GetWindowRect(&spinRect);
	ScreenToClient(&spinRect);
	CSize offset(spinRect.left - editRect.left, spinRect.top - editRect.top);
	_EditStopVal.SetWindowPos(NULL, rcGrid.right + 36, editRect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	spin->SetWindowPos(NULL, rcGrid.right + 36 + offset.cx, editRect.top + offset.cy, 0, 0, SWP_NOSIZE |
		SWP_NOZORDER);

	_StaticStop.GetWindowRect(rcCtrl);
	ScreenToClient(rcCtrl);
	_StaticStop.SetWindowPos(nullptr, rcGrid.right + 5, rcCtrl.top, rcCtrl.Width(), rcCtrl.Height(), SWP_NOZORDER);
}

int VtOrderCenterFundWnd::GetMaxWidthForMode()
{
	return 0;
}

void VtOrderCenterFundWnd::RefreshOrderPositon()
{
	_OrderPanelGrid.RefreshOrderPosition();
}


void VtOrderCenterFundWnd::OnOrderReceivedHd(VtOrder* order)
{
	_OrderPanelGrid.OnOrderReceivedHd(order);
}

void VtOrderCenterFundWnd::OnOrderAcceptedHd(VtOrder* order)
{
	_OrderPanelGrid.OnOrderAcceptedHd(order);
}

void VtOrderCenterFundWnd::OnOrderUnfilledHd(VtOrder* order)
{
	_OrderPanelGrid.OnOrderUnfilledHd(order);
}

void VtOrderCenterFundWnd::OnOrderFilledHd(VtOrder* order)
{
	_ProductRemainGrid.ShowPosition();
	_OrderPanelGrid.OnOrderFilledHd(order);
}

void VtOrderCenterFundWnd::OnRemain(VtPosition* posi)
{
	if (!posi)
		return;
	_ProductRemainGrid.OnRemain(posi);
	GetDeposit(posi);
	RefreshOrderPositon();
}

void VtOrderCenterFundWnd::OnReceiveHoga(VtSymbol* sym)
{
	if (!sym)
		return;
	_OrderPanelGrid.OnReceiveHoga(sym);
}

void VtOrderCenterFundWnd::OnReceiveQuote(VtSymbol* sym)
{
	if (!sym)
		return;
	SetProductName(sym);
	//InitInfo();
	if (_ShowRealTickWndMode != 0)
		_TickGrid.OnReceiveQuote(sym);
	_OrderPanelGrid.OnReceiveQuote(sym);
	_ProductRemainGrid.OnReceiveQuote(sym);
	if (_RealTickWnd)
	{
		_RealTickWnd->OnReceiveQuote(sym);
	}
}


void VtOrderCenterFundWnd::OnSymbolMaster(VtSymbol* sym)
{
	_OrderPanelGrid.OnSymbolMaster(sym);
}

void VtOrderCenterFundWnd::OnReceiveMsg(CString msg)
{
	_StaticMsg.SetWindowText(msg);
}

void VtOrderCenterFundWnd::OnReceiveMsgWithReqId(int id, CString msg)
{
	CString message;
	message.Format(_T("Request ID : %d - %s"), id, msg);
	_StaticMsg.SetWindowText(message);
}

void VtOrderCenterFundWnd::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnClose();
}


void VtOrderCenterFundWnd::OnDestroy()
{
	CDialog::OnDestroy();

	_OrderPanelGrid.ClearAllStopOrders();
	_OrderPanelGrid.CenterWnd(nullptr);
	// TODO: Add your message handler code here
	UnregisterRealtimeSymbol();
	UnregisterRealtimeAccount();
	UnregisterSymbolWnd();
	UnregisterAccountWnd();
}

void VtOrderCenterFundWnd::RepositionControl(int height)
{
	CRect rcConfig, rcWnd, rcGrid;
	GetWindowRect(rcWnd);
	_ConfigDlg->GetWindowRect(rcConfig);
	_OrderPanelGrid.GetWindowRect(rcGrid);
	ScreenToClient(rcGrid);
	ScreenToClient(rcConfig);
	_OrderPanelGrid.SetWindowPos(nullptr, rcGrid.left, rcGrid.top, rcGrid.Width(), (height - rcGrid.top - rcConfig.Height()), SWP_NOMOVE);
	_ConfigDlg->MoveWindow(0, height - rcConfig.Height(), rcConfig.Width(), rcConfig.Height());
	//_ConfigDlg->RecalateLayout();
}

void VtOrderCenterFundWnd::AdjustControlForMode()
{
	CRect  rcGrid, rcTick;
	_OrderPanelGrid.GetWindowRect(rcGrid);
	_TickGrid.GetWindowRect(rcTick);
	ScreenToClient(rcGrid);
	ScreenToClient(rcTick);
	if (_ShowRealTickWndMode == 0)
	{
		_OrderPanelGrid.SetWindowPos(nullptr, 0, rcGrid.top, rcGrid.Width(), rcGrid.Height(), SWP_NOSIZE);
		_TickGrid.SetWindowPos(nullptr, rcGrid.Width() + _GridGap, rcGrid.top, rcGrid.Width(), rcGrid.Height(), SWP_NOSIZE);
	}
	else if (_ShowRealTickWndMode == 1)
	{
		_OrderPanelGrid.SetWindowPos(nullptr, rcTick.Width() + _GridGap, rcGrid.top, rcGrid.Width(), rcGrid.Height(), SWP_NOSIZE);
		_TickGrid.SetWindowPos(nullptr, 0, rcGrid.top, rcGrid.Width(), rcGrid.Height(), SWP_NOSIZE);
	}
	else
	{
		_OrderPanelGrid.SetWindowPos(nullptr, 0, rcGrid.top, rcGrid.Width(), rcGrid.Height(), SWP_NOSIZE);
		_TickGrid.SetWindowPos(nullptr, rcGrid.Width() + _GridGap, rcGrid.top, rcGrid.Width(), rcGrid.Height(), SWP_NOSIZE);
	}
}

void VtOrderCenterFundWnd::GetDeposit(VtPosition* posi)
{
	if (!posi)
		return;

	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	VtAccount* acnt = acntMgr->FindAccount(posi->AccountNo);
	{
		acnt->GetDeposit();
	}
}

int VtOrderCenterFundWnd::GetTickCount()
{
	return _TickGrid.MaxRow();
}

void VtOrderCenterFundWnd::SetShowRealTickWnd(int mode)
{
	_ShowRealTickWndMode = mode;
	if (_OrderConfigMgr && _OrderConfigMgr->_HdOrderWnd)
	{
		_OrderConfigMgr->_HdOrderWnd->RecalcLayout(true);
	}
}

void VtOrderCenterFundWnd::SetShowLeftButtonText(CString text)
{
	//_BtnLeftExtend.SetWindowText(text);
}

void VtOrderCenterFundWnd::SetShowRightButtonText(CString text)
{
	//_BtnRightExtend.SetWindowText(text);
}

void VtOrderCenterFundWnd::SetRemain(int remain)
{
	CString strValue;
	strValue.Format(_T("ÀÜ°í(%d)"), remain);
	_BtnRemain.SetWindowText(strValue);
}

void VtOrderCenterFundWnd::SetSymbol(VtSymbol* sym)
{
	if (!sym)
		return;

	_Symbol = sym;
	_ProductRemainGrid.SetSymbol(sym);
	if (_OrderMgr)
	{
		_OrderMgr->CalcTotalProfitLoss(sym);
		_PrdtOrderMgr = _OrderMgr->FindAddProductOrderManager(sym->ShortCode);
	}
}

void VtOrderCenterFundWnd::SetSymbol()
{
	if (!_Symbol)
		return;

	_ProductRemainGrid.SetSymbol(_Symbol);
	if (_OrderMgr)
	{
		_OrderMgr->CalcTotalProfitLoss(_Symbol);
		_PrdtOrderMgr = _OrderMgr->FindAddProductOrderManager(_Symbol->ShortCode);
		_PrdtOrderMgr->OrderMgr(_OrderMgr);
	}
}

void VtOrderCenterFundWnd::SetAccount(VtAccount* acnt)
{
	if (!acnt)
		return;
	_Account = acnt;

	VtOrderManagerSelector* orderMgrSelecter = VtOrderManagerSelector::GetInstance();
	_OrderMgr = orderMgrSelecter->FindOrderManager(acnt->AccountNo);
}

void VtOrderCenterFundWnd::RegisterSymbolWnd()
{
	if (!_Symbol)
		return;

	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	//orderDlgMgr->AddSymbolWnd(_Symbol->ShortCode, this);
}

void VtOrderCenterFundWnd::UnregisterSymbolWnd()
{
	if (!_Symbol)
		return;
	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	//orderDlgMgr->RemoveSymbolWnd(_Symbol->ShortCode, this);
}

void VtOrderCenterFundWnd::UnregisterAccountWnd()
{
	if (!_Account)
		return;
	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	//orderDlgMgr->RemoveAccountWnd(_Account->AccountNo, this);
}

void VtOrderCenterFundWnd::RegisterRealtimeSymbol()
{
	if (!_Symbol)
		return;
	VtRealtimeRegisterManager* realtimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	realtimeRegiMgr->RegisterProduct(_Symbol->ShortCode);
}

void VtOrderCenterFundWnd::UnregisterRealtimeSymbol()
{
	if (!_Symbol)
		return;
	VtRealtimeRegisterManager* realtimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	realtimeRegiMgr->UnregisterProduct(_Symbol->ShortCode);
}

void VtOrderCenterFundWnd::RegisterRealtimeAccount()
{
	if (!_Account)
		return;
	VtRealtimeRegisterManager* realtimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	realtimeRegiMgr->RegisterAccount(_Account->AccountNo);
}

void VtOrderCenterFundWnd::UnregisterRealtimeAccount()
{
	if (!_Account)
		return;
	VtRealtimeRegisterManager* realtimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	realtimeRegiMgr->UnregisterAccount(_Account->AccountNo);
}


void VtOrderCenterFundWnd::RegisterOrderWnd()
{
	if (!_Account || !_Symbol)
		return;

	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	std::string key = _Account->AccountNo;
	key.append(_T(":"));
	key.append(_Symbol->ShortCode);
	//orderDlgMgr->AddOrderWnd(key, this);

	if (_OrderMgr)
	{
		_PrdtOrderMgr = _OrderMgr->FindAddProductOrderManager(_Symbol->ShortCode);
		_PrdtOrderMgr->OrderMgr(_OrderMgr);
	}
}

void VtOrderCenterFundWnd::UnregisterOrderWnd()
{
	if (!_Account || !_Symbol)
		return;

	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	std::string key = _Account->AccountNo;
	key.append(_T(":"));
	key.append(_Symbol->ShortCode);
	//orderDlgMgr->RemoveOrderWnd(key, this);
}

void VtOrderCenterFundWnd::OnCbnSelchangeComboProductHd()
{
	//ChangeSymbol();
	int curSel = _ComboSymbol.GetCurSel();
	if (curSel != -1)
	{
		VtSymbol* sym = (VtSymbol*)_ComboSymbol.GetItemDataPtr(curSel);
		if (!sym)
			return;
		ChangeSymbol(sym);
		sym->GetSymbolMaster();
	}
}

void VtOrderCenterFundWnd::ClearPosition()
{
	_ProductRemainGrid.ClearPosition();
}


void VtOrderCenterFundWnd::InitAccount()
{
	if (_OrderConfigMgr)
	{
		SetAccount(_OrderConfigMgr->Account());
	}
}


void VtOrderCenterFundWnd::InitSymbol()
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	std::string secName = prdtCatMgr->MainFutureVector.front();
	VtProductSection* section = prdtCatMgr->FindProductSection(secName);
	if (section)
	{

		if (section->SubSectionVector.size() > 0)
		{
			VtProductSubSection* subSection = section->SubSectionVector.front();
			if (subSection->_SymbolVector.size() > 0)
			{
				VtSymbol* sym = subSection->_SymbolVector.front();
				SetSymbol(sym);
				AddSymbolToCombo(sym);
				SetProductName(sym);
			}
		}
	}
}

void VtOrderCenterFundWnd::InitPosition()
{
	_ProductRemainGrid.InitPosition();
}

void VtOrderCenterFundWnd::InitOrder()
{

}

void VtOrderCenterFundWnd::OnBnClickedCkFixedCenter()
{
	// TODO: Add your control notification handler code here
	_FixedCenter = !_FixedCenter;
	_OrderPanelGrid.InitInfo();
}


void VtOrderCenterFundWnd::OnBnClickedBtnOrderAmt1()
{
	// TODO: Add your control notification handler code here
	CString strText;
	_BtnAmt1.GetWindowText(strText);
	_EditOrderAmount.SetWindowText(strText);
	_OrderAmount = _ttoi(strText);
}


void VtOrderCenterFundWnd::OnBnClickedBtnOrderAmt2()
{
	// TODO: Add your control notification handler code here
	CString strText;
	_BtnAmt2.GetWindowText(strText);
	_EditOrderAmount.SetWindowText(strText);
	_OrderAmount = _ttoi(strText);
}


void VtOrderCenterFundWnd::OnBnClickedBtnOrderAmt3()
{
	// TODO: Add your control notification handler code here
	CString strText;
	_BtnAmt3.GetWindowText(strText);
	_EditOrderAmount.SetWindowText(strText);
	_OrderAmount = _ttoi(strText);
}


void VtOrderCenterFundWnd::OnBnClickedBtnOrderAmt4()
{
	// TODO: Add your control notification handler code here
	CString strText;
	_BtnAmt4.GetWindowText(strText);
	_EditOrderAmount.SetWindowText(strText);
	_OrderAmount = _ttoi(strText);
}


void VtOrderCenterFundWnd::OnBnClickedBtnOrderAmt5()
{
	// TODO: Add your control notification handler code here
	CString strText;
	_BtnAmt5.GetWindowText(strText);
	_EditOrderAmount.SetWindowText(strText);
	_OrderAmount = _ttoi(strText);
}


void VtOrderCenterFundWnd::OnBnClickedBtnOrderAmt6()
{
	// TODO: Add your control notification handler code here
	CString strText;
	_BtnAmt6.GetWindowText(strText);
	_EditOrderAmount.SetWindowText(strText);
	_OrderAmount = _ttoi(strText);
}


void VtOrderCenterFundWnd::OnDeltaposSpinOrderAmount(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void VtOrderCenterFundWnd::OnEnChangeEditOrderAmount()
{
	// TODO:  Add your control notification handler code here
	if (!_EditOrderAmount.GetSafeHwnd())
		return;

	CString strAmount;
	_EditOrderAmount.GetWindowText(strAmount);
	std::string amt = strAmount;
	if (!NumberParser::tryParse(amt, _OrderAmount))
	{
		return;
	}
}


void VtOrderCenterFundWnd::OnEnChangeEditStopval()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CRHGenericChildDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (!_EditStopVal.GetSafeHwnd())
		return;

	CString strVal;
	_EditStopVal.GetWindowText(strVal);
	std::string amt = strVal;
	if (!NumberParser::tryParse(amt, _StopVal))
	{
		return;
	}
}


void VtOrderCenterFundWnd::OnBnClickedBtnLiq()
{
	// TODO: Add your control notification handler code here
	_OrderPanelGrid.LiqudAll(VtPriceType::Market, 0);
}


void VtOrderCenterFundWnd::OnBnClickedBtnRemain()
{
	// TODO: Add your control notification handler code here
	CString strValue;
	_BtnRemain.GetWindowText(strValue);
	int first = 0, last = 0;
	first = strValue.Find('(');
	last = strValue.Find(')');
	strValue = strValue.Mid(first + 1, last - first - 1);
	_OrderAmount = std::abs(_ttoi(strValue));
	strValue.Format(_T("%d"), _OrderAmount);
	_EditOrderAmount.SetWindowText(strValue);
}


void VtOrderCenterFundWnd::OnBnClickedBtnSelSymbol()
{
	// TODO: Add your control notification handler code here
	HdSymbolSelecter symSelecter;
	symSelecter.OrderConfigMgr(_OrderConfigMgr);
	symSelecter.DoModal();
}

void VtOrderCenterFundWnd::ChangeAccount(VtAccount* acnt)
{
	if (!acnt)
		return;
	ClearPosition();
	UnregisterOrderWnd();
	SetAccount(acnt);
	SetSymbol();
	InitInfo();
	InitPosition();
	RegisterOrderWnd();
}

void VtOrderCenterFundWnd::ChangeSymbol(VtSymbol* newSymbol)
{
	if (!newSymbol)
		return;
	_TickGrid.ClearValues();
	ClearPosition();
	UnregisterOrderWnd();
	UnregisterRealtimeSymbol();
	UnregisterSymbolWnd();
	SetSymbol(newSymbol);
	AddSymbolToCombo(newSymbol);
	SetProductName(newSymbol);
	InitInfo();
	InitPosition();
	RegisterSymbolWnd();
	RegisterRealtimeSymbol();
	RegisterOrderWnd();
}

void VtOrderCenterFundWnd::AddSymbolToCombo(VtSymbol* symbol)
{
	if (!symbol)
		return;
	int index = _ComboSymbol.FindString(0, symbol->ShortCode.c_str());
	if (index == -1)
	{
		index = _ComboSymbol.AddString(symbol->ShortCode.c_str());
		_ComboSymbol.SetItemDataPtr(index, symbol);
	}
	_ComboSymbol.SetCurSel(index);
}

void VtOrderCenterFundWnd::SetProductName(VtSymbol* symbol)
{
	if (!symbol)
		return;

	_StaticProductName.SetWindowText(symbol->Name.c_str());
}

void VtOrderCenterFundWnd::ChangeFocus()
{
	SendMessage(WM_NEXTDLGCTL, (WPARAM)_OrderPanelGrid.GetSafeHwnd(), TRUE);
}

void VtOrderCenterFundWnd::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	if (_Activated)
		dc.FillSolidRect(&rect, RGB(255, 255, 0));
	else
		dc.FillSolidRect(&rect, GetSysColor(COLOR_3DFACE));
	CDialog::OnPaint();
}

void VtOrderCenterFundWnd::OnDeltaposSpinExpand(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	int count = pNMUpDown->iDelta;
	if (_OrderConfigMgr && _OrderConfigMgr->_HdOrderWnd)
	{
		if (count < 0)
			_OrderConfigMgr->_HdOrderWnd->RemoveWindow();
		else
			_OrderConfigMgr->_HdOrderWnd->AddWindow();
	}
	*pResult = 0;
}


void VtOrderCenterFundWnd::OnStnClickedStaticMsg()
{
	// TODO: Add your control notification handler code here
}


BOOL VtOrderCenterFundWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
	//return CRHGenericChildDialog::OnEraseBkgnd(pDC);
}


void VtOrderCenterFundWnd::OnBnClickedBtnShowRealtick()
{
	// TODO: Add your control notification handler code here
	_ShowRealTickWndMode++;
	if (_ShowRealTickWndMode > 2)
		_ShowRealTickWndMode = 0;
	if (_OrderConfigMgr && _OrderConfigMgr->_HdOrderWnd)
	{
		_OrderConfigMgr->_HdOrderWnd->RecalcLayout(true);
	}
}


void VtOrderCenterFundWnd::OnBnClickedStaticFillPl()
{
	// TODO: Add your control notification handler code here
}

void VtOrderCenterFundWnd::CreateChildWindow(VtOrderConfigDlg* centerWnd, UINT id, CWnd* parent)
{
	if (!centerWnd || !parent)
		return;

	//centerWnd->CRHCreateGenericChildDialog(this, &rect, _WindowID++, NULL);
	centerWnd->Create(id, parent);
	centerWnd->ModifyStyle(WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, WS_CHILD);
	// Adjust extended styles. 1st parameter removes, 2nd adds.
	centerWnd->ModifyStyleEx(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE, WS_EX_CONTROLPARENT);
	// As we have changed the frame, we let Windows recalculate the non-client area
	// by passing the SWP_FRAMECHANGED flag to SetWindowPos().
	centerWnd->SetWindowPos(nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED |
		SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_DRAWFRAME);
}


void VtOrderCenterFundWnd::OnBnClickedButtonSetting()
{
	// TODO: Add your control notification handler code here
	ClearConfigDlg();
	VtOrderGridConfig* grid = new VtOrderGridConfig();
	//grid->CenterWnd(this);
	_ConfigDlgVector.push_back(grid);
	grid->Create(IDD_ORDERGRID_CONFIG, this);
	grid->ShowWindow(SW_SHOW);
}

void VtOrderCenterFundWnd::ShowOrderAreaInGrid(bool flag)
{
	_ShowOrderArea = flag;
	_OrderPanelGrid.ShowOrderAreaInGrid(flag);
}

void VtOrderCenterFundWnd::ShowStopAreaInGrid(bool flag)
{
	_ShowStopArea = flag;
	_OrderPanelGrid.ShowStopAreaInGrid(flag);
	if (!_ShowStopArea && !_ShowOrderCountArea)
	{
		_ProductRemainGrid.SetShowAvage(false);
		_ProductRemainGrid.SetShowRemainType(false);
	}
	else
	{
		_ProductRemainGrid.SetShowAvage(true);
		_ProductRemainGrid.SetShowRemainType(true);
	}
}

void VtOrderCenterFundWnd::ShowOrderCountInGrid(bool flag)
{
	_ShowOrderCountArea = flag;
	_OrderPanelGrid.ShowOrderCountInGrid(flag);
	if (!_ShowStopArea && !_ShowOrderCountArea)
	{
		_ProductRemainGrid.SetShowAvage(false);
		_ProductRemainGrid.SetShowRemainType(false);
	}
	else
	{
		_ProductRemainGrid.SetShowAvage(true);
		_ProductRemainGrid.SetShowRemainType(true);
	}
}

void VtOrderCenterFundWnd::ShowTickWnd(bool flag)
{
	if (_OrderConfigMgr && _OrderConfigMgr->_HdOrderWnd)
	{
		_OrderConfigMgr->_HdOrderWnd->RecalcLayout(true);
	}
}

void VtOrderCenterFundWnd::SetTickWndPos(int pos)
{
	_ShowRealTickWndMode = pos;
	if (_OrderConfigMgr && _OrderConfigMgr->_HdOrderWnd)
	{
		_OrderConfigMgr->_HdOrderWnd->RecalcLayout(true);
	}
}

void VtOrderCenterFundWnd::SetTickCount(int count)
{
	_TickGrid.MaxRow(count);
	CRect rcGrid, rcWnd;
	GetWindowRect(rcWnd);
	ScreenToClient(rcWnd);
	_TickGrid.GetWindowRect(rcGrid);
	ScreenToClient(rcGrid);
	rcGrid.bottom = rcWnd.bottom;
	InvalidateRect(rcGrid);
}

void VtOrderCenterFundWnd::SetOrderCellHeight(int value)
{
	_OrderPanelGrid.SetOrderCellHeight(value);
}

void VtOrderCenterFundWnd::SetOrderCellWidth(int value)
{
	_OrderPanelGrid.SetOrderCellHeight(value);
}

void VtOrderCenterFundWnd::SetShowPLConfigWnd(bool flag)
{

}

void VtOrderCenterFundWnd::SetOrderArea(int height, int width)
{
	_OrderPanelGrid.SetOrderArea(height, width);
}

int VtOrderCenterFundWnd::GetMaxWidth()
{
	//return _OrderPanelGrid.GetMaxWidth();
	if (_ShowRealTickWndMode == 0)
		return _OrderPanelGrid.GetMaxWidth();
	else  // For the tick grid on the left or right side
	{
		CRect rcTick;
		_TickGrid.GetWindowRect(rcTick);
		return _OrderPanelGrid.GetMaxWidth() + rcTick.Width() + _GridGap;
	}
}

void VtOrderCenterFundWnd::RecalcLayout()
{
	if (_OrderConfigMgr && _OrderConfigMgr->_HdOrderWnd)
	{
		_OrderConfigMgr->_HdOrderWnd->RecalcLayout(true);
	}

	SetProductGridWidth();
}

void VtOrderCenterFundWnd::SetMaxTickCount(int count)
{
	if (_RealTickWnd)
	{
		_RealTickWnd->SetMaxTickCount(count);
	}
}


void VtOrderCenterFundWnd::OnBnClickedButtonProfitLoss()
{
	// TODO: Add your control notification handler code here
}


void VtOrderCenterFundWnd::OnStnClickedStaticOrderPanel()
{
	// TODO: Add your control notification handler code here
}


BOOL VtOrderCenterFundWnd::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			OnEntered();
			return TRUE;
		}
	}


	return CDialog::PreTranslateMessage(pMsg);
}

void VtOrderCenterFundWnd::OnRClicked(NMHDR * pNotifyStruct, LRESULT * result)
{
	if (_CurBtn)
		return;

	int id = pNotifyStruct->idFrom;
	if (id == IDC_BTN_ORDER_AMT1)
	{
		CRect rcWnd;
		_BtnAmt1.GetWindowRect(rcWnd);
		ScreenToClient(rcWnd);
		_BtnAmt1.ShowWindow(SW_HIDE);
		CString strText;
		_BtnAmt1.GetWindowText(strText);
		_EditAmt.SetWindowText(strText);
		_EditAmt.ShowWindow(SW_SHOW);
		_EditAmt.MoveWindow(rcWnd);
		_EditAmt.SetFocus();
		_CurBtn = &_BtnAmt1;
	}
	else if (id == IDC_BTN_ORDER_AMT2)
	{
		CRect rcWnd;
		_BtnAmt2.GetWindowRect(rcWnd);
		ScreenToClient(rcWnd);
		_BtnAmt2.ShowWindow(SW_HIDE);
		CString strText;
		_BtnAmt2.GetWindowText(strText);
		_EditAmt.SetWindowText(strText);
		_EditAmt.ShowWindow(SW_SHOW);
		_EditAmt.MoveWindow(rcWnd);
		_EditAmt.SetFocus();
		_CurBtn = &_BtnAmt2;
	}
	else if (id == IDC_BTN_ORDER_AMT3)
	{
		CRect rcWnd;
		_BtnAmt3.GetWindowRect(rcWnd);
		ScreenToClient(rcWnd);
		_BtnAmt3.ShowWindow(SW_HIDE);
		CString strText;
		_BtnAmt3.GetWindowText(strText);
		_EditAmt.SetWindowText(strText);
		_EditAmt.ShowWindow(SW_SHOW);
		_EditAmt.MoveWindow(rcWnd);
		_EditAmt.SetFocus();
		_CurBtn = &_BtnAmt3;
	}
	else if (id == IDC_BTN_ORDER_AMT4)
	{
		CRect rcWnd;
		_BtnAmt4.GetWindowRect(rcWnd);
		ScreenToClient(rcWnd);
		_BtnAmt4.ShowWindow(SW_HIDE);
		CString strText;
		_BtnAmt4.GetWindowText(strText);
		_EditAmt.SetWindowText(strText);
		_EditAmt.ShowWindow(SW_SHOW);
		_EditAmt.MoveWindow(rcWnd);
		_EditAmt.SetFocus();
		_CurBtn = &_BtnAmt4;
	}
	else if (id == IDC_BTN_ORDER_AMT5)
	{
		CRect rcWnd;
		_BtnAmt5.GetWindowRect(rcWnd);
		ScreenToClient(rcWnd);
		_BtnAmt5.ShowWindow(SW_HIDE);
		CString strText;
		_BtnAmt5.GetWindowText(strText);
		_EditAmt.SetWindowText(strText);
		_EditAmt.ShowWindow(SW_SHOW);
		_EditAmt.MoveWindow(rcWnd);
		_EditAmt.SetFocus();
		_CurBtn = &_BtnAmt5;
	}
	else if (id == IDC_BTN_ORDER_AMT6)
	{
		CRect rcWnd;
		_BtnAmt6.GetWindowRect(rcWnd);
		ScreenToClient(rcWnd);
		_BtnAmt6.ShowWindow(SW_HIDE);
		CString strText;
		_BtnAmt6.GetWindowText(strText);
		_EditAmt.SetWindowText(strText);
		_EditAmt.ShowWindow(SW_SHOW);
		_EditAmt.MoveWindow(rcWnd);
		_EditAmt.SetFocus();
		_CurBtn = &_BtnAmt6;
	}
}

void VtOrderCenterFundWnd::OnEntered()
{
	if (_CurBtn)
	{
		CString strText;
		_EditAmt.GetWindowText(strText);
		_CurBtn->SetWindowText(strText);
		_EditAmt.ShowWindow(SW_HIDE);
		_CurBtn->ShowWindow(SW_SHOW);
		_CurBtn->SetFocus();
		_CurBtn = nullptr;
	}
}

void VtOrderCenterFundWnd::OnStnClickedStaticRealTick()
{
	// TODO: Add your control notification handler code here
}

void VtOrderCenterFundWnd::Save(simple::file_ostream<same_endian_type>& ss)
{
	if (!_Symbol)
		return;
	ss << _Symbol->ShortCode;
	ss << _OrderAmount;
	ss << _OrderPanelGrid.CellHeight();
	ss << _OrderPanelGrid.OrderWidth();
	ss << _OrderPanelGrid._CloseRow;
}

void VtOrderCenterFundWnd::Load(simple::file_istream<same_endian_type>& ss)
{
	std::string shortCode;
	int orderAmount;
	int cellHeight;
	int cellWidth;
	int closeRow;
	CRect rcWnd;
	ss >> shortCode;
	ss >> orderAmount;
	ss >> cellHeight;
	ss >> cellWidth;
	ss >> closeRow;
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindHdSymbol(shortCode);
	SetSymbol(sym);
	AddSymbolToCombo(sym);
	SetProductName(sym);
	_OrderAmount = orderAmount;
	_OrderPanelGrid.CellHeight(cellHeight);
	_OrderPanelGrid.OrderWidth(cellWidth);
	_OrderPanelGrid._CloseRow = closeRow;


	VtSaveManager* saveMgr = VtSaveManager::GetInstance();
	std::vector<VtSymbol*>& symvec = saveMgr->GetSymbolVector();
	symvec.push_back(sym);
}
