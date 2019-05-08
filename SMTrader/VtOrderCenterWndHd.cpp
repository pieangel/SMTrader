// VtOrderCenterHd.cpp : implementation file
//

#include "resource.h"
#include "stdafx.h"
#include "SMTrader.h"
#include "VtOrderCenterWndHd.h"
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
#include "VtFundOrderManager.h"
#include "VtCutManager.h"
#include <map>
#include "VtLayoutManager.h"

using Poco::NumberParser;

extern TApplicationFont g_Font;

IMPLEMENT_DYNAMIC(VtOrderCenterWndHd, CDialogEx)


VtOrderCenterWndHd::VtOrderCenterWndHd(CWnd* pParent )
	: CDialogEx(IDD_ORDER_CENTER_HD, pParent)
{
	//_CutMgr = new VtCutManager();
	_ParentDlg = nullptr;
	_Symbol = nullptr;
	_OrderAmount = 1;
	_FillCondition = VtFilledCondition::Fas;
	_PriceType = VtPriceType::Price;
	_FixedCenter = false;
	/*
	_EnableCutProfit = false;
	_EnableCutLoss = false;
	_CutProfit = 5;
	_CutLoss = 5;
	_OrderType = 1;
	_OrderTypeSlip = 2;
	*/
	_StopVal = 2;
	_Activated = false;
	_RealTickWnd = nullptr;
	_ConfigDlg = nullptr;
	_Init = false;
	_TickWndPos = 0;
	x = 0;
	y = 0;
	width = 0;
	height = 0;
	_GridGap = 0;
	_OrderConfigMgr = nullptr;
	_OrderPanelGrid.CenterWnd(this);
	_ProductRemainGrid.CenterWnd(this);
	_Unregistered = false;
	_OrderByRemain = false;
	_LayoutMgr = new VtLayoutManager((CWnd*)this);

	_OrderGridColOption.push_back(true);
	_OrderGridColOption.push_back(true);
	_OrderGridColOption.push_back(true);

	// create the background brush
	VERIFY(_BrushBackSel.CreateSolidBrush(VtGlobal::SelDialogBackColor));
	VERIFY(_BrushBackNor.CreateSolidBrush(GetSysColor(COLOR_BTNFACE)));
}

VtOrderCenterWndHd::~VtOrderCenterWndHd()
{
	if (_RefreshManager) {
		delete _RefreshManager;
		_RefreshManager = nullptr;
	}
	if (_ConfigDlg) {
		_ConfigDlg->DestroyWindow();
		delete _ConfigDlg;
		_ConfigDlg = nullptr;
	}

	if (_LayoutMgr) {
		delete _LayoutMgr;
		_LayoutMgr = nullptr;
	}


	ClearConfigDlg();
}

void VtOrderCenterWndHd::OrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
	_OrderPanelGrid.CutMgr()->OrderConfigMgr(_OrderConfigMgr);
	_OrderPanelGrid.OrderConfigMgr(_OrderConfigMgr);
	_ProductRemainGrid.OrderConfigMgr(_OrderConfigMgr);
}

bool VtOrderCenterWndHd::EnableCutProfit() const
{
	return _OrderPanelGrid.CutMgr()->EnableCutProfit();
}

void VtOrderCenterWndHd::EnableCutProfit(bool val)
{
	_OrderPanelGrid.CutMgr()->EnableCutProfit(val);
}

bool VtOrderCenterWndHd::EnableCutLoss() const
{
	return _OrderPanelGrid.CutMgr()->EnableCutLoss();
}

void VtOrderCenterWndHd::EnableCutLoss(bool val)
{
	_OrderPanelGrid.CutMgr()->EnableCutLoss(val);
}

int VtOrderCenterWndHd::CutProfit() const
{
	return _OrderPanelGrid.CutMgr()->CutProfit();
}

void VtOrderCenterWndHd::CutProfit(int val)
{
	_OrderPanelGrid.CutMgr()->CutProfit(val);
}

int VtOrderCenterWndHd::CutLoss() const
{
	return _OrderPanelGrid.CutMgr()->CutLoss();
}

void VtOrderCenterWndHd::CutLoss(int val)
{
	_OrderPanelGrid.CutMgr()->CutLoss(val);
}

int VtOrderCenterWndHd::OrderType() const
{
	return _OrderPanelGrid.CutMgr()->OrderType();
}

void VtOrderCenterWndHd::OrderType(int val)
{
	_OrderPanelGrid.CutMgr()->OrderType(val);
}

int VtOrderCenterWndHd::OrderTypeSlip() const
{
	return _OrderPanelGrid.CutMgr()->OrderTypeSlip();
}

void VtOrderCenterWndHd::OrderTypeSlip(int val)
{
	_OrderPanelGrid.CutMgr()->OrderTypeSlip(val);
}

void VtOrderCenterWndHd::Activated(bool val)
{
	_Activated = val;
	/*
	CRect& rcGrid = _LayoutMgr->GetRect(IDC_STATIC_ORDER_PANEL);

	
	CRect rect;
	GetClientRect(&rect);
	rect.bottom = rcGrid.top;
	InvalidateRect(rect, TRUE);

	if (_ConfigDlg) {
		_ConfigDlg->Invalidate(TRUE);
	}
	*/
	if (_Symbol && _ParentDlg) {
		SetProductName(_Symbol);
		CString title;
		title.Format(_T("계좌주문창 : 종목 - %s"), _Symbol->Name.c_str());
		_ParentDlg->SetWindowText(title);
	}

	if (_Activated) {
		_StaticProductName.SetTextColor(RGB(255, 255, 255));
		_StaticProductName.SetColor(RGB(19, 137, 255));
		_StaticProductName.SetGradientColor(RGB(19, 137, 255));


		_StaticProduct.SetTextColor(RGB(255, 255, 255));
		_StaticProduct.SetColor(RGB(19, 137, 255));
		_StaticProduct.SetGradientColor(RGB(19, 137, 255));

	}
	else {
		_StaticProductName.SetTextColor(RGB(0, 0, 0));
		_StaticProductName.SetColor(GetSysColor(COLOR_BTNFACE));
		_StaticProductName.SetGradientColor(GetSysColor(COLOR_BTNFACE));

		_StaticProduct.SetTextColor(RGB(0, 0, 0));
		_StaticProduct.SetColor(GetSysColor(COLOR_BTNFACE));
		_StaticProduct.SetGradientColor(GetSysColor(COLOR_BTNFACE));
	}

	_StaticProductName.Invalidate();
	_StaticProduct.Invalidate();
}

void VtOrderCenterWndHd::ResizeOrderGrid(int maxHeight)
{
	if (!_Init)
		return;

	_OrderPanelGrid.ResizeGrid(maxHeight);
}

void VtOrderCenterWndHd::ResizeOrderGrid()
{
	CalcLayout();
	RepositionControl();
	_OrderPanelGrid.ResizeGrid(0);
	RepositionProductGrid();
	RepositionConfigWnd();
}

void VtOrderCenterWndHd::SetFund(VtFund* fund)
{
	if (!fund)
		return;
	//_Fund = fund;
}

int VtOrderCenterWndHd::GetCenterWndCount()
{
	if (_ParentDlg)
		return  _ParentDlg->GetCenterWndCount();
	else
		return -1;
}

void VtOrderCenterWndHd::UpdateGrid()
{
	PostMessage(WM_REFRESH_GRID, 0, 0);
}

void VtOrderCenterWndHd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
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
	DDX_Control(pDX, IDC_BTN_REMAIN_FUND, _BtnRemainFund);
	DDX_Control(pDX, IDC_BTN_SEL_SYMBOL, _BtnSearch);
	DDX_Control(pDX, IDC_BUTTON_SETTING, _BtnSettings);
	DDX_Control(pDX, IDC_STATIC_PRODUCT, _StaticProduct);
}


BEGIN_MESSAGE_MAP(VtOrderCenterWndHd, CDialogEx)
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
	//ON_WM_ERASEBKGND()
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
	ON_BN_CLICKED(IDC_BTN_REMAIN_FUND, &VtOrderCenterWndHd::OnBnClickedBtnRemainFund)
	//ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_REFRESH_GRID, &VtOrderCenterWndHd::OnRefreshGrid)
END_MESSAGE_MAP()


// VtOrderCenterHd message handlers


void VtOrderCenterWndHd::InitGridInfo()
{
	_OrderPanelGrid.InitInfo();
}

