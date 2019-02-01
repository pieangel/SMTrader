#pragma once
#include <string>
#include <sstream>
#include <msgpack.hpp>
#include "SimpleBinStream.h"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;

class VtFundAccount
{
public:
	VtFundAccount();
	~VtFundAccount();
	std::string AccountNo;
	std::string AccountName;
	double Ratio;
	int SeungSu;
	void Save(std::stringstream& ss);
	void Save(msgpack::sbuffer& ss);
	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
};

