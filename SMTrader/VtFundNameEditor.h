#pragma once
#include "afxwin.h"


// VtFundNameEditor dialog
class VtFund;
class VtFundNameEditor : public CDialogEx
{
	DECLARE_DYNAMIC(VtFundNameEditor)

public:
	VtFundNameEditor(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtFundNameEditor();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FUND_NAME };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CEdit _EditFundName;
	afx_msg void OnEnChangeEditFundName();
	CString fundName;
private:
	VtFund* _fund;
public:
	afx_msg void OnClose();
};
