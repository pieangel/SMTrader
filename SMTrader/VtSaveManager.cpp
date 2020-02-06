#include "stdafx.h"
#include "VtSaveManager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <filesystem>
#include "ZmConfigManager.h"
#include "VtFundManager.h"
#include <msgpack.hpp>
#include "VtAccountManager.h"
#include "VtOrderDialogManager.h"
#include "SimpleBinStream.h"
#include "VtSymbol.h"
#include "VtOrderManagerSelector.h"
#include "HdWindowManager.h"
#include "VtStringUtil.h"
#include "File/path.h"
#include "File/resolver.h"
#include <exception>
#include "VtSystemGroupManager.h"
#include "VtStrategyWndManager.h"
#include "VtOutSystemManager.h"
#include "VtGlobal.h"
#include "cryptor.hpp"
#include "VtLoginManager.h"
#include "Log/loguru.hpp"
#include "Market/SmMarketManager.h"
//#include "Format/format_string.h"

using namespace std;

using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;

VtSaveManager::VtSaveManager()
{
}


VtSaveManager::~VtSaveManager()
{
}

void VtSaveManager::SaveAccountListToXml()
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string id = VtLoginManager::GetInstance()->ID;
	// 아이디가 없으면 그냥 반환한다.
	if (id.length() == 0)
		return;

	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));
	appPath.append(id);
	// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.

	appPath.append(_T("\\"));
	appPath.append(VtStringUtil::getTimeStr());
	appPath.append(_T(".xml"));
	filesystem::path path1(appPath);
	if (!path1.exists()) {
		return;
	}

	/// [load xml file]
	// Create empty XML document within memory
	pugi::xml_document doc;
	// Load XML file into memory
	// Remark: to fully read declaration entries you have to specify
	// "pugi::parse_declaration"
	pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
		pugi::parse_default | pugi::parse_declaration);
	if (!result) {
		return;
	}

	pugi::xml_node application = doc.child("application");

	// 	application.remove_child("login_info");
	// 	pugi::xml_node login_info = application.append_child("login_info");
	// 	VtLoginManager::GetInstance()->SaveToXml(login_info);

	application.remove_child("account_list");
	pugi::xml_node account_list = application.append_child("account_list");
	VtAccountManager::GetInstance()->SaveToXml(account_list);

	doc.save_file(appPath.c_str());
}

void VtSaveManager::WriteSettings()
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string id = VtLoginManager::GetInstance()->ID;
	// 아이디가 없으면 그냥 반환한다.
	if (id.length() == 0)
		return;

	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));
	appPath.append(id);
	// 사용자 디렉토리가 있나 검사하고 없으면 만들어 준다.

	appPath.append(_T("\\"));
	appPath.append(VtStringUtil::getTimeStr());
	appPath.append(_T(".xml"));
	filesystem::path path1(appPath);
	if (!path1.exists()) {
		/// [create new xml file]
		// Generate new XML document within memory
		pugi::xml_document doc;
		// Alternatively store as shared pointer if tree shall be used for longer
		// time or multiple client calls:
		// std::shared_ptr<pugi::xml_document> spDoc = std::make_shared<pugi::xml_document>();
		// Generate XML declaration
		auto declarationNode = doc.append_child(pugi::node_declaration);
		declarationNode.append_attribute("version") = "1.0";
		declarationNode.append_attribute("encoding") = "euc-kr";
		declarationNode.append_attribute("standalone") = "yes";
		// A valid XML doc must contain a single root node of any name
		auto root = doc.append_child("application");
		// Save XML tree to file.
		// Remark: second optional param is indent string to be used;
		// default indentation is tab character.
		bool saveSucceeded = doc.save_file(appPath.c_str(), PUGIXML_TEXT("  "));
		assert(saveSucceeded);
	}

	/// [load xml file]
	// Create empty XML document within memory
	pugi::xml_document doc;
	// Load XML file into memory
	// Remark: to fully read declaration entries you have to specify
	// "pugi::parse_declaration"
	pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
		pugi::parse_default | pugi::parse_declaration);
	if (!result)
	{
		std::cout << "Parse error: " << result.description()
			<< ", character pos= " << result.offset;
	}
	
	pugi::xml_node application = doc.child("application");

