#pragma once
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <msgpack.hpp>
#include "SimpleBinStream.h"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;
class VtAccount;
class VtFund;
class VtFundMap
{
public:
	VtFundMap();
	~VtFundMap();
	std::vector<VtAccount*>& GetAccountVector()
	{
		return _AccountVector;
	}
	VtAccount* AddAccount(VtAccount* acnt);
	void RemoveAccount(std::string acntNo);
	VtAccount* FindAccount(std::string acntNo);
	void ClearAllFundAccounts();
	void Save(std::stringstream& ss);
	void Save(msgpack::sbuffer& ss);
	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
	VtFund* Fund() const { return _Fund; }
	void Fund(VtFund* val) { _Fund = val; }
private:
	// Key : AccountNo , Val : VtAccount object.
	std::vector<VtAccount*> _AccountVector;
	VtFund* _Fund;
};

