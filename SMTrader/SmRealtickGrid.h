#pragma once
#include "Grid/GridCtrl.h"
#include <vector>
#include <map>
#include <set>
#include <functional>
#include "Global/VtDefine.h"
#include <memory>
struct VtQuote;
class VtOrderConfigManager;
class VtSymbol;
class VtOrderCenterWndHd;
class SmOrderPanelOut;

class SmRealtickGrid : public CGridCtrl
{
public:
	SmRealtickGrid();
	~SmRealtickGrid();

	void UnregisterAllCallback();

	void RegisterQuoteCallback();
	void OnQuoteEvent(VtSymbol* symbol);

	void Init();


	void SetColTitle();
	int _ColCount = 3;
	int _RowCount = 20;
	CFont _defFont;
	CFont _titleFont;
	void OnReceiveQuote(VtSymbol* sym);
	void SetOrderConfigMgr(VtOrderConfigManager* val);
	void ClearText();
	int MaxRow() const { return _RowCount; }
	void MaxRow(int val); // { _MaxRow = val; }
	void ClearValues();
	SmOrderPanelOut* CenterWnd() const { return _CenterWnd; }
	void CenterWnd(SmOrderPanelOut* val) { _CenterWnd = val; }
	int GetGridWidth();
	VtSymbol* Symbol() const { return _Symbol; }
	void Symbol(VtSymbol* val);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
private:
	VtSymbol* _Symbol = nullptr;
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	int GetMaxRow();
	int _CellHeight;
	std::vector<int> _ColWidths;
	SmOrderPanelOut* _CenterWnd = nullptr;
};


