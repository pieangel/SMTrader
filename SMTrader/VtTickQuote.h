#pragma once
#include "Global/VtDefine.h"
#include <string>
#include <queue>
struct	VtTickQuoteItem
{
	std::string	Time;	/* �ð�(HH:MM:SS)												*/
	double	ClosePrice;	/* ���簡														*/
	int	ContQty;	/* ü�ᷮ														*/
	int	MatchKind;	/* ���簡�� ȣ������ (+.�ż� -.�ŵ�)							*/
	int Decimal;
	std::string	Date;	/* ����(YYYYMMDD)												*/
};

struct	VtTickQuote
{
	std::string	FullCode;	/* ����ǥ���ڵ�													*/
	int	JinbubGb;	/* ���� 0:10���� 1:32����	*/
	int	DataCnt;	/* Data Count*/
	std::queue<VtTickQuoteItem> TickQuoteQ;
};