// 	application.remove_child("login_info");
// 	pugi::xml_node login_info = application.append_child("login_info");
// 	VtLoginManager::GetInstance()->SaveToXml(login_info);

	application.remove_child("account_list");
	pugi::xml_node account_list = application.append_child("account_list");
	VtAccountManager::GetInstance()->SaveToXml(account_list);

	application.remove_child("fund_list");
	pugi::xml_node fund_list = application.append_child("fund_list");
	VtFundManager::GetInstance()->SaveToXml(fund_list);

	application.remove_child("external_system_list");
	pugi::xml_node external_system_list = application.append_child("external_system_list");
	VtOutSystemManager::GetInstance()->SaveToXml(external_system_list);

	application.remove_child("system_group_list");
	pugi::xml_node system_group_list = application.append_child("system_group_list");
	VtSystemGroupManager::GetInstance()->SaveToXml(system_group_list);

	application.remove_child("order_window_list");
	pugi::xml_node window_list = application.append_child("order_window_list");
	VtOrderDialogManager::GetInstance()->SaveToXml(window_list);

	application.remove_child("stratege_window_list");
	pugi::xml_node stratege_window_list = application.append_child("stratege_window_list");
	VtStrategyWndManager::GetInstance()->SaveToXml(stratege_window_list);

	application.remove_child("etc_window_list");
	pugi::xml_node etc_window_list = application.append_child("etc_window_list");
	HdWindowManager::GetInstance()->SaveToXml(etc_window_list);

	application.remove_child("abroad_order_window_list");
	pugi::xml_node abroad_window_list = application.append_child("abroad_order_window_list");
	VtOrderDialogManager::GetInstance()->AbSaveToXml(abroad_window_list);

	doc.save_file(appPath.c_str());
}

void VtSaveManager::ReadSettings()
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string id = VtLoginManager::GetInstance()->ID;
	// 아이디가 없으면 그냥 반환한다.
	if (id.length() == 0)
		return;

	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));
	appPath.append(id);
	appPath.append(_T("\\"));
	std::string config_path = appPath;
	appPath.append(_T("*.*"));
	std::map<std::string, std::string> file_list;
	ListContents(file_list, config_path, "*.xml", false);
	
	if (file_list.size() == 0)
		return;

	std::string file_name = file_list.rbegin()->second;
	/// [load xml file]
	// Create empty XML document within memory
	pugi::xml_document doc;
	// Load XML file into memory
	// Remark: to fully read declaration entries you have to specify
	// "pugi::parse_declaration"
	pugi::xml_parse_result result = doc.load_file(file_name.c_str(),
		pugi::parse_default | pugi::parse_declaration);
	if (!result)
	{
		std::cout << "Parse error: " << result.description()
			<< ", character pos= " << result.offset;
	}

	pugi::xml_node application = doc.child("application");
// 	pugi::xml_node login_info = application.child("login_info");
// 	if (login_info) {
// 		VtLoginManager::GetInstance()->LoadFromXml(login_info);
// 	}
	pugi::xml_node account_list = application.child("account_list");
	if (account_list) {
		VtAccountManager::GetInstance()->LoadFromXml(account_list);
	}

	pugi::xml_node fund_list = application.child("fund_list");
	if (fund_list) {
		VtFundManager::GetInstance()->LoadFromXml(fund_list);
	}

	/*
	pugi::xml_node order_window_list = application.child("order_window_list");
	if (order_window_list) {
		VtOrderDialogManager::GetInstance()->LoadFromXml(order_window_list);
	}

	pugi::xml_node stratege_window_list = application.child("stratege_window_list");
	if (stratege_window_list) {
		VtStrategyWndManager::GetInstance()->LoadFromXml(stratege_window_list);
	}

	pugi::xml_node etc_window_list = application.child("etc_window_list");
	if (etc_window_list) {
		HdWindowManager::GetInstance()->LoadFromXml(etc_window_list);
	}
	*/
}

bool VtSaveManager::ListContents(std::map<std::string, std::string>& dest, std::string dir, std::string filter, bool recursively)
{
	WIN32_FIND_DATAA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;

	// Prepare string
	//if (dir.back() != '\\') dir += "\\";

	// Safety check
	if (dir.length() >= MAX_PATH) {
		LOG_F(INFO, "Cannot open folder %s: path too long", dir.c_str());
		return false;
	}

	// First entry in directory
	hFind = FindFirstFileA((dir + filter).c_str(), &ffd);

	if (hFind == INVALID_HANDLE_VALUE) {
		LOG_F(INFO, "Cannot open folder in folder %s: error accessing first entry.", dir.c_str());
		return false;
	}

	// List files in directory
	do {
		// Ignore . and .. folders, they cause stack overflow
		if (strcmp(ffd.cFileName, ".") == 0) continue;
		if (strcmp(ffd.cFileName, "..") == 0) continue;

		
		// Is directory?
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// Go inside recursively
			//if (recursively)
			//	ListContents(dest, dir + ffd.cFileName, filter, recursively, content_type);
			continue;
		}
		// Add file to our list
		else {
			
			SYSTEMTIME stLocal;
			
			// Convert the last-write time to local time.
			FileTimeToSystemTime(&ffd.ftLastWriteTime, &stLocal);
			std::string local_time;
			local_time += std::to_string(stLocal.wYear);
			local_time += std::to_string(stLocal.wMonth);
			local_time += std::to_string(stLocal.wDay);
			CString local_file_time;
			local_file_time.Format("%04d%02d%02d", stLocal.wYear, stLocal.wMonth, stLocal.wDay);
			dest[(LPCTSTR)local_file_time] = dir + ffd.cFileName;
		}

	} while (FindNextFileA(hFind, &ffd));

	// Get last error
	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES) {
		LOG_F(INFO, "Error reading file list in folder %s.", dir.c_str());
		return false;
	}

	return true;
}

