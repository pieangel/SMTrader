#pragma once
#include <map>
#include <string>
#include "Global/TemplateSingleton.h"
#include "VtSymbolEvent.h"
#include "Poco/BasicEvent.h"
#include <set>
#include <tuple>
#include <cmath>
#include "HdProductInfo.h"

using Poco::BasicEvent;

class VtSymbol;
class VtSymbolMaster;
struct VtRealtimeSymbolMaster;
class VtSymbolManager : public TemplateSingleton<VtSymbolManager>
{
public:
	void RequestSymbolMasterBatch();
	void RequestSymbolBundleInfo(std::string fullCode);
	void RequestSymbolMaster(std::string fullCode);
	void RequestSymbolQuote(std::string fullCode);
	void RequestSymbolHoga(std::string fullCode);
	void RequestSymbolTickQuote(std::string fullCode);

	VtSymbolManager();
	~VtSymbolManager();

	void AddSymbolMasterToSet(std::string fullCode);
	std::set<std::string> SymbolMasterUpdateSet;
	std::map<std::string, std::string> FullCodeMap;
	std::map<std::string, std::string> ShortCodeMap;
	std::map<std::string, VtSymbol*> SymbolMap;
	std::map<std::string, VtSymbolMaster*> SymbolMasterMap;
	std::map<std::string, VtRealtimeSymbolMaster*> RealtimeSymbolMasterMap;
	//void AddSymbol(VtSymbol* symbol);
	
	VtSymbol* FindAddSymbol(std::string symbol_code);
	VtSymbol* FindSymbol(std::string shortCode);
	VtSymbol* FindHdSymbol(std::string shortCode);
	VtSymbol* FindSymbolByFullCode(std::string code);

	void AddSymbolMaster(VtSymbolMaster* symbol);
	VtRealtimeSymbolMaster* FindRealtimeSymbolMasterByFullCode(std::string code);
	VtSymbolMaster* FindSymbolMasterByFullCode(std::string code);
	VtSymbolMaster* FindAddSymbolMaster(std::string fullCode);
	VtSymbolMaster* FindSymbolMasterByShortCode(std::string code);
	VtSymbol* ActiveSymbol() const { return _ActiveSymbol; }
	void ActiveSymbol(VtSymbol* val) { _ActiveSymbol = val; }
	std::string GetShortCode(std::string fullCode);
	std::string GetFullCode(std::string shortCode);
	int GetSymbolDecimal(std::string shortCode);
	static long GetInt(std::string value);
	static int GetTickDiff(int src, int dst, VtSymbol* sym);
	static int GetTickDiff(std::string src, std::string dst, VtSymbolMaster* symMaster);
	static int GetTickDiff(double src, double dst, VtSymbolMaster* symMaster);
	static int GetTickDiff(double src, std::string dst, VtSymbolMaster* symMaster);
	static std::string GetNext(std::string src, VtSymbolMaster* symMaster, int size);
	static double GetNextValue(double src, VtSymbolMaster* symMaster, int size);
	static int GetNextValue(int src, int size, VtSymbol* sym);
	static std::tuple<int, int> GetExpireYearMonth(std::string symCode);
	static std::tuple<int, int> GetYearMonth(std::string code);
	static std::string& TrimRight(std::string& input, char c);
	static std::string& TrimLeft(std::string& input, char c);
	static std::string& TrimBoth(std::string& input, char c);
	BasicEvent<VtSymbolEventArgs> _SymbolListReceivedEvent;
	void FireCodeListReceived(VtSymbolEventArgs&& arg)
	{
		_SymbolListReceivedEvent(this, arg);
	}
	HdProductInfo* AddProductInfo(std::string code);
	HdProductInfo* FindProductInfo(std::string code);


	void LoadAbroadSymbols();
private:
	std::map<std::string, HdProductInfo*> ProductInfoMap;
	VtSymbol* _ActiveSymbol = nullptr;
	void AddHdSymbol(VtSymbol* symbol);
public:
	void OnReceivedSymbolList();

	void UpdateSymbolMaster();

	void OnReceivedSymbolMaster(VtSymbolMaster* symMaster);
	void OnReceivedRealtimeSymbolMaster(VtSymbolMaster* symMaster);
	void OnReceivedSymbolMaster(VtRealtimeSymbolMaster* symMaster);

	void Begin();
	void End();

	int KospiCurrent;
	int Kospi200Current;
};

