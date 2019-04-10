// VtOrderCenterWnd.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtOrderCenterWnd.h"
#include "afxdialogex.h"
#include "VtAccountManager.h"
#include "VtAccount.h"
#include "VtSymbolManager.h"
#include "VtSymbol.h"
#include "VtRealtimeRegisterManager.h"
#include "VtTaskManager.h"
#include "VtOrderRightWnd.h"
#include "VtPosition.h"
#include "VtOrderWnd.h"
#include "VtOrderDialogManager.h"
#include "VtSymbolSelector.h"
#include "VtSymbolFavoriteDlg.h"
#include "DeferPos.h"
#include "VtOrder.h"
#include "VtOrderManager.h"
#include "VtProductOrderManager.h"
#include "VtGlobal.h"
#include "VtProductOrderManagerSelector.h"
#include "VtProductOrderManager.h"
#include "VtQuoteManager.h"
#include "VtQuote.h"
#include "VtHogaManager.h"
#include "VtHoga.h"

CVtOrderCenterWnd::CVtOrderCenterWnd()
{
	_ParentOrderWnd = nullptr;
	_LeftWnd = nullptr;
	_RightWnd = nullptr;
	_StartProductIndex = 58;
	_Account = nullptr;
	_Symbol = nullptr;
	SFont.CreateFont(10, 0, 0, 0, FW_REGULAR, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, _T("굴림"));
}

// CVtOrderCenterWnd dialog

IMPLEMENT_DYNAMIC(CVtOrderCenterWnd, CDialog)

/*
CVtOrderCenterWnd::CVtOrderCenterWnd(CWnd* pParent)
	: CDialogEx(IDD_ORDER_CENTER, pParent)
{

}
*/



CVtOrderCenterWnd::~CVtOrderCenterWnd()
{
	if (_RefreshManger) {
		delete _RefreshManger;
		_RefreshManger = nullptr;
	}
	if (_SymSelector)
	{
		delete _SymSelector;
		_SymSelector = nullptr;
	}
}

int CVtOrderCenterWnd::CRHGetDialogID()
{
	return (IDD_ORDER_CENTER);
}

void CVtOrderCenterWnd::SetOrderConfigMgr(VtOrderConfigManager* val)
{
	_OrderConfigMgr = val;
	_OrderPanelGrid.SetOrderConfigMgr(val);
	_ProductRemainGrid.SetOrderConfigMgr(val);
}

void CVtOrderCenterWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ACCOUNT5, _ComboAccount);
	DDX_Control(pDX, IDC_COMBO_PRODUCT5, _ComboProduct);
	DDX_Control(pDX, IDC_STATIC_PRODUCT_NAME, _ProductName);
	DDX_Control(pDX, IDC_EDIT_ORDER_AMOUNT, _EditOrderAmount);
	DDX_Control(pDX, IDC_EDIT_STOPVAL, _EditStopVal);
	DDX_Control(pDX, IDC_SPIN_ORDER_AMOUNT, _ScOrderAmount);
	DDX_Control(pDX, IDC_SPIN_STOPVAL, _ScStopVal);
	DDX_Control(pDX, IDC_STATIC_ORDER_AVBL, _RemainAmount);
	DDX_Control(pDX, IDC_STATIC_REMAIN, _OrderableAmount);
	DDX_Control(pDX, IDC_EDIT_ACNT_PWD, _AccountPwd);
	DDX_Control(pDX, IDC_BTN_FV1, _BtnFv1);
	DDX_Control(pDX, IDC_BTN_FV2, _BtnFv2);
	DDX_Control(pDX, IDC_BTN_FV3, _BtnFv3);
	DDX_Control(pDX, IDC_BTN_FV4, _BtnFv4);
	DDX_Control(pDX, IDC_BTN_FV5, _BtnFv5);
	DDX_Control(pDX, IDC_BTN_FV6, _BtnFv6);
	DDX_Control(pDX, IDC_BTN_FV7, _BtnFv7);
	DDX_Control(pDX, IDC_BTN_FV8, _BtnFv8);
	DDX_Control(pDX, IDC_BTN_LEFT_EXTEND, _BtnLeftExtend);
	DDX_Control(pDX, IDC_BTN_RIGHT_EXTEND, _BtnRightExtend);
}


