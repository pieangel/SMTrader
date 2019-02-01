//////////////////////////////////////////////////////////////////////
// ESApiLayout.h	: 응용 프로그램에서 사용하는 포맷
//
//////////////////////////////////////////////////////////////////////

#if !defined(__COMMON_ESAPILAYOUT_FORMAT_H__)
#define __COMMON_ESAPILAYOUT_FORMAT_H__

#include <afxtempl.h>

enum _tagErrorCode
{
	ERR_NONE			= 0,
	ERR_AUTOKEY,			// 계좌 자동업데이트 key 미입력
	ERR_USERID,				// 접속 아이디 미입력
	ERR_USERPASS,			// 접속 비밀번호 미입력
	ERR_CERTPASS,			// 공인 인증 비밀번호 미입력 (데모는 제외)
	ERR_SVRSENDDATA,		// 서버 전송 오류
	ERR_SVRCONNECT,			// 서버 접속 상태
	ERR_SVRNOCONNECT,		// 서버 미접속 상태
	ERR_CONNECT,			// 서버 접속 오류
	ERR_CERTSET,			// 인증 데이터 가져오기 실패
	ERR_DLLNOEXIST,			// signkorea dll 파일 경로
	ERR_TRCODE,				// 허용되지 않은 TR번호
};

enum _tagServerKind
{
	SERVER_REAL,
	SERVER_TEST,
	SERVER_DEMO,
};

enum _tagNextKind
{
	NEXT_NONE	= '0',
	NEXT_DATA	= '1',
};

///////////////////////////////////////////////////////////////////////////////
// 사용계좌 정보
typedef	struct	_ESAccntInfo
{
	char	AccNo				[11];	/* 계좌번호														*/
	char	AccNm				[30];	/* 계좌명														*/
}	ESAccntInfo_st, * lpESAccntInfo_st;
#define	SZ_ESAccntInfo	sizeof(ESAccntInfo_st)

typedef CArray < ESAccntInfo_st, ESAccntInfo_st > CArrayAcct;

typedef	struct _ESHeader_st
{
	char	WinId				[10];	/* Client WINDOWS ID										 	*/
	char	PCKey				[ 1];	/* Request key													*/
	char	ErrCode				[ 4];	/* Error Code
											공백이나 0000이면 정상, 그외 오류, _ESErr_st 로 받음)		*/
	char	NextKind			[ 1];	/* 다음 구분 (0.다음없음, 1.다음 있음)							*/
	char	filler				[15];	/* filler														*/
} ESHeader_st, *lpESHeader_st;
#define SZ_ESHeader sizeof(ESHeader_st)

typedef struct _ESErr_st
{
	ESHeader_st					Head;
	char	ErrMsg				[100];	/* Message														*/
}ESErr_st, * lpESErr_st;
#define	SZ_ESErr_st	sizeof(ESErr_st)

typedef struct _ESAutoKey
{
	ESHeader_st					Head;
	char	AutoKey				[32];	/* 계좌의 경우 계좌번호, 시세 경우 종목표준코드					*/
} ESAutoKey_st, *lpESAutoKey_st;
#define SZ_ESAutoKey	sizeof(ESAutoKey_st)


//---------------------------------------------------------------------------------------------------------------------------------
//
// 해외용 API 내용
//
//---------------------------------------------------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// 종목 정보
typedef	struct	_ESCodeInfo
{
	char	FullCode			[32];	/* 종목 Full code												*/
	char	ShortCode			[ 5];	/* 종목 Short code												*/
	char	Index				[ 4];	/* 종목 인덱스													*/
	char	CodeName			[32];	/* 종목명														*/
	char	DecimalInfo			[ 5];	/* 상품가격에대한소수점정보										*/
	char	TickSize			[20];	/* TICK SIZE 소수점7자리포함									*/
	char	TickValue			[20];	/* 최소가격변동금액 소수점7자리포함								*/
}	ESCodeInfo_st, * lpESCodeInfo_st;
#define	SZ_ESCodeInfo	sizeof(ESCodeInfo_st)

typedef CArray < ESCodeInfo_st, ESCodeInfo_st >	CArrayCode;

//////////////////////////////////////////////////////////////////////////////////
// 서버 전송 패킷
//////////////////////////////////////////////////////////////////////////////////

