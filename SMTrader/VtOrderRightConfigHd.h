#pragma once
#include "afxwin.h"


// VtOrderRightConfigHd dialog
class VtOrderConfigManager;
class VtOrderRightConfigHd : public CDialogEx
{
	DECLARE_DYNAMIC(VtOrderRightConfigHd)

public:
	VtOrderRightConfigHd(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtOrderRightConfigHd();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_RIGHT_CONFIG_HD };
#endif

	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton _FormatThousand;
	CButton _OneClick;
	CButton _SpaceBarOrder;

private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
public:
	afx_msg void OnBnClickedOneClick();
	afx_msg void OnBnClickedSpacebarOrder();
	afx_msg void OnBnClickedRadioByMouseSelect();
	afx_msg void OnBnClickedRadioByMousePos();
	afx_msg void OnBnClickedCancelLastOrder();
	afx_msg void OnBnClickedCancelSelected();
	afx_msg void OnBnClickedFormatThousand();
	virtual BOOL OnInitDialog();
	void InitCtrl();
};
