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
	case VtTaskType::VtTaskOrderFilled: // ��ü��
		DoTaskOrderFilled(task);
		break;
	case VtTaskType::VtTaskAccountRemain: // ���ܰ�
		DoTaskAccountRemain(task);
		break;
	case VtTaskType::VtTaskAccountFilled: // ���º� �ֹ� ü�� ��Ȳ
		DoTaskAccountFilled(task);
		break;
	case VtTaskType::VtTaskAccountDeposit: // ��Ź�ڻ� �� ���ű�
		DoTaskAccountDeposit(task);
		break;
	case VtTaskType::VtTaskSymbolMaster: // ���� ������
		DoTaskSymbolMaster(task);
		break;
	case VtTaskType::VtTaskSymbolQuote: // ���� �ü�
		DoTaskSymbolQuote(task);
		break;
	case VtTaskType::VtTaskSymbolHoga: // ���� ȣ��
		DoTaskSymbolHoga(task);
		break;
	case VtTaskType::VtTaskSymbolTickQuote: // ���� ü�᳻��
		DoTaskSymbolTickQuote(task);
		break;
	case VtTaskType::VtTaskChartData: // ��Ʈ ������
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
		// ���� �۾��� ȣ���Ѵ�.
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
	
	// ���� �ܰ� ��Ȳ�� �޴´�
	VtTaskAccountRemainArg* arg2 = new VtTaskAccountRemainArg(VtTaskType::VtTaskAccountRemain);
	arg2->Account = acnt->AccountNo;
	arg2->Pass = acnt->Password;
	AddTask(arg2);
	
	// ���� ��ü�� ������ �޴´�.
	VtTaskOrderFilledArg*  arg = new VtTaskOrderFilledArg(VtTaskType::VtTaskOrderFilled);
	arg->Account = acnt->AccountNo;
	arg->BaseDate = VtTimeUtil::GetDateTimeString();
	arg->Divide = 2;
	arg->Pass = acnt->Password;
	arg->OrderBy = 2;

	AddTask(arg);
	

	// ���¿� ���� ��Ź�� �� ���ű�
	VtTaskAccountDepositArg* arg3 = new VtTaskAccountDepositArg(VtTaskType::VtTaskAccountDeposit);
	arg3->Account = acnt->AccountNo;
	arg3->Pass = acnt->Password;
	arg3->Currency = _T("USD");

	AddTask(arg3);

	// ����� ��� �۾��� �����Ѵ�.
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