// 계좌 관련
#define ESID_5601				5601	// 신규주문
#define ESID_5602				5602	// 정정주문
#define ESID_5603				5603	// 취소주문
#define ESID_5611				5611	// 실체결
#define ESID_5612				5612	// 실잔고
#define ESID_5614				5614	// 계좌별 주문체결현황
#define ESID_5615				5615	// 예탁자산및 증거금

// 시세 관련
#define ESID_5501				5501	// 종목 Master
#define ESID_5502				5502	// 종목 시세
#define ESID_5503				5503	// 종목 호가
#define ESID_5511				5511	// 종목 체결내역
#define	ESID_5522				5522	//선물 CHART DATA

// 자동업데이트
#define AUTO_0931				 931	// 종목 Master 실시간
#define AUTO_0932				 932	// 종목 호가 실시간
#define AUTO_0933				 933	// 종목 시세 실시간
#define AUTO_0985				 985	// 잔고/체결 실시간

//////////////////////////////////////////////////////////////////////////////////
// 주문 전송 (신규, 정정, 취소)
typedef struct _Es5601I
{
	ESHeader_st					Head;
	char	Account				[11];	/* 계좌번호														*/
	char	Pass				[ 8];	/* 비밀번호														*/
	char	Order_Type			[ 1];	/* 주문구분	(1.신규주문 2.정정주문 3.취소주문)					*/
	char	ShortCode			[32];	/* 종목단축코드													*/
	char	BuySell_Type		[ 1];	/* 매도매수구분 (1.매수 2.매도)									*/
	char    Price_Type			[ 1];	/* 가격조건	(1.지정가 2.시장가)									*/
	char	Trace_Type			[ 1];	/* 체결조건 (시장가일경우(3) 지정가일경우(1))					*/
	char    Order_Price			[20];	/* 주문가격														*/
	char    Order_Volume		[ 5];	/* 주문수량														*/
	char	Order_Org_No		[ 5];	/* 원주문번호 (정정/취소시만)									*/
	char    Order_Comm_Type		[ 1];	/* 통신주문구분													*/
	char	Stop_Type			[ 1];	/* 주문전략구분 (1.일반주문 2.STOP주문)							*/
	char	Stop_Price			[20];	/* STOP주문가격 (STOP주문이면 입력 아니면 0 셋팅)				*/
	char	Oper_Type			[ 1];	/* 주문구분	(1.신규주문 2.정정주문 3.취소주문)					*/
}	Es5601I_st, *lpEs5601I_st;
#define	SZ_Es5601I	sizeof(Es5601I_st)
	
typedef struct _Es5601O
{
	ESHeader_st					Head;
	char	Order_No  			[ 5];	/* 주문번호														*/
	char    Order_Org_No		[ 5];	/* 원주문번호													*/
	char	Account				[11];	/* 계좌번호														*/
	char	Order_Type			[ 1];	/* 주문구분	(1.신규주문 2.정정주문 3.취소주문)					*/
	char	ShortCode			[32];	/* 종목단축코드														*/
	char	BuySell_Type		[ 1];	/* 매도매수구분 (1.매수 2.매도)									*/
	char    Order_Volume		[ 5];	/* 주문수량														*/
	char    Order_Price			[20];	/* 주문가격														*/
	char    Price_Type			[ 1];	/* 가격조건	(1.지정가 2.시장가)									*/
	char	Trade_Type			[ 1];	/* 체결조건 (시장가일경우(3) 지정가일경우(1))					*/
	char	Stop_Price			[20];	/* STOP주문가격													*/
}	Es5601O_st,	* lpEs5601O_st;
#define	SZ_Es5601O	sizeof(Es5601O_st)


//////////////////////////////////////////////////////////////////////////////////
// 계좌 실체결 조회
typedef struct _Es5611I
{
	ESHeader_st					Head;
	char	Account				[11];	/* 계좌번호														*/
	char	Pass				[ 8];	/* 비밀번호														*/
	char	Trd_gb				[ 1];	/* 체결구분 (0:전체 1:체결2:미체결)								*/
	char	Base_dt				[ 8];	/* 주문일자														*/
	char	Gubn				[ 1];	/* 조회순서 (1:정순 2:역순)										*/
}	Es5611I_st,	* lpEs5611I_st;
#define	SZ_Es5611I	sizeof(Es5611I_st)

