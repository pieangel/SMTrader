// VtFDMultiAcntPage.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtFDMultiAcntPage.h"
#include "afxdialogex.h"
#include "VtFundEditor.h"
#include "VtFundNameEditor.h"
#include "VtFundManager.h"
#include "VtFund.h"
#include "VtFundAccount.h"
#include "VtAccount.h"
#include "VtOrderDialogManager.h"
#include "VtOrderWndHd.h"
#include "HdWindowManager.h"
#include "VtFundMiniJango.h"
#include "VtGlobal.h"
// VtFDMultiAcntPage dialog

extern TApplicationFont g_Font;

IMPLEMENT_DYNAMIC(VtFDMultiAcntPage, ETSLayoutDialog)

VtFDMultiAcntPage::VtFDMultiAcntPage(CWnd* pParent /*=NULL*/)
	: ETSLayoutDialog(IDD_FUND_ACCOUNTPAGE, pParent)
{
	_CurFund = nullptr;
	_OldFund = nullptr;
	_SelectedTargetRow = 0;
}

VtFDMultiAcntPage::~VtFDMultiAcntPage()
{
}

void VtFDMultiAcntPage::DoDataExchange(CDataExchange* pDX)
{
	ETSLayoutDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_FUNDLIST, _TreeCtrl);
}


BEGIN_MESSAGE_MAP(VtFDMultiAcntPage, ETSLayoutDialog)
	ON_BN_CLICKED(IDC_BTN_FUND_SETTINGS, &VtFDMultiAcntPage::OnBnClickedBtnFundSettings)
	ON_BN_CLICKED(IDC_BTN_FUND_NEW, &VtFDMultiAcntPage::OnBnClickedBtnFundNew)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_FUNDLIST, &VtFDMultiAcntPage::OnTvnSelchangedTreeFundlist)
	ON_BN_CLICKED(IDC_BTN_UP, &VtFDMultiAcntPage::OnBnClickedBtnUp)
	ON_BN_CLICKED(IDC_BTN_DOWN, &VtFDMultiAcntPage::OnBnClickedBtnDown)
	ON_BN_CLICKED(IDC_BTN_FUND_MODIFY, &VtFDMultiAcntPage::OnBnClickedBtnFundModify)
	ON_BN_CLICKED(IDC_BTN_FUND_DELETE, &VtFDMultiAcntPage::OnBnClickedBtnFundDelete)
END_MESSAGE_MAP()


// VtFDMultiAcntPage message handlers


