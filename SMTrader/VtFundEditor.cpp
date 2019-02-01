// VtFundEditor.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtFundEditor.h"
#include "afxdialogex.h"
#include "VtFund.h"
#include "VtAccountManager.h"
#include "VtFundAccount.h"
#include "VtAccount.h"
#include "VtFundManager.h"
#include "VtFundOrderManagerSelecter.h"
#include "VtFundOrderManager.h"
#include <set>
#include <algorithm>
#include "VtGlobal.h"
// VtFundEditor dialog
extern TApplicationFont g_Font;

IMPLEMENT_DYNAMIC(VtFundEditor, CDialogEx)

VtFundEditor::VtFundEditor(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FUND_EDITOR, pParent)
{
	_SelFund = nullptr;
	_SelSourceFundAcnt = nullptr;
	_SelTargetFundAcnt = nullptr;
	_SelectedTargetRow = 0;
	_Editing = false;
}

VtFundEditor::~VtFundEditor()
{
	
}

void VtFundEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FUND_NAME, _EditFundName);
	DDX_Control(pDX, IDC_EDIT_SEUNSU, _EditSeungSu);
	DDX_Control(pDX, IDC_STATIC_ACNT_NAME, _StaticAcntName);
	DDX_Control(pDX, IDC_STATIC_RATIO, _StaticRatio);
	DDX_Control(pDX, IDC_STATIC_SUM_RATIO, _StaticSumRatio);
	DDX_Control(pDX, IDC_STATIC_SUM_SEUNGSU, _StaticSumSeungSu);
}


BEGIN_MESSAGE_MAP(VtFundEditor, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_ACNT_IN, &VtFundEditor::OnBnClickedBtnAcntIn)
	ON_BN_CLICKED(IDC_BTN_ACNT_IN_ALL, &VtFundEditor::OnBnClickedBtnAcntInAll)
	ON_BN_CLICKED(IDC_BTN_ACNT_OUT, &VtFundEditor::OnBnClickedBtnAcntOut)
	ON_BN_CLICKED(IDC_BTN_ACNT_OUT_ALL, &VtFundEditor::OnBnClickedBtnAcntOutAll)
	ON_BN_CLICKED(IDC_BTN_UP, &VtFundEditor::OnBnClickedBtnUp)
	ON_BN_CLICKED(IDC_BTN_DOWN, &VtFundEditor::OnBnClickedBtnDown)
	ON_BN_CLICKED(IDC_BTN_OK, &VtFundEditor::OnBnClickedBtnOk)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &VtFundEditor::OnBnClickedBtnCancel)
	ON_EN_CHANGE(IDC_EDIT_SEUNSU, &VtFundEditor::OnEnChangeEditSeunsu)
	ON_EN_UPDATE(IDC_EDIT_SEUNSU, &VtFundEditor::OnEnUpdateEditSeunsu)
END_MESSAGE_MAP()


// VtFundEditor message handlers


BOOL VtFundEditor::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	this->SetIcon(hIcon, FALSE);

	::EnumChildWindows(m_hWnd, ::SetChildFont, (LPARAM)g_Font.GetFont());
	// TODO:  Add extra initialization here
	_FundGrid.Mode(1);
	_FundGrid.FundEditor(this);
	_AcntGrid.FundEditor(this);
	_FundGrid.AttachGrid(this, IDC_STATIC_ACNT_TARGET);
	_AcntGrid.AttachGrid(this, IDC_STATIC_ACNT_SOURCE);
	InitGrid();
	if (_SelFund)
	{
		_EditFundName.SetWindowText(_SelFund->Name.c_str());
	}
	SetTargetSumInfo();
	SetSelectedFundAccountInfo();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void VtFundEditor::InitGrid()
{
	if (!_SelFund)
		return;

	// Set the target list first
	VtFundMap& fundMap = _SelFund->GetFundMap();
	std::vector<VtAccount*>& targetAcntVector = fundMap.GetAccountVector();
	TargetVector.assign(targetAcntVector.begin(), targetAcntVector.end());

	VtFundManager* fundMgr = VtFundManager::GetInstance();
	fundMgr->DivideAccountFromFundList();
	std::map<std::string, VtAccount*>& unusedMap = fundMgr->GetUnusedAccountList();
	for (auto it = unusedMap.begin(); it != unusedMap.end(); ++it)
	{
		SourceVector.push_back(it->second);
	}
	_FundGrid.InitGrid(TargetVector);
	_AcntGrid.InitGrid(SourceVector);
}


