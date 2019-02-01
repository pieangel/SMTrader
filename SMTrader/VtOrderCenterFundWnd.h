
#pragma once
#include "GenericChildDialog.h"
#include "VtOrderPanelGrid.h"
#include "VtProductRemainGrid.h"
#include "afxwin.h"
#include "afxcmn.h"
#include <vector>
#include "BtnST.h"
#include "VtEditCtrl.h"
#include "VtRealtimeTickQuoteGrid.h"
#include "SimpleBinStream.h"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;
//#include "ResizableLib/ResizableDialog.h"
// VtOrderCenterHd dialog
class VtOrderWndHd;
class VtOrderConfigManager;
class VtSymbol;
class VtAccount;
class VtOrderManager;
class VtProductOrderManager;
class VtRealTickWnd;
class VtOrderConfigDlg;
class VtOrderGridConfig;
class VtFund;

class VtOrderCenterFundWnd : public CDialog
{
	DECLARE_DYNAMIC(VtOrderCenterFundWnd)

public:
	VtOrderCenterFundWnd(CWnd* pParent = NULL);   // standard constructor
												//VtOrderCenterWndHd();
	virtual ~VtOrderCenterFundWnd();
	//virtual int CRHGetDialogID();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_CENTER_FUND };
#endif

	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val);
	int OrderAmount() const { return _OrderAmount; }
	void OrderAmount(int val) { _OrderAmount = val; }
	VtSymbol* Symbol() const { return _Symbol; }
	void Symbol(VtSymbol* val) { _Symbol = val; }
	VtAccount* Account() const { return _Account; }
	void Account(VtAccount* val) { _Account = val; }
	VtOrderManager* OrderMgr() const { return _OrderMgr; }
	void OrderMgr(VtOrderManager* val) { _OrderMgr = val; }
	VtFilledCondition FillCondition() const { return _FillCondition; }
	void FillCondition(VtFilledCondition val) { _FillCondition = val; }
	VtPriceType PriceType() const { return _PriceType; }
	void PriceType(VtPriceType val) { _PriceType = val; }
	VtProductOrderManager* PrdtOrderMgr() const { return _PrdtOrderMgr; }
	void PrdtOrderMgr(VtProductOrderManager* val) { _PrdtOrderMgr = val; }
	bool EnableCutProfit() const { return _EnableCutProfit; }
	void EnableCutProfit(bool val) { _EnableCutProfit = val; }
	bool EnableCutLoss() const { return _EnableCutLoss; }
	void EnableCutLoss(bool val) { _EnableCutLoss = val; }
	int CutProfit() const { return _CutProfit; }
	void CutProfit(int val) { _CutProfit = val; }
	int CutLoss() const { return _CutLoss; }
	void CutLoss(int val) { _CutLoss = val; }
	int OrderType() const { return _OrderType; }
	void OrderType(int val) { _OrderType = val; }
	int OrderTypeSlip() const { return _OrderTypeSlip; }
	void OrderTypeSlip(int val) { _OrderTypeSlip = val; }
	int StopVal() const { return _StopVal; }
	void StopVal(int val) { _StopVal = val; }
	bool Activated() const { return _Activated; }
	void Activated(bool val) { _Activated = val; }
	int ShowRealTickWndMode() const { return _ShowRealTickWndMode; }
	VtRealTickWnd* RealTickWnd() const { return _RealTickWnd; }
	void RealTickWnd(VtRealTickWnd* val) { _RealTickWnd = val; }
	void ResizeOrderGrid(int maxHeight);
	void SetFund(VtFund* fund);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void InitInfo();
	void InitAll();
	void InitByLoading(int height);
	virtual BOOL OnInitDialog();
	VtOrderPanelGrid& GetOrderPanelGrid()
	{
		return _OrderPanelGrid;
	}
private:
	void ClearConfigDlg();
	std::vector<VtOrderGridConfig*> _ConfigDlgVector;
	VtOrderWndHd* _ParentDlg = nullptr;
	VtRealtimeTickQuoteGrid _TickGrid;
	VtOrderPanelGrid _OrderPanelGrid;
	VtProductRemainGrid _ProductRemainGrid;
	bool _FixedCenter;
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	VtRealTickWnd* _RealTickWnd = nullptr;
	bool _Init = false;
	void SetProductGridWidth();
	void RePosStopCtrl(int level);
public:
	int GetMaxWidthForMode();
	void RefreshOrderPositon();
	void OnOrderReceivedHd(VtOrder* order);
	void OnOrderAcceptedHd(VtOrder* order);
	void OnOrderUnfilledHd(VtOrder* order);
	void OnOrderFilledHd(VtOrder* order);
	void OnRemain(VtPosition* posi);

	void OnReceiveHoga(VtSymbol* sym);
	void OnReceiveQuote(VtSymbol* sym);
	void OnSymbolMaster(VtSymbol* sym);

	void OnReceiveMsg(CString msg);
	void OnReceiveMsgWithReqId(int id, CString msg);
	void RepositionControl(int height);
	void AdjustControlForMode();
	int x, y, width, height;
