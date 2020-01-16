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
	/// 주문을 낸 차트 아이디.
	/// 만약 이 아이디를 가진 차트가 없으면
	/// 차트를 하나 만들고 거기다 표시한다.
	/// </summary>
	int chartID;
	/// <summary>
	/// 주문 요구 고유 번호
	/// 서버에서 주문번호를 주기전에
	/// 클라이언트 측에서 주문을 구별하기 위한 번호
	/// 이 번호가 없는 것은 외부 주문으로 간주하고 처리한다.
	/// </summary>
	int requestID;
	/// <summary>
	/// 주문 계좌
	/// </summary>
	std::string accountNo;
	/// <summary>
	/// 주문 계좌 비밀 번호
	/// </summary>
	std::string password;
	/// <summary>
	/// 주문 타입 : 신규, 정정, 취소
	/// </summary>
	VtOrderType orderType;
	/// <summary>
	/// 포지션 타입 : 매도, 매수
	/// </summary>
	VtPositionType orderPosition;
	/// <summary>
	/// 가격 형태 : 지정가, 시장가, 조건, 최상위 유리가
	/// </summary>
	VtPriceType priceType;
	/// <summary>
	/// 체결조건 : 지정가일 경우 1, 시장가일 경우 3
	/// </summary>
	VtTradeType tradeType;
	/// <summary>
	/// 종목 코드
	/// 차트가 동일한 종목 코드를 가질 경우 모두 업데이트 대상이 된다.
	/// </summary>
	std::string shortCode;
	/// <summary>
	/// 주문 가격
	/// </summary>
	double orderPrice;
	/// <summary>
	/// 주문 수량
	/// </summary>
	int amount;
	/// <summary>
	/// 원 주문 번호 : 정정이나 취소 때 필요함
	/// </summary>
	int oriOrderNo;
	/// <summary>
	/// 제품의 틱의 자릿수
	/// </summary>
	int productDecimal;
	std::string fullCode;

	/// <summary>
	/// 주문이 나갈때 현재 값보다 몇 틱 위로 혹은 밑으로 나갈지 정하는 값
	/// </summary>
	int slip;

	/// <summary>
	/// 상대편 포지션 주문 요구 고유 번호
	/// </summary>
	int counterReqId;
	/// <summary>
	/// 스탑주문 타입 0 : 일반 , 1 : 자동익절 손절 스탑
	/// </summary>
	int stopType = 0;
	VtOrderEvent orderEvent = VtOrderEvent::None;
};


struct VtOrderDirectRequest
{
	/// <summary>
	/// 주문을 낸 차트 아이디.
	/// 만약 이 아이디를 가진 차트가 없으면
	/// 차트를 하나 만들고 거기다 표시한다.
	/// </summary>
	int chartID;
	/// <summary>
	/// 주문 요구 고유 번호
	/// 서버에서 주문번호를 주기전에
	/// 클라이언트 측에서 주문을 구별하기 위한 번호
	/// 이 번호가 없는 것은 외부 주문으로 간주하고 처리한다.
	/// </summary>
	int requestID;
	/// <summary>
	/// 주문 계좌
	/// </summary>
	std::string accountNo;
	/// <summary>
	/// 주문 계좌 비밀 번호
	/// </summary>
	std::string password;
	/// <summary>
	/// 주문 타입 : 신규, 정정, 취소
	/// </summary>
	VtOrderType orderType;
	/// <summary>
	/// 포지션 타입 : 매도, 매수
	/// </summary>
	VtPositionType orderPosition;
	/// <summary>
	/// 가격 형태 : 지정가, 시장가, 조건, 최상위 유리가
	/// </summary>
	VtPriceType priceType;
	/// <summary>
	/// 체결조건 : 지정가일 경우 1, 시장가일 경우 3
	/// </summary>
	VtTradeType tradeType;
	/// <summary>
	/// 종목 코드
	/// 차트가 동일한 종목 코드를 가질 경우 모두 업데이트 대상이 된다.
	/// </summary>
	std::string shortCode;
	/// <summary>
	/// 주문 가격
	/// </summary>
	std::string orderPrice;
	/// <summary>
	/// 주문 수량
	/// </summary>
	int amount;
	/// <summary>
	/// 원 주문 번호 : 정정이나 취소 때 필요함
	/// </summary>
	int oriOrderNo;
	/// <summary>
	/// 제품의 틱의 자릿수
	/// </summary>
	int productDecimal;
	/// <summary>
	/// 주문이 나갈때 현재 값보다 몇 틱 위로 혹은 밑으로 나갈지 정하는 값
	/// </summary>
	int slip;

	/// <summary>
	/// 상대편 포지션 주문 요구 고유 번호
	/// </summary>
	int counterReqId;
	/// <summary>
	/// 스탑주문 타입 0 : 일반 , 1 : 자동익절 손절 스탑
	/// </summary>
	int stopType = 0;
};

