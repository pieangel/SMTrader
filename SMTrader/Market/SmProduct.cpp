#include "afx.h"
#include "SmProduct.h"
#include "../VtSymbol.h"
#include "../VtSymbolManager.h"
#include "SmProductYearMonth.h"
#include "../VtStringUtil.h"
#include "../Util/SmUtil.h"
#include <ctime>
SmProduct::SmProduct()
{
	_DomesticYearTable["A"] = "2006";
	_DomesticYearTable["B"] = "2007";
	_DomesticYearTable["C"] = "2008";
	_DomesticYearTable["D"] = "2009";
	_DomesticYearTable["E"] = "2010";
	_DomesticYearTable["F"] = "2011";
	_DomesticYearTable["G"] = "2012";
	_DomesticYearTable["H"] = "2013";
	_DomesticYearTable["J"] = "2014";
	_DomesticYearTable["K"] = "2015";
	_DomesticYearTable["L"] = "2016";
	_DomesticYearTable["M"] = "2017";
	_DomesticYearTable["N"] = "2018";
	_DomesticYearTable["P"] = "2019";
	_DomesticYearTable["Q"] = "2020";
	_DomesticYearTable["R"] = "2021";
	_DomesticYearTable["S"] = "2022";
	_DomesticYearTable["T"] = "2023";
	_DomesticYearTable["V"] = "2024";
	_DomesticYearTable["W"] = "2025";

	_DomesticMonthTable["1"] = "01";
	_DomesticMonthTable["2"] = "02";
	_DomesticMonthTable["3"] = "03";
	_DomesticMonthTable["4"] = "04";
	_DomesticMonthTable["5"] = "05";
	_DomesticMonthTable["6"] = "06";
	_DomesticMonthTable["7"] = "07";
	_DomesticMonthTable["8"] = "08";
	_DomesticMonthTable["9"] = "09";
	_DomesticMonthTable["A"] = "10";
	_DomesticMonthTable["B"] = "11";
	_DomesticMonthTable["C"] = "12";

	_AbroadMonthTable["F"] = "01";
	_AbroadMonthTable["G"] = "02";
	_AbroadMonthTable["H"] = "03";
	_AbroadMonthTable["J"] = "04";
	_AbroadMonthTable["K"] = "05";
	_AbroadMonthTable["M"] = "06";
	_AbroadMonthTable["N"] = "07";
	_AbroadMonthTable["Q"] = "08";
	_AbroadMonthTable["U"] = "09";
	_AbroadMonthTable["V"] = "10";
	_AbroadMonthTable["X"] = "11";
	_AbroadMonthTable["Z"] = "12";
}


SmProduct::~SmProduct()
{
	for (auto it = _YearMonthMap.begin(); it != _YearMonthMap.end(); ++it) {
		delete it->second;
	}
}

VtSymbol* SmProduct::AddSymbol(std::string symCode)
{
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindAddSymbol(symCode);
	sym->ShortCode = symCode;
	_SymbolList.push_back(sym);
	AddToYearMonth(symCode, sym);
	return sym;
}

VtSymbol* SmProduct::AddSymbol(std::string symCode, std::string name)
{
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindAddSymbol(symCode);
	sym->ShortCode = symCode;
	sym->Name = name;
	_SymbolList.push_back(sym);
	AddToYearMonth(symCode, name, sym);
	return sym;
}

VtSymbol* SmProduct::GetRecentMonthSymbol()
{
	if (_SymbolList.size() == 0)
		return nullptr;
	return _SymbolList[0];
}

VtSymbol* SmProduct::GetNextMonthSymbol()
{
	if (_SymbolList.size() < 2)
		return nullptr;
	return _SymbolList[1];
}

SmProductYearMonth* SmProduct::GetRecentYearMonth()
{
	if (_YearMonthMap.size() == 0)
		return nullptr;
	else
		return _YearMonthMap.begin()->second;
}

SmProductYearMonth* SmProduct::GetNextYearMonth()
{
	if (_YearMonthMap.size() <= 1)
		return nullptr;
	else
		return (++_YearMonthMap.begin())->second;
}

SmProductYearMonth* SmProduct::GetYearMonth(std::string year_month)
{
	auto it = _YearMonthMap.find(year_month);
	if (it == _YearMonthMap.end())
		return nullptr;
	return it->second;
}

void SmProduct::MakeYearMonth()
{
	for (auto it = _SymbolList.begin(); it != _SymbolList.end(); ++it) {
		AddToYearMonth(*it);
	}
}

void SmProduct::AddToYearMonth(std::string symbol_code, VtSymbol* symbol)
{
	if (!symbol)
		return;
	std::string local_date_time = VtStringUtil::getCurentDate();
	std::string local_year_month = local_date_time.substr(0, 6);

	if (isdigit(symbol_code.at(2))) { // 국내 상풍
		std::string product_code = symbol_code.substr(0, 3);
		std::string year = symbol_code.substr(3, 1);
		year = _DomesticYearTable[year];
		std::string month = symbol_code.substr(4, 1);
		month = _DomesticMonthTable[month];
		std::string year_month_temp = year + month;
		std::string year_month = year + "-"+ month;
		if (year_month_temp.compare(local_year_month) < 0)
			return;
		auto it = _YearMonthMap.find(year_month);
		SmProductYearMonth* ym = nullptr;
		if (it == _YearMonthMap.end()) {
			ym = new SmProductYearMonth();
			ym->ProductCode = product_code;
			ym->YearMonthCode = year_month;
			_YearMonthMap[year_month] = ym;
		}
		else {
			ym = it->second;
		}
		ym->AddSymbol(symbol);
	}
	else { // 해외 상품
		std::string product_code = symbol_code.substr(0, 2);
		std::string year = "20";
		year.append(symbol_code.substr(symbol_code.length() - 2, 2));
		std::string month = symbol_code.substr(symbol_code.length() - 3, 1);
		month = _AbroadMonthTable[month];
		std::string year_month = year + "-" + month;
		std::string year_month_temp = year + month;
		if (year_month_temp.compare(local_year_month) < 0)
			return;
		auto it = _YearMonthMap.find(year_month);
		SmProductYearMonth* ym = nullptr;
		if (it == _YearMonthMap.end()) {
			ym = new SmProductYearMonth();
			ym->ProductCode = product_code;
			ym->YearMonthCode = year_month;
			_YearMonthMap[year_month] = ym;
		}
		else {
			ym = it->second;
		}
		ym->AddSymbol(symbol);
	}
}

