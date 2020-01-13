#pragma once
#include "Grid/GridCtrl.h"
#include <vector>
#include <map>
#include <set>
#include <functional>
#include "Global/VtDefine.h"
#include <memory>

class VtSymbol;
class VtAccount;
class VtOrderConfigManager;
class VtStopOrderManager;
struct VtPosition;
class SmOrderPanel;
class SmOrderPanelOut;
class VtCutManager;
class SmOrderGridOut : public CGridCtrl
{
public:
	SmOrderGridOut();
	~SmOrderGridOut();
	SmOrderPanelOut* CenterWnd() const { return _CenterWnd; }
	void CenterWnd(SmOrderPanelOut* val) { _CenterWnd = val; }
	VtCutManager* CutMgr() const { return _CutMgr; }
	void CutMgr(VtCutManager* val) { _CutMgr = val; }
	bool ShowOrderQuantityCol() const { return _ShowOrderQuantityCol; }
	void ShowOrderQuantityCol(bool val) { _ShowOrderQuantityCol = val; }
	int OrderWidth() const { return _OrderWidth; }
	void OrderWidth(int val) { _OrderWidth = val; }
	int CellHeight() const { return _CellHeight; }
	void CellHeight(int val) { _CellHeight = val; }
	int OrderAmount() const { return _OrderAmount; }
	void OrderAmount(int val) { _OrderAmount = val; }
public:
	// 심볼마스터 이벤트 처리
	void OnSymbolMaster(VtSymbol* symbol);
	// 자동스탑 주문 처리
	void SetAutoStopOnFilled(VtOrder* order);
	// 콜백 함수 취소
	void UnregisterAllCallback();

	void RegisterQuoteCallback();
	void UnregisterQuoteCallback();
	void OnQuoteEvent(VtSymbol* symbol);

	void RegisterHogaCallback();
	void UnregisterHogaCallback();
	void OnHogaEvent(VtSymbol* symbol);

	void RegisterOrderallback();
	void UnregisterOrderCallback();
	void OnOrderEvent(VtOrder* order);

	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	void SetCenterValue();
	void RefreshAllValues();
	void Init();
	// 스페이스바 주문
	void OrderBySpaceBar();
	// 그리드의 걧수를 다시 설정한다.
	void ResizeGrid();
	// 셀의 높이와 주문영역의 너비를 다시 설정하고 그리드의 갯수를 다시 설정한다.
	void ResizeGrid(int cellHeight, int orderAreaWidth);
	void LiqudAll(VtPriceType priceType, int price = 0);
	void PutOrder(VtPosition* posi, int price, bool liqud = false);
	void RefreshOrderPosition();
	void SetOrderArea(int height, int width);
	void AddOrderToCell(VtOrder* order);
	void RemoveOrderFromCell(VtOrder* order);
	void RefreshTotalOrderCount(VtOrder* order);
private:
	void CheckProfitLossTouchHd(int intClose);
	bool CheckStopTouchedHd(int intClose);
	int _OrderWidth = 60;
	bool _ShowOrderQuantityCol = true;
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
	int _HeadHeight = 20;
	int _FixedCol = 0;
	int _FixedRow = 1;

	CFont _defFont;
	CFont _titleFont;
	CFont _CenterFont;
	CFont _CursorFont;
	bool _Init = false;

	int _StartRowForValue = 2;
	int _EndRowForValue = 0;

	// 이 행을 기준으로 시세를 그려나간다.
	int _IndexRow = 0;
	int _OldIndexRow = -1;
	int _CellHeight = 0;

	SmOrderPanelOut* _CenterWnd = nullptr;
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
	int FindIndexRow();
	// 현재 최대 행갯수 구하기
	int GetMaxValueRowCount();
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
	// 갱신목록에 있는 셀들을 모두 갱신한다.
	void RefreshCells(std::set<std::pair<int, int>>& refreshSet);
	// 주문을 낸다.
	void PutOrder(int price, VtPositionType position, VtPriceType priceType = VtPriceType::Price);
	// 주문영역의 셀을 색칠한다.
	void SetOrderAreaColor();

	// 포지션 표시
	void SetPositionInfo(std::set<std::pair<int, int>>& refreshSet);
	// 포지션 없애기
	void ClearPositionInfo(std::set<std::pair<int, int>>& refreshSet);
	// 포지션을 보여준다. 매도, 매수 포지션이 있을 경우
	void ShowPosition(std::set<std::pair<int, int>>& refreshSet, VtPosition* posi, VtSymbol* sym);
	// 마우스 클릭으로 주문을 낸다.
	void OrderByMouseClick();
	// 마우스 위치로 주문을 낸다.
	void OrderByMousePosition();
public:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

private:
	//VtSymbol* _Symbol = nullptr;
	//std::string _SymbolCode;
	// VtAccount* _Account = nullptr;
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
private:
	int _OrderAmount;
	bool _MouseIn = true;
	CRect _DragStartRect;
	CRect _DragEndRect;
	int _DragStartCol = 0;
	int _DragStartRow = 0;
	// 주문을 그린다.
	void DrawArrow(int direction, POINT *point, CDC* pdc, POINT p0, POINT p1, int head_length, int head_width);
	BOOL m_bMouseTracking;
	void RedrawOrderTrackCells();
	// 스탑주문을 추가한다.
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
	void RegisterButtons();
	void UnregisterButtons();

	void BuyAtMarket();
	void SellAtMarket();
	void CancelAllSellStop();
	void CancelAllBuyStop();
	void CancelAllSellAccepted();
	void CancelAllBuyAccepted();
	void CancelAllAccepted();
	void RefreshStopOrder();
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);

private:
	VtStopOrderManager* _StopOrderMgr = nullptr;
	int FindPositionRow(VtPosition* posi);
public:
	afx_msg void OnClose();
	afx_msg void OnDestroy();
public:
	int ShowHideOrderGrid(std::vector<bool>& colOptions);
	int GetGridWidth(std::vector<bool>& colOptions);
	int GetGridHeight();
	void ApplyProfitLossForPosition();
	void ResetByCenterRow();
private:
	VtCutManager* _CutMgr;
};


