#pragma once
#include "Skel/VtGrid.h"
#include <vector>
#include <map>
#include <string>
class VtChartWindow;
class VtColorManager;
class VtSymbol;
class VtChartData;
class VtChartContainer;
class VtChartFavoriteGrid : public VtGrid
{
public:
	VtChartFavoriteGrid();
	virtual ~VtChartFavoriteGrid();

	virtual void OnSetup();
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual int OnCellTypeNotify(long ID, int col, long row, long msg, long param);
	void SetColTitle();
	void ResizeColWidth(int parendWidth);
	int _ColCount = 3;
	int _RowCount = 100;
	CFont _defFont;
	CFont _titleFont;
	CPen m_Pen;
	void QuickRedrawCell(int col, long row);
	void InitGrid();
	void SetChartData(VtChartWindow* activeWnd);
	void OnReceiveChartData(std::string symCode);
	VtChartContainer* ChartContainer() const { return _ChartContainer; }
	void ChartContainer(VtChartContainer* val) { _ChartContainer = val; }
private:
	std::vector<int> _ColWidthVector;
	VtColorManager* _ColorMgr;
	std::map<std::string, std::pair<int,VtChartData*>> _DataMap;
	VtChartContainer* _ChartContainer = nullptr;
};

