#pragma once
#include "Grid/GridCtrl.h"
#include <vector>
#include <map>
#include <set>
#include <functional>
#include "Global/VtDefine.h"

//m_Pen.CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
enum class SmOrderGridCol {
	STOP,
	ORDER,
	COUNT,
	QUANTITY,
	CENTER
};

class VtOrderCenterWndHd;
class VtSymbol;
class VtAccount;
class VtOrderConfigManager;
class VtStopOrderManager;
class VtCutManager;
class SmOrderGrid : public CGridCtrl
{
public:
	SmOrderGrid();
	~SmOrderGrid();
	VtSymbol* Symbol() const { return _Symbol; }
	void Symbol(VtSymbol* val);
	VtAccount* Account() const { return _Account; }
	void Account(VtAccount* val) { _Account = val; }
	VtCutManager* CutMgr() const { return _CutMgr; }
	void CutMgr(VtCutManager* val) { _CutMgr = val; }
	int CellHeight() const { return _CellHeight; }
	void CellHeight(int val) { _CellHeight = val; }
	bool ShowOrderQuantityCol() const { return _ShowOrderQuantityCol; }
	void ShowOrderQuantityCol(bool val) { _ShowOrderQuantityCol = val; }
	bool ShowStopCol() const { return _ShowStopCol; }
	void ShowStopCol(bool val) { _ShowStopCol = val; }
	bool ShowingQuoteHoga() const { return _ShowingQuoteHoga; }
	void ShowingQuoteHoga(bool val) { _ShowingQuoteHoga = val; }
	int OrderCountWidth() const { return _OrderCountWidth; }
	void OrderCountWidth(int val) { _OrderCountWidth = val; }
	int OrderQuantityWidth() const { return _OrderQuantityWidth; }
	void OrderQuantityWidth(int val) { _OrderQuantityWidth = val; }
	bool ShowOrderCol() const { return _ShowOrderCol; }
	void ShowOrderCol(bool val) { _ShowOrderCol = val; }
	bool ShowOrderCountCol() const { return _ShowOrderCountCol; }
	void ShowOrderCountCol(bool val) { _ShowOrderCountCol = val; }
	bool ShowingCells() const { return _ShowingCells; }
	void ShowingCells(bool val) { _ShowingCells = val; }
	VtOrderCenterWndHd* CenterWnd() const { return _CenterWnd; }
	void CenterWnd(VtOrderCenterWndHd* val) { _CenterWnd = val; }
	int OrderWidth() const { return _OrderWidth; }
	void OrderWidth(int val) { _OrderWidth = val; }
public:
	void RegisterQuoteCallback();
	void UnregisterQuoteCallback();
	void OnUpdateSise(const VtSymbol* symbol);

	void RegisterHogaCallback();
	void UnregisterHogaCallback();
	void OnUpdateHoga(const VtSymbol* symbol);

	void RegisterOrderallback();
	void UnregisterOrderCallback();
	void OnOrderEvent(const VtOrder* order);

	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	void SetCenterValue();
	void RefreshAllValues();
	void Init();
private:
	// 컬럼 타이틀 설정
	void SetColTitle(bool init);

	// 중간값 설정
	void SetCenterValue(const VtSymbol*, std::set<std::pair<int, int>>& refreshSet);
	void SetCenterValueByFixed(const VtSymbol* symbol, std::set<std::pair<int, int>>& refreshSet);

	VtOrderConfigManager* _OrderConfigMgr = nullptr;
	std::vector<COLORREF> BuyColor;
	std::vector<COLORREF> SellColor;
	std::map<SmOrderGridCol, int> _GridColMap;
	int _ColCount = 9;
	int _RowCount = 60;
	int _HeadHeight = 30;
	int _FixedCol = 0;
	int _FixedRow = 1;

	CFont _defFont;
	CFont _titleFont;
	CFont _CenterFont;
	CFont _CursorFont;
	bool _Init = false;

	int _StartRowForValue = 2;
	int _EndRowForValue = 0;

	int _CloseRow = 0;
	int _HighRow = 0;
	int _LowRow = 0;
	int _OpenRow = 0;
	int _PreCloseRow = 0;

	std::map<int, int> ValueToRowMap;
	std::map<int, int> RowToValueMap;
	std::set<std::pair<int, int>> _OrderPos;
	std::set<std::pair<int, int>> _HogaPos;
	std::set<std::pair<int, int>> _StopOrderPos;
	std::set<std::pair<int, int>> _QuotePos;

