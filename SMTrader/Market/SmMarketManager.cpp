#include "afx.h"
#include "SmMarketManager.h"
#include <string>
#include "../Xml/pugixml.hpp"
#include "SmSymbolReader.h"
#include "SmMarket.h"
#include "SmProduct.h"
#include <chrono>
#include "SmProduct.h"
#include "SmProductYearMonth.h"
#include "../ZmConfigManager.h"
#include <iostream>
#include "../VtSymbol.h"
#include "../VtStringUtil.h"
#include "../VtGlobal.h"
#include "../Log/loguru.hpp"

SmMarketManager::SmMarketManager()
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));
	appPath.append(_T("env"));
	appPath.append(_T("\\"));
	appPath.append(_T("config.xml"));
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
		return;
	}

	pugi::xml_node application = doc.child("application");

	pugi::xml_node symbol_file_list_node = doc.child("symbol_file_list");

	pugi::xml_node domestic_file_node = symbol_file_list_node.child("domestic_symbol_file");

	int index = 0;
	for (auto it = domestic_file_node.begin(); it != domestic_file_node.end(); ++it) {
		std::string file_name = it->text().as_string();
		TRACE(file_name.c_str());
		_DomesticList.insert(file_name);
	}
}


SmMarketManager::~SmMarketManager()
{
	for (auto it = _MarketList.begin(); it != _MarketList.end(); ++it) {
		delete* it;
	}
}

void SmMarketManager::ReadAbroadSymbolsFromFile()
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));
	std::string config_path = appPath;
	config_path.append("config.xml");
	appPath.append(_T("mst"));
	appPath.append(_T("\\"));
	pugi::xml_document doc;

	pugi::xml_parse_result result = doc.load_file(config_path.c_str());
	pugi::xml_node app = doc.child("application");
	pugi::xml_node sym_file_list = app.child("symbol_file_list");
	pugi::xml_node domestic_list = sym_file_list.child("abroad_symbol_file");
	int index = 11;
	for (auto it = domestic_list.begin(); it != domestic_list.end(); ++it) {
		std::string file_name = it->text().as_string();
		TRACE(file_name.c_str());
		std::string file_path = appPath + file_name;
		SmSymbolReader::GetInstance()->ReadSymbolFromFile(index++, file_path);
	}
}

void SmMarketManager::ReadDomesticSymbolsFromFile()
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string appPath;
	appPath = configMgr->GetAppPath();
	appPath.append(_T("\\"));
	std::string config_path = appPath;
	config_path.append("config.xml");
	appPath.append(_T("mst"));
	appPath.append(_T("\\"));
	pugi::xml_document doc;

	pugi::xml_parse_result result = doc.load_file(config_path.c_str());
	pugi::xml_node app = doc.child("application");
	pugi::xml_node sym_file_list = app.child("symbol_file_list");
	pugi::xml_node domestic_list = sym_file_list.child("domestic_symbol_file");
	int index = 0;
	for (auto it = domestic_list.begin(); it != domestic_list.end(); ++it) {
		std::string file_name = it->text().as_string();
		TRACE(file_name.c_str());
		std::string file_path = appPath + file_name;
		SmSymbolReader::GetInstance()->ReadSymbolFromFile(index++, file_path);
	}
}

SmMarket* SmMarketManager::AddMarket(std::string name)
{
	SmMarket* found_market = FindMarket(name);
	if (found_market)
		return found_market;

	SmMarket* market = new SmMarket();
	market->Name(name);
	_MarketList.push_back(market);
	return market;
}

SmProduct* SmMarketManager::FindProduct(std::string mrkt_name, std::string cat_code)
{
	SmMarket* cur_market = FindMarket(mrkt_name);
	if (!cur_market)
		return nullptr;
	return cur_market->FindProduct(cat_code);
}

SmProduct* SmMarketManager::FindProduct(std::string cat_code)
{
	auto it = _CategoryToMarketMap.find(cat_code);
	if (it != _CategoryToMarketMap.end()) {
		std::string market_name = it->second;
		SmMarket* mrkt = FindMarket(market_name);
		if (mrkt)
			return mrkt->FindProduct(cat_code);
		else
			return nullptr;
	}

	return nullptr;
}

