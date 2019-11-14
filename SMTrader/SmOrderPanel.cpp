// SmOrderPanel.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "SmOrderPanel.h"
#include "afxdialogex.h"
#include "SmCallbackManager.h"
#include <functional>

#include "resource.h"
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

using namespace std;
using namespace std::placeholders;


// SmOrderPanel dialog

IMPLEMENT_DYNAMIC(SmOrderPanel, CDialogEx)

SmOrderPanel::SmOrderPanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ORDER_PANEL, pParent)
{
	_ParentDlg = nullptr;
	_Symbol = nullptr;
	_OrderAmount = 1;
	_FillCondition = VtFilledCondition::Fas;
	_PriceType = VtPriceType::Price;
	_FixedCenter = false;
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
	m_Grid.CenterWnd(this);
	_ProductRemainGrid.CenterWnd(this);
	_Unregistered = false;
	_OrderByRemain = false;
	_LayoutMgr = new VtLayoutManager((CWnd*)this);

	_OrderGridColOption.push_back(true);
	_OrderGridColOption.push_back(true);
	_OrderGridColOption.push_back(true);
}

SmOrderPanel::~SmOrderPanel()
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

void SmOrderPanel::OrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
	m_Grid.OrderConfigMgr(val);
}

void SmOrderPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ORDER_GRID, m_Grid);

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


BEGIN_MESSAGE_MAP(SmOrderPanel, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_PRODUCT_HD, &SmOrderPanel::OnCbnSelchangeComboProductHd)
	ON_BN_CLICKED(IDC_CK_FIXED_CENTER, &SmOrderPanel::OnBnClickedCkFixedCenter)
	ON_BN_CLICKED(IDC_BTN_ORDER_AMT1, &SmOrderPanel::OnBnClickedBtnOrderAmt1)
	ON_BN_CLICKED(IDC_BTN_ORDER_AMT2, &SmOrderPanel::OnBnClickedBtnOrderAmt2)
	ON_BN_CLICKED(IDC_BTN_ORDER_AMT3, &SmOrderPanel::OnBnClickedBtnOrderAmt3)
	ON_BN_CLICKED(IDC_BTN_ORDER_AMT4, &SmOrderPanel::OnBnClickedBtnOrderAmt4)
	ON_BN_CLICKED(IDC_BTN_ORDER_AMT5, &SmOrderPanel::OnBnClickedBtnOrderAmt5)
	ON_BN_CLICKED(IDC_BTN_ORDER_AMT6, &SmOrderPanel::OnBnClickedBtnOrderAmt6)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ORDER_AMOUNT, &SmOrderPanel::OnDeltaposSpinOrderAmount)
	ON_EN_CHANGE(IDC_EDIT_ORDER_AMOUNT, &SmOrderPanel::OnEnChangeEditOrderAmount)
	ON_EN_CHANGE(IDC_EDIT_STOPVAL, &SmOrderPanel::OnEnChangeEditStopval)
	ON_BN_CLICKED(IDC_BTN_LIQ, &SmOrderPanel::OnBnClickedBtnLiq)
	ON_BN_CLICKED(IDC_BTN_REMAIN, &SmOrderPanel::OnBnClickedBtnRemain)
	ON_BN_CLICKED(IDC_BTN_SEL_SYMBOL, &SmOrderPanel::OnBnClickedBtnSelSymbol)
	ON_STN_CLICKED(IDC_STATIC_MSG, &SmOrderPanel::OnStnClickedStaticMsg)
	ON_BN_CLICKED(IDC_STATIC_FILL_PL, &SmOrderPanel::OnBnClickedStaticFillPl)
	ON_BN_CLICKED(IDC_BUTTON_SETTING, &SmOrderPanel::OnBnClickedButtonSetting)
	ON_BN_CLICKED(IDC_BUTTON_PROFIT_LOSS, &SmOrderPanel::OnBnClickedButtonProfitLoss)
	ON_STN_CLICKED(IDC_STATIC_ORDER_PANEL, &SmOrderPanel::OnStnClickedStaticOrderPanel)
	ON_NOTIFY(NM_RCLICK, IDC_BTN_ORDER_AMT1, &SmOrderPanel::OnRClicked)
	ON_NOTIFY(NM_RCLICK, IDC_BTN_ORDER_AMT2, &SmOrderPanel::OnRClicked)
	ON_NOTIFY(NM_RCLICK, IDC_BTN_ORDER_AMT3, &SmOrderPanel::OnRClicked)
	ON_NOTIFY(NM_RCLICK, IDC_BTN_ORDER_AMT4, &SmOrderPanel::OnRClicked)
	ON_NOTIFY(NM_RCLICK, IDC_BTN_ORDER_AMT5, &SmOrderPanel::OnRClicked)
	ON_NOTIFY(NM_RCLICK, IDC_BTN_ORDER_AMT6, &SmOrderPanel::OnRClicked)
	ON_STN_CLICKED(IDC_STATIC_REAL_TICK, &SmOrderPanel::OnStnClickedStaticRealTick)
	ON_BN_CLICKED(IDC_BTN_REMAIN_FUND, &SmOrderPanel::OnBnClickedBtnRemainFund)
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// SmOrderPanel message handlers

