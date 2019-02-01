#pragma once
#include "Skel/VtGrid.h"
class VtRemainOrderGrid : public VtGrid
{
public:
	VtRemainOrderGrid();
	~VtRemainOrderGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);

	void SetColTitle();
	int _ColCount = 8;
	int _RowCount = 200;
	CFont _defFont;
	CFont _titleFont;
	CBitmap				m_OutBitmap;

	void SetFilledOrderList();
	void QuickRedrawCell(int col, long row);
	void ResetCellText();
	void SetRemainList();
};