typedef struct _Es5611Sub
{
	char	Ord_No				[ 5];	/* 주문주번호													*/
	char	Org_ord_No			[ 5];	/* 원주문부번호													*/
	char	Trd_cond			[ 1];	/* 체결조건 (1.FAS 2.FOK 3.FAK)									*/
	char	ShortCode			[32];	/* 종목단축코드													*/
	char	Bysl_tp				[ 1];	/* 매매구분코드
											1.매수 2.매도 3.매수정정
											4.매도정정 5.매수취소 6.매도취소 ' '.기타					*/
	char	Prce_tp				[ 1];	/* 가격조건	(1.지정가 2.시장가)									*/
	char	Ord_q				[ 5];	/* 주문수량														*/
	char	Ord_p				[20];	/* 주문지수 or 체결지수											*/
	char	Trd_q				[ 5];	/* 체결수량														*/
	char	Mcg_q				[ 5];	/* 미체결수량													*/
	char	Ord_tp				[ 1];	/* 주문구분 (1.신규 2.정정 3.취소)								*/
	char	Stop_p				[20];	/* STOP주문가격													*/
	char	Ex_ord_tm			[ 6];	/* 주문시간														*/
	char	Proc_stat			[ 1];	/* 주문처리상태 (0.원장접수 1.거래소접수 2.접수거부 3.FEP거부)	*/
	char	Account				[11];	/* 계좌번호														*/
}	Es5611Sub_st, *lpEs5611Sub_st;

typedef struct _Es5611O
{
	ESHeader_st					Head;
	char	Renu				[ 5];	/* 반복횟수														*/
	char	Account				[11];	/* 계좌번호														*/
	char	AcctNm				[20];	/* 계좌명														*/
	char	Dtno				[ 5];	/* 반복횟수														*/
	Es5611Sub_st Sub			[12];	/* 체결 데이타													*/
}	Es5611O_st,	*lpEs5611O_st;
#define	SZ_Es5611O	sizeof(Es5611O_st)


//////////////////////////////////////////////////////////////////////////////////
// 계좌 실잔고 조회
typedef struct _Es5612I
{
	ESHeader_st					Head;
	char	Account				[11];	/* 계좌번호														*/
	char	Pass				[ 8];	/* 비밀번호														*/
}	Es5612I_st,	* lpEs5612I_st;
#define	SZ_Es5612I	sizeof(Es5612I_st)

typedef struct _Es5612Sub
{
	char	Base_dt				[ 8];	/* 생성일자														*/
	char	FullCode			[32];	/* 종목표준코드													*/
	char	Bysl_tp				[ 1];	/* 매매구분	(1.매수 2.매도)										*/
	char	Trd_no				[ 5];	/* 체결번호														*/
	char	Open_q				[10];	/* 미결제수량													*/
	char	Avgt_p				[20];	/* 평균가														*/
	char	Curr_p				[20];	/* 현재가 														*/
	char	Open_pl				[20];	/* 평가손익														*/
	char	Rsrb_q				[10];	/* 청산가능수량													*/
	char	Trd_amt				[20];	/* 체결금액														*/
	char	Account				[11];	/* 계좌번호														*/
}	Es5612Sub_st, *lpEs5612Sub_st;

typedef struct _Es5612O
{
	ESHeader_st					Head;
	char	Renu				[ 5];	/* 반복횟수														*/
	char	Account				[11];	/* 계좌번호														*/
	char	AcctNm				[20];	/* 계좌명														*/
	char	Dtno				[ 5];	/* 반복횟수														*/
	Es5612Sub_st	Sub			[16];
}	Es5612O_st,	*lpEs5612O_st;
#define	SZ_Es5612O	sizeof(Es5612O_st)


//////////////////////////////////////////////////////////////////////////////////
// (해외선물)계좌별 주문체결현황
//
typedef struct _Es5614I
{
	ESHeader_st					Head;
	char	Account				[11];	/* 계좌번호														*/
	char	Pass				[ 8];	/* 비밀번호														*/
	char	Trd_gb				[ 1];	/* 체결구분
											0.전체 1.체결 2.미체결 3.전량정정 4.전량취소 5.주문거부
											6.체결+미체결												*/
	char	Base_dt				[ 8];	/* 주문일자														*/
	char	Gubn				[ 1];	/* 조회순서 (1.정순 2.역순)										*/
	char	Ord_no				[ 5];	/* 시작주문번호													*/
	char	Dtno				[ 5];	/* 요청갯수														*/
}	Es5614I_st,	* lpEs5614I_st;
#define	SZ_Es5614I	sizeof(Es5614I_st)

