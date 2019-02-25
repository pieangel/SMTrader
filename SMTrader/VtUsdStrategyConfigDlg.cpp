// VtUsdStrategyConfigDlg.cpp : implementation file
//
#include "stdafx.h"
#include "VtUsdStrategyConfigDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include "VtAccountFundSelector.h"
#include "VtSymbol.h"
#include "HdSymbolSelecter.h"
#include "VtSystemGroupManager.h"
#include "VtSystemGroup.h"
#include "VtSystemDef.h"
#include "VtSystemArgGroup.h"
#include "VtSystemArg.h"
#include "VtAccount.h"
#include "VtFund.h"
#include "VtSymbolManager.h"
#include "VtStrategyGrid.h"
#include "Poco/NumberFormatter.h"
#include "XFormatNumber.h"
#include "System/VtSystemManager.h"
#include "VtRealtimeRegisterManager.h"
#include "HdWindowManager.h"
#include "Poco/Delegate.h"
#include "VtStrategyWndManager.h"
#include "VtGlobal.h"
#include "VtStrategyToolWnd.h"
#include "Format/time.h"
using Poco::Delegate;
using Poco::NumberFormatter;


// VtUsdStrategyConfigDlg dialog

IMPLEMENT_DYNAMIC(VtUsdStrategyConfigDlg, CDialogEx)

VtUsdStrategyConfigDlg::VtUsdStrategyConfigDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SYSTEM_USD, pParent)
{
	InitComboMap();
	_ShowArgGrid = true;
	_SelSymbol = nullptr;
	_EnableTrailStop = false;
	_EnableLossCut = false;
	_EnableTargetCut = false;
}

VtUsdStrategyConfigDlg::~VtUsdStrategyConfigDlg()
{
	VtStrategyWndManager* wndMgr = VtStrategyWndManager::GetInstance();
	_WindowEvent -= delegate(wndMgr, &VtStrategyWndManager::OnWindowEvent);
}

void VtUsdStrategyConfigDlg::System(VtSystem* val)
{
	_System = val;
}

void VtUsdStrategyConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_LOSSCUT, _ChecktLossCut);
	DDX_Control(pDX, IDC_CHECK_TS, _CheckTs);
	DDX_Control(pDX, IDC_COMBO_LOSSCUT_TYPE, _ComboLosscutType);
	DDX_Control(pDX, IDC_COMBO_STRATEGY, _ComboStrategy);
	DDX_Control(pDX, IDC_DP_ENT_BEGIN, _DpEntBegin);
	DDX_Control(pDX, IDC_DP_ENT_END, _DpEntEnd);
	DDX_Control(pDX, IDC_DP_LIQ, _DpLiq);
	DDX_Control(pDX, IDC_EDIT_ENT_MAX, _EditEntMax);
	DDX_Control(pDX, IDC_EDIT_LOSSCUT, _EditLosscut);
	DDX_Control(pDX, IDC_EDIT_TS_PERCENT, _EditTsPercent);
	DDX_Control(pDX, IDC_EDIT_TS_PL, _EditTsPl);
	DDX_Control(pDX, IDC_STATIC_PL, _StaticPl);
	DDX_Control(pDX, IDC_STATIC_ACCOUNT, _StaticAccount);
	DDX_Control(pDX, IDC_STATIC_SYMBOL, _StaticSymbol);
	DDX_Control(pDX, IDC_COMBO_PROFIT_TARGET, _ComboProfitTarget);
	DDX_Control(pDX, IDC_CHECK_PROFIT_TARGET, _CheckTargetCut);
	DDX_Control(pDX, IDC_EDIT_PROFIT_TARGET, _EditProfitTarget);
	DDX_Control(pDX, IDC_CK_LIQ_ALL, _CheckLiqByStop);
	DDX_Control(pDX, IDC_BTN_ACCOUNT, _BtnAcnt);
	DDX_Control(pDX, IDC_BTN_APPLY, _BtnApply);
	DDX_Control(pDX, IDC_BTN_EXTRA_VAR, _BtnExtraVar);
	DDX_Control(pDX, IDC_BTN_SYMBOL, _BtnSymbol);
	DDX_Control(pDX, IDC_EDIT_ORDER_AMT, _EditOrderAmt);
	DDX_Control(pDX, IDC_SPIN_ORDER_AMT, _SpinOrderAmt);
	DDX_Control(pDX, IDC_STATIC_ENT_CNT_TODAY, _EntCntToday);
	DDX_Control(pDX, IDC_CHECK_RUN, _CheckRun);
	DDX_Control(pDX, IDC_STATIC_LAST_ENT_PRICE, _StaticLastEntPrice);
	//  DDX_Control(pDX, IDC_STATIC_POSITION, _);
	DDX_Control(pDX, IDC_STATIC_POSITION, _StaticPosition);
}


