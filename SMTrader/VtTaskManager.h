#pragma once
#include "Global/TemplateSingleton.h"
#include <list>
#include "VtTaskEvent.h"

class VtTaskArg;
class VtTaskManager : public TemplateSingleton<VtTaskManager>
{
public:
	VtTaskManager();
	~VtTaskManager();
	std::list<VtTaskArg*> TaskList;
	void AddTask(VtTaskArg* task);
	void StartTask();
	void DoBundleTaskForAccount();
	void DoBundleTaskForAccount(std::string acntNo);
	void DoBundleTaskForSymbol(std::string fullCode);

	bool IsExecuting() const { return _IsExecuting; }
	void IsExecuting(bool val) { _IsExecuting = val; }

	void OnTaskCompleted(VtTaskEventArgs& arg);
private:
	void RemoveTask(VtTaskArg* task);
	void ExecTask(VtTaskArg* task);
	void DoTaskOrderFilled(VtTaskArg* task);
	void DoTaskAccountRemain(VtTaskArg* task);
	void DoTaskAccountFilled(VtTaskArg* task);
	void DoTaskAccountDeposit(VtTaskArg* task);
	void DoTaskSymbolMaster(VtTaskArg* task);
	void DoTaskSymbolQuote(VtTaskArg* task);
	void DoTaskSymbolHoga(VtTaskArg* task);
	void DoTaskSymbolTickQuote(VtTaskArg* task);
	void DoTaskChartData(VtTaskArg* task);

	bool _IsExecuting = false;
};

