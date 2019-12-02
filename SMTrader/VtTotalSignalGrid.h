#pragma once
#include "Skel/VtGrid.h"
#include "UGrid/CellTypes/UGCTSeperateText.h"
#include "Global/VtDefine.h"
#include <map>
#include <vector>

class VtTotalSignalGrid : public VtGrid
{
public:
	VtTotalSignalGrid();
	virtual ~VtTotalSignalGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual int  OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow);

	void SetColTitle();
	int _ColCount = 9;
	int _RowCount = 10;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);
	void Refresh();
	void RefreshOrders();
	void ClearCells();
private:
	void InitGrid();
	int _RowNumber = 0;
};


