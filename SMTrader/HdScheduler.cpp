#include "stdafx.h"
#include "HdScheduler.h"
#include "VtHdClient.h"
#include "VtSymbolManager.h"
#include "VtProductCategoryManager.h"
#include "VtAccountManager.h"
#include "VtAccount.h"
#include "VtGlobal.h"
#include "resource.h"
#include "MainFrm.h"
#include "VtProgressDlg.h"


HdScheduler::HdScheduler()
{
	Begin();
}


HdScheduler::~HdScheduler()
{
	End();
}

void HdScheduler::OnTaskCompleted(HdTaskEventArgs& arg)
{
	if (!_Available)
		_Available = true;

	switch (arg.TaskType) { // 작업 타입에 따라 분기
	case HdTaskType::HdSymbolCode: { // 각 섹셕편 심볼코드를 가져온다. - 오직 심볼 코드만 가져온다.
		int remCnt = RemoveRequest(HdTaskType::HdSymbolCode, arg.RequestId);
		if (_ProgressDlg) {
			SetTaskInfo(_T("GetSymbolCode"), remCnt);
		}
		if (!_ReceivedBatchInfo && remCnt == 0) { // 심볼 코드 가져오기가 끝났다면 심볼 마스터 요청
			GetSymbolMaster();
		}
	}
	break;
	case HdTaskType::HdSymbolMaster: { // 종목 종합 정보를 읽어 온다.
		int remCnt = RemoveRequest(HdTaskType::HdSymbolMaster, arg.RequestId);
		if (_ProgressDlg) {
			SetTaskInfo(_T("GetSymbolMaster"), remCnt);
		}
		if (!_ReceivedBatchInfo && remCnt == 0){
			if (GetDeposit() == 0) {
				HdTaskEventArgs eventArg;
				eventArg.TaskType = HdTaskType::HdDeposit;
				OnTaskCompleted(eventArg);
			}
		}
	}
	break;
	case HdTaskType::HdDeposit: { // 계좌별 자금 현황을 가져온다.
		int remCnt = RemoveRequest(HdTaskType::HdDeposit, arg.RequestId);
		if (_ProgressDlg) {
			SetTaskInfo(_T("GetDeposit"), remCnt);
		}
		Sleep(700);
		if (!_ReceivedBatchInfo && remCnt == 0) {
			if (GetCustomProfitLoss() == 0) {
				HdTaskEventArgs eventArg;
				eventArg.TaskType = HdTaskType::HdApiCustomerProfitLoss;
				OnTaskCompleted(eventArg);
			}
		}
	}
	break;
	case HdTaskType::HdApiCustomerProfitLoss: { // 계좌별, 종목별 손익을 가져온다.
		int remCnt = RemoveRequest(HdTaskType::HdApiCustomerProfitLoss, arg.RequestId);
		Sleep(700);
		if (!_ReceivedBatchInfo && remCnt == 0) {
			if (GetOutstanding() == 0) {
				HdTaskEventArgs eventArg;
				eventArg.TaskType = HdTaskType::HdOutstanding;
				OnTaskCompleted(eventArg);
			}
		}
	}
	break;
	case HdTaskType::HdOutstanding: { // 종목별 잔고를 가져온다.
		int remCnt = RemoveRequest(HdTaskType::HdOutstanding, arg.RequestId);
		Sleep(700);
		if (!_ReceivedBatchInfo && remCnt == 0) {
			if (GetAcceptedHistory() == 0) {
				HdTaskEventArgs eventArg;
				eventArg.TaskType = HdTaskType::HdAcceptedHistory;
				OnTaskCompleted(eventArg);
			}
		}
	}
	break;
	case HdTaskType::HdAcceptedHistory: { // 종목별 접수확인 목록을 가져온다.
		int remCnt = RemoveRequest(HdTaskType::HdAcceptedHistory, arg.RequestId);
		Sleep(700);
		if (remCnt == 0) {
			_ReceivedBatchInfo = true;
			FinishGetData();
		}
	}
	break;
	case HdTaskType::HdAccountFeeInfoStep1: {
		if (arg.Acnt) {
			arg.Acnt->GetAccountInfoNFee(2);
		}
	}
	break;
	case HdTaskType::HdAccountFeeInfoStep2: {

	}
	default:
		break;
	}
}

void HdScheduler::AddTask(HdTaskArg&& taskArg)
{
	_TaskQueue.push(taskArg);
}

void HdScheduler::ProcessTask()
{
	while (true)  {
		if (!_Available) { continue; }
		_Available = false;
		HdTaskArg taskArg = _TaskQueue.pop();
		if (taskArg.Type == HdTaskType::HdTaskComplete) {
			break;
		}
		ExecTask(std::move(taskArg));
	}
}

