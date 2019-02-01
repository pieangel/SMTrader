#pragma once
#include "Global/VtDefine.h"
// 종목 호가 자동 업데이트
#include <string>
struct	VtRealtimeHoga
{
//VtKrHeader Head;
	std::string	FullCode;// 			[32];	/* 종목표준코드													*/
	int	JinbubGb;//[1];	/* 진법 (0.10진법 1.32진법 2.64진법 3.128진법 4.8진법)			*/
	std::string	Time;//[8];	/* 시간(HH:MM:SS)												*/
	double	ClosePrice;//[20];	/* Close Price 1												*/

struct	VtHogaItem
{
	int	BuyNo;//[10];	/* 매수번호														*/
	double	BuyPrice;//[20];	/* 매수호가														*/
	int	BuyQty; //[20];	/* 매수수량														*/
	int	SellNo;//[10];	/* 매도번호														*/
	double	SellPrice;//[20];	/* 매도호가														*/
	int	SellQty;//[20];	/* 매도수량														*/

	long IntSellPrice;
	long IntBuyPrice;

}	Ary[5];

int	TotSellQty;//[20];	/* 매도총호가수량9(6)											*/
int	TotBuyQty;//[20];	/* 매수총호가수량9(6)											*/
int	TotSellNo;//[20];	/* 매도총호가건수9(5)											*/
int	TotBuyNo;//[20];	/* 매수총호가건수9(5)											*/
}; 


struct VtHoga
{
	//VtKrHeader					Head;
	std::string	FullCode;//[32];	/* 종목표준코드													*/
	int	JinbubGb; //[1];	/* 진법 (0.10진법 1.32진법 2.64진법 3.128진법 4.8진법)			*/
	std::string	ISINCd; //[12];	/* 종목표준코드													*/
	std::string	Time; //[8];	/* 입력시간 HH:MM:SS											*/
	double	ClosePrice;//[20];	/* Close Price 1												*/

	struct	VtHogaItem
	{
		int	BuyNo;//[10];	/* 매수번호														*/
		double	BuyPrice;//[20];	/* 매수호가														*/
		int	BuyQty; //[20];	/* 매수수량														*/
		int	SellNo;//[10];	/* 매도번호														*/
		double	SellPrice;//[20];	/* 매도호가														*/
		int	SellQty;//[20];	/* 매도수량														*/
		long IntSellPrice;
		long IntBuyPrice;
	}	Ary[5];

	int	TotSellQty;//[20];	/* 매도총호가수량9(6)											*/
	int	TotBuyQty;//[20];	/* 매수총호가수량9(6)											*/
	int	TotSellNo;//[20];	/* 매도총호가건수9(5)											*/
	int	TotBuyNo;//[20];	/* 매수총호가건수9(5)											*/

};