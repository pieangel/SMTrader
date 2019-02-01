#pragma once
#include "GenericChildDialog.h"
#include "VtRealtimeTickQuoteGrid.h"

// VtRealTickWnd dialog
class VtOrderConfigManager;
class VtSymbol;
class VtRealTickWnd : public CRHGenericChildDialog
{
	DECLARE_DYNAMIC(VtRealTickWnd)

public:
	//VtRealTickWnd(CWnd* pParent = NULL);   // standard constructor
	VtRealTickWnd();
	virtual ~VtRealTickWnd();
	virtual int CRHGetDialogID();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REAL_TICK };
#endif

	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val);
protected:
	VtRealtimeTickQuoteGrid _TickGrid;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void SetMaxTickCount(int count);
	void OnReceiveQuote(VtSymbol* sym);
	void ClearValues();
	int GetRowCount()
	{
		return _TickGrid.GetNumberRows();
	}
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