void VtSaveManager::ReadWindows()
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string id = VtLoginManager::GetInstance()->ID;
	// 아이디가 없으면 그냥 반환한다.
	if (id.length() == 0)
		return;
	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));
	appPath.append(id);
	appPath.append(_T("\\"));
	std::string config_path = appPath;
	appPath.append(_T("*.*"));
	std::map<std::string, std::string> file_list;
	ListContents(file_list, config_path, "*.xml", false);

	if (file_list.size() == 0)
		return;

	std::string file_name = file_list.rbegin()->second;
	/// [load xml file]
	// Create empty XML document within memory
	pugi::xml_document doc;
	// Load XML file into memory
	// Remark: to fully read declaration entries you have to specify
	// "pugi::parse_declaration"
	pugi::xml_parse_result result = doc.load_file(file_name.c_str(),
		pugi::parse_default | pugi::parse_declaration);
	if (!result)
	{
		std::cout << "Parse error: " << result.description()
			<< ", character pos= " << result.offset;
	}

	pugi::xml_node application = doc.child("application");

	pugi::xml_node system_group_list = application.child("system_group_list");
	if (system_group_list) {
		VtSystemGroupManager::GetInstance()->LoadFromXml(system_group_list);
	}


	pugi::xml_node external_system_list = application.child("external_system_list");
	if (external_system_list) {
		VtOutSystemManager::GetInstance()->LoadFromXml(external_system_list);
	}
	
	pugi::xml_node order_window_list = application.child("order_window_list");
	if (order_window_list) {
		VtOrderDialogManager::GetInstance()->LoadFromXml(order_window_list);
	}

	pugi::xml_node stratege_window_list = application.child("stratege_window_list");
	if (stratege_window_list) {
		VtStrategyWndManager::GetInstance()->LoadFromXml(stratege_window_list);
	}

	pugi::xml_node etc_window_list = application.child("etc_window_list");
	if (etc_window_list) {
		HdWindowManager::GetInstance()->LoadFromXml(etc_window_list);
	}

	pugi::xml_node abroad_order_window_list = application.child("abroad_order_window_list");
	if (abroad_order_window_list) {
		VtOrderDialogManager::GetInstance()->AbLoadFromXml(abroad_order_window_list);
	}
}

std::string VtSaveManager::GetLastestFile(std::string file_name)
{
	FILETIME bestDate = { 0, 0 };
	FILETIME curDate;
	string name = "";
	CFileFind finder;

	finder.FindFile(file_name.c_str());
	while (finder.FindNextFile())
	{
		finder.GetCreationTime(&curDate);

		if (CompareFileTime(&curDate, &bestDate) > 0)
		{
			bestDate = curDate;
			name = finder.GetFileName().GetString();
			AfxMessageBox(name.c_str());
		}
	}
	return name;
}

void VtSaveManager::SaveOrders()
{
	SaveOrders(_T("orderlist.dat"));
}

void VtSaveManager::SaveOrders(std::string fileName)
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));
	appPath.append(VtGlobal::LoginUserID);
	appPath.append(_T("\\"));
	appPath.append(fileName);

	if (!fexists(appPath)) {
		std::ofstream newFile(appPath);
		newFile.close();
	}

	simple::file_ostream<same_endian_type> outfile(appPath.c_str());
}

void VtSaveManager::SaveFundList(std::string fileName)
{
	
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));
	appPath.append(VtGlobal::LoginUserID);
	appPath.append(_T("\\"));
	appPath.append(fileName);	

	filesystem::path path1(appPath);
	if (!path1.exists()) {
		std::ofstream outfile(appPath);
		outfile.close();
	}

	simple::file_ostream<same_endian_type> outfile(appPath.c_str());
	
	VtFundManager* fundMgr = VtFundManager::GetInstance();
	fundMgr->Save(outfile);
	outfile.flush();
	outfile.close();
}

