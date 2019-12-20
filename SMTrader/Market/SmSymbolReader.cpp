#include "afx.h"
#include "SmSymbolReader.h"
#include <filesystem>
#include <sstream>
#include <string>
#include <fstream>
#include "SmMarket.h"
#include "SmMarketManager.h"
#include "SmProduct.h"
#include "../VtSymbol.h"
#include "../VtSymbolManager.h"
#include "../ZmConfigManager.h"
#include "../Xml/pugixml.hpp"

SmSymbolReader::SmSymbolReader()
{
	
}


SmSymbolReader::~SmSymbolReader()
{
}

void SmSymbolReader::ReadSymbolFileList()
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string appPath = configMgr->GetAppPath();
	std::string configPath = appPath;
	configPath.append(_T("\\config.xml"));
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(configPath.c_str());

	pugi::xml_node sym_file_list = doc.child("application").child("symbol_file_list");
	pugi::xml_node domestic_list = sym_file_list.first_child().next_sibling();
	for (auto it = domestic_list.begin(); it != domestic_list.end(); ++it) {
		std::string file_name = it->text().as_string();
		TRACE(file_name.c_str());
		DomesticSymbolMasterFileSet.insert(file_name);
	}

// 	pugi::xml_node abroad_list = sym_file_list.first_child();
// 	for (auto it = abroad_list.begin(); it != abroad_list.end(); ++it) {
// 		std::string file_name = it->text().as_string();
// 		TRACE(file_name.c_str());
// 		DomesticSymbolMasterFileSet.insert(file_name);
// 	}

	
	pugi::xml_node app = doc.first_child();
	pugi::xml_node domestic_list_node = doc.child("application").child("domestic_list");
	pugi::xml_node domestic_node = domestic_list_node.first_child();
	while (domestic_node) {
		std::string code = domestic_node.text().as_string();
		_DomesticList.insert(code);
		SmMarketManager::GetInstance()->AddDomesticItem(code);
		domestic_node = domestic_node.next_sibling();
	}
}

std::string SmSymbolReader::GetWorkingDir()
{
	std::string dir;
	return dir;
}


void SmSymbolReader::ReadSymbolFromFile(int index, std::string fullPath)
{
	switch (index)
	{
	case 0:
		ReadKospiFutureFile(fullPath);
		break;
	case 1:
		ReadKospiOptionFile(fullPath);
		break;
	case 2:
		ReadKospiWeeklyOptionFile(fullPath);
		break;
	case 3:
		ReadKosdaqFutureFile(fullPath);
		break;
	case 4:
		ReadMiniKospiFutureFile(fullPath);
		break;
	case 5:
		ReadCommodityFutureFile(fullPath);
		break;
	case 6:
		ReadKospiFutureInfo(fullPath);
		break;
	case 7:
		ReadKospiOptionInfo(fullPath);
		break;
	case 8:
		ReadUsDollarFutureInfo(fullPath);
		break;
	case 9:
		ReadKosdaqOptionFile(fullPath);
		break;
	case 10:
		ReadMiniKospiOptionFile(fullPath);
		break;
	case 11:
		ReadMarketFile(fullPath);
		break;
	case 12:
		ReadPmFile(fullPath);
		break;
	case 13:
		ReadJmFile(fullPath);
		break;
	default:
		break;
	}
}

void SmSymbolReader::ReadMarketFile()
{
	CString msg;
	std::ifstream infile("C:\\Users\\piean\\Source\\Repos\\ReadSymbol\\Debug\\MRKT.cod");
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		std::string market = line.substr(0, 20);
		std::string exchange = line.substr(20, 5);
		std::string pmCode = line.substr(25, 3);
		std::string enName = line.substr(28, 50);
		std::string name = line.substr(78, 50);
		msg.Format(_T("market = %s, ex = %s, pmCode = %s, name = %s, ename = %s\n"), market.c_str(), exchange.c_str(), pmCode.c_str(), name.c_str(), enName.c_str());
		//TRACE(msg);
	}
}

void SmSymbolReader::ReadMarketFile(std::string fullPath)
{
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		std::string market_type = line.substr(0, 20);
		std::string exchange = line.substr(20, 5);
		std::string pmCode = line.substr(25, 3);
		std::string enName = line.substr(28, 50);
		std::string name = line.substr(78, 50);
		
		rtrim(market_type);
		rtrim(exchange);
		rtrim(pmCode);
		rtrim(enName);
		rtrim(name);
		
		SmMarket* market = marketMgr->AddMarket(market_type);
		SmProduct* cat = market->AddProduct(pmCode);
		marketMgr->AddProduct(cat);
		cat->MarketName(market_type);
		cat->Exchange(exchange);
		cat->Name(enName);
		cat->NameKr(name);
		msg.Format(_T("market = %s, ex = %s, pmCode = %s, name = %s, ename = %s\n"), market_type.c_str(), exchange.c_str(), pmCode.c_str(), name.c_str(), enName.c_str());
		//TRACE(msg);
	}
}

