#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "Global/VtDefine.h"

// VtSystemOrderConfig dialog

class VtSystemOrderConfig : public CDialogEx
{
	DECLARE_DYNAMIC(VtSystemOrderConfig)

public:
	VtSystemOrderConfig(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtSystemOrderConfig();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYS_ORDER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox _ComboPrice;
	CComboBox _ComboPriceType;
	CEdit _EditTick;
	CSpinButtonCtrl _SpinTick;
	virtual BOOL OnInitDialog();

	VtPriceType _PriceType = VtPriceType::Price;
	int _OrderTick = 5;
	afx_msg void OnCbnSelchangeComboPriceType();
	afx_msg void OnCbnSelchangeComboPrice();
	afx_msg void OnBnClickedBtnOk();
	afx_msg void OnBnClickedBtnCancel();
};
