#pragma once
#include "afxwin.h"
#include "HdOptionGrid.h"

// HdOptionPage dialog
class HdSymbolSelecter;
class VtOrderConfigManager;
class VtUsdStrategyConfigDlg;
class VtAddConnectSignalDlg;
class VtSignalConnectionGrid;
class HdOptionPage : public CDialogEx
{
	DECLARE_DYNAMIC(HdOptionPage)

public:
	HdOptionPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~HdOptionPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPT_PAGE };
#endif

	HdSymbolSelecter* SymSelecter() const { return _SymSelecter; }
	void SymSelecter(HdSymbolSelecter* val) { _SymSelecter = val; }
	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) 
	{ 
		_OrderConfigMgr = val;
		_OptionGrid.OrderConfigMgr(_OrderConfigMgr);
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox _ComboOptionMarket;
	CComboBox _ComboOptionMonth;

	HdOptionGrid _OptionGrid;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboOptMarket();
	afx_msg void OnCbnSelchangeComboOptMonth();

private:
	HdSymbolSelecter* _SymSelecter;
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
public:
	void GetSymbolMaster();
	void SetConfigDlg(VtUsdStrategyConfigDlg* ConfigDlg);
	void SetAddConSigDlg(VtAddConnectSignalDlg* conSigDlg);
	void SetSigConGrid(VtSignalConnectionGrid* sigConGrid);
};