void SmSymbolReader::ReadPmFile()
{
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	CString msg;
	std::ifstream infile("C:\\Users\\piean\\Source\\Repos\\ReadSymbol\\Debug\\PMCODE.cod");
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		std::string market = line.substr(0, 20);
		// 거래소 인덱스 코드
		std::string exIndexCode = line.substr(20, 4);
		// 거래소 코드 
		std::string exChangeCode = line.substr(24, 5);
		// 품목 코드
		std::string pmCode = line.substr(29, 5);
		// 시장 구분을 숫자로 함
		std::string pmGubun = line.substr(34, 3);
		msg.Format(_T("market = %s, ex = %s, pmCode = %s, name = %s, ename = %s\n"), market.c_str(), exIndexCode.c_str(), exChangeCode.c_str(), pmCode.c_str(), pmGubun.c_str());
		//TRACE(msg);
		rtrim(market);
		rtrim(exIndexCode);
		rtrim(exChangeCode);
		rtrim(pmCode);
		rtrim(pmGubun);
		SmProduct* cat = marketMgr->FindProduct(market, pmCode);
		if (cat) {
			cat->ExchangeCode(exChangeCode);
			cat->ExchangeIndex(exIndexCode);
			cat->MarketCode(pmGubun);
		}
	}
}



void SmSymbolReader::ReadPmFile(std::string fullPath)
{
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		std::string market = line.substr(0, 20);
		std::string exIndexCode = line.substr(20, 4);
		std::string exChangeCode = line.substr(24, 5);
		std::string pmCode = line.substr(29, 5);
		std::string pmGubun = line.substr(34, 3);
		msg.Format(_T("market = %s, ex = %s, pmCode = %s, name = %s, ename = %s\n"), market.c_str(), exIndexCode.c_str(), exChangeCode.c_str(), pmCode.c_str(), pmGubun.c_str());
		//TRACE(msg);

		rtrim(market);
		rtrim(exIndexCode);
		rtrim(exChangeCode);
		rtrim(pmCode);
		rtrim(pmGubun);
		SmProduct* cat = marketMgr->FindProduct(market, pmCode);
		if (cat) {
			cat->ExchangeCode(exChangeCode);
			cat->ExchangeIndex(exIndexCode);
			cat->MarketCode(pmGubun);
			marketMgr->AddCategoryMarket(pmCode, market);
		}
	}
}

void SmSymbolReader::ReadJmFile()
{
	CString msg;
	std::ifstream infile("C:\\Users\\piean\\Source\\Repos\\ReadSymbol\\Debug\\JMCODE.cod");
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string Series = line.substr(0, 32);
		/* 종목코드                             */

		std::string ExchCd = line.substr(32, 5);
		/* 거래소                               */

		std::string IndexCode = line.substr(37, 4);
		/* 품목 인덱스코드                      */

		std::string MrktCd = line.substr(41, 5);
		/* 품목코드                             */

		std::string ExchNo = line.substr(46, 5);
		/* 거래소 번호                          */

		std::string Pdesz = line.substr(51, 5);
		/* 소수점 정보                          */

		std::string Rdesz = line.substr(56, 5);
		/* 소수점 정보2                         */

		std::string CtrtSize = line.substr(61, 20);
		/* 계약크기                             */

		std::string TickSize = line.substr(81, 20);
		/* Tick Size                            */

		std::string TickValue = line.substr(101, 20);
		/* Tick Value                           */

		std::string MltiPler = line.substr(121, 20);
		/* 거래승수                             */

		std::string DispDigit = line.substr(141, 10);
		/* 진법                                 */

		std::string SeriesNm = line.substr(151, 32);
		/* Full 종목명                          */

		std::string SeriesNmKor = line.substr(183, 32);
		/* Full 종목명한글                      */

		std::string NearSeq = line.substr(215, 1);
		/* 최근월물, 주요종목표시               */

		std::string StatTp = line.substr(216, 1);
		/* 거래가능여부                         */

		std::string LockDt = line.substr(217, 8);
		/* 신규거래제한일                       */

		std::string TradFrDt = line.substr(225, 8);
		/* 최초거래일                           */

		std::string TradToDt = line.substr(233, 8);
		/* 최종거래일                           */

		std::string ExprDt = line.substr(241, 8);
		/* 만기일, 최종결제일                   */

		std::string RemnCnt = line.substr(249, 4);
		/* 잔존일수                             */

		std::string HogaMthd = line.substr(253, 30);
		/* 호가방식                             */

		std::string MinMaxRt = line.substr(283, 6);
		/* 상하한폭비율                         */

		std::string BaseP = line.substr(289, 20);
		/* 기준가                               */

		std::string MaxP = line.substr(309, 20);
		/* 상한가                               */

		std::string MinP = line.substr(329, 20);
		/* 하한가                               */

		std::string TrstMgn = line.substr(349, 20);
		/* 신규주문증거금                       */

		std::string MntMgn = line.substr(369, 20);
		/* 유지증거금                           */

		std::string CrcCd = line.substr(389, 3);
		/* 결제통화코드                         */

		std::string BaseCrcCd = line.substr(392, 3);
		/* BASE CRC CD                          */

		std::string CounterCrcCd = line.substr(395, 3);
		/* COUNTER CRC CD                       */

		std::string PipCost = line.substr(398, 20);
		/* PIP COST                             */

		std::string BuyInt = line.substr(418, 20);
		/* 매수이자                             */

		std::string SellInt = line.substr(438, 20);
		/* 매도이자                             */

		std::string RoundLots = line.substr(458, 6);
		/* LOUND LOTS                           */

		std::string ScaleChiper = line.substr(464, 10);
		/* 진법자리수                           */

		std::string decimalchiper = line.substr(474, 5);
		/* 소수점 정보(KTB기준)                 */

		std::string JnilVolume = line.substr(479, 10);
		/* 전일거래량                           */



		msg.Format(_T("code = %s, name = %s, name_kr = %s\n"), Series.c_str(), SeriesNm.c_str(), SeriesNmKor.c_str());
		//TRACE(msg);
	}
}