std::vector<VtSymbol*> SmMarketManager::GetRecentMonthSymbolList()
{
	std::vector<VtSymbol*> symvec;
	
	for (auto it = _MarketList.begin(); it != _MarketList.end(); ++it) {
		SmMarket* mrkt = *it;
		auto cat_vec = mrkt->GetProductList();
		for (auto itc = cat_vec.begin(); itc != cat_vec.end(); ++itc) {
			if (!IsInRunList((*itc)->Code())) {
				continue;
			}
			SmProductYearMonth* ym = (*itc)->GetRecentYearMonth();
			if (ym) {
				if (ym->ProductCode.compare("175") == 0) {
					VtSymbol* recent_symbol = GetRecentSymbol(ym->ProductCode);
					std::string current_date = VtStringUtil::getCurentDate();
					if (current_date.compare(recent_symbol->LastDate) >= 0) {
						ym = (*itc)->GetNextYearMonth();
					}
				}
				std::vector<VtSymbol*> symbol_list = ym->SymbolList();
				for (auto itym = symbol_list.begin(); itym != symbol_list.end(); ++itym) {
					(*itym)->Quote.shortCode = (*itym)->ShortCode;
					symvec.push_back(*itym);
				}
			}
		}
	}

	return symvec;
}

VtSymbol* SmMarketManager::GetRecentSymbol(std::string market_name, std::string product_name)
{
	SmMarket* market = FindMarket(market_name);
	if (!market)
		return nullptr;
	SmProduct* product = market->FindProduct(product_name);
	if (!product)
		return nullptr;
	SmProductYearMonth* ym = product->GetRecentYearMonth();
	std::vector<VtSymbol*> symbol_list = ym->SymbolList();
	if (symbol_list.size() == 0)
		return nullptr;
	return *symbol_list.begin();
}

VtSymbol* SmMarketManager::GetRecentSymbol(std::string product_name)
{
	auto it = _CategoryToMarketMap.find(product_name);
	if (it == _CategoryToMarketMap.end())
		return nullptr;
	std::string market_name = it->second;

	return GetRecentSymbol(market_name, product_name);
}

void SmMarketManager::SendMarketList(std::string user_id)
{
	
}

void SmMarketManager::SendSymbolListByCategory(std::string user_id)
{
	for (size_t i = 0; i < _MarketList.size(); ++i) {
		SmMarket* market = _MarketList[i];
		std::vector<SmProduct*>& cat_list = market->GetProductList();
		for (size_t j = 0; j < cat_list.size(); ++j) {
			SmProduct* cat = cat_list[j];
			std::vector<VtSymbol*>& sym_list = cat->GetSymbolList();
			for (size_t k = 0; k < sym_list.size(); ++k) {
				SendSymbolMaster(user_id, sym_list[k]);
			}
		}
	}
}

int SmMarketManager::GetTotalCategoryCount()
{
	int total = 0;
	for (size_t i = 0; i < _MarketList.size(); ++i) {
		SmMarket* market = _MarketList[i];
		total += market->GetProductList().size();
	}

	return total;
}

int SmMarketManager::GetTotalSymbolCount()
{
	int total = 0;
	for (size_t i = 0; i < _MarketList.size(); ++i) {
		SmMarket* market = _MarketList[i];
		std::vector<SmProduct*>& cat_list = market->GetProductList();
		for (size_t j = 0; j < cat_list.size(); ++j) {
			total += cat_list[j]->GetSymbolList().size();
		}
	}

	return total;
}

void SmMarketManager::requestRecentAllSise()
{

}

bool SmMarketManager::IsInRunList(std::string product_code)
{
	if (product_code.length() > 2 && isdigit(product_code.at(2))) {
		auto it = _DomesticList.find(product_code);
		if (it == _DomesticList.end())
			return false;
		else
			return true;
	}
	else {
		return true;
	}
}

void SmMarketManager::AddDomesticItem(std::string item)
{
	_DomesticList.insert(item);
}

void SmMarketManager::AddProduct(SmProduct* product)
{
	if (!product)
		return;
	_ProductMap[product->Code()] = product;
}

SmProduct* SmMarketManager::FindProductFromMap(std::string product_code)
{
	auto it = _ProductMap.find(product_code);
	if (it != _ProductMap.end()) {
		return it->second;
	}
	return nullptr;
}

std::pair<SmProduct*, SmProduct*> SmMarketManager::GetProductPair(SmRunInfo run_info)
{
	SmProduct* call_product = FindProduct(run_info.CallCode);
	SmProduct* put_product = FindProduct(run_info.PutCode);
	return std::make_pair(call_product, put_product);
}

