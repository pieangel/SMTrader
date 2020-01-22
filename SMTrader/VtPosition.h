#pragma once
#include <string>
#include "Global/VtDefine.h"
struct VtPosition
{
	// �����̸�
	std::string StrategyName;
	// �ݵ� �̸� - �ݵ�� ���� ���·� �̷���� ����� ������ ����
	std::string FundName;
	std::string SubAccountNo; // ���� ���� ��ȣ
	std::string AccountNo; //	12	string	������¹�ȣ
	std::string FullCode; //	16	string	�����ڵ�
	std::string ShortCode;
	VtPositionType Position = VtPositionType::None; //	1	
	int TotVolume; //	10	int	��ü�ܰ�
	int TodayVol; //	10	int	�����ܰ�
	int PrevOpenQty; //	10	int	�����ܰ�
	double Fee; //	15	double	������
	double OpenAmt; //	15	double	�򰡱ݾ�
	double TradePL; //	15	double	�Ÿż���

	std::string	BaseDate;	/* ��������														*/
	int	TradeNo = 0;	/* ü���ȣ														*/
	int	OpenQty = 0;	/* �̰�������													*/
	double	AvgPrice = 0.0f;	/* ��հ�														*/
	double	CurPrice = 0.0f;	/* ���簡 														*/
	double	OpenProfitLoss = 0.0f;	/* �򰡼���														*/
	int 	RsrbQty = 0;	/* û�갡�ɼ���													*/
	double	TradeAmount = 0.0f;	/* ü��ݾ�														*/
	int intAvgPrice = 0;
	int intCurPrice = 0;
	// �̹� �����̳� ������ �����Ǿ� ���� ���
	// �����̳� ������ ������ û��Ǳ� ������
	// Ȥ�� �������� ����, ������ ����� ��������  true,
	// �� �̿ܿ��� false�� �ȴ�. 
	// false �� ���¿����� ���� ������ üũ������ true���� üũ���� �ʴ´�.
	bool ProfitCutSet = false;
	bool LossCutSet = false;
	// �ֹ� Ÿ�� -> 0 : �Ϲ� �ֹ�, 1 : ������� �ֹ� , 2 : �ݵ� �ֹ�
	int orderType = 0;
};