void VtSaveManager::SaveFundList()
{
	SaveFundList(_T("fundlist.dat"));
}

void VtSaveManager::LoadFundList(std::string fileName)
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));
	appPath.append(VtGlobal::LoginUserID);
	appPath.append(_T("\\"));
	appPath.append(fileName);

	try {
		filesystem::path path1(appPath);
		if (!path1.exists()) {
			std::ofstream outfile(appPath);
			outfile.close();
		}

		std::string name;
		name = path1.filename();

		simple::file_istream<same_endian_type> in(appPath.c_str());
		if (in.file_length() == 0)
			return;
		VtFundManager* fundMgr = VtFundManager::GetInstance();

		fundMgr->Load(in);
	}
	catch (std::exception& e) {
		std::string error = e.what();
		LOG_F(INFO, _T("Error : %s in LoadFundList"), error);
	}
}

void VtSaveManager::SaveAccountList(std::string fileName)
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));
	appPath.append(VtGlobal::LoginUserID);
	appPath.append(_T("\\"));
	appPath.append(fileName);
	simple::file_ostream<same_endian_type> outfile(appPath.c_str());

	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	acntMgr->Save(outfile);

	outfile.flush();
	outfile.close();
}

void VtSaveManager::SaveAccountList()
{
	SaveAccountList(_T("accountlist.dat"));
}

void VtSaveManager::LoadAccountList(std::string fileName)
{
	try {
		ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
		std::string appPath;
		appPath = configMgr->GetAppPath();
		appPath.append(_T("\\"));
		appPath.append(VtGlobal::LoginUserID);
		appPath.append(_T("\\"));
		appPath.append(fileName);

		filesystem::path path1(appPath);
		if (!path1.exists()) {
			std::ofstream outfile(appPath);
			outfile.close();
		}

		simple::file_istream<same_endian_type> in(appPath.c_str());
		if (in.file_length() == 0)
			return;

		VtAccountManager* acntMgr = VtAccountManager::GetInstance();
		acntMgr->Load(in);
	} catch (std::exception& e) {
		std::string error = e.what();
		LOG_F(INFO, _T("Error : %s in LoadAccountList"), error);
	}
}

void VtSaveManager::SaveOrderWndList(std::string fileName, CMainFrame* mainFrm)
{
	try {
		ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
		std::string appPath;
		appPath = configMgr->GetAppPath();
		appPath.append(_T("\\"));
		appPath.append(VtGlobal::LoginUserID);
		appPath.append(_T("\\"));
		appPath.append(fileName);
		simple::file_ostream<same_endian_type> outfile(appPath.c_str());
		std::string seperator;
		seperator = _T("order_wnd_list_begin");
		outfile << seperator;
		VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
		orderDlgMgr->MainFrm(mainFrm);
		orderDlgMgr->Save(outfile);
		seperator = _T("order_wnd_list_end");
		outfile << seperator;

		seperator = _T("strategy_wnd_list_begin");
		outfile << seperator;
		VtStrategyWndManager* stgWndMgr = VtStrategyWndManager::GetInstance();
		stgWndMgr->Save(outfile);
		seperator = _T("strategy_wnd_list_end");
		outfile << seperator;

		seperator = _T("others_wnd_list_begin");
		outfile << seperator;
		HdWindowManager* dlgMgr = HdWindowManager::GetInstance();
		dlgMgr->MainFrm(mainFrm);
		dlgMgr->Save(outfile);
		seperator = _T("others_wnd_list_end");
		outfile << seperator;

		outfile.flush();
		outfile.close();
	} catch (std::exception& e) {
		std::string error = e.what();
		LOG_F(INFO, _T("Error : %s in SaveOrderWndList"), error);
	}
}

void VtSaveManager::LoadOrderWndList(std::string fileName, CMainFrame* mainFrm)
{
	if (!mainFrm)
		return;

	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));
	appPath.append(VtGlobal::LoginUserID);
	appPath.append(_T("\\"));
	appPath.append(fileName);

	filesystem::path path1(appPath);
	if (!path1.exists()) {
		std::ofstream outfile(appPath);
		outfile.close();
	}

	simple::file_istream<same_endian_type> in(appPath.c_str());
	if (in.file_length() == 0)
		return;
	std::string seperator;
	in >> seperator;
	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	orderDlgMgr->MainFrm(mainFrm);
	orderDlgMgr->Load(in);
	in >> seperator;

	in >> seperator;
	VtStrategyWndManager* stgWndMgr = VtStrategyWndManager::GetInstance();
	stgWndMgr->Load(in);
	in >> seperator;

	in >> seperator;
	HdWindowManager* dlgMgr = HdWindowManager::GetInstance();
	dlgMgr->MainFrm(mainFrm);
	dlgMgr->Load(in);
	in >> seperator;

	GetSymbolMasters();
}

