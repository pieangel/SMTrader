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
#include "OXLayoutManager.h"
#include "VtCutManager.h"
#include "VtConfigGrid.h"
#include "ShadeButtonST.h"
#include "VtRefreshManager.h"
#include <thread>
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
class VtCutManager;
class VtLayoutManager;
const int ConfigHeight = 94;
const int ConfigWidth = 480;
const int TickWndWidth = 155;
class VtRefreshManager;
class VtOrderCenterWndHd : public CDialogEx
{
	DECLARE_DYNAMIC(VtOrderCenterWndHd)

public:
	VtOrderCenterWndHd(CWnd* pParent = NULL);   // standard constructor
	//VtOrderCenterWndHd();
	virtual ~VtOrderCenterWndHd();
	//virtual int CRHGetDialogID();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_CENTER_HD };
#endif

	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val);
	int OrderAmount() const { return _OrderAmount; }
	void OrderAmount(int val) { _OrderAmount = val; }
	VtSymbol* Symbol() const { return _Symbol; }
	void Symbol(VtSymbol* val) { _Symbol = val; }
	VtFilledCondition FillCondition() const { return _FillCondition; }
	void FillCondition(VtFilledCondition val) { _FillCondition = val; }
	VtPriceType PriceType() const { return _PriceType; }
	void PriceType(VtPriceType val) { _PriceType = val; }
	bool EnableCutProfit() const;
	
	void EnableCutProfit(bool val);
	
	bool EnableCutLoss() const;
	
	void EnableCutLoss(bool val);
	
	int CutProfit() const;
	
	void CutProfit(int val);
	
	int CutLoss() const;
	
	void CutLoss(int val);
	
	int OrderType() const;
	
	void OrderType(int val);
	
	int OrderTypeSlip() const;
	
	void OrderTypeSlip(int val);
	

	int StopVal() const { return _StopVal; }
	void StopVal(int val) { _StopVal = val; }
	bool Activated() const { return _Activated; }
	void Activated(bool val);
	int TickWndPos() const { return _TickWndPos; }
	VtRealTickWnd* RealTickWnd() const { return _RealTickWnd; }
	void RealTickWnd(VtRealTickWnd* val) { _RealTickWnd = val; }
	void ResizeOrderGrid(int maxHeight);
	void ResizeOrderGrid();
	void SetFund(VtFund* fund);
	bool OrderByRemain() const { return _OrderByRemain; }
	void OrderByRemain(bool val) { _OrderByRemain = val; }
	int DefaultWidth() const { return _DefaultWidth; }
	void DefaultWidth(int val) { _DefaultWidth = val; }
	int DefaultHeight() const { return _DefaultHeight; }
	void DefaultHeight(int val) { _DefaultHeight = val; }
	VtOrderWndHd* ParentDlg() const { return _ParentDlg; }
	void ParentDlg(VtOrderWndHd* val) { _ParentDlg = val; }
	int GetCenterWndCount();
	void UpdateGrid();
	bool UseHogaSiseFilter() const { return _UseHogaSiseFilter; }
	void UseHogaSiseFilter(bool val) { _UseHogaSiseFilter = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void InitGridInfo();
	void InitAll();
	void InitByLoading(int height);
	virtual BOOL OnInitDialog();
	VtOrderPanelGrid& GetOrderPanelGrid()
	{
		return _OrderPanelGrid;
	}
	void ShowHideCtrl();
private:
	void ClearConfigDlg();
	std::vector<VtOrderGridConfig*> _ConfigDlgVector;
	VtOrderWndHd* _ParentDlg = nullptr;
	VtRealtimeTickQuoteGrid _TickGrid;
	VtOrderPanelGrid _OrderPanelGrid;
	VtProductRemainGrid _ProductRemainGrid;
	VtConfigGrid _ConfigGrid;
	bool _FixedCenter;
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	VtRealTickWnd* _RealTickWnd = nullptr;
	bool _Init = false;
	void RepositionProductGrid();
	void RePosStopCtrl(int level);
	bool _ShowRemainConfig = true;
	void RepositionTickGrid();
	CBrush _BrushBackSel;
	bool _ShowOrderCountArea = true;
	CBrush _BrushBackNor;
	bool _UseHogaSiseFilter = true;
public:
	void RecalControls();
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
	void RecalControl(int height);
	void AdjustControlForMode();
	int x, y, width, height;
	void ResetByCenterRow();
private:
	void GetDeposit(VtPosition* posi);
	VtSymbol*  _Symbol;
	int _OrderAmount;
	VtFilledCondition _FillCondition;
	VtPriceType _PriceType;
	
	int _StopVal;
	bool _Activated = false;
	int _TickWndPos = 0;
	int _GridGap;
	COXLayoutManager m_LayoutManager;
protected:
	CBrush m_NormalBrush;
	CBrush m_SelBrush;
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
	void RegisterRealtimeSymbol();
	void UnregisterRealtimeSymbol();
	void RegisterRealtimeAccount();
	void UnregisterRealtimeAccount();
	void RegisterOrderWnd();
	void UnregisterOrderWnd(bool useCurParent);
	CComboBox _ComboSymbol;

	void ClearPosition();
	void InitSymbol();
	void InitPosition();
	void InitOrder();

	bool FixedCenter() const { return _FixedCenter; }
	void FixedCenter(bool val);
	CEdit _EditOrderAmount;
	CSpinButtonCtrl _ScOrderAmount;
	CSpinButtonCtrl _ScStopVal;
	CEdit _EditStopVal;
	CButton _BtnAvail;
	CShadeButtonST _BtnRemain;
	CShadeButtonST _BtnSearch;
	CStatic _StaticMsg;
	CStatic _StaticProductName;
	CShadeButtonST _BtnAmt1;
	CShadeButtonST _BtnAmt2;
	CShadeButtonST _BtnAmt3;
	CShadeButtonST _BtnAmt4;
	CShadeButtonST _BtnAmt5;
	CShadeButtonST _BtnAmt6;
	CShadeButtonST _BtnSettings;
	VtEditCtrl _EditAmt;
	CShadeButtonST* _CurBtn = nullptr;
	CStatic _StaticStop;
	CButton _CheckFixedCenter;
	CButtonST _BtnRemainFund;

	void ChangeFund(VtFund* fund);
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
	bool ShowOrderCountArea() const { return _ShowOrderCountArea; }
	void ShowOrderCountArea(bool val) { _ShowOrderCountArea = val; }
	bool ShowRemainConfig() const { return _ShowRemainConfig; }
	bool _ShowTickWnd = false;
	void ShowOrderAreaInGrid(bool flag);
	void ShowStopAreaInGrid(bool flag);
	void ShowOrderCountInGrid(bool flag);
	void ShowTickWnd(bool flag);
	bool ShowTickWnd() const {
		return _ShowTickWnd;
	}
	void SetTickWndPos(int pos);
	void SetTickCount(int count);
	void SetOrderCellHeight(int value);
	void SetOrderCellWidth(int value);
	void SetShowPLConfigWnd(bool flag);
	void SetOrderArea(int height, int width);
	int GetMaxWidth();
	int GetWindowWidth();
	int GetWindowHeight();
	void RecalcLayout();
	void SetMaxTickCount(int count);
	void OnEntered();
	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
	void ApplyProfitLossForPosition();
	int GetClientAreaWidth();
	void RepositionControl();
	void RepositionConfigWnd();
	std::vector<bool>& GetGridColOptions() {
		return _OrderGridColOption;
	}
private:
	VtRefreshManager* _RefreshManager = nullptr;
	bool _Unregistered = false;
	//VtCutManager* _CutMgr;
	// 남은 잔고 만큼 주문을 낸다.
	bool _OrderByRemain;
	VtLayoutManager* _LayoutMgr;
	void AddDefaultControls();
	BOOL EnumerateDialogResource();
	std::vector<bool> _OrderGridColOption;
	void SaveControlPos();
	CRect GetClientArea(int resourceID);
public:
	VtCutManager* GetCutManager() {
		return _OrderPanelGrid.CutMgr();
	}
	int GetCountOrderGridEnabledCol();
	afx_msg void OnBnClickedBtnRemainFund();
	void ResetRemainFund();
	void CalcLayout();
	void RefreshLayout(bool recalGrid = false);
private:
	int _DefaultWidth = 482;
	int _DefaultHeight = 750;
	int GetParentHeight();
	int GetParentTitleBarHeight();
	int GetGridWidth();
	int GetRemainWidth(int parentWidth);
	int GetRemainWidth();
	VtSymbol* _DefaultSymbol = nullptr;
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnRefreshGrid(WPARAM wParam, LPARAM lParam);
};
