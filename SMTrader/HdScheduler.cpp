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
#include "Market/SmMarketManager.h"
#include "Market/SmProduct.h"
#include "Market/SmProductYearMonth.h"
#include "Market/SmSymbolReader.h"
#include "VtSaveManager.h"
#include "VtSymbol.h"


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

	switch (arg.TaskType) { // �۾� Ÿ�Կ� ���� �б�
	case HdTaskType::HdSymbolFileDownload: { // �� ������ �ɺ������� �����´�. 
		int remCnt = RemoveRequest(HdTaskType::HdSymbolFileDownload, arg.RequestId);
		if (_ProgressDlg) {
			SetTaskInfo(_T("GetSymbolFile"), remCnt);
		}
		if (!_ReceivedBatchInfo && remCnt == 0) { // �ɺ� ���� �������Ⱑ �����ٸ� �ɺ� �ڵ� ��û
			// ������ �� ���� �޾Ҵٸ� �ɺ� ������ �ε��Ѵ�.
			SmMarketManager::GetInstance()->ReadDomesticSymbolsFromFile();
			GetSymbolCode();
		}
	}
	break;
	case HdTaskType::HdSymbolCode: { // �� ������ �ɺ��ڵ带 �����´�. - ���� �ɺ� �ڵ常 �����´�.
		int remCnt = RemoveRequest(HdTaskType::HdSymbolCode, arg.RequestId);
		if (_ProgressDlg) {
			SetTaskInfo(_T("GetSymbolCode"), remCnt);
		}
		if (!_ReceivedBatchInfo && remCnt == 0) { // �ɺ� �ڵ� �������Ⱑ �����ٸ� �ɺ� ������ ��û
			GetSymbolMaster2();
		}
	}
	break;
	case HdTaskType::HdSymbolMaster: { // ���� ���� ������ �о� �´�.
		int remCnt = RemoveRequest(HdTaskType::HdSymbolMaster, arg.RequestId);
		if (_ProgressDlg) {
			SetTaskInfo(_T("GetSymbolMaster"), remCnt);
		}
		if (!_ReceivedBatchInfo && remCnt == 0){
			Sleep(VtGlobal::ServerSleepTime);
			if (GetDeposit() == 0) {
				HdTaskEventArgs eventArg;
				eventArg.TaskType = HdTaskType::HdDeposit;
				OnTaskCompleted(eventArg);
			}
		}
	}
	break;
	case HdTaskType::HdDeposit: { // ���º� �ڱ� ��Ȳ�� �����´�.
		int remCnt = RemoveRequest(HdTaskType::HdDeposit, arg.RequestId);
		if (_ProgressDlg) {
			SetTaskInfo(_T("GetDeposit"), remCnt);
		}
		//Sleep(700);
		if (!_ReceivedBatchInfo && remCnt == 0) {
			if (GetCustomProfitLoss() == 0) {
				HdTaskEventArgs eventArg;
				eventArg.TaskType = HdTaskType::HdApiCustomerProfitLoss;
				OnTaskCompleted(eventArg);
			}
		}
	}
	break;
	case HdTaskType::HdApiCustomerProfitLoss: { // ���º�, ���� ������ �����´�.
		int remCnt = RemoveRequest(HdTaskType::HdApiCustomerProfitLoss, arg.RequestId);
		//Sleep(700); // GetCustomProfitLoss
		if (_ProgressDlg) {
			SetTaskInfo(_T("GetCustomProfitLoss"), remCnt);
		}
		if (!_ReceivedBatchInfo && remCnt == 0) {
			if (GetOutstanding() == 0) {
				HdTaskEventArgs eventArg;
				eventArg.TaskType = HdTaskType::HdOutstanding;
				OnTaskCompleted(eventArg);
			}
		}
	}
	break;
	case HdTaskType::HdOutstanding: { // ���� �ܰ� �����´�.
		int remCnt = RemoveRequest(HdTaskType::HdOutstanding, arg.RequestId);
		//Sleep(700); // GetOutstanding
		if (_ProgressDlg) {
			SetTaskInfo(_T("GetOutstanding"), remCnt);
		}
		if (!_ReceivedBatchInfo && remCnt == 0) {
			if (GetAcceptedHistory() == 0) {
				HdTaskEventArgs eventArg;
				eventArg.TaskType = HdTaskType::HdAcceptedHistory;
				OnTaskCompleted(eventArg);
			}
		}
	}
	break;
	case HdTaskType::HdAcceptedHistory: { // ���� ����Ȯ�� ����� �����´�.
		int remCnt = RemoveRequest(HdTaskType::HdAcceptedHistory, arg.RequestId);
		//Sleep(700); //GetAcceptedHistory
		if (_ProgressDlg) {
			SetTaskInfo(_T("GetAcceptedHistory"), remCnt);
		}
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
	case HdTaskType::HdSymbolFileDownload:
	{
		std::string file_name = taskArg.GetArg(_T("file_name"));
		client->DownloadDomesticMasterFile(file_name);
	}
	break;
	default:
		break;
	}
}
// �۾� Ÿ�� ���� ť�� �־� ���´�. reqId�� �ܼ��� �۾� ������ �ǹ��� �� �ٸ� �ǹ̴� ����.
// Ư�� Ÿ���� �۾��� �ϳ� �Ϸ�Ǹ� �� �۾� Ÿ���� ť���� �ܼ��� �ϳ��� �����ؼ� ���� �۾��� �ϳ� �پ����� ǥ���� �ش�.
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
/// ���̻� ������ ����� ���ų� �ƹ��͵� ���ٸ� 0�� ��ȯ�Ѵ�.
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
	prdtCatMgr->GetSymbolCode(taskInfo.argVec, _T("�Ϲ�"));
	taskInfo.TaskName = _T("���������� �������� ���Դϴ�.");
	taskInfo.TotalTaskCount = taskInfo.argVec.size();
	taskInfo.RemainTaskCount = taskInfo.TotalTaskCount;
	_TaskInfoMap[_T("GetSymbolCode")] = taskInfo;
	if (_ProgressDlg) {
		_ProgressDlg->SetTaskInfo(taskInfo);
	}
	// �۾� ť�� �۾� ����� �߰��Ѵ�.
	AddTaskList(taskInfo.argVec);
}

