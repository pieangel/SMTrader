#pragma once
#include "afxwin.h"
#include "VtFundGrid.h"
#include "VtAccountGrid.h"
#include <map>
#include <string>
#include <vector>

// VtFundEditor dialog
class VtAccount;
class VtFund;
class VtFundAccount;
class VtFundEditor : public CDialogEx
{
	DECLARE_DYNAMIC(VtFundEditor)

public:
	VtFundEditor(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtFundEditor();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FUND_EDITOR };
#endif

	VtFund* SelFund() const { return _SelFund; }
	void SelFund(VtFund* val) { _SelFund = val; }
	VtAccount* SelAcnt() const { return _SelSourceFundAcnt; }
	void SelAcnt(VtAccount* val) { _SelSourceFundAcnt = val; }
	VtAccount* SelFundAcnt() const { return _SelTargetFundAcnt; }
	void SelFundAcnt(VtAccount* val) 
	{
		_SelTargetFundAcnt = val; 
		SetSelectedFundAccountInfo();
	}
	int SelectedTargetRow() const { return _SelectedTargetRow; }
	void SelectedTargetRow(int val) { _SelectedTargetRow = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	VtAccountGrid _AcntGrid;
	VtFundGrid _FundGrid;
	CEdit _EditFundName;
	CEdit _EditSeungSu;
	CStatic _StaticAcntName;
	CStatic _StaticRatio;
	CStatic _StaticSumRatio;
	CStatic _StaticSumSeungSu;
	virtual BOOL OnInitDialog();
	//std::map <std::string, VtAccount*> SourceMap;
	//std::map<std::string, VtFundAccount*> TargetMap;
	std::vector<VtAccount*> SourceVector;
	std::vector<VtAccount*> TargetVector;
	void InitGrid();
private:
	void RefreshGrid();
	VtFund* _SelFund;
	VtAccount* _SelSourceFundAcnt;
	VtAccount* _SelTargetFundAcnt;
	int _SelectedTargetRow;
public:
	afx_msg void OnBnClickedBtnAcntIn();
	afx_msg void OnBnClickedBtnAcntInAll();
	afx_msg void OnBnClickedBtnAcntOut();
	afx_msg void OnBnClickedBtnAcntOutAll();
	afx_msg void OnBnClickedBtnUp();
	afx_msg void OnBnClickedBtnDown();
	afx_msg void OnBnClickedBtnOk();
	afx_msg void OnBnClickedBtnCancel();

private:
	//VtAccount* FindAccountFromSource(std::string acntNo);
	//VtFundAccount* FindFundAccountFromTarget(std::string acntNo);
	void RemoveFromSource(std::string acntNo);
	void RemoveFromTarget(std::string acntNo);
	void AddToSource(VtAccount* acnt);
	void AddToTarget(VtAccount* acnt);
	std::vector<VtAccount*> _NewFundAcntLis;
	void SetTargetSumInfo();
	void SetSelectedFundAccountInfo(bool include = true);
	bool _Editing;
public:
	afx_msg void OnEnChangeEditSeunsu();
	afx_msg void OnEnUpdateEditSeunsu();
};