void SmSymbolReader::ReadJmFile(std::string fullPath)
{
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string Series = line.substr(0, 32);
		/* 종목코드                             */

		std::string ExchCd = line.substr(32, 5);
		/* 거래소                               */

		std::string IndexCode = line.substr(37, 4);
		/* 품목 인덱스코드                      */

		std::string MrktCd = line.substr(41, 5);
		/* 품목코드                             */

		std::string ExchNo = line.substr(46, 5);
		/* 거래소 번호                          */

		std::string Pdesz = line.substr(51, 5);
		/* 소수점 정보                          */

		std::string Rdesz = line.substr(56, 5);
		/* 소수점 정보2                         */

		std::string CtrtSize = line.substr(61, 20);
		/* 계약크기                             */

		std::string TickSize = line.substr(81, 20);
		/* Tick Size                            */

		std::string TickValue = line.substr(101, 20);
		/* Tick Value                           */

		std::string MltiPler = line.substr(121, 20);
		/* 거래승수                             */

		std::string DispDigit = line.substr(141, 10);
		/* 진법                                 */

		std::string SeriesNm = line.substr(151, 32);
		/* Full 종목명                          */

		std::string SeriesNmKor = line.substr(183, 32);
		/* Full 종목명한글                      */

		std::string NearSeq = line.substr(215, 1);
		/* 최근월물, 주요종목표시               */

		std::string StatTp = line.substr(216, 1);
		/* 거래가능여부                         */

		std::string LockDt = line.substr(217, 8);
		/* 신규거래제한일                       */

		std::string TradFrDt = line.substr(225, 8);
		/* 최초거래일                           */

		std::string last_date = line.substr(233, 8);
		/* 최종거래일                           */

		std::string ExprDt = line.substr(241, 8);
		/* 만기일, 최종결제일                   */

		std::string RemnCnt = line.substr(249, 4);
		/* 잔존일수                             */

		std::string HogaMthd = line.substr(253, 30);
		/* 호가방식                             */

		std::string MinMaxRt = line.substr(283, 6);
		/* 상하한폭비율                         */

		std::string BaseP = line.substr(289, 20);
		/* 기준가                               */

		std::string MaxP = line.substr(309, 20);
		/* 상한가                               */

		std::string MinP = line.substr(329, 20);
		/* 하한가                               */

		std::string TrstMgn = line.substr(349, 20);
		/* 신규주문증거금                       */

		std::string MntMgn = line.substr(369, 20);
		/* 유지증거금                           */

		std::string CrcCd = line.substr(389, 3);
		/* 결제통화코드                         */

		std::string BaseCrcCd = line.substr(392, 3);
		/* BASE CRC CD                          */

		std::string CounterCrcCd = line.substr(395, 3);
		/* COUNTER CRC CD                       */

		std::string PipCost = line.substr(398, 20);
		/* PIP COST                             */

		std::string BuyInt = line.substr(418, 20);
		/* 매수이자                             */

		std::string SellInt = line.substr(438, 20);
		/* 매도이자                             */

		std::string RoundLots = line.substr(458, 6);
		/* LOUND LOTS                           */

		std::string ScaleChiper = line.substr(464, 10);
		/* 진법자리수                           */

		std::string decimalchiper = line.substr(474, 5);
		/* 소수점 정보(KTB기준)                 */

		std::string JnilVolume = line.substr(479, 10);
		/* 전일거래량                           */


		rtrim(Series);
		rtrim(SeriesNm);
		rtrim(SeriesNmKor);
		rtrim(MrktCd);
		msg.Format(_T("code = %s, name = %s, name_kr = %s\n"), Series.c_str(), SeriesNm.c_str(), SeriesNmKor.c_str());
		//TRACE(msg);

		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		SmProduct* cat = marketMgr->FindProduct(MrktCd);
		if (cat) {
			VtSymbol* sym = cat->AddSymbol(Series);
			sym->Index = std::stoi(IndexCode);
			sym->NearMonth = std::stoi(IndexCode);
			sym->Name = SeriesNmKor;
			sym->EngName = SeriesNm;
			//symMgr->AddHdSymbol(sym);
			sym->ProductCode = cat->Code();
			sym->MarketName = cat->MarketName();
			sym->Decimal = std::stoi(Pdesz);
			sym->Seungsu = std::stoi(MltiPler);
			sym->CtrUnit = std::stod(CtrtSize);
			sym->TickValue = std::stod(TickValue);
			sym->TickSize = std::stod(TickSize);
			sym->LastDate = last_date;
		}
	}
}

