// VtRealTickWnd.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtRealTickWnd.h"
#include "afxdialogex.h"
#include "VtSymbol.h"


// VtRealTickWnd dialog

IMPLEMENT_DYNAMIC(VtRealTickWnd, CDialogEx)

/*
VtRealTickWnd::VtRealTickWnd(CWnd* pParent )
	: CDialogEx(IDD_REAL_TICK, pParent)
{

}
*/

int VtRealTickWnd::CRHGetDialogID()
{
	return (IDD_REAL_TICK);
}

VtRealTickWnd::VtRealTickWnd()
{
	_OrderConfigMgr = nullptr;
}

VtRealTickWnd::~VtRealTickWnd()
{
}

void VtRealTickWnd::OrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
	_TickGrid.SetOrderConfigMgr(val);
}

void VtRealTickWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(VtRealTickWnd, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// VtRealTickWnd message handlers


BOOL VtRealTickWnd::OnInitDialog()
{
	CRHGenericChildDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	_TickGrid.AttachGrid(this, IDC_STATIC_REAL_TICK);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void VtRealTickWnd::SetMaxTickCount(int count)
{
	_TickGrid.MaxRow(count);
}

void VtRealTickWnd::OnReceiveQuote(VtSymbol* sym)
{
	_TickGrid.OnReceiveQuote(sym);
}

void VtRealTickWnd::ClearValues()
{
	_TickGrid.ClearValues();
}


void VtRealTickWnd::OnSize(UINT nType, int cx, int cy)
{
	CRHGenericChildDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (_TickGrid.GetSafeHwnd())
		_TickGrid.MoveWindow(0, 0, cx, cy);
}
