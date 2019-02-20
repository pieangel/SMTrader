#pragma once
#include "VtOrderListGrid.h"
#include <string>
#include <set>
#include <map>
#include "afxwin.h"
// HdOrderDialog dialog

class HdOrderDialog : public CDialogEx
{
	DECLARE_DYNAMIC(HdOrderDialog)

public:
	HdOrderDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~HdOrderDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_TOTAL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	VtOrderListGrid _OrderGrid;
	std::set<std::pair<int, std::string>> _AccountSet;
	std::set<std::string> _SymbolSet;
	std::map<int, std::pair<int, std::string>> _AccountComboMap;
	std::map<int, std::string> _SymbolComboMap;
	std::vector<VtOrder*> FindByAccount(std::pair<int, std::string> acntNo);
	std::vector<VtOrder*> FindBySymbol(std::string symbolCode);
public:
	virtual BOOL OnInitDialog();
	CButton _CheckShowCancelOrders;
	CButton _CheckShowRealAccountOnly;
	CComboBox _ComboAccount;
	CComboBox _ComboSymbol;
	CEdit _EditAccountName;
	afx_msg void OnCbnSelchangeComboAccount();
	afx_msg void OnCbnSelchangeComboSymbol();
	afx_msg void OnBnClickedCheckShowCancelOrder();
	afx_msg void OnBnClickedCheckShowRealAccountOnly();
	afx_msg void OnBnClickedMfcbuttonState();
};