// 국내는 앞에 3자리가 상품을 나타낸다. 그 다음 한자리가 년도를 나타내며 그 다음 한자리가 월을 나타낸다.
// 나머지 네자리는 행사가를 의미한다.
void SmSymbolReader::ReadKospiFutureFile(std::string fullPath)
{
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	std::string market_name = "코스피선물";
	SmMarket *market = marketMgr->AddMarket(market_name);
	market->Code("101");
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string shcode = line.substr(0, 8);
		/* 종목코드                             */

		std::string expcode = line.substr(8, 12);
		/* 표준코드                               */

		std::string hname = line.substr(20, 30);
		/* 한글 종목명                      */

		std::string item = line.substr(50, 6);
		/* 기초자산코드                             */

		std::string deli = line.substr(56, 2);
		/* 소수점                          */

		std::string hounit = line.substr(58, 5);
		/*호가단위                          */

		// 0이 8개 더 붙어 있어 일부러 13자리만 읽는다.
		std::string tradewin = line.substr(63, 13);
		/* 거래승수                         */

		std::string spjmgubun = line.substr(84, 1);
		/* 스프레드기준종목구분코드*/

		std::string gshcode = line.substr(85, 8);
		/* 근월물코드                            */

		std::string wshcode = line.substr(93, 8);
		/* 원월물코드                           */

		std::string product_code = shcode.substr(0, 3);

		

		rtrim(shcode);
		rtrim(hname);
		trim(item);
		trim(deli);
		msg.Format(_T("code = %s, name = %s, name_kr = %s\n"), shcode.c_str(), hname.c_str(), item.c_str());
		//TRACE(msg);

		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		SmProduct* product = market->FindAddProduct(product_code);
		marketMgr->AddProduct(product);
		marketMgr->AddCategoryMarket(product_code, market_name);
		product->MarketName(market_name);
		product->NameKr("코스피200선물");
		product->NameKr("Kospi200F");

		VtSymbol* sym = product->AddSymbol(shcode);
		sym->Name = hname;
		//symMgr->AddHdSymbol(sym);
		sym->ProductCode = product->Code();
		sym->MarketName = product->MarketName();
		sym->Decimal = std::stoi(deli);
		sym->Seungsu = std::stoi(tradewin);
		int hoga_unit = std::stoi(hounit);
		int deci = std::stoi(deli);
		sym->intTickSize = hoga_unit;
		double tick_size = hoga_unit / std::pow(10, deci);
		sym->TickSize = tick_size;
		sym->CtrUnit = tick_size;
		double tick_value = std::stoi(tradewin) * tick_size;
		sym->TickValue = tick_value;
	}
}