void VtSaveManager::SaveDialogList(std::string fileName, CMainFrame* mainFrm)
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));
	appPath.append(VtGlobal::LoginUserID);
	appPath.append(_T("\\"));
	appPath.append(fileName);
	simple::file_ostream<same_endian_type> outfile(appPath.c_str());

	HdWindowManager* dlgMgr = HdWindowManager::GetInstance();
	dlgMgr->MainFrm(mainFrm);
	dlgMgr->Save(outfile);

	outfile.flush();
	outfile.close();
}

void VtSaveManager::LoadDialogList(std::string fileName, CMainFrame* mainFrm)
{
	if (!mainFrm)
		return;

	try {
		ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
		std::string appPath;
		appPath = configMgr->GetAppPath();
		appPath.append(_T("\\"));
		appPath.append(VtGlobal::LoginUserID);
		appPath.append(_T("\\"));
		appPath.append(fileName);

		simple::file_istream<same_endian_type> in(appPath.c_str());
		if (in.file_length() == 0)
			return;

		HdWindowManager* dlgMgr = HdWindowManager::GetInstance();
		dlgMgr->MainFrm(mainFrm);
		dlgMgr->Load(in);
	} catch (std::exception& e) {
		std::string error = e.what();
		LOG_F(INFO, _T("Error : %s in LoadDialogList"), error);
	}
}

void VtSaveManager::SaveTotal(std::string fileName, CMainFrame* mainFrm)
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));
	appPath.append(VtGlobal::LoginUserID);
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
	// 파일을 열어 저장을 시작한다.
	simple::file_ostream<same_endian_type> outfile(appPath.c_str());

	VtFundManager* fundMgr = VtFundManager::GetInstance();
	fundMgr->Save(outfile);

	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	acntMgr->Save(outfile);

	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	orderDlgMgr->MainFrm(mainFrm);
	orderDlgMgr->Save(outfile);

	HdWindowManager* dlgMgr = HdWindowManager::GetInstance();
	dlgMgr->MainFrm(mainFrm);
	dlgMgr->Save(outfile);

	// 저장을 끝내고 파일을 닫는다.
	outfile.flush();
	outfile.close();
}

void VtSaveManager::SaveLoginInfo(std::string fileName, std::string id, std::string pwd, std::string cert, bool save)
{
	try {
		ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
		std::string appPath;
		appPath = configMgr->GetAppPath();
		appPath.append(_T("\\"));
		//appPath.append(VtGlobal::LoginUserID);
		//appPath.append(_T("\\"));
		appPath.append(fileName);
		simple::file_ostream<same_endian_type> outfile(appPath.c_str());

		auto enc_id = cryptor::encrypt(move(id));
		auto enc_pwd = cryptor::encrypt(move(pwd));
		auto enc_cert = cryptor::encrypt(move(cert));

		std::string nice = _T("");
		auto test = cryptor::decrypt(nice);

		outfile << enc_id;
		outfile << enc_pwd;
		outfile << enc_cert;
		outfile << save;

		outfile.flush();
		outfile.close();
	}
	catch (std::exception& e) {
		std::string error = e.what();
		LOG_F(INFO, _T("Error : %s in SaveOrderWndList"), error);
	}
}

void VtSaveManager::LoadLoginInfo(std::string fileName, std::string& id, std::string& pwd, std::string& cert, bool& save)
{
	try {
		ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
		std::string appPath;
		appPath = configMgr->GetAppPath();
		appPath.append(_T("\\"));
		//appPath.append(VtGlobal::LoginUserID);
		//appPath.append(_T("\\"));
		appPath.append(fileName);

		filesystem::path path1(appPath);
		if (!path1.exists()) { // 디렉토리가 존재하지 않을 경우
			std::ofstream outfile(appPath);
			outfile.close();
		}

		simple::file_istream<same_endian_type> in(appPath.c_str());
		if (in.file_length() == 0)
			return;
		std::string dec_id, dec_pwd, dec_cert;
		in >> dec_id;
		in >> dec_pwd;
		in >> dec_cert;
		in >> save;

		id = cryptor::decrypt(dec_id);
		pwd = cryptor::decrypt(dec_pwd);
		cert = cryptor::decrypt(dec_cert);
	}
	catch (std::exception& e) {
		std::string error = e.what();
		LOG_F(INFO, _T("Error : %s in LoginInfo"), error);
	}
}

