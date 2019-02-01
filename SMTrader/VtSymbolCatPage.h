#pragma once
#include "VtSymboGrid.h"

// VtSymbolCatPage1 dialog
class VtSymbolSelector;
class CVtSymbolFavoriteDlg;
class VtSymbolCatPage : public CDialogEx
{
	DECLARE_DYNAMIC(VtSymbolCatPage)

public:
	VtSymbolCatPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtSymbolCatPage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYM_CAT1 };
#endif

	int Section() const { return _Section; }
	void Section(int val) { _Section = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	VtSymboGrid  _GridCtrl;
	void SetProduct();
	int _Section;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	virtual BOOL OnInitDialog();

public:
	void SetOrderCenterWindow(CVtOrderCenterWnd* centerWnd);
	void SetSymbolSelector(VtSymbolSelector* symSelector);
	void SetSource(int src);
	void SetFavoriteDlg(CVtSymbolFavoriteDlg* favoriteDlg);
};
