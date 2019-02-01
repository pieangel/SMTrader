#pragma once
#include "Poco/BasicEvent.h"
#include "Global/TemplateSingleton.h"
#include <list>
#include <thread>
#include <map>
#include <queue>
#include "HdTaskEventArg.h"
#include "VtCorrentQueue.h"
#include "HdTaskArg.h"
#include "delegate.h"
#include "closure.h"

typedef dlgt::delegate<void(*)()> ReceivedComplete;

typedef std::queue<int> RequestQueue;
// request id , request id
typedef std::map<int, int> HdReqeust;
class VtProgressDlg;
class HdScheduler : public TemplateSingleton<HdScheduler>
{
public:
	HdScheduler();
	~HdScheduler();
	
	void OnTaskCompleted(HdTaskEventArgs& arg);
	void AddTask(HdTaskArg&& taskArg);
	void StartTaskThread();
	int SymbolCategoryCount() const { return _SymbolCategoryCount; }
	void SymbolCategoryCount(int val) { _SymbolCategoryCount = val; }
	int SymbolCount() const { return _SymbolCount; }
	void SymbolCount(int val) { _SymbolCount = val; }
	int SectionCount() const { return _SectionCount; }
	void SectionCount(int val) { _SectionCount = val; }
	int SubSectionCount() const { return _SubSectionCount; }
	void SubSectionCount(int val) { _SubSectionCount = val; }
	bool Available() const { return _Available; }
	void Available(bool val) { _Available = val; }
	void ClearTasks();
	VtProgressDlg* ProgressDlg() const { return _ProgressDlg; }
	void ProgressDlg(VtProgressDlg* val) { _ProgressDlg = val; }
private:
	void ProcessTask();
	std::thread _TaskThread;
	VtQueue<HdTaskArg> _TaskQueue;
	bool _Available = true;
	void Begin();
	void End();
	void ExecTask(HdTaskArg&& taskArg);

	int _SymbolCategoryCount;
	int _SymbolCount;
	int _SectionCount;
	int _SubSectionCount;
	std::map<HdTaskType, RequestQueue> RequestMap;
	bool _ReceivedBatchInfo;
public:
	int AddRequest(HdTaskType reqType, int reqId);
	int RemoveRequest(HdTaskType reqType, int reqId);
	void RegisterDelegate(ReceivedComplete& d) {
		_RecvComp = &d;
	}
	void NotifyDelegate() {
		(*_RecvComp)();
	}
	void GetSymbolCode();
	void GetSymbolMaster();
	void GetDeposit();
	void GetCustomProfitLoss();
	void GetOutstanding();
	void GetAcceptedHistory();
	void AddTaskList(std::vector<std::pair<std::string, HdTaskArg>>& argVec);
private:
	ReceivedComplete* _RecvComp;
	VtProgressDlg* _ProgressDlg;
	std::map<std::string, HdTaskInfo> _TaskInfoMap;
	void SetTaskInfo(std::string code, int remCnt);
	void FinishGetData();
};