BOOL VtFDMultiAcntPage::OnInitDialog()
{
	ETSLayoutDialog::OnInitDialog();

	::EnumChildWindows(m_hWnd, ::SetChildFont, (LPARAM)g_Font.GetFont());
	/*
	CreateRoot(VERTICAL)
		<< (pane(HORIZONTAL, ABSOLUTE_VERT)
		<< item(IDC_BTN_FUND_NEW, NORESIZE)
		<< item(IDC_BTN_FUND_SETTINGS, NORESIZE)
		<< item(IDC_BTN_FUND_MODIFY, NORESIZE)
		<< item(IDC_BTN_FUND_DELETE, NORESIZE)
		<< itemGrowing(HORIZONTAL))
		<< (pane(HORIZONTAL, GREEDY)
			<< item(IDC_TREE_FUNDLIST, ABSOLUTE_HORZ | GREEDY)
			<< item(IDC_STATIC_FUND_DETAIL, GREEDY)
			);

	UpdateLayout();
	*/
	_FundGrid.FundPage(this);
	_FundGrid.AttachGrid(this, IDC_STATIC_FUND_DETAIL);

	InitFundTree();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void VtFDMultiAcntPage::OnBnClickedBtnFundSettings()
{
	// TODO: Add your control notification handler code here
	VtFundEditor fundEditor;
	fundEditor.SelFund(_CurFund);
	INT_PTR nResult = fundEditor.DoModal();
	if (nResult == IDOK)
	{
		HTREEITEM selNode = FindNode(_TreeCtrl.GetRootItem(), _CurFund);
		if (selNode)
		{
			HTREEITEM hItemChild = _TreeCtrl.GetChildItem(selNode);
			while (hItemChild != NULL)
			{
				HTREEITEM hItemNextChild = _TreeCtrl.GetNextSiblingItem(hItemChild);
				_TreeCtrl.DeleteItem(hItemChild);
				hItemChild = hItemNextChild;
			}

			VtFundMap& fundMap = _CurFund->GetFundMap();
			std::vector<VtAccount*>& targetAcntVector = fundMap.GetAccountVector();
			for (auto it = targetAcntVector.begin(); it != targetAcntVector.end(); ++it)
			{
				VtAccount* fundAcnt = *it;
				HTREEITEM item =  _TreeCtrl.InsertItem(fundAcnt->AccountNo.c_str(), selNode);
				//_TreeCtrl.SetItemData(item, (DWORD_PTR)_CurFund);
			}
			_TreeCtrl.Expand(selNode, TVE_EXPAND);
			_FundGrid.ClearCells();
			_FundGrid.InitGrid(targetAcntVector);
		}
	}
}


void VtFDMultiAcntPage::OnBnClickedBtnFundNew()
{
	// TODO: Add your control notification handler code here
	VtFundNameEditor nameEditor;
	INT_PTR nRet = -1; 
	nRet = nameEditor.DoModal();
	if (nRet == IDOK)
	{
		std::string name;
		name = nameEditor.fundName;

		VtFundManager* fundMgr = VtFundManager::GetInstance();
		VtFund* newFund = fundMgr->FindFund(name);
		if (newFund)
		{
			AfxMessageBox(_T("같은 이름의 펀드가 이미 존재합니다. 다른 이름을 사용하십시오."));
			//nRet = nameEditor.DoModal();
			return;
		}
		newFund = fundMgr->AddFund(name);
		HTREEITEM item = AddFundItem(newFund);
		if (item)
			_TreeCtrl.SelectItem(item);

		VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
		orderDlgMgr->OnFundAdded();

		HdWindowManager* wndMgr = HdWindowManager::GetInstance();
		auto wndMap = wndMgr->GetWindowMap();
		for (auto it = wndMap.begin(); it != wndMap.end(); ++it)
		{
			auto item = it->second;
			HdWindowType type = item.first;
			CWnd* wnd = item.second;
			if (type == HdWindowType::FundMiniJangoWindow)
			{
				((VtFundMiniJango*)wnd)->OnFundAdded();
			}
		}
	}
}

void VtFDMultiAcntPage::RefreshFundList(VtFundManager* fundMgr)
{
	if (fundMgr)
		return;

}

HTREEITEM VtFDMultiAcntPage::AddFundItem(VtFund* fund)
{
	if (!fund)
		return nullptr;

	HTREEITEM item = _TreeCtrl.InsertItem(fund->Name.c_str());
	_TreeCtrl.SetItemData(item, (DWORD_PTR)fund);
	_TreeCtrl.Invalidate(TRUE);

	//item = _TreeCtrl.GetRootItem();

	return item;
}


void VtFDMultiAcntPage::OnTvnSelchangedTreeFundlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	CString TreeText;
	HTREEITEM TreeItem = _TreeCtrl.GetSelectedItem();
	TreeText = _TreeCtrl.GetItemText(TreeItem);
	//AfxMessageBox(TreeText);
	_SelItem = TreeItem;

	 HTREEITEM topItem =  _TreeCtrl.GetParentItem(_SelItem);
	 if (topItem)
	 {
		 TreeItem = _TreeCtrl.GetParentItem(_SelItem);
		 _CurFund = (VtFund*)_TreeCtrl.GetItemData(TreeItem);
	 }
	 else
	 {
		 _CurFund = (VtFund*)_TreeCtrl.GetItemData(_SelItem);
	 }

	 if (_OldFund != _CurFund)
	 {
		 VtFundMap& fundMap = _CurFund->GetFundMap();
		 std::vector<VtAccount*>& targetAcntVector = fundMap.GetAccountVector();
		 _FundGrid.ClearCells();
		 _FundGrid.InitGrid(targetAcntVector);

		 _OldFund = _CurFund;
	 }
	*pResult = 0;
}

HTREEITEM VtFDMultiAcntPage::FindNode(HTREEITEM node, VtFund* fund)
{
	if (node == nullptr) return nullptr;

	do
	{
		if (_TreeCtrl.GetItemData(node) == (DWORD_PTR)fund)
		{
			_TreeCtrl.Select(node, TVGN_CARET);
			_TreeCtrl.EnsureVisible(node);
			return node;
		}
	} while ((node = _TreeCtrl.GetNextSiblingItem(node)) != nullptr);

	return nullptr;
}


