#pragma once
#include "VtSystemArg.h"
#include <string>
#include <map>
#include <vector>
#include "SimpleBinStream.h"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;

class VtSystemArgGroup
{
public:
	VtSystemArgGroup();
	~VtSystemArgGroup();
	std::string Name() const { return _Name; }
	void Name(std::string val) { _Name = val; }
	void AddSystemArg(std::string name, VtSystemArg arg);
	VtSystemArg* GetSystemArg(std::string name);
	std::vector<VtSystemArg>& GetArgVec() {
		return _ArgMap;
	}

	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);

private:
	std::string _Name;
	std::vector<VtSystemArg> _ArgMap;
};

