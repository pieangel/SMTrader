#pragma once
#include "Skel/VtGrid.h"
#include <vector>
#include <map>
#include <string>
class CMainFrame;
class VtSymbol;
class VtMainSiseGrid : public VtGrid
{
public:
	VtMainSiseGrid();
	~VtMainSiseGrid();
	CMainFrame* MainFrm() const { return _MainFrm; }
	void MainFrm(CMainFrame* val) { _MainFrm = val; }

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);

	void OnReceiveComplete();
	void OnReceiveQuoteHd(VtSymbol* sym);
private:
	ArrowCellType	m_celllArrow;
	int				m_nArrowCellIndx;
	void ShowCurrent(VtSymbol* sym, int col, int row);
	std::pair<int, int> FindPos(std::string code);
	void SetRowTitle();
	int _ColCount = 6;
	int _RowCount = 2;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);
	std::vector<int> _ColWidths;
	std::map<std::string, std::pair<int, int>> _ItemMap;
	CPen m_ClosePen;
	CMainFrame* _MainFrm = nullptr;
};

