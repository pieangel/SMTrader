#include "stdafx.h"
#include "VtBaseBand.h"


VtBaseBand::VtBaseBand()
{
}


VtBaseBand::VtBaseBand(VtBandType type)
{
	BandBase.Type = type;
	Show = true;
}

VtBaseBand::~VtBaseBand()
{
}

void VtBaseBand::SaveToXml(pugi::xml_node& node)
{
	CString msg;
	std::string value;
	msg.Format(_T("%d"), (int)BandBase.Type);
	value = (LPCTSTR)msg;
	pugi::xml_node band_type = node.append_child("band_type");
	band_type.append_child(pugi::node_pcdata).set_value(value.c_str());

	pugi::xml_node band_name = node.append_child("band_name");
	band_name.append_child(pugi::node_pcdata).set_value(BandBase.Name.c_str());

	pugi::xml_node band_desc = node.append_child("band_desc");
	band_desc.append_child(pugi::node_pcdata).set_value(BandBase.Name.c_str());
}

void VtBaseBand::LoadFromXml(pugi::xml_node& node)
{
	std::string band_type = node.child_value("band_type");
	std::string band_name = node.child_value("band_name");
	std::string band_desc = node.child_value("band_desc");

	BandBase.Type = (VtBandType)std::stoi(band_type);
	BandBase.Name = band_name;
	BandBase.Desc = band_desc;
}
