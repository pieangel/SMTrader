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
		// �ŷ��� �ε��� �ڵ�
		std::string exIndexCode = line.substr(20, 4);
		// �ŷ��� �ڵ� 
		std::string exChangeCode = line.substr(24, 5);
		// ǰ�� �ڵ�
		std::string pmCode = line.substr(29, 5);
		// ���� ������ ���ڷ� ��
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
		/* �����ڵ�                             */

		std::string ExchCd = line.substr(32, 5);
		/* �ŷ���                               */

		std::string IndexCode = line.substr(37, 4);
		/* ǰ�� �ε����ڵ�                      */

		std::string MrktCd = line.substr(41, 5);
		/* ǰ���ڵ�                             */

		std::string ExchNo = line.substr(46, 5);
		/* �ŷ��� ��ȣ                          */

		std::string Pdesz = line.substr(51, 5);
		/* �Ҽ��� ����                          */

		std::string Rdesz = line.substr(56, 5);
		/* �Ҽ��� ����2                         */

		std::string CtrtSize = line.substr(61, 20);
		/* ���ũ��                             */

		std::string TickSize = line.substr(81, 20);
		/* Tick Size                            */

		std::string TickValue = line.substr(101, 20);
		/* Tick Value                           */

		std::string MltiPler = line.substr(121, 20);
		/* �ŷ��¼�                             */

		std::string DispDigit = line.substr(141, 10);
		/* ����                                 */

		std::string SeriesNm = line.substr(151, 32);
		/* Full �����                          */

		std::string SeriesNmKor = line.substr(183, 32);
		/* Full ������ѱ�                      */

		std::string NearSeq = line.substr(215, 1);
		/* �ֱٿ���, �ֿ�����ǥ��               */

		std::string StatTp = line.substr(216, 1);
		/* �ŷ����ɿ���                         */

		std::string LockDt = line.substr(217, 8);
		/* �ű԰ŷ�������                       */

		std::string TradFrDt = line.substr(225, 8);
		/* ���ʰŷ���                           */

		std::string TradToDt = line.substr(233, 8);
		/* �����ŷ���                           */

		std::string ExprDt = line.substr(241, 8);
		/* ������, ����������                   */

		std::string RemnCnt = line.substr(249, 4);
		/* �����ϼ�                             */

		std::string HogaMthd = line.substr(253, 30);
		/* ȣ�����                             */

		std::string MinMaxRt = line.substr(283, 6);
		/* ������������                         */

		std::string BaseP = line.substr(289, 20);
		/* ���ذ�                               */

		std::string MaxP = line.substr(309, 20);
		/* ���Ѱ�                               */

		std::string MinP = line.substr(329, 20);
		/* ���Ѱ�                               */

		std::string TrstMgn = line.substr(349, 20);
		/* �ű��ֹ����ű�                       */

		std::string MntMgn = line.substr(369, 20);
		/* �������ű�                           */

		std::string CrcCd = line.substr(389, 3);
		/* ������ȭ�ڵ�                         */

		std::string BaseCrcCd = line.substr(392, 3);
		/* BASE CRC CD                          */

		std::string CounterCrcCd = line.substr(395, 3);
		/* COUNTER CRC CD                       */

		std::string PipCost = line.substr(398, 20);
		/* PIP COST                             */

		std::string BuyInt = line.substr(418, 20);
		/* �ż�����                             */

		std::string SellInt = line.substr(438, 20);
		/* �ŵ�����                             */

		std::string RoundLots = line.substr(458, 6);
		/* LOUND LOTS                           */

		std::string ScaleChiper = line.substr(464, 10);
		/* �����ڸ���                           */

		std::string decimalchiper = line.substr(474, 5);
		/* �Ҽ��� ����(KTB����)                 */

		std::string JnilVolume = line.substr(479, 10);
		/* ���ϰŷ���                           */



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
		/* �����ڵ�                             */

		std::string ExchCd = line.substr(32, 5);
		/* �ŷ���                               */

		std::string IndexCode = line.substr(37, 4);
		/* ǰ�� �ε����ڵ�                      */

		std::string MrktCd = line.substr(41, 5);
		/* ǰ���ڵ�                             */

		std::string ExchNo = line.substr(46, 5);
		/* �ŷ��� ��ȣ                          */

		std::string Pdesz = line.substr(51, 5);
		/* �Ҽ��� ����                          */

		std::string Rdesz = line.substr(56, 5);
		/* �Ҽ��� ����2                         */

		std::string CtrtSize = line.substr(61, 20);
		/* ���ũ��                             */

		std::string TickSize = line.substr(81, 20);
		/* Tick Size                            */

		std::string TickValue = line.substr(101, 20);
		/* Tick Value                           */

		std::string MltiPler = line.substr(121, 20);
		/* �ŷ��¼�                             */

		std::string DispDigit = line.substr(141, 10);
		/* ����                                 */

		std::string SeriesNm = line.substr(151, 32);
		/* Full �����                          */

		std::string SeriesNmKor = line.substr(183, 32);
		/* Full ������ѱ�                      */

		std::string NearSeq = line.substr(215, 1);
		/* �ֱٿ���, �ֿ�����ǥ��               */

		std::string StatTp = line.substr(216, 1);
		/* �ŷ����ɿ���                         */

		std::string LockDt = line.substr(217, 8);
		/* �ű԰ŷ�������                       */

		std::string TradFrDt = line.substr(225, 8);
		/* ���ʰŷ���                           */

		std::string last_date = line.substr(233, 8);
		/* �����ŷ���                           */

		std::string ExprDt = line.substr(241, 8);
		/* ������, ����������                   */

		std::string RemnCnt = line.substr(249, 4);
		/* �����ϼ�                             */

		std::string HogaMthd = line.substr(253, 30);
		/* ȣ�����                             */

		std::string MinMaxRt = line.substr(283, 6);
		/* ������������                         */

		std::string BaseP = line.substr(289, 20);
		/* ���ذ�                               */

		std::string MaxP = line.substr(309, 20);
		/* ���Ѱ�                               */

		std::string MinP = line.substr(329, 20);
		/* ���Ѱ�                               */

		std::string TrstMgn = line.substr(349, 20);
		/* �ű��ֹ����ű�                       */

		std::string MntMgn = line.substr(369, 20);
		/* �������ű�                           */

		std::string CrcCd = line.substr(389, 3);
		/* ������ȭ�ڵ�                         */

		std::string BaseCrcCd = line.substr(392, 3);
		/* BASE CRC CD                          */

		std::string CounterCrcCd = line.substr(395, 3);
		/* COUNTER CRC CD                       */

		std::string PipCost = line.substr(398, 20);
		/* PIP COST                             */

		std::string BuyInt = line.substr(418, 20);
		/* �ż�����                             */

		std::string SellInt = line.substr(438, 20);
		/* �ŵ�����                             */

		std::string RoundLots = line.substr(458, 6);
		/* LOUND LOTS                           */

		std::string ScaleChiper = line.substr(464, 10);
		/* �����ڸ���                           */

		std::string decimalchiper = line.substr(474, 5);
		/* �Ҽ��� ����(KTB����)                 */

		std::string JnilVolume = line.substr(479, 10);
		/* ���ϰŷ���                           */


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