typedef struct _Es5614Sub
{
	char	Ord_no				[ 5];	/* 주문주번호													*/
	char	Orig_ord_no			[ 5];	/* 원주문부번호													*/
	char	Trd_cond			[ 1];	/* 체결조건	(1.FAS 2.FOK 3.FAK)									*/
	char	Series				[32];	/* 종목번호														*/
	char	Bysl_tp				[ 1];	/* 매매구분코드
											1.매수 2.매도 3.매수정정 4.매도정정 5.매수취소 
											6.매도취소 ' '.기타											*/
	char	Prce_tp				[ 1];	/* 가격조건 (1.지정가 2.시장가)									*/
	char	Ord_q				[ 5];	/* 주문수량														*/
	char	Ord_p				[20];	/* 주문지수														*/
	char	Trd_q				[ 5];	/* 체결수량 합													*/
	char	Trd_p				[20];	/* 체결가격	평균가												*/
										/* 체결없으면 체결가격 SPACE									*/
	char	Trd_no				[ 5];	/* 체결번호	SPACE												*/
	char	Ex_trd_tm			[ 6];	/* 체결시간	SPACE												*/
	char	Mcg_q				[ 5];	/* 미체결수량													*/
	char	Ord_tp				[ 1];	/* 주문구분 (1.신규주문 2.정정주문 3.취소주문)					*/
	char	Stop_p				[20];	/* STOP주문가격													*/
	char	Ex_ord_tm			[ 6];	/* 주문시간														*/
	char	Proc_stat			[ 1];	/* 주문처리상태	
											'0'.접수전 '1'.정상접수
											'2'.접수거부 '3'.FEP접수거부								*/
	char	Trd_cnt				[ 1];	/* 체결건수 (1.한방 2.여러건)									*/
	char	Trd_gb				[ 1];	/* 체결구분 
											1.체결 2.미체결 3.전량정정 4.전량취소 5.주문거부
											6.체결+미체결												*/
	char	Account				[11];	/* 계좌번호														*/
}	Es5614Sub_st, *lpEs5614Sub_st;

typedef struct _Es5614O
{
	ESHeader_st					Head;
	char	Renu				[ 5];	/* 반복횟수														*/
	char	Account				[11];	/* 계좌번호														*/
	char	Acct_nm				[20];	/* 계좌명														*/
	char	Dtno				[ 5];	/* 반복횟수														*/
	Es5614Sub_st		Sub		[23];
}	Es5614O_st,	*lpEs5614O_st;
#define	SZ_Es5614O	sizeof(Es5614O_st)


//////////////////////////////////////////////////////////////////////////////////
// 예탁자산및 증거금
typedef struct _Es5615I
{
	ESHeader_st					Head;
	char	Account				[11];	/* 계좌번호														*/
	char	Pass				[ 8];	/* 비밀번호														*/
	char	Crc_cd				[ 3];	/* 통화코드	(USD 로 기본으로 사용)								*/
}	Es5615I_st,	* lpEs5615I_st;
#define	SZ_Es5615I	sizeof(Es5615I_st)

typedef struct _Es5615O
{
	ESHeader_st					Head;
	char	Renu				[ 5];	/* 반복횟수 													*/
	char	AcctNm				[20];	/* 계좌명														*/
	char	Entr_ch				[20];	/* 예탁금잔액													*/
	char	tdy_repl_amt		[20];	/* 원화대용금액(추가)											*/
	char	repl_use_amt		[20];	/* 원화대용사용금액(추가)										*/
	char	Fut_rsrb_pl			[20];	/* 청산손익														*/
	char	Pure_ote_amt		[20];	/* 평가손익														*/
	char	Fut_trad_fee		[20];	/* 수수료														*/
	char	Dfr_amt				[20];	/* 미수금액														*/
	char	Te_amt				[20];	/* 예탁자산평가액												*/
	char	Open_pos_mgn		[20];	/* 미결제증거금													*/
	char	Ord_mgn				[20];	/* 주문증거금													*/
	char	Trst_mgn			[20];	/* 위탁증거금													*/
	char	Mnt_mgn				[20];	/* 유지증거금													*/
	char	With_psbl_amt		[20];	/* 인출가능금액													*/
	char	krw_with_psbl_amt	[20];	/* 원화인출가능금액												*/
	char	Add_mgn				[20];	/* 추가증거금													*/
	char	Ord_psbl_amt		[20];	/* 주문가능금액													*/
	char	Han_psbl_amt		[20];	/* 환전대기금액													*/
	char	Crc_cd				[ 3];	/* 통화코드														*/
}	Es5615O_st,	* lpEs5615O_st;
#define	SZ_Es5615O	sizeof(Es5615O_st)



