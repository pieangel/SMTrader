#pragma once
#include "Skel/VtGrid.h"
#include "UGrid/CellTypes/UGCTSeperateText.h"
#include "Global/VtDefine.h"
#include <map>
#include <vector>
#include "UGrid/CellTypes/ugctelps.h"
#include "UGrid/CellTypes/ugctspin.h"

#define ELLIPSISBUTTON_CLICK_ACNT		150
#define ELLIPSISBUTTON_CLICK_PRDT		151
#define SPIN_TYPE_SEUNGSU               152

class VtSignalConnectionGrid : public VtGrid
{
public:
	VtSignalConnectionGrid();
	virtual ~VtSignalConnectionGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	//cell type notifications
	int OnCellTypeNotify(long ID, int col, long row, long msg, long param);

	void SetColTitle();
	int _ColCount = 5;
	int _RowCount = 100;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);
	void InitGrid();

	void ClearCells();

	//Ellipsis Button cell type
	CUGEllipsisType		m_ellipsis;
	int					m_nEllipsisIndex;
	//Spin Button cell type
	CUGSpinButtonType	m_spin;
	int					m_nSpinIndex;

private:
	//celltype notification handlers
	int OnDropList(long ID, int col, long row, long msg, long param);
	int OnCheckbox(long ID, int col, long row, long msg, long param);
	int OnEllipsisButton(long ID, int col, long row, long msg, long param);
	int OnSpinButton(long ID, int col, long row, long msg, long param);
};


