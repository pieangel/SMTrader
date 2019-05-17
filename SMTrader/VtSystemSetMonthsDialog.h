#pragma once
#include "afxwin.h"
#include <string>
#include <map>

// VtSystemSetMonthsDialog dialog

class VtSystemSetMonthsDialog : public CDialogEx
{
	DECLARE_DYNAMIC(VtSystemSetMonthsDialog)

public:
	VtSystemSetMonthsDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtSystemSetMonthsDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYS_SET_MONTH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox _ComboKp;
	CComboBox _ComboKq;
	CComboBox _ComboUd;
	std::map<std::string, int> _KpMap;
	std::map<std::string, int> _KqMap;
	std::map<std::string, int> _UdMap;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboKp();
	afx_msg void OnCbnSelchangeComboKq();
	afx_msg void OnCbnSelchangeComboUd();
};
