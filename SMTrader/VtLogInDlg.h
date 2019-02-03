#pragma once
#include "GradientStatic.h"
#include "ShadeButtonST.h"
#include "BtnST.h"
#include "afxwin.h"

// VtLogInDlg dialog

class VtLogInDlg : public CDialogEx
{
	DECLARE_DYNAMIC(VtLogInDlg)

public:
	VtLogInDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~VtLogInDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGIN_MAIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
//	CEdit _EditId;
	CEdit _EditID;
	CEdit _EditCert;
	CEdit _EditPwd;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheckSave();
	bool Save = true;
	CString id, pwd, cert;
	virtual BOOL OnInitDialog();
private:
	CBrush _BrushBackNor;
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	CGradientStatic _StaticCert;
	CGradientStatic _StaticID;
	CGradientStatic _StaticPwd;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CButtonST _BtnCancel;
	CButtonST _BtnSignIn;
	afx_msg void OnBnClickedBtnLogin();
	afx_msg void OnBnClickedBtnCancel();
	afx_msg void OnBnClickedBtnCan();
	CButtonST _BtnCan;
};
