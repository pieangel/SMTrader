#include "stdafx.h"
#include "VtRsiIndex.h"


VtRsiIndex::VtRsiIndex()
{
}


VtRsiIndex::VtRsiIndex(VtIndexType type)
	: VtBaseIndex(type)
{

}

VtRsiIndex::~VtRsiIndex()
{
}

void VtRsiIndex::SaveToXml(pugi::xml_node& node)
{
	VtBaseIndex::SaveToXml(node);

	/*
	int Height;
	int Period;
	int Color;
	int Range;
	int UpColor;
	int DownColor;*/

	CString msg;
	std::string value;

	msg.Format(_T("%d"), Height);
	value = (LPCTSTR)msg;
	pugi::xml_node index_height = node.append_child("index_height");
	index_height.append_child(pugi::node_pcdata).set_value(value.c_str());


	msg.Format(_T("%d"), Period);
	value = (LPCTSTR)msg;
	pugi::xml_node index_period = node.append_child("index_period");
	index_period.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), Color);
	value = (LPCTSTR)msg;
	pugi::xml_node index_color = node.append_child("index_color");
	index_color.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), Range);
	value = (LPCTSTR)msg;
	pugi::xml_node index_range = node.append_child("index_range");
	index_range.append_child(pugi::node_pcdata).set_value(value.c_str());


	msg.Format(_T("%d"), UpColor);
	value = (LPCTSTR)msg;
	pugi::xml_node index_up_color = node.append_child("index_up_color");
	index_up_color.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), DownColor);
	value = (LPCTSTR)msg;
	pugi::xml_node index_down_color = node.append_child("index_down_color");
	index_down_color.append_child(pugi::node_pcdata).set_value(value.c_str());
}

void VtRsiIndex::LoadFromXml(pugi::xml_node& node)
{
	VtBaseIndex::LoadFromXml(node);

	std::string index_height = node.child_value("index_height");
	Height = std::stoi(index_height);

	std::string index_period = node.child_value("index_period");
	Period = std::stoi(index_period);

	std::string index_color = node.child_value("index_color");
	Color = std::stoi(index_color);

	std::string index_range = node.child_value("index_range");
	Range = std::stoi(index_range);

	std::string index_up_color = node.child_value("index_up_color");
	UpColor = std::stoi(index_up_color);

	std::string index_down_color = node.child_value("index_down_color");
	DownColor = std::stoi(index_down_color);
}
