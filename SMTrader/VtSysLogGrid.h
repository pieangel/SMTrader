#pragma once
#include "Skel/VtGrid.h"
#include <vector>
#include "VtGlobal.h"
struct VtQuote;
class VtOrderConfigManager;
class VtSymbol;
class VtOrderCenterWndHd;
class VtSysLogGrid : public VtGrid
{
public:
	VtSysLogGrid();
	~VtSysLogGrid();

	virtual void OnSetup();


	void SetColTitle();
	int _ColCount = 3;
	int _RowCount = 20;
	CFont _defFont;
	CFont _titleFont;
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnRClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	void QuickRedrawCell(int col, long row);
	void OnReceiveQuote(VtSymbol* sym);
	void OnReceiveRealtimeQuote(VtQuote* quote);
	void SetOrderConfigMgr(VtOrderConfigManager* val);
	void ClearText();
	void SetQuote();
	int MaxRow() const { return _MaxRow; }
	void MaxRow(int val); // { _MaxRow = val; }
	void ClearValues();
	VtOrderCenterWndHd* CenterWnd() const { return _CenterWnd; }
	void CenterWnd(VtOrderCenterWndHd* val) { _CenterWnd = val; }
	int GetGridWidth();
	void UpdateLog();
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	int GetMaxRow();
	int _MaxRow = 20;
	int _CellHeight;
	std::vector<int> _ColWidths;
	VtOrderCenterWndHd* _CenterWnd = nullptr;
};


