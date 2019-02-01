#pragma once
#include "Skel\VtGrid.h"
#include <string>
class VtOrderConfigManager;
class CVtSymbolFavoriteDlg;
class VtFavoritGrid : public VtGrid
{
public:
	VtFavoritGrid();
	~VtFavoritGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);

	virtual int OnEditStart(int col, long row, CWnd **edit);
	virtual int OnEditVerify(int col, long row, CWnd *edit, UINT *vcKey);
	virtual int OnEditFinish(int col, long row, CWnd *edit, LPCTSTR string, BOOL cancelFlag);
	virtual int OnEditContinue(int oldcol, long oldrow, int* newcol, long* newrow);

	void QuickRedrawCell(int col, long row);
	void SetColTitle();
	int _ColCount = 3;
	int _RowCount = 8;
	CFont _defFont;
	CFont _titleFont;

	void SetFavorite();
	void SetOrderConfigMgr(VtOrderConfigManager* val);
	void ClearAllText();
	CVtSymbolFavoriteDlg* SymFvrtDlg() const { return _SymFvrtDlg; }
	void SymFvrtDlg(CVtSymbolFavoriteDlg* val) { _SymFvrtDlg = val; }
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	CVtSymbolFavoriteDlg* _SymFvrtDlg = nullptr;
};

