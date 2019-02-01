#pragma once
#include "Skel/VtGrid.h"
#include "UGrid/CellTypes/UGCTSeperateText.h"
#include "UGrid/CellTypes/UGCTbutn.h"
#include "UGrid/CellTypes/UGCTRemain.h"
#include "Global/VtDefine.h"
#include <vector>
#include <set>
#include <map>
#include "UGrid/CellTypes/UGCTOrder.h"
#include "VtOrderConfigManager.h"
#include "Global/VtDefine.h"

#define BUTTON_SET_CENTER 123
#define BUTTON_SELL 124
#define BUTTON_BUY 125
#define BUTTON_BUY_ATMARKET 126
#define BUTTON_SELL_ATMARKET 127
#define BUTTON_CANCEL_SELL_ALLSTOP 128
#define BUTTON_CANCEL_BUY_ALLSTOP 129
#define BUTTON_CANCEL_BUY_ALL_ACCEPTED 130
#define BUTTON_CANCEL_SELL_ALL_ACCEPTED 131
#define BUTTON_CANCEL_ALL_ACCEPTED 132


class VtSymbolMaster;
class VtAccount;
class VtSymbol;
struct VtQuote;
struct VtHoga;
struct VtRealtimeHoga;
struct VtRealtimeQuote;
class VtProductOrderManager;
struct VtOrder;
struct VtOrderRequest;
class VtStopOrderManager;
class VtOrderCenterWndHd;
struct HdOrderRequest;
class VtCutManager;

//m_Pen.CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
enum class VtOrderGridCol {
	STOP,
	ORDER,
	COUNT,
	QUANTITY,
	CENTER
};

const int RefreshTimer = 0x00000007;
typedef std::map<VtCellPos, VtCellPos> CellPosMap;
class VtOrderConfigManager;
class VtOrderPanelGrid : public VtGrid
{
public:
	VtOrderPanelGrid();
	~VtOrderPanelGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnRClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual int  OnCanViewMove(int oldcol, long oldrow, int newcol, long newrow);
	virtual void OnKeyDown(UINT *vcKey, BOOL processed);
	virtual void OnGetCell(int col, long row, CUGCell *cell);
	//virtual void OnDrawSelectedRect(CDC *dc, RECT *rect);
	void SetColTitle(bool init);
	int _ColCount = 9;
	int _RowCount = 60;
	int _HeadHeight = 18;
	CFont _defFont;
	CFont _titleFont;
	CFont _CenterFont;
	CFont _CursorFont;
	CBitmap				m_CancelBitmap;
	CPen m_Pen;
	CPen m_ClosePen;
	

	void QuickRedrawCell(int col, long row);


	/// <summary>
	/// 중심 행
	/// 이 행에는 항상 현재값이 표시되며
	/// 중심행을 중심으로 위로는 증가값, 아래로는 감소값이 표시된다.
	/// 고정모드에서는 중심행이 고정되며,
	/// 비고정모드에서는 중심행은 마우스 휠의 움직임에 따라 끊임없이 변한다.
	/// 비고정모드에서는 셀의 표시범위를 벗어나는 정보들은 표시하지 않는다.
	/// </summary>
	int _CloseRow = 0;
	int _HighRow = 0;
	int _LowRow = 0;
	int _OpenRow = 0;
	int _PreCloseRow = 0;

	int _OldCloseRow = 0;
	int _OldHighRow = 0;
	int _OldLowRow = 0;

	int _SelectedRow = -1;
	int _SelectedCol = -1;
	int _OldSelectedRow = -1;
	int _OldSelectedCol = -1;
	int _SelectedVal = 0;
	int _SelectedValInt = 0;

	int _CloseLineRow = -2;
	int _OldCloseLineRow = -2;

	void ClearSelectedCell();
	void SetSelectedRect(int col, int row);