void HdScheduler::GetSymbolFile()
{
	SmSymbolReader* symReader = SmSymbolReader::GetInstance();
	HdTaskInfo taskInfo;
	// �۾� ������ ä���.
	if (symReader->SymbolMasterFileSet.size() > 0) {
		int i = 0;
		for (auto it = symReader->SymbolMasterFileSet.begin(); it != symReader->SymbolMasterFileSet.end(); ++it) {
			std::string file_name = *it;
			HdTaskArg arg;
			arg.Type = HdTaskType::HdSymbolFileDownload;
			arg.AddArg("file_name", file_name);
			taskInfo.argVec.push_back(std::make_pair(file_name, arg));
			// ��û ť�� ��û ������ �߰��Ѵ�.
			AddRequest(HdTaskType::HdSymbolFileDownload, i++);
		}
	}

	taskInfo.TaskName = _T("���������� �������� ���Դϴ�.");
	taskInfo.TotalTaskCount = taskInfo.argVec.size();
	taskInfo.RemainTaskCount = taskInfo.TotalTaskCount;
	_TaskInfoMap[_T("GetSymbolFile")] = taskInfo;
	if (_ProgressDlg) {
		_ProgressDlg->SetTaskInfo(taskInfo);
	}

	// �۾� ť�� �۾� ����� �߰��Ѵ�.
	AddTaskList(taskInfo.argVec);
}

void HdScheduler::GetSymbolMaster()
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	HdTaskInfo taskInfo;
	prdtCatMgr->GetRecentMonthSymbolMasterByCategory(taskInfo.argVec, _T("�Ϲ�"));
	taskInfo.TaskName = _T("���簡�� �������� ���Դϴ�.");
	taskInfo.TotalTaskCount = taskInfo.argVec.size();
	taskInfo.RemainTaskCount = taskInfo.TotalTaskCount;
	_TaskInfoMap[_T("GetSymbolMaster")] = taskInfo;
	if (_ProgressDlg) {
		_ProgressDlg->SetTaskInfo(taskInfo);
	}
	AddTaskList(taskInfo.argVec);
}

void HdScheduler::GetSymbolMaster2()
{
	std::set<std::string> master_vector;
	// ���� ��Ͽ��� �ٿ����� �������� �ڵ带 �о� �´�.
	std::vector<SmRunInfo> future_list = SmMarketManager::GetInstance()->GetFutureRunVector();
	for (auto it = future_list.begin(); it != future_list.end(); ++it) {
		SmRunInfo run_info = *it;
		SmProduct* product = SmMarketManager::GetInstance()->FindProductFromMap(run_info.Code.substr(0, 3));
		if (product) {
			VtSymbol* symbol = product->GetRecentMonthSymbol();
			if (symbol)
				master_vector.insert(symbol->ShortCode);
			symbol = product->GetNextMonthSymbol();
			if (symbol)
				master_vector.insert(symbol->ShortCode);
		}
	}

	VtSaveManager::GetInstance()->GetWindowSymbolList(master_vector);
	HdTaskInfo taskInfo;
	int i = 0;
	// ��û ������ ä���.
	for (auto it = master_vector.begin(); it != master_vector.end(); ++it) {
		std::string symbol_code = *it;
		HdTaskArg arg;
		arg.info_text = symbol_code;
		arg.Type = HdTaskType::HdSymbolMaster;
		arg.AddArg("SymbolCode", symbol_code);
		taskInfo.argVec.push_back(std::make_pair(symbol_code, arg));
		// ��û ť�� ��û ������ �߰��Ѵ�.
		AddRequest(HdTaskType::HdSymbolMaster, i++);
	}
	taskInfo.TaskName = _T("���簡�� �������� ���Դϴ�.");
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

	taskInfo.TaskName = _T("��Ź�� ������ �������� ���Դϴ�.");
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

	taskInfo.TaskName = _T("���������� �������� ���Դϴ�.");
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

	taskInfo.TaskName = _T("�ܰ������� �������� ���Դϴ�.");
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

	taskInfo.TaskName = _T("��ü�� ������ �������� ���Դϴ�.");
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
