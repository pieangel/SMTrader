// VtOrderWndHd.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtOrderWndHd.h"
#include "afxdialogex.h"
#include "VtOrderConfigManager.h"
#include "VtOrderCenterWndHd.h"
#include "VtOrderDialogManager.h"
#include "VtProductCategoryManager.h"
#include "VtProductSection.h"
#include "VtProductSubSection.h"
#include "VtSymbol.h"
#include "VtOrderLeftWndHd.h"
#include "MainFrm.h"
#include "VtRealTickWnd.h"
#include "VtGlobal.h"
#include "VtAccountManager.h"
#include "VtAccount.h"
#include "VtRealtimeRegisterManager.h"
#include "VtFundManager.h"
#include "VtFund.h"
#include "VtFundDesigner.h"
#include "VtOrderManagerSelector.h"
#include "VtFundOrderManagerSelecter.h"
#include "Poco/Delegate.h"
#include <set>
#include <tuple>
#include <vector>
#include "VtLayoutManager.h"
#include <libloaderapi.h>
#include <map>
using Poco::Delegate;


extern TApplicationFont g_Font;
// VtOrderWndHd dialog

IMPLEMENT_DYNAMIC(VtOrderWndHd, ETSLayoutDialog)

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor,
	HDC      hdcMonitor,
	LPRECT   lprcMonitor,
	LPARAM   dwData)
{
	MONITORINFO info;
	info.cbSize = sizeof(info);
	if (GetMonitorInfo(hMonitor, &info)) {
		VtOrderWndHd::_MaxWidth += std::abs(info.rcMonitor.right - info.rcMonitor.left);
	}
	return TRUE;  // continue enumerating
}

int VtOrderWndHd::_MaxWidth = 0;

VtOrderWndHd::VtOrderWndHd(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_ORDER_WND_HD, pParent)
{
	_OrderConfigMgr = new VtOrderConfigManager();
	_OrderConfigMgr->SetDefaultAccount();
	_OrderConfigMgr->SetDefaultSymbol();

	_LeftWnd.OrderConfigMgr(_OrderConfigMgr);
	_RightWnd.OrderConfigMgr(_OrderConfigMgr);

	_Fund = nullptr;
	_Account = nullptr;
	_MaxWidth = 0;
	_WindowID = 0;
	_ShowLeftWnd = true;
	_ShowRightWnd = true;
	_Sizing = false;
	ClickedRightExtend = false;
	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);
	_LayoutMgr = new VtLayoutManager(this);
}

VtOrderWndHd::~VtOrderWndHd()
{
	if (_OrderConfigMgr) {
		delete _OrderConfigMgr;
		_OrderConfigMgr = nullptr;
	}

	if (_LayoutMgr) {
		delete _LayoutMgr;
		_LayoutMgr = nullptr;
	}
	End();
}

void VtOrderWndHd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_ADDWND, _BtnAddWnd);
	DDX_Control(pDX, IDC_BTN_DELWND, _BtnDelWnd);
	DDX_Control(pDX, IDC_BTN_GET_ACNT_INFO, _BtnGetAcntInfo);
	DDX_Control(pDX, IDC_BTN_SHOW_LEFT, _BtnShowLeft);
	DDX_Control(pDX, IDC_BTN_SHOWRIGHT, _BtnShowRight);
	DDX_Control(pDX, IDC_COMBO_ACCOUNT_HD, _ComboAcnt);
	DDX_Control(pDX, IDC_STATIC_ACNT, _StaticAcnt);
	DDX_Control(pDX, IDC_STATIC_ACNT_NAME, _StaticAcntName);
	DDX_Control(pDX, IDC_BTN_EDIT_FUND, _BtnFundEditor);
	DDX_Control(pDX, IDC_EDIT_PWD, _EditPwd);
}


void VtOrderWndHd::RegisterRealtimeAccount(VtAccount* acnt)
{
	if (!acnt)
		return;
	VtRealtimeRegisterManager* realtimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	realtimeRegiMgr->RegisterAccount(acnt->AccountNo);
}

void VtOrderWndHd::UnregisterRealtimeAccount(VtAccount* acnt)
{
	if (!acnt)
		return;
	VtRealtimeRegisterManager* realtimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	realtimeRegiMgr->UnregisterAccount(acnt->AccountNo);
}

void VtOrderWndHd::Begin()
{
	
}

void VtOrderWndHd::End()
{
	for (auto it = _CenterWndVector.begin(); it != _CenterWndVector.end(); ++it) {
		VtOrderCenterWndHd* centerWnd = *it;
		centerWnd->DestroyWindow();
		delete centerWnd;
	}

	for (auto it = _RealTickWndVector.begin(); it != _RealTickWndVector.end(); ++it) {
		VtRealTickWnd* realWnd = it->second;
		realWnd->DestroyWindow();
		delete realWnd;
	}

	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	_OrderWindowEvent -= delegate(orderDlgMgr, &VtOrderDialogManager::OnOrderWndEventReceived);
}

BEGIN_MESSAGE_MAP(VtOrderWndHd, CDialog)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SIZING()
	ON_WM_GETMINMAXINFO()
	ON_CBN_SELCHANGE(IDC_COMBO_ACCOUNT_HD, &VtOrderWndHd::OnCbnSelchangeComboAccountHd)
	ON_BN_CLICKED(IDC_BTN_SHOW_LEFT, &VtOrderWndHd::OnBnClickedBtnShowLeft)
	ON_BN_CLICKED(IDC_BTN_GET_ACNT_INFO, &VtOrderWndHd::OnBnClickedBtnGetAcntInfo)
	ON_BN_CLICKED(IDC_BTN_DELWND, &VtOrderWndHd::OnBnClickedBtnDelwnd)
	ON_BN_CLICKED(IDC_BTN_ADDWND, &VtOrderWndHd::OnBnClickedBtnAddwnd)
	ON_BN_CLICKED(IDC_BTN_SHOWRIGHT, &VtOrderWndHd::OnBnClickedBtnShowright)
	ON_WM_SYSCOMMAND()
	ON_STN_CLICKED(IDC_STATIC_ACNT_NAME, &VtOrderWndHd::OnStnClickedStaticAcntName)
	ON_STN_CLICKED(IDC_STATIC_ACNT, &VtOrderWndHd::OnStnClickedStaticAcnt)
	ON_BN_CLICKED(IDC_BTN_EDIT_FUND, &VtOrderWndHd::OnBnClickedBtnEditFund)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEHOVER()
	ON_EN_CHANGE(IDC_EDIT_PWD, &VtOrderWndHd::OnEnChangeEditPwd)
END_MESSAGE_MAP()


// VtOrderWndHd message handlers

/*
CShadeButtonST _BtnAddWnd;
CShadeButtonST _BtnDelWnd;
CShadeButtonST _BtnGetAcntInfo;
CShadeButtonST _BtnShowLeft;
CShadeButtonST _BtnShowRight;
CTTComboBox _ComboAcnt;
CStatic _StaticAcnt;
CStatic _StaticAcntName;
// Edit the funds.
CShadeButtonST _BtnFundEditor;*/