// 서브계좌 주문은 accountNo는 부모의 계좌번호를 사용하고
// SubAccountNo 에 계좌 번호를 따로 저장해서 사용한다.
// 일반계좌 주문이든 서브계좌 주문이든  accountNo에는 실제 계좌의 번호가 들어있다.
struct VtOrder 
{
	/// <summary>
	/// 주문을 낸 차트 아이디.
	/// 만약 이 아이디를 가진 차트가 없으면
	/// 차트를 하나 만들고 거기다 표시한다.
	/// </summary>
	int chartID = 0;
	/// <summary>
	/// 주문 요구 고유 번호
	/// 서버에서 주문번호를 주기전에
	/// 클라이언트 측에서 주문을 구별하기 위한 번호
	/// </summary>
	int requestID = 0;
	/// <summary>
	/// 주문 계좌
	/// </summary>
	std::string AccountNo;
	/// <summary>
	/// 주문 타입 : 신규, 정정, 취소
	/// </summary>
	VtOrderType orderType;
	/// <summary>
	/// 포지션 타입 : 매도, 매수
	/// </summary>
	VtPositionType orderPosition;
	/// <summary>
	/// 가격 형태 : 지정가, 시장가, 조건, 최상위 유리가
	/// </summary>
	VtPriceType priceType;
	/// <summary>
	/// 체결조건 : 지정가일 경우 1, 시장가일 경우 3
	/// </summary>
	VtTradeType tradeType;
	/// <summary>
	/// 종목 코드
	/// 차트가 동일한 종목 코드를 가질 경우 모두 업데이트 대상이 된다.
	/// </summary>
	std::string shortCode;
	std::string fullCode;
	/// <summary>
	/// 주문 가격
	/// </summary>
	double orderPrice;
	/// <summary>
	/// 주문번호
	/// </summary>
	int orderNo;
	/// <summary>
	/// 주문 수량
	/// </summary>
	int amount = 0;
	/// <summary>
	/// 원 주문 번호 : 정정이나 취소 때 필요함
	/// </summary>
	int oriOrderNo;
	int firstOrderNo;
	/// <summary>
	/// 거래 날짜
	/// </summary>
	std::string tradeDate;
	/// <summary>
	/// 체결 수량
	/// </summary>
	int filledQty;
	/// <summary>
	/// 체결 금액
	/// </summary>
	double filledMoney;
	/// <summary>
	/// 체결 가격
	/// </summary>
	double filledPrice;
	/// <summary>
	/// 거래 시간
	/// </summary>
	std::string tradeTime;

	//double stopPrice;
	/// <summary>
	/// 주문 상태
	/// </summary>
	VtOrderState state = VtOrderState::None;

	/// <summary>
	/// 체결조건 (1.FAS 2.FOK 3.FAK 4.GTC 5.GTD)	
	/// </summary>
	int tradeCondition;
	/// <summary>
	/// 주문 잔량 : 주문을 냈을 때 접수는 되었지만 체결이 되지 않은 수량
	/// </summary>
	int unacceptedQty;
	/// <summary>
	/// 체결 번호
	/// </summary>
	int TradeNo;
	int productDecimal;

	// 디스플레이를 위한 주문 가격
	//std::string strOrderPrice;
	// 디스플레이를 위한 체결 가격
	//std::string strFilledPrice;

	/// <summary>
	/// 체결된 잔고 수량 - 청산이 되면 0이 된다.
	/// </summary>
	int remainQty = 0;
	/// <summary>
	/// 주문 가능 수량
	/// </summary>
	int orderableAmount = 0;
	/// <summary>
	/// 주문가격을 정수로 환산한 것
	/// </summary>
	int intOrderPrice;
	// 체결 가격을 정수로 환산한 것
	int intFilledPrice;

	// 서브 계좌 번호
	std::string SubAccountNo;
	// 부모 계좌 번호
	std::string ParentAccountNo;
	// 주문요청 아이디
	std::string RequestId;
	// 주문요청 출처 : 윈도우나 차트의 아이디를 넣는다.
	std::string SourceId;
	std::string filledTime;
	int modifiedOrderCount;
	/// <summary>
	/// The name of the underlying fund.
	/// </summary>
	std::string FundName;
	// 정산 주문이냐 청산
	// 0 : 일반 , 1 : 익절 청산, 2 : 손절 청산
	int RequestType = 0;
	// 주문 타입 ->-1 : 외부주문,  0 : 일반 주문, 1 : 서브계좌 주문 , 2 : 펀드 주문
	int Type = -1;
	// 청산 요청 번호 - 청산 주문일 경우 청산 대상 주문번호를 넣어 준다.
	// 그 주문은 청산 주문에 의해서 상쇄 된다.
	// 청산 주문 요청을 받으면 잔고 목록에서 즉시 제거된다. 
	// 왜냐하면 손절 익절 영향을 받지 않기 위해서 이다.
	int LiqReqOrderNo  = -1;
	// 손절 확인 여부 플래그 - 설정되어 있으면 손절 평가 대상이 된다.
	bool EnableLossCut = false;
	// 익절 확인 여부 플래그 - 설정되어 있으면 익절 평가 대상이 된다.
	bool EnableProfitCut = false;
	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
	// 선물사에서 받은 주문요청 번호 - 이것으로 본래 요청 정보를 식별한다.
	int HtsOrderReqID = -1;
	VtOrderEvent orderEvent = VtOrderEvent::None;
};

