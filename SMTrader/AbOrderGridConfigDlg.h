#pragma once
// AbOrderGridConfigDlg dialog

#pragma once
#include "afxwin.h"


// AbOrderGridConfigDlg dialog
class VtOrderCenterWndHd;
class SmOrderPanelOut;
class AbOrderGridConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(AbOrderGridConfigDlg)

public:
	AbOrderGridConfigDlg(CWnd* pParent = NULL);   // standard constructor
	AbOrderGridConfigDlg(SmOrderPanelOut* centerWnd);
	virtual ~AbOrderGridConfigDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum {
		IDD = IDD_ORDERGRID_CONFIG
	};
#endif

	SmOrderPanelOut* CenterWnd() const { return _CenterWnd; }
	void CenterWnd(SmOrderPanelOut* val) { _CenterWnd = val; }
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
	SmOrderPanelOut* _CenterWnd = nullptr;
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