// ������ �տ� 3�ڸ��� ��ǰ�� ��Ÿ����. �� ���� ���ڸ��� �⵵�� ��Ÿ���� �� ���� ���ڸ��� ���� ��Ÿ����.
// ������ ���ڸ��� ��簡�� �ǹ��Ѵ�.
void SmSymbolReader::ReadKospiFutureFile(std::string fullPath)
{
	SmMarketManager* marketMgr = SmMarketManager::GetInstance();
	std::string market_name = "�ڽ��Ǽ���";
	SmMarket *market = marketMgr->AddMarket(market_name);
	market->Code("101");
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string shcode = line.substr(0, 8);
		/* �����ڵ�                             */

		std::string expcode = line.substr(8, 12);
		/* ǥ���ڵ�                               */

		std::string hname = line.substr(20, 30);
		/* �ѱ� �����                      */

		std::string item = line.substr(50, 6);
		/* �����ڻ��ڵ�                             */

		std::string deli = line.substr(56, 2);
		/* �Ҽ���                          */

		std::string hounit = line.substr(58, 5);
		/*ȣ������                          */

		// 0�� 8�� �� �پ� �־� �Ϻη� 13�ڸ��� �д´�.
		std::string tradewin = line.substr(63, 13);
		/* �ŷ��¼�                         */

		std::string spjmgubun = line.substr(84, 1);
		/* ��������������񱸺��ڵ�*/

		std::string gshcode = line.substr(85, 8);
		/* �ٿ����ڵ�                            */

		std::string wshcode = line.substr(93, 8);
		/* �������ڵ�                           */

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
		product->NameKr("�ڽ���200����");
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
	std::string market_name = "�ڽ��ǿɼ�";
	SmMarket* market = marketMgr->AddMarket(market_name);
	market->Code("201");
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string shcode = line.substr(0, 8);
		/* �����ڵ�                             */

		std::string expcode = line.substr(8, 12);
		/* ǥ���ڵ�                               */

		std::string hname = line.substr(20, 30);
		/* �ѱ� �����                      */

		std::string atm = line.substr(50, 1);
		/* ATM ����                      */

		std::string item = line.substr(51, 6);
		/* �����ڻ��ڵ�                             */

		std::string deli = line.substr(57, 2);
		/* �Ҽ���                          */

		std::string hounit = line.substr(59, 5);
		/*ȣ������                          */

		// 0�� 8�� �� �پ� �־� �Ϻη� 13�ڸ��� �д´�.
		std::string tradewin = line.substr(64, 13);
		/* �ŷ��¼�                         */

		std::string spjmgubun = line.substr(85, 1);
		/* ��������������񱸺��ڵ�*/

		std::string gshcode = line.substr(86, 8);
		/* �ٿ����ڵ�                            */

		std::string wshcode = line.substr(94, 8);
		/* �������ڵ�                           */

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
		product->NameKr("�ڽ���200�ɼ�");
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
	std::string market_name = "�ڽ�����Ŭ���ɼ�";
	SmMarket* market = marketMgr->AddMarket(market_name);
	market->Code("209");
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string shcode = line.substr(0, 8);
		/* �����ڵ�                             */

		std::string expcode = line.substr(8, 12);
		/* ǥ���ڵ�                               */

		std::string hname = line.substr(20, 30);
		/* �ѱ� �����                      */

		std::string atm = line.substr(50, 1);
		/* ATM ����                      */

		std::string item = line.substr(51, 6);
		/* �����ڻ��ڵ�                             */

		std::string deli = line.substr(57, 2);
		/* �Ҽ���                          */

		std::string hounit = line.substr(59, 5);
		/*ȣ������                          */

		// 0�� 8�� �� �پ� �־� �Ϻη� 13�ڸ��� �д´�.
		std::string tradewin = line.substr(64, 13);
		/* �ŷ��¼�                         */

		std::string spjmgubun = line.substr(85, 1);
		/* ��������������񱸺��ڵ�*/

		std::string gshcode = line.substr(86, 8);
		/* �ٿ����ڵ�                            */

		std::string wshcode = line.substr(94, 8);
		/* �������ڵ�                           */

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
		product->NameKr("�ڽ���200��Ŭ���ɼ�");
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
	std::string market_name = "�ڽ��ڼ���";
	SmMarket* market = marketMgr->AddMarket(market_name);
	market->Code("106");
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string shcode = line.substr(0, 8);
		/* �����ڵ�                             */

		std::string expcode = line.substr(8, 12);
		/* ǥ���ڵ�                               */

		std::string hname = line.substr(20, 30);
		/* �ѱ� �����                      */

		std::string item = line.substr(50, 6);
		/* �����ڻ��ڵ�                             */

		std::string deli = line.substr(56, 2);
		/* �Ҽ���                          */

		std::string hounit = line.substr(58, 5);
		/*ȣ������                          */

		// 0�� 8�� �� �پ� �־� �Ϻη� 13�ڸ��� �д´�.
		std::string tradewin = line.substr(63, 13);
		/* �ŷ��¼�                         */

		std::string spjmgubun = line.substr(84, 1);
		/* ��������������񱸺��ڵ�*/

		std::string gshcode = line.substr(85, 8);
		/* �ٿ����ڵ�                            */

		std::string wshcode = line.substr(93, 8);
		/* �������ڵ�                           */

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
		product->NameKr("�ڽ���150����");
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
	std::string market_name = "�̴��ڽ��Ǽ���";
	SmMarket* market = marketMgr->AddMarket(market_name);
	market->Code("105");
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string shcode = line.substr(0, 8);
		/* �����ڵ�                             */

		std::string expcode = line.substr(8, 12);
		/* ǥ���ڵ�                               */

		std::string hname = line.substr(20, 30);
		/* �ѱ� �����                      */

		std::string item = line.substr(50, 6);
		/* �����ڻ��ڵ�                             */

		std::string deli = line.substr(56, 2);
		/* �Ҽ���                          */

		std::string hounit = line.substr(58, 5);
		/*ȣ������                          */

		// 0�� 8�� �� �پ� �־� �Ϻη� 13�ڸ��� �д´�.
		std::string tradewin = line.substr(63, 13);
		/* �ŷ��¼�                         */

		std::string spjmgubun = line.substr(84, 1);
		/* ��������������񱸺��ڵ�*/

		std::string gshcode = line.substr(85, 8);
		/* �ٿ����ڵ�                            */

		std::string wshcode = line.substr(93, 8);
		/* �������ڵ�                           */

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
		product->NameKr("�ڽ���200�̴ϼ���");
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
	std::string market_name = "�ڽ��ǻ�ǰ����";
	SmMarket* market = marketMgr->AddMarket(market_name);
	market->Code("107");
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string shcode = line.substr(0, 8);
		/* �����ڵ�                             */

		std::string expcode = line.substr(8, 12);
		/* ǥ���ڵ�                               */

		std::string hname = line.substr(20, 30);
		/* �ѱ� �����                      */

		std::string item = line.substr(50, 6);
		/* �����ڻ��ڵ�                             */

		std::string deli = line.substr(56, 2);
		/* �Ҽ���                          */

		std::string hounit = line.substr(58, 5);
		/*ȣ������                          */

		// 0�� 8�� �� �پ� �־� �Ϻη� 13�ڸ��� �д´�.
		std::string tradewin = line.substr(63, 13);
		/* �ŷ��¼�                         */

		std::string spjmgubun = line.substr(84, 1);
		/* ��������������񱸺��ڵ�*/

		std::string gshcode = line.substr(85, 8);
		/* �ٿ����ڵ�                            */

		std::string wshcode = line.substr(93, 8);
		/* �������ڵ�                           */

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
		product->NameKr("�ڽ��ǻ�ǰ����");
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
		/* �����ڵ�                             */

		std::string deli = line.substr(8, 2);
		/* �Ҽ���                          */

		std::string tick_size = line.substr(10, 5);
		/* ƽ������                               */

		std::string tick_value = line.substr(15, 5);
		/* ƽ ���                      */

		std::string trade_win = line.substr(20, 10);
		/* �ŷ� �¼�                            */

		
		std::string near_month = line.substr(90, 2);
		/*�ٿ���                          */

		std::string last_date = line.substr(92, 8);
		/*�����ŷ���                         */

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
		/* �����ڵ�                             */

		std::string deli = line.substr(8, 2);
		/* �Ҽ���                          */

		std::string tick_size = line.substr(10, 5);
		/* ƽ������                               */

		std::string tick_value = line.substr(15, 5);
		/* ƽ ���                      */

		std::string trade_win = line.substr(20, 10);
		/* �ŷ� �¼�                            */


		std::string near_month = line.substr(90, 2);
		/*�ٿ���                          */
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
		/*�����ŷ���                         */



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
		/* �����ڵ�                             */

		std::string deli = line.substr(8, 2);
		/* �Ҽ���                          */

		std::string tick_size = line.substr(10, 5);
		/* ƽ������                               */

		std::string tick_value = line.substr(15, 5);
		/* ƽ ���                      */

		std::string trade_win = line.substr(20, 10);
		/* �ŷ� �¼�                            */


		std::string near_month = line.substr(90, 2);
		/*�ٿ���                          */
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
		/*�����ŷ���                         */



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
	std::string market_name = "�ڽ��ǹ̴Ͽɼ�";
	SmMarket* market = marketMgr->AddMarket(market_name);
	market->Code("205");
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string shcode = line.substr(0, 8);
		/* �����ڵ�                             */

		std::string expcode = line.substr(8, 12);
		/* ǥ���ڵ�                               */

		std::string hname = line.substr(20, 30);
		/* �ѱ� �����                      */

		std::string atm = line.substr(50, 1);
		/* ATM ����                      */

		std::string item = line.substr(51, 6);
		/* �����ڻ��ڵ�                             */

		std::string deli = line.substr(57, 2);
		/* �Ҽ���                          */

		std::string hounit = line.substr(59, 5);
		/*ȣ������                          */

		// 0�� 8�� �� �پ� �־� �Ϻη� 13�ڸ��� �д´�.
		std::string tradewin = line.substr(64, 13);
		/* �ŷ��¼�                         */

		std::string spjmgubun = line.substr(85, 1);
		/* ��������������񱸺��ڵ�*/

		std::string gshcode = line.substr(86, 8);
		/* �ٿ����ڵ�                            */

		std::string wshcode = line.substr(94, 8);
		/* �������ڵ�                           */

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
		//product->NameKr("�̴��ڽ���200�ɼ�");
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
	std::string market_name = "�ڽ��ڿɼ�";
	SmMarket* market = marketMgr->AddMarket(market_name);
	market->Code("206");
	CString msg;
	std::ifstream infile(fullPath);
	std::string line;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);

		std::string shcode = line.substr(0, 8);
		/* �����ڵ�                             */

		std::string expcode = line.substr(8, 12);
		/* ǥ���ڵ�                               */

		std::string hname = line.substr(20, 30);
		/* �ѱ� �����                      */

		std::string atm = line.substr(50, 1);
		/* ATM ����                      */

		std::string item = line.substr(51, 6);
		/* �����ڻ��ڵ�                             */

		std::string deli = line.substr(57, 2);
		/* �Ҽ���                          */

		std::string hounit = line.substr(59, 5);
		/*ȣ������                          */

		// 0�� 8�� �� �پ� �־� �Ϻη� 13�ڸ��� �д´�.
		std::string tradewin = line.substr(64, 13);
		/* �ŷ��¼�                         */

		std::string spjmgubun = line.substr(85, 1);
		/* ��������������񱸺��ڵ�*/

		std::string gshcode = line.substr(86, 8);
		/* �ٿ����ڵ�                            */

		std::string wshcode = line.substr(94, 8);
		/* �������ڵ�                           */

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
		//product->NameKr("�ڽ���200�ɼ�");
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