BEGIN_MESSAGE_MAP(VtUsdStrategyConfigDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_ACCOUNT, &VtUsdStrategyConfigDlg::OnBnClickedBtnAccount)
	ON_BN_CLICKED(IDC_BTN_SYMBOL, &VtUsdStrategyConfigDlg::OnBnClickedBtnSymbol)
	ON_CBN_SELCHANGE(IDC_COMBO_STRATEGY, &VtUsdStrategyConfigDlg::OnCbnSelchangeComboStrategy)
	ON_BN_CLICKED(IDC_BTN_EXTRA_VAR, &VtUsdStrategyConfigDlg::OnBnClickedBtnExtraVar)
	ON_BN_CLICKED(IDC_BTN_APPLY, &VtUsdStrategyConfigDlg::OnBnClickedBtnApply)
	ON_CBN_SELCHANGE(IDC_COMBO_LOSSCUT_TYPE, &VtUsdStrategyConfigDlg::OnCbnSelchangeComboLosscutType)
	ON_CBN_SELCHANGE(IDC_COMBO_PROFIT_TARGET, &VtUsdStrategyConfigDlg::OnCbnSelchangeComboProfitTarget)
	ON_BN_CLICKED(IDC_CHECK_TS, &VtUsdStrategyConfigDlg::OnBnClickedCheckTs)
	ON_BN_CLICKED(IDC_CHECK_LOSSCUT, &VtUsdStrategyConfigDlg::OnBnClickedCheckLosscut)
	ON_BN_CLICKED(IDC_CHECK_PROFIT_TARGET, &VtUsdStrategyConfigDlg::OnBnClickedCheckProfitTarget)
	ON_BN_CLICKED(IDC_CK_LIQ_ALL, &VtUsdStrategyConfigDlg::OnBnClickedCkLiqAll)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHECK_RUN, &VtUsdStrategyConfigDlg::OnBnClickedCheckRun)
	ON_EN_CHANGE(IDC_EDIT_ORDER_AMT, &VtUsdStrategyConfigDlg::OnEnChangeEditOrderAmt)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// VtUsdStrategyConfigDlg message handlers


