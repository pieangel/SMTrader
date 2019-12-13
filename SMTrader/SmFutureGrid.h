#pragma once
#include "Grid/GridCtrl.h"
#include <vector>
#include <map>
#include <set>
#include <functional>
#include "Global/VtDefine.h"
#include <memory>
#include <map>
#include <tuple>
#include "Global/VtDefine.h"
#include <set>
#include <string>
class VtOrderLeftWndHd;
class VtProductSection;
class VtOrderConfigManager;
class VtSymbol;
struct VtPosition;
struct VtOrder;
class VtProductOrderManager;

class SmFutureGrid : public CGridCtrl
{
public:
	SmFutureGrid();
	~SmFutureGrid();

	
	void RegisterMasterCallback();
	void RegisterQuoteCallback();
	void RegisterOrderCallback();

	void OnMasterEvent(VtSymbol* sym);
	void OnOrderEvent(VtOrder* order);
	void OnQuoteEvent(VtSymbol* sym);

	// 콜백 함수 취소
	void UnregisterAllCallback();
	
	void Init();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	
	void SetColTitle();
	int _ColCount = 3;
	int _RowCount = 50;
	int _CellHeight = 21;
	CFont _defFont;
public:
	void InitGrid();

	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	std::map<std::string, std::tuple<int, int, VtSymbol*>> _FutureCurrentValueRowMap;
	int _Mode = 1;
	COLORREF _SellColor;
	COLORREF _BuyColor;
	COLORREF _DefaultColor;
	void ShowPosition(bool init, int acptCnt, VtPosition* posi, std::string symCode);
	void GetSymbolMaster(VtSymbol* sym);

	void QuickSetBackColor(int row, int col, COLORREF color);
	void QuickSetText(int row, int col, LPCTSTR text);
	void QuickSetNumber(int row, int col, int number);
	void QuickSetTextColor(int row, int col, COLORREF color);

public:
	void OnReceiveQuote(VtSymbol* sym);
	void OnSymbolMaster(VtSymbol* sym);
	void SetRemain(VtPosition* posi);
	void SetRemain(VtOrder* order);
	void ShowRemain(VtSymbol* sym);
	void ShowCurrent(VtSymbol* sym, int row);
	void OnOrderFilled(VtOrder* order);
	void OnExpected(VtSymbol* sym);
	void ShowExpected(VtSymbol* sym, int row);
};

