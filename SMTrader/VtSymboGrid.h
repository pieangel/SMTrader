#pragma once
#include "Skel\VtGrid.h"
#include <string>



class CVtOrderCenterWnd;
class VtSymbolSelector;
class CVtSymbolFavoriteDlg;
class VtSymboGrid :
	public VtGrid
{
public:
	VtSymboGrid();
	~VtSymboGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	void QuickRedrawCell(int col, long row);
	void SetColTitle();
	int _ColCount = 40;
	int _RowCount = 20;
	CFont _defFont;
	CFont _titleFont;
	DECLARE_MESSAGE_MAP()
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
public:
	int Section() const { return _Section; }
	void Section(int val) { _Section = val; }
public:
	CVtSymbolFavoriteDlg* FavoriteDlg() const { return _FavoriteDlg; }
	void FavoriteDlg(CVtSymbolFavoriteDlg* val) { _FavoriteDlg = val; }
public:
	VtSymbolSelector* SymSelector() const { return _SymSelector; }
	void SymSelector(VtSymbolSelector* val) { _SymSelector = val; }
public:
	CVtOrderCenterWnd* OrderCenterWnd() const { return _OrderCenterWnd; }
	void OrderCenterWnd(CVtOrderCenterWnd* val) { _OrderCenterWnd = val; }
private:
	CVtOrderCenterWnd* _OrderCenterWnd = nullptr;
	VtSymbolSelector* _SymSelector = nullptr;
	CVtSymbolFavoriteDlg* _FavoriteDlg = nullptr;
	int _Source = 0;
	int _Section = 0;
public:
	void SetSource(int src);
	void SetProduct();
};

