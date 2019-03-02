#pragma once
#include "VtTotalSignalGrid.h"
#include "VtSignalConnectionGrid.h"
#include "VtSignalDefinitionGrid.h"

// VtAutoSignalManagerDialog dialog

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
public:
	afx_msg void OnBnClickedBtnAddConnect();
	afx_msg void OnBnClickedBtnAddSignal();
};