//////////////////////////////////////////////////////////////////////////////////
// 시세
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// 종목 Master
typedef struct _Es5501I
{
	ESHeader_st					Head;
	char	FullCode			[32];	/* 종목표준코드													*/
	char	Index				[ 4];	/* 종목 인덱스													*/
}	Es5501I_st, * lpEs5501I_st,
	Es5502I_st, * lpEs5502I_st,
	Es5503I_st, * lpEs5503I_st;
#define	SZ_Es5501I	sizeof( Es5501I_st)
#define	SZ_Es5502I	sizeof( Es5502I_st)
#define	SZ_Es5503I	sizeof( Es5503I_st)

//////////////////////////////////////////////////////////////////////////////////
// 종목 Master
typedef	struct	_Es5501O
{
	ESHeader_st					Head;
	char	FullCode			[32];	/* 종목표준코드													*/
	char	JinbubGb			[ 1];	/* 진법 (0.10진법 1.32진법 2.64진법 3:128진법 4.8진법)			*/
	char	StandeCode			[12];	/* 종목표준코드													*/
	char	HLimitPrice			[20];	/* 상한가														*/
	char	LLimitPrice			[20];	/* 하한가														*/
	char	RemainDays			[ 5];	/* 잔존일수9(3)													*/
	char	LastDate			[ 8];	/* 최종거래일YYYYMMDD											*/
	char	ExpiryDate			[ 8];	/* 만기일    YYYYMMDD											*/
	char	ListedHighPrice		[20];	/* 상장중최고가													*/
	char	ListedHighPriceDate	[ 8];	/* 상장중최고일YYYYMMDD											*/
	char	ListedLowPrice		[20];	/* 상장중최저가													*/
	char	ListedLowPriceDate	[ 8];	/* 상장중최저일YYYYMMDD											*/
	char	ClosePrice_1		[20];	/* 본정산가														*/
	char	ClosePrice_2		[20];	/* 전산종가														*/
	char	Scn_Exch_cd			[ 5];	/* 거래소코드													*/
	char	Scn_Disp_digit 		[10];	/* 표시진법														*/
	char	Scn_Fut_ord_mgn		[20];	/* 선물신규주문증거금											*/
	char	Scn_Ctrt_size   	[20];	/* 계약단위														*/
	char	Scn_Tick_size    	[20];	/* 틱															*/
	char	Scn_Tick_val    	[20];	/* 틱가치														*/
	char	PrevTotQty			[20];	/* 전일 거래량													*/
}	Es5501O_st, * lpEs5501O_st;
#define	SZ_Es5501O	sizeof( Es5501O_st)

//////////////////////////////////////////////////////////////////////////////////
// 종목 시세
typedef	struct	_Es5502O
{
	ESHeader_st					Head;
	char	FullCode  			[32];	/* 종목표준코드													*/
	char	JinbubGb  			[ 1];	/* 진법 (0.10진법 1.32진법 2.64진법 3.128진법 4.8진법)			*/
	char	ISINCd   			[12];	/* 종목표준코드													*/
	char	ClosePrice			[20];	/* 현재가														*/
	char	CmpSign				[ 1];	/* 대비부호
											0.보합 1.상한 2.하한 3.상승 4.하락 5.기세상한
											6.기세하한 7.기세상승 8.기세하락							*/
	char	CmpPrice			[20];	/* 전일대비9(5)V9(2)											*/
	char	CmpRate				[ 8];	/* 등락율9(5)V9(2) 												*/
	char	OpenPrice			[20];	/* 시가 														*/
	char	HighPrice			[20];	/* 고가 														*/
	char	LowPrice			[20];	/* 저가 														*/
	char	ContQty				[20];	/* 체결량 														*/
	char	TotQty 				[20];	/* 거래량 														*/
	char	ClosePrice_1		[20];	/* Close Price 1												*/
}	Es5502O_st, * lpEs5502O_st;
#define	SZ_Es5502O	sizeof( Es5502O_st)

