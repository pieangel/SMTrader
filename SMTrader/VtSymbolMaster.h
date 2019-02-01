#pragma once
#include <string>
class VtSymbolMaster
{
public:
	VtSymbolMaster();
	~VtSymbolMaster();

	std::string	FullCode;	/* 종목표준코드													*/
	std::string	JinbubGb;	/* 진법 (0.10진법 1.32진법 2.64진법 3:128진법 4.8진법)			*/
	std::string	StandeCode;	/* 종목표준코드													*/
	std::string	HLimitPrice;	/* 상한가														*/
	std::string	LLimitPrice;	/* 하한가														*/
	std::string	RemainDays;	/* 잔존일수9(3)													*/
	std::string	LastDate;	/* 최종거래일YYYYMMDD											*/
	std::string	ExpiryDate;	/* 만기일    YYYYMMDD											*/
	std::string	ListedHighPrice;	/* 상장중최고가													*/
	std::string	ListedHighPriceDate;	/* 상장중최고일YYYYMMDD											*/
	std::string	ListedLowPrice;	/* 상장중최저가													*/
	std::string	ListedLowPriceDate;	/* 상장중최저일YYYYMMDD											*/
	std::string	ClosePrice_1;	/* 본정산가														*/
	std::string	ClosePrice_2;	/* 전산종가														*/
	std::string	Scn_Exch_cd;	/* 거래소코드													*/
	std::string	Scn_Disp_digit;	/* 표시진법														*/
	std::string	Scn_Fut_ord_mgn;	/* 선물신규주문증거금											*/
	std::string	Scn_Ctrt_size;	/* 계약단위														*/
	std::string	Scn_Tick_size;	/* 틱															*/
	std::string	Scn_Tick_val;	/* 틱가치														*/
	std::string	PrevTotQty;	/* 전일 거래량*/
	double close1 = 0;
	double close2 = 0;
	int decimal = 0;
	double tickVal = 0;
	double tickSize = 0;
	std::string ShortCode;

	std::string	Date;	/* 일자(YYYYMMDD) 												*/
	std::string	Time;	/* 시간(HH:MM:SS)*/
	int	CmpSign;	/* 대비부호
					0.보합 1.상한 2.하한 3.상승 4.하락 5.기세상한
					6.기세하한 7.기세상승 8.기세하락							*/
	double	CmpPrice = 0;	/* 전일대비														*/
	double	CmpRate = 0;	/* 등락율9(5)V9(2)												*/
	double	OpenPrice = 0;	/* 시가 														*/
	double	HighPrice = 0;	/* 고가 														*/
	double	LowPrice = 0;	/* 저가 														*/
	int	ContQty = 0;	/* 체결량 														*/
	int	TotQty = 0;	/* 거래량*/
	int TickSizeInt = 0;
};

struct VtRealtimeSymbolMaster
{
	std::string	FullCode;	/* 종목표준코드													*/
	int	JinbubGb;	/* 진법 (0.10진법 1.32진법 2.64진법 3.128진법 4.8진법)			*/
	std::string	Date;	/* 일자(YYYYMMDD) 												*/
	std::string	Time;	/* 시간(HH:MM:SS) 												*/
	double	ClosePrice;	/* 현재가														*/
	int	CmpSign;	/* 대비부호
						0.보합 1.상한 2.하한 3.상승 4.하락 5.기세상한
						6.기세하한 7.기세상승 8.기세하락							*/
	double	CmpPrice;	/* 전일대비														*/
	double	CmpRate;	/* 등락율9(5)V9(2)												*/
	double	OpenPrice;	/* 시가 														*/
	double	HighPrice;	/* 고가 														*/
	double	LowPrice;	/* 저가 														*/
	int	ContQty;	/* 체결량 														*/
	int	TotQty;	/* 거래량 														*/
	double	ClosePrice_1;	/* Close Price 1												*/
	double	ListedHighPrice;	/* 상장중최고가													*/
	std::string	ListedHighPriceDate;	/* 상장중최고일YYYYMMDD											*/
	double	ListedLowPrice;	/* 상장중최저가													*/
	std::string	ListedLowPriceDate;
};