void VtSaveManager::SaveSystems(std::string fileName)
{
	try {
		ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
		std::string appPath;
		appPath = configMgr->GetAppPath();
		appPath.append(_T("\\"));
		appPath.append(VtGlobal::LoginUserID);
		appPath.append(_T("\\"));
		appPath.append(fileName);

		filesystem::path path1(appPath);
		if (!path1.exists()) { // 디렉토리가 존재하지 않을 경우
			std::ofstream outfile(appPath);
			outfile.close();
		}
		simple::file_ostream<same_endian_type> outfile(appPath.c_str());

		VtSystemGroupManager* sysGrpMgr = VtSystemGroupManager::GetInstance();
		sysGrpMgr->Save(outfile);

		// 저장을 끝내고 파일을 닫는다.
		outfile.flush();
		outfile.close();
	}
	catch (std::exception& e) {
		std::string error = e.what();
		LOG_F(INFO, _T("Error : %s in LoginInfo"), error);
	}
}

void VtSaveManager::LoadSystems(std::string fileName)
{
	try {
		ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
		std::string appPath;
		appPath = configMgr->GetAppPath();
		appPath.append(_T("\\"));
		appPath.append(VtGlobal::LoginUserID);
		appPath.append(_T("\\"));
		appPath.append(fileName);

		filesystem::path path1(appPath);
		if (!path1.exists()) { // 디렉토리가 존재하지 않을 경우
			std::ofstream outfile(appPath);
			outfile.close();
		}

		VtSystemGroupManager* sysGrpMgr = VtSystemGroupManager::GetInstance();
		simple::file_istream<same_endian_type> in(appPath.c_str());
		if (in.file_length() == 0) {
			sysGrpMgr->InitSystemGroup();
			return;
		}

		sysGrpMgr->Load(in);
	}
	catch (std::exception& e) {
		std::string error = e.what();
		LOG_F(INFO, _T("Error : %s in LoginInfo"), error);
	}
}

void VtSaveManager::SaveOutSystems(std::string fileName)
{
	try {
		ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
		std::string appPath;
		appPath = configMgr->GetAppPath();
		appPath.append(_T("\\"));
		appPath.append(VtGlobal::LoginUserID);
		appPath.append(_T("\\"));
		appPath.append(fileName);

		filesystem::path path1(appPath);
		if (!path1.exists()) { // 디렉토리가 존재하지 않을 경우
			std::ofstream outfile(appPath);
			outfile.close();
		}
		simple::file_ostream<same_endian_type> outfile(appPath.c_str());

		VtOutSystemManager* outSysMgr = VtOutSystemManager::GetInstance();
		outSysMgr->Save(outfile);

		// 저장을 끝내고 파일을 닫는다.
		outfile.flush();
		outfile.close();
	}
	catch (std::exception& e) {
		std::string error = e.what();
		LOG_F(INFO, _T("Error : %s in LoginInfo"), error);
	}
}

void VtSaveManager::LoadOutSystems(std::string fileName)
{
	try {
		ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
		std::string appPath;
		appPath = configMgr->GetAppPath();
		appPath.append(_T("\\"));
		appPath.append(VtGlobal::LoginUserID);
		appPath.append(_T("\\"));
		appPath.append(fileName);

		filesystem::path path1(appPath);
		if (!path1.exists()) { // 디렉토리가 존재하지 않을 경우
			std::ofstream outfile(appPath);
			outfile.close();
		}

		
		simple::file_istream<same_endian_type> in(appPath.c_str());
		if (in.file_length() == 0) {
			return;
		}

		VtOutSystemManager* outSysMgr = VtOutSystemManager::GetInstance();
		outSysMgr->Load(in);

	}
	catch (std::exception& e) {
		std::string error = e.what();
		LOG_F(INFO, _T("Error : %s in LoginInfo"), error);
	}
}

void VtSaveManager::SaveOutSignal(std::string fileName)
{

}

void VtSaveManager::LoadOutSignal(std::string fileName)
{

}

void VtSaveManager::CreateUserDirectory()
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));
	appPath.append(VtGlobal::LoginUserID);
	filesystem::path path1(appPath);
	if (!path1.exists()) { // 디렉토리가 존재하지 않을 경우
		filesystem::create_directory(path1);
	}
}

bool VtSaveManager::IsAccountFileExist()
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));
	appPath.append(VtGlobal::LoginUserID);
	appPath.append(_T("\\"));
	appPath.append(_T("accountlist.dat"));

	filesystem::path path1(appPath);
	if (path1.exists()) {
		simple::file_istream<same_endian_type> in(appPath.c_str());
		if (in.file_length() == 0)
			return false;
		else
			return true;
	}
	else
		return false;
}

