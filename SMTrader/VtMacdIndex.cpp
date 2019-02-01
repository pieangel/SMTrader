#include "stdafx.h"
#include "VtMacdIndex.h"


VtMacdIndex::VtMacdIndex()
{
}


VtMacdIndex::VtMacdIndex(VtIndexType type)
	: VtBaseIndex(type)
{

}

VtMacdIndex::~VtMacdIndex()
{
}

void VtMacdIndex::SaveToXml(pugi::xml_node& node)
{
	VtBaseIndex::SaveToXml(node);

	/*
		int Height;
	int ShortPeriod;
	int LongPeriod;
	int Period;
	int Color;
	int SignalColor;
	int DivColor;*/

	CString msg;
	std::string value;

	msg.Format(_T("%d"), Height);
	value = (LPCTSTR)msg;
	pugi::xml_node index_height = node.append_child("index_height");
	index_height.append_child(pugi::node_pcdata).set_value(value.c_str());


	msg.Format(_T("%d"), ShortPeriod);
	value = (LPCTSTR)msg;
	pugi::xml_node index_short_period = node.append_child("index_short_period");
	index_short_period.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), LongPeriod);
	value = (LPCTSTR)msg;
	pugi::xml_node index_long_period = node.append_child("index_long_period");
	index_short_period.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), Period);
	value = (LPCTSTR)msg;
	pugi::xml_node index_period = node.append_child("index_period");
	index_period.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), Color);
	value = (LPCTSTR)msg;
	pugi::xml_node index_color = node.append_child("index_color");
	index_color.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), SignalColor);
	value = (LPCTSTR)msg;
	pugi::xml_node index_signal_color = node.append_child("index_signal_color");
	index_signal_color.append_child(pugi::node_pcdata).set_value(value.c_str());


	msg.Format(_T("%d"), DivColor);
	value = (LPCTSTR)msg;
	pugi::xml_node index_div_color = node.append_child("index_div_color");
	index_div_color.append_child(pugi::node_pcdata).set_value(value.c_str());
}

void VtMacdIndex::LoadFromXml(pugi::xml_node& node)
{
	VtBaseIndex::LoadFromXml(node);

	std::string index_height = node.child_value("index_height");
	Height = std::stoi(index_height);

	std::string index_short_period = node.child_value("index_short_period");
	ShortPeriod = std::stoi(index_short_period);

	std::string index_long_period = node.child_value("index_long_period");
	LongPeriod = std::stoi(index_long_period);

	std::string index_period = node.child_value("index_period");
	Period = std::stoi(index_period);

	std::string index_color = node.child_value("index_color");
	Color = std::stoi(index_color);

	std::string index_signal_color = node.child_value("index_signal_color");
	SignalColor = std::stoi(index_signal_color);

	std::string index_div_color = node.child_value("index_div_color");
	DivColor = std::stoi(index_div_color);
}
