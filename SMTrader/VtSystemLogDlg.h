#pragma once
#include "VtSysLogGrid.h"

// VtSystemLogDlg dialog
class CMainFrame;
class VtSystemLogDlg : public CDialogEx
{
	DECLARE_DYNAMIC(VtSystemLogDlg)

public:
	VtSystemLogDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtSystemLogDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYS_LOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void RefreshGrid();
	VtSysLogGrid _SysLogGrid;
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();

	CMainFrame* MainFrm = nullptr;
	afx_msg void OnClose();
};