BOOL VtOrderWndHd::OnInitDialog()
{
	CDialog::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	this->SetIcon(hIcon, FALSE);

	::EnumChildWindows(m_hWnd, ::SetChildFont, (LPARAM)g_Font.GetFont());


	_BtnAddWnd.SetIcon(IDI_PLUS, 16, 16, IDI_PLUS, 16, 16);
	_BtnAddWnd.OffsetColor(BTNST_COLOR_BK_IN, 30);
	_BtnAddWnd.SetAlign(ST_ALIGN_VERT);

	_BtnDelWnd.SetIcon(IDI_MINUS, 16, 16, IDI_MINUS, 16, 16);
	_BtnDelWnd.OffsetColor(BTNST_COLOR_BK_IN, 30);
	_BtnDelWnd.SetAlign(ST_ALIGN_VERT);

	_BtnShowLeft.SetIcon(IDI_RIGHT, 16, 16, IDI_RIGHT, 16, 16);
	_BtnShowLeft.OffsetColor(BTNST_COLOR_BK_IN, 30);
	_BtnShowLeft.SetAlign(ST_ALIGN_VERT);

	_BtnShowRight.SetIcon(IDI_LEFT, 16, 16, IDI_LEFT, 16, 16);
	_BtnShowRight.OffsetColor(BTNST_COLOR_BK_IN, 30);
	_BtnShowRight.SetAlign(ST_ALIGN_VERT);

	std::vector<CShadeButtonST*> _btnVec;
	_btnVec.push_back(&_BtnAddWnd);
	_btnVec.push_back(&_BtnDelWnd);
	_btnVec.push_back(&_BtnGetAcntInfo);
	_btnVec.push_back(&_BtnShowLeft);
	_btnVec.push_back(&_BtnShowRight);
	_btnVec.push_back(&_BtnFundEditor);
	
	for (auto it = _btnVec.begin(); it != _btnVec.end(); ++it) {
		CShadeButtonST* btn = *it;
		btn->SetShade(SHS_HSHADE, 8, 20, 5, RGB(55, 55, 255));
		btn->DrawFlatFocus(TRUE);
	}

	if (!_OrderConfigMgr)
		return FALSE;
	_EnableOnSizeEvent = false;

	// TODO:  Add extra initialization here
	SaveControlPos();
	CreateChildWindows();
	ResizeWindow();
	RecalChildWindows();
	ReposChildWindowsForward();
	RepositionControl();
	ShowHideCtrl();

	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	_OrderWindowEvent += delegate(orderDlgMgr, &VtOrderDialogManager::OnOrderWndEventReceived);

	VtOrderWndEventArgs arg;
	arg.pOrderWnd = this;
	arg.type = VtOrderWindowEventType::Created;
	FireOrderWindowEvent(std::move(arg));

	SetWindows();

	if (_OrderConfigMgr->Type() == 0) {
		SetWindowText(_T("계좌주문창"));
		InitAccount();
	} else {
		InitFund();
		SetWindowText(_T("펀드주문창"));
	}

	for (auto it = _CenterWndVector.begin(); it != _CenterWndVector.end(); ++it) {
		(*it)->InitAll();
	}

	if (_OrderConfigMgr && _OrderConfigMgr->_HdRightWnd) {
		_OrderConfigMgr->_HdRightWnd->InitCtrl();
	}

	_ComboAcnt.SetDroppedWidth(150);

	VtOrderCenterWndHd* centerWnd = _CenterWndVector.front();
	centerWnd->Activated(true);
	_OrderConfigMgr->_HdCenterWnd = centerWnd;

	_EnableOnSizeEvent = true;
	_XPos = 0;
	_YPos = 0;

	_LeftWnd.OnResizeWnd();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void VtOrderWndHd::SetWindows()
{
	_OrderConfigMgr->_HdLeftWnd = &_LeftWnd;
	_OrderConfigMgr->_HdRightWnd = &_RightWnd;
	if (_CenterWndVector.size() > 0)
		_OrderConfigMgr->_HdCenterWnd = _CenterWndVector.front();
	_OrderConfigMgr->_HdOrderWnd = this;
}

void VtOrderWndHd::InitSettings()
{

}

void VtOrderWndHd::RemoveRealTickWnd(VtRealTickWnd* realWnd)
{
	if (!realWnd)
		return;
	auto it = _RealTickWndVector.find(realWnd);
	if (it != _RealTickWndVector.end()) {
		VtRealTickWnd* realWnd = it->second;
		realWnd->DestroyWindow();
		delete realWnd;
		_RealTickWndVector.erase(it);
	}
}

void VtOrderWndHd::ResetByCenterRow()
{
 	for (auto it = _CenterWndVector.begin(); it != _CenterWndVector.end(); ++it) {
 		(*it)->ResetByCenterRow();
 	}

	//SendMessage(WM_NEXTDLGCTL, (WPARAM)_BtnGetAcntInfo.GetSafeHwnd(), TRUE);
	if (_OrderConfigMgr->_HdCenterWnd)
		_OrderConfigMgr->_HdCenterWnd->ChangeFocus();
}

int VtOrderWndHd::ResizeWindow()
{
	CRect rcWnd;
	GetWindowRect(rcWnd);
	int totalWidth = GetTotalCenterWidth();
	// 좌측창 길이를 합한다.
	totalWidth += _ShowLeftWnd ? _LeftWnd.DefaultWidth() : 0;
	// 우측창 길이를 합한다.
	totalWidth += _ShowRightWnd ? _RightWnd.DefaultWidth() : 0;
	// 윈도우 시스템 오차 교정
	totalWidth += (!_ShowLeftWnd || !_ShowRightWnd) ? VtGlobal::GetHorWidthByScaleFactor(16) : 0;
	// 전체 크기를 정한다.
	SetWindowPos(nullptr, _XPos == 0 ? rcWnd.left : _XPos, _YPos == 0 ? rcWnd.top : _YPos, totalWidth == 0 ? _WindowWidth : totalWidth, _WindowHeight == 0 ? _DefaultHeight : _WindowHeight, SWP_NOZORDER | SWP_FRAMECHANGED);

	return totalWidth == 0 ? _WindowWidth : totalWidth;
}

void VtOrderWndHd::RepositionChildWindowsByReverse(int winWidth)
{
	std::vector<std::tuple<CWnd*, bool, CRect>> rcVec;
	CRect rcChild;
	rcChild.left = winWidth - _RightWnd.DefaultWidth();
	rcChild.right = rcChild.left + _RightWnd.DefaultWidth();
	rcChild.top = 0;
	rcChild.bottom = _WindowHeight;
	rcVec.push_back(std::make_tuple(&_RightWnd, _ShowRightWnd, rcChild));

	int right = rcChild.left;
	for (auto it = _CenterWndVector.rbegin(); it != _CenterWndVector.rend(); ++it) {
		VtOrderCenterWndHd* centerWnd = *it;
		rcChild.top = CenterTop;
		rcChild.bottom = _WindowHeight - CenterTop;
		rcChild.left = right - centerWnd->GetWindowWidth();
		rcChild.right = rcChild.left + centerWnd->GetWindowWidth();
		rcVec.push_back(std::make_tuple(centerWnd, true, rcChild));
		right = rcChild.left;
	}

	rcChild.left = right - _LeftWnd.DefaultWidth();
	rcChild.right = rcChild.left + _LeftWnd.DefaultWidth();
	rcChild.top = 0;
	rcChild.bottom = _WindowHeight;
	rcVec.push_back(std::make_tuple(&_LeftWnd, _ShowLeftWnd, rcChild));

	for (auto it = rcVec.begin(); it != rcVec.end(); ++it) {
		auto item = *it;
		CWnd* wnd = std::get<0>(item);
		CRect& rect = std::get<2>(item);
		if (std::get<1>(item)) {
			wnd->ShowWindow(SW_SHOW);
			wnd->SetWindowPos(nullptr, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER | SWP_FRAMECHANGED | SWP_DRAWFRAME);
		} else {
			wnd->ShowWindow(SW_HIDE);
		}
	}
}

void VtOrderWndHd::ResizeWindowByGridOption()
{
	ResizeWindow();
	RecalChildWindows();
	RepositionControl();
}

void VtOrderWndHd::RefreshLayout(bool resize, bool recalGrid, bool forward)
{
	// 크기가 늘어날 때는 역순으로 이동시키고
	// 크기가 줄어들 때는 정순으로 이동을 시킨다.
	// 이것은 윈도우가 겹쳐 컨트롤이 그려지지 않는 것을 방지하기 위함이다.
	if (resize) {
		ResizeWindow();	
	}
	RecalChildWindows();
	if (forward) {
		ReposChildWindowsForward();
	} else {
		ReposChildWindowsBackward();
	}
	RepositionControl();
	ShowHideCtrl();

	if (!resize) {
		for (auto it = _CenterWndVector.begin(); it != _CenterWndVector.end(); ++it) {
			(*it)->RefreshLayout(recalGrid);
			//(*it)->Invalidate(TRUE);
		}
	}
}

int VtOrderWndHd::GetTitleBarHeight()
{
	CRect rcClient;
	CRect rcWnd;
	GetWindowRect(rcWnd);
	_BtnShowLeft.GetWindowRect(rcClient);

	return std::abs(rcClient.top - rcWnd.top);
}

void VtOrderWndHd::CreateChildWindows()
{
	if (_CenterWndVector.size() == 0) { // 주문창이 하나도 없을 때 기본을 하나 생성해서 목록에 넣는다.
		VtOrderCenterWndHd* centerWnd = new VtOrderCenterWndHd();
		centerWnd->ParentDlg(this);
		centerWnd->OrderConfigMgr(_OrderConfigMgr);
		centerWnd->Create(IDD_ORDER_CENTER_HD, this);
		_CenterWndVector.push_back(centerWnd);
		//_CenterWndCount = _CenterWndVector.size();
	}
	else { // 주문창 객체가 외부에서 만들어 질때
		for (auto it = _CenterWndVector.begin(); it != _CenterWndVector.end(); ++it) {
			VtOrderCenterWndHd* centerWnd = *it;
			centerWnd->ParentDlg(this);
			centerWnd->OrderConfigMgr(_OrderConfigMgr);
			centerWnd->Create(IDD_ORDER_CENTER_HD, this);
		}
	}

	// 자식 윈도우들을 만든다.
	_LeftWnd.Create(IDD_ORDER_LEFT_HD, this);
	_RightWnd.Create(IDD_ORDER_RIGHT_HD, this);
}


void VtOrderWndHd::CreateChildWindows(std::vector<CRect>& rcVec)
{
	
}

void VtOrderWndHd::AddWindow()
{
	_EnableOnSizeEvent = false;
	VtOrderCenterWndHd* centerWnd = new VtOrderCenterWndHd();
	centerWnd->ParentDlg(this);
	centerWnd->OrderConfigMgr(_OrderConfigMgr);
	centerWnd->Create(IDD_ORDER_CENTER_HD, this);
	centerWnd->ShowWindow(SW_HIDE);
	SetActiveCenterWnd(centerWnd);
	_OrderConfigMgr->_HdCenterWnd = centerWnd;
	centerWnd->InitAll();
	_CenterWndVector.push_back(centerWnd);
	//_CenterWndCount = _CenterWndVector.size();
	RefreshLayout(true, false);
	_EnableOnSizeEvent = true;
}

VtOrderCenterWndHd* VtOrderWndHd::AddWindow(CRect& rcWnd)
{
	VtOrderCenterWndHd* centerWnd = nullptr;
	centerWnd = new VtOrderCenterWndHd();
	centerWnd->x = rcWnd.left;
	centerWnd->y = 0;
	centerWnd->width = rcWnd.Width();
	centerWnd->height = rcWnd.Height();
	_CenterWndVector.push_back(centerWnd);
	CreateChildWindow(centerWnd, IDD_ORDER_CENTER_HD, this);
	SetActiveCenterWnd(centerWnd);
	centerWnd->OrderConfigMgr(_OrderConfigMgr);
	if (_OrderConfigMgr) {
		_OrderConfigMgr->_HdCenterWnd = centerWnd;
	}
	return centerWnd;
}

void VtOrderWndHd::RemoveWindow()
{
	_EnableOnSizeEvent = false;
	int count = _CenterWndVector.size();
	if (count < 2) {
		_EnableOnSizeEvent = true;
		return;
	}
	auto it = std::prev(_CenterWndVector.end());
	VtOrderCenterWndHd* centerWnd = *it;
	bool curCenterWnd = false;
	if (_OrderConfigMgr && centerWnd == _OrderConfigMgr->_HdCenterWnd)
		curCenterWnd = true;

	centerWnd->DestroyWindow();
	delete centerWnd;
	_CenterWndVector.erase(it);

	//_CenterWndCount = _CenterWndVector.size();

	if (curCenterWnd && _OrderConfigMgr) {
		it = std::prev(_CenterWndVector.end());
		_OrderConfigMgr->_HdCenterWnd = *it;
		SetActiveCenterWnd(*it);
	}

	RefreshLayout(true, false);

	_EnableOnSizeEvent = true;
}

void VtOrderWndHd::RecalcLayout(bool fixed, int maxHeight)
{
	if (!_LeftWnd.GetSafeHwnd() || !_RightWnd.GetSafeHwnd())
		return;

	LockWindowUpdate();

	CRect rcWnd;
	GetWindowRect(rcWnd);

	CRect rect, rcTemp;
	int left = 0;
	int top = 0;
	if (fixed)
		maxHeight = rcWnd.Height();

	_LeftWnd.GetWindowRect(&rcTemp);
	if (_ShowLeftWnd)
	{
		_LeftWnd.ShowWindow(SW_SHOW);
		rect.left = left;
		rect.top = top;
		rect.right = rect.left + rcTemp.Width();
		rect.bottom = rect.top + rcTemp.Height();
		_LeftWnd.MoveWindow(rect);

		left += rcTemp.Width();
		if (rcTemp.bottom - rcTemp.top > maxHeight)
			maxHeight = rcTemp.bottom - rcTemp.top;

		SetControlPositionsOnLeft(rect.Width());

	}
	else
	{
		_LeftWnd.ShowWindow(SW_HIDE);
		SetControlPositionsOnLeft(0);
	}
	VtRealTickWnd* realWnd = nullptr;
	int centerWidth = 0;
	for (auto it = _CenterWndVector.begin(); it != _CenterWndVector.end(); ++it)
	{
		VtOrderCenterWndHd* curWnd = *it;
	
		curWnd->GetWindowRect(rcTemp);
		ScreenToClient(rcTemp);
		centerWidth = curWnd->GetMaxWidth();

		rect.left = left;
		rect.top = VtGlobal::CenterWndYPos;
		rect.right = rect.left + centerWidth;
		rect.bottom = maxHeight - VtGlobal::CenterWndYPos;

		
		curWnd->MoveWindow(rect);
		//curWnd->RepositionControl(maxHeight);
		curWnd->AdjustControlForMode();
		

		left += centerWidth;
		if (rcTemp.bottom - rcTemp.top > maxHeight)
			maxHeight = rcTemp.bottom - rcTemp.top;
	}

	SetControlPositionsOnRight(left);

	_RightWnd.GetWindowRect(&rcTemp);

	if (_ShowRightWnd)
	{
		_RightWnd.ShowWindow(SW_SHOW);
		rect.left = left;
		rect.top = top;
		rect.right = rect.left + rcTemp.Width();
		rect.bottom = maxHeight;

		_RightWnd.MoveWindow(rect, FALSE);

		left += rect.Width();
		if (rcTemp.bottom - rcTemp.top > maxHeight)
			maxHeight = rcTemp.bottom - rcTemp.top;
	}
	else
	{
		left += 16;
		_RightWnd.ShowWindow(SW_HIDE);
	}
	if (fixed)
	{
		MoveWindow(rcWnd.left, rcWnd.top, left, maxHeight, TRUE);
	}
	UnlockWindowUpdate();
}

void VtOrderWndHd::ResizeWnd(int maxHeight)
{
	if (!_LeftWnd.GetSafeHwnd() || !_RightWnd.GetSafeHwnd())
		return;

	CRect rcWnd;
	GetWindowRect(rcWnd);

	CRect rect, rcTemp;
	int left = 0;
	int top = 0;

	_LeftWnd.GetWindowRect(&rcTemp);
	if (_ShowLeftWnd)
	{
		_LeftWnd.ShowWindow(SW_SHOW);
		rect.left = left;
		rect.top = top;
		rect.right = rect.left + rcTemp.Width();
		rect.bottom = rect.top + rcTemp.Height();
		//_LeftWnd.MoveWindow(rect);

		left += rcTemp.Width();
	}
	else
	{
		_LeftWnd.ShowWindow(SW_HIDE);
	}
	VtRealTickWnd* realWnd = nullptr;

	for (auto it = _CenterWndVector.begin(); it != _CenterWndVector.end(); ++it)
	{
		VtOrderCenterWndHd* curWnd = *it;
		rect.left = left;
		rect.top = VtGlobal::CenterWndYPos;
		rect.right = rect.left + curWnd->GetMaxWidth();
		rect.bottom = maxHeight;

		curWnd->MoveWindow(rect, FALSE);
		curWnd->AdjustControlForMode();

		left += curWnd->GetMaxWidth();
		curWnd->RepositionControl(maxHeight - VtGlobal::CenterWndYPos);
		curWnd->ResizeOrderGrid(maxHeight);
	}

	_RightWnd.GetWindowRect(&rcTemp);

	if (_ShowRightWnd)
	{
		_RightWnd.ShowWindow(SW_SHOW);
		rect.left = left;
		rect.top = top;
		rect.right = rect.left + rcTemp.Width();
		rect.bottom = maxHeight;

		_RightWnd.MoveWindow(rect, FALSE);

		left += rect.Width();
	}
	else
	{
		left += 16;
		_RightWnd.ShowWindow(SW_HIDE);
	}

	//Invalidate(TRUE);
	ClickedRightExtend = false;
}

void VtOrderWndHd::ShowLeftWnd()
{
	_ShowLeftWnd = !_ShowLeftWnd;
	if (_ShowLeftWnd) {
		//_BtnShowLeft.SetWindowText(_T("▶"));
		_BtnShowLeft.SetIcon(IDI_RIGHT, 16, 16, IDI_RIGHT, 16, 16);
		_BtnShowLeft.OffsetColor(BTNST_COLOR_BK_IN, 30);
		_BtnShowLeft.SetAlign(ST_ALIGN_VERT);
	} else {
		//_BtnShowLeft.SetWindowText(_T("◀"));
		_BtnShowLeft.SetIcon(IDI_LEFT, 16, 16, IDI_LEFT, 16, 16);
		_BtnShowLeft.OffsetColor(BTNST_COLOR_BK_IN, 30);
		_BtnShowLeft.SetAlign(ST_ALIGN_VERT);
	}
	RefreshLayout(true, false);
}

void VtOrderWndHd::ShowRightWnd()
{
	_ShowRightWnd = !_ShowRightWnd;
	if (_ShowRightWnd) {
		//_BtnShowRight.SetWindowText(_T("◀"));
		_BtnShowRight.SetIcon(IDI_LEFT, 16, 16, IDI_LEFT, 16, 16);
		_BtnShowRight.OffsetColor(BTNST_COLOR_BK_IN, 30);
		_BtnShowRight.SetAlign(ST_ALIGN_VERT);
	} else {
		//_BtnShowRight.SetWindowText(_T("▶"));
		_BtnShowRight.SetIcon(IDI_RIGHT, 16, 16, IDI_RIGHT, 16, 16);
		_BtnShowRight.OffsetColor(BTNST_COLOR_BK_IN, 30);
		_BtnShowRight.SetAlign(ST_ALIGN_VERT);
	}
	RefreshLayout(true, false);
}

VtFund* VtOrderWndHd::GetCurrentFund()
{
	if (!_OrderConfigMgr)
		return nullptr;
	else
		return _OrderConfigMgr->Fund();
}

void VtOrderWndHd::ShowHideCtrl()
{
	if ( _CenterWndVector.size() == 0)
		return;
	if (_CenterWndVector.size() == 1) {
		VtOrderCenterWndHd* centerWnd = _CenterWndVector.front();
		int optionCnt = centerWnd->GetCountOrderGridEnabledCol();
		if (optionCnt == 0) {
			_BtnGetAcntInfo.ShowWindow(SW_HIDE);
			_StaticAcntName.ShowWindow(SW_HIDE);
			if (centerWnd->ShowTickWnd()) {
				_BtnShowRight.ShowWindow(SW_SHOW);
				_BtnAddWnd.ShowWindow(SW_SHOW);
				_BtnDelWnd.ShowWindow(SW_SHOW);
				if (_OrderConfigMgr->Type() == 0) {
					_BtnFundEditor.ShowWindow(SW_HIDE);
				}
				else {
					_BtnFundEditor.ShowWindow(SW_SHOW);
				}
			}
			else {
				_BtnShowRight.ShowWindow(SW_HIDE);
				_BtnAddWnd.ShowWindow(SW_HIDE);
				_BtnDelWnd.ShowWindow(SW_HIDE);
				_BtnFundEditor.ShowWindow(SW_HIDE);
			}
		}
		else if (optionCnt == 1) {
			_BtnShowRight.ShowWindow(SW_SHOW);
			_BtnAddWnd.ShowWindow(SW_SHOW);
			_BtnDelWnd.ShowWindow(SW_SHOW);
			_StaticAcntName.ShowWindow(SW_SHOW);
			if (centerWnd->ShowTickWnd()) {
				if (_OrderConfigMgr->Type() == 0) {
					_BtnGetAcntInfo.ShowWindow(SW_SHOW);
					_EditPwd.ShowWindow(SW_SHOW);
					_BtnFundEditor.ShowWindow(SW_HIDE);
				}
				else {
					_EditPwd.ShowWindow(SW_HIDE);
					_BtnGetAcntInfo.ShowWindow(SW_HIDE);
					_BtnFundEditor.ShowWindow(SW_SHOW);
				}
			}
			else {
				_BtnGetAcntInfo.ShowWindow(SW_HIDE);
				_StaticAcntName.ShowWindow(SW_HIDE);
				_BtnFundEditor.ShowWindow(SW_HIDE);
			}
		}
		else if (optionCnt == 2) {
			_StaticAcntName.ShowWindow(SW_SHOW);
			_BtnShowRight.ShowWindow(SW_SHOW);
			_BtnAddWnd.ShowWindow(SW_SHOW);
			_BtnDelWnd.ShowWindow(SW_SHOW);
			if (centerWnd->ShowTickWnd()) {
				if (_OrderConfigMgr->Type() == 0) {
					_EditPwd.ShowWindow(SW_SHOW);
					_BtnFundEditor.ShowWindow(SW_HIDE);
					_BtnGetAcntInfo.ShowWindow(SW_SHOW);
				}
				else {
					_EditPwd.ShowWindow(SW_HIDE);
					_BtnFundEditor.ShowWindow(SW_SHOW);
					_BtnGetAcntInfo.ShowWindow(SW_HIDE);
				}
			}
			else {
				_BtnFundEditor.ShowWindow(SW_HIDE);
				if (_OrderConfigMgr->Type() == 0)
					_BtnGetAcntInfo.ShowWindow(SW_SHOW);
				else
					_BtnGetAcntInfo.ShowWindow(SW_HIDE);
			}
		}
		else {
			_StaticAcntName.ShowWindow(SW_SHOW);
			_BtnShowRight.ShowWindow(SW_SHOW);
			_BtnAddWnd.ShowWindow(SW_SHOW);
			_BtnDelWnd.ShowWindow(SW_SHOW);
			if (_OrderConfigMgr->Type() == 0) {
				_EditPwd.ShowWindow(SW_SHOW);
				_BtnGetAcntInfo.ShowWindow(SW_SHOW);
				_BtnFundEditor.ShowWindow(SW_HIDE);
			}
			else {
				_EditPwd.ShowWindow(SW_HIDE);
				_BtnGetAcntInfo.ShowWindow(SW_HIDE);
				_BtnFundEditor.ShowWindow(SW_SHOW);
			}
		}
	}
	else {
		_StaticAcntName.ShowWindow(SW_SHOW);
		_BtnShowRight.ShowWindow(SW_SHOW);
		_BtnAddWnd.ShowWindow(SW_SHOW);
		_BtnDelWnd.ShowWindow(SW_SHOW);
		if (_OrderConfigMgr->Type() == 0) {
			_EditPwd.ShowWindow(SW_SHOW);
			_BtnGetAcntInfo.ShowWindow(SW_SHOW);
			_BtnFundEditor.ShowWindow(SW_HIDE);
		}
		else {
			_EditPwd.ShowWindow(SW_HIDE);
			_BtnGetAcntInfo.ShowWindow(SW_HIDE);
			_BtnFundEditor.ShowWindow(SW_SHOW);
		}
	}
}

void VtOrderWndHd::SetActiveCenterWnd(VtOrderCenterWndHd* centerWnd)
{
	if (_OrderConfigMgr && _OrderConfigMgr->_HdCenterWnd == centerWnd)
		return;

	_OrderConfigMgr->_HdCenterWnd->Activated(false);
	centerWnd->Activated(true);
	_OrderConfigMgr->_HdCenterWnd = centerWnd;
}

void VtOrderWndHd::RemoveLastWindow()
{
	if (_CenterWndVector.size() == 1)
		return;

	VtOrderCenterWndHd* centerWnd = _CenterWndVector[_CenterWndVector.size() - 1];
	if (centerWnd)
	{
		centerWnd->DestroyWindow();
		delete centerWnd;

		auto it = _CenterWndVector.end();
		_CenterWndVector.erase(it);
	}
}


void VtOrderWndHd::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	//if (_OrderConfigMgr && _OrderConfigMgr->_HdCenterWnd)
	//{
	//	_OrderConfigMgr->_HdCenterWnd->UnregisterOrderWnd();
	//	_OrderConfigMgr->_HdCenterWnd->UnregisterSymbolWnd();
	//}
	CDialog::OnClose();
	VtOrderWndEventArgs arg;
	arg.pOrderWnd = this;
	arg.type = VtOrderWindowEventType::Closed;
	FireOrderWindowEvent(std::move(arg));
}

