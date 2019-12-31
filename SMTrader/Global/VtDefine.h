#pragma once
#include <string>
#include <tuple>
#include <cmath>

#define WM_ORDERWND_CLOSED (WM_USER + 100)

#define ROUNDING(x, dig)	( floor((x) * pow(float(10), dig) + 0.5f) / pow(float(10), dig) )
/*
double Rounding(double x, int digit)
{
	return (floor((x)* pow(float(10), digit) + 0.5f) / pow(float(10), digit));
}
*/
const int ServerSleepTime = 500; // mili second.
const int ChartDataSize = 1500;
const int TempDataSize = ChartDataSize;
const std::string id = _T("angelpie");
const std::string pwd = _T("orion1");
const std::string cert = _T("11orion@@@");

const int BitTrexTimerID = WM_USER + 1000;
const int BithumbTimerID = WM_USER + 1001;

enum class VtTargetServer { KR, HANA, HD };
enum class VtChartType
{
	NONE = 0,
	DAY,
	WEEK,
	MONTH,
	HOUR,
	MIN,
	TICK
};

enum class VtPositionType
{
	None = 0,
	Buy,
	Sell,
	ExitBuy,
	ExitSell
};

enum class VtOrderType
{
	None = 0,
	New,
	Change,
	Cancel
};

enum class VtPriceType
{
	None = 0,
	Price, // 지정가
	Market, // 시장가
	Condition, // 조건
	BestPrice // 최상위 유리가
};

enum class VtTradeType
{
	None = 0,
	Price = 1, // 지정가
	Market = 3, // 시장가
	Condition = 4, // 조건
	BestPrice = 5 // 최상위 유리가
};
//1:FAS, 2:FOK, 3:FAK
enum class VtFilledCondition
{
	None = 0,
	Fas = 1, //
	Fok = 2, //
	Fak = 3
};


enum class VtOrderState
{
	None,
	/// <summary>
	/// 예약 주문이 만들어 졌음.
	/// </summary>
	Preordered,
	/// <summary>
	/// 서버에서 주문이 왔음
	/// </summary>
	OrderReceived,
	/// <summary>
	/// 원장접수
	/// </summary>
	Ledger,
	/// <summary>
	/// 주문접수
	/// </summary>
	Accepted,
	/// <summary>
	/// 체결
	/// </summary>
	Filled,
	/// <summary>
	/// 정정 확인
	/// </summary>
	ConfirmModify,
	/// <summary>
	/// 취소 확인
	/// </summary>
	ConfirmCancel,
	/// <summary>
	/// 신규주문거부
	/// </summary>
	RejectNew,
	/// <summary>
	/// 정정거부
	/// </summary>
	RejectModify,
	/// <summary>
	/// 취소 거부
	/// </summary>
	RejectCancel,
	/// <summary>
	/// Response from server before the order is accepted.
	/// </summary>
	NewOrderFromServer,
	/// <summary>
	/// Response from server before the order is accepted.
	/// </summary>
	ModifyOrderFromServer,
	/// <summary>
	/// Response from server before the order is accepted.
	/// </summary>
	CancelOrderFromServer,
	/// <summary>
	/// Reserved state
	/// </summary>
	Reserved,
	/// <summary>
	/// Canceled the reserved
	/// </summary>
	CancelReserved,
	NewOrderFromExternal,
	// 청산됨 - 다른 주문에 의해서 혹은 잔고를 청산하는 용도로
	Settled
};

struct VtDate 
{
	int year = 0;
	int month = 0;
	int day = 0;
};

struct VtTime
{
	int hour = 0;
	int min = 0;
	int sec = 0;
	int mil = 0;
};

struct VtKrHeader
{
	char	WinId[10];	/* Client WINDOWS ID										 	*/
	char	PCKey[1];	/* Request key													*/
	char	ErrCode[4];	/* Error Code
						공백이나 0000이면 정상, 그외 오류, _ESErr_st 로 받음)		*/
	char	NextKind[1];	/* 다음 구분 (0.다음없음, 1.다음 있음)							*/
	char	filler[15];	/* filler														*/
};

enum class VtOrderCmd
{
	PreBuy, // 예약 매수
	PreSell, // 예약 매도
	BuyAtPrice, // 지정가 매수
	SellAtPrice, // 지정가 매도
	BuyAtMarket, // 시장가 매수
	SellAtMarket // 시장가 매도
};

enum class VtMainChartType
{
	CandleStick,
	ClosePrice,
	TypicalPrice,
	MedianPrice,
	WeightedClose,
	OHLC
};

enum class VtChartIndex
{
	MA,
	BolingerBand,
	Stocastic,
	MACD,
	RSI,
	TRIX,
	Parabolic,
	Volume,
	PriceChannel
};

enum VtExchangeName
{
	BITHUMB,
	KORBIT,
	BITREX,
	KRAKEN,
	BITFINEX,
	POLONIEX
};

class IDGenerator 
{
public:
	uint32_t next() { return _id++; }
	IDGenerator() : _id(1) {}
	uint32_t _id;
};

enum VtAttachDirection
{
	AttachLeft,
	AttachRight,
	AttachTop,
	AttachBottom
};

enum class VtSystemType
{
	None,
	MaSystem,
	StoSystem,
	MacdSystem,
	ChannelCross,
	ChannelJump,
	Osillator,
	DailyHeight,
	Alpha,
	Beta,
	KP_1A,
	KP_2S,
	KP_3S,
	KP_4B,
	KP_5IN1,
	KP_6IN2,
	KO_3S,
	KO_4B,
	Q_IN1,
	Q_IN2,
	Q_IN3,
	Q_IN4,
	Q_H2,
	Q_F2,
	Q_CH,
	TR_US1,
	TR_US2,
	TR_US3,
	US_IN1,
	US_IN2,
	US_IN3,
	US_I2,
	US_Comp,
	US_H2,
	SYS_OUT
};

enum class VtOrderEvent
{
	None,
	PutNew,
	Modified,
	Cancelled,
	Accepted,
	Unfilled,
	Filled,
	Released
};

enum class VtServerType
{
	Simul, // 모의 서버
	Test, // 테스트 서버
	Op // 실제 서버
};

enum class VtChartRequestType
{
	HISTORY, // 초기 데이터
	REF, // 참조 데이터
	TIMER // 타이머 요청 데이터
};


enum class CheckType
{
	Profit,
	Loss
};

struct VtSystemTemplate
{
	VtSystemType Type;
	std::string Name;
};

struct VtCellPos
{
	int Col, Row;

	// compare for order.     
	bool operator <(const VtCellPos& pt) const
	{
		return (Col < pt.Col) || ((!(pt.Col < Col)) && (Row < pt.Row));
	}
};

const int DefaultCellHeight = 18;
const int CenterCol = 4;
class VtChartData;
class Layer;
class Axis;
class XYChart;

struct VtLayerInfo
{
	bool embed = false;
	bool shareAxis = false;
	bool selected = false;
	Layer* layer;
	Axis* axis;
	XYChart* chart;
	VtChartData* data;
	COLORREF color;
	int colorIndex;
};

const int StressLineWidth = 5;

