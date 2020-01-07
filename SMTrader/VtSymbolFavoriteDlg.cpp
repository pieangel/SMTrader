// VtSymbolFavoriteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtSymbolFavoriteDlg.h"
#include "afxdialogex.h"
#include "VtProductCategoryManager.h"
#include "VtProductCategory.h"
#include "VtGlobal.h"
#include "SmOrderPanelOut.h"
// CVtSymbolFavoriteDlg dialog

IMPLEMENT_DYNAMIC(CVtSymbolFavoriteDlg, CDialogEx)

CVtSymbolFavoriteDlg::CVtSymbolFavoriteDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SYMBOL_FAVORITE, pParent)
{
	_SrcSymbol = nullptr;
	_DstSymbol = nullptr;
}

CVtSymbolFavoriteDlg::CVtSymbolFavoriteDlg(VtOrderConfigManager* orderConfigMgr, CWnd* pParent /*= NULL*/)
{
	_OrderConfigMgr = orderConfigMgr;
	_FavoriteGrid.SetOrderConfigMgr(orderConfigMgr);
}

CVtSymbolFavoriteDlg::~CVtSymbolFavoriteDlg()
{
}

void CVtSymbolFavoriteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_SYMBOL, _TabCtrl);
}


void CVtSymbolFavoriteDlg::InitTabCtrl()
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
	_Page2.Create(IDD_SYM_CAT1, &_TabCtrl);
	_Page2.SetWindowPos(nullptr, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);

	_Page3.Create(IDD_SYM_CAT1, &_TabCtrl);
	_Page3.SetWindowPos(nullptr, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);

	_Page4.Create(IDD_SYM_CAT1, &_TabCtrl);
	_Page4.SetWindowPos(nullptr, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);

	_Page5.Create(IDD_SYM_CAT1, &_TabCtrl);
	_Page5.SetWindowPos(nullptr, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);

	_Page6.Create(IDD_SYM_CAT1, &_TabCtrl);
	_Page6.SetWindowPos(nullptr, 5, 25, rect.Width() - 10, rect.Height() - 30, SWP_NOZORDER);

	_CurrentPage->ShowWindow(SW_SHOW);

	_Page1.SetSource(1);
	_Page2.SetSource(1);
	_Page3.SetSource(1);
	_Page4.SetSource(1);
	_Page5.SetSource(1);
	_Page6.SetSource(1);

	_Page1.SetFavoriteDlg(this);
	_Page2.SetFavoriteDlg(this);
	_Page3.SetFavoriteDlg(this);
	_Page4.SetFavoriteDlg(this);
	_Page5.SetFavoriteDlg(this);
	_Page6.SetFavoriteDlg(this);
}

void CVtSymbolFavoriteDlg::InitFavorite()
{
	_FavoriteGrid.SetFavorite();
}

BEGIN_MESSAGE_MAP(CVtSymbolFavoriteDlg, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SYMBOL, &CVtSymbolFavoriteDlg::OnTcnSelchangeTabCategory)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_IN_ONE, &CVtSymbolFavoriteDlg::OnBnClickedBtnInOne)
	ON_BN_CLICKED(IDC_BTN_OUT_ONE, &CVtSymbolFavoriteDlg::OnBnClickedBtnOutOne)
	ON_BN_CLICKED(IDC_BTN_OUT_ALL, &CVtSymbolFavoriteDlg::OnBnClickedBtnOutAll)
	ON_WM_CLOSE()
	ON_BN_CLICKED(ID_OK, &CVtSymbolFavoriteDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CVtSymbolFavoriteDlg message handlers


BOOL CVtSymbolFavoriteDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	_FavoriteGrid.AttachGrid(this, IDC_STATIC_FAVORITE);
	_FavoriteGrid.SymFvrtDlg(this);
	_Page1.Section(0);
	_Page2.Section(1);
	_Page3.Section(2);
	_Page4.Section(3);
	_Page5.Section(4);
	_Page6.Section(5);
	InitTabCtrl();

	InitFavorite();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CVtSymbolFavoriteDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (_TabCtrl.GetSafeHwnd())
	{
		// Tab 컨트롤은 전체 클라이언트 영역을 처리해야 합니다.
		_TabCtrl.SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	}
}


void CVtSymbolFavoriteDlg::AddFavorite(VtSymbol* symbol)
{
	if (!symbol)
		return;

	VtGlobal* global = VtGlobal::GetInstance();
	_SrcSymbol = symbol;
	global->AddSymbol(symbol);
	_FavoriteGrid.SetFavorite();
}

void CVtSymbolFavoriteDlg::OnTcnSelchangeTabCategory(NMHDR *pNMHDR, LRESULT *pResult)
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
	_CurrentPage->ShowWindow(SW_SHOW);
	*pResult = 0;
}




void CVtSymbolFavoriteDlg::OnBnClickedBtnInOne()
{
	// TODO: Add your control notification handler code here
	if (_SrcSymbol)
	{
		VtGlobal* global = VtGlobal::GetInstance();
		global->AddSymbol(_SrcSymbol);
		_FavoriteGrid.SetFavorite();
	}
}


void CVtSymbolFavoriteDlg::OnBnClickedBtnOutOne()
{
	// TODO: Add your control notification handler code here
	if (_DstSymbol)
	{
		VtGlobal* global = VtGlobal::GetInstance();
		global->RemoveSymbol(_DstSymbol);
		_FavoriteGrid.SetFavorite();
	}
}


void CVtSymbolFavoriteDlg::OnBnClickedBtnOutAll()
{
	// TODO: Add your control notification handler code here
	VtGlobal* global = VtGlobal::GetInstance();
	global->ClearSymbol();
	_FavoriteGrid.SetFavorite();
}


void CVtSymbolFavoriteDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	CVtOrderCenterWnd* wnd = (CVtOrderCenterWnd*)GetParent();
	CDialogEx::OnClose();
}


void CVtSymbolFavoriteDlg::OnBnClickedBtnCloseFv()
{
	// TODO: Add your control notification handler code here
}


void CVtSymbolFavoriteDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	EndDialog(ID_OK);
}
