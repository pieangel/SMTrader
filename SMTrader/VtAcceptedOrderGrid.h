#pragma once
#include "Skel/VtGrid.h"
class VtAcceptedOrderGrid : public VtGrid
{
public:
	VtAcceptedOrderGrid();
	~VtAcceptedOrderGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);

	void SetColTitle();
	int _ColCount = 10;
	int _RowCount = 200;
	CFont _defFont;
	CFont _titleFont;

	bool InitOrder = false;

	CBitmap				m_CancelBitmap;

	void SetAcptOrderList();
	void QuickRedrawCell(int col, long row);
	void ResetCellText();
};