void SmSymbolReader::ReadKospiOptionFile(std::string fullPath)
{
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	std::string market_name = "코스피옵션";
	SmMarket* market = marketMgr->AddMarket(market_name);
	market->Code("201");
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string shcode = line.substr(0, 8);
		/* 종목코드                             */

		std::string expcode = line.substr(8, 12);
		/* 표준코드                               */

		std::string hname = line.substr(20, 30);
		/* 한글 종목명                      */

		std::string atm = line.substr(50, 1);
		/* ATM 구분                      */

		std::string item = line.substr(51, 6);
		/* 기초자산코드                             */

		std::string deli = line.substr(57, 2);
		/* 소수점                          */

		std::string hounit = line.substr(59, 5);
		/*호가단위                          */

		// 0이 8개 더 붙어 있어 일부러 13자리만 읽는다.
		std::string tradewin = line.substr(64, 13);
		/* 거래승수                         */

		std::string spjmgubun = line.substr(85, 1);
		/* 스프레드기준종목구분코드*/

		std::string gshcode = line.substr(86, 8);
		/* 근월물코드                            */

		std::string wshcode = line.substr(94, 8);
		/* 원월물코드                           */

		std::string product_code = shcode.substr(0, 3);



		rtrim(shcode);
		rtrim(hname);
		trim(item);
		trim(deli);
		msg.Format(_T("code = %s, name = %s, name_kr = %s\n"), shcode.c_str(), hname.c_str(), item.c_str());
		//TRACE(msg);

		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		SmProduct* product = market->FindAddProduct(product_code);
		marketMgr->AddProduct(product);
		marketMgr->AddCategoryMarket(product_code, market_name);
		product->MarketName(market_name);
		product->NameKr("코스피200옵션");
		product->NameKr("Kospi200O");
		VtSymbol* sym = product->AddSymbol(shcode);
		sym->Name = hname;
		//symMgr->AddHdSymbol(sym);
		sym->ProductCode = product->Code();
		sym->MarketName = product->MarketName();
		sym->Decimal = std::stoi(deli);
		sym->Seungsu = std::stoi(tradewin);
		int hoga_unit = std::stoi(hounit);
		int deci = std::stoi(deli);
		sym->intTickSize = hoga_unit;
		double tick_size = hoga_unit / std::pow(10, deci);
		sym->TickSize = tick_size;
		sym->CtrUnit = tick_size;
		double tick_value = std::stoi(tradewin) * tick_size;
		sym->TickValue = tick_value;
	}
}

void SmSymbolReader::ReadKospiWeeklyOptionFile(std::string fullPath)
{
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	std::string market_name = "코스피위클리옵션";
	SmMarket* market = marketMgr->AddMarket(market_name);
	market->Code("209");
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string shcode = line.substr(0, 8);
		/* 종목코드                             */

		std::string expcode = line.substr(8, 12);
		/* 표준코드                               */

		std::string hname = line.substr(20, 30);
		/* 한글 종목명                      */

		std::string atm = line.substr(50, 1);
		/* ATM 구분                      */

		std::string item = line.substr(51, 6);
		/* 기초자산코드                             */

		std::string deli = line.substr(57, 2);
		/* 소수점                          */

		std::string hounit = line.substr(59, 5);
		/*호가단위                          */

		// 0이 8개 더 붙어 있어 일부러 13자리만 읽는다.
		std::string tradewin = line.substr(64, 13);
		/* 거래승수                         */

		std::string spjmgubun = line.substr(85, 1);
		/* 스프레드기준종목구분코드*/

		std::string gshcode = line.substr(86, 8);
		/* 근월물코드                            */

		std::string wshcode = line.substr(94, 8);
		/* 원월물코드                           */

		std::string product_code = shcode.substr(0, 3);



		rtrim(shcode);
		rtrim(hname);
		trim(item);
		trim(deli);
		msg.Format(_T("code = %s, name = %s, name_kr = %s\n"), shcode.c_str(), hname.c_str(), item.c_str());
		//TRACE(msg);

		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		SmProduct* product = market->FindAddProduct(product_code);
		marketMgr->AddProduct(product);
		marketMgr->AddCategoryMarket(product_code, market_name);
		product->MarketName(market_name);
		product->NameKr("코스피200위클리옵션");
		product->NameKr("Kospi200WeeklyO");
		VtSymbol* sym = product->AddSymbol(shcode, hname);
		sym->Name = hname;
		sym->ProductCode = product->Code();
		sym->MarketName = product->MarketName();
		sym->Decimal = std::stoi(deli);
		sym->Seungsu = std::stoi(tradewin);
		int hoga_unit = std::stoi(hounit);
		int deci = std::stoi(deli);
		sym->intTickSize = hoga_unit;
		double tick_size = hoga_unit / std::pow(10, deci);
		sym->TickSize = tick_size;
		sym->CtrUnit = tick_size;
		double tick_value = std::stoi(tradewin) * tick_size;
		sym->TickValue = tick_value;
	}
}

