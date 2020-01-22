#pragma once
#include "VtSysLogGrid.h"

// VtOrderLogDlg dialog

class VtOrderLogDlg : public CDialogEx
{
	DECLARE_DYNAMIC(VtOrderLogDlg)

public:
	VtOrderLogDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtOrderLogDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_LOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	VtSysLogGrid _SysLogGrid;
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	void UpdateOrderLog(std::vector<std::pair<std::string, std::string>> log_vec);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void Resize();
};