BEGIN_MESSAGE_MAP(CVtOrderCenterWnd, CDialog)
	//ON_BN_CLICKED(IDC_BTN_LOADINFO, &CVtOrderCenterWnd::OnBnClickedBtnLoadinfo)
	ON_CBN_SELCHANGE(IDC_COMBO_ACCOUNT5, &CVtOrderCenterWnd::OnCbnSelchangeComboAccount)
	ON_CBN_SELCHANGE(IDC_COMBO_PRODUCT5, &CVtOrderCenterWnd::OnCbnSelchangeComboProduct)
	ON_BN_CLICKED(IDC_BTN_LEFT_EXTEND, &CVtOrderCenterWnd::OnBnClickedBtnLeftExtend)
	ON_EN_CHANGE(IDC_EDIT_STOPVAL, &CVtOrderCenterWnd::OnEnChangeEditStopval)
	ON_EN_CHANGE(IDC_EDIT_ORDER_AMOUNT, &CVtOrderCenterWnd::OnEnChangeEditOrderAmount)
	ON_BN_CLICKED(IDC_BTN_ORDER_AMT1, &CVtOrderCenterWnd::OnBnClickedBtnOrderAmt1)
	ON_BN_CLICKED(IDC_BTN_ORDER_AMT2, &CVtOrderCenterWnd::OnBnClickedBtnOrderAmt2)
	ON_BN_CLICKED(IDC_BTN_ORDER_AMT3, &CVtOrderCenterWnd::OnBnClickedBtnOrderAmt3)
	ON_BN_CLICKED(IDC_BTN_ORDER_AMT4, &CVtOrderCenterWnd::OnBnClickedBtnOrderAmt4)
	ON_BN_CLICKED(IDC_BTN_ORDER_AMT5, &CVtOrderCenterWnd::OnBnClickedBtnOrderAmt5)
	ON_BN_CLICKED(IDC_BTN_ORDER_AMT6, &CVtOrderCenterWnd::OnBnClickedBtnOrderAmt6)
	ON_BN_CLICKED(IDC_BTN_SEL_SYMBOL, &CVtOrderCenterWnd::OnBnClickedBtnSelSymbol)
	ON_BN_CLICKED(IDC_BTN_FAVORITE, &CVtOrderCenterWnd::OnBnClickedBtnFavorite)
	ON_BN_CLICKED(IDC_BTN_GET_ACNT_INFO, &CVtOrderCenterWnd::OnBnClickedBtnGetAcntInfo)
	ON_BN_CLICKED(IDC_BTN_LIQ_ALL, &CVtOrderCenterWnd::OnBnClickedBtnLiqAll)
	ON_BN_CLICKED(IDC_BTN_LIQ_SEL, &CVtOrderCenterWnd::OnBnClickedBtnLiqSel)
	ON_BN_CLICKED(IDC_BTN_CANCEL_ALL, &CVtOrderCenterWnd::OnBnClickedBtnCancelAll)
	ON_BN_CLICKED(IDC_BTN_CANCEL_SEL, &CVtOrderCenterWnd::OnBnClickedBtnCancelSel)
	ON_BN_CLICKED(IDC_BTN_FV1, &CVtOrderCenterWnd::OnBnClickedBtnFv1)
	ON_BN_CLICKED(IDC_BTN_FV2, &CVtOrderCenterWnd::OnBnClickedBtnFv2)
	ON_BN_CLICKED(IDC_BTN_FV3, &CVtOrderCenterWnd::OnBnClickedBtnFv3)
	ON_BN_CLICKED(IDC_BTN_FV4, &CVtOrderCenterWnd::OnBnClickedBtnFv4)
	ON_BN_CLICKED(IDC_BTN_FV5, &CVtOrderCenterWnd::OnBnClickedBtnFv5)
	ON_BN_CLICKED(IDC_BTN_FV6, &CVtOrderCenterWnd::OnBnClickedBtnFv6)
	ON_BN_CLICKED(IDC_BTN_FV7, &CVtOrderCenterWnd::OnBnClickedBtnFv7)
	ON_BN_CLICKED(IDC_BTN_FV8, &CVtOrderCenterWnd::OnBnClickedBtnFv8)
	ON_BN_CLICKED(IDC_BTN_RIGHT_EXTEND, &CVtOrderCenterWnd::OnBnClickedBtnRightExtend)
	ON_MESSAGE(WM_REFRESH_GRID, &CVtOrderCenterWnd::OnRefreshGrid)
END_MESSAGE_MAP()


// CVtOrderCenterWnd message handlers


