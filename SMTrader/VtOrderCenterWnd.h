#pragma once
#include "GenericChildDialog.h"
#include "VtOrderPanelGrid.h"
#include "VtProductRemainGrid.h"
#include "afxwin.h"
#include "afxcmn.h"
#include <vector>
// CVtOrderCenterWnd dialog
class VtProductOrderManager;
struct VtRealtimeHoga;
struct VtRealtimeQuote;
class CVtOrderRightWnd;
class CVtOrderLeftWnd;
class VtAccount;
class VtSymbol;
class CVtOrderWnd;
class VtOrderConfigManager;
class VtSymbolSelector;
class CVtOrderCenterWnd : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(CVtOrderCenterWnd)

public:
	//CVtOrderCenterWnd(CWnd* pParent = NULL);   // standard constructor
	CVtOrderCenterWnd();
	virtual ~CVtOrderCenterWnd();
	virtual int CRHGetDialogID();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_CENTER };
#endif

	CVtOrderRightWnd* RightWnd() const { return _RightWnd; }
	void RightWnd(CVtOrderRightWnd* val) { _RightWnd = val; }
	CVtOrderLeftWnd* LeftWnd() const { return _LeftWnd; }
	void LeftWnd(CVtOrderLeftWnd* val) { _LeftWnd = val; }
	CVtOrderWnd* ParentOrderWnd() const { return _ParentOrderWnd; }
	void ParentOrderWnd(CVtOrderWnd* val) { _ParentOrderWnd = val; }
	void SetOrderConfigMgr(VtOrderConfigManager* val);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

private:
	int _StartProductIndex = 0;
	VtOrderPanelGrid _OrderPanelGrid;
	VtProductRemainGrid _ProductRemainGrid;
	CFont SFont;
	//VtProductOrderManager* _OrderMgr = nullptr;
	CComboBox _ComboAccount;
	CComboBox _ComboProduct;
	CStatic   _ProductName;
	afx_msg void OnCbnSelchangeComboAccount();
	afx_msg void OnCbnSelchangeComboProduct();
	void InitAccount();
	void InitProduct();
	void SetSymbolByFavoriteButton(int index);
	void InitPosition();
	void DoBundleTaskForSymbol(std::string fullCode);
public:
	afx_msg void OnBnClickedBtnLoadinfo();

public:
	void InitFavorite();
	void InitQuote(VtQuote* quote);
	void InitHoga(VtHoga* hoga);
	void OnReceiveRealtimeQuote(VtRealtimeQuote* realQuote);
	void OnReceiveRealtimeHoga(VtRealtimeHoga* realHoga);
	void OnReceiveRealtimeQuote(VtQuote* quote);
	void OnReceiveRealtimeHoga(VtHoga* hoga);
	void OnOrderReceived(VtOrder* order);

	void OnAccepted(VtOrder* order);
	/// <summary>
	/// 체결 확인 메시지 처리
	/// </summary>
	/// <param name="real"></param>
	void OnFilled(VtOrder* order);
	/// <summary>
	/// 수정확인 메시지 처리
	/// </summary>
	/// <param name="real"></param>
	void OnConfirmModify(VtOrder* order);
	/// <summary>
	/// 취소확인 메시지 처리
	/// </summary>
	/// <param name="real"></param>
	void OnConfirmCancel(VtOrder* order);
private:
	CVtOrderRightWnd* _RightWnd = nullptr;
	CVtOrderLeftWnd* _LeftWnd = nullptr;
	void SetPosition(VtSymbol* symbol);
	VtAccount* _Account = nullptr;
	VtSymbol* _Symbol = nullptr;

	void SetAccount(VtAccount* acnt);
	void SetSymbol(VtSymbol* sym);
	void UpdateRealtimeOrder(VtOrder* order);

	CVtOrderWnd* _ParentOrderWnd = nullptr;
public:
	afx_msg void OnBnClickedBtnLeftExtend();

private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
public:
	std::vector<CButton*> FavoriteButtonVector;
	// the count of the order
	CEdit _EditOrderAmount;
	// the stop slip size
	CEdit _EditStopVal;
	CSpinButtonCtrl _ScOrderAmount;
	CSpinButtonCtrl _ScStopVal;
	afx_msg void OnEnChangeEditStopval();
	afx_msg void OnEnChangeEditOrderAmount();
	afx_msg void OnBnClickedBtnOrderAmt1();
	afx_msg void OnBnClickedBtnOrderAmt2();
	afx_msg void OnBnClickedBtnOrderAmt3();
	afx_msg void OnBnClickedBtnOrderAmt4();
	afx_msg void OnBnClickedBtnOrderAmt5();
	afx_msg void OnBnClickedBtnOrderAmt6();
	afx_msg void OnBnClickedBtnSelSymbol();
public:
	void OnSymbolClicked(VtSymbol* symbol, int src, VtSymbolSelector* srdWnd);
private:
	VtSymbolSelector* _SymSelector = nullptr;
public:
	void ChangeFocus();
	afx_msg void OnBnClickedBtnFavorite();
	afx_msg void OnBnClickedBtnGetAcntInfo();
	CStatic _RemainAmount;
	CStatic _OrderableAmount;
	CEdit _AccountPwd;
	void ResetSymbol(VtSymbol* sym);
	afx_msg void OnBnClickedBtnLiqAll();
	afx_msg void OnBnClickedBtnLiqSel();
	afx_msg void OnBnClickedBtnCancelAll();
	afx_msg void OnBnClickedBtnCancelSel();
	afx_msg void OnBnClickedBtnFv1();
	afx_msg void OnBnClickedBtnFv2();
	afx_msg void OnBnClickedBtnFv3();
	afx_msg void OnBnClickedBtnFv4();
	afx_msg void OnBnClickedBtnFv5();
	afx_msg void OnBnClickedBtnFv6();
	afx_msg void OnBnClickedBtnFv7();
	afx_msg void OnBnClickedBtnFv8();
	CButton _BtnFv1;
	CButton _BtnFv2;
	CButton _BtnFv3;
	CButton _BtnFv4;
	CButton _BtnFv5;
	CButton _BtnFv6;
	CButton _BtnFv7;
	CButton _BtnFv8;

public:
	void SetFavorite(int index, VtSymbol* sym);
	afx_msg void OnBnClickedBtnRightExtend();
	CButton _BtnLeftExtend;
	CButton _BtnRightExtend;
};
