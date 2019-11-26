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
	appPath.append(_T("mst"));
	appPath.append(_T("\\"));
	pugi::xml_document doc;

	pugi::xml_parse_result result = doc.load_file(appPath.c_str());
	pugi::xml_node app = doc.first_child();
	pugi::xml_node sym_file_list = app.first_child();
	pugi::xml_node abroad_list = sym_file_list.first_child();
	int index = 9;
	for (auto it = abroad_list.begin(); it != abroad_list.end(); ++it) {
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
	appPath.append(_T("mst"));
	appPath.append(_T("\\"));
	pugi::xml_document doc;

	pugi::xml_parse_result result = doc.load_file(appPath.c_str());
	pugi::xml_node app = doc.first_child();
	pugi::xml_node sym_file_list = app.first_child();
	pugi::xml_node abroad_list = sym_file_list.first_child().next_sibling();
	int index = 0;
	for (auto it = abroad_list.begin(); it != abroad_list.end(); ++it) {
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
	_MarketList.emplace_back(market);
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
	if (ym->SymbolList.size() == 0)
		return nullptr;
	return *ym->SymbolList.begin();
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
