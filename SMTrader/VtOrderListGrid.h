#pragma once
#include "Skel/VtGrid.h"
#include <map>
#include <vector>
class VtOrderListGrid : public VtGrid
{
public:
	VtOrderListGrid();
	~VtOrderListGrid();

	virtual void OnSetup();
	virtual int  OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow);
	void SetColTitle();
	int _ColCount = 12;
	int _RowCount = 60;
	int _HeadHeight = 18;
	CFont _defFont;
	CFont _titleFont;
	CFont _CenterFont;
	CFont _CursorFont;
	CBitmap				m_CancelBitmap;
	CPen m_Pen;
	CPen m_ClosePen;


	void QuickRedrawCell(int col, long row);
private:
	std::vector<int> _GridColVec;

};

