#include "stdafx.h"
#include "VtTaskManager.h"
#include "VtTaskArg.h"
#include "VtOrderManagerSelector.h"
#include "VtOrderManager.h"
#include "VtAccountManager.h"
#include "VtTaskArg.h"
#include "VtTaskManager.h"
#include "Util/VtTime.h"
#include "Global/VtDefine.h"
#include "VtAccount.h"
#include "VtSymbolManager.h"
#include "VtTaskArg.h"
#include "VtProductOrderManagerSelector.h"
#include "VtProductOrderManager.h"
#include "VtQuoteManager.h"
#include "VtQuote.h"
#include "VtHogaManager.h"
#include "VtHoga.h"

VtTaskManager::VtTaskManager()
{
	_IsExecuting = false;
}


VtTaskManager::~VtTaskManager()
{
	for (auto it = TaskList.begin(); it != TaskList.end(); ++it)
	{
		delete *it;
	}
}

void VtTaskManager::AddTask(VtTaskArg* task)
{
	TaskList.push_back(task);
}

void VtTaskManager::RemoveTask(VtTaskArg* task)
{
	if (!task) return;

	for (auto it = TaskList.begin(); it != TaskList.end(); ++it)
	{
		VtTaskArg* curTask = *it;
		if (curTask == task)
		{
			TaskList.erase(it);
			delete curTask;
			break;
		}
	}
}

void VtTaskManager::StartTask()
{
	if (TaskList.size() == 0)
	{
		_IsExecuting = false;
		return;
	}
	_IsExecuting = true;
	VtTaskArg* task = TaskList.front();
	ExecTask(task);
}

void VtTaskManager::ExecTask(VtTaskArg* task)
{
	if (!task) return;
	switch (task->Type)
	{
	case VtTaskType::VtTaskOrderFilled: // 실체결
		DoTaskOrderFilled(task);
		break;
	case VtTaskType::VtTaskAccountRemain: // 실잔고
		DoTaskAccountRemain(task);
		break;
	case VtTaskType::VtTaskAccountFilled: // 계좌별 주문 체결 현황
		DoTaskAccountFilled(task);
		break;
	case VtTaskType::VtTaskAccountDeposit: // 예탁자산 및 증거금
		DoTaskAccountDeposit(task);
		break;
	case VtTaskType::VtTaskSymbolMaster: // 종목 마스터
		DoTaskSymbolMaster(task);
		break;
	case VtTaskType::VtTaskSymbolQuote: // 종목 시세
		DoTaskSymbolQuote(task);
		break;
	case VtTaskType::VtTaskSymbolHoga: // 종목 호가
		DoTaskSymbolHoga(task);
		break;
	case VtTaskType::VtTaskSymbolTickQuote: // 종목 체결내역
		DoTaskSymbolTickQuote(task);
		break;
	case VtTaskType::VtTaskChartData: // 차트 데이터
		DoTaskChartData(task);
		break;
	default:
		break;
	}
}

void VtTaskManager::OnTaskCompleted(VtTaskEventArgs& arg)
{
	if (TaskList.empty())
	{
		_IsExecuting = false;
		return;
	}

	VtTaskArg* task = TaskList.front();
	if (task->Type == arg.TaskType)
	{
		TaskList.pop_front();
		delete task;
		// 다음 작업을 호출한다.
		StartTask();
	}
}

void VtTaskManager::DoTaskOrderFilled(VtTaskArg* task)
{
	if (!task)
		return;
	VtTaskOrderFilledArg* arg = (VtTaskOrderFilledArg*)task;
	VtOrderManagerSelector* orderMgrSelector = VtOrderManagerSelector::GetInstance();
	VtOrderManager* orderMgr = orderMgrSelector->FindOrderManager(arg->Account);
	if (!orderMgr)
		return;

	orderMgr->RequestFilledOrder(arg->Account, arg->Divide, arg->OrderBy);
}

void VtTaskManager::DoTaskAccountRemain(VtTaskArg* task)
{
	if (!task)
		return;
	VtTaskAccountRemainArg* arg = (VtTaskAccountRemainArg*)task;
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	acntMgr->RequestProductRemain(arg->Account);
}

void VtTaskManager::DoTaskAccountFilled(VtTaskArg* task)
{

}

