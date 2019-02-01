#pragma once
#include "Global/VtDefine.h"
// ���� ȣ�� �ڵ� ������Ʈ
#include <string>
struct	VtRealtimeHoga
{
//VtKrHeader Head;
	std::string	FullCode;// 			[32];	/* ����ǥ���ڵ�													*/
	int	JinbubGb;//[1];	/* ���� (0.10���� 1.32���� 2.64���� 3.128���� 4.8����)			*/
	std::string	Time;//[8];	/* �ð�(HH:MM:SS)												*/
	double	ClosePrice;//[20];	/* Close Price 1												*/

struct	VtHogaItem
{
	int	BuyNo;//[10];	/* �ż���ȣ														*/
	double	BuyPrice;//[20];	/* �ż�ȣ��														*/
	int	BuyQty; //[20];	/* �ż�����														*/
	int	SellNo;//[10];	/* �ŵ���ȣ														*/
	double	SellPrice;//[20];	/* �ŵ�ȣ��														*/
	int	SellQty;//[20];	/* �ŵ�����														*/

	long IntSellPrice;
	long IntBuyPrice;

}	Ary[5];

int	TotSellQty;//[20];	/* �ŵ���ȣ������9(6)											*/
int	TotBuyQty;//[20];	/* �ż���ȣ������9(6)											*/
int	TotSellNo;//[20];	/* �ŵ���ȣ���Ǽ�9(5)											*/
int	TotBuyNo;//[20];	/* �ż���ȣ���Ǽ�9(5)											*/
}; 


struct VtHoga
{
	//VtKrHeader					Head;
	std::string	FullCode;//[32];	/* ����ǥ���ڵ�													*/
	int	JinbubGb; //[1];	/* ���� (0.10���� 1.32���� 2.64���� 3.128���� 4.8����)			*/
	std::string	ISINCd; //[12];	/* ����ǥ���ڵ�													*/
	std::string	Time; //[8];	/* �Է½ð� HH:MM:SS											*/
	double	ClosePrice;//[20];	/* Close Price 1												*/

	struct	VtHogaItem
	{
		int	BuyNo;//[10];	/* �ż���ȣ														*/
		double	BuyPrice;//[20];	/* �ż�ȣ��														*/
		int	BuyQty; //[20];	/* �ż�����														*/
		int	SellNo;//[10];	/* �ŵ���ȣ														*/
		double	SellPrice;//[20];	/* �ŵ�ȣ��														*/
		int	SellQty;//[20];	/* �ŵ�����														*/
		long IntSellPrice;
		long IntBuyPrice;
	}	Ary[5];

	int	TotSellQty;//[20];	/* �ŵ���ȣ������9(6)											*/
	int	TotBuyQty;//[20];	/* �ż���ȣ������9(6)											*/
	int	TotSellNo;//[20];	/* �ŵ���ȣ���Ǽ�9(5)											*/
	int	TotBuyNo;//[20];	/* �ż���ȣ���Ǽ�9(5)											*/

};