BOOL CVtOrderCenterWnd::OnInitDialog()
{
	CRHGenericChildDialog::OnInitDialog();

	_ScOrderAmount.SetRange32(_OrderConfigMgr->MinOrderAmount, _OrderConfigMgr->MaxOrderAmount);
	_ScStopVal.SetRange32(_OrderConfigMgr->MinStopVal, _OrderConfigMgr->MaxStopVal);
	// TODO:  Add extra initialization here
	_OrderPanelGrid.AttachGrid(this, IDC_STATIC_ORDER_PANEL);
	_ProductRemainGrid.AttachGrid(this, IDC_STATIC_PRODUCT_REMAIN);
	

	CString msg;
	msg.Format(_T("%d"), _OrderConfigMgr->OrderAmount);
	_EditOrderAmount.SetWindowText(msg);
	msg.Format(_T("%d"), _OrderConfigMgr->StopVal);
	_EditStopVal.SetWindowText(msg);
	InitAccount();
	InitProduct();

	FavoriteButtonVector.push_back(&_BtnFv1);
	FavoriteButtonVector.push_back(&_BtnFv2);
	FavoriteButtonVector.push_back(&_BtnFv3);
	FavoriteButtonVector.push_back(&_BtnFv4);
	FavoriteButtonVector.push_back(&_BtnFv5);
	FavoriteButtonVector.push_back(&_BtnFv6);
	FavoriteButtonVector.push_back(&_BtnFv7);
	FavoriteButtonVector.push_back(&_BtnFv8);

	InitFavorite();

	InitPosition();

	_RefreshManger = new VtRefreshManager();
	_RefreshManger->StartTaskThread();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CVtOrderCenterWnd::OnBnClickedBtnLoadinfo()
{
	// TODO: Add your control notification handler code here
}

void CVtOrderCenterWnd::InitQuote(VtQuote* quote)
{
	_OrderPanelGrid.InitQuote(quote);
}

void CVtOrderCenterWnd::InitHoga(VtHoga* hoga)
{
	_OrderPanelGrid.InitHoga(hoga);
}

void CVtOrderCenterWnd::OnReceiveRealtimeQuote(VtRealtimeQuote* realQuote)
{
	_OrderPanelGrid.OnReceiveRealtimeQuote(realQuote);
}

void CVtOrderCenterWnd::OnReceiveRealtimeQuote(VtQuote* quote)
{
	_OrderPanelGrid.OnReceiveRealtimeQuote(quote);
	_ProductRemainGrid.OnReceiveRealtimeQuote(quote);
}

void CVtOrderCenterWnd::OnReceiveRealtimeHoga(VtRealtimeHoga* realHoga)
{
	_OrderPanelGrid.OnReceiveRealtimeHoga(realHoga);
}

void CVtOrderCenterWnd::OnReceiveRealtimeHoga(VtHoga* hoga)
{
	_OrderPanelGrid.OnReceiveRealtimeHoga(hoga);
}

void CVtOrderCenterWnd::OnOrderReceived(VtOrder* order)
{
	_OrderPanelGrid.OnOrderReceived(order);
}

void CVtOrderCenterWnd::OnAccepted(VtOrder* order)
{
	_OrderPanelGrid.OnAccepted(order);
}

void CVtOrderCenterWnd::OnFilled(VtOrder* order)
{
	_OrderPanelGrid.OnFilled(order);
	UpdateRealtimeOrder(order);
	if (_OrderConfigMgr->Position())
		_ProductRemainGrid.ShowPosition(_OrderConfigMgr->Position());
}

void CVtOrderCenterWnd::OnConfirmModify(VtOrder* order)
{
	_OrderPanelGrid.OnConfirmModify(order);
}

void CVtOrderCenterWnd::OnConfirmCancel(VtOrder* order)
{
	_OrderPanelGrid.OnConfirmCancel(order);
}

void CVtOrderCenterWnd::SetPosition(VtSymbol* symbol)
{
	;
}

void CVtOrderCenterWnd::SetAccount(VtAccount* acnt)
{
	if (_OrderConfigMgr)
	{
		if (_OrderConfigMgr->Account() == acnt)
			return;

		_OrderConfigMgr->Account(acnt);
	}
	if (_ParentOrderWnd)
		_ParentOrderWnd->RefreshAccount();

	int sel = _ComboAccount.FindString(0, acnt->AccountNo.c_str());
	if (sel != -1)
	{
		_ComboAccount.SetCurSel(sel);
	}

	if (_AccountPwd.GetSafeHwnd())
	{
		_AccountPwd.SetWindowText(acnt->Password.c_str());
	}
}

void CVtOrderCenterWnd::SetSymbol(VtSymbol* sym)
{
	if (_OrderConfigMgr)
	{
		if (_OrderConfigMgr->Symbol() == sym)
			return;

		_OrderConfigMgr->Symbol(sym);
	}
	if (_ParentOrderWnd)
		_ParentOrderWnd->RefreshSymbol();

	SetPosition(sym);
	//_ProductRemainGrid.Symbol(sym);
	_ProductName.SetWindowText(sym->Name.c_str());
}

void CVtOrderCenterWnd::UpdateRealtimeOrder(VtOrder* order)
{
	if (!order)
		return;

	CString msg;
	msg.Format(_T("잔고(%d)"), order->remainQty);
	_RemainAmount.SetWindowText(msg);

	msg.Format(_T("가능(%d)"), order->orderableAmount);
	_OrderableAmount.SetWindowText(msg);
}

void CVtOrderCenterWnd::OnCbnSelchangeComboAccount()
{
	// TODO: Add your control notification handler code here
	int curSel = _ComboAccount.GetCurSel();
	if (curSel > 0)
	{
		_OrderPanelGrid.InitAllInfo();
		VtAccount* acnt = (VtAccount*)_ComboAccount.GetItemData(curSel);

		VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
		orderDlgMgr->ChangeAccountReceiverWindow(_Account->AccountNo, acnt->AccountNo, this->ParentOrderWnd());

		_Account = acnt;
		SetAccount(acnt);
		
		VtRealtimeRegisterManager* realtimeManager = VtRealtimeRegisterManager::GetInstance();
		realtimeManager->RegisterAccount(acnt->AccountNo);

		VtTaskManager* taskMgr = VtTaskManager::GetInstance();
		//taskMgr->DoBundleTaskForAccount(acnt->AccountNo);
	}
}

void CVtOrderCenterWnd::OnCbnSelchangeComboProduct()
{
	int curSel = _ComboProduct.GetCurSel();
	if (curSel > 0)
	{
		_OrderPanelGrid.InitAllInfo();
		VtSymbol* sym = (VtSymbol*)_ComboProduct.GetItemData(curSel);
		if (!sym)
			return;

		VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
		orderDlgMgr->ChangeSymbolReceiverWindow(_Symbol->FullCode, sym->FullCode, this->ParentOrderWnd());

		_Symbol = sym;
		SetSymbol(sym);

		VtRealtimeRegisterManager* realtimeManager = VtRealtimeRegisterManager::GetInstance();
		realtimeManager->RegisterProduct(sym->FullCode);
		VtTaskManager* taskMgr = VtTaskManager::GetInstance();
		//taskMgr->DoBundleTaskForSymbol(sym->FullCode);
		DoBundleTaskForSymbol(sym->FullCode);
	}
}

void CVtOrderCenterWnd::InitAccount()
{
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	int i = 0;
	for (auto it = acntMgr->AccountMap.begin(); it != acntMgr->AccountMap.end(); ++it)
	{
		VtAccount* acnt = it->second;
		_ComboAccount.AddString(acnt->AccountNo.c_str());
		_ComboAccount.SetItemData(i++, (DWORD_PTR)acnt);
	}

	if (_ComboAccount.GetCount() > 0)
	{
		_ComboAccount.SetCurSel(0);
		
		VtAccount* acnt = (VtAccount*)_ComboAccount.GetItemData(0);

		VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
		orderDlgMgr->AddAccountReceiverWindow(acnt->AccountNo, this->ParentOrderWnd());

		_Account = acnt;
		
		SetAccount(acnt);

		VtRealtimeRegisterManager* realtimeManager = VtRealtimeRegisterManager::GetInstance();
		realtimeManager->RegisterAccount(acnt->AccountNo);

		VtTaskManager* taskMgr = VtTaskManager::GetInstance();
		//taskMgr->DoBundleTaskForAccount(acnt->AccountNo);
	}
}

void CVtOrderCenterWnd::InitProduct()
{
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	int i = 0;
	for (auto it = symMgr->SymbolMap.begin(); it != symMgr->SymbolMap.end(); ++it)
	{
		VtSymbol* sym = it->second;
		_ComboProduct.AddString(sym->ShortCode.c_str());
		_ComboProduct.SetItemData(i++, (DWORD_PTR)sym);
	}

	
	if (_ComboProduct.GetCount() > 0)
	{
		_ComboProduct.SetCurSel(_StartProductIndex);
		CString symCode;
		_ComboProduct.GetLBText(_StartProductIndex, symCode);
		
		std::string code = symCode;
		VtSymbol* sym = symMgr->FindSymbol(code);
		if (!sym)
			return;
		
		_Symbol = sym;

		VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
		orderDlgMgr->AddSymbolReceiverWindow(sym->FullCode, this->ParentOrderWnd());

		
		SetSymbol(sym);
		
		VtRealtimeRegisterManager* realtimeManager = VtRealtimeRegisterManager::GetInstance();
		realtimeManager->RegisterProduct(sym->FullCode);
		VtTaskManager* taskMgr = VtTaskManager::GetInstance();
		//taskMgr->DoBundleTaskForSymbol(sym->FullCode);
		DoBundleTaskForSymbol(sym->FullCode);
	}
	
}


void CVtOrderCenterWnd::InitFavorite()
{
	VtGlobal* global = VtGlobal::GetInstance();
	int i = 0;
	for (auto it = global->FavoriteSymbolList.begin(); it != global->FavoriteSymbolList.end(); ++it)
	{
		VtSymbol* sym = *it;
		CButton* btn = FavoriteButtonVector[i];
		btn->SetFont(&SFont);
		CString msg;
		msg = sym->UserDefinedName.c_str();
		btn->SetWindowText(msg);
		i++;
	}
}

void CVtOrderCenterWnd::SetSymbolByFavoriteButton(int index)
{
	VtGlobal* global = VtGlobal::GetInstance();
	if (index > global->FavoriteSymbolList.size() - 1)
		return;

	VtSymbol* sym = global->FavoriteSymbolList[index];

	if (!sym)
		return;

	if (sym == _OrderConfigMgr->Symbol())
		return;

	_OrderPanelGrid.InitAllInfo();
	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	orderDlgMgr->ChangeSymbolReceiverWindow(_Symbol->FullCode, sym->FullCode, this->ParentOrderWnd());

	_Symbol = sym;
	SetSymbol(sym);

	VtRealtimeRegisterManager* realtimeManager = VtRealtimeRegisterManager::GetInstance();
	realtimeManager->RegisterProduct(sym->FullCode);
	VtTaskManager* taskMgr = VtTaskManager::GetInstance();
	//taskMgr->DoBundleTaskForSymbol(sym->FullCode);
	DoBundleTaskForSymbol(sym->FullCode);
}

void CVtOrderCenterWnd::InitPosition()
{
	if (!_OrderConfigMgr || !_OrderConfigMgr->Account() || !_OrderConfigMgr->Symbol())
		return;
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbolMaster* master = symMgr->FindSymbolMasterByShortCode(_OrderConfigMgr->Symbol()->ShortCode);
	_OrderConfigMgr->Master(master);

	VtAccount* acnt = _OrderConfigMgr->Account();
	VtPosition* posi = acnt->FindPosition(_OrderConfigMgr->Symbol()->FullCode);
	if (posi)
	{
		_ProductRemainGrid.ShowPosition(posi);
	}
}

void CVtOrderCenterWnd::DoBundleTaskForSymbol(std::string fullCode)
{
	VtTaskManager* taskMgr = VtTaskManager::GetInstance();

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbolMaster* master = symMgr->FindSymbolMasterByFullCode(fullCode);
	if (!master)
	{
		VtTaskSymbolMasterArg* arg = new VtTaskSymbolMasterArg(VtTaskType::VtTaskSymbolMaster);
		arg->FullCode = fullCode;
		taskMgr->AddTask(arg);
	}
	else
	{
		_ParentOrderWnd->SetMaster(master);
	}


	VtQuoteManager* quoteMgr = VtQuoteManager::GetInstance();
	VtQuote* quote = quoteMgr->FindQuote(fullCode);
	if (!quote)
	{
		VtTaskSymbolQuoteArg* arg2 = new VtTaskSymbolQuoteArg(VtTaskType::VtTaskSymbolQuote);
		arg2->FullCode = fullCode;
		taskMgr->AddTask(arg2);
	}
	else
	{
		_ParentOrderWnd->SetQuote(quote);
		InitQuote(quote);
	}

	VtHogaManager* hogaMgr = VtHogaManager::GetInstance();
	VtHoga* hoga = hogaMgr->FindHoga(fullCode);

	if (!hoga)
	{
		VtTaskSymbolHogaArg* arg3 = new VtTaskSymbolHogaArg(VtTaskType::VtTaskSymbolHoga);
		arg3->FullCode = fullCode;
		taskMgr->AddTask(arg3);
	}
	else
	{
		_ParentOrderWnd->SetHoga(hoga);
		InitHoga(hoga);
	}

	taskMgr->StartTask();
}

void CVtOrderCenterWnd::OnBnClickedBtnLeftExtend()
{
	if (!_OrderConfigMgr)
		return;
	_OrderConfigMgr->ShowLeft = !_OrderConfigMgr->ShowLeft;
	// TODO: Add your control notification handler code here
	if (_ParentOrderWnd)
	{
		_ParentOrderWnd->RecalcLayout();
	}
}


void CVtOrderCenterWnd::OnEnChangeEditStopval()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CRHGenericChildDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (_EditStopVal.GetSafeHwnd())
	{
		CString msg;
		_EditStopVal.GetWindowText(msg);

		_OrderConfigMgr->StopVal = std::stoi(msg.GetBuffer());
	}
}


