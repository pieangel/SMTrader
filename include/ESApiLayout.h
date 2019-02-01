//////////////////////////////////////////////////////////////////////
// ESApiLayout.h	: ���� ���α׷����� ����ϴ� ����
//
//////////////////////////////////////////////////////////////////////

#if !defined(__COMMON_ESAPILAYOUT_FORMAT_H__)
#define __COMMON_ESAPILAYOUT_FORMAT_H__

#include <afxtempl.h>

enum _tagErrorCode
{
	ERR_NONE			= 0,
	ERR_AUTOKEY,			// ���� �ڵ�������Ʈ key ���Է�
	ERR_USERID,				// ���� ���̵� ���Է�
	ERR_USERPASS,			// ���� ��й�ȣ ���Է�
	ERR_CERTPASS,			// ���� ���� ��й�ȣ ���Է� (����� ����)
	ERR_SVRSENDDATA,		// ���� ���� ����
	ERR_SVRCONNECT,			// ���� ���� ����
	ERR_SVRNOCONNECT,		// ���� ������ ����
	ERR_CONNECT,			// ���� ���� ����
	ERR_CERTSET,			// ���� ������ �������� ����
	ERR_DLLNOEXIST,			// signkorea dll ���� ���
	ERR_TRCODE,				// ������ ���� TR��ȣ
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
// ������ ����
typedef	struct	_ESAccntInfo
{
	char	AccNo				[11];	/* ���¹�ȣ														*/
	char	AccNm				[30];	/* ���¸�														*/
}	ESAccntInfo_st, * lpESAccntInfo_st;
#define	SZ_ESAccntInfo	sizeof(ESAccntInfo_st)

typedef CArray < ESAccntInfo_st, ESAccntInfo_st > CArrayAcct;

typedef	struct _ESHeader_st
{
	char	WinId				[10];	/* Client WINDOWS ID										 	*/
	char	PCKey				[ 1];	/* Request key													*/
	char	ErrCode				[ 4];	/* Error Code
											�����̳� 0000�̸� ����, �׿� ����, _ESErr_st �� ����)		*/
	char	NextKind			[ 1];	/* ���� ���� (0.��������, 1.���� ����)							*/
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
	char	AutoKey				[32];	/* ������ ��� ���¹�ȣ, �ü� ��� ����ǥ���ڵ�					*/
} ESAutoKey_st, *lpESAutoKey_st;
#define SZ_ESAutoKey	sizeof(ESAutoKey_st)


//---------------------------------------------------------------------------------------------------------------------------------
//
// �ؿܿ� API ����
//
//---------------------------------------------------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// ���� ����
typedef	struct	_ESCodeInfo
{
	char	FullCode			[32];	/* ���� Full code												*/
	char	ShortCode			[ 5];	/* ���� Short code												*/
	char	Index				[ 4];	/* ���� �ε���													*/
	char	CodeName			[32];	/* �����														*/
	char	DecimalInfo			[ 5];	/* ��ǰ���ݿ����ѼҼ�������										*/
	char	TickSize			[20];	/* TICK SIZE �Ҽ���7�ڸ�����									*/
	char	TickValue			[20];	/* �ּҰ��ݺ����ݾ� �Ҽ���7�ڸ�����								*/
}	ESCodeInfo_st, * lpESCodeInfo_st;
#define	SZ_ESCodeInfo	sizeof(ESCodeInfo_st)

typedef CArray < ESCodeInfo_st, ESCodeInfo_st >	CArrayCode;

//////////////////////////////////////////////////////////////////////////////////
// ���� ���� ��Ŷ
//////////////////////////////////////////////////////////////////////////////////

// ���� ����
#define ESID_5601				5601	// �ű��ֹ�
#define ESID_5602				5602	// �����ֹ�
#define ESID_5603				5603	// ����ֹ�
#define ESID_5611				5611	// ��ü��
#define ESID_5612				5612	// ���ܰ�
#define ESID_5614				5614	// ���º� �ֹ�ü����Ȳ
#define ESID_5615				5615	// ��Ź�ڻ�� ���ű�

// �ü� ����
#define ESID_5501				5501	// ���� Master
#define ESID_5502				5502	// ���� �ü�
#define ESID_5503				5503	// ���� ȣ��
#define ESID_5511				5511	// ���� ü�᳻��
#define	ESID_5522				5522	//���� CHART DATA

// �ڵ�������Ʈ
#define AUTO_0931				 931	// ���� Master �ǽð�
#define AUTO_0932				 932	// ���� ȣ�� �ǽð�
#define AUTO_0933				 933	// ���� �ü� �ǽð�
#define AUTO_0985				 985	// �ܰ�/ü�� �ǽð�

//////////////////////////////////////////////////////////////////////////////////
// �ֹ� ���� (�ű�, ����, ���)
typedef struct _Es5601I
{
	ESHeader_st					Head;
	char	Account				[11];	/* ���¹�ȣ														*/
	char	Pass				[ 8];	/* ��й�ȣ														*/
	char	Order_Type			[ 1];	/* �ֹ�����	(1.�ű��ֹ� 2.�����ֹ� 3.����ֹ�)					*/
	char	ShortCode			[32];	/* ��������ڵ�													*/
	char	BuySell_Type		[ 1];	/* �ŵ��ż����� (1.�ż� 2.�ŵ�)									*/
	char    Price_Type			[ 1];	/* ��������	(1.������ 2.���尡)									*/
	char	Trace_Type			[ 1];	/* ü������ (���尡�ϰ��(3) �������ϰ��(1))					*/
	char    Order_Price			[20];	/* �ֹ�����														*/
	char    Order_Volume		[ 5];	/* �ֹ�����														*/
	char	Order_Org_No		[ 5];	/* ���ֹ���ȣ (����/��ҽø�)									*/
	char    Order_Comm_Type		[ 1];	/* ����ֹ�����													*/
	char	Stop_Type			[ 1];	/* �ֹ��������� (1.�Ϲ��ֹ� 2.STOP�ֹ�)							*/
	char	Stop_Price			[20];	/* STOP�ֹ����� (STOP�ֹ��̸� �Է� �ƴϸ� 0 ����)				*/
	char	Oper_Type			[ 1];	/* �ֹ�����	(1.�ű��ֹ� 2.�����ֹ� 3.����ֹ�)					*/
}	Es5601I_st, *lpEs5601I_st;
#define	SZ_Es5601I	sizeof(Es5601I_st)
	
typedef struct _Es5601O
{
	ESHeader_st					Head;
	char	Order_No  			[ 5];	/* �ֹ���ȣ														*/
	char    Order_Org_No		[ 5];	/* ���ֹ���ȣ													*/
	char	Account				[11];	/* ���¹�ȣ														*/
	char	Order_Type			[ 1];	/* �ֹ�����	(1.�ű��ֹ� 2.�����ֹ� 3.����ֹ�)					*/
	char	ShortCode			[32];	/* ��������ڵ�														*/
	char	BuySell_Type		[ 1];	/* �ŵ��ż����� (1.�ż� 2.�ŵ�)									*/
	char    Order_Volume		[ 5];	/* �ֹ�����														*/
	char    Order_Price			[20];	/* �ֹ�����														*/
	char    Price_Type			[ 1];	/* ��������	(1.������ 2.���尡)									*/
	char	Trade_Type			[ 1];	/* ü������ (���尡�ϰ��(3) �������ϰ��(1))					*/
	char	Stop_Price			[20];	/* STOP�ֹ�����													*/
}	Es5601O_st,	* lpEs5601O_st;
#define	SZ_Es5601O	sizeof(Es5601O_st)


//////////////////////////////////////////////////////////////////////////////////
// ���� ��ü�� ��ȸ
typedef struct _Es5611I
{
	ESHeader_st					Head;
	char	Account				[11];	/* ���¹�ȣ														*/
	char	Pass				[ 8];	/* ��й�ȣ														*/
	char	Trd_gb				[ 1];	/* ü�ᱸ�� (0:��ü 1:ü��2:��ü��)								*/
	char	Base_dt				[ 8];	/* �ֹ�����														*/
	char	Gubn				[ 1];	/* ��ȸ���� (1:���� 2:����)										*/
}	Es5611I_st,	* lpEs5611I_st;
#define	SZ_Es5611I	sizeof(Es5611I_st)

typedef struct _Es5611Sub
{
	char	Ord_No				[ 5];	/* �ֹ��ֹ�ȣ													*/
	char	Org_ord_No			[ 5];	/* ���ֹ��ι�ȣ													*/
	char	Trd_cond			[ 1];	/* ü������ (1.FAS 2.FOK 3.FAK)									*/
	char	ShortCode			[32];	/* ��������ڵ�													*/
	char	Bysl_tp				[ 1];	/* �Ÿű����ڵ�
											1.�ż� 2.�ŵ� 3.�ż�����
											4.�ŵ����� 5.�ż���� 6.�ŵ���� ' '.��Ÿ					*/
	char	Prce_tp				[ 1];	/* ��������	(1.������ 2.���尡)									*/
	char	Ord_q				[ 5];	/* �ֹ�����														*/
	char	Ord_p				[20];	/* �ֹ����� or ü������											*/
	char	Trd_q				[ 5];	/* ü�����														*/
	char	Mcg_q				[ 5];	/* ��ü�����													*/
	char	Ord_tp				[ 1];	/* �ֹ����� (1.�ű� 2.���� 3.���)								*/
	char	Stop_p				[20];	/* STOP�ֹ�����													*/
	char	Ex_ord_tm			[ 6];	/* �ֹ��ð�														*/
	char	Proc_stat			[ 1];	/* �ֹ�ó������ (0.�������� 1.�ŷ������� 2.�����ź� 3.FEP�ź�)	*/
	char	Account				[11];	/* ���¹�ȣ														*/
}	Es5611Sub_st, *lpEs5611Sub_st;

typedef struct _Es5611O
{
	ESHeader_st					Head;
	char	Renu				[ 5];	/* �ݺ�Ƚ��														*/
	char	Account				[11];	/* ���¹�ȣ														*/
	char	AcctNm				[20];	/* ���¸�														*/
	char	Dtno				[ 5];	/* �ݺ�Ƚ��														*/
	Es5611Sub_st Sub			[12];	/* ü�� ����Ÿ													*/
}	Es5611O_st,	*lpEs5611O_st;
#define	SZ_Es5611O	sizeof(Es5611O_st)


//////////////////////////////////////////////////////////////////////////////////
// ���� ���ܰ� ��ȸ
typedef struct _Es5612I
{
	ESHeader_st					Head;
	char	Account				[11];	/* ���¹�ȣ														*/
	char	Pass				[ 8];	/* ��й�ȣ														*/
}	Es5612I_st,	* lpEs5612I_st;
#define	SZ_Es5612I	sizeof(Es5612I_st)

typedef struct _Es5612Sub
{
	char	Base_dt				[ 8];	/* ��������														*/
	char	FullCode			[32];	/* ����ǥ���ڵ�													*/
	char	Bysl_tp				[ 1];	/* �Ÿű���	(1.�ż� 2.�ŵ�)										*/
	char	Trd_no				[ 5];	/* ü���ȣ														*/
	char	Open_q				[10];	/* �̰�������													*/
	char	Avgt_p				[20];	/* ��հ�														*/
	char	Curr_p				[20];	/* ���簡 														*/
	char	Open_pl				[20];	/* �򰡼���														*/
	char	Rsrb_q				[10];	/* û�갡�ɼ���													*/
	char	Trd_amt				[20];	/* ü��ݾ�														*/
	char	Account				[11];	/* ���¹�ȣ														*/
}	Es5612Sub_st, *lpEs5612Sub_st;

typedef struct _Es5612O
{
	ESHeader_st					Head;
	char	Renu				[ 5];	/* �ݺ�Ƚ��														*/
	char	Account				[11];	/* ���¹�ȣ														*/
	char	AcctNm				[20];	/* ���¸�														*/
	char	Dtno				[ 5];	/* �ݺ�Ƚ��														*/
	Es5612Sub_st	Sub			[16];
}	Es5612O_st,	*lpEs5612O_st;
#define	SZ_Es5612O	sizeof(Es5612O_st)


//////////////////////////////////////////////////////////////////////////////////
// (�ؿܼ���)���º� �ֹ�ü����Ȳ
//
typedef struct _Es5614I
{
	ESHeader_st					Head;
	char	Account				[11];	/* ���¹�ȣ														*/
	char	Pass				[ 8];	/* ��й�ȣ														*/
	char	Trd_gb				[ 1];	/* ü�ᱸ��
											0.��ü 1.ü�� 2.��ü�� 3.�������� 4.������� 5.�ֹ��ź�
											6.ü��+��ü��												*/
	char	Base_dt				[ 8];	/* �ֹ�����														*/
	char	Gubn				[ 1];	/* ��ȸ���� (1.���� 2.����)										*/
	char	Ord_no				[ 5];	/* �����ֹ���ȣ													*/
	char	Dtno				[ 5];	/* ��û����														*/
}	Es5614I_st,	* lpEs5614I_st;
#define	SZ_Es5614I	sizeof(Es5614I_st)

typedef struct _Es5614Sub
{
	char	Ord_no				[ 5];	/* �ֹ��ֹ�ȣ													*/
	char	Orig_ord_no			[ 5];	/* ���ֹ��ι�ȣ													*/
	char	Trd_cond			[ 1];	/* ü������	(1.FAS 2.FOK 3.FAK)									*/
	char	Series				[32];	/* �����ȣ														*/
	char	Bysl_tp				[ 1];	/* �Ÿű����ڵ�
											1.�ż� 2.�ŵ� 3.�ż����� 4.�ŵ����� 5.�ż���� 
											6.�ŵ���� ' '.��Ÿ											*/
	char	Prce_tp				[ 1];	/* �������� (1.������ 2.���尡)									*/
	char	Ord_q				[ 5];	/* �ֹ�����														*/
	char	Ord_p				[20];	/* �ֹ�����														*/
	char	Trd_q				[ 5];	/* ü����� ��													*/
	char	Trd_p				[20];	/* ü�ᰡ��	��հ�												*/
										/* ü������� ü�ᰡ�� SPACE									*/
	char	Trd_no				[ 5];	/* ü���ȣ	SPACE												*/
	char	Ex_trd_tm			[ 6];	/* ü��ð�	SPACE												*/
	char	Mcg_q				[ 5];	/* ��ü�����													*/
	char	Ord_tp				[ 1];	/* �ֹ����� (1.�ű��ֹ� 2.�����ֹ� 3.����ֹ�)					*/
	char	Stop_p				[20];	/* STOP�ֹ�����													*/
	char	Ex_ord_tm			[ 6];	/* �ֹ��ð�														*/
	char	Proc_stat			[ 1];	/* �ֹ�ó������	
											'0'.������ '1'.��������
											'2'.�����ź� '3'.FEP�����ź�								*/
	char	Trd_cnt				[ 1];	/* ü��Ǽ� (1.�ѹ� 2.������)									*/
	char	Trd_gb				[ 1];	/* ü�ᱸ�� 
											1.ü�� 2.��ü�� 3.�������� 4.������� 5.�ֹ��ź�
											6.ü��+��ü��												*/
	char	Account				[11];	/* ���¹�ȣ														*/
}	Es5614Sub_st, *lpEs5614Sub_st;

typedef struct _Es5614O
{
	ESHeader_st					Head;
	char	Renu				[ 5];	/* �ݺ�Ƚ��														*/
	char	Account				[11];	/* ���¹�ȣ														*/
	char	Acct_nm				[20];	/* ���¸�														*/
	char	Dtno				[ 5];	/* �ݺ�Ƚ��														*/
	Es5614Sub_st		Sub		[23];
}	Es5614O_st,	*lpEs5614O_st;
#define	SZ_Es5614O	sizeof(Es5614O_st)


//////////////////////////////////////////////////////////////////////////////////
// ��Ź�ڻ�� ���ű�
typedef struct _Es5615I
{
	ESHeader_st					Head;
	char	Account				[11];	/* ���¹�ȣ														*/
	char	Pass				[ 8];	/* ��й�ȣ														*/
	char	Crc_cd				[ 3];	/* ��ȭ�ڵ�	(USD �� �⺻���� ���)								*/
}	Es5615I_st,	* lpEs5615I_st;
#define	SZ_Es5615I	sizeof(Es5615I_st)

typedef struct _Es5615O
{
	ESHeader_st					Head;
	char	Renu				[ 5];	/* �ݺ�Ƚ�� 													*/
	char	AcctNm				[20];	/* ���¸�														*/
	char	Entr_ch				[20];	/* ��Ź���ܾ�													*/
	char	tdy_repl_amt		[20];	/* ��ȭ���ݾ�(�߰�)											*/
	char	repl_use_amt		[20];	/* ��ȭ�����ݾ�(�߰�)										*/
	char	Fut_rsrb_pl			[20];	/* û�����														*/
	char	Pure_ote_amt		[20];	/* �򰡼���														*/
	char	Fut_trad_fee		[20];	/* ������														*/
	char	Dfr_amt				[20];	/* �̼��ݾ�														*/
	char	Te_amt				[20];	/* ��Ź�ڻ��򰡾�												*/
	char	Open_pos_mgn		[20];	/* �̰������ű�													*/
	char	Ord_mgn				[20];	/* �ֹ����ű�													*/
	char	Trst_mgn			[20];	/* ��Ź���ű�													*/
	char	Mnt_mgn				[20];	/* �������ű�													*/
	char	With_psbl_amt		[20];	/* ���Ⱑ�ɱݾ�													*/
	char	krw_with_psbl_amt	[20];	/* ��ȭ���Ⱑ�ɱݾ�												*/
	char	Add_mgn				[20];	/* �߰����ű�													*/
	char	Ord_psbl_amt		[20];	/* �ֹ����ɱݾ�													*/
	char	Han_psbl_amt		[20];	/* ȯ�����ݾ�													*/
	char	Crc_cd				[ 3];	/* ��ȭ�ڵ�														*/
}	Es5615O_st,	* lpEs5615O_st;
#define	SZ_Es5615O	sizeof(Es5615O_st)



//////////////////////////////////////////////////////////////////////////////////
// �ü�
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// ���� Master
typedef struct _Es5501I
{
	ESHeader_st					Head;
	char	FullCode			[32];	/* ����ǥ���ڵ�													*/
	char	Index				[ 4];	/* ���� �ε���													*/
}	Es5501I_st, * lpEs5501I_st,
	Es5502I_st, * lpEs5502I_st,
	Es5503I_st, * lpEs5503I_st;
#define	SZ_Es5501I	sizeof( Es5501I_st)
#define	SZ_Es5502I	sizeof( Es5502I_st)
#define	SZ_Es5503I	sizeof( Es5503I_st)

//////////////////////////////////////////////////////////////////////////////////
// ���� Master
typedef	struct	_Es5501O
{
	ESHeader_st					Head;
	char	FullCode			[32];	/* ����ǥ���ڵ�													*/
	char	JinbubGb			[ 1];	/* ���� (0.10���� 1.32���� 2.64���� 3:128���� 4.8����)			*/
	char	StandeCode			[12];	/* ����ǥ���ڵ�													*/
	char	HLimitPrice			[20];	/* ���Ѱ�														*/
	char	LLimitPrice			[20];	/* ���Ѱ�														*/
	char	RemainDays			[ 5];	/* �����ϼ�9(3)													*/
	char	LastDate			[ 8];	/* �����ŷ���YYYYMMDD											*/
	char	ExpiryDate			[ 8];	/* ������    YYYYMMDD											*/
	char	ListedHighPrice		[20];	/* �������ְ�													*/
	char	ListedHighPriceDate	[ 8];	/* �������ְ���YYYYMMDD											*/
	char	ListedLowPrice		[20];	/* ������������													*/
	char	ListedLowPriceDate	[ 8];	/* ������������YYYYMMDD											*/
	char	ClosePrice_1		[20];	/* �����갡														*/
	char	ClosePrice_2		[20];	/* ��������														*/
	char	Scn_Exch_cd			[ 5];	/* �ŷ����ڵ�													*/
	char	Scn_Disp_digit 		[10];	/* ǥ������														*/
	char	Scn_Fut_ord_mgn		[20];	/* �����ű��ֹ����ű�											*/
	char	Scn_Ctrt_size   	[20];	/* ������														*/
	char	Scn_Tick_size    	[20];	/* ƽ															*/
	char	Scn_Tick_val    	[20];	/* ƽ��ġ														*/
	char	PrevTotQty			[20];	/* ���� �ŷ���													*/
}	Es5501O_st, * lpEs5501O_st;
#define	SZ_Es5501O	sizeof( Es5501O_st)

//////////////////////////////////////////////////////////////////////////////////
// ���� �ü�
typedef	struct	_Es5502O
{
	ESHeader_st					Head;
	char	FullCode  			[32];	/* ����ǥ���ڵ�													*/
	char	JinbubGb  			[ 1];	/* ���� (0.10���� 1.32���� 2.64���� 3.128���� 4.8����)			*/
	char	ISINCd   			[12];	/* ����ǥ���ڵ�													*/
	char	ClosePrice			[20];	/* ���簡														*/
	char	CmpSign				[ 1];	/* ����ȣ
											0.���� 1.���� 2.���� 3.��� 4.�϶� 5.�⼼����
											6.�⼼���� 7.�⼼��� 8.�⼼�϶�							*/
	char	CmpPrice			[20];	/* ���ϴ��9(5)V9(2)											*/
	char	CmpRate				[ 8];	/* �����9(5)V9(2) 												*/
	char	OpenPrice			[20];	/* �ð� 														*/
	char	HighPrice			[20];	/* �� 														*/
	char	LowPrice			[20];	/* ���� 														*/
	char	ContQty				[20];	/* ü�ᷮ 														*/
	char	TotQty 				[20];	/* �ŷ��� 														*/
	char	ClosePrice_1		[20];	/* Close Price 1												*/
}	Es5502O_st, * lpEs5502O_st;
#define	SZ_Es5502O	sizeof( Es5502O_st)

//////////////////////////////////////////////////////////////////////////////////
// ���� ȣ��
typedef	struct	_Es5503O
{
	ESHeader_st					Head;
	char	FullCode  			[32];	/* ����ǥ���ڵ�													*/
	char	JinbubGb  			[ 1];	/* ���� (0.10���� 1.32���� 2.64���� 3.128���� 4.8����)			*/
	char	ISINCd   			[12];	/* ����ǥ���ڵ�													*/
	char	Time				[ 8];	/* �Է½ð� HH:MM:SS											*/
	char	ClosePrice_1		[20];	/* Close Price 1												*/

	struct	_Es5503OSub
	{
		char	BuyNo			[10];	/* �ż���ȣ														*/
		char	BuyPrice		[20];	/* �ż�ȣ��														*/
		char	BuyQty			[20];	/* �ż�����														*/
		char	SellNo			[10];	/* �ŵ���ȣ														*/
		char	SellPrice		[20];	/* �ŵ�ȣ��														*/
		char	SellQty			[20];	/* �ŵ�����														*/
	}	Ary	[ 5];

	char	TotSellQty			[20];	/* �ŵ���ȣ������9(6)											*/
	char	TotBuyQty			[20];	/* �ż���ȣ������9(6)											*/
	char	TotSellNo			[20];	/* �ŵ���ȣ���Ǽ�9(5)											*/
	char	TotBuyNo			[20];	/* �ż���ȣ���Ǽ�9(5)											*/
}	Es5503O_st, * lpEs5503O_st;
#define	SZ_Es5503O	sizeof( Es5503O_st)


//////////////////////////////////////////////////////////////////////////////////
// ���� ü�᳻��
typedef struct _Es5511I
{
	ESHeader_st					Head;
	char	FullCode			[32];	/* ����ǥ���ڵ�													*/
	char	Index				[ 4];	/* �����ڵ� index												*/
} Es5511I_st, * lpEs5511I_st;
#define	SZ_Es5511I	sizeof( Es5511I_st)

typedef	struct	_Es5511OSub
{
	char	Time				[ 8];	/* �ð�(HH:MM:SS)												*/
	char	CmpSign				[ 1];	/* ����ȣ
											0.���� 1.���� 2.���� 3.��� 4.�϶� 5.�⼼����
											6.�⼼���� 7.�⼼��� 8.�⼼�϶�							*/
	char	CmpPrice			[20];	/* ���ϴ��														*/
	char	ClosePrice			[20];	/* ���簡														*/
	char	CmpRate				[ 8];	/* �����														*/
	char	TotQty 				[20];	/* �ŷ���														*/
	char	ContQty				[20];	/* ü�ᷮ														*/
	char	ClosePrice1			[20];	/* Close Price 1												*/
	char	MatchKind			[ 1];	/* ���簡�� ȣ������ (+.�ż� -.�ŵ�)							*/
	char	Date				[ 8];	/* ����(YYYYMMDD)												*/
}	Es5511OSub_st, * lpEs5511OSub_st;

typedef	struct	_Es5511O
{
	ESHeader_st					Head;
	char	FullCode			[32];	/* ����ǥ���ڵ�													*/
	char	JinbubGb  			[ 1];	/* ���� 0:10���� 1:32����										*/
	char	DataCnt				[ 2];	/* Data Count													*/
	Es5511OSub_st		Ary		[20];
}	Es5511O_st, * lpEs5511O_st;
#define	SZ_Es5511O	sizeof( Es5511O_st)


//////////////////////////////////////////////////////////////////////////////////
// ���� CHART DATA
typedef struct _Es5522I
{
	ESHeader_st					Head;
	char	JongUp				[ 1];	/* '5'�� ����													*/
	char	DataGb				[ 1];	/* 1:��															*/
										/* 2:��															*/
										/* 3:��															*/
										/* 4:tick��														*/
										/* 5:�к�														*/
	char	DiviGb				[ 1];	/* ����, �Ϻ��� �׸���� ���� ���Ѵ�: '0'���� ����				*/
	char	FullCode			[32];	/* ����ǥ���ڵ�													*/
	char	Index				[ 4];	/* �ڵ� index													*/
	char	InxDay				[ 8];	/* ��������														*/
	char	DayCnt				[ 4];	/* ���ڰ���														*/
	char	Summary				[ 3];	/* tick, �п��� ������ ����										*/
	char	Linked				[ 1];	/* ���ἱ������ Y/N(�Ϻ�)										*/
	char	JunBonGb			[ 1];	/* 1.������ 2.���� (��/ƽ��)									*/
} Es5522I_st, * lpEs5522I_st;
#define	SZ_Es5522I	sizeof( Es5522I_st)

typedef struct  _Es5522OSub
{
    char    Date				[ 8];   /* YYYYMMDD														*/
    char    Time				[ 8];   /* �ð�(HH:MM:SS)												*/
    char    OpenPrice			[20];   /* �ð� double													*/
    char    HighPrice			[20];   /* �� double													*/
    char    LowPrice			[20];   /* ���� double													*/
    char    ClosePrice			[20];   /* ���� double													*/
    char    Volume				[20];   /* �ŷ��� double												*/
}   Es5522OSub_st, * lpEs5522OSub_st;
#define SZ_Es5522OSub   sizeof( Es5522OSub_st)

typedef struct  _Es5522O
{
	ESHeader_st					Head;
	char    FullCode			[32];   /* ����ǥ���ڵ�													*/
	char    MaxDataCnt			[ 4];   /* ��ü���� ����												*/
	char    DataCnt				[ 3];   /* ����۽����� ����											*/
	char    TickCnt				[ 2];   /* ���������� tick ����											*/
	char    Today				[ 8];   /* �翵����(StockDate[0])										*/
	char    nonedata			[ 4];   /* �������� double												*/
	char    DataGb				[ 1];   /* 1:��															*/
								        /* 2:��															*/
								        /* 3:��															*/
								        /* 4:tick��														*/
								        /* 5:�к�														*/
	char    DayCnt				[ 4];   /* ���ڰ���														*/
	char    Summary				[ 3];   /* tick, �п��� ������ ����										*/
	char    PrevLast			[20];   /* ��������														*/
	Es5522OSub_st		Ary	   [120];
}   Es5522O_st, * lpEs5522O_st;
#define SZ_Es5522O  sizeof( Es5522O_st)


//////////////////////////////////////////////////////////////////////////////////
// ���� Master �ڵ� ������Ʈ
typedef	struct	_At0931O
{
	ESHeader_st					Head;
	char	FullCode 			[32];	/* ����ǥ���ڵ�													*/
	char	JinbubGb  			[ 1];	/* ���� (0.10���� 1.32���� 2.64���� 3.128���� 4.8����)			*/
	char	Date				[ 8];	/* ����(YYYYMMDD) 												*/
	char	Time				[ 8];	/* �ð�(HH:MM:SS) 												*/
	char	ClosePrice			[20];	/* ���簡														*/
	char	CmpSign				[ 1];	/* ����ȣ
											0.���� 1.���� 2.���� 3.��� 4.�϶� 5.�⼼����
											6.�⼼���� 7.�⼼��� 8.�⼼�϶�							*/
	char	CmpPrice			[20];	/* ���ϴ��														*/
	char	CmpRate				[ 8];	/* �����9(5)V9(2)												*/
	char	OpenPrice			[20];	/* �ð� 														*/
	char	HighPrice			[20];	/* �� 														*/
	char	LowPrice			[20];	/* ���� 														*/
	char	ContQty				[20];	/* ü�ᷮ 														*/
	char	TotQty 				[20];	/* �ŷ��� 														*/
	char	ClosePrice_1		[20];	/* Close Price 1												*/
	char	ListedHighPrice		[20];	/* �������ְ�													*/
	char	ListedHighPriceDate	[ 8];	/* �������ְ���YYYYMMDD											*/
	char	ListedLowPrice		[20];	/* ������������													*/
	char	ListedLowPriceDate	[ 8];	/* ������������YYYYMMDD											*/
}	At0931O_st, * lpAt0931O_st;
#define	SZ_At0931O	sizeof( At0931O_st)

//////////////////////////////////////////////////////////////////////////////////
// ���� ȣ�� �ڵ� ������Ʈ
typedef	struct	_At0932O
{
	ESHeader_st					Head;
	char	FullCode 			[32];	/* ����ǥ���ڵ�													*/
	char	JinbubGb  			[ 1];	/* ���� (0.10���� 1.32���� 2.64���� 3.128���� 4.8����)			*/
	char	Time				[ 8];	/* �ð�(HH:MM:SS)												*/
	char	ClosePrice_1		[20];	/* Close Price 1												*/

	struct	_At0932O_Arr
	{
		char	BuyNo			[10];	/* �ż���ȣ														*/
		char	BuyPrice		[20];	/* �ż�ȣ��														*/
		char	BuyQty			[20];	/* �ż�����														*/
		char	SellNo			[10];	/* �ŵ���ȣ														*/
		char	SellPrice		[20];	/* �ŵ�ȣ��														*/
		char	SellQty			[20];	/* �ŵ�����														*/
	}	Ary	[ 5];

	char	TotSellQty			[20];	/* �ŵ���ȣ������9(6)											*/
	char	TotBuyQty			[20];	/* �ż���ȣ������9(6)											*/
	char	TotSellNo			[20];	/* �ŵ���ȣ���Ǽ�9(5)											*/
	char	TotBuyNo			[20];	/* �ż���ȣ���Ǽ�9(5)											*/
}	At0932O_st, * lpAt0932O_st;
#define	SZ_At0932O	sizeof( At0932O_st)

//////////////////////////////////////////////////////////////////////////////////
// ���� �ü� �ڵ� ������Ʈ
typedef	struct	_At0933O
{
	ESHeader_st					Head;
	char	FullCode	 		[32];	/* ����ǥ���ڵ� 												*/
	char	JinbubGb  			[ 1];	/* ���� (0.10���� 1.32���� 2.64���� 3.128���� 4.8����)			*/
	char	Time				[ 8];	/* �ð�(HH:MM:SS)												*/
	char	CmpSign				[ 1];	/* ����ȣ
											0.���� 1.���� 2.���� 3.��� 4.�϶� 5.�⼼����
											6.�⼼���� 7.�⼼��� 8.�⼼�϶�							*/
	char	CmpPrice			[20];	/* ���ϴ��														*/
	char	ClosePrice			[20];	/* ���簡														*/
	char	CmpRate				[ 8];	/* �����9(5)V9(2)												*/
	char	TotQty 				[20];	/* �ŷ��� 														*/
	char	ContQty				[20];	/* ü�ᷮ 														*/
	char	MatchKind			[ 1];	/* ���簡�� ȣ������ (+.�ż� -.�ŵ�)							*/
	char	Date				[ 8];	/* ����(YYYYMMDD) 												*/
	char	OpenPrice			[20];	/* �ð� 														*/
	char	HighPrice			[20];	/* �� 														*/
	char	LowPrice			[20];	/* ���� 														*/
	char	BuyPrice			[20];	/* �ż�ȣ��														*/
	char	SellPrice			[20];	/* �ŵ�ȣ��														*/
	char	MarketFlag			[ 1];	/* �屸�� 0.���� 1.������										*/
	char	DecLen				[ 5];	/* ���� �Ҽ��� ����												*/
}	At0933O_st, * lpAt0933O_st;
#define	SZ_At0933O	sizeof( At0933O_st)

//////////////////////////////////////////////////////////////////////////////////
// �ֹ�/ü�� �ڵ� ������Ʈ
typedef	struct	_At0985O
{
	ESHeader_st					Head;
	char	Account				[11]; 	/* ���¹�ȣ														*/
	char	ReplyType			[ 1];	/* �������� 
											1.�ֹ����� 2.ü�� 3.����Ȯ�� 4.���Ȯ�� 
											5.�ű԰ź� 6.�����ź� 7.��Ұź� 0.��������					*/
	char	FullCode			[32];	/* �����ڵ� (���������϶� ǥ���ڵ�, �׿� �����ڵ�)				*/
	char	Side				[ 1];	/* �ż�/�ŵ����� (1.�ż� 2.�ŵ�)								*/
	char	Qty					[20];	/* �ֹ�����														*/
	char	Modality			[ 1];	/* ��������	(1.������ 2.���尡)									*/
	char	Price				[20];	/* �ֹ�����														*/
	char	Validity			[ 1];	/* ü������ (1.FAS 2.FOK 3.FAK 4.GTC 5.GTD)						*/
	char	StopLossLimit		[20];	/* stop order ��������											*/
	char	ExecPrice			[20];	/* ü�ᰡ��														*/
	char	ExecQty				[20];	/* ü�����														*/
	char	RemainQty			[20];	/* �ֹ��ܷ�														*/
	char	Ord_no				[ 5];	/* �ֹ���ȣ														*/
	char	Orig_ord_no			[ 5];	/* ���ֹ���ȣ													*/
	char	TradeTime			[ 8];	/* �ֹ�Ȯ��,ü��,�ź� �ð�										*/
	char	ExecAmt				[20];	/* ü��ݾ�														*/
	char	ORD_TP 				[ 1];	/* �ֹ�����	(ReplyType==0�϶� 1.�ű� 2.���� 3.���)				*/
	char    Trd_no				[ 5];   /* ü���ȣ														*/
	char    Trd_date			[ 8];   /* ü������														*/
	char	Rsrb_q				[10];	/* û�갡�ɼ���													*/
	char	Open_q				[10];	/* �ܰ����														*/
	char	Open_tp				[ 1];	/* �ܰ������Ǳ��� (1.�ż� 2.�ŵ�)								*/
	char    Ordp_q				[10];   /* �ֹ����ɼ���													*/
}	At0985O_st, * lpAt0985O_st;
#define	SZ_At0985O	sizeof( At0985O_st)


#endif