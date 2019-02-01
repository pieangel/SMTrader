#pragma once
#include "afxpropertygridctrl.h"


// VtChartProperty dialog

class VtChartProperty : public CDialogEx
{
	DECLARE_DYNAMIC(VtChartProperty)

public:
	VtChartProperty(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtChartProperty();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHART_PROPERTY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CMFCPropertyGridCtrl _PropCtrl;
};