void SmSymbolReader::ReadKosdaqFutureFile(std::string fullPath)
{
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	std::string market_name = "코스닥선물";
	SmMarket* market = marketMgr->AddMarket(market_name);
	market->Code("106");
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string shcode = line.substr(0, 8);
		/* 종목코드                             */

		std::string expcode = line.substr(8, 12);
		/* 표준코드                               */

		std::string hname = line.substr(20, 30);
		/* 한글 종목명                      */

		std::string item = line.substr(50, 6);
		/* 기초자산코드                             */

		std::string deli = line.substr(56, 2);
		/* 소수점                          */

		std::string hounit = line.substr(58, 5);
		/*호가단위                          */

		// 0이 8개 더 붙어 있어 일부러 13자리만 읽는다.
		std::string tradewin = line.substr(63, 13);
		/* 거래승수                         */

		std::string spjmgubun = line.substr(84, 1);
		/* 스프레드기준종목구분코드*/

		std::string gshcode = line.substr(85, 8);
		/* 근월물코드                            */

		std::string wshcode = line.substr(93, 8);
		/* 원월물코드                           */

		std::string product_code = shcode.substr(0, 3);



		rtrim(shcode);
		rtrim(hname);
		trim(item);
		trim(deli);
		msg.Format(_T("code = %s, name = %s, name_kr = %s\n"), shcode.c_str(), hname.c_str(), item.c_str());
		//TRACE(msg);

		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		SmProduct* product = market->FindAddProduct(product_code);
		marketMgr->AddProduct(product);
		marketMgr->AddCategoryMarket(product_code, market_name);
		product->MarketName(market_name);
		product->NameKr("코스닥150선물");
		product->NameKr("Kosdaqq150F");

		VtSymbol* sym = product->AddSymbol(shcode);
		sym->Name = hname;
		//symMgr->AddHdSymbol(sym);
		sym->ProductCode = product->Code();
		sym->MarketName = product->MarketName();
		sym->Decimal = std::stoi(deli);
		sym->Seungsu = std::stoi(tradewin);
		int hoga_unit = std::stoi(hounit);
		int deci = std::stoi(deli);
		sym->intTickSize = hoga_unit;
		double tick_size = hoga_unit / std::pow(10, deci);
		sym->TickSize = tick_size;
		sym->CtrUnit = tick_size;
		double tick_value = std::stoi(tradewin) * tick_size;
		sym->TickValue = tick_value;
	}
}

void SmSymbolReader::ReadMiniKospiFutureFile(std::string fullPath)
{
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	std::string market_name = "미니코스피선물";
	SmMarket* market = marketMgr->AddMarket(market_name);
	market->Code("105");
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string shcode = line.substr(0, 8);
		/* 종목코드                             */

		std::string expcode = line.substr(8, 12);
		/* 표준코드                               */

		std::string hname = line.substr(20, 30);
		/* 한글 종목명                      */

		std::string item = line.substr(50, 6);
		/* 기초자산코드                             */

		std::string deli = line.substr(56, 2);
		/* 소수점                          */

		std::string hounit = line.substr(58, 5);
		/*호가단위                          */

		// 0이 8개 더 붙어 있어 일부러 13자리만 읽는다.
		std::string tradewin = line.substr(63, 13);
		/* 거래승수                         */

		std::string spjmgubun = line.substr(84, 1);
		/* 스프레드기준종목구분코드*/

		std::string gshcode = line.substr(85, 8);
		/* 근월물코드                            */

		std::string wshcode = line.substr(93, 8);
		/* 원월물코드                           */

		std::string product_code = shcode.substr(0, 3);



		rtrim(shcode);
		rtrim(hname);
		trim(item);
		trim(deli);
		msg.Format(_T("code = %s, name = %s, name_kr = %s\n"), shcode.c_str(), hname.c_str(), item.c_str());
		//TRACE(msg);

		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		SmProduct* product = market->FindAddProduct(product_code);
		marketMgr->AddProduct(product);
		marketMgr->AddCategoryMarket(product_code, market_name);
		product->MarketName(market_name);
		product->NameKr("코스피200미니선물");
		product->NameKr("Kospi200MiniF");

		VtSymbol* sym = product->AddSymbol(shcode);
		sym->Name = hname;
		//symMgr->AddHdSymbol(sym);
		sym->ProductCode = product->Code();
		sym->MarketName = product->MarketName();
		sym->Decimal = std::stoi(deli);
		sym->Seungsu = std::stoi(tradewin);
		int hoga_unit = std::stoi(hounit);
		int deci = std::stoi(deli);
		sym->intTickSize = hoga_unit;
		double tick_size = hoga_unit / std::pow(10, deci);
		sym->TickSize = tick_size;
		sym->CtrUnit = tick_size;
		double tick_value = std::stoi(tradewin) * tick_size;
		sym->TickValue = tick_value;
	}
}