	int _StartRowForValue = 1;
	int _EndRowForValue = 0;
	virtual void OnGridWheel(int oldCenter, int newCenter);


	double _Close, _High, _Low;

	// Order
	std::vector<VtCellPos> AcptOrderRow;
	std::vector<VtCellPos> OldAcptOrderRow;
	std::vector<VtCellPos> StopOrderRow;
	std::vector<VtCellPos> OldStopOrderRow;
	void SetCenterValue();
	void SetCenterValue(CellPosMap& refreshMap);
	void SetUpperCtrls();
	void SetLowerCtrls();
	void SetLowerCtrls(CellPosMap& refreshMap);
	void ShowLowerCtrls();
	void ShowUpperCtrls();
	void ShowUpperCtrls(CellPosMap& refreshMap);
	void SetTotalHoga();
	VtOrderConfigManager* OrderConfigMgr() const { return _OrderConfigMgr; }
	void OrderConfigMgr(VtOrderConfigManager* val) { _OrderConfigMgr = val; }
	
	VtOrderCenterWndHd* CenterWnd() const { return _CenterWnd; }
	void CenterWnd(VtOrderCenterWndHd* val) { _CenterWnd = val; }
	void ClearTotalInfoCells();
	void ClearTotalHogaInfo();
	bool ShowStopCol() const { return _ShowStopCol; }
	void ShowStopCol(bool val) { _ShowStopCol = val; }
	bool ShowOrderCol() const { return _ShowOrderCol; }
	void ShowOrderCol(bool val) { _ShowOrderCol = val; }
	bool ShowOrderCountCol() const { return _ShowOrderCountCol; }
	void ShowOrderCountCol(bool val) { _ShowOrderCountCol = val; }
	bool ShowOrderQuantityCol() const { return _ShowOrderQuantityCol; }
	void ShowOrderQuantityCol(bool val) { _ShowOrderQuantityCol = val; }
	int CellHeight() const { return _CellHeight; }
	void CellHeight(int val) { _CellHeight = val; }
	int OldOrderWidth() const { return _OldOrderWidth; }
	void OldOrderWidth(int val) { _OldOrderWidth = val; }
	int OrderWidth() const { return _OrderWidth; }
	void OrderWidth(int val) { _OrderWidth = val; }
	VtCutManager* CutMgr() const { return _CutMgr; }
	void CutMgr(VtCutManager* val) { _CutMgr = val; }
	void InitButtonCol();
	void ResetByCenterRow();
private:
	std::vector<COLORREF> BuyColor;
	std::vector<COLORREF> SellColor;
	std::map<VtOrderGridCol, int> _GridColMap;
	void ShowCenterValue(double close, int closeRow);
	void AddOrderToCell(VtOrder* order);
	void AddOrderToCell(VtOrder* order, bool include);
	void AddOrderToCell(CellPosMap& refreshMap, VtOrder* order);
	VtCellPos FindOrderPos(VtOrder* order);
	void RemoveOrderFromCell(int col, int row);
	void ShowUnionCells();
	void ClearTotalHoga();
	void ClearTotalHoga(CellPosMap& refreshMap);
	void ClearTotalOrder();
	void ClearTotalOrder(CellPosMap& refreshMap);
	void ClearLowerCtrls();
	void ClearLowerCtrls(CellPosMap& refreshMap);
	
	std::set<VtCellPos> _CloseLinePos;
	
	std::set<VtCellPos> _HogaPos;
	
	std::set<VtCellPos> _OrderPos;
	
	std::set<VtCellPos> _StopOrderPos;
	
	std::set<VtCellPos> _QuotePos;
	
	std::set<VtCellPos> _TotalHogaPos;
	std::set<VtCellPos> _LowerCtrlPos;