void SmOrderPanel::OnBnClickedButtonSetting()
{

}

void SmOrderPanel::OnDestroy()
{

}

void SmOrderPanel::OnDeltaposSpinExpand(NMHDR *pNMHDR, LRESULT *pResult)
{

}

void SmOrderPanel::OnStnClickedStaticMsg()
{

}

void SmOrderPanel::OnBnClickedBtnShowRealtick()
{

}

void SmOrderPanel::OnBnClickedStaticFillPl()
{

}

void SmOrderPanel::OnCbnSelchangeComboProductHd()
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


void SmOrderPanel::OnBnClickedCkFixedCenter()
{

}

void SmOrderPanel::OnBnClickedBtnOrderAmt1()
{

}

void SmOrderPanel::OnBnClickedBtnOrderAmt2()
{

}

void SmOrderPanel::OnBnClickedBtnOrderAmt3()
{

}

void SmOrderPanel::OnBnClickedBtnOrderAmt4()
{

}

void SmOrderPanel::OnBnClickedBtnOrderAmt5()
{

}

void SmOrderPanel::OnBnClickedBtnOrderAmt6()
{

}

void SmOrderPanel::OnDeltaposSpinOrderAmount(NMHDR *pNMHDR, LRESULT *pResult)
{

}

void SmOrderPanel::OnEnChangeEditOrderAmount()
{

}

void SmOrderPanel::OnEnChangeEditStopval()
{

}

void SmOrderPanel::OnBnClickedBtnLiq()
{

}

void SmOrderPanel::OnBnClickedBtnRemain()
{

}

void SmOrderPanel::OnBnClickedBtnSelSymbol()
{

}

void SmOrderPanel::OnBnClickedButtonProfitLoss()
{

}

void SmOrderPanel::OnStnClickedStaticOrderPanel()
{

}

void SmOrderPanel::OnRClicked(NMHDR * pNotifyStruct, LRESULT * result)
{

}

void SmOrderPanel::OnStnClickedStaticRealTick()
{

}

void SmOrderPanel::OnBnClickedBtnRemainFund()
{

}

void SmOrderPanel::OnLButtonDown(UINT nFlags, CPoint point)
{

}

void SmOrderPanel::CreateChildWindow(VtOrderConfigDlg* centerWnd, UINT id, CWnd* parent)
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

