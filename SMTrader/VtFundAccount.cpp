#include "stdafx.h"
#include "VtFundAccount.h"
#include <msgpack.hpp>

VtFundAccount::VtFundAccount()
{
	SeungSu = 1;
	Ratio = 0.0;
}


VtFundAccount::~VtFundAccount()
{
}

void VtFundAccount::Save(std::stringstream& ss)
{
	msgpack::pack(ss, AccountName);
	msgpack::pack(ss, AccountNo);
	msgpack::pack(ss, SeungSu);
	msgpack::pack(ss, Ratio);
}

void VtFundAccount::Save(msgpack::sbuffer& ss)
{
	msgpack::pack(ss, AccountName);
	msgpack::pack(ss, AccountNo);
	msgpack::pack(ss, SeungSu);
	msgpack::pack(ss, Ratio);
}

void VtFundAccount::Save(simple::file_ostream<same_endian_type>& ss)
{
	ss << AccountName << AccountNo << SeungSu << Ratio;
}

void VtFundAccount::Load(simple::file_istream<same_endian_type>& ss)
{
	ss >> AccountName >> AccountNo >> SeungSu >> Ratio;
}