void HdScheduler::StartTaskThread()
{
	_TaskThread = std::thread(&HdScheduler::ProcessTask, this);
}

void HdScheduler::ClearTasks()
{
	_TaskQueue.Clear();
}

void HdScheduler::Begin()
{
	_RecvComp = nullptr;
	_ReceivedBatchInfo = false;
	_SubSectionCount = 0;
	_SectionCount = 0;
	_SymbolCategoryCount = 0;
	_SymbolCount = 0;
	_Available = true;
	StartTaskThread();
}

void HdScheduler::End()
{
	ClearTasks();
	HdTaskArg arg;
	arg.Type = HdTaskType::HdTaskComplete;
	_TaskQueue.push(std::move(arg));
	_Available = true;
	if (_TaskThread.joinable()) _TaskThread.join();
}

void HdScheduler::ExecTask(HdTaskArg&& taskArg)
{
	VtHdClient* client = VtHdClient::GetInstance();
	switch (taskArg.Type)
	{
	case HdTaskType::HdAcceptedHistory:
	{
		std::string accountNo = taskArg.GetArg(_T("AccountNo"));
		std::string password = taskArg.GetArg(_T("Password"));
		client->GetAcceptedHistory(accountNo, password);
	}
		break;
	case HdTaskType::HdFilledHistory:
		break;
	case HdTaskType::HdOutstandingHistory:
	{
		std::string accountNo = taskArg.GetArg(_T("AccountNo"));
		std::string password = taskArg.GetArg(_T("Password"));
		client->GetOutstandingHistory(accountNo, password);
	}
		break;
	case HdTaskType::HdOutstanding:
	{
		std::string accountNo = taskArg.GetArg(_T("AccountNo"));
		std::string password = taskArg.GetArg(_T("Password"));
		client->GetOutstanding(accountNo, password);
	}
		break;
	case HdTaskType::HdCmeAcceptedHistory:
		break;
	case HdTaskType::HdCmeFilledHistory:
		break;
	case HdTaskType::HdCmeOutstandingHistory:
		break;
	case HdTaskType::HdCmeOutstanding:
		break;
	case HdTaskType::HdCmeAsset:
		break;
	case HdTaskType::HdCmePureAsset:
		break;
	case HdTaskType::HdAsset:
	{
		std::string accountNo = taskArg.GetArg(_T("AccountNo"));
		std::string password = taskArg.GetArg(_T("Password"));
		client->GetAsset(accountNo, password);
	}
		break;
	case HdTaskType::HdDeposit:
	{
		std::string accountNo = taskArg.GetArg(_T("AccountNo"));
		std::string password = taskArg.GetArg(_T("Password"));
		client->GetDeposit(accountNo, password);
	}
		break;
	case HdTaskType::HdDailyProfitLoss:
	{
		std::string accountNo = taskArg.GetArg(_T("AccountNo"));
		std::string password = taskArg.GetArg(_T("Password"));
		client->GetDailyProfitLoss(accountNo, password);
	}
		break;
	case HdTaskType::HdFilledHistoryTable:
	{
		std::string accountNo = taskArg.GetArg(_T("AccountNo"));
		std::string password = taskArg.GetArg(_T("Password"));
		client->GetFilledHistoryTable(accountNo, password);
	}
		break;
	case HdTaskType::HdAccountProfitLoss:
	{
		std::string accountNo = taskArg.GetArg(_T("AccountNo"));
		std::string password = taskArg.GetArg(_T("Password"));
		client->GetAccountProfitLoss(accountNo, password);
	}
		break;
	case HdTaskType::HdSymbolCode:
	{
		std::string symCode = taskArg.GetArg(_T("Category"));
		//TRACE(symCode.c_str());
		//TRACE(_T("\n"));
		client->GetSymbolCode(CString(symCode.c_str()));
	}
		break;
	case HdTaskType::HdTradableCodeTable:
		break;
	case HdTaskType::HdApiCustomerProfitLoss:
	{
		std::string accountNo = taskArg.GetArg(_T("AccountNo"));
		std::string password = taskArg.GetArg(_T("Password"));
		client->GetApiCustomerProfitLoss(accountNo, password);
	}
		break;
	case HdTaskType::HdChartData:
		break;
	case HdTaskType::HdCurrentQuote:
		break;
	case HdTaskType::HdDailyQuote:
		break;
	case HdTaskType::HdTickQuote:
		break;
	case HdTaskType::HdSecondQutoe:
		break;
	case HdTaskType::HdSymbolMaster:
	{
		std::string symCode = taskArg.GetArg(_T("SymbolCode"));
		//TRACE(symCode.c_str());
		//TRACE(_T("\n"));
		client->GetSymbolMaster(CString(symCode.c_str()));
	}
		break;
	case HdTaskType::HdStockFutureSymbolMaster:
		break;
	case HdTaskType::HdIndustryMaster:
		break;
	case HdTaskType::HdAccountFeeInfoStep1:
	{
		std::string accountNo = taskArg.GetArg(_T("AccountNo"));
		std::string password = taskArg.GetArg(_T("Password"));
		client->GetApiCustomerProfitLoss(HdTaskType::HdAccountFeeInfoStep1, accountNo, password);
	}
		break;
	case HdTaskType::HdAccountFeeInfoStep2:
	{
		std::string accountNo = taskArg.GetArg(_T("AccountNo"));
		std::string password = taskArg.GetArg(_T("Password"));
		client->GetFilledHistoryTable(HdTaskType::HdAccountFeeInfoStep2, accountNo, password);
	}
		break;
	default:
		break;
	}
}