	int _OldBtnCol = -2;
	int _OldBtnRow = -2;
	// Key : col, row, Value : text color, back color.
	std::map<std::pair<int, int>, std::pair<COLORREF, COLORREF>> _ButtonCol;
	void HitTestButtonCol(int col, int row);
	void RestoreButtonCell();
	void SetSelect(int col, int row);
	void ShowSelect(int col, int row);
	void RefreshStopOrder();
	void ShowTotolHogas(VtHoga* hoga);
	void ShowTotalOrder();
	void ShowTotalOrder(CellPosMap& refreshMap);
	void ShowTotalOrder(bool include);
	void ShowTotalStopOrder(bool include);
	void ShowTotalStopOrder();
	void ShowTotalStopOrder(CellPosMap& refreshMap);
	void SetQuoteInfo();
	void SetQuoteInfo(CellPosMap& refreshMap);
	void ShowQuoteInfo();
	void ShowQuoteInfo(CellPosMap& refreshMap);
	void SetOrderInfo();
	void SetOrderInfo(CellPosMap& refreshMap);
	void ShowOrderInfo();
	void ShowOrderInfo(CellPosMap& refreshMap);
	void SetStopOrderInfo();
	void SetStopOrderInfo(CellPosMap& refreshMap);
	void ShowStopOrderInfo();
	void ClearOldStopOrders();
	void ClearOldStopOrders(CellPosMap& refreshMap);
	void SetCloseLine();
	void SetCloseLine(CellPosMap& refreshMap);
	//void SetBuyPrice(int index, std::string buyPrice, int buyRow);
	//void SetSellPrice(int index, std::string sellPrice, int sellRow);
	void SetPositionInfo();
	void SetPositionInfo(CellPosMap& refreshMap);
	void SetHogaInfo(VtHoga* hoga);
	void SetHogaInfo();
	void SetHogaInfo(CellPosMap& refreshMap);
	void ShowHogaInfo();
	void ShowHogaInfo(CellPosMap& refreshMap);
	void RefreshCells(CellPosMap& refreshMap);
	void InitHogaFromQuote(VtQuote* quote);
	void ClearText(int col, int row);
	void ClearOldValues();
	void ClearOldValues(CellPosMap& refreshMap);
	void ClearOldHogas();
	void ClearOldHogas(CellPosMap& refreshMap);
	void ClearOldOrders();
	void ClearOldOrders(CellPosMap& refreshMap);
	void ClearOldQuotes();
	void ClearOldQuotes(CellPosMap& refreshMap);
	void ClearOldPosition();
	void ClearOldPosition(CellPosMap& refreshMap);
	void ClearOldCloseLine();
	void ClearOldCloseLine(CellPosMap& refreshMap);
	void ClearHogas();
	void ClearOrders();
	void ShowProductRemain();
	void DrawMovingRect(int col, long row);
	void DrawSelectedRect(int col, long row);
	void ClearOldMovingRect();
	void ChangeCenter(int oldCenter, int newCenter);
	virtual void OnMouseMove(int col, long row, POINT *point, UINT nFlags, BOOL processed = 0);
	virtual void OnKillFocus(int section, CWnd *pNewWnd);
	virtual void OnMouseLeaveFromMainGrid();
	void ShowPosition(VtPosition* posi, VtSymbol* sym);
	void ShowPosition(CellPosMap& refreshMap, VtPosition* posi, VtSymbol* sym);
	int _MMCol;
	int _MMRow;
	int _OldMMCol;
	int _OldMMRow;

	bool _MouseIn = true;
	bool _InitHoga = false;
	bool _InitOrder = false;
	bool _InitQuote = false;
	bool _InOrderArea = true;
	RECT rcSelected;
	CUGCTOrder m_OrderCellType;
	int m_OrderCellTypeIndex;
	//Push Button cell type
	CUGButtonType		m_button;
	int					m_nButtonIndex;

