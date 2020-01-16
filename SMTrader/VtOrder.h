#pragma once
#include <string>
#include "Global/VtDefine.h"
#include "SimpleBinStream.h"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;
// 	virtual bool PutNewOrder(std::string msgid, std::string accno, 
//std::string pwd, VtOrderType orderType, std::string product, 
//VtPositionType buysell, VtPriceType priceCondition, std::string 
//orderPrice, std::string orderAmount, std::string originOrderNo) = 0;

struct VtOrderRequest
{
	/// <summary>
	/// �ֹ��� �� ��Ʈ ���̵�.
	/// ���� �� ���̵� ���� ��Ʈ�� ������
	/// ��Ʈ�� �ϳ� ����� �ű�� ǥ���Ѵ�.
	/// </summary>
	int chartID;
	/// <summary>
	/// �ֹ� �䱸 ���� ��ȣ
	/// �������� �ֹ���ȣ�� �ֱ�����
	/// Ŭ���̾�Ʈ ������ �ֹ��� �����ϱ� ���� ��ȣ
	/// �� ��ȣ�� ���� ���� �ܺ� �ֹ����� �����ϰ� ó���Ѵ�.
	/// </summary>
	int requestID;
	/// <summary>
	/// �ֹ� ����
	/// </summary>
	std::string accountNo;
	/// <summary>
	/// �ֹ� ���� ��� ��ȣ
	/// </summary>
	std::string password;
	/// <summary>
	/// �ֹ� Ÿ�� : �ű�, ����, ���
	/// </summary>
	VtOrderType orderType;
	/// <summary>
	/// ������ Ÿ�� : �ŵ�, �ż�
	/// </summary>
	VtPositionType orderPosition;
	/// <summary>
	/// ���� ���� : ������, ���尡, ����, �ֻ��� ������
	/// </summary>
	VtPriceType priceType;
	/// <summary>
	/// ü������ : �������� ��� 1, ���尡�� ��� 3
	/// </summary>
	VtTradeType tradeType;
	/// <summary>
	/// ���� �ڵ�
	/// ��Ʈ�� ������ ���� �ڵ带 ���� ��� ��� ������Ʈ ����� �ȴ�.
	/// </summary>
	std::string shortCode;
	/// <summary>
	/// �ֹ� ����
	/// </summary>
	double orderPrice;
	/// <summary>
	/// �ֹ� ����
	/// </summary>
	int amount;
	/// <summary>
	/// �� �ֹ� ��ȣ : �����̳� ��� �� �ʿ���
	/// </summary>
	int oriOrderNo;
	/// <summary>
	/// ��ǰ�� ƽ�� �ڸ���
	/// </summary>
	int productDecimal;
	std::string fullCode;

	/// <summary>
	/// �ֹ��� ������ ���� ������ �� ƽ ���� Ȥ�� ������ ������ ���ϴ� ��
	/// </summary>
	int slip;

	/// <summary>
	/// ����� ������ �ֹ� �䱸 ���� ��ȣ
	/// </summary>
	int counterReqId;
	/// <summary>
	/// ��ž�ֹ� Ÿ�� 0 : �Ϲ� , 1 : �ڵ����� ���� ��ž
	/// </summary>
	int stopType = 0;
	VtOrderEvent orderEvent = VtOrderEvent::None;
};


