#pragma once
#include "../Global/TemplateSingleton.h"
#include <string>
#include <map>
#include <vector>
#include <set>
#include "../SmRunInfo.h"
#include "../Xml/pugixml.hpp"
class SmMarket;
class SmProduct;
class VtSymbol;
class SmMarketManager : public TemplateSingleton<SmMarketManager>
{
public:
	SmMarketManager();
	~SmMarketManager();
	void ReadAbroadSymbolsFromFile();
	void ReadDomesticSymbolsFromFile();
	/// <summary>
	/// �̸����� ���� ��ü�� �߰��Ѵ�.
	/// </summary>
	/// <param name="name"></param>
	/// <returns>������ ���� ��ü�� �����ش�.</returns>
	SmMarket* AddMarket(std::string name);
	SmProduct* FindProduct(std::string mrkt_name, std::string cat_code);
	SmMarket* FindMarket(std::string mrkt_name);
	void AddCategoryMarket(std::string cat_code, std::string mrkt_name);
	SmProduct* FindProduct(std::string cat_code);
	std::vector<VtSymbol*> GetRecentMonthSymbolList();
	VtSymbol* GetRecentSymbol(std::string market_name, std::string product_name);
	VtSymbol* GetRecentSymbol(std::string product_name);
	void SendMarketList(std::string user_id);
	void SendSymbolListByCategory(std::string user_id);
	int GetTotalCategoryCount();
	int GetTotalSymbolCount();
	std::vector<SmMarket*>& GetMarketList() {
		return _MarketList;
	}

	void requestRecentAllSise();
	bool IsInRunList(std::string product_code);
	void AddDomesticItem(std::string item);
	void AddProduct(SmProduct* product);
	SmProduct* FindProductFromMap(std::string product_code);
	std::pair<SmProduct*, SmProduct*> GetProductPair(SmRunInfo run_info);
	void AddFutureRunInfo(SmRunInfo run_info);
	void AddOptionRunInfo(SmRunInfo run_info);
	void LoadRunInfo();
	std::vector<SmRunInfo> GetFutureRunVector() {
		return _FutureRunVector;
	}

	std::vector<SmRunInfo> GetOptionRunVector() {
		return _OptionRunVector;
	}
	void ReadDomesticMarketTable();
	std::pair<std::string, std::string> FindMarketInfo(std::string market_code);
	std::vector<VtSymbol*> GetSymbolListByCode(std::string market_code);
	void MakeYearMonthForWeeklyOption();
	void ReadAbroadMarketList();
	std::vector<SmMarket*> GetAbroadMarketList();
private:
	void SendSymbolMaster(std::string user_id, VtSymbol* sym);
	std::vector<SmMarket*> _MarketList;
	std::map<std::string, SmProduct*> _ProductMap;
	/// ǰ���� ���� ���� �̸� ���� ǥ
	/// <summary>
	/// �� ǰ�� ���Ͽ� �����̸��� ��Ī���� �ش�.
	/// Ű : ǰ�� �ڵ� 
	/// �� : ���� �̸�
	/// </summary>
	std::map<std::string, std::string> _CategoryToMarketMap;
	std::set<std::string> _DomesticList;
	std::vector<SmRunInfo> _FutureRunVector;
	std::vector<SmRunInfo> _OptionRunVector;
	// code -> market name
	std::map<std::string, std::pair<std::string, std::string>> _DomesticMarketTable;
	std::vector<std::string> _AbroadMarketList;
};

