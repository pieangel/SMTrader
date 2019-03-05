#include "stdafx.h"
#include "VtOutSignalDef.h"


VtOutSignalDef::VtOutSignalDef()
{
}


VtOutSignalDef::~VtOutSignalDef()
{
}

void VtOutSignalDef::Save(simple::file_ostream<same_endian_type>& ss)
{
	ss << SignalName;
	ss << SymbolCode;
	ss << StrategyName;
	ss << Desc;
}

void VtOutSignalDef::Load(simple::file_istream<same_endian_type>& ss)
{
	ss >> SignalName;
	ss >> SymbolCode;
	ss >> StrategyName;
	ss >> Desc;
}
