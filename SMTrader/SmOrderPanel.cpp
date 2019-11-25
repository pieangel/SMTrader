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
#include "SmCallbackManager.h"

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
	m_Grid.CutMgr()->OrderConfigMgr(_OrderConfigMgr);
	m_Grid.OrderConfigMgr(_OrderConfigMgr);
	_ProductRemainGrid.OrderConfigMgr(_OrderConfigMgr);
}

void SmOrderPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ORDER_GRID, m_Grid);
	DDX_Control(pDX, IDC_STATIC_REAL_TICK, _TickGrid);
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
	_FixedCenter = !_FixedCenter;
	m_Grid.Init();
}

void SmOrderPanel::OnBnClickedBtnOrderAmt1()
{
	// TODO: Add your control notification handler code here
	CString strText;
	_BtnAmt1.GetWindowText(strText);
	_EditOrderAmount.SetWindowText(strText);
	_OrderAmount = _ttoi(strText);
}

void SmOrderPanel::OnBnClickedBtnOrderAmt2()
{
	// TODO: Add your control notification handler code here
	CString strText;
	_BtnAmt2.GetWindowText(strText);
	_EditOrderAmount.SetWindowText(strText);
	_OrderAmount = _ttoi(strText);
}

void SmOrderPanel::OnBnClickedBtnOrderAmt3()
{
	// TODO: Add your control notification handler code here
	CString strText;
	_BtnAmt3.GetWindowText(strText);
	_EditOrderAmount.SetWindowText(strText);
	_OrderAmount = _ttoi(strText);
}

void SmOrderPanel::OnBnClickedBtnOrderAmt4()
{
	// TODO: Add your control notification handler code here
	CString strText;
	_BtnAmt4.GetWindowText(strText);
	_EditOrderAmount.SetWindowText(strText);
	_OrderAmount = _ttoi(strText);
}

void SmOrderPanel::OnBnClickedBtnOrderAmt5()
{
	// TODO: Add your control notification handler code here
	CString strText;
	_BtnAmt5.GetWindowText(strText);
	_EditOrderAmount.SetWindowText(strText);
	_OrderAmount = _ttoi(strText);
}

void SmOrderPanel::OnBnClickedBtnOrderAmt6()
{
	// TODO: Add your control notification handler code here
	CString strText;
	_BtnAmt6.GetWindowText(strText);
	_EditOrderAmount.SetWindowText(strText);
	_OrderAmount = _ttoi(strText);
}

