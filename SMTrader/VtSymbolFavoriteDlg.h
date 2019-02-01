#pragma once
#include "Skel/VtGrid.h"
#include "VtSymbolCatPage.h"
#include "afxcmn.h"
#include "VtFavoritGrid.h"

// CVtSymbolFavoriteDlg dialog
class VtOrderConfigManager;
class VtSymbol;
class CVtOrderCenterWnd;
class CVtSymbolFavoriteDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CVtSymbolFavoriteDlg)

public:
	CVtSymbolFavoriteDlg(CWnd* pParent = NULL);   // standard constructor
	CVtSymbolFavoriteDlg(VtOrderConfigManager* orderConfigMgr, CWnd* pParent = NULL);   // standard constructor
	virtual ~CVtSymbolFavoriteDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYMBOL_FAVORITE };
#endif

	VtSymbol* SrcSymbol() const { return _SrcSymbol; }
	void SrcSymbol(VtSymbol* val) { _SrcSymbol = val; }
	VtSymbol* DstSymbol() const { return _DstSymbol; }
	void DstSymbol(VtSymbol* val) { _DstSymbol = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnTcnSelchangeTabCategory(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()

private:
	VtFavoritGrid    _FavoriteGrid;

	void InitTabCtrl();
	void InitFavorite();

	VtSymbolCatPage _Page1;
	VtSymbolCatPage _Page2;
	VtSymbolCatPage _Page3;
	VtSymbolCatPage _Page4;
	VtSymbolCatPage _Page5;
	VtSymbolCatPage _Page6;

	CWnd* _CurrentPage = nullptr;
	VtSymbol* _SrcSymbol = nullptr;
	VtSymbol* _DstSymbol = nullptr;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CTabCtrl _TabCtrl;
	void AddFavorite(VtSymbol* symbol);
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
public:
	afx_msg void OnBnClickedBtnInOne();
	afx_msg void OnBnClickedBtnOutOne();
	afx_msg void OnBnClickedBtnOutAll();
	afx_msg void OnClose();

	CVtOrderCenterWnd* OrderCenterWnd = nullptr;
	afx_msg void OnBnClickedBtnCloseFv();
	afx_msg void OnBnClickedOk();
};