void SmOrderPanel::SetRemain(int remain)
{
	if (!_OrderConfigMgr)
		return;

	CString strValue;
	if (_OrderConfigMgr->Type() == 0) {
		strValue.Format(_T("잔고(%d)"), remain);
		_BtnRemain.SetWindowText(strValue);
	}
	else {
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

void SmOrderPanel::RefreshOrderPositon()
{
	
}

void SmOrderPanel::ClearConfigDlg()
{
	for (auto it = _ConfigDlgVector.begin(); it != _ConfigDlgVector.end(); ++it) {
		VtOrderGridConfig* dlg = *it;
		dlg->DestroyWindow();
		delete dlg;
	}

	_ConfigDlgVector.clear();
}

void SmOrderPanel::SaveControlPos()
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
	_LayoutMgr->AddWindow(_T("주문창그리드"), IDC_ORDER_GRID, GetClientArea(IDC_ORDER_GRID));
	_LayoutMgr->AddWindow(_T("틱그리드"), IDC_STATIC_REAL_TICK, GetClientArea(IDC_STATIC_REAL_TICK));
	_LayoutMgr->AddWindow(_T("설정창"), IDC_STATIC_MSG, GetClientArea(IDC_STATIC_MSG));
	_LayoutMgr->AddWindow(_T("잔고버튼"), IDC_BTN_REMAIN, GetClientArea(IDC_BTN_REMAIN));
}

void SmOrderPanel::CalcLayout()
{
	if (!_LayoutMgr)
		return;
	// 전체 주문창의 가용 높이를 가져온다.
	int availableHeight = GetParentHeight() - CenterTop - GetParentTitleBarHeight();
	// 주문그리드 옵션에서 선택된 칼럼의 수를 가져온다.
	int colCnt = GetCountOrderGridEnabledCol();
	int orderGridWidth = GetGridWidth();
	// 틱윈도우 위치에 따라 주문 그리드 시작 위치를 결정해 준다.
	//int orderGridLeft = _ShowTickWnd ? ( _TickWndPos == 1 ? _LayoutMgr->GetWidth(IDC_STATIC_REAL_TICK) : 0 ) : 0;
	int orderGridLeft = _ShowTickWnd ? (_TickWndPos == 1 ? _TickGrid.GetGridWidth() : 0) : 0;
	// 주문그리드 위치를 가져온다.
	CRect& rcGrid = _LayoutMgr->GetRect(IDC_ORDER_GRID);
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

int SmOrderPanel::GetParentHeight()
{
	if (!_ParentDlg)
		return -1;
	return _ParentDlg->WindowHeight() == 0 ? _ParentDlg->DefaultHeight() : _ParentDlg->WindowHeight();
}

int SmOrderPanel::GetParentTitleBarHeight()
{
	if (!_ParentDlg)
		return -1;
	return _ParentDlg->GetTitleBarHeight();
}

int SmOrderPanel::GetGridWidth()
{
	return m_Grid.GetGridWidth(_OrderGridColOption);
}

int SmOrderPanel::GetRemainWidth(int parentWidth)
{
	return _ProductRemainGrid.GetGridWidth(parentWidth);
}

int SmOrderPanel::GetRemainWidth()
{
	return _ProductRemainGrid.GetGridWidth(_OrderGridColOption);
}

void SmOrderPanel::RepositionConfigWnd()
{
	if (!_LayoutMgr || !_ConfigDlg)
		return;
	CRect& rcConfig = _LayoutMgr->GetRect(IDC_STATIC_MSG);
	_ConfigDlg->SetWindowPos(nullptr, rcConfig.left, rcConfig.top, rcConfig.Width(), rcConfig.Height(), SWP_NOZORDER);
	_ConfigDlg->RecalateLayout(rcConfig);
}

void SmOrderPanel::RepositionTickGrid()
{
	if (_ShowTickWnd) {
		CRect& rcTick = _LayoutMgr->GetRect(IDC_STATIC_REAL_TICK);
		_TickGrid.SetWindowPos(nullptr, rcTick.left, rcTick.top, rcTick.Width(), rcTick.Height(), SWP_NOSIZE);
	}
}

void SmOrderPanel::ShowHideCtrl()
{
	if (!_OrderConfigMgr)
		return;

	if (_OrderConfigMgr->Type() == 0) {
		//_BtnAvail.ShowWindow(SW_SHOW);
		_BtnRemain.ShowWindow(SW_SHOW);
		_BtnRemainFund.ShowWindow(SW_HIDE);
	}
	else {
		_BtnAvail.ShowWindow(SW_HIDE);
		_BtnRemain.ShowWindow(SW_SHOW);
		_BtnRemainFund.ShowWindow(SW_SHOW);
	}
}

void SmOrderPanel::RepositionProductGrid()
{
	_ProductRemainGrid.SetColTitleForLevel(_OrderGridColOption);
}

void SmOrderPanel::ResetRemainFund()
{
	_OrderByRemain = false;
	_BtnRemainFund.SetColor(BTNST_COLOR_FG_IN, RGB(0, 0, 0), true);
	_BtnRemainFund.SetColor(BTNST_COLOR_FG_OUT, RGB(0, 0, 0), true);
	_BtnRemainFund.SetColor(BTNST_COLOR_FG_FOCUS, RGB(0, 0, 0), true);
	_BtnRemainFund.SetColor(BTNST_COLOR_BK_IN, RGB(220, 220, 220), true);
	_BtnRemainFund.SetColor(BTNST_COLOR_BK_OUT, RGB(220, 220, 220), true);
	_BtnRemainFund.SetColor(BTNST_COLOR_BK_FOCUS, RGB(220, 220, 220), true);
}

CRect SmOrderPanel::GetClientArea(int resourceID)
{
	CWnd* wnd = (CWnd*)GetDlgItem(resourceID);
	CRect rcWnd;
	wnd->GetWindowRect(rcWnd);
	ScreenToClient(rcWnd);

	return rcWnd;
}

BOOL SmOrderPanel::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	::EnumChildWindows(m_hWnd, ::SetChildFont, (LPARAM)g_Font.GetFont());


	_ConfigDlg = new VtOrderConfigDlg();
	_ConfigDlg->CenterWnd(this);
	_ConfigDlg->Create(IDD_CENTER_CONFIG, this);

	_ConfigGrid.CenterWnd(this);
	_ProductRemainGrid.AttachGrid(this, IDC_STATIC_PRODUCT_REMAIN);
	_TickGrid.AttachGrid(this, IDC_STATIC_REAL_TICK);
	_ConfigGrid.AttachGrid(this, IDC_STATIC_SET);
	// 레이아웃에 따라 위치가 바뀌는 컨트롤들의 원래 위치를 저장한다.
	SaveControlPos();
	// 레이아웃에 따라 위치가 바뀌는 컨트롤들의 위치를 계산한다.
	CalcLayout();
	// 위에서 계산된 위치에 컨트롤들을 옮겨 준다.
	RepositionControl();
	// 설정창을 옮겨 준다.
	RepositionConfigWnd();
	// 틱창을 옮겨 준다.
	RepositionTickGrid();
	// 주문 그리드 셀들을 설정한다.
	m_Grid.Init();
	// 그리드 컬럼 옵션을 설정해 준다.
	m_Grid.ShowHideOrderGrid(_OrderGridColOption);

	_StaticProductName.SetTextColor(RGB(0, 0, 0));
	_StaticProductName.SetColor(GetSysColor(COLOR_BTNFACE));
	_StaticProductName.SetGradientColor(GetSysColor(COLOR_BTNFACE));
	_StaticProductName.SetTextAlign(1);
	_StaticProduct.SetTextColor(RGB(0, 0, 0));
	_StaticProduct.SetColor(GetSysColor(COLOR_BTNFACE));
	_StaticProduct.SetGradientColor(GetSysColor(COLOR_BTNFACE));
	_StaticProduct.SetTextAlign(1);

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
	strValue.Format(_T("%d"), m_Grid.CutMgr()->CutLoss());

	strValue.Format(_T("%d"), m_Grid.CutMgr()->CutProfit());

	strValue.Format(_T("%d"), _OrderAmount);
	_EditOrderAmount.SetWindowText(strValue);

	strValue.Format(_T("%d"), _StopVal);
	_EditStopVal.SetWindowText(strValue);

	_BtnRemainFund.DrawBorder(TRUE, TRUE);
	_BtnRemainFund.SetAlign(ST_ALIGN_VERT | ST_ALIGN_HORIZ);
	_BtnRemainFund.SetColor(BTNST_COLOR_BK_IN, RGB(220, 220, 220), true);
	_BtnRemainFund.SetColor(BTNST_COLOR_FG_IN, RGB(0, 0, 0), true);
	_BtnRemainFund.SetColor(BTNST_COLOR_BK_OUT, RGB(220, 220, 220), true);

	m_Grid.OrderConfigMgr(_OrderConfigMgr);
	_TickGrid.SetOrderConfigMgr(_OrderConfigMgr);
	_TickGrid.CenterWnd(this);
	ShowHideCtrl();
	RepositionProductGrid();

	SetTimer(1, 10, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void SmOrderPanel::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	m_Grid.UnregisterQuoteCallback();
	KillTimer(1);
	CDialogEx::OnClose();
}


void SmOrderPanel::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	//for(int i = 0; i < 100; ++i)
	//	m_Grid.RefreshAllValues();
	CDialogEx::OnTimer(nIDEvent);
}

void SmOrderPanel::InitAll()
{

}

void SmOrderPanel::Activated(bool flag)
{

}

void SmOrderPanel::ResetByCenterRow()
{
	m_Grid.ResetByCenterRow();
}

void SmOrderPanel::ChangeFocus()
{

}

int SmOrderPanel::GetWindowWidth()
{
	if (!_LayoutMgr)
		return 0;
	int orderGridWidth = GetGridWidth();
	CRect& rcTick = _LayoutMgr->GetRect(IDC_STATIC_REAL_TICK);
	return _ShowTickWnd ? orderGridWidth + rcTick.Width() : orderGridWidth;
}

void SmOrderPanel::RefreshLayout(bool flag)
{
	CalcLayout();
	RepositionControl();
	m_Grid.ResizeGrid();
	RepositionProductGrid();
	RepositionConfigWnd();
	RepositionTickGrid();
}

int SmOrderPanel::GetMaxWidth()
{
	return 800;
}

void SmOrderPanel::AdjustControlForMode()
{

}

void SmOrderPanel::RepositionControl(int height)
{
	CRect rcConfig, rcWnd, rcGrid;
	GetWindowRect(rcWnd);
	_ConfigDlg->GetWindowRect(rcConfig);
	m_Grid.GetWindowRect(rcGrid);
	ScreenToClient(rcGrid);
	ScreenToClient(rcConfig);
	m_Grid.SetWindowPos(nullptr, rcGrid.left, rcGrid.top, rcGrid.Width(), (height - rcGrid.top - rcConfig.Height()), SWP_NOMOVE);
	_ConfigDlg->MoveWindow(0, height - rcConfig.Height(), rcConfig.Width(), rcConfig.Height());
}

void SmOrderPanel::RepositionControl()
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
					SWP_NOZORDER );
				pWnd->RedrawWindow();

			}
		}
		::EndDeferWindowPos(hdwp);
	}
}