BOOL VtUsdStrategyConfigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	this->SetIcon(hIcon, FALSE);

	_EntGrid.Type(0);
	_EntGrid.AttachGrid(this, IDC_STATIC_ENT_GRID);
	_LiqGrid.Type(1);
	_LiqGrid.AttachGrid(this, IDC_STATIC_LIQ_GRID);

	_ArgGrid.AttachGrid(this, IDC_STATIC_ARG);

	_ComboProfitTarget.AddString(_T("틱"));
	_ComboProfitTarget.AddString(_T("%"));
	_ComboLosscutType.AddString(_T("틱"));
	_ComboLosscutType.AddString(_T("%"));
	_ComboProfitTarget.SetCurSel(0);
	_ComboLosscutType.SetCurSel(0);

	_SpinOrderAmt.SetRange32(0, 100);

	std::vector<CShadeButtonST*> _btnVec;
	_btnVec.push_back(&_BtnAcnt);
	_btnVec.push_back(&_BtnApply);
	_btnVec.push_back(&_BtnExtraVar);
	_btnVec.push_back(&_BtnSymbol);

	for (auto it = _btnVec.begin(); it != _btnVec.end(); ++it) {
		CShadeButtonST* btn = *it;
		btn->SetShade(SHS_HSHADE, 8, 20, 5, RGB(55, 55, 255));
		btn->DrawFlatFocus(TRUE);
	}

	_BtnExtraVar.SetIcon(IDI_DOWN);

	for (auto it = _SystemMap.begin(); it != _SystemMap.end(); ++it) {
		auto item = it->second;
		int index = _ComboStrategy.AddString(std::get<0>(item).c_str());
		_ComboStrategy.SetItemDataPtr(index, (void*)std::get<1>(item));
	}

	if (!_System) {
		_ComboStrategy.SetCurSel(0);
		_System = (VtSystem*)_ComboStrategy.GetItemDataPtr(0);
	}

	InitControls();

	CRect rcWnd;
	GetWindowRect(rcWnd);
	_WndHeight = rcWnd.Height();
	ResizeWindow();

	VtSystemManager* sysMgr = VtSystemManager::GetInstance();
	sysMgr->AddSystemDialog(this);

	SetTimer(ArgTimer, 100, NULL);

	VtStrategyWndManager* wndMgr = VtStrategyWndManager::GetInstance();
	_WindowEvent += delegate(wndMgr, &VtStrategyWndManager::OnWindowEvent);

	HdWindowEventArgs arg;
	arg.pWnd = this;
	arg.wndType = HdWindowType::StrategySetWindow;
	arg.eventType = HdWindowEventType::Created;
	FireWindowEvent(std::move(arg));

	if (_System) {
		VtStrategyWndManager* stgWndMgr = VtStrategyWndManager::GetInstance();
		stgWndMgr->AddSystemDialog(_System->Name(), this);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void VtUsdStrategyConfigDlg::ClearArgMap()
{
	_EntGrid.ClearArgMap();
	_LiqGrid.ClearArgMap();
}

void VtUsdStrategyConfigDlg::InitComboMap()
{
	VtSystemGroupManager* strCatMgr = VtSystemGroupManager::GetInstance();
	VtSystemGroup* cat = strCatMgr->GetSystemGroup(VtSystemGroupType::KOSPI200F);
	std::vector<VtSystem*>& sysList1 = cat->GetSystemList();
	size_t i = 0;
	size_t total = 0;
	for (i = 0; i < sysList1.size(); ++i) {
		_SystemMap[i] = std::make_pair(sysList1[i]->Name(), sysList1[i]);
	}
	total += sysList1.size();
	cat = strCatMgr->GetSystemGroup(VtSystemGroupType::KOSDAQ150F);
	std::vector<VtSystem*>& sysList2 = cat->GetSystemList();
	for (size_t j = 0; j < sysList2.size(); ++j) {
		_SystemMap[total + j] = std::make_pair(sysList2[j]->Name(), sysList2[j]);
	}

	total += sysList2.size();
	cat = strCatMgr->GetSystemGroup(VtSystemGroupType::USDF);
	std::vector<VtSystem*>& sysList3 = cat->GetSystemList();
	for (size_t k = 0; k < sysList3.size(); ++k) {
		_SystemMap[total + k] = std::make_pair(sysList3[k]->Name(), sysList3[k]);
	}
}


void VtUsdStrategyConfigDlg::ResizeWindow()
{
	CRect rcGrid, rcWnd;
	GetWindowRect(rcWnd);
	_ArgGrid.GetWindowRect(rcGrid);
	_ShowArgGrid = !_ShowArgGrid;
	if (_ShowArgGrid) {
		_ArgGrid.ShowWindow(SW_SHOW);
		_BtnExtraVar.SetIcon(IDI_UP);
		SetWindowPos(nullptr, 0, 0, rcWnd.Width(), _WndHeight + rcGrid.Height() - 71, SWP_NOMOVE);
	}
	else {
		_ArgGrid.ShowWindow(SW_HIDE);
		_BtnExtraVar.SetIcon(IDI_DOWN);
		SetWindowPos(nullptr, 0, 0, rcWnd.Width(), _WndHeight - rcGrid.Height() + 4, SWP_NOMOVE);
	}
}

void VtUsdStrategyConfigDlg::InitControls()
{
	if (!_System)
		return;

	_System->Enable() ? _CheckRun.SetCheck(BST_CHECKED) : _CheckRun.SetCheck(BST_UNCHECKED);

	_SelSysIndex = FindSystemIndex(_System->Name());
	_ComboStrategy.SetCurSel(_SelSysIndex);
	_Type = (int)_System->SysTargetType();
	if (_System->SysTargetType() == TargetType::RealAccount ||
		_System->SysTargetType() == TargetType::SubAccount) {
		if (_System->Account()) {
			_Account = _System->Account();
			_StaticAccount.SetWindowText(_System->Account()->AccountNo.c_str());
		} 
		else {
			_Account = nullptr;
			_StaticAccount.SetWindowText(_T(""));
		}
	}
	else {
		if (_System->Fund()) {
			_Fund = _System->Fund();
			_StaticAccount.SetWindowText(_System->Fund()->Name.c_str());
		}
		else {
			_Fund = nullptr;
			_StaticAccount.SetWindowText(_T(""));
		}
	}

	if (_System->Symbol()) {
		_SelSymbol = _System->Symbol();
		_StaticSymbol.SetWindowText(_System->Symbol()->ShortCode.c_str());
	}
	else {
		_SelSymbol = nullptr;
		_StaticSymbol.SetWindowText(_T(""));
	}

	_EditOrderAmt.SetWindowText(std::to_string(_System->OrderAmount()).c_str());

	std::string title = _T("전략설정창 - ");
	title.append(_System->Name());
	title.append(_T(" - "));
	title.append(std::to_string(_System->Cycle()));
	title.append(_T("M"));
	SetWindowText(title.c_str());

	

	_System->LiqByStop() ? _CheckLiqByStop.SetCheck(BST_CHECKED) : _CheckLiqByStop.SetCheck(BST_UNCHECKED);
	_LiqByStop = _System->LiqByStop();

	CString profit;
	profit.Format(_T("%.0f"), _System->ProfitLoss());
	_StaticPl.SetWindowText(profit);

	profit.Format(_T("%d"), _System->EntryToday());
	_EntCntToday.SetWindowText(profit);

	profit.Format(_T("%d"), _System->LatestEntPrice());
	_StaticLastEntPrice.SetWindowText(profit);
	_EntGrid.ClearArgMap();
	_LiqGrid.ClearArgMap();
	_EntGrid.System(_System);
	_LiqGrid.System(_System);
	std::vector<VtSystemArgGroup>& argGrpVec = _System->GetArgGroupVec();
	for (auto it = argGrpVec.begin(); it != argGrpVec.end(); ++it) {
		VtSystemArgGroup& argGrp = *it;
		if (argGrp.Name().compare(_T("매수진입")) == 0 || 
			argGrp.Name().compare(_T("매도진입")) == 0) {
			_EntGrid.SetArg(argGrp);
		}

		if (argGrp.Name().compare(_T("매수청산")) == 0 ||
			argGrp.Name().compare(_T("매도청산")) == 0) {
			_LiqGrid.SetArg(argGrp);
		}
		if (argGrp.Name().compare(_T("기타변수")) == 0) {
			_ArgGrid.SetArg(argGrp);
		}
	}

	_System->EnableTrailStop() ? _CheckTs.SetCheck(BST_CHECKED) : _CheckTs.SetCheck(BST_UNCHECKED);
	_System->EnableLossCut() ? _ChecktLossCut.SetCheck(BST_CHECKED) : _ChecktLossCut.SetCheck(BST_UNCHECKED);
	_System->EnableTargetCut() ? _CheckTargetCut.SetCheck(BST_CHECKED) : _CheckTargetCut.SetCheck(BST_UNCHECKED);

	if (_System->LossCutType() == ValueType::Tick) {
		_ComboLosscutType.SetCurSel(0);
		_LossCutType = ValueType::Tick;
	}
	else {
		_ComboLosscutType.SetCurSel(1);
		_LossCutType = ValueType::Percent;
	}

	if (_System->TargetProfitType() == ValueType::Tick) {
		_ComboProfitTarget.SetCurSel(0);
		_TargetProfitType = ValueType::Tick;
	}
	else {
		_ComboProfitTarget.SetCurSel(1);
		_TargetProfitType = ValueType::Percent;
	}
	
	_EnableTrailStop = _System->EnableTrailStop();
	_EnableLossCut = _System->EnableLossCut();
	_EnableTargetCut = _System->EnableTargetCut();

	CString value;
	value.Format(_T("%.0f"), _System->TrailStop().MinProfit);
	_EditTsPl.SetWindowText(value);
	value.Format(_T("%.0f"), _System->TrailStop().TrailingPercent);
	_EditTsPercent.SetWindowText(value);
	afx_msg void OnBnClickedCheckProfitTarget();
	_TargetProfitType = _System->TargetProfitType();
	value.Format(_T("%.0f"), _System->LossCut());
	_EditLosscut.SetWindowText(value);
	value.Format(_T("%.0f"), _System->TargetProfit());
	_EditProfitTarget.SetWindowText(value);

	value.Format(_T("%d"), _System->MaxEntrance());
	_EditEntMax.SetWindowText(value);

	

	COleDateTime NewDate;
	CTime curTime = CTime::GetCurrentTime();
	VtTime esTime = _System->EntranceStartTime();
	NewDate.SetDateTime(curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), esTime.hour, esTime.min, esTime.sec);
	_DpEntBegin.SetTime(NewDate);

	esTime = _System->EntranceEndTime();
	NewDate.SetDateTime(curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), esTime.hour, esTime.min, esTime.sec);
	_DpEntEnd.SetTime(NewDate);

	esTime = _System->LiqTime();
	NewDate.SetDateTime(curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), esTime.hour, esTime.min, esTime.sec);
	_DpLiq.SetTime(NewDate);
}

