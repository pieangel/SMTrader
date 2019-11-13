// SmOrderPanel.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "SmOrderPanel.h"
#include "afxdialogex.h"
#include "SmCallbackManager.h"
#include <functional>
using namespace std;
using namespace std::placeholders;


// SmOrderPanel dialog

IMPLEMENT_DYNAMIC(SmOrderPanel, CDialogEx)

SmOrderPanel::SmOrderPanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ORDER_PANEL, pParent)
{

}

SmOrderPanel::~SmOrderPanel()
{
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
}


BEGIN_MESSAGE_MAP(SmOrderPanel, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// SmOrderPanel message handlers

BOOL SmOrderPanel::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_Grid.Init();
	SetTimer(1, 10, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void SmTest::OnSise(VtSymbol* symbol)
{

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

}

void SmOrderPanel::ChangeFocus()
{

}

int SmOrderPanel::GetWindowWidth()
{
	return 800;
}

void SmOrderPanel::RefreshLayout(bool flag)
{

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

}

void SmOrderPanel::ResizeOrderGrid(int maxHeight)
{

}

void SmOrderPanel::ResizeOrderGrid()
{

}

int SmOrderPanel::GetCountOrderGridEnabledCol()
{
	return 1;
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
