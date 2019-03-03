#pragma once
#include "Skel\VtGrid.h"
#include <map>
class HdFuturePage;
class VtProductSection;
class VtSymbol;
class VtOrderConfigManager;
class VtUsdStrategyConfigDlg;
class HdSymbolSelecter;
class VtAddConnectSignalDlg;
class VtSignalConnectionGrid;
class HdFutureGrid :
	public VtGrid
{
public:
	HdFutureGrid();
	virtual ~HdFutureGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnMouseMove(int col, long row, POINT *point, UINT nFlags, BOOL processed = 0);
	virtual void OnMouseLeaveFromMainGrid();

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
	HdSymbolSelecter* SymSelecter() const { return _SymSelecter; }
	void SymSelecter(HdSymbolSelecter* val) { _SymSelecter = val; }
	void SetAddConSigDlg(VtAddConnectSignalDlg* conSigDlg)
	{
		_AddConSigDlg = conSigDlg;
	}
	void SetSigConGrid(VtSignalConnectionGrid* sigConGrid)
	{
		_SigConGrid = sigConGrid;
	}
private:
	HdSymbolSelecter* _SymSelecter;
	int _OldSelRow = -2;
	int _ClickedRow = -2;
	COLORREF _SelColor = RGB(255, 227, 132);
	COLORREF _ClickedColor = RGB(216, 234, 253);

	HdFuturePage* _FuturePage = nullptr;
	VtProductSection* _PrdtSec = nullptr;
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	void ClearSymbolInfo();
	VtUsdStrategyConfigDlg* _UsdConfigDlg;
	VtAddConnectSignalDlg* _AddConSigDlg = nullptr;
	VtSignalConnectionGrid* _SigConGrid = nullptr;
};