//////////////////////////////////////////////////////////////////////////////////
// 종목 호가
typedef	struct	_Es5503O
{
	ESHeader_st					Head;
	char	FullCode  			[32];	/* 종목표준코드													*/
	char	JinbubGb  			[ 1];	/* 진법 (0.10진법 1.32진법 2.64진법 3.128진법 4.8진법)			*/
	char	ISINCd   			[12];	/* 종목표준코드													*/
	char	Time				[ 8];	/* 입력시간 HH:MM:SS											*/
	char	ClosePrice_1		[20];	/* Close Price 1												*/

	struct	_Es5503OSub
	{
		char	BuyNo			[10];	/* 매수번호														*/
		char	BuyPrice		[20];	/* 매수호가														*/
		char	BuyQty			[20];	/* 매수수량														*/
		char	SellNo			[10];	/* 매도번호														*/
		char	SellPrice		[20];	/* 매도호가														*/
		char	SellQty			[20];	/* 매도수량														*/
	}	Ary	[ 5];

	char	TotSellQty			[20];	/* 매도총호가수량9(6)											*/
	char	TotBuyQty			[20];	/* 매수총호가수량9(6)											*/
	char	TotSellNo			[20];	/* 매도총호가건수9(5)											*/
	char	TotBuyNo			[20];	/* 매수총호가건수9(5)											*/
}	Es5503O_st, * lpEs5503O_st;
#define	SZ_Es5503O	sizeof( Es5503O_st)


//////////////////////////////////////////////////////////////////////////////////
// 종목 체결내역
typedef struct _Es5511I
{
	ESHeader_st					Head;
	char	FullCode			[32];	/* 종목표준코드													*/
	char	Index				[ 4];	/* 종목코드 index												*/
} Es5511I_st, * lpEs5511I_st;
#define	SZ_Es5511I	sizeof( Es5511I_st)

typedef	struct	_Es5511OSub
{
	char	Time				[ 8];	/* 시간(HH:MM:SS)												*/
	char	CmpSign				[ 1];	/* 대비부호
											0.보합 1.상한 2.하한 3.상승 4.하락 5.기세상한
											6.기세하한 7.기세상승 8.기세하락							*/
	char	CmpPrice			[20];	/* 전일대비														*/
	char	ClosePrice			[20];	/* 현재가														*/
	char	CmpRate				[ 8];	/* 등락율														*/
	char	TotQty 				[20];	/* 거래량														*/
	char	ContQty				[20];	/* 체결량														*/
	char	ClosePrice1			[20];	/* Close Price 1												*/
	char	MatchKind			[ 1];	/* 현재가의 호가구분 (+.매수 -.매도)							*/
	char	Date				[ 8];	/* 일자(YYYYMMDD)												*/
}	Es5511OSub_st, * lpEs5511OSub_st;

typedef	struct	_Es5511O
{
	ESHeader_st					Head;
	char	FullCode			[32];	/* 종목표준코드													*/
	char	JinbubGb  			[ 1];	/* 진법 0:10진법 1:32진법										*/
	char	DataCnt				[ 2];	/* Data Count													*/
	Es5511OSub_st		Ary		[20];
}	Es5511O_st, * lpEs5511O_st;
#define	SZ_Es5511O	sizeof( Es5511O_st)


//////////////////////////////////////////////////////////////////////////////////
// 선물 CHART DATA
typedef struct _Es5522I
{
	ESHeader_st					Head;
	char	JongUp				[ 1];	/* '5'로 고정													*/
	char	DataGb				[ 1];	/* 1:일															*/
										/* 2:주															*/
										/* 3:월															*/
										/* 4:tick봉														*/
										/* 5:분봉														*/
	char	DiviGb				[ 1];	/* 종목, 일별시 액면분할 보정 안한다: '0'으로 고정				*/
	char	FullCode			[32];	/* 종목표준코드													*/
	char	Index				[ 4];	/* 코드 index													*/
	char	InxDay				[ 8];	/* 기준일자														*/
	char	DayCnt				[ 4];	/* 일자갯수														*/
	char	Summary				[ 3];	/* tick, 분에서 모으는 단위										*/
	char	Linked				[ 1];	/* 연결선물구분 Y/N(일봉)										*/
	char	JunBonGb			[ 1];	/* 1.전산장 2.본장 (분/틱봉)									*/
} Es5522I_st, * lpEs5522I_st;
#define	SZ_Es5522I	sizeof( Es5522I_st)

