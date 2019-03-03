#pragma once
#include "afxcmn.h"
#include "HdFuturePage.h"
#include "HdOptionPage.h"

// HdSymbolSelecter dialog
class VtOrderConfigManager;
class VtUsdStrategyConfigDlg;
class VtAddConnectSignalDlg;
class VtSignalConnectionGrid;
class HdSymbolSelecter : public CDialogEx
{
	DECLARE_DYNAMIC(HdSymbolSelecter)

public:
	HdSymbolSelecter(CWnd* pParent = NULL);   // standard constructor
	virtual ~HdSymbolSelecter();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYMBOL_SELECTER_HD };
#endif

	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) 
	{
		_OrderConfigMgr = val; 
		_FutPage.OrderConfigMgr(_OrderConfigMgr);
		_OptPage.OrderConfigMgr(_OrderConfigMgr);
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl _TabCtrl;
	HdFuturePage _FutPage;
	HdOptionPage _OptPage;
	virtual BOOL OnInitDialog();
	void InitTabCtrl();
	CWnd* _CurrentPage = nullptr;
	afx_msg void OnTcnSelchangeTabFutopt(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	int SelTab;
	void SetConfigDlg(VtUsdStrategyConfigDlg* ConfigDlg);
	void SetAddConSigDlg(VtAddConnectSignalDlg* conSigDlg);
	void SetSigConGrid(VtSignalConnectionGrid* sigConGrid);
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
};
