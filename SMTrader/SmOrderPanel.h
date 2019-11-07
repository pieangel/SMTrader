#pragma once
#include "SmOrderGrid.h"

// SmOrderPanel dialog
class VtSymbol;
class SmTest {
public:
	void OnSise(VtSymbol* symbol);
};

class SmOrderPanel : public CDialogEx
{
	DECLARE_DYNAMIC(SmOrderPanel)

public:
	SmOrderPanel(CWnd* pParent = NULL);   // standard constructor
	virtual ~SmOrderPanel();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_PANEL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	SmOrderGrid m_Grid;
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