typedef struct  _Es5522OSub
{
    char    Date				[ 8];   /* YYYYMMDD														*/
    char    Time				[ 8];   /* 시간(HH:MM:SS)												*/
    char    OpenPrice			[20];   /* 시가 double													*/
    char    HighPrice			[20];   /* 고가 double													*/
    char    LowPrice			[20];   /* 저가 double													*/
    char    ClosePrice			[20];   /* 종가 double													*/
    char    Volume				[20];   /* 거래량 double												*/
}   Es5522OSub_st, * lpEs5522OSub_st;
#define SZ_Es5522OSub   sizeof( Es5522OSub_st)

typedef struct  _Es5522O
{
	ESHeader_st					Head;
	char    FullCode			[32];   /* 종목표준코드													*/
	char    MaxDataCnt			[ 4];   /* 전체일자 갯수												*/
	char    DataCnt				[ 3];   /* 현재송신일자 갯수											*/
	char    TickCnt				[ 2];   /* 마지막봉의 tick 갯수											*/
	char    Today				[ 8];   /* 당영업일(StockDate[0])										*/
	char    nonedata			[ 4];   /* 전일종가 double												*/
	char    DataGb				[ 1];   /* 1:일															*/
								        /* 2:주															*/
								        /* 3:월															*/
								        /* 4:tick봉														*/
								        /* 5:분봉														*/
	char    DayCnt				[ 4];   /* 일자갯수														*/
	char    Summary				[ 3];   /* tick, 분에서 모으는 단위										*/
	char    PrevLast			[20];   /* 전일종가														*/
	Es5522OSub_st		Ary	   [120];
}   Es5522O_st, * lpEs5522O_st;
#define SZ_Es5522O  sizeof( Es5522O_st)


//////////////////////////////////////////////////////////////////////////////////
// 종목 Master 자동 업데이트
typedef	struct	_At0931O
{
	ESHeader_st					Head;
	char	FullCode 			[32];	/* 종목표준코드													*/
	char	JinbubGb  			[ 1];	/* 진법 (0.10진법 1.32진법 2.64진법 3.128진법 4.8진법)			*/
	char	Date				[ 8];	/* 일자(YYYYMMDD) 												*/
	char	Time				[ 8];	/* 시간(HH:MM:SS) 												*/
	char	ClosePrice			[20];	/* 현재가														*/
	char	CmpSign				[ 1];	/* 대비부호
											0.보합 1.상한 2.하한 3.상승 4.하락 5.기세상한
											6.기세하한 7.기세상승 8.기세하락							*/
	char	CmpPrice			[20];	/* 전일대비														*/
	char	CmpRate				[ 8];	/* 등락율9(5)V9(2)												*/
	char	OpenPrice			[20];	/* 시가 														*/
	char	HighPrice			[20];	/* 고가 														*/
	char	LowPrice			[20];	/* 저가 														*/
	char	ContQty				[20];	/* 체결량 														*/
	char	TotQty 				[20];	/* 거래량 														*/
	char	ClosePrice_1		[20];	/* Close Price 1												*/
	char	ListedHighPrice		[20];	/* 상장중최고가													*/
	char	ListedHighPriceDate	[ 8];	/* 상장중최고일YYYYMMDD											*/
	char	ListedLowPrice		[20];	/* 상장중최저가													*/
	char	ListedLowPriceDate	[ 8];	/* 상장중최저일YYYYMMDD											*/
}	At0931O_st, * lpAt0931O_st;
#define	SZ_At0931O	sizeof( At0931O_st)

//////////////////////////////////////////////////////////////////////////////////
// 종목 호가 자동 업데이트
typedef	struct	_At0932O
{
	ESHeader_st					Head;
	char	FullCode 			[32];	/* 종목표준코드													*/
	char	JinbubGb  			[ 1];	/* 진법 (0.10진법 1.32진법 2.64진법 3.128진법 4.8진법)			*/
	char	Time				[ 8];	/* 시간(HH:MM:SS)												*/
	char	ClosePrice_1		[20];	/* Close Price 1												*/

	struct	_At0932O_Arr
	{
		char	BuyNo			[10];	/* 매수번호														*/
		char	BuyPrice		[20];	/* 매수호가														*/
		char	BuyQty			[20];	/* 매수수량														*/
		char	SellNo			[10];	/* 매도번호														*/
		char	SellPrice		[20];	/* 매도호가														*/
		char	SellQty			[20];	/* 매도수량														*/
	}	Ary	[ 5];

	char	TotSellQty			[20];	/* 매도총호가수량9(6)											*/
	char	TotBuyQty			[20];	/* 매수총호가수량9(6)											*/
	char	TotSellNo			[20];	/* 매도총호가건수9(5)											*/
	char	TotBuyNo			[20];	/* 매수총호가건수9(5)											*/
}	At0932O_st, * lpAt0932O_st;
#define	SZ_At0932O	sizeof( At0932O_st)