void SmOrderPanel::ResizeOrderGrid(int maxHeight)
{

}

void SmOrderPanel::ResizeOrderGrid()
{

}

int SmOrderPanel::GetCountOrderGridEnabledCol()
{
	return std::count_if(_OrderGridColOption.begin(), _OrderGridColOption.end(), [](auto i) { return i; });
}

bool SmOrderPanel::ShowTickWnd()
{
	return true;
}

void SmOrderPanel::ChangeAccount(VtAccount* acnt)
{

}

void SmOrderPanel::ChangeFund(VtFund* fund)
{

}

void SmOrderPanel::Save(simple::file_ostream<same_endian_type>& ss)
{

}

void SmOrderPanel::Load(simple::file_istream<same_endian_type>& ss)
{

}

void SmOrderPanel::ChangeSymbol(VtSymbol* symbol)
{

}


void SmOrderPanel::UnregisterOrderWnd()
{
	m_Grid.UnregisterHogaCallback();
	m_Grid.UnregisterOrderCallback();
	m_Grid.UnregisterQuoteCallback();
}

BOOL SmOrderPanel::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_SPACE ) {
			m_Grid.OrderBySpaceBar();
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}



bool SmOrderPanel::EnableCutProfit() const
{
	return m_Grid.CutMgr()->EnableCutProfit();
}