void VtOrderCenterWndHd::InitAll()
{
	if (!_OrderConfigMgr)
		return;
	InitSymbol();
	InitPosition();
	RegisterSymbolWnd();
	RegisterOrderWnd();
	RegisterRealtimeSymbol();
	RegisterRealtimeAccount();
	InitGridInfo();
	_Init = true;
}

void VtOrderCenterWndHd::InitByLoading(int height)
{
	InitPosition();
	RegisterSymbolWnd();
	RegisterOrderWnd();
	RegisterRealtimeSymbol();
	RegisterRealtimeAccount();
	_OrderPanelGrid.ResizeGrid(height);
	_Init = true;
}

BOOL VtOrderCenterWndHd::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	::EnumChildWindows(m_hWnd, ::SetChildFont, (LPARAM)g_Font.GetFont());
	
	m_SelBrush.CreateSolidBrush(RGB(255, 255, 0)); // color yellow brush 
	m_NormalBrush.CreateSolidBrush(RGB(255, 255, 255)); // color yellow brush 

	_ConfigDlg = new VtOrderConfigDlg();
	_ConfigDlg->CenterWnd(this);
	_ConfigDlg->Create(IDD_CENTER_CONFIG, this);

	_ConfigGrid.CenterWnd(this);
	_OrderPanelGrid.AttachGrid(this, IDC_STATIC_ORDER_PANEL);
	_ProductRemainGrid.AttachGrid(this, IDC_STATIC_PRODUCT_REMAIN);
	_TickGrid.AttachGrid(this, IDC_STATIC_REAL_TICK);
	_ConfigGrid.AttachGrid(this, IDC_STATIC_SET);

	SaveControlPos();
	CalcLayout();
	RepositionControl();
	RepositionConfigWnd();
	RepositionTickGrid();

	_StaticProductName.SetTextColor(RGB(0, 0, 0));
	_StaticProductName.SetColor(GetSysColor(COLOR_BTNFACE));
	_StaticProductName.SetGradientColor(GetSysColor(COLOR_BTNFACE));
	_StaticProductName.SetTextAlign(1);
	_StaticProduct.SetTextColor(RGB(0, 0, 0));
	_StaticProduct.SetColor(GetSysColor(COLOR_BTNFACE));
	_StaticProduct.SetGradientColor(GetSysColor(COLOR_BTNFACE));
	_StaticProduct.SetTextAlign(1);

	/*
	_StaticProductName.SetTextColor(RGB(255, 255, 255));
	_StaticProductName.SetColor(RGB(255, 0, 0));
	_StaticProductName.SetGradientColor(RGB(255, 0, 0));
	_StaticProductName.SetTextAlign(0);
	//_StaticProductName.SetLeftSpacing(0);

	_StaticProduct.SetTextColor(RGB(255, 255, 255));
	_StaticProduct.SetColor(RGB(255, 0, 0));
	_StaticProduct.SetGradientColor(RGB(255, 0, 0));
	_StaticProduct.SetTextAlign(0);
	//_StaticProduct.SetLeftSpacing(0);
	*/

	std::vector<CShadeButtonST*> btnVec;
	btnVec.push_back(&_BtnAmt1);
	btnVec.push_back(&_BtnAmt2);
	btnVec.push_back(&_BtnAmt3);
	btnVec.push_back(&_BtnAmt4);
	btnVec.push_back(&_BtnAmt5);
	btnVec.push_back(&_BtnAmt6);
	btnVec.push_back(&_BtnRemain);
	btnVec.push_back(&_BtnSettings);
	btnVec.push_back(&_BtnSearch);
	
	for (auto it = btnVec.begin(); it != btnVec.end(); ++it) {
		(*it)->SetShade(SHS_HSHADE, 8, 20, 5, RGB(55, 55, 255));
		(*it)->DrawFlatFocus(TRUE);
	}


	_BtnSearch.SetIcon(IDI_SEARCH4, 16, 16, IDI_SEARCH4, 16, 16);
	_BtnSearch.OffsetColor(BTNST_COLOR_BK_IN, 30);
	_BtnSearch.SetAlign(ST_ALIGN_VERT);


	_ScOrderAmount.SetRange32(1, 100);
	_ScStopVal.SetRange32(0, 100);

	CString strValue;
	strValue.Format(_T("%d"), _OrderPanelGrid.CutMgr()->CutLoss());

	strValue.Format(_T("%d"), _OrderPanelGrid.CutMgr()->CutProfit());

	strValue.Format(_T("%d"), _OrderAmount);
	_EditOrderAmount.SetWindowText(strValue);

	strValue.Format(_T("%d"), _StopVal);
	_EditStopVal.SetWindowText(strValue);

	_BtnRemainFund.DrawBorder(TRUE, TRUE);
	_BtnRemainFund.SetAlign(ST_ALIGN_VERT | ST_ALIGN_HORIZ);
	_BtnRemainFund.SetColor(BTNST_COLOR_BK_IN, RGB(220, 220, 220), true);
	_BtnRemainFund.SetColor(BTNST_COLOR_FG_IN, RGB(0, 0, 0), true);
	_BtnRemainFund.SetColor(BTNST_COLOR_BK_OUT, RGB(220, 220, 220), true);

	_OrderPanelGrid.OrderConfigMgr(_OrderConfigMgr);
	_TickGrid.SetOrderConfigMgr(_OrderConfigMgr);
	_TickGrid.CenterWnd(this);
	ShowHideCtrl();
	RepositionProductGrid();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void VtOrderCenterWndHd::ShowHideCtrl()
{
	if (!_OrderConfigMgr)
		return;

	if (_OrderConfigMgr->Type() == 0) {
		//_BtnAvail.ShowWindow(SW_SHOW);
		_BtnRemain.ShowWindow(SW_SHOW);
		_BtnRemainFund.ShowWindow(SW_HIDE);
	} else {
		_BtnAvail.ShowWindow(SW_HIDE);
		_BtnRemain.ShowWindow(SW_SHOW);
		_BtnRemainFund.ShowWindow(SW_SHOW);
	}
}

void VtOrderCenterWndHd::ClearConfigDlg()
{
	for (auto it = _ConfigDlgVector.begin(); it != _ConfigDlgVector.end(); ++it) {
		VtOrderGridConfig* dlg = *it;
		dlg->DestroyWindow();
		delete dlg;
	}

	_ConfigDlgVector.clear();
}

void VtOrderCenterWndHd::RepositionProductGrid()
{
	_ProductRemainGrid.SetColTitleForLevel(_OrderGridColOption);
}

void VtOrderCenterWndHd::RePosStopCtrl(int level)
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

void VtOrderCenterWndHd::RepositionTickGrid()
{
	if (_ShowTickWnd) {
		CRect& rcTick = _LayoutMgr->GetRect(IDC_STATIC_REAL_TICK);
		_TickGrid.SetWindowPos(nullptr, rcTick.left, rcTick.top, rcTick.Width(), rcTick.Height(), SWP_NOSIZE);
	}
}

