#pragma once
#include "Skel/VtGrid.h"
#include "UGrid/CellTypes/UGCTSpin.h"
#include "UGrid/CellTypes/UGCTbutn.h"
#include <vector>
#define SPIN_TYPE1					160
class VtOrderCenterWndHd;
class SmOrderPanel;
class VtConfigGrid : public VtGrid
{
public:
	VtConfigGrid();
	virtual ~VtConfigGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	//cell type notifications
	int OnCellTypeNotify(long ID, int col, long row, long msg, long param);
	int OnCheckbox(long ID, int col, long row, long msg, long param);
	int OnSpinButton(long ID, int col, long row, long msg, long param);
	int OnPushButton(long ID, int col, long row, long msg, long param);
	SmOrderPanel* CenterWnd() const { return _CenterWnd; }
	void CenterWnd(SmOrderPanel* val) { _CenterWnd = val; }
	virtual void OnMouseLeaveFromMainGrid();
	virtual void OnMouseMove(int col, long row, POINT *point, UINT nFlags, BOOL processed = 0);
	void RestoreButtonCell();
private:
	int _OldBtnCol = -2;
	int _OldBtnRow = -2;
	void SetRowTitle();
	int _ColCount = 9;
	int _RowCount = 1;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);
	std::vector<int> _ColWidths;
	CPen m_ClosePen;
	//Spin Button cell type
	CUGSpinButtonType	m_spin;
	int					m_nSpinIndex;

	//Push Button cell type
	CUGButtonType		m_button;
	int					m_nButtonIndex;

	SmOrderPanel* _CenterWnd = nullptr;
};

