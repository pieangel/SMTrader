#pragma once
#include <string>
class VtSymbolMaster
{
public:
	VtSymbolMaster();
	~VtSymbolMaster();

	std::string	FullCode;	/* ����ǥ���ڵ�													*/
	std::string	JinbubGb;	/* ���� (0.10���� 1.32���� 2.64���� 3:128���� 4.8����)			*/
	std::string	StandeCode;	/* ����ǥ���ڵ�													*/
	std::string	HLimitPrice;	/* ���Ѱ�														*/
	std::string	LLimitPrice;	/* ���Ѱ�														*/
	std::string	RemainDays;	/* �����ϼ�9(3)													*/
	std::string	LastDate;	/* �����ŷ���YYYYMMDD											*/
	std::string	ExpiryDate;	/* ������    YYYYMMDD											*/
	std::string	ListedHighPrice;	/* �������ְ�													*/
	std::string	ListedHighPriceDate;	/* �������ְ���YYYYMMDD											*/
	std::string	ListedLowPrice;	/* ������������													*/
	std::string	ListedLowPriceDate;	/* ������������YYYYMMDD											*/
	std::string	ClosePrice_1;	/* �����갡														*/
	std::string	ClosePrice_2;	/* ��������														*/
	std::string	Scn_Exch_cd;	/* �ŷ����ڵ�													*/
	std::string	Scn_Disp_digit;	/* ǥ������														*/
	std::string	Scn_Fut_ord_mgn;	/* �����ű��ֹ����ű�											*/
	std::string	Scn_Ctrt_size;	/* ������														*/
	std::string	Scn_Tick_size;	/* ƽ															*/
	std::string	Scn_Tick_val;	/* ƽ��ġ														*/
	std::string	PrevTotQty;	/* ���� �ŷ���*/
	double close1 = 0;
	double close2 = 0;
	int decimal = 0;
	double tickVal = 0;
	double tickSize = 0;
	std::string ShortCode;

	std::string	Date;	/* ����(YYYYMMDD) 												*/
	std::string	Time;	/* �ð�(HH:MM:SS)*/
	int	CmpSign;	/* ����ȣ
					0.���� 1.���� 2.���� 3.��� 4.�϶� 5.�⼼����
					6.�⼼���� 7.�⼼��� 8.�⼼�϶�							*/
	double	CmpPrice = 0;	/* ���ϴ��														*/
	double	CmpRate = 0;	/* �����9(5)V9(2)												*/
	double	OpenPrice = 0;	/* �ð� 														*/
	double	HighPrice = 0;	/* �� 														*/
	double	LowPrice = 0;	/* ���� 														*/
	int	ContQty = 0;	/* ü�ᷮ 														*/
	int	TotQty = 0;	/* �ŷ���*/
	int TickSizeInt = 0;
};

struct VtRealtimeSymbolMaster
{
	std::string	FullCode;	/* ����ǥ���ڵ�													*/
	int	JinbubGb;	/* ���� (0.10���� 1.32���� 2.64���� 3.128���� 4.8����)			*/
	std::string	Date;	/* ����(YYYYMMDD) 												*/
	std::string	Time;	/* �ð�(HH:MM:SS) 												*/
	double	ClosePrice;	/* ���簡														*/
	int	CmpSign;	/* ����ȣ
						0.���� 1.���� 2.���� 3.��� 4.�϶� 5.�⼼����
						6.�⼼���� 7.�⼼��� 8.�⼼�϶�							*/
	double	CmpPrice;	/* ���ϴ��														*/
	double	CmpRate;	/* �����9(5)V9(2)												*/
	double	OpenPrice;	/* �ð� 														*/
	double	HighPrice;	/* �� 														*/
	double	LowPrice;	/* ���� 														*/
	int	ContQty;	/* ü�ᷮ 														*/
	int	TotQty;	/* �ŷ��� 														*/
	double	ClosePrice_1;	/* Close Price 1												*/
	double	ListedHighPrice;	/* �������ְ�													*/
	std::string	ListedHighPriceDate;	/* �������ְ���YYYYMMDD											*/
	double	ListedLowPrice;	/* ������������													*/
	std::string	ListedLowPriceDate;
};

