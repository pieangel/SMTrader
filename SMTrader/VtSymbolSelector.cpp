// VtSymbolSelector.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtSymbolSelector.h"
#include "afxdialogex.h"
#include "VtProductCategoryManager.h"
#include "VtProductCategory.h"


// VtSymbolSelector dialog

IMPLEMENT_DYNAMIC(VtSymbolSelector, CDialogEx)

VtSymbolSelector::VtSymbolSelector(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SYMBOL_SELECTOR, pParent)
{
	_CurrentPage = nullptr;
}

VtSymbolSelector::~VtSymbolSelector()
{
}

void VtSymbolSelector::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_CATEGORY, _TabCtrl);
}


BEGIN_MESSAGE_MAP(VtSymbolSelector, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CATEGORY, &VtSymbolSelector::OnTcnSelchangeTabCategory)
	ON_WM_SIZE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// VtSymbolSelector message handlers


void VtSymbolSelector::OnTcnSelchangeTabCategory(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	if (_CurrentPage != nullptr)
	{
		_CurrentPage->ShowWindow(SW_HIDE);
		_CurrentPage = nullptr;
	}

	int curIndex = _TabCtrl.GetCurSel();
	switch (curIndex)
	{
	case 0:
		_Page1.ShowWindow(SW_SHOW);
		_CurrentPage = &_Page1;
		break;
	case 1:
		_Page2.ShowWindow(SW_SHOW);
		_CurrentPage = &_Page2;
		break;
	case 2:
		_Page3.ShowWindow(SW_SHOW);
		_CurrentPage = &_Page3;
		break;
	case 3:
		_Page4.ShowWindow(SW_SHOW);
		_CurrentPage = &_Page4;
		break;
	case 4:
		_Page5.ShowWindow(SW_SHOW);
		_CurrentPage = &_Page5;
		break;
	case 5:
		_Page6.ShowWindow(SW_SHOW);
		_CurrentPage = &_Page6;
		break;
	}

	*pResult = 0;
}


BOOL VtSymbolSelector::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	this->SetIcon(hIcon, FALSE);

	// TODO:  Add extra initialization here
	_Page1.Section(0);
	_Page2.Section(1);
	_Page3.Section(2);
	_Page4.Section(3);
	_Page5.Section(4);
	_Page6.Section(5);
	InitTabCtrl();
	SetSymbolSelector();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void VtSymbolSelector::InitTabCtrl()
{
	VtProductCategoryManager* proMgr = VtProductCategoryManager::GetInstance();
	int i = 0;
	for (auto it = proMgr->CategoryList.begin(); it != proMgr->CategoryList.end(); ++it)
	{
		VtProductCategory* cat = *it;
		_TabCtrl.InsertItem(i++, cat->Name.c_str());
	}

	CRect rect;
	_TabCtrl.GetClientRect(rect);

	_Page1.Create(IDD_SYM_CAT1, &_TabCtrl);
	_Page1.SetWindowPos(nullptr, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);
	_CurrentPage = &_Page1;

	_Page2.Create(IDD_SYM_CAT2, &_TabCtrl);
	_Page2.SetWindowPos(nullptr, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);

	_Page3.Create(IDD_SYM_CAT3, &_TabCtrl);
	_Page3.SetWindowPos(nullptr, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);

	_Page4.Create(IDD_SYM_CAT4, &_TabCtrl);
	_Page4.SetWindowPos(nullptr, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);

	_Page5.Create(IDD_SYM_CAT5, &_TabCtrl);
	_Page5.SetWindowPos(nullptr, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);

	_Page6.Create(IDD_SYM_CAT6, &_TabCtrl);
	_Page6.SetWindowPos(nullptr, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);

	_CurrentPage->ShowWindow(SW_SHOW);
}


void VtSymbolSelector::SetSymbolSelector()
{
	_Page1.SetSymbolSelector(this);
	_Page2.SetSymbolSelector(this);
	_Page3.SetSymbolSelector(this);
	_Page4.SetSymbolSelector(this);
	_Page5.SetSymbolSelector(this);
	_Page6.SetSymbolSelector(this);
}

void VtSymbolSelector::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	
	if (_TabCtrl.GetSafeHwnd())
	{
		// Tab 컨트롤은 전체 클라이언트 영역을 처리해야 합니다.
		_TabCtrl.SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	}
	
}

void VtSymbolSelector::SetOrderCenterWindow(CVtOrderCenterWnd* centerWnd)
{
	_Page1.SetOrderCenterWindow(centerWnd);
	_Page2.SetOrderCenterWindow(centerWnd);
	_Page3.SetOrderCenterWindow(centerWnd);
	_Page4.SetOrderCenterWindow(centerWnd);
	_Page5.SetOrderCenterWindow(centerWnd);
	_Page6.SetOrderCenterWindow(centerWnd);

}


void VtSymbolSelector::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	//ShowWindow(SW_HIDE);
	CDialogEx::OnClose();
}
