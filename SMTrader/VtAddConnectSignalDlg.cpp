// VtAddConnectSignalDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtAddConnectSignalDlg.h"
#include "afxdialogex.h"
#include "VtAccountManager.h"
#include "VtSubAccountManager.h"
#include "VtFundManager.h"
#include "VtFund.h"
#include "VtAccount.h"
#include "HdSymbolSelecter.h"
#include "VtSymbol.h"
#include "VtOutSignalDefManager.h"
#include "VtOutSignalDef.h"
#include "VtOutSystemManager.h"
#include "Global/VtDefine.h"
#include "System/VtSystem.h"
#include "VtSignalConnectionGrid.h"
// VtAddConnectSignalDlg dialog

IMPLEMENT_DYNAMIC(VtAddConnectSignalDlg, CDialogEx)

VtAddConnectSignalDlg::VtAddConnectSignalDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ADD_SIG_CONNECT, pParent)
{

}

VtAddConnectSignalDlg::~VtAddConnectSignalDlg()
{
}

void VtAddConnectSignalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ACNT, _ComboAcnt);
	DDX_Control(pDX, IDC_COMBO_SIGNAL, _ComboSignal);
	DDX_Control(pDX, IDC_COMBO_SYMBOL, _ComboSymbol);
	DDX_Control(pDX, IDC_COMBO_TYPE, _ComboType);
	DDX_Control(pDX, IDC_EDIT_SEUNGSU, _EditSeungsu);
	DDX_Control(pDX, IDC_SPIN_SEUNGSU, _SpinSeungsu);
}


BEGIN_MESSAGE_MAP(VtAddConnectSignalDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &VtAddConnectSignalDlg::OnCbnSelchangeComboType)
	ON_CBN_SELCHANGE(IDC_COMBO_ACNT, &VtAddConnectSignalDlg::OnCbnSelchangeComboAcnt)
	ON_CBN_SELCHANGE(IDC_COMBO_SYMBOL, &VtAddConnectSignalDlg::OnCbnSelchangeComboSymbol)
	ON_BN_CLICKED(IDC_BTN_FIND_SYMBOL, &VtAddConnectSignalDlg::OnBnClickedBtnFindSymbol)
	ON_CBN_SELCHANGE(IDC_COMBO_SIGNAL, &VtAddConnectSignalDlg::OnCbnSelchangeComboSignal)
	ON_BN_CLICKED(IDC_BTN_OK, &VtAddConnectSignalDlg::OnBnClickedBtnOk)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &VtAddConnectSignalDlg::OnBnClickedBtnCancel)
END_MESSAGE_MAP()


// VtAddConnectSignalDlg message handlers


void VtAddConnectSignalDlg::OnCbnSelchangeComboType()
{
	int curSel = _ComboType.GetCurSel();
	if (curSel != -1) {
		_Mode = curSel;
		InitCombo();
	}
}


void VtAddConnectSignalDlg::OnCbnSelchangeComboAcnt()
{
	int selIndex = _ComboAcnt.GetCurSel();
	if (selIndex != -1) {
		if (_Mode == 0)
			_Acnt = (VtAccount*)_ComboAcnt.GetItemDataPtr(selIndex);
		else 
			_Fund = (VtFund*)_ComboAcnt.GetItemDataPtr(selIndex);
	}
}


void VtAddConnectSignalDlg::OnCbnSelchangeComboSymbol()
{
	int selIndex = _ComboSymbol.GetCurSel();
	if (selIndex != -1) {
		_Symbol = (VtSymbol*)_ComboSymbol.GetItemDataPtr(selIndex);
	}
}


void VtAddConnectSignalDlg::OnBnClickedBtnFindSymbol()
{
	HdSymbolSelecter dlg;
	dlg.SetAddConSigDlg(this);
	dlg.DoModal();
}


void VtAddConnectSignalDlg::OnCbnSelchangeComboSignal()
{
	int selIndex = _ComboSignal.GetCurSel();
	if (selIndex != -1) {
		VtOutSignalDefManager* outSigDefMgr = VtOutSignalDefManager::GetInstance();
		CString strName;
		_ComboSignal.GetLBText(selIndex, strName);
		SharedOutSigDef sig = outSigDefMgr->FindOutSigDef((LPCTSTR)strName);
		sig ? _Signal = sig : _Signal = nullptr;
	}
}


