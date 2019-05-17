// VtSystemSetMonthsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtSystemSetMonthsDialog.h"
#include "afxdialogex.h"
#include "System/VtSystemManager.h"
#include "VtProductCategoryManager.h"
#include "VtSymbol.h"


// VtSystemSetMonthsDialog dialog

IMPLEMENT_DYNAMIC(VtSystemSetMonthsDialog, CDialogEx)

VtSystemSetMonthsDialog::VtSystemSetMonthsDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SYS_SET_MONTH, pParent)
{

}

VtSystemSetMonthsDialog::~VtSystemSetMonthsDialog()
{
}

void VtSystemSetMonthsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_KP, _ComboKp);
	DDX_Control(pDX, IDC_COMBO_KQ, _ComboKq);
	DDX_Control(pDX, IDC_COMBO_UD, _ComboUd);
}


BEGIN_MESSAGE_MAP(VtSystemSetMonthsDialog, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_KP, &VtSystemSetMonthsDialog::OnCbnSelchangeComboKp)
	ON_CBN_SELCHANGE(IDC_COMBO_KQ, &VtSystemSetMonthsDialog::OnCbnSelchangeComboKq)
	ON_CBN_SELCHANGE(IDC_COMBO_UD, &VtSystemSetMonthsDialog::OnCbnSelchangeComboUd)
END_MESSAGE_MAP()


// VtSystemSetMonthsDialog message handlers


BOOL VtSystemSetMonthsDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	VtProductCategoryManager* catMgr = VtProductCategoryManager::GetInstance();
	std::vector<VtSymbol*> symVecKp = catMgr->GetSectionSymbolVector(_T("101F"));
	std::string itemTile;
	for (auto it = symVecKp.begin(); it != symVecKp.end(); ++it) {
		VtSymbol* sym = *it;
		itemTile = sym->ShortCode;
		itemTile.append(_T("  "));
		itemTile.append(sym->Name);
		int index = _ComboKp.AddString(itemTile.c_str());
		_ComboKp.SetItemDataPtr(index, (void*)sym);
		_KpMap[sym->ShortCode] = index;
	}

	std::vector<VtSymbol*> symVecKq = catMgr->GetSectionSymbolVector(_T("106F"));
	for (auto it = symVecKq.begin(); it != symVecKq.end(); ++it) {
		VtSymbol* sym = *it;
		itemTile = sym->ShortCode;
		itemTile.append(_T("  "));
		itemTile.append(sym->Name);
		int index = _ComboKq.AddString(itemTile.c_str());
		_ComboKq.SetItemDataPtr(index, (void*)sym);
		_KqMap[sym->ShortCode] = index;
	}

	std::vector<VtSymbol*> symVecUd = catMgr->GetSectionSymbolVector(_T("175F"));
	for (auto it = symVecUd.begin(); it != symVecUd.end(); ++it) {
		VtSymbol* sym = *it;
		itemTile = sym->ShortCode;
		itemTile.append(_T("  "));
		itemTile.append(sym->Name);
		int index = _ComboUd.AddString(itemTile.c_str());
		_ComboUd.SetItemDataPtr(index, (void*)sym);
		_UdMap[sym->ShortCode] = index;
	}

	VtSystemManager* sysMgr = VtSystemManager::GetInstance();
	std::map<std::string, VtSymbol*> argMap = sysMgr->GetArgMap();
	for (auto it = argMap.begin(); it != argMap.end(); ++it) {
		std::string symCode = it->first;
		if (symCode.find(_T("101")) != std::string::npos) {
			auto it = _KpMap.find(symCode);
			if (it != _KpMap.end()) {
				_ComboKp.SetCurSel(it->second);
			}
		}
		else if (symCode.find(_T("106")) != std::string::npos) {
			auto it = _KqMap.find(symCode);
			if (it != _KqMap.end()) {
				_ComboKq.SetCurSel(it->second);
			}
		}
		else if (symCode.find(_T("175")) != std::string::npos) {
			auto it = _UdMap.find(symCode);
			if (it != _UdMap.end()) {
				_ComboUd.SetCurSel(it->second);
			}
		}
	}

	_ComboKp.SetDroppedWidth(200);
	_ComboKq.SetDroppedWidth(200);
	_ComboUd.SetDroppedWidth(200);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void VtSystemSetMonthsDialog::OnCbnSelchangeComboKp()
{
	int curSel = _ComboKp.GetCurSel();
	if (curSel == -1)
		return;

	VtSystemManager* sysMgr = VtSystemManager::GetInstance();
	VtSymbol* sym = (VtSymbol*)_ComboKp.GetItemDataPtr(curSel);
	if (sym) {
		sysMgr->ReplaceSymbol(sym);
	}
}


void VtSystemSetMonthsDialog::OnCbnSelchangeComboKq()
{
	// TODO: Add your control notification handler code here
}


void VtSystemSetMonthsDialog::OnCbnSelchangeComboUd()
{
	int curSel = _ComboUd.GetCurSel();
	if (curSel == -1)
		return;

	VtSystemManager* sysMgr = VtSystemManager::GetInstance();
	VtSymbol* sym = (VtSymbol*)_ComboUd.GetItemDataPtr(curSel);
	if (sym) {
		sysMgr->ReplaceSymbol(sym);
	}
}