	CUGCTSeperateText		m_SeperateText;
	int					m_nSeperateIndex;
	CFont				m_medBoldFont;
	CUGCTRemain m_RemainCell;
	int m_RemainCellIndex;
	int _OrderPositionRow = -2;
	//cell type notifications
	virtual int OnCellTypeNotify(long ID, int col, long row, long msg, long param);
	//celltype notification handlers
	int OnDropList(long ID, int col, long row, long msg, long param);
	int OnCheckbox(long ID, int col, long row, long msg, long param);
	int OnDateTime(long ID, int col, long row, long msg, long param);
	int OnPushButton(long ID, int col, long row, long msg, long param);
	int OnEllipsisButton(long ID, int col, long row, long msg, long param);
	int OnSpinButton(long ID, int col, long row, long msg, long param);
	int OnSlider(long ID, int col, long row, long msg, long param);

	VtStopOrderManager* _StopOrderMgr = nullptr;
	int FindCenterRow();
	int GetMaxRow();
	void SetOrderArea();
	void SetOrderArea(int endRow);
	void ShowCloseLine();
public:
	void RefreshOrderPosition();
	void ResizeGrid(int height);
	void InitInfo();

	void InitQuote(VtQuote* quote);
	void InitHoga(VtHoga* hoga);
	void InitOrder();

	void InitAllInfo();

	void OnReceiveRealtimeQuote(VtQuote* quote);
	void OnReceiveRealtimeHoga(VtHoga* hoga);
	void OnReceiveRealtimeQuote(VtRealtimeQuote* realQuote);
	void OnReceiveRealtimeHoga(VtRealtimeHoga* realHoga);
	void PutOrder(CString price, VtPositionType posi);
	void PutOrder(double price, VtPositionType posi);
	void LiqudAll(VtPriceType priceType, int price = 0);
	VtOrderRequest* AddNormalStopOrder(double price, VtPositionType posi);
	VtOrderRequest* AddAutoStopOrder(double price, VtPositionType posi);
	void AddNormalStopOrderHd(int price, VtPositionType posi);
	void AddStopOrderForFilled(VtOrder* order, int mode);
	void ChangeOrder(VtOrder* order, CString newValue);
	void ChangeOrder(VtOrder* order, int newPrice);
	void CancelOrder(VtOrder* order);
	void OnOrderReceived(VtOrder* order);

	void OnAccepted(VtOrder* order);
	/// <summary>
	/// 체결 확인 메시지 처리
	/// </summary>
	/// <param name="real"></param>
	void OnFilled(VtOrder* order);
	/// <summary>
	/// 수정확인 메시지 처리
	/// </summary>
	/// <param name="real"></param>
	void OnConfirmModify(VtOrder* order);
	/// <summary>
	/// 취소확인 메시지 처리
	/// </summary>
	/// <param name="real"></param>
	void OnConfirmCancel(VtOrder* order);
	void SetOrderConfigMgr(VtOrderConfigManager* val);
	void ClearAllStopOrders();
	int GetGridWidth(std::vector<bool>& colOptions);
	int GetGridHeight();
private:
	int _OldRowCount = 0;
	void DrawOrderLine(int a_Col, long a_Row);
	void SetCellBackColor(int minRow, int maxRow, int start, int end, COLORREF color);
	/// <summary>
	/// 
	/// </summary>
	/// <param name="direction">
	/// 1 : left to right
	/// 2 : right to left
	/// 3 : up to down
	/// 4 : down to up
	/// </param>
	/// <param name="pdc"></param>
	/// <param name="p0"></param>
	/// <param name="p1"></param>
	/// <param name="head_length"></param>
	/// <param name="head_width"></param>
	void DrawArrow(int direction, POINT *point, CDC* pdc, POINT p0, POINT p1, int head_length, int head_width);
	void DrawMovingOrder(int col, int row, POINT *point);

	// for Moving orders
	POINT m_Prev;
	bool m_bDrawingOrder;
	int m_DragStartCol;
	int m_DragStartRow;
	CString m_PrevPrice;
	CString m_PrevAmount;

