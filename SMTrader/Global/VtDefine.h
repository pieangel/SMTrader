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
	Price, // ������
	Market, // ���尡
	Condition, // ����
	BestPrice // �ֻ��� ������
};

enum class VtTradeType
{
	None = 0,
	Price = 1, // ������
	Market = 3, // ���尡
	Condition = 4, // ����
	BestPrice = 5 // �ֻ��� ������
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
	/// ���� �ֹ��� ����� ����.
	/// </summary>
	Preordered,
	/// <summary>
	/// �������� �ֹ��� ����
	/// </summary>
	OrderReceived,
	/// <summary>
	/// ��������
	/// </summary>
	Ledger,
	/// <summary>
	/// �ֹ�����
	/// </summary>
	Accepted,
	/// <summary>
	/// ü��
	/// </summary>
	Filled,
	/// <summary>
	/// ���� Ȯ��
	/// </summary>
	ConfirmModify,
	/// <summary>
	/// ��� Ȯ��
	/// </summary>
	ConfirmCancel,
	/// <summary>
	/// �ű��ֹ��ź�
	/// </summary>
	RejectNew,
	/// <summary>
	/// �����ź�
	/// </summary>
	RejectModify,
	/// <summary>
	/// ��� �ź�
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
	// û��� - �ٸ� �ֹ��� ���ؼ� Ȥ�� �ܰ� û���ϴ� �뵵��
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
						�����̳� 0000�̸� ����, �׿� ����, _ESErr_st �� ����)		*/
	char	NextKind[1];	/* ���� ���� (0.��������, 1.���� ����)							*/
	char	filler[15];	/* filler														*/
};

enum class VtOrderCmd
{
	PreBuy, // ���� �ż�
	PreSell, // ���� �ŵ�
	BuyAtPrice, // ������ �ż�
	SellAtPrice, // ������ �ŵ�
	BuyAtMarket, // ���尡 �ż�
	SellAtMarket // ���尡 �ŵ�
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
	Simul, // ���� ����
	Test, // �׽�Ʈ ����
	Op // ���� ����
};

enum class VtChartRequestType
{
	HISTORY, // �ʱ� ������
	REF, // ���� ������
	TIMER // Ÿ�̸� ��û ������
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