void VtAddConnectSignalDlg::OnBnClickedBtnOk()
{
	SharedSystem sys = std::make_shared<VtSystem>(VtSystemType::SYS_OUT);
	if (_Mode == 0) {
		if (_Acnt) {
			_Acnt->AccountLevel() == 0 ? sys->SysTargetType(TargetType::RealAccount) : sys->SysTargetType(TargetType::SubAccount);
			sys->Account(_Acnt);
		}
	}
	else {
		if (_Fund) {
			sys->SysTargetType(TargetType::Fund);
			sys->Fund(_Fund);
		}
	}

	if (_Symbol) sys->Symbol(_Symbol);

	if (_Signal) sys->OutSignal(_Signal);

	CString strSeungSu;
	_EditSeungsu.GetWindowText(strSeungSu);
	sys->SeungSu(_ttoi(strSeungSu));

	if (_SigConGrid) {
		_SigConGrid->AddSystem(sys);
	}
	CDialogEx::OnOK();
}


void VtAddConnectSignalDlg::OnBnClickedBtnCancel()
{
	CDialogEx::OnCancel();
}


BOOL VtAddConnectSignalDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	_ComboType.AddString(_T("°èÁÂ"));
	_ComboType.AddString(_T("ÆÝµå"));
	_Mode = 0;
	_ComboType.SetCurSel(0);
	_EditSeungsu.SetWindowText(_T("1"));
	_SpinSeungsu.SetRange(0, 100);
	InitCombo();
	InitOutSigDefCombo();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void VtAddConnectSignalDlg::InitCombo()
{
	_ComboAcnt.ResetContent();
	int index = -1;
	if (_Mode == 0) {
		VtAccountManager* acntMgr = VtAccountManager::GetInstance();
		for (auto it = acntMgr->AccountMap.begin(); it != acntMgr->AccountMap.end(); ++it) {
			VtAccount* acnt = it->second;
			index = _ComboAcnt.AddString(acnt->AccountNo.c_str());
			if (index == 0) _Acnt = acnt;
			_ComboAcnt.SetItemDataPtr(index, acnt);
		}

		VtSubAccountManager* subAcntMgr = VtSubAccountManager::GetInstance();
		for (auto it = subAcntMgr->AccountMap.begin(); it != subAcntMgr->AccountMap.end(); ++it) {
			VtAccount* acnt = it->second;
			index = _ComboAcnt.AddString(acnt->AccountNo.c_str());
			_ComboAcnt.SetItemDataPtr(index, acnt);
		}
	}
	else {
		VtFundManager* fundMgr = VtFundManager::GetInstance();
		std::map<std::string, VtFund*>& fundList = fundMgr->GetFundList();
		for (auto it = fundList.begin(); it != fundList.end(); ++it) {
			VtFund* fund = it->second;
			index = _ComboAcnt.AddString(fund->Name.c_str());
			if (index == 0) _Fund = fund;
			_ComboAcnt.SetItemDataPtr(index, fund);
		}
	}

	if (index != -1)
		_ComboAcnt.SetCurSel(0);
}

void VtAddConnectSignalDlg::InitOutSigDefCombo()
{
	VtOutSignalDefManager* outSigDefMgr = VtOutSignalDefManager::GetInstance();
	OutSigDefVec& sigDefVec = outSigDefMgr->GetSignalDefVec();
	int selIndex = -1;
	for (auto it = sigDefVec.begin(); it != sigDefVec.end(); ++it) {
		SharedOutSigDef& sig = *it;
		selIndex = _ComboSignal.AddString(sig->SignalName.c_str());
	}
	if (selIndex != -1) {
		_ComboSignal.SetCurSel(0);
		_Signal = sigDefVec[selIndex];
	}
}

void VtAddConnectSignalDlg::SetSymbol(VtSymbol* sym)
{
	if (!sym)
		return;
	int index = _ComboSymbol.AddString(sym->ShortCode.c_str());
	_ComboSymbol.SetItemDataPtr(index, sym);
	_ComboSymbol.SetCurSel(index);
	_Symbol = sym;
}