	void ClearQuotes(std::set<std::pair<int, int>>& refreshSet);
	// 선물 중간값 설정
	void SetCenterValueForFuture(const VtSymbol* symbol, std::set<std::pair<int, int>>& refreshSet);
	// 옵션 중간값 설정
	void SetCenterValueForOption(const VtSymbol* symbol, std::set<std::pair<int, int>>& refreshSet);
	// 시세 고정시 선물 중간값 설정
	void SetCenterValueByFixedForFuture(const VtSymbol* symbol, std::set<std::pair<int, int>>& refreshSet);
	// 시세 고정시 옵션 중간값 설정
	void SetCenterValueByFixedForOption(const VtSymbol* symbol, std::set<std::pair<int, int>>& refreshSet);
	// 중간값 찾기 함수
	int FindCenterRow();
	// 현재 최대 행갯수 구하기
	int GetMaxRow();
	// 호가 정보 설정
	void SetHogaInfo(const VtSymbol* symbol, std::set<std::pair<int, int>>& refreshSet);
	int FindRowFromCenterValue(int value);
	int FindRowFromCenterValue(const VtSymbol* sym, int value);
	// 시세 색상 설정
	void SetQuoteColor(const VtSymbol* symbol, std::set<std::pair<int, int>>& refreshSet);
	// 셀 배경색 설정
	void SetSiseCellBackColor(int minRow, int maxRow, int start, int end, COLORREF color);
	// 시세 텍스트 색상과 레이블 설정
	void SetSiseLabel(int row, LPCTSTR text, COLORREF textColor);
	// 호가 표시를 없앤다.
	void ClearHogas(std::set<std::pair<int, int>>& refreshSet);
	// 주문 설정
	void SetOrderInfo(std::set<std::pair<int, int>>& refreshSet);
	// 주문 표시 없앰
	void ClearOldOrders(std::set<std::pair<int, int>>& refreshSet);

	// 스탑 주문 설정
	void SetStopOrderInfo(std::set<std::pair<int, int>>& refreshSet);
	// 스탑 주문 표시 없앰
	void ClearOldStopOrders(std::set<std::pair<int, int>>& refreshSet);
	// 스탑 주문 라인 그리기
	void CalcPosStopOrders(std::set<std::pair<int, int>>& refreshSet);
	
	void RefreshCells(std::set<std::pair<int, int>>& refreshSet);

	void PutOrder(int price, VtPositionType position, VtPriceType priceType = VtPriceType::Price);

	void SetOrderAreaColor();


public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

private:
	VtSymbol* _Symbol = nullptr;
	std::string _SymbolCode;
	VtAccount* _Account = nullptr;
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

private:
	CRect _DragStartRect;
	CRect _DragEndRect;
	int _DragStartCol = 0;
	int _DragStartRow = 0;
	
	void DrawArrow(int direction, POINT *point, CDC* pdc, POINT p0, POINT p1, int head_length, int head_width);
	//int _OldMMRow = 0;
	//int _OldMMCol = 0;
	BOOL m_bMouseTracking;
	void CleanOldOrderLine(CCellID& cell);
	void CleanOldOrderTrackLine(CCellID& cell);
	void RedrawOrderTrackCells();
	void AddStopOrder(int price, VtPositionType posi);
	CCellID _OldClickedCell;
	CCellID _OldMMCell;
	CCellID _OldMovingCellCenter;
	CCellID _OldMovingCellSide;
	void InvalidateClickedCell();
	void SetMovingCell(CCellID cell);
	void HandleButtonEvent(int button_id);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);

private:
	VtStopOrderManager* _StopOrderMgr = nullptr;
	bool _ShowingQuoteHoga;
	int _OldOrderWidth;
	int _CellHeight = 18;
	int _StopWidth = 48;
	int _OrderWidth = 60;
	int _OrderCountWidth = 45;
	int _OrderQuantityWidth = 45;
	bool _ShowStopCol = true;
	bool _ShowOrderCol = true;
	bool _ShowOrderCountCol = true;
	bool _ShowOrderQuantityCol = true;
	bool _ShowingCells = false;

	VtCutManager* _CutMgr;

	VtOrderCenterWndHd* _CenterWnd;

public:
	void ResizeGrid(int height);
	void InitInfo();
	void ClearAllStopOrders();
	void ResetByCenterRow();
	void LiqudAll(VtPriceType priceType, int price = 0);
	int ShowHideOrderGrid(std::vector<bool>& colOptions);
	void SetOrderCellHeight(int value);
	void SetOrderCellWidth(int value);
	void SetOrderArea(int height, int width);
	int GetMaxWidth();
	void ApplyProfitLossForPosition();
	int GetGridWidth(std::vector<bool>& colOptions);
	void RefreshInfo();
};

