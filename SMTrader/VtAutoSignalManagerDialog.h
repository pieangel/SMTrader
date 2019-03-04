#pragma once
#include "VtTotalSignalGrid.h"
#include "VtSignalConnectionGrid.h"
#include "VtSignalDefinitionGrid.h"

// VtAutoSignalManagerDialog dialog
const int RefTimer = 0x00000012;
class VtAutoSignalManagerDialog : public CDialogEx
{
	DECLARE_DYNAMIC(VtAutoSignalManagerDialog)

public:
	VtAutoSignalManagerDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtAutoSignalManagerDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYS_AUTO_CONNECT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();

private:
	VtTotalSignalGrid _TotalSigGrid;
	VtSignalConnectionGrid _ConnectGrid;
	VtSignalDefinitionGrid _DefineGrid;
	void RefreshOrder();
public:
	afx_msg void OnBnClickedBtnAddConnect();
	afx_msg void OnBnClickedBtnAddSignal();
	afx_msg void OnBnClickedBtnDelConnect();
	afx_msg void OnBnClickedBtnDelSignal();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
};
