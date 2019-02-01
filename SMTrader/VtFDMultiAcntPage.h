#pragma once
#include "ETSLayout.h"
#include "VtFundGrid.h"
// VtFDMultiAcntPage dialog
class VtFundManager;
class VtFund;
class VtFDMultiAcntPage : public ETSLayoutDialog
{
	DECLARE_DYNAMIC(VtFDMultiAcntPage)

public:
	VtFDMultiAcntPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtFDMultiAcntPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FUND_ACCOUNTPAGE };
#endif

	int SelectedTargetRow() const { return _SelectedTargetRow; }
	void SelectedTargetRow(int val) { _SelectedTargetRow = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	VtFundGrid _FundGrid;
	CTreeCtrl	_TreeCtrl;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnFundSettings();
	afx_msg void OnBnClickedBtnFundNew();

	void RefreshFundList(VtFundManager* fundMgr);
	HTREEITEM AddFundItem(VtFund* fund);
	afx_msg void OnTvnSelchangedTreeFundlist(NMHDR *pNMHDR, LRESULT *pResult);

private:
	VtFund* _CurFund;
	VtFund* _OldFund;
	HTREEITEM _SelItem;
	HTREEITEM FindNode(HTREEITEM node, VtFund* acnt);
	int _SelectedTargetRow;
	void InitFundTree();
public:
	afx_msg void OnBnClickedBtnUp();
	afx_msg void OnBnClickedBtnDown();
	afx_msg void OnBnClickedBtnFundModify();
	afx_msg void OnBnClickedBtnFundDelete();
};