struct VtOrderDirectRequest
{
	/// <summary>
	/// �ֹ��� �� ��Ʈ ���̵�.
	/// ���� �� ���̵� ���� ��Ʈ�� ������
	/// ��Ʈ�� �ϳ� ����� �ű�� ǥ���Ѵ�.
	/// </summary>
	int chartID;
	/// <summary>
	/// �ֹ� �䱸 ���� ��ȣ
	/// �������� �ֹ���ȣ�� �ֱ�����
	/// Ŭ���̾�Ʈ ������ �ֹ��� �����ϱ� ���� ��ȣ
	/// �� ��ȣ�� ���� ���� �ܺ� �ֹ����� �����ϰ� ó���Ѵ�.
	/// </summary>
	int requestID;
	/// <summary>
	/// �ֹ� ����
	/// </summary>
	std::string accountNo;
	/// <summary>
	/// �ֹ� ���� ��� ��ȣ
	/// </summary>
	std::string password;
	/// <summary>
	/// �ֹ� Ÿ�� : �ű�, ����, ���
	/// </summary>
	VtOrderType orderType;
	/// <summary>
	/// ������ Ÿ�� : �ŵ�, �ż�
	/// </summary>
	VtPositionType orderPosition;
	/// <summary>
	/// ���� ���� : ������, ���尡, ����, �ֻ��� ������
	/// </summary>
	VtPriceType priceType;
	/// <summary>
	/// ü������ : �������� ��� 1, ���尡�� ��� 3
	/// </summary>
	VtTradeType tradeType;
	/// <summary>
	/// ���� �ڵ�
	/// ��Ʈ�� ������ ���� �ڵ带 ���� ��� ��� ������Ʈ ����� �ȴ�.
	/// </summary>
	std::string shortCode;
	/// <summary>
	/// �ֹ� ����
	/// </summary>
	std::string orderPrice;
	/// <summary>
	/// �ֹ� ����
	/// </summary>
	int amount;
	/// <summary>
	/// �� �ֹ� ��ȣ : �����̳� ��� �� �ʿ���
	/// </summary>
	int oriOrderNo;
	/// <summary>
	/// ��ǰ�� ƽ�� �ڸ���
	/// </summary>
	int productDecimal;
	/// <summary>
	/// �ֹ��� ������ ���� ������ �� ƽ ���� Ȥ�� ������ ������ ���ϴ� ��
	/// </summary>
	int slip;

	/// <summary>
	/// ����� ������ �ֹ� �䱸 ���� ��ȣ
	/// </summary>
	int counterReqId;
	/// <summary>
	/// ��ž�ֹ� Ÿ�� 0 : �Ϲ� , 1 : �ڵ����� ���� ��ž
	/// </summary>
	int stopType = 0;
};

// ������� �ֹ��� accountNo�� �θ��� ���¹�ȣ�� ����ϰ�
// SubAccountNo �� ���� ��ȣ�� ���� �����ؼ� ����Ѵ�.
// �Ϲݰ��� �ֹ��̵� ������� �ֹ��̵�  accountNo���� ���� ������ ��ȣ�� ����ִ�.
struct VtOrder 
{
	/// <summary>
	/// �ֹ��� �� ��Ʈ ���̵�.
	/// ���� �� ���̵� ���� ��Ʈ�� ������
	/// ��Ʈ�� �ϳ� ����� �ű�� ǥ���Ѵ�.
	/// </summary>
	int chartID = 0;
	/// <summary>
	/// �ֹ� �䱸 ���� ��ȣ
	/// �������� �ֹ���ȣ�� �ֱ�����
	/// Ŭ���̾�Ʈ ������ �ֹ��� �����ϱ� ���� ��ȣ
	/// </summary>
	int requestID = 0;
	/// <summary>
	/// �ֹ� ����
	/// </summary>
	std::string AccountNo;
	/// <summary>
	/// �ֹ� Ÿ�� : �ű�, ����, ���
	/// </summary>
	VtOrderType orderType;
	/// <summary>
	/// ������ Ÿ�� : �ŵ�, �ż�
	/// </summary>
	VtPositionType orderPosition;
	/// <summary>
	/// ���� ���� : ������, ���尡, ����, �ֻ��� ������
	/// </summary>
	VtPriceType priceType;
	/// <summary>
	/// ü������ : �������� ��� 1, ���尡�� ��� 3
	/// </summary>
	VtTradeType tradeType;
	/// <summary>
	/// ���� �ڵ�
	/// ��Ʈ�� ������ ���� �ڵ带 ���� ��� ��� ������Ʈ ����� �ȴ�.
	/// </summary>
	std::string shortCode;
	std::string fullCode;
	/// <summary>
	/// �ֹ� ����
	/// </summary>
	double orderPrice;
	/// <summary>
	/// �ֹ���ȣ
	/// </summary>
	int orderNo;
	/// <summary>
	/// �ֹ� ����
	/// </summary>
	int amount = 0;
	/// <summary>
	/// �� �ֹ� ��ȣ : �����̳� ��� �� �ʿ���
	/// </summary>
	int oriOrderNo;
	int firstOrderNo;
	/// <summary>
	/// �ŷ� ��¥
	/// </summary>
	std::string tradeDate;
	/// <summary>
	/// ü�� ����
	/// </summary>
	int filledQty;
	/// <summary>
	/// ü�� �ݾ�
	/// </summary>
	double filledMoney;
	/// <summary>
	/// ü�� ����
	/// </summary>
	double filledPrice;
	/// <summary>
	/// �ŷ� �ð�
	/// </summary>
	std::string tradeTime;