	void ShowOrderMenu(POINT* point);
	std::map<int, int> RowToValueMap;
	int FindRowFromCenterValue(int value);
	int FindRowFromCenterValue(VtSymbol* sym, int value);
private:
	VtOrderConfigManager* _OrderConfigMgr = nullptr;

	void BuyAtMarket();
	void SellAtMarket();
	void CancelAllSellStop();
	void CancelAllBuyStop();
	void CancelAllSellAccepted();
	void CancelAllBuyAccepted();
	void CancelAllAccepted();
	void SetAutoStopOnFilled(VtOrder* order);
	void CheckStopTouched(double close);
	void CheckStopTouchedHd(int intClose);
	void CheckProfitLossTouchHd(int intClose);
	void SetStopOrder(CellPosMap& refreshMap, HdOrderRequest* order);
	void OrderByMouseClick();
	void OrderByMousePosition();

	void ShowTotalHoga();
	void ShowTotalHoga(CellPosMap& refreshMap);
	void ShowTotalHoga(bool include);
	void ShowLowerCtrls(bool include);
	void ShowLowerCtrls(CellPosMap& refreshMap);
	void ShowCenterValueForOption();
	void ShowCenterValueForOption(CellPosMap& refreshMap);
	void ShowCenterValueForOptionFixed();
public:

	void RefreshCenterValue();
	void ShowCenterValueByFixed();
	void RefreshCenterValue(CellPosMap& refreshMap);
	void ShowCenterValueByFixed(CellPosMap& refreshMap);
	void OnReceiveHoga(VtSymbol* sym);
	void OnReceiveQuote(VtSymbol* sym);
	void OnSymbolMaster(VtSymbol* sym);

	void OnOrderReceivedHd(VtOrder* order);
	void OnOrderAcceptedHd(VtOrder* order);
	void OnOrderUnfilledHd(VtOrder* order);
	void OnOrderFilledHd(VtOrder* order);

	void OnReceiveHoga();
	void OnReceiveQuote();

private:
	VtOrderCenterWndHd* _CenterWnd;

	void PutOrder(int price, VtPositionType position, VtPriceType priceType = VtPriceType::Price);
	void PutOrder(VtPosition* posi, int price, bool liqud = false);
	//void CancelOrderHd(VtOrder* order);
	bool CheckProfitLoss(VtSymbol* sym, CheckType type);
	//void CheckCutLoss(VtSymbol* sym);
	//void CheckCutProfit(VtSymbol* sym);

public:
	void ShowOrderAreaInGrid(bool flag);
	void ShowStopAreaInGrid(bool flag);
	void ShowOrderCountInGrid(bool flag);
	void SetOrderCellHeight(int value);
	void SetOrderCellWidth(int value);
	void SetShowPLConfigWnd(bool flag);
	void SetOrderArea(int height, int width);
	int GetMaxWidth();
	int GetMaxHeight();
	void ApplyProfitLossForPosition();
	int ShowHideOrderGrid(std::vector<bool>& colOptions);
	void StopTimer();
private:
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
	int _Test = 0;
	VtCellPos _OldPositionPos;
	int FindNearestCellPos(VtPosition* posi);
	int FindNearestCellRow(VtPosition* posi);
	/*
	/// <summary>
	/// mode : 0 -> 익절, 1 ->손절
	/// </summary>
	/// <param name="mode"></param>
	void MakePositionStop(int mode);
	void MakeAutoStopList(int mode, VtPosition* posi, VtSymbol* sym, VtFund* fund);
	HdOrderRequest* MakePositionStopOrderRequest(int mode, VtPosition* posi, VtSymbol* sym, VtAccount* acnt);
	void MakePairStopOrderRequest(VtPosition* posi, VtSymbol* sym, VtAccount* acnt);
	*/
	VtCutManager* _CutMgr;
	void PutOrderByRemain(int price);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