void CVtOrderCenterWnd::OnEnChangeEditOrderAmount()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CRHGenericChildDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (_EditStopVal.GetSafeHwnd())
	{
		CString msg;
		_EditOrderAmount.GetWindowText(msg);

		_OrderConfigMgr->OrderAmount = std::stoi(msg.GetBuffer());
	}
}


void CVtOrderCenterWnd::OnBnClickedBtnOrderAmt1()
{
	// TODO: Add your control notification handler code here
	_OrderConfigMgr->OrderAmount = 1;
	CString msg;
	msg.Format(_T("%d"), _OrderConfigMgr->OrderAmount);
	_EditOrderAmount.SetWindowText(msg);
}


void CVtOrderCenterWnd::OnBnClickedBtnOrderAmt2()
{
	// TODO: Add your control notification handler code here
	_OrderConfigMgr->OrderAmount = 2;
	CString msg;
	msg.Format(_T("%d"), _OrderConfigMgr->OrderAmount);
	_EditOrderAmount.SetWindowText(msg);
}


void CVtOrderCenterWnd::OnBnClickedBtnOrderAmt3()
{
	// TODO: Add your control notification handler code here
	_OrderConfigMgr->OrderAmount = 3;
	CString msg;
	msg.Format(_T("%d"), _OrderConfigMgr->OrderAmount);
	_EditOrderAmount.SetWindowText(msg);
}


