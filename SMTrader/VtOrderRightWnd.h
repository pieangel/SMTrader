#pragma once

#include "GenericChildDialog.h"
#include "VtAccountRemainGrid.h"
#include "VtSymbolMasterGrid.h"
#include "VtOrderSettlePage.h"
#include "VtOrderConfigPage.h"
#include "afxcmn.h"
// CVtOrderRightWnd dialog
class VtSymbolMaster;
class VtSymbol;
struct VtRealtimeSymbolMaster;
class CVtOrderWnd;
class VtOrderConfigManager;
class VtAccount;
class CVtOrderRightWnd : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(CVtOrderRightWnd)

public:
	//CVtOrderRightWnd(CWnd* pParent = NULL);   // standard constructor
	CVtOrderRightWnd();
	virtual ~CVtOrderRightWnd();
	virtual int CRHGetDialogID();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_RIGHT };
#endif

	CVtOrderWnd* ParentOrderWnd() const { return _ParentOrderWnd; }
	void ParentOrderWnd(CVtOrderWnd* val) { _ParentOrderWnd = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	VtAccountRemainGrid _AccountRemainGrid;
	VtSymbolMasterGrid _SymMasterGrid;
	CVtOrderWnd* _ParentOrderWnd = nullptr;
public:
	void ShowAccountInfo(VtAccount* acnt);
	virtual BOOL OnInitDialog();
	void OnReceivedSymbolMaster(VtSymbolMaster* symMaster);
	void OnReceivedSymbolMaster(VtRealtimeSymbolMaster* symMaster);
	void OnReceiveRealtimeSymbolMaster(VtSymbolMaster* symMaster);
	void OnReceiveAccountDeposit(VtAccount* acnt);
	void SetSymbol(VtSymbol* sym);
	afx_msg void OnClose();
private:
	void InitTabCtrl();
	void InitAccountInfo();
	void InitMasterInfo();

	VtOrderSettlePage _Page1;
	VtOrderConfigPage _Page2;
	CWnd* _CurrentPage;
public:
	afx_msg void OnTcnSelchangeTabConfig(NMHDR *pNMHDR, LRESULT *pResult);
	// 주문창 설정 탭 컨트롤
	CTabCtrl _TabCtrl;
	void Reset();
	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	int DefaultWidth() const { return _DefaultWidth; }
	void DefaultWidth(int val) { _DefaultWidth = val; }
	int DefaultHeight() const { return _DefaultHeight; }
	void DefaultHeight(int val) { _DefaultHeight = val; }
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	int _DefaultWidth = 167;
	int _DefaultHeight = 561;
};
