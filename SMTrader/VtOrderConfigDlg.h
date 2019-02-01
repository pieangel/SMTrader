#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "ETSLayout.h"
#include "ShadeButtonST.h"

// VtOrderConfigDlg dialog
class VtOrderCenterWndHd;
class VtLayoutManager;
class VtOrderConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(VtOrderConfigDlg)

public:
	VtOrderConfigDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtOrderConfigDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CENTER_CONFIG };
#endif

	VtOrderCenterWndHd* CenterWnd() const { return _CenterWnd; }
	void CenterWnd(VtOrderCenterWndHd* val) { _CenterWnd = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckCutProfit();
	afx_msg void OnBnClickedCheckCutLoss();
	afx_msg void OnEnChangeEditCutProfit();
	afx_msg void OnEnChangeEditCutLoss();
	afx_msg void OnBnClickedButtonProfitLoss();
	afx_msg void OnBnClickedRadioAtmarket();
	afx_msg void OnBnClickedRadioAtprice();
	afx_msg void OnEnChangeEditOrderTypeSlip();

private:
	VtOrderCenterWndHd* _CenterWnd = nullptr;
	VtLayoutManager* _LayoutMgr;
	void SaveControls();
	CRect GetClientArea(int resourceID);
	CBrush _BrushBackSel;
	CBrush _BrushBackNor;
public:
	CButton _CheckCutLoss;
	CButton _CheckCutProfit;
	CEdit _EditCutLoss;
	CEdit _EditCutProfit;
	CEdit _EditOrderTypeSlip;
	CSpinButtonCtrl _SpinCutLoss;
	CSpinButtonCtrl _SpinCutProfit;
	CSpinButtonCtrl _SpinOrderTypeSlip;
	virtual BOOL OnInitDialog();
	void RecalateLayout(CRect rcWnd);
	afx_msg void OnBnClickedStaticFillPl();
	afx_msg void OnBnClickedStaticOrderType();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CShadeButtonST _BtnApply;
};
