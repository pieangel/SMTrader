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
struct VtPosition;
class SmOrderGrid : public CGridCtrl
{
public:
	SmOrderGrid();
	~SmOrderGrid();
	VtSymbol* Symbol() const { return _Symbol; }
	void Symbol(VtSymbol* val);
	VtAccount* Account() const { return _Account; }
	void Account(VtAccount* val) { _Account = val; }
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
	// 스페이스바 주문
	void OrderBySpaceBar();
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
	int _CellHeight = 18;

	VtOrderCenterWndHd* _CenterWnd;
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

	// 포지션 표시
	void SetPositionInfo(std::set<std::pair<int, int>>& refreshSet);
	// 포지션 없애기
	void ClearPositionInfo(std::set<std::pair<int, int>>& refreshSet);
	void ShowPosition(std::set<std::pair<int, int>>& refreshSet, VtPosition* posi, VtSymbol* sym);
	
	void OrderByMouseClick();
	void OrderByMousePosition();
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
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
private:
	bool _MouseIn = true;
	CRect _DragStartRect;
	CRect _DragEndRect;
	int _DragStartCol = 0;
	int _DragStartRow = 0;
	
	void DrawArrow(int direction, POINT *point, CDC* pdc, POINT p0, POINT p1, int head_length, int head_width);
	//int _OldMMRow = 0;
	//int _OldMMCol = 0;
	BOOL m_bMouseTracking;
	void RedrawOrderTrackCells();
	void AddStopOrder(int price, VtPositionType posi);
	CCellID _OldClickedCell;
	CCellID _OldMMCell;
	CCellID _OldMovingCellCenter;
	CCellID _OldMovingCellSide;
	CCellID _OldPositionCell;
	void InvalidateClickedCell();
	void SetMovingCell(CCellID cell);
	void HandleButtonEvent(int button_id);
	void ChangeOrder(VtOrder* order, int newPrice);
	void CancelOrder(VtOrder* order);
	void ChangeOrder();
	void ChangeStopOrder();
	void CancelOrder();
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);

private:
	VtStopOrderManager* _StopOrderMgr = nullptr;
	int FindPositionRow(VtPosition* posi);
public:
	afx_msg void OnClose();
	afx_msg void OnDestroy();
};

