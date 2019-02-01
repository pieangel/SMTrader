#include "stdafx.h"
#include "VtLogManager.h"
#include "Log/loguru.hpp"
#include "ZmConfigManager.h"
#include "VtStringUtil.h"
#include "File/path.h"
#include "File/resolver.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdint>

VtLogManager::VtLogManager()
{
}


VtLogManager::~VtLogManager()
{
}

void VtLogManager::InitLog()
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string fileName = _T("SmTrader.log");
	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));

	std::string dirName = appPath;

	// 오늘날짜 디렉토리 이름을 만든다.
	dirName.append(VtStringUtil::getTimeStr());
	filesystem::path path1(dirName);
	if (!path1.exists()) { // 디렉토리가 존재하지 않을 경우
						   // 오늘 날짜로 디렉토리 생성
		filesystem::create_directory(path1);
		// 그 디렉토리 밑에 파일을 만든다.
		dirName.append(_T("\\"));
		dirName.append(fileName);
		std::ofstream outfile(dirName);
		outfile.close();
	}
	appPath.append(VtStringUtil::getTimeStr());
	appPath.append(_T("\\"));
	appPath.append(fileName);

	// Put every log message in "everything.log":
	loguru::add_file(appPath.c_str(), loguru::Append, loguru::Verbosity_MAX);

	// Only log INFO, WARNING, ERROR and FATAL to "latest_readable.log":
	//loguru::add_file("latest_readable.log", loguru::Truncate, loguru::Verbosity_INFO);

	// Only show most relevant things on stderr:
	loguru::g_stderr_verbosity = 1;

	LOG_F(INFO, _T("Log manager was created!"));
	
}
