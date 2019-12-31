#include "stdafx.h"
#include "SmTaskManager.h"
#include "Market/SmSymbolReader.h"
#include "VtHdClient.h"
#include "VtProgressDlg.h"
#include "Market/SmMarketManager.h"
#include "Market/SmProduct.h"
#include "VtSaveManager.h"
#include <set>
#include "VtSymbol.h"
#include "VtGlobal.h"

SmTaskManager::SmTaskManager()
{
}


SmTaskManager::~SmTaskManager()
{
}

void SmTaskManager::StartTask()
{
	if (_TaskState == HdTaskState::HdTaskInit) {
		// �������� �ɺ� ���� ����� �����. 
		MakeSymbolFileRequest();
		_TaskInfo.TaskName = _T("���� ���������� �����ް� �ִ� ���Դϴ�.");
		_TaskInfo.TotalTaskCount = _TaskInfo.argMap.size();
		_TaskInfo.RemainTaskCount = _TaskInfo.TotalTaskCount;
		if (_ProgressDlg) {
			_ProgressDlg->SetTaskInfo2(_TaskInfo);
		}
		// �ɺ� ������ �ޱ� �����Ѵ�.
		std::shared_ptr<HdTaskArg> arg = _TaskInfo.argMap.begin()->second;
		VtHdClient::GetInstance()->ExecuteRequest(arg);
		_TaskState = HdTaskState::HdSymbolFileDownloading;
	}
}

// �ϳ��� ��û�� �Ϸ�Ǹ� �ʿ��� ��û�� ã�Ƴ��� ������ �ش�.
void SmTaskManager::OnRequestComplete(std::shared_ptr<HdTaskArg> arg)
{
	if (!arg)
		return;
	Sleep(VtGlobal::ServerSleepTime);
	auto it = _TaskInfo.argMap.find(arg->RequestID);
	if (it != _TaskInfo.argMap.end()) {
		_TaskInfo.argMap.erase(it);
		_TaskInfo.RemainTaskCount = _TaskInfo.argMap.size();
	}

	if (_TaskState == HdTaskState::HdSymbolFileDownloading) {
		if (_ProgressDlg) {
			_ProgressDlg->SetTaskInfo2(_TaskInfo);
		}
		// ���� ���� �ٿ�ε� �Ϸ�
		if (_TaskInfo.argMap.size() == 0) {
			_TaskState = HdTaskState::HdSymbolFileDownloaded;
			StartAbroadFileDownload();
			VtHdClient::GetInstance()->DownloadMasterFiles("futures");
		}
		else {
			std::shared_ptr<HdTaskArg> arg = _TaskInfo.argMap.begin()->second;
			VtHdClient::GetInstance()->ExecuteRequest(arg);
		}
	} 
	else if (_TaskState == HdTaskState::HdSymbolMasterDownloading) {
		if (_ProgressDlg) {
			_ProgressDlg->SetTaskInfo2(_TaskInfo);
		}

		// ������ ���� �ٿ�ε� �Ϸ�
		if (_TaskInfo.argMap.size() == 0) {
			_TaskState = HdTaskState::HdSymbolMasterDownloaded;
			
		}
		else {
			std::shared_ptr<HdTaskArg> arg = _TaskInfo.argMap.begin()->second;
			VtHdClient::GetInstance()->ExecuteRequest(arg);
		}
	}
}

void SmTaskManager::SetTaskInfo(int code, std::string msg)
{
	if (_ProgressDlg) {
		_ProgressDlg->SetTaskInfo(msg);
	}
	// �ؿ� ���� �ɺ� ���� �Ϸ�
	if (code == 332) {
		//SmMarketManager::GetInstance()->ReadDomesticSymbolsFromFile();
		//SmMarketManager::GetInstance()->ReadAbroadSymbolsFromFile();

		// �ɺ� ������ ����� �����.
		MakeSymbolMasterList();

		_TaskInfo.TaskName = _T("���簡�� �����ް� �ִ� ���Դϴ�.");
		_TaskInfo.TotalTaskCount = _TaskInfo.argMap.size();
		_TaskInfo.RemainTaskCount = _TaskInfo.TotalTaskCount;
		if (_ProgressDlg) {
			_ProgressDlg->SetTaskInfo2(_TaskInfo);
		}
		// �ɺ� ������ ������ �ޱ� �����Ѵ�.
		std::shared_ptr<HdTaskArg> arg = _TaskInfo.argMap.begin()->second;
		VtHdClient::GetInstance()->ExecuteRequest(arg);
		_TaskState = HdTaskState::HdSymbolMasterDownloading;
	}
}

void SmTaskManager::StartAbroadFileDownload()
{
	_TaskInfo.TaskName = _T("�ؿ� ���������� �����ް� �ִ� ���Դϴ�.");
	_TaskInfo.TotalTaskCount = 100;
	_TaskInfo.RemainTaskCount = 0;
	if (_ProgressDlg) {
		_ProgressDlg->SetTaskInfo2(_TaskInfo);
	}
}

void SmTaskManager::MakeSymbolMasterList()
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

	for (auto it = master_vector.begin(); it != master_vector.end(); ++it) {
		std::string symbol_code = *it;
		std::shared_ptr<HdTaskArg> arg = std::make_shared<HdTaskArg>();
		arg->RequestID = GetRequestID();
		arg->info_text = symbol_code;
		arg->Type = HdTaskType::HdSymbolMaster;
		arg->AddArg("symbol_code", symbol_code);
		// ��û��ȣ�� �Բ� ��û ��ü�� �����Ѵ�.
		_TaskInfo.argMap[arg->RequestID] = arg;
	}
}

// ��û��ȣ�� �����ؼ� ��ȯ�Ѵ�.
int SmTaskManager::GetRequestID()
{
	return _RequestID++;
}

void SmTaskManager::MakeSymbolFileRequest()
{
	SmSymbolReader* symReader = SmSymbolReader::GetInstance();
	if (symReader->DomesticSymbolMasterFileSet.size() > 0) {
		for (auto it = symReader->DomesticSymbolMasterFileSet.begin(); it != symReader->DomesticSymbolMasterFileSet.end(); ++it) {
			std::string file_name = *it;
			std::shared_ptr<HdTaskArg> arg = std::make_shared<HdTaskArg>();
			arg->RequestID = GetRequestID();
			arg->info_text = file_name;
			arg->Type = HdTaskType::HdSymbolFileDownload;
			arg->AddArg("file_name", file_name);
			// ��û��ȣ�� �Բ� ��û ��ü�� �����Ѵ�.
			_TaskInfo.argMap[arg->RequestID] = arg;
		}
	}
}
