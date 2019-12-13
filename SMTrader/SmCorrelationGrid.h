#pragma once
#include "Skel/VtGrid.h"
#include "UGrid/CellTypes/UGCTSeperateText.h"
#include "UGrid/CellTypes/UGCTbutn.h"
#include "UGrid/CellTypes/UGCTRemain.h"
#include "Global/VtDefine.h"
#include <vector>
#include <set>
#include <map>
#include <queue>
#include "UGrid/CellTypes/UGCTOrder.h"
#include "VtOrderConfigManager.h"
#include "Global/VtDefine.h"
#include "VtDoubleBufferedQueue.h"
class SmCorrelationGrid : public VtGrid
{
public:
	SmCorrelationGrid();
	~SmCorrelationGrid();

	virtual void OnSetup();

	void SetColTitle(int mode);
	void SetRowTitle(int mode);
	int _ColCount = 16;
	int _RowCount = 8;
	int _HeadHeight = 18;
	int _CellHeight = 22;
	CFont _defFont;
	CFont _titleFont;
	CFont _CenterFont;
	CFont _CursorFont;
	CBitmap				m_CancelBitmap;
	CPen m_Pen;
	CPen m_ClosePen;
	CBitmap				m_noteBitmap;
	//Push Button cell type
	CUGButtonType		m_button;
	int					m_nButtonIndex;


	void QuickRedrawCell(int col, long row);
	void SetContents(int mode);
	int Mode() const { return _mode; }
	void Mode(int val) { _mode = val; }
private:
	int _mode = 0;
};

