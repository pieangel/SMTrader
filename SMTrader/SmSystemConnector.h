#pragma once
#include "SplitterControl.h"

// SmSystemConnector dialog
#define STD_GAP             8
#define STD_BUTTON_WIDTH    90
#define STD_BUTTON_HEIGHT   25

class SmSystemConnector : public CDialogEx
{
	DECLARE_DYNAMIC(SmSystemConnector)

public:
	SmSystemConnector(CWnd* pParent = NULL);   // standard constructor
	virtual ~SmSystemConnector();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYS_CONNECTOR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	CSplitterControl    m_wndSplitterHor;
	CSplitterControl    m_wndSplitterVer;

	afx_msg void OnMaxMinInfo(NMHDR* pNMHDR, LRESULT* pResult);

	void Resize();
	void MoveDlgItem(int nD, const CRect& rcPos, BOOL bRepaint);
	afx_msg void OnSplitter1Delta(NMHDR* pNMHDR, LRESULT* pResult);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
