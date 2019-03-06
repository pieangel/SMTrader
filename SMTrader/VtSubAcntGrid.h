#pragma once
#include "Skel/VtGrid.h"
#include <set>

class VtFDSubAcntPage;
class VtAccount;
class VtSubAcntGrid : public VtGrid
{
public:
	VtSubAcntGrid();
	virtual ~VtSubAcntGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnRClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnMouseMove(int col, long row, POINT *point, UINT nFlags, BOOL processed = 0);
	virtual void OnMouseLeaveFromMainGrid();
	void SetColTitle();
	int _ColCount = 3;
	int _RowCount = 100;
	CFont _defFont;
	CFont _titleFont;

	void QuickRedrawCell(int col, long row);
	
	void InitGrid(VtAccount* acnt);
	void ClearCells();
	VtAccount* GetSelectedAccount();
	void SetDefaultSelect();
	VtFDSubAcntPage* SubAcntPage() const { return _SubAcntPage; }
	void SubAcntPage(VtFDSubAcntPage* val) { _SubAcntPage = val; }
	VtAccount* ParentAccount() const { return _ParentAccount; }
	void ParentAccount(VtAccount* val) { _ParentAccount = val; }
private:
	int _SelRow = -2;
	int _OldSelRow = -2;
	int _ClickedRow = 0;
	int _ButtonRow = -2;
	int _OccupiedRowCount = 0;
	COLORREF _SelColor = RGB(255, 227, 132);
	COLORREF _ClickedColor = RGB(216, 234, 253);
	void ChangeSelectedRow(int oldRow, int newRow);
	std::set<std::pair<int, int>> _RefreshCells;
	VtFDSubAcntPage* _SubAcntPage;
	VtAccount* _ParentAccount;
};