void VtOrderWndHd::OnFundAdded()
{
	if (_OrderConfigMgr->Type() == 0)
		return;

	CString str1;
	int selAcnt = _ComboAcnt.GetCurSel();
	if (selAcnt != -1)
	{
		_ComboAcnt.GetLBText(selAcnt, str1);

		_StaticAcnt.SetWindowText(_T("펀드"));
		VtFundManager* fundMgr = VtFundManager::GetInstance();
		std::map<std::string, VtFund*>& fundList = fundMgr->GetFundList();
		if (fundList.size() == 0)
		{
			_ComboAcnt.ResetContent();
			_OrderConfigMgr->Fund(nullptr);
			return;
		}
		_ComboAcnt.ResetContent();
		for (auto it = fundList.begin(); it != fundList.end(); ++it)
		{
			VtFund* fund = it->second;
			int index = _ComboAcnt.AddString(fund->Name.c_str());
			_ComboAcnt.SetItemDataPtr(index, fund);
		}

		int index = _ComboAcnt.FindString(0, str1);
		int selIndex = 0;
		if (index == CB_ERR)
		{
			_ComboAcnt.SetCurSel(selIndex);
		}
		else
		{
			_ComboAcnt.SetCurSel(index);
			selIndex = index;
		}

		VtFund* fund = (VtFund*)_ComboAcnt.GetItemDataPtr(selIndex);
		_OrderConfigMgr->Fund(fund);
		if (_OrderConfigMgr->Fund())
		{
			_StaticAcntName.SetWindowText(_OrderConfigMgr->Fund()->Name.c_str());
			_OrderConfigMgr->_HdLeftWnd->RefreshProfitLoss();
			_OrderConfigMgr->_HdLeftWnd->RefreshAsset();
		}
	}
	else
	{
		InitFund();
	}
}

