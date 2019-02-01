#pragma once
#include "VtAccountPasswordGrid.h"
#include "afxwin.h"

// VtAccountPasswordDlg dialog

class VtAccountPasswordDlg : public CDialogEx
{
	DECLARE_DYNAMIC(VtAccountPasswordDlg)

public:
	VtAccountPasswordDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtAccountPasswordDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ACCOUNT_PWD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();

private:
	VtAccountPasswordGrid _PasswordGrid;
public:
	void FromServer(bool val);
	afx_msg void OnBnClickedCheckShowPwd();
	CButton _CheckShowPwd;
	afx_msg void OnBnClickedBtnClose();
	afx_msg void OnBnClickedBtnSave();
};
