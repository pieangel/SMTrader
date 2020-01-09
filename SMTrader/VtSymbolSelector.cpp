// VtSymbolSelector.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtSymbolSelector.h"
#include "afxdialogex.h"
#include "VtProductCategoryManager.h"
#include "VtProductCategory.h"
#include "Market/SmMarketManager.h"
#include "Market/SmMarket.h"
#include "Market/SmProduct.h"

// VtSymbolSelector dialog

IMPLEMENT_DYNAMIC(VtSymbolSelector, CDialogEx)

VtSymbolSelector::VtSymbolSelector(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SYMBOL_SELECTOR, pParent)
{
	_CurrentPage = nullptr;
}

VtSymbolSelector::~VtSymbolSelector()
{
	if (_PageVector.size() > 0) {
		for (auto it = _PageVector.begin(); it != _PageVector.end(); ++it) {
			(*it)->DestroyWindow();
			delete *it;
		}
	}
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
	for (size_t i = 0; i < _PageVector.size(); ++i) {
		if (i == curIndex) {
			_PageVector[curIndex]->ShowWindow(SW_SHOW);
			_CurrentPage = _PageVector[curIndex];
		}
		else {
			_PageVector[i]->ShowWindow(SW_HIDE);
		}
	}

	*pResult = 0;
}


BOOL VtSymbolSelector::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	this->SetIcon(hIcon, FALSE);
	InitTabCtrl();
	SetSymbolSelector();
	Resize();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void VtSymbolSelector::InitTabCtrl()
{
	CRect rect;
	_TabCtrl.GetClientRect(rect);
	std::vector<SmMarket*> market_list = SmMarketManager::GetInstance()->GetAbroadMarketList();
	for (size_t i = 0; i < market_list.size(); ++i)
	{
		SmMarket* cat = market_list[i];
		_TabCtrl.InsertItem(i, cat->Name().c_str());
		VtSymbolCatPage* page = new VtSymbolCatPage();
		page->Section(i);
		page->Create(IDD_SYM_CAT1, &_TabCtrl);
		page->SetWindowPos(nullptr, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);
		_PageVector.push_back(page);
	}
	_CurrentPage = _PageVector[0];
	_CurrentPage->ShowWindow(SW_SHOW);
}


void VtSymbolSelector::SetSymbolSelector()
{
	for (size_t i = 0; i < _PageVector.size(); ++i) {
		_PageVector[i]->SetSymbolSelector(this);
	}
}

void VtSymbolSelector::Resize()
{
	CRect rcClient;
	GetClientRect(rcClient);
	if (_TabCtrl.GetSafeHwnd())
	{
		// Tab 컨트롤은 전체 클라이언트 영역을 처리해야 합니다.
		_TabCtrl.MoveWindow(0, 0, rcClient.Width(), rcClient.Height());
	}

	for (size_t i = 0; i < _PageVector.size(); ++i) {
		_PageVector[i]->Resize();
	}
}

void VtSymbolSelector::Resize(int width, int height)
{
	if (_TabCtrl.GetSafeHwnd())
	{
		// Tab 컨트롤은 전체 클라이언트 영역을 처리해야 합니다.
		_TabCtrl.MoveWindow(0, 0, width, height);
	}

	for (size_t i = 0; i < _PageVector.size(); ++i) {
		_PageVector[i]->Resize(width, height);
	}
}

void VtSymbolSelector::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	
	Resize(cx, cy);
	
}

void VtSymbolSelector::SetOrderCenterWindow(CVtOrderCenterWnd* centerWnd)
{
	for (size_t i = 0; i < _PageVector.size(); ++i) {
		_PageVector[i]->SetOrderCenterWindow(centerWnd);
	}
}


void VtSymbolSelector::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	//ShowWindow(SW_HIDE);
	CDialogEx::OnClose();
}


void VtSymbolSelector::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();
	delete this;
}
