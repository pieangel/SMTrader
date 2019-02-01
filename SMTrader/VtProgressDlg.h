#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "HdTaskArg.h"

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
	virtual BOOL OnInitDialog();
	CStatic _StaticState;
	void CloseDialog();
	CProgressCtrl _PrgCtrl;
	CStatic _StaticTask;
	void SetPrgressPos(int pos);
	void SetTaskInfo(HdTaskInfo& taskInfo);
	CMainFrame* MainFrm = nullptr;
	virtual void PostNcDestroy();
	virtual void OnCancel();
};