	//double stopPrice;
	/// <summary>
	/// �ֹ� ����
	/// </summary>
	VtOrderState state = VtOrderState::None;

	/// <summary>
	/// ü������ (1.FAS 2.FOK 3.FAK 4.GTC 5.GTD)	
	/// </summary>
	int tradeCondition;
	/// <summary>
	/// �ֹ� �ܷ� : �ֹ��� ���� �� ������ �Ǿ����� ü���� ���� ���� ����
	/// </summary>
	int unacceptedQty;
	/// <summary>
	/// ü�� ��ȣ
	/// </summary>
	int TradeNo;
	int productDecimal;

	// ���÷��̸� ���� �ֹ� ����
	//std::string strOrderPrice;
	// ���÷��̸� ���� ü�� ����
	//std::string strFilledPrice;

	/// <summary>
	/// ü��� �ܰ� ���� - û���� �Ǹ� 0�� �ȴ�.
	/// </summary>
	int remainQty = 0;
	/// <summary>
	/// �ֹ� ���� ����
	/// </summary>
	int orderableAmount = 0;
	/// <summary>
	/// �ֹ������� ������ ȯ���� ��
	/// </summary>
	int intOrderPrice;
	// ü�� ������ ������ ȯ���� ��
	int intFilledPrice;

	// ���� ���� ��ȣ
	std::string SubAccountNo;
	// �θ� ���� ��ȣ
	std::string ParentAccountNo;
	// �ֹ���û ���̵�
	std::string RequestId;
	// �ֹ���û ��ó : �����쳪 ��Ʈ�� ���̵� �ִ´�.
	std::string SourceId;
	std::string filledTime;
	int modifiedOrderCount;
	/// <summary>
	/// The name of the underlying fund.
	/// </summary>
	std::string FundName;
	// ���� �ֹ��̳� û��
	// 0 : �Ϲ� , 1 : ���� û��, 2 : ���� û��
	int RequestType = 0;
	// �ֹ� Ÿ�� ->-1 : �ܺ��ֹ�,  0 : �Ϲ� �ֹ�, 1 : ������� �ֹ� , 2 : �ݵ� �ֹ�
	int Type = -1;
	// û�� ��û ��ȣ - û�� �ֹ��� ��� û�� ��� �ֹ���ȣ�� �־� �ش�.
	// �� �ֹ��� û�� �ֹ��� ���ؼ� ��� �ȴ�.
	// û�� �ֹ� ��û�� ������ �ܰ� ��Ͽ��� ��� ���ŵȴ�. 
	// �ֳ��ϸ� ���� ���� ������ ���� �ʱ� ���ؼ� �̴�.
	int LiqReqOrderNo  = -1;
	// ���� Ȯ�� ���� �÷��� - �����Ǿ� ������ ���� �� ����� �ȴ�.
	bool EnableLossCut = false;
	// ���� Ȯ�� ���� �÷��� - �����Ǿ� ������ ���� �� ����� �ȴ�.
	bool EnableProfitCut = false;
	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
	// �����翡�� ���� �ֹ���û ��ȣ - �̰����� ���� ��û ������ �ĺ��Ѵ�.
	int HtsOrderReqID = -1;
	VtOrderEvent orderEvent = VtOrderEvent::None;
};

struct VtPosition;
struct HdOrderRequest
{
	//���¹�ȣ		11 8�ڸ� ���� �������� ä��
	std::string AccountNo;
	//��й�ȣ08 4�ڸ� ���� �������� ä��
	std::string Password;
	//�����ڵ�32 8�ڸ� ���� �������� ä��
	std::string SymbolCode;
	//�Ÿű���01 1: �ż�, 2 : �ŵ�
	VtPositionType Position;
	//��������011 : ������, 2 : ���尡
	VtPriceType PriceType;
	//ü������011 : FAS, 2 : FOK, 3 : FAK
	VtFilledCondition FillCondition;
	//�ֹ�����13�Ҽ��� ���� ���(ex : 242.15 -> "24215        ")
	//�ؽ��尡�ΰ�� ������ "0            "���� ����
	int Price;
	//�ֹ�����05�ֹ� ����(ex:"1    ")
	int Amount;
	// �����̳� ��ҽ� �� �ֹ� ��ȣ
	int OrderNo = -1;
	// �����̳� ��ҷ� ���� ������ �ֹ���ȣ
	int NewOrderNo = -1;
	// ���� ���� ��ȣ
	std::string SubAccountNo = _T("";)
	// �ֹ���û ���̵�
	int RequestId = 0;
	// �ֹ���û ��ó : �����쳪 ��Ʈ�� ���̵� �ִ´�.
	long SourceId = 0;
	int slip = 2;

