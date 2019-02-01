#pragma once


// CVtOrderWnd dialog
#include "VtOrderCenterWnd.h"
#include "VtOrderLeftWnd.h"
#include "VtOrderRightWnd.h"
#include "VtOrderConfigManager.h"
#include "afxwin.h"
#include "VtOrderWndEvent.h"
#include "Poco/BasicEvent.h"

using Poco::BasicEvent;

class VtProductOrderManager;
struct VtRealtimeQuote;
class VtAccount;
class VtSymbol;
class VtSymbolSelector;
class CVtOrderWnd : public CDialog
{
	DECLARE_DYNAMIC(CVtOrderWnd)

public:
	CVtOrderWnd(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVtOrderWnd();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_WND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	bool _IsOpen = true;
public:
	virtual BOOL OnInitDialog();

private:
	CVtOrderCenterWnd  _CenterWnd;
	CVtOrderLeftWnd _LeftWnd;
	CVtOrderRightWnd _RightWnd;
	CStatic _leftCtrl;
	CStatic _CenterCtrl;
	CStatic _RigthCtrl;
private:
	//VtProductOrderManager* _OrderMgr = nullptr;
public:
	void OnRealtimeQuoteReceived(VtRealtimeQuote* realtimeQuote);
	afx_msg void OnClose();

private:
	BasicEvent<VtOrderWindowEventArgs> _OrderWindowEvent;
	void FireOrderWindowEvent(VtOrderWindowEventArgs&& arg)
	{
		_OrderWindowEvent(this, arg);
	}

	VtSymbol* FindSymbol(std::string fullCode);

public:
	void OnReceiveRealtimeHoga(VtHoga* hoga);
	void OnReceiveHoga(VtHoga* hoga);
	void OnOrderReceived(VtOrder* order);
	void OnAccepted(VtOrder* order);
	void OnFilled(VtOrder* order);
	void OnConfirmModify(VtOrder* order);
	void OnConfirmCancel(VtOrder* order);
	void OnReceiveRealtimeQuote(VtQuote* quote);
	void OnReceiveQuote(VtQuote* quote);
	void OnReceiveSymbolMaster(VtSymbolMaster* master);
	void OnReceiveRealtimeSymbolMaster(VtSymbolMaster* master);
	void OnSymbolClicked(VtSymbol* symbol, int src, VtSymbolSelector* srcWnd);
	void OnReceiveAccountDeposit(VtAccount* acnt);

	void ResetSymbol(VtSymbol* sym);
	void SetHoga(VtHoga* hoga);
	void SetQuote(VtQuote* quote);
	void SetMaster(VtSymbolMaster* master);
public:
	void ExtendLeftWnd(bool flag);
	VtOrderConfigManager OrderConfigMgr;

	void RefreshAccount();
	void RefreshSymbol();

	void RecalcLayout();

	static int GetValue(CEdit* edit)
	{
		CString msg;
		edit->GetWindowText(msg);

		return std::stoi(msg.GetBuffer());
	}

	static void SetValue(CEdit* edit, int value)
	{
		CString msg;
		msg.Format(_T("%d"), value);
		edit->SetWindowText(msg);
	}
};
