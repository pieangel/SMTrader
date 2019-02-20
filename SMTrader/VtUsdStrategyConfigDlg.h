#pragma once
#include "VtUsdStrategyConfigGrid.h"
#include <map>
#include <string>
#include "VtSysArgGrid.h"
#include <tuple>
#include "System/VtSystem.h"
#include "ShadeButtonST.h"
#include "VtUsdEntConfigGrid.h"
#include "afxwin.h"
#include "GradientStatic.h"
#include "HdWindowEvent.h"
#include "Poco/BasicEvent.h"

using Poco::BasicEvent;

// VtUsdStrategyConfigDlg dialog
class VtSystem;
class VtSymbol;
class VtAccount;
class VtFund;
class VtStrategyGrid;
const int ArgTimer = 0x00000008;
class VtUsdStrategyConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(VtUsdStrategyConfigDlg)

public:
	VtUsdStrategyConfigDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~VtUsdStrategyConfigDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYSTEM_USD };
#endif

	VtSystem* System() const { return _System; }
	void System(VtSystem* val);
	VtStrategyGrid* StGrid() const { return _StGrid; }
	void StGrid(VtStrategyGrid* val) { _StGrid = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton _ChecktLossCut;
	CButton _CheckTs;
	CComboBox _ComboLosscutType;
	CComboBox _ComboStrategy;
	CDateTimeCtrl _DpEntBegin;
	CDateTimeCtrl _DpEntEnd;
	CDateTimeCtrl _DpLiq;
	CEdit _EditEntMax;
	CEdit _EditLosscut;
	CEdit _EditTsPercent;
	CEdit _EditTsPl;
	CStatic _StaticPl;
	CStatic _StaticAccount;
	CStatic _StaticSymbol;
	virtual BOOL OnInitDialog();

private:
	void InitComboMap();
	std::map<int, std::pair<std::string, VtSystem*>> _SystemMap;
	VtUsdEntConfigGrid _EntGrid;
	VtUsdStrategyConfigGrid _LiqGrid;
	VtSysArgGrid _ArgGrid;
	int _SelSysIndex = 0;
	bool _ShowArgGrid;
	void ResizeWindow();
	VtSystem* _System = nullptr;
	void InitControls();
	int FindSystemIndex(std::string sysName);
	bool _EnableTrailStop;
	bool _EnableLossCut;
	bool _EnableTargetCut;
	bool _LiqByStop;
	bool _ShowingGrid = false;
	VtStrategyGrid* _StGrid = nullptr;
	int _WndHeight = 0;
	BasicEvent<HdWindowEventArgs> _WindowEvent;
	void FireWindowEvent(HdWindowEventArgs&& arg)
	{
		_WindowEvent(this, arg);
	}
	VtSymbol* _SelSymbol;
	VtFund* _Fund = nullptr;
	VtAccount* _Account = nullptr;
	int _Type = 0;
	ValueType _LossCutType = ValueType::Tick;
	ValueType _TargetProfitType = ValueType::Tick;
public:
	afx_msg void OnBnClickedBtnAccount();
	afx_msg void OnBnClickedBtnSymbol();
	CComboBox _ComboProfitTarget;
	CButton _CheckTargetCut;
	CEdit _EditProfitTarget;
	void SetTargetAcntOrFund(std::tuple<int, VtAccount*, VtFund*>& selItem);
	afx_msg void OnCbnSelchangeComboStrategy();
	void SetSymbol(VtSymbol* sym);
	afx_msg void OnBnClickedBtnExtraVar();
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnCbnSelchangeComboLosscutType();
	afx_msg void OnCbnSelchangeComboProfitTarget();
	void SetSystem(VtSystem* sys);
public:
	afx_msg void OnBnClickedCheckTs();
	afx_msg void OnBnClickedCheckLosscut();
	afx_msg void OnBnClickedCheckProfitTarget();
	afx_msg void OnBnClickedCkLiqAll();
	CButton _CheckLiqByStop;
	CShadeButtonST _BtnAcnt;
	CShadeButtonST _BtnApply;
	CShadeButtonST _BtnExtraVar;
	CShadeButtonST _BtnSymbol;
	CEdit _EditOrderAmt;
	CSpinButtonCtrl _SpinOrderAmt;
	CStatic _EntCntToday;
	void OnHogaCount(double uac, double ubc);
	void OnHogaQty(double uas, double ubs);
	afx_msg void OnClose();
	afx_msg void OnBnClickedCheckRun();
	CButton _CheckRun;
	CStatic _StaticLastEntPrice;
	void OnRealTimeEvent();
	void RefreshRealTimeValue(std::string argName, double value, int decimal = 2);
	void RefreshRealTimeValue(std::string argName, double value);
	void RefreshRealTimeValue(std::string argName, CString value);
	void RefreshRealTimeValue(int index, std::string argName, CString value);
	afx_msg void OnEnChangeEditOrderAmt();
	void UpdateRunCheck(VtSystem* sys);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CGradientStatic _StaticPosition;
};
