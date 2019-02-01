#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// VtOrderSettlePage dialog
class VtOrderConfigManager;
class VtOrderSettlePage : public CDialogEx
{
	DECLARE_DYNAMIC(VtOrderSettlePage)

public:
	VtOrderSettlePage(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtOrderSettlePage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_SETTLE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void SetOrderConfigMgr(VtOrderConfigManager* val);
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
public:
	// The button for profit and loss 
	CButton _BtnProfitLoss;
	CButton _CheckLoss;
	CButton _CheckProfit;
	CButton _CheckTrailStop;
	CEdit _EditLoss;
	CEdit _EditProfit;
	CEdit _EditPsStop;
	CEdit _EditTs;
	CEdit _EditTsDefaultLoss;
	CEdit _EditTsProfit;
	CEdit _EditTsTrail;
	CEdit _EditTsStop;
	CSpinButtonCtrl _SpinLoss;
	CSpinButtonCtrl _SpinProfit;
	CSpinButtonCtrl _SpinPsProfit;
	CSpinButtonCtrl _SpinPsStop;
	CSpinButtonCtrl _SpinTs;
	CSpinButtonCtrl _SpinTsDefaultLoss;
	CSpinButtonCtrl _SpinTsStop;
	afx_msg void OnBnClickedCheckProfit();
	afx_msg void OnBnClickedCheckLoss();
	afx_msg void OnEnChangeEditProfit();
	afx_msg void OnEnChangeEditLoss();
	afx_msg void OnBnClickedButtonProfitLoss();
	afx_msg void OnEnChangeEditPsStop();
	afx_msg void OnBnClickedCheckTrailStop();
	afx_msg void OnEnChangeEditTsDefaultLoss();
	afx_msg void OnEnChangeEditTsProfit();
	afx_msg void OnEnChangeEditTsTrail();
	afx_msg void OnBnClickedRadioTsAtmarket();
	afx_msg void OnBnClickedRadioTsStop();
	afx_msg void OnEnChangeEditTsStop();
	virtual BOOL OnInitDialog();
	void InitControlValues();
	afx_msg void OnRadioAtmarket();
	afx_msg void OnRadioStop();
};