int VtUsdStrategyConfigDlg::FindSystemIndex(std::string sysName)
{
	for (auto it = _SystemMap.begin(); it != _SystemMap.end(); ++it) {
		auto item = it->second;
		auto sys = std::get<1>(item);
		if (sys->Name().compare(sysName) == 0) {
			return it->first;
		}
	}

	return -1;
}

void VtUsdStrategyConfigDlg::OnBnClickedBtnAccount()
{
	VtAccountFundSelector dlg;
	dlg.SetConfigDlg(this);
	dlg.DoModal();
}


void VtUsdStrategyConfigDlg::OnBnClickedBtnSymbol()
{
	HdSymbolSelecter dlg;
	dlg.SetConfigDlg(this);
	dlg.DoModal();
}

void VtUsdStrategyConfigDlg::SetTargetAcntOrFund(std::tuple<int, VtAccount*, VtFund*>& selItem)
{
	_Type = std::get<0>(selItem);
	if (_Type == 0 || _Type == 1) {
		_Account = std::get<1>(selItem);
		_StaticAccount.SetWindowText(_Account->AccountNo.c_str());
		_Fund = nullptr;
	}
	else {
		_Fund = std::get<2>(selItem);
		_StaticAccount.SetWindowText(_Fund->Name.c_str());
		_Account = nullptr;
	}
}


