#pragma once
#include <list>
#include "Global/VtDefine.h"
class VtSymbol;
struct VtHoga;
class VtAccount;
class VtSymbolMaster;
struct VtQuote;
struct VtOrder;
struct VtPosition;
class VtOrderManager;
class VtProductOrderManager;
class VtOrderWnd;
class CVtOrderLeftWnd;
class CVtOrderRightWnd;
class SmOrderPanelOut;
class VtOrderLeftWndHd;
class VtOrderRightWndHd;
class VtOrderCenterWndHd;
class VtOrderWndHd;
class VtFund;
class SmOrderPanel;
enum class VtSpaceByOrderType
{
	NONE = 0,
	ByMouseClick,
	ByMousePositon
};

enum class VtRButtonCancelOrder
{
	NONE = 0,
	CancelLastOrder,
	CancelOnMousePosition
};


class CVtOrderWnd;
class VtOrderConfigManager
{
public:
	VtOrderConfigManager();
	~VtOrderConfigManager();
	int DefaultCellHeight = 18;
	/// <summary>
	/// 주문영역 기본 셀 높이
	/// </summary>
	int OrderCellHeight = 18;
	/// <summary>
	/// 주문영역 기본 셀 너비
	/// </summary>
	int OrderCellWidth = 58;
	int ProfitSlip = -2;
	int LossSlip = 2;

	int OrderAmount = 1;
	int StopVal = 2;

	int MinOrderAmount = 1;
	int MaxOrderAmount = 100;

	int MinStopVal = -100;
	int MaxStopVal = 100;

	int MinAutoStopVal = 0;
	int MaxAutoStopVal = 100;


	bool UseAutoStopProfit = false;
	bool UseAutoStopLosscut = false;
	int AutoStopProfitValue = 2;
	int AutoStopLosscutValue = 2;
	VtPriceType AutoStopPriceType = VtPriceType::Market;
	int AutoStopPriceSlip = 2;

	bool UseTrailStop = false;
	int TrailStopDefaultLosscut = 2;
	int TrailStopProfit = 2;
	int TrailStopTrail = 2;
	VtPriceType TrailStopPriceType = VtPriceType::Market;
	int TrailStopPriceSlip = 2;

	bool UseOneClickOrder = false;
	bool UseFixHoga = false;
	bool UseSpaceBarOrder = true;
	VtSpaceByOrderType SpaceBarorderType = VtSpaceByOrderType::ByMouseClick;

	bool UseShortCutOrder = false;
	bool UseShortCutOrderConfirm = false;
	VtRButtonCancelOrder RButtonCancel = VtRButtonCancelOrder::CancelOnMousePosition;

	bool UseThousandOnProfit = true;
	
	VtAccount* Account() const { return _Account; }
	void Account(VtAccount* val);
	// 이 함수는 중앙창에서 주문을 낼 때는 절대로 사용하면 안됨.
	// 대신 반드시 중앙창에 있는 심볼을 이용할것.
	VtSymbol* Symbol() const { return _Symbol; }
	void Symbol(VtSymbol* val);
	VtHoga* Hoga() const { return _Hoga; }
	void Hoga(VtHoga* val) { _Hoga = val; }
	VtSymbolMaster* Master() const { return _Master; }
	void Master(VtSymbolMaster* val) { _Master = val; }
	VtQuote* Quote() const { return _Quote; }
	void Quote(VtQuote* val) { _Quote = val; }
	VtPosition* Position() const { return _Position; }
	void Position(VtPosition* val) { _Position = val; }
	VtOrderManager* OrderMgr() const { return _OrderMgr; }
	void OrderMgr(VtOrderManager* val) { _OrderMgr = val; }
	VtProductOrderManager* PrdtOrderMgr() const { return _PrdtOrderMgr; }
	void PrdtOrderMgr(VtProductOrderManager* val) { _PrdtOrderMgr = val; }
	void RegisterRealtimeSymbol();
	void UnregisterRealtimeSymbol();
	VtFund* Fund() const { return _Fund; }
	void Fund(VtFund* val);
	int Type() const { return _Type; }
	void Type(int val) { _Type = val; }

	VtAccount* OldAccount() const { return _OldAccount; }
	VtFund* OldFund() const { return _OldFund; }
	void OldFund(VtFund* val) { _OldFund = val; }
private:
	int _Type; // 0 : Normal Order, 1 : Fund Order.
	VtFund* _Fund = nullptr;
	VtFund* _OldFund = nullptr;
	VtAccount* _Account = nullptr;
	VtAccount* _OldAccount = nullptr;
	VtSymbol* _Symbol = nullptr;
	VtHoga* _Hoga = nullptr;
	VtSymbolMaster* _Master = nullptr;
	VtQuote* _Quote = nullptr;
	VtPosition* _Position = nullptr;
	VtOrderManager* _OrderMgr = nullptr;
	VtProductOrderManager* _PrdtOrderMgr = nullptr;
	void Reset();

public:
	VtOrderWnd* orderWnd = nullptr;
	CVtOrderLeftWnd* leftWnd = nullptr;
	CVtOrderRightWnd* rightWnd = nullptr;
	SmOrderPanelOut* centerWnd = nullptr;
	bool ShowLeft = true;
	bool ShowRight = true;
	int leftWidth = 0;
	int centerWidth = 0;
	int rightWidth = 0;

	VtOrderLeftWndHd* _HdLeftWnd = nullptr;
	VtOrderRightWndHd* _HdRightWnd = nullptr;
	SmOrderPanel* _HdCenterWnd = nullptr;
	VtOrderWndHd*       _HdOrderWnd = nullptr;

	void SetDefaultSymbol();
	void SetDefaultAccount();
};

