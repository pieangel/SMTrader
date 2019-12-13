#pragma once
#include "GenericChildDialog.h"
#include "HdSymbolFutureGrid.h"
#include "HdProfitLossGrid.h"
#include "HdSymbolOptionGrid.h"
#include "HdAssetGrid.h"
#include "afxwin.h"
#include "SmOptionGrid.h"
#include "SmFutureGrid.h"
#include "SmPLGrid.h"
#include "SmAssetGrid.h"
// VtOrderLeftHd dialog
class VtOrderConfigManager;
class VtSymbol;
struct VtPosition;
struct VtOrder;
class VtOrderLeftWndHd : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(VtOrderLeftWndHd)

public:
	//VtOrderLeftWndHd(CWnd* pParent = NULL);   // standard constructor
	VtOrderLeftWndHd();
	virtual ~VtOrderLeftWndHd();
	virtual int CRHGetDialogID();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_LEFT_HD };
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

	void OnReceiveQuote(VtSymbol* sym);
	void OnOutstanding();
	void OnExpected(VtSymbol* sym);
	void BlockEvent();
private:
	//HdSymbolFutureGrid _SymbolFutureGrid;
	SmFutureGrid _SymbolFutureGrid;
	//HdProfitLossGrid _ProfitLossGrid;
	SmPLGrid _ProfitLossGrid;
	//HdAssetGrid      _AssetGrid;
	SmAssetGrid      _AssetGrid;
	//HdSymbolOptionGrid _SymbolOptionGrid;
	SmOptionGrid _SymbolOptionGrid;
	VtOrderConfigManager* _OrderConfigMgr = nullptr;

	void Begin();
	void End();

	int _FutureSymbolMode = 1;
	int _Mode = 1;
public:
	CComboBox _ComboProduct;
	CComboBox _ComboOptionMonth;
	afx_msg void OnCbnSelchangeComboProduct();
	afx_msg void OnCbnSelchangeComboOption();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedRadioBalance();
	afx_msg void OnBnClickedRadioCurrent();
	afx_msg void OnBnClickedRadioExpect();
	void OnSymbolMaster(VtSymbol* sym);
public:
	void OnRemain(VtPosition* posi);
	void OnOrderAccepted(VtOrder* order);
	void OnOrderUnfilled(VtOrder* order);
	void OnOrderFilled(VtOrder* order);
	void OnReceiveAccountInfo();
	void RefreshProfitLoss();
	void RefreshAsset();
	void OnResizeWnd();
	void OnAccountChanged();
private:
	int _EventSeq = 0;
	void OnOrderEvent(VtOrder* order);
	int _DefaultWidth = 174;
	int _DefaultHeight = 774;
};
