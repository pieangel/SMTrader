#pragma once
#include "GenericChildDialog.h"
#include "VtRealtimeTickQuoteGrid.h"
#include "VtProductAcceptedGrid.h"
#include "VtProductRemainGridEx.h"
#include "VtTotalRemainGrid.h"
#include "SmRealtickGrid.h"
#include "ShadeButtonST.h"
#include "afxwin.h"
// CVtOrderLeftWnd dialog
struct VtQuote;
class CVtOrderWnd;
class VtOrderConfigManager;
class VtAccount;
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
	SmRealtickGrid _TickQuoteGrid;
	VtProductAcceptedGrid  _AcceptGrid;
	VtProductRemainGridEx _RemainGrid;
	VtTotalRemainGrid _TotalGrid;
public:
	void SetRealtickSymbol(VtSymbol* symbol);
	// 좌측창의 그리드들의 정보를 채운다.
	void InitGridInfo();
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
public:
	afx_msg void OnStnClickedStaticAccepted();
	CShadeButtonST _BtnCancellAll;
	CShadeButtonST _BtnCancelSel;
	CShadeButtonST _BtnLiqAll;
	CShadeButtonST _BtnLiqSel;
};