/*
IDC_STATIC_PRODUCT, IDC_COMBO_PRODUCT_HD,IDC_BTN_SEL_SYMBOL,IDC_STATIC_PRODUCT_NAME, IDC_BTN_LIQ
IDC_BUTTON_SETTING, IDC_EDIT_ORDER_AMOUNT, IDC_SPIN_ORDER_AMOUNT, IDC_BTN_ORDER_AMT1, IDC_BTN_ORDER_AMT2, IDC_BTN_ORDER_AMT3, IDC_BTN_ORDER_AMT4, IDC_BTN_ORDER_AMT5, IDC_BTN_ORDER_AMT6, IDC_BTN_REMAIN, IDC_BTN_AVAIL
IDC_STATIC_PRODUCT_REMAIN, IDC_CK_FIXED_CENTER
						   IDC_STATIC_STOP,IDC_EDIT_STOPVAL
IDC_STATIC_ORDER_PANEL,IDC_STATIC_REAL_TICK
IDC_STATIC_MSG
*/
void VtOrderCenterWndHd::RecalControls()
{
	CRect rcParent;
	CWnd* parent = GetParent();
	if (!parent->GetSafeHwnd())
		return;

	parent->GetWindowRect(rcParent);
	m_LayoutManager.SetDefaultConstraint(IDC_STATIC_PRODUCT);
	m_LayoutManager.SetDefaultConstraint(IDC_COMBO_PRODUCT_HD);
	m_LayoutManager.SetDefaultConstraint(IDC_BTN_SEL_SYMBOL);
	m_LayoutManager.SetDefaultConstraint(IDC_STATIC_PRODUCT_NAME);
	m_LayoutManager.SetDefaultConstraint(IDC_BTN_LIQ);
	m_LayoutManager.SetDefaultConstraint(IDC_BUTTON_SETTING);
	m_LayoutManager.SetDefaultConstraint(IDC_EDIT_ORDER_AMOUNT);
	m_LayoutManager.SetDefaultConstraint(IDC_SPIN_ORDER_AMOUNT);
	m_LayoutManager.SetDefaultConstraint(IDC_BTN_ORDER_AMT1);
	m_LayoutManager.SetDefaultConstraint(IDC_BTN_ORDER_AMT2);
	m_LayoutManager.SetDefaultConstraint(IDC_BTN_ORDER_AMT3);
	m_LayoutManager.SetDefaultConstraint(IDC_BTN_ORDER_AMT4);
	m_LayoutManager.SetDefaultConstraint(IDC_BTN_ORDER_AMT5);
	m_LayoutManager.SetDefaultConstraint(IDC_BTN_ORDER_AMT6);
	m_LayoutManager.SetDefaultConstraint(IDC_BTN_REMAIN);
	m_LayoutManager.SetDefaultConstraint(IDC_BTN_AVAIL);
	m_LayoutManager.SetDefaultConstraint(IDC_STATIC_PRODUCT_REMAIN);
	
	m_LayoutManager.SetConstraint(IDC_CK_FIXED_CENTER, OX_LMS_LEFT, OX_LMT_OPPOSITE, 10, IDC_STATIC_PRODUCT_REMAIN);
	m_LayoutManager.SetConstraint(IDC_STATIC_STOP, OX_LMS_LEFT, OX_LMT_OPPOSITE, 10, IDC_STATIC_PRODUCT_REMAIN);
	m_LayoutManager.SetConstraint(IDC_EDIT_STOPVAL, OX_LMS_LEFT, OX_LMT_OPPOSITE, 10, IDC_STATIC_STOP);

	m_LayoutManager.SetConstraint(IDC_STATIC_ORDER_PANEL, OX_LMS_TOP, OX_LMT_OPPOSITE, 10, IDC_STATIC_PRODUCT_REMAIN);
	m_LayoutManager.SetConstraint(IDC_STATIC_ORDER_PANEL, OX_LMS_BOTTOM, OX_LMT_OPPOSITE, 10, IDC_STATIC_MSG);

	m_LayoutManager.SetConstraint(IDC_STATIC_MSG, OX_LMS_TOP, OX_LMT_OPPOSITE, 10, IDC_STATIC_ORDER_PANEL);
	m_LayoutManager.SetConstraint(IDC_STATIC_MSG, OX_LMS_BOTTOM, OX_LMT_SAME, 10);

	m_LayoutManager.RedrawLayout();
}

int VtOrderCenterWndHd::GetMaxWidthForMode()
{
	return 0;
}

void VtOrderCenterWndHd::RefreshOrderPositon()
{
	_OrderPanelGrid.RefreshOrderPosition();
}


void VtOrderCenterWndHd::OnOrderReceivedHd(VtOrder* order)
{
	if (_OrderPanelGrid.GetSafeHwnd())
		_OrderPanelGrid.OnOrderReceivedHd(order);
}

void VtOrderCenterWndHd::OnOrderAcceptedHd(VtOrder* order)
{
	if (_OrderPanelGrid.GetSafeHwnd())
		_OrderPanelGrid.OnOrderAcceptedHd(order);
}

void VtOrderCenterWndHd::OnOrderUnfilledHd(VtOrder* order)
{
	if (_OrderPanelGrid.GetSafeHwnd())
		_OrderPanelGrid.OnOrderUnfilledHd(order);
}

void VtOrderCenterWndHd::OnOrderFilledHd(VtOrder* order)
{
	if (_ProductRemainGrid.GetSafeHwnd())
		_ProductRemainGrid.ShowPosition();
	if (_OrderPanelGrid.GetSafeHwnd())
		_OrderPanelGrid.OnOrderFilledHd(order);
}

void VtOrderCenterWndHd::OnRemain(VtPosition* posi)
{
	if (!posi)
		return;
	_ProductRemainGrid.OnRemain(posi);
	//GetDeposit(posi);
	RefreshOrderPositon();
}

void VtOrderCenterWndHd::OnReceiveHoga(VtSymbol* sym)
{
	if (!sym)
		return;
	_OrderPanelGrid.OnReceiveHoga(sym);
}

void VtOrderCenterWndHd::OnReceiveQuote(VtSymbol* sym)
{
	if (!sym)
		return;
	_TickGrid.OnReceiveQuote(sym);
	_OrderPanelGrid.OnReceiveQuote(sym);
	_ProductRemainGrid.OnReceiveQuote(sym);
	if (_RealTickWnd)
	{
		_RealTickWnd->OnReceiveQuote(sym);
	}
}


void VtOrderCenterWndHd::OnSymbolMaster(VtSymbol* sym)
{
	if (_OrderPanelGrid.GetSafeHwnd()) {
		_OrderPanelGrid.OnSymbolMaster(sym);
		SetProductName(sym);
	}
}

void VtOrderCenterWndHd::OnReceiveMsg(CString msg)
{
	_StaticMsg.SetWindowText(msg);
}

void VtOrderCenterWndHd::OnReceiveMsgWithReqId(int id, CString msg)
{
	CString message;
	message.Format(_T("Request ID : %d - %s"), id, msg);
	_StaticMsg.SetWindowText(message);
}

void VtOrderCenterWndHd::OnClose()
{
	if (_RefreshManager) {
		_RefreshManager->SetReady(true);
		_RefreshManager->AddTask(-1);
	}
	_OrderPanelGrid.StopTimer();
	// TODO: Add your message handler code here and/or call default
	CDialogEx::OnClose();
}


void VtOrderCenterWndHd::OnDestroy()
{
	CDialogEx::OnDestroy();

	_OrderPanelGrid.ClearAllStopOrders();
	_OrderPanelGrid.CenterWnd(nullptr);
	// TODO: Add your message handler code here
	UnregisterRealtimeSymbol();
	UnregisterRealtimeAccount();
	UnregisterSymbolWnd();
	UnregisterOrderWnd(true);
}

void VtOrderCenterWndHd::RepositionControl(int height)
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

void VtOrderCenterWndHd::RepositionControl()
{
	std::map<int, std::pair<std::string, CRect>>& _WndMap = _LayoutMgr->GetWndMap();
	HDWP hdwp = ::BeginDeferWindowPos(_WndMap.size());
	if (hdwp) {
		for (auto it = _WndMap.begin(); it != _WndMap.end(); ++it) {
			std::pair<std::string, CRect> item = it->second;
			CWnd* pWnd = (CWnd*)GetDlgItem(it->first);
			ASSERT(pWnd);
			if (pWnd->GetSafeHwnd()) {
				CRect rectWC = std::get<1>(item);
				::DeferWindowPos(hdwp, pWnd->m_hWnd, NULL,
					rectWC.left, rectWC.top, rectWC.Width(), rectWC.Height(),
					SWP_NOZORDER);
				pWnd->RedrawWindow();

			}
		}
		::EndDeferWindowPos(hdwp);
	}
}

void VtOrderCenterWndHd::RepositionConfigWnd()
{
	if (!_LayoutMgr || !_ConfigDlg)
		return;
	CRect& rcConfig = _LayoutMgr->GetRect(IDC_STATIC_MSG);
	_ConfigDlg->SetWindowPos(nullptr, rcConfig.left, rcConfig.top, rcConfig.Width(), rcConfig.Height(), SWP_NOZORDER);
	_ConfigDlg->RecalateLayout(rcConfig);
}

void VtOrderCenterWndHd::RecalControl(int height)
{
	CRect rcConfig, rcWnd, rcGrid;
	GetWindowRect(rcWnd);
	_ConfigDlg->GetWindowRect(rcConfig);
	_OrderPanelGrid.GetWindowRect(rcGrid);
	ScreenToClient(rcGrid);
	ScreenToClient(rcConfig);
	_OrderPanelGrid.SetWindowPos(nullptr, rcGrid.left, rcGrid.top, rcGrid.Width(), (height - rcGrid.top - rcConfig.Height()), SWP_NOMOVE);
	_ConfigDlg->MoveWindow(0, height - rcConfig.Height(), rcConfig.Width(), rcConfig.Height());
}

