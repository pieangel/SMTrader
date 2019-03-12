#pragma once
#include "Skel/VtGrid.h"
#include "UGrid/CellTypes/UGCTSeperateText.h"
#include "Global/VtDefine.h"
#include <map>
#include <vector>
#include "VtOutSignalDefManager.h"
class VtSignalDefinitionGrid : public VtGrid
{
public:
	VtSignalDefinitionGrid();
	virtual ~VtSignalDefinitionGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnRClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnMouseMove(int col, long row, POINT *point, UINT nFlags, BOOL processed = 0);
	virtual void OnMouseLeaveFromMainGrid();
	void SetColTitle();
	int _ColCount = 4;
	int _RowCount = 100;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);
	void InitGrid();

	void ClearCells();
	void AddOutSigDef(SharedOutSigDef sig);
private:
	int _SelRow = -2;
	int _OldSelRow = -2;
	int _ClickedRow = -2;
	int _ButtonRow = -2;
	COLORREF _SelColor = RGB(255, 227, 132);
	COLORREF _ClickedColor = RGB(216, 234, 253);
};


