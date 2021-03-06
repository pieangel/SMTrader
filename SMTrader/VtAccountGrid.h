#pragma once
#include "Skel/VtGrid.h"
#include <map>
#include <string>
#include <vector>
class VtAccount;
class VtFundEditor;
class VtFundAccount;
class VtFDSubAcntPage;
class VtAccountGrid : public VtGrid
{
public:
	VtAccountGrid();
	virtual ~VtAccountGrid();

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

	void InitGrid(std::vector<VtAccount*>& acntVector);
	void InitGrid();
	VtFundEditor* FundEditor() const { return _FundEditor; }
	void FundEditor(VtFundEditor* val) { _FundEditor = val; }

	void ClearCells();
	VtAccount* GetSelectedAccount();
	VtFDSubAcntPage* SubAcntPage() const { return _SubAcntPage; }
	void SubAcntPage(VtFDSubAcntPage* val) { _SubAcntPage = val; }
private:
	int _SelRow = -2;
	int _OldSelRow = -2;
	int _ClickedRow = -2;
	int _ButtonRow = -2;
	int _OccupiedRowCount = 0;
	COLORREF _SelColor = RGB(255, 227, 132);
	COLORREF _ClickedColor = RGB(216, 234, 253);
	void ChangeSelectedRow(int oldRow, int newRow);
	VtFundEditor* _FundEditor;
	VtFDSubAcntPage* _SubAcntPage;
};