	/// <summary>
	/// �ֹ� Ÿ�� : �ű�, ����, ���
	/// </summary>
	VtOrderType orderType;
	// Fund Name
	std::string FundName = _T("");
	/// <summary>
	/// ���� 0 : �θ����, 1 : ���� ����
	/// </summary>
	int AccountLevel = 0;
	// Ÿ�� - 0 : ���� �ֹ�, 1 : ������� �ֹ�, 2 : �ݵ� �ֹ�
	int Type = 0;
	// ���� �̸�
	std::string StratageName = _T("");
	bool Paired = false;
	HdOrderRequest* PairedReq = nullptr;
	// ���� �ֹ��̳� û��
	// 0 : �Ϲ� , 1 : ����, 2 : ����
	int RequestType = 0;
	// ���Ŀ� �����ǿ� ������ ���� ���� ������ �����ϱ� ���� �ʿ���
	VtPosition* SrcPosition = nullptr;
	// û�� ��û ��ȣ - û�� �ֹ��� ��� û�� ��� �ֹ���ȣ�� �־� �ش�.
	// �� �ֹ��� û�� �ֹ��� ���ؼ� ��� �ȴ�.
	// û�� �ֹ� ��û�� ������ �ܰ� ��Ͽ��� ��� ���ŵȴ�. 
	// �ֳ��ϸ� ���� ���� ������ ���� �ʱ� ���ؼ� �̴�.
	int LiqReqOrderNo = -1;
	// �����翡�� ���� �ֹ���û ��ȣ - �̰����� ���� ��û ������ �ĺ��Ѵ�.
	int HtsOrderReqID = -1;
	// ���� : 1 : �ؿ�, 2 : FX, 9 : ����
	int Market = 9;
};

struct VtRealtimeOrder
{
	std::string	accountNo; 	/* ���¹�ȣ														*/
	VtOrderState state;	/* ��������
							1.�ֹ����� 2.ü�� 3.����Ȯ�� 4.���Ȯ��
							5.�ű԰ź� 6.�����ź� 7.��Ұź� 0.��������					*/
	std::string	fullCode;	/* �����ڵ� (���������϶� ǥ���ڵ�, �׿� �����ڵ�)				*/
	VtPositionType	orderPosition;	/* �ż�/�ŵ����� (1.�ż� 2.�ŵ�)								*/
	int	amount;	/* �ֹ�����														*/
	VtPriceType	priceType;	/* ��������	(1.������ 2.���尡)									*/
	double	orderedPrice;	/* �ֹ�����														*/
	int	tradeCondition;	/* ü������ (1.FAS 2.FOK 3.FAK 4.GTC 5.GTD)						*/
	double	filledPrice;	/* ü�ᰡ��														*/
	int	filledAmount;	/* ü�����														*/
	int	remainQty;	/* �ֹ��ܷ�	: ��ü�� ����													*/
	int	orderNo;	/* �ֹ���ȣ														*/
	int	oriOrderNo;	/* ���ֹ���ȣ													*/
	std::string	tradeTime;	/* �ֹ�Ȯ��,ü��,�ź� �ð�										*/
	double	filledMoney;	/* ü��ݾ�														*/
	VtOrderType	orderType;	/* �ֹ�����	(ReplyType==0�϶� 1.�ű� 2.���� 3.���)				*/
	int    tradeNo;   /* ü���ȣ														*/
	std::string    tradeDate;   /* ü������														*/
	int	liqAmount;	/* û�갡�ɼ���													*/
	int	remainAmount;	/* �ܰ����														*/
	VtPositionType	remainPosition;	/* �ܰ������Ǳ��� (1.�ż� 2.�ŵ�)								*/
	int    orderableAmount;   /* �ֹ����ɼ���													*/
	// ���÷��̸� ���� ü�� ����
	//std::string strOrderPrice;
	//std::string strFilledPrice;
	int intOrderPrice;
	int intFilledPrice;
	int productDecimal;
};