void SmMarketManager::AddFutureRunInfo(SmRunInfo run_info)
{
	_FutureRunVector.push_back(run_info);
}

void SmMarketManager::AddOptionRunInfo(SmRunInfo run_info)
{
	_OptionRunVector.push_back(run_info);
}

void SmMarketManager::LoadRunInfo()
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
	pugi::xml_node running_list = application.child("runnig_list");
	if (running_list) {
		pugi::xml_node future_list = running_list.child("future_list");
		if (future_list) {
			for (pugi::xml_node future_node = future_list.first_child(); future_node; future_node = future_node.next_sibling()) {
				std::string code = future_node.attribute("code").as_string();
				std::string name = future_node.attribute("name").as_string();
				std::string custom_name = future_node.attribute("custom_name").as_string();
				SmRunInfo run_info;
				run_info.Code = code;
				run_info.Name = name;
				run_info.UserDefinedName = custom_name;
				AddFutureRunInfo(run_info);
			}
		}

		pugi::xml_node option_list = running_list.child("option_list");
		if (option_list) {
			for (pugi::xml_node option_node = option_list.first_child(); option_node; option_node = option_node.next_sibling()) {
				std::string call_code = option_node.attribute("call").as_string();
				std::string put_code = option_node.attribute("put").as_string();
				std::string name = option_node.attribute("name").as_string();
				std::string code = option_node.attribute("code").as_string();
				std::string custom_name = option_node.attribute("custom_name").as_string();
				SmRunInfo run_info;
				run_info.Code = code;
				run_info.CallCode = call_code;
				run_info.PutCode = put_code;
				run_info.Name = name;
				run_info.UserDefinedName = custom_name;
				AddOptionRunInfo(run_info);
			}
		}
	}

// 	pugi::xml_node file_watch = application.child("file_watch");
// 	if (file_watch) {
// 		VtGlobal::GetInstance()->EnableFileWatch = file_watch.attribute("enable").as_bool();
// 		VtGlobal::GetInstance()->FileWatchPath = file_watch.child_value("file_watch_path");
// 	}
}

void SmMarketManager::ReadDomesticMarketTable()
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
	pugi::xml_node running_list = application.child("runnig_list");
	if (running_list) {
		pugi::xml_node market_code_table = running_list.child("market_code_table");
		if (market_code_table) {
			for (pugi::xml_node future_node = market_code_table.first_child(); future_node; future_node = future_node.next_sibling()) {
				std::string code = future_node.attribute("code").as_string();
				std::string name = future_node.attribute("name").as_string();
				std::string custom_name = future_node.attribute("custom_name").as_string();
				_DomesticMarketTable[code] = std::make_pair(name, custom_name);
			}
		}
	}
}

std::pair<std::string, std::string> SmMarketManager::FindMarketInfo(std::string market_code)
{
	return _DomesticMarketTable[market_code];
}

std::vector<VtSymbol*> SmMarketManager::GetSymbolListByCode(std::string market_code)
{
	std::vector<VtSymbol*> list;
	for (auto it = _MarketList.begin(); it != _MarketList.end(); ++it) {
		SmMarket* mrkt = *it;
		if (mrkt->Code().compare(market_code) == 0) {
			std::vector<SmProduct*>& product_list = mrkt->GetProductList();
			for (size_t i = 0; i < product_list.size(); ++i) {
				SmProduct* product = product_list[i];
				std::vector<VtSymbol*>& sym_list = product->GetSymbolList();
				for (size_t j = 0; j < sym_list.size(); ++j) {
					list.push_back(sym_list[j]);
				}
			}
			break;
		}
	}

	return list;
}

void SmMarketManager::MakeYearMonthForWeeklyOption()
{
	SmProduct* product = FindProductFromMap("209");
	if (product) {
		product->MakeYearMonth();
	}

	product = FindProductFromMap("309");
	if (product) {
		product->MakeYearMonth();
	}
}

void SmMarketManager::SendSymbolMaster(std::string user_id, VtSymbol* sym)
{

}

SmMarket* SmMarketManager::FindMarket(std::string mrkt_name)
{
	for (auto it = _MarketList.begin(); it != _MarketList.end(); ++it) {
		SmMarket* mrkt = *it;
		if (mrkt->Name().compare(mrkt_name) == 0) {
			return mrkt;
		}
	}

	return nullptr;
}

void SmMarketManager::AddCategoryMarket(std::string cat_code, std::string mrkt_name)
{
	_CategoryToMarketMap[cat_code] = mrkt_name;
}
