// VtOrderRightWndHd.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtOrderRightWndHd.h"
#include "afxdialogex.h"
#include "VtOrderConfigManager.h"
#include "VtGlobal.h"

extern TApplicationFont g_Font;

VtOrderRightWndHd::VtOrderRightWndHd()
{

}

// VtOrderRightWndHd dialog

IMPLEMENT_DYNAMIC(VtOrderRightWndHd, CDialogEx)
/*
VtOrderRightWndHd::VtOrderRightWndHd(CWnd* pParent )
	: CDialogEx(IDD_ORDER_RIGHT_HD, pParent)
{

}
*/
VtOrderRightWndHd::~VtOrderRightWndHd()
{
	_DefaultWidth = VtGlobal::GetHorWidthByScaleFactor(167);
	_DefaultHeight = VtGlobal::GetVerHeightByScaleFactor(561);
}

int VtOrderRightWndHd::CRHGetDialogID()
{
	return (IDD_ORDER_RIGHT_HD);
}

void VtOrderRightWndHd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_CONFIG, _TabCtrl);
}


BEGIN_MESSAGE_MAP(VtOrderRightWndHd, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CONFIG, &VtOrderRightWndHd::OnTcnSelchangeTabConfig)
END_MESSAGE_MAP()


// VtOrderRightWndHd message handlers


BOOL VtOrderRightWndHd::OnInitDialog()
{
	CRHGenericChildDialog::OnInitDialog();

	::EnumChildWindows(m_hWnd, ::SetChildFont, (LPARAM)g_Font.GetFont());

	InitTabCtrl();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void VtOrderRightWndHd::InitTabCtrl()
{
	_TabCtrl.InsertItem(0, _T("설정"));
	_TabCtrl.InsertItem(1, _T("취소"));

	CRect rect;
	_TabCtrl.GetClientRect(rect);

	_ConfigPage.Create(IDD_ORDER_RIGHT_CONFIG_HD, &_TabCtrl);
	_ConfigPage.OrderConfigMgr(_OrderConfigMgr);
	_ConfigPage.SetWindowPos(nullptr, 0, 25, rect.Width(), rect.Height() - 30, SWP_NOZORDER);
	_CurrentPage = &_ConfigPage;

	_CancelPage.Create(IDD_ORDER_RIGHT_CANCEL_HD, &_TabCtrl);
	_CancelPage.OrderConfigMgr(_OrderConfigMgr);
	_CancelPage.SetWindowPos(nullptr, 0, 25, rect.Width(), rect.Height() - 30, SWP_NOZORDER);
	_CancelPage.ShowWindow(SW_HIDE);

	_CurrentPage->ShowWindow(SW_SHOW);
}


void VtOrderRightWndHd::OnSize(UINT nType, int cx, int cy)
{
	CRHGenericChildDialog::OnSize(nType, cx, cy);

	if (_TabCtrl.GetSafeHwnd())
	{
		// Tab 컨트롤은 전체 클라이언트 영역을 처리해야 합니다.
		_TabCtrl.SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

		CRect rect;
		_TabCtrl.GetClientRect(rect);

		_ConfigPage.SetWindowPos(nullptr, 0, 25, rect.Width(), rect.Height() - 30, SWP_NOZORDER);

		_CancelPage.SetWindowPos(nullptr, 0, 25, rect.Width(), rect.Height() - 30, SWP_NOZORDER);
	}
}


void VtOrderRightWndHd::OnTcnSelchangeTabConfig(NMHDR *pNMHDR, LRESULT *pResult)
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
		_ConfigPage.ShowWindow(SW_SHOW);
		_CurrentPage = &_ConfigPage;
		break;
	case 1:
		_CancelPage.ShowWindow(SW_SHOW);
		_CurrentPage = &_CancelPage;
		break;
	}

	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void VtOrderRightWndHd::InitCtrl()
{
	_ConfigPage.OrderConfigMgr(_OrderConfigMgr);
	_CancelPage.OrderConfigMgr(_OrderConfigMgr);
	_ConfigPage.InitCtrl();
}
