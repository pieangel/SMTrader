// VtStrategyToolWnd.cpp : implementation file
//

#include "stdafx.h"
#include "VtStrategyToolWnd.h"
#include "afxdialogex.h"
#include "resource.h"
#include "HdWindowManager.h"
#include "Poco/Delegate.h"
#include "VtGlobal.h"
using Poco::Delegate;

// VtStrategyToolWnd dialog

IMPLEMENT_DYNAMIC(VtStrategyToolWnd, CDialogEx)

VtStrategyToolWnd::VtStrategyToolWnd(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_STRATEGE_TOOLS, pParent)
{

}

VtStrategyToolWnd::~VtStrategyToolWnd()
{
	HdWindowManager* wndMgr = HdWindowManager::GetInstance();
	_WindowEvent -= delegate(wndMgr, &HdWindowManager::OnWindowEvent);
}

void VtStrategyToolWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(VtStrategyToolWnd, CDialogEx)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// VtStrategyToolWnd message handlers


void VtStrategyToolWnd::UpdateSystem(VtSystem* sys, bool enable)
{
	if (!sys)
		return;
	_GridCtrl.UpdateSystem(sys, enable);
}

BOOL VtStrategyToolWnd::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	this->SetIcon(hIcon, FALSE);

	_GridCtrl.ToolWnd(this);
	_GridCtrl.AttachGrid(this, IDC_STATIC_STRATEGY);

	HdWindowManager* wndMgr = HdWindowManager::GetInstance();
	_WindowEvent += delegate(wndMgr, &HdWindowManager::OnWindowEvent);

	HdWindowEventArgs arg;
	arg.pWnd = this;
	arg.wndType = HdWindowType::StrategyToolWindow;
	arg.eventType = HdWindowEventType::Created;
	FireWindowEvent(std::move(arg));

	VtGlobal::StrategyToolWnd = this;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void VtStrategyToolWnd::OnClose()
{
	_GridCtrl.DisableAllSystems();
	// TODO: Add your message handler code here and/or call default
	CDialogEx::OnClose();

	HdWindowEventArgs arg;
	arg.pWnd = this;
	arg.wndType = HdWindowType::StrategyToolWindow;
	arg.eventType = HdWindowEventType::Closed;
	FireWindowEvent(std::move(arg));
	VtGlobal::StrategyToolWnd = nullptr;
}
