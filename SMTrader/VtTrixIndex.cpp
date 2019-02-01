#include "stdafx.h"
#include "VtTrixIndex.h"


VtTrixIndex::VtTrixIndex()
{
}


VtTrixIndex::VtTrixIndex(VtIndexType type)
	: VtBaseIndex(type)
{

}

VtTrixIndex::~VtTrixIndex()
{
}

void VtTrixIndex::SaveToXml(pugi::xml_node & node)
{
	VtBaseIndex::SaveToXml(node);
	/*
		int Height;
	int Period;
	int Color;*/


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

}

void VtTrixIndex::LoadFromXml(pugi::xml_node& node)
{
	VtBaseIndex::LoadFromXml(node);

	std::string index_height = node.child_value("index_height");
	Height = std::stoi(index_height);

	std::string index_period = node.child_value("index_period");
	Period = std::stoi(index_period);

	std::string index_color = node.child_value("index_color");
	Color = std::stoi(index_color);
}