void VtOrderWndHd::OnFundDeleted(VtFund* delFund)
{
	if (_OrderConfigMgr->Type() == 0 || !delFund)
		return;

	CString str1;
	int selAcnt = _ComboAcnt.GetCurSel();
	if (selAcnt != -1)
	{
		_ComboAcnt.GetLBText(selAcnt, str1);

		_StaticAcnt.SetWindowText(_T("펀드"));
		VtFundManager* fundMgr = VtFundManager::GetInstance();
		std::map<std::string, VtFund*>& fundList = fundMgr->GetFundList();
		if (fundList.size() == 0)
		{
			_ComboAcnt.ResetContent();
			_OrderConfigMgr->Fund(nullptr);
			return;
		}
		_ComboAcnt.ResetContent();
		for (auto it = fundList.begin(); it != fundList.end(); ++it)
		{
			VtFund* fund = it->second;
			int index = _ComboAcnt.AddString(fund->Name.c_str());
			_ComboAcnt.SetItemDataPtr(index, fund);
		}

		int index = _ComboAcnt.FindString(0, str1);
		int selIndex = 0;
		if (index == CB_ERR)
		{
			_ComboAcnt.SetCurSel(selIndex);
		}
		else
		{
			_ComboAcnt.SetCurSel(index);
			selIndex = index;
		}

		VtFund* fund = (VtFund*)_ComboAcnt.GetItemDataPtr(selIndex);
		_OrderConfigMgr->Fund(fund);
		if (_OrderConfigMgr->Fund())
		{
			_StaticAcntName.SetWindowText(_OrderConfigMgr->Fund()->Name.c_str());
			_OrderConfigMgr->_HdLeftWnd->RefreshProfitLoss();
			_OrderConfigMgr->_HdLeftWnd->RefreshAsset();
		}
	}
	else
	{
		InitFund();
	}
}

