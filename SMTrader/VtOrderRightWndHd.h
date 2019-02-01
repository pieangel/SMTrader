#pragma once
#include "GenericChildDialog.h"
#include "VtOrderRightConfigHd.h"
#include "VtOrderRightCancelHd.h"
#include "afxcmn.h"
// VtOrderRightWndHd dialog
class VtOrderConfigManager;
class VtOrderRightWndHd : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(VtOrderRightWndHd)

public:
	//VtOrderRightWndHd(CWnd* pParent = NULL);   // standard constructor
	VtOrderRightWndHd();
	virtual ~VtOrderRightWndHd();
	virtual int CRHGetDialogID();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_RIGHT_HD };
#endif

	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	int DefaultWidth() const { return _DefaultWidth; }
	void DefaultWidth(int val) { _DefaultWidth = val; }
	int DefaultHeight() const { return _DefaultHeight; }
	void DefaultHeight(int val) { _DefaultHeight = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	CTabCtrl _TabCtrl;
	VtOrderRightConfigHd _ConfigPage;
	VtOrderRightCancelHd _CancelPage;
	void InitTabCtrl();
	CWnd* _CurrentPage = nullptr;

private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTcnSelchangeTabConfig(NMHDR *pNMHDR, LRESULT *pResult);
	void InitCtrl();

private:
	int _DefaultWidth = 167;
	int _DefaultHeight = 561;
};
