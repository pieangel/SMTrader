#pragma once
#include "Skel/VtGrid.h"
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
class HdSymbolOptionGrid : public VtGrid
{
public:
	HdSymbolOptionGrid();
	~HdSymbolOptionGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);

	void SetColTitle();
	int _ColCount = 3;
	int _RowCount = 100;
	CFont _defFont;
	CFont _titleFont;
	CPen m_Pen;
	void QuickRedrawCell(int col, long row);

	void Begin();
	void End();

	void InitGrid();
	void InitSymbol();
	void InitYearMonth();
	void SetYearMonth();
	void SetProductSection();
	void GetSymbolMaster();
	void GetSymbolMasterByCenter();

	VtOrderLeftWndHd* LeftWnd() const { return _LeftWnd; }
	void LeftWnd(VtOrderLeftWndHd* val) { _LeftWnd = val; }
	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
	void SetRemain();
	void SetCurrent();
	void SetExpected();
	void SetRemain(VtPosition* posi);
	void SetRemain(VtOrder* order);
private:
	VtOrderLeftWndHd* _LeftWnd = nullptr;
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	VtProductSection* _CurPrdtSec = nullptr;
	void SetEqualRow(int equalRow);
	int GetMaxRow();

	std::map<std::string, std::tuple<int, int, VtSymbol*>> _CallSymbolRowMap;
	std::map<std::string, std::tuple<int, int, VtSymbol*>> _PutSymbolRowMap;
public:
	void OnSymbolMaster(VtSymbol* sym);
	void OnOrderFilled(VtOrder* order);
	void OnReceiveQuote(VtSymbol* sym);
	void OnExpected(VtSymbol* sym);
private:
	int _EqualCol;
	int _EqualRow;
	int _Mode = 1;
	std::set<VtCellPos> _RemainPos;
	void ResetRemainCells();
	void ShowPosition(bool init, int acptCnt, VtPosition* posi, std::string symCode);
	void ShowCurrent(VtSymbol* sym);
	void ShowExpected(VtSymbol* sym);
	void ShowExpected(int col, int row, VtSymbol* sym);
	void ShowCurrent(int col, int row, VtSymbol* sym);
	void SetRemain(VtSymbol* sym);
};