void VtSaveManager::SaveLoginInfoToXml(std::string id, std::string pwd, std::string cert, bool save)
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));
	appPath.append(_T("config.xml"));

	/// [load xml file]
	// Create empty XML document within memory
	pugi::xml_document doc;
	// Load XML file into memory
	// Remark: to fully read declaration entries you have to specify
	// "pugi::parse_declaration"
	pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
		pugi::parse_default | pugi::parse_declaration);
	if (!result)
	{
		std::cout << "Parse error: " << result.description()
			<< ", character pos= " << result.offset;
		return ;
	}

	pugi::xml_node application = doc.child("application");
	application.remove_child("login_info");
	pugi::xml_node login_info = application.append_child("login_info");
	login_info.append_attribute("save") = save;

	auto enc_id = cryptor::encrypt(move(id));
	auto enc_pwd = cryptor::encrypt(move(pwd));
	auto enc_cert = cryptor::encrypt(move(cert));

	pugi::xml_node login_info_child = login_info.append_child("login_id");
	login_info_child.append_child(pugi::node_pcdata).set_value(enc_id.c_str());

	login_info_child = login_info.append_child("login_password");
	login_info_child.append_child(pugi::node_pcdata).set_value(enc_pwd.c_str());

	login_info_child = login_info.append_child("login_cert");
	login_info_child.append_child(pugi::node_pcdata).set_value(enc_cert.c_str());

	doc.save_file(appPath.c_str());
}

int VtSaveManager::LoadLoginInfoFromXml(std::string& id, std::string& pwd, std::string& cert, bool& save)
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));
	appPath.append(_T("config.xml"));

	/// [load xml file]
	// Create empty XML document within memory
	pugi::xml_document doc;
	// Load XML file into memory
	// Remark: to fully read declaration entries you have to specify
	// "pugi::parse_declaration"
	pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
		pugi::parse_default | pugi::parse_declaration);
	if (!result)
	{
		std::cout << "Parse error: " << result.description()
			<< ", character pos= " << result.offset;
		return -1;
	}

	pugi::xml_node application = doc.child("application");
	pugi::xml_node login_info = application.child("login_info");
	if (login_info) {
		bool save_value = login_info.attribute("save").as_bool();
		save_value == 1 ? save = true : save = false;
		id = login_info.child_value("login_id");
		pwd = login_info.child_value("login_password");
		cert = login_info.child_value("login_cert");
		return 1;
	}
	else {
		return -1;
	}
}

int VtSaveManager::LoadLoginInfoFromXml()
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));
	appPath.append(_T("config.xml"));

	/// [load xml file]
	// Create empty XML document within memory
	pugi::xml_document doc;
	// Load XML file into memory
	// Remark: to fully read declaration entries you have to specify
	// "pugi::parse_declaration"
	pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
		pugi::parse_default | pugi::parse_declaration);
	if (!result)
	{
		// 설정 파일이 없을 때
		std::cout << "Parse error: " << result.description()
			<< ", character pos= " << result.offset;
		return -1;
	}

	pugi::xml_node application = doc.child("application");
	pugi::xml_node login_info = application.child("login_info");
	if (login_info) {
		VtLoginManager* loginMgr = VtLoginManager::GetInstance();
		loginMgr->Save = login_info.attribute("save").as_bool();
		std::string id = login_info.child_value("login_id");
		std::string pwd = login_info.child_value("login_password");
		std::string cert = login_info.child_value("login_cert");

		loginMgr->ID = cryptor::decrypt(id);
		loginMgr->Password = cryptor::decrypt(pwd);
		loginMgr->Cert = cryptor::decrypt(cert);
		return 1;
	}
	else {
		// 설정 파일에서 로그인 항목이 없을 때
		return -1;
	}
}