void SmOrderPanel::OnDeltaposSpinOrderAmount(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void SmOrderPanel::OnEnChangeEditOrderAmount()
{
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

void SmOrderPanel::OnEnChangeEditStopval()
{
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

void SmOrderPanel::OnBnClickedBtnLiq()
{
	m_Grid.LiqudAll(VtPriceType::Market, 0);
	ResetRemainFund();
}

void SmOrderPanel::OnBnClickedBtnRemain()
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

void SmOrderPanel::OnBnClickedBtnSelSymbol()
{
	HdSymbolSelecter symSelecter;
	symSelecter.OrderConfigMgr(_OrderConfigMgr);
	symSelecter.DoModal();
}

void SmOrderPanel::OnBnClickedButtonProfitLoss()
{

}

void SmOrderPanel::OnStnClickedStaticOrderPanel()
{

}

void SmOrderPanel::OnRClicked(NMHDR * pNotifyStruct, LRESULT * result)
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

void SmOrderPanel::OnStnClickedStaticRealTick()
{

}

void SmOrderPanel::OnBnClickedBtnRemainFund()
{
	if (!_OrderConfigMgr || !_Symbol)
		return;
	std::pair<bool, int> curRemain;
	if (_OrderConfigMgr->Type() == 0) { // 게좌 주문 일 때
		if (!_OrderConfigMgr->Account())
			return;
		curRemain = _OrderConfigMgr->Account()->GetRemainCount(_Symbol->ShortCode);
	}
	else { // 펀드 주문일 때
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
	}
	else {
		ResetRemainFund();
	}
}

void SmOrderPanel::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (_ParentDlg) {
		_ParentDlg->SetActiveCenterWnd(this);
	}
	CDialogEx::OnLButtonDown(nFlags, point);
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
	m_Grid.RefreshOrderPosition();
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
	rcConfig.right = rcConfig.left + clientWidth + 4;

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
	//_ConfigDlg->SetWindowPos(nullptr, rcConfig.left, rcConfig.top, rcConfig.Width(), rcConfig.Height(), SWP_NOZORDER);
	_ConfigDlg->MoveWindow(rcConfig, TRUE);
	_ConfigDlg->RecalateLayout(rcConfig);
}

void SmOrderPanel::RepositionTickGrid()
{
	if (_ShowTickWnd) {
		CRect& rcTick = _LayoutMgr->GetRect(IDC_STATIC_REAL_TICK);
		//_TickGrid.SetWindowPos(nullptr, rcTick.left, rcTick.top, rcTick.Width(), rcTick.Height(), SWP_NOSIZE);
		_TickGrid.MoveWindow(rcTick, TRUE);
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

void SmOrderPanel::OnEntered()
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

void SmOrderPanel::OnSymbolMaster(VtSymbol* sym)
{
	if (!sym || !_Symbol)
		return;
	if (_Symbol->ShortCode.compare(sym->ShortCode) != 0)
		return;

	SetProductName(sym);
	if (m_Grid.GetSafeHwnd()) {
		m_Grid.OnSymbolMaster(sym);
	}
}

void SmOrderPanel::InitSymbol()
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

void SmOrderPanel::SetSymbol(VtSymbol* sym)
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

void SmOrderPanel::SetSymbol()
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

void SmOrderPanel::AddSymbolToCombo(VtSymbol* symbol)
{
	if (!symbol || !_ComboSymbol.GetSafeHwnd())
		return;
	int index = _ComboSymbol.FindString(0, symbol->ShortCode.c_str());
	if (index == -1)
	{
		index = _ComboSymbol.AddString(symbol->ShortCode.c_str());
		_ComboSymbol.SetItemDataPtr(index, symbol);
	}
	_ComboSymbol.SetCurSel(index);
}

void SmOrderPanel::SetProductName(VtSymbol* symbol)
{
	if (!symbol)
		return;

	if (_StaticProductName.GetSafeHwnd()) {
		_StaticProductName.SetWindowText(symbol->Name.c_str());
		_StaticProductName.Invalidate();
	}
}

void SmOrderPanel::InitPosition()
{
	_ProductRemainGrid.InitPosition();
}

void SmOrderPanel::RegisterRealtimeSymbol()
{
	if (!_Symbol)
		return;
	VtRealtimeRegisterManager* realtimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	realtimeRegiMgr->RegisterProduct(_Symbol->ShortCode);
}

void SmOrderPanel::UnregisterRealtimeSymbol()
{
	if (!_Symbol)
		return;
	VtRealtimeRegisterManager* realtimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	realtimeRegiMgr->UnregisterProduct(_Symbol->ShortCode);
}

void SmOrderPanel::RegisterRealtimeAccount()
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

void SmOrderPanel::UnregisterRealtimeAccount()
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

BOOL SmOrderPanel::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	::EnumChildWindows(m_hWnd, ::SetChildFont, (LPARAM)g_Font.GetFont());


	_ConfigDlg = new VtOrderConfigDlg();
	_ConfigDlg->CenterWnd(this);
	_ConfigDlg->Create(IDD_CENTER_CONFIG, this);

	_ConfigGrid.CenterWnd(this);
	_ProductRemainGrid.AttachGrid(this, IDC_STATIC_PRODUCT_REMAIN);
	//_TickGrid.AttachGrid(this, IDC_STATIC_REAL_TICK);
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

	SmCallbackManager::GetInstance()->SubscribeMasterCallback((long)this, std::bind(&SmOrderPanel::OnSymbolMaster, this, _1));
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void SmOrderPanel::OnClose()
{
	// TODO: Add your message handler code here and/or call default
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

void SmOrderPanel::InitGridInfo()
{
	if (m_Grid.GetSafeHwnd())
		m_Grid.Init();
}

void SmOrderPanel::ClearPosition()
{
	_ProductRemainGrid.ClearPosition();
}

void SmOrderPanel::SetTickCount(int count)
{
	_TickGrid.MaxRow(count);
}

int SmOrderPanel::GetTickCount()
{
	return _TickGrid.MaxRow();
}

void SmOrderPanel::SetOrderArea(int height, int width)
{
	m_Grid.SetOrderArea(height, width);
	// 컨트롤들의 위치를 계산한다.
	CalcLayout();
	// 컨트롤들을 계산된 위치로 옮겨 준다.
	RepositionControl();
	// 그리드를 다시 설정한다.
	m_Grid.ResizeGrid(height, width);
	// 잔고 그리드를 위치 재설정
	RepositionProductGrid();
	// 설정 윈도 위치 재설성
	RepositionConfigWnd();
}

void SmOrderPanel::ShowOrderCountInGrid(bool flag)
{
	_ShowOrderCountArea = flag;
	_OrderGridColOption[2] = flag;
	m_Grid.ShowHideOrderGrid(_OrderGridColOption);
	if (!_ShowStopArea && !_ShowOrderCountArea) {
		_ProductRemainGrid.SetShowAvage(false);
		_ProductRemainGrid.SetShowRemainType(false);
	}
	else {
		_ProductRemainGrid.SetShowAvage(true);
		_ProductRemainGrid.SetShowRemainType(true);
	}
}

void SmOrderPanel::SetShowPLConfigWnd(bool flag)
{
	_ShowRemainConfig = flag;
}

void SmOrderPanel::InitAll()
{
	if (!_OrderConfigMgr)
		return;
	InitSymbol();
	InitPosition();
	// 주문 그리드 셀들을 설정한다.
	m_Grid.Init();
	_TickGrid.Init();
	// 그리드 컬럼 옵션을 설정해 준다.
	m_Grid.ShowHideOrderGrid(_OrderGridColOption);
	_Init = true;
}

void SmOrderPanel::Activated(bool flag)
{
	_Activated = flag;
	m_Grid.Selected(flag);
	m_Grid.Invalidate();

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

	if (_StaticProductName.GetSafeHwnd()) {
		_StaticProductName.Invalidate();
		_StaticProduct.Invalidate();
	}
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
	// 컨트롤들의 레이아웃을 계산한다.
	CalcLayout();
	// 컨트롤들을 위치를 설정한다.
	RepositionControl();
	// 주문 그리들의 크기를 조절한다.
	m_Grid.ResizeGrid();
	// 제품정보 그리드를 위치를 설정한다.
	RepositionProductGrid();
	// 설정 윈도우의 위치를 재설정한다.
	RepositionConfigWnd();
	// 틱윈도우의 위치를 재설정한다.
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
	// 컨트롤들의 위치를 계산한다.
	CalcLayout();
	// 컨트롤들을 계산된 위치로 옮겨 준다.
	RepositionControl();
	// 그리드를 다시 설정한다.
	m_Grid.ResizeGrid();
	// 잔고 그리드를 위치 재설정
	RepositionProductGrid();
	// 설정 윈도 위치 재설성
	RepositionConfigWnd();
}

int SmOrderPanel::GetCountOrderGridEnabledCol()
{
	return std::count_if(_OrderGridColOption.begin(), _OrderGridColOption.end(), [](auto i) { return i; });
}

bool SmOrderPanel::ShowTickWnd()
{
	return _ShowTickWnd;
}

void SmOrderPanel::BlockEvent()
{
	SmCallbackManager::GetInstance()->UnsubscribeMasterCallback((long)this);
	m_Grid.UnregisterAllCallback();
	_ProductRemainGrid.UnregisterAllCallback();
	_TickGrid.UnregisterAllCallback();
}

void SmOrderPanel::ChangeAccount(VtAccount* acnt)
{
	if (!acnt)
		return;
	ClearPosition();
	SetSymbol();
	InitGridInfo();
	InitPosition();
}

void SmOrderPanel::ChangeFund(VtFund* fund)
{
	if (!fund)
		return;
	ClearPosition();
	SetSymbol();
	InitGridInfo();
	InitPosition();
}

void SmOrderPanel::Save(simple::file_ostream<same_endian_type>& ss)
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
	ss << m_Grid.CellHeight();
	// 셀 너비
	ss << m_Grid.OrderWidth();
	// 손절 익절창 표시여부
	ss << _ShowRemainConfig;
}

void SmOrderPanel::Load(simple::file_istream<same_endian_type>& ss)
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
	m_Grid.CellHeight(cellHeight);
	// 셀 너비 복원
	m_Grid.OrderWidth(cellWidth);

	// 저장된 심볼은 목록으로 만들어 심볼 마스터 요청한다.
	VtSaveManager* saveMgr = VtSaveManager::GetInstance();
	std::vector<VtSymbol*>& symvec = saveMgr->GetSymbolVector();
	if (_DefaultSymbol)
		symvec.push_back(_DefaultSymbol);
}

void SmOrderPanel::ChangeSymbol(VtSymbol* symbol)
{
	if (!symbol)
		return;
	_TickGrid.ClearValues();
	ClearPosition();
	SetSymbol(symbol);
	AddSymbolToCombo(symbol);
	SetProductName(symbol);
	InitGridInfo();
	InitPosition();
}

BOOL SmOrderPanel::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) {
			OnEntered();
			return TRUE;
		}
		else if (pMsg->wParam == VK_SPACE ) {
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
	m_Grid.SetCenterValue();
}



BOOL SmOrderPanel::OnEraseBkgnd(CDC* pDC)
{

	BOOL bRet = CDialog::OnEraseBkgnd(pDC);

	return bRet;
}

void SmOrderPanel::ShowOrderAreaInGrid(bool flag)
{
	_ShowOrderArea = flag;
	_OrderGridColOption[0] = flag;
	m_Grid.ShowHideOrderGrid(_OrderGridColOption);
}

void SmOrderPanel::ShowStopAreaInGrid(bool flag)
{
	_ShowStopArea = flag;
	_OrderGridColOption[1] = flag;
	m_Grid.ShowHideOrderGrid(_OrderGridColOption);
}

void SmOrderPanel::SetTickWndPos(int pos)
{
	_TickWndPos = pos;
}

void SmOrderPanel::SaveToXml(pugi::xml_node& node_center_window)
{
	if (!_Symbol)
		return;
	pugi::xml_node center_window_child = node_center_window.append_child("symbol_code");
	center_window_child.append_child(pugi::node_pcdata).set_value(_Symbol->ShortCode.c_str());
	bool val = _OrderGridColOption[0];
	center_window_child = node_center_window.append_child("show_order_area");
	center_window_child.append_child(pugi::node_pcdata).set_value(std::to_string(val).c_str());
	val = _OrderGridColOption[1];
	center_window_child = node_center_window.append_child("show_stop_area");
	center_window_child.append_child(pugi::node_pcdata).set_value(std::to_string(val).c_str());
	val = _OrderGridColOption[2];
	center_window_child = node_center_window.append_child("show_count_area");
	center_window_child.append_child(pugi::node_pcdata).set_value(std::to_string(val).c_str());
	center_window_child = node_center_window.append_child("show_tick_window");
	center_window_child.append_child(pugi::node_pcdata).set_value(std::to_string(_ShowTickWnd).c_str());
	center_window_child = node_center_window.append_child("tick_window_pos");
	center_window_child.append_child(pugi::node_pcdata).set_value(std::to_string(_TickWndPos).c_str());
	center_window_child = node_center_window.append_child("order_amount");
	center_window_child.append_child(pugi::node_pcdata).set_value(std::to_string(_OrderAmount).c_str());
	center_window_child = node_center_window.append_child("order_area_width");
	center_window_child.append_child(pugi::node_pcdata).set_value(std::to_string(m_Grid.OrderWidth()).c_str());
	center_window_child = node_center_window.append_child("order_row_height");
	center_window_child.append_child(pugi::node_pcdata).set_value(std::to_string(m_Grid.CellHeight()).c_str());
	center_window_child = node_center_window.append_child("show_profitloss_config");
	center_window_child.append_child(pugi::node_pcdata).set_value(std::to_string(_ShowRemainConfig).c_str());
}

void SmOrderPanel::LoadFromXml(pugi::xml_node& node_center_window)
{
	std::string symbol_code = node_center_window.child_value("symbol_code");
	std::stoi(node_center_window.child_value("show_order_area")) == 0 ? _OrderGridColOption[0] = false : _OrderGridColOption[0] = true;
	std::stoi(node_center_window.child_value("show_stop_area")) == 0 ? _OrderGridColOption[1] = false : _OrderGridColOption[0] = true;
	std::stoi(node_center_window.child_value("show_count_area")) == 0 ? _OrderGridColOption[2] = false : _OrderGridColOption[0] = true;
	std::stoi(node_center_window.child_value("show_tick_window")) == 0 ? _ShowTickWnd = false : _ShowTickWnd = true;
	_TickWndPos = std::stoi(node_center_window.child_value("tick_window_pos"));
	_OrderAmount = std::stoi(node_center_window.child_value("order_amount"));
	int order_area_width = std::stoi(node_center_window.child_value("order_area_width"));
	int order_area_height = std::stoi(node_center_window.child_value("order_row_height"));
	m_Grid.OrderWidth(order_area_width);
	m_Grid.CellHeight(order_area_height);
	std::stoi(node_center_window.child_value("show_profitloss_config")) == 0 ? _ShowRemainConfig = false : _ShowRemainConfig = true;
	// 주문창 그리드 속성 대입
	_ShowOrderArea = _OrderGridColOption[0];
	_ShowStopArea = _OrderGridColOption[1];
	_ShowOrderCountArea = _OrderGridColOption[2];
}