void CVtOrderCenterWnd::OnBnClickedBtnOrderAmt4()
{
	// TODO: Add your control notification handler code here
	_OrderConfigMgr->OrderAmount = 4;
	CString msg;
	msg.Format(_T("%d"), _OrderConfigMgr->OrderAmount);
	_EditOrderAmount.SetWindowText(msg);
}


void CVtOrderCenterWnd::OnBnClickedBtnOrderAmt5()
{
	// TODO: Add your control notification handler code here
	_OrderConfigMgr->OrderAmount = 5;
	CString msg;
	msg.Format(_T("%d"), _OrderConfigMgr->OrderAmount);
	_EditOrderAmount.SetWindowText(msg);
}


void CVtOrderCenterWnd::OnBnClickedBtnOrderAmt6()
{
	// TODO: Add your control notification handler code here
	_OrderConfigMgr->OrderAmount = 6;
	CString msg;
	msg.Format(_T("%d"), _OrderConfigMgr->OrderAmount);
	_EditOrderAmount.SetWindowText(msg);
}


void CVtOrderCenterWnd::OnBnClickedBtnSelSymbol()
{
	// TODO: Add your control notification handler code here
	if (_SymSelector)
	{
		delete _SymSelector;
		_SymSelector = nullptr;
	}
	_SymSelector = new VtSymbolSelector(this);
	_SymSelector->Create(IDD_SYMBOL_SELECTOR, this);
	_SymSelector->ShowWindow(SW_SHOW);
	_SymSelector->SetOrderCenterWindow(this);

	CButton *pButton = (CButton*)GetDlgItem(IDC_BTN_SEL_SYMBOL);
	if (pButton)
	{
		CRect rcButton;
		pButton->GetWindowRect(rcButton);
		CPoint pos;
		GetCursorPos(&pos);
		
		pos.x = rcButton.left - 7;
		pos.y = rcButton.bottom - 7;
		CRect dlgRect;
		_SymSelector->GetWindowRect(dlgRect);
		dlgRect.left = pos.x;
		dlgRect.top = pos.y;
		dlgRect.right = dlgRect.Width() + pos.x + 36;
		dlgRect.bottom = dlgRect.Height() + pos.y - 56;

		CDeferPos dp(1);	
		dp.MoveWindow(_SymSelector->GetSafeHwnd(), dlgRect.left, dlgRect.top, dlgRect.Width(), dlgRect.Height(), TRUE);
	}
}

