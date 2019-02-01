#pragma once
#include "VtAccountAssetGrid.h"
#include "afxwin.h"

// VtAssetPage dialog
class VtAccount;
class VtAssetPage : public CDialogEx
{
	DECLARE_DYNAMIC(VtAssetPage)

public:
	VtAssetPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtAssetPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ASSET };
#endif
	VtAccount* Account() const { return _Account; }
	void Account(VtAccount* val) { _Account = val; }
private:
	VtAccountAssetGrid _AssetGrid;
	VtAccount* _Account = nullptr;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void InitAccount();
	virtual BOOL OnInitDialog();
	CComboBox _ComboAccount;
	afx_msg void OnBnClickedBtnSearch();
	afx_msg void OnCbnSelchangeComboAccount();
	void OnReceiveAccountInfo();
};