struct VtPosition;
struct HdOrderRequest
{
	//계좌번호		11 8자리 이후 공백으로 채움
	std::string AccountNo;
	//비밀번호08 4자리 이후 공백으로 채움
	std::string Password;
	//종목코드32 8자리 이후 공백으로 채움
	std::string SymbolCode;
	//매매구분01 1: 매수, 2 : 매도
	VtPositionType Position;
	//가격조건011 : 지정가, 2 : 시장가
	VtPriceType PriceType;
	//체결조건011 : FAS, 2 : FOK, 3 : FAK
	VtFilledCondition FillCondition;
	//주문가격13소수점 없이 사용(ex : 242.15 -> "24215        ")
	//※시장가인경우 가격은 "0            "으로 세팅
	int Price;
	//주문수량05주문 수량(ex:"1    ")
	int Amount;
	// 정정이나 취소시 원 주문 번호
	int OrderNo = -1;
	// 정정이나 취소로 새로 생성된 주문번호
	int NewOrderNo = -1;
	// 서브 계좌 번호
	std::string SubAccountNo = _T("";)
	// 주문요청 아이디
	int RequestId = 0;
	// 주문요청 출처 : 윈도우나 차트의 아이디를 넣는다.
	long SourceId = 0;
	int slip = 2;

	/// <summary>
	/// 주문 타입 : 신규, 정정, 취소
	/// </summary>
	VtOrderType orderType;
	// Fund Name
	std::string FundName = _T("");
	/// <summary>
	/// 레벨 0 : 부모계좌, 1 : 서브 계좌
	/// </summary>
	int AccountLevel = 0;
	// 타입 - 0 : 계좌 주문, 1 : 서브계좌 주문, 2 : 펀드 주문
	int Type = 0;
	// 전략 이름
	std::string StratageName = _T("");
	bool Paired = false;
	HdOrderRequest* PairedReq = nullptr;
	// 정산 주문이냐 청산
	// 0 : 일반 , 1 : 익절, 2 : 손절
	int RequestType = 0;
	// 추후에 포지션에 설정된 익절 손절 정보를 해제하기 위해 필요함
	VtPosition* SrcPosition = nullptr;
	// 청산 요청 번호 - 청산 주문일 경우 청산 대상 주문번호를 넣어 준다.
	// 그 주문은 청산 주문에 의해서 상쇄 된다.
	// 청산 주문 요청을 받으면 잔고 목록에서 즉시 제거된다. 
	// 왜냐하면 손절 익절 영향을 받지 않기 위해서 이다.
	int LiqReqOrderNo = -1;
	// 선물사에서 받은 주문요청 번호 - 이것으로 본래 요청 정보를 식별한다.
	int HtsOrderReqID = -1;
	// 마켓 : 1 : 해외, 2 : FX, 9 : 국내
	int Market = 9;
};

struct VtRealtimeOrder
{
	std::string	accountNo; 	/* 계좌번호														*/
	VtOrderState state;	/* 응답유형
							1.주문접수 2.체결 3.정정확인 4.취소확인
							5.신규거부 6.정정거부 7.취소거부 0.원장접수					*/
	std::string	fullCode;	/* 종목코드 (원장접수일때 표준코드, 그외 단축코드)				*/
	VtPositionType	orderPosition;	/* 매수/매도구분 (1.매수 2.매도)								*/
	int	amount;	/* 주문수량														*/
	VtPriceType	priceType;	/* 가격조건	(1.지정가 2.시장가)									*/
	double	orderedPrice;	/* 주문가격														*/
	int	tradeCondition;	/* 체결조건 (1.FAS 2.FOK 3.FAK 4.GTC 5.GTD)						*/
	double	filledPrice;	/* 체결가격														*/
	int	filledAmount;	/* 체결수량														*/
	int	remainQty;	/* 주문잔량	: 미체결 수량													*/
	int	orderNo;	/* 주문번호														*/
	int	oriOrderNo;	/* 원주문번호													*/
	std::string	tradeTime;	/* 주문확인,체결,거부 시간										*/
	double	filledMoney;	/* 체결금액														*/
	VtOrderType	orderType;	/* 주문구분	(ReplyType==0일때 1.신규 2.정정 3.취소)				*/
	int    tradeNo;   /* 체결번호														*/
	std::string    tradeDate;   /* 체결일자														*/
	int	liqAmount;	/* 청산가능수량													*/
	int	remainAmount;	/* 잔고수량														*/
	VtPositionType	remainPosition;	/* 잔고포지션구분 (1.매수 2.매도)								*/
	int    orderableAmount;   /* 주문가능수량													*/
	// 디스플레이를 위한 체결 가격
	//std::string strOrderPrice;
	//std::string strFilledPrice;
	int intOrderPrice;
	int intFilledPrice;
	int productDecimal;
};

