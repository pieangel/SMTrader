#include "stdafx.h"
#include "RemainOrderWnd.h"
#include "Resource.h"

CRemainOrderWnd::CRemainOrderWnd()
{
}


CRemainOrderWnd::~CRemainOrderWnd()
{
}


BEGIN_MESSAGE_MAP(CRemainOrderWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	//ON_BN_CLICKED(ID_BTN_FILLORDER, &CRemainOrderWnd::OnBnClickedCommandButton)
	//ON_UPDATE_COMMAND_UI(ID_BTN_FILLORDER, OnUpdateClickCommandButton)
END_MESSAGE_MAP()


int CRemainOrderWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here


	CRect rectDummy;
	rectDummy.SetRectEmpty();
	rectDummy.left = commandButtonWidth + 1;
	if (!_GridCtrl.CreateGrid(WS_CHILD | WS_VISIBLE, rectDummy, this, 1))
	{
		TRACE0("그리드를 만들지 못했습니다.\n");
		return -1;
	}

	CRect rectClient;
	GetClientRect(rectClient);
	rectClient.right = commandButtonWidth;
	/*
	if (!_BtnCommand.Create(_T("모두청산"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_OWNERDRAW, rectClient, this, ID_BTN_FILLORDER))
	{
		TRACE0("버튼을 만들지 못했습니다.\n");
		return -1;
	}
	*/
	return 0;
}


void CRemainOrderWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	// Tab 컨트롤은 전체 클라이언트 영역을 처리해야 합니다.
	_GridCtrl.SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	_BtnCommand.SetWindowPos(NULL, -1, -1, commandButtonWidth, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}


void CRemainOrderWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here
	//_GridCtrl.SetFocus();
}

void CRemainOrderWnd::OnBnClickedCommandButton()
{
	AfxMessageBox(_T("Button Clicked!"));
}

void CRemainOrderWnd::OnUpdateClickCommandButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

void CRemainOrderWnd::RefreshOrders()
{
	_GridCtrl.ResetCellText();
	_GridCtrl.SetFilledOrderList();
}

void CRemainOrderWnd::ReceivedRemain()
{
	_GridCtrl.ResetCellText();
	_GridCtrl.SetRemainList();
}

