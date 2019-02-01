#pragma once
#include "afxpropertygridctrl.h"


// VtSystemSetDlg dialog
class VtSystem;
class VtSystemSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(VtSystemSetDlg)

public:
	VtSystemSetDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtSystemSetDlg();

	void InitSystemProperty(VtSystem* sys);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYS_PROP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CMFCPropertyGridCtrl _PropCtrl;
protected:
	afx_msg LRESULT OnAfxWmPropertyChanged(WPARAM wParam, LPARAM lParam);
};
