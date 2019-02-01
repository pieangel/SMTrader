#include "stdafx.h"
#include "VtBollingerBandIndex.h"


VtBollingerBandIndex::VtBollingerBandIndex()
{
}


VtBollingerBandIndex::VtBollingerBandIndex(VtIndexType type)
	: VtBaseIndex(type)
{

}

VtBollingerBandIndex::~VtBollingerBandIndex()
{
}

void VtBollingerBandIndex::SaveToXml(pugi::xml_node& node)
{
	VtBaseIndex::SaveToXml(node);

	/*
		int Period;
	int Width;
	int LineColor;
	int FillColor;*/

	CString msg;
	std::string value;

	msg.Format(_T("%d"), Period);
	value = (LPCTSTR)msg;
	pugi::xml_node index_period = node.append_child("index_period");
	index_period.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), Width);
	value = (LPCTSTR)msg;
	pugi::xml_node index_width = node.append_child("index_width");
	index_width.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), LineColor);
	value = (LPCTSTR)msg;
	pugi::xml_node index_line_color = node.append_child("index_line_color");
	index_line_color.append_child(pugi::node_pcdata).set_value(value.c_str());


	msg.Format(_T("%d"), FillColor);
	value = (LPCTSTR)msg;
	pugi::xml_node index_fill_color = node.append_child("index_fill_color");
	index_fill_color.append_child(pugi::node_pcdata).set_value(value.c_str());

}

void VtBollingerBandIndex::LoadFromXml(pugi::xml_node& node)
{
	VtBaseIndex::LoadFromXml(node);

	std::string index_period = node.child_value("index_period");
	Period = std::stoi(index_period);

	std::string index_width = node.child_value("index_width");
	Width = std::stoi(index_width);

	std::string index_line_color = node.child_value("index_line_color");
	LineColor = std::stoi(index_line_color);

	std::string index_fill_color = node.child_value("index_fill_color");
	FillColor = std::stoi(index_fill_color);
}
