#pragma once
#include <string>
#include "SimpleBinStream.h"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;

class VtOutSignalDef
{
public:
	VtOutSignalDef();
	~VtOutSignalDef();
	std::string SignalName;
	std::string SymbolCode;
	std::string StrategyName;
	std::string Desc;

	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
};

