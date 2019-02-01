#pragma once
#include "Skel\VtGrid.h"
class VtPreOrderGrid :
	public VtGrid
{
public:
	VtPreOrderGrid();
	virtual ~VtPreOrderGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);

	void SetColTitle();
	int _ColCount = 5;
	int _RowCount = 200;
	CFont _defFont;
	CFont _titleFont;
	CBitmap				m_CancelBitmap;

	void SetPreOrderList();
	void QuickRedrawCell(int col, long row);
	void ResetCellText();
};

