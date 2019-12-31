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
		// 내려받을 심볼 파일 목록을 만든다. 
		MakeSymbolFileRequest();
		_TaskInfo.TaskName = _T("국내 종목파일을 내려받고 있는 중입니다.");
		_TaskInfo.TotalTaskCount = _TaskInfo.argMap.size();
		_TaskInfo.RemainTaskCount = _TaskInfo.TotalTaskCount;
		if (_ProgressDlg) {
			_ProgressDlg->SetTaskInfo2(_TaskInfo);
		}
		// 심볼 파일을 받기 시작한다.
		std::shared_ptr<HdTaskArg> arg = _TaskInfo.argMap.begin()->second;
		VtHdClient::GetInstance()->ExecuteRequest(arg);
		_TaskState = HdTaskState::HdSymbolFileDownloading;
	}
}

// 하나의 요청이 완료되면 맵에서 요청을 찾아내어 삭제해 준다.
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
		// 국내 파일 다운로드 완료
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

		// 마스터 정보 다운로드 완료
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
	// 해외 종목 심볼 파일 완료
	if (code == 332) {
		//SmMarketManager::GetInstance()->ReadDomesticSymbolsFromFile();
		//SmMarketManager::GetInstance()->ReadAbroadSymbolsFromFile();

		// 심볼 마스터 목록을 만든다.
		MakeSymbolMasterList();

		_TaskInfo.TaskName = _T("현재가를 내려받고 있는 중입니다.");
		_TaskInfo.TotalTaskCount = _TaskInfo.argMap.size();
		_TaskInfo.RemainTaskCount = _TaskInfo.TotalTaskCount;
		if (_ProgressDlg) {
			_ProgressDlg->SetTaskInfo2(_TaskInfo);
		}
		// 심볼 마스터 정보를 받기 시작한다.
		std::shared_ptr<HdTaskArg> arg = _TaskInfo.argMap.begin()->second;
		VtHdClient::GetInstance()->ExecuteRequest(arg);
		_TaskState = HdTaskState::HdSymbolMasterDownloading;
	}
}

void SmTaskManager::StartAbroadFileDownload()
{
	_TaskInfo.TaskName = _T("해외 종목파일을 내려받고 있는 중입니다.");
	_TaskInfo.TotalTaskCount = 100;
	_TaskInfo.RemainTaskCount = 0;
	if (_ProgressDlg) {
		_ProgressDlg->SetTaskInfo2(_TaskInfo);
	}
}

void SmTaskManager::MakeSymbolMasterList()
{
	std::set<std::string> master_vector;
	// 선물 목록에서 근월물과 차월물의 코드를 읽어 온다.
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
		// 요청번호와 함께 요청 객체를 저장한다.
		_TaskInfo.argMap[arg->RequestID] = arg;
	}
}

// 요청번호를 생성해서 반환한다.
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
			// 요청번호와 함께 요청 객체를 저장한다.
			_TaskInfo.argMap[arg->RequestID] = arg;
		}
	}
}