void VtOrderCenterWndHd::AdjustControlForMode()
{
	return;

	CRect  rcGrid, rcTick;
	_OrderPanelGrid.GetWindowRect(rcGrid);
	_TickGrid.GetWindowRect(rcTick);
	ScreenToClient(rcGrid);
	ScreenToClient(rcTick);
	if (_TickWndPos == 0)
	{
		_OrderPanelGrid.SetWindowPos(nullptr, 0, rcGrid.top, rcGrid.Width(), rcGrid.Height(), SWP_NOSIZE);
		_TickGrid.SetWindowPos(nullptr, rcGrid.Width() + _GridGap, rcGrid.top, rcGrid.Width(), rcGrid.Height(), SWP_NOSIZE);
	}
	else if (_TickWndPos == 1)
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

void VtOrderCenterWndHd::ResetByCenterRow()
{
	_OrderPanelGrid.ResetByCenterRow();
}

void VtOrderCenterWndHd::GetDeposit(VtPosition* posi)
{
	if (!posi)
		return;

	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	VtAccount* acnt = acntMgr->FindAccount(posi->AccountNo);
	{
		acnt->GetDeposit();
	}
}

int VtOrderCenterWndHd::GetTickCount()
{
	return _TickGrid.MaxRow();
}

void VtOrderCenterWndHd::SetShowRealTickWnd(int mode)
{
	_TickWndPos = mode;
	if (_OrderConfigMgr && _OrderConfigMgr->_HdOrderWnd)
	{
		_OrderConfigMgr->_HdOrderWnd->RecalcLayout(true);
	}
}

void VtOrderCenterWndHd::SetShowLeftButtonText(CString text)
{
	//_BtnLeftExtend.SetWindowText(text);
}

void VtOrderCenterWndHd::SetShowRightButtonText(CString text)
{
	//_BtnRightExtend.SetWindowText(text);
}

void VtOrderCenterWndHd::SetRemain(int remain)
{
	if (!_OrderConfigMgr)
		return;

	CString strValue;
	if (_OrderConfigMgr->Type() == 0) {
		strValue.Format(_T("잔고(%d)"), remain);
		_BtnRemain.SetWindowText(strValue);
	} else {
		strValue.Format(_T("잔고(%d)"), remain);
		_BtnRemain.SetWindowText(strValue);
		strValue.Format(_T("체결된잔고(%d)"), remain);
		_BtnRemainFund.SetWindowText(strValue);
		if (remain < 0) {
			_BtnRemainFund.SetColor(BTNST_COLOR_FG_IN, RGB(255, 255, 255), true);
			_BtnRemainFund.SetColor(BTNST_COLOR_BK_IN, RGB(19, 137, 255), true);
			_BtnRemainFund.SetColor(BTNST_COLOR_FG_OUT, RGB(255, 255, 255), true);
			_BtnRemainFund.SetColor(BTNST_COLOR_BK_OUT, RGB(19, 137, 255), true);

			_BtnRemainFund.SetColor(BTNST_COLOR_FG_FOCUS, RGB(255, 255, 255), true);
			_BtnRemainFund.SetColor(BTNST_COLOR_BK_FOCUS, RGB(19, 137, 255), true);
		}
		else if (remain > 0) {
			_BtnRemainFund.SetColor(BTNST_COLOR_FG_IN, RGB(255, 255, 255), true);
			_BtnRemainFund.SetColor(BTNST_COLOR_BK_IN, RGB(240, 51, 58), true);
			_BtnRemainFund.SetColor(BTNST_COLOR_FG_OUT, RGB(255, 255, 255), true);
			_BtnRemainFund.SetColor(BTNST_COLOR_BK_OUT, RGB(240, 51, 58), true);

			_BtnRemainFund.SetColor(BTNST_COLOR_FG_FOCUS, RGB(255, 255, 255), true);
			_BtnRemainFund.SetColor(BTNST_COLOR_BK_FOCUS, RGB(240, 51, 58), true);
		}
		else
			ResetRemainFund();
	}
}

void VtOrderCenterWndHd::SetSymbol(VtSymbol* sym)
{
	if (!sym || !_OrderConfigMgr)
		return;

	_Symbol = sym;
	SetProductName(_Symbol);
	_ProductRemainGrid.SetSymbol(sym);

	if (_OrderConfigMgr->OrderMgr())
	{
		_OrderConfigMgr->OrderMgr()->CalcTotalProfitLoss(sym);
		VtProductOrderManager* PrdtOrderMgr = _OrderConfigMgr->OrderMgr()->FindAddProductOrderManager(sym->ShortCode);
		PrdtOrderMgr->OrderMgr(_OrderConfigMgr->OrderMgr());
	}
}

void VtOrderCenterWndHd::SetSymbol()
{
	if (!_Symbol || !_OrderConfigMgr)
		return;

	_ProductRemainGrid.SetSymbol(_Symbol);
	if (_OrderConfigMgr->Type() == 0)
	{
		if (_OrderConfigMgr->OrderMgr())
		{
			_OrderConfigMgr->OrderMgr()->CalcTotalProfitLoss(_Symbol);
			VtProductOrderManager* PrdtOrderMgr = _OrderConfigMgr->OrderMgr()->FindAddProductOrderManager(_Symbol->ShortCode);
			PrdtOrderMgr->OrderMgr(_OrderConfigMgr->OrderMgr());
		}
	}
	else
	{
		VtFundOrderManager* fundOrderMgr = (VtFundOrderManager*)_OrderConfigMgr->OrderMgr();
		std::map<std::string, VtOrderManager*> orderMgrMap = fundOrderMgr->GetOrderManagerMap();
		for (auto it = orderMgrMap.begin(); it != orderMgrMap.end(); ++it)
		{
			VtOrderManager* orderMgr = it->second;
			orderMgr->CalcTotalProfitLoss(_Symbol);
			VtProductOrderManager* PrdtOrderMgr = orderMgr->FindAddProductOrderManager(_Symbol->ShortCode);
			PrdtOrderMgr->OrderMgr(orderMgr);
		}
	}
}

void VtOrderCenterWndHd::SetAccount(VtAccount* acnt)
{
	if (!acnt)
		return;
	//_Account = acnt;

	//VtOrderManagerSelector* orderMgrSelecter = VtOrderManagerSelector::GetInstance();
	//_OrderMgr = orderMgrSelecter->FindOrderManager(acnt->AccountNo);
}

void VtOrderCenterWndHd::RegisterSymbolWnd()
{
	if (!_Symbol)
		return;

	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	orderDlgMgr->AddSymbolWnd(_Symbol->ShortCode, this);
}

void VtOrderCenterWndHd::UnregisterSymbolWnd()
{
	if (!_Symbol)
		return;
	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	orderDlgMgr->RemoveSymbolWnd(_Symbol->ShortCode, this);
}

void VtOrderCenterWndHd::RegisterRealtimeSymbol()
{
	if (!_Symbol)
		return;
	VtRealtimeRegisterManager* realtimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	realtimeRegiMgr->RegisterProduct(_Symbol->ShortCode);
}

void VtOrderCenterWndHd::UnregisterRealtimeSymbol()
{
	if (!_Symbol)
		return;
	VtRealtimeRegisterManager* realtimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	realtimeRegiMgr->UnregisterProduct(_Symbol->ShortCode);
}

void VtOrderCenterWndHd::RegisterRealtimeAccount()
{
	if (!_OrderConfigMgr)
		return;
	VtRealtimeRegisterManager* realtimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	if (_OrderConfigMgr->Type() == 0)
	{
		if (!_OrderConfigMgr->Account())
			return;
		
		realtimeRegiMgr->RegisterAccount(_OrderConfigMgr->Account()->AccountNo);
	}
	else
	{
		if (!_OrderConfigMgr->Fund())
			return;
		// Register the new account to the Event Map.
		std::set<VtAccount*> parendAcntSet = _OrderConfigMgr->Fund()->GetParentAccountSet();
		for (auto it = parendAcntSet.begin(); it != parendAcntSet.end(); ++it)
		{
			VtAccount* parentAcnt = *it;
			realtimeRegiMgr->RegisterAccount(parentAcnt->AccountNo);
		}
	}
}

void VtOrderCenterWndHd::UnregisterRealtimeAccount()
{
	if (!_OrderConfigMgr)
		return;
	VtRealtimeRegisterManager* realtimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	if (_OrderConfigMgr->Type() == 0)
	{
		if (!_OrderConfigMgr->Account())
			return;
		realtimeRegiMgr->UnregisterAccount(_OrderConfigMgr->Account()->AccountNo);
	}
	else
	{
		std::set<VtAccount*> parendAcntSet = _OrderConfigMgr->Fund()->GetParentAccountSet();
		for (auto it = parendAcntSet.begin(); it != parendAcntSet.end(); ++it)
		{
			VtAccount* acnt = *it;
			realtimeRegiMgr->UnregisterAccount(acnt->AccountNo);
		}
	}
}


void VtOrderCenterWndHd::RegisterOrderWnd()
{
	if (!_Symbol || !_OrderConfigMgr)
		return;

	if (_OrderConfigMgr->Type() == 0)
	{
		if (!_OrderConfigMgr->Account())
			return;
	}
	else
		if (!_OrderConfigMgr->Fund())
			return;

	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	std::string key;
	if (_OrderConfigMgr->Type() == 0)
		key = _OrderConfigMgr->Account()->AccountNo;
	else
		key = _OrderConfigMgr->Fund()->Name;
	key.append(_T(":"));
	key.append(_Symbol->ShortCode);
	orderDlgMgr->AddOrderWnd(key, this);
}

/*
void VtOrderCenterWndHd::UnregisterOrderWnd()
{
	_Unregistered = true;

	if (!_Symbol || !_OrderConfigMgr)
		return;
	if (_OrderConfigMgr->Type() == 0)
	{
		if (!_OrderConfigMgr->OldAccount())
			return;
	}
	else
		if (!_OrderConfigMgr->OldFund())
			return;

	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	std::string key;
	if (_OrderConfigMgr->Type() == 0)
		key = _OrderConfigMgr->OldAccount()->AccountNo;
	else
		key = _OrderConfigMgr->OldFund()->Name;
	key.append(_T(":"));
	key.append(_Symbol->ShortCode);
	orderDlgMgr->RemoveOrderWnd(key, this);
}
*/


void VtOrderCenterWndHd::UnregisterOrderWnd(bool useCurParent)
{
	_Unregistered = true;

	if (!_Symbol || !_OrderConfigMgr)
		return;
	std::string key;
	if (useCurParent)
	{
		if (_OrderConfigMgr->Type() == 0)
		{
			if (!_OrderConfigMgr->Account())
				return;
			key = _OrderConfigMgr->Account()->AccountNo;
		}
		else
		{
			if (!_OrderConfigMgr->Fund())
				return;
			key = _OrderConfigMgr->Fund()->Name;
		}
	}
	else
	{
		if (_OrderConfigMgr->Type() == 0)
		{
			if (!_OrderConfigMgr->OldAccount())
				return;
			key = _OrderConfigMgr->OldAccount()->AccountNo;
		}
		else
		{
			if (!_OrderConfigMgr->OldFund())
				return;
			key = _OrderConfigMgr->OldFund()->Name;
		}
	}

	key.append(_T(":"));
	key.append(_Symbol->ShortCode);
	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	orderDlgMgr->RemoveOrderWnd(key, this);
}

void VtOrderCenterWndHd::OnCbnSelchangeComboProductHd()
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

void VtOrderCenterWndHd::ClearPosition()
{
	_ProductRemainGrid.ClearPosition();
}

/*
void VtOrderCenterWndHd::InitAccount()
{
	if (_OrderConfigMgr)
	{
		SetAccount(_OrderConfigMgr->Account());
	}
}


void VtOrderCenterWndHd::InitFund()
{
	if (_OrderConfigMgr)
	{
		SetFund(_OrderConfigMgr->Fund());
	}
}
*/
void VtOrderCenterWndHd::InitSymbol()
{
	// 기본 심볼 설정
	VtSymbol* sym = _DefaultSymbol;

	if (!sym) { // 기본 심볼이 없으면 목록에서 첫번째 것을 가져온다.
		VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
		std::string secName = prdtCatMgr->MainFutureVector.front();
		VtProductSection* section = prdtCatMgr->FindProductSection(secName);
		if (section) {
			if (section->SubSectionVector.size() > 0) {
				VtProductSubSection* subSection = section->SubSectionVector.front();
				if (subSection->_SymbolVector.size() > 0) {
					sym = subSection->_SymbolVector.front();
				}
			}
		}
	}
	SetSymbol(sym);
	AddSymbolToCombo(sym);
	SetProductName(sym);
}

void VtOrderCenterWndHd::InitPosition()
{
	_ProductRemainGrid.InitPosition();
}

void VtOrderCenterWndHd::InitOrder()
{
	
}

void VtOrderCenterWndHd::FixedCenter(bool val)
{
	ResetByCenterRow();
	_FixedCenter = val;
}

void VtOrderCenterWndHd::OnBnClickedCkFixedCenter()
{
	// TODO: Add your control notification handler code here
	_FixedCenter = !_FixedCenter;
	_OrderPanelGrid.InitInfo();
}


void VtOrderCenterWndHd::OnBnClickedBtnOrderAmt1()
{
	// TODO: Add your control notification handler code here
	CString strText;
	_BtnAmt1.GetWindowText(strText);
	_EditOrderAmount.SetWindowText(strText);
	_OrderAmount = _ttoi(strText);
}


void VtOrderCenterWndHd::OnBnClickedBtnOrderAmt2()
{
	// TODO: Add your control notification handler code here
	CString strText;
	_BtnAmt2.GetWindowText(strText);
	_EditOrderAmount.SetWindowText(strText);
	_OrderAmount = _ttoi(strText);
}


void VtOrderCenterWndHd::OnBnClickedBtnOrderAmt3()
{
	// TODO: Add your control notification handler code here
	CString strText;
	_BtnAmt3.GetWindowText(strText);
	_EditOrderAmount.SetWindowText(strText);
	_OrderAmount = _ttoi(strText);
}


void VtOrderCenterWndHd::OnBnClickedBtnOrderAmt4()
{
	// TODO: Add your control notification handler code here
	CString strText;
	_BtnAmt4.GetWindowText(strText);
	_EditOrderAmount.SetWindowText(strText);
	_OrderAmount = _ttoi(strText);
}


void VtOrderCenterWndHd::OnBnClickedBtnOrderAmt5()
{
	// TODO: Add your control notification handler code here
	CString strText;
	_BtnAmt5.GetWindowText(strText);
	_EditOrderAmount.SetWindowText(strText);
	_OrderAmount = _ttoi(strText);
}


void VtOrderCenterWndHd::OnBnClickedBtnOrderAmt6()
{
	// TODO: Add your control notification handler code here
	CString strText;
	_BtnAmt6.GetWindowText(strText);
	_EditOrderAmount.SetWindowText(strText);
	_OrderAmount = _ttoi(strText);
}


void VtOrderCenterWndHd::OnDeltaposSpinOrderAmount(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void VtOrderCenterWndHd::OnEnChangeEditOrderAmount()
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


void VtOrderCenterWndHd::OnEnChangeEditStopval()
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


void VtOrderCenterWndHd::OnBnClickedBtnLiq()
{
	// TODO: Add your control notification handler code here
	_OrderPanelGrid.LiqudAll(VtPriceType::Market, 0);
	ResetRemainFund();
}


void VtOrderCenterWndHd::OnBnClickedBtnRemain()
{
	if (!_OrderConfigMgr)
		return;

	if (_OrderConfigMgr->Type() == 0)
	{
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
}


void VtOrderCenterWndHd::OnBnClickedBtnSelSymbol()
{
	// TODO: Add your control notification handler code here
	HdSymbolSelecter symSelecter;
	symSelecter.OrderConfigMgr(_OrderConfigMgr);
	symSelecter.DoModal();
}

void VtOrderCenterWndHd::ChangeFund(VtFund* fund)
{
	if (!fund)
		return;
	ClearPosition();
	UnregisterOrderWnd(false);
	SetSymbol();
	InitGridInfo();
	InitPosition();
	RegisterOrderWnd();
}

void VtOrderCenterWndHd::ChangeAccount(VtAccount* acnt)
{
	if (!acnt)
		return;
	ClearPosition();
	UnregisterOrderWnd(false);
	SetSymbol();
	InitGridInfo();
	InitPosition();
	RegisterOrderWnd();
}

void VtOrderCenterWndHd::ChangeSymbol(VtSymbol* newSymbol)
{
	if (!newSymbol)
		return;
	_TickGrid.ClearValues();
	ClearPosition();
	UnregisterOrderWnd(true);
	UnregisterRealtimeSymbol();
	UnregisterSymbolWnd();
	SetSymbol(newSymbol);
	AddSymbolToCombo(newSymbol);
	SetProductName(newSymbol);
	InitGridInfo();
	InitPosition();
	RegisterSymbolWnd();
	RegisterRealtimeSymbol();
	RegisterOrderWnd();
}

void VtOrderCenterWndHd::AddSymbolToCombo(VtSymbol* symbol)
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

void VtOrderCenterWndHd::SetProductName(VtSymbol* symbol)
{
	if (!symbol)
		return;

	_StaticProductName.SetWindowText(symbol->Name.c_str());
	_StaticProductName.Invalidate();
}

void VtOrderCenterWndHd::ChangeFocus()
{
	SendMessage(WM_NEXTDLGCTL, (WPARAM)_OrderPanelGrid.GetSafeHwnd(), TRUE);
}

void VtOrderCenterWndHd::OnPaint()
{
// 	CPaintDC dc(this);
//  	CRect rect;
//  	GetClientRect(&rect);
//  	if (_Activated)
//  		dc.FillSolidRect(&rect, RGB(255, 255, 0));
//  	else
//  		dc.FillSolidRect(&rect, GetSysColor(COLOR_3DFACE));
 	CDialogEx::OnPaint();
}

void VtOrderCenterWndHd::OnDeltaposSpinExpand(NMHDR *pNMHDR, LRESULT *pResult)
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


void VtOrderCenterWndHd::OnStnClickedStaticMsg()
{
	// TODO: Add your control notification handler code here
}


BOOL VtOrderCenterWndHd::OnEraseBkgnd(CDC* pDC)
{
	//if (_ParentDlg && _ParentDlg->GetCenterWndCount() == 1)
	//	return FALSE;
	/*
	// 주문그리드 위치를 가져온다.
	CRect& rcGrid = _LayoutMgr->GetRect(IDC_STATIC_ORDER_PANEL);

	if (Activated())
		pDC->SelectObject(&_BrushBackSel);
	else
		pDC->SelectObject(&_BrushBackNor);

	CRect rect;
	GetClientRect(&rect);
	rect.bottom = rcGrid.top;

	if (Activated())
		pDC->FillRect(&rect, &_BrushBackSel);
	else
		pDC->FillRect(&rect, &_BrushBackNor);
	
	return TRUE;
	*/
	return TRUE;
}


void VtOrderCenterWndHd::OnBnClickedBtnShowRealtick()
{
	// TODO: Add your control notification handler code here
	_TickWndPos++;
	if (_TickWndPos > 2)
		_TickWndPos = 0;
	if (_OrderConfigMgr && _OrderConfigMgr->_HdOrderWnd)
	{
		_OrderConfigMgr->_HdOrderWnd->RecalcLayout(true);
	}
}


void VtOrderCenterWndHd::OnBnClickedStaticFillPl()
{
	// TODO: Add your control notification handler code here
}

void VtOrderCenterWndHd::CreateChildWindow(VtOrderConfigDlg* centerWnd, UINT id, CWnd* parent)
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


void VtOrderCenterWndHd::OnBnClickedButtonSetting()
{
	if (_ParentDlg) {
		_ParentDlg->SetActiveCenterWnd(this);
	}
	ClearConfigDlg();
	VtOrderGridConfig* grid = new VtOrderGridConfig();
	grid->CenterWnd(this);
	_ConfigDlgVector.push_back(grid);
	grid->Create(IDD_ORDERGRID_CONFIG, this);
	grid->ShowWindow(SW_SHOW);
}

void VtOrderCenterWndHd::ShowOrderAreaInGrid(bool flag)
{
	_ShowOrderArea = flag;
	_OrderGridColOption[0] = flag;
	_OrderPanelGrid.ShowHideOrderGrid(_OrderGridColOption);
}

void VtOrderCenterWndHd::ShowStopAreaInGrid(bool flag)
{
	_ShowStopArea = flag;
	_OrderGridColOption[1] = flag;
	_OrderPanelGrid.ShowHideOrderGrid(_OrderGridColOption);
}

void VtOrderCenterWndHd::ShowOrderCountInGrid(bool flag)
{
	_ShowOrderCountArea = flag;
	_OrderGridColOption[2] = flag;
	_OrderPanelGrid.ShowHideOrderGrid(_OrderGridColOption);
	if (!_ShowStopArea && !_ShowOrderCountArea) {
		_ProductRemainGrid.SetShowAvage(false);
		_ProductRemainGrid.SetShowRemainType(false);
	} else {
		_ProductRemainGrid.SetShowAvage(true);
		_ProductRemainGrid.SetShowRemainType(true);
	}
}

void VtOrderCenterWndHd::ShowTickWnd(bool flag)
{
	_ShowTickWnd = flag;
	if (flag) {
		_TickGrid.ShowWindow(SW_SHOW);
	}
	else {
		_TickGrid.ShowWindow(SW_HIDE);
	}
}

void VtOrderCenterWndHd::SetTickWndPos(int pos)
{
	_TickWndPos = pos;
}

void VtOrderCenterWndHd::SetTickCount(int count)
{
	_TickGrid.MaxRow(count);
}

void VtOrderCenterWndHd::SetOrderCellHeight(int value)
{
	_OrderPanelGrid.SetOrderCellHeight(value);
	ResizeOrderGrid();
}

void VtOrderCenterWndHd::SetOrderCellWidth(int value)
{
	_OrderPanelGrid.SetOrderCellHeight(value);
	ResizeOrderGrid();
}

void VtOrderCenterWndHd::SetShowPLConfigWnd(bool flag)
{
	_ShowRemainConfig = flag;
}

void VtOrderCenterWndHd::SetOrderArea(int height, int width)
{
	//_OrderPanelGrid.SetColTitle(false);
	_OrderPanelGrid.SetOrderArea(height, width);
	ResizeOrderGrid();
	_OrderPanelGrid.RedrawAll();
}

int VtOrderCenterWndHd::GetMaxWidth()
{
	//return _OrderPanelGrid.GetMaxWidth();
	if (_TickWndPos == 0)
		return _OrderPanelGrid.GetMaxWidth();
	else  // For the tick grid on the left or right side
	{
		CRect rcTick;
		_TickGrid.GetWindowRect(rcTick);
		return _OrderPanelGrid.GetMaxWidth() + rcTick.Width() + _GridGap;
	}
}

int VtOrderCenterWndHd::GetWindowWidth()
{
	if (!_LayoutMgr)
		return 0;
	int orderGridWidth = GetGridWidth();
	CRect& rcTick = _LayoutMgr->GetRect(IDC_STATIC_REAL_TICK);
	return _ShowTickWnd ? orderGridWidth + rcTick.Width() : orderGridWidth;
}

int VtOrderCenterWndHd::GetWindowHeight()
{
	return 0;
}

void VtOrderCenterWndHd::RecalcLayout()
{
	if (_OrderConfigMgr && _OrderConfigMgr->_HdOrderWnd)
	{
		_OrderConfigMgr->_HdOrderWnd->RefreshLayout(true, false);
	}

	//SetProductGridWidth();
}

void VtOrderCenterWndHd::SetMaxTickCount(int count)
{
	if (_RealTickWnd)
	{
		_RealTickWnd->SetMaxTickCount(count);
	}
}


void VtOrderCenterWndHd::OnBnClickedButtonProfitLoss()
{
	// TODO: Add your control notification handler code here
}


void VtOrderCenterWndHd::OnStnClickedStaticOrderPanel()
{
	// TODO: Add your control notification handler code here
}


BOOL VtOrderCenterWndHd::PreTranslateMessage(MSG* pMsg)
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

void VtOrderCenterWndHd::OnRClicked(NMHDR * pNotifyStruct, LRESULT * result)
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

void VtOrderCenterWndHd::OnEntered()
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

void VtOrderCenterWndHd::OnStnClickedStaticRealTick()
{
	// TODO: Add your control notification handler code here
}

void VtOrderCenterWndHd::Save(simple::file_ostream<same_endian_type>& ss)
{
	if (!_Symbol)
		return;
	// 종목코드
	ss << _Symbol->ShortCode;
	// 배열 요소를 직접 저장하지 말것.원하는 대로 저장되지 않음
	// 반드시 변수에 넣은 다음 저장할 것.
	// 그리드 주문열 표시여부
	bool val = _OrderGridColOption[0];
	ss << val;
	// 그리도 스탑열 표시여부
	val = _OrderGridColOption[1];
	ss << val;
	// 그리드 건수열 표시여부
	val = _OrderGridColOption[2];
	ss << val;
	// 틱윈도우 표시여부
	ss << _ShowTickWnd;
	// 틱 윈도우 위치
	ss << _TickWndPos;
	// 주문 수량
	ss << _OrderAmount;
	// 셀 높이
	ss << _OrderPanelGrid.CellHeight();
	// 셀 너비
	ss << _OrderPanelGrid.OrderWidth();
	// 손절 익절창 표시여부
	ss << _ShowRemainConfig;
}

void VtOrderCenterWndHd::Load(simple::file_istream<same_endian_type>& ss)
{
	std::string shortCode;
	int orderAmount;
	int cellHeight;
	int cellWidth;
	bool showOrderCol;
	bool showStopCol;
	bool showCountCol;
	bool showTickWnd;
	int tickWndPos;
	// 종목코드 복원
	ss >> shortCode;
	// 그리드 주문열 표시 여부 복원
	ss >> showOrderCol;
	// 그리드 스탑열 표시 여부 복원
	ss >> showStopCol;
	// 그리드 건수 표시 여부 복원
	ss >> showCountCol;
	// 틱윈도우 보이기 복원
	ss >> showTickWnd;
	// 틱 윈도우 위치 복원
	ss >> tickWndPos;
	// 주문 개수 복원
	ss >> orderAmount;
	// 주문창 셀 높이 복원
	ss >> cellHeight;
	// 주문창 셀 너비 복원
	ss >> cellWidth;
	// 손절익절창 표시여부
	ss >> _ShowRemainConfig;
	// 주문창 그리드 속성 대입
	_OrderGridColOption[0] = showOrderCol;
	_ShowOrderArea = showOrderCol;
	_OrderGridColOption[1] = showStopCol;
	_ShowStopArea = showStopCol;
	_OrderGridColOption[2] = showCountCol;
	_ShowOrderCountArea = showCountCol;

	// 틱윈도우 속성 대입
	_ShowTickWnd = showTickWnd;
	_TickWndPos = tickWndPos;

	// 심볼 대입
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	_DefaultSymbol = symMgr->FindHdSymbol(shortCode);
	// 주문 개수 복원
	_OrderAmount = orderAmount;
	// 셀 높이 복원
	_OrderPanelGrid.CellHeight(cellHeight);
	// 셀 너비 복원
	_OrderPanelGrid.OrderWidth(cellWidth);

	// 저장된 심볼은 목록으로 만들어 심볼 마스터 요청한다.
	VtSaveManager* saveMgr = VtSaveManager::GetInstance();
	std::vector<VtSymbol*>& symvec = saveMgr->GetSymbolVector();
	if (_DefaultSymbol)
		symvec.push_back(_DefaultSymbol);
}

void VtOrderCenterWndHd::ApplyProfitLossForPosition()
{
	_OrderPanelGrid.ApplyProfitLossForPosition();
}


int VtOrderCenterWndHd::GetClientAreaWidth()
{
	if (!_LayoutMgr)
		return 0;

	CRect rcClient;
	GetClientRect(rcClient);
	CRect& rcGrid = _LayoutMgr->GetRect(IDC_STATIC_ORDER_PANEL);
	CRect& rcTick = _LayoutMgr->GetRect(IDC_STATIC_REAL_TICK);
	return _ShowTickWnd ? rcGrid.Width() + rcTick.Width() : rcGrid.Width();
}

void VtOrderCenterWndHd::AddDefaultControls()
{
	if (!_LayoutMgr)
		return;


}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	int nCtrlID = ::GetDlgCtrlID(hwnd);
	VtLayoutManager* layoutMgr = (VtLayoutManager*)lParam;
	if (layoutMgr) {
		layoutMgr->AddWindow(nCtrlID);
	}

	LOG_F(INFO, _T("resourceid = %d"), nCtrlID);
	return TRUE;
}

BOOL VtOrderCenterWndHd::EnumerateDialogResource()
{
	EnumChildWindows(this->GetSafeHwnd(), EnumWindowsProc, (LPARAM)_LayoutMgr);
	return TRUE;
}

int VtOrderCenterWndHd::GetCountOrderGridEnabledCol()
{
	return std::count_if(_OrderGridColOption.begin(), _OrderGridColOption.end(), [](auto i) { return i; });
}

//IDC_BTN_REMAIN
void VtOrderCenterWndHd::SaveControlPos()
{
	if (!_LayoutMgr)
		return;
	CRect rcWnd;
	_LayoutMgr->AddWindow(_T("모두청산"), IDC_BTN_LIQ, GetClientArea(IDC_BTN_LIQ));
	_LayoutMgr->AddWindow(_T("체결된잔고"), IDC_BTN_REMAIN_FUND, GetClientArea(IDC_BTN_REMAIN_FUND));
	_LayoutMgr->AddWindow(_T("스탑스핀"), IDC_SPIN_STOPVAL, GetClientArea(IDC_SPIN_STOPVAL));
	_LayoutMgr->AddWindow(_T("호가고정"), IDC_CK_FIXED_CENTER, GetClientArea(IDC_CK_FIXED_CENTER));
	_LayoutMgr->AddWindow(_T("스탑레이블"), IDC_STATIC_STOP, GetClientArea(IDC_STATIC_STOP));
	_LayoutMgr->AddWindow(_T("스탑입력"), IDC_EDIT_STOPVAL, GetClientArea(IDC_EDIT_STOPVAL));
	_LayoutMgr->AddWindow(_T("잔고그리드"), IDC_STATIC_PRODUCT_REMAIN, GetClientArea(IDC_STATIC_PRODUCT_REMAIN));
	_LayoutMgr->AddWindow(_T("주문창그리드"), IDC_STATIC_ORDER_PANEL, GetClientArea(IDC_STATIC_ORDER_PANEL));
	_LayoutMgr->AddWindow(_T("틱그리드"), IDC_STATIC_REAL_TICK, GetClientArea(IDC_STATIC_REAL_TICK));
	_LayoutMgr->AddWindow(_T("설정창"), IDC_STATIC_MSG, GetClientArea(IDC_STATIC_MSG));
	_LayoutMgr->AddWindow(_T("잔고버튼"), IDC_BTN_REMAIN, GetClientArea(IDC_BTN_REMAIN));
}

CRect VtOrderCenterWndHd::GetClientArea(int resourceID)
{
	CWnd* wnd = (CWnd*)GetDlgItem(resourceID);
	CRect rcWnd;
	wnd->GetWindowRect(rcWnd);
	ScreenToClient(rcWnd);

	return rcWnd;
}

void VtOrderCenterWndHd::OnBnClickedBtnRemainFund()
{
	if (!_OrderConfigMgr || !_Symbol)
		return;
	std::pair<bool, int> curRemain;
	if (_OrderConfigMgr->Type() == 0) { // 게좌 주문 일 때
		if (!_OrderConfigMgr->Account())
			return;
		curRemain = _OrderConfigMgr->Account()->GetRemainCount(_Symbol->ShortCode);
	} else { // 펀드 주문일 때
		if (!_OrderConfigMgr->Fund())
			return;
		curRemain = _OrderConfigMgr->Fund()->GetRemainCount(_Symbol->ShortCode);
	}

	// 잔고가 한번도 생기지 않았거나 0일 때는 처리하지 않는다.
	if (!curRemain.first || curRemain.second == 0)
		return;

	_OrderByRemain = !_OrderByRemain;
	if (_OrderByRemain) {
		if (curRemain.second < 0) {
			_BtnRemainFund.SetColor(BTNST_COLOR_FG_IN, RGB(255, 255, 255), true);
			_BtnRemainFund.SetColor(BTNST_COLOR_BK_IN, RGB(19, 137, 255), true);
			_BtnRemainFund.SetColor(BTNST_COLOR_FG_OUT, RGB(255, 255, 255), true);
			_BtnRemainFund.SetColor(BTNST_COLOR_BK_OUT, RGB(19, 137, 255), true);

			_BtnRemainFund.SetColor(BTNST_COLOR_FG_FOCUS, RGB(255, 255, 255), true);
			_BtnRemainFund.SetColor(BTNST_COLOR_BK_FOCUS, RGB(19, 137, 255), true);
		}
		else if (curRemain.second > 0) {
			_BtnRemainFund.SetColor(BTNST_COLOR_FG_IN, RGB(255, 255, 255), true);
			_BtnRemainFund.SetColor(BTNST_COLOR_BK_IN, RGB(240, 51, 58), true);
			_BtnRemainFund.SetColor(BTNST_COLOR_FG_OUT, RGB(255, 255, 255), true);
			_BtnRemainFund.SetColor(BTNST_COLOR_BK_OUT, RGB(240, 51, 58), true);

			_BtnRemainFund.SetColor(BTNST_COLOR_FG_FOCUS, RGB(255, 255, 255), true);
			_BtnRemainFund.SetColor(BTNST_COLOR_BK_FOCUS, RGB(240, 51, 58), true);
		}
	} else {
		ResetRemainFund();
	}
}

void VtOrderCenterWndHd::ResetRemainFund()
{
	_OrderByRemain = false;
	_BtnRemainFund.SetColor(BTNST_COLOR_FG_IN, RGB(0, 0, 0), true);
	_BtnRemainFund.SetColor(BTNST_COLOR_FG_OUT, RGB(0, 0, 0), true);
	_BtnRemainFund.SetColor(BTNST_COLOR_FG_FOCUS, RGB(0, 0, 0), true);
	_BtnRemainFund.SetColor(BTNST_COLOR_BK_IN, RGB(220, 220, 220), true);
	_BtnRemainFund.SetColor(BTNST_COLOR_BK_OUT, RGB(220, 220, 220), true);
	_BtnRemainFund.SetColor(BTNST_COLOR_BK_FOCUS, RGB(220, 220, 220), true);
}

void VtOrderCenterWndHd::CalcLayout()
{
	if (!_LayoutMgr)
		return;
	int availableHeight = GetParentHeight() - CenterTop - GetParentTitleBarHeight();
	// 주문그리드 옵션에서 선택된 칼럼의 수를 가져온다.
	int colCnt = GetCountOrderGridEnabledCol();
	int orderGridWidth = GetGridWidth();
	// 그리드 컬럼 옵션을 설정해 준다.
	_OrderPanelGrid.ShowHideOrderGrid(_OrderGridColOption);
	// 틱윈도우 위치에 따라 주문 그리드 시작 위치를 결정해 준다.
	//int orderGridLeft = _ShowTickWnd ? ( _TickWndPos == 1 ? _LayoutMgr->GetWidth(IDC_STATIC_REAL_TICK) : 0 ) : 0;
	int orderGridLeft = _ShowTickWnd ? (_TickWndPos == 1 ? _TickGrid.GetGridWidth() : 0) : 0;
	// 주문그리드 위치를 가져온다.
	CRect& rcGrid = _LayoutMgr->GetRect(IDC_STATIC_ORDER_PANEL);
	// 주문설정 보기 옵션에 따라 주문 그리드 하단을 설정한다.
	int orderGridBottom = _ShowRemainConfig ? availableHeight - rcGrid.top - ConfigHeight : availableHeight - rcGrid.top;
	CRect& rcTick = _LayoutMgr->GetRect(IDC_STATIC_REAL_TICK);
	int clientWidth = _ShowTickWnd ? orderGridWidth + TickWndWidth : orderGridWidth;
	int remainGridWidth = GetRemainWidth();
	// 주문 그리드 위치 및 크기 설정
	rcGrid.left = orderGridLeft;
	rcGrid.right = rcGrid.left + orderGridWidth;
	rcGrid.bottom = rcGrid.top + orderGridBottom;

	// 하단 주문설정 위치를 가져온다.
	CRect& rcConfig = _LayoutMgr->GetRect(IDC_STATIC_MSG);
	// 잔고 설정 위치 및 크기 설정
	rcConfig.top = _ShowRemainConfig ? availableHeight - ConfigHeight : availableHeight;
	rcConfig.bottom = rcConfig.top + ConfigHeight;
	rcConfig.left = 0;
	//rcConfig.right = rcConfig.left + clientWidth > ConfigWidth ?  ConfigWidth : clientWidth;
	rcConfig.right = rcConfig.left + clientWidth;

	// 틱윈도우 위치 및 크기 설정
	int tickBottom = _ShowRemainConfig ? availableHeight - rcTick.top - ConfigHeight : availableHeight - rcTick.top;
	rcTick.left = _ShowTickWnd ? (_TickWndPos == 1 ? 0 : orderGridWidth) : orderGridWidth;
	rcTick.right = rcTick.left + TickWndWidth;
	rcTick.bottom = rcTick.top + tickBottom;

	// 잔고그리드 위치 및 크기 설정
	CRect& rcRemainGrid = _LayoutMgr->GetRect(IDC_STATIC_PRODUCT_REMAIN);
	rcRemainGrid.right = rcRemainGrid.left + remainGridWidth;
	// 잔고 버튼
	CRect& rcRemain = _LayoutMgr->GetRect(IDC_BTN_REMAIN);
	// 체결된 잔고 위치 및 크기 설정
	CRect& rcRemainFund = _LayoutMgr->GetRect(IDC_BTN_REMAIN_FUND);
	int fundRemainWidth = rcRemainFund.Width();
	int fundRemainHeight = rcRemainFund.Height();
	rcRemainFund.top = rcRemain.top;
	rcRemainFund.bottom = rcRemain.bottom;
	rcRemainFund.left = rcRemain.right;
	rcRemainFund.right = rcRemainFund.left + fundRemainWidth;
	// 스탑 위치, 호가고정 위치 설정
	CRect& rcStaticStop = _LayoutMgr->GetRect(IDC_STATIC_STOP);
	CRect& rcEditStop = _LayoutMgr->GetRect(IDC_EDIT_STOPVAL);
	CRect& rcSpinStop = _LayoutMgr->GetRect(IDC_SPIN_STOPVAL);
	CRect& rcFixedCenter = _LayoutMgr->GetRect(IDC_CK_FIXED_CENTER);
	int gap = 0;
	int ctrlWidth = rcFixedCenter.Width();
	int ctrlHeight = rcFixedCenter.Height();
	rcFixedCenter.left = remainGridWidth + gap;
	rcFixedCenter.right = rcFixedCenter.left + ctrlWidth;

	ctrlWidth = rcStaticStop.Width();
	ctrlHeight = rcStaticStop.Height();
	rcStaticStop.left = remainGridWidth + gap;
	rcStaticStop.right = rcStaticStop.left + ctrlWidth;

	ctrlWidth = rcEditStop.Width();
	ctrlHeight = rcEditStop.Height();
	rcEditStop.left = rcStaticStop.right + gap;
	rcEditStop.right = rcEditStop.left + ctrlWidth;

	ctrlWidth = rcSpinStop.Width();
	ctrlHeight = rcSpinStop.Height();
	rcSpinStop.left = rcEditStop.right + gap;
	rcSpinStop.right = rcSpinStop.left + ctrlWidth;
}

void VtOrderCenterWndHd::RefreshLayout(bool recalGrid)
{
	CalcLayout();
	RepositionControl();
	_OrderPanelGrid.ResizeGrid(0);
	RepositionProductGrid();
	RepositionConfigWnd();
	RepositionTickGrid();
}

int VtOrderCenterWndHd::GetParentHeight()
{
	if (!_ParentDlg)
		return -1;
	return _ParentDlg->WindowHeight() == 0 ? _ParentDlg->DefaultHeight() : _ParentDlg->WindowHeight();
}

int VtOrderCenterWndHd::GetParentTitleBarHeight()
{
	if (!_ParentDlg)
		return -1;
	return _ParentDlg->GetTitleBarHeight();
}

int VtOrderCenterWndHd::GetGridWidth()
{
	return _OrderPanelGrid.GetGridWidth(_OrderGridColOption) + 6;
}

int VtOrderCenterWndHd::GetRemainWidth(int parentWidth)
{
	return _ProductRemainGrid.GetGridWidth(parentWidth) + 6;
}

int VtOrderCenterWndHd::GetRemainWidth()
{
	return _ProductRemainGrid.GetGridWidth(_OrderGridColOption) + 6;
}


HBRUSH VtOrderCenterWndHd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	/*
	HBRUSH hBrush = NULL;
	switch (nCtlColor)
	{
// 	case CTLCOLOR_DLG:
// 		// just return a not NULL brush handle
// 		if (Activated())
// 			hBrush = (HBRUSH)_BrushBackSel;
// 		else
// 			hBrush = (HBRUSH)_BrushBackNor;
// 		break;
	case CTLCOLOR_STATIC:
	{
		// set text color, transparent back node then 
		//pDC->SetTextColor(m_crStaticText);
		pDC->SetBkMode(TRANSPARENT);
		// return a not NULL brush handle
		//hBrush = (HBRUSH)m_brushBack;
		if (Activated())
			hBrush = (HBRUSH)_BrushBackSel;
		else
			hBrush = (HBRUSH)_BrushBackNor;
	}
	break;
	default:
		// do the default processing
		hBrush = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
		break;
	}
	return hBrush;
	*/
}


void VtOrderCenterWndHd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (_ParentDlg) {
		_ParentDlg->SetActiveCenterWnd(this);
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}

LRESULT VtOrderCenterWndHd::OnRefreshGrid(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	//_OrderPanelGrid.RefreshGrid();
	_OrderPanelGrid.RefreshInfo();

	return 0;
}