void SmSymbolReader::ReadCommodityFutureFile(std::string fullPath)
{
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	std::string market_name = "코스피상품선물";
	SmMarket* market = marketMgr->AddMarket(market_name);
	market->Code("107");
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string shcode = line.substr(0, 8);
		/* 종목코드                             */

		std::string expcode = line.substr(8, 12);
		/* 표준코드                               */

		std::string hname = line.substr(20, 30);
		/* 한글 종목명                      */

		std::string item = line.substr(50, 6);
		/* 기초자산코드                             */

		std::string deli = line.substr(56, 2);
		/* 소수점                          */

		std::string hounit = line.substr(58, 5);
		/*호가단위                          */

		// 0이 8개 더 붙어 있어 일부러 13자리만 읽는다.
		std::string tradewin = line.substr(63, 13);
		/* 거래승수                         */

		std::string spjmgubun = line.substr(84, 1);
		/* 스프레드기준종목구분코드*/

		std::string gshcode = line.substr(85, 8);
		/* 근월물코드                            */

		std::string wshcode = line.substr(93, 8);
		/* 원월물코드                           */

		std::string product_code = shcode.substr(0, 3);



		rtrim(shcode);
		rtrim(hname);
		trim(item);
		trim(deli);
		msg.Format(_T("code = %s, name = %s, name_kr = %s\n"), shcode.c_str(), hname.c_str(), item.c_str());
		//TRACE(msg);

		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		SmProduct* product = market->FindAddProduct(product_code);
		marketMgr->AddProduct(product);
		marketMgr->AddCategoryMarket(product_code, market_name);
		product->MarketName(market_name);
		product->NameKr("코스피상품선물");
		product->NameKr("KospiCommodityF");

		VtSymbol* sym = product->AddSymbol(shcode);
		sym->Name = hname;
		//symMgr->AddHdSymbol(sym);
		sym->ProductCode = product->Code();
		sym->MarketName = product->MarketName();
		sym->Decimal = std::stoi(deli);
		sym->Seungsu = std::stoi(tradewin);
		int hoga_unit = std::stoi(hounit);
		int deci = std::stoi(deli);
		sym->intTickSize = hoga_unit;
		double tick_size = hoga_unit / std::pow(10, deci);
		sym->TickSize = tick_size;
		sym->CtrUnit = tick_size;
		double tick_value = std::stoi(tradewin) * tick_size;
		sym->TickValue = tick_value;
	}
}

void SmSymbolReader::ReadKospiFutureInfo(std::string fullPath)
{
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string shcode = line.substr(0, 8);
		/* 종목코드                             */

		std::string deli = line.substr(8, 2);
		/* 소수점                          */

		std::string tick_size = line.substr(10, 5);
		/* 틱사이즈                               */

		std::string tick_value = line.substr(15, 5);
		/* 틱 밸류                      */

		std::string trade_win = line.substr(20, 10);
		/* 거래 승수                            */

		
		std::string near_month = line.substr(90, 2);
		/*근월물                          */

		std::string last_date = line.substr(92, 8);
		/*최종거래일                         */

		SmMarketManager* mrktMgr = SmMarketManager::GetInstance();

		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		VtSymbol* sym = symMgr->FindSymbol(shcode);
		if (sym) {
			sym->NearMonth = std::stoi(near_month);
			sym->LastDate = last_date;
		}
	}
}

void SmSymbolReader::ReadKospiOptionInfo(std::string fullPath)
{
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string shcode = line.substr(0, 8);
		/* 종목코드                             */

		std::string deli = line.substr(8, 2);
		/* 소수점                          */

		std::string tick_size = line.substr(10, 5);
		/* 틱사이즈                               */

		std::string tick_value = line.substr(15, 5);
		/* 틱 밸류                      */

		std::string trade_win = line.substr(20, 10);
		/* 거래 승수                            */


		std::string near_month = line.substr(90, 2);
		/*근월물                          */
		if (near_month.compare("A"))
			near_month = "10";
		else if (near_month.compare("B"))
			near_month = "11";
		else if (near_month.compare("C"))
			near_month = "12";
		else if (near_month.compare("D"))
			near_month = "13";
		else if (near_month.compare("E"))
			near_month = "14";
		else if (near_month.compare("F"))
			near_month = "15";

		std::string last_date = line.substr(92, 8);
		/*최종거래일                         */



		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		VtSymbol* sym = symMgr->FindSymbol(shcode);
		if (sym) {
			sym->NearMonth = std::stoi(near_month);
			sym->LastDate = last_date;
		}
	}
}

void SmSymbolReader::ReadUsDollarFutureInfo(std::string fullPath)
{
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string shcode = line.substr(0, 8);
		/* 종목코드                             */

		std::string deli = line.substr(8, 2);
		/* 소수점                          */

		std::string tick_size = line.substr(10, 5);
		/* 틱사이즈                               */

		std::string tick_value = line.substr(15, 5);
		/* 틱 밸류                      */

		std::string trade_win = line.substr(20, 10);
		/* 거래 승수                            */


		std::string near_month = line.substr(90, 2);
		/*근월물                          */
		if (near_month.compare("A"))
			near_month = "10";
		else if (near_month.compare("B"))
			near_month = "11";
		else if (near_month.compare("C"))
			near_month = "12";
		else if (near_month.compare("D"))
			near_month = "13";
		else if (near_month.compare("E"))
			near_month = "14";
		else if (near_month.compare("F"))
			near_month = "15";

		std::string last_date = line.substr(92, 8);
		/*최종거래일                         */



		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		VtSymbol* sym = symMgr->FindSymbol(shcode);
		if (sym) {
			sym->NearMonth = std::stoi(near_month);
			sym->LastDate = last_date;
		}
	}
}

