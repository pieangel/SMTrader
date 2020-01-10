#pragma once
#include "Global/TemplateSingleton.h"
#include "VtOrder.h"
#include "VtChartDataRequest.h"
#include "HdTaskArg.h"
#include "Chart/SmChartDefine.h"
class VtHdCtrl;
class HdFutureGrid;
class HdOptionGrid;
class VtHdClient : public TemplateSingleton<VtHdClient>
{
public:
	int LogIn(CString id, CString pwd, CString cert);
	int LogOut(CString id);

	int Init();
	VtHdClient();
	~VtHdClient();

	void LogIn();
	void LogOut();

	CString GetUserId();

	CString GetUserPassword();

	CString GetAccountPassword();

	void GetFutureCodeList(std::string catCode);
	void GetOptionCodeList();
	void GetTradableCodeTable();
	// 개별 종목에 대한 심볼 마스터를 가져온다.
	void GetSymbolMaster(CString symCode);
	// 주어진 캐터고리로 심볼 목록을 가져온다.
	void GetSymbolCode(CString catCode);
	void RegisterProduct(CString symCode);
	void UnregisterProduct(CString symCode);
	void RegisterAccount(CString accountNo);
	void UnregisterAccount(CString accountNo);
	void SendOrder(HdOrderRequest request);
	void PutOrder(HdOrderRequest&& request);
	void ChangeOrder(HdOrderRequest&& request);
	void CancelOrder(HdOrderRequest&& request);

	void GetDailyProfitLoss(std::string accountNo, std::string pwd);
	void GetAccountProfitLoss(std::string accountNo, std::string pwd);
	void GetAccountInfo();
	void GetFilledHistoryTable(std::string accountNo, std::string pwd);
	void GetOutstandingHistory(std::string accountNo, std::string pwd);
	void GetOutstanding(std::string accountNo, std::string pwd);
	void GetAcceptedHistory(std::string accountNo, std::string pwd);
	void GetApiCustomerProfitLoss(std::string accountNo, std::string pwd);

	void GetAsset(std::string accountNo, std::string pwd);
	void GetDeposit(std::string accountNo, std::string pwd);
	void SetFutureGrid(HdFutureGrid* grid);
	void SetOptionGrid(HdOptionGrid* grid);

	void GetChartData(VtChartDataRequest&& chartReqeust);
	void GetMasterFile(std::string fileName);
	void GetSymbolMasterWithCount(std::string symCode, int count);
	void RegisterCurrent();
	void UnregisterCurrent();

	void GetApiCustomerProfitLoss(HdTaskType taskType, std::string accountNo, std::string pwd);
	void GetFilledHistoryTable(HdTaskType taskType, std::string accountNo, std::string pwd);

	void AbGetChartData(VtChartDataRequest&& chartReqeust);
	void SetBlock(bool flag);
	void RegisterExpected(CString symCode);
	void UnregisterExpected(CString symCode);

	void GetServerTime();
	void RegisterRealtimeDataKey(std::string symCode, std::string dataKey);

	void DownloadMasterFiles(std::string param);
	void DownloadDomesticMasterFile(std::string file_name);

	void GetChartData(SmChartDataRequest req);
	void ExecuteRequest(std::shared_ptr<HdTaskArg> arg);

	// 해외 종목 현재 시세
	void GetAbroadQuote(std::string symbol_code);
	// 해외 종목 호가
	void GetAbroadHoga(std::string symbol_code);
private:
	VtHdCtrl* _HdCtrl = nullptr;
};