void VtFDMultiAcntPage::InitFundTree()
{
	VtFundManager* fundMgr = VtFundManager::GetInstance();
	std::map<std::string, VtFund*>& fundList = fundMgr->GetFundList();
	for (auto it = fundList.begin(); it != fundList.end(); ++it)
	{
		VtFund* fund = it->second;
		HTREEITEM fundNode = AddFundItem(fund);
		if (fundNode)
		{
			VtFundMap& fundMap = fund->GetFundMap();
			std::vector<VtAccount*>& acntVector = fundMap.GetAccountVector();
			if (it == fundList.begin())
			{
				_CurFund = fund;
				_TreeCtrl.SelectItem(fundNode);
				_OldFund = _CurFund;
				_FundGrid.ClearCells();
				_FundGrid.InitGrid(acntVector);
			}

			for (auto it = acntVector.begin(); it != acntVector.end(); ++it)
			{
				VtAccount* acnt = *it;
				HTREEITEM item = _TreeCtrl.InsertItem(acnt->AccountNo.c_str(), fundNode);
			}

			_TreeCtrl.Expand(fundNode, TVE_EXPAND);
		}
	}
}


void VtFDMultiAcntPage::OnBnClickedBtnUp()
{
	if (!_CurFund)
		return;

	VtFundMap& fundMap = _CurFund->GetFundMap();
	std::vector<VtAccount*>& targetAcntVector = fundMap.GetAccountVector();

	int size = targetAcntVector.size();
	if (_SelectedTargetRow < 1 || _SelectedTargetRow >= size)
		return;

	std::swap(targetAcntVector[_SelectedTargetRow], targetAcntVector[_SelectedTargetRow - 1]);

	_FundGrid.ChangeSelectedRow(_SelectedTargetRow, _SelectedTargetRow - 1);
	_FundGrid.InitGrid(targetAcntVector);
}


void VtFDMultiAcntPage::OnBnClickedBtnDown()
{
	if (!_CurFund)
		return;

	VtFundMap& fundMap = _CurFund->GetFundMap();
	std::vector<VtAccount*>& targetAcntVector = fundMap.GetAccountVector();

	int size = targetAcntVector.size();
	if (_SelectedTargetRow < 0 || _SelectedTargetRow >= size - 1)
		return;

	std::swap(targetAcntVector[_SelectedTargetRow], targetAcntVector[_SelectedTargetRow + 1]);

	_FundGrid.ChangeSelectedRow(_SelectedTargetRow, _SelectedTargetRow + 1);
	_FundGrid.InitGrid(targetAcntVector);
}


void VtFDMultiAcntPage::OnBnClickedBtnFundModify()
{
	if (!_CurFund)
		return;

	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	if (orderDlgMgr->FindFund(_CurFund))
	{
		AfxMessageBox(_T("현재 사용중인 펀드는 변경할 수 없습니다."));
		return;
	}

	// TODO: Add your control notification handler code here
	VtFundNameEditor nameEditor;
	nameEditor.fundName = _CurFund->Name.c_str();
	INT_PTR nRet = -1;
	nRet = nameEditor.DoModal();
	if (nRet == IDOK)
	{
		std::string name;
		name = nameEditor.fundName;

		VtFundManager* fundMgr = VtFundManager::GetInstance();
		VtFund* newFund = fundMgr->FindFund(name);
		if (newFund)
		{
			AfxMessageBox(_T("같은 이름의 펀드가 이미 존재합니다. 다른 이름을 사용하십시오."));
			//nRet = nameEditor.DoModal();
			return;
		}
		_CurFund->Name = name;
		_TreeCtrl.DeleteAllItems();
		InitFundTree();
	}
}


void VtFDMultiAcntPage::OnBnClickedBtnFundDelete()
{
	if (!_CurFund)
		return;

	VtFundManager* fundMgr = VtFundManager::GetInstance();
	VtFund* oldFund = fundMgr->FindFund(_CurFund->Name);
	if (oldFund)
	{
		VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
		if (orderDlgMgr->FindFund(oldFund))
		{
			AfxMessageBox(_T("현재 사용중인 펀드는 삭제할 수 없습니다."));
			return;
		}
		fundMgr->DeleteFund(_CurFund->Name);
		_TreeCtrl.DeleteAllItems();
		InitFundTree();

		orderDlgMgr->OnFundDeleted(oldFund);

		HdWindowManager* wndMgr = HdWindowManager::GetInstance();
		auto wndMap = wndMgr->GetWindowMap();
		for (auto it = wndMap.begin(); it != wndMap.end(); ++it)
		{
			auto item = it->second;
			HdWindowType type = item.first;
			CWnd* wnd = item.second;
			if (type == HdWindowType::FundMiniJangoWindow)
			{
				((VtFundMiniJango*)wnd)->OnFundDeleted(oldFund);
			}
		}
	}
}
