#pragma once
#include "afxwin.h"


// VtOrderGridConfig dialog
class VtOrderCenterWndHd;
class VtOrderGridConfig : public CDialogEx
{
	DECLARE_DYNAMIC(VtOrderGridConfig)

public:
	VtOrderGridConfig(CWnd* pParent = NULL);   // standard constructor
	VtOrderGridConfig(VtOrderCenterWndHd* centerWnd);
	virtual ~VtOrderGridConfig();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum {
		IDD = IDD_ORDERGRID_CONFIG};
#endif

	VtOrderCenterWndHd* CenterWnd() const { return _CenterWnd; }
	void CenterWnd(VtOrderCenterWndHd* val) { _CenterWnd = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit _EditCellHeight;
	CEdit _EditCellWidth;
	CEdit _EditTickCount;
	afx_msg void OnBnClickedCheckShowOrdercol();
	afx_msg void OnBnClickedCheckShowStopcol();
	afx_msg void OnBnClickedCheckShowCountcol();
	afx_msg void OnBnClickedCheckShowTickwnd();
	afx_msg void OnBnClickedRadioTickleft();
	afx_msg void OnBnClickedRadioTickright();
	afx_msg void OnBnClickedCheckShowPlconfigwnd();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedApply();
	virtual BOOL OnInitDialog();

private:
	VtOrderCenterWndHd* _CenterWnd = nullptr;
public:
	CButton _CheckShowCountCol;
	CButton _CheckShowOrderCol;
	CButton _CheckShowPLConfigWnd;
	CButton _CheckShowStopCol;
	CButton _CheckShowTickWnd;
	afx_msg void OnEnChangeEditTickCount();
	afx_msg void OnEnChangeEditCellHeight();
	afx_msg void OnBnClickedCheckFilterHoga();
	CButton _CheckFilterHoga;
};