void VtFundEditor::RefreshGrid()
{
	_FundGrid.ClearCells();
	_AcntGrid.ClearCells();
	_FundGrid.InitGrid(TargetVector);
	_AcntGrid.InitGrid(SourceVector);
}

void VtFundEditor::OnBnClickedBtnAcntIn()
{
	// TODO: Add your control notification handler code here
	if (!_SelSourceFundAcnt || !_SelFund || SourceVector.size() == 0)
		return;

	RemoveFromSource(_SelSourceFundAcnt->AccountNo);
	AddToTarget(_SelSourceFundAcnt);
	RefreshGrid();
	SetTargetSumInfo();
	SetSelectedFundAccountInfo();
}


void VtFundEditor::OnBnClickedBtnAcntInAll()
{
	if (!_SelFund || SourceVector.size() == 0)
		return;
	for (auto it = SourceVector.begin(); it != SourceVector.end(); ++it)
	{
		VtAccount* acnt = *it;
		AddToTarget(acnt);
	}
	SourceVector.clear();
	RefreshGrid();
	SetTargetSumInfo();
	SetSelectedFundAccountInfo();
}


void VtFundEditor::OnBnClickedBtnAcntOut()
{
	if (!_SelTargetFundAcnt || !_SelFund || TargetVector.size() == 0)
		return;

	RemoveFromTarget(_SelTargetFundAcnt->AccountNo);
	AddToSource(_SelTargetFundAcnt);
	RefreshGrid();
	SetTargetSumInfo();
	SetSelectedFundAccountInfo();
}


void VtFundEditor::OnBnClickedBtnAcntOutAll()
{
	if (!_SelFund || TargetVector.size() == 0)
		return;

	for (auto it = TargetVector.begin(); it != TargetVector.end(); ++it)
	{
		VtAccount* acnt = *it;
		// Set the default value of SeungSu to the account.
		acnt->SeungSu = 1;
		AddToSource(acnt);
	}
	TargetVector.clear();
	RefreshGrid();
	SetTargetSumInfo();
	SetSelectedFundAccountInfo();
}


void VtFundEditor::OnBnClickedBtnUp()
{
	int size = TargetVector.size();
	if (_SelectedTargetRow < 1 || _SelectedTargetRow >= size)
		return;

	std::swap(TargetVector[_SelectedTargetRow], TargetVector[_SelectedTargetRow - 1]);

	//_FundGrid.ClearCells();
	_FundGrid.ChangeSelectedRow(_SelectedTargetRow, _SelectedTargetRow - 1);
	_FundGrid.InitGrid(TargetVector);
}


void VtFundEditor::OnBnClickedBtnDown()
{
	int size = TargetVector.size();
	if (_SelectedTargetRow < 0 || _SelectedTargetRow >= size - 1)
		return;

	std::swap(TargetVector[_SelectedTargetRow], TargetVector[_SelectedTargetRow + 1]);

	_FundGrid.ChangeSelectedRow(_SelectedTargetRow, _SelectedTargetRow + 1);
	_FundGrid.InitGrid(TargetVector);
}


void VtFundEditor::OnBnClickedBtnOk()
{
	std::set<VtAccount*> srcSet;
	_SelFund->GetFundAccountList(srcSet);
	std::set<VtAccount*> tgtSet;
	std::copy(TargetVector.begin(), TargetVector.end(), inserter(tgtSet, tgtSet.begin()));

	std::set<VtAccount*> diff;

	std::set_difference(srcSet.begin(), srcSet.end(), tgtSet.begin(), tgtSet.end(),
		std::inserter(diff, diff.begin()));

	// 기존에 펀드에 속해있던 서브 계좌들
	// Remove the underlying sub account from the fund map of its parent account.
	for (auto it = diff.begin(); it != diff.end(); ++it)
	{
		VtAccount* subAcnt = *it;
		VtAccount* parentAcnt = subAcnt->ParentAccount();
		if (parentAcnt)
		{
			parentAcnt->RemoveFromFundMap(_SelFund->Name, subAcnt);
		}
		// 펀드와의 연결을 끊는다.
		subAcnt->Fund(nullptr);
	}

	// Add the underlying sub account to the fund map of its parent account.
	for (auto it = TargetVector.begin(); it != TargetVector.end(); ++it)
	{
		VtAccount* subAcnt = *it;
		VtAccount* parentAcnt = subAcnt->ParentAccount();
		if (parentAcnt)
		{
			parentAcnt->AddToFundMap(_SelFund->Name, subAcnt);
		}
	}

	_SelFund->ClearAccounts();
	for (auto it = TargetVector.begin(); it != TargetVector.end(); ++it)
	{
		VtAccount* subAcnt = *it;
		_SelFund->AddFundAccount(subAcnt);
		// 서브계좌와 펀드를 연결시켜 준다.
		subAcnt->Fund(_SelFund);
	}
	TargetVector.clear();

	SourceVector.clear();

	VtFundOrderManagerSelecter* fundOrderMgrSelecter = VtFundOrderManagerSelecter::GetInstance();
	VtFundOrderManager* fundOrderMgr = fundOrderMgrSelecter->FindOrderManager(_SelFund->Name);
	if (fundOrderMgr)
	{
		fundOrderMgr->UpdateOrderManagers();
	}

	EndDialog(IDOK);
}


