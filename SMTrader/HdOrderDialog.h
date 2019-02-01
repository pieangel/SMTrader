#pragma once
#include "VtOrderListGrid.h"

// HdOrderDialog dialog

class HdOrderDialog : public CDialogEx
{
	DECLARE_DYNAMIC(HdOrderDialog)

public:
	HdOrderDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~HdOrderDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_TOTAL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	VtOrderListGrid _OrderGrid;
public:
	virtual BOOL OnInitDialog();
};
