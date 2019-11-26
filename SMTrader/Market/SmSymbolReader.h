#pragma once
#include "../Global/TemplateSingleton.h"
#include <string>
#include <set>
#include <queue>
#include <map>

class SmSymbolReader : public TemplateSingleton<SmSymbolReader>
{
public:
	SmSymbolReader();
	~SmSymbolReader();

public:
	void ReadSymbolFileList();
	std::string GetWorkingDir();
	void ReadSymbolFromFile(int index, std::string fullPath);
	std::set<std::string> DomesticSymbolMasterFileSet;

	inline std::string& rtrim(std::string& s)
	{
		s.erase(s.find_last_not_of(ws) + 1);
		return s;
	}

	// trim from beginning of string (left)
	inline std::string& ltrim(std::string& s)
	{
		s.erase(0, s.find_first_not_of(ws));
		return s;
	}

	// trim from both ends of string (right then left)
	inline std::string& trim(std::string& s)
	{
		return ltrim(rtrim(s));
	}
private:
	char* ws = " \t\n\r\f\v";
	void ReadMarketFile(std::string fullPath);
	void ReadPmFile(std::string fullPath);
	void ReadJmFile(std::string fullPath);
	void ReadMarketFile();
	void ReadPmFile();
	void ReadJmFile();
	void ReadKospiFutureFile(std::string fullPath);
	void ReadKospiOptionFile(std::string fullPath);
	void ReadKospiWeeklyOptionFile(std::string fullPath);
	void ReadKosdaqFutureFile(std::string fullPath);
	void ReadMiniKospiFutureFile(std::string fullPath);
	void ReadCommodityFutureFile(std::string fullPath);
	void ReadKospiFutureInfo(std::string fullPath);
	void ReadKospiOptionInfo(std::string fullPath);
	void ReadUsDollarFutureInfo(std::string fullPath);
	std::set<std::string> _DomesticList;
};

