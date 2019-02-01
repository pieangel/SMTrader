#pragma once
#include "Global/VtDefine.h"
#include <string>
#include <list>
struct	VtRealtimeQuote
{
	VtKrHeader Head;
	char	FullCode[32];	/* ����ǥ���ڵ� 												*/
	char	JinbubGb[1];	/* ���� (0.10���� 1.32���� 2.64���� 3.128���� 4.8����)			*/
	char	Time[8];	/* �ð�(HH:MM:SS)												*/
	char	CmpSign[1];	/* ����ȣ
						0.���� 1.���� 2.���� 3.��� 4.�϶� 5.�⼼����
						6.�⼼���� 7.�⼼��� 8.�⼼�϶�							*/
	char	CmpPrice[20];	/* ���ϴ��														*/
	char	ClosePrice[20];	/* ���簡														*/
	char	CmpRate[8];	/* �����9(5)V9(2)												*/
	char	TotQty[20];	/* �ŷ��� 														*/
	char	ContQty[20];	/* ü�ᷮ 														*/
	char	MatchKind[1];	/* ���簡�� ȣ������ (+.�ż� -.�ŵ�)							*/
	char	Date[8];	/* ����(YYYYMMDD) 												*/
	char	OpenPrice[20];	/* �ð� 														*/
	char	HighPrice[20];	/* �� 														*/
	char	LowPrice[20];	/* ���� 														*/
	char	BuyPrice[20];	/* �ż�ȣ��														*/
	char	SellPrice[20];	/* �ŵ�ȣ��														*/
	char	MarketFlag[1];	/* �屸�� 0.���� 1.������										*/
	char	DecLen[5];	/* ���� �Ҽ��� ����												*/
};

struct	VtQuoteItem
{
	std::string	Time;	/* �ð�(HH:MM:SS)												*/
	double	ClosePrice;	/* ���簡														*/
	int	ContQty;	/* ü�ᷮ														*/
	int	MatchKind;	/* ���簡�� ȣ������ (+.�ż� -.�ŵ�)							*/
	int Decimal;
	std::string	Date;	/* ����(YYYYMMDD)												*/
};


struct	VtQuote
{
	int	JinbubGb;	/* ���� 0:10���� 1:32����	*/
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
