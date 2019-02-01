#pragma once
#include "Global/VtDefine.h"
#include <string>
#include <list>
struct	VtRealtimeQuote
{
	VtKrHeader Head;
	char	FullCode[32];	/* 종목표준코드 												*/
	char	JinbubGb[1];	/* 진법 (0.10진법 1.32진법 2.64진법 3.128진법 4.8진법)			*/
	char	Time[8];	/* 시간(HH:MM:SS)												*/
	char	CmpSign[1];	/* 대비부호
						0.보합 1.상한 2.하한 3.상승 4.하락 5.기세상한
						6.기세하한 7.기세상승 8.기세하락							*/
	char	CmpPrice[20];	/* 전일대비														*/
	char	ClosePrice[20];	/* 현재가														*/
	char	CmpRate[8];	/* 등락율9(5)V9(2)												*/
	char	TotQty[20];	/* 거래량 														*/
	char	ContQty[20];	/* 체결량 														*/
	char	MatchKind[1];	/* 현재가의 호가구분 (+.매수 -.매도)							*/
	char	Date[8];	/* 일자(YYYYMMDD) 												*/
	char	OpenPrice[20];	/* 시가 														*/
	char	HighPrice[20];	/* 고가 														*/
	char	LowPrice[20];	/* 저가 														*/
	char	BuyPrice[20];	/* 매수호가														*/
	char	SellPrice[20];	/* 매도호가														*/
	char	MarketFlag[1];	/* 장구분 0.본장 1.전산장										*/
	char	DecLen[5];	/* 종목 소숫점 정보												*/
};

struct	VtQuoteItem
{
	std::string	Time;	/* 시간(HH:MM:SS)												*/
	double	ClosePrice;	/* 현재가														*/
	int	ContQty;	/* 체결량														*/
	int	MatchKind;	/* 현재가의 호가구분 (+.매수 -.매도)							*/
	int Decimal;
	std::string	Date;	/* 일자(YYYYMMDD)												*/
};


struct	VtQuote
{
	int	JinbubGb;	/* 진법 0:10진법 1:32진법	*/
	int	DataCnt;	/* Data Count*/
	double high;
	double low;
	double close;
	double open;
	std::string fullCode;

	long intHigh;
	long intLow;
	long intClose;
	long intOpen;
	long intPreClose;
	long intExpected;

	long intPreDayHigh;
	long intPreDayLow;

	std::list<VtQuoteItem> QuoteItemQ;
};
