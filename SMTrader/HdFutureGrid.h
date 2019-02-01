#pragma once
#include "Skel\VtGrid.h"
#include <map>
class HdFuturePage;
class VtProductSection;
class VtSymbol;
class VtOrderConfigManager;
class VtUsdStrategyConfigDlg;
class HdFutureGrid :
	public VtGrid
{
public:
	HdFutureGrid();
	virtual ~HdFutureGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);

	void SetColTitle();
	int _ColCount = 3;
	int _RowCount = 10;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);
	void InitMarket();
	HdFuturePage* FuturePage() const { return _FuturePage; }
	void FuturePage(HdFuturePage* val) { _FuturePage = val; }
	void SetProductSection();
	void InitGrid();
	void GetSymbolMaster();
	void OnSymbolMaster(VtSymbol* sym);
	std::map<VtSymbol*, int> SymbolRowMap;
	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	void SetConfigDlg(VtUsdStrategyConfigDlg* ConfigDlg);
private:
	HdFuturePage* _FuturePage = nullptr;
	VtProductSection* _PrdtSec = nullptr;
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	void ClearSymbolInfo();
	VtUsdStrategyConfigDlg* _UsdConfigDlg;
};

