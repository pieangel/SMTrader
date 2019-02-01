#pragma once
#include "Global/VtDefine.h"
#include <string>
#include <queue>
struct	VtTickQuoteItem
{
	std::string	Time;	/* 시간(HH:MM:SS)												*/
	double	ClosePrice;	/* 현재가														*/
	int	ContQty;	/* 체결량														*/
	int	MatchKind;	/* 현재가의 호가구분 (+.매수 -.매도)							*/
	int Decimal;
	std::string	Date;	/* 일자(YYYYMMDD)												*/
};

struct	VtTickQuote
{
	std::string	FullCode;	/* 종목표준코드													*/
	int	JinbubGb;	/* 진법 0:10진법 1:32진법	*/
	int	DataCnt;	/* Data Count*/
	std::queue<VtTickQuoteItem> TickQuoteQ;
};

