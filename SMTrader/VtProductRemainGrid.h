#pragma once
#include "Skel/VtGrid.h"
#include "UGrid/CellTypes/UGCTSeperateText.h"
#include "Global/VtDefine.h"
class VtSymbol;
class VtAccount;
struct VtQuote;
struct VtPosition;
class VtOrderConfigManager;
class VtOrderCenterWndHd;
class SmOrderPanel;
class SmOrderPanelOut;
struct VtOrder;
class VtProductRemainGrid : public VtGrid
{
public:
	void UnregisterAllCallback();

	void RegisterQuoteCallback();
	void OnQuoteEvent(const VtSymbol* symbol);

	void RegisterOrderallback();
	void OnOrderEvent(const VtOrder* order);


	VtProductRemainGrid();
	~VtProductRemainGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual int  OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow);

	void SetColTitle();
	int _ColCount = 6;
	int _RowCount = 1;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);

	//void Symbol(VtSymbol* val);
	//void Account(VtAccount* val) { _Account = val; }

	//void Position(VtPosition* val);
	void ShowPosition(VtPosition* posi);
	void ShowPosition();
	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	SmOrderPanel* CenterWnd() const { return _CenterWnd; }
	void CenterWnd(SmOrderPanel* val) { _CenterWnd = val; }
	void SetSymbol(VtSymbol* sym);
	SmOrderPanelOut* CenterWndOut() const { return _CenterWndOut; }
	void CenterWndOut(SmOrderPanelOut* val) { _CenterWndOut = val; }
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
private:
	void ShowSinglePosition();
	void ShowFundPosition();
	//VtSymbol* _Symbol = nullptr;
	//VtAccount* _Account = nullptr;
	//VtPosition* _Position = nullptr;
	SmOrderPanel* _CenterWnd;
	SmOrderPanelOut* _CenterWndOut = nullptr;
	void ResetColTitle();
	void SetColTitle(int level);
	int FindLevel(int ParentWidth);
	int FindLevel(std::vector<bool>& colOptions);
	void ShowPosition(VtPosition* posi, VtSymbol* sym);
public:
	int SetColTitleForLevel(int parentWidth);
	int SetColTitleForLevel(std::vector<bool>& colOptions);
	void OnReceiveRealtimeQuote(VtQuote* quote);
	void InitPosition();
	void ClearPosition();
	void OnReceiveQuote(VtSymbol* sym);
	void SetOrderConfigMgr(VtOrderConfigManager* val);
	void OnRemain(VtPosition* posi);
	void SetShowAvage(bool flag);
	void SetShowRemainType(bool flag);
	void OnReceiveAccountInfo();
	int GetGridWidth(int parentWidth);
	int GetGridWidth(std::vector<bool>& colOptions);
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	// 모드 : 0 : 국내, 1 : 해외
	int _Mode = 0;
};

