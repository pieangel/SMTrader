#pragma once


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
};