private:
	void GetDeposit(VtPosition* posi);
	VtSymbol*  _Symbol;
	VtAccount* _Account;
	VtFund* _Fund;
	int _OrderAmount;
	VtFilledCondition _FillCondition;
	VtPriceType _PriceType;
	VtOrderManager* _OrderMgr;
	VtProductOrderManager* _PrdtOrderMgr;

	bool _EnableCutProfit = false;
	bool _EnableCutLoss = false;

	int _CutProfit = 5;
	int _CutLoss = 5;
	int _OrderType = 1;
	int _OrderTypeSlip = 0;

	int _StopVal;
	bool _Activated = false;
	int _ShowRealTickWndMode = 0;
	int _GridGap;
protected:
	CBrush m_brush;
public:
	int GetOrderCellWidth()
	{
		return _OrderPanelGrid.OrderWidth();
	}

	int GetCellHeight()
	{
		return _OrderPanelGrid.CellHeight();
	}

	int GetTickCount();
	void SetShowRealTickWnd(int mode);
	void SetShowLeftButtonText(CString text);
	void SetShowRightButtonText(CString text);
	void SetRemain(int remain);
	void SetSymbol(VtSymbol* sym);
	void SetSymbol();
	void SetAccount(VtAccount* acnt);
	void RegisterSymbolWnd();
	void UnregisterSymbolWnd();
	void UnregisterAccountWnd();
	void RegisterRealtimeSymbol();
	void UnregisterRealtimeSymbol();
	void RegisterRealtimeAccount();
	void UnregisterRealtimeAccount();
	void RegisterOrderWnd();
	void UnregisterOrderWnd();
	//CComboBox _ComboAccount;
	CComboBox _ComboSymbol;

	void ClearPosition();
	void InitAccount();
	void InitSymbol();
	void InitPosition();
	void InitOrder();
	//CStatic _AcntName;

	bool FixedCenter() const { return _FixedCenter; }
	void FixedCenter(bool val) { _FixedCenter = val; }
	CEdit _EditOrderAmount;
	CSpinButtonCtrl _ScOrderAmount;
	CSpinButtonCtrl _ScStopVal;
	CEdit _EditStopVal;
	CButton _BtnAvail;
	CButton _BtnRemain;
	CStatic _StaticMsg;
	CStatic _StaticProductName;
	CButtonST _BtnAmt1;
	CButtonST _BtnAmt2;
	CButtonST _BtnAmt3;
	CButtonST _BtnAmt4;
	CButtonST _BtnAmt5;
	CButtonST _BtnAmt6;
	VtEditCtrl _EditAmt;
	CButtonST* _CurBtn = nullptr;
	CStatic _StaticStop;
	CButton _CheckFixedCenter;

	void ChangeAccount(VtAccount* acnt);
	void ChangeSymbol(VtSymbol* newSymbol);
	void AddSymbolToCombo(VtSymbol* symbol);
	void SetProductName(VtSymbol* symbol);
	void ChangeFocus();

private:
	void CreateChildWindow(VtOrderConfigDlg* centerWnd, UINT id, CWnd* parent);
	VtOrderConfigDlg* _ConfigDlg = nullptr;
public:
	afx_msg void OnBnClickedButtonSetting();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnDeltaposSpinExpand(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnStnClickedStaticMsg();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedBtnShowRealtick();
	afx_msg void OnBnClickedStaticFillPl();
	afx_msg void OnCbnSelchangeComboProductHd();
	afx_msg void OnBnClickedCkFixedCenter();
	afx_msg void OnBnClickedBtnOrderAmt1();
	afx_msg void OnBnClickedBtnOrderAmt2();
	afx_msg void OnBnClickedBtnOrderAmt3();
	afx_msg void OnBnClickedBtnOrderAmt4();
	afx_msg void OnBnClickedBtnOrderAmt5();
	afx_msg void OnBnClickedBtnOrderAmt6();
	afx_msg void OnDeltaposSpinOrderAmount(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditOrderAmount();
	afx_msg void OnEnChangeEditStopval();
	afx_msg void OnBnClickedBtnLiq();
	afx_msg void OnBnClickedBtnRemain();
	afx_msg void OnBnClickedBtnSelSymbol();
	afx_msg void OnBnClickedButtonProfitLoss();
	afx_msg void OnStnClickedStaticOrderPanel();
	afx_msg void OnRClicked(NMHDR * pNotifyStruct, LRESULT * result);
	afx_msg void OnStnClickedStaticRealTick();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	bool _ShowOrderArea = true;
	bool _ShowStopArea = true;
	bool _ShowOrderCountArea = true;
	void ShowOrderAreaInGrid(bool flag);
	void ShowStopAreaInGrid(bool flag);
	void ShowOrderCountInGrid(bool flag);
	void ShowTickWnd(bool flag);
	void SetTickWndPos(int pos);
	void SetTickCount(int count);
	void SetOrderCellHeight(int value);
	void SetOrderCellWidth(int value);
	void SetShowPLConfigWnd(bool flag);
	void SetOrderArea(int height, int width);
	int GetMaxWidth();
	void RecalcLayout();
	void SetMaxTickCount(int count);
	void OnEntered();
	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
};
