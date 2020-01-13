#pragma once
#include "GenericChildDialog.h"
#include "VtRealtimeTickQuoteGrid.h"
#include "VtProductAcceptedGrid.h"
#include "VtProductRemainGridEx.h"
#include "VtTotalRemainGrid.h"
// CVtOrderLeftWnd dialog
struct VtQuote;
class CVtOrderWnd;
class VtOrderConfigManager;
class CVtOrderLeftWnd : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(CVtOrderLeftWnd)

public:
	//CVtOrderLeftWnd(CWnd* pParent = NULL);   // standard constructor
	CVtOrderLeftWnd();
	virtual ~CVtOrderLeftWnd();
	virtual int CRHGetDialogID();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_LEFT };
#endif

	CVtOrderWnd* ParentOrderWnd() const { return _ParentOrderWnd; }
	void ParentOrderWnd(CVtOrderWnd* val) { _ParentOrderWnd = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void OnReceiveRealtimeQuote(VtQuote* quote);
private:
	VtRealtimeTickQuoteGrid _TickQuoteGrid;
	VtProductAcceptedGrid  _AcceptGrid;
	VtProductRemainGridEx _RemainGrid;
	VtTotalRemainGrid _TotalGrid;
public:
	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val);
	int DefaultWidth() const { return _DefaultWidth; }
	void DefaultWidth(int val) { _DefaultWidth = val; }
	int DefaultHeight() const { return _DefaultHeight; }
	void DefaultHeight(int val) { _DefaultHeight = val; }

	virtual BOOL OnInitDialog();
	void InitRealtimeQuote();
	void InitRemainList();
	void InitAcceptedList();
	void InitAccountInfo();
private:
	CVtOrderWnd* _ParentOrderWnd = nullptr;
	VtOrderConfigManager* _OrderConfigMgr = nullptr;

public:
	void RefreshRealtimeQuote();
	void RefreshRemainList();
	void RefreshAcceptedList();
	void ClearRealtimeTickQuoteGrid();
	void OnReceiveAccountDeposit(VtAccount* acnt);
	afx_msg void OnBnClickedBtnCancelSel();
	afx_msg void OnBnClickedBtnCancelAll();
	afx_msg void OnBnClickedBtnLiqSel();
	afx_msg void OnBnClickedBtnLiqAll();

	void OnOutstanding();

private:
	int _DefaultWidth = 162;
	int _DefaultHeight = 774;
};
