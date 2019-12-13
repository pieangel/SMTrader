#pragma once


// SmPortfolioDlg dialog
#include "SmCorrelationGrid.h"
class SmPortfolioDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SmPortfolioDlg)

public:
	SmPortfolioDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SmPortfolioDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PORT_COR };
#endif

	int Mode() const { return _mode; }
	void Mode(int val) { _mode = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	SmCorrelationGrid _Grid;

private:
	int _mode = 0;
	virtual void PostNcDestroy();
};
