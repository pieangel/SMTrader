// VtSymbolCatPage1.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtSymbolCatPage.h"
#include "afxdialogex.h"
#include "VtProductCategoryManager.h"
#include "VtProductCategory.h"
#include "VtProductSection.h"
#include "VtSymbol.h"


// VtSymbolCatPage1 dialog

IMPLEMENT_DYNAMIC(VtSymbolCatPage, CDialogEx)

VtSymbolCatPage::VtSymbolCatPage(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SYM_CAT1, pParent)
{

}

VtSymbolCatPage::~VtSymbolCatPage()
{
}

void VtSymbolCatPage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(VtSymbolCatPage, CDialogEx)
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


// VtSymbolCatPage1 message handlers


void VtSymbolCatPage::SetProduct()
{
	CUGCell cell;
	VtProductCategoryManager* catMgr = VtProductCategoryManager::GetInstance();
	VtProductCategory* cat = catMgr->CategoryList[_Section];
	int i = 0;
	for (auto it = cat->SectionList.begin(); it != cat->SectionList.end(); ++it)
	{
		VtProductSection* sec = *it;
		_GridCtrl.QuickSetText(0, i, sec->ExPos.c_str());
		_GridCtrl.QuickSetBackColor(0, i, RGB(240, 240, 240));
		_GridCtrl.QuickSetText(1, i, sec->Name.c_str());
		_GridCtrl.QuickSetBackColor(1, i, RGB(240, 240, 240));
		_GridCtrl.QuickSetText(2, i, sec->Code.c_str());
		_GridCtrl.QuickSetBackColor(2, i, RGB(240, 240, 240));
		_GridCtrl.QuickSetBorderColor(2, i, RGB(0, 0, 0));
		int j = 3;
		for (auto itcol = sec->SymbolList.begin(); itcol != sec->SymbolList.end(); ++itcol)
		{
			try
			{
				VtSymbol* sym = *itcol;
				//TRACE(sym->ShortName.c_str());
				//TRACE(_T("\n"));
				_GridCtrl.QuickSetText(j, i, sym->ShortName.c_str());
				_GridCtrl.GetCell(j, i, &cell);
				cell.Tag((void*)sym);
				//cell.UserMessage(sym->ShortCode.c_str());
				if (j == 3)
					cell.SetBackColor(RGB(255, 255, 0));
				_GridCtrl.SetCell(j, i, &cell);
			}
			catch (std::exception ex)
			{
				std::string msg = ex.what();

			}
			j++;
		}
		i++;
	}
}

void VtSymbolCatPage::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (_GridCtrl.GetSafeHwnd())
	{
		_GridCtrl.MoveWindow(0, 0, cx, cy);
	}
}


void VtSymbolCatPage::OnSetFocus(CWnd* pOldWnd)
{
	CDialogEx::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here
	_GridCtrl.SetFocus();
}


BOOL VtSymbolCatPage::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	_GridCtrl.CreateGrid(WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 0x12345678);

	//SetProduct();
	_GridCtrl.Section(_Section);
	_GridCtrl.SetProduct();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void VtSymbolCatPage::SetOrderCenterWindow(CVtOrderCenterWnd* centerWnd)
{
	_GridCtrl.OrderCenterWnd(centerWnd);
}

void VtSymbolCatPage::SetSymbolSelector(VtSymbolSelector* symSelector)
{
	_GridCtrl.SymSelector(symSelector);
}

void VtSymbolCatPage::SetSource(int src)
{
	_GridCtrl.SetSource(src);
}

void VtSymbolCatPage::SetFavoriteDlg(CVtSymbolFavoriteDlg* favoriteDlg)
{
	_GridCtrl.FavoriteDlg(favoriteDlg);
}
