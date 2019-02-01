#pragma once
#include "VtChartDataCollector.h"

// VtDataCollectWnd dialog
const int RegularTimerID = 0x0001;
const int MinTimerID = 0x0002;
class VtChartData;
class VtDataCollectWnd : public CDialogEx
{
	DECLARE_DYNAMIC(VtDataCollectWnd)

public:
	VtDataCollectWnd(CWnd* pParent = nullptr);   // standard constructor
	virtual ~VtDataCollectWnd();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DATA_COL};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnEnd();

	VtChartDataCollector _DataCollector;
	VtChartData* _ChartData;
};
