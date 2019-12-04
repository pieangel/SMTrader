#pragma once
#include <string>
#include <map>
#include "VtQuote.h"
#include "VtHoga.h"
class SmChartData;
class VtSymbol
{
public:
	VtSymbol();
	~VtSymbol();

	int Index;
	int NearMonth;
	std::string Currency = "KRW";
	std::string Exchange = "CME";
	std::string ProductCode;
	std::string MarketName;
	double CtrUnit;
	std::string LastDate;
	// Ǯ�ڵ�
	std::string FullCode;
	// ª�� �ڵ�
	std::string ShortCode;
	// �����
	std::string Name;
	std::string ShortName;
	// ����� - ����
	std::string EngName;
	std::string BriefName;
	std::string Initial;

	// ƽũ��
	double TickSize;
	std::string UserDefinedName;
	// ƽũ�� - ������ �ٲ۰�
	int intTickSize;
	// �Ҽ��� �ڸ��� - ����
	int Decimal;

	int SymbolDate;
	// ƽ��ġ
	double TickValue;
	// �ŷ� �¼�
	int Seungsu = 250000;
	int ExpireYear;
	int ExpireMonth;
	int ComToPrev;
	int UpdownRate;


	VtHoga Hoga;
	VtQuote Quote;

	void Begin();
	void End();

	bool RecentMonth() const { return _RecentMonth; }
	void RecentMonth(bool val) { _RecentMonth = val; }
	bool RequestedSymbolMaster() const { return _RequestedSymbolMaster; }
	void RequestedSymbolMaster(bool val) { _RequestedSymbolMaster = val; }
public:
	void GetSymbolMaster();

	void addChartData(std::string dataKey, SmChartData* chart_data) {
		_ChartDataMap[dataKey] = chart_data;
	}

	void removeChartData(std::string dataKey) {
		auto it = _ChartDataMap.find(dataKey);
		if (it != _ChartDataMap.end()) {
			_ChartDataMap.erase(it);
		}
	}

	void UpdateChartValue();

private:
	bool _RecentMonth = false;
	bool _RequestedSymbolMaster = false;

	std::map<std::string, SmChartData*> _ChartDataMap;
	
};

struct CompareSymbol {
	bool operator()(VtSymbol * lhs, VtSymbol * rhs) { return lhs->SymbolDate < rhs->SymbolDate; }
};

