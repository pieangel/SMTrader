#pragma once
#include <map>
#include <string>
#include "Global/TemplateSingleton.h"
#include <sstream>
#include <msgpack.hpp>
#include "SimpleBinStream.h"
#include "Xml/pugixml.hpp"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;

class VtFund;
class VtAccount;
class VtFundManager : public TemplateSingleton<VtFundManager>
{
public:
	VtFundManager();
	~VtFundManager();
	std::map<std::string, VtFund*>& GetFundList()
	{
		return _FundList;
	}
	void CreateDefaultFund();
	VtFund* FindFund(std::string fundName);
	VtFund* AddFund(std::string fundName);
	void DeleteFund(std::string fundName);
	void Save(std::stringstream& ss);
	void Load(std::stringstream& ss);
	void Save(msgpack::sbuffer& ss);

	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
	void AddToUsedAccountMap(VtAccount* acnt);
	void RemoveFromUsedAccountMap(std::string acntNo);
	void AddToUnUsedAccountMap(VtAccount* acnt);
	void RemoveFromUnUsedAccountMap(std::string acntNo);
	void DivideAccountFromFundList();
	void ClearUsedAccountList();
	void ClearUnusedAccountList();
	VtAccount* FindFromUnusedAccountList(std::string acntNo);
	VtAccount* FindFromUsedAccountList(std::string acntNo);
	std::map<std::string, VtAccount*>& GetUnusedAccountList()
	{
		return _UnusedAccountMap;
	}

	void SaveToXml(pugi::xml_node& node);
	void LoadFromXml(pugi::xml_node& node);
private:
	// Key : Fund Name, Value : VtFund object.
	std::map<std::string, VtFund*> _FundList;

	std::map<std::string, VtAccount*> _UsedAccountMap;
	std::map<std::string, VtAccount*> _UnusedAccountMap;
};