bool VtOrderWndHd::InitFund()
{
	if (!_OrderConfigMgr)
		return false;

	_ComboAcnt.ResetContent();

	_StaticAcnt.SetWindowText(_T("펀드"));
	VtFundManager* fundMgr = VtFundManager::GetInstance();
	std::map<std::string, VtFund*>& fundList = fundMgr->GetFundList();
	if (fundList.size() == 0) {
		_ComboAcnt.ResetContent();
		_OrderConfigMgr->Fund(nullptr);
		return false;
	}
	// 콤보 박스에 펀드 추가
	int selIndex = -1;
	for (auto it = fundList.begin(); it != fundList.end(); ++it) {
		VtFund* fund = it->second;
		int index = _ComboAcnt.AddString(fund->Name.c_str());
		if (_DefaultFundName.compare(fund->Name) == 0) { // 정해진 펀드가 있으면 선택
			selIndex = index;
			_OrderConfigMgr->Fund(fund);
		}
		_ComboAcnt.SetItemDataPtr(index, fund);
	}
	 
	if (selIndex == -1) { // 정해진 펀드가 없을 때 맨처음 펀드 선택
		VtFund* fund = fundList.begin()->second;
		selIndex = 0;
		_OrderConfigMgr->Fund(fund);
	}
	// 찾은 펀드 선택
	_ComboAcnt.SetCurSel(selIndex);
	if (_OrderConfigMgr->Fund()) {
		_StaticAcntName.SetWindowText(_OrderConfigMgr->Fund()->Name.c_str());
		_OrderConfigMgr->_HdLeftWnd->RefreshProfitLoss();
		_OrderConfigMgr->_HdLeftWnd->RefreshAsset();
	}

	return true;
}

void VtOrderWndHd::InitAccount()
{
	if (!_OrderConfigMgr)
		return;

	_StaticAcnt.SetWindowText(_T("계좌"));
	std::map<std::string, std::pair<int, VtAccount*>> comboMap;

	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	int selAcnt = -1, index = 0;
	std::string acntName;
	for (auto it = acntMgr->AccountMap.begin(); it != acntMgr->AccountMap.end(); ++it) {
		VtAccount* acnt = it->second;
		acntName = acnt->AccountNo;
		acntName.append(_T(" "));
		acntName.append(acnt->AccountName);
		index = _ComboAcnt.AddString(acntName.c_str());
		comboMap[acnt->AccountNo] = std::make_pair(index, acnt);
		_ComboAcnt.SetItemDataPtr(index, acnt);
		if (acnt->AccountLevel() == 0) {
			std::vector<VtAccount*>& subAcntList = acnt->GetSubAccountList();
			for (auto it = subAcntList.begin(); it != subAcntList.end(); ++it) {
				VtAccount* subAcnt = *it;
				acntName = subAcnt->AccountNo;
				acntName.append(_T(" "));
				acntName.append(subAcnt->AccountName);
				index = _ComboAcnt.AddString(acntName.c_str());
				comboMap[subAcnt->AccountNo] = std::make_pair(index, subAcnt);
				_ComboAcnt.SetItemDataPtr(index, subAcnt);
				
			}
		}
	}

	if (comboMap.size() == 0)
		return;
	auto it = comboMap.find(_DefaultAccountNo);
	if (it != comboMap.end()) {
		selAcnt = it->second.first;
		_OrderConfigMgr->Account(it->second.second);
	}
	else {
		selAcnt = 0;
		VtAccount* acnt = (VtAccount*)_ComboAcnt.GetItemDataPtr(selAcnt);
		_OrderConfigMgr->Account(acnt);
	}
	_ComboAcnt.SetCurSel(selAcnt);
	if (_OrderConfigMgr->Account()) {
		_StaticAcntName.SetWindowText(_OrderConfigMgr->Account()->AccountName.c_str());
		_OrderConfigMgr->_HdLeftWnd->RefreshProfitLoss();
		_OrderConfigMgr->_HdLeftWnd->RefreshAsset();
		CString pwd;
		pwd.Format(_T("%s"), _OrderConfigMgr->Account()->Password.c_str());
		_EditPwd.SetWindowText(pwd);
	}
}

void VtOrderWndHd::OnReceiveHoga(VtSymbol* sym)
{
	if (!sym)
		return;
	VtOrderCenterWndHd* centerWnd = nullptr;
	for (auto it = _CenterWndVector.begin(); it != _CenterWndVector.end(); ++it) {
		(*it)->OnReceiveHoga(sym);
	}
}

void VtOrderWndHd::OnReceiveQuote(VtSymbol* sym)
{
	if (!sym)
		return;

	_LeftWnd.OnReceiveQuote(sym);
}

void VtOrderWndHd::OnExpected(VtSymbol* sym)
{
	_LeftWnd.OnExpected(sym);
}

void VtOrderWndHd::OnReceiveAccountInfo()
{
	_LeftWnd.OnReceiveAccountInfo();
}

void VtOrderWndHd::OnReceiveMsg(CString msg)
{
	for (auto it = _CenterWndVector.begin(); it != _CenterWndVector.end(); ++it) {
		(*it)->OnReceiveMsg(msg);
	}
}

void VtOrderWndHd::OnReceiveMsgWithReqId(int id, CString msg)
{
	for (auto it = _CenterWndVector.begin(); it != _CenterWndVector.end(); ++it) {
		(*it)->OnReceiveMsgWithReqId(id, msg);
	}
}

void VtOrderWndHd::GetOptionSymbolMaster()
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	for (auto it = prdtCatMgr->MainOptionVector.begin(); it != prdtCatMgr->MainOptionVector.end(); ++it) {
		std::string secName = *it;
		VtProductSection* section = prdtCatMgr->FindProductSection(secName);
		if (section && section->SubSectionVector.size() == 2) {
			VtProductSubSection* subSec = section->SubSectionVector[0];
			for (auto its = subSec->_SymbolVector.begin(); its != subSec->_SymbolVector.end(); ++its) {
				VtSymbol* sym = *its;
				sym->GetSymbolMaster();
			}

			subSec = section->SubSectionVector[1];
			for (auto its = subSec->_SymbolVector.begin(); its != subSec->_SymbolVector.end(); ++its) {
				VtSymbol* sym = *its;
				sym->GetSymbolMaster();
			}
		}
	}
}

void VtOrderWndHd::OnOutstanding()
{
	_LeftWnd.OnOutstanding();
}


void VtOrderWndHd::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	CRect rcWnd;
	GetWindowRect(rcWnd);
	if (_EnableOnSizeEvent) {
		_WindowHeight = rcWnd.Height();
		RefreshLayout(false, true);
	}
	_LeftWnd.OnResizeWnd();
}


void VtOrderWndHd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnLButtonDown(nFlags, point);
}


void VtOrderWndHd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnLButtonUp(nFlags, point);
}


void VtOrderWndHd::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);
	_Sizing = true;

}

void VtOrderWndHd::CreateChildWindow(VtOrderCenterWndHd* centerWnd, UINT id, CWnd* parent)
{
	if (!centerWnd || !parent)
		return;

	//centerWnd->CRHCreateGenericChildDialog(this, &rect, _WindowID++, NULL);
	centerWnd->Create(IDD_ORDER_CENTER_HD, parent);
	centerWnd->ModifyStyle(WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, WS_CHILD);
	// Adjust extended styles. 1st parameter removes, 2nd adds.
	centerWnd->ModifyStyleEx(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE, WS_EX_CONTROLPARENT);
	// As we have changed the frame, we let Windows recalculate the non-client area
	// by passing the SWP_FRAMECHANGED flag to SetWindowPos().
}

void VtOrderWndHd::SetControlPositionsOnLeft(int left)
{
	if (!_OrderConfigMgr)
		return;

	CRect rcCtrl;
	int staticY = 5;
	_BtnShowLeft.GetWindowRect(rcCtrl);
	_BtnShowLeft.SetWindowPos(nullptr, left, 0, rcCtrl.Width(), rcCtrl.Height(), SWP_NOZORDER);
	left += rcCtrl.Width();
	_StaticAcnt.GetWindowRect(rcCtrl);
	_StaticAcnt.SetWindowPos(nullptr, left, staticY, rcCtrl.Width(), rcCtrl.Height(), SWP_NOZORDER);
	left += rcCtrl.Width();
	_ComboAcnt.GetWindowRect(rcCtrl);
	_ComboAcnt.SetWindowPos(nullptr, left, 0, rcCtrl.Width(), rcCtrl.Height(), SWP_NOZORDER);
	left += rcCtrl.Width();

	if (_OrderConfigMgr->Type() == 0)
	{
		_BtnGetAcntInfo.GetWindowRect(rcCtrl);
		_BtnGetAcntInfo.SetWindowPos(nullptr, left, 0, rcCtrl.Width(), rcCtrl.Height(), SWP_NOZORDER);

		left += rcCtrl.Width();
		_StaticAcntName.GetWindowRect(rcCtrl);
		CSize size;
		CString sText;
		_StaticAcntName.GetWindowText(sText);
		CClientDC dc(this);
		CFont *font;
		font = GetFont();
		dc.SelectObject(font);
		CRect rect;
		size = dc.GetTextExtent(sText, sText.GetLength());

		_StaticAcntName.SetWindowPos(nullptr, left, staticY, rcCtrl.Width(), rcCtrl.Height(), SWP_NOZORDER);

		_BtnGetAcntInfo.ShowWindow(SW_SHOW);
		_StaticAcntName.ShowWindow(SW_SHOW);

		_BtnFundEditor.ShowWindow(SW_HIDE);
	}
	else
	{
		_BtnFundEditor.GetWindowRect(rcCtrl);
		_BtnFundEditor.SetWindowPos(nullptr, left, 0, rcCtrl.Width(), rcCtrl.Height(), SWP_NOZORDER);

		_BtnGetAcntInfo.ShowWindow(SW_HIDE);
		_StaticAcntName.ShowWindow(SW_HIDE);

		_BtnFundEditor.ShowWindow(SW_SHOW);
	}
	
}