void SmProduct::AddToYearMonth(std::string symbol_code, std::string name, VtSymbol* symbol)
{
	if (!symbol)
		return;

	if (isdigit(symbol_code.at(2))) { // 국내 상풍
		std::string product_code = symbol_code.substr(0, 3);
		std::string yeay_month_week = name.substr(15, 6);

		std::string year = "20";
		year.append(yeay_month_week.substr(0, 2));
		
		std::string month = yeay_month_week.substr(2, 2);

		std::string week = yeay_month_week.substr(4, 2);
		
		std::string local_date_time = VtStringUtil::getCurentDate();
		std::string local_year_month = local_date_time.substr(0, 6);
		std::string week_info = yeay_month_week.substr(5, 1);
		std::string year_month_temp = year + month;
		std::string year_month = year + "-" + month + "-";

		if (year_month_temp.compare(local_year_month) < 0)
			return;

		std::vector<int> date_time_vec = SmUtil::GetLocalDateTime();

		// 그 달의 첫째 날의  일요일에서 몇번째에 있는지 숫자로 나타내 현재 날짜에 더한다.
		// 그것을 7로 나누면 몫이 몇번째 주이고 나머지가 0보다 크면 한주를 더 더해준다.
		// 그러면 몇 번째 주인지 알수 있다. 그리고 매주 목요일이 만기일이므로  3을 더하면 주차가 나온다.
		int day_week = dow(date_time_vec[0], date_time_vec[1], 1);

		int whole_days = day_week + date_time_vec[2];
		// 목요일이 만기일인걸 감안해서 2를 더해 준다.
		whole_days += 2;
		// 몇째 주가 나온다.
		int q = (int)(whole_days / 7);
		// 나머지가 0보다 크면 1주를 더 더해준다.
		int rem = whole_days % 7;
		if (rem > 0)
			q++;

		if (q > 4)
			q = 4;
		
		// 현재 주보다 작으면 추가하지 않는다.
		if (std::stoi(week_info) < q)
			return;

		year_month.append(week);
		auto it = _YearMonthMap.find(year_month);
		SmProductYearMonth* ym = nullptr;
		if (it == _YearMonthMap.end()) {
			ym = new SmProductYearMonth();
			ym->ProductCode = product_code;
			ym->YearMonthCode = year_month;
			_YearMonthMap[year_month] = ym;
		}
		else {
			ym = it->second;
		}
		ym->AddSymbol(symbol);
	}
}

void SmProduct::AddToYearMonth(VtSymbol* symbol)
{
	if (!symbol)
		return;

	std::string symbol_code = symbol->ShortCode;
	std::string name = symbol->Name;

	if (isdigit(symbol_code.at(2))) { // 국내 상풍
		std::string product_code = symbol_code.substr(0, 3);
		std::string yeay_month_week = name.substr(15, 6);

		std::string year = "20";
		year.append(yeay_month_week.substr(0, 2));

		std::string month = yeay_month_week.substr(2, 2);

		std::string week = yeay_month_week.substr(4, 2);

		std::string local_date_time = VtStringUtil::getCurentDate();
		std::string local_year_month = local_date_time.substr(0, 6);
		std::string week_info = yeay_month_week.substr(5, 1);
		std::string year_month_temp = year + month;
		std::string year_month = year + "-" + month + "-";

		if (year_month_temp.compare(local_year_month) < 0)
			return;

		std::vector<int> date_time_vec = SmUtil::GetLocalDateTime();

		// 그 달의 첫째 날의  일요일에서 몇번째에 있는지 숫자로 나타내 현재 날짜에 더한다.
		// 그것을 7로 나누면 몫이 몇번째 주이고 나머지가 0보다 크면 한주를 더 더해준다.
		// 그러면 몇 번째 주인지 알수 있다. 그리고 매주 목요일이 만기일이므로  3을 더하면 주차가 나온다.
		int day_week = dow(date_time_vec[0], date_time_vec[1], 1);

		int whole_days = day_week + date_time_vec[2];
		// 목요일이 만기일인걸 감안해서 2를 더해 준다.
		whole_days += 2;
		// 몇째 주가 나온다.
		int q = (int)(whole_days / 7);
		// 나머지가 0보다 크면 1주를 더 더해준다.
		int rem = whole_days % 7;
		if (rem > 0)
			q++;

		if (q > 4)
			q = 4;

		// 현재 주보다 작으면 추가하지 않는다.
		if (std::stoi(week_info) < q)
			return;

		year_month.append(week);
		auto it = _YearMonthMap.find(year_month);
		SmProductYearMonth* ym = nullptr;
		if (it == _YearMonthMap.end()) {
			ym = new SmProductYearMonth();
			ym->ProductCode = product_code;
			ym->YearMonthCode = year_month;
			_YearMonthMap[year_month] = ym;
		}
		else {
			ym = it->second;
		}
		ym->AddSymbol(symbol);
	}
}

int SmProduct::dow(int y, int m, int d)
{
	static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
	y -= m < 3;
	return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}

