// VtFundMiniJango.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtFundMiniJango.h"
#include "afxdialogex.h"
#include "VtFundManager.h"
#include "VtFund.h"
#include "HdWindowManager.h"
#include "Poco/Delegate.h"
using Poco::Delegate;
// VtFundMiniJango dialog

IMPLEMENT_DYNAMIC(VtFundMiniJango, CDialogEx)

VtFundMiniJango::VtFundMiniJango(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MINI_JANGO_FUND, pParent)
{
	_Fund = nullptr;
}

VtFundMiniJango::~VtFundMiniJango()
{
	HdWindowManager* wndMgr = HdWindowManager::GetInstance();
	_WindowEvent -= delegate(wndMgr, &HdWindowManager::OnWindowEvent);
}

void VtFundMiniJango::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FUND, _ComboFund);
	DDX_Control(pDX, IDC_CHECK_ACNT, _CheckMode);
}


BEGIN_MESSAGE_MAP(VtFundMiniJango, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_COMBO_FUND, &VtFundMiniJango::OnCbnSelchangeComboFund)
	ON_BN_CLICKED(IDC_CHECK_SUBACNT, &VtFundMiniJango::OnBnClickedCheckSubacnt)
	ON_BN_CLICKED(IDC_CHECK_ACNT, &VtFundMiniJango::OnBnClickedCheckAcnt)
END_MESSAGE_MAP()


// VtFundMiniJango message handlers


void VtFundMiniJango::OnFundAdded()
{
	CString str1;
	int selAcnt = _ComboFund.GetCurSel();
	if (selAcnt != -1) {
		_ComboFund.GetLBText(selAcnt, str1);

		VtFundManager* fundMgr = VtFundManager::GetInstance();
		std::map<std::string, VtFund*>& fundList = fundMgr->GetFundList();

		_ComboFund.ResetContent();
		for (auto it = fundList.begin(); it != fundList.end(); ++it) {
			VtFund* fund = it->second;
			int index = _ComboFund.AddString(fund->Name.c_str());
			_ComboFund.SetItemDataPtr(index, fund);
		}

		int index = _ComboFund.FindString(0, str1);
		int selIndex = 0;
		if (index == CB_ERR) {
			_ComboFund.SetCurSel(selIndex);
		} else {
			_ComboFund.SetCurSel(index);
			selIndex = index;
		}
	} else {
		InitFund();
	}
}

void VtFundMiniJango::OnFundDeleted(VtFund* fund)
{
	if (fund == _Fund) {
		_Fund = nullptr;
		_ProductGrid.ClearCells();
	}

	CString str1;
	int selAcnt = _ComboFund.GetCurSel();
	if (selAcnt != -1) {
		_ComboFund.GetLBText(selAcnt, str1);

		VtFundManager* fundMgr = VtFundManager::GetInstance();
		std::map<std::string, VtFund*>& fundList = fundMgr->GetFundList();
		if (fundList.size() == 0) {
			//AfxMessageBox(_T("펀드가 만들어지지 않았습니다. 최소한 하나의 펀드는 존재해야 합니다."));
			_ComboFund.ResetContent();
			_Fund = nullptr;
			return;
		}
		_ComboFund.ResetContent();
		for (auto it = fundList.begin(); it != fundList.end(); ++it) {
			VtFund* fund = it->second;
			int index = _ComboFund.AddString(fund->Name.c_str());
			_ComboFund.SetItemDataPtr(index, fund);
		}

		int index = _ComboFund.FindString(0, str1);
		int selIndex = 0;
		if (index == CB_ERR) {
			_ComboFund.SetCurSel(selIndex);
		} else {
			_ComboFund.SetCurSel(index);
			selIndex = index;
		}
	} else {
		InitFund();
	}
}

void VtFundMiniJango::InitFund()
{
	_ComboFund.ResetContent();

	VtFundManager* fundMgr = VtFundManager::GetInstance();
	std::map<std::string, VtFund*>& fundList = fundMgr->GetFundList();
	if (fundList.size() == 0) {
		//AfxMessageBox(_T("펀드가 만들어지지 않았습니다. 최소한 하나의 펀드는 존재해야 합니다."));
		_ComboFund.ResetContent();
		_Fund = nullptr;
		return;
	}
	for (auto it = fundList.begin(); it != fundList.end(); ++it) {
		VtFund* fund = it->second;
		int index = _ComboFund.AddString(fund->Name.c_str());
		if (index == 0)
			_Fund = fund;
		_ComboFund.SetItemDataPtr(index, fund);
	}

	_ComboFund.SetCurSel(0);
}

BOOL VtFundMiniJango::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	this->SetIcon(hIcon, FALSE);

	InitFund();

	_FundPLGrid.FundPLDlg(this);
	_ProductGrid.FundPLDlg(this);
	_FundPLGrid.AttachGrid(this, IDC_STATIC_MINI_TOP);
	_ProductGrid.AttachGrid(this, IDC_STATIC_MINI_SUB);

	// TODO:  Add extra initialization here
	HdWindowManager* wndMgr = HdWindowManager::GetInstance();
	_WindowEvent += delegate(wndMgr, &HdWindowManager::OnWindowEvent);

	HdWindowEventArgs arg;
	arg.pWnd = this;
	arg.wndType = HdWindowType::FundMiniJangoWindow;
	arg.eventType = HdWindowEventType::Created;
	FireWindowEvent(std::move(arg));

	CRect rcWnd;
	GetWindowRect(rcWnd);
	SetWindowPos(nullptr, 0, 0, 215, rcWnd.Height(), SWP_NOMOVE);

	_ComboFund.SetDroppedWidth(150);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void VtFundMiniJango::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnClose();

	HdWindowEventArgs arg;
	arg.pWnd = this;
	arg.wndType = HdWindowType::FundMiniJangoWindow;
	arg.eventType = HdWindowEventType::Closed;
	FireWindowEvent(std::move(arg));
}


void VtFundMiniJango::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}


void VtFundMiniJango::OnCbnSelchangeComboFund()
{
	// TODO: Add your control notification handler code here
	int curSel = _ComboFund.GetCurSel();
	if (curSel != -1) {
		_Fund = (VtFund*)_ComboFund.GetItemDataPtr(curSel);
		_FundPLGrid.InitGrid();
		//_ProductGrid.ClearCells();
		_ProductGrid.InitGrid();
	}
}

void VtFundMiniJango::OnReceiveQuote(VtSymbol* sym)
{
	_FundPLGrid.InitGrid();
	_ProductGrid.InitGrid();
}

void VtFundMiniJango::OnOrderFilledHd(VtOrder* order)
{
	_FundPLGrid.InitGrid();
	//_ProductGrid.ClearCells();
	_ProductGrid.InitGrid();
}

void VtFundMiniJango::OnSymbolMaster(VtSymbol* sym)
{

}

void VtFundMiniJango::OnReceiveAccountInfo()
{
	_FundPLGrid.InitGrid();
	_ProductGrid.InitGrid();
}


void VtFundMiniJango::OnBnClickedCheckSubacnt()
{
	// TODO: Add your control notification handler code here
}


void VtFundMiniJango::OnBnClickedCheckAcnt()
{
	// TODO: Add your control notification handler code here
	if (_CheckMode.GetCheck() == BST_CHECKED) {
		_ProductGrid.Mode(1);
		_ProductGrid.SetColTitle(1);
	} else {
		_ProductGrid.Mode(0);
		_ProductGrid.SetColTitle(0);
	}
	_ProductGrid.InitGrid();
	_ProductGrid.RedrawAll();
}