void VtFundEditor::OnBnClickedBtnCancel()
{
	/*
	for (auto it = SourceVector.begin(); it != SourceVector.end(); ++it)
	{
		delete *it;
	}
	*/
	SourceVector.clear();
	TargetVector.clear();
	EndDialog(IDCANCEL);
}

void VtFundEditor::RemoveFromSource(std::string acntNo)
{
	for (auto it = SourceVector.begin(); it != SourceVector.end(); ++it)
	{
		VtAccount* acnt = *it;
		if (acnt->AccountNo.compare(acntNo) == 0)
		{
			SourceVector.erase(it);
			break;
		}
	}
}

void VtFundEditor::RemoveFromTarget(std::string acntNo)
{
	for (auto it = TargetVector.begin(); it != TargetVector.end(); ++it)
	{
		VtAccount* acnt = *it;
		if (acnt->AccountNo.compare(acntNo) == 0)
		{
			// Set the default value of SeungSu to the account.
			acnt->SeungSu = 1;
			TargetVector.erase(it);
			break;
		}
	}
}

void VtFundEditor::AddToSource(VtAccount* acnt)
{
	if (!acnt)
		return;
	SourceVector.push_back(acnt);
}

void VtFundEditor::AddToTarget(VtAccount* acnt)
{
	if (!acnt)
		return;
	TargetVector.push_back(acnt);
}

void VtFundEditor::SetTargetSumInfo()
{
	int seungSuSum = 0;
	for (auto it = TargetVector.begin(); it != TargetVector.end(); ++it)
	{
		VtAccount* fundAcnt = *it;
		seungSuSum += fundAcnt->SeungSu;
	}
	double ratio = 0.0;
	double sumRatio = 0.0;
	for (auto it = TargetVector.begin(); it != TargetVector.end(); ++it)
	{
		VtAccount* fundAcnt = *it;
		ratio = fundAcnt->SeungSu / double(seungSuSum);
		fundAcnt->Ratio = ratio;
		sumRatio += ratio;
	}

	CString strSumSeungSu;
	CString strSumRatio;
	strSumSeungSu.Format(_T("%d"), seungSuSum);
	strSumRatio.Format(_T("%.2f%%"), sumRatio * 100);
	_StaticSumSeungSu.SetWindowTextA(strSumSeungSu);
	_StaticSumRatio.SetWindowTextA(strSumRatio);
}

void VtFundEditor::SetSelectedFundAccountInfo(bool include)
{
	if (!_SelTargetFundAcnt)
		return;

	CString strFundAcntName;
	CString strFundAcntSeungSu;
	CString strFundAcntRatio;

	strFundAcntName.Format(_T("%s"), _SelTargetFundAcnt->AccountNo.c_str());
	strFundAcntSeungSu.Format(_T("%d"), _SelTargetFundAcnt->SeungSu);
	strFundAcntRatio.Format(_T("%.2f%%"), _SelTargetFundAcnt->Ratio * 100);

	_StaticAcntName.SetWindowTextA(strFundAcntName);
	if (include)
		_EditSeungSu.SetWindowTextA(strFundAcntSeungSu);
	_StaticRatio.SetWindowTextA(strFundAcntRatio);
}


void VtFundEditor::OnEnChangeEditSeunsu()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	if (_Editing)
		return;

	_Editing = true;
	// TODO:  Add your control notification handler code here
	if (_EditSeungSu.GetSafeHwnd() && _SelTargetFundAcnt)
	{
		CString strText;
		_EditSeungSu.GetWindowText(strText);
		if (strText.GetLength() > 0)
		{
			_SelTargetFundAcnt->SeungSu = _ttoi(strText);
			SetTargetSumInfo();
			SetSelectedFundAccountInfo();
			_FundGrid.ClearCells();
			_FundGrid.InitGrid(TargetVector);
		}
	}
	_Editing = false;
}


void VtFundEditor::OnEnUpdateEditSeunsu()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

	// TODO:  Add your control notification handler code here
	
}
