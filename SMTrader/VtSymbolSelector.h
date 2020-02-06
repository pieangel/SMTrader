#pragma once
#include "afxcmn.h"
#include "Skel/VtGrid.h"
#include "VtSymbolCatPage.h"
#include <vector>

// VtSymbolSelector dialog
class CVtOrderCenterWnd;
class SmOrderPanelOut;
class VtSymbolSelector : public CDialogEx
{
	DECLARE_DYNAMIC(VtSymbolSelector)

public:
	VtSymbolSelector(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtSymbolSelector();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYMBOL_SELECTOR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl _TabCtrl;
	VtGrid    _Grid1;
	afx_msg void OnTcnSelchangeTabCategory(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();

private:
	void InitTabCtrl();

	VtSymbolCatPage _Page1;
	VtSymbolCatPage _Page2;
	VtSymbolCatPage _Page3;
	VtSymbolCatPage _Page4;
	VtSymbolCatPage _Page5;
	VtSymbolCatPage _Page6;

	CWnd* _CurrentPage = nullptr;
	void SetSymbolSelector();
	std::vector<VtSymbolCatPage*> _PageVector;
	void Resize();
	void Resize(int width, int height);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	void SetOrderCenterWindow(CVtOrderCenterWnd* centerWnd);
	void SetOrderCenterWindow(SmOrderPanelOut* centerWnd);
	afx_msg void OnClose();
	virtual void PostNcDestroy();
};