void SmSymbolReader::ReadMiniKospiOptionFile(std::string fullPath)
{
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	std::string market_name = "코스피미니옵션";
	SmMarket* market = marketMgr->AddMarket(market_name);
	market->Code("205");
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string shcode = line.substr(0, 8);
		/* 종목코드                             */

		std::string expcode = line.substr(8, 12);
		/* 표준코드                               */

		std::string hname = line.substr(20, 30);
		/* 한글 종목명                      */

		std::string atm = line.substr(50, 1);
		/* ATM 구분                      */

		std::string item = line.substr(51, 6);
		/* 기초자산코드                             */

		std::string deli = line.substr(57, 2);
		/* 소수점                          */

		std::string hounit = line.substr(59, 5);
		/*호가단위                          */

		// 0이 8개 더 붙어 있어 일부러 13자리만 읽는다.
		std::string tradewin = line.substr(64, 13);
		/* 거래승수                         */

		std::string spjmgubun = line.substr(85, 1);
		/* 스프레드기준종목구분코드*/

		std::string gshcode = line.substr(86, 8);
		/* 근월물코드                            */

		std::string wshcode = line.substr(94, 8);
		/* 원월물코드                           */

		std::string product_code = shcode.substr(0, 3);



		rtrim(shcode);
		rtrim(hname);
		trim(item);
		trim(deli);
		msg.Format(_T("code = %s, name = %s, name_kr = %s\n"), shcode.c_str(), hname.c_str(), item.c_str());
		//TRACE(msg);

		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		SmProduct* product = market->FindAddProduct(product_code);
		marketMgr->AddProduct(product);
		marketMgr->AddCategoryMarket(product_code, market_name);
		product->MarketName(market_name);
		//product->NameKr("미니코스피200옵션");
		product->NameKr("MiniKospi200O");
		VtSymbol* sym = product->AddSymbol(shcode);
		sym->Name = hname;
		//symMgr->AddHdSymbol(sym);
		sym->ProductCode = product->Code();
		sym->MarketName = product->MarketName();
		sym->Decimal = std::stoi(deli);
		sym->Seungsu = std::stoi(tradewin);
		int hoga_unit = std::stoi(hounit);
		int deci = std::stoi(deli);
		sym->intTickSize = hoga_unit;
		double tick_size = hoga_unit / std::pow(10, deci);
		sym->TickSize = tick_size;
		sym->CtrUnit = tick_size;
		double tick_value = std::stoi(tradewin) * tick_size;
		sym->TickValue = tick_value;
	}
}

void SmSymbolReader::ReadKosdaqOptionFile(std::string fullPath)
{
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	std::string market_name = "코스닥옵션";
	SmMarket* market = marketMgr->AddMarket(market_name);
	market->Code("206");
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string shcode = line.substr(0, 8);
		/* 종목코드                             */

		std::string expcode = line.substr(8, 12);
		/* 표준코드                               */

		std::string hname = line.substr(20, 30);
		/* 한글 종목명                      */

		std::string atm = line.substr(50, 1);
		/* ATM 구분                      */

		std::string item = line.substr(51, 6);
		/* 기초자산코드                             */

		std::string deli = line.substr(57, 2);
		/* 소수점                          */

		std::string hounit = line.substr(59, 5);
		/*호가단위                          */

		// 0이 8개 더 붙어 있어 일부러 13자리만 읽는다.
		std::string tradewin = line.substr(64, 13);
		/* 거래승수                         */

		std::string spjmgubun = line.substr(85, 1);
		/* 스프레드기준종목구분코드*/

		std::string gshcode = line.substr(86, 8);
		/* 근월물코드                            */

		std::string wshcode = line.substr(94, 8);
		/* 원월물코드                           */

		std::string product_code = shcode.substr(0, 3);



		rtrim(shcode);
		rtrim(hname);
		trim(item);
		trim(deli);
		msg.Format(_T("code = %s, name = %s, name_kr = %s\n"), shcode.c_str(), hname.c_str(), item.c_str());
		//TRACE(msg);

		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		SmProduct* product = market->FindAddProduct(product_code);
		marketMgr->AddProduct(product);
		marketMgr->AddCategoryMarket(product_code, market_name);
		product->MarketName(market_name);
		//product->NameKr("코스피200옵션");
		product->NameKr("Kosqaq150O");
		VtSymbol* sym = product->AddSymbol(shcode);
		sym->Name = hname;
		//symMgr->AddHdSymbol(sym);
		sym->ProductCode = product->Code();
		sym->MarketName = product->MarketName();
		sym->Decimal = std::stoi(deli);
		sym->Seungsu = std::stoi(tradewin);
		int hoga_unit = std::stoi(hounit);
		int deci = std::stoi(deli);
		sym->intTickSize = hoga_unit;
		double tick_size = hoga_unit / std::pow(10, deci);
		sym->TickSize = tick_size;
		sym->CtrUnit = tick_size;
		double tick_value = std::stoi(tradewin) * tick_size;
		sym->TickValue = tick_value;
	}
}

