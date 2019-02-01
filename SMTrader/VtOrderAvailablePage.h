#pragma once


// VtOrderAvailablePage dialog

class VtOrderAvailablePage : public CDialogEx
{
	DECLARE_DYNAMIC(VtOrderAvailablePage)

public:
	VtOrderAvailablePage(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtOrderAvailablePage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ORDER_AVAILABLE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