void CVtOrderCenterWnd::OnSymbolClicked(VtSymbol* sym, int src, VtSymbolSelector* srdWnd)
{
	if (!sym)
		return;
	_OrderPanelGrid.InitAllInfo();

	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	orderDlgMgr->ChangeSymbolReceiverWindow(_Symbol->FullCode, sym->FullCode, this->ParentOrderWnd());

	_Symbol = sym;
	//SetPosition(sym);

	SetSymbol(sym);

	VtRealtimeRegisterManager* realtimeManager = VtRealtimeRegisterManager::GetInstance();
	realtimeManager->RegisterProduct(sym->FullCode);

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	symMgr->ActiveSymbol(sym);
	symMgr->RequestSymbolBundleInfo(sym->FullCode);

	_ProductName.SetWindowText(sym->Name.c_str());

	srdWnd->DestroyWindow();

	if (_ParentOrderWnd)
		_ParentOrderWnd->OnSymbolClicked(sym, src, srdWnd);

	int index = _ComboProduct.FindString(0, sym->ShortCode.c_str());
	if (index != -1)
		_ComboProduct.SetCurSel(index);
}

void CVtOrderCenterWnd::ChangeFocus()
{
	SendMessage(WM_NEXTDLGCTL, (WPARAM)_OrderPanelGrid.GetSafeHwnd(), TRUE);
}


