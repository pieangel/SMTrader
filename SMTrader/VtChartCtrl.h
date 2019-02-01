#pragma once


// VtChartCtrl dialog

class VtChartCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(VtChartCtrl)

public:
	VtChartCtrl(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtChartCtrl();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHART };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
