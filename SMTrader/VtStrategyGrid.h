#pragma once
#include "Skel/VtGrid.h"
#include <map>
#include <vector>
#include "UGrid/CellTypes/UGCTbutn.h"
class VtStrategyToolWnd;
class VtSystem;
class VtStrategyGrid : public VtGrid
{
public:
	VtStrategyGrid();
	virtual ~VtStrategyGrid();

	virtual void OnSetup();
	virtual int  OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow);

	//cell type notifications
	int OnCellTypeNotify(long ID, int col, long row, long msg, long param);
	int OnCheckbox(long ID, int col, long row, long msg, long param);
	int OnPushButton(long ID, int col, long row, long msg, long param);
	void SetRowTitle();
	int _ColCount = 2;
	int _RowCount = 60;
	int _HeadHeight = 24;
	CFont _defFont;
	CPen m_Pen;
	//Push Button cell type
	CUGButtonType		m_button;
	int					m_nButtonIndex;
	CBitmap				_SetBitmap;
	void QuickRedrawCell(int col, long row);
	std::map<int, std::pair<bool, std::vector<int>>> _CatMap;
	std::map<int, int> _HeightMap;
	std::map<int, VtSystem*> _SystemMap;
	std::vector<int> _ColWidthMap;
	void InitGrid();
	void ResizeWindow();
	VtStrategyToolWnd* ToolWnd() const { return _ToolWnd; }
	void ToolWnd(VtStrategyToolWnd* val) { _ToolWnd = val; }
	void DisableAllSystems();
	void UpdateSystem(int row, bool enable);
private:
	VtStrategyToolWnd* _ToolWnd;
};

