#pragma once
#include <string>
#include "Global/VtDefine.h"
struct VtPosition
{
	// 전략이름
	std::string StrategyName;
	// 펀드 이름 - 펀드는 서브 계좌로 이루어져 현재는 사용되지 않음
	std::string FundName;
	std::string SubAccountNo; // 서브 계좌 번호
	std::string AccountNo; //	12	string	가상계좌번호
	std::string FullCode; //	16	string	종목코드
	std::string ShortCode;
	VtPositionType Position = VtPositionType::None; //	1	
	int TotVolume; //	10	int	전체잔고
	int TodayVol; //	10	int	당일잔고
	int PrevOpenQty; //	10	int	전일잔고
	double Fee; //	15	double	수수료
	double OpenAmt; //	15	double	평가금액
	double TradePL; //	15	double	매매손익

	std::string	BaseDate;	/* 생성일자														*/
	int	TradeNo = 0;	/* 체결번호														*/
	int	OpenQty = 0;	/* 미결제수량													*/
	double	AvgPrice = 0.0f;	/* 평균가														*/
	double	CurPrice = 0.0f;	/* 현재가 														*/
	double	OpenProfitLoss = 0.0f;	/* 평가손익														*/
	int 	RsrbQty = 0;	/* 청산가능수량													*/
	double	TradeAmount = 0.0f;	/* 체결금액														*/
	int intAvgPrice = 0;
	int intCurPrice = 0;
	// 이미 익절이나 손절이 설정되어 있을 경우
	// 익절이나 손절이 완전히 청산되기 전까지
	// 혹은 수동으로 손절, 익절을 취소할 때까지는  true,
	// 그 이외에는 false가 된다. 
	// false 인 상태에서는 익절 손절을 체크하지만 true경우는 체크하지 않는다.
	bool ProfitCutSet = false;
	bool LossCutSet = false;
	// 주문 타입 -> 0 : 일반 주문, 1 : 서브계좌 주문 , 2 : 펀드 주문
	int orderType = 0;
};

