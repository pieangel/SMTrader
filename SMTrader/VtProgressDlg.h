#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "HdTaskArg.h"
#include "GradientStatic.h"
#include "XStatic/XColorStatic.h"
#include <map>
#include <memory>

// VtProgressDlg dialog
UINT threadfunc(LPVOID pp);
class CMainFrame;
class VtProgressDlg : public CDialogEx
{
	DECLARE_DYNAMIC(VtProgressDlg)

public:
	VtProgressDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtProgressDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROGRESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CBrush _BrushBackNor;
	virtual BOOL OnInitDialog();
	CGradientStatic _StaticState;
	CGradientStatic _StaticTask;
	void CloseDialog();
	CProgressCtrl _PrgCtrl;
	void SetPrgressPos(int pos);
	void SetTaskInfo(HdTaskInfo& taskInfo);
	void SetTaskInfo2(HdTaskInfo& taskInfo);
	CMainFrame* MainFrm = nullptr;
	virtual void PostNcDestroy();
	virtual void OnCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	void SetTaskInfo(std::string msg);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
