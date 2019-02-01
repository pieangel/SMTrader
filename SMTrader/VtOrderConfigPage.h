#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "Global/VtDefine.h"


// VtOrderConfigPage dialog
class VtOrderConfigManager;
class VtOrderConfigPage : public CDialogEx
{
	DECLARE_DYNAMIC(VtOrderConfigPage)

public:
	VtOrderConfigPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtOrderConfigPage();
	void SetOrderConfigMgr(VtOrderConfigManager* val);
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_CONFIG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
public:
	CButton _CheckConfirmShortcutOrder;
	CButton _CheckFixHoga;
	CButton _CheckOneClick;
	CButton _CheckOrderBySpacebar;
	CButton _CheckUseShortcut;
	CEdit _EditOrderCellHeight;
	CEdit _EditOrderCellWidth;
	CSpinButtonCtrl _SpinOrderCellHeight;
	CSpinButtonCtrl _SpinOrderCellWidth;
	virtual BOOL OnInitDialog();
	void InitControlValues();
	afx_msg void OnBnClickedCheckOneclick();
	afx_msg void OnBnClickedCheckFixHoga();
	afx_msg void OnBnClickedCheckOrderByspace();
	afx_msg void OnBnClickedRadioByMouseSelect();
	afx_msg void OnBnClickedRadioByMousePos();
	afx_msg void OnBnClickedCheckUseShortcut();
	afx_msg void OnBnClickedCheckConfirmShortcutorder();
	afx_msg void OnBnClickedRadioCancelLastorder();
	afx_msg void OnBnClickedRadioCancelOrderByselpos();
	afx_msg void OnEnChangeEditOrderCellHeight();
	afx_msg void OnEnChangeEditOrderCellWidth();
};
