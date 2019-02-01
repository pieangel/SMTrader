#pragma once
#include "ShadeButtonST.h"
#include "afxwin.h"
#include "VtMainSiseGrid.h"
#include "BtnST.h"
// VtMainToolbar dialog
class VtSymbol;
class CMainFrame;
class VtMainToolbar : public CDialogEx
{
	DECLARE_DYNAMIC(VtMainToolbar)

public:
	VtMainToolbar(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtMainToolbar();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_TOOLBAR };
#endif

	CMainFrame* MainFrm() const { return _MainFrm; }
	void MainFrm(CMainFrame* val) { _MainFrm = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	VtMainSiseGrid _SiseGrid;
	CButtonST _BtnAcntMj;
	CButtonST _BtnAcntOrder;
	CButtonST _BtnFundMj;
	CButtonST _BtnFundOrder;
	virtual BOOL OnInitDialog();
	void OnReceiveComplete();
	void OnReceiveQuoteHd(VtSymbol* sym);
	afx_msg void OnBnClickedBtnAcntOrder();
	afx_msg void OnBnClickedFundOrder();
	afx_msg void OnBnClickedBtnAcntMj();
	afx_msg void OnBnClickedBtnFundMj();

private:
	CMainFrame* _MainFrm = nullptr;
};
