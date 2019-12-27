#pragma once
#include "Global/TemplateSingleton.h"
#include "HdTaskArg.h"
#include <map>
#include <memory>
class VtProgressDlg;
class SmTaskManager : public TemplateSingleton<SmTaskManager>
{
public:
	SmTaskManager();
	~SmTaskManager();
	HdTaskState TaskState() const { return _TaskState; }
	void TaskState(HdTaskState val) { _TaskState = val; }
	void StartTask();
	void OnRequestComplete(std::shared_ptr<HdTaskArg> arg);
	VtProgressDlg* ProgressDlg() const { return _ProgressDlg; }
	void ProgressDlg(VtProgressDlg* val) { _ProgressDlg = val; }
	void SetTaskInfo(int code, std::string msg);
private:
	void StartAbroadFileDownload();
	void MakeSymbolMasterList();
	int GetRequestID();
	std::map<int, std::shared_ptr<HdTaskArg>> _TaskReqMap;
	void MakeSymbolFileRequest();
	HdTaskState _TaskState = HdTaskState::HdTaskInit;
	int _RequestID = 0;
	VtProgressDlg* _ProgressDlg = nullptr;
	//std::map<std::string, HdTaskInfo> _TaskInfoMap;
	HdTaskInfo _TaskInfo;
};