void CVtOrderCenterWnd::OnBnClickedBtnFavorite()
{
	// TODO: Add your control notification handler code here
	CVtSymbolFavoriteDlg dlg(this);
	dlg.OrderCenterWnd = this;
	int result = dlg.DoModal();
	if (result == ID_OK)
	{
		VtGlobal* global = VtGlobal::GetInstance();
		int i = 0;
		for (auto it = global->FavoriteSymbolList.begin(); it != global->FavoriteSymbolList.end(); ++it)
		{
			VtSymbol* sym = *it;
			SetFavorite(i, sym);
			i++;
		}
	}
}


void CVtOrderCenterWnd::OnBnClickedBtnGetAcntInfo()
{
	if (!_OrderConfigMgr || !_OrderConfigMgr->Account())
		return;

	CString msg;
	_AccountPwd.GetWindowText(msg);

	std::string pwd = msg;

	if (pwd.compare(_OrderConfigMgr->Account()->Password) != 0)
	{
		AfxMessageBox(_T("비밀번호가 틀립니다.!"));
		return;
	}

	// TODO: Add your control notification handler code herevoid OnReceiveAccountDeposit(VtAccount* acnt);
	VtTaskManager* taskMgr = VtTaskManager::GetInstance();
	taskMgr->DoBundleTaskForAccount(_OrderConfigMgr->Account()->AccountNo);
}

void CVtOrderCenterWnd::ResetSymbol(VtSymbol* sym)
{
	if (!sym)
		return;

	if (sym == _OrderConfigMgr->Symbol())
		return;

	_OrderPanelGrid.InitAllInfo();
	

	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	orderDlgMgr->ChangeSymbolReceiverWindow(_Symbol->FullCode, sym->FullCode, this->ParentOrderWnd());

	_Symbol = sym;

	int sel = _ComboProduct.FindString(0, sym->ShortCode.c_str());
	if (sel != -1)
	{
		_ComboProduct.SetCurSel(sel);
	}

	SetSymbol(sym);

	VtRealtimeRegisterManager* realtimeManager = VtRealtimeRegisterManager::GetInstance();
	realtimeManager->RegisterProduct(sym->FullCode);
	VtTaskManager* taskMgr = VtTaskManager::GetInstance();
	//taskMgr->DoBundleTaskForSymbol(sym->FullCode);
	DoBundleTaskForSymbol(sym->FullCode);
}


