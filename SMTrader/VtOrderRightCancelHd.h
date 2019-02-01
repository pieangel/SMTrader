#pragma once
#include "afxwin.h"


// VtOrderRightCancelHd dialog
class VtOrderConfigManager;
class VtOrderRightCancelHd : public CDialogEx
{
	DECLARE_DYNAMIC(VtOrderRightCancelHd)

public:
	VtOrderRightCancelHd(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtOrderRightCancelHd();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_RIGHT_CANCEL_HD };
#endif

	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit _EditCancel;
	afx_msg void OnBnClickedBtnCancelLast();
	afx_msg void OnBnClickedCkFromrecent();
	afx_msg void OnBnClickedBtnCancelSelected();

private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
public:
	virtual BOOL OnInitDialog();
};
