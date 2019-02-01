#pragma once
#include "VtAccountGrid.h"
#include "VtSubAcntGrid.h"
#include "afxwin.h"

// VtFDSubAcntPage dialog
class VtAccount;
class VtFundDesigner;
class VtFDSubAcntPage : public CDialogEx
{
	DECLARE_DYNAMIC(VtFDSubAcntPage)

public:
	VtFDSubAcntPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtFDSubAcntPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FUND_SUBACCOUNTPAGE };
#endif

	VtFundDesigner* FundDesigner() const { return _FundDesigner; }
	void FundDesigner(VtFundDesigner* val) { _FundDesigner = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	VtAccountGrid _AcntGrid;
	VtSubAcntGrid _SubAcntGrid;
	virtual BOOL OnInitDialog();
	CEdit _EditSubAcntName;
	CEdit _EditSubAcntCode;
	afx_msg void OnBnClickedBtnMod();
	afx_msg void OnBnClickedBtnCreate();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnClose();
	afx_msg void OnBnClickedBtnOk();

private:
	VtAccount* _SelAccount;
	VtFundDesigner* _FundDesigner;
public:
	void RefreshSubAccountList(VtAccount* acnt);
	void SelAccount(VtAccount* acnt);
	void SelSubAccount(VtAccount* acnt);
	CButton _BtnMod;
	CButton _BtnCreate;
	afx_msg void OnBnClickedBtnDel();
};
