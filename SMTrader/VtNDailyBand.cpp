#include "stdafx.h"
#include "VtNDailyBand.h"


VtNDailyBand::VtNDailyBand()
{
	BackLen = 1;

	HighColor = 0xff0000;
	LowColor = 0x0000ff;
}


VtNDailyBand::VtNDailyBand(VtBandType type)
	:VtBaseBand(type)
{
	BackLen = 1;
	HighColor = 0xff0000;
	LowColor = 0x0000ff;
}

VtNDailyBand::~VtNDailyBand()
{
}

void VtNDailyBand::SaveToXml(pugi::xml_node& node)
{
	VtBaseBand::SaveToXml(node);

	CString msg;
	std::string value;


	msg.Format(_T("%d"), HighColor);
	value = (LPCTSTR)msg;
	pugi::xml_node high_color = node.append_child("high_color");
	high_color.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), LowColor);
	value = (LPCTSTR)msg;
	pugi::xml_node low_color = node.append_child("low_color");
	low_color.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), BackLen);
	value = (LPCTSTR)msg;
	pugi::xml_node back_len = node.append_child("back_len");
	back_len.append_child(pugi::node_pcdata).set_value(value.c_str());
}

void VtNDailyBand::LoadFromXml(pugi::xml_node& node)
{
	VtBaseBand::LoadFromXml(node);

	std::string high_color = node.child_value("high_color");
	HighColor = std::stoi(high_color);

	std::string low_color = node.child_value("low_color");
	LowColor = std::stoi(low_color);

	std::string back_len = node.child_value("back_len");
	BackLen = std::stoi(back_len);

}