void VtOrderWndHd::SetControlPositionsOnRight(int right)
{
	CRect rcCtrl;
	_BtnShowRight.GetWindowRect(rcCtrl);
	right -= rcCtrl.Width();
	_BtnShowRight.SetWindowPos(nullptr, right, 0, rcCtrl.Width(), rcCtrl.Height(), SWP_NOZORDER);
	
	_BtnAddWnd.GetWindowRect(rcCtrl);
	right -= rcCtrl.Width();
	_BtnAddWnd.SetWindowPos(nullptr, right, 0, rcCtrl.Width(), rcCtrl.Height(), SWP_NOZORDER);

	_BtnDelWnd.GetWindowRect(rcCtrl);
	right -= rcCtrl.Width();
	_BtnDelWnd.SetWindowPos(nullptr, right, 0, rcCtrl.Width(), rcCtrl.Height(), SWP_NOZORDER);
}

CRect VtOrderWndHd::GetLastCenterWindowPos()
{
	CRect rcWnd;
	VtOrderCenterWndHd* centerWnd = _CenterWndVector.back();
	centerWnd->GetWindowRect(rcWnd);
	ScreenToClient(rcWnd);
	return rcWnd;
}

void VtOrderWndHd::SaveControlPos()
{
	if (!_LayoutMgr)
		return;
	CRect rcWnd;
	_LayoutMgr->AddWindow(_T("왼쪽창보이기버튼"), IDC_BTN_SHOW_LEFT, GetClientArea(IDC_BTN_SHOW_LEFT));
	_LayoutMgr->AddWindow(_T("계좌레이블"), IDC_STATIC_ACNT, GetClientArea(IDC_STATIC_ACNT));
	_LayoutMgr->AddWindow(_T("계좌콤보"), IDC_COMBO_ACCOUNT_HD, GetClientArea(IDC_COMBO_ACCOUNT_HD));
	_LayoutMgr->AddWindow(_T("계좌조회버튼"), IDC_BTN_GET_ACNT_INFO, GetClientArea(IDC_BTN_GET_ACNT_INFO));
	_LayoutMgr->AddWindow(_T("계좌이름"), IDC_STATIC_ACNT_NAME, GetClientArea(IDC_STATIC_ACNT_NAME));
	_LayoutMgr->AddWindow(_T("주문창제거버튼"), IDC_BTN_DELWND, GetClientArea(IDC_BTN_DELWND));
	_LayoutMgr->AddWindow(_T("주문창추가버튼"), IDC_BTN_ADDWND, GetClientArea(IDC_BTN_ADDWND));
	_LayoutMgr->AddWindow(_T("오른쪽창보이기버튼"), IDC_BTN_SHOWRIGHT, GetClientArea(IDC_BTN_SHOWRIGHT));
	_LayoutMgr->AddWindow(_T("펀드편집창버튼"), IDC_BTN_EDIT_FUND, GetClientArea(IDC_BTN_EDIT_FUND));
	_LayoutMgr->AddWindow(_T("비밀번호입력창"), IDC_EDIT_PWD, GetClientArea(IDC_EDIT_PWD));

}

CRect VtOrderWndHd::GetClientArea(int resourceID)
{
	CWnd* wnd = (CWnd*)GetDlgItem(resourceID);
	CRect rcWnd;
	wnd->GetWindowRect(rcWnd);
	ScreenToClient(rcWnd);

	return rcWnd;
}

void VtOrderWndHd::ReposChildWindowsForward()
{
	for (auto it = _ChildWindowInfo.begin(); it != _ChildWindowInfo.end(); ++it) {
		auto item = *it;
		CWnd* wnd = std::get<0>(item);
		CRect& rect = std::get<2>(item);
		if (std::get<1>(item) && wnd->GetSafeHwnd()) {
			wnd->ShowWindow(SW_SHOW);
			wnd->SetWindowPos(nullptr, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER | SWP_FRAMECHANGED | SWP_DRAWFRAME);
			wnd->Invalidate(FALSE);
		}
		else {
			if (wnd->GetSafeHwnd())
				wnd->ShowWindow(SW_HIDE);
		}
	}
}

void VtOrderWndHd::ReposChildWindowsBackward()
{
	for (auto it = _ChildWindowInfo.rbegin(); it != _ChildWindowInfo.rend(); ++it) {
		auto item = *it;
		CWnd* wnd = std::get<0>(item);
		CRect& rect = std::get<2>(item);
		if (std::get<1>(item)) {
			wnd->ShowWindow(SW_SHOW);
			wnd->SetWindowPos(nullptr, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER | SWP_FRAMECHANGED | SWP_DRAWFRAME);
			wnd->Invalidate(FALSE);
		}
		else {
			wnd->ShowWindow(SW_HIDE);
		}
	}
}

void VtOrderWndHd::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnGetMinMaxInfo(lpMMI);
}

void VtOrderWndHd::OnSymbolMaster(VtSymbol* sym)
{
	_LeftWnd.OnSymbolMaster(sym);
}

void VtOrderWndHd::OnRemain(VtPosition* posi)
{
	_LeftWnd.OnRemain(posi);
}

void VtOrderWndHd::OnOrderAccepted(VtOrder* order)
{
	_LeftWnd.OnOrderAccepted(order);
}

void VtOrderWndHd::OnOrderUnfilled(VtOrder* order)
{
	_LeftWnd.OnOrderUnfilled(order);
}

void VtOrderWndHd::OnOrderFilled(VtOrder* order)
{
	_LeftWnd.OnOrderFilled(order);
}

void VtOrderWndHd::ChangeAccount(VtAccount* acnt)
{

}


void VtOrderWndHd::OnCbnSelchangeComboAccountHd()
{
	/*
	계좌 변경 과정
	0. 주문설정 관리자의 계좌를 바꿔준다.아래의 정보는 각창에 모두 적용한다.왼쪽창에도 적용해 준다.
	1. 주문관리자는 각 주문차에서 직접 설정해서 사용한다. 외부에서 전체적으로 따로 설정하지 않는다.
	// 각주문창에서 할 일들
	1. 주문관리자를 계좌에 맞게 바꿔준다.
	1. 계좌 실시간 이벤트를 등록해 준다.
	2. 주문창(가운데)의 현재 포지션을 없앤다.
	3. 기존 주문창 이벤트 등록 정보를 해제한다.
	4. 종목 주문 관리자를 바꿔준다.
	5. 그리드 정보를 모두 새로운 정보에 맞춰 갱신한다.
	6. 포지션 정보를 설정해 준다.
	7. 주문창 이벤트 정보를 등록해 준다.
	*/
	if (_OrderConfigMgr->Type() == 0) {
		// 기존 주문 실시간 등록 해제
		if (_Account)
			UnregisterRealtimeAccount(_Account);
		int curSel = _ComboAcnt.GetCurSel();
		if (curSel != -1) {
			VtAccount* acnt = (VtAccount*)_ComboAcnt.GetItemDataPtr(curSel);
			if (!acnt)
				return;

			// 주문 설정 객체 계좌 설정
			_OrderConfigMgr->Account(acnt);
			_StaticAcntName.SetWindowText(_OrderConfigMgr->Account()->AccountName.c_str());

			for (auto it = _CenterWndVector.begin(); it != _CenterWndVector.end(); ++it) {
				VtOrderCenterWndHd* centerWnd = *it;
				centerWnd->ChangeAccount(_OrderConfigMgr->Account());
			}
			if (acnt->AccountLevel() == 0 && acnt->hasValidPassword()) {
				// Register the new account to the Event Map.
				RegisterRealtimeAccount(acnt);
				acnt->GetAccountInfoNFee(1);
			}
		}
	}
	else {
		if (_OrderConfigMgr->Fund()) {
			std::set<VtAccount*> parendAcntSet = _OrderConfigMgr->Fund()->GetParentAccountSet();
			for (auto it = parendAcntSet.begin(); it != parendAcntSet.end(); ++it) {
				VtAccount* acnt = *it;
				UnregisterRealtimeAccount(acnt);
			}
		}
		int curSel = _ComboAcnt.GetCurSel();
		if (curSel != -1) {
			VtFund* fund = (VtFund*)_ComboAcnt.GetItemDataPtr(curSel);
			if (!fund)
				return;

			// 주문 설정 객체 계좌 설정
			_OrderConfigMgr->Fund(fund);
			_StaticAcntName.SetWindowText(_OrderConfigMgr->Fund()->Name.c_str());

			// Register the new account to the Event Map.
			std::set<VtAccount*> parendAcntSet = _OrderConfigMgr->Fund()->GetParentAccountSet();
			for (auto it = parendAcntSet.begin(); it != parendAcntSet.end(); ++it) {
				VtAccount* parentAcnt = *it;
				RegisterRealtimeAccount(parentAcnt);
			}
			for (auto it = _CenterWndVector.begin(); it != _CenterWndVector.end(); ++it) {
				VtOrderCenterWndHd* centerWnd = *it;
				centerWnd->ChangeFund(_OrderConfigMgr->Fund());
			}
		}
	}

	if (_LeftWnd) {
		_LeftWnd.OnAccountChanged();
	}
}


