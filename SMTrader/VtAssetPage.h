#pragma once
#include "VtAccountAssetGrid.h"
#include "afxwin.h"
#include "ShadeButtonST.h"
#include <string>
#include <map>
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
	void SetAccount(std::string acntNo);
private:
	VtAccountAssetGrid _AssetGrid;
	VtAccount* _Account = nullptr;
	std::map<std::string, std::pair<int, VtAccount*>> _ComboMap;
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
	afx_msg void OnBnClickedBtnCommit();
	CShadeButtonST _BtnCommit;
	CShadeButtonST _BtnSearch;
	CEdit _EditPwd;
};
