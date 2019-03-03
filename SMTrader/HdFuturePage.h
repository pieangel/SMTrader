#pragma once
#include "afxwin.h"
#include "HdFutureGrid.h"

// HdFuturePage dialog
class HdSymbolSelecter;
class VtOrderConfigManager;
class VtUsdStrategyConfigDlg;
class VtAddConnectSignalDlg;
class VtSignalConnectionGrid;
class HdFuturePage : public CDialogEx
{
	DECLARE_DYNAMIC(HdFuturePage)

public:
	HdFuturePage(CWnd* pParent = NULL);   // standard constructor
	virtual ~HdFuturePage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FUT_PAGE };
#endif

	HdSymbolSelecter* SymSelecter() const { return _SymSelecter; }
	void SymSelecter(HdSymbolSelecter* val) { _SymSelecter = val; }
	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) 
	{ 
		_OrderConfigMgr = val; 
		_FutureGrid.OrderConfigMgr(_OrderConfigMgr);
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox _ComboFutureMarket;
	HdFutureGrid _FutureGrid;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboFutMarket();

private:
	HdSymbolSelecter* _SymSelecter;
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
public:
	void GetSymbolMaster();
	void SetConfigDlg(VtUsdStrategyConfigDlg* ConfigDlg);
	void SetAddConSigDlg(VtAddConnectSignalDlg* conSigDlg);
	void SetSigConGrid(VtSignalConnectionGrid* sigConGrid);
};