void VtOrderWndHd::OnBnClickedBtnShowLeft()
{
	// TODO: Add your control notification handler code here
	ShowLeftWnd();
}


void VtOrderWndHd::OnBnClickedBtnGetAcntInfo()
{
	// TODO: Add your control notification handler code here
	int curSel = _ComboAcnt.GetCurSel();
	if (curSel != -1)
	{
		VtAccount* acnt = (VtAccount*)_ComboAcnt.GetItemDataPtr(curSel);
		if (acnt && acnt->AccountLevel() == 0 && acnt->hasValidPassword())
			acnt->GetAccountProfitLossDirect();
	}
}


void VtOrderWndHd::OnBnClickedBtnDelwnd()
{
	// TODO: Add your control notification handler code here
	RemoveWindow();
}


void VtOrderWndHd::OnBnClickedBtnAddwnd()
{
	// TODO: Add your control notification handler code here
	AddWindow();
}


void VtOrderWndHd::OnBnClickedBtnShowright()
{
	// TODO: Add your control notification handler code here
	ShowRightWnd();
}


void VtOrderWndHd::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0x0000FFF0) == SC_MINIMIZE)
	{
		// Minimizing, post to main dialogue also.
		//AfxGetMainWnd()->ShowWindow(SW_MINIMIZE);
	}
	else if ((nID & 0x0000FFF0) == SC_RESTORE && IsIconic())
	{
		// Restoring, post to main dialogue also.
		//AfxGetMainWnd()->ShowWindow(SW_RESTORE);
		Invalidate(TRUE);
	}

	if (_EnableOnSizeEvent && (nID & 0x0000FFF0) == SC_KEYMENU) {
		ResetByCenterRow();
		return;
	}

	//SC_KEYMENU   
	CDialog::OnSysCommand(nID, lParam);
}

void VtOrderWndHd::Save(simple::file_ostream<same_endian_type>& ss)
{
	if (_OrderConfigMgr) {
		// 주문창 타입 저장
		ss << _OrderConfigMgr->Type();

		// 주문창 타입에 따라 계좌번호 혹은 펀드 이름 저장
		if (_OrderConfigMgr->Type() == 0)
			ss << _OrderConfigMgr->Account()->AccountNo;
		else
			ss << _OrderConfigMgr->Fund()->Name;

		CRect rcWnd;
		GetWindowRect(rcWnd);
		WINDOWPLACEMENT wp;
		GetWindowPlacement(&wp);
		// 주문창 크기 및 위치 저장
		ss << rcWnd.left << rcWnd.top << rcWnd.right << rcWnd.bottom;
		// 좌측창  보기 여부, 우측창 보기여부
		ss << _ShowLeftWnd << _ShowRightWnd;

		// 중앙창 갯수 저장
		ss << (int)_CenterWndVector.size();

		for (auto it = _CenterWndVector.begin(); it != _CenterWndVector.end(); ++it) {
			VtOrderCenterWndHd* centerWnd = *it;
			centerWnd->GetWindowRect(rcWnd);
			ScreenToClient(rcWnd);
			// 중앙창 위치 및 크기 저장
			ss << rcWnd.left << rcWnd.top << rcWnd.right << rcWnd.bottom;
			CString msg;
			msg.Format(_T("save = left = %d, top = %d, right = %d, bottom = %d, width = %d, height = %d \n"), rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, rcWnd.Width(), rcWnd.Height());
			// 중앙창 옵션 저장
			centerWnd->Save(ss);
		}
	}
}

void VtOrderWndHd::Load(simple::file_istream<same_endian_type>& ss)
{
	if (!_OrderConfigMgr)
		return;

	int type = 0;
	// 주문창 타입 복원
	ss >> type;
	// 주문창 타입 대입
	_OrderConfigMgr->Type(type);

	// 주문창 타입에 따른 계좌 혹은 펀드 이름 복원
	if (_OrderConfigMgr->Type() == 0) {
		ss >> _DefaultAccountNo;
	} else {
		ss >> _DefaultFundName;
	}
	

	CRect rcWnd;
	// 윈도우 크기 위치 복원
	ss >> rcWnd.left >> rcWnd.top >> rcWnd.right >> rcWnd.bottom;

	// 윈도우 위치 대입
	_XPos = rcWnd.left;
	_YPos = rcWnd.top;
	// 윈도우 높이 대입
	_WindowHeight = rcWnd.Height();
	// 윈도우 너비 대입
	_WindowWidth = rcWnd.Width();

	ss >> _ShowLeftWnd;
	ss >> _ShowRightWnd;
	
	int count = 0;
	// 중앙창 갯수 복원
	ss >> count;
	for (int i = 0; i < count; i++) {
		// 중앙창 크기 위치 복원
		ss >> rcWnd.left >> rcWnd.top >> rcWnd.right >> rcWnd.bottom;

		CString msg;
		msg.Format(_T("load :: left = %d, top = %d, right = %d, bottom = %d, width = %d, height = %d \n"), rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, rcWnd.Width(), rcWnd.Height());

		VtOrderCenterWndHd* centerWnd = new VtOrderCenterWndHd();
		// 중앙창 모든 옵션 복원
		centerWnd->Load(ss);
		// 중앙창 목록에 추가
		_CenterWndVector.push_back(centerWnd);
	}
	// 중앙창 갯수 대입
	//_CenterWndCount = _CenterWndVector.size();
}

void VtOrderWndHd::SetAccount(VtAccount* acnt)
{
	if (!acnt || !_OrderConfigMgr)
		return;
	_Account = acnt;

	VtOrderManagerSelector* orderMgrSelecter = VtOrderManagerSelector::GetInstance();
	VtOrderManager* _OrderMgr = orderMgrSelecter->FindOrderManager(acnt->AccountNo);
	_OrderConfigMgr->OrderMgr(_OrderMgr);
}

void VtOrderWndHd::SetFund(VtFund* fund)
{
	if (!fund || !_OrderConfigMgr)
		return;
	_Fund = fund;

	VtFundOrderManagerSelecter* fundOrderMgrSelecter = VtFundOrderManagerSelecter::GetInstance();
	VtOrderManager* fundOrderMgr = (VtOrderManager*)fundOrderMgrSelecter->FindAddOrderManager(fund->Name);
	_OrderConfigMgr->OrderMgr(fundOrderMgr);
}

void VtOrderWndHd::SetType(int type)
{
	if (!_OrderConfigMgr)
	{
		AfxMessageBox(_T("주문설정관리자가 만들어지지 않았습니다.!"));
		return;
	}

	_OrderConfigMgr->Type(type);
}


void VtOrderWndHd::OnStnClickedStaticAcntName()
{
	// TODO: Add your control notification handler code here
}


void VtOrderWndHd::OnStnClickedStaticAcnt()
{
	// TODO: Add your control notification handler code here
}

void VtOrderWndHd::OnBnClickedBtnEditFund()
{
	VtFundDesigner* fundDesigner = new VtFundDesigner();
	fundDesigner->SelTab = 1;
	fundDesigner->Create(IDD_FUND_DESIGNER, this);
	fundDesigner->ShowWindow(SW_SHOW);
}