void CVtOrderCenterWnd::OnBnClickedBtnLiqAll()
{
	// TODO: Add your control notification handler code here
	if (!_OrderConfigMgr || !_OrderConfigMgr->Account())
		return;

	for (auto it = _OrderConfigMgr->Account()->PositionMap.begin(); it != _OrderConfigMgr->Account()->PositionMap.end(); it++)
	{
		VtPosition* posi = it->second;
		if (posi->Position != VtPositionType::None)
			_OrderPanelGrid.LiqudAll(VtPriceType::Market, 0);
	}
}


void CVtOrderCenterWnd::OnBnClickedBtnLiqSel()
{
	// TODO: Add your control notification handler code here
	if (_OrderConfigMgr || _OrderConfigMgr->Position())
	{
		VtPosition* posi = _OrderConfigMgr->Position();
		if (posi->Position != VtPositionType::None)
			_OrderPanelGrid.LiqudAll(VtPriceType::Market, 0);
	}
}


void CVtOrderCenterWnd::OnBnClickedBtnCancelAll()
{
	// TODO: Add your control notification handler code here
	if (!_OrderConfigMgr || !_OrderConfigMgr->OrderMgr())
		return;

	for (auto it = _OrderConfigMgr->OrderMgr()->AcceptedMap.begin(); it != _OrderConfigMgr->OrderMgr()->AcceptedMap.end(); ++it)
	{
		VtOrder* order = it->second;
		_OrderPanelGrid.CancelOrder(order);
	}
}


void CVtOrderCenterWnd::OnBnClickedBtnCancelSel()
{
	// TODO: Add your control notification handler code here
	if (!_OrderConfigMgr || !_OrderConfigMgr->PrdtOrderMgr())
		return;

	for (auto it = _OrderConfigMgr->PrdtOrderMgr()->AcceptedMap.begin(); it != _OrderConfigMgr->PrdtOrderMgr()->AcceptedMap.end(); ++it)
	{
		VtOrder* order = it->second;
		_OrderPanelGrid.CancelOrder(order);
	}
}


void CVtOrderCenterWnd::OnBnClickedBtnFv1()
{
	// TODO: Add your control notification handler code here
	SetSymbolByFavoriteButton(0);
}


void CVtOrderCenterWnd::OnBnClickedBtnFv2()
{
	// TODO: Add your control notification handler code here
	SetSymbolByFavoriteButton(1);
}


void CVtOrderCenterWnd::OnBnClickedBtnFv3()
{
	// TODO: Add your control notification handler code here
	SetSymbolByFavoriteButton(2);
}


void CVtOrderCenterWnd::OnBnClickedBtnFv4()
{
	// TODO: Add your control notification handler code here
	SetSymbolByFavoriteButton(3);
}


void CVtOrderCenterWnd::OnBnClickedBtnFv5()
{
	// TODO: Add your control notification handler code here
	SetSymbolByFavoriteButton(4);
}


void CVtOrderCenterWnd::OnBnClickedBtnFv6()
{
	// TODO: Add your control notification handler code here
	SetSymbolByFavoriteButton(5);
}


void CVtOrderCenterWnd::OnBnClickedBtnFv7()
{
	// TODO: Add your control notification handler code here
	SetSymbolByFavoriteButton(6);
}


void CVtOrderCenterWnd::OnBnClickedBtnFv8()
{
	// TODO: Add your control notification handler code here
	SetSymbolByFavoriteButton(7);
}

void CVtOrderCenterWnd::SetFavorite(int index, VtSymbol* sym)
{
	if (index > 7 || !sym)
		return;

	CButton* btn = FavoriteButtonVector[index];
	btn->SetWindowText(sym->UserDefinedName.c_str());
	btn->Invalidate(TRUE);
}


void CVtOrderCenterWnd::OnBnClickedBtnRightExtend()
{
	// TODO: Add your control notification handler code here
	if (!_OrderConfigMgr)
		return;
	_OrderConfigMgr->ShowRight = !_OrderConfigMgr->ShowRight;
	// TODO: Add your control notification handler code here
	if (_ParentOrderWnd)
	{
		_ParentOrderWnd->RecalcLayout();
	}
}

LRESULT CVtOrderCenterWnd::OnRefreshGrid(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	_OrderPanelGrid.RefreshGrid();

	return 0;
}