void VtTaskManager::DoTaskAccountDeposit(VtTaskArg* task)
{
	if (!task)
		return;
	VtTaskAccountDepositArg* arg = (VtTaskAccountDepositArg*)task;
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	acntMgr->RequestAccountDeposit(arg->Account);
}

void VtTaskManager::DoTaskSymbolMaster(VtTaskArg* task)
{
	if (!task)
		return;

	VtTaskSymbolMasterArg* arg = (VtTaskSymbolMasterArg*)task;
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	symMgr->RequestSymbolMaster(arg->FullCode);
}

void VtTaskManager::DoTaskSymbolQuote(VtTaskArg* task)
{
	VtTaskSymbolQuoteArg* arg = (VtTaskSymbolQuoteArg*)task;
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	symMgr->RequestSymbolQuote(arg->FullCode);
}

void VtTaskManager::DoTaskSymbolHoga(VtTaskArg* task)
{
	VtTaskSymbolHogaArg* arg = (VtTaskSymbolHogaArg*)task;
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	symMgr->RequestSymbolHoga(arg->FullCode);
}

void VtTaskManager::DoTaskSymbolTickQuote(VtTaskArg* task)
{
	VtTaskSymbolTickQuoteArg* arg = (VtTaskSymbolTickQuoteArg*)task;
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	symMgr->RequestSymbolTickQuote(arg->FullCode);
}

void VtTaskManager::DoTaskChartData(VtTaskArg* task)
{

}

void VtTaskManager::DoBundleTaskForAccount()
{
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	VtAccount* acnt = acntMgr->ActiveAccount();
	if (!acnt)
		return;
	DoBundleTaskForAccount(acnt->AccountNo);
}

void VtTaskManager::DoBundleTaskForAccount(std::string acntNo)
{
	if (IsExecuting())
		return;

	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	VtAccount* acnt = acntMgr->FindAccount(acntNo);
	if (!acnt)
		return;

	VtOrderManagerSelector* orderMgrSelector = VtOrderManagerSelector::GetInstance();
	VtOrderManager* orderMgr = orderMgrSelector->FindOrderManager(acnt->AccountNo);
	orderMgr->ClearOrderMap();
	orderMgr->ClearAcceptedList();
	//orderMgr->ProductOrderManagerSelector()->ClearAllAccepted();
	
	// 종목별 잔고 상황을 받는다
	VtTaskAccountRemainArg* arg2 = new VtTaskAccountRemainArg(VtTaskType::VtTaskAccountRemain);
	arg2->Account = acnt->AccountNo;
	arg2->Pass = acnt->Password;
	AddTask(arg2);
	
	// 종목별 미체결 내역만 받는다.
	VtTaskOrderFilledArg*  arg = new VtTaskOrderFilledArg(VtTaskType::VtTaskOrderFilled);
	arg->Account = acnt->AccountNo;
	arg->BaseDate = VtTimeUtil::GetDateTimeString();
	arg->Divide = 2;
	arg->Pass = acnt->Password;
	arg->OrderBy = 2;

	AddTask(arg);
	

	// 계좌에 대한 위탁금 및 증거금
	VtTaskAccountDepositArg* arg3 = new VtTaskAccountDepositArg(VtTaskType::VtTaskAccountDeposit);
	arg3->Account = acnt->AccountNo;
	arg3->Pass = acnt->Password;
	arg3->Currency = _T("USD");

	AddTask(arg3);

	// 예약된 모든 작업을 시작한다.
	StartTask();
}

void VtTaskManager::DoBundleTaskForSymbol(std::string fullCode)
{
	VtTaskSymbolMasterArg* arg = new VtTaskSymbolMasterArg(VtTaskType::VtTaskSymbolMaster);
	arg->FullCode = fullCode;
	AddTask(arg);


	VtTaskSymbolQuoteArg* arg2 = new VtTaskSymbolQuoteArg(VtTaskType::VtTaskSymbolQuote);
	arg2->FullCode = fullCode;
	AddTask(arg2);

	VtTaskSymbolHogaArg* arg3 = new VtTaskSymbolHogaArg(VtTaskType::VtTaskSymbolHoga);
	arg3->FullCode = fullCode;
	AddTask(arg3);

	StartTask();
}

