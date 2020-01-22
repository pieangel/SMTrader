#pragma once
#include "hdfcommagent.h"
#include "Poco/BasicEvent.h"
#include "HdTaskEventArg.h"
#include <string>
#include <map>
#include <set>
#include <tuple>
#include <vector>
#include <mutex>
#include <queue>
#include "VtOrder.h"
#include "VtChartDataRequest.h"
#include "HdTaskArg.h"
#include "Global/VtDefine.h"
#include "Chart/SmChartDefine.h"

using Poco::BasicEvent;
class VtChartDataCollector;
// VtHdCtrl dialog
class HdFutureGrid;
class HdOptionGrid;
// VtHdCtrl dialog

class VtHdCtrl : public CDialogEx
{
	DECLARE_DYNAMIC(VtHdCtrl)

public:
	VtHdCtrl(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtHdCtrl();

	CHDFCommAgent m_CommAgent;

	HdFutureGrid* FutureGrid() const { return _FutureGrid; }
	void FutureGrid(HdFutureGrid* val) { _FutureGrid = val; }
	HdOptionGrid* OptionGrid() const { return _OptionGrid; }
	void OptionGrid(HdOptionGrid* val) { _OptionGrid = val; }
	bool Blocked() const { return _Blocked; }
	void Blocked(bool val) { _Blocked = val; }

	BasicEvent<HdTaskEventArgs> _TaskCompleteEvent;
	void FireTaskCompleted(HdTaskEventArgs&& arg)
	{
		_TaskCompleteEvent(this, arg);
	}

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HDF_CTRL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	DECLARE_EVENTSINK_MAP()
	afx_msg void OnDataRecv(CString sTrCode, LONG nRqID);
	afx_msg void OnGetBroadData(CString strKey, LONG nRealType);
	afx_msg void OnGetMsg(CString strCode, CString strMsg);
	afx_msg void OnGetMsgWithRqId(int nRqId, CString strCode, CString strMsg);

public:
	void OnRcvdDomesticChartData(CString& sTrCode, LONG& nRqID);
	void OnRcvdAbroadChartData(CString& sTrCode, LONG& nRqID);
	void OnRcvdAbroadChartData2(CString& sTrCode, LONG& nRqID);

	void GetChartData(SmChartDataRequest req);
	void GetChartDataForDomestic(SmChartDataRequest req);
	void GetChartDataShortCycle(SmChartDataRequest req);
	void GetChartDataLongCycle(SmChartDataRequest req);
	int Init();
	void LogIn();
	void LogOut();

	int LogIn(CString id, CString pwd, CString cert);
	int LogOut(CString id);

	void GetSymbolMasterWithCount(std::string symCode, int count);
	void GetMasterFile(std::string fileName);

	virtual BOOL OnInitDialog();

	void GetFutureCodeList();
	int GetSymbolCode(CString categoryName);
	void GetFutureCodeList(CString categoryName);
	void GetOptionCodeList(CString categoryName);
	void GetTradableCodeTable();
	int GetSymbolMaster(CString symCode);

	void RegisterProduct(CString symCode);
	void UnregisterProduct(CString symCode);
	void RegisterAccount(CString accountNo);
	void UnregisterAccount(CString accountNo);

	CString GetUserId()
	{
		return m_sUserId;
	}
	CString GetUserPassword()
	{
		return m_sUserPw;
	}
	CString GetAccountPassword()
	{
		return m_sAcntPw;
	}

private:
	void RequestChartDataFromQ();
	std::queue< SmChartDataRequest> _ChartDataReqQueue;
	std::map<int, SmChartDataRequest> _ChartDataReqMap;
	int GetIntOrderPrice(CString symbol_code, CString strPrice, CString strOrderPrice);
	bool CheckPassword(HdOrderRequest& request);
	void OnReceiveChartData(CString& sTrCode, LONG& nRqID);
	virtual void OnChartData(CString& sTrCode, LONG& nRqID);

	void OnNewOrderHd(CString& sTrCode, LONG& nRqID);
	void OnModifyOrderHd(CString& sTrCode, LONG& nRqID);
	void OnCancelOrderHd(CString& sTrCode, LONG& nRqID);

	void OnOrderReceivedHd(CString& sTrCode, LONG& nRqID);
	void OnOrderAcceptedHd(CString& sTrCode, LONG& nRqID);
	void OnOrderUnfilledHd(CString& sTrCode, LONG& nRqID);
	void OnOrderFilledHd(CString& sTrCode, LONG& nRqID);

	void OnAcceptedHistory(CString& sTrCode, LONG& nRqID);
	void OnFilledHistory(CString& sTrCode, LONG& nRqID);
	void OnOutstandingHistory(CString& sTrCode, LONG& nRqID);
	void OnOutstanding(CString& sTrCode, LONG& nRqID);

	void OnCmeAcceptedHistory(CString& sTrCode, LONG& nRqID);
	void OnCmeFilledHistory(CString& sTrCode, LONG& nRqID);
	void OnCmeOutstandingHistory(CString& sTrCode, LONG& nRqID);
	void OnCmeOutstanding(CString& sTrCode, LONG& nRqID);

	void OnAsset(CString& sTrCode, LONG& nRqID);
	void OnDeposit(CString& sTrCode, LONG& nRqID);

	void OnCmeAsset(CString& sTrCode, LONG& nRqID);
	void OnCmePureAsset(CString& sTrCode, LONG& nRqID);
	void OnDailyProfitLoss(CString& sTrCode, LONG& nRqID);
	void OnFilledHistoryTable(CString& sTrCode, LONG& nRqID);
	void OnAccountProfitLoss(CString& sTrCode, LONG& nRqID);
	void OnApiCustomerProfitLoss(CString& sTrCode, LONG& nRqID);
	void OnServerTime(CString& sTrCode, LONG& nRqID);

	void OnSymbolCode(CString& sTrCode, LONG& nRqID);
	void OnSymbolMaster(CString& sTrCode, LONG& nRqID);

	void OnFutureHoga(CString& sTrCode, LONG& nRqID);
	void OnOptionHoga(CString& sTrCode, LONG& nRqID);
	void OnProductHoga(CString& sTrCode, LONG& nRqID);

	void OnRemain(CString& strKey, LONG& nRealType);
	void OnRealPosition(CString& strKey, LONG& nRealType);
	void OnRealFutureQuote(CString& strKey, LONG& nRealType);
	void OnRealOptionQuote(CString& strKey, LONG& nRealType);
	void OnRealProductQuote(CString& strKey, LONG& nRealType);
	void OnSubAccountOrder(VtOrderEvent event, CString& strSubAcntNo, CString& strFundName, VtOrder* parentOrder, VtOrderState prevState);
	void SendOrderMessage(VtOrderEvent orderEvent, VtOrder* order);
	void OnExpected(CString& strKey, LONG& nRealType);

	CString m_sUserId;
	CString m_sUserPw;
	CString m_sAuthPw;
	CString m_sAcntPw;
	CString m_strOrdNo;
public:

	void RegisterCurrent();
	void UnregisterCurrent();

	int GetAcceptedHistory(std::string accountNo, std::string pwd);
	void GetFilledHistory(std::string accountNo, std::string pwd);
	int GetOutstandingHistory(std::string accountNo, std::string pwd);
	int GetOutstanding(std::string accountNo, std::string pwd);

	void GetCmeAcceptedHistory(std::string accountNo, std::string pwd);
	void GetCmeFilledHistory(std::string accountNo, std::string pwd);
	void GetCmeOutstandingHistory(std::string accountNo, std::string pwd);
	void GetCmeOutstanding(std::string accountNo, std::string pwd);

	int GetAsset(std::string accountNo, std::string pwd);
	int GetDeposit(std::string accountNo, std::string pwd);

	void GetCmeAsset(std::string accountNo, std::string pwd);
	void GetCmePureAsset(std::string accountNo, std::string pwd);
	int GetDailyProfitLoss(std::string accountNo, std::string pwd);
	int GetFilledHistoryTable(std::string accountNo, std::string pwd);
	int GetAccountProfitLoss(std::string accountNo, std::string pwd);
	int GetApiCustomerProfitLoss(std::string accountNo, std::string pwd);

	void GetChartData();
	void GetChartData(VtChartDataRequest&& chartReqeust);
	void GetAccountInfo();
	void SendOrder(HdOrderRequest request);
	void PutOrder(HdOrderRequest&& request);
	void ChangeOrder(HdOrderRequest&& request);
	void CancelOrder(HdOrderRequest&& request);

	// 해외 신규 주문
	void AbPutOrder(HdOrderRequest& request);
	// 해외 정정 주문
	void AbChangeOrder(HdOrderRequest& request);
	// 해외 취소 주문
	void AbCancelOrder(HdOrderRequest& request);
	
	// 해외 신규 주문 서버 응답
	void AbOnNewOrderHd(CString& sTrCode, LONG& nRqID);
	// 해외 정정 주문 서버 응답
	void AbOnModifyOrderHd(CString& sTrCode, LONG& nRqID);
	// 해외 취소 주문 서버 응답
	void AbOnCancelOrderHd(CString& sTrCode, LONG& nRqID);
	// 해외 주문 서버 응답
	void AbOnOrderReceivedHd(CString& sTrCode, LONG& nRqID);
	// 해외 주문 접수확인
	void AbOnOrderAcceptedHd(CString& sTrCode, LONG& nRqID);
	// 해외 주문 미체결 
	void AbOnOrderUnfilledHd(CString& sTrCode, LONG& nRqID);
	// 해외 주문 체결
	void AbOnOrderFilledHd(CString& sTrCode, LONG& nRqID);

	static std::string PadLeft(double input, char padding, int len, int decimal);
	static std::string PadRight(double input, char padding, int len, int decimal);
	static std::string PadLeft(int input, char padding, int len);
	static std::string PadRight(int input, char padding, int len);
	static std::string PadLeft(std::string input, char padding, int len);
	static std::string PadRight(std::string input, char padding, int len);

	int GetApiCustomerProfitLoss(HdTaskType taskType, std::string accountNo, std::string pwd);
	int GetFilledHistoryTable(HdTaskType taskType, std::string accountNo, std::string pwd);

	void AbGetChartData(VtChartDataRequest&& chartReqeust);

	void RegisterExpected(CString symCode);
	void UnregisterExpected(CString symCode);

	void GetServerTime();
	void RegisterRealtimeDataKey(std::string symCode, std::string dataKey);

	void DownloadMasterFiles(std::string param);
	int DownloadDomesticMasterFile(std::string file_name);
	void ExecuteRequest(std::shared_ptr<HdTaskArg> arg);

	// 해외 종목 현재 시세
	int GetAbroadQuote(std::string symbol_code);
	// 해외 종목 호가
	int GetAbroadHoga(std::string symbol_code);
	// 해외 현재 시세 수신
	void OnAbQuote(CString& sTrCode, LONG& nRqID);
	// 해외 현재 호가 수신
	void OnAbHoga(CString& sTrCode, LONG& nRqID);
private:
	int _RqID = 0;
	std::map<int, std::shared_ptr<HdTaskArg>> _TaskReqMap;
	bool _FileDownloading = false;
	std::map<std::string, std::set<std::string>> _DataKeyMap;
	void OnReceiveRealTimeValue(std::string symCode);
	void OnReceiveHoga(int time, VtSymbol* sym);
	void OnReceiveSise(int time, VtSymbol* sym);
	void OnOrderReceived(int reqId, VtOrder* order);
	void OnOrderReceivedHd(VtOrder* order);
	void OnOrderAcceptedHd(VtOrder* order);
	void OnOrderUnfilledHd(VtOrder* order);
	void OnOrderFilledHd(VtOrder* order);

	HdFutureGrid* _FutureGrid = nullptr;
	HdOptionGrid* _OptionGrid = nullptr;

	std::map<int, HdTaskArg> RequestMap;
	// 선물사 요청 번호 : 차트 데이터 키
	std::map<int, std::string> _ChartDataRequestMap;
	// 선물사 요청 번호 와 주문 요청 정보 매칭 맵 
	std::map<int, HdOrderRequest> _ReqIdToRequestMap;
	// 주문 번호와 주문 요청 정보 매칭 맵
	std::map<std::string, HdOrderRequest> _OrderNoToRequestMap;
	// 주문번호로 원래 요청을 찾아 낸다.
	// 주문번호로 원래 요청을 찾을 수 없다면 외부에서 이루어진 주문이다.
	HdOrderRequest* GetOrderRequestByOrderNo(std::string order_no);
	// 주문요청번호로 원래 요청을 찾아 낸다.
	// 요청번호가 없는 것은 전부 외부에서 이루어진 주문이다.
	HdOrderRequest* GetOrderRequestByOrderReqId(int req_id);
	void AddRequest(int reqId, HdTaskType taskType);
	void AddRequest(int reqId, HdTaskType taskType, std::string acntNo);
	void AddRequest(int reqId, HdTaskType taskType, std::string acntNo, std::string symCode);
	void RemoveRequest(int reqId);
	void RemoveOrderRequest(int reqId);
	void RemoveOrderRequest(VtOrder* order);
	HdTaskArg FindRequest(int reqId);

	bool _ServerConnected = true;
	bool _Blocked = false;
	//VtChartDataCollector* _DataCollector;
	void HandleRealData(std::string symCode, int time, int value);
	/// <summary>
	/// 주문에 오류가 생겼을 때 주문목록의 상태를 갱신해 준다.
	/// 특히 정정이나 취소시 이미 체결된 주문이 있을 때 그 목록을 없애 준다.
	/// </summary>
	void RefreshAcceptedOrderByError(int reqId);
	std::mutex m_;
	std::map<int, std::string> _SymbolFileReqMap;
	std::map<int, std::string> _AccountPLReqMap;
};