void SmOrderPanel::EnableCutProfit(bool val)
{
	m_Grid.CutMgr()->EnableCutProfit(val);
}

bool SmOrderPanel::EnableCutLoss() const
{
	return m_Grid.CutMgr()->EnableCutLoss();
}

void SmOrderPanel::EnableCutLoss(bool val)
{
	m_Grid.CutMgr()->EnableCutLoss(val);
}

int SmOrderPanel::CutProfit() const
{
	return m_Grid.CutMgr()->CutProfit();
}

void SmOrderPanel::CutProfit(int val)
{
	m_Grid.CutMgr()->CutProfit(val);
}

int SmOrderPanel::CutLoss() const
{
	return m_Grid.CutMgr()->CutLoss();
}

void SmOrderPanel::CutLoss(int val)
{
	m_Grid.CutMgr()->CutLoss(val);
}

int SmOrderPanel::OrderType() const
{
	return m_Grid.CutMgr()->OrderType();
}

void SmOrderPanel::OrderType(int val)
{
	m_Grid.CutMgr()->OrderType(val);
}

int SmOrderPanel::OrderTypeSlip() const
{
	return m_Grid.CutMgr()->OrderTypeSlip();
}

void SmOrderPanel::OrderTypeSlip(int val)
{
	m_Grid.CutMgr()->OrderTypeSlip(val);
}

void SmOrderPanel::ApplyProfitLossForPosition()
{
	m_Grid.ApplyProfitLossForPosition();
}

void SmOrderPanel::FixedCenter(bool val)
{
	ResetByCenterRow();
	_FixedCenter = val;
}



BOOL SmOrderPanel::OnEraseBkgnd(CDC* pDC)
{

	BOOL bRet = CDialog::OnEraseBkgnd(pDC);

	return bRet;
}
