#pragma once
#include "../Global/TemplateSingleton.h"
#include <string>
#include <map>
#include <vector>
#include <set>
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
private:
	void SendSymbolMaster(std::string user_id, VtSymbol* sym);
	std::vector<SmMarket*> _MarketList;
	/// ǰ���� ���� ���� �̸� ���� ǥ
	/// <summary>
	/// �� ǰ�� ���Ͽ� �����̸��� ��Ī���� �ش�.
	/// Ű : ǰ�� �ڵ� 
	/// �� : ���� �̸�
	/// </summary>
	std::map<std::string, std::string> _CategoryToMarketMap;
	std::set<std::string> _DomesticList;
};

