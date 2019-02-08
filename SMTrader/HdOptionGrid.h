#pragma once
#include "Skel\VtGrid.h"
#include <map>
class VtProductSection;
class HdOptionPage;
class VtSymbol;
class VtOrderConfigManager;
class VtUsdStrategyConfigDlg;
class HdSymbolSelecter;
class HdOptionGrid :
	public VtGrid
{
public:
	HdOptionGrid();
	virtual ~HdOptionGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnMouseMove(int col, long row, POINT *point, UINT nFlags, BOOL processed = 0);
	virtual void OnMouseLeaveFromMainGrid();
	void SetColTitle();
	int _ColCount = 3;
	int _RowCount = 100;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);

	void Begin();
	void End();

	void InitGrid();
	void InitSymbol();
	void InitYearMonth();
	void SetYearMonth();
	void SetProductSection();

	HdOptionPage* OptionPage() const { return _OptionPage; }
	void OptionPage(HdOptionPage* val) { _OptionPage = val; }

	void GetSymbolMaster();
	void GetSymbolMasterLately();
	void OnSymbolMaster(VtSymbol* sym);
	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	void SetConfigDlg(VtUsdStrategyConfigDlg* ConfigDlg);
	HdSymbolSelecter* SymSelecter() const { return _SymSelecter; }
	void SymSelecter(HdSymbolSelecter* val) { _SymSelecter = val; }
private:
	HdSymbolSelecter* _SymSelecter;
	VtProductSection* _CurPrdtSec = nullptr;
	HdOptionPage* _OptionPage = nullptr;
	void ClearInfo();
	std::map<VtSymbol*, std::pair<int, int>> _CallSymbolRowMap;
	std::map<VtSymbol*, std::pair<int, int>> _PutSymbolRowMap;
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	void SetEqualRow(int equalRow);
	int GetMaxRow();
	int _EqualCol;
	int _EqualRow;
	VtUsdStrategyConfigDlg* _UsdConfigDlg;
	std::pair<int, int> _OldSelCell;
	std::pair<int, int> _ClickedCell;
	COLORREF _SelColor = RGB(255, 227, 132);
	COLORREF _ClickedColor = RGB(216, 234, 253);
	COLORREF _CallColor = RGB(252, 226, 228);
	COLORREF _PutColor = RGB(218, 226, 245);
};

