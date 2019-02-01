#pragma once
#include "Skel/VtGrid.h"
#include <map>
#include <string>
#include <tuple>
class VtOrderConfigManager;
class VtSymbol;
struct VtPosition;
struct VtOrder;
class HdSymbolFutureGrid : public VtGrid
{
public:
	HdSymbolFutureGrid();
	~HdSymbolFutureGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual int  OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow);

	void SetColTitle();
	int _ColCount = 3;
	int _RowCount = 5;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);

	void Begin();
	void End();

	void InitGrid();

	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	std::map<std::string, int> SymbolMap;
	std::map<std::string, std::tuple<int, int, VtSymbol*>> _FutureCurrentValueRowMap;
	int _Mode = 1;
	COLORREF _SellColor;
	COLORREF _BuyColor;
	COLORREF _DefaultColor;
	void ShowPosition(bool init, int acptCnt, VtPosition* posi, std::string symCode);
	void GetSymbolMaster(VtSymbol* sym);
public:
	void OnReceiveQuote(VtSymbol* sym);
	void OnSymbolMaster(VtSymbol* sym);
	void SetRemain(VtPosition* posi);
	void SetRemain(VtOrder* order);
	void ShowRemain(VtSymbol* sym);
	void ShowCurrent(VtSymbol* sym, int row);
	//void ShowPosition(int acptCnt, VtPosition* posi, VtOrder* order);
	void OnOrderFilled(VtOrder* order);
	void OnExpected(VtSymbol* sym);
	void ShowExpected(VtSymbol* sym, int row);
};