void VtUsdStrategyConfigDlg::OnCbnSelchangeComboStrategy()
{
	_SelSysIndex = _ComboStrategy.GetCurSel();
	auto it = _SystemMap.find(_SelSysIndex);
	if (it != _SystemMap.end()) {
		_System = std::get<1>(it->second);
		InitControls();
	}
}

void VtUsdStrategyConfigDlg::SetSymbol(VtSymbol* sym)
{
	if (!sym)
		return;
	_SelSymbol = sym;
	_StaticSymbol.SetWindowText(sym->ShortCode.c_str());
}


void VtUsdStrategyConfigDlg::OnBnClickedBtnExtraVar()
{
	ResizeWindow();
}


void VtUsdStrategyConfigDlg::PostNcDestroy()
{
	if (_StGrid) {
		_StGrid->RemoveDlg(this);
	}
	// TODO: Add your specialized code here and/or call the base class
	//delete this;
	CDialogEx::PostNcDestroy();
}


void VtUsdStrategyConfigDlg::OnBnClickedBtnApply()
{
	if (!_System)
		return;
	VtRealtimeRegisterManager* realtimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	//realtimeRegiMgr->RegisterProduct(_Symbol->ShortCode);
	if (_Type == 0 || _Type == 1) {
		if (_Account) {
			_System->Account(_Account);
			_Type == 0 ? _System->SysTargetType(TargetType::RealAccount) : _System->SysTargetType(TargetType::SubAccount);
			_System->SysTargetName(_Account->AccountNo);
			_System->Fund(nullptr);
			if (_Account->AccountLevel() == 0) {
				realtimeRegiMgr->RegisterAccount(_Account->AccountNo);
			} 
			else {
				VtAccount* parentAcnt = _Account->ParentAccount();
				if (parentAcnt) {
					realtimeRegiMgr->RegisterAccount(parentAcnt->AccountNo);
				}
			}
		}
	}
	else {
		if (_Fund) {
			_System->Fund(_Fund);
			_System->SysTargetType(TargetType::Fund);
			_System->SysTargetName(_Fund->Name);
			_System->Account(nullptr);
			std::set<VtAccount*> parendAcntSet = _Fund->GetParentAccountSet();
			for (auto it = parendAcntSet.begin(); it != parendAcntSet.end(); ++it) {
				VtAccount* parentAcnt = *it;
				realtimeRegiMgr->RegisterAccount(parentAcnt->AccountNo);
			}
		}
	}

	if (_SelSymbol) {
		_System->Symbol(_SelSymbol);
		_System->SymbolCode(_SelSymbol->ShortCode);
		realtimeRegiMgr->RegisterProduct(_SelSymbol->ShortCode);
	}
	
	CTime esTime;
	_DpEntBegin.GetTime(esTime);
	VtTime& startTime = _System->EntranceStartTime();
	startTime.hour = esTime.GetHour();
	startTime.min = esTime.GetMinute();
	startTime.sec = esTime.GetSecond();

	_DpEntEnd.GetTime(esTime);
	VtTime& endTime  = _System->EntranceEndTime();
	endTime.hour = esTime.GetHour();
	endTime.min = esTime.GetMinute();
	endTime.sec = esTime.GetSecond();

	_DpLiq.GetTime(esTime);
	VtTime& ligTime = _System->LiqTime();
	ligTime.hour = esTime.GetHour();
	ligTime.min = esTime.GetMinute();
	ligTime.sec = esTime.GetSecond();

	_System->EnableTrailStop(_EnableTrailStop);
	_System->EnableLossCut(_EnableLossCut);
	_System->EnableTargetCut(_EnableTargetCut);

	_System->LiqByStop(_LiqByStop);

	CString value;
	_EditEntMax.GetWindowText(value);
	_System->MaxEntrance(_ttoi(value));

	_EditOrderAmt.GetWindowText(value);
	_System->OrderAmount(_ttoi(value));

	TrailingStop& ts = _System->TrailStop();
	_EditTsPl.GetWindowText(value);
	ts.MinProfit = _ttoi(value);
	_EditTsPercent.GetWindowText(value);
	ts.TrailingPercent = _ttof(value);

	_EditLosscut.GetWindowText(value);
	_System->LossCut(_ttof(value));
	_EditProfitTarget.GetWindowText(value);
	_System->TargetProfit(_ttof(value));

	_System->LossCutType(_LossCutType);
	_System->TargetProfitType(_TargetProfitType);


	_ShowingGrid = true;

	CUGCell cell;
	VtSystemArgGroup* argGrp = _System->GetArgGroup(_T("매수진입"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (size_t i = 0; i < argVec.size(); ++i) {
			VtSystemArg& arg = argVec[i];
			CString strValue;
			_EntGrid.QuickGetText(2, i, &strValue);
			arg.sValue = (LPCTSTR)strValue;
			_EntGrid.GetCell(1, i, &cell);
			double num = cell.GetNumber();
			num == 1.0 ? arg.Enable = true : arg.Enable = false;
		}
	}

	argGrp = _System->GetArgGroup(_T("매도진입"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (size_t i = 0; i < argVec.size(); ++i) {
			VtSystemArg& arg = argVec[i];
			CString strValue;
			_EntGrid.QuickGetText(7, i, &strValue);
			arg.sValue = (LPCTSTR)strValue;
			_EntGrid.GetCell(6, i, &cell);
			double num = cell.GetNumber();
			num == 1.0 ? arg.Enable = true : arg.Enable = false;
		}
	}

	argGrp = _System->GetArgGroup(_T("매수청산"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (size_t i = 0; i < argVec.size(); ++i) {
			VtSystemArg& arg = argVec[i];
			CString strValue;
			_LiqGrid.QuickGetText(2, i, &strValue);
			arg.sValue = (LPCTSTR)strValue;
			_LiqGrid.GetCell(1, i, &cell);
			double num = cell.GetNumber();
			num == 1.0 ? arg.Enable = true : arg.Enable = false;
		}
	}

	argGrp = _System->GetArgGroup(_T("매도청산"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (size_t i = 0; i < argVec.size(); ++i) {
			VtSystemArg& arg = argVec[i];
			CString strValue;
			_LiqGrid.QuickGetText(7, i, &strValue);
			arg.sValue = (LPCTSTR)strValue;
			_LiqGrid.GetCell(6, i, &cell);
			double num = cell.GetNumber();
			num == 1.0 ? arg.Enable = true : arg.Enable = false;
		}
	}

	_ShowingGrid = false;

	argGrp = _System->GetArgGroup(_T("기타변수"));
	if (argGrp) {
		std::vector<VtSystemArg>& argVec = argGrp->GetArgVec();
		for (size_t i = 0; i < argVec.size(); ++i) {
			VtSystemArg& arg = argVec[i];
			int row = (int)(i / 2);
			if (i % 2 == 0) {
				CString strValue;
				_ArgGrid.QuickGetText(2, row, &strValue);
				arg.sValue = (LPCTSTR)strValue;
				_ArgGrid.GetCell(0, row, &cell);
				double num = cell.GetNumber();
				num == 1.0 ? arg.Enable = true : arg.Enable = false;
			}
			else {
				CString strValue;
				_ArgGrid.QuickGetText(5, row, &strValue);
				arg.sValue = (LPCTSTR)strValue;
				_ArgGrid.GetCell(3, row, &cell);
				double num = cell.GetNumber();
				num == 1.0 ? arg.Enable = true : arg.Enable = false;
			}
		}
	}

	// 기타 변수를 시스템에 로드해 준다.
	_System->ReadExtraArgs();
}


void VtUsdStrategyConfigDlg::OnCbnSelchangeComboLosscutType()
{
	if (_ComboLosscutType.GetCurSel() == 0) {
		_LossCutType = ValueType::Tick;
	}
	else {
		_LossCutType = ValueType::Percent;
	}
}


void VtUsdStrategyConfigDlg::OnCbnSelchangeComboProfitTarget()
{
	if (_ComboProfitTarget.GetCurSel() == 0) {
		_TargetProfitType = ValueType::Tick;
	}
	else {
		_TargetProfitType = ValueType::Percent;
	}
}


void VtUsdStrategyConfigDlg::SetSystem(VtSystem* sys)
{
	if (!sys)
		return;
	_System = sys;
	_Type = (int)sys->SysTargetType();
	_SelSymbol = sys->Symbol();
	if (_Type == 0) {
		_Account = sys->Account();
	}
	else if (_Type == 1) {
		_Account = sys->Account();
	}
	else if (_Type == 2) {
		_Fund = sys->Fund();
	}
	InitControls();
}

void VtUsdStrategyConfigDlg::OnBnClickedCheckTs()
{
	_CheckTs.GetCheck() == BST_CHECKED ? _EnableTrailStop = true : _EnableTrailStop = false;
}


void VtUsdStrategyConfigDlg::OnBnClickedCheckLosscut()
{
	_ChecktLossCut.GetCheck() == BST_CHECKED ? _EnableLossCut = true : _EnableLossCut = false;
}


void VtUsdStrategyConfigDlg::OnBnClickedCheckProfitTarget()
{
	_CheckTargetCut.GetCheck() == BST_CHECKED ? _EnableTargetCut = true : _EnableTargetCut = false;
}


void VtUsdStrategyConfigDlg::OnBnClickedCkLiqAll()
{
	_CheckLiqByStop.GetCheck() == BST_CHECKED ? _LiqByStop = true : _LiqByStop = false;
}

void VtUsdStrategyConfigDlg::OnHogaCount(double uac, double ubc)
{
	if (_ShowingGrid)
		return;
	_ShowingGrid = true;
	_EntGrid.SetHogaCountEntRatio(uac, ubc);
	_ShowingGrid = false;
}

void VtUsdStrategyConfigDlg::OnHogaQty(double uas, double ubs)
{
	if (_ShowingGrid)
		return;
	_ShowingGrid = true;
	_EntGrid.SetHogaQtyEntRatio(uas, ubs);
	_ShowingGrid = false;
}


void VtUsdStrategyConfigDlg::OnClose()
{
	for (auto it = _SystemMap.begin(); it != _SystemMap.end(); ++it) {
		auto item = it->second;
		VtSystem* sys = std::get<1>(item);
		//sys->ShowRealtime(false);
	}
	KillTimer(ArgTimer);
	VtSystemManager* sysMgr = VtSystemManager::GetInstance();
	sysMgr->RemoveSystemDialog(this);

	CDialogEx::OnClose();

	HdWindowEventArgs arg;
	arg.pWnd = this;
	arg.wndType = HdWindowType::StrategySetWindow;
	arg.eventType = HdWindowEventType::Closed;
	FireWindowEvent(std::move(arg));
}


void VtUsdStrategyConfigDlg::OnBnClickedCheckRun()
{
	if (VtGlobal::StrategyToolWnd) {
		VtGlobal::StrategyToolWnd->UpdateSystem(_System, _CheckRun.GetCheck() == BST_CHECKED ? true : false);
	}
	VtStrategyWndManager* stgWndMgr = VtStrategyWndManager::GetInstance();
	stgWndMgr->UpdateDialog(_System);
	if (_System) {
		_System->SetDataSrc();
		_System->RegisterRealtimeAccountEvent();
	}
}

void VtUsdStrategyConfigDlg::OnRealTimeEvent()
{
	if (!_System)
		return;

	std::string temp = NumberFormatter::format(_System->ProfitLoss(), 0);
	CString profitLoss = XFormatNumber(temp.c_str(), -1);

	_StaticPl.SetWindowText(profitLoss);

	CString profit;
	profit.Format(_T("%d"), _System->EntryToday());
	_EntCntToday.SetWindowText(profit);

	if (_System->Symbol()) {
		temp = NumberFormatter::format(_System->LatestEntPrice() / std::pow(10, _System->Symbol()->IntDecimal), _System->Symbol()->IntDecimal);
		profitLoss = XFormatNumber(temp.c_str(), _System->Symbol()->IntDecimal);
	}
	else {
		profitLoss.Format(_T("%d"), _System->LatestEntPrice());
	}
	_StaticLastEntPrice.SetWindowText(profitLoss);
	if (_System->PositionState.Find(_T("매수")) >= 0) {
		_StaticPosition.SetTextColor(RGB(255, 255, 255));
		_StaticPosition.SetColor(RGB(240, 51, 58));
		_StaticPosition.SetGradientColor(RGB(240, 51, 58));
	} 
	else if (_System->PositionState.Find(_T("매도")) >= 0) {
		_StaticPosition.SetTextColor(RGB(255, 255, 255));
		_StaticPosition.SetColor(RGB(19, 137, 255));
		_StaticPosition.SetGradientColor(RGB(19, 137, 255));
	}
	else {
		_StaticPosition.SetTextColor(GetSysColor(COLOR_3DFACE));
		_StaticPosition.SetColor(GetSysColor(COLOR_3DFACE));
		_StaticPosition.SetGradientColor(GetSysColor(COLOR_3DFACE));
	}
	_StaticPosition.SetWindowText(_System->PositionState);


	VtDate curDate = VtGlobal::GetLocalDate();
	VtTime curTime = VtGlobal::GetLocalTime();
	CString td;
	td.Format(_T("%4d-%02d-%02d %02d:%02d:%02d"), curDate, curTime);

	std::string title = _T("전략설정창 - ");
	title.append(_System->Name());
	title.append(_T(" - "));
	title.append(std::to_string(_System->Cycle()));
	title.append(_T("M"));
	title.append(_T("    "));
	title.append(td);
	SetWindowText(title.c_str());
}

void VtUsdStrategyConfigDlg::RefreshRealTimeValue(std::string argName, CString value)
{
	_EntGrid.RefreshRealTimeValue(argName, value);
	_LiqGrid.RefreshRealTimeValue(argName, value);
}

void VtUsdStrategyConfigDlg::RefreshRealTimeValue(int index, std::string argName, CString value)
{
	index == 0 ? _EntGrid.RefreshRealTimeValue(argName, value) : _LiqGrid.RefreshRealTimeValue(argName, value);
}


void VtUsdStrategyConfigDlg::RefreshRealTimeValue(std::string argName, double val)
{
	CString value;
	value.Format(_T("%.0f"), val);
	RefreshRealTimeValue(argName, value);
}

void VtUsdStrategyConfigDlg::RefreshRealTimeValue(std::string argName, double val, int decimal)
{
	CString value, format;
	if (decimal == 0)
		value.Format(_T("%.0f"), val);
	else
		value.Format(_T("%.2f"), val);
	RefreshRealTimeValue(argName, value);
}

void VtUsdStrategyConfigDlg::OnEnChangeEditOrderAmt()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void VtUsdStrategyConfigDlg::UpdateRunCheck(VtSystem* sys)
{
	if (!sys || _System != sys)
		return;
	sys->Enable() ? _CheckRun.SetCheck(BST_CHECKED) : _CheckRun.SetCheck(BST_UNCHECKED);
}


void VtUsdStrategyConfigDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == ArgTimer) {
		VtSystemManager* sysMgr = VtSystemManager::GetInstance();
		double _KbsMiKas = sysMgr->Kbs - sysMgr->Kas;
		RefreshRealTimeValue(_T("Kbs-Kas"), _KbsMiKas, 0);
		double _KasMiKbs = sysMgr->Kas - sysMgr->Kbs;
		RefreshRealTimeValue(_T("Kas-Kbs"), _KasMiKbs, 0);

		double _KbsGtKas = sysMgr->Kas == 0 ? 0 : sysMgr->Kbs / sysMgr->Kas;
		RefreshRealTimeValue(_T("Kbs>Kas"), _KbsGtKas, 2);
		double _KasGtKbs = sysMgr->Kbs == 0 ? 0 : sysMgr->Kas / sysMgr->Kbs;
		RefreshRealTimeValue(_T("Kas>Kbs"), _KasGtKbs, 2);

		double _KbcGtKac = sysMgr->Kbc == 0 ? 0 : sysMgr->Kac / sysMgr->Kbc;
		RefreshRealTimeValue(_T("Kbc>Kac"), _KbcGtKac, 2);
		double _KacGtKbc = sysMgr->Kac == 0 ? 0 : sysMgr->Kbc / sysMgr->Kac;
		RefreshRealTimeValue(_T("Kac>Kbc"), _KacGtKbc, 2);
		double _QbsGtQas = sysMgr->Qbs == 0 ? 0 : sysMgr->Qas / sysMgr->Qbs;
		RefreshRealTimeValue(_T("Qbs>Qas"), _QbsGtQas, 2);
		double _QasGtQbs = sysMgr->Qas == 0 ? 0 : sysMgr->Qbs / sysMgr->Qas;
		RefreshRealTimeValue(_T("Qas>Qbs"), _QasGtQbs, 2);
		double _QbcGtQac = sysMgr->Qbc == 0 ? 0 : sysMgr->Qac / sysMgr->Qbc;
		RefreshRealTimeValue(_T("Qbc>Qac"), _QbcGtQac, 2);
		double _QacGtQbc = sysMgr->Qac == 0 ? 0 : sysMgr->Qbc / sysMgr->Qac;
		RefreshRealTimeValue(_T("Qac>Qbc"), _QacGtQbc, 2);
		
		double _UbsGtUas = sysMgr->Ubs == 0 ? 0 : sysMgr->Uas / sysMgr->Ubs;
		RefreshRealTimeValue(_T("Ubs>Uas"), _UbsGtUas, 2);
		double _UasGtUbs = sysMgr->Uas == 0 ? 0 : sysMgr->Ubs / sysMgr->Uas;
		RefreshRealTimeValue(_T("Uas>Ubs"), _UasGtUbs, 2);
		double _UbcGtUac = sysMgr->Ubc == 0 ? 0 : sysMgr->Uac / sysMgr->Ubc;
		RefreshRealTimeValue(_T("Ubc>Uac"), _UbcGtUac, 2);
		double _UacGtUbc = sysMgr->Uac == 0 ? 0 : sysMgr->Ubc / sysMgr->Uac;
		RefreshRealTimeValue(_T("Uac>Ubc"), _UacGtUbc, 2);

		OnRealTimeEvent();
	}

	CDialogEx::OnTimer(nIDEvent);
}
