#pragma once
#include "afxcmn.h"
#include "VtFDMultiAcntPage.h"
#include "VtFDSubAcntPage.h"
#include "ETSLayout.h"
#include "HdWindowEvent.h"
#include "Poco/BasicEvent.h"

using Poco::BasicEvent;
// VtFundDesigner dialog

class VtFundDesigner : public CDialogEx
{
	DECLARE_DYNAMIC(VtFundDesigner)

public:
	VtFundDesigner(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtFundDesigner();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FUND_DESIGNER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	VtFDMultiAcntPage _MultiAcntPage;
	VtFDSubAcntPage _SubAcntPage;
	void InitTabCtrl();
	CWnd* _CurrentPage = nullptr;

	int SelTab;
	virtual BOOL OnInitDialog();

	afx_msg void OnTcnSelchangeTabFund(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	CTabCtrl _TabCtrl;
private:
	BasicEvent<HdWindowEventArgs> _WindowEvent;
	void FireWindowEvent(HdWindowEventArgs&& arg)
	{
		_WindowEvent(this, arg);
	}
};
