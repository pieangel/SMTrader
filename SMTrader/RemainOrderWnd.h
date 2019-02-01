#pragma once
#include "afxdockablepane.h"
#include "GridButton.h"
#include "VtRemainOrderGrid.h"
class CRemainOrderWnd :
	public CDockablePane
{
public:
	CRemainOrderWnd();
	~CRemainOrderWnd();

	VtRemainOrderGrid _GridCtrl;
	CGridButton     _BtnCommand;
	int commandButtonWidth = 22;
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnBnClickedCommandButton();
	void OnUpdateClickCommandButton(CCmdUI* pCmdUI);
public:
	void RefreshOrders();
	void ReceivedRemain();
};