/*
_LayoutMgr->AddWindow(_T("왼쪽창보이기버튼"), IDC_BTN_SHOW_LEFT, GetClientArea(IDC_BTN_SHOW_LEFT));
_LayoutMgr->AddWindow(_T("계좌레이블"), IDC_STATIC_ACNT, GetClientArea(IDC_STATIC_ACNT));
_LayoutMgr->AddWindow(_T("계좌콤보"), IDC_COMBO_ACCOUNT_HD, GetClientArea(IDC_COMBO_ACCOUNT_HD));
_LayoutMgr->AddWindow(_T("계좌조회버튼"), IDC_BTN_GET_ACNT_INFO, GetClientArea(IDC_BTN_GET_ACNT_INFO));
_LayoutMgr->AddWindow(_T("계좌이름"), IDC_STATIC_ACNT_NAME, GetClientArea(IDC_STATIC_ACNT_NAME));
_LayoutMgr->AddWindow(_T("주문창제거버튼"), IDC_BTN_DELWND, GetClientArea(IDC_BTN_DELWND));
_LayoutMgr->AddWindow(_T("주문창추가버튼"), IDC_BTN_ADDWND, GetClientArea(IDC_BTN_ADDWND));
_LayoutMgr->AddWindow(_T("오른쪽창보이기버튼"), IDC_BTN_SHOWRIGHT, GetClientArea(IDC_BTN_SHOWRIGHT));
_LayoutMgr->AddWindow(_T("펀드편집창버튼"), IDC_BTN_EDIT_FUND, GetClientArea(IDC_BTN_EDIT_FUND));
*/
void VtOrderWndHd::CalcLayout()
{
	int left = _ShowLeftWnd ? _LeftWnd.DefaultWidth() : 0;
	int right = left + GetTotalCenterWidth();
	// buttons on the left side
	CRect& rcShowLeft = _LayoutMgr->GetRect(IDC_BTN_SHOW_LEFT);
	CRect& rcAcntLable = _LayoutMgr->GetRect(IDC_STATIC_ACNT);
	CRect& rcAcntCombo = _LayoutMgr->GetRect(IDC_COMBO_ACCOUNT_HD);
	CRect& rcRefreshAcnt = _LayoutMgr->GetRect(IDC_BTN_GET_ACNT_INFO);
	CRect& rcEditPwd = _LayoutMgr->GetRect(IDC_EDIT_PWD);
	CRect& rcAcntName = _LayoutMgr->GetRect(IDC_STATIC_ACNT_NAME);
	CRect& rcFundEdit = _LayoutMgr->GetRect(IDC_BTN_EDIT_FUND);

	// buttons on the right side
	CRect& rcDelWnd = _LayoutMgr->GetRect(IDC_BTN_DELWND);
	CRect& rcAddWnd = _LayoutMgr->GetRect(IDC_BTN_ADDWND);
	CRect& rcShowRight = _LayoutMgr->GetRect(IDC_BTN_SHOWRIGHT);

	int ctrlWidth = rcShowLeft.Width();
	int ctrlHeight = rcShowLeft.Height();

	rcShowLeft.left = left;
	rcShowLeft.right = rcShowLeft.left + ctrlWidth;
	rcShowLeft.top = 0;
	rcShowLeft.bottom = rcShowLeft.top + ctrlHeight;

	int staticY = 5;
	ctrlWidth = rcAcntLable.Width();
	ctrlHeight = rcAcntLable.Height();
	rcAcntLable.left = rcShowLeft.right;
	rcAcntLable.right = rcAcntLable.left +ctrlWidth;
	rcAcntLable.top = staticY;
	rcAcntLable.bottom = rcAcntLable.top + ctrlHeight;

	ctrlWidth = rcAcntCombo.Width();
	ctrlHeight = rcAcntCombo.Height();
	rcAcntCombo.left = rcAcntLable.right;
	rcAcntCombo.right = rcAcntCombo.left + ctrlWidth;
	rcAcntCombo.top = 2;
	rcAcntCombo.bottom = rcAcntCombo.top + ctrlHeight;

	if (_OrderConfigMgr->Type() == 0) {
		int editWidth = rcEditPwd.Width();
		int editHeight = rcEditPwd.Height();
		rcEditPwd.left = rcAcntCombo.right;
		rcEditPwd.right = rcEditPwd.left + editWidth;
		rcEditPwd.top = 2;
		rcEditPwd.bottom = rcEditPwd.top + editHeight;

		ctrlWidth = rcRefreshAcnt.Width();
		ctrlHeight = rcRefreshAcnt.Height();
		rcRefreshAcnt.left = rcEditPwd.right;
		rcRefreshAcnt.right = rcRefreshAcnt.left + ctrlWidth;
		rcRefreshAcnt.top = 0;
		rcRefreshAcnt.bottom = rcRefreshAcnt.top + ctrlHeight;

		ctrlWidth = rcAcntName.Width();
		ctrlHeight = rcAcntName.Height();
		rcAcntName.left = rcRefreshAcnt.right;
		rcAcntName.right = rcAcntName.left + ctrlWidth;
		rcAcntName.top = staticY;
		rcAcntName.bottom = rcAcntName.top + ctrlHeight;
	}
	else {
		ctrlWidth = rcAcntName.Width();
		ctrlHeight = rcAcntName.Height();
		rcAcntName.left = rcAcntCombo.right;
		rcAcntName.right = rcAcntName.left + ctrlWidth;
		rcAcntName.top = staticY;
		rcAcntName.bottom = rcAcntName.top + ctrlHeight;
	}

	int gap = 2;
	ctrlWidth = rcShowRight.Width();
	ctrlHeight = rcShowRight.Height();
	rcShowRight.left = right - ctrlWidth;
	rcShowRight.right = rcShowRight.left + ctrlWidth;
	rcShowRight.top = 0;
	rcShowRight.bottom = rcShowRight.top + ctrlHeight;

	ctrlWidth = rcAddWnd.Width();
	ctrlHeight = rcAddWnd.Height();
	rcAddWnd.left = rcShowRight.left - ctrlWidth - gap;
	rcAddWnd.right = rcAddWnd.left + ctrlWidth;
	rcAddWnd.top = 0;
	rcAddWnd.bottom = rcAddWnd.top + ctrlHeight;

	ctrlWidth = rcDelWnd.Width();
	ctrlHeight = rcDelWnd.Height();
	rcDelWnd.left = rcAddWnd.left - ctrlWidth - gap;
	rcDelWnd.right = rcDelWnd.left + ctrlWidth;
	rcDelWnd.top = 0;
	rcDelWnd.bottom = rcDelWnd.top + ctrlHeight;

	ctrlWidth = rcFundEdit.Width();
	ctrlHeight = rcFundEdit.Height();
	rcFundEdit.left = rcDelWnd.left - ctrlWidth - gap;
	rcFundEdit.right = rcFundEdit.left + ctrlWidth;
	rcFundEdit.top = 0;
	rcFundEdit.bottom = rcFundEdit.top + ctrlHeight;
}

int VtOrderWndHd::GetTotalCenterWidth()
{
	int totalWidth = 0;
	// 중앙창 너비를 모두 합한다.
	// 중앙창은 반드시 모든 옵션이 설정되거나 로드된 상태여야 한다.
	for (auto it = _CenterWndVector.begin(); it != _CenterWndVector.end(); ++it) {
		VtOrderCenterWndHd* centerWnd = *it;
		totalWidth += centerWnd->GetWindowWidth();
	}

	return totalWidth;
}

void VtOrderWndHd::RepositionControl()
{
	CalcLayout();
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

void VtOrderWndHd::RecalChildWindows()
{
	_ChildWindowInfo.clear();

	CRect rcChild;
	rcChild.left = 0;
	rcChild.right = rcChild.left + _LeftWnd.DefaultWidth();
	rcChild.top = 0;
	rcChild.bottom = _WindowHeight;
	_ChildWindowInfo.push_back(std::make_tuple(&_LeftWnd, _ShowLeftWnd, rcChild));

	int left = _ShowLeftWnd ? _LeftWnd.DefaultWidth() : 0;
	int totalCenterWndWidth = 0;
	for (auto it = _CenterWndVector.begin(); it != _CenterWndVector.end(); ++it) {
		VtOrderCenterWndHd* centerWnd = *it;
		totalCenterWndWidth += centerWnd->GetWindowWidth();
		rcChild.top = CenterTop;
		rcChild.bottom = _WindowHeight - CenterTop;
		rcChild.left = left;
		rcChild.right = rcChild.left + centerWnd->GetWindowWidth();
		_ChildWindowInfo.push_back(std::make_tuple(centerWnd, true, rcChild));
		left = rcChild.right;
	}

	rcChild.left = left;
	rcChild.right = rcChild.left + _RightWnd.DefaultWidth();
	rcChild.top = 0;
	rcChild.bottom = _WindowHeight;
	_ChildWindowInfo.push_back(std::make_tuple(&_RightWnd, _ShowRightWnd, rcChild));
}

void VtOrderWndHd::ShowCtrlByType()
{
	if (_OrderConfigMgr->Type() == 0) {
		_BtnGetAcntInfo.ShowWindow(SW_SHOW);
		_StaticAcntName.ShowWindow(SW_SHOW);
		_BtnFundEditor.ShowWindow(SW_HIDE);
	}
	else {
		_BtnGetAcntInfo.ShowWindow(SW_HIDE);
		_StaticAcntName.ShowWindow(SW_HIDE);
		_BtnFundEditor.ShowWindow(SW_SHOW);
	}
}


void VtOrderWndHd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnMouseMove(nFlags, point);
}


void VtOrderWndHd::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnMouseLeave();
}


void VtOrderWndHd::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnMouseHover(nFlags, point);
}



// Mouse and keyBoard Handling Functions
LRESULT VtOrderWndHd::OnMyMouseMove(WPARAM, LPARAM param)
{
	AfxMessageBox(_T("MouseMove"));
	return 1;
}


LRESULT VtOrderWndHd::OnMyMouseLButtonUp(WPARAM, LPARAM param)
{
	AfxMessageBox(_T("MouseMove1"));
	return 1;
}
LRESULT VtOrderWndHd::OnMyMouseLButtonDown(WPARAM, LPARAM param)
{
	AfxMessageBox(_T("MouseMove2"));
	return 1;
}
LRESULT VtOrderWndHd::OnMyMouseRButtonUp(WPARAM, LPARAM param)
{
	AfxMessageBox(_T("MouseMove3"));
	return 1;
}

LRESULT VtOrderWndHd::OnMyMouseRButtonDown(WPARAM, LPARAM param)
{
	AfxMessageBox(_T("MouseMove4"));
	return 1;
}

LRESULT VtOrderWndHd::OnMyMouseLDBClick(WPARAM, LPARAM param)
{
	AfxMessageBox(_T("MouseMove5"));
	return 1;
}

/*
BOOL VtOrderWndHd::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_SYSKEYDOWN) {
		if (pMsg->wParam == VK_MENU) {
			ResetByCenterRow();
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
*/


void VtOrderWndHd::OnEnChangeEditPwd()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (_EditPwd.GetSafeHwnd()) {
		CString pwd;
		_EditPwd.GetWindowText(pwd);
		if (_OrderConfigMgr && _OrderConfigMgr->Account()) {
			_OrderConfigMgr->Account()->Password = (LPCTSTR)pwd;
		}
	}
}