//////////////////////////////////////////////////////////////////////////////////
// 종목 시세 자동 업데이트
typedef	struct	_At0933O
{
	ESHeader_st					Head;
	char	FullCode	 		[32];	/* 종목표준코드 												*/
	char	JinbubGb  			[ 1];	/* 진법 (0.10진법 1.32진법 2.64진법 3.128진법 4.8진법)			*/
	char	Time				[ 8];	/* 시간(HH:MM:SS)												*/
	char	CmpSign				[ 1];	/* 대비부호
											0.보합 1.상한 2.하한 3.상승 4.하락 5.기세상한
											6.기세하한 7.기세상승 8.기세하락							*/
	char	CmpPrice			[20];	/* 전일대비														*/
	char	ClosePrice			[20];	/* 현재가														*/
	char	CmpRate				[ 8];	/* 등락율9(5)V9(2)												*/
	char	TotQty 				[20];	/* 거래량 														*/
	char	ContQty				[20];	/* 체결량 														*/
	char	MatchKind			[ 1];	/* 현재가의 호가구분 (+.매수 -.매도)							*/
	char	Date				[ 8];	/* 일자(YYYYMMDD) 												*/
	char	OpenPrice			[20];	/* 시가 														*/
	char	HighPrice			[20];	/* 고가 														*/
	char	LowPrice			[20];	/* 저가 														*/
	char	BuyPrice			[20];	/* 매수호가														*/
	char	SellPrice			[20];	/* 매도호가														*/
	char	MarketFlag			[ 1];	/* 장구분 0.본장 1.전산장										*/
	char	DecLen				[ 5];	/* 종목 소숫점 정보												*/
}	At0933O_st, * lpAt0933O_st;
#define	SZ_At0933O	sizeof( At0933O_st)

//////////////////////////////////////////////////////////////////////////////////
// 주문/체결 자동 업데이트
typedef	struct	_At0985O
{
	ESHeader_st					Head;
	char	Account				[11]; 	/* 계좌번호														*/
	char	ReplyType			[ 1];	/* 응답유형 
											1.주문접수 2.체결 3.정정확인 4.취소확인 
											5.신규거부 6.정정거부 7.취소거부 0.원장접수					*/
	char	FullCode			[32];	/* 종목코드 (원장접수일때 표준코드, 그외 단축코드)				*/
	char	Side				[ 1];	/* 매수/매도구분 (1.매수 2.매도)								*/
	char	Qty					[20];	/* 주문수량														*/
	char	Modality			[ 1];	/* 가격조건	(1.지정가 2.시장가)									*/
	char	Price				[20];	/* 주문가격														*/
	char	Validity			[ 1];	/* 체결조건 (1.FAS 2.FOK 3.FAK 4.GTC 5.GTD)						*/
	char	StopLossLimit		[20];	/* stop order 지정가격											*/
	char	ExecPrice			[20];	/* 체결가격														*/
	char	ExecQty				[20];	/* 체결수량														*/
	char	RemainQty			[20];	/* 주문잔량														*/
	char	Ord_no				[ 5];	/* 주문번호														*/
	char	Orig_ord_no			[ 5];	/* 원주문번호													*/
	char	TradeTime			[ 8];	/* 주문확인,체결,거부 시간										*/
	char	ExecAmt				[20];	/* 체결금액														*/
	char	ORD_TP 				[ 1];	/* 주문구분	(ReplyType==0일때 1.신규 2.정정 3.취소)				*/
	char    Trd_no				[ 5];   /* 체결번호														*/
	char    Trd_date			[ 8];   /* 체결일자														*/
	char	Rsrb_q				[10];	/* 청산가능수량													*/
	char	Open_q				[10];	/* 잔고수량														*/
	char	Open_tp				[ 1];	/* 잔고포지션구분 (1.매수 2.매도)								*/
	char    Ordp_q				[10];   /* 주문가능수량													*/
}	At0985O_st, * lpAt0985O_st;
#define	SZ_At0985O	sizeof( At0985O_st)


#endif