int HdScheduler::AddRequest(HdTaskType reqType, int reqId)
{
	auto it = RequestMap.find(reqType);
	if (it != RequestMap.end())
	{
		RequestQueue& rq = RequestMap[reqType];
		rq.push(reqId);
		return rq.size();
	}
	else
	{
		RequestQueue rq;
		rq.push(reqId);
		RequestMap[reqType] = rq;
		return rq.size();
	}
}

/// <summary>
/// 더이상 제거할 목록이 없거나 아무것도 없다면 0을 반환한다.
/// </summary>
/// <param name="reqType"></param>
/// <param name="reqId"></param>
/// <returns></returns>
int HdScheduler::RemoveRequest(HdTaskType reqType, int reqId)
{
	auto it = RequestMap.find(reqType);
	if (it != RequestMap.end())
	{
		RequestQueue& rq = RequestMap[reqType];
		if (rq.size() == 0)
			return 0;
		rq.pop();
		return rq.size();
	}

	return 0;
}

void HdScheduler::GetSymbolCode()
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	HdTaskInfo taskInfo;
	prdtCatMgr->GetSymbolCode(taskInfo.argVec, _T("일반"));
	taskInfo.TaskName = _T("종목정보를 가져오는 중입니다.");
	taskInfo.TotalTaskCount = taskInfo.argVec.size();
	taskInfo.RemainTaskCount = taskInfo.TotalTaskCount;
	_TaskInfoMap[_T("GetSymbolCode")] = taskInfo;
	if (_ProgressDlg) {
		_ProgressDlg->SetTaskInfo(taskInfo);
	}
	AddTaskList(taskInfo.argVec);
}

void HdScheduler::GetSymbolMaster()
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	HdTaskInfo taskInfo;
	prdtCatMgr->GetRecentMonthSymbolMasterByCategory(taskInfo.argVec, _T("일반"));
	taskInfo.TaskName = _T("현재가를 가져오는 중입니다.");
	taskInfo.TotalTaskCount = taskInfo.argVec.size();
	taskInfo.RemainTaskCount = taskInfo.TotalTaskCount;
	_TaskInfoMap[_T("GetSymbolMaster")] = taskInfo;
	if (_ProgressDlg) {
		_ProgressDlg->SetTaskInfo(taskInfo);
	}
	AddTaskList(taskInfo.argVec);
}

int HdScheduler::GetDeposit()
{
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	int i = 0, realCnt = 0;
	for (auto it = acntMgr->AccountMap.begin(); it != acntMgr->AccountMap.end(); ++it) {
		VtAccount* acnt = it->second;
		if (acnt->hasValidPassword()) {
			realCnt++;
			AddRequest(HdTaskType::HdDeposit, i++);
		}
	}
	if (realCnt == 0)
		return realCnt;

	HdTaskInfo taskInfo;
	for (auto it = acntMgr->AccountMap.begin(); it != acntMgr->AccountMap.end(); ++it) {
		VtAccount* acnt = it->second;
		if (acnt->hasValidPassword())
			acnt->GetDeposit(taskInfo.argVec);
	}

	taskInfo.TaskName = _T("예탁금 정보를 가져오는 중입니다.");
	taskInfo.TotalTaskCount = taskInfo.argVec.size();
	taskInfo.RemainTaskCount = taskInfo.TotalTaskCount;
	_TaskInfoMap[_T("GetDeposit")] = taskInfo;
	if (_ProgressDlg) {
		_ProgressDlg->SetTaskInfo(taskInfo);
	}
	AddTaskList(taskInfo.argVec);

	return realCnt;
}