void VtSaveManager::LoadRunInfoFromXml()
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));
	appPath.append(_T("config.xml"));

	/// [load xml file]
	// Create empty XML document within memory
	pugi::xml_document doc;
	// Load XML file into memory
	// Remark: to fully read declaration entries you have to specify
	// "pugi::parse_declaration"
	pugi::xml_parse_result result = doc.load_file(appPath.c_str(),
		pugi::parse_default | pugi::parse_declaration);
	if (!result)
	{
		// 설정 파일이 없을 때
		std::cout << "Parse error: " << result.description()
			<< ", character pos= " << result.offset;
		return;
	}

	pugi::xml_node application = doc.child("application");
	pugi::xml_node run_info = application.child("run_info");
	if (run_info) {
		VtGlobal::OpenTime.hour = std::stoi(run_info.child_value("start_hour"));
		VtGlobal::OpenTime.min = std::stoi(run_info.child_value("start_min"));
		VtGlobal::OpenTime.sec = std::stoi(run_info.child_value("start_sec"));
		VtGlobal::CloseTime.hour = std::stoi(run_info.child_value("end_hour"));
		VtGlobal::CloseTime.min = std::stoi(run_info.child_value("end_min"));
		VtGlobal::CloseTime.sec = std::stoi(run_info.child_value("end_sec"));

		VtGlobal::ServerSleepTime = std::stoi(run_info.child_value("server_sleep_time"));
		std::string default_abroad_product_code = run_info.child_value("default_abroad_product_code");
		SmMarketManager::GetInstance()->DefaultAbroadProductCode(default_abroad_product_code);
		std::string default_abroad_market_name = run_info.child_value("default_abroad_market_name");
		SmMarketManager::GetInstance()->DefaultAbroadMarketName(default_abroad_market_name);
	}

	pugi::xml_node file_watch = application.child("file_watch");
	if (file_watch) {
		VtGlobal::GetInstance()->EnableFileWatch = file_watch.attribute("enable").as_bool();
		VtGlobal::GetInstance()->FileWatchPath = file_watch.child_value("file_watch_path");
	}
}

void VtSaveManager::GetWindowSymbolList(std::set<std::string>& symbol_list)
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string id = VtLoginManager::GetInstance()->ID;
	// 아이디가 없으면 그냥 반환한다.
	if (id.length() == 0)
		return ;

	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));
	appPath.append(id);
	appPath.append(_T("\\"));
	std::string config_path = appPath;
	appPath.append(_T("*.*"));
	std::map<std::string, std::string> file_list;
	ListContents(file_list, config_path, "*.xml", false);

	if (file_list.size() == 0)
		return ;

	std::string file_name = file_list.rbegin()->second;
	
	pugi::xml_document doc;
	
	pugi::xml_parse_result result = doc.load_file(file_name.c_str(),
		pugi::parse_default | pugi::parse_declaration);
	if (!result)
	{
		std::cout << "Parse error: " << result.description()
			<< ", character pos= " << result.offset;
	}

	pugi::xml_node application = doc.child("application");
	
	pugi::xml_node order_window_list_node = application.child("order_window_list");
	if (!order_window_list_node)
		return ;

	for (pugi::xml_node order_window_node = order_window_list_node.child("order_window"); order_window_node; order_window_node = order_window_node.next_sibling("order_window")) {
		pugi::xml_node center_window_list_node = order_window_node.child("center_window_list");
		if (center_window_list_node) {
			for (pugi::xml_node center_window_node = center_window_list_node.child("center_window"); center_window_node; center_window_node = center_window_node.next_sibling("center_window")) {
				std::string symbol_code = center_window_node.child_value("symbol_code");
				symbol_list.insert(symbol_code);
			}
		}
	}

	
}

void VtSaveManager::GetWindowAbSymbolList(std::set<std::string>& symbol_list)
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string id = VtLoginManager::GetInstance()->ID;
	// 아이디가 없으면 그냥 반환한다.
	if (id.length() == 0)
		return;

	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));
	appPath.append(id);
	appPath.append(_T("\\"));
	std::string config_path = appPath;
	appPath.append(_T("*.*"));
	std::map<std::string, std::string> file_list;
	ListContents(file_list, config_path, "*.xml", false);

	if (file_list.size() == 0)
		return;

	std::string file_name = file_list.rbegin()->second;

	pugi::xml_document doc;

	pugi::xml_parse_result result = doc.load_file(file_name.c_str(),
		pugi::parse_default | pugi::parse_declaration);
	if (!result)
	{
		std::cout << "Parse error: " << result.description()
			<< ", character pos= " << result.offset;
	}

	pugi::xml_node application = doc.child("application");

	pugi::xml_node order_window_list_node = application.child("abroad_order_window_list");
	if (!order_window_list_node)
		return;

	for (pugi::xml_node order_window_node = order_window_list_node.child("order_window"); order_window_node; order_window_node = order_window_node.next_sibling("order_window")) {
		pugi::xml_node center_window_list_node = order_window_node.child("center_window_list");
		if (center_window_list_node) {
			for (pugi::xml_node center_window_node = center_window_list_node.child("center_window"); center_window_node; center_window_node = center_window_node.next_sibling("center_window")) {
				std::string symbol_code = center_window_node.child_value("symbol_code");
				symbol_list.insert(symbol_code);
			}
		}
	}
}

void VtSaveManager::GetSymbolMasters()
{
	for (auto it = _SymbolVector.begin(); it != _SymbolVector.end(); ++it) {
		VtSymbol* sym = *it;
		sym->GetSymbolMaster();
	}
}
