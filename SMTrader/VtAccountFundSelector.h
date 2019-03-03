#pragma once
#include "VtAccountFundGrid.h"

// VtAccountFundSelector dialog
class VtUsdStrategyConfigDlg;
class VtSignalConnectionGrid;
class VtAccountFundSelector : public CDialogEx
{
	DECLARE_DYNAMIC(VtAccountFundSelector)

public:
	VtAccountFundSelector(CWnd* pParent = nullptr);   // standard constructor
	virtual ~VtAccountFundSelector();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ACNT_FUND_SELECTOR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	VtAccountFundGrid _Grid;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioEntire();
	afx_msg void OnBnClickedRadioAcnt();
	afx_msg void OnBnClickedRadioFund();
	void SetConfigDlg(VtUsdStrategyConfigDlg* ConfigDlg);
	void SetSignalConnectionGrid(VtSignalConnectionGrid* SigConGrid);
};