int HdScheduler::GetCustomProfitLoss()
{
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	HdTaskInfo taskInfo;
	int i = 0, realCnt = 0;
	for (auto it = acntMgr->AccountMap.begin(); it != acntMgr->AccountMap.end(); ++it) {
		VtAccount* acnt = it->second;
		if (acnt->hasValidPassword()) {
			realCnt++;
			AddRequest(HdTaskType::HdApiCustomerProfitLoss, i++);
		}
	}

	if (realCnt == 0)
		return realCnt;

	for (auto it = acntMgr->AccountMap.begin(); it != acntMgr->AccountMap.end(); ++it) {
		VtAccount* acnt = it->second;
		if (acnt->hasValidPassword())
			acnt->GetApiCustomerProfitLoss(taskInfo.argVec);
	}

	taskInfo.TaskName = _T("손익정보를 가져오는 중입니다.");
	taskInfo.TotalTaskCount = taskInfo.argVec.size();
	taskInfo.RemainTaskCount = taskInfo.TotalTaskCount;
	_TaskInfoMap[_T("GetCustomProfitLoss")] = taskInfo;
	if (_ProgressDlg) {
		_ProgressDlg->SetTaskInfo(taskInfo);
	}
	AddTaskList(taskInfo.argVec);

	return realCnt;
}

int HdScheduler::GetOutstanding()
{
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	int i = 0, realCnt = 0;
	for (auto it = acntMgr->AccountMap.begin(); it != acntMgr->AccountMap.end(); ++it) {
		VtAccount* acnt = it->second;
		if (acnt->hasValidPassword()) {
			realCnt++;
			AddRequest(HdTaskType::HdOutstanding, i++);
		}
	}

	if (realCnt == 0)
		return realCnt;

	HdTaskInfo taskInfo;
	for (auto it = acntMgr->AccountMap.begin(); it != acntMgr->AccountMap.end(); ++it) {
		VtAccount* acnt = it->second;
		if (acnt->hasValidPassword())
			acnt->GetOutstanding(taskInfo.argVec);
	}

	taskInfo.TaskName = _T("잔고정보를 가져오는 중입니다.");
	taskInfo.TotalTaskCount = taskInfo.argVec.size();
	taskInfo.RemainTaskCount = taskInfo.TotalTaskCount;
	_TaskInfoMap[_T("GetOutstanding")] = taskInfo;
	if (_ProgressDlg) {
		_ProgressDlg->SetTaskInfo(taskInfo);
	}
	AddTaskList(taskInfo.argVec);

	return realCnt;
}

int HdScheduler::GetAcceptedHistory()
{
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	int i = 0, realCnt = 0;
	for (auto it = acntMgr->AccountMap.begin(); it != acntMgr->AccountMap.end(); ++it) {
		VtAccount* acnt = it->second;
		if (acnt->hasValidPassword()) {
			realCnt++;
			AddRequest(HdTaskType::HdAcceptedHistory, i++);
		}
	}

	if (realCnt == 0)
		return realCnt;

	HdTaskInfo taskInfo;
	for (auto it = acntMgr->AccountMap.begin(); it != acntMgr->AccountMap.end(); ++it) {
		VtAccount* acnt = it->second;
		if (acnt->hasValidPassword())
			acnt->GetAcceptedHistory(taskInfo.argVec);
	}

	taskInfo.TaskName = _T("미체결 정보를 가져오는 중입니다.");
	taskInfo.TotalTaskCount = taskInfo.argVec.size();
	taskInfo.RemainTaskCount = taskInfo.TotalTaskCount;
	_TaskInfoMap[_T("GetAcceptedHistory")] = taskInfo;
	if (_ProgressDlg) {
		_ProgressDlg->SetTaskInfo(taskInfo);
	}
	AddTaskList(taskInfo.argVec);

	return realCnt;
}

void HdScheduler::AddTaskList(std::vector<std::pair<std::string, HdTaskArg>>& argVec)
{
	for (auto it = argVec.begin(); it != argVec.end(); ++it) {
		_TaskQueue.push(std::get<1>(*it));
	}
}

void HdScheduler::SetTaskInfo(std::string code, int remCnt)
{
	HdTaskInfo& taskInfo = _TaskInfoMap[code];
	taskInfo.RemainTaskCount = remCnt;
	if (_ProgressDlg) {
		_ProgressDlg->SetTaskInfo(taskInfo);
	}
}

void HdScheduler::FinishGetData()
{
	//CMainFrame* pMainWnd = (CMainFrame*)AfxGetMainWnd();
	//pMainWnd->OnReceiveComplete();

	if (_ProgressDlg) {
		_ProgressDlg->CloseDialog();
		_ProgressDlg = nullptr;
	}
}
