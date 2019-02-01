#pragma once
#include "Skel/VtGrid.h"
class HdOrderGrid : public VtGrid
{
public:
	HdOrderGrid();
	~HdOrderGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);

	void SetColTitle();
	int _ColCount = 12;
	int _RowCount = 200;
	CFont _defFont;
	CFont _titleFont;

	bool InitOrder = false;

	CBitmap				m_CancelBitmap;

	void QuickRedrawCell(int col, long row);
	void ResetCellText();

	void InitGrid();
